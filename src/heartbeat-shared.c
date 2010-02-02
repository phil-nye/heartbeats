#include "heartbeat.h"
#include <stdlib.h>
#include <string.h>

/**
       * Helper function for allocating shared memory
       */
static inline heartbeat_record_t* HB_alloc_log(int pid, int64_t buffer_size) {

  heartbeat_record_t* p = NULL;
#if 1
  int shmid;

  printf("Allocating log for %d, %d\n", pid, pid << 1);

  if ((shmid = shmget(pid << 1, buffer_size*sizeof(heartbeat_record_t), IPC_CREAT | 0666)) < 0) {
    //perror("cannot allocate shared memory for heartbeat records");
    p = NULL;
  }
  
  /*
   * Now we attach the segment to our data space.
   */
  if ((p = (heartbeat_record_t*) shmat(shmid, NULL, 0)) == (heartbeat_record_t *) -1) {
    //perror("cannot attach shared memory to heartbeat enabled process");
    p = NULL;
  }

#endif


  return p;
  
}

/**
       * 
       * @param pid integer 
       */
static inline HB_global_state_t* HB_alloc_state(int pid) {

  HB_global_state_t* p = NULL;
  int shmid;

  if ((shmid = 
       shmget((pid << 1) | 1, 
	      1*sizeof(HB_global_state_t), 
	      IPC_CREAT | 0666)) < 0) {
    p = NULL;
  }
  
  /*
   * Now we attach the segment to our data space.
   */
  if ((p = (HB_global_state_t*) shmat(shmid, NULL, 0)) == (HB_global_state_t *) -1) {
    p = NULL;
  }

  return p;
  
}

/**
       * Initialization function for process that
       * wants to register heartbeats
       * @param hb pointer to heartbeat_t
       * @param min_target double
       * @param max_target double
       * @param window_size int64_t
       * @param buffer_depth int64_t
       * @param log_name pointer to char
       */
int heartbeat_init(heartbeat_t* hb, 
		   double min_target, 
		   double max_target, 
		   int64_t window_size,
		   int64_t buffer_depth,
		   char* log_name) {
  // FILE* file;
  int rc = 0;
  int pid = getpid();
  //  char hb_filename[256];

  hb->state = HB_alloc_state(pid);
  hb->state->pid = pid;

  if(log_name != NULL) {
    hb->text_file = fopen(log_name, "w");
    fprintf(hb->text_file, "Beat    Tag    Timestamp    Global Rate    Window Rate    Instant Rate\n" );
  }
  else 
    hb->text_file = NULL;

  sprintf(hb->filename, "/data/oldcag/home/bits7/hank/heartbeats/%d", hb->state->pid);  
  
  hb->binary_file = fopen(hb->filename, "w");
  if ( hb->binary_file == NULL ) {
    rc = 1;
  }
  fclose(hb->binary_file);
  
  hb->log = HB_alloc_log(hb->state->pid, buffer_depth);
  //hb->monitor_log = NULL;

  if(hb->log == NULL)
    rc = 2;

  hb->first_timestamp = hb->last_timestamp = -1;
  hb->window_size = window_size;
  hb->window = (int64_t*) malloc(window_size*sizeof(int64_t));
  hb->current_index = 0;
  hb->state->min_heartrate = min_target;
  hb->state->max_heartrate = max_target;
  hb->state->counter = 0;
  hb->state->buffer_index = 0;
  hb->state->read_index = 0;
  hb->state->buffer_depth = buffer_depth;
  pthread_mutex_init(&hb->mutex, NULL);
  hb->steady_state = 0;
  hb->state->valid = 0;

  return rc;
}

/**
       * Cleanup function for process that
       * wants to register heartbeats
       * @param hb pointer to heartbeat_t
       */
void heartbeat_finish(heartbeat_t* hb) {
  free(hb->window);
  if(hb->text_file != NULL)
    fclose(hb->text_file);
  remove(hb->filename);
  /*TODO : need to deallocate log */
}

/**
       * Returns the record for the current heartbeat
       * currently may read old data
       * @param hb pointer to heartbeat_t
       * @see
       * @return 
       */
void hb_get_current(heartbeat_t volatile * hb, 
		    heartbeat_record_t volatile * record) {
  // uint64_t read_index =  (hb->state->buffer_index-1) % hb->state->buffer_depth;
  //memcpy(record, &hb->log[hb->state->read_index], sizeof(heartbeat_record_t));
  record->beat         = hb->log[hb->state->read_index].beat;
  record->tag          = hb->log[hb->state->read_index].tag;
  record->timestamp    = hb->log[hb->state->read_index].timestamp;
  record->global_rate  = hb->log[hb->state->read_index].global_rate;
  record->window_rate  = hb->log[hb->state->read_index].window_rate;
  record->instant_rate = hb->log[hb->state->read_index].instant_rate;
}

/**
       * Returns all heartbeat information for the last n heartbeats
       * @param hb pointer to heartbeat_t
       * @param record pointer to heartbeat_record_t
       * @param n integer
       */
int hb_get_history(heartbeat_t volatile * hb,
		   heartbeat_record_t volatile * record,
		   int n) {
  if(hb->state->counter > hb->state->buffer_index) {
     memcpy(record, 
	    &hb->log[hb->state->buffer_index], 
	    (hb->state->buffer_index*hb->state->buffer_depth)*sizeof(heartbeat_record_t));
     memcpy(record + (hb->state->buffer_index*hb->state->buffer_depth), 
	    &hb->log[0], 
	    (hb->state->buffer_index)*sizeof(heartbeat_record_t));
     return hb->state->buffer_depth;
  }
  else {
    memcpy(record, 
	   &hb->log[0], 
	   hb->state->buffer_index*sizeof(heartbeat_record_t));
    return hb->state->buffer_index;
  }
}

/**
       * Returns the heart rate over the life 
       * of the entire application
       * @param hb pointer to heartbeat_t
       * @return the heart rate (double) over the entire life of the application
       */
double hb_get_global_rate(heartbeat_t volatile * hb) {
  //uint64_t read_index =  (hb->state->buffer_index-1) % hb->state->buffer_depth;
  //printf("Reading from %lld\n", (long long int) read_index);
  return hb->log[hb->state->read_index].global_rate;
}

/**
       * Returns the heart rate over the last 
       * window (as specified to init) heartbeats
       * @param hb pointer to heartbeat_t
       * @return the heart rate (double) over the last window 
       */
double hb_get_windowed_rate(heartbeat_t volatile * hb) {
  //uint64_t read_index =  (hb->state->buffer_index-1) % hb->state->buffer_depth;
  //printf("Reading from %lld\n", (long long int) read_index);
  return hb->log[hb->state->read_index].window_rate;
}

/**
       * Returns the minimum desired heart rate
       * @param hb pointer to heartbeat_t
       * @return the minimum desired heart rate (double)
       */
double hb_get_min_rate(heartbeat_t volatile * hb) {
  return hb->state->min_heartrate;
}

/**
       * Returns the maximum desired heart rate
       * @param hb pointer to heartbeat_t
       * @return the maximum desired heart rate (double)
       */
double hb_get_max_rate(heartbeat_t volatile * hb) {
  return hb->state->max_heartrate;
}

/**
       * Returns the size of the sliding window 
       * used to compute the current heart rate
       * @param hb pointer to heartbeat_t 
       * @return the size of the sliding window (int64_t)
       */
int64_t hb_get_window_size(heartbeat_t volatile * hb) {
  return hb->state->window_size;
}

/**
       * Helper function to compute windowed heart rate
       * @param hb pointer to heartbeat_t
       * @param time int64_t
       */
static inline float hb_window_average(heartbeat_t volatile * hb, 
				      int64_t time) {
  int i;
  double average_time = 0;
  double fps;
  

  if(!hb->steady_state) {
    hb->window[hb->current_index] = time;

    for(i = 0; i < hb->current_index+1; i++) {
      average_time += (double) hb->window[i];
    }
    average_time = average_time / ((double) hb->current_index+1);
    hb->last_average_time = average_time;
    hb->current_index++;
    if( hb->current_index == hb->window_size) {
      hb->current_index = 0;
      hb->steady_state = 1;
    }
  }
  else {
    average_time = 
      hb->last_average_time - 
      ((double) hb->window[hb->current_index]/ (double) hb->window_size);
    average_time += (double) time /  (double) hb->window_size;

    hb->last_average_time = average_time;

    hb->window[hb->current_index] = time;
    hb->current_index++;

    if( hb->current_index == hb->window_size)
      hb->current_index = 0;
  }
  fps = (1.0 / (float) average_time)*1000000000;

  return fps;
}

/**
       * 
       * @param hb pointer to heartbeat_t
       */
static void hb_flush_buffer(heartbeat_t volatile * hb) {
  int64_t i;
  int64_t nrecords = hb->state->buffer_depth;

  //printf("Flushing buffer - %lld records\n", 
  //	 (long long int) nrecords);

  if(hb->text_file != NULL) {
    for(i = 0; i < nrecords; i++) {
      fprintf(hb->text_file, 
	      "%lld    %d    %lld    %f    %f    %f\n", 
	      (long long int) hb->log[i].beat,
	      hb->log[i].tag,
	      (long long int) hb->log[i].timestamp,
	      hb->log[i].global_rate,
	      hb->log[i].window_rate,
	      hb->log[i].instant_rate);
    }
    
    fflush(hb->text_file);
  }
}


/**
       * Registers a heartbeat
       * @param hb pointer to heartbeat_t
       * @param tag integer
       */
int64_t heartbeat( heartbeat_t* hb, int tag )
{
    struct timespec time_info;
    int64_t time;
    int64_t old_last_time = hb->last_timestamp;

    //printf("Registering Heartbeat\n");
   clock_gettime( CLOCK_REALTIME, &time_info );

    time = ( (int64_t) time_info.tv_sec * 1000000000 + (int64_t) time_info.tv_nsec );
    pthread_mutex_lock(&hb->mutex);
    hb->last_timestamp = time;

    
    if(hb->first_timestamp == -1) {
      //printf("In heartbeat - first time stamp\n");
      hb->first_timestamp = time;
      hb->last_timestamp  = time;
      hb->window[0] = 0;
      
      //printf("             - accessing state and log\n");
      hb->log[0].beat = hb->state->counter;
      hb->log[0].tag = tag;
      hb->log[0].timestamp = time;
      hb->log[0].window_rate = 0;
      hb->log[0].instant_rate = 0;
      hb->log[0].global_rate = 0;
      hb->state->counter++;
      hb->state->buffer_index++;
      hb->state->valid = 1;
    }
    else {
      //printf("In heartbeat - NOT first time stamp - read index = %d\n",hb->state->read_index );
      int index =  hb->state->buffer_index;
      hb->last_timestamp = time;
      double window_heartrate = hb_window_average(hb, time-old_last_time);
      double global_heartrate = 
	(((double) hb->state->counter+1) / 
	 ((double) (time - hb->first_timestamp)))*1000000000.0;
      double instant_heartrate = 1.0 /(((double) (time - old_last_time))) * 
	1000000000.0;

      hb->log[index].beat = hb->state->counter;
      hb->log[index].tag = tag;
      hb->log[index].timestamp = time;
      hb->log[index].window_rate = window_heartrate;
      hb->log[index].instant_rate = instant_heartrate;
      hb->log[index].global_rate = global_heartrate;
      hb->state->buffer_index++;
      hb->state->counter++;
      hb->state->read_index++;

      if(hb->state->buffer_index%hb->state->buffer_depth == 0) {
	if(hb->text_file != NULL)
	  hb_flush_buffer(hb);
	hb->state->buffer_index = 0;
      }
      if(hb->state->read_index%hb->state->buffer_depth == 0) {
	hb->state->read_index = 0;
      }
    }
    pthread_mutex_unlock(&hb->mutex);
    return time;

}

#if 0

/**
       * Initializes a heartbeat monitor
       * @param hb pointer to heartbeat_t
       * @param pid integer
       * @return rc
       */
int heart_monitor_init(heartbeat_t volatile * hb, int pid) {
  int shmid;
  key_t key;
  int rc = 0;

  key = pid;

  /* Need to find a way to incorporate window size into this */
  if((shmid = shmget(key, 1*sizeof(heartbeat_record_t), 0666)) < 0) {
    rc = 1;
  }
  
  if ((hb->monitor_log = (HB_record_t*) shmat(shmid, NULL, 0)) == (HB_record_t*) -1) {
    rc = 1;
  }


  return rc;

}


/**
       * Finalizes the heartbeat monitor
       * @param hb pointer to heartbeat_t
       */
void heart_monitor_finish(heartbeat_t* heart) {

  // TODO: put something here.

}

#endif

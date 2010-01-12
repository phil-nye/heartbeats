#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef FILEBASED
   #include "heartbeat.h"
#else
   #include "heartbeat.h"
#endif

heartbeat_t heart;

/**
       * 
       * @param argv[1]: total number of heartbeats to issue
       * @param argv[2]: name of log file
       * @see
       * @return 
       */
int main(int argc, char** argv) {

   if ( argc != 3 )
   {
      printf("usage:\n");
      printf("  application num_beats, log_file\n");
      return -1;
   }

   int i;
   const int MAX = atoi(argv[1]);

      heartbeat_init(&heart, 0, 1000000, 100, 1000, NULL);
   //      heartbeat_init(&heart, 0, 1000000, 10, 1000, argv[2]);

   usleep(1000);

   for(i = 0; i < MAX; i++) {
      #ifdef FILEBASED
         heartbeat(&heart);
      #else
         heartbeat(&heart, i);
      #endif
   }

   printf("Global heart rate: %f, Current heart rate: %f\n",
	  hb_get_global_rate(&heart), hb_get_windowed_rate(&heart));

   heartbeat_finish(&heart);
   return 0;
}

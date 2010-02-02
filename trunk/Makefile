CXX = /usr/bin/gcc
CXXFLAGS = -Wall -Wno-unknown-pragmas -Iinc -Llib -O6
DBG = -g
DEFINES ?= 
#LDFLAGS = -lpthread -lrt -lhb-file -lhrm-file
LDFLAGS = -lpthread -lrt -lhb-shared -lhrm-shared

BINDIR = bin
LIBDIR = lib
INCDIR = ./inc
SCRATCH = ./scratch
OUTPUT = ./output
SRCDIR = ./src
ROOTS = application system tp lat
TEST_ROOTS = test1 test2
BINS = $(ROOTS:%=$(BINDIR)/%)
TESTS = $(TEST_ROOTS:%=$(BINDIR)/%)
OBJS = $(ROOTS:%=$(BINDIR)/%.o)
TEST_OBJS = $(TEST_ROOTS:%=$(BINDIR)/%.o)

all: $(BINDIR) $(SCRATCH) $(OUTPUT) $(BINS) $(TESTS)

$(BINDIR):
	-mkdir -p $(BINDIR)

$(SCRATCH):
	-mkdir -p $(SCRATCH)

$(OUTPUT):
	-mkdir -p $(OUTPUT)


$(BINDIR)/%.o : $(SRCDIR)/%.c
	$(CXX) -c $(CXXFLAGS) $(DEFINES) $(DBG) -o $@ $<


$(BINS) : $(OBJS) 

$(BINS) : % : %.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(TESTS) : $(TEST_OBJS) 

$(TESTS) : % : %.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)


bench-tp:
	$(MAKE) clean
	$(MAKE) all DEFINES="-DSHARED"
	./bin/tp 1000 "" > $(OUTPUT)/tp_shmem_based.out
	cat $(OUTPUT)/tp_shmem_based.out
	$(MAKE) clean
	$(MAKE) all DEFINES="-DFILEBASED"
	./bin/tp 1000 $(OUTPUT)/tp_file_based.log > $(OUTPUT)/tp_file_based.out
	cat $(OUTPUT)/tp_file_based.out

bench-lat:
	$(MAKE) clean
	$(MAKE) all
	ls $(SCRATCH) | $(BINDIR)/lat 1000 $(OUTPUT)/log > $(OUTPUT)/lat_shmem_based.out
	cat $(OUTPUT)/lat_shmem_based.out

$(LIBDIR)/libhb-shared.a: $(SRCDIR)/heartbeat-shared.c $(INCDIR)/heartbeat.h
	$(MAKE) $(BINDIR)/heartbeat-shared.o
	ar r $(LIBDIR)/libhb-shared.a $(BINDIR)/heartbeat-shared.o
	ranlib $(LIBDIR)/libhb-shared.a

$(LIBDIR)/libhrm-shared.a: $(SRCDIR)/heart_rate_monitor-shared.c $(INCDIR)/heart_rate_monitor.h
	$(MAKE) $(BINDIR)/heart_rate_monitor-shared.o
	ar r $(LIBDIR)/libhrm-shared.a $(BINDIR)/heart_rate_monitor-shared.o
	ranlib $(LIBDIR)/libhrm-shared.a

$(LIBDIR)/libhb-file.a: $(SRCDIR)/heartbeat-file.c $(INCDIR)/heartbeat.h
	$(MAKE) $(BINDIR)/heartbeat-file.o
	ar r $(LIBDIR)/libhb-file.a $(BINDIR)/heartbeat-file.o
	ranlib $(LIBDIR)/libhb-file.a

$(LIBDIR)/libhrm-file.a: $(SRCDIR)/heart_rate_monitor-file.c $(INCDIR)/heart_rate_monitor.h
	$(MAKE) $(BINDIR)/heart_rate_monitor-file.o
	ar r $(LIBDIR)/libhrm-file.a $(BINDIR)/heart_rate_monitor-file.o
	ranlib $(LIBDIR)/libhrm-file.a

## cleaning
clean:
	-rm -rf $(BINDIR) $(SCRATCH) *.log *~ $(SRCDIR)/*~

squeaky: clean
	-rm -rf $(OUTPUT)
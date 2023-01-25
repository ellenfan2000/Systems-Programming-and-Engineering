CC=gcc
CFLAGS=-O3 -fPIC
WDIR=`pwd`/..

TARGETS= ff_equal_size_allocs  ff_small_range_rand_allocs  ff_large_range_rand_allocs  ff_mymalloc_test  ff_mymalloc_test2  ff_mymalloc_test3

all: $(TARGETS)

ff_equal_size_allocs: equal_size_allocs.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -DBF -Wl,-rpath=$(WDIR) -o $@ $< -lmymalloc -lrt

ff_small_range_rand_allocs: small_range_rand_allocs.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -DBF -Wl,-rpath=$(WDIR) -o $@ $< -lmymalloc -lrt

ff_large_range_rand_allocs: large_range_rand_allocs.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -DBF -Wl,-rpath=$(WDIR) -o $@ $< -lmymalloc -lrt

ff_mymalloc_test: mymalloc_test.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -DBF -Wl,-rpath=$(WDIR) -o $@ $< -lmymalloc -lrt

ff_mymalloc_test2: mymalloc_test2.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -DBF -Wl,-rpath=$(WDIR) -o $@ $< -lmymalloc -lrt

ff_mymalloc_test3: mymalloc_test3.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -DBF -Wl,-rpath=$(WDIR) -o $@ $< -lmymalloc -lrt

clean:
	rm -f *~ *.o $(TARGETS)

clobber:
	rm -f *~ *.o

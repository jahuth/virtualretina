HDF5=../hdf5-1.6.5/hdf5/

MVAVERSION=MVASPIKE1
MVA=../MvaSpike/src/
MVASPIKE_INCLUDES=${MVA}
MVASPIKE_LIBS=${MVA}/.libs/

CXXFLAGS+=-g -Wall -I$(MVASPIKE_INCLUDES) -D$(MVAVERSION)
LDFLAGS+=-L$(MVASPIKE_LIBS) -lmvaspike -Wl,-rpath,$(MVASPIKE_LIBS)

OBJS=hdf5memspikerecorder.o
TESTS=test1 test2 test3

.PHONY: all clean allclean

all: $(OBJS) libh5wrapper.so libmvaspike_hdf5.so $(TESTS)

libmvaspike_hdf5.so: $(OBJS) libh5wrapper.so
	$(CXX)  $(OBJS) $(LDFLAGS)  -Wl,-rpath,./ -L./ -lh5wrapper -shared -o libmvaspike_hdf5.so

test1: test1.cc libmvaspike_hdf5.so
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -Wl,-rpath,./ -L./ -lmvaspike_hdf5 test1.cc -o test1
test2: test2.cc libmvaspike_hdf5.so
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -Wl,-rpath,./ -L./ -lmvaspike_hdf5 test2.cc -o test2

# tests without mvaspike
test3: test3.c libh5wrapper.so
	$(CC) $(LDFLAGS) $(CXXFLAGS) -Wl,-rpath,./ -L./ -lh5wrapper test3.c -o test3


libh5wrapper.so: h5wrapper.c
	$(HDF5)/bin/h5cc -g -Wall h5wrapper.c -shared -o libh5wrapper.so

clean:
	rm -vf $(OBJS) $(TESTS)
	rm -vf *o

allclean: clean
	rm *~ -vf

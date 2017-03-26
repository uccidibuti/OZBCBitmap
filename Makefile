#-----------------------------------------------------------

CC=g++


FLAGSLIB=-O3


IDIR=-I./headers 
	

SOURCE=./src/ozbcbitmap.cpp	
	

OBJECT=ozbcbitmap.o
	
	
LIB=-L./lib -lOZBC
#------------------------------------------------------------

all:
	rm -fr lib
	$(CC) $(SOURCE) $(IDIR) $(FLAGSLIB) -c
	mkdir lib
	ar rvs ./lib/libOZBC.a $(OBJECT)
	rm -f *.o


index: ./test/index.cpp
	$(CC) ./test/index.cpp $(IDIR) $(LIB) -o index
	./index



perf: ./test/perf.cpp
	$(CC) ./test/perf.cpp $(IDIR) $(LIB) -o perf
	./perf

	
serialize: ./test/serialize.cpp
	$(CC) ./test/serialize.cpp $(IDIR) $(LIB) -o serialize
	./serialize

	
clean:
	rm -f index 
	rm -f perf
	rm -f serialize 
	rm -f *.o


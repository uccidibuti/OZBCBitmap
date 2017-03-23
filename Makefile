#-----------------------------------------------------------

CC=g++


FLAGSLIB=-O3


IDIR=-I./headers 
	

SOURCE=./src/ozbc.cpp	
	

OBJECT=ozbc.o
	
	
LIB=-L./lib -lOZBC
#------------------------------------------------------------

all:
	rm -fr lib
	$(CC) $(SOURCE) $(IDIR) $(FLAGSLIB) -c
	mkdir lib
	ar rvs ./lib/libOZBC.a $(OBJECT)
	rm -f *.o


index:
	$(CC) ./test/index.cpp $(IDIR) $(LIB) -o index
	./index



perf:
	$(CC) ./test/perf.cpp $(IDIR) $(LIB) -o perf
	./perf

	
serialize:
	$(CC) ./test/serialize.cpp $(IDIR) $(LIB) -o serialize
	./serialize

	
clean:
	rm -f index 
	rm -f perf
	rm -f serialize 
	rm -f *.o


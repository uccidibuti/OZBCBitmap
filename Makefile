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


test:
	$(CC) main_test.cpp $(IDIR) $(LIB) -o main_test
	./main_test

example:
	$(CC) example_index.cpp $(IDIR) $(LIB) -o example_index
	./example_index

	
clean:
	rm -f main_test 
	rm -f example_index 
	rm -f *.o


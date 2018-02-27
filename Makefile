#-----------------------------------------------------------
CPP = g++

CPPFLAGS = -O3

IDIR=-I./include 
	
SOURCE=./src/ozbcbitmap.cpp	
	
OBJECT=ozbcbitmap.o
	
LIBOZBCBITMAP=./lib/libOZBCBitmap.a

LIB=$(LIBOZBCBITMAP)

TEST_1 = index
TEST_2 = perf
TEST_3 = serialize

TESTS = $(TEST_1) $(TEST_2) $(TEST_3)

#------------------------------------------------------------

all: $(LIBOZBCBITMAP) $(TESTS)

$(LIBOZBCBITMAP):
	$(CPP) $(SOURCE) $(IDIR) $(CPPFLAGS) -c
	ar rvs $(LIBOZBCBITMAP) $(OBJECT)
	rm -f *.o

$(TEST_1): ./test/index.cpp
	$(CPP) ./test/index.cpp $(IDIR) $(LIB) $(CPPFLAGS) -o $(TEST_1)

$(TEST_2): ./test/perf.cpp
	$(CPP) ./test/perf.cpp $(IDIR) $(LIB) $(CPPFLAGS) -o $(TEST_2)
	
$(TEST_3): ./test/serialize.cpp
	$(CPP) ./test/serialize.cpp $(IDIR) $(LIB) $(CPPFLAGS) -o $(TEST_3)
	
clean:
	rm -f $(LIBOZBCBITMAP)
	rm -f $(TEST_1) 
	rm -f $(TEST_2)
	rm -f $(TEST_3) 
	rm -f *.o


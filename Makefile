PROG := RTest
CXX := g++
CXXFLAGS := -std=c++17 -pedantic -Wall -fpermissive -g
MEMDEBUG := -fsanitize=address

all: $(PROG)

RTest: RTest.o RCode.o
	$(CXX) $(CXXFLAGS) RCode.o RTest.o  -o RTest -lboost_unit_test_framework 
RTest.o: ./R0/RTest.cpp 
	$(CXX) $(CXXFLAGS) -c ./R0/RTest.cpp
RCode.o: ./R0/RCode.cpp ./R0/RCode.h
	$(CXX) $(CXXFLAGS) -c ./R0/RCode.h  ./R0/RCode.cpp 
clean:
	rm *.o $(PROG)
new:
	make clean
	make

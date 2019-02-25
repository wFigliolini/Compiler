PROG := RTest
CXX := g++
CXXFLAGS := -std=c++11 -pedantic -Wall -Werror

all: $(PROG)

RTest: RTest.o RCode.o
	$(CXX) $(CXXFLAGS)  RTest.o RCode.o -o RTest -lboost_unit_test_framework
	
RTest.o: ./R0/RTest.cpp 
	$(CXX) $(CXXFLAGS) -c ./R0/RTest.cpp
RCode.o: ./R0/RCode.cpp ./R0/RCode.h
	$(CXX) $(CXXFLAGS) -c ./R0/RCode.h  ./R0/RCode.cpp
clean:
	rm *.o $(PROG)

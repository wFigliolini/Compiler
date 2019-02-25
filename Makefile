PROG := RTest
CXX := g++
CXXFLAGS := -std=c++11 -pedantic -Wall -Werror

all: $(PROG)

RTest: RTest.o
	$(CXX) $(CXXFLAGS)  RTest.o -o RTest -lboost_unit_test_framework
	
RTest.o: ./R0/RTest.cpp ./R0/RCode.h
	$(CXX) $(CXXFLAGS) -c ./R0/RCode.h  ./R0/RTest.cpp
clean:
	rm *.o $(PROG)

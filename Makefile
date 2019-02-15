PROG := R0Test
CXX := g++
CXXFLAGS := -std=c++11 -pedantic -Wall -Werror

all: $(PROG)

R0Test: R0Test.o
	$(CXX) $(CXXFLAGS)  R0Test.o -o R0Test -lboost_unit_test_framework
	
R0Test.o: ./R0/R0Test.cpp ./R0/R0.h
	$(CXX) $(CXXFLAGS) -c ./R0/R0.h  ./R0/R0Test.cpp
clean:
	rm *.o $(PROG)

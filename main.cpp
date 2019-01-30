#include "R0/R0.h"
#include <stack>
#include <boost/test/unit_test.hpp>

int RTest();

int main(int argc, char* argv) {
	int status = RTest();
	if ( status = 0 ) {
		return 0;
	}
	else {
		std::cout << "Tests Failed with code " << status << std::endl;
	}
}

int RTest()
{
	int status = 0;
	//init stack for reads
	std::stack<int> numgen;
	for (int i = 0; i < 43; ++i) {
		numgen.push(i);
	}

	//testing
	Program* R0Test1 = new Program();
	R0Test1->setExpr(new Neg(new Add(new Num(17), new Add(new Read(numgen.top()), new Num(42)))));
	numgen.pop();
	std::string temp1, result1("(-(+17 (+(Read) 42)))");
	int  int1, final1(-101);

	Program* R0Test2 = new Program(NULL, new Add(new Num(7), new Num(13)));
	std::string temp2, result2("(+17 13)");
	int  int2, final2(20);

	auto R0TestNulls = new Program();
	Program* testRead(NULL, new Read());
	// setting up for first set of tests
	test1 = R0Test1->print();
	int1 = R0Test1->run();

	BOOST_AUTO_TEST_CASE(AST_Test1) {
		BOOST_REQUIRE(temp1.compare(result1) == 0);
	};

	test2 = R0Test2->print();
	int2 = R0Test2->run();
	BOOST_AUTO_TEST_CASE(Inter_Test1) {
		BOOST_REQUIRE(int1 == final1);
	};
	BOOST_AUTO_TEST_CASE(AST_Test2) {
		BOOST_REQUIRE(temp2.compare(result2) == 0);
	};
	BOOST_AUTO_TEST_CASE(Inter_Test2) {
		BOOST_REQUIRE(int2 == final2);
	};
	BOOST_AUTO_TEST_CASE(Null_Test) {
		BOOST_REQUIRE(R0TestNulls->getInfo() == NULL);
		BOOST_REQUIRE(R0TestNulls->getExpr() == NULL);
	};

	//read test, needs user input & cant be automated to my knowledge

	int out;

	out = testRead->run();
	std::cout << out << endl;

	return status;
}

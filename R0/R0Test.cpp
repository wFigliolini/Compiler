#include "R0Test.h"

int RTest()
{
	int status = 0;
	//init stack for reads
	//testing
	Program* R0Test1 = new Program();
	R0Test1->setExpr(new Neg(new Add(new Num(17), new Add(new Read(1), new Num(42)))));
	numgen.pop();
	std::string temp1, result1("(-(+17 (+(Read) 42)))");
	int  int1, final1(-101);

	Program* R0Test2 = new Program(NULL, new Add(new Num(7), new Num(13)));
	std::string temp2, result2("(+17 13)");
	int  int2, final2(20);

	Program* R0Test3 = new Program(NULL, new Add(new Add(new Num(7), new Read(1)), new Add(new Num(13), new Read(1))));
	std::string temp3, result3("(+(+17 (Read)) (+13 (Read)))");
	int  int3, final3(103);

	Program* R0Test4 = new Program(NULL, new Neg(new Num(7)));
	std::string temp4, result4("(-7)");
	int  int4, final4(7);

	Program* R0Test5 = new Program(NULL, new Num(5));
	std::string temp5, result5("5");
	int  int5, final5(5);

	auto R0TestNulls = new Program();
	Program* testRead(NULL, new Read());
	// setting up for first set of tests
	test1 = R0Test1->print();
	int1 = R0Test1->run();

	BOOST_AUTO_TEST_CASE(AST_Test1) {
		BOOST_REQUIRE(temp1.compare(result1) == 0);
	};
	BOOST_AUTO_TEST_CASE(Inter_Test1) {
		BOOST_REQUIRE(int1 == final1);
	};
	//test for optimizations
	test2 = R0Test2->print();
	int2 = R0Test2->run();
	BOOST_AUTO_TEST_CASE(AST_Test2) {
		BOOST_REQUIRE(temp2.compare(result2) == 0);
	};
	BOOST_AUTO_TEST_CASE(Inter_Test2) {
		BOOST_REQUIRE(int2 == final2);
	};
	// test for two branches with reads
	test3 = R0Test3->print();
	int3 = R0Test3->run();
	BOOST_AUTO_TEST_CASE(AST_Test3) {
		BOOST_REQUIRE(temp3.compare(result3) == 0);
	};
	BOOST_AUTO_TEST_CASE(Inter_Test3) {
		BOOST_REQUIRE(int3 == final3);
	};

	test4 = R0Test4->print();
	int4 = R0Test4->run();
	BOOST_AUTO_TEST_CASE(AST_Test4) {
		BOOST_REQUIRE(temp4.compare(result4) == 0);
	};
	BOOST_AUTO_TEST_CASE(Inter_Test4) {
		BOOST_REQUIRE(int4 == final4);
	};

	test5 = R0Test5->print();
	int5 = R0Test5->run();
	BOOST_AUTO_TEST_CASE(AST_Test5) {
		BOOST_REQUIRE(temp5.compare(result5) == 0);
	};
	BOOST_AUTO_TEST_CASE(Inter_Test5) {
		BOOST_REQUIRE(int5 == final5);
	};

	BOOST_AUTO_TEST_CASE(Null_Test) {
		BOOST_REQUIRE(R0TestNulls->getInfo() == NULL);
		BOOST_REQUIRE(R0TestNulls->getExpr() == NULL);
	};

	return status;
}

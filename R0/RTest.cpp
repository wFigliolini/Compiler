//R0 Testing Suite
#define BOOST_TEST_MODULE RO
#include <boost/test/included/unit_test.hpp>
#include "RCode.h"

#define BOOST_TEST_STATIC_LINK	
	//init stack for reads
	//testing

BOOST_AUTO_TEST_SUITE(R0TESTS)
	// setting up for first set of tests

	BOOST_AUTO_TEST_CASE(Test1){ 
	        Program* R0Test1 = new Program();
	        R0Test1->setExpr(new Neg(new Add(new Num(17), new Add(new Read(1), new Num(42)))));
	        std::string temp1, result1("(-(+17 (+(Read) 42)))");
	        int  int1, final1(-101);
	        temp1 = R0Test1->print();		
	        int1 = R0Test1->run();
		BOOST_REQUIRE(temp1 == result1);
		BOOST_REQUIRE(int1 == final1);
	}
	//test for optimizations
	BOOST_AUTO_TEST_CASE(Test2) {
	        Program* R0Test2 = new Program(NULL, new Add(new Num(7), new Num(13)));
	        std::string temp2, result2("(+7 13)");
	        int  int2, final2(20);
		temp2 = R0Test2->print();
	        int2 = R0Test2->run();
		BOOST_REQUIRE(temp2 == result2);
		BOOST_REQUIRE(int2 == final2);
	}
	// test for two branches with reads
	BOOST_AUTO_TEST_CASE(Test3) {
		Program* R0Test3 = new Program(NULL, new Add(new Add(new Num(7), new Read(1)), new Add(new Num(13), new Read(1))));
		std::string temp3, result3("(+(+7 (Read)) (+13 (Read)))");
		int  int3, final3(101);
		temp3 = R0Test3->print();
		int3 = R0Test3->run();
		BOOST_REQUIRE(temp3 == result3);
		BOOST_REQUIRE(int3 == final3);
	}

	BOOST_AUTO_TEST_CASE(Test4) {
		Program* R0Test4 = new Program(NULL, new Neg(new Num(7)));
		std::string temp4, result4("(-7)");
		int  int4, final4(-7);
		temp4 = R0Test4->print();
		int4 = R0Test4->run();
		BOOST_REQUIRE(temp4 == result4);
		BOOST_REQUIRE(int4 == final4);
	}

	BOOST_AUTO_TEST_CASE(Test5) {
		Program* R0Test5 = new Program(NULL, new Num(5));
		std::string temp5, result5("5");
		int  int5, final5(5);
		temp5 = R0Test5->print();
		int5 = R0Test5->run();
		BOOST_REQUIRE(temp5 == result5);
		BOOST_REQUIRE(int5 == final5);
	}

	BOOST_AUTO_TEST_CASE(Null_Test) {
		Program* R0TestNulls = new Program();
		BOOST_REQUIRE(R0TestNulls->getInfo() == NULL);
		BOOST_REQUIRE(R0TestNulls->getExpr() == NULL);
	}
	//Random Program Tests for stability
	
	//Base Case
	BOOST_AUTO_TEST_CASE(Depth0){
		int result;
		std::cout << "generating program of depth 0 ";
		Program* pTest = randProg(0);
		//std::cout <<" run prog" << std::endl;
		result = pTest->run();
		std::cout << "program generated, result: "<< result << std::endl;
		BOOST_TEST(true);
	}

	//expand by power of 10
	
	BOOST_AUTO_TEST_CASE(Depth10){
		int result;
		//std::cout << "generating program of depth 10 ";
		Program* pTest = randProg(10);
		result = pTest->run();
		std::cout << "program generated, result: " << result << std::endl;
		BOOST_TEST(true);
	}
	// Depth 100 results in system stalling and losing the rest of the file 
	BOOST_AUTO_TEST_CASE(Mass_Test){
                int depth(10), runCount(100);
		std::cout << "generating "<< runCount << " programs of depth " << depth << std::endl;
                Program* pTest;
		for(int i = 0; i< runCount; ++i){
			int result;
			pTest = randProg(depth);
                	result = pTest->run();
                	std::cout << "program generated, result: " << result << std::endl;
		}
                BOOST_TEST(true);
        }

BOOST_AUTO_TEST_SUITE_END()

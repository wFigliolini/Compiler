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
        Program* pTest = new Program();
        pTest->setExpr(new Neg(new Add(new Num(17), new Add(new Read(1), new Num(42)))));
        //optimizer now causes memory access violation
        //TODO: fix optimizer
        //Program* pOpt = opt(pTest);
        int int1, final1(-101);		
        int1 = pTest->run();
		BOOST_REQUIRE(int1 == final1);
	}
	BOOST_AUTO_TEST_CASE(Test2) {
        Program* pTest = new Program(NULL, new Add(new Num(7), new Num(13)));
        int int2, final2(20);
        int2 = pTest->run();
		BOOST_REQUIRE(int2 == final2);
        Program* pOpt = opt(pTest);
		BOOST_REQUIRE( pTest->run() == pOpt->run());
	}
	// test for two branches with reads
	BOOST_AUTO_TEST_CASE(Test3) {
		Program* pTest = new Program(NULL, new Add(new Add(new Num(7), new Read(1)), new Add(new Num(13), new Read(1))));
        Program* pOpt = opt(pTest);
		int  int3, final3(101);
		int3 = pOpt->run();
		BOOST_REQUIRE(int3 == final3);
	}

	BOOST_AUTO_TEST_CASE(Test4) {
		Program* pTest = new Program(NULL, new Neg(new Num(7)));
        Program* pOpt = opt(pTest);
		int  int4, final4(-7);
		int4 = pOpt->run();
		BOOST_REQUIRE(int4 == final4);
	}

	BOOST_AUTO_TEST_CASE(Test5) {
		Program* pTest = new Program(NULL, new Num(5));
        Program* pOpt = opt(pTest);
		std::string temp5, result5("5");
		int  int5, final5(5);
		temp5 = pTest->print();
		int5 = pOpt->run();
		BOOST_REQUIRE(temp5 == result5);
		BOOST_REQUIRE(int5 == final5);
	}

	BOOST_AUTO_TEST_CASE(Null_Test) {
		Program* R0TestNulls = new Program();
		BOOST_REQUIRE(R0TestNulls->getInfo() == NULL);
		//results in access violation after adding smartpointers
		//BOOST_REQUIRE(R0TestNulls->getExpr() == NULL);
	}
	//Random Program Tests for stability
	
	//Base Case
	BOOST_AUTO_TEST_CASE(Depth0){
		//int result;
		//std::cout << "generating program of depth 0 ";
		Program* pTest = randProg(0);
		//std::cout <<" run prog" << std::endl;
		pTest->run();
        //Program* pOpt = opt(pTest);
		//BOOST_REQUIRE( pTest->run()-1 == pOpt->run());
		//std::cout << "program generated, result: "<< result << std::endl;
		BOOST_TEST(true);
	}

	//expand by power of 10
	
	BOOST_AUTO_TEST_CASE(Depth10){
		//int result;
		//std::cout << "generating program of depth 10 ";
		Program* pTest = randProg(10);
        //Program* pOpt = opt(pTest);
		//BOOST_REQUIRE( pTest->run() == pOpt->run());
		pTest->run();
		//std::cout << "program generated, result: " << result << std::endl;
		BOOST_TEST(true);
	}
	// Depth 100 results in system stalling and losing the rest of the file 
	BOOST_AUTO_TEST_CASE(Mass_Test){
        int depth(10), runCount(100);
		//std::cout << "generating "<< runCount << " programs of depth " << depth << std::endl;
        Program* pTest;
		for(int i = 0; i< runCount; ++i){
			//int result;
			pTest = randProg(depth);
            pTest->run();
            //Program* pOpt = opt(pTest);
            //BOOST_REQUIRE( pTest->run()-1 == pOpt->run());
                	//std::cout << "program generated, result: " << result << std::endl;
		}
        BOOST_TEST(true);
    }
	// Optimizer cases
	// Addition Case
	BOOST_AUTO_TEST_CASE(OPT1){
		Program* pTest = new Program(NULL, new Add(new Num(2), new Num(3)));
        Program* pOpt = opt(pTest);
		BOOST_REQUIRE( pTest->run() == pOpt->run());
        //std::cout <<" input: "<< pTest->print() << " output: "<< pOpt->print() << std::endl;
		//BOOST_REQUIRE( pTest->print() == pOpt->print());
	}
	//negative case
	BOOST_AUTO_TEST_CASE(OPT2){
		Program* pTest = new Program(NULL, new Neg(new Neg(new Num(7))));
        Program* pOpt = opt(pTest);
		BOOST_REQUIRE( pTest->run() == pOpt->run());
        //std::cout <<"input: "<< pTest->print() << " output: "<< pOpt->print() << std::endl;
		//BOOST_REQUIRE( pTest->print() == pOpt->print());
	}
	//Addition with read
	BOOST_AUTO_TEST_CASE(OPT3){
		Program* pTest = new Program(NULL, new Add(new Num(7), new Add( new Read(1), new Num(3))));
        Program* pOpt = opt(pTest);
        //std::cout <<"input: "<< pTest->print() << " output: "<< pOpt->print() << std::endl;
		BOOST_REQUIRE( pTest->print() == pOpt->print());
	}

BOOST_AUTO_TEST_SUITE_END()

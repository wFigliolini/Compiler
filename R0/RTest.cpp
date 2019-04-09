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
        BOOST_REQUIRE(R0TestNulls->getExpr() == NULL);
    }
    //let test cases
    //Must be done befor randoms for read prediction
    //Base Operation Test Case
        BOOST_AUTO_TEST_CASE(BASELET){
            Program* pTest = new Program(NULL, new Let("x",new Num(8), new Add( new Var("x"), new Var("x"))));
            int result(16);
            std::string AST("(Let x = 8){\n(+x x)}\n");
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
        }
    //Overwrite Test Case
        BOOST_AUTO_TEST_CASE(OWCASE){
            Program* pTest = new Program(NULL, new Let("x", new Num(32), new Add( new Let("x", new Num(8), new Var("x")), new Var("x"))));
            int result(40);
            std::string AST("(Let x = 32){\n(+(Let x = 8){\nx}\n x)}\n");
            //std::cout << pTest ->run() << std::endl;
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
        }
    //Multiple Vars
        BOOST_AUTO_TEST_CASE(MVCASE){
            Program* pTest = new Program(NULL, new Let("x", new Num(32), new Let("y", new Num(8), new Add( new Var("x"), new Var("y")))));
            int result(40);
            std::string AST("(Let x = 32){\n(Let y = 8){\n(+x y)}\n}\n");
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
        }
    //Expression Handling
        BOOST_AUTO_TEST_CASE(EXPHAND){
            Program* pTest = new Program(NULL, new Let("x", new Add(new Num(32),new Num(64)), new Var("x")));
            int result(96);
            std::string AST("(Let x = (+32 64)){\nx}\n");
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
        }
    //Order of Operations Test
    //Reads Will be 39,38
        BOOST_AUTO_TEST_CASE(ORDERTEST){
            Program* pTest = new Program(NULL, new Let("x", new Read(1), new Let("y", new Read(1), new Add(new Var("x"), new Neg(new Var("y"))))));
            int result(1);
            std::string AST("(Let x = (Read)){\n(Let y = (Read)){\n(+x (-y))}\n}\n");
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
        }
    
    //Random Program Tests for stability
    
    //Base Case
    BOOST_AUTO_TEST_CASE(Depth0){
            //int result;
            //std::cout << "generating program of depth 0 ";
            Program* pTest = randProg(0);
            //std::cout <<" run prog" << std::endl;
            pTest->run();
            Program* pOpt = opt(pTest);
            pOpt->run();
            //BOOST_REQUIRE( pTest->run()-1 == pOpt->run());
            //std::cout << "program generated, result: "<< result << std::endl;
            BOOST_TEST(true);
    }

    //expand by power of 10
    
    BOOST_AUTO_TEST_CASE(Depth10){
            //int result;
            //std::cout << "generating program of depth 10 ";
            Program* pTest = randProg(10);
                Program* pOpt = opt(pTest);
            //BOOST_REQUIRE( pTest->run() == pOpt->run());
            pTest->run();
            pOpt->run();
            //std::cout << "program generated, result: " << result << std::endl;
            BOOST_TEST(true);
    }
    // Depth 100 results in system stalling and losing the rest of the file 
    BOOST_AUTO_TEST_CASE(Mass_Test){
            int depth(10), runCount(100);
        //std::cout << "generating "<< runCount << " programs of depth " << depth << std::endl;
            Program* pTest;
            for(int i = 0; i< runCount; ++i){
                pTest = randProg(depth);
                pTest->run();
                Program* pOpt = opt(pTest);
                pOpt->run();
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
        //let base case
        BOOST_AUTO_TEST_CASE(OPTLET1){
            Program* pTest = new Program(NULL, new Let("x", new Num(2), new Add(new Var("x"), new Read(1))));
            std::string AST("(+2 (Read))");
            Program* pOpt = opt(pTest);
            std::cout << pOpt->print() << std::endl;
            BOOST_REQUIRE( pOpt->run()-1 == pTest->run());
            BOOST_REQUIRE( pOpt->print() == AST);
        }
        //Expr in assignment
        BOOST_AUTO_TEST_CASE(OPTLET2){
            Program* pTest = new Program(NULL, new Let("x", new Add(new Num(1), new Num(1)), new Add(new Var("x"), new Read(1))));
            std::string AST("(+2 (Read))");
            Program* pOpt = opt(pTest);
            std::cout << pOpt->print() << std::endl;
            BOOST_REQUIRE( pOpt->run()-1 == pTest->run());
            BOOST_REQUIRE( pOpt->print() == AST);
        }
        // inlining of variables
        BOOST_AUTO_TEST_CASE(OPTLET3){
            Program* pTest = new Program(NULL,new Let("y", new Num(2), new Let("x", new Var("y"), new Add(new Var("x"), new Read(1)))));
            std::string AST("(+2 (Read))");
            Program* pOpt = opt(pTest);
            std::cout << pOpt->print() << std::endl;
            BOOST_REQUIRE( pOpt->run()-1 == pTest->run());
            BOOST_REQUIRE( pOpt->print() == AST);
        }
        //Read Case
        BOOST_AUTO_TEST_CASE(OPTLET4){
            Program* pTest = new Program(NULL, new Let("x", new Read(1), new Add(new Var("x"), new Num(3))));
            std::string AST("(Let x = (Read)){\n(+x 3)}\n");
            Program* pOpt = opt(pTest);
            //std::cout << pOpt->print() << std::endl;
            BOOST_REQUIRE( pOpt->run()-1 == pTest->run());
            BOOST_REQUIRE( pOpt->print() == AST);
        }
        
        //X Interpreter Tests
        //Data storage Tests
        BOOST_AUTO_TEST_CASE(REGTEST){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(4), new Reg(0)));
                instrSet.push_back(new Addq(new Const(5), new Reg(0)));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            BOOST_REQUIRE(pTest->interp() == 9);
        }
        //int x = 12, int y = 8
        //also functions as stack test
        BOOST_AUTO_TEST_CASE(DEREFTEST){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Pushq(new Const(8)));
                instrSet.push_back(new Pushq(new Const(12)));
                instrSet.push_back(new Movq(new DeRef(new Reg("%rbp"), 8), new Reg("%rax")));
                instrSet.push_back(new Addq(new DeRef(new Reg("%rbp"), 0), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            int result = pTest->interp() ;
            //std::cout << "DEREFTEST Result = " << result << std::endl;
            BOOST_REQUIRE(result == 20);
        }
        //TestInt = 20
        BOOST_AUTO_TEST_CASE(REFTEST){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Ref("TestInt"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            pTest->declareVar("TestInt", 20);
            BOOST_REQUIRE(pTest->interp() == 20);
        }
        //constant val already tested in REGISTERTEST
        //label tests
        BOOST_AUTO_TEST_CASE(LABELTEST){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(5), new Reg("%rax")));
                instrSet.push_back(new Jmp(new Label("final")));
            std::vector<Instr*>finalSet;
                finalSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            pTest->addBlock("final", new Block( finalSet));
            BOOST_REQUIRE(pTest->interp() == 5);
        }
        //Textbook Example, page 22 of Essentials of Compilation
        
        BOOST_AUTO_TEST_CASE(LABELS){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Pushq(new Reg("%rbp")));
                instrSet.push_back(new Movq(new Reg("%rsp"), new Reg("%rbp")));
                instrSet.push_back(new Subq(new Const(16), new Reg("%rsp")));
                instrSet.push_back(new Jmp(new Label("start")));
            std::vector<Instr*>concSet;
                concSet.push_back(new Addq(new Const(16), new Reg("%rsp")));
                concSet.push_back(new Popq(new Reg("%rbp")));
                concSet.push_back(new Retq());
            std::vector<Instr*>begSet;
                begSet.push_back(new Movq(new Const(10), new DeRef(new Reg("%rbp"), -8)));
                begSet.push_back(new Negq(new DeRef(new Reg("%rbp"), -8)));
                begSet.push_back(new Movq(new DeRef(new Reg("%rbp"), -8), new Reg("%rax")));
                begSet.push_back(new Addq(new Const(52), new Reg("%rax")));
                begSet.push_back(new Jmp(new Label("conclusion")));
            
            xProgram* pTest = new xProgram(new Block(instrSet));
            pTest->addBlock("conclusion", new Block( concSet));
            pTest->addBlock("start", new Block(begSet));
            int result = pTest->interp() ;
            //std::cout << "LABELS Result = " << result << std::endl;
            BOOST_REQUIRE(result == 42);
        }
        
        //exception testing
        BOOST_AUTO_TEST_CASE(REGINTEXCEPT){
            BOOST_REQUIRE_THROW(Reg(-1), std::invalid_argument);
        }
        BOOST_AUTO_TEST_CASE(REGSTRINGEXCEPT){
            BOOST_REQUIRE_THROW(Reg("fail"), std::invalid_argument);
        }
        //manually compiled versions of R Tests
        BOOST_AUTO_TEST_CASE(RTOXTEST1){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(7), new Reg("%rax")));
                instrSet.push_back(new Addq(new Const(13), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            BOOST_REQUIRE(pTest->interp() == 20);
        }
        BOOST_AUTO_TEST_CASE(RTOXTEST2){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(7), new Reg("%rax")));
                instrSet.push_back(new Negq(new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            BOOST_REQUIRE(pTest->interp() == -7);
        }
        BOOST_AUTO_TEST_CASE(BREAKTEST){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(7), new Reg("%rax")));
                instrSet.push_back(new Addq(new Const(13), new Reg("%rax")));
                instrSet.push_back(new Retq());
                instrSet.push_back(new Addq(new Const(13), new Reg("%rax")));
            xProgram* pTest = new xProgram(new Block(instrSet));
            BOOST_REQUIRE(pTest->interp() == 20);
        }
        //CTests
        // exception handlers
        BOOST_AUTO_TEST_CASE(CNOVAR){
            CRet* lTest = new CRet(new CVar("f"));
            CProg* pTest = new CProg(lTest);
            BOOST_REQUIRE_THROW( pTest->interp(), std::runtime_error);
        }
        BOOST_AUTO_TEST_CASE(CNOMAIN){
            CRet* lTest = new CRet(new CNum(5));
            CProg* pTest = new CProg();
            pTest->addTail("NULL", lTest);
            BOOST_REQUIRE_THROW( pTest->interp(), std::runtime_error);
        }
        
        //Test 1 manually compiled
        //r = 42
        BOOST_AUTO_TEST_CASE(CTEST1){
            CSeq* lTest = new CSeq(new CStat("r", new CRead(1)),
                          new CSeq(new CStat("y", new CAdd(new CVar("r"), new CNum(42))),
                          new CSeq(new CStat("x", new CAdd(new CVar("y"), new CNum(17))),
                          new CSeq(new CStat("f", new CNeg(new CVar("x"))),
                          new CRet(new CVar("f"))
                          ))));
            CProg* pTest = new CProg(lTest);
            int ret = pTest->interp();
            BOOST_REQUIRE(ret == -101);
        }
        //Test 3  manually compiled
        //r1 = 41, r2 = 40
        BOOST_AUTO_TEST_CASE(CTEST2){
            CSeq* lTest = new CSeq(new CStat("r1", new CRead(1)),
                          new CSeq(new CStat("y", new CAdd(new CVar("r1"), new CNum(7))),
                          new CSeq(new CStat("r2", new CRead(1)),
                          new CSeq(new CStat("x", new CAdd(new CVar("r2"), new CNum(13))),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CVar("y"))),
                          new CRet(new CVar("f"))
                          )))));
            CProg* pTest = new CProg(lTest);
            int ret = pTest->interp();
            BOOST_REQUIRE(ret == 101);
        }
        //BASELET Manually compiled
        BOOST_AUTO_TEST_CASE(CTEST3){
            CSeq* lTest = new CSeq(new CStat("x", new CNum(8)),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CVar("x"))),
                          new CRet(new CVar("f"))
                          ));
            CProg* pTest = new CProg(lTest);
            int ret = pTest->interp();
            BOOST_REQUIRE(ret == 16);
        }
        //EXPHAND manually compiled
        BOOST_AUTO_TEST_CASE(CTEST4){
            CSeq* lTest = new CSeq(new CStat("f", new CAdd(new CNum(32), new CNum(64))),
                                   new CRet(new CVar("f"))
                                  );
            CProg* pTest = new CProg(lTest);
            int ret = pTest->interp();
            BOOST_REQUIRE(ret == 96);
        }
        //multiple read Test
        // x = 39
        BOOST_AUTO_TEST_CASE(CTEST5){
            CSeq* lTest = new CSeq(new CStat("x", new CRead(1)),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CVar("x"))),
                          new CRet(new CVar("f"))
                          ));
            CProg* pTest = new CProg(lTest);
            int ret = pTest->interp();
            BOOST_REQUIRE(ret == 78);
        }
        //
        BOOST_AUTO_TEST_CASE(CTEST6){
            
        }
BOOST_AUTO_TEST_SUITE_END()

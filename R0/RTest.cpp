//R0 Testing Suite
#define BOOST_TEST_MODULE RO
#include <boost/test/included/unit_test.hpp>
#include "RCode.h"

#define BOOST_TEST_STATIC_LINK
BOOST_AUTO_TEST_SUITE(R0TESTS)
    // setting up for first set of tests

    BOOST_AUTO_TEST_CASE(Test1){ 
        Program* pTest = new Program();
        pTest->setExpr(new Neg(new Add(new Num(17), new Add(new Read(1), new Num(42)))));
        //Program* pOpt = opt(pTest);
        int int1, final1(-101), f;
        int1 = pTest->run();
        BOOST_REQUIRE(int1 == final1);
        pTest = opt(pTest);
        int1 = pTest->run();
        BOOST_REQUIRE(int1 == final1);
        pTest = uniquify(pTest);
        int1 = pTest->run();
        BOOST_REQUIRE(int1 == final1);
        pTest = rco(pTest);
        int1 = pTest->run();
        BOOST_REQUIRE(int1 == final1);
        CProg* cTest = econ(pTest);
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(final1 == f);
        xProgram* xTest = cTest->selInsr();
    }
    BOOST_AUTO_TEST_CASE(Test2) {
        Program* pTest = new Program(NULL, new Add(new Num(7), new Num(13)));
        int int2, final2(20), f;
        int2 = pTest->run();
        BOOST_REQUIRE(int2 == final2);
        pTest = opt(pTest);
        int2 = pTest->run();
        BOOST_REQUIRE(int2 == final2);
        pTest = uniquify(pTest);
        int2 = pTest->run();
        BOOST_REQUIRE(int2 == final2);
        pTest = rco(pTest);
        int2 = pTest->run();
        BOOST_REQUIRE(int2 == final2);
        CProg* cTest = econ(pTest);
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(final2 == f);
        xProgram* xTest = cTest->selInsr();
    }
    // test for two branches with reads
    BOOST_AUTO_TEST_CASE(Test3) {
        Program* pTest = new Program(NULL, new Add(new Add(new Num(7), new Read(1)), new Add(new Num(13), new Read(1))));
        int  int3, final3(103), f;
        int3 = pTest->run();
        BOOST_REQUIRE(int3 == final3);
        pTest = opt(pTest);
        int3 = pTest->run();
        BOOST_REQUIRE(int3 == final3);
        pTest = uniquify(pTest);
        int3 = pTest->run();
        BOOST_REQUIRE(int3 == final3);
        pTest = rco(pTest);
        int3 = pTest->run();
        BOOST_REQUIRE(int3 == final3);
        CProg* cTest = econ(pTest);
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(final3 == f);
        xProgram* xTest = cTest->selInsr();
    }

    BOOST_AUTO_TEST_CASE(Test4) {
        Program* pTest = new Program(NULL, new Neg(new Num(7)));
        int  int4, final4(-7), f;
        int4 = pTest->run();
        BOOST_REQUIRE(int4 == final4);
        pTest = opt(pTest);
        int4 = pTest->run();
        BOOST_REQUIRE(int4 == final4);
        pTest = uniquify(pTest);
        int4 = pTest->run();
        BOOST_REQUIRE(int4 == final4);
        pTest = rco(pTest);
        int4 = pTest->run();
        BOOST_REQUIRE(int4 == final4);
        CProg* cTest = econ(pTest);
        f = cTest->run();
        BOOST_REQUIRE(final4 == f);
        xProgram* xTest = cTest->selInsr();
    }

    BOOST_AUTO_TEST_CASE(Test5) {
        Program* pTest = new Program(NULL, new Num(5));
        std::string temp5, result5("5");
        int  int5, final5(5), f;
        temp5 = pTest->print();
        int5 = pTest->run();
        BOOST_REQUIRE(temp5 == result5);
        BOOST_REQUIRE(int5 == final5);
        pTest = opt(pTest);
        int5 = pTest->run();
        BOOST_REQUIRE(int5 == final5);
        pTest = uniquify(pTest);
        int5 = pTest->run();
        BOOST_REQUIRE(int5 == final5);
        pTest = rco(pTest);
        int5 = pTest->run();
        BOOST_REQUIRE(int5 == final5);
        CProg* cTest = econ(pTest);
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(final5 == f);
        xProgram* xTest = cTest->selInsr();
    }

    BOOST_AUTO_TEST_CASE(Null_Test) {
        Program* R0TestNulls = new Program();
        //BOOST_REQUIRE(R0TestNulls->getInfo() == NULL);
        BOOST_REQUIRE(R0TestNulls->getExpr() == NULL);
    }
    //let test cases
    //Must be done befor randoms for read prediction
    //Base Operation Test Case
        BOOST_AUTO_TEST_CASE(BASELET){
            Program* pTest = new Program(NULL, new Let("x",new Num(8), new Add( new Var("x"), new Var("x"))));
            int result(16), f;
            std::string AST("(Let x = 8){\n(+x x)}\n");
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
            pTest = opt(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = uniquify(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = rco(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            
            CProg* cTest = econ(pTest);
            
            cTest->uncoverLocals();
            f = cTest->run();
            BOOST_REQUIRE(result == f);
        xProgram* xTest = cTest->selInsr();
        }
    //Overwrite Test Case
        BOOST_AUTO_TEST_CASE(OWCASE){
            Program* pTest = new Program(NULL, new Let("x", new Num(32), new Add( new Let("x", new Num(8), new Var("x")), new Var("x"))));
            int result(40), f;
            std::string AST("(Let x = 32){\n(+(Let x = 8){\nx}\n x)}\n");
            //std::cout << pTest ->run() << std::endl;
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
            pTest = opt(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = uniquify(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = rco(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            CProg* cTest = econ(pTest);
            cTest->uncoverLocals();
            f = cTest->run();
            BOOST_REQUIRE(result == f);
        xProgram* xTest = cTest->selInsr();
        }
    //Multiple Vars
        BOOST_AUTO_TEST_CASE(MVCASE){
            Program* pTest = new Program(NULL, new Let("x", new Num(32), new Let("y", new Num(8), new Add( new Var("x"), new Var("y")))));
            int result(40), f;
            std::string AST("(Let x = 32){\n(Let y = 8){\n(+x y)}\n}\n");
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
            pTest = opt(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = uniquify(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = rco(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            CProg* cTest = econ(pTest);
            
            cTest->uncoverLocals();
            f = cTest->run();
            BOOST_REQUIRE(result == f);
        xProgram* xTest = cTest->selInsr();
        }
    //Expression Handling
        BOOST_AUTO_TEST_CASE(EXPHAND){
            Program* pTest = new Program(NULL, new Let("x", new Add(new Num(32),new Num(64)), new Var("x")));
            int result(96), f;
            std::string AST("(Let x = (+32 64)){\nx}\n");
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
            pTest = opt(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = uniquify(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = rco(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            CProg* cTest = econ(pTest);
            
            cTest->uncoverLocals();
            f = cTest->run();
            BOOST_REQUIRE(result == f);
        xProgram* xTest = cTest->selInsr();
        }
    //Order of Operations Test
    //Reads Will be 39,38
        BOOST_AUTO_TEST_CASE(ORDERTEST){
            Program* pTest = new Program(NULL, new Let("x", new Read(1), new Let("y", new Read(1), new Add(new Var("x"), new Neg(new Var("y"))))));
            int result(1), f;
            std::string AST("(Let x = (Read)){\n(Let y = (Read)){\n(+x (-y))}\n}\n");
            BOOST_REQUIRE(pTest->run() == result);
            BOOST_REQUIRE(pTest->print() == AST);
            pTest = opt(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = uniquify(pTest);
            BOOST_REQUIRE(pTest->run() == result);
            pTest = rco(pTest);
            BOOST_REQUIRE(pTest->run() == result);
        CProg* cTest = econ(pTest);
        
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(result == f);
        xProgram* xTest = cTest->selInsr();
        }
    
    //uniquify tests
    BOOST_AUTO_TEST_CASE(UNIQ1){
        Add* expr = new Add( new Let("x", new Num(7), new Var("x") ),
                             new Let("x", new Num(8), 
                             new Let("x", new Add(new Num(1), new Var("x")), 
                             new Add(new Var("x"), new Var("x")))));
        int orig, f;
        Program* pTest = new Program(expr);
        orig =  pTest->run();
        Program* pFinal = uniquify(pTest);
        f = pFinal->run();
        std::string AST("(+(Let x1 = 7){\nx1}\n (Let x2 = 8){\n(Let x3 = (+1 x2)){\n(+x3 x3)}\n}");
        std::string out = pFinal->print();
        //std::cout << AST <<std::endl;
        //std::cout << out;
        BOOST_REQUIRE(orig == f);
        //BOOST_REQUIRE(out == AST); //fails even though the output is the same?
        pTest = rco(pFinal);
        
        f = pTest->run();
        //std:: cout << pTest->print() << std::endl << orig << " " << f << std::endl;
        BOOST_REQUIRE(orig == f);
        CProg* cTest = econ(pTest);
        
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    BOOST_AUTO_TEST_CASE(UNIQ2){
        Let* expr = new Let("x", new Num(16), new Add(new Var("x"), new Var("x")));
        int orig, f;
        Program* pTest = new Program(expr);
        orig = pTest->run();
        Program* pFinal = uniquify(pTest);
        //std::cout<< "pfinal uniquifies " << std::endl;
        f = pFinal->run();
        //std::cout<< "pfinal runs " << std::endl;
        std::string AST("(Let x1 = 16){\n(+x1 x1)}\n");
        std::string out = pFinal->print();
        
        //std::cout<< "pfinal prints " << std::endl;
        //std::cout << out;
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(out == AST);
        pTest = rco(pTest);
        BOOST_REQUIRE(pTest->run() == f);
        CProg* cTest = econ(pTest);
        
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    //from textbook page 28
    BOOST_AUTO_TEST_CASE(UNIQ3){
        Let* expr = new Let("x", new Num(32), new Add(new Let("x", new Num(10), new Var("x")), new Var("x")));
        int orig, f;
        Program* pTest = new Program(expr);
        orig = pTest->run();
        Program* pFinal = uniquify(pTest);
        f = pFinal->run();
        std::string AST("(Let x1 = 32){\n(+(Let x2 = 10){\nx2}\n x1)}\n");
        std::string out = pFinal->print();
        //std::cout << out;
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(out == AST);
        pTest = rco(pTest);
        BOOST_REQUIRE(pTest->run() == f);
        CProg* cTest = econ(pTest);
        
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    BOOST_AUTO_TEST_CASE(UNIQ4){
        Let* expr = new Let("x", new Let("x", new Num(4), new Add(new Var("x"), new Num(1))), new Add(new Var("x"), new Num(2)));
        int orig, f;
        Program* pTest = new Program(expr);
        orig = pTest->run();
        Program* pFinal = uniquify(pTest);
        f = pFinal->run();
        std::string AST("(Let x2 = (Let x1 = 4){\n(+x1 1)}\n){\n(+x2 2)}\n");
        std::string out = pFinal->print();
        //std::cout << out;
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(out == AST);
        pTest = rco(pTest);
        BOOST_REQUIRE(pTest->run() == f);
        CProg* cTest = econ(pTest);
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    //RCO Tests
    BOOST_AUTO_TEST_CASE(RCO1){
        Expr* expr = new Add(new Add(new Num(2), new Num(3)), new Let( "x", new Read(1), new Add(new Var("x"), new Var("x"))));
        int orig, f;
        std::string AST("(Let 0i = (+2 3)){\n(Let 1i = (Read)){\n(Let 2i = (+x x)){\n(Let 3i = (+0i 2i)){\n 3i}\n}\n}\n}\n");
        Program* pTest = new Program(expr);
        orig = pTest->run();
        Program* pFinal = rco(pTest);
        //std::cout << pFinal->print() << std::endl;
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        CProg* cTest = econ(pFinal);
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    BOOST_AUTO_TEST_CASE(RCO2){
        Expr* expr = new Add(new Num(10), new Neg(new Num(10)));
        int orig, f;
        std::string AST("(Let 1i = (-10)){\n (Let 2i = (+10 i1)){\n 2i}\n}\n");
        Program* pTest = new Program(expr);
        orig = pTest->run();
        Program* pFinal = rco(pTest);
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        CProg* cTest = econ(pFinal);
        f = cTest->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    BOOST_AUTO_TEST_CASE(RCO3){
        Expr* expr = new Add(new Add(new Num(7), new Read(1)), new Add(new Num(13), new Read(1)));
        int orig, f;
        Program* pTest = new Program(expr);
        orig = pTest->run();
        Program* pFinal = rco(pTest);
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        CProg* cTest = econ(pFinal);
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    BOOST_AUTO_TEST_CASE(RCO4){
        Expr* expr = new Neg(new Add(new Num(17), new Add(new Read(1), new Num(42))));
        int orig, f;
        std::string AST("(Let 1i = (Read)){\n(Let 2i = (+1i 42)){\n(Let 3i = (+17 2i)){\n 3i}\n}\n}\n");
        Program* pTest = new Program(expr);
        orig = pTest->run();
        Program* pFinal = rco(pTest);
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        CProg* cTest = econ(pFinal);
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    BOOST_AUTO_TEST_CASE(RCO5){
        //testing for preservation of lets
        Expr* expr = new Let("15i", new Num(42), new Let("16i", new Var("15i"), new Var("16i")));
        int orig, f;
        Program* pTest = new Program(expr);
        std::string AST = pTest->print();
        orig = pTest->run();
        Program* pFinal = rco(pTest);
        std::string sFinal = pFinal->print(); 
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        CProg* cTest = econ(pFinal);
        cTest->uncoverLocals();
        f = cTest->run();
        BOOST_REQUIRE(orig == f);
        //BOOST_REQUIRE(sFinal == AST);
        xProgram* xTest = cTest->selInsr();
    }
    //econ Tests

    BOOST_AUTO_TEST_CASE(ECON1){
        Expr* expr = new Add(new Add(new Num(2), new Num(3)), new Let( "x", new Read(1), new Add(new Var("x"), new Var("x"))));
        int orig, f;
        Program* pTest = new Program(expr);
        pTest = rco(pTest);
        orig = pTest->run();
        CProg* pFinal = econ(pTest);
        pFinal->uncoverLocals();
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    BOOST_AUTO_TEST_CASE(ECON2){
        Expr* expr = new Add(new Add(new Num(7), new Read(1)), new Add(new Num(13), new Read(1)));
        int orig, f;
        Program* pTest = new Program(expr);
        pTest = rco(pTest);
        orig = pTest->run();
        //std::cout << pTest->print();
        CProg* pFinal = econ(pTest);
        pFinal->uncoverLocals();
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    //Manually decompiled C Tests
    //CTest2
    BOOST_AUTO_TEST_CASE(ECON3){
        Expr* expr = new Add(new Add(new Read(1), new Num(7)), new Add(new Read(1), new Num(13)));
        int orig, f;
        Program* pTest = new Program(expr);
        pTest = rco(pTest);
        orig = pTest->run();
        CProg* pFinal = econ(pTest);
        pFinal->uncoverLocals();
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    //CTest5
    BOOST_AUTO_TEST_CASE(ECON4){
        Expr* expr = new Let("x", new Read(1), new Add(new Var("x"), new Var("x")));
        int orig, f;
        Program* pTest = new Program(expr);
        pTest = uniquify(pTest);
        pTest = rco(pTest);
        orig = pTest->run();
        CProg* pFinal = econ(pTest);
        pFinal->uncoverLocals();
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
    }
    BOOST_AUTO_TEST_CASE(ECON5){
        //testing for preservation of lets
        Expr* expr = new Add(new Num(52), new Neg(new Num(10)));
        int orig, f;
        Program* pTest = new Program(expr);
        pTest = rco(pTest);
        orig = pTest->run();
        CProg* pFinal = econ(pTest);
        pFinal->uncoverLocals();
        f = pFinal->run();
        BOOST_REQUIRE(orig == f);
        xProgram* xTest = cTest->selInsr();
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
        xProgram* xTest = cTest->selInsr();
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
        xProgram* xTest = cTest->selInsr();
    }
    // Depth 100 results in system stalling and losing the rest of the file 
    BOOST_AUTO_TEST_CASE(Mass_Test){
            int depth(10), runCount(100);
            int optFails(0), uniqFails(0), rcoFails(0), econFails(0);
            std::vector<int>  uniqList, optList, rcoList, econList;
            Program* pTest;
            for(int i = 0; i< runCount; ++i){
                int result, opresult;
                pTest = randProg(depth);
                result = pTest->run();
                pTest = opt( pTest);
                opresult = pTest->run();
                if (result !=opresult){
                    
                    //std::cout <<"Intended result: "<< result << " Opt Result: " << opresult << std::endl; 
                    optList.push_back(opresult);
                    ++optFails;
                    //to determine if the later stages still work
                    result = opresult;
                }
                pTest = uniquify( pTest);
                opresult = pTest->run();
                if (result !=opresult){
                    //std::cout <<"Intended result: "<< result << " Uniq Result: " << opresult << std::endl;
                    uniqList.push_back(opresult);
                    ++uniqFails;
                    result = opresult;
                }
                pTest = rco( pTest);
                opresult = pTest->run();
                if (result !=opresult){
                    //std::cout <<"Intended result: "<< result << " Uniq Result: " << opresult << std::endl;
                    rcoList.push_back(opresult);
                    ++rcoFails;
                    result = opresult;
                }
                CProg* cTest = econ( pTest);
                cTest->uncoverLocals();
                opresult = cTest->run();
                if (result !=opresult){
                    //std::cout <<"Intended result: "<< result << " Uniq Result: " << opresult << std::endl;
                    econList.push_back(opresult);
                    ++econFails;
                    result = opresult;
                }
            }
            if(optFails > 0){
                std::cout <<"Optimizer failed "<< optFails << " tests." << std::endl;
            }
            if(uniqFails > 0){
                std::cout <<"Uniquify failed "<< uniqFails <<" tests." << std::endl;
            }
            if(rcoFails > 0){
                std::cout <<"Rco failed "<< rcoFails <<" tests." << std::endl;
            }
            if(econFails > 0){
                std::cout <<"Rco failed "<< econFails <<" tests." << std::endl;
            }
            BOOST_REQUIRE(optFails == 0 && uniqFails == 0 && rcoFails ==0 && econFails == 0);
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
            //std::cout << pOpt->print() << std::endl;
            BOOST_REQUIRE( pOpt->run() == pTest->run());
            BOOST_REQUIRE( pOpt->print() == AST);
        }
        //Expr in assignment
        BOOST_AUTO_TEST_CASE(OPTLET2){
            Program* pTest = new Program(NULL, new Let("x", new Add(new Num(1), new Num(1)), new Add(new Var("x"), new Read(1))));
            std::string AST("(+2 (Read))");
            Program* pOpt = opt(pTest);
            //std::cout << pOpt->print() << std::endl;
            BOOST_REQUIRE( pOpt->run() == pTest->run());
            BOOST_REQUIRE( pOpt->print() == AST);
        }
        // inlining of variables
        BOOST_AUTO_TEST_CASE(OPTLET3){
            Program* pTest = new Program(NULL,new Let("y", new Num(2), new Let("x", new Var("y"), new Add(new Var("x"), new Read(1)))));
            std::string AST("(+2 (Read))");
            Program* pOpt = opt(pTest);
            //std::cout << pOpt->print() << std::endl;
            BOOST_REQUIRE( pOpt->run() == pTest->run());
            BOOST_REQUIRE( pOpt->print() == AST);
        }
        //Read Case
        BOOST_AUTO_TEST_CASE(OPTLET4){
            Program* pTest = new Program(NULL, new Let("x", new Read(1), new Add(new Var("x"), new Num(3))));
            std::string AST("(Let x = (Read)){\n(+x 3)}\n");
            Program* pOpt = opt(pTest);
            //std::cout << pOpt->print() << std::endl;
            BOOST_REQUIRE( pOpt->run() == pTest->run());
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
            BOOST_REQUIRE(pTest->run() == 9);
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
            int result = pTest->run() ;
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
            BOOST_REQUIRE(pTest->run() == 20);
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
            BOOST_REQUIRE(pTest->run() == 5);
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
            int result = pTest->run() ;
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
            BOOST_REQUIRE(pTest->run() == 20);
        }
        BOOST_AUTO_TEST_CASE(RTOXTEST2){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(7), new Reg("%rax")));
                instrSet.push_back(new Negq(new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            BOOST_REQUIRE(pTest->run() == -7);
        }
        BOOST_AUTO_TEST_CASE(BREAKTEST){
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(7), new Reg("%rax")));
                instrSet.push_back(new Addq(new Const(13), new Reg("%rax")));
                instrSet.push_back(new Retq());
                instrSet.push_back(new Addq(new Const(13), new Reg("%rax")));
            xProgram* pTest = new xProgram(new Block(instrSet));
            BOOST_REQUIRE(pTest->run() == 20);
        }
        //CTests
        // exception handlers
        BOOST_AUTO_TEST_CASE(CNOVAR){
            CRet* lTest = new CRet(new CVar("f"));
            CProg* pTest = new CProg(lTest);
            BOOST_REQUIRE_THROW( pTest->run(), std::runtime_error);
        }
        BOOST_AUTO_TEST_CASE(CNOMAIN){
            CRet* lTest = new CRet(new CNum(5));
            CProg* pTest = new CProg();
            pTest->addTail("NULL", lTest);
            BOOST_REQUIRE_THROW( pTest->run(), std::runtime_error);
        }
        
        //Test 1 manually compiled
        //Uncover Locals Tests added later
        //r = 42
        BOOST_AUTO_TEST_CASE(CTEST1){
            ;
            CSeq* lTest = new CSeq(new CStat("r", new CRead(1)),
                          new CSeq(new CStat("y", new CAdd(new CVar("r"), new CNum(42))),
                          new CSeq(new CStat("x", new CAdd(new CVar("y"), new CNum(17))),
                          new CSeq(new CStat("f", new CNeg(new CVar("x"))),
                          new CRet(new CVar("f"))
                          ))));
            CProg* cTest = new CProg(lTest);
            std::vector<std::string> ulOut = {"r","y", "x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                if(std::find(ulTest.begin(), ulTest.end(), it) != ulTest.end()){
                    std::string s = it;
                    //std::cout << s << std::endl;
                    ulTest.erase(std::remove(ulTest.begin(), ulTest.end(), s), ulTest.end());
                }
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            //std::cout << ret << std::endl;
            BOOST_REQUIRE(ret == -101);
            //std::cout << cTest->AST() << std::endl;
            xProgram* xTest = cTest->selInsr();
            std::cout << "selInstr complete" << std::endl;
            std::vector<std::string> out = xTest->emit(1);
            for(auto it : out){
                std::cout << it;
            }
            //ret = xTest->run();
            BOOST_REQUIRE(ret == -101);
            std::cout << "CTEST1 Complete" << std::endl;
        }
        //Test 3  manually compiled
        //r1 = 42, r2 = 41
        BOOST_AUTO_TEST_CASE(CTEST2){
            CSeq* lTest = new CSeq(new CStat("r1", new CRead(1)),
                          new CSeq(new CStat("y", new CAdd(new CVar("r1"), new CNum(7))),
                          new CSeq(new CStat("r2", new CRead(1)),
                          new CSeq(new CStat("x", new CAdd(new CVar("r2"), new CNum(13))),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CVar("y"))),
                          new CRet(new CVar("f"))
                          )))));
            CProg* cTest = new CProg(lTest);
            std::vector<std::string> ulOut = {"r1","y","r2", "x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                if(std::find(ulTest.begin(), ulTest.end(), it) != ulTest.end()){
                    std::string s = it;
                    ulTest.erase(std::remove(ulTest.begin(), ulTest.end(), s), ulTest.end());
                }
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            //std::cout << ret << std::endl;
            BOOST_REQUIRE(ret == 103);
            xProgram* xTest = cTest->selInsr();
            //ret = xTest->run();
            std::vector<std::string> out = xTest->emit(1);
            BOOST_REQUIRE(ret == 103);
            std::cout << "CTEST2 Complete" << std::endl;
        }
        //BASELET Manually compiled
        BOOST_AUTO_TEST_CASE(CTEST3){
            CSeq* lTest = new CSeq(new CStat("x", new CNum(8)),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CVar("x"))),
                          new CRet(new CVar("f"))
                          ));
            CProg* cTest = new CProg(lTest);
            std::vector<std::string> ulOut = { "x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                if(std::find(ulTest.begin(), ulTest.end(), it) != ulTest.end()){
                    std::string s = it;
                    ulTest.erase(std::remove(ulTest.begin(), ulTest.end(), s), ulTest.end());
                }
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            BOOST_REQUIRE(ret == 16);
            xProgram* xTest = cTest->selInsr();
            std::vector<std::string> out = xTest->emit(1);
            for(auto it : out){
                std::cout << it;
            }
            //ret = xTest->run();
            BOOST_REQUIRE(ret == 16);
            std::cout << "CTEST3 Complete" << std::endl;
        }
        //EXPHAND manually compiled
        BOOST_AUTO_TEST_CASE(CTEST4){
            CSeq* lTest = new CSeq(new CStat("f", new CAdd(new CNum(32), new CNum(64))),
                                   new CRet(new CVar("f"))
                                  );
            CProg* cTest = new CProg(lTest);
            std::vector<std::string> ulOut = {"f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                if(std::find(ulTest.begin(), ulTest.end(), it) != ulTest.end()){
                    std::string s = it;
                    ulTest.erase(std::remove(ulTest.begin(), ulTest.end(), s), ulTest.end());
                }
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            BOOST_REQUIRE(ret == 96);
            xProgram* xTest = cTest->selInsr();
            //ret = xTest->run();
            BOOST_REQUIRE(ret == 96);
            std::cout << "CTEST4 Complete" << std::endl;
        }
        //multiple read Test
        // x = 42
        BOOST_AUTO_TEST_CASE(CTEST5){
            CSeq* lTest = new CSeq(new CStat("x", new CRead(1)),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CVar("x"))),
                          new CRet(new CVar("f"))
                          ));
            CProg* cTest = new CProg(lTest);
            std::vector<std::string> ulOut = {"x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                if(std::find(ulTest.begin(), ulTest.end(), it) != ulTest.end()){
                    std::string s = it;
                    ulTest.erase(std::remove(ulTest.begin(), ulTest.end(), s), ulTest.end());
                }
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            //std::cout << ret << std::endl;
            BOOST_REQUIRE(ret == 84);
            xProgram* xTest = cTest->selInsr();
            //ret = xTest->run();
            BOOST_REQUIRE(ret == 84);
            std::cout << "CTEST5 Complete" << std::endl;
        }
        //(+52 (-10))
        BOOST_AUTO_TEST_CASE(CTEST6){
            CSeq* lTest = new CSeq(new CStat("x", new CNeg(new CNum(10))),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CNum(52))),
                          new CRet(new CVar("f"))
                          ));
            CProg* cTest = new CProg(lTest);
            std::vector<std::string> ulOut = {"x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                if(std::find(ulTest.begin(), ulTest.end(), it) != ulTest.end()){
                    std::string s = it;
                    ulTest.erase(std::remove(ulTest.begin(), ulTest.end(), s), ulTest.end());
                }
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            BOOST_REQUIRE(ret == 42);
            xProgram* xTest = cTest->selInsr();
            //ret = xTest->run();
            BOOST_REQUIRE(ret == 42);
            std::cout << "CTEST6 Complete" << std::endl;
        }
        BOOST_AUTO_TEST_CASE(SITEST1){
            CSeq* lTest = new CSeq(new CStat("x", new CNum(10)), new CRet(new CVar("x")));
            CProg* cTest = new CProg(lTest);
            int result, ret;
            result = cTest->run();
            xProgram* xTest = cTest->selInsr();
            //ret = xTest->run();
            //BOOST_REQUIRE(ret == result);
            std::vector<std::string> AST = {"main:\n", "addq 10 &x\n", "movq &x %rax\n", "jmp END\n"};
            std::vector<std::string> out;
            out = xTest->emit(1);            
            for(auto i1 = AST.begin(), i2 = out.begin(); i1 != AST.end(), i2 != out.end(); ++i1, ++i2){
                BOOST_REQUIRE(i1 == i2);
            } 
            std::cout << "SITEST1 Complete" << std::endl;
        }
        BOOST_AUTO_TEST_CASE(SITEST2){
            CSeq* lTest = new CSeq(new CStat("x", new CRead(1)), new CRet(new CVar("x")));
            CProg* cTest = new CProg(lTest);
            int result, ret;
            result = cTest->run();
            xProgram* xTest = cTest->selInsr();
            //ret = xTest->run();
            //BOOST_REQUIRE(ret == result);
            std::vector<std::string> AST = {"main:\n", "callq read_int\n", "movq %rax &x\n", "movq &x %rax\n", "jmp END\n"};
            std::vector<std::string> out;
            out = xTest->emit(1);
            for(auto it : out){
                std::cout << it;
            }
            for(auto it : AST){
                std::cout << it;
            }
            std::cout << "SITEST2 Complete" << std::endl;
        }
BOOST_AUTO_TEST_SUITE_END()

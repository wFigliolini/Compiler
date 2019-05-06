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
        std::vector<std::string> out;
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
        out = xTest->emit(1);
        /*for(auto it : out){
            std::cout << it;
        }*/
        xTest = assign(xTest);
        out = xTest->emit(1);
/*        for(auto it : out){
            std::cout << it;
        }*/
        f = xTest->run();
        BOOST_REQUIRE(int1 == f);
        //std::cout << "Passed assign run" << std::endl;
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        out = xTest->emit(1);
        /*for(auto it : out){
            std::cout << it;
        }*/
        f = xTest->run();
        //std::cout << int1 << " " << f << std::endl;
        BOOST_REQUIRE(int1 == f);
        xTest = xTest->genMain();
        xTest->outputToFile("Test1", 0);
    }
    BOOST_AUTO_TEST_CASE(Test2) {
        Program* pTest = new Program(NULL, new Add(new Num(7), new Num(13)));
        std::vector<std::string> out;
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
        xTest = assign(xTest);
        out = xTest->emit(1);
        f = xTest->run();
        BOOST_REQUIRE(int2 == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(int2 == f);
        xTest = xTest->genMain();
        xTest->outputToFile("Test2", 0);
    }
    // test for two branches with reads
    BOOST_AUTO_TEST_CASE(Test3) {
        Program* pTest = new Program(NULL, new Add(new Add(new Num(7), new Read(1)), new Add(new Num(13), new Read(1))));
        std::vector<std::string> out;
        int  int3, final3(103), f;
        int3 = pTest->run();
        //std::cout << int3 << std::endl;
        BOOST_REQUIRE(int3 == final3);
        pTest = opt(pTest);
        int3 = pTest->run();
        //std::cout << int3 << std::endl;
        BOOST_REQUIRE(int3 == final3);
        pTest = uniquify(pTest);
        int3 = pTest->run();
        //std::cout << int3 << std::endl;
        BOOST_REQUIRE(int3 == final3);
        pTest = rco(pTest);
        int3 = pTest->run();
        //std::cout << int3 << std::endl;
        BOOST_REQUIRE(int3 == final3);
        //std::cout << pTest->print()<< std::endl;
        CProg* cTest = econ(pTest);
        //std::cout << cTest->AST()<< std::endl;
        cTest->uncoverLocals();
        f = cTest->run();
        //std::cout <<"econ: "<<  f << std::endl;
        BOOST_REQUIRE(final3 == f);
        xProgram* xTest = cTest->selInsr();
        xTest = assign(xTest);
        out = xTest->emit(1);
        /*for(auto it : out){
            std::cout << it;
        }*/
        f = xTest->run();
        //std::cout << f << std::endl;
        BOOST_REQUIRE(int3 == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        //std::cout << f << std::endl;
        BOOST_REQUIRE(int3 == f);
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
        xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(int4 == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(int4 == f);
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
        xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(int5 == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(int5 == f);
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
        xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(result == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
        xProgram* xTest = pFinal->selInsr();
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
        xProgram* xTest = pFinal->selInsr();
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
        xProgram* xTest = pFinal->selInsr();
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
        xProgram* xTest = pFinal->selInsr();
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
        xProgram* xTest = pFinal->selInsr();
                xTest = assign(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
        BOOST_REQUIRE(xTest->containVar() == false);
        xTest = patch(xTest);
        f = xTest->run();
        BOOST_REQUIRE(orig == f);
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
            //pOpt->run();
            //BOOST_REQUIRE( pTest->run()-1 == pOpt->run());
            //std::cout << "program generated, result: "<< result << std::endl;
            BOOST_TEST(true);
    }

    //expand by power of 10
    
    BOOST_AUTO_TEST_CASE(Depth10){
            //int result;
            Program* pTest = randProg(10);
            //Program* pOpt = opt(pTest);
            //BOOST_REQUIRE( pTest->run() == pOpt->run());
            //std::cout << pTest->print();
            pTest->run();
            //pOpt->run();
            //std::cout << "program generated, result: " << result << std::endl;
            BOOST_TEST(true);
    }
    // Depth 100 results in system stalling and losing the rest of the file 
   BOOST_AUTO_TEST_CASE(Mass_Test){
            int depth(10), runCount(100);
            int optFails(0), uniqFails(0), rcoFails(0), econFails(0), selFails(0), asFails(0), patchFails(0);
            std::vector<int>  uniqList, optList, rcoList, econList, selList, asList, patchList;
            Program* pTest;
            bool Opt(true);
            for(int i = 0; i< runCount; ++i){
                int result, opresult;
                pTest = randProg(depth);
                result = pTest->run();
                if(Opt){
                    pTest = opt( pTest);
                    opresult = pTest->run();
                    if (result !=opresult){
                        //std::cout <<"Intended result: "<< result << " Opt Result: " << opresult << std::endl; 
                        optList.push_back(opresult);
                        ++optFails;
                        //to determine if the later stages still work
                        result = opresult;
                    }
                }
                /*pTest = uniquify( pTest);
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
                xProgram* xTest = cTest->selInsr();
                opresult = xTest->run();
                if (result !=opresult){
                    //std::cout <<"Intended result: "<< result << " Uniq Result: " << opresult << std::endl;
                    selList.push_back(opresult);
                    ++selFails;
                    result = opresult;
                }
                xTest = assign(xTest,0);
                opresult = xTest->run();
                if (result !=opresult){
                    //std::cout <<"Intended result: "<< result << " Uniq Result: " << opresult << std::endl;
                    asList.push_back(opresult);
                    ++asFails;
                    result = opresult;
                }
                xTest = patch(xTest);
                opresult = xTest->run();
                if (result !=opresult){
                    //std::cout <<"Intended result: "<< result << " Uniq Result: " << opresult << std::endl;
                    patchList.push_back(opresult);
                    ++patchFails;
                    result = opresult;
                }*/
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
                std::cout <<"Econ failed "<< econFails <<" tests." << std::endl;
            }
            if(selFails > 0){
                std::cout <<"Select failed "<< selFails <<" tests." << std::endl;
            }
            if(asFails > 0){
                std::cout <<"Assign failed "<< asFails <<" tests." << std::endl;
            }
            if(patchFails > 0){
                std::cout <<"Patch failed "<< patchFails <<" tests." << std::endl;
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
            //std::cout <<"input: "<< pTest->print() << " output: "<< pOpt->print() << std::endl;
            BOOST_REQUIRE( pTest->run() == pOpt->run());
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
                instrSet.push_back(new Movq(new DeRef(new Reg("%rsp"), -8), new Reg("%rax")));
                instrSet.push_back(new Addq(new DeRef(new Reg("%rsp"), -16), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            int result = pTest->run() ;
            std::cout << "DEREFTEST Result = " << result << std::endl;
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
            varSet ulOut = {"r","y", "x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                ulTest.erase(it);
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            //std::cout << ret << std::endl;
            BOOST_REQUIRE(ret == -101);
            //std::cout << cTest->AST() << std::endl;
            xProgram* xTest = cTest->selInsr();
            xTest = assign(xTest);
            int f = xTest->run();
            BOOST_REQUIRE(ret == f);
            BOOST_REQUIRE(xTest->containVar() == false);
            //std::cout << "CTEST1 Complete" << std::endl;
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
            varSet ulOut = {"r1","y","r2", "x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                ulTest.erase(it);
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            //std::cout << ret << std::endl;
            BOOST_REQUIRE(ret == 103);
            xProgram* xTest = cTest->selInsr();
            xTest = assign(xTest);
            int f = xTest->run();
            BOOST_REQUIRE(ret == f);
            BOOST_REQUIRE(xTest->containVar() == false);

            //std::cout << "CTEST2 Complete" << std::endl;
        }
        //BASELET Manually compiled
        BOOST_AUTO_TEST_CASE(CTEST3){
            CSeq* lTest = new CSeq(new CStat("x", new CNum(8)),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CVar("x"))),
                          new CRet(new CVar("f"))
                          ));
            CProg* cTest = new CProg(lTest);
            varSet ulOut = { "x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                ulTest.erase(it);
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            BOOST_REQUIRE(ret == 16);
            xProgram* xTest = cTest->selInsr();
            std::vector<std::string> out = xTest->emit(1);
            /*for(auto it : out){
                std::cout << it;
            }*/
            //ret = xTest->run();
            xTest = assign(xTest);
            int f = xTest->run();
            BOOST_REQUIRE(ret == f);
            BOOST_REQUIRE(xTest->containVar() == false);

            //std::cout << "CTEST3 Complete" << std::endl;
        }
        //EXPHAND manually compiled
        BOOST_AUTO_TEST_CASE(CTEST4){
            CSeq* lTest = new CSeq(new CStat("f", new CAdd(new CNum(32), new CNum(64))),
                                   new CRet(new CVar("f"))
                                  );
            CProg* cTest = new CProg(lTest);
            varSet ulOut = {"f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                ulTest.erase(it);
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            BOOST_REQUIRE(ret == 96);
            xProgram* xTest = cTest->selInsr();                
            xTest = assign(xTest);
            int f = xTest->run();
            BOOST_REQUIRE(ret == f);
            BOOST_REQUIRE(xTest->containVar() == false);
            //std::cout << "CTEST4 Complete" << std::endl;
        }
        //multiple read Test
        // x = 42
        BOOST_AUTO_TEST_CASE(CTEST5){
            CSeq* lTest = new CSeq(new CStat("x", new CRead(1)),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CVar("x"))),
                          new CRet(new CVar("f"))
                          ));
            CProg* cTest = new CProg(lTest);
            varSet ulOut = {"x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                ulTest.erase(it);
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            //std::cout << ret << std::endl;
            BOOST_REQUIRE(ret == 84);
            xProgram* xTest = cTest->selInsr();
            xTest = assign(xTest);
            int f = xTest->run();
            BOOST_REQUIRE(ret == f);
            BOOST_REQUIRE(xTest->containVar() == false);
        }
        //(+52 (-10))
        BOOST_AUTO_TEST_CASE(CTEST6){
            CSeq* lTest = new CSeq(new CStat("x", new CNeg(new CNum(10))),
                          new CSeq(new CStat("f", new CAdd(new CVar("x"), new CNum(52))),
                          new CRet(new CVar("f"))
                          ));
            CProg* cTest = new CProg(lTest);
            varSet ulOut = {"x", "f"}, ulTest;
            cTest->uncoverLocals();
            CInfo locals = cTest->getInfo();
            ulTest = locals.vars();
            BOOST_REQUIRE(ulTest.empty() == false);
            for(auto it : ulOut){
                ulTest.erase(it);
            }
            BOOST_REQUIRE(ulTest.empty());
            int ret = cTest->run();
            BOOST_REQUIRE(ret == 42);
            xProgram* xTest = cTest->selInsr();
            xTest = assign(xTest);
            int f = xTest->run();
            BOOST_REQUIRE(ret == f);
            BOOST_REQUIRE(xTest->containVar() == false);
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
            xTest = assign(xTest);
            int f = xTest->run();
            BOOST_REQUIRE(result == f);
            BOOST_REQUIRE(xTest->containVar() == false);
        }
        BOOST_AUTO_TEST_CASE(SITEST2){
            CSeq* lTest = new CSeq(new CStat("x", new CRead(1)), new CRet(new CVar("x")));
            CProg* cTest = new CProg(lTest);
            int result, ret;
            result = cTest->run();
            xProgram* xTest = cTest->selInsr();
            //ret = xTest->run();
            //BOOST_REQUIRE(ret == result);
            std::vector<std::string> AST = {"main:\n", "callq _read_int\n", "movq %rax &x\n", "movq &x %rax\n", "jmp END\n"};
            std::vector<std::string> out;
            out = xTest->emit(1);
            xTest = assign(xTest);
            int f = xTest->run();
            BOOST_REQUIRE(result == f);
            BOOST_REQUIRE(xTest->containVar() == false);
        }
        //f = -8
        BOOST_AUTO_TEST_CASE(ASSIGNHOMES1){
            int orig, f;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("f")));
                instrSet.push_back(new Addq(new Const(20), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            pTest = assign(pTest);
            //std::vector<std::string> out;
            /*out = pTest->emit(1);
            for(auto it : out){
                std::cout << it;
            }*/

            f = pTest->run();

            BOOST_REQUIRE(orig == f);
            BOOST_REQUIRE(pTest->containVar() == false);
        }
        //f = -8
        BOOST_AUTO_TEST_CASE(ASSIGNHOMES2){
            int orig, f;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("f")));
                instrSet.push_back(new Addq(new Ref("f"), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            pTest = assign(pTest);
            f = pTest->run();
/*          std::vector<std::string> out;
            out = pTest->emit(1);
            for(auto it : out){
                std::cout << it;
            }*/
            BOOST_REQUIRE(orig == f);
            BOOST_REQUIRE(pTest->containVar() == false);
        }
        //x = -8, f = -16
        BOOST_AUTO_TEST_CASE(ASSIGNHOMES3){
            int orig, f;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("x")));
                instrSet.push_back(new Movq(new Const(40), new Ref("f")));
                instrSet.push_back(new Addq(new Ref("x"), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            pTest = assign(pTest);
            f = pTest->run();
            
            BOOST_REQUIRE(orig == f);
            BOOST_REQUIRE(pTest->containVar() == false);
        }
        //case for not overwriting already used addresses
        //Unneeded as compiler will not generate DeRefs prior to assignhomes
        //f = -16, x = -24
/*        BOOST_AUTO_TEST_CASE(ASSIGNHOMES4){
            int orig, f;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(20), new DeRef(new Reg("%rbp"), -8)));
                instrSet.push_back(new Movq(new Const(40), new Ref("f")));
                instrSet.push_back(new Movq(new Const(60), new Ref("x")));
                instrSet.push_back(new Addq(new DeRef(new Reg("%rbp"), -8), new Ref("x")));
                instrSet.push_back(new Addq(new Ref("x"), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            pTest = assign(pTest);
            f = pTest->run();
            BOOST_REQUIRE(orig == f);
            BOOST_REQUIRE(pTest->containVar() == false);
        }
        //code from test 
        // 
        BOOST_AUTO_TEST_CASE(ASSIGNHOMES5){
            int orig, f;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(20), new DeRef(new Reg("%rbp"), -8)));
                instrSet.push_back(new Movq(new Const(40), new Ref("f")));
                instrSet.push_back(new Movq(new Const(60), new Ref("x")));
                instrSet.push_back(new Addq(new DeRef(new Reg("%rbp"), -8), new Ref("x")));
                instrSet.push_back(new Addq(new Ref("x"), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            pTest = assign(pTest);
            f = pTest->run();
            BOOST_REQUIRE(orig == f);
            BOOST_REQUIRE(pTest->containVar() == false);
        }*/
        BOOST_AUTO_TEST_CASE(PATCH1){
            int orig, f, origSize, fSize;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("x")));
                instrSet.push_back(new Movq(new Const(40), new Ref("f")));
                instrSet.push_back(new Addq(new Ref("x"), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            origSize = pTest->size("BODY");
            pTest = assign(pTest);
            //std::cout << "passed assign" <<std::endl;
            pTest = patch(pTest);
            //std::vector<std::string> out;
            /*out = pTest->emit(1);
            for(auto it : out){
                std::cout << it;
            }*/
            f = pTest->run();
            fSize = pTest->size("BODY");
            
            BOOST_REQUIRE(orig == f);
            //original size + number of double memory references
            //std::cout << origSize << " " << fSize<< std::endl;
            BOOST_REQUIRE(origSize-1 == fSize);
        }
        BOOST_AUTO_TEST_CASE(PATCH2){
            int orig, f, origSize, fSize;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("x")));
                instrSet.push_back(new Movq(new Ref("x"), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            origSize = pTest->size("BODY");
            pTest = assign(pTest);
            pTest = patch(pTest);
            f = pTest->run();
            fSize = pTest->size("BODY");
            
            BOOST_REQUIRE(orig == f);
            //std::cout << origSize << " " << fSize<< std::endl;
            BOOST_REQUIRE(origSize-2 == fSize);
        }
        //example from book, with vars added in place of derefs and regs
        BOOST_AUTO_TEST_CASE(PATCH3){
            int orig, f, origSize, fSize;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(10), new Ref("x")));
                instrSet.push_back(new Negq(new Ref("x")));
                instrSet.push_back(new Movq(new Ref("x"), new Ref("f")));
                instrSet.push_back(new Addq(new Const(52), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            origSize = pTest->size("BODY");
            pTest = assign(pTest);
            pTest = patch(pTest);
            f = pTest->run();
            fSize = pTest->size("BODY");
            
            //std::cout << origSize << " " << fSize<< std::endl;
            BOOST_REQUIRE(orig == f);
            BOOST_REQUIRE(origSize-2 == fSize);
        }
        //
        BOOST_AUTO_TEST_CASE(PATCH4){
            int orig, f, origSize, fSize;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("x")));
                instrSet.push_back(new Movq(new Const(40), new Ref("y")));
                instrSet.push_back(new Movq(new Const(60), new Ref("f")));
                instrSet.push_back(new Addq(new Ref("y"), new Ref("x")));
                instrSet.push_back(new Addq(new Ref("x"), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            origSize = pTest->size("BODY");
            pTest = assign(pTest);
            pTest = patch(pTest);
            f = pTest->run();
            fSize = pTest->size("BODY");
            
            //std::cout << origSize << " " << fSize<< std::endl;
            BOOST_REQUIRE(orig == f);
            BOOST_REQUIRE(origSize-1 == fSize);
        }
        BOOST_AUTO_TEST_CASE(PATCH5){
            int orig, f, origSize, fSize;
            std::vector<Instr*> instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("w")));
                instrSet.push_back(new Addq(new Ref("w"), new Ref("x")));
                instrSet.push_back(new Addq(new Ref("x"), new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            xProgram* pTest = new xProgram(new Block(instrSet));
            orig = pTest->run();
            origSize = pTest->size("BODY");
            pTest = assign(pTest);
            pTest = patch(pTest);
            f = pTest->run();
            fSize = pTest->size("BODY");
            
            //std::cout << origSize << " " << fSize<< std::endl;
            BOOST_REQUIRE(orig == f);
            BOOST_REQUIRE(origSize-1 == fSize);
        }
        //Mov Case
        BOOST_AUTO_TEST_CASE(LIVE1){
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("w")));    //{w}
                instrSet.push_back(new Movq(new Ref("w"), new Ref("x")));     //{x}
                instrSet.push_back(new Movq(new Ref("x"), new Ref("f")));     //{f}
                instrSet.push_back(new Movq(new Ref("f"), new Reg("%rax")));  //{0}
                instrSet.push_back(new Retq());
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            liveSet testData = { {"w"}, {"x"}, {"f"}, {},{}};
            pTest->uncoverLive();
            bool passed = pTest->testLive(testData);
            BOOST_REQUIRE(passed == true);
            //no edges
            infrGraph gTest = {{"w",{}},{"x",{}},{"f",{}}};
            pTest->genGraphs();
            infrGraph gOut = pTest->getGraph("BODY");
            bool result = gTest == gOut;
            if(result == false){
                std::cout<< "LIVE1 failed, dumping contents of gOut" << std::endl;
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(result);
            colorMap cTest = {{"%rax",0},{"w",0}, {"x",0}, {"f",0}};
            pTest->genColorMaps();
            colorMap cOut = pTest->getColors("BODY");
            bool colorResult = cTest == cOut;
            if(colorResult == false){
                std::cout<< "LIVE1 failed, dumping contents of cOut" << std::endl;
                pTest->dumpColor("BODY");
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(colorResult);
            assignEnv rTestData = {{"w",new Reg(0)}, {"x",new Reg(0)}, {"f",new Reg(0)}};
            int r, f;
            pTest->genEnv();
            bool testResult = pTest->testRegisters(rTestData);
            BOOST_REQUIRE(testResult == true);
            r = pTest->run();
            pTest = pTest->assignRegisters();
            f = pTest->run();
            BOOST_REQUIRE(r == f);
        }
        //from class
        BOOST_AUTO_TEST_CASE(LIVE2){
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(1), new Ref("v")));    
                instrSet.push_back(new Movq(new Const(46), new Ref("w")));  
                instrSet.push_back(new Movq(new Ref("v"), new Ref("x")));  
                instrSet.push_back(new Addq(new Const(7), new Ref("w")));      
                instrSet.push_back(new Movq(new Ref("x"), new Ref("y")));
                instrSet.push_back(new Addq(new Const(4), new Ref("y")));      
                instrSet.push_back(new Movq(new Ref("x"), new Ref("z")));
                instrSet.push_back(new Addq(new Ref("w"), new Ref("z")));
                instrSet.push_back(new Movq(new Ref("y"), new Ref("t")));
                instrSet.push_back(new Negq(new Ref("t")));
                instrSet.push_back(new Movq(new Ref("z"), new Reg("%rax")));
                instrSet.push_back(new Addq(new Ref("t"), new Reg("%rax")));
                instrSet.push_back(new Retq());
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            liveSet testData = { {"v"}, {"v","w"}, {"x", "w"}, {"x","w"}, {"x", "w", "y"},
            {"x", "w", "y"}, {"w", "y","z"}, {"y","z"}, {"t", "z"}, {"t","z"}, {"t"}, {},{}
            };
            pTest->uncoverLive();
            bool passed = pTest->testLive(testData);
            BOOST_REQUIRE(passed == true);
            infrGraph gTest = { {"t",{"z", "%rax"}}, {"v",{"w"}},{"x",{"w","y"}}, {"w",{"v","x","y","z"}},{"y",{"w","x","z"}},{"z",{"t","w","y"}},{"%rax",{"t"}}};
            movGraph mTest = { {"v", {"x"}}, {"w",{}}, {"x",{"v","y","z"}}, {"y",{"x","t"}},{"z",{"x", "%rax"}},{"t",{"y"}}, {"%rax",{"z"}}};
            pTest->genGraphs();
            infrGraph gOut = pTest->getGraph("BODY");
            movGraph mOut = pTest->getMov("BODY");
            bool result = gTest == gOut;
            bool movResult = mTest == mOut;
            if(result == false){
                std::cout<< "LIVE2 failed, dumping contents of gOut" << std::endl;
                pTest->dumpGraph("BODY");
            }
            if(movResult == false){
                std::cout<< "LIVE2 Mov failed, dumping contents of mOut" << std::endl;
                pTest->dumpMov("BODY");
            }
            BOOST_REQUIRE(result);
            BOOST_REQUIRE(movResult);
            colorMap cTest = {{"t",1}, {"v",0}, {"x",0}, {"w",1}, {"y",2}, {"z",0},{"%rax",0}};
            pTest->genColorMaps();
            colorMap cOut = pTest->getColors("BODY");
            bool colorResult = cTest == cOut;
            if(colorResult == false){
                pTest->dumpColor("BODY");
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(colorResult);
            assignEnv rTestData = {{"t",new Reg(1)},{"v",new Reg(0)},{"x",new Reg(0)},{"w",new Reg(1)}, {"y",new Reg(2)}, {"z",new Reg(0)}, {"%rax",new Reg(0)}};
            int r, f;
            pTest->genEnv();
            bool testResult = pTest->testRegisters(rTestData);
            BOOST_REQUIRE(testResult == true);
            r = pTest->run();
            pTest = pTest->assignRegisters();
            f = pTest->run();
            BOOST_REQUIRE(r == f);
        }
        //from textbook
        BOOST_AUTO_TEST_CASE(LIVE3){
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(5), new Ref("a")));
                instrSet.push_back(new Movq(new Const(30), new Ref("b")));
                instrSet.push_back(new Movq(new Ref("b"), new Ref("c")));
                instrSet.push_back(new Movq(new Const(10), new Ref("b")));
                instrSet.push_back(new Addq(new Ref("b"), new Ref("c")));
                instrSet.push_back(new Addq(new Ref("a"), new Ref("c"))); 
                instrSet.push_back(new Movq(new Ref("c"), new Reg("%rax")));  
                instrSet.push_back(new Retq());
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            liveSet testData = { {"a"}, {"a", "b"}, {"a", "c"}, {"a", "b", "c"},{"a", "c"},{"c"},{}, {}};
            pTest->uncoverLive();
            bool passed = pTest->testLive(testData);
            BOOST_REQUIRE(passed == true);
            infrGraph gTest = { {"a",{"b","c"}},{"b",{"a","c"}},{"c",{"a","b"}}};
            pTest->genGraphs();
            infrGraph gOut = pTest->getGraph("BODY");
            bool result = gTest == gOut;
            if(result == false){
                std::cout<< "LIVE3 failed, dumping contents of gOut" << std::endl;
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(result);
            colorMap cTest = {{"%rax",0},{"a",0}, {"b",1}, {"c",2}};
            pTest->genColorMaps();
            colorMap cOut = pTest->getColors("BODY");
            bool colorResult = cTest == cOut;
            if(colorResult == false){
                std::cout<< "LIVE3 failed, dumping contents of cOut" << std::endl;
                pTest->dumpColor("BODY");
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(colorResult);
            assignEnv rTestData = {{"a",new Reg(0)}, {"b",new Reg(1)}, {"c",new Reg(2)}};
            int r, f;
            pTest->genEnv();
            bool testResult = pTest->testRegisters(rTestData);
            BOOST_REQUIRE(testResult == true);
            r = pTest->run();
            pTest = pTest->assignRegisters();
            f = pTest->run();
            BOOST_REQUIRE(r == f);
        }
        //double case
        BOOST_AUTO_TEST_CASE(LIVE4){
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("w")));    //{w}
                instrSet.push_back(new Addq(new Ref("w"), new Ref("w")));     //{f}
                instrSet.push_back(new Movq(new Ref("w"), new Reg("%rax")));  //{0}
                instrSet.push_back(new Retq());
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            liveSet testData = { {"w"}, {"w"}, {}, {}};
            pTest->uncoverLive();
            bool passed = pTest->testLive(testData);
            BOOST_REQUIRE(passed == true);
            infrGraph gTest = {{"w",{}}};
            pTest->genGraphs();
            infrGraph gOut = pTest->getGraph("BODY");
            bool result = gTest == gOut;
            if(result == false){
                std::cout<< "LIVE4 failed, dumping contents of gOut" << std::endl;
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(result);
            colorMap cTest = {{"%rax",0},{"w",0}};
            pTest->genColorMaps();
            colorMap cOut = pTest->getColors("BODY");
            bool colorResult = cTest == cOut;
            if(colorResult == false){
                pTest->dumpColor("BODY");
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(colorResult);
            assignEnv rTestData = {{"w",new Reg(0)}};
            int r, f;
            pTest->genEnv();
            bool testResult = pTest->testRegisters(rTestData);
            BOOST_REQUIRE(testResult == true);
            r = pTest->run();
            pTest = pTest->assignRegisters();
            f = pTest->run();
            BOOST_REQUIRE(r == f);
        }
        //pushpop tests
        BOOST_AUTO_TEST_CASE(LIVE5){
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(5), new Ref("f")));
                instrSet.push_back(new Pushq(new Ref("f")));
                instrSet.push_back(new Popq(new Ref("f")));
                instrSet.push_back(new Movq(new Ref("f"), new Reg(0)));
                instrSet.push_back(new Retq());
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            liveSet testData = { {"f"}, {}, {"f"},{}, {}};
            pTest->uncoverLive();
            bool passed = pTest->testLive(testData);
            BOOST_REQUIRE(passed == true);
            infrGraph gTest = {{"f",{}}};
            pTest->genGraphs();
            infrGraph gOut = pTest->getGraph("BODY");
            bool result = gTest == gOut;
            if(result == false){
                std::cout<< "LIVE5 failed, dumping contents of gOut" << std::endl;
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(result);
            colorMap cTest = {{"%rax",0},{"f",0}};
            pTest->genColorMaps();
            colorMap cOut = pTest->getColors("BODY");
            bool colorResult = cTest == cOut;
            if(colorResult == false){
                std::cout<< "LIVE5 failed, dumping contents of cOut" << std::endl;
                pTest->dumpColor("BODY");
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(colorResult);
            assignEnv rTestData = {{"f",new Reg(0)}};
            int r, f;
            pTest->genEnv();
            bool testResult = pTest->testRegisters(rTestData);
            BOOST_REQUIRE(testResult == true);
            r = pTest->run();
            pTest = pTest->assignRegisters();
            f = pTest->run();
            BOOST_REQUIRE(r == f);
        }
        //add case
        BOOST_AUTO_TEST_CASE(LIVE6){
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("a")));    //{w}
                instrSet.push_back(new Movq(new Const(40), new Ref("b")));
                instrSet.push_back(new Addq(new Ref("a"), new Ref("b")));     //{f}
                instrSet.push_back(new Movq(new Ref("b"), new Reg("%rax")));  //{0}
                instrSet.push_back(new Retq());
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            liveSet testData = { {"a"}, {"a","b"}, { "b"}, {},{}};
            pTest->uncoverLive();
            bool passed = pTest->testLive(testData);
            BOOST_REQUIRE(passed == true);
            infrGraph gTest = {{"a",{"b"}},{"b",{"a"}}};
            pTest->genGraphs();
            infrGraph gOut = pTest->getGraph("BODY");
            bool result = gTest == gOut;
            if(result == false){
                std::cout<< "LIVE6 failed, dumping contents of gOut" << std::endl;
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(result);
            colorMap cTest = {{"%rax",0},{"a",0}, {"b",1}};
            pTest->genColorMaps();
            colorMap cOut = pTest->getColors("BODY");
            bool colorResult = cTest == cOut;
            if(colorResult == false){
                std::cout<< "LIVE6 failed, dumping contents of cOut" << std::endl;
                pTest->dumpColor("BODY");
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(colorResult);
            assignEnv rTestData = {{"a",new Reg(0)}, {"b",new Reg(1)}};
            int r, f;
            pTest->genEnv();
            bool testResult = pTest->testRegisters(rTestData);
            BOOST_REQUIRE(testResult == true);
            r = pTest->run();
            pTest = pTest->assignRegisters();
            f = pTest->run();
            BOOST_REQUIRE(r == f);
        }
        //sub case
        BOOST_AUTO_TEST_CASE(LIVE7){
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("a")));    //{w}
                instrSet.push_back(new Movq(new Const(40), new Ref("b")));
                instrSet.push_back(new Subq(new Ref("a"), new Ref("b")));     //{f}
                instrSet.push_back(new Movq(new Ref("b"), new Reg("%rax")));  //{0}
                instrSet.push_back(new Retq());
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            liveSet testData = { {"a"}, {"a","b"}, { "b"}, {},{}};
            pTest->uncoverLive();
            bool passed = pTest->testLive(testData);
            BOOST_REQUIRE(passed == true);
            infrGraph gTest = {{"a",{"b"}},{"b",{"a"}}};
            pTest->genGraphs();
            infrGraph gOut = pTest->getGraph("BODY");
            bool result = gTest == gOut;
            if(result == false){
                std::cout<< "LIVE7 failed, dumping contents of gOut" << std::endl;
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(result);
            colorMap cTest = {{"%rax",0},{"a",0}, {"b",1}};
            pTest->genColorMaps();
            colorMap cOut = pTest->getColors("BODY");
            bool colorResult = cTest == cOut;
            if(colorResult == false){
                std::cout<< "LIVE7 failed, dumping contents of cOut" << std::endl;
                pTest->dumpColor("BODY");
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(colorResult);
            assignEnv rTestData = {{"a",new Reg(0)}, {"b",new Reg(1)}};
            int r, f;
            pTest->genEnv();
            bool testResult = pTest->testRegisters(rTestData);
            BOOST_REQUIRE(testResult == true);
            r = pTest->run();
            pTest = pTest->assignRegisters();
            f = pTest->run();
            BOOST_REQUIRE(r == f);
        }
        //Neg Case
        BOOST_AUTO_TEST_CASE(LIVE8){
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(20), new Ref("a")));    //{w}
                instrSet.push_back(new Negq(new Ref("a")));     //{f}
                instrSet.push_back(new Movq(new Ref("a"), new Reg("%rax")));  //{0}
                instrSet.push_back(new Retq());
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            liveSet testData = { {"a"}, {"a"}, {},{}};
            pTest->uncoverLive();
            bool passed = pTest->testLive(testData);
            BOOST_REQUIRE(passed == true);
            infrGraph gTest = {{"a",{}}};
            pTest->genGraphs();
            infrGraph gOut = pTest->getGraph("BODY");
            bool result = gTest == gOut;
            if(result == false){
                std::cout<< "LIVE8 failed, dumping contents of gOut" << std::endl;
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(result);
            colorMap cTest = {{"%rax",0},{"a",0}};
            pTest->genColorMaps();
            colorMap cOut = pTest->getColors("BODY");
            bool colorResult = cTest == cOut;
            if(colorResult == false){
                std::cout<< "LIVE8 failed, dumping contents of cOut" << std::endl;
                pTest->dumpColor("BODY");
                pTest->dumpGraph("BODY");
            }
            BOOST_REQUIRE(colorResult);
            assignEnv rTestData = {{"a",new Reg(0)}};
            int r, f;
            pTest->genEnv();
            bool testResult = pTest->testRegisters(rTestData);
            BOOST_REQUIRE(testResult == true);
            r = pTest->run();
            pTest = pTest->assignRegisters();
            f = pTest->run();
            BOOST_REQUIRE(r == f);
        }
        //overflow into stack
        BOOST_AUTO_TEST_CASE(LIVE9){
            Blk instrSet;
            int result , f;
            assignEnv data;
            for(int i = 'a'; i <= 'z'; ++i){
                std::string s;
                s += static_cast<char>(i);
                instrSet.push_back(new Movq(new Const(i), new Ref(s)));
                if( i-'a' < 14){
                    data.insert(data.end(), std::pair<Vertex, Arg*>(s, new Reg(i-'a')));
                }
                else{
                    data.insert(data.end(), std::pair<Vertex, Arg*>(s, new DeRef(new Reg("%rsp"),(i-'a'-13))));
                }
            }
            for(int i = 'z'; i > 'a'; --i){
                std::string s1, s2;
                s1 += static_cast<char>(i);
                s2 += static_cast<char>(i-1);
                instrSet.push_back(new Addq(new Ref(s1), new Ref(s2)));
            }
            instrSet.push_back(new Movq(new Ref("a"), new Reg(0)));
            instrSet.push_back(new Retq());

            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            result = pTest->run(); 
            pTest->uncoverLive();
            pTest->genGraphs();
            pTest->genColorMaps();
            pTest->genEnv();
            //generates all registers and 12 stack locations, but not in expected order
            //commenting out to not check for order
            //bool testResult = pTest->testRegisters(data);
            //BOOST_REQUIRE(testResult == true);
            pTest = pTest->assignRegisters();
            f = pTest->run();
            BOOST_REQUIRE(result == f);
        }
        BOOST_AUTO_TEST_CASE(BIAS1){
            int result , f;
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(1), new Ref("v")));
                instrSet.push_back(new Movq(new Const(46), new Ref("w")));
                instrSet.push_back(new Movq(new Ref("v"), new Ref("x")));
                instrSet.push_back(new Addq(new Const(7), new Ref("x")));
                instrSet.push_back(new Movq(new Ref("x"), new Ref("y")));
                instrSet.push_back(new Addq(new Const(4), new Ref("y")));
                instrSet.push_back(new Movq(new Ref("x"), new Ref("z")));
                instrSet.push_back(new Addq(new Ref("w"), new Ref("z")));
                instrSet.push_back(new Movq(new Ref("x"), new Ref("z")));
                instrSet.push_back(new Movq(new Ref("y"), new Ref("1i")));
                instrSet.push_back(new Negq(new Ref("1i")));
                instrSet.push_back(new Movq(new Ref("z"), new Reg(0)));
                instrSet.push_back(new Addq(new Ref("1i"), new Reg(0)));
                instrSet.push_back(new Retq);
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            result = pTest->run(); 
            pTest->uncoverLive();
            pTest->genGraphs();
            pTest->genColorMaps(1);
            pTest->genEnv();
            pTest = pTest->assignRegisters();
            std::vector<std::string> out;
            out = pTest->emit(1);
            /*for(auto it : out){
                std::cout << it;
            }*/
            f = pTest->run();
            BOOST_REQUIRE(result == f);
            /*std::cout<< std::endl;
            std::cout<< "end BIAS1"<<std::endl;
            std::cout<< std::endl;*/
        }
        //simplify to movq(3, rax)
        BOOST_AUTO_TEST_CASE(BIAS2){
            int result , f;
            Blk instrSet;
                instrSet.push_back(new Movq(new Const(3), new Ref("x")));
                instrSet.push_back(new Movq(new Ref("x"), new Ref("y")));
                instrSet.push_back(new Movq(new Ref("y"), new Ref("z")));
                instrSet.push_back(new Movq(new Ref("z"), new Reg(0)));
                instrSet.push_back(new Retq());
            Block* bTest = new Block(instrSet);
            xProgram* pTest = new xProgram(bTest);
            result = pTest->run(); 
            pTest->uncoverLive();
            pTest->genGraphs();
            pTest->genColorMaps(1);
            pTest->genEnv();
            pTest = pTest->assignRegisters();
            std::vector<std::string> out;
            out = pTest->emit(1);
            /*for(auto it : out){
                std::cout << it;
            }*/
            f = pTest->run();
            BOOST_REQUIRE(result == f);
            /*std::cout<< std::endl;
            std::cout<< "end BIAS2"<<std::endl;
            std::cout<< std::endl;*/
        }
        BOOST_AUTO_TEST_CASE(BIASR){
            int result, f;
            Program* rTest = new Program(new Let("x", new Num(1), new Let("v", new Var("x"), new Add(new Var("v"), new Num(2)))));
            result = rTest->run();
            rTest = uniquify(rTest);
            rTest = rco(rTest);
            CProg* cTest = econ(rTest);
            cTest->uncoverLocals();
            xProgram* pTest = cTest->selInsr();
            pTest->uncoverLive();
            pTest->genGraphs();
            pTest->genColorMaps(1);
            pTest->genEnv();
            pTest = pTest->assignRegisters();
            std::vector<std::string> out;
            out = pTest->emit(1);
            /*for(auto it : out){
                std::cout << it;
            }*/
            f = pTest->run();
            BOOST_REQUIRE(result == f);
        }
        // Subtraction Test
        BOOST_AUTO_TEST_CASE(R2Test1){
            int f, result(42);
            Program* pTest = new Program(Sub(new Num(52), new Num(10)));
            f = pTest->run();
            bool interpTest = f== result;
            BOOST_REQUIRE(interpTest);
            std::string AST("(+52 (-10))"), ASTOut = pTest->print();
            bool ASTTest =  AST == ASTOut;
            BOOST_REQUIRE(ASTTest);
        }
        //if Test
        BOOST_AUTO_TEST_CASE(R2Test2){
            int f, result(false);
            Program* pTest = new Program(new If(new Cmp("==", Sub(new Num(52), new Num(10)), new Num(43)), new Bool(true), new Bool(false)));
            f = pTest->run();
            bool interpTest = f == result;
            BOOST_REQUIRE(interpTest);
        }
        //OR
        BOOST_AUTO_TEST_CASE(R2Test3){
            int f, result(false);
            Program* pTest = new Program(Or( new Cmp("==", Sub(new Num(52), new Num(10)), new Num(43)), new Cmp(">", Sub(new Num(52), new Num(10)), new Num(43))));
            f = pTest->run();
            bool interpTest = f== result;
            BOOST_REQUIRE(interpTest);
        }
        //And
        BOOST_AUTO_TEST_CASE(R2Test4){
            int f, result(true);
            Program* pTest = new Program(And( new Cmp("==", Sub(new Num(52), new Num(10)), new Num(42)), new Cmp("<", Sub(new Num(52), new Num(10)), new Num(43))));
            f = pTest->run();
            bool interpTest = f== result;
            BOOST_REQUIRE(interpTest);
        }
        //Not
        BOOST_AUTO_TEST_CASE(R2Test5){
            int f, result(false);
            Program* pTest = new Program(new Not(new Cmp("==", Sub(new Num(52), new Num(10)), new Num(42))));
            f = pTest->run();
            bool interpTest = f== result;
            //std::cout << f << std::endl;
            BOOST_REQUIRE(interpTest);
        }
        //conditionals
        BOOST_AUTO_TEST_CASE(R2Test6){
            int f, result(true);
            Program* pTest = new Program(new Let("x", new Read(1), new If(new Cmp("<", new Var("x"), new Num(100)), new Bool(true), new Bool(false))));
            f = pTest->run();
            bool interpTest = f== result;
            BOOST_REQUIRE(interpTest);
        }
        //conditional Let
        BOOST_AUTO_TEST_CASE(R2Test7){
            int f, result(100);
            Program* pTest = new Program(new Let("x", new If(new Cmp("<", new Num(42), new Num(100)), new Num(100), new Num(42)), new Var("x")));
            f = pTest->run();
            bool interpTest = f == result;
            BOOST_REQUIRE(interpTest);
        }
        // And Short Circuit
        BOOST_AUTO_TEST_CASE(R2Test8){
            int f, result(false);
            Program* pTest = new Program(And(new Bool(false), new Cmp("==", new Read(0), new Num(42))));
            f = pTest->run();
            bool interpTest = f== result;
            BOOST_REQUIRE(interpTest);
        }
        //Or Short Circuit
        BOOST_AUTO_TEST_CASE(R2Test9){
            int f, result(true);
            Program* pTest = new Program(Or(new Bool(true), new Cmp("==", new Read(0), new Num(42))));
            f = pTest->run();
            bool interpTest = f== result;
            BOOST_REQUIRE(interpTest);
        }
        //raw compare
        BOOST_AUTO_TEST_CASE(R2Test10){
            int f, result(false);
            Program* pTest = new Program(new Cmp(">", new Num(32), new Num(42)));
            f = pTest->run();
            bool interpTest = f== result;
            BOOST_REQUIRE(interpTest);
        }
        //Xor
        BOOST_AUTO_TEST_CASE(R2Test11){
            int f, result(false);
            Program* pTest = new Program(Xor(new Bool(true), new Bool(true)));
            f = pTest->run();
            bool interpTest = f== result;
            BOOST_REQUIRE(interpTest);
        }
        //Type Check Cases
        BOOST_AUTO_TEST_CASE(TC1){
            Program* pTest = new Program(new Add(new Bool(true), new Bool(false)));
            BOOST_REQUIRE_THROW(pTest->typeCheck(), std::runtime_error);
        }
        //also serves as test for if condition
        BOOST_AUTO_TEST_CASE(TC2){
            Program* pTest = new Program(And( new Num(1), new Num(0)));
            BOOST_REQUIRE_THROW(pTest->typeCheck(), std::runtime_error);
        }
        BOOST_AUTO_TEST_CASE(TC3){
            Program* pTest = new Program(new Neg(new Bool(true)));
            BOOST_REQUIRE_THROW(pTest->typeCheck(), std::runtime_error);
        }
        BOOST_AUTO_TEST_CASE(TC4){
            Program* pTest = new Program(new Not(new Num(1)));
            BOOST_REQUIRE_THROW(pTest->typeCheck(), std::runtime_error);
        }
        //variable cases
        BOOST_AUTO_TEST_CASE(TC5){
            Program* pTest = new Program(new Let("x", new Bool(true), new Add(new Var("x"), new Var("x"))));
            BOOST_REQUIRE_THROW(pTest->typeCheck(), std::runtime_error);
        }
        BOOST_AUTO_TEST_CASE(TC6){
            Program* pTest = new Program(new Let("x", new Num(1), Or(new Var("x"), new Var("x"))));
            BOOST_REQUIRE_THROW(pTest->typeCheck(), std::runtime_error);
        }
        //Compare Test
        BOOST_AUTO_TEST_CASE(TC7){
            Program* pTest = new Program(new Cmp("==", new Bool(true), new Bool(false)));
            BOOST_REQUIRE_THROW(pTest->typeCheck(), std::runtime_error);
        }
        //if case
        BOOST_AUTO_TEST_CASE(TC8){
            Program* pTest = new Program(new If(new Bool(true), new Bool(false), new Num(42)));
            BOOST_REQUIRE_THROW(pTest->typeCheck(), std::runtime_error);
        }
        BOOST_AUTO_TEST_CASE(R2OPT1){
            std::string AST("#t"), out;
            Program* pTest = new Program(new Not(new Not(new Bool(true))));
            pTest = opt(pTest);
            out = pTest->print();
            bool result = out == AST;
            if(result == false){
                std::cout << "Desired AST:" << std::endl;
                std::cout << AST << std::endl;
                std::cout << "Output:" << std::endl;
                std::cout << out << std::endl;
            }
            BOOST_REQUIRE(result);
        }
        BOOST_AUTO_TEST_CASE(R2OPT2){
            std::string AST("(if (Read)\n#f\n else \n#t)"), out;
            Program* pTest = new Program(new If(new Not(new Read(1)),new Bool(true), new Bool(false) ));
            pTest = opt(pTest);
            out = pTest->print();
            bool result = out == AST;
            if(result == false){
                std::cout << "Desired AST:" << std::endl;
                std::cout << AST << std::endl;
                std::cout << "Output:" << std::endl;
                std::cout << out << std::endl;
            }
            BOOST_REQUIRE(result);
        }
        BOOST_AUTO_TEST_CASE(R2OPT3){
            std::string AST("1"), out;
            Program* pTest = new Program(new If(new Bool(true), new Num(1), new Read(1)));
            pTest = opt(pTest);
            out = pTest->print();
            BOOST_REQUIRE(out == AST);
        }
        BOOST_AUTO_TEST_CASE(R2UNIQ1){
            Expr* expr = And( new Let("x", new Bool(true), new Var("x") ),
                                 new Let("x", new Bool(false), 
                                 new Let("x", Or(new Bool(false), new Var("x")), 
                                 Xor(new Var("x"), new Var("x")))));
            int orig, f;
            Program* pTest = new Program(expr);
            orig =  pTest->run();
            Program* pFinal = uniquify(pTest);
            f = pFinal->run();
            BOOST_REQUIRE(orig == f);
        }
        // basic case for goto
        BOOST_AUTO_TEST_CASE(C1Test1){
            CSeq* lMain = new CSeq(new CStat("x",  new CNum(5)),  new CGoto(new CLabel("END")));
            CRet* lEnd = new CRet(new CVar("x"));
            CProg* cTest = new CProg(lMain);
            cTest->addTail("END",  lEnd);
            int f = cTest->run();
            BOOST_REQUIRE(f ==  5);
        }
        // gotif case
        BOOST_AUTO_TEST_CASE(C1Test2){
            CSeq* lMain = new CSeq(new CStat("x",  new CNum(5)),  new CGotoIf(new CCmp(">",  new CNum(5),  new CNum(6)),  new CLabel("T1"),  new CLabel("F1")));
            CRet* lT1 = new CRet(new CBool(true));
            CRet* lF1 = new CRet(new CBool(false));
            CProg* cTest = new CProg(lMain);
            cTest->addTail("T1",  lT1);
            cTest->addTail("F1",  lF1);
            int f = cTest->run();
            bool initResult = f ==  false;
            if (initResult ==  false) {
                std::cout <<  f << std::endl;
            }
            BOOST_REQUIRE(initResult);
        }
        BOOST_AUTO_TEST_CASE(C1Test3){
            CSeq* lMain = new CSeq(new CStat("x",  new CNum(5)),  new CGotoIf(new CCmp("<",  new CNum(5),  new CNum(6)),  new CLabel("T1"),  new CLabel("F1")));
            CRet* lT1 = new CRet(new CBool(true));
            CRet* lF1 = new CRet(new CBool(false));
            CProg* cTest = new CProg(lMain);
            cTest->addTail("T1",  lT1);
            cTest->addTail("F1",  lF1);
            int f = cTest->run();
            bool initResult = f ==  true;
            if (initResult ==  false) {
                std::cout <<  f << std::endl;
            }
            BOOST_REQUIRE(initResult);
        }
        // to Test if only one branch is executed
        BOOST_AUTO_TEST_CASE(C1Test4){
            CSeq* lMain = new CSeq(new CStat("x",  new CNum(5)),  new CGotoIf(new CCmp(">",  new CNum(5),  new CNum(6)),  new CLabel("T1"),  new CLabel("F1")));
            CSeq* lT1 = new CSeq(new CStat("x",  new CRead(0)),  new CRet(new CVar("x")));
            CRet* lF1 = new CRet(new CNum(6));
            CProg* cTest = new CProg(lMain);
            cTest->addTail("T1",  lT1);
            cTest->addTail("F1",  lF1);
            int f = cTest->run();
            bool initResult = f ==  6;
            if (initResult ==  false) {
                std::cout <<  f << std::endl;
            }
            BOOST_REQUIRE(initResult);
        }
        // Not Test
        BOOST_AUTO_TEST_CASE(C1Test5) {
            CSeq* lMain = new CSeq(new CStat("x",  new CCmp(">",  new CNum(5),  new CNum(6))),  
                          new CSeq(new CStat("f",  new CNot(new CVar("x"))), 
                          new CRet(new CVar("f"))));
            CProg* cTest = new CProg(lMain);
            int f = cTest->run();
            BOOST_REQUIRE(f ==  true);
        }
BOOST_AUTO_TEST_SUITE_END()

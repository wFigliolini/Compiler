//RCode.cpp
//Contains function definition of Tests

#include "RCode.h"

Expr* powE(int x, int b){
	if(x == 1){
		return new Num(b);
	}
	else{
		return new Add(powE(x-1 , b) , powE(x-1 , b) );
	}
}
Program* pow(int x, int b ){
	Expr* val = powE(x , b);
	Program* ret = new Program(NULL, val);
	return ret;
}

 //Global generators for rand Expr
 //choiceGen to determine path
 //numGen to determine num values
 
 std::mt19937 choiceGen, numGen;
 
 //associated distributions
 
 std::uniform_int_distribution<int> choiceDis(0,1), numDis(0,1024);


Expr* randExpr(int depth){
	int choice =  choiceDis(choiceGen);
	//std::cout << "choice: " << choice << std::endl;
	if(depth <= 0){
		if( choice == 1 ){
			return new Read(1);
		}
		else{
            int i(numDis(numGen));
			Num* num = new Num(i);
			return num;
		}
	}
	else{
		int nextDepth= depth-1 ;
		if( choice == 1 ){
			return new Neg( randExpr(nextDepth) );
		}
		else{
			return new Add( randExpr(nextDepth), randExpr(nextDepth) );
		}
	}
}

Program* randProg(int depth){
	Expr* val = randExpr(depth);
	Program* ret = new Program(NULL, val);
	//std::cout << "returning prog" << std::endl;
	return ret;
}

Expr* optE( Expr* orig, Environ env){
    if(orig->e1_){
        orig->e1_.reset(optE(orig->e1_.release(), env));
        if(orig->e2_){
            orig->e2_.reset(optE(orig->e2_.release(), env));
        }
    }
    if(orig->isPure(env)){
        Num* retN = orig->inter(env);
        return retN;
    }
    return orig;
}

Program* opt( Program* orig){
    Expr* e = orig->getExpr();
    Environ env;
    e = optE(e,  env);
    return new Program(orig->getInfo(), e);
}

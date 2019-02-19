//RCode.cpp
//Contains function definition of Tests

#include <random>
#include "RCode.cpp"

Expr* powE(int x, int b){
	if(x == 1){
		return new Num(b);
	}
	else{
		return new Add(powE(x-1 , b) , powE(x-1 , b) );
	}
}
Program* pow(int x, int b = 2){
	Expr* val = powE(x , b);
	Program* ret = new Program(NULL, val);
	return ret;
}

Expr* randExpr(int depth){

}

Program* randProg(int depth){
	Expr* val = randExpr(depth);
	Program* ret = new Program(NULL, val);
	return ret;
}

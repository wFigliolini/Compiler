//RCode.cpp
//Contains function definition of Tests

#include "RCode.h"
#include <list>

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
 
 std::mt19937 choiceGen, numGen, stringGen;
 
 //associated distributions
 
 std::uniform_int_distribution<int> choiceDis(0,2), numDis(0,1024), stringDis(0,61);

typedef std::vector<std::string> vars;
std::string randString(int len);
 
Expr* randExpr(int depth, vars env){
    int choice =  choiceDis(choiceGen);
    //std::cout << "choice: " << choice << std::endl;
    if(depth <= 0){
        if( choice == 0 ){
            return new Read(1);
        }
        else if(choice == 1){
            int i(numDis(numGen));
            Num* num = new Num(i);
            return num;
        }else{
            int i(numDis(numGen));
            //generate var
            if(env.empty()) {
                //fallback if no lets is to act as a number
                //will result in uneven distributions prior to first let, after should have 33% chance of any 
                return new Num(0);
            }
            else{
                //use i to randomly select from set of vars
                std::string out = env[i%env.size()];
                return new Var(out);
            }
            
        }
    }
    else{
        int nextDepth= depth-1 ;
        if( choice == 0 ){
            return new Neg( randExpr(nextDepth, env) );
        }
        else if(choice == 1){
            return new Add( randExpr(nextDepth, env), randExpr(nextDepth, env) );
        }
        else{
            //generate let
            std::string name = randString(10);
            vars env2(env);
            env2.push_back(name);
            return new Let(name, randExpr(nextDepth, env), randExpr(nextDepth,env2)); 
        }
    }
}

std::string randString(int len){
    int randNum;
    std::string result;
    static const char alphaNum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for(int i = 0; i < len; ++i){
        randNum = stringDis(stringGen);
        result += alphaNum[randNum];
    }
    return result;
}

Program* randProg(int depth){
    vars env;
    Expr* val = randExpr(depth, env);
    Program* ret = new Program(NULL, val);
    //std::cout << "returning prog" << std::endl;
    return ret;
}

//depricated optimizer, saved for refrence
/*Expr* optE( Expr* orig, Environ env){
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
}*/

Program* opt( Program* orig){
    Expr *e = orig->getExpr();
    Environ env;
    e = e->optE(env);
    return new Program(orig->getInfo(), e);
}
Program* uniquify(Program* orig){
    Expr* e = orig->getExpr();
    envmap* env = new envmap;
    e = e->uniquify(env);
    return new Program(e);
}
Program* rco(Program* orig){
    Expr* e = orig->getExpr();
    Expr* e2, *temp;
    bool debug = 0;
    std::vector<rcoPair> eMap;
    envmap env;
    eMap = e->rco(env);
    for(auto it = eMap.rbegin(); it != eMap.rend(); ++it){
        if(debug) std::cout << it->first << " = " << it->second->AST() << std::endl;
        if(it == eMap.rbegin()){
            temp = new Var(it->first);
        }else{
            temp = e2;
            e2 = NULL;
        }
        if(it->first.empty() == false){
            e2 = new Let(it->first, it->second, temp);
        }else{
            e2 = it->second;
        }
    }
    if(debug) std::cout << "End RCO Pass" << std::endl;
    genNewVar("i", 1);
    return new Program(e2);
}
CProg* econ(Program* orig){
    CProg * out = new CProg();
    CTail * t = orig->e_->econTail();
    out->addTail("main", t);
    return out;
}
std::string genNewVar(std::string type, bool reset){
    
    static int count = 0;
    std::string out = std::to_string(count++);
    out += type;
    if(reset){
        count = 0;
    }
    return out;
}

xProgram* assign(xProgram* orig){
    return orig->assignHomes();
}

xProgram* patch(xProgram* orig){
    return orig->patchP();
}
void blkInfo::setIndex(unsigned int i, Instr* instr){
    varSet val, prev;
    //return empty set if last index
    if(i < (l_.size()-1)){
        prev = getIndex(i+1);
        val = instr->ul(prev);
    }
    l_[i] = val;
}
//set of register names
int Label::num_ = 42;
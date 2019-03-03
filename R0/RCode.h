//RCode.h
//Header file containing function prototypes and definitions of Expressions
#ifndef R0_DEF_H_
#define R0_DEF_H_

#include <iostream>
#include <string>
#include <random>
#include <memory>
#include <unordered_map>



//virtual Expression class
enum opCode { num, add, neg, rRead, ERROR = -1};
class Expr;
class Num;
typedef std::unordered_map<std::string, Expr*> Environ;

class Expr {
public:
	//standard constructors
	explicit Expr(){}
	explicit Expr(Expr* n):e1_(n){}
	explicit Expr(Expr* n1, Expr* n2):e1_(n1), e2_(n2){}
	//clone function to handle copies
 	auto clone() const { return std::unique_ptr<Expr>(cloneImpl()); };
	virtual Num* inter(Environ env) = 0;
	virtual std::string AST() = 0;
    virtual Expr* optE(Environ env) = 0;
    virtual bool isPure(Environ env) = 0;
    virtual bool containVar(std::string name) = 0;
protected:
	virtual Expr* cloneImpl() const = 0;
	std::unique_ptr<Expr> e1_, e2_;
	opCode op_;
private:

};



//int class
class Num : public Expr {
public:
	explicit Num(int n) {
	       	i_ = n;
            op_= num;
    };
    explicit Num(Num* n) {
	       	i_ = n->i_;
            op_= n->op_;
    };
    friend Num* numAdd(Num* const l, Num* const r){
        Num* out = new Num(0);
        out->i_ = l->i_ + r->i_;
        return out;
    }
    friend Num* numNeg(Num* const l){
        Num* i = new Num(-l->i_);
        return i;
    }
    int output(){
        return i_;
    }
	Num* inter(Environ env) { return this; };
	std::string AST()  {
		std::string str = std::to_string(i_);
		return str;
	}
	bool isPure(Environ env) { return true;}
	bool containVar(std::string name){return false;}
	Expr* optE(Environ env) { return this;}
protected:
	Num* cloneImpl() const override {
		return new Num(i_);
	}
private:
    void seti(int i){i_=i;};
	int i_;
};


//addition class
//+ n1 n2 -> int
class Add : public Expr {
public:
	explicit Add(Expr* n1, Expr* n2):Expr(n1, n2) {
		op_ = add;
	}
	Num* inter(Environ env) {
		Num *i, *j;
		i = e1_->inter(env);
		j = e2_->inter(env);
        i = numAdd(i,j);
		return i;
	}
	std::string AST()  {
		std::string str("(+");
		str += e1_->AST();
		str += " ";
		str += e2_->AST();
		str += ")";
		return str;
	}
	bool isPure(Environ env) { return e1_->isPure(env) && e2_->isPure(env); }
	Expr* optE(Environ env){
        Expr *e1 =e1_.release(), *e2 = e2_.release();
        e1_.reset(e1->optE(env));
        e2_.reset(e2->optE(env));
        if(isPure(env)){
            return new Num(inter(env));
        }
        else{
            return this;
        }
    }
    bool containVar(std::string name){ return e1_->containVar(name) || e2_->containVar(name);}
protected:
	Add* cloneImpl() const override {
	       	return new Add((e1_->clone().release()), (e2_->clone().release()));
	};
private:
};

//negation class
class Neg : public Expr {
public:
	explicit Neg(Expr* n): Expr(n) {
		op_ = neg;
	}
	Num* inter(Environ env) {
		Num* i;
		i = e1_->inter(env);
        i = numNeg(i);
		return i;
	}
	std::string AST() {
		std::string str("(-");
		str += e1_->AST();
		str += ")";
		return str;
	}
	bool isPure(Environ env) {
        return e1_->isPure(env);
    }
    bool containVar(std::string name){ return e1_->containVar(name);}
	Expr* optE(Environ env){
        Expr* e = e1_.release();
        e1_.reset(e->optE(env));
        if(isPure(env)){
            return new Num(inter(env));
        }
        else{
            return this;
        }
    };
protected:
	Neg* cloneImpl() const override {
	       	return new Neg((e1_->clone().release()));
	};
private:
};

// Read class
namespace{
class Read : public Expr {
public:
	explicit Read(bool mode = 0):mode_(mode) {op_ = rRead;};
	Num* inter(Environ env) {
		int i;
		if (mode_) {
			i = num_;
			--num_;
		}
		else {
			std::cin >> i;
		}
		return new Num(i);
	}
	std::string AST() {
		std::string str("(Read)");
	//	std::cout << str << std::endl;
		return str;
	}
	bool isPure(Environ env) { return false; }
	Expr* optE(Environ env) { return this; }
	bool containVar(std::string name){return false;}
protected:
	Read* cloneImpl() const override { return new Read(mode_);};
private:
	bool mode_;
	static int num_;
};
int Read::num_ = 42;
}

class Let: public Expr{
public:
    Let(std::string var, Expr* exp, Expr* body): Expr(exp, body), var_(var){}
    Num* inter(Environ env){
        env[var_] = e1_->clone().release();
        Num* out;
        out = e2_->inter(env);
        return out;
    }
	std::string AST()  {
		std::string str("(Let ");
        str += var_;
        str += " = ";
		str += e1_->AST();
		str += "){\n";
		str += e2_->AST();
        str += "}\n";
		return str;
	}
	bool isPure(Environ env){
        env[var_] = e1_->clone().release();
        //e1 does not need to be evaluated here, as it may not be referenced
        //its evaluation will occur at first var instance
        return e2_->isPure(env);
    }
    bool containVar(std::string name){ return e1_->containVar(name) || e2_->containVar(name);}
    Expr* optE(Environ env){
        Expr* e1 = e1_->clone().release();
        Expr* e2 = e2_->clone().release();
        e1 = e1->optE(env);
        e1_.reset(e1);
        env[var_] = e1;
        e2 = e2->optE(env);
        e2_.reset(e2);
        if(isPure(env)){
            return new Num(inter(env));
        }
        if(containVar(var_)){
            return this;
        }
        else{
            return e2;
        }
    }
protected:
    Let* cloneImpl() const override {
        return new Let(var_,(e1_->clone().release()), (e2_->clone().release()));
	};
private:
    std::string var_;
};

class Var:public Expr{
public:
    Var(std::string name):name_(name){};
    Num* inter(Environ env){
        Expr* container = env[name_];
        Num* result = container->inter(env);
        return result;
    }
    std::string AST() {
        return name_;
    }
    bool isPure(Environ env){
        return env[name_]->isPure(env);
    }
    bool containVar(std::string name){return name == name_;}
    Expr* optE(Environ env) {
        Expr* temp= env[name_];
        if( temp->isPure(env)){
            return temp;
        }
        return this;
    }
protected:
    Var* cloneImpl() const override {
		return new Var(name_);
	}
private:
    std::string name_;
};

//temp Info class
class Info {
public:
	explicit Info(){};
};

//program
class Program {
public:
	//constructors and = operators
	explicit Program():i_(NULL) {   };
	explicit Program(Info* i, Expr* e):e_(e), i_(i) { };
	~Program() = default;
	explicit Program(const Program& orig):e_(orig.e_->clone()), i_(orig.i_){}
	Program( Program&& orig) = default;
	Program& operator= (const Program& orig){
		e_ = orig.e_->clone();
		return *this;
	}
	Program& operator= (Program&& orig) = default;
	int run() {
        Num* out;
        Environ env;
		out = e_->inter(env);
        return out->output();
	}
	std::string print() {
		std::string out;
	        out = e_->AST();	
		return out;
	}
	inline Expr* getExpr() {
        if(e_){
		Expr* e = (e_->clone()).release();
        return e;
        }
        else{
            return NULL;
        }
    }
	inline Info* getInfo() { return i_; };
	inline void setExpr(Expr* n) { e_.reset(n); };
	inline void setInfo(Info* n) { i_ = n; };
    friend Program* opt( Program* orig);
private:
	std::unique_ptr<Expr> e_;
	Info* i_;
};

//function declarations

Program* pow(int x, int b = 2);
Program* randProg(int depth);
#endif


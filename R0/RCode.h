//RCode.h
//Header file containing function prototypes and definitions of Expressions
#ifndef R0_DEF_H_
#define R0_DEF_H_

#include <iostream>
#include <string>
#include <random>
#include <memory>

//virtual Expression class
enum opCode { num, add, neg, rRead, ERROR = -1};
class Expr {
public:
	//standard constructors
	explicit Expr(){}
	explicit Expr(Expr* n):e1_(n){}
	explicit Expr(Expr* n1, Expr* n2):e1_(n1), e2_(n2){}
	//clone function to handle copies
 	auto clone() const { return std::unique_ptr<Expr>(cloneImpl()); };
	virtual int inter() = 0;
	virtual std::string AST() = 0;
    friend Expr* optE( Expr* orig);
    virtual bool isPure() = 0;
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
	int inter() { return i_; };
	std::string AST()  {
		std::string str = std::to_string(i_);
		return str;
	}
	bool isPure() { return true; }
protected:
	Num* cloneImpl() const override {
		return new Num(i_);
	}
private:
	int i_;
};


//addition class
//+ n1 n2 -> int
class Add : public Expr {
public:
	explicit Add(Expr* n1, Expr* n2):Expr(n1, n2) {
		op_ = add;
	}
	int inter() {
		int i, j;
		i = e1_->inter();
		j = e2_->inter();
		return i + j;
	}
	std::string AST()  {
		std::string str("(+");
		str += e1_->AST();
		str += " ";
		str += e2_->AST();
		str += ")";
		return str;
	}
	bool isPure() { return e1_->isPure() && e2_->isPure(); }
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
	int inter() {
		int i;
		i = e1_->inter();
		return -i;
	}
	std::string AST() {
		std::string str("(-");
		str += e1_->AST();
		str += ")";
		return str;
	}
	bool isPure() { return e1_->isPure();}
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
	int inter() {
		int i;
		if (mode_) {
			i = num_;
			--num_;
		}
		else {
			std::cin >> i;
		}
		return i;
	}
	std::string AST() {
		std::string str("(Read)");
	//	std::cout << str << std::endl;
		return str;
	}
	bool isPure() { return false; }
protected:
	Read* cloneImpl() const override { return new Read(mode_);};
private:
	bool mode_;
	static int num_;
};
int Read::num_ = 42;
}

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
		return e_->inter();
	}
	std::string print() {
		std::string out;
	        out = e_->AST();	
		return out;
	}
	inline Expr* getExpr() {
		Expr* e = (e_->clone()).release();
        return e;
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


//RCode.h
//Header file containing function prototypes and definitions of Expressions
#ifndef R0_DEF_H_
#define R0_DEF_H_

#include <iostream>
#include <string>
#include <random>
enum opCode {num, add, neg, rRead, ERROR = -1};
//virtual Expression class
class Expr {
public:
	explicit Expr(){}
	explicit Expr(Expr* n):e1_(n){}
	explicit Expr(Expr* n1, Expr* n2):e1_(n1), e2_(n2){}
	virtual int inter() = 0;
	virtual std::string AST() = 0;
protected:
	std::unique_ptr<Expr> e1_, e2_;
private:

};

//int class
class Num : public Expr {
public:
	explicit Num(int n) { i_ = n; };
	int inter() { return i_; };
	std::string AST()  {
		std::string str = std::to_string(i_);
		return str;
	}
	static const opCode op_;
private:
	int i_;
};

//addition class
//+ n1 n2 -> int
class Add : public Expr {
public:
	explicit Add(Expr* n1, Expr* n2):Expr(n1, n2) {
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
private:
};

//negation class
class Neg : public Expr {
public:
	explicit Neg(Expr* n): Expr(n) {
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
private:
};

// Read class
namespace{
class Read : public Expr {
public:
	explicit Read() { mode_ = 0; };
	Read(bool mode) { mode_ = mode; }
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
	static const opCode op_;
	inline int getMode(){ return mode_;}
private:
	bool mode_;
	static int num_;
};

int Read::num_ = 42;
}
//temp Info class
class Info {};

//program
class Program {
public:
	explicit Program() {   };
	explicit Program(Info* i, Expr* e):e_(e), i_(i) { };
	int run() {
		return e_->inter();
	}
	std::string print() {
		std::string out;
	        out = e_->AST();	
		return out;
	}
	inline std::unique_ptr<Expr> getExpr() {
		if(e_ == NULL) return NULL;
		std::unique_ptr<Expr> e( new Expr(*e_));
	       	return e;
       	}
	inline Info* getInfo() { return i_; };
	inline void setExpr(Expr* n) { e_.reset(n); };
	inline void setInfo(Info* n) { i_ = n; };
private:
	std::unique_ptr<Expr> e_;
	Info* i_;
};

//function declarations

Program* pow(int x, int b = 2);
Program* randProg(int depth);
#endif

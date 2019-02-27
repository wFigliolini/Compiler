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
	virtual int inter() = 0;
	virtual std::string AST() = 0;
protected:
	std::unique_ptr<Expr> e1_,e2_;
	opCode op_;
private:

};

//int class
class Num : public Expr {
public:
	explicit Num(int n) { i = n; };
	int inter() { return i; };
	std::string AST()  {
		std::string str = std::to_string(i);
		return str;
	}
	static const  opCode op_;
private:
	int i;
};
const opCode Num::op_ = num;

//addition class
//+ n1 n2 -> int
class Add : public Expr {
public:
	explicit Add(Expr n1, Expr n2): e1_(n1), e2_(n2) {
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
	static const opCode op_;
private:
};
const opCode Add::op_ = add;
//negation class
class Neg : public Expr {
public:
	explicit Neg(Expr* n): e1_(n) {
	}
	int inter() {
		int i;
		i = e->inter();
		return -i;
	}
	std::string AST() {
		std::string str("(-");
		str += e->AST();
		str += ")";
		return str;
	}
	static const opCode op_;
private:
};
const opCode Neg::op_ = neg;

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
	inline int getMode(){ return mode_;}
private:
	bool mode_;
	static int num_;
	static const opCode op_;
};
const opCode Read::op_ = rRead;

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
	explicit Program() {   };
	explicit Program(Info* in, Expr* n):i_(i), e_(n) { };
	int run() {
		return e_->inter();
	}
	std::string print() {
		std::string out;
	        out = e_->AST();	
		return out;
	}
	inline std::unique_ptr<Expr> getExpr() { return e_; };
	inline Info* getInfo() { return i; };
	inline void setExpr(Expr* n) { e.reset(n); };
	inline void setInfo(Info* n) { i = n; };
private:
	std::unique_ptr<Expr> e_;
	Info* i_;
};

//function declarations

Program* pow(int x, int b = 2);
Program* randProg(int depth);
#endif

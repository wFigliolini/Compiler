//RCode.h
//Header file containing function prototypes and definitions of Expressions
#ifndef R0_DEF_H_
#define R0_DEF_H_

#include <iostream>
#include <string>

//virtual Expression class
class Expr {
public:
	virtual int inter() = 0;
	virtual std::string AST() = 0;
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
private:
	int i;
};

//addition class
//+ n1 n2 -> int
class Add : public Expr {
public:
	explicit Add(Expr* n1, Expr* n2) {
		e1 = n1;
		e2 = n2;
	}
	int inter() {
		int i, j;
		i = e1->inter();
		j = e2->inter();
		return i + j;
	}
	std::string AST()  {
		std::string str("(+");
		str += e1->AST();
		str += " ";
		str += e2->AST();
		str += ")";
		return str;
	}
private:
	Expr* e1, *e2;
};

//negation class
class Neg : public Expr {
public:
	explicit Neg(Expr* n) {
		e = n;
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
private:
	Expr* e;
};

// Read class
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
private:
	bool mode_;
	static int num_;
};

int Read::num_ = 42;
//temp Info class
class Info {};

//program
class Program {
public:
	explicit Program() { i = NULL; e = NULL; };
	explicit Program(Info* in, Expr* n) { i = in; e = n; };
	int run() {
		return e->inter();
	}
	std::string print() {
		std::string out;
	        out = e->AST();	
		return out;
	}
	inline Expr* getExpr() { return e; };
	inline Info* getInfo() { return i; };
	inline void setExpr(Expr* n) { e = n; };
	inline void setInfo(Info* n) { i = n; };
private:
	Expr* e;
	Info* i;
};

Program* pow(int x, int b = 2); 
#endif

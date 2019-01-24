//R0.h
#ifndef R0_DEF_H_
#define R0_DEF_H_

#include <iostream>
#include <string>


//virtual Expression class
class Expr {
public:
	virtual int inter() = 0;
	virtual std::string AST() = 0;
	friend std::ostream& operator<<(std::ostream& os, Expr* e) {
		os << e->AST();
		return os;
	}
};

//int class
class Num : public Expr {
public:
	Num(int n) { i = n; };
	inline int inter() { return i; };
	inline std::string AST() {
		return std::to_string(i);
	}
private:
	int i;
};

//addition class
//+ n1 n2 -> int
class Add : public Expr {
public:
	Add(Expr* n1, Expr* n2) {
		e1 = n1;
		e2 = n2;
	}
	inline int inter() {
		int i, j;
		i = e1->inter();
		j = e2->inter();
		return i + j;
	}
	inline std::string AST() {
		std::string s1, s2, sFinal("(+ ");
		s1 = e1->AST();
		s2 = e2->AST();
		sFinal += s1;
		sFinal += ' ';
		sFinal += s2;
		sFinal += ')';
		return sFinal;
	}
private:
	Expr* e1, *e2;
};

//negation class
class Neg : public Expr {
public:
	Neg(Expr* n) {
		e = n;
	}
	inline int inter() {
		int i;
		i = e->inter();
		return -i;
	}
	inline std::string AST() {
		std::string s1, sFinal("(- ");
		s1 = e->AST();
		sFinal += s1;
		sFinal += ')';
		return sFinal;
	}
private:
	Expr* e;
};

// Read class
class Read : public Expr {
public:
	Read() {}
	int inter() {
		int i;
		std::cin >> i;
		return i;
	}
	inline std::string AST() {
		return "(Read)";
	}
};

//temp Info class
class Info {};

//program
class Program {
public:
	Program(Expr* n) { e = n; };
private:
	Expr* e;
};
#endif

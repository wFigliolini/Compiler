//R0.h
#ifndef R0_DEF_H_
#define R0_DEF_H_

#include <iostream>


//virtual Expression class
class Expr{
	virtual int inter() = 0;
};

//int class
class Num : public Expr {
	Num( int n ) { i = n };
	inline int inter() { return i };
private:
	int i;
};

//addition class
//+ n1 n2 -> int
class Add : public Expr {
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
private:
	Expr* e1, e2;
};

//negation class
class Neg : public Expr {
	Neg(Expr* n) {
		e = n;
	}
	inline int inter() {
		int i;
		i = e->inter();
		return -i;
	}
private:
	Expr e;
};

// Read class
class Read : public Expr {
	Read() {}
	int inter() {
		int i;
		std::cin >> i;
		return i;
	}
};

//temp Info class
class Info{};

//program
class Program{
	Program(Expr* n) { e = n };
private:
	Expr* e;
};

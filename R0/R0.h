//R0.h
#ifndef R0_DEF_H_
#define R0_DEF_H_

#include <iostream>
#include <string>
#include <sstream>

//virtual Expression class
class Expr {
public:
	virtual int inter() = 0;
	virtual void AST(std::ostream& os) const = 0;
	friend std::ostream& operator<<(std::ostream& os, Expr* e) {
		e->AST(os);
		return os;
	}
};

//int class
class Num : public Expr {
public:
	explicit Num(int n) { i = n; };
	int inter() { return i; };
	void AST(std::ostream& os)  const {
		os << e->i;
		return os;
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
	void AST(std::ostream& os)  const{
		os << "(+";
		e1->AST(os);
		os << " ";
		e2->AST(os);
		os << ")";
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
	void AST(std::ostream& os)  const {
		os << "(-";
		e->AST(os);
		os << ")";
	}
private:
	Expr* e;
};

// Read class
class Read : public Expr {
public:
	explicit Read() { num = -1 };
	Read(int in) { num = in; }
	int inter() {
		int i;
		if (num != -1) {
			i = num;
		}
		else {
			std::cin >> i;
		}
		return i;
	}
	void AST(std::ostream& os)  const {
		os << "(Read)";
	}
private:
	int num
};

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
		std::ostringstream os;
		os << e;
		std::string out(os.str());
		return out;
	}
	friend std::ostream& operator<<(std::ostream& os, Program* p) {
		os << p.e;
		os << std::endl;
		return os;
	}
	inline Expr* getExpr() { return e };
	inline Info* getInfo() { return i };
	inline void setExpr(Expr* n) { e = n; };
	inline void setInfo(Info* n) { i = n; };
private:
	Expr* e;
	Info* i;
};
#endif

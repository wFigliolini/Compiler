//RCode.h
//Header file containing function prototypes and definitions of Expressions
#ifndef R0_DEF_H_
#define R0_DEF_H_

#include <iostream>
#include <string>
#include <random>
#include <memory>
#include <unordered_map>
#include <vector>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

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
//functions
Program* pow(int x, int b = 2);
Program* randProg(int depth);
namespace{
//X definitions
typedef std::unordered_map<std::string, int> varList;

//Info class
//Handles the Stack and Registers
class xInfo{
public:
    xInfo(): pc_(0),result_(0), done_(false){
        regs_.reserve(16);
    };
    void setLabel(std::string s){
        currLabel_ = s;
    }
    void setReg(int reg, int val){
        if( reg < 0 || reg > 15) throw std::invalid_argument("Register must be in range 0->15");
        regs_[reg] = val;
    }
    void setStack(int ad, int val){
        if( ad < 0 ) throw std::invalid_argument("Stack reference must be above 0");
        stack_[ad/8] = val;
    }
    void setVar(std::string s, int val){
        v_[s] = val;
    }
    int getReg(int reg){
        if( reg < 0 || reg > 15) throw std::invalid_argument("Register must be in range 0->15");
        return regs_[reg];
    }
    int getStack(int ad){
        if( ad < 0 ) throw std::invalid_argument("Stack reference must be above 0");
        return stack_[ad/8];
    }
    std::string getLabel(){
        return currLabel_;
    }
    int getVar(std::string name){
        return v_[name];
    }
    void resetPC(){
        pc_=0;
    }
    int getPC(){
        return pc_;
    }
    void updatePC(){
        ++pc_;
    }
    void setResult(){result_ = regs_[0]; done_= true;};
    int getResult(){return result_;}
    bool isDone(){return done_;}
    void pushStack(int i){
        //std::cout << "attempting to push " << i << " to stack location " << regs_[7] << std::endl;
        stack_.push_back(i);
        regs_[7]+=8;
    }
    int popStack(){
        int result;
        regs_[7]-=8;
        result = stack_[(regs_[7]/8)];
        stack_.pop_back();
        return result;
    }
private:
    std::vector<int> regs_;
    std::vector<int> stack_;
    std::string currLabel_;
    varList v_;
    int pc_;
    int result_;
    bool done_;
};

class X{
public:
    void setInfo(std::shared_ptr<xInfo> i){i_ = i;}
protected:
    static std::shared_ptr<xInfo> i_;

};

class Label: public X{
public:
    explicit Label (std::string label): label_(label){};
    std::string emitL(bool vars){
        return label_;
    }
    void interp(){
        std::vector<std::string> sys_ = {"read", "print"};
        auto it = std::find(std::begin(sys_), std::end(sys_), label_);
        int index, i;
        index = std::distance(sys_.begin(), it);
        switch(index){
                //read
            case 0:
                std::cin >> i;
                i_->setReg(0,i);
                break;
                //write
            case 1:
                std::cout << i_->getReg(0) << std::endl;
                break;
                //else
            default:
                i_->setLabel(label_);
        }
    }
protected:
   std::string label_;
};


class Arg: public X{
public:
    virtual const std::string emitA(bool vars) = 0;
    virtual int get() = 0;
    virtual void set(int i) = 0;
};
class Reg: public Arg{
public:
    explicit Reg(int reg){
        if( reg > 15 || reg < 0 ){
            throw std::invalid_argument("Register assignment attempted outside of range 0->15\n");
        }
        reg_ = reg;
        return;
    }
    explicit Reg(std::string reg){
        auto it = std::find(regNames.begin(), regNames.end(), reg);
        if( it == regNames.end() ){
            std::string errorstring("Register assignment attempted to invalid register ");
            errorstring+=reg;
            errorstring+="\n";
            throw std::invalid_argument(errorstring);
        }
        reg_ = std::distance(regNames.begin(), it);
        return;
    }
    const std::string emitA(bool vars){
        return regNames[reg_];
    }
    int get(){
        return i_->getReg(reg_);
    }
    void set(int i){
        i_->setReg(reg_,i);
    }
private:
    int reg_;
    static std::vector<std::string> regNames;
};

class Const: public Arg{
public:
    explicit Const( int con ):con_(con){};
    const std::string emitA(bool vars){
        return std::to_string(con_);
    }
    int get(){
        return con_;
    }
    void set(int i){
        throw std::logic_error("Can't Write to Constant");
    }
private:
    int con_;
};

class DeRef: public Arg{
public:
    explicit DeRef(Reg* reg):reg_(reg), offset_(0){};
    explicit DeRef(Reg* reg, int off):reg_(reg){
        offset_ = off;
    }
    const std::string emitA(bool vars){
        std::string output;
        if( offset_ != 0 ) output += std::to_string(offset_);
        output += "(";
        output += reg_->emitA(vars);
        output += ")";
        return output;
    }
    int get(){
        int addr;
        addr = reg_->get() + offset_;
        //std::cout << "getting stack offset " << addr <<std::endl;
        return i_->getStack(addr);
    }
    void set(int i){
        int addr;
        addr = reg_->get() + offset_;
        //std::cout << "setting stack offset " << addr << " to " << i <<std::endl;
        i_->setStack(addr, i);
        //std::cout << "Stack set" << std::endl;
    }
private:
    Reg* reg_;
    int offset_;
};

class Ref: public Arg{
public:
    explicit Ref(std::string n):name_(n){};
    const std::string emitA(bool vars){
        std::string output;
        output += "&";
        if(vars){
            output += name_;
        }else{
            output += i_->getVar(name_);
        }
        return output;
    }
    int get(){
        return i_->getVar(name_);
    }
    void set(int i){
        i_->setVar(name_,i);
    }
private:
    std::string name_;
};
class BlkComplete : public std::exception{
public:
    const char * what () const throw () {
      return "Used to break current Iteration on Label Change";
   }
};
//instructions
class Instr: public X{
public:
   explicit Instr(){al_ = NULL; ar_ = NULL;}
   explicit Instr(Arg* a1): al_(a1){ar_ = NULL;}
   explicit Instr(Arg* a1, Arg* a2): al_(a1), ar_(a2){}
   virtual std::string emitI(bool vars) = 0;
   virtual void interp() = 0;
protected:
    Arg* al_, *ar_;
    bool isControl;
};
class Addq: public Instr{
public:
    Addq(Arg* a1, Arg* a2): Instr(a1, a2){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "addq ";
        output += al_->emitA(vars);
        output += " ";
        output += ar_->emitA(vars);
        output += "\n";
        return output;
    }
    void interp(){
        int out;
        out =  al_->get() + ar_->get();
        ar_->set(out);
    }
};
class Subq: public Instr{
public:
    Subq(Arg* a1, Arg* a2): Instr(a1, a2){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "subq ";
        output += al_->emitA(vars);
        output += " ";
        output += ar_->emitA(vars);
        output += "\n";
        return output;
    }
    void interp(){
        int out;
        out =  al_->get() - ar_->get();
        ar_->set(out);
    }
};
class Movq: public Instr{
public:
    Movq(Arg* a1, Arg* a2): Instr(a1, a2){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "movq ";
        output += al_->emitA(vars);
        output += " ";
        output += ar_->emitA(vars);
        output += "\n";
        return output;
    }
    void interp(){
        ar_->set(al_->get());
    }
}; 
class Retq: public Instr{
public:
    Retq(): Instr(){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "retq";
        output += "\n";
        return output;
    }
    void interp(){
        //int rbp = 6, retaddr = i_->getStack(i_->getReg(rbp)-1);
        
        i_->setResult();
        /*if(retaddr == 0){
        }
        else{
            //i_->setReg(rbp, retaddr);
        }
         */
    }
};
class Negq: public Instr{
public:
    Negq(Arg* a1): Instr(a1){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "negq ";
        output += al_->emitA(vars);
        output += "\n";
        return output;
    }
    void interp(){
        al_->set(-al_->get());
    }
};
class Callq: public Instr{
public:
    Callq(Label* lab): lab_(lab){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "callq ";
        output += lab_->emitL(vars);
        output += "\n";
        return output;
    }
    void interp(){
        i_->pushStack(i_->getReg(6));
        i_->setReg(6, i_->getReg(7));
        lab_->interp();
        throw BlkComplete();
    }
private:
    Label* lab_;
};
class Jmp: public Instr{
public:
    Jmp(Label* lab): lab_(lab){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "jmp ";
        output += lab_->emitL(vars);
        output += "\n";
        return output;
    }
    void interp(){
        lab_->interp();
        throw BlkComplete();
    }
private:
    Label* lab_;
};
class Pushq: public Instr{
public:
    Pushq(Arg* a1): Instr(a1){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "pushq ";
        output += al_->emitA(vars);
        output += "\n";
        return output;
    }
    void interp(){
        //std::cout << "calling pushstack with val " << al_->get()<< std::endl;
        i_->pushStack(al_->get());
    }
};
class Popq: public Instr{
public:
    Popq(Arg* a1): Instr(a1){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "popq ";
        output += al_->emitA(vars);
        output += "\n";
        return output;
    }
    void interp(){
        al_->set(i_->popStack());
    }
};
//block definition
typedef std::vector<Instr*> blk;
class Block: public X{
public:
    Block(blk b ): blk_(b){}
    std::vector<std::string> emitB(bool vars){
        std::vector<std::string> output;
        for( auto it : blk_){
            output.push_back(it->emitI(vars));
        }
        return output;
    }
    void setInfo(std::shared_ptr<xInfo> i){
        i_ = i;
    }
    void interp(){
        for(auto it : blk_){
            try{
                it->interp();
            }
            catch(BlkComplete& e){
                return;
            }
        }
    }
private:
    blk blk_;
};
typedef std::unordered_map<std::string, Block*> blkList; 
//Program definition
class xProgram: public X{
public:
    xProgram(Block* blk){
        i_ = std::make_shared<xInfo>();
        blks_["main"] = blk;
        init();
    }
    void declareVar(std::string n, int v){
        i_->setVar(n,v);
    }
    std::vector<std::string> emit(bool vars){
        std::vector<std::string> output;
        output.push_back(".globl main");
        for(auto [ name, block] : blks_){
            std::vector<std::string> blockOut;
            std::string outName(name);
            outName+=":\n";
            output.push_back(outName);
            blockOut = block->emitB(vars);
            output.insert(output.end(), blockOut.begin(), blockOut.end());
        }
        return output;
    }
    void addBlock(std::string name, Block* blk){
        blks_[name] = blk;
    }
    int interp(){
        while(!(i_->isDone())){
            blks_[i_->getLabel()]->interp();
        }
        return i_->getResult();
    }
    void outputToFile(std::string n, bool vars){
        std::vector<std::string> output;
        std::ofstream ofs;
        n+=".asm";
        ofs.open(n);
        
        output = emit(vars);
        
        for(auto it: output){
            ofs << it;
        }
    }
    //TODO: find how to get return value out of as
    int runAsm(){
        outputToFile("o", false);
        int* err;
        pid_t pid = fork();

        if(pid == 0){
            //child
            //run as
            *err = execl("/usr/bin/as", "-- o.asm" ,NULL);
        }
        else if(pid > 0){
            //parent
            
        }
        else{
            //fork failed
            throw std::runtime_error("runAsm Fork Failed");
        }
        if(err == 0){
            std::cout << "execution successful" << std::endl;
        }
        else{
            std::cout << "as terminated with error code " << err << std::endl;
        }
    }
private:
    void init(){
        i_->setLabel("main");
        return;
    }
    blkList blks_;
};
std::shared_ptr<xInfo> X::i_ = NULL;
//set of register names
std::vector<std::string> Reg::regNames = {
    "%rax", "%rbx", "%rcx",
    "%rdx", "%rsi", "%rdi",
    "%rbp", "%rsp", "%r8",
    "%r9", "%r10", "%r11",
    "%r12", "%r13", "%r14",
    "%r15"};
}
#endif

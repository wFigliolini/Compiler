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
class CExp;
typedef std::unordered_map<std::string, Expr*> Environ;
typedef std::pair<std::string, int> strCount;
typedef std::unordered_map<std::string, strCount> envmap;
typedef std::pair<std::string, Expr*> rcoPair;
std::string genNewVar(std::string type = "i", bool reset = 0);

namespace{
//X definitions
typedef std::unordered_map<std::string, int> varList;
class BlkComplete : public std::exception{
public:
    const char * what () const throw () {
      return "Used to break current Iteration on Label Change";
   }
};
//Info class
//Handles the Stack and Registers
class xInfo{
public:
    xInfo():regs_(16,0),result_(0), done_(false){};
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
        //std::cout << "setting var "<< s << " to " << val<< std::endl;
        try{
            v_.at(s) = val;
        } catch(std::out_of_range &e){
            v_.insert(std::pair<std::string, int>(s, val));
        }
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
        int i;
        std::cout << "getting var "<< name << std::endl;
        //segfaults in all of the select tests, evne though variable will have been inserted prior
        try{
            i = v_.at(name);
        }
        catch(std::out_of_range &e){
            throw std::runtime_error("undefined Var Read");
        }
        std::cout << name << " = " << i << std::endl;
        return i;
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
        std::vector<std::string> sys_ = {"read_int", "print_int"};
        auto it = std::find(std::begin(sys_), std::end(sys_), label_);
        int index, i;
        index = std::distance(sys_.begin(), it);
        switch(index){
                //read
            case 0:
                //std::cout <<"input an integer" << std::endl;
                //std::cin >> i;
                i = num_--;
                //std::cout << "int " << i << " read" << std::endl;
                i_->setReg(0,i);
                break;
                //write
            case 1:
                std::cout << i_->getReg(0) << std::endl;
                break;
                //else
            default:
                //std::cout << "setting label to " << label_ << std::endl;
                i_->setLabel(label_);
                throw BlkComplete();
        }
    }
    void reset(){ num_=42;}
protected:
   std::string label_;
   static int num_;
};
int Label::num_ = 42;

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
        int i;
        try{
            i = i_->getVar(name_);
        }catch(std::out_of_range &e){
            std::cerr << "Attempted to read undefined Var " << name_ << std::endl;
        }
        return i;
    }
    void set(int i){
        i_->setVar(name_,i);
    }
private:
    std::string name_;
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
        //throw BlkComplete();
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
        //i_->pushStack(i_->getReg(6));
        //i_->setReg(6, i_->getReg(7));
        lab_->interp();
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
    xProgram(){
        i_ = std::make_shared<xInfo>();
        init();
    }
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
        for(auto [ name, block] : blks_){
            std::vector<std::string> blockOut;
            if(name == "main") blockOut.push_back(".globl main");
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
    int run(){
        while(!(i_->isDone())){
            blks_[i_->getLabel()]->interp();
        }
        Label* temp = new Label("temp");
        temp->reset();
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

//C Definitions

typedef std::unordered_map<std::string, CExp*> CEnv;

class CLabel{
public:
    explicit CLabel(std::string name): name_(name){};
    std::string AST(){
        return name_;
    }
private:
    std::string name_;
};

class CExp{
public:
    virtual std::string AST() = 0;
    virtual int interp(CEnv* e) = 0;
    virtual blk SIExp(std::string dest) = 0;
private:
    
};

class CArg : public CExp{
public:
    virtual Arg* SIArg() = 0;
    blk SIExp(std::string dest){
        blk out;
        out.push_back(new Movq(this->SIArg(), new Ref(dest)));
        return out;
    }
private:
    
};
class CNum : public CArg{
public:
    explicit CNum(int i): i_(i){};
    std::string AST(){
        return std::to_string(i_);
    }
    int interp(CEnv* e){
        return i_;
    }
    Arg* SIArg(){
        return new Const(i_);
    }

private:
    int i_;
};
class CVar : public CArg{
public:
    explicit CVar(std::string name):n_(name){};
    std::string AST(){
        return n_;
    }
    int interp(CEnv* e){
        CExp* out;
        try{
            out = e->at(n_);
        }
        catch(std::out_of_range& e){
            std::cerr << "Attempted to read from undefined Variable " << n_ << std::endl;
            std::string errorString("Undefined Var ");
            errorString+= n_;
            throw std::runtime_error(errorString);
            
        }
        return out->interp(e);
    }
    Arg* SIArg(){
            return new Ref(n_);
    }
private:
    std::string n_;
};
class CAdd: public CExp{
public:
    explicit CAdd(CArg* ar, CArg* al):ar_(ar), al_(al){};
    std::string AST(){
        std::string out;
        out += "(+ ";
        out += ar_->AST();
        out += " ";
        out += al_->AST();
        out +=")\n";
        return out;
    }
    int interp(CEnv* e){
        return ar_->interp(e) + al_->interp(e);
    }
    blk SIExp(std::string dest){
        blk out;
        Ref* d = new Ref(dest);
        Arg* ar = ar_->SIArg(), *al = al_->SIArg();
        std::string tr = ar_->AST(), tl = al_->AST();
        if(tl != dest || tr != dest){
            out.push_back(new Movq(ar, d));
        }
        if(tl == dest){
            out.push_back(new Addq(ar, d));
        }
        else{
            out.push_back(new Addq(al, d));
        }
        return out;
    }
private:
    CArg* ar_, *al_;
};
namespace{
class CRead: public CExp{
public:
    explicit CRead():auto_(false), i_(NULL){};
    explicit CRead(bool a):auto_(a), i_(NULL){};
    std::string AST(){
        std::string out;
        out = "(Read)\n";
        return out;
    }
    int interp(CEnv* e){
        if(i_ == NULL){
            int i;
            if(auto_){
                i = n_;
                n_--;
            } else{
                std::cin >> i;
            }
            i_ = new int(i);
            return *i_;
        }
        else return *i_;
    }
    void reset(){ n_= 42;}
    blk SIExp(std::string dest){
        blk out;
        out.push_back(new Callq(new Label("read_int")));
        out.push_back(new Movq(new Reg(0), new Ref(dest)));
        return out;
    }
private:
    bool auto_;
    static int n_;
    int* i_;
};
int CRead::n_ = 42;
}
class CNeg: public CExp{
public:
    explicit CNeg(CArg* a): a_(a){};
    std::string AST(){
        std::string out;
        out += "(- ";
        out += a_->AST();
        out +=")\n";
        return out;
    }
    int interp(CEnv* e){
        return -a_->interp(e);
    }
    blk SIExp(std::string dest){
        blk out;
        Arg* a = a_->SIArg();
        Ref* r = new Ref(dest);
        if(a_->AST() != dest) out.push_back(new Movq(a, r));
        out.push_back(new Negq(r));
        return out;
    }
private:
    CArg* a_;
};
class CStat{
public:
    explicit CStat(std::string n, CExp* e):name_(n), e_(e){};
    std::string AST(){
        std::string out;
        out += "(Set! ";
        out += name_;
        out += " = ";
        out += e_->AST();
        out +=")\n";
        return out;
    }
    void interp(CEnv* e){
        (*e)[name_] = e_;
    }
    std::string getVar(){ return name_;}
    blk SIStat(){
        blk out = e_->SIExp(name_);
        return out;
    }
private:
    std::string name_;
    CExp* e_;
};
class CTail{
public:
    virtual std::string AST() = 0;
    virtual int interp(CEnv* e) = 0;
    virtual std::vector<std::string> getVars() = 0;
    virtual blk SITail() = 0;
};
class CRet: public CTail{
public:
    explicit CRet(CArg* a): ret_(a){};
    std::string AST(){
        std::string out;
        out+= "(Ret ";
        out+= ret_->AST();
        out +=")\n";
        return out;
    }
    int interp(CEnv* e){
        return ret_->interp(e);
    }
    std::vector<std::string> getVars(){
        std::vector<std::string> v;
        return v;
    }
    blk SITail(){
        blk out;
        Arg* temp = ret_->SIArg();
        out.push_back(new Movq(temp, new Reg(0)));
        out.push_back(new Jmp(new Label("END")));
        return out;
    }
private:
    CArg* ret_;
};
class CSeq: public CTail{
public:
    explicit CSeq(CStat* s, CTail* t):stmt_(s), tail_(t){};
    std::string AST(){
        std::string out;
        out+= "(seq ";
        out+= stmt_->AST();
        out += " in ";
        out += tail_->AST();
        out += ")\n";
        return out;
    }
    int interp(CEnv* e){
        stmt_->interp(e);
        return tail_->interp(e);
    }
    std::vector<std::string> getVars(){
        std::vector<std::string> v;
        v = tail_->getVars();
        v.push_back(stmt_->getVar());
        return v;
    }
    blk SITail(){
        //std::cout << "entering tail" << std::endl;
        blk out = stmt_->SIStat(), temp = tail_->SITail();
        out.insert(out.end(), temp.begin(), temp.end());
        return out;
    }
private:
    CStat* stmt_;
    CTail* tail_;
};
typedef std::unordered_map<std::string,CTail*> CTailTable;
class CInfo{
public:
    std::vector<std::string> vars(){ return vars_;}
    void setVars(std::vector<std::string> s){ vars_ = s;}
private:
    std::vector<std::string> vars_;
};
class CProg{
public:
    CProg(){};
    CProg(CTail* t){
        instr_["main"] = t;
        
    }
    std::string AST(){
        std::string out;
        for( auto [name, l]: instr_){
            out += name;
            out += ":\n";
            out += l->AST();
        }
        return out;
    }
    void addTail(std::string name, CTail* l){
        instr_[name] = l;
    }
    int run(){
        int i;
        CEnv* e = new CEnv();
        CTail* t;
        try{
            t = instr_.at("main");
        } catch( std::out_of_range& e){
            std::cerr << "Main not defined" << std::endl;
            throw std::runtime_error("Main Not Defined");
        }
        i = t->interp(e);
        CRead* temp = new CRead();
        temp->reset();
        return i;
    }
    void uncoverLocals(){
        std::vector<std::string> temp;
        for(auto [name, l] : instr_){
            std::vector<std::string> local;
            local = l->getVars();
            temp.insert(temp.end(), local.begin(), local.end());
        }
        i_.setVars(temp);
    }
    CInfo getInfo(){ return i_;}
    xProgram* selInsr(){
        xProgram* out = new xProgram();
        for(auto it = instr_.begin(); it != instr_.end(); ++it){
            //std::cout << "getting Block " << it->first << std::endl;
            Block* bl = new Block(it->second->SITail());
            out->addBlock(it->first, bl);
        }
        //initialize vars
        return out;
    }
private:
   CTailTable instr_;
   CInfo i_;
};

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
    virtual Expr* uniquify(envmap* env) = 0;
    virtual std::vector<rcoPair> rco(envmap env) = 0;
    virtual CTail* econTail() = 0;
    virtual CExp* econAssign() = 0;
protected:
    virtual Expr* cloneImpl() const = 0;
    std::unique_ptr<Expr> e1_, e2_;
    opCode op_;
    static int varCount;
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
    Expr* uniquify(envmap* env){
        return this;
    }
    bool isPure(Environ env) { return true;}
    bool containVar(std::string name){return false;}
    Expr* optE(Environ env) { return this;}
    std::vector<rcoPair> rco(envmap env){
        std::vector<rcoPair> out;
        std::string s;
        out.push_back(rcoPair(s, new Num(i_)));
        return out;
    }
    CTail* econTail(){
        return new CRet(this->econAssign());
    }
    CArg* econAssign() override{
        return new CNum(i_);
    }
protected:
    Num* cloneImpl() const override {
        return new Num(i_);
    }
private:
    void seti(int i){i_=i;};
    int i_;
};

class Var:public Expr{
public:
    Var(std::string name):name_(name){};
    Num* inter(Environ env){
        Expr* container;
        try{
            container = env.at(name_);
        }
        catch(std::out_of_range &e){
            std::cerr << "Attemted to read undefined var " << name_ << std::endl;
            throw std::runtime_error("Undefined var read");
        }
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
        Expr* temp;
        try{
            temp = env.at(name_);
        }
        catch(std::out_of_range &e){
            std::cerr << "Attemted to read undefined var " << name_ << std::endl;
            throw std::runtime_error("Undefined var read");
        }
        if( temp->isPure(env)){
            return temp;
        }
        return this;
    }
    Expr* uniquify(envmap* env){
        //update based on envmap
        std::string s;
        try{
        name_ =env->at(name_).first;
        }catch(std::out_of_range &e){
            std::cerr<< "attempted to update variable that was not previously found" << std::endl;
            throw std::runtime_error("Var uniquify failed");
        }
        return this;
    }
    std::vector<rcoPair> rco(envmap env){
        std::vector<rcoPair> out;
        std::string s;
        out.push_back(rcoPair(s, new Var(env.at(name_).first)));
        return out;
    }
    CTail* econTail(){
        return new CRet(this->econAssign());
    }
    CArg* econAssign() override{
        return new CVar(name_);
    }
protected:
    Var* cloneImpl() const override {
        return new Var(name_);
    }
private:
    std::string name_;
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
    Expr* uniquify(envmap* env){
        Expr *e1 =e1_.release(), *e2 = e2_.release();
        e1_.reset(e1->uniquify(env));  e2_.reset(e2->uniquify(env));

        return this;
    }
    bool containVar(std::string name){ return e1_->containVar(name) || e2_->containVar(name);}
    std::vector<rcoPair> rco(envmap env){
        std::vector<rcoPair> out, temp;
        std::string s;
        Expr* t1, *t2;
        temp = e1_->rco(env);
        s = temp.back().first;
        if(s.empty()){
            t1 = temp.back().second;
        }else{
            t1 = new Var(s);
            out.insert(out.end(), temp.begin(), temp.end());
        }
        temp = e2_->rco(env);
        s = temp.back().first;
        if(s.empty()){
            t2 = temp.back().second;
        }else{
            t2 = new Var(s);
            out.insert(out.end(), temp.begin(), temp.end());
        }
        out.push_back(rcoPair(genNewVar(), new Add(t1, t2)));
        return out;
    }
    CTail* econTail(){
        throw std::runtime_error("RCO Pass Failed, Add not contained in Let");
        return NULL;
    }
    CExp* econAssign(){
        CArg* c1, *c2;
        c1 = e1_->econAssign();
        c2 = e2_->econAssign();
        return new CAdd(c1, c2);
    }
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
    Expr* uniquify(envmap* env){
        Expr* e = e1_.release();
        e1_.reset(e->uniquify(env));
        return this;
    }
    std::vector<rcoPair> rco(envmap env){
        std::vector<rcoPair> out, temp;
        std::string s;
        Expr* t1;
        temp = e1_->rco(env);
        s = temp.back().first;
        if(s.empty()){
            t1 = temp.back().second;
        }else{
            t1 = new Var(s);
            out.insert(out.end(), temp.begin(), temp.end());
        }
        out.push_back(rcoPair(genNewVar(), new Neg(t1)));
        return out;
    }
    CTail* econTail(){
        throw std::runtime_error("RCO Pass Failed, Neg not contained in Let");
        return NULL;
    }
    CExp* econAssign(){
        CArg* c1;
        c1 = e1_->econAssign();
        return new CNeg(c1);
    }
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
    Expr* uniquify(envmap* env){
        return this;
    }
    void reset(){ num_= 42;}
    bool isPure(Environ env) { return false; }
    Expr* optE(Environ env) { return this; }
    bool containVar(std::string name){return false;}
    std::vector<rcoPair> rco(envmap env){
        std::vector<rcoPair> out;
        out.push_back(rcoPair(genNewVar(), new Read(mode_)));
        return out;
    }
    CTail* econTail(){
        throw std::runtime_error("RCO Pass Failed, Read not contained in Let");
        return NULL;
    }
    CExp* econAssign(){
        return new CRead(mode_);
    }
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
        env[var_] = e1_->inter(env);
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
        Expr* e1 = e1_.release();
        Expr* e2 = e2_.release();
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
    Expr* uniquify(envmap* env){
        std::string oldName = var_; std::string oldMap = var_;
        Expr* e1 = e1_.release();
        e1 = e1->uniquify(env);
        e1_.reset(e1);
        if(env->count(var_) == 0){
            std::string newString = var_;
            newString +="1"; 
            strCount newCount(newString, 1);
            env->insert(std::pair<std::string,strCount>(var_, newCount));
            oldMap = newString;
        }else{
            try{
                std::string newString = var_;
                env->at(var_).second++;
                newString +=std::to_string(env->at(var_).second);
                oldMap = env->at(var_).first;
                env->at(var_).first = newString;
            }catch(std::out_of_range &e){
                std::cerr<< "updating var in let failed" << std::endl;
                throw std::runtime_error("Let uniquify failed");
            }
        }
        var_ =env->at(var_).first;
        Expr* e2 = e2_.release();
        e2 = e2->uniquify(env);
        e2_.reset(e2);
        env->at(oldName).first = oldMap;
        return this;
    }
    std::vector<rcoPair> rco(envmap env){
        std::vector<rcoPair> out, temp;
        std::string s;
        strCount newCount;
        //Expr* t1;
        temp = e1_->rco(env);
        s = temp.back().first;
        if(s.empty()){
            s = genNewVar();
            newCount.first = s;
            temp.back().first = s;

        }else{
            newCount.first = s;
        }
        env.insert(std::pair<std::string, strCount>(var_, newCount));

        out.insert(out.end(), temp.begin(), temp.end());
        temp = e2_->rco(env);
        s = temp.back().first;
        if(s.empty() == false){
            out.insert(out.end(), temp.begin(), temp.end());
        }
        return out;
    }
    CTail* econTail(){
        CExp* assign = e1_->econAssign();
        CTail* body = e2_->econTail();
        return new CSeq(new CStat(var_, assign), body);
    }
    CExp* econAssign(){
        throw std::runtime_error("RCO Pass Failed, Let used in Assignment");
        return NULL;
    }
protected:
    Let* cloneImpl() const override {
        return new Let(var_,(e1_->clone().release()), (e2_->clone().release()));
    };
private:
    std::string var_;
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
    explicit Program(){ 
        i_ = new Info();
    };
    explicit Program(Info* i, Expr* e):e_(e){
        i_ = new Info();
    };
    explicit Program(Expr* e):e_(e) {
        i_ = new Info();
    };
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
        try{
            out = e_->inter(env);
        }
        catch(std::runtime_error &e){
            std::cerr << "run() failed, dumping AST" << std::endl;
            std::cerr << print() << std::endl;
            throw e;
        }
        Read* temp = new Read();
        temp->reset();
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
    friend Program* uniquify(Program* orig);
    friend Program* rco(Program* orig);
    inline Info* getInfo() { return i_; };
    inline void setExpr(Expr* n) { e_.reset(n); };
    inline void setInfo(Info* n) { i_ = n; };
    friend Program* opt( Program* orig);
    friend CProg* econ(Program* orig);
    private:
    std::unique_ptr<Expr> e_;
    Info* i_;
};
//functions
Program* pow(int x, int b = 2);
Program* randProg(int depth);



#endif

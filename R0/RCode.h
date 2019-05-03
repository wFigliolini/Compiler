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
#include <set>
#include <map>
#include <queue>
#include <unistd.h>
#include <sys/wait.h>

//virtual Expression class
const std::map<std::string,int> regNames = {
    {"%rax",0}, {"%rbx",1}, {"%rcx",2},
    {"%rdx",3}, {"%rsi",4}, {"%rdi",5},
    {"%rbp",6}, {"%rsp",7}, {"%r8",8},
    {"%r9",9}, {"%r10",10}, {"%r11",11},
    {"%r12",12}, {"%r13",13}, {"%r14",14},
    {"%r15",15}};
const std::map<int,std::string> regNums = {
    {0,"%rax"}, {1,"%rbx"}, {2,"%rcx"},
    {3,"%rdx"}, {4,"%rsi"}, {5,"%rdi"},
    {6,"%rbp"}, {7,"%rsp"}, {8,"%r8"},
    {9,"%r9"}, {10,"%r10"}, {11,"%r11"},
    {12,"%r12"}, {13,"%r13"}, {14,"%r14"},
    {15,"%r15"}};
//check which ones to be removed
const std::map<int,std::string> regAsn = {
    {0,"%rax"}, {1,"%rbx"}, {2,"%rcx"},
    {3,"%rdx"}, {4,"%rsi"}, {5,"%rdi"},
    {6,"%r8"},  {7,"%r9"}, {8,"%r10"}, 
    {9,"%r11"}, {10,"%r12"}, {11,"%r13"},
    {12,"%r14"},{13,"%r15"}};
const std::map<std::string,int> cmpOps = {
    {"<",0}, {"<=",1}, {"==",2}, {">=",3}, {">",4}
};
const std::map<int,std::string> cmpOut = {
    {0, "<"}, {1,"<="}, {2,"=="}, {3,">="}, {4,">"}
};
enum ty {S64, BOOL, UNINIT = -1};
class Expr;
class Type;
class CExp;
class Instr;
class Arg;
class Reg;
class DeRef;
typedef std::unordered_map<std::string, Expr*> Environ;
typedef std::pair<std::string, int> strCount;
typedef std::unordered_map<std::string, strCount> envmap;
typedef std::pair<std::string, Expr*> rcoPair;
typedef std::unordered_map<std::string, int> localVars;
typedef std::vector<Instr*> Blk;
typedef std::set<std::string> varSet;
typedef std::vector<varSet> liveSet;
typedef std::string Vertex;
typedef std::map<Vertex,std::set<Vertex>>  infrGraph;
typedef std::map<Vertex,std::set<Vertex>>  movGraph;
typedef std::pair<Vertex, Vertex> Edge;
typedef std::map<Vertex, int> colorMap;
typedef std::pair<int*,Vertex> Sat;
typedef std::map<Vertex, int*> satSet;
typedef std::map<Vertex, Arg*> assignEnv;
typedef std::map<Vertex, int> regMap;
typedef std::unordered_map<std::string, ty> TCEnv;
std::string genNewVar(std::string type = "i", bool reset = 0);

class compareClass{
public:
    bool operator()(const Sat a, const Sat b){
        return (*a.first) < (*b.first);
    }
};

Type* numAdd(Type* const l, Type* const r);
Type* numNeg(Type* const l);
Type* boolAnd(Type* const l, Type* const r);
Type* boolOr(Type* const l, Type* const r);
Type* boolNot(Type* const l);
typedef std::priority_queue<Sat, std::vector<Sat>, compareClass> satHeap;
//X definitions
typedef std::unordered_map<std::string, int> varList;
class BlkComplete : public std::exception{
public:
    const char * what () const throw () {
      return "Used to break current Iteration on Label Change";
   }
};
//compare function for saturation queue



//Info class
//Handles the Stack and Registers
class xInfo{
public:
    xInfo():regs_(16,0), stack_(1024,0),result_(0),varCount_(0), done_(false){};
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
        int out;
        //std::cout << "accessing register "<< reg << std::endl;
        try{
            out = regs_.at(reg);
        }
        catch(std::out_of_range &e){
            std::cerr << "Register get failed. Regs_ size: " << regs_.size()<< " Register accessed: "<< reg << std::endl;
            throw std::runtime_error("RegAccess Failed");
        }
        return out;
    }
    int getStack(int ad){
        int out;
        try{
            out = stack_.at(ad/8);
        }
        catch(std::out_of_range &e){
            std::cerr << "Stack get failed. Stack_ size: " << stack_.size()<< " Address accessed: "<< ad<< std::endl;
            throw std::runtime_error("StackAccess Failed");
        }
        return out;
    }
    std::string getLabel(){
        return currLabel_;
    }
    int getVar(std::string name){
        int i;
        //std::cout << "getting var "<< name << std::endl;
        //segfaults in all of the select tests, even though variable will have been inserted prior
        try{
            i = v_.at(name);
        }
        catch(std::out_of_range &e){
            throw std::runtime_error("undefined Var Read");
        }
        //std::cout << name << " = " << i << std::endl;
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
    int getVC(){ return varCount_;}
    void initVar(std::string n){
        try{
            v_.at(n);
        }
        catch(std::out_of_range &e){
            v_.insert(std::pair<std::string, int>(n,0));
            ++varCount_;
        }
    }
    void destroyVar(){ --varCount_;}
private:
    std::vector<int> regs_;
    std::vector<int> stack_;
    std::string currLabel_;
    varList v_;
    int result_;
    int varCount_;
    bool done_;
};
class blkInfo{
public:
    blkInfo(int size):l_(size, varSet()){
        stackVars_ = 0;
    };
    varSet getIndex(int i){
        if(i >= l_.size()) return varSet();
        varSet out(l_[i]);
        return out;
    }
    void setIndex(unsigned int i, Instr* instr);
    unsigned int getSetSize() const{
        return l_.size();
    }
    void printSet(){
        for(auto it1 : l_){
            std::cout << "{ ";
            for(auto it2 :it1){
                std::cout << it2 << " ";
            }
            std::cout <<"}"<< std::endl;
        }
    }
    void setSet(liveSet i){
        l_ = i;
    }
    void genVars(){
        if(vars_.empty()){
            for(auto it: l_){
                vars_.insert(it.begin(), it.end());
            }
        }
    }
    varSet getVars(){
        varSet out(vars_);
        return out;
    }
    infrGraph getGraph(){
        infrGraph out(graph_);
        return out;
    }
    void addEdge(std::string a, std::string b){
        graph_[a].insert(b);
        graph_[b].insert(a);
    }
    void addMov(std::string a, std::string b){
        mov_[a].insert(b);
        mov_[b].insert(a);
    }
    void addVertex(std::string a){
        graph_.insert(std::pair<Vertex, std::set<Vertex>>(a, {}));
        mov_.insert(std::pair<Vertex, std::set<Vertex>>(a, {}));
    }
    //possibly overdone & could be simplified
    void genColorMap(bool moveBias){
        //initialize colormap
        satHeap W;
        satSet G;
        int i;
        for(auto it: vars_){
            //default to -1
            int unassigned = -1;
            //if member is a register, assign to register
            //should only be rax at end of block and potential calls
            if(regNames.find(it) != regNames.end()){
                unassigned = regNames.find(it)->second;
            }
            assignments_.insert(std::pair<Vertex, int>(it, unassigned));
            if(unassigned == -1) G.insert(std::pair<std::string,int*>(it, new int(0)));
        }
        //initialize saturations and Heap
        for(auto it = graph_.begin(); it != graph_.end(); ++it){
            std::string name = it->first;
            int* s;
            try{
                s = G.at(name);
            }
            catch(std::out_of_range &e){
                //found a register in the interference graph
                //registers have inherent color, and as such
                //should not be added to the Heap or have saturation calculated
                continue;
            }
            for(auto it2: it->second){
                if(hasColor(it2)){
                    (*s)++;
                }
            }
            
            W.push(std::pair<int*,std::string>(s, name));
        }
        while(W.empty() == false){
            std::pair<int*, std::string> g = W.top();
            std::string name = g.second;
            //std::cout << "generating color for "<< name << " Saturation: "<< (*g.first) <<std::endl;
            std::set<int> unavail;
            //generate vertex's unvailable color set
            for(auto it: graph_[name]){
                int color = assignments_[it];
                if(color != -1) unavail.insert(color);
            }
            if(moveBias){
                for( auto it : mov_[name]){
                    i = assignments_[it];
                    if(unavail.find(i) == unavail.end()){
                        assignments_[name] = i;
                        for(auto it2: graph_[name]){
                            try{
                                int* s = G.at(it2);
                                (*s)++;
                            }
                            catch(std::out_of_range &e){
                                continue;
                            }
                        }
                        //loop will terminate once a color has been found
                        break;
                    }
                }
            }
            if(moveBias == false || assignments_[name] == -1){
                //run until color is found
                for(i = 0; ; i++){
                    //if i is not in the unavailible set, 
                    //use and update all adjacent nodes
                    if(unavail.find(i) == unavail.end()){
                        assignments_[name] = i;
                        for(auto it: graph_[name]){
                            try{
                                int* s = G.at(it);
                                (*s)++;
                            }
                            catch(std::out_of_range &e){
                                continue;
                            }
                        }
                        //loop will terminate once a color has been found
                        break;
                    }
                }
            }
            //heap does not update until after pop,
            //so it must be placed after updating 
            //saturation
            W.pop();
        }
        
    }
    colorMap getColorMap(){
        colorMap out(assignments_);
        return out;
    }
    bool hasColor(Vertex v){
        return assignments_[v] != -1;
    }
    void dumpColor(){
        for(auto it :assignments_){
            std::cout << it.first << ": " << it.second << std::endl;
        }
    }
    void dumpGraph(){
        for(auto it : graph_){
            std::cout << it.first << ": { ";
            for(auto it2 : it.second){
                std::cout << it2 << " ";
            }
            std::cout << "}" << std::endl;
        }
    }
    void dumpMov(){
        for(auto it : mov_){
            std::cout << it.first << ": { ";
            for(auto it2 : it.second){
                std::cout << it2 << " ";
            }
            std::cout << "}" << std::endl;
        }
    }
    //Vertex -> Reg/Stack
    //push back to uCR if  r12-r15
    //increment stackvars if color > 13
    
    void genEnv();
    Arg* getArg(std::string name){
        return env_[name];
    }
    assignEnv getEnv(){
        assignEnv out(env_);
        return out;
    }
    int getStackVars(){
        int out(stackVars_);
        return out;
    }
    std::set<std::string> getCalleeRegs(){
        std::set<std::string> out(usedCalleeRegs_);
        return out;
    }
    void printEnv();
    movGraph getMov(){
        movGraph out(mov_);
        return out;
    }
    int getColor(std::string name){
        return assignments_[name];
    }
    void dumpVars(){
        for(auto it :vars_){
            std::cout << it<< std::endl;
        }
    }
    void setVars(varSet v){
        vars_ = v;
    }
private:
    liveSet l_;
    infrGraph graph_;
    movGraph mov_;
    varSet vars_;
    colorMap assignments_;
    assignEnv env_;
    int stackVars_;
    std::set<std::string> usedCalleeRegs_;
};

class X{
public:
    void setInfo(std::shared_ptr<xInfo> i){i_ = i;}
protected:
    std::shared_ptr<xInfo> i_;
    std::shared_ptr<blkInfo> bi_;
};

class Label: public X{
public:
    explicit Label (std::string label): label_(label){};
    std::string emitL(bool vars){
        return label_;
    }
    void interp(){
        std::vector<std::string> sys_ = {"_read_int", "_print_int"};
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

class Arg: public X{
public:
    virtual const std::string emitA(bool vars) = 0;
    virtual int get() = 0;
    virtual void set(int i) = 0;
    virtual Arg* asHA(localVars* e) = 0;
    virtual void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi) = 0;
    virtual bool isMemRef() = 0;
    virtual std::string ul() = 0;
    virtual Arg* asRA() = 0;
    virtual bool isConst() = 0;
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
        try{
            reg_ = regNames.at(reg);
        }catch(std::out_of_range &e){
            std::string err("Register assignment attempted to nonexistant register ");
            err+= reg;
            throw std::invalid_argument(err);
        }
        return;
    }
    const std::string emitA(bool vars){
        std::string out = regNums[reg_];
        return out;
    }
    int get(){
        return i_->getReg(reg_);
    }
    void set(int i){
        i_->setReg(reg_,i);
    }
    Arg* asHA(localVars* e){
        return new Reg(reg_);
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        setInfo(i);
        bi_ = bi;
    }
    bool isMemRef(){
        return false;
    }
    std::string ul(){
        std::string out(regNums[reg_]);
        return out;
    }
    Arg* asRA(){
        return new Reg(reg_);
    }
    bool isConst(){
        return false;
    }
private:
    int reg_;
};

class Const: public Arg{
public:
    explicit Const( int con ):con_(con){};
    const std::string emitA(bool vars){
        std::string out("$");
        out+= std::to_string(con_);
        return out;
    }
    int get(){
        return con_;
    }
    void set(int i){
        throw std::logic_error("Can't Write to Constant");
    }
    Arg* asHA(localVars* e){
        return new Const(con_);
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        setInfo(i);
        bi_ = bi;
    }
    bool isMemRef(){
        return false;
    }
    std::string ul(){
        std::string out;
        return out;
    }
    Arg* asRA(){
        return new Const(con_);
    }
    bool isConst(){
        return true;
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
        //std::cout << "getting register " << reg_->emitA(1) <<std::endl;
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
    Arg* asHA(localVars* e){
        return new DeRef(new Reg(reg_->emitA(0)), offset_);
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        setInfo(i);
        bi_ = bi;
        reg_->init(i, bi);
    }
    bool isMemRef(){
        return true;
    }
    std::string ul(){
        std::string out;
        return out;
    }
    Arg* asRA(){
        return new DeRef(new Reg(reg_->emitA(0)), offset_);
    }
    bool isConst(){
        return false;
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
        int i =0;
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
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        setInfo(i);
        bi_ = bi;
        i_->initVar(name_);
    }
    Arg* asHA(localVars* e){
        int offset;
        try{
            offset = e->at(name_);
        }
        catch(std::out_of_range &ex){
            offset = 8*(e->size()+1); 
            e->insert(std::pair<std::string, int>(name_, offset));
        }
        return new DeRef(new Reg("%rsp"), offset);
    }
    bool isMemRef(){
        return true;
    }
    std::string ul(){
        return name_;
    }
    Arg* asRA(){
        Arg* out;
        if(bi_->getArg(name_) != NULL) out = bi_->getArg(name_);
        else{
            std::string err("Arg ");
            err+= name_;
            err += " has been assigned to NULL. Dumping colors:\n";
            bi_->dumpColor();
            bi_->dumpVars();
            throw std::runtime_error(err);
        }
        return out;
    }
    bool isConst(){
        return false;
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
   virtual void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi) = 0;
   virtual Instr* asHI(localVars *e) = 0;
   virtual Blk patchI() = 0;
   virtual bool isRet(){
       return false;
   }
   virtual varSet ul(varSet lb) = 0;
   virtual void genInterferences(int index) =0;
   virtual Instr* asRI() = 0;
protected:
    Arg* al_, *ar_;
    bool isControl;
};
class Movq: public Instr{
public:
    Movq(Arg* a1, Arg* a2): Instr(a1, a2){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "movq ";
        output += al_->emitA(vars);
        output += ", ";
        output += ar_->emitA(vars);
        output += "\n";
        return output;
    }
    void interp(){
        ar_->set(al_->get());
    }
    Instr* asHI(localVars *e){
        return new Movq(al_->asHA(e), ar_->asHA(e));
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        bi_ = bi;
        al_->init(i, bi);
        ar_->init(i, bi);

    }
    Blk patchI();
    varSet ul(varSet lb){
        std::string t1, t2;
        t1 = al_->ul();
        t2 = ar_->ul();
        if(t2.empty() ==false){
            auto it = lb.find(t2);
            if(it != lb.end()) lb.erase(it);
        }
        if(t1.empty() == false){
            lb.insert(t1);
        }
        return lb;
    }
    void genInterferences(int index){
        varSet after = bi_->getIndex(index);
        std::string s(al_->ul()),d(ar_->ul());
        if(!al_->isConst() && !ar_->isConst()) bi_->addMov(s,d);
        for(auto it : after){
            if(d.empty()) break;
            if(it == s || it == d) continue;
            bi_->addEdge(it, d);
        }
    }
    Instr* asRI(){
        return new Movq(al_->asRA(), ar_->asRA());
    }
};
class Addq: public Instr{
public:
    Addq(Arg* a1, Arg* a2): Instr(a1, a2){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "addq ";
        output += al_->emitA(vars);
        output += ", ";
        output += ar_->emitA(vars);
        output += "\n";
        return output;
    }
    void interp(){
        int out;
        out =  al_->get() + ar_->get();
        ar_->set(out);
    }
    Instr* asHI(localVars *e){
        return new Addq(al_->asHA(e), ar_->asHA(e));
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        bi_ = bi;
        al_->init(i, bi);
        ar_->init(i, bi);

    }
    Blk patchI();
    varSet ul(varSet lb){
        std::string t1, t2;
        t1 = al_->ul();
        t2 = ar_->ul();
        /*if(t2.empty() ==false){
            auto it = lb.find(t2);
            if(it != lb.end()) lb.erase(it);
        }*/
        if(t1.empty() == false){
            lb.insert(t1);
        }
        return lb;
    }
    void genInterferences(int index){
        varSet after = bi_->getIndex(index);
        std::string d(ar_->ul());
        for(auto it : after){
            if( it == d) continue;
            bi_->addEdge(it, d);
        }
    }
    Instr* asRI(){
        return new Addq(al_->asRA(), ar_->asRA());
    }
};
class Subq: public Instr{
public:
    Subq(Arg* a1, Arg* a2): Instr(a1, a2){};
    virtual std::string emitI(bool vars){
        std::string output;
        output += "subq ";
        output += al_->emitA(vars);
        output += ", ";
        output += ar_->emitA(vars);
        output += "\n";
        return output;
    }
    void interp(){
        int out;
        out =  al_->get() - ar_->get();
        ar_->set(out);
    }
    Instr* asHI(localVars *e){
        return new Subq(al_->asHA(e), ar_->asHA(e));
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        bi_ = bi;
        al_->init(i,bi);
        ar_->init(i,bi);

    }
    Blk patchI();
    varSet ul(varSet lb){
        std::string t1, t2;
        t1 = al_->ul();
        t2 = ar_->ul();
        /*if(t2.empty() ==false){
            auto it = lb.find(t2);
            if(it != lb.end()) lb.erase(it);
        }*/
        if(t1.empty() == false){
            lb.insert(t1);
        }
        return lb;
    }
    void genInterferences(int index){
        varSet after = bi_->getIndex(index);
        std::string d(ar_->ul());
        for(auto it : after){
            if( it == d) continue;
            bi_->addEdge(it, d);
        }
    }
    Instr* asRI(){
        return new Subq(al_->asRA(), ar_->asRA());
    }
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
    Instr* asHI(localVars *e){
        return new Jmp(new Label(lab_->emitL(1)));
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        bi_ = bi;
        lab_->setInfo(i);
    }
    Blk patchI(){
        Blk out;
        out.push_back(this->asHI(NULL));
        return out;
    }
    varSet ul(varSet lb){
        return lb;
    }
    void genInterferences(int index){
        
    }
    Instr* asRI(){
        return new Jmp(new Label(lab_->emitL(1)));
    }
private:
    Label* lab_;
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
        i_->setResult();

    }
    Instr* asHI(localVars *e){
        //return END fto make compatible with headers added at end of assign
        return new Jmp(new Label("END"));
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        bi_ = bi;
    }
    Blk patchI(){
        Blk out;
        out.push_back(new Retq());
        return out;
    }
    bool isRet(){
        return true;
    }
    varSet ul(varSet lb){
        return lb;
    }
    void genInterferences(int index){
        
    }
    Instr* asRI(){
        return new Jmp(new Label("END"));
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
    Instr* asHI(localVars *e){
        return new Negq(al_->asHA(e));
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        bi_ = bi;
        al_->init(i,bi);
    }
    Blk patchI(){
        Blk out;
        out.push_back(this->asHI(NULL));
        return out;
    }
    varSet ul(varSet lb){
        //return lb as negq is a unary operator which reads and writes,
        //resulting in removing then immediately re-adding the variable
        return lb;
    }
    void genInterferences(int index){
        
    }
    Instr* asRI(){
        return new Negq(al_->asRA());
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
    Instr* asHI(localVars *e){
        return new Callq(new Label(lab_->emitL(1)));
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        bi_ = bi;
        lab_->setInfo(i);
    }
    Blk patchI(){
        Blk out;
        out.push_back(this->asHI(NULL));
        return out;
    }
    varSet ul(varSet lb){
        return lb;
    }
    void genInterferences(int index){
        varSet after = bi_->getIndex(index);
        std::vector<std::string> callerRegs = { "%rax", "%rdx", "%rcx", "%rsi", "%rdi", "%r8", "%r9", "%r10", "%r11"};
        for(auto it : callerRegs){
            bi_->addVertex(it);
            for(auto it2: after){
                if(it == it2) continue;
                bi_->addEdge(it, it2);
            }
        }
    }
    Instr* asRI(){
        return new Callq(new Label(lab_->emitL(1)));
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
    Instr* asHI(localVars *e){
        return new Pushq(al_->asHA(e));
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        bi_ = bi;
        al_->init(i,bi);
    }
    Blk patchI(){
        Blk out;
        out.push_back(this->asHI(NULL));
        return out;
    }
    varSet ul(varSet lb){
        //unary read
        std::string t =al_->ul();
        lb.insert(t);
        return lb;
    }
    void genInterferences(int index){
        
    }
    Instr* asRI(){
        return new Pushq(al_->asRA());
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
    Instr* asHI(localVars *e){
        return new Popq(al_->asHA(e));
    }
    void init(std::shared_ptr<xInfo> i, std::shared_ptr<blkInfo> bi){
        bi_ = bi;
        al_->init(i,bi);
    }
    Blk patchI(){
        Blk out;
        out.push_back(this->asHI(NULL));
        return out;
    }
    varSet ul(varSet lb){
        std::string t =al_->ul();
        //unary write
        if(t.empty() == false){
            auto it = lb.find(t);
            lb.erase(it);
        }
        return lb;
    }
    void genInterferences(int index){
        varSet after = bi_->getIndex(index);
        std::string d(al_->ul());
        for(auto it: after){
            if(it == d) continue;
            bi_->addEdge(it, d);
        }
    }
    Instr* asRI(){
        return new Popq(al_->asRA());
    }
};
//block definition
class Block: public X{
    public:
    Block(){}
    Block(Blk b ): blk_(b){}
    std::vector<std::string> emitB(bool vars){
        std::vector<std::string> output;
        for( auto it : blk_){
            output.push_back(it->emitI(vars));
        }
        return output;
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
    void init(std::shared_ptr<xInfo> i){
        setInfo(i);
        bi_ = std::make_shared<blkInfo>(blk_.size());
        for(auto it : blk_){
            it->setInfo(i);
            it->init(i, bi_);
        }
    }
    Block* asHB(localVars* e){
        Blk exprs;
        for(auto it : blk_){
            Instr* temp = it->asHI(e);
            exprs.push_back(temp);
        }
        Block* out = new Block(exprs);
        return out;
    }
    Block* patchB(){
        Blk exprs;
        for(auto it : blk_){
            Blk temp = it->patchI();
            for(auto it2 : temp){
                exprs.push_back(it2);
            }
        }
        Block* out = new Block(exprs);
        return out;
    }
    unsigned int size() const{
        return blk_.size();
    }
    void uncoverLive(){
        int last = blk_.size();
        for( int i = last; i >= 0; --i){
            if(i == last){
                //bi_->setSet(liveSet());
                continue;
            }
            else{
                bi_->setIndex(i, blk_[i+1]);
            }
        }
        bi_->genVars();
    }
    bool testLive(liveSet d){
        varSet act, exp;
        bool result;
        int size1 = bi_->getSetSize(), size2 =blk_.size();
        if(size1 != size2){
            std::cout<< "Set size mismatch, set size = " << size1 << " Block size = " << size2 << std::endl;
            return false;
        }
        for(int i = 0; i < blk_.size(); ++i){
            act = bi_->getIndex(i);
            exp = d[i];
            result = (act == exp);
            if(result == false){
                std::cout << "testLive Failed at index i = " << i<<std::endl;
                std::cout << "act contained: ";
                for(auto it :act){
                    std::cout << it << " ";
                }
                std::cout << std::endl;
                std::cout << "exp contained: ";
                for(auto it :exp){
                    std::cout << it << " ";
                }
                std::cout << std::endl;
                std::cout<< "dumping liveSet"<< std::endl;
                bi_->printSet();
                break;
            }
        }
        return result;
    }
    void printSet(){
        bi_->printSet();
    }
    varSet getVars(){
        return bi_->getVars();
    }
    infrGraph getGraph(){
        return bi_->getGraph();
    }
    void genGraph(){
        //initialize graph vertexs
        varSet t = bi_->getVars();
        for(auto it : t){
            bi_->addVertex(it);
        }
        for(int i = 0; i < blk_.size(); ++i){
            blk_[i]->genInterferences(i);
        }
    }
    void genColorMap(bool moveBias){
        bi_->genColorMap(moveBias);
    }
    colorMap getColorMap(){
        return bi_->getColorMap();
    }
    void dumpColor(){
        bi_->dumpColor();
    }
    void dumpGraph(){
        bi_->dumpGraph();
    }
    void dumpMov(){
        bi_->dumpMov();
    }
    Block* asRB(){
        Blk exprs;
        for(auto it : blk_){
            Instr* temp = it->asRI();
            exprs.push_back(temp);
        }
        Block* out = new Block(exprs);
        return out;
    }
    int getStackVars(){
        return bi_->getStackVars();
    }
    std::set<std::string> getCalleeRegs(){
        return bi_->getCalleeRegs();
    }
    void genEnv(){
        bi_->genEnv();
    }
    
    bool testRegisters(assignEnv d){
        bool result;
        assignEnv act = bi_->getEnv();
        Arg* a;
        std::string gen, data, failVar;
        for(auto it : bi_->getVars()){
            failVar = it;
            try{
                a = act.at(it);
                gen = a->emitA(1);
            }
            catch(std::out_of_range & e){
                result = false;
                break;
            }
            try{
                a = d.at(it);
                data = a->emitA(1);
            }
            catch(std::out_of_range & e){
                result = false;
                break;
            }
            result = (data == gen);
            if(result == false){
                break;
            }
        }
        if(result == false){
            std::cout << "testRegister Failed at variable = " << failVar <<std::endl;
            std::cout << "generated arg: "<< gen<< " given: " << data<<std::endl;
            std::cout<< "dumping assignEnv"<< std::endl;
            bi_->printEnv();
        }
        return result;
    }
    movGraph getMov(){
        return bi_->getMov();
    }
    void setVars(std::set<std::string> var){
        bi_->setVars(var);
    }
private:
    Blk blk_;
};
typedef std::unordered_map<std::string, Block*> blkList; 
//Program definition
class xProgram{
    public:
    xProgram(){
        init();
    }
    xProgram(Block* blk){
        i_ = std::make_shared<xInfo>();
        blks_["BODY"] = blk;
        init();
    }
    void declareVar(std::string n, int v){
        i_->setVar(n,v);
    }
    std::vector<std::string> emit(bool vars){
        std::vector<std::string> output;
        output.push_back(".globl main\n");
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
        blks_[name]->init(i_);
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
        n+=".s";
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
            *err = execl("/usr/bin/cc","R0/runtime.c", "o.s" ,"-o", "o.bin", NULL);
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
    xProgram* assignHomes(){
        Block *start = genHeader(), *end = genEnd(), *temp = new Block();
        xProgram* out = new xProgram();
        localVars* env = new localVars();
        
        for(auto [name, block]: blks_){
            temp = block->asHB(env);
            out->addBlock(name, temp);
        }
        out->addBlock("START", start);
        out->addBlock("END", end);

        out->i_->setLabel("START");
        return out;
    }
    xProgram* patchP(){
        Block* temp;
        xProgram* out = new xProgram();
        for(auto [name, block]: blks_){
            temp = block->patchB();
            out->addBlock(name, temp);
        }
        out->i_->setLabel("START");
        return out;
        
    }
    bool containVar(){
        return ((i_->getVC()!=0) ? true : false);
    }
    int size(std::string name) const{
        Block* temp= blks_[name];
        unsigned int out = temp->size();
        return out;
    }
    xProgram* genMain(){
        xProgram* out = this->patchP();
        std::string sMain("main");
        Blk temp;
        temp.push_back(new Callq(new Label("START")));
        temp.push_back(new Movq(new Reg(0), new Reg("%rdi")));
        temp.push_back(new Callq(new Label("_print_int")));
        temp.push_back(new Retq());
        Block* main = new Block(temp);
        out->addBlock(sMain, main);
        out->i_->setLabel(sMain);
        return out;
    }
    
    void genEnv(){
        for(auto [name, blk]: blks_){
            blk->genEnv();
        }
    }
    xProgram* assignRegisters(){
        Block  *temp = new Block();
        xProgram* out = new xProgram();
        
        for(auto [name, block]: blks_){
            temp = block->asRB();
            out->addBlock(name, temp);
        }
        Block *start = genSmartHeader(temp->getStackVars(), temp->getCalleeRegs()), *tail = genSmartEnd(temp->getStackVars(), temp->getCalleeRegs());
        out->addBlock("START", start);
        out->addBlock("END", tail);

        out->i_->setLabel("START");
        return out;
    }

    void uncoverLive(){
        for(auto [name, blk]: blks_){
            blk->uncoverLive();
        }
    }
    bool testLive(liveSet d){
        Block* blk = blks_["BODY"];
        bool result = blk->testLive(d);
        return result;
    }
    void printSet(std::string name){
        blks_[name]->printSet();
    }
    varSet getVars(std::string name){
        return blks_[name]->getVars();
    }
    infrGraph getGraph(std::string name){
        return blks_[name]->getGraph();
    }
    movGraph getMov(std::string name){
        return blks_[name]->getMov();
    }
    void genGraphs(){
        for(auto [name, blk] : blks_){
            blk->genGraph();
        }
    }
    void genColorMaps(bool moveBias = 1){
        for(auto [name, blk] : blks_){
            blk->genColorMap(moveBias);
        }
    }
    colorMap getColors(std::string name){
        return blks_[name]->getColorMap();
    }
    void dumpColor(std::string name){
        blks_[name]->dumpColor();
    }
    void dumpGraph(std::string name){
        blks_[name]->dumpGraph();
    }
    void dumpMov(std::string name){
        blks_[name]->dumpMov();
    }
    bool testRegisters(assignEnv d){
        Block* blk = blks_["BODY"];
        bool result = blk->testRegisters(d);
        return result;
    }
    void setVars(std::set<std::string> var){
        blks_["BODY"]->setVars(var);
    }
private:
    void init(){
        i_ = std::make_shared<xInfo>();
        i_->setLabel("BODY");
        for(auto [name, block] : blks_){
            block->init(i_);
        }
        return;
    }
    Block* genHeader(){
        std::vector<Instr*> start;
        int vc = i_->getVC();
        if(vc % 2 == 1) vc+=1;
        vc *= 8;
        start.push_back(new Pushq(new Reg("%rbp")));
        start.push_back(new Movq(new Reg("%rsp"), new Reg("%rbp")));
        start.push_back(new Subq(new Const(vc), new Reg("%rsp")));
        start.push_back(new Jmp(new Label("BODY")));
        Block* out = new Block(start);
        return out;
    }
    Block* genEnd(){
        std::vector<Instr*> end;
        int vc = i_->getVC();
        if(vc % 2 == 1) vc+=1;
        vc *= 8;
        end.push_back(new Addq(new Const(vc), new Reg("%rsp")));
        end.push_back(new Popq(new Reg("%rbp")));
        end.push_back(new Retq());

        Block* out = new Block(end);
        return out;
    }
    Block* genSmartHeader(int regs, std::set<std::string> callees){
        std::vector<Instr*> start;
        if(regs % 2 == 1) regs+=1;
        regs *= 8;
        start.push_back(new Pushq(new Reg("%rbp")));
        start.push_back(new Movq(new Reg("%rsp"), new Reg("%rbp")));
        for(auto it:callees){
            start.push_back(new Pushq(new Reg(it)));
        }
        start.push_back(new Subq(new Const(regs), new Reg("%rsp")));
        start.push_back(new Jmp(new Label("BODY")));
        Block* out = new Block(start);
        return out;
    }
    Block* genSmartEnd(int regs, std::set<std::string> callees){
        std::vector<Instr*> end;
        if(regs % 2 == 1) regs+=1;
        regs *= 8;
        end.push_back(new Addq(new Const(regs), new Reg("%rsp")));
        for(auto it = callees.rbegin(); it != callees.rend(); ++it){
            end.push_back(new Popq(new Reg(*it)));
        }
        end.push_back(new Popq(new Reg("%rbp")));
        end.push_back(new Retq());

        Block* out = new Block(end);
        return out;
    }
    blkList blks_;
    std::shared_ptr<xInfo> i_;
};


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
    virtual Blk SIExp(std::string dest) = 0;
private:
    
};

class CArg : public CExp{
public:
    virtual Arg* SIArg() = 0;
    Blk SIExp(std::string dest){
        Blk out;
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
    Blk SIExp(std::string dest){
        Blk out;
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
    Blk SIExp(std::string dest){
        Blk out;
        out.push_back(new Callq(new Label("_read_int")));
        out.push_back(new Movq(new Reg(0), new Ref(dest)));
        return out;
    }
private:
    bool auto_;
    static int n_;
    int* i_;
};
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
    Blk SIExp(std::string dest){
        Blk out;
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
    Blk SIStat(){
        Blk out = e_->SIExp(name_);
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
    virtual std::set<std::string> getVars() = 0;
    virtual Blk SITail() = 0;
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
    std::set<std::string> getVars(){
        std::set<std::string> v;
        return v;
    }
    Blk SITail(){
        Blk out;
        Arg* temp = ret_->SIArg();
        out.push_back(new Movq(temp, new Reg(0)));
        out.push_back(new Retq());
        //out.push_back(new Jmp(new Label("END")));
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
    std::set<std::string> getVars(){
        std::set<std::string> v;
        v = tail_->getVars();
        v.insert(stmt_->getVar());
        return v;
    }
    Blk SITail(){
        //std::cout << "entering tail" << std::endl;
        Blk out = stmt_->SIStat(), temp = tail_->SITail();
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
    std::set<std::string> vars(){ return vars_;}
    void setVars(std::set<std::string> s){ vars_ = s;}
private:
    std::set<std::string> vars_;
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
        std::set<std::string> temp;
        for(auto [name, l] : instr_){
            std::set<std::string> local;
            local = l->getVars();
            temp.insert(local.begin(), local.end());
        }
        i_.setVars(temp);
    }
    CInfo getInfo(){ return i_;}
    xProgram* selInsr(){
        xProgram* out = new xProgram();
        for(auto it = instr_.begin(); it != instr_.end(); ++it){
            //std::cout << "getting Block " << it->first << std::endl;
            Block* bl = new Block(it->second->SITail());
            if(it->first == "main"){
                out->addBlock("BODY", bl);
            }
            else{
                out->addBlock(it->first, bl);
            }
        }
        out->setVars(i_.vars());
        //initialize vars
        return out;
    }
private:
   CTailTable instr_;
   CInfo i_;
};
class Type;
class Expr {
public:
    //standard constructors
    explicit Expr(){}
    explicit Expr(Expr* n):e1_(n){}
    explicit Expr(Expr* n1, Expr* n2):e1_(n1), e2_(n2){}
    //clone function to handle copies
    auto clone() const { return std::unique_ptr<Expr>(cloneImpl()); };
    virtual Type* inter(Environ env) = 0;
    virtual std::string AST() = 0;
    virtual Expr* optE(Environ env) = 0;
    virtual bool isPure(Environ env) = 0;
    virtual bool containVar(std::string name) = 0;
    virtual Expr* uniquify(envmap* env) = 0;
    virtual std::vector<rcoPair> rco(envmap env) = 0;
    virtual CTail* econTail() = 0;
    virtual CExp* econAssign() = 0;
    virtual ty typeCheck(TCEnv e) = 0;
    bool operator== (const Expr* rhs){ return AST() == rhs->AST(); }
    virtual bool isNot(){
        return false;
    }
    virtual bool isNeg(){
        return false;
    }
    virtual bool isBool(){
        return false;
    }
    virtual Expr* removeNestedUnary(){
        return this;
    }
    Expr* getE1(){
        return e1_.release();
    }
protected:
    virtual Expr* cloneImpl() const = 0;
    std::unique_ptr<Expr> e1_, e2_;
    ty type_;
private:

};
class Type : public Expr{
public:
    int getValue(){
        return value_;
    }
    void setValue(int i){
        value_ = i;
    }
protected:

    int value_;
};
//int class
class Num : public Type {
public:
    explicit Num(int n) {
        value_ = n;
        type_ = S64;
    };
    explicit Num(Num* n) {
        value_ = n->value_;
    };
    friend Type* numAdd(Type* const l, Type* const r);
    friend Type* numNeg(Type* const l);
    int output(){
        return value_;
    }
    Type* inter(Environ env) { return this; };
    std::string AST()  {
        std::string str = std::to_string(value_);
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
        out.push_back(rcoPair(s, new Num(value_)));
        return out;
    }
    CTail* econTail(){
        return new CRet(this->econAssign());
    }
    CArg* econAssign() override{
        return new CNum(value_);
    }
    ty typeCheck(TCEnv e){
        return type_;
    }
protected:
    Num* cloneImpl() const override {
        return new Num(value_);
    }
    
private:
    void seti(int i){value_=i;};

};

class Var:public Expr{
public:
    Var(std::string name):name_(name){
        type_ =-1;
    };
    Type* inter(Environ env){
        Expr* container;
        try{
            container = env.at(name_);
        }
        catch(std::out_of_range &e){
            std::cerr << "Attemted to read undefined var " << name_ << std::endl;
            throw std::runtime_error("Undefined var read");
        }
        Type* result = container->inter(env);
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
    ty typeCheck(TCEnv e){
        return e[name_];
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
        type_ = S64;
    }
    Type* inter(Environ env) {
        Type *i, *j;
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
            return new Num(inter(env)->getValue());
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
    ty typeCheck(TCEnv e){
        ty l,r;
        l = e1_->typeCheck(e);
        r = e2_->typeCheck(e);
        if(l != type_) throw std::runtime_error("l is wrong type in add");
        if(r != type_) throw std::runtime_error("r is wrong type in add");
        return l;
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
        type_ = S64;
    }
    Type* inter(Environ env) {
        Type* i;
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
        if(e->isNeg()){
            e = e->getE1();
        }
        e1_.reset(e->optE(env));
        if(isPure(env)){
            return new Num(inter(env)->getValue());
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
    ty typeCheck(TCEnv e){
        ty l;
        l = e1_->typeCheck(e);
        if(l != type_) throw std::runtime_error("l is wrong type in neg");
        return l;
    }
    bool isNeg(){
        return true;
    }
    Expr* removeNestedUnary(){
        Expr* e = this;
        if(e1_->isNeg()){
            e = e1_.release();
        }
        return e;
    }
protected:
    Neg* cloneImpl() const override {
            return new Neg((e1_->clone().release()));
    };
private:
};

// Read class
class Read : public Expr {
public:
    explicit Read(bool mode = 0):mode_(mode) {
        type_ = S64; 
    };
    Type* inter(Environ env) {
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
    ty typeCheck(TCEnv e){
        return type_;
    }
protected:
    Read* cloneImpl() const override { return new Read(mode_);};
private:
    bool mode_;
    static int num_;
};


class Let: public Expr{
public:
    Let(std::string var, Expr* exp, Expr* body): Expr(exp, body), var_(var){
        type_ = -1;
    }
    Type* inter(Environ env){
        env[var_] = e1_->inter(env);
        Type* out;
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
        try{
            env.at(var_) = e1;
        }
        catch(std::out_of_range &e){
            env.insert(std::pair<std::string, Expr*>(var_, e1));
        }
        e2 = e2->optE(env);
        e2_.reset(e2);
        if(isPure(env)){
            return new Num(inter(env)->getValue());
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
    ty typeCheck(TCEnv e){
        ty l,r;
        l = e1_->typeCheck(e);
        try{
            e.at(var_) = l;
        }
        catch(std::out_of_range &ex){
            e.insert(std::pair<std::string, ty>(var_, l));
        }
        r = e2_->typeCheck(e);
        type_ = r;
        return r;
    }
protected:
    Let* cloneImpl() const override {
        return new Let(var_,(e1_->clone().release()), (e2_->clone().release()));
    };
private:
    std::string var_;
};

//R2 Definitions
class Bool : public Type{
public:
    Bool(bool v){
        value_ = v;
        type_ = BOOL;
    };
    std::string AST(){
        std::string out;
        if(value_){
            out+="#t";
        }
        else{
            out+="#f";
        }
        return out;
    }
    bool containVar(std::string name){
        return false;
    }
    CExp* econAssign(){
        return NULL;
    }
    CTail* econTail(){
        return NULL;
    }
    Type* inter(Environ env){
        return this;
    }
    bool isPure(Environ env){
        return true;
    }
    Expr* optE(Environ env){
        return NULL;
    }
    std::vector<rcoPair> rco(envmap env){
        return std::vector<rcoPair>();
    }
    Expr* uniquify(envmap* env){
        return NULL;
    }
    ty typeCheck(TCEnv e){
        return type_;
    }
protected:
    Expr* cloneImpl() const{
        return new Bool(value_);
    }
};
class Not : public Expr{
public:
    Not(Expr* l): Expr(l){
        type_ = BOOL;
    };
    std::string AST(){
        std::string out;
        out += "(! ";
        out += e1_->AST();
        out +=")";
        return out;
    }
    bool containVar(std::string name){
        return false;
    }
    CExp* econAssign(){
        return NULL;
    }
    CTail* econTail(){
        return NULL;
    }
    Type* inter(Environ env){
        Type* out = e1_->inter(env);
        out = boolNot(out);
        return out;
    }
    bool isPure(Environ env){
        return e1_->isPure(env);
    }
    Expr* optE(Environ env){
        Expr* e =e1_.release();
        if(e->isNot()){
            e = e->getE1();
            return e->optE(env);
        }        
        e1_.reset(e->optE(env));
        return this;
    }
    std::vector<rcoPair> rco(envmap env){
        return std::vector<rcoPair>();
    }
    Expr* uniquify(envmap* env){
        return NULL;
    }
    ty typeCheck(TCEnv e){
        ty l;
        l = e1_->typeCheck(e);
        if(l != type_) throw std::runtime_error("l is wrong type in Not");
        return l;
    }
    bool isNot(){
        return true;
    }
    Expr* removeNestedUnary(){
        Expr* e = this;
        if(e1_->isNot()){
            e = getE1();
        }
        return e;
    }
protected:
    Expr* cloneImpl() const{
        return new Not(e1_->clone().release());
    }
};
//type determined at runtime
class Cmp : public Expr{
public:
    Cmp(std::string cmp, Expr* l, Expr* r): Expr(l, r){
        type_ = BOOL;
        try{
            cmp_ = cmpOps.at(cmp);
        }
        catch(std::out_of_range &e){
            std::string err("Invalid Comparison Operator ");
            err+=cmp;
            throw std::invalid_argument(err);
        }
    };
    std::string AST(){
        std::string out;
        out += "(";
        out += cmpOut[cmp_];
        out += " ";
        out += e1_->AST();
        out +=" ";
        out += e2_->AST();
        out+= ")";
        return out;
    }
    bool containVar(std::string name){
        return false;
    }
    CExp* econAssign(){
        return NULL;
    }
    CTail* econTail(){
        return NULL;
    }
    Type* inter(Environ env){
        Type* l = e1_->inter(env);
        Type* r = e2_->inter(env);
        switch(cmp_){
            //l
            case 0:
                return new Bool(l->getValue() < r->getValue());
                break;
            //le
            case 1:
                return new Bool(l->getValue() <= r->getValue());
                break;
            //e
            case 2:
                return new Bool(l->getValue() == r->getValue());
                break;
            //ge
            case 3:
                return new Bool(l->getValue() >= r->getValue());
                break;
            //g
            case 4:
                return new Bool(l->getValue() > r->getValue());
                break;
            default:
                throw std::invalid_argument("cmp initialization failed, non accepted comparator");
        }
    }
    bool isPure(Environ env) { return e1_->isPure(env) && e2_->isPure(env); }
    Expr* optE(Environ env){
        if(isPure(env)){
            return new Bool(inter(env)->getValue());
        }
        Expr* e1 = e1_.release(), *e2 = e2_.release();
        e1_.reset(e1->optE(env));
        e2_.reset(e2->optE(env));
        return this;
    }
    std::vector<rcoPair> rco(envmap env){
        return std::vector<rcoPair>();
    }
    Expr* uniquify(envmap* env){
        return NULL;
    }
    ty typeCheck(TCEnv e){
        ty l,r;
        l = e1_->typeCheck(e);
        r = e2_->typeCheck(e);
        if(l != S64) throw std::runtime_error("l is wrong type in Cmp");
        if(r != S64) throw std::runtime_error("r is wrong type in Cmp");
        return type_;
    }
protected:
    Expr* cloneImpl() const{
        return new Cmp(cmpOut[cmp_], e1_->clone().release(), e2_->clone().release());
    }
private:
    int cmp_;
};
//type determined at runtime
class If : public Expr{
public:
    If(Expr* op, Expr* tr, Expr* fa): Expr(tr,fa), b_(op){
        type_ = -1;
    };
    std::string AST(){
        std::string out;
        out += "(if ";
        out += b_->AST();
        out += "\n";
        out += e1_->AST();
        out +="\n else \n";
        out += e2_->AST();
        out+= ")";
        return out;
    }
    bool containVar(std::string name){
        bool v = false;
        v = b_->containVar(name);
        v = v || e1_->containVar(name);
        v = v || e2_->containVar(name);
        return v;
    }
    CExp* econAssign(){
        return NULL;
    }
    CTail* econTail(){
        return NULL;
    }
    Type* inter(Environ env){
        if(b_->inter(env)->getValue() == true){
            return e1_->inter(env);
        }
        return e2_->inter(env);
    }
    bool isPure(Environ env) { return b_->isPure(env) && e1_->isPure(env) && e2_->isPure(env); }
    Expr* optE(Environ env){
        //if b is a not, reverse the order of results
        if(b_->isNot()){
            b_.reset(b_->getE1());
            Expr* temp = e2_.release();
            e2_.reset(e1_.release());
            e1_.reset(temp);
        }
        // if b_ is solvable, inline the correct block
        if(b_->isPure(env)){
            Bool* temp = b_->inter(env);
            Expr* e;
            if(temp->getValue()){
                e = e1_->optE(env);
                return e;
            }else{
                e = e2_->optE(env);
                return e;
            }
        }
        //else, optimize all contained blocks
        Expr* e1 = e1_.release(), *e2 = e2_.release(), *b = b_.release();
        b_.reset(b->optE(env));
        e1_.reset(e1->optE(env));
        e2_.reset(e2->optE(env));
        return this;
    }
    std::vector<rcoPair> rco(envmap env){
        return std::vector<rcoPair>();
    }
    Expr* uniquify(envmap* env){
        return NULL;
    }
    ty typeCheck(TCEnv e){
        ty l,r;
        l = e1_->typeCheck(e);
        r = e2_->typeCheck(e);
        if(l != r) throw std::runtime_error("l is different type from r");
        type_ = l;
        return l;
    }
protected:
    Expr* cloneImpl() const{
        return new If(b_->clone().release(), e1_->clone().release(), e2_->clone().release());
    }
private:
    std::unique_ptr<Expr> b_;
};
//simplifiable functions
Expr* And(Expr* l, Expr* r);
Expr* Or(Expr* l, Expr* r);
Expr* Sub(Expr* l, Expr* r);
Expr* Xor(Expr* l, Expr* r);


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
        TCEnv tce;
        try{
            e_->typeCheck(tce);
        }
        catch(std::runtime_error &e){
            //std::cerr << "type mismatch in program" << std::endl;
            throw e;
        }
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
    void typeCheck(){
        TCEnv e;
        e_->typeCheck(e);
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
xProgram* assign(xProgram* orig, bool moveBias = 1);
xProgram* patch(xProgram* orig);
xProgram* compile(Program* orig);
#endif

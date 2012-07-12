/*
   POET : Parameterized Optimizations for Empirical Tuning
   Copyright (c)  2008,  Qing Yi.  All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 3. Neither the name of UTSA nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific
    prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISEDOF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef POET_AST_H
#define POET_AST_H
#include <string>
#include <map>
#include <list>
#include <vector>
#include <sstream>
#include <iostream>
#include <poet_config.h>
#include <SymbolTable.h>
#include <assert.h>

extern bool debug_xform();
extern "C" void yyerror(const char* msg);

class POETCode;
class POETString;
class POETIconst;
class POETTuple;
class POETList;
class POETAssign;
class CodeVar;
class LocalVar;
class XformVar;
class POETType;
class POETMap;
class POETOperator;
class TupleAccess;
typedef enum {SRC_ANY, 
   SRC_STRING, SRC_ICONST, SRC_LIST, SRC_CVAR, SRC_TUPLE, SRC_MAP,
   SRC_LVAR, SRC_TYPE, SRC_XVAR, SRC_TUPLE_ACCESS, SRC_OP, SRC_ASSIGN, 
   SRC_EVAL, SRC_CTRL, SRC_PARAM_DECL, SRC_TRACE_DECL, 
   SRC_READ_INPUT, SRC_WRITE_OUTPUT, SRC_PROGRAM} POETEnum;

typedef enum {OUTPUT_NO_DEBUG=0, QUOTE_STRING=1, OUTPUT_TRACE_VAL=2, OUTPUT_LIST_BORD=4, DEBUG_NO_VAR=5, DEBUG_OUTPUT=7, OUTPUT_SHORT=8, DEBUG_OUTPUT_SHORT=15, OUTPUT_VAR_VAL=16, OUTPUT_VAR_RESTR=32, DEBUG_VAR_RESTR=39} ASTOutputEnum;

class POETCodeVisitor {
 public:
  virtual void defaultVisit(POETCode*) {};
  virtual void visitString( POETString*) ;
  virtual void visitIconst( POETIconst*) ;
  virtual void visitCodeVar( CodeVar*) ;
  virtual void visitList( POETList*) ;
  virtual void visitAssign(POETAssign*) ;
  virtual void visitTuple( POETTuple*) ;
  virtual void visitMap(POETMap*) ;
  virtual void visitLocalVar(LocalVar*) ;
  virtual void visitXformVar(XformVar*) ;
  virtual void visitType(POETType*);
  virtual void visitOperator(POETOperator*) ;
  virtual void visitTupleAccess(TupleAccess*) ;
  void visit(POETCode* c);
};

class EvaluatePOET;
class FileInfo;
class POETCode {
static int maxID;  /* QY: used to track the id created so far */
  int id; /*QY: an unique number that identifies the AST node */
 public:
  POETCode() { id = maxID++; }
  bool LessThan (const POETCode* other) const { return id < other->id; }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) 
       { return "_"; }
  virtual POETEnum get_enum() const { return SRC_ANY; }
  virtual std:: string get_className() const { return "POETCode"; }
  virtual void visit(POETCodeVisitor* op)  { op->defaultVisit(this); }
  virtual const FileInfo* get_fileinfo() const { return 0; }
};

inline void POETCodeVisitor::visit(POETCode* c) { c->visit(this); }

class POETIconst : public POETCode {
  int val;
  POETIconst(int v) : val(v) {}
 public:
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) { 
     std:: stringstream out; out << val; return out.str();
  }
  virtual POETEnum get_enum() const { return SRC_ICONST; }
  virtual std:: string get_className() const { return "POETIconst"; }
  virtual void visit(POETCodeVisitor* op)  { op->visitIconst(this); }
  int get_val() const { return val; }
  void set_val(int v) { val = v; }
 friend class ASTFactory;
};

class POETString : public POETCode {
  std:: string code;
  POETString(const std:: string& _code) : code(_code) {}
 public:
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) 
         { return (config & QUOTE_STRING)? "\""+code+"\"" : code; }
  virtual std:: string get_className() const { return "POETString"; }
  virtual POETEnum get_enum() const { return SRC_STRING; }
  virtual void visit(POETCodeVisitor* op) { op->visitString(this); }
  std:: string get_content() const { return code; }
  friend class ASTFactory;
};


class LocalVar : public POETCode {
  LvarSymbolTable::Entry entry;
  LocalVar(const LvarSymbolTable::Entry& _entry) : entry(_entry){}
 public:
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) ;
  virtual POETEnum get_enum() const { return SRC_LVAR; }
  virtual std:: string get_className() const { return "Variable"; }
  virtual void visit(POETCodeVisitor* op) { op->visitLocalVar(this); }
  LvarSymbolTable::Entry get_entry() const { return entry; }
  friend class LvarSymbolTable;
};

class POETList : public POETCode {
  POETCode *first;
  POETList *rest;
  unsigned len;
 protected:
  POETList(POETCode* f, POETList* r) : first(f), rest(r) 
    {
      if (rest == 0) len = 1;
      else len = r->size() + 1;
    }
 public:
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) ;
  virtual POETEnum get_enum() const { return SRC_LIST; }
  virtual std:: string get_className() const { return "POETList"; }
  virtual void visit(POETCodeVisitor* op) { op->visitList(this); }
  POETCode* get_first() const { return first; }
  POETList* get_rest() const { return rest; }
  unsigned size() const { return len; }
  friend class ListFactory;
};

class POETInputList : public POETList
{
 public:
  POETInputList(POETCode* f, POETCode* r) 
     : POETList(f,  (r==0)? 0 :
                    (r->get_enum()==SRC_LIST)? static_cast<POETList*>(r)
                   : new POETInputList(r,0)) {}
  virtual std:: string get_className() const { return "POETInputList"; }
};

class CodeVar : public POETCode {
  CvarSymbolTable::Entry entry;
  POETCode *args;
  POETCode* attr;
  CodeVar(const CvarSymbolTable::Entry& _entry,POETCode* _args,POETCode* _attr) 
         : entry(_entry), args(_args), attr(_attr) {} 

  POETCode* invoke_func(const std::string& name, POETCode* args);

 public:
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) ;
  virtual POETEnum get_enum() const { return SRC_CVAR; }
  virtual std:: string get_className() const { return "CodeVar"; }
  virtual void visit(POETCodeVisitor* op) { op->visitCodeVar(this); }
  POETCode* get_codeDef() const;
  CvarSymbolTable::Entry get_entry() const { return entry; }
  POETCode* get_args() const { return args; }
  POETCode* get_attr() const { return attr; }
  void set_attr(); /*QY: copy attribute values to the local symbol table*/

  POETCode* get_static_attr(const std::string& name) const;
  bool check_cond(POETCode* _args) ;
  POETCode* invoke_rebuild(POETCode* _args) ;
  POETCode* invoke_output(POETCode* input) 
       { return  invoke_func("output",input); }

  /*QY: compute and return parsing information*/
  POETCode* get_parseInfo(); 
  friend class CvarTable;
};

class POETTuple : public POETCode {
  std::vector <POETCode*> impl;
  POETTuple(unsigned _size=0) : impl(_size) 
   { for (unsigned i = 0; i < _size; ++i) impl[i]=0; }
 public:
  virtual std::string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT); 
  virtual std:: string get_className() const { return "POETTuple"; }
  virtual POETEnum get_enum() const { return SRC_TUPLE; }
  virtual void visit(POETCodeVisitor* op) { op->visitTuple(this); }

  void get_impl(std::vector<POETCode*> &res) { res = impl; } 
  POETCode* get_entry(unsigned i) const 
     { assert(i < impl.size()); return impl[i]; } 
  unsigned size() const { return impl.size(); }
 friend class PairTable;
 friend class TupleTable;
 friend class TupleFactory;
};

struct POETCodeLessThan
{
 bool operator() (const POETCode* c1, const POETCode *c2) const
{ return c1->LessThan(c2); }
};

class POETMap : public POETCode {
  std::map<POETCode*,POETCode*,POETCodeLessThan> impl;
 public:
  POETMap() {}
  typedef std::map<POETCode*,POETCode*,POETCodeLessThan>::const_iterator const_iterator;
  const_iterator begin() const { return impl.begin(); }
  const_iterator end() { return impl.end(); }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT); 
  virtual std:: string get_className() const { return "POETMap"; }
  virtual POETEnum get_enum() const { return SRC_MAP; }
  virtual void visit(POETCodeVisitor* op) { op->visitMap(this); }
  POETCode* find(POETCode* from) const {
         const_iterator p = impl.find(from);
         if (p == impl.end()) return 0;
         return (*p).second;
     }
  void set(POETCode* from, POETCode* to) 
     { assert(to != 0); impl[from] = to; }
  unsigned size() const { return impl.size(); }
};
class POETCodeArg {
 protected:
  POETCode* args;
 public:
  POETCodeArg(POETCode* a = 0) : args(a) {}
  POETCode* get_args() const { return args; }
  void set_args(POETCode* c) { args = c; }
};

class XformVar : public POETCode {
  XvarSymbolTable::Entry entry;
protected:
  XformVar(const XvarSymbolTable::Entry& _entry) : entry(_entry)  {}
 public:
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) 
    { return entry.get_name()->toString(OUTPUT_NO_DEBUG); }
  virtual POETEnum get_enum() const { return SRC_XVAR; }
  virtual std:: string get_className() const { return "XformVar"; }
  virtual void visit(POETCodeVisitor* op) { op->visitXformVar(this); }

  virtual POETCode* get_config() const { return 0; }
  virtual POETCode* get_args() const { return 0; }

  XvarSymbolTable::Entry get_entry() const { return entry; }
  POETCode* eval(POETCode* args, bool evalArg=true, POETCode* config=0); 
 friend class ASTFactory;
};

class XformVarInvoke : public XformVar, public POETCodeArg {
  POETCode* config;
 public:
  XformVarInvoke(const XvarSymbolTable::Entry& _entry, 
                 POETCode* _config, POETCode* _args) 
     : XformVar(_entry), POETCodeArg(_args), config(_config) 
        { assert(config != 0 || args != 0); }
  virtual std:: string toString(ASTOutputEnum outputConfig=DEBUG_OUTPUT_SHORT) ;
  virtual std:: string get_className() const { return "XformVarInvoke"; }
  virtual POETCode* get_args() const { return POETCodeArg::get_args(); }
  virtual POETCode* get_config() const { return config;} 

  POETCode* eval(bool evalArg = 1); 
};

class POETType : public POETCode
{
  POETTypeEnum t;
 protected:
  POETType(POETTypeEnum _t) : t(_t) {} 
 public:
  virtual POETEnum get_enum() const { return SRC_TYPE; }
  virtual std:: string get_className() const { return "POETType"; }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) { return POETTypeName[t]; }
  virtual void visit(POETCodeVisitor* op) { op->visitType(this); }
  POETTypeEnum get_type() const { return t; }
 friend class ASTFactory;
};

class POETTypeNot : public POETType
{
  POETCode* arg;
  POETTypeNot(POETCode* _arg) : POETType(TYPE_NOT), arg(_arg) {}
 public:
  virtual std:: string get_className() const { return "POETTypeNot"; }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) 
     {  return "~("+arg->toString()+")"; }
  POETCode* get_arg() const { return arg; }
 friend class ASTFactory;
};

class POETOperator : public POETCode
{
  POETOperatorType t;
 protected:
  POETOperator(POETOperatorType _t) : t(_t) {} 
 public:
  virtual POETEnum get_enum() const { return SRC_OP; }
  virtual std:: string get_className() const { return "POETOperator"; }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) ;
  virtual void visit(POETCodeVisitor* op) { op->visitOperator(this); }
  virtual unsigned numOfArgs() const { return 0; }
  virtual POETCode* get_arg(unsigned i) const { return 0; }

  POETOperatorType get_op() const { return t; }
 friend class ASTFactory;
};

class FileInfo {
  std::string fname;
  int lineno;
 public:
  FileInfo(const std::string& f, int i) : fname(f), lineno(i) {}
  const std::string& get_filename() const { return fname; }
  std:: string toString() const {
    std:: stringstream out;
    out << "line " << lineno << " of file " << fname;
    return out.str();
  }
  int get_lineno() const { return lineno; }
};

class POETEvalExp: public POETCode, public FileInfo
{
  POETCode* exp;
 public:
  POETEvalExp(POETCode* _exp, const std::string& fname, int i) 
     : FileInfo(fname,i), exp(_exp) {}
  virtual POETEnum get_enum() const { return SRC_EVAL; }
  virtual std:: string get_className() const { return "POETEvalExp";}
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) 
         { return exp->toString(config); }
  virtual void visit(POETCodeVisitor* op) ;
  virtual const FileInfo* get_fileinfo() const { return this; }
};

class POETUop : public POETOperator
{
  POETCode* arg;
 public:
  POETUop(POETOperatorType _t, POETCode* _arg)
     : POETOperator(_t), arg(_arg) { assert(arg != 0); }
  virtual std:: string get_className() const { return "POETUop"; }
  virtual std:: string toString(ASTOutputEnum config);
  POETCode* get_arg() const { return arg; }
  virtual unsigned numOfArgs() const { return 1; }
  virtual POETCode* get_arg(unsigned i) const { return (i == 0)? arg : 0; }
};

class POETTypeUop : public POETUop
{
  POETTypeUop(POETOperatorType _t, POETCode* _arg) : POETUop(_t,_arg) {}
 public:
  virtual std:: string get_className() const { return "POETTypeUop"; }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) 
     {  switch (get_op())
        { 
          case TYPE_LIST: return "("+get_arg()->toString(config) + ")...";
          case TYPE_LIST1: return "("+get_arg()->toString(config) + ")....";
          default: std::cerr << "unrecognized type: " << get_op() << "\n"; assert(0);
        }
     }

 friend class ASTFactory;
};

template <POETOperatorType op> class BopFactory;
class POETBop : public POETOperator
{
 protected:
  POETCode* arg1, *arg2;
  POETBop(POETOperatorType _t, POETCode* _arg1, POETCode* _arg2)
     : POETOperator(_t), arg1(_arg1),arg2(_arg2) 
      { assert(arg1 != 0 && arg2 != 0); }
  void set_arg1(POETCode* r) { arg1 = r; }
  void set_arg2(POETCode* r) { arg2 = r; }
 public:
  POETCode* get_arg1() const { return arg1; }
  POETCode* get_arg2() const { return arg2; }
  virtual unsigned numOfArgs() const { return 2; }
  virtual std:: string get_className() const { return "POETBop"; }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT);
  virtual POETCode* get_arg(unsigned i) const { 
     switch (i) {
     case 0: return arg1;
     case 1: return arg2;
     default: return 0;
     }
   }
  friend class BopFactory<POET_OP_MAP>;
  friend class ApplyTokenOperator;
  friend class POETProgram;
};

class POETTop : public POETOperator
{
  POETCode* arg1, *arg2, *arg3;
 public:
  POETTop(POETOperatorType _t, POETCode* _arg1, POETCode* _arg2, POETCode* _arg3)
     : POETOperator(_t), arg1(_arg1),arg2(_arg2),arg3(_arg3) 
      { assert(arg1 != 0 && arg2 != 0); }
  POETCode* get_arg1() const { return arg1; }
  POETCode* get_arg2() const { return arg2; }
  POETCode* get_arg3() const { return arg3; }
  virtual unsigned numOfArgs() const { return 3; }
  virtual std:: string get_className() const { return "POETTop"; }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT);
  virtual POETCode* get_arg(unsigned i) const { 
     switch (i) {
     case 0: return arg1;
     case 1: return arg2;
     case 2: return arg3;
     default: return 0;
     }
   }
};

class POETQop : public POETOperator
{
  POETCode* arg1, *arg2, *arg3, *arg4;
 public:
  POETQop(POETOperatorType _t, POETCode* _arg1, POETCode* _arg2, POETCode* _arg3,
          POETCode* _arg4, const std::string& fname, int i) 
     : POETOperator(_t), arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4) {}
  virtual unsigned numOfArgs() const { return 4; }
  virtual std:: string get_className() const { return "POETQop"; }
  virtual POETCode* get_arg(unsigned i) const { 
     switch (i) {
     case 0: return arg1;
     case 1: return arg2;
     case 2: return arg3;
     case 3: return arg4;
     default: return 0;
     }
   }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT);
};

class POETParseList : public POETBop
{
  std::vector<POETCode*> itemFilter;
  unsigned lookahead;
 public:
  POETParseList(POETOperatorType _t, POETCode* arg1, POETCode* arg2) 
    : POETBop(_t,arg1,arg2),lookahead(0) {}
  /*QY: compute lookahead info; return the min len of tokens in lookahead */
  unsigned compute_lookaheadInfo(EvaluatePOET* op, std::vector<POETCode*>& res, 
                            unsigned need); 
  bool match_lookahead(EvaluatePOET* op, POETCode* input);
};

class POETTypeTor : public POETOperator
{
  struct ParseInfo {
      std::vector<POETCode*> filter;
      unsigned lookahead;
      ParseInfo() : lookahead(0) {} 
  };
  std::vector<POETCode*> args;
  std::vector<ParseInfo> parseInfo;
  bool allowEmpty;

  /*QY:compute lookahead for args[index]; 
       return the min len of tokens computed*/
  unsigned compute_lookaheadInfo(EvaluatePOET* op, unsigned index, unsigned  need); 
 public:
  POETTypeTor(POETCode* arg1, POETCode* arg2) 
      : POETOperator(TYPE_TOR),allowEmpty(false) 
       { args.push_back(arg1); args.push_back(arg2); }
  bool operator ==(const POETTypeTor&  that) const { return args == that.args; }
  virtual std:: string get_className() const { return "POETTypeTor"; }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) 
     { 
        std::string res=args[0]->toString();
        for (unsigned i = 1; i < args.size(); ++i)  
           res = res + "|" + args[i]->toString();
        return res;
     }
  virtual unsigned numOfArgs() const { return args.size(); }
  virtual POETCode* get_arg(unsigned i) const { return (i < args.size())? args[i] : 0; }
  void push_back(POETCode* arg) { args.push_back(arg); }

  /*QY: compute lookahead info for all alternatives; 
        return the min len of tokens in the lookahead */
  unsigned compute_lookaheadInfo(EvaluatePOET* op, std::vector<POETCode*>& res, 
                            unsigned need);
  /*QY: return the proper alternative for parsing the given input */
  POETCode* get_parseInfo(EvaluatePOET* op, POETCode* input);
};

class TupleAccess : public POETCode {
   POETCode *tuple, *index;
 public:
  TupleAccess( POETCode* t, POETCode* a) : tuple(t), index(a) {}
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) 
    { 
       std::string r = tuple->toString(config)+"["+index->toString(config)+"]"; 
       return r;
    }
  virtual std:: string get_className() const { return "TupleAccess" ; }
  virtual POETEnum get_enum() const { return SRC_TUPLE_ACCESS; }
  virtual void visit(POETCodeVisitor* op) { op->visitTupleAccess(this); }
  POETCode* get_tuple() const { return tuple; }
  POETCode* get_access() const { return index; }
};

class POETAssign : public POETCode
{
   POETCode* lhs, *rhs;
 public:
  POETAssign(POETCode* l, POETCode* r) : lhs(l), rhs(r) {}
  virtual POETEnum get_enum() const { return SRC_ASSIGN; }
  virtual std:: string get_className() const { return "POETAssign"; }
  virtual void visit(POETCodeVisitor* op) 
         { op->visitAssign(this); }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) {
      return  lhs->toString(config) + "=" + rhs->toString(config);
    }
  POETCode* get_lhs() const { return lhs; }
  POETCode* get_rhs() const { return rhs; }
};

inline void POETCodeVisitor::visitString(POETString* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitIconst(POETIconst* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitCodeVar(CodeVar* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitLocalVar(LocalVar* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitAssign(POETAssign* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitXformVar(XformVar* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitType(POETType* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitTuple(POETTuple* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitMap(POETMap* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitTupleAccess(TupleAccess* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitOperator(POETOperator* c) 
   {defaultVisit(c); }
inline void POETCodeVisitor::visitList(POETList* c) 
   { defaultVisit(c); }

class POETTraceDecl : public POETCode, public FileInfo
{
  LocalVar* var;
 public:
  POETTraceDecl(LocalVar* _var, const std::string& fname, int i) 
     : FileInfo(fname,i),var(_var) {}
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) ;
  virtual POETEnum get_enum() const { return SRC_TRACE_DECL; }
  virtual std:: string get_className() const { return "POETTraceDecl"; }
  virtual void visit(POETCodeVisitor* op) { op->defaultVisit(this); }
  virtual const FileInfo* get_fileinfo() const { return this; }
  LocalVar* get_trace() const { return var; }
};

class ParameterDecl : public POETCode, public FileInfo
{
  LocalVar* var;
  POETCode* parse, *msg, *init;
 public:
  ParameterDecl(LocalVar* _var, const std::string& fname, int i) 
     : FileInfo(fname,i),var(_var), parse(0), msg(0), init(0) {}
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT); 
  std::string get_name() const { return var->get_entry().get_name()->toString(OUTPUT_NO_DEBUG); }
  LocalVar* get_var() const { return var; }
  POETCode* get_parse() const { return parse; }
  void set_parse(POETCode* val) { parse=val; }
  void set_message(POETCode* val) { msg=val; }
  POETCode* get_message() const { return msg; }
  void set_default(POETCode* val) { init=val; }
  POETCode* get_default() const { return init; }
  void set_type(POETCode* val) { var->get_entry().set_restr(val); }
  POETCode* get_type() const { return var->get_entry().get_restr(); }
  void set_val(POETCode* val, bool doparse);

  virtual POETEnum get_enum() const { return SRC_PARAM_DECL; }
  virtual std:: string get_className() const { return "Parameter"; }
  virtual void visit(POETCodeVisitor* op) { op->defaultVisit(this); }
  virtual const FileInfo* get_fileinfo() const { return this; }
};

class ReadInput : public POETCode, public FileInfo
{
  POETCode* debug, *cond, *annot, *target, *type, *syntaxFiles, *inputFiles, *inlineInput;
 public:
  ReadInput(const std::string& fname, int i) 
     : FileInfo(fname,i),debug(0),cond(0),annot(0), target(0),type(0),
       syntaxFiles(0), inputFiles(0), inlineInput(0) { }
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT);
  POETCode* get_annot() const { return annot; }
  void set_annot(POETCode* val) { annot=val; }
  void set_cond(POETCode* val) { cond=val; }
  POETCode* get_cond() const { return cond; }
  void set_debug(POETCode* val) { debug=val; }
  POETCode* get_debug() const { return debug; }
  void set_var(POETCode* var) { target = var; }
  POETCode* get_var() const { return target; }
  void set_syntaxFiles(POETCode* code) { syntaxFiles = code; }
  POETCode* get_syntaxFiles() const { return syntaxFiles; }
  void set_inputFiles(POETCode* files) { inputFiles = files; }
  POETCode* get_inputFiles() const { return inputFiles; }
  void set_type(POETCode* codeType) { type = codeType; }
  POETCode* get_type() const { return type; }
  void set_inputInline(POETCode* input) { inlineInput = input; }
  POETCode* get_inputInline() const { return inlineInput; }
  std::string fileinfo_string() const { return FileInfo::toString(); }

  virtual POETEnum get_enum() const { return SRC_READ_INPUT; }
  virtual std:: string get_className() const { return "ReadInput"; }
  virtual void visit(POETCodeVisitor* op) { op->defaultVisit(this); }
  virtual const FileInfo* get_fileinfo() const { return this; }
};

class WriteOutput : public POETCode, public FileInfo
{
  POETCode* outputFile, *outputExp, *syntaxFiles, *cond;
 public:
  WriteOutput(const std::string& fname, int i)
    : FileInfo(fname,i),outputFile(0), outputExp(0), syntaxFiles(0), cond(0) {}
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT); 
  void set_cond(POETCode* val) { cond=val; }
  POETCode* get_cond() const { return cond; }
  POETCode* get_syntaxFiles() const { return syntaxFiles; }
  POETCode* get_outputFile() const { return outputFile; }
  POETCode* get_outputExp() const { return outputExp; }
  void set_outputFile(POETCode* file) { outputFile = file; }
  void set_syntaxFiles(POETCode* code) { syntaxFiles = code; }
  void set_outputExp(POETCode* exp) { outputExp = exp; }

  virtual POETEnum get_enum() const { return SRC_WRITE_OUTPUT; }
  virtual std:: string get_className() const { return "WriteOutput"; }
  virtual void visit(POETCodeVisitor* op) { op->defaultVisit(this); }
  virtual const FileInfo* get_fileinfo() const { return this; }
};

class POETProgram : public POETCode
{
  static LvarSymbolTable traceDef, paramDef, macroDef;
  LvarSymbolTable evalDef;
  static bool backtrack;
  struct LocalVarSave 
    { 
       LocalVar* lvar; POETCode* restr;
       LocalVarSave() : lvar(0),restr(0) {}
       LocalVarSave(LocalVar* _lvar, POETCode* _restr)
        : lvar(_lvar), restr(_restr) {} 
    };
  struct CodeDefSave 
    { 
      POETCode *code_def, *parse;
      std::vector<LocalVarSave> par_type; 
      LocalVarSave unparse;
      CodeDefSave() : code_def(0), parse(0) {}
    };

  std::list<POETCode*> define;
  std::list<POETProgram*> includeFiles;
  std::map<CodeVar*, CodeDefSave > codeDef;
  std::string filename, ext;
  bool done_save;

  LocalVarSave prep_save, unparse_save, token_save, parse_save;
 public:
  typedef std::list<POETCode*>::const_iterator const_iterator;
  const_iterator begin() { return define.begin(); }
  const_iterator end() { return define.end(); }
  unsigned size() const { return define.size(); }
  LvarSymbolTable* get_evalTable() { return &evalDef; }
 
  POETProgram(const std::string& fname) : filename(fname),done_save(false) 
    {  size_t pos = filename.rfind("."); 
       if (pos < filename.size()) 
          ext = filename.substr(pos, filename.size()-pos+1); 
    } 

  static POETString* make_string(const std::string& r);
  static POETIconst* make_Iconst(int val);
  static POETOperator* make_dummyOperator(POETOperatorType t);
  static POETCode* make_empty();
  static POETCode* make_any() ;
  static POETCode* make_rangeType(POETCode* lb, POETCode* ub);
  static POETCode* make_listType(POETCode* elemType);
  static POETCode* make_atomType(POETTypeEnum t);
  static POETCode* make_pair(POETCode* r1, POETCode* r2);
  static POETCode* append_tuple(POETCode* r1, POETCode* r2);
  static POETCode* make_tuple3(POETCode* r1, POETCode* r2, POETCode* r3);
  static POETCode* make_tuple4(POETCode* r1, POETCode* r2, POETCode* r3, POETCode* r4);
  static POETCode* make_tuple5(POETCode* r1, POETCode* r2, POETCode* r3, POETCode* r4, POETCode* r5);
  static POETCode* make_tuple6(POETCode* r1, POETCode* r2, POETCode* r3, POETCode* r4, POETCode* r5, POETCode* r6);
  static POETCode* make_list(POETCode* head, POETCode* tail);

  static XformVar* make_xformVar(POETCode* name);
  static CodeVar* make_codeRef(POETCode* name, POETCode* arg=0);
  static LocalVar* make_localVar(LvarSymbolTable* symTab,POETCode* name,LocalVarType vartype);
  static POETCode* make_attrAccess(POETCode* scope, POETCode* name);
  static POETCode* make_Iconst( char* text, int len, bool parseInput);
  static LocalVar* find_macroVar(POETCode* name) { return macroDef.find(name); }
  static LocalVar* make_macroVar(POETCode* name) 
      { return make_localVar(&macroDef, name, LVAR_TUNE); }
  static LocalVar* find_paramVar(POETCode* name) { return paramDef.find(name); }
  static LocalVar* make_paramVar(POETCode* name) 
      { return make_localVar(&paramDef, name, LVAR_TUNE); }
  static LocalVar* find_traceVar(POETCode* name) { return traceDef.find(name); }
  static LocalVar* make_traceVar(POETCode* name) 
      { return make_localVar(&traceDef, name, LVAR_TRACE); }
  static POETCode* make_Uop( POETOperatorType t, POETCode* arg, int lineno = -1);
  static POETCode* make_Top( POETOperatorType t, POETCode* arg1, POETCode* arg2, POETCode* arg3, int lineno = -1);
  static POETCode* make_typeTor(POETCode* arg1, POETCode* arg2);

  std::list<POETProgram*>& get_includeFiles() { return includeFiles; }
  void insert_includeFile(POETProgram* incl) { includeFiles.push_back(incl); }

  LocalVar* find_evalVar(POETCode* name) { return evalDef.find(name); }
  LocalVar* make_evalVar(POETCode* name) 
      { return make_localVar(&evalDef, name, LVAR_REG); }

  POETCode* make_tupleAccess( POETCode* f, POETCode* args);
  POETCode* make_Bop( POETOperatorType t, POETCode* arg1, POETCode* arg2, int lineno=-1);

  ParameterDecl* insert_parameterDecl(POETCode* par, int lineno);
  void insert_traceDecl(LocalVar* traceVars, int lineno);
  void insert_codeDecl(CodeVar* cvar); 
  XformVar* insert_xformDecl(POETCode* name);
  ReadInput* insert_inputDecl(int lineno); 
  WriteOutput* insert_outputDecl(int lineno);
  void insert_evalDecl(POETCode* code, int lineNo);
  void insert_sourceCode(POETCode* code) /* input source;not POET code*/
      { define.push_back(code); }
  void insert_define(LocalVar* var, POETCode* code); /* no evaluation */ 
  void eval_define(LocalVar* var, POETCode* code); /* evaluate the instr */

  void set_codeDef(CodeVar* cvar, POETCode * code);
  LocalVar* set_local_static(LvarSymbolTable* local, POETCode* id, 
                 LocalVarType t, POETCode* code, POETCode * restr, bool insert);
  void set_xformDef(XformVar* xvar, POETCode * code);
  /* remove all the syntax/xform definitions from global scope */
  void clear_syntax();
  void set_syntax();

  std::string get_filename() { return filename; }
  std::string get_file_ext() const { return ext; }
  bool get_backtrack() const { return backtrack; }
  void write_to_file( std::ostream& out);
  virtual std:: string toString(ASTOutputEnum config=DEBUG_OUTPUT_SHORT) 
     { return "Program " + get_filename(); }
  virtual POETEnum get_enum() const { return SRC_PROGRAM; }
  virtual std:: string get_className() const { return "POETProgram"; }
  virtual void visit(POETCodeVisitor* op) { op->defaultVisit(this); }
};

/*QY: utilities for examining POET AST (no evaluation) */

#define IS_SPACE(c)  c == " " || c == "\t" || c == "\n"

inline POETCode* EvalTrace(POETCode* var) {
      assert(var != 0);
      while (var->get_enum() == SRC_LVAR) {
         LvarSymbolTable::Entry e =  static_cast<LocalVar*>(var)->get_entry();
         if (e.get_entry_type() != LVAR_TRACE)  return var;
         POETCode* code =  e.get_code();
         if (code == 0 || code == var) return var;
         var = code;
      }
      return var;
   }

class VisitNestedTraceVars
{
  virtual void preVisitTraceVar(LocalVar* v) {}
  virtual void postVisitTraceVar(LocalVar* v) {}
 public:
  void apply( LocalVar* traceVar) {
     assert(traceVar->get_entry().get_entry_type() == LVAR_TRACE);
     preVisitTraceVar(traceVar);

     LvarSymbolTable::Entry entry = traceVar->get_entry();
     POETCode* next = entry.get_restr();
     if (next != 0) {
         LocalVar* nv = dynamic_cast<LocalVar*>(next);
         assert(nv != 0);
         apply(nv);
     }
     postVisitTraceVar(traceVar);
  }
};

class SaveNestedTraceVars : public VisitNestedTraceVars
{
  virtual void preVisitTraceVar(LocalVar* v) { v->get_entry().push(true); }
};
class RestoreNestedTraceVars : public VisitNestedTraceVars
{
  virtual void preVisitTraceVar(LocalVar* v) { v->get_entry().pop(); }
};

int inline AST2Bool(POETCode* r) 
{   r = EvalTrace(r); 
  if (r->get_enum() == SRC_ICONST) { 
     int v = static_cast<POETIconst*>(r)->get_val(); 
     return v; 
  } 
  return (r->toString(OUTPUT_NO_DEBUG) != ""); 
} 

inline POETIconst* string2Iconst(const char* text, int len) 
{ 
  if (len == 0) return 0; 
  int val = 0, i = 0; 
  const char* p = text;  
  if (*p == '-') { ++p; ++i; } 
  if (i == len) return 0; 
  for (; i < len; ++p,++i) { 
    if ((*p) < '0' || (*p) > '9')  
       return 0; 
    val = val * 10 + ((*p)-'0'); 
  }  
  if (*text == '-') val = -val; 
  return POETProgram::make_Iconst(val); 
} 

#endif

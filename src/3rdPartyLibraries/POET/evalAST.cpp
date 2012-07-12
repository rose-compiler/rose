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

/********************************************************************
  Routines for implementing code generation from POET AST
********************************************************************/

#include <fstream>
#include <iostream>
#include <set>
#include <list>
#include <error_config.h>
#include <poetAST.h>
#include <ASTvisitor.h>
#include <ASTeval.h>
#include <assert.h>
#include <string>
#include <cstdlib>
#include <timing.h>

#define SYSVARS_MAX 25536
static LvarSymbolTable sysVars;
static int sysVars_index = 0;

extern bool debug_time(); 
extern bool redefine_code;
inline bool allow_redefine_code() { return redefine_code; }

extern POETProgram* curfile;
extern std::list<std::string> lib_dir;

extern "C" LexState lexState;
extern "C" int yylineno;

POETCode*  EvaluatePOET::lp = 0;
POETCode*  EvaluatePOET::rp = 0;
POETCode*  EvaluatePOET::lb = 0;
POETCode*  EvaluatePOET::rb = 0;
POETCode*  EvaluatePOET::space = 0;
POETCode*  EvaluatePOET::linebreak = 0;
POETCode*  EvaluatePOET::tab = 0;
POETCode*  EvaluatePOET::comma = 0;
ASTFactory* EvaluatePOET::fac=0;
LocalVar* EvaluatePOET::exp_item= 0;
LocalVar* EvaluatePOET::exp_bop= 0;
LocalVar* EvaluatePOET::exp_uop= 0;
LocalVar* EvaluatePOET::tokens = 0;
LocalVar*  EvaluatePOET::funcall = 0;
LocalVar*  EvaluatePOET::keywords = 0;
LocalVar*  EvaluatePOET::arrref = 0;
LocalVar*  EvaluatePOET::parseBop = 0;
LocalVar*  EvaluatePOET::buildBop = 0;
LocalVar*  EvaluatePOET::parseUop = 0;
LocalVar*  EvaluatePOET::buildUop = 0;
LocalVar*  EvaluatePOET::prep= 0;
LocalVar*  EvaluatePOET::parseTarget= 0;
LocalVar*  EvaluatePOET::unparseTarget= 0;

class GetLenVisitor : public POETCodeVisitor
{
  unsigned res;
 public:
  virtual void defaultVisit(POETCode*) { res = 1; };
  virtual void visitString(POETString* s) 
     { res = s->get_content().size(); }
  virtual void visitList(POETList* l) 
     {  res = l->size(); }
  virtual void visitTuple( POETTuple* v) 
     { res = v->size(); } 
  virtual void visitMap( POETMap *m)
     { res = m->size(); }
 public:
  unsigned apply(POETCode* code) { res = 1; code->visit(this);  return res; }
};

void ReadFiles(POETCode* _files, LexState _lexStateStart, std::list<POETProgram*>& resultFiles)
{
   POETCode* files = eval_AST(_files);
   POETCode* p_files=files;
   while (p_files != 0) {
       POETList* fileList = dynamic_cast<POETList*>(p_files);
       POETCode* fileCur = p_files;
       if (fileList != 0) {
              fileCur = fileList->get_first();
              p_files=fileList->get_rest();
        }
       else  p_files = 0;
       
       std::string fname= fileCur->toString(OUTPUT_NO_DEBUG); 
       if (fname == "") {
          std::cerr << "Empty file name: " << fileCur->toString() << " from list " << files->toString() << "\n";
       } 
       lexState = _lexStateStart;
       POETProgram* programFile =  process_file(fname.c_str());
       resultFiles.push_back(programFile);
       lexState=LEX_DEFAULT;
    }
}

class ApplyTokenOperator : public ReplInfoVisitor
{
  POETCode* tokens;
 public: 
  ApplyTokenOperator(POETCode* _t) : tokens(_t) { assert(tokens!=0); }
  virtual void visitList(POETList* l)
   { 
     ReplInfoVisitor::visitList(l);
     if (res->get_enum() == SRC_LIST)
       res = apply_tokens(static_cast<POETList*>(res)); 
     if (res == 0) res = EMPTY;
   }
   virtual void visitCodeVar( CodeVar* v) { res = v; }
   virtual void visitOperator(POETOperator *op)
   {
    if (op->get_op() == POET_OP_TYPEMATCH || op->get_op() == POET_OP_TYPEMATCH_Q) 
    {
       POETBop* bop = dynamic_cast<POETBop*>(op);
       assert(bop != 0);
       POETCode* arg = bop->get_arg1();
       arg->visit(this); 
       if (arg != res) bop->set_arg1(res);
       res = op;
     }
   }
};

POETCode* EvaluatePOET::parse_input(POETCode* input, POETCode* pattern)
{
   static double token_time = 0, prep_time=0, parse_time=0;
   static bool  first=true;

   bool dt = debug_time();
if (dt) {
 if (first) {
    register_timing(&token_time, "time spent in tokenizer:");
    register_timing(&prep_time, "time spent in parse preparation:");
    register_timing(&parse_time, "time spent in parsing and AST construction:");
 }
first=false;
}

   if (prep->get_entry().get_code() != 0) {
      double cur = (dt?GetWallTime():0);
      XformVar* prep_xform = dynamic_cast<XformVar*>(prep->get_entry().get_code());
      if (prep_xform == 0) INCORRECT_XVAR(prep);
      input = prep_xform->eval(input, false);
      if (dt) prep_time +=GetWallTime()-cur;
   }
   if (tokens->get_entry().get_code() != 0) {
      double cur = (dt?GetWallTime():0);
      ApplyTokenOperator op(tokens->get_entry().get_code());
      input = op.apply(input);
      if (dt) token_time +=GetWallTime()-cur;
   }
   input = eval_AST(input);
   if (pattern == 0) pattern = parseTarget->get_entry().get_code();
   if (pattern == 0) return input; 
   switch (pattern->get_enum())
    {
    case SRC_READ_INPUT: return input;
    default:
       try { 
          double cur = (dt?GetWallTime():0);
          POETCode* result = parse_AST(input, pattern);
          if (dt) parse_time +=GetWallTime()-cur;
            return result;
          }
       catch (ParseError err) { EXIT(err.message()); }
   }
}


class InsertTraceInside : public VisitNestedTraceVars, public ReplInfoVisitor
{
  std::vector<LocalVar*> traceVars;
  int curVar;
  virtual void preVisitTraceVar(LocalVar* v) 
  {
     POETCode* content = v->get_entry().get_code() ;
     if (content != 0 && content != EMPTY)  
            traceVars.push_back(v);
  }

  virtual void defaultVisit(POETCode* val) 
   {
     int size = traceVars.size();
     LocalVar *v = traceVars[curVar];
     if (curVar >= size || EvalTrace(v) != val) { return; }
     res = v;

     while (++curVar < size) {
        LocalVar* trace = traceVars[curVar];
        if (EvalTrace(trace) != val)  return;
        if (trace != v) {
            v->get_entry().set_code(trace);
            v = trace; 
        }
     }
   }
  virtual void visitLocalVar( LocalVar* v) {
     if (curVar >= traceVars.size()) return;
     
     POETCode* val = EvalTrace(v);
     res = 0; defaultVisit(val);
     if (res == 0) { res = v; }
     else if (res != v)  {
        v->get_entry().set_code(res);
        res = v;
     }
     ReplInfoVisitor::visitLocalVar(v); 
  }
  virtual void visitList(POETList* l) {
     if (curVar >= traceVars.size()) return;
     res = 0; defaultVisit(l);
     if (res == 0) 
         ReplInfoVisitor::visitList(l);
     else res->visit(this);
  }
  virtual void visitTuple( POETTuple* v)  {
     if (curVar >= traceVars.size()) return;
     res = 0; defaultVisit(v);
     if (res == 0)
        ReplInfoVisitor::visitTuple(v);
     else res->visit(this);
  }
  virtual void visitCodeVar( CodeVar* v) { 
     if (curVar >= traceVars.size()) return;
     res = 0; defaultVisit(v);
     if (res == 0) 
         ReplInfoVisitor::visitCodeVar(v);
     else res->visit(this);
   }
 public:
  POETCode* apply(LocalVar* traceVar, POETCode* res)
  {
   VisitNestedTraceVars::apply(traceVar);
   curVar=0;
   ReplInfoVisitor::apply(res);

   if (curVar < traceVars.size())
         TRACE_INCOMPLETE(traceVars[curVar]);
   return res;
 }
};

POETCode* EraseTrace(POETCode* vars, POETCode* res)
{
  std::vector<LocalVar*> vec;
  switch (vars->get_enum()) {
  case SRC_LVAR: {
     vars = eval_AST(vars);
     if (vars->get_enum() != SRC_LVAR) break;
     LocalVar* lvar = static_cast<LocalVar*>(vars);
     if (lvar->get_entry().get_entry_type() == LVAR_TRACE)
        vec.push_back(lvar);
     break;
  }
 case SRC_LIST: {
   POETList* l = static_cast<POETList*>(vars);
   while (l != 0) {
     POETCode* cur = l->get_first();
     if (cur->get_enum() == SRC_LVAR) {
        cur = eval_AST(cur);
        if (cur->get_enum() == SRC_LVAR) {
           LocalVar* cur_lvar = static_cast<LocalVar*>(cur);
           if (cur_lvar->get_entry().get_entry_type() == LVAR_TRACE) 
                vec.push_back(cur_lvar);
        }
     }
     l = dynamic_cast<POETList*>(l->get_rest());
   }
  }
  break;
 default: return res;
 }
 int size = vec.size();
 if (size == 0) return res;
 for (int i = size-1; i >= 0; --i) {
   LvarSymbolTable::Entry e = vec[i]->get_entry();
   assert (e.get_entry_type() == LVAR_TRACE); 
   e.set_entry_type(LVAR_TRACE_OUTDATE);
 }
 res = ReplInfoVisitor().apply(res);
 for (int i = size-1; i >= 0; --i) {
   LvarSymbolTable::Entry e = vec[i]->get_entry();
   e.set_entry_type(LVAR_TRACE);
 }
 return res;
}

POETCode* TraceEval(POETCode* vars, POETCode* res)
{
  std::vector<LocalVar*> vec;
  switch (vars->get_enum()) {
  case SRC_LVAR: {
     LocalVar* lvar = static_cast<LocalVar*>(vars);
     vec.push_back(lvar);
     break;
  }
 case SRC_LIST: {
   POETList* l = static_cast<POETList*>(vars);
   while (l != 0) {
     POETCode* cur = l->get_first();
     if (cur->get_enum() != SRC_LVAR) INCORRECT_TRACE_HANDLE(cur->toString());
     vec.push_back(static_cast<LocalVar*>(cur));
     l = dynamic_cast<POETList*>(l->get_rest());
   }
  } 
   break;
 default: INCORRECT_TRACE_HANDLE(vars->toString()); 
 }
 for (int i = vec.size()-1; i >= 0; --i) {
    LocalVar* lvar = vec[i];
    LvarSymbolTable::Entry entry = lvar->get_entry();
    entry.push(true);
    entry.set_entry_type(LVAR_TRACE);
 }
 res = eval_AST(res);
 for (int i = vec.size()-1; i >= 0; --i) {
    LocalVar* lvar = dynamic_cast<LocalVar*>(vec[i]);
    assert(lvar != 0);
    LvarSymbolTable::Entry entry = lvar->get_entry();
    POETCode* val = entry.get_code();
    entry.pop();
    entry.set_code(val);
 }
 return res;
}

inline POETCode* 
eval_tuple_access(POETCode* tuple, int i)
{
  POETTuple* tuple1 = dynamic_cast<POETTuple*>(tuple);
  if (tuple1==0)
     { if (i == 0) return tuple; }
  else {
     if (i >= 0 && i < tuple1->size()) 
        return tuple1->get_entry(i);
  }
  return 0;
}

POETCode* eval_tuple_access(POETCode* tuple, POETCode* access)
{
  if (access == 0 || access->get_enum() != SRC_ICONST) 
      return 0;
  int i = static_cast<POETIconst*>(access)->get_val();
  return eval_tuple_access(tuple,i);
}

class XformEvalVisitor : public ReplInfoVisitor
{
 protected:
  virtual void defaultVisit(POETCode* c) { res = c; }
  virtual void visitLocalVar(LocalVar* v) { 
       if (v->get_entry().get_entry_type() == LVAR_TRACE)
         res = v;
       else {
          res = v->get_entry().get_code(); 
          if (res == 0) 
             { SYM_UNDEFINED(v->toString()); }
          if (res != v) {
             while (res->get_enum() == SRC_LVAR) {
                LvarSymbolTable::Entry e = static_cast<LocalVar*>(res)->get_entry();
                if (e.get_entry_type() == LVAR_TRACE) break;
                POETCode* code = e.get_code();
                if (code == 0) SYM_UNDEFINED(res->toString());
                if (code == res) break;
                res = code;
             }
          }
       }
  }
  virtual void visitOperator(POETOperator* op); 
  virtual void visitAssign(POETAssign* assign) 
     { 
       POETCode* r1 = assign->get_lhs(), *r2 = assign->get_rhs();
     try {
       r2 = apply(r2); 
       if (r2 == 0) r2 = EMPTY;
       res = assign_AST(r2, r1);
     }
      catch (Error err) { std::cerr << "From invoking " << SHORT(assign->toString(DEBUG_NO_VAR),500) << "\n"; throw err; }
      if (user_debug) {
        std::cerr << assign->toString() << "\n===>" << res->toString() << "\n";
     }
    }

    POETCode* eval_cvar_attr(CodeVar* cv, LocalVar* lv)
    {
      CvarSymbolTable::Entry cv_e = cv->get_entry();
      LvarSymbolTable* symTable = cv_e.get_symTable();
      LvarSymbolTable::Entry e = lv->get_entry();
      if (symTable->find(e.get_name()) != lv) return 0;

      POETCode* res = 0;
      switch (e.get_entry_type()) {
       case LVAR_CODEPAR:
           res = eval_tuple_access(cv->get_args(), e.get_code()); 
           break;
       case LVAR_ATTR:
          for (int i = 0; i < cv_e.attr_size(); ++i) {
             if (cv_e.get_attr(i) == lv) { 
                POETCode* tuple = cv->get_attr();
                if (tuple == 0) res = e.get_code();
                else res = eval_tuple_access(tuple,i);
                break;
             }
          }
          break;
       default: /*QY: access is a local variable. treat this as tuple access*/
           res = eval_tuple_access(cv, lv);
      }
      return res;
   }

  POETCode* eval_cvar_access(CodeVar* cv, POETCode* access)
  {
   POETCode* res = 0;
   CvarSymbolTable::Entry cv_e = cv->get_entry();
   LvarSymbolTable* symTable = cv_e.get_symTable();

   symTable->push_table(true); /* push symbol table for evaluation of access*/
   if (access->get_enum() != SRC_LVAR) 
      access= apply(access);
   switch (access->get_enum()) {
   case SRC_LVAR: {
      cv->get_entry().get_symTable()->pop_table();
      LocalVar* lv = static_cast<LocalVar*>(access);
      res = eval_cvar_attr(cv, lv);
      if (res == 0) { CVAR_ACC_MISMATCH( cv,access); }
      break;
   }
   case SRC_OP: {
     POETOperator* op = static_cast<POETOperator*>(access);
     if (op->get_op() == TYPE_TOR) {
        cv->get_entry().get_symTable()->pop_table();
        for (int i = 0; i < op->numOfArgs(); ++i) {
            POETCode* cur = apply(op->get_arg(i));
            LocalVar* lv = dynamic_cast<LocalVar*>(cur);
            if (lv == 0) { CVAR_ACC_MISMATCH( cv,access); }
            res = eval_cvar_attr(cv, lv);
            if (res != 0) return res;
        }
        CVAR_ACC_MISMATCH( cv,access); 
     }
   }
   default:
      if (cv->get_args() != 0) CVAR_ACC_MISMATCH( cv, access); 
       /*QY: access is used to set optional attribute values of code template;
            return a new code template object with the given attribute*/
      res = ASTFactory::inst()->build_codeRef(cv->get_entry(),0);
      cv->get_entry().get_symTable()->pop_table();
      break;
   }
   return res;
  }
  virtual void visitTupleAccess(TupleAccess* fc) 
     { 
      try {
       POETCode* tuple = apply(fc->get_tuple());
       POETCode* tuple1 = EvalTrace(tuple);
       if (tuple1 == 0) SYM_UNDEFINED(tuple->toString())
       else tuple = tuple1;
       POETCode* access= EvalTrace(fc->get_access()); 
       switch (tuple->get_enum()) {
         case SRC_TUPLE: {
            access= apply(access);
            if (access->get_enum() == SRC_LVAR) {
              LvarSymbolTable::Entry e = static_cast<LocalVar*>(access)->get_entry();
              if (e.get_entry_type() == LVAR_OUTPUT) access = e.get_code(); 
              else TUPLE_ACC_MISMATCH(fc, tuple,access);
            }
            res = eval_tuple_access(tuple,access);
            if (res == 0) { TUPLE_ACC_MISMATCH(fc, tuple,access); }
            return;
          }
         case SRC_MAP: {
            POETMap* m = static_cast<POETMap*>(tuple);
            access= apply(access);
            res = m->find(access);
            if (res == 0) 
                res = EMPTY;
            return;
         }
         case SRC_CVAR: {
            CodeVar* cv = static_cast<CodeVar*>(tuple);
            CvarSymbolTable::Entry cv_e = cv->get_entry();
            LvarSymbolTable* symTable = cv_e.get_symTable();
            res = eval_cvar_access(cv, access);
            return;
         }
         default: { 
            res = eval_tuple_access(tuple,access);
            if (res == 0)
             TUPLE_ACC_MISMATCH( fc, tuple, access); 
           }
        }
      }
      catch (Error err) { std::cerr << "From invoking " << SHORT(fc->toString(),500) << "\n"; throw err; }
     } 
  virtual void visitXformVar(XformVar* v) 
     { 
       XformVarInvoke *invoke = dynamic_cast<XformVarInvoke*>(v);
       if (invoke == 0) { res = v; return; }
       int userDebugSave = user_debug;
       int isInDebug = user_debug || debug_xform();
       if (isInDebug > 0) {
           user_debug = userDebugSave-1;
           if (user_debug) {
              std:: cerr << "Invoking " << SHORT(v->toString(DEBUG_NO_VAR),500) << "\n";
           }
       }
       res = invoke->eval(); 
       if (isInDebug) {
           std:: cerr << "Result of invoking " << SHORT(v->toString(DEBUG_NO_VAR),500) << "\n";
           std:: cerr << "===>" << res->toString() << "\n";
           user_debug = userDebugSave;
       }
    }
 public:
  XformEvalVisitor() {}
  POETCode* apply(POETCode* code) 
   { 
      POETList* l = dynamic_cast<POETList*>(code);
      if (l != 0 && l->get_rest() == 0)
        code = l->get_first();
      res = 0; code->visit(this); return res; 
   } 
};

POETCode* eval_AST(POETCode* code) {
  XformEvalVisitor xform;
  POETCode* r = xform.apply(code);
  return r;
}

void XformEvalVisitor::visitOperator(POETOperator* op) 
   {
     POETCode *r1 = op->get_arg(0), *r2 = op->get_arg(1), *r3 = op->get_arg(2), *r4 = op->get_arg(3);
     switch(op->get_op()) {
     case POET_OP_IFELSE: case POET_OP_CASE: case POET_OP_FOR: case POET_OP_FOREACH: case POET_OP_FOREACHR: case POET_OP_CONTINUE: case POET_OP_BREAK: case POET_OP_ERROR:
     case POET_OP_CLEAR: case POET_OP_ASTMATCH: case POET_OP_TYPEMATCH: case POET_OP_ANNOT: case POET_OP_TYPEMATCH_Q:
     case POET_OP_TRACE: case POET_OP_DELAY: case POET_OP_AND: case POET_OP_TUPLE: case POET_OP_LIST: case POET_OP_LIST1: 
     case POET_OP_DEBUG: case POET_OP_PRINT: case POET_OP_OR:  case POET_OP_ERASE:
           break;
     default:
        if (r1 != 0) { r1->visit(this); r1 = res; }
        if (r2 != 0) { r2->visit(this); r2 = res; }
        if (r3 != 0) { r3->visit(this); r3 = res; }
        if (r4 != 0) { r4->visit(this); r4 = res; }
     }

     try {
     switch (op->get_op()) {
     case POET_OP_VAR: 
     case POET_OP_XFORM:
     case POET_OP_CODE:
     case POET_OP_EXP:
         assert (r1 == 0); 
     case TYPE_LIST: case TYPE_LIST1: case TYPE_TOR: 
     case POET_OP_RANGE:
     case POET_OP_TUPLE:
     case POET_OP_LIST:
     case POET_OP_ANNOT:
         res = op; return;
     case POET_OP_PRINT: 
       assert(r1 != 0);
       print_AST(std::cerr, r1);
       res = EMPTY;
       return;
     case POET_OP_DEBUG: {
       assert(r1 != 0);
        int userDebugSave = user_debug;  
        user_debug = AST2Int(r1);
        std::cerr << "\nDEBUGGING at level " << user_debug << "\n";
        r2->visit(this);
        std::cerr << "END DEBUGGING at level " << user_debug << "\n";
        POETOperator* r2op = dynamic_cast<POETOperator*>(r2);
        if (r2op == 0 || r2op->get_op() < POET_OP_SEQ)
            std::cerr << "DEBUGGING result: " << r2->toString()  << "===>" << res->get_className() << ":" << res->toString() << "\n";
        user_debug = userDebugSave;
        return;
     }
     case POET_OP_NOT:  res = fac->new_iconst(!AST2Bool(res)); return;
     case POET_OP_AND: 
           r1->visit(this); r1 = res;
           if (AST2Bool(r1))  {
              r2->visit(this); r2 = res;
              res = fac->new_iconst(AST2Bool(r2));
           }
           else
               res = fac->new_iconst(0);
            return;
     case POET_OP_OR:   
           r1->visit(this); r1 = res;
           if (!AST2Bool(r1))  {
              r2->visit(this); r2 = res;
               res = fac->new_iconst(AST2Bool(r2));
           }
           else
               res = fac->new_iconst(1);
           return;
     case POET_OP_UMINUS:  res = build_Uop(op->get_op(), res, true); break;
     case POET_OP_MINUS:   
     case POET_OP_PLUS:    
     case POET_OP_MULT:   
     case POET_OP_DIVIDE: 
     case POET_OP_MOD:  
           res = build_Bop(op->get_op(), r1,r2, true);
           return;
     case POET_OP_LT:   case POET_OP_LE:   case POET_OP_GT:   case POET_OP_GE:   case POET_OP_EQ:   case POET_OP_NE:   
                  res = build_Bop(op->get_op(), r1, r2, false); return;
     case POET_OP_DOT: res = ASTFactory::inst()->make_attrAccess(r1,r2); return;
     case POET_OP_CAR:  
              r1 = EvalTrace(r1);
              if (r1->get_enum() == SRC_LIST) 
                 res = static_cast<POETList*>(r1)->get_first();
              break;
     case POET_OP_CDR:  
              r1 = EvalTrace(r1);
              if (r1->get_enum() == SRC_LIST) {
                res = static_cast<POETList*>(r1)->get_rest();
                if (res == 0) res = EMPTY;
              }
              else res = EMPTY;
              break;
     case POET_OP_CONS:  {
             assert(r1 != 0 && r2 != 0 && r3 == 0);
             POETList* l2 = dynamic_cast<POETList*>(EvalTrace(r2));
             if (l2 != 0)
                 res = fac->new_list(r1,r2); 
             else
                 res = fac->new_list(r1, fac->new_list(r2,0)); 
             return;
         }
     case POET_OP_POND: {
 	 assert(r1 != 0 && r2 != 0 && r3 == 0);
         res = build_codeRef(r1,r2,false);
         return;
        }
     case POET_OP_DELAY: res = r1; return;
     case POET_OP_APPLY: r1 = EvalTrace(r1); r1->visit(this); return;
     case POET_OP_ASTMATCH: {
       r1->visit(this); r1 = res;
       res = match_AST(r1, r2, MATCH_AST_PATTERN);
       res = fac->new_iconst(res != 0); 
       break;
     }   
     case POET_OP_CLEAR: 
            if (r1->get_enum() != SRC_LVAR) 
               r1 = apply(r1);
            if (r1->get_enum() != SRC_LVAR) 
               INCORRECT_VAR(r1->toString());
            static_cast<LocalVar*>(r1)->get_entry().set_code(r1);
            res = r1;
            return;
     case POET_OP_MAP: {
           POETMap* m =new POETMap();
           res = m;
           if (r1 != 0 && r2 == 0 && r3 == 0) {
             for (POETList* p1 = dynamic_cast<POETList*>(r1); p1!=0; 
                   p1 = p1->get_rest()) {
                POETList* cur = dynamic_cast<POETList*>(p1->get_first());
                if (cur==0) { std::cerr << "Unexpected error : " << p1->get_first()->toString() << "\n"; assert(0); }
                m->set(cur->get_first(), cur->get_rest());
             }    
           }
           else assert(r1 != 0 && r2 != 0 && r3 == 0);
           return;
     }
     case POET_OP_LEN: {
        GetLenVisitor app;
        res =  fac->new_iconst(app.apply(r1));
        break;
     }
     case POET_OP_SPLIT: {
           std::string space="";
           assert(r1 != 0 && r2 != 0 && r3 == 0);
           switch (r1->get_enum()) {
           case SRC_ICONST: {
              POETCode* left=EMPTY;
              unsigned prefix = static_cast<POETIconst*>(r1)->get_val();
              POETCode* split = split_prefix(r2, prefix, left);
              res = fac->new_list(split, left);
              break;
           }
           case SRC_STRING:
               space = static_cast<POETString*>(r1)->get_content();
               res = split_string(space, r2);
               break;
           default:
               INCORRECT_STRING(r1->toString());
          }
        break;
     }
     case POET_OP_REBUILD: res = eval_rebuild(r1); break;
     case POET_OP_REPLACE: 
            if (r2 == 0) XFORM_CONFIG_INCORRECT("REPLACE", r1->toString());
            res = (r3 == 0)? eval_replace(r1,r2)
                           : eval_replace(r1,r2,r3);
            break;
     case POET_OP_DUPLICATE: res = eval_duplicate(r1,r2,r3); break;
     case POET_OP_COPY: { res = eval_copy(r1);  break; }
     case POET_OP_CONCAT:
          assert(r1 != 0 && r2 != 0 && r3 == 0);
          res = ASTFactory::inst()->new_string(r1->toString(OUTPUT_NO_DEBUG) + r2->toString(OUTPUT_NO_DEBUG));
          break;
     case POET_OP_PERMUTE: {
           POETEnum t1 = r1->get_enum(), t2 = r2->get_enum(); 
           std::vector<POETCode*> config, orig;
           if (t1 == SRC_TUPLE)
              static_cast<POETTuple*>(r1)->get_impl(config);
           else if (t1 == SRC_LIST) 
              List2Vector(static_cast<POETList*>(r1), config);
           else XFORM_PARAM_INCORRECT("PERMUTE", r1->toString());
           if (t2 != SRC_LIST) { res = r2; break; }
           List2Vector(static_cast<POETList*>(r2),orig); 
           std:: vector<POETCode*> targets;
           if (orig.size() < config.size()) 
              INCORRECT_TUPLE_SIZE(r1,orig.size());
           for (int i = 0; i < config.size();  ++i) {
              POETCode* cur=orig[i], *c = config[i]; 
              if (c->get_enum() != SRC_ICONST) XFORM_CONFIG_INCORRECT("PERMUTE", c->toString());
              int j = static_cast<POETIconst*>(c)->get_val();
              if (j <= 0) continue;
              for (int i = targets.size(); i < j; ++i)
                 targets.push_back(0);
              if (targets[j-1] != 0) XFORM_CONFIG_INCORRECT("PERMUTE",c->toString());
              targets[j-1] = cur;
           }
           res = Vector2List(targets);
           break;
          }
     case POET_OP_TYPEMATCH: 
     case POET_OP_TYPEMATCH_Q: {
       assert (r1 != 0 && r2 != 0);
       r1->visit(this); r1 = res;

       int userDebugSave = user_debug;
       try {
         if (user_debug < 11)
            user_debug = 0;

         res = parse_AST(r1, r2);
         user_debug = userDebugSave;
         if (op->get_op() == POET_OP_TYPEMATCH_Q)
             res = fac->new_iconst(1);
       }
       catch (ParseError err) {
         user_debug=userDebugSave;
         if (op->get_op() == POET_OP_TYPEMATCH_Q) {
             res = fac->new_iconst( 0);
         }
         else { EXIT(err.message()); }
       }
       break;
     }   
     case POET_OP_ERASE:  
        r2->visit(this); r2 = res;
        assert (r1 != 0 && r2 != 0); 
        res = EraseTrace(r1,r2); 
        return;
     case POET_OP_TRACE: 
       if (r2 != 0)
           res = TraceEval(r1, r2); 
       else {
           r1->visit(this); r1=res;
           ++sysVars_index;
           if (sysVars_index > SYSVARS_MAX)
             sysVars_index=1;
           std:: stringstream out; out << "_" << sysVars_index; 
           LocalVar* sysvar = sysVars.insert(ASTFactory::inst()->new_string(out.str()), LVAR_TRACE);
           sysvar->get_entry().set_code(r1);
           res = sysvar;
           r1 = 0;
       }
       return;
     case POET_OP_INSERT: { 
         if (r1->get_enum() != SRC_LVAR)  
            INCORRECT_TRACE_HANDLE(r1->toString());
         LocalVar* lvar = static_cast<LocalVar*>(r1);
         if (lvar->get_entry().get_entry_type() != LVAR_TRACE)  
            INCORRECT_TRACE_HANDLE(r1->toString());
         res = InsertTraceInside().apply(lvar, r2);
         return;
        }
     case POET_OP_RESTORE: {
        LocalVar* var = dynamic_cast<LocalVar*>(r1);
        if (var == 0) INCORRECT_TRACE_HANDLE( r1->toString())
        RestoreNestedTraceVars().apply(var); 
        res = EMPTY; return;
      }
     case POET_OP_SAVE:  {
        LocalVar* var = dynamic_cast<LocalVar*>(r1);
        if (var == 0) INCORRECT_TRACE_HANDLE( r1->toString())
        SaveNestedTraceVars().apply(var); 
        res = EMPTY; return;
      }
     case POET_OP_SEQ:    
         assert(r1 != 0 && r2 != 0 && r3 == 0);
         res = r2;
         return;
     case POET_OP_IFELSE: { 
       assert(r1 != 0 && r2 != 0 && r4 == 0);
       r1->visit(this);  
       int v = AST2Bool(res);
       if (user_debug) {
          std::cerr << "if " << SHORT(r1->toString(DEBUG_NO_VAR),500) << "===>" << v << "\n";
       }
       if (v)  { r2->visit(this); }
       else if (r3 != 0) r3->visit(this);
       return;
       }
     case POET_OP_RETURN: 
          assert(r1 != 0 && r2 == 0);
          r1 = eval_AST(r1);
          if (user_debug)
             std::cerr << "RETURN " << r1->toString() << ";\n"; 
           throw r1;
     case POET_OP_CONTINUE: 
     case POET_OP_BREAK:  
           if (user_debug)
             std::cerr << OpName[op->get_op()] << ";\n"; 
           throw op; 
     case POET_OP_FOREACHR: 
     case POET_OP_FOREACH: {
       assert(r1 != 0 && r2 != 0 && r3 != 0 && r4 != 0);
       r1->visit(this); r1 = res;
       if (user_debug) 
          std::cerr << "Entering " << OpName[op->get_op()] << "(" << SHORT(r1->toString(DEBUG_NO_VAR),200) << ")\n";
       try { eval_foreach(r1, r2, r3, r4, op->get_op() == POET_OP_FOREACHR); }
       catch (POETOperator* c) { if (c->get_op() != POET_OP_BREAK) throw c; }
       if (user_debug) 
          std::cerr << "Exiting " << OpName[op->get_op()] << "(" << SHORT(r1->toString(DEBUG_NO_VAR),200) << ")\n";
       res = EMPTY;
       return;
      } 
     case POET_OP_FOR: {
       assert(r1 != 0 && r2 != 0 && r3 != 0 && r4 != 0);
           if (r1 != 0) {
              r1->visit(this); 
              if (user_debug)
                std::cerr << "FOR init: " << r1->toString(DEBUG_NO_VAR) << "===>" << SHORT(res->toString(DEBUG_NO_VAR),500) << "\n";
           }
           assert(r2 != 0);
           r2->visit(this); POETCode* rr2 = res;
           if (user_debug)
              std::cerr << "FOR cond: " << r2->toString(DEBUG_NO_VAR) << "===>" << SHORT(rr2->toString(DEBUG_NO_VAR),500) << "\n";
           while (AST2Bool(rr2)) {
              if (r4 != 0) 
                 try {r4->visit(this); }
                 catch (POETOperator* c) {
                    if (c->get_op() == POET_OP_BREAK)
                        break;
                 }
              if (r3 != 0) r3->visit(this); 
              if (user_debug) 
                 std::cerr << "FOR incr: " << r3->toString(DEBUG_NO_VAR) << "===>" << SHORT(res->toString(DEBUG_NO_VAR),500) << "\n";
              r2->visit(this); rr2 = res;
              if (user_debug) 
                 std::cerr << "FOR cond: " << r2->toString(DEBUG_NO_VAR) << "===>" << SHORT(rr2->toString(DEBUG_NO_VAR),500) << "\n";
           }
            res = EMPTY;
           return;
        }
     case POET_OP_CASE: {
       assert(r1 != 0 && r2 != 0 && r3 == 0);
       r1->visit(this); 
       if (user_debug)
         std::cerr << "CASE cond: " << r1->toString(DEBUG_NO_VAR) << "===>" << res->get_className() << ":" << res->toString(DEBUG_NO_VAR) << "\n";
       r1 = res;
       POETTuple* v2 = dynamic_cast<POETTuple*>(r2);
       if (v2 == 0) v2 = ASTFactory::inst()->append_tuple(0, r2);
       bool succ=false;
       for (int i = 0; i < v2->size(); ++i) {
          POETList* cur = dynamic_cast<POETList*>(v2->get_entry(i));
          if (cur == 0) {
             std::cerr << "v2[i]=" << v2->get_entry(i)->toString() << "\n";
             std:: cerr << OpName[op->get_op()] << "\n";
             assert(false);
          }
          if (match_AST(r1, cur->get_first(), MATCH_AST_PATTERN)) {
               succ = true;
               cur->get_rest()->visit(this);
              if (user_debug)
                 std::cerr << "CASE " << r1->toString() << " matched label: " << cur->get_first()->toString(DEBUG_NO_VAR) << "===>" << res->toString(DEBUG_NO_VAR) << "\n";
               break;
          } 
       } 
       if (!succ)  {
           CASE_NOTFOUND( r1);
       }
       return;
      }
     case POET_OP_ERROR: 
          std::cerr << "User Error: ";
          print_AST(std::cerr, r1);
          USER_EXIT();
     default: 
         std::cerr << "ERROR: undefined operator: " << op->toString() << "\n";
         assert(0);
     }
     } 
     catch (Error err) { 
        std::cerr << "From operator " << SHORT(op->toString(DEBUG_NO_VAR),500) << "\n";
        throw err;
     }
     if (user_debug) {
        std::cerr << op->toString() << "\n===>" << res->toString() << "\n";
     }
   }

void set_syntaxFiles(std::list<POETProgram*>& syntaxFiles)
{
      for (std::list<POETProgram*>::const_iterator pSyntaxFiles = syntaxFiles.begin();
           pSyntaxFiles!=syntaxFiles.end(); ++pSyntaxFiles) {
          POETProgram* cur = (*pSyntaxFiles);
          set_syntaxFiles(cur->get_includeFiles()); 
          if (cur->get_file_ext() == ".code") cur->set_syntax();
      }
}

void clear_syntaxFiles(std::list<POETProgram*>& syntaxFiles)
{
      for (std::list<POETProgram*>::const_iterator pSyntaxFiles = syntaxFiles.begin();
           pSyntaxFiles!=syntaxFiles.end(); ++pSyntaxFiles) {
          POETProgram* cur = (*pSyntaxFiles);
          clear_syntaxFiles(cur->get_includeFiles()); 
          if (cur->get_file_ext() == ".code")
             cur->clear_syntax();
      }
}

POETCode* EvaluatePOET::eval_readInput(POETCode* input)
{
   assert (input->get_enum() == SRC_READ_INPUT);
   ReadInput* impl = dynamic_cast<ReadInput*>(input);
 try {
   POETCode* debug = impl->get_debug(), *annot = impl->get_annot(), *cond=impl->get_cond();

   if (cond != 0) 
    { cond = eval_AST(cond); 
      if (AST2Int(cond) == 0) return EMPTY; }

   int hasannot = (annot == 0)? 1 : AST2Int(eval_AST(annot));
   LocalVar* var = dynamic_cast<LocalVar*>(impl->get_var());
   POETCode* langFiles = impl->get_syntaxFiles();
   POETCode* inputFiles = impl->get_inputFiles();
   POETCode* codeType = impl->get_type();
   POETCode* inputInline = impl->get_inputInline();

   std::list<POETProgram*> syntaxPrograms;
   if (langFiles != 0) { 
      ReadFiles(langFiles, LEX_SYNTAX, syntaxPrograms); 
      set_syntaxFiles(syntaxPrograms); 
   }
   POETCode* res = 0;
   if (inputFiles != 0) {
      LexState inputlex = LEX_DEFAULT;
      if (codeType != input) 
          inputlex = (hasannot)? LEX_INPUT : LEX_INPUT_NOANNOT;
      std::list<POETProgram*> inputPrograms;
      ReadFiles(inputFiles, inputlex, inputPrograms);
      std::vector<POETCode*> vec;
      for (std::list<POETProgram*>::const_iterator p = inputPrograms.begin(); p != inputPrograms.end(); ++p) {
          POETCode* eval = 0;
          if (codeType != input) 
          {
            POETProgram* prog = *p;
            if (prog->size() != 1)
               { std::cerr << "unexpected program size: " << prog->size() << " for program: " << prog->get_filename() << "\n"; assert(0); } 
            eval = *(prog->begin());
            eval = parse_input( eval, codeType);  
          }
          else if (impl->get_var() == impl)
            eval = eval_program(*p);
          else eval = *p;
          vec.push_back(eval); 
       }
       res = Vector2List(vec);
   }
   int debugSave = user_debug;
   if (debug != 0)
      user_debug = AST2Int(debug);
   if (inputInline != 0) {
      inputInline = parse_input( inputInline, codeType);
      if (!res) res = inputInline;
      else res = ASTFactory::inst()->new_list(res, inputInline);
    assert(res != 0);
   }
   if (var != 0)
   {
       if (res == 0) 
       {
           std::cerr << "Warning: No input specified for input at " << impl->fileinfo_string() << "\n"; 
           var->get_entry().set_code(EMPTY);
       }
       else var->get_entry().set_code(res);
   }
   user_debug = debugSave;
   clear_syntaxFiles(syntaxPrograms);
   return res;
  }
  catch (Error err) { std::cerr << "At line " << impl->get_lineno() << " of file " << impl->get_filename() << " from " << SHORT(impl->toString(),200) << "\n"; exit(1); }
}

POETCode* EvaluatePOET:: eval_writeOutput(POETCode* output)
{
    WriteOutput* curOutput = dynamic_cast<WriteOutput*>(output);
    assert(curOutput != 0);
  try {
    POETCode* cond = curOutput->get_cond();
    if (cond != 0) 
     {
         cond = eval_AST(cond);
         if (AST2Int(cond) == 0) return 0;
     }
    std::list<POETProgram*> syntaxFiles;
    POETCode* langFiles = curOutput->get_syntaxFiles(); 
    if (langFiles != 0) { 
      ReadFiles(langFiles, LEX_SYNTAX, syntaxFiles); 
      set_syntaxFiles(syntaxFiles); 
    }
    POETCode* target = EvalTrace(curOutput->get_outputExp());
    target = eval_AST(target);

    const char* fnamep = 0;
    POETCode *file = curOutput->get_outputFile();
    if (file != 0) {
       file = eval_AST(file);
       if (file != EMPTY) {
          POETString* fname = dynamic_cast<POETString*>(file);
          if (fname ==0) INCORRECT_STRING( file->toString());
          fnamep = fname->get_content().c_str();
       }
    }

    if (fnamep == 0)
        code_gen(std::cout, target, unparseTarget->get_entry().get_code());
    else
    {
       std::fstream out;
       out.open(fnamep, std::ios::out);
       if (!out.good())  INCORRECT_FNAME(fnamep);
       code_gen(out, target, unparseTarget->get_entry().get_code());
       out.close();
    }
    clear_syntaxFiles(syntaxFiles);
    return target;
  }
  catch (Error err) { std::cerr << "At line " << curOutput->get_lineno() << " of file " << curOutput->get_filename() << " from " << SHORT(curOutput->toString(),50) << "\n"; exit(1); }
}

void POETProgram::set_syntax()
{
  if (prep_save.lvar != 0) prep_save.lvar->get_entry().set_code(prep_save.restr);
  if (parse_save.lvar != 0) parse_save.lvar->get_entry().set_code(parse_save.restr); 
  if (token_save.lvar != 0) { token_save.lvar->get_entry().set_code(token_save.restr); }
  if (unparse_save.lvar != 0) unparse_save.lvar->get_entry().set_code(unparse_save.restr);
  for (std::map<CodeVar*,POETProgram::CodeDefSave>::const_iterator p_codeDef = codeDef.begin();
       p_codeDef != codeDef.end(); ++p_codeDef) {
      std::pair<CodeVar*,CodeDefSave > cur = *p_codeDef;
      CvarSymbolTable::Entry e = cur.first->get_entry();

      if (cur.second.code_def != 0) {
         POETCode* oldcode = e.get_code();
         if (!allow_redefine_code() && oldcode != 0 && oldcode != cur.second.code_def) 
              SYM_ALREADY_DEFINED(e.get_name()->toString(OUTPUT_NO_DEBUG)); 
         e.set_codeDef(cur.second.code_def); 
      }
   
      int size = cur.second.par_type.size();
      for (int i = 0; i < size; ++i) 
       {
         LocalVarSave& par_save = cur.second.par_type[i];
         assert(par_save.lvar != 0); 
         if (par_save.restr != 0)
            par_save.lvar->get_entry().set_restr(par_save.restr);
       } 
      if (cur.second.parse != 0) e.set_parse(cur.second.parse);
      if (cur.second.unparse.lvar != 0 && cur.second.unparse.restr != 0)
         cur.second.unparse.lvar->get_entry().set_code(cur.second.unparse.restr);
  }
}

void POETProgram::clear_syntax()
{
  if (prep_save.lvar != 0) {  prep_save.lvar->get_entry().set_code(0); }
  if (parse_save.lvar != 0) parse_save.lvar->get_entry().set_code(0);
  if (token_save.lvar != 0) token_save.lvar->get_entry().set_code(0);
  if (unparse_save.lvar != 0) unparse_save.lvar->get_entry().set_code(0);
  for (std::map<CodeVar*,CodeDefSave>::iterator p_codeDef = codeDef.begin();
       p_codeDef != codeDef.end(); ++p_codeDef) {
      std::pair<CodeVar* const,CodeDefSave>& cur = *p_codeDef;
      CvarSymbolTable::Entry e = cur.first->get_entry();
      if (cur.second.code_def != 0)
          e.set_codeDef(0);
 
      if (!done_save) 
       {
           POETCode* parse = e.get_parse();
           if (parse != 0)
                cur.second.parse = parse;
           LocalVar* unparse = e.get_symTable()->find(ASTFactory::inst()->new_string("output"));
           if (unparse != 0)
           {
                cur.second.unparse.lvar = unparse;
                cur.second.unparse.restr= unparse->get_entry().get_code();
           }
           POETCode* _pars = e.get_param();    
           if (_pars == 0) continue;
           switch (_pars->get_enum()) {
             case SRC_LVAR:
             {
                LocalVar* cur_par = static_cast<LocalVar*>(_pars);
                cur.second.par_type.push_back(LocalVarSave(cur_par,cur_par->get_entry().get_restr())); 
                break;
             }
            case SRC_TUPLE:
            {
               POETTuple* pars = static_cast<POETTuple*>(_pars);
               for (int i = 0; i < pars->size(); ++i) {
                  LocalVar* cur_par = static_cast<LocalVar*>(pars->get_entry(i));
                  cur.second.par_type.push_back(LocalVarSave(cur_par,cur_par->get_entry().get_restr())); 
               }
               break;
            }
            default: assert(0);
          } 
        } 
       e.set_parse(0);
       if (cur.second.unparse.lvar != 0)
           cur.second.unparse.lvar->get_entry().set_code(0);
       int size = cur.second.par_type.size();
       for (int i = 0; i < size; ++i) {
          LocalVarSave& cur_save = cur.second.par_type[i];
          cur_save.lvar->get_entry().set_restr(0);
       }
    }
    done_save = true;
}

POETCode* EvaluatePOET::eval_program(POETProgram* prog)
{
  try {
    POETProgram* curfile_save = curfile;
    if (curfile != prog) 
         curfile = prog;
    POETCode* res = 0;
    for (std::list<POETProgram*>::const_iterator pincludeFiles = curfile->get_includeFiles().begin();
           pincludeFiles!=curfile->get_includeFiles().end(); ++pincludeFiles) {
          POETProgram* cur = (*pincludeFiles);
          res = eval_program(cur);
      }
    for (POETProgram::const_iterator 
         pdef = curfile->begin(); pdef != curfile->end(); ++pdef) 
     {
       POETCode* cur = *pdef;
       switch (cur->get_enum())
       {
       case SRC_WRITE_OUTPUT: res = eval_writeOutput(cur); break;
       case SRC_READ_INPUT: res = eval_readInput(cur); break;
       case SRC_PARAM_DECL: 
       case SRC_CVAR: 
       case SRC_LVAR: break;
       default: /* eval command */ res = eval_AST(cur);
       }
     }
     curfile = curfile_save;
     return res;
  }
  catch (Error err) { 
      if (curfile != 0) 
          std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; 
      exit(1); 
  }
  catch (POETOperator* s) { std::cerr << "Undefined destination: " << s->toString() << "\n"; exit(1); } 
}

POETCode* XformVar::eval(POETCode* args, bool evalArg, POETCode* config)
{
  if (get_entry().get_code() == 0)
       XFORM_UNDEFINED(toString());
  if (config == 0) config = get_config();
  XformVarInvoke invoke(get_entry(), config, args);
  POETCode* res = invoke.eval(evalArg);
  return res;
}

bool CodeVar:: check_cond(POETCode* args)
{
  POETCode* cond = get_static_attr("cond");
  if (cond != 0) {
          CvarSymbolTable::Entry e = get_entry();
          LvarSymbolTable *local = e.get_symTable();
          POETCode* pars = e.get_param();
          if (local != 0) local->push_table(false);
          if (pars != 0 && !match_parameters(pars, args,MATCH_PAR_MOD_CODE)) 
                return false;
          POETCode* res = eval_AST(cond); 
          if (local != 0)
             local->pop_table();
          return AST2Bool(res);
   }
   return true;
}

inline void ModifyParameter(LocalVar* par, POETCode* val, MatchParameterConfig modpar)
{
     switch (modpar) {
     case MATCH_PAR_NOMOD: break;
     case MATCH_PAR_MOD_CODE:
     case MATCH_PAR_MOD_RESTR:
      {
        LocalVar* var = static_cast<LocalVar*>(par);
        LvarSymbolTable::Entry e = var->get_entry();
        if (modpar==MATCH_PAR_MOD_CODE) e.set_code(val) ;
        else if (e.get_restr() != val) {  assert(e.get_restr() == 0) ; e.set_restr(val); }
        break;
      }
     default: std::cerr << "unknown match config!\n"; assert(0);
     }
}

bool 
match_parameters(POETCode* _pars, POETCode* _args, MatchParameterConfig modpar)
  {
   if (_pars == 0 && _args == 0) return true;
   else if (_pars == 0 || _args == 0) return false; 
  if (_args == ASTFactory::inst()->make_any() && modpar == MATCH_PAR_NOMOD) return true;
  if (_args->get_enum() != SRC_TUPLE) 
   {
     if (_pars->get_enum() != SRC_LVAR)
        return false;
     ModifyParameter(static_cast<LocalVar*>(_pars), _args, modpar);
   }
  else if (_pars->get_enum() != SRC_TUPLE) 
   {
     if (_pars->get_enum() != SRC_LVAR)
        return false;
     ModifyParameter(static_cast<LocalVar*>(_pars), _args, modpar);
  }
  else {
    POETTuple* pars = static_cast<POETTuple*>(_pars);
    POETTuple* args = static_cast<POETTuple*>(_args);
    if (pars->size() != args->size())
      return false;
    if (modpar) {
       for (int i = 0; i < pars->size(); ++i) {
          assert(pars->get_entry(i)->get_enum() == SRC_LVAR);
          ModifyParameter(static_cast<LocalVar*>(pars->get_entry(i)), args->get_entry(i), modpar);
      }
    }
  }
  return true;
}

POETCode* XformVarInvoke:: eval(bool evalArg)
{
   XvarSymbolTable::Entry e = get_entry();
   POETCode* f1 = e.get_code();
   if (f1 == 0) XFORM_UNDEFINED(e.get_name()->toString(OUTPUT_NO_DEBUG));
   POETCode* pars = e.get_param();
   POETCode* args = get_args();
   if (pars == 0) {
      if (args != 0) FUNC_MISMATCH(e.get_name()->toString(OUTPUT_NO_DEBUG), args);
   }
   else if (args == 0) return this; 
   else {
      int parsize = (pars->get_enum() == SRC_TUPLE)? 
                 static_cast<POETTuple*>(pars)->size() : 1;
      int argsize = (args->get_enum() == SRC_TUPLE)? 
                 static_cast<POETTuple*>(args)->size() : 1;
      if (argsize > parsize)  FUNC_MISMATCH(e.get_name()->toString(OUTPUT_NO_DEBUG), args);
      if (evalArg) { args = eval_AST(args); }
   }
   POETCode *config = get_config();
   LvarSymbolTable* local = e.get_symTable();
   if (local != 0) {
       local->push_table(true);
       if (config != 0) eval_AST(config);
       local->reset_table();
   }

   POETCode* result = 0;
   if (pars == 0 || match_parameters(pars, args,MATCH_PAR_MOD_CODE))  
      { 
        try { result = eval_AST(f1); }
        catch (POETCode* c) { result = c; }
        catch (Error err) { 
           std::cerr << "From invoking " << SHORT(toString(DEBUG_NO_VAR),500) << "\n"; throw err;
        }
      }
   else FUNC_MISMATCH(e.get_name()->toString(OUTPUT_NO_DEBUG), args)
   if (local != 0)
      local->pop_table();
   return result;
}

void ParameterDecl::set_val(POETCode* code, bool doparse)
  {
    assert(var != 0 && code != 0);
    POETCode* pattern = parse, *type = var->get_entry().get_restr();
    if (pattern == 0 && type != 0) pattern = type;
    if (doparse && pattern != 0 && code->get_enum() == SRC_STRING) {
      code = split_string("",code);
      assert(code != 0);
      if (pattern != 0) {
        try { POETCode* res = parse_AST(code, pattern);
            if (res == 0) SYM_INCORRECT_VAL(var->get_entry().get_name()->toString(OUTPUT_NO_DEBUG), code->toString(), pattern->toString());
            code = res;
          }
        catch (ParseError err)  {
             std::cerr << "From parsing " << code->toString(DEBUG_NO_VAR) << "\n=>" << pattern->toString(DEBUG_NO_VAR) << "\n"; 
             EXIT(err.message());
        }
       }
    }
    if (type != 0 && type != pattern) {
       POETCode* res = match_AST(code, type, MATCH_AST_PATTERN);
       if (res == 0)
          SYM_INCORRECT_VAL(var->get_entry().get_name()->toString(OUTPUT_NO_DEBUG), code->toString(), type->toString());
       code = res;
    }
    assert(code != 0);
    var->get_entry().set_code(code);
}

POETCode* CodeVar::invoke_func(const std::string& name, POETCode* args)
 {
    try {
       POETCode* code = get_static_attr(name);
       if (code != 0) {
            entry.get_symTable()->push_table(false);
            set_attr();
            POETCode* pars=entry.get_param();
            if (pars != args) {
                match_parameters(pars,args,MATCH_PAR_MOD_CODE);
            }
            POETCode* res = eval_AST(code);
            entry.get_symTable()->pop_table();
            assert(res != 0);
            return res;
       }
       return 0;
   }
  catch (Error err) { std::cerr << "From invoking parsing function : " << name << " of code template :" << entry.get_name()->toString(OUTPUT_NO_DEBUG) << "\n"; throw err; }
   }

void POETProgram::eval_define(LocalVar* var, POETCode* code)
{ 
  code = eval_AST(code);
  var->get_entry().set_code(code); 
  std::string name = var->get_entry().get_name()->toString(OUTPUT_NO_DEBUG);
  if (name == "PREP") 
      { prep_save.lvar = var; prep_save.restr=code; }
  else if (name == "PARSE") 
      { parse_save.lvar = var; parse_save.restr=code; }
  else if (name == "TOKEN") 
      { token_save.lvar = var; token_save.restr=code; }
  else if (name == "UNPARSE") 
      { unparse_save.lvar = var; unparse_save.restr=code; }
  else if (name == "BACKTRACK")
     { backtrack = AST2Int(code); }
}


CodeVar* ASTFactory::
build_codeRef(const CvarSymbolTable::Entry e, POETCode* args)
     {
     CvarTable* m = get_cvarTable(e);
     int size = e.attr_size();
     POETCode* attr = 0;
     if (size == 0) ;
     else if (size == 1) attr = eval_AST(e.get_attr(0)->get_entry().get_code());
     else { 
            attr = new_pair(eval_AST(e.get_attr(0)->get_entry().get_code()), 
                       eval_AST(e.get_attr(1)->get_entry().get_code()));
            for (int i = 2; i < size; ++i) {
               attr = append_tuple(attr, 
                    eval_AST(e.get_attr(i)->get_entry().get_code()));
            }
     }
     return static_cast<CodeVar*>(m->new_item(args, attr));
   }

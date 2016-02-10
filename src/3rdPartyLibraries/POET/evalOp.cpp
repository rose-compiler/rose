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
  Routines for implementing arithmetic, comparison, and pattern matching 
  operations  for POET AST.
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <set>
#include <list>
#include <poet_ASTvisitor.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <poet_ASTinterface.h>

extern POETProgram* curfile;
extern bool debug_pattern();
extern bool debug_repl();

class CodeReplOperator {
 public:
  virtual POETCode* apply(POETCode* f) = 0;
};

class CodeReplVisitor : public ReplInfoVisitor
{
 protected:
  CodeReplOperator& Repl;
  virtual void defaultVisit(POETCode* s) { 
      if (s->get_enum() == SRC_ANY) { res = s; }
      else { std::cerr << "unhandled case:" << s->toString() << "\n"; assert(0); }
      }
  virtual void visitNULL(POETNull* n) { res = n; }
  virtual void visitMap(POETMap* m) { res = m; }
  virtual void visitIconst(POETIconst* l) { res=Repl.apply(l); }
  virtual void visitString(POETString* l) { res=Repl.apply(l); }
  virtual void visitLocalVar(LocalVar* v) {
    res = Repl.apply(v);
    if (res == v) ReplInfoVisitor::visitLocalVar(v);
  }
   
  virtual void visitList(POETList* l) {
    res = Repl.apply(l);
    if (res == l) ReplInfoVisitor::visitList(l);
  }
  virtual void visitTuple( POETTuple* v)  {
    res = Repl.apply(v);
    if (res == v) ReplInfoVisitor::visitTuple(v);
  }
  virtual void visitCodeVar( CodeVar* v) { 
      res = Repl.apply(v);
      if (res == v)  ReplInfoVisitor::visitCodeVar(v); 
   }
  virtual void visitUnknown (POETCode_ext* e) {
      res = Repl.apply(e);
      if (res == e)  ReplInfoVisitor::visitUnknown(e); 
  }
 public:
  CodeReplVisitor(CodeReplOperator& _op) : Repl(_op) {}
};

class SplitStringOperator : public CodeReplOperator
{
  std::string split;
  POETString* splitString;
 protected:
  POETCode* apply(POETCode* f)
     { 
       if (f->get_enum() == SRC_STRING) {
          if (f == EMPTY) return f; 
          POETList* res = 0;
          std::string input = static_cast<POETString*>(f)->get_content();
          int splitsize = (splitString)? split.size() : 1;
          int inputsize = input.size();
          const char* splitp = (splitString)? split.c_str() : 0;
          const char* inputp1 = input.c_str();
          const char*  inputp2 = inputp1 + inputsize - 1;
          int size2end = 1, size2start=inputsize-1;
          bool isint = true;
          for (const char* inputp = inputp2; inputp >= inputp1; 
               --inputp,++size2end,--size2start) {
             if ( (splitp && memcmp(inputp, splitp, splitsize) == 0)
                  || (splitp == 0 && !isalnum(*inputp) && (*inputp) != '_')) {
                 int cursize = size2end - splitsize;
                 if (cursize) {
                    std::string newitem = input.substr(size2start+splitsize, cursize);
                    POETCode* newval = ASTFactory::inst()->new_string(newitem);
                    /*POETCode* newval = (isint)? 
                          (POETCode*)ASTFactory::inst()->new_iconst(atoi(newitem.c_str()))
                        : (POETCode*)ASTFactory::inst()->new_string(newitem); */
                    res = ASTFactory::inst()->new_list(newval, res);
                 }
                 POETCode* newsplit = splitString;
                 if (!newsplit) {
                     std::string newstring; newstring.push_back(*inputp);
                     newsplit = ASTFactory::inst()->new_string(newstring);
                 }
                 res = ASTFactory::inst()->new_list(newsplit, res);
                 size2end = 0;
                 isint = true;
             }
             else if (!isdigit(*inputp))
                  isint = false;
          }
          assert(size2start == -1);
          if ( size2end > 1) {
             std::string newitem = input.substr(0, size2end-1);
             POETString* newval = ASTFactory::inst()->new_string(newitem);
             if (res != 0)
                 return ASTFactory::inst()->new_list(newval, res);
             else return newval;
          }
          return res;
     }
     return f;
  }
 public:
  SplitStringOperator( const std::string& _split) : split(_split) 
  {
     if (split == "") 
          splitString = 0;
     else
         splitString = ASTFactory::inst()->new_string(split);
  }
};

POETCode* split_string(const std::string& space, POETCode* code)
{
           SplitStringOperator op(space);
           CodeReplVisitor app(op);
           return app.apply(code);
}

POETCode* split_prefix(POETCode* code, unsigned& prefix, POETCode*& left)
{
   switch (code->get_enum()) {
   case SRC_STRING: {
        POETString* str = static_cast<POETString*>(code);
        std::string content = str->get_content();
        unsigned size = content.size();
        if (prefix < size) {
           unsigned size1 = prefix, size2 = size - prefix + 1;
           prefix = 0;
           left = ASTFactory::inst()->new_string(content.substr(size1,size2));
           return ASTFactory::inst()->new_string(content.substr(0,size1));
        }
        else if (prefix == size) {
           left = EMPTY; prefix=0; return code;
        }
        else {
           left =  EMPTY; prefix -= size;  return code;
        }
    }
   case SRC_LIST: {
        POETList* l = static_cast<POETList*>(code);
        POETCode* first = l->get_first(), *rest = l->get_rest();
        POETCode* split1 = split_prefix(first, prefix, left);
        if (split1 != 0 && prefix > 0) {
           if (rest == 0) { left=EMPTY; return split1; }
           POETCode* split2 = split_prefix(rest, prefix, left);
           return ASTFactory::inst()->new_list(split1, split2);
        }
        if (left == EMPTY && rest != 0) left = rest;
        else left = ASTFactory::inst()->new_list(left, rest);
        return split1;
    }
   default:
       left=code;
       return EMPTY;
   }
}

int apply_Uop(POETOperatorType op, POETCode* op1)
{
     int res = 0;
     switch (op) {
       case POET_OP_UMINUS:
           res = - AST2Int(op1); break;
       default: assert(false);
     }
     return res;
}

int apply_Bop(POETOperatorType op, POETCode* op1, POETCode* op2)
{
     int res = 0;
     switch (op) {
        case POET_OP_MINUS:  
                res = AST2Int(op1) - AST2Int(op2); break;
        case POET_OP_PLUS:    
                res = AST2Int(op1) + AST2Int(op2); break;
        case POET_OP_MULT:   
                res = AST2Int(op1) * AST2Int(op2); break;
        case POET_OP_DIVIDE: 
                res = AST2Int(op1) / AST2Int(op2); break;
        case POET_OP_MOD:  
                res = AST2Int(op1) % AST2Int(op2); break;
        case POET_OP_EQ:    
                res = (op1 == op2); break; 
        case POET_OP_NE:   
                res = (op1 != op2); break; 
        case POET_OP_LT: 
           {  
             POETIconst* op1Int = AST2Iconst(op1), *op2Int=AST2Iconst(op2);
             if (op1Int != 0 && op2Int != 0) 
                { res = (op1Int->get_val() < op2Int->get_val()); }
             else if (op1->get_enum() == SRC_STRING && op2->get_enum() == SRC_STRING)
                { res = static_cast<POETString*>(op1)->get_content() 
                        < static_cast<POETString*>(op2)->get_content(); } 
             else { res = 0; }
                break;
           }
        case POET_OP_LE:   
           {  
             POETIconst* op1Int = AST2Iconst(op1), *op2Int=AST2Iconst(op2);
             if (op1Int != 0 && op2Int != 0) 
                { res = (op1Int->get_val() <= op2Int->get_val()); }
             else if (op1->get_enum() == SRC_STRING && op2->get_enum() == SRC_STRING)
                { res = static_cast<POETString*>(op1)->get_content() 
                        <= static_cast<POETString*>(op2)->get_content(); } 
             else { res = 0; }
                break;
           }
        case POET_OP_GT:   
           {  
             POETIconst* op1Int = AST2Iconst(op1), *op2Int=AST2Iconst(op2);
             if (op1Int != 0 && op2Int != 0) 
                { res = (op1Int->get_val() > op2Int->get_val()); }
             else if (op1->get_enum() == SRC_STRING && op2->get_enum() == SRC_STRING)
                { 
                  std::string r1= static_cast<POETString*>(op1)->get_content() ;
                  std::string r2= static_cast<POETString*>(op2)->get_content();
                  res = static_cast<POETString*>(op1)->get_content() 
                        > static_cast<POETString*>(op2)->get_content(); } 
             else { res = 0; }
                break;
           }
        case POET_OP_GE:   
           {  
             POETIconst* op1Int = AST2Iconst(op1), *op2Int=AST2Iconst(op2);
             if (op1Int != 0 && op2Int != 0) 
                { res = (op1Int->get_val() >= op2Int->get_val()); }
             else if (op1->get_enum() == SRC_STRING && op2->get_enum() == SRC_STRING)
                { res = static_cast<POETString*>(op1)->get_content() 
                        >= static_cast<POETString*>(op2)->get_content(); } 
             else { res = 0; }
                break;
           }
        default: assert(false);
     }
     return res;
}

POETCode* EvaluatePOET::
build_Uop(POETOperatorType op, POETCode* _op1, bool parse)
{
  POETCode* op1 = EvalTrace(_op1);
  POETCode* op1Int = op1;
  if ( parse && (op1Int=AST2Iconst(op1)) == 0) {
      POETTuple* r= fac->new_pair(fac->new_string(OpName[op]), op1);
      CodeVar* cvar = dynamic_cast<CodeVar*>(parseUop->get_entry().get_code());
      if (cvar != 0) return build_codeRef(cvar, r, false);
      else return r;
  }
  else if (op1Int != 0) {
     int res = apply_Uop(op, op1Int);
     return fac->new_iconst(res);
   }
  else  INCORRECT_ICONST(op1);
}

POETCode* EvaluatePOET::
build_Bop(POETOperatorType op, POETCode* _op1, POETCode* _op2, bool parse)
{
  assert(_op1 != 0 && _op2 != 0);
  POETCode* op1 = EvalTrace(_op1); 
  POETCode* op2 = EvalTrace(_op2);

  POETCode* op1Int = op1, *op2Int = op2;
  if (parse && ((op1Int=AST2Iconst(op1)) == 0 || (op2Int=AST2Iconst(op2))==0)){
      CodeVar* cvar = dynamic_cast<CodeVar*>(parseBop->get_entry().get_code());
      if (cvar == 0) INCORRECT_CVAR(parseBop); 
      POETCode* r= fac->append_tuple(fac->new_pair(fac->new_string(OpName[op]),_op1), _op2);
      POETCode* result = cvar->invoke_rebuild(r);
      return result;
  }
  if (op1Int != 0 && op2Int != 0) {
     int res = apply_Bop(op, op1Int, op2Int);
     return fac->new_iconst(res);
   }
  else  {
      if (op1Int == 0) INCORRECT_ICONST(op1);
      if (op2Int == 0) INCORRECT_ICONST(op2);
      return 0;
  }
}

class MaskLocalVars  : public CollectInfoVisitor
{
 public:
  virtual void visitLocalVar( LocalVar* v)
     { 
       LvarSymbolTable::Entry e = v->get_entry();
       switch (e.get_entry_type()) {
       case LVAR_REG:
       case LVAR_CODEPAR:
       case LVAR_XFORMPAR:
          e.push(false); 
          e.set_code(v);
       default: ;
       }
     }
};

class UnmaskLocalVars  : public CollectInfoVisitor
{
 public:
  virtual void visitLocalVar( LocalVar* v)
     { 
       LvarSymbolTable::Entry e = v->get_entry();
       switch (e.get_entry_type()) {
       case LVAR_REG:
       case LVAR_CODEPAR:
       case LVAR_XFORMPAR:
          e.pop(); 
       default: ;
       }
     }
};

class FOREACH_Collect : public CollectInfoVisitor
{
  POETCode *cond, *op, *found;
  bool backward;
 public:
  int match_eval(POETCode* ff) 
    {
     int res_found = 0;
     if (found == ZERO) res_found = -1;
     else if (found == ONE) res_found = 1;
     else res_found = AST2Int(eval_AST(found));
     if (res_found != 0) {
        if (user_debug) 
           std::cerr << "FOREACH found : " << SHORT(ff->toString(DEBUG_NO_VAR),500) << "\n";
        try { eval_AST(op);  }
        catch (POETOperator* c) {
          if (c->get_op() != POET_OP_CONTINUE)
             throw c;
        }
      }
      return res_found;
   }
  
  int match(POETCode* ff) {
       POETCode* r = match_AST(ff,cond, MATCH_AST_PATTERN);
       if (r) { 
         return match_eval(ff); 
       }
       return 0;
    }
  virtual void visitIconst(POETIconst* l) { match(l); }
  virtual void visitString(POETString* l) { match(l); }
  virtual void visitLocalVar(LocalVar* v) 
     { if (match(v) != 1) {
                 CollectInfoVisitor tmp1(this);
                 CollectInfoVisitor tmp2(&tmp1);
                 tmp2.visitLocalVar(v); 
       }
     }
  virtual void visitMap(POETMap* m) {
     for (POETMap::const_iterator p = m->begin(); p != m->end(); ++p) {
        std::pair<POETCode*,POETCode*> curpair = *p;
        POETTuple * cur = ASTFactory::inst()->new_pair(curpair.first, curpair.second); 
        cur->visit(this);
     }
  }
  virtual void visitList(POETList* l) {
    if (match(l) == 1) return;
    if (l->get_rest() == 0) {l->get_first()->visit(this); }
    else if (!backward) 
         { l->get_first()->visit(this); l->get_rest()->visit(this) ; }
    else
         { l->get_rest()->visit(this); l->get_first()->visit(this) ; }
  }
  virtual void visitTuple( POETTuple* v) {
    if (match(v) == 1) return;
    if (!backward) CollectInfoVisitor::visitTuple(v);
    else if (v->size() > 0) {
       for (int i = v->size()-1; i >= 0; --i) 
          v->get_entry(i)->visit(this);
    }
  }
  virtual void visitCodeVar(CodeVar* v)  {
    if (match(v) != 1) { CollectInfoVisitor::visitCodeVar(v); }
  }
  virtual void visitUnknown(POETCode_ext* v)
  {
    if (match(v) != 1) { CollectInfoVisitor::visitUnknown(v); }
  }

  FOREACH_Collect( POETCode* _cond, POETCode* _op, POETCode* _found, bool back)
     : cond(_cond), op(_op), found(_found), backward(back) {}
};

void EvaluatePOET::
eval_foreach(POETCode* input, POETCode* pattern, POETCode* found, POETCode* body)
{
  POETOperator* op = dynamic_cast<POETOperator*>(input);
  bool backward = false;
  if (op != 0 && op->get_op() == POET_OP_REVERSE) {
     input = op->get_arg(0);
     backward=true;
  } 
  input = eval_AST(input);
  FOREACH_Collect collect( pattern,body, found, backward);
  collect.apply(input); 
}

class UnTraceVars : public ReplInfoVisitor
{
 public:
  virtual void defaultVisit(POETCode* s) { res = s; }
  virtual void visitLocalVar(LocalVar* v)
     { 
       LvarSymbolTable::Entry e = v->get_entry();
       POETCode* save = e.get_code();
       if (save != 0 && save != v) {
             e.set_code(0);
             apply(save);
             e.set_code(save);
       }
       else if (e.get_entry_type() == LVAR_TRACE) TRACE_RECURSIVE(v)
       else SYM_UNDEFINED(v->toString())
     }
};

POETCode* eval_copy(POETCode* input)
{ return UnTraceVars().apply(input); }

class DupInfoVisitor : public POETCodeVisitor
{
 protected:
  std::vector<POETCode*> res;
  ASTFactory* fac;
 public:
  DupInfoVisitor() { fac = ASTFactory::inst(); }
  virtual void visitLocalVar(LocalVar* v) { 
          LvarSymbolTable::Entry e = v->get_entry();
          POETCode* cur = e.get_code(); 
          if (cur == 0) {
              if (e.get_entry_type() == LVAR_TRACE) TRACE_RECURSIVE(v)
              else SYM_UNDEFINED(v->toString())
          }
          if (cur != v) {
             e.set_code(0);
             apply(cur);
             if (res.size() == 1) {
                 e.set_code(res[0]); 
                 res[0] = v;
             }
             else {
                 e.set_code(cur);
                 std::cerr << "Warning: tracing is lost as code is duplicated: " << v->toString() << "\n";
             }
          }
     }
  virtual void visitList(POETList* l) 
     { 
          l->get_first()->visit(this) ; 
          std::vector<POETCode*> rf = res;
          res.clear();
          if (l->get_rest() != 0)
              l->get_rest()->visit(this); 
          if (res.size() > 0 || rf.size() > 0) {
            if (rf.size() == 0)
              for (unsigned i = 0; i < res.size(); ++i) 
                res[i] = fac->new_list(l->get_first(), res[i]);
            else if (res.size() == 0)
              for (unsigned i = 0; i < rf.size(); ++i) 
                res.push_back(fac->new_list(rf[i],l->get_rest()));
            else
              for (unsigned i = 0; i < res.size(); ++i) 
                res[i] = fac->new_list(rf[i], res[i]);
         }
     }
  virtual void visitTuple(POETTuple* v) 
     {
          std::vector< std::vector<POETCode*> > tmp;
          int dsize = 0;
          for (unsigned i=0 ; i < v->size(); ++i) {
            v->get_entry(i)->visit(this);
            tmp.push_back(res);
            if (res.size() > 0) dsize = res.size();
            res.clear();
          }
         for (int j=0 ; j < dsize; ++j) {
            POETCode* cur = 0;
            for (unsigned i=0 ; i < v->size(); ++i) {
               if (tmp[i].size() > 0)
                  cur = fac->append_tuple(cur, tmp[i][j]);
               else
                  cur = fac->append_tuple(cur, v->get_entry(i));
            }
            res.push_back(cur);
         }
     } 
  virtual void visitCodeVar( CodeVar* v) 
     { 
       if (v->get_args() != 0) {
          v->get_args()->visit(this) ; 
          for (unsigned i = 0; i < res.size(); ++i) 
              res[i] = build_codeRef(v, res[i], true);
       }
     }
  POETCode* apply(POETCode* code) 
     { 
       res.clear(); code->visit(this); 
       return Vector2List(res);
     }
};

class CodeDupVisitor : public DupInfoVisitor
{
 protected:
  POETCode *from, *to;
  bool Dup(POETCode* ff) { 
       if (ff == from) {
           POETCode* cur = to;
           while (cur != 0) {
              POETList* l = dynamic_cast<POETList*>(cur);
              if (l != 0) { 
                 res.push_back(l->get_first());
                 cur = l->get_rest(); 
              }
              else { res.push_back(cur); 
                   cur = 0; }
           }
           return true;
       } 
       return false;
     }
  virtual void visitIconst(POETIconst* l) { Dup(l); }
  virtual void visitString(POETString* l) { Dup(l); }
  virtual void visitList(POETList* l) {
    if (!Dup(l)) DupInfoVisitor::visitList(l);
  }
  virtual void visitTuple( POETTuple* v)  {
    if (!Dup(v)) DupInfoVisitor::visitTuple(v);
  }
  virtual void visitCodeVar(CodeVar* v)  {
    if (!Dup(v)) DupInfoVisitor::visitCodeVar(v);
  }
 public:
  CodeDupVisitor( POETCode* f, POETCode* dup) 
     : from(f), to(dup) { assert(from != 0 && to != 0); }
  POETCode* apply(POETCode* code)
  {
     POETCode* res2 = DupInfoVisitor::apply(code);
     if (res2 == EMPTY_LIST && code != 0 && code != EMPTY_LIST) {
          assert(to != 0);
           POETCode* cur = to;
           res2 = 0;
           while (cur != 0) {
              POETList* l = dynamic_cast<POETList*>(cur);
              res2 = ASTFactory::inst()->new_list(code, res2);
              if (l != 0) cur = l->get_rest(); 
              else  cur = 0;
           }
     }
     return res2;
  }
};

POETCode* eval_duplicate(POETCode* sel, POETCode* dup, POETCode* input)
{
       assert (sel != 0 && dup != 0 && input !=0); 
       input = UnTraceVars().apply(input);
        CodeDupVisitor dupop(sel,dup);
        return dupop.apply(input);
}

class CodeRebuildVisitor : public ReplInfoVisitor
{
  bool inList;
 public:
  CodeRebuildVisitor() : inList(false) {}
  virtual void defaultVisit(POETCode* s) { res = s; }
  virtual void visitCodeVar( CodeVar* v)
     { 
       if (v->get_args() != 0) {
          POETCode* args = v->get_args();
          args->visit(this); args = res;
          res = v->invoke_rebuild(args);
       }
       else
          res = v;
     }
  virtual void visitList(POETList* l) 
   {
       bool inList_save = inList;
       POETCode* first = l->get_first(), *rest = l->get_rest();
       inList=false;
       apply(first);
       inList=inList_save;
       POETCode* r1 = res;
       if (r1 == EMPTY || r1 == EMPTY_LIST || r1 == 0) { 
          if (rest == 0) res = 0;
          else apply(rest);
       }
       else if (rest != 0) {
         inList = true;
	 apply(rest);
	 inList=inList_save;

	 POETCode* r2 = res;
	 res = MakeXformList(r1,r2,false); 
	 if (res == 0) {
            res = fac->new_list(r1,r2); 
         }
         else res->visit(this);
       }
       else if (inList) res = fac->new_list(r1,0);
       else res = r1;
   }
};

POETCode* eval_rebuild(POETCode* input)
{
   CodeRebuildVisitor op;
   return op.apply(input);
}

class CodeReplSingleOperator : public CodeReplOperator
{
 protected:
  POETCode *from, *to;
  POETCode* apply(POETCode* f)
     { 
       if (f==from) {
         if (debug_repl()) std::cerr << "replacing " << f->toString() << "\n with " << to->toString() << "\n";
         return to;
       } 
       return f;
     }
  CodeReplSingleOperator() : from(0), to(0) {}
 public:
  CodeReplSingleOperator( POETCode* _from, POETCode* _to) 
    : from(_from),to(_to) {}
};

class CodeReplMapOperator : public CodeReplOperator
{
  POETMap* repl;
  POETCode* apply(POETCode* f)
     { 
       POETCode* r = repl->find(f);
       if (r != 0) {
           return r;
       } 
       return f;
     }
  public:
   CodeReplMapOperator( POETCode* r) 
    { repl = dynamic_cast<POETMap*>(r);
      if (repl == 0) 
         XFORM_CONFIG_INCORRECT("REPLACE", r->toString(DEBUG_NO_VAR));
    }
};


class CodeReplListOperator : public CodeReplSingleOperator
{
 protected:
  POETCode* rest;

  POETCode* apply(POETCode* f)
  {
   if (from == 0) { return f; }

    POETCode* res = CodeReplSingleOperator::apply(f);
    if (f == from) {
       do set_repl(); while (from != 0 && from == to);
    }
    return res;
  }
  bool set_repl() {
     if (rest == 0) { from = to = 0; return false; }
     POETCode* cur = rest;
     POETList* tmp = dynamic_cast<POETList*>(rest);
     if (tmp != 0) {
        cur = tmp->get_first();
        rest = tmp->get_rest();
     }    
     else rest = 0;
     if (cur == EMPTY) return set_repl();
     POETTuple* tuple = static_cast<POETTuple*>(cur);
     if (tuple == 0 || tuple->size() != 2)
          XFORM_CONFIG_INCORRECT("REPLACE", cur->toString(DEBUG_NO_VAR));
     from = tuple->get_entry(0); to = tuple->get_entry(1);
     return true;
  }

 public:
  CodeReplListOperator( POETCode* config) : rest(config) { set_repl(); }
  bool check_rest() { 
      if (rest != 0) REPL_INCOMPLETE(rest);
      return true;
   }
};

POETCode* eval_replace(POETCode* r1, POETCode * r2)
{
        if (r1 == EMPTY)  {
             return r2;
         }
         if (r1->get_enum() == SRC_MAP) {
                 CodeReplMapOperator op(r1);
                 CodeReplVisitor app(op);
                 return app.apply(r2);
         }
         CodeReplListOperator replop(r1);
         CodeReplVisitor app(replop);
         POETCode* res = app.apply(r2);
         replop.check_rest();
         return res;
}
POETCode* eval_replace(POETCode* r1, POETCode * r2, POETCode* r3) 
{
     if (r1 == r2) return r3;
     if (r1 == r3) return r2;
     if (r3->get_enum() == SRC_LVAR) {
          LvarSymbolTable::Entry entry 
                   = static_cast<LocalVar*>(r3)->get_entry();
          assert (entry.get_entry_type() == LVAR_TRACE); 
          if (entry.get_code() == r1) { // replace content of r3
             entry.set_entry_type(LVAR_TRACE_OUTDATE);
             POETCode* res = eval_AST(r2); 
             entry.set_entry_type(LVAR_TRACE);
             entry.set_code(res);
             return r3;
          }
     }
     CodeReplSingleOperator op(r1,r2);
     CodeReplVisitor app(op);
     return app.apply(r3);
}

/********************* pattern matching ********************************/
POETCode* EvaluatePOET::
match_Type(POETCode* r1, POETType* type, bool convertType)
{
     switch (type->get_type())
     {
     case TYPE_ANY: return r1;
     case TYPE_INT: return AST2Iconst(r1);
     case TYPE_STRING: 
     case TYPE_ID: {
         POETString* res = (convertType)? AST2String(r1): dynamic_cast<POETString*>(r1);
         if (res != 0 && type->get_type() == TYPE_ID) {
             std::string content = res->get_content();
             if (!content.size()) res = 0;
             else {
                 char first = content[0];
                 if ( (first >= 'A' && first <= 'Z') 
                     || (first >= 'a' && first <= 'z') || first == '_');
                 else res = 0;
             }
             if (res != 0 && keywords->get_entry().get_code() != 0) {
                for (POETCode* p = keywords->get_entry().get_code(); 
                     p != 0; p = get_tail(p)) 
                   if (res == get_head(p)) { res = 0; break; }
             }
         }
         return res;
       }
     case TYPE_FLOAT: 
       {
         POETString* res = (convertType)? AST2String(r1): dynamic_cast<POETString*>(r1);
         if (res != 0) {
            std::string content = res->get_content();
             if (content.size() < 2) res = 0;
             else {
                 int point = 0;
                 for (int i = 0; i < content.size(); ++i) {
                    char cur = content[i];
                    if (cur == '.') {
                       point = point + 1;
                       if (point > 1) { res = 0; break; }
                    }
                    else if (cur < '0' || cur > '9') { res = 0; break; }
                 }
             }
         }
         return res;
       }
     case TYPE_NOT: {
        POETTypeNot* typenot = static_cast<POETTypeNot*>(type);
        if (match_AST(r1, typenot->get_arg(), MATCH_AST_PATTERN)) return 0;
        return r1;
      }
     default: std::cerr << "unrecognized POET type: " << type->toString() << "\n"; assert(0);
     }
  }

class MatchASTVisitor :  public ReplInfoVisitor
{
 protected:
  POETCode* r1;
  LocalVar* lvar;
  MatchOption config;
  POETCode* any;
  ASTFactory* fac;
 public:
  MatchASTVisitor(MatchOption _config) : config(_config)
   { fac = ASTFactory::inst(); any = fac->make_any(); lvar = 0; r1 = 0; }
  POETCode* get_result() { return res; }
  bool apply(POETCode* _r1, POETCode* _r2) {
    if (_r1 == _r2) {  res = _r2; return true; }
    if (_r1 == 0 || _r2 == 0) { return false; }
    if (_r2 == any) { res = _r1; return true; }
    if (debug_pattern()) {
       std::cerr << "Pattern matching " << _r1->get_className() << ":" << _r1->toString() << " vs. " << _r2->get_className() << ":" << _r2->toString() << "\n";
    }

    POETCode* v1 = r1;
    LocalVar* vlvar = lvar;
    if (r1 != _r1) { r1 = _r1; lvar = 0; }

    if (r1 != 0 && r1->get_enum() == SRC_LVAR) {
       LocalVar* v1 = static_cast<LocalVar*>(r1);
       if (v1->get_entry().get_entry_type() == LVAR_TRACE)
           lvar = v1;
       r1 = v1->get_entry().get_code();
       while (r1 != 0 && r1->get_enum() == SRC_LVAR) {
              LocalVar* v1 = static_cast<LocalVar*>(r1);
              if (lvar == 0 && v1->get_entry().get_entry_type() == LVAR_TRACE)
                 lvar = v1;
              POETCode* tmp = v1->get_entry().get_code();
              if (tmp == r1) break;
              r1 = tmp;
       }
    }
    if (r1 == 0) r1 = EMPTY;

    if (debug_pattern()) {
      if (lvar != 0) std::cerr << "Tracing " << lvar->toString(OUTPUT_NO_DEBUG) << "\n";
    }

    res = 0;
    _r2->visit(this);
    if (res == r1)
        res = _r1;
    r1 = v1; // restore r1 to previous state
    lvar = vlvar;
    return res != 0;
  }
  virtual void visitIconst(POETIconst* v) {
      if (AST2Iconst(r1) == v) res = v;
  }
  virtual void visitString(POETString* v) {
     if (r1->get_enum() == SRC_STRING && 
        static_cast<POETString*>(r1)->get_content() == v->get_content())
        res = r1;
  }
  bool match_unknown(POETCode* input, CodeVar* ct, POETCode* args)
  {
     POETCode_ext* v2 = dynamic_cast<POETCode_ext*>(input);
     if (v2 != 0 && POETAstInterface::MatchAstTypeName(v2->get_content(), ct->get_entry().get_name()->toString(OUTPUT_NO_DEBUG))) {
         POETCode* children = v2->get_children();
         if (args == 0) { res = v2; return true; }
         else if (children==0) { return false; }
         if (apply(children, args)) {
             res = input; return true; }
     }
     return false;
  }
  bool matchCodeVar(POETCode *_r1, CodeVar* v2, POETCode* args)  {
     CodeVar* v1 = dynamic_cast<CodeVar*>(_r1);
     if (v1 != 0 && v1->get_entry() == v2->get_entry()) {
       if (args == 0) { return true; }
       apply(v1->get_args(),args);
       return (res != 0);
     }
     if (match_unknown(_r1, v2, args)) {
       if (debug_pattern()) {
         std::cerr << "Matched unknown: " << _r1->toString() << " with code template " << v2->toString() << "\n";
       }
       return true;
     }
     if (v1 == 0) { 
        if (debug_pattern()) 
           std::cerr << "Failed to match " << _r1->toString() << " with code template " << v2->toString() << "\n";
        res = 0; return false; 
     }
     POETCode* v1match = v1->invoke_match(v1->get_args());
     if (v1match != 0 && config==MATCH_AST_PATTERN)
        {
          if (debug_pattern()) 
            std::cerr << "invoking matching attr of code template: " << v1->toString() << "\n";
          return matchCodeVar(v1match, v2, args);
        }  
      return false;
  }
  virtual void visitUnknown (POETCode_ext* e) {
     if (r1 == e) res = r1; 
     else res = 0;
  }
  virtual void visitCodeVar(CodeVar* v2)  {
     if (matchCodeVar(r1, v2, v2->get_args())) res = r1;
     else res = 0;
  }
  virtual void visitLocalVar(LocalVar* v2) { 
      if (any == v2) { res = r1; return; }
      LvarSymbolTable::Entry e2 = v2->get_entry();
      if (e2.get_code() != 0 && e2.get_code() != v2) 
          { apply(r1, e2.get_code()); return; }
      else if (config==MATCH_AST_EQ) return; 
      if (lvar == 0)
         e2.set_code(r1); 
      else e2.set_code(lvar);
      res = e2.get_code();
      if (debug_pattern()) {
          if (res != 0) 
           std::cerr << "Matched variable " << r1->toString() << " with " << v2->toString() << "\n";
          else
           std::cerr << "Fail to match variable " << r1->toString() << " with " << v2->toString() << "\n";
      }
    }
  virtual void visitTuple( POETTuple* r2) {
       if (r1 == 0 || r1->get_enum() != SRC_TUPLE) { res = 0; return; }
       POETTuple* v1 =static_cast<POETTuple*>(r1);
       if (v1->size() != r2->size()) { res = 0; return; }
       POETCode* tmp = 0;
       for (unsigned i = 0; i < v1->size(); ++i) {
           if (! apply(v1->get_entry(i),r2->get_entry(i))) { res = 0; return; }
           tmp = fac->append_tuple(tmp, res);
       }
       res = tmp;
   }
  virtual void visitType( POETType* t) { res = match_Type(r1, t, false); }
  virtual void visitList(POETList* v2)  {
       if (debug_pattern()) 
           std::cerr << "Trying to match list " << r1->toString() << " with " << v2->toString() << "\n";
       POETList* v1 = dynamic_cast<POETList*>(r1);
       POETList* v2Rest = v2->get_rest();
       if (v1 == 0) { 
           if (v2Rest == 0) apply(r1, v2->get_first()); 
           else res = 0;
           return;
       }
       POETList* v1Rest = v1->get_rest();
       if (v2Rest == 0 && v1Rest == 0) {
           if (apply(v1->get_first(), v2->get_first()))
               res = ASTFactory::inst()->new_list(res, 0); 
       }
       else if (v2Rest == 0) {
           apply(r1, v2->get_first()); 
       }
       else if (v1Rest == 0) { 
          if (apply(EMPTY_LIST, v2Rest) && apply(v1->get_first(), v2->get_first()))
                 res = ASTFactory::inst()->new_list(res,0);
          else res = 0; 
       }
       else {
           POETCode* t1 = 0;
           if (apply(v1->get_first(), v2->get_first())) {
               t1 = res;
               if (apply(v1Rest,v2Rest)) 
                  res = ASTFactory::inst()->new_list(t1, res);
           }
       }
   }
  virtual void visitAssign(POETAssign* assign)  {
          POETCode* lhs = assign->get_lhs(); 
          if (apply(r1,assign->get_rhs())) {
             if (lvar != 0 && lhs->get_enum() == SRC_LVAR) {
                static_cast<LocalVar*>(lhs)->get_entry().set_code(lvar);
                res = lvar;
             }
             else res = assign_AST(r1, lhs);
          }
          else res = 0;
       }
  virtual void visitOperator(POETOperator* op) {
     switch (op->get_op()) {
     case POET_OP_DOT: eval_AST(op)->visit(this); break; 
     case TYPE_LIST1: 
         if (r1 == EMPTY_LIST) { res=0; break; } 
     case TYPE_LIST: {
         if (r1 == EMPTY_LIST) { res = r1; break; }
         POETCode* arg = op->get_arg(0);
         std::vector<POETCode*> match_res;
         POETList* r1_list = dynamic_cast<POETList*>(r1);
         POETCode* r1_first = (r1_list == 0)? r1 : r1_list->get_first();
         POETList* r1_rest = (r1_list == 0)? 0 : r1_list->get_rest();
         do {
            apply(r1_first,arg);
            if (res == 0)  { return; }
            match_res.push_back(r1_first);
            r1_list = r1_rest;
            r1_first = (r1_rest == 0)? 0 : r1_rest->get_first(); 
            r1_rest = (r1_rest == 0)? 0 : r1_rest->get_rest();
         } while (r1_list != 0);
         res= r1; //Vector2List(match_res);
         break;
       }
     case TYPE_TOR: {
         if (config == MATCH_AST_EQ) {
             unsigned num = 0;
             POETTypeTor* r1_tor = dynamic_cast<POETTypeTor*>(r1);
             if (r1_tor == 0 || ((num=r1_tor->numOfArgs()) != op->numOfArgs())) 
                { res = 0; return; }
             else {
                for (unsigned i = 0; i < num; ++i) {
                   apply( op->get_arg(i), r1_tor->get_arg(i));
                   if (res == 0) return;
                }
                res = r1; return;
             }
         }
         else for (unsigned i = 0; i < op->numOfArgs(); ++i)
         {
            POETCode* arg = op->get_arg(i);
            apply(r1, arg);
            if (res != 0) return;
         }
         return;
      }
     case POET_OP_POND: 
       {
          POETCode* d1 = eval_AST(op->get_arg(0));
          CodeVar* cvar = dynamic_cast<CodeVar*>(d1);
          if (cvar == 0)  {
             d1 = match_AST(r1,d1, MATCH_AST_PATTERN);
             if (d1 == 0) { res = 0; return; }
             cvar = dynamic_cast<CodeVar*>(d1);
             if (cvar == 0) CODE_UNDEFINED(d1->toString());  
          }
          if (matchCodeVar(r1, cvar, op->get_arg(1))) res = r1;
          else res = 0;
          return;
       }
     case POET_OP_VAR: if (lvar != 0) res = lvar; return;
     case POET_OP_XFORM: if (r1->get_enum() == SRC_XVAR) res = r1; return;
     case POET_OP_CODE: if (r1->get_enum() == SRC_CVAR) res = r1; return;
     case POET_OP_TUPLE: if (r1->get_enum() == SRC_TUPLE) res = r1; return;
     case POET_OP_EXP: {
           POETCode_ext* p = dynamic_cast<POETCode_ext*>(r1);
           if (p != 0) { 
              if (POETAstInterface::MatchAstTypeName(p->get_content(), "EXP"))
                 res=r1; 
              return; } 
           if (arrref->get_entry().get_code() != 0)
              { arrref->get_entry().get_code()->visit(this); if (res != 0) return; }
           if (parseBop->get_entry().get_code() != 0) 
              { parseBop->get_entry().get_code()->visit(this); if (res != 0) return; }
           if (parseUop->get_entry().get_code() != 0) 
              { parseUop->get_entry().get_code()->visit(this); if (res != 0) return; }
           if (exp_item->get_entry().get_code() != 0) 
              exp_item->get_entry().get_code()->visit(this); 
           if (exp_match->get_entry().get_code() != 0) 
              exp_match->get_entry().get_code()->visit(this); 
           return;
       }
     case POET_OP_RANGE:
      {
        POETCode *first = op->get_arg(0), *second = op->get_arg(1);
        assert(first != 0 && second != 0);
        if (any != first && !apply_Bop(POET_OP_GE, r1, eval_AST(first)))
              { res = 0; return; }
        if (any != second && !apply_Bop(POET_OP_LE,r1,eval_AST(second)))
             { res = 0; return; }
        if (first->get_enum() == SRC_ICONST || second->get_enum() == SRC_ICONST)
            res = AST2Iconst(r1);
        else res = r1; 
        return;
       }
     case POET_OP_MAP: {
         if (r1->get_enum() != SRC_MAP) { res = 0; return; }
         POETMap* m = static_cast<POETMap*>(r1);
         POETCode* first = op->get_arg(0), *second = op->get_arg(1);
         if (first != 0 && second != 0) {
           if (first != any || second != any) {
            for (POETMap::const_iterator p = m->begin(); p != m->end(); ++p) {
               std::pair<POETCode*,POETCode*> curpair = *p;
               if (!apply(curpair.first, first)) return; 
               if (!apply(curpair.second, second)) return; 
            }
           }
         }
         res = r1; return;
       }
     case POET_OP_UMINUS:  apply(r1, build_Uop(op->get_op(), op->get_arg(0), true)); break;
     case POET_OP_MINUS:   
     case POET_OP_PLUS:    
     case POET_OP_MULT:   
     case POET_OP_DIVIDE: 
     case POET_OP_MOD:  
          {
           POETCode* p = 0, *arg1 = op->get_arg(0), *arg2 = op->get_arg(1);
           if (AST2Iconst(arg1) && AST2Iconst(arg2)) 
               p = fac->new_iconst(apply_Bop(op->get_op(), arg1, arg2)); 
           else {
               CodeVar* cvar = dynamic_cast<CodeVar*>(parseBop->get_entry().get_code());
               if (cvar == 0) INCORRECT_CVAR(parseBop); 
               p= fac->append_tuple(fac->new_pair(fac->new_string(OpName[op->get_op()]),arg1), arg2);
               p = build_codeRef(cvar, p, true);
            }
            apply(r1, p); return;
          }
     case POET_OP_SEQ:
          eval_AST(op->get_arg(0));
          apply(r1,op->get_arg(1)); return;
     default:
          defaultVisit(op);
     }
   }
  virtual void visitXformVar( XformVar* v) 
   {  
      if (r1->get_enum() == SRC_XVAR)
      {
          if (static_cast<XformVar*>(r1)->get_entry() == v->get_entry()) res = r1;
      }
      else if (AST2Bool(v->eval(r1))) res = r1; 
   }
  virtual void defaultVisit(POETCode* f) {
      if (any == f) { res = r1; return; }
      POETCode* ff = eval_AST(f);
      if (ff == f) res = 0;
      else apply(r1,ff);  
   }
};

class AssignASTVisitor : public MatchASTVisitor 
{
  public:
  AssignASTVisitor() : MatchASTVisitor(MATCH_AST_PATTERN) {}
  virtual void visitLocalVar(LocalVar* v2) { 
      LvarSymbolTable::Entry e2 = v2->get_entry();
      LocalVarType t2 = e2.get_entry_type();
      switch (t2) {
       case LVAR_OUTPUT:
            ASSIGN_INCORRECT(r1, v2)
       case LVAR_TUNE_GROUP:
             e2.get_code()->visit(this);
             return;
       default: 
            if (lvar == 0) e2.set_code(r1); 
            else e2.set_code(lvar);
            res = r1;
      }
    }
  virtual void visitTupleAccess(TupleAccess* fc) {
       POETCode* tuple= eval_AST(fc->get_tuple());
       POETCode* access = eval_AST(fc->get_access()); 
       if (tuple != 0 && tuple->get_enum() == SRC_MAP) {
          POETMap* m = static_cast<POETMap*>(tuple);
          m->set(EvalTrace(access), (lvar == 0)? r1 : lvar);
           res = r1;
        }
       else defaultVisit(fc); 
  }
};
class OutdateTraceVars : public CollectInfoVisitor
{
 public:
  virtual void visitLocalVar( LocalVar* v)
     { 
       LvarSymbolTable::Entry e = v->get_entry();
       if (e.get_entry_type() == LVAR_TRACE) 
            e.set_entry_type(LVAR_TRACE_OUTDATE);
     }
};
class UpdateTraceVars : public CollectInfoVisitor
{
 public:
  virtual void visitLocalVar( LocalVar* v)
     { 
       LvarSymbolTable::Entry e = v->get_entry();
       if (e.get_entry_type() == LVAR_TRACE_OUTDATE) 
            e.set_entry_type(LVAR_TRACE);
     }
};

POETCode* assign_AST(POETCode* r1, POETCode* r2)
{ 
  OutdateTraceVars().apply(r2);
  AssignASTVisitor op;
  if (!op.apply(r1,r2)) ASSIGN_INCORRECT(r2, r1);
  POETCode* c = op.get_result();
  UpdateTraceVars().apply(r2);
  if (r2->get_enum() == SRC_LVAR && static_cast<LocalVar*>(r2)->get_entry().get_entry_type() == LVAR_TRACE) return r2;
  return c;
}

POETCode* match_AST(POETCode* input, POETCode* pattern, MatchOption config)
{  
   if (input == 0) return 0;
   if (input->get_enum() == SRC_ASSIGN)
   {
      if (pattern->get_enum() != SRC_ASSIGN) return 0;
      POETAssign* input_assign = static_cast<POETAssign*>(input); 
      POETAssign* pattern_assign = static_cast<POETAssign*>(pattern); 
      if (input_assign->get_lhs() == pattern_assign->get_lhs() && 
             match_AST(input_assign->get_rhs(), pattern_assign->get_rhs(), config))
            return input;
      return 0;
   }
   MatchASTVisitor op(config);
   if (!op.apply(input,pattern)) return 0;
   return op.get_result(); 
}


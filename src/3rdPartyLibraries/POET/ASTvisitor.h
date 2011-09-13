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

#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <ASTeval.h>
#include <ASTfactory.h>

class CollectInfoVisitor : public POETCodeVisitor, public EvaluatePOET
{
 public:
  virtual void visitList(POETList* l) 
     { 
         l->get_first()->visit(this) ; 
         if (l->get_rest() != 0)
               l->get_rest()->visit(this); 
     }
  virtual void visitTuple( POETTuple* v) 
     {
          for (unsigned i = 0; i < v->size(); ++i)
              v->get_entry(i)->visit(this);
     } 
  virtual void visitCodeVar(CodeVar* v) 
     { 
       if (v->get_args() != 0)
          v->get_args()->visit(this) ; 
       if (v->get_attr() != 0)
          v->get_attr()->visit(this) ; 
     }
  virtual void visitLocalVar(LocalVar* v) { 
          LvarSymbolTable::Entry e = v->get_entry();
          POETCode* r = e.get_code(); 
          if (r == 0) {
              if (e.get_entry_type() == LVAR_TRACE) TRACE_RECURSIVE(v)
              else SYM_UNDEFINED(v->toString())
          }
          if (r != v) {
             e.set_code(0); 
             try { r->visit(this); } 
             catch (POETOperator *c) { e.set_code(r); throw c; }
             e.set_code(r);
          }
        }
  virtual void visitXformVar( XformVar* v) 
    {  if (v->get_args() != 0) v->get_args()->visit(this); }
  virtual void visitOperator(POETOperator* v) 
    {  
       for (unsigned i = 0; i < v->numOfArgs(); ++i) {
           v->get_arg(i)->visit(this); 
       }
    }
  virtual void visitAssign(POETAssign* v) {  
       v->get_lhs()->visit(this); v->get_rhs()->visit(this);
    }
  virtual void visitTupleAccess(TupleAccess* v) 
    {  if (v->get_tuple() != 0) v->get_tuple()->visit(this); }
 public:
  void apply(POETCode* code) { code->visit(this); }
};


class BackwardCollectInfoVisitor : public CollectInfoVisitor
{
 public:
  virtual void visitList(POETList* l) 
     { 
         if (l->get_rest() != 0)
             l->get_rest()->visit(this); 
         l->get_first()->visit(this) ; 
     }
  virtual void visitTuple( POETTuple* v) 
     {
          for (int i = v->size()-1; i >= 0; --i) 
             v->get_entry(i)->visit(this);
     } 
};


class ReplInfoVisitor : public EvaluatePOET, public POETCodeVisitor
{
 protected:
  POETCode* res;
  bool inList;
 public:
  ReplInfoVisitor() : inList(false) { res = 0; }
  virtual void defaultVisit(POETCode* c) { res = c; }
  virtual void visitLocalVar(LocalVar* v); 
  virtual void visitAssign(POETAssign* assign)  {
      POETCode* lhs = apply(assign->get_lhs());
      POETCode* rhs = apply(assign->get_rhs());
      if (lhs == assign->get_lhs() && rhs == assign->get_rhs()) 
         res = assign;
      else
         res = new POETAssign(lhs,rhs);
    }
  virtual void visitList(POETList* l) 
     { 
       POETCode* first = l->get_first(), *rest = l->get_rest();
       apply(first);
       POETCode* r1 = res;
       if (r1 == 0) { 
          if (rest == 0) res = 0;
          else apply(rest);
       }
       else if (rest != 0) {
		   bool inList_save = inList;

		   inList = true;
		   apply(rest);
		   inList=inList_save;

		   POETCode* r2 = res;
		   res = MakeXformList(r1,r2,false); 
		   if (res == 0) res = fac->new_list(r1,r2); 
           else res->visit(this);
       }
       else if (inList) res = fac->new_list(r1,0);
       else res = r1;
     }
  virtual void visitTuple(POETTuple* v) 
     {
       int size = v->size();
       std::vector<POETCode*> cur(size);
       bool change=false;
       for (int i = 0; i < size; ++i) {
           POETCode* elem1 = v->get_entry(i);
           POETCode* elem2 = apply(elem1);
           if (elem2 != elem1) change = true;
           if (elem2 == 0) elem2 = EMPTY;
           cur[i] = elem2;
       }
       if (!change)  { res = v; return;}
       assert(size >= 2);
       res = 0;
       for (int i = 0; i < size; ++i)
          res = fac->append_tuple(res, cur[i]);
     } 
  virtual void visitCodeVar( CodeVar* v) 
     { 
       if (v->get_args() != 0) {
          POETCode *nargs = apply(v->get_args());
          if (nargs != v->get_args())
              res = fac->build_codeRef(v, nargs, true);
          else res = v;
       }
       else
          res = v;
     }
  POETCode* apply(POETCode* code) 
     { res = code; code->visit(this); return res; }
};

inline void ReplInfoVisitor:: visitLocalVar(LocalVar* v) { 
          LvarSymbolTable::Entry e = v->get_entry();
          POETCode* save = e.get_code(); 
          if (save == 0)  {
              if (e.get_entry_type() == LVAR_TRACE) TRACE_RECURSIVE(v)
              else { res = v; return; }
          }
          if (save != v) {
             e.set_code(0);
             res = apply(save);
             if (v != res) {
               if (user_debug)
                  std::cerr << "setting LocalVar " << v->toString() << " with " << res->toString() << "\n";
                e.set_code(res);
                if (e.get_entry_type() == LVAR_TRACE)
                    res = v;
             }
             else if (e.get_code() == 0) {
               if (user_debug)
                 std::cerr << "LocalVar unchanged: " << v->toString() << "\n";
                e.set_code(save);
             }
          }
        }

#endif

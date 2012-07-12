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
#include <assert.h>
#include <string>

extern bool debug_parse();


extern "C" POETCode* make_sourceVector(POETCode *v1, POETCode* v2);

class PrintASTInfo : public POETCodeVisitor
{
public:
 PrintASTInfo(std::ostream& _out) : out(_out), col(0),line(0) {}
 void apply (POETCode* output)
 {
  while (output != 0) {
     POETList* outputList = dynamic_cast<POETList*>(output);
     if (outputList != 0) {
        apply(outputList->get_first());
        output = outputList->get_rest();
     }
     else {
        if (output->get_enum() == SRC_STRING) {
           out << output->toString(OUTPUT_NO_DEBUG);
        }
        else {
           POETCode* res = eval_AST(output);
           res->visit(this);
        }
        output = 0;
    }
  }
 }

private: 
 std::ostream& out;
 int col, line; 

 void print_linebreak() { out << "\n"; ++line;
                 for (int i = 0; i < col; ++i) out << " ";  }

 virtual void defaultVisit(POETCode* c) { out << c->toString(); }
 virtual void visitMap(POETMap* m) 
  {
    out << "MAP:{\n";
    col += 2;
    for (POETMap::const_iterator p=m->begin(); p != m->end(); ++p)
    {
        (*p).first->visit(this);
        out << "->" ;
        (*p).second->visit(this);
        out << "\n";
    }
    out << "}";
    col -= 2;
  }
 virtual void visitLocalVar(LocalVar* v)
  {
     POETCode* res = v->get_entry().get_code(); 
     if (res != 0 && res != v) {
         v->get_entry().set_code(0);
         out << v->get_entry().get_name()->toString(OUTPUT_NO_DEBUG) << "={";
         res->visit(this);
         out << "}";
         v->get_entry().set_code(res);
     }
  }
 virtual void visitCodeVar(CodeVar* v) 
  {
     out << v->get_entry().get_name()->toString(OUTPUT_NO_DEBUG);
     if (v->get_args() != 0) 
     {
         out << "#";
         col += 2;
         v->get_args()->visit(this);
         col-=2; 
      }
          
     if (v->get_attr() != 0)
       { out << "#"; col += 2;  v->get_attr()->visit(this); col -= 2; }
  }
 virtual void visitTuple(POETTuple* l)
  {
    out << "("; print_linebreak();
    int size=l->size();
    for (int i = 0; i < size; ++i) {
       POETCode* cur = l->get_entry(i);
       if (cur == 0) out << "NULL,";
       else if (cur->get_enum() == SRC_LIST) 
           { col+=2; out << "(";  cur->visit(this); 
             out << ")"; col-=2; }
       else cur->visit(this); 
       if (i < size-1) { out << ","; print_linebreak(); }
     }
     out << ")"; 
  }
 virtual void visitList(POETList* l)
  {
     POETCode* first = l->get_first();
     int prevline = line;
     if (first->get_enum() == SRC_LIST) 
         {out << "("; first->visit(this); out << ")"; }
     else first->visit(this);
     if (line > prevline) print_linebreak(); 
     POETCode* rest = l->get_rest();
     if (rest != 0) {
        rest->visit(this);
     }
     else out << " NULL";
  }
 
};

void print_AST(std::ostream& out, POETCode* code)
{
  PrintASTInfo(out).apply(code);
  out << "\n";
}

class CodeGenVisitor : public CollectInfoVisitor
{
 public:
  CodeGenVisitor(std:: ostream& _out, XformVar* invoke) 
   : out(_out), output_xform(invoke), 
    align(0), start_pos(0),cur_pos(0),all_space_sofar(1),listsep(0),listelem(0) 
   {
     if (space == 0) space = ASTFactory::inst()->new_string(" ");
     if (comma == 0) comma = ASTFactory::inst()->new_string(",");
   }

 private:
  std:: ostream& out;
  XformVar* output_xform;
  int align, start_pos, cur_pos, all_space_sofar;
  POETCode* listsep;
  CodeVar* listelem;

  static POETString* space, *comma;

  void output_content(const std::string& r, const std::string& orig)
    {
        for (unsigned j = 0; j < orig.size(); ++j)
        {
         if (orig[j] == '\n') 
             { start_pos=0; all_space_sofar=1; }
         else if (orig[j] != ' ') all_space_sofar = 0;
         else if (all_space_sofar)  ++start_pos;
       }
       for (unsigned i = 0; i < r.size(); ++i)
        {
         out << r[i];
         if (r[i] == '\n') 
             cur_pos=0;
         else  
             ++ cur_pos;
       }
    }
  void output(POETString* s) {  
        ASTFactory* fac = ASTFactory::inst();
        std::string r = s->get_content();
        if (r == "\n") {
            for (int i = 0; i < align; ++i) r = r + " ";
            s = fac->new_string(r);
        }
        if (output_xform == 0) 
             output_content(r,r); 
        else {
           POETCode* args = fac->new_pair(s, fac->new_iconst(cur_pos));
           if (!match_parameters(output_xform->get_entry().get_param(), args,
                         MATCH_PAR_MOD_CODE))
                INCORRECT_XFORM_DEF("output(string,cur_pos)", output_xform->toString()); 
           XformVarInvoke invoke(output_xform->get_entry(), output_xform->get_config(), args);
           POETCode* res = invoke.eval(0);
           output_content(res->toString(OUTPUT_NO_DEBUG),r);
        }
     }
  virtual void visitString(POETString* s) 
     {  output(s); }
  virtual void visitIconst(POETIconst* s) 
     {  
          std:: stringstream tmp;
          tmp << s->get_val(); 
          output(ASTFactory::inst()->new_string(tmp.str()));
     }
  virtual void visitList(POETList* l) 
    {
       POETCode* cur = l->get_first();
/*
       if (cur->get_enum() != SRC_LIST &&
           listelem != 0 && !match_AST(cur, listelem,MATCH_AST_EQ)) {
          cur = ASTFactory::inst()->new_codeRef(listelem->get_entry(), cur);
       }
*/
       cur->visit(this) ; 
       if (l->get_rest() != 0) {
           if (listsep != 0)
              listsep->visit(this);
           l->get_rest()->visit(this); 
       }
    }
  virtual void visitTuple(POETTuple* v)
    {
         POETCode* _listsep = listsep;
         CodeVar* _listelem = listelem;
         listsep = 0; listelem=0;
         int size = v->size();
          if (size == 0) return;
          v->get_entry(0)->visit(this);
          for (int i = 1; i < size; ++i)  {
             output(comma); 
             v->get_entry(i)->visit(this);
          }
         listsep = _listsep; listelem=_listelem;
    }
  virtual void visitCodeVar(CodeVar* v) 
   {
     try {
       POETCode* _listsep = listsep;
       CodeVar* _listelem = listelem;
       POETCode* result = v->invoke_output(v->get_args());
       if (result != 0 && result != v) { 
           listsep = 0; listelem=0; 
           result->visit(this); 
           listsep = _listsep; listelem=_listelem;
           return; 
       }
       CvarSymbolTable::Entry e = v->get_entry();
       LvarSymbolTable *local = 0;
       POETCode* f = 0;
       POETCode *parse = e.get_parse();
       if (parse != 0 && parse->get_enum()==SRC_OP) { 
           POETOperator* op = static_cast<POETOperator*>(parse);
           if (op->get_op() == POET_OP_LIST || op->get_op()==POET_OP_LIST1)  {
               listsep = op->get_arg(1);
               listelem=dynamic_cast<CodeVar*>(op->get_arg(0));
               f = v->get_args();
            }
       }
       if (f == 0) { 
         listsep = 0; listelem=0;
         f = e.get_code();
         if (f == 0) { 
               if (parse==0) { CODE_SYNTAX_UNDEFINED(v->toString()); }
               else f = v->get_args(); 
         }
         else {
            local = e.get_symTable();
            if (local != 0) 
               local->push_table(false);
            POETCode* pars = e.get_param();
            if (pars != 0 && !match_parameters(pars, v->get_args(),MATCH_PAR_MOD_CODE))   
               CVAR_MISMATCH(v,pars, v->get_args());
            v->set_attr();
         }
       }
       if (f == 0) { CODE_SYNTAX_UNDEFINED(v->toString()); }
       int save = align;
       align = start_pos;
       f->visit(this); 
       align = save;
       if (local != 0)
           local->pop_table();
       listsep = _listsep; listelem=_listelem;
     }
     catch (Error err) { 
             std::cerr << " From unparsing code template " << SHORT(v->toString(),500) << "\n"; 
             throw err;
          }
     }
  void defaultVisit(POETCode* fc) 
     { 
       if (fc->get_enum() == SRC_PROGRAM) {
             static_cast<POETProgram*>(fc)->write_to_file(out); 
             return;
       }
       POETCode* fc_eval = eval_AST(fc); 
       if (fc_eval == fc) 
       {
          std::string res = fc->toString(OUTPUT_NO_DEBUG);
          output_content(res, res);
       }
       else fc_eval->visit(this);
     }
  virtual void visitLocalVar(LocalVar* var) 
    {
       LvarSymbolTable::Entry e = var->get_entry();
       POETCode *restr = e.get_restr();
       if (restr == 0) {
          CollectInfoVisitor::visitLocalVar(var);
          return;
       }
       if (restr->get_enum() == SRC_CVAR) {
          CodeVar* cvar = static_cast<CodeVar*>(restr);
          POETCode* code = e.get_code();
          if (code!=0 && !match_AST(code, cvar,MATCH_AST_EQ)) {
             CodeVar* tmp = ASTFactory::inst()->new_codeRef(cvar->get_entry(), code); 
             tmp->visit(this); 
             return; 
          }
       }
       else if (restr->get_enum() == SRC_OP) {
           POETOperator* op = static_cast<POETOperator*>(restr);
           if (op->get_op() == POET_OP_LIST || op->get_op()==POET_OP_LIST1)  {
               POETCode* _listsep = listsep;
               CodeVar* _listelem = listelem;
               int save = align;
               align = start_pos;
               listsep = op->get_arg(1);
               listelem=dynamic_cast<CodeVar*>(op->get_arg(0));
               CollectInfoVisitor::visitLocalVar(var);
               align = save; listsep = _listsep; listelem = _listelem;
               return;
            }
       }
       CollectInfoVisitor::visitLocalVar(var);
    }

   virtual void visitXformVar( XformVar* v) { defaultVisit(v); }
   virtual void visitOperator(POETOperator* v) { defaultVisit(v); }
   virtual void visitTupleAccess(TupleAccess* v) { defaultVisit(v); }

};

POETString* CodeGenVisitor::space=0;
POETString* CodeGenVisitor::comma=0;

void code_gen(std:: ostream& out, POETCode *code, POETCode* output_invoke) 
{
 XformVar *output_xform = 0;
 if (output_invoke!=0)
 {
    output_xform = dynamic_cast<XformVar*>(output_invoke);
    if (output_xform == 0) 
    {
       CodeVar* output_cvar = dynamic_cast<CodeVar*>(output_invoke);
       if (output_cvar == 0)
          XFORM_CONFIG_INCORRECT("UNPARSE", output_invoke->toString());
       code = ASTFactory::inst()->new_codeRef(output_cvar->get_entry(),code); 
    }
 }
 CodeGenVisitor op(out, output_xform);
 code->visit(&op); 
}


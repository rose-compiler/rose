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

//********************************************************************
//  Driver program for code generation from POET specefications
//********************************************************************

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <list>

#include <stdio.h>
#include <stdlib.h>

#include <poetAST.h>
#include <ASTvisitor.h>
#include <assert.h>

extern EvaluatePOET* evalPOET;
extern POETProgram* curfile;
typedef enum {DEBUG_NONE, DEBUG_XFORM = 1, DEBUG_PATTERN = 2, DEBUG_TIME = 4, DEBUG_PARSE= 8, DEBUG_LEX = 16}   
    DebugEnum;
int debug=DEBUG_NONE;
bool debug_time() { return debug & DEBUG_TIME; } 
bool debug_parse() { return debug & DEBUG_PARSE; }
bool debug_lex() { return debug & DEBUG_LEX; }
bool debug_xform() { return debug & DEBUG_XFORM; }
bool debug_pattern() { return (debug & DEBUG_PATTERN) || user_debug > 5; }

static  std::map<std::string, POETCode*> parmap;
static std::list<std::pair<LvarSymbolTable*,bool> > tmpScope;
std::list<std::string> lib_dir;
bool redefine_code = false;

extern "C" FILE* yyin;
extern "C" void yyparse();
extern int yylineno;
extern int yydebug;
extern "C" LexState lexState;
extern "C" YaccState yaccState;

extern const char* yytext;
extern "C" void yyerror(const char* msg)
{ 
  std::cerr << "Line " << yylineno << " of file " << curfile->get_filename() << ": " << msg << " : " << yytext << "\n"; 
  exit(1);
}



template <class Entry>
void set_code_xform_parameters( Entry e, POETCode* par) 
{
   POETCode* p = e.get_param();
   if (p == 0) 
      e.set_param(par); 
   else {
     if (p != par && !match_AST(p, par, MATCH_AST_EQ))  {
       std::cerr << "par=" << par << "; p = " << p << "\n";
        SYM_DIFF_DEFINED(par->get_className() + ":" + par->toString(OUTPUT_VAR_VAL), p->get_className() + ":" + p->toString(OUTPUT_VAR_VAL)); 
     }
     return;
   }
   switch (par->get_enum()) {
   case SRC_LVAR:  
      static_cast<LocalVar*>(par)->get_entry().set_code(ASTFactory::inst()->new_iconst(0));
      break;
   case SRC_TUPLE: {
      POETTuple* vec = static_cast<POETTuple*>(par);
      for (unsigned i = 0; i < vec->size(); ++i)
       {
         LocalVar* cur_par = dynamic_cast<LocalVar*>(vec->get_entry(i));
         assert (cur_par != 0);
         cur_par->get_entry().set_code(ASTFactory::inst()->new_iconst(i) );
       }
      break;
   }
   default:
      assert(false);
   }
}


void pop_scope(POETCode* s) 
{
  if (s == 0) {
     assert (tmpScope.size());
     tmpScope.pop_front();
  }
  else switch (s->get_enum()) {
     case SRC_XVAR:
     case SRC_CVAR:
        assert (tmpScope.size());
        tmpScope.pop_front();
        break;
     default: assert(0);
  }
}

void push_scope(POETCode* s, bool insert)
{
     switch (s->get_enum()) {
     case SRC_XVAR: {
         LvarSymbolTable* t = static_cast<XformVar*>(s)->get_entry().get_symTable();
         if (t == 0) {
               std::cerr << "Error: xform routine undefined: " << s->toString() << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); 
         }
         tmpScope.push_front(std::pair<LvarSymbolTable*,bool>(t,insert));
         break;
       }
     case SRC_CVAR: {
         LvarSymbolTable* t = static_cast<CodeVar*>(s)->get_entry().get_symTable();
         assert(t != 0);
         tmpScope.push_front(std::pair<LvarSymbolTable*,bool>(t,insert));
         break;
      }
     default: assert(0);
     } 
}

inline LvarSymbolTable* top_scope()
{
  if (!tmpScope.size()) return 0;
  return tmpScope.front().first;
}

inline POETCode* find_code_or_xform_var(POETCode* name)
{
     POETCode* res = curfile->find_macroVar(name);
     if (res != 0) return res;
     ASTFactory* fac = ASTFactory::inst();
     CvarSymbolTable::Entry ge;
     if (fac->find_codeDef(name, ge))
         return fac->new_codeRef(ge, 0); 
     res= fac->find_xformDef(name, false);
     if (res != 0) return res; 
     return 0;
}

inline POETCode* find_global_var(POETCode* name)
{
     POETCode* res = curfile->find_paramVar(name);
     if (res != 0) return res;
     res = curfile->find_traceVar(name);
     if (res != 0) return res;
     res = curfile->find_evalVar(name); 
     if (res != 0) return res;
     return 0;
}
 
extern "C" POETCode* make_varRef(POETCode* name, int config) 
{
  try {
  ASTFactory* fac = ASTFactory::inst();
  if (name == fac->make_any()) return name;
  switch (config) {
  case CODE_VAR: return curfile->make_codeRef(name, 0);
  case XFORM_VAR: return curfile->make_xformVar(name);
  case CODE_OR_XFORM_VAR: {
     POETCode* res = find_code_or_xform_var(name);
     if (res != 0) return res; 
     CODE_OR_XFORM_UNDEFINED(name->toString()); 
  }
  case GLOBAL_SCOPE: {
     POETCode* res = find_code_or_xform_var(name);
     if (res != 0) return res; 
     res = find_global_var(name);
     if (res != 0) return res;
     SYM_UNDEFINED(name->toString());
   }
  case GLOBAL_VAR: {
     POETCode* res = find_code_or_xform_var(name);
     if (res != 0) return res; 
     res = find_global_var(name);
     if (res != 0) return res;
     return curfile->make_evalVar(name);
  }
  case ID_DEFAULT: {
     LvarSymbolTable* local = top_scope(); 
     POETCode* res = find_code_or_xform_var(name);
     if (res != 0) return res; 
     if (local == 0) {
        res = find_global_var(name);
        if (res != 0) return res;
        return curfile->make_evalVar(name);
     }
     return POETProgram::make_localVar(local, name, LVAR_REG);
  }
  case ASSIGN_VAR: {
     LvarSymbolTable* local = top_scope(); 
     POETCode* res = find_code_or_xform_var(name);
     if (res != 0) VAR_ASSIGN_ERROR(name->toString());
     if (local == 0) {
        res = find_global_var(name);
        if (res != 0) return res;
        return curfile->make_evalVar(name);
     }
     return POETProgram::make_localVar(local, name, LVAR_REG);
  }
  default:
     std::cerr << "something is wrong with variable creation config: " << name->toString() << " config = " << config << "\n";
     std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n";
     assert(0);
   }
  }
  catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" POETCode* make_attrAccess(POETCode* scope, POETCode* name2)
{
  try { return curfile->make_attrAccess(scope,name2); }
  catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" POETCode* make_macroVar(POETCode* id)
{ return curfile->make_macroVar(id); }

extern "C" POETCode* make_localPar(POETCode* id, POETCode* restr, LocalVarType type)
{ 
 try {
    LvarSymbolTable* local = top_scope();
    assert(local != 0);
    LocalVar*  res = local->insert(id, type);
    POETCode* restr1 = res->get_entry().get_restr(); 
    if (restr != 0 && restr1 != 0 && restr != restr1 && !match_AST(restr,restr1, MATCH_AST_EQ))
    {
       SYM_DIFF_DEFINED(id->toString() + ":" + restr->get_className() + ":" + restr->toString(),restr1->get_className() + ":" + restr1->toString());
        return res;
    }
    if (restr != 0) res->get_entry().set_restr(restr);
    return res;
  }
  catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}
extern "C" void* insert_parameter(POETString* name)
{ return curfile->insert_parameterDecl(name, yylineno); }
extern "C" void set_param_type(ParameterDecl* param, POETCode* type)
{  if (param->get_type() != 0 && param->get_type() != type) {
       std::cerr << "Error: parameter already defined differently: " << param->toString() << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n";
       exit(1);
    }
    param->set_type(type); 
 }
extern "C" void set_param_default(ParameterDecl* param, POETCode* init)
{  param->set_default(init); }
extern "C" void set_param_parse(ParameterDecl* param, POETCode* parse)
{  param->set_parse(parse); }
extern "C" void set_param_message(ParameterDecl* param, POETCode* msg)
{  param->set_message(msg); }

extern "C" void finish_parameter(ParameterDecl* param)
{
  try {
    POETCode* code = 0;
    std::string name = param->get_name();
    if (parmap.find(name) != parmap.end()) { 
       code = parmap[name]; 
       assert(code != 0);
       param->set_val(code, true);
    }
    else { 
          code = param->get_default();
         if (code == 0) code = EMPTY;
         else code = eval_AST(code);
         param->set_val(code, false); }
  }
  catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" void insert_source(POETCode* code)
{ curfile->insert_sourceCode(code); }
extern "C" void insert_eval(POETCode* code)
{ curfile->insert_evalDecl(code,yylineno); }
extern "C" void insert_trace(LocalVar* code)
{  curfile->insert_traceDecl(code, yylineno); }
extern "C" void eval_define(LocalVar* var, POETCode* code)
{ 
 try { curfile->eval_define(var, code); }
  catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" void* insert_input()
{  return curfile->insert_inputDecl(yylineno);  }
extern "C" void set_input_annot(ReadInput* read, POETCode* annot)
{ read->set_annot(annot); }
extern "C" void set_input_cond(ReadInput* read, POETCode* cond)
{ read->set_cond(cond); }
extern "C" void set_input_debug(ReadInput* read, POETCode* debug)
{ read->set_debug(debug); }
extern "C" void set_input_target(ReadInput* read, POETCode* var)
{ read->set_var(var); }
extern "C" void set_input_codeType(ReadInput* read, POETCode* type)
{ read->set_type(type); }
extern "C" void set_input_syntax(ReadInput* read, POETCode* langFiles)
{ read->set_syntaxFiles(langFiles); }
extern "C" void set_input_files(ReadInput* read, POETCode* inputFiles)
{ read->set_inputFiles(inputFiles); }
extern "C" void set_input_inline(ReadInput* read, POETCode* inputInline)
{ read->set_inputInline(inputInline); }

extern "C" void* insert_output()
{  return curfile->insert_outputDecl(yylineno); }
extern "C" void set_output_target(WriteOutput* write, POETCode* exp)
{ write->set_outputExp(exp); }
extern "C" void set_output_cond(WriteOutput* output, POETCode* cond)
{ output->set_cond(cond); }
extern "C" void set_output_syntax(WriteOutput* write, POETCode* langFiles)
{ write->set_syntaxFiles(langFiles); }
extern "C" void set_output_file(WriteOutput* write, POETCode* outputFile)
{ write->set_outputFile(outputFile); }

extern "C" void* insert_code(POETCode* id)
{
  CodeVar* res = curfile->make_codeRef(id);
  curfile->insert_codeDecl(res);
  push_scope(res,true);
  return res;
}
extern "C" XformVar* insert_xform(POETCode* xvar)
{ 
  XformVar* res = curfile->insert_xformDecl(xvar);
  while (tmpScope.size())
       pop_scope(0);
  push_scope(res,true);
  return res;
}

extern "C" POETCode* make_empty() { return EMPTY; }

extern "C" POETCode* make_any() 
{ return  ASTFactory::inst()->make_any(); }

extern "C" POETCode* make_sourceVector2(POETCode *v1, POETCode* v2)
{ try { return ASTFactory::inst()->new_pair(v1,v2); }
  catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" void* make_tupleAccess( POETCode* f, POETCode* args)
{  try {return curfile->make_tupleAccess(f,args); }
  catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" void* 
make_dummyOperator( POETOperatorType t)
{ return ASTFactory::inst()->new_op(t); }

extern "C" void* 
make_type( POETTypeEnum t)
{ return ASTFactory::inst()->new_type(t); }

extern "C" void* 
make_typeNot( POETCode* arg)
{ return ASTFactory::inst()->new_typeNot(arg); }

extern "C" void* 
make_typeUop( POETOperatorType t, POETCode* arg)
{ return ASTFactory::inst()->new_typeUop(t,arg); }

extern "C" void* 
make_typeTor( POETCode* arg1, POETCode* arg2)
{ 
  return POETProgram::make_typeTor(arg1,arg2);
}

extern "C" void* 
make_sourceQop( POETOperatorType t, POETCode* arg1, POETCode* arg2, POETCode* arg3, POETCode* arg4)
{ return new POETQop(t, arg1, arg2, arg3, arg4, curfile->get_filename(),yylineno); } 

extern "C" void* 
make_sourceTop( POETOperatorType t, POETCode* arg1, POETCode* arg2, POETCode* arg3)
{ return new POETTop(t, arg1, arg2, arg3); }

extern "C" void* 
make_ifElse( POETCode* arg1, POETCode* arg2, POETCode* arg3)
{ return new POETEvalExp(new POETTop(POET_OP_IFELSE, arg1, arg2, arg3), curfile->get_filename(), yylineno); }

extern "C" void* 
make_sourceBop( POETOperatorType t,  POETCode* arg1, POETCode* arg2)
{ 
  try {
      return curfile->make_Bop(t,arg1, arg2, yylineno); 
  } 
  catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" void* 
make_sourceUop( POETOperatorType t,  POETCode* arg)
{ try {return curfile->make_Uop(t,arg); }
  catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" void* 
make_sourceStmt( POETCode* arg)
{ try {return new POETEvalExp(arg, curfile->get_filename(), yylineno); } 
  catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}


/*
extern "C" void* 
make_sourceOperator( POETOperatorType t,  POETCode* args)
{ try {return curfile->make_operatorInvocation(t,args, yylineno, false); } 
  catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}
*/
  
extern "C" void* make_sourceAssign( POETCode* lhs, POETCode* rhs)
{ return new POETAssign(lhs, rhs); }

extern "C" void* strip_listSingle(POETCode* item)
{
   if (item->get_enum() == SRC_LIST) {
      POETList* l = static_cast<POETList*>(item);
      if (l->get_rest() == 0) return l->get_first();
   }
   return item; 
}

extern "C" void* make_typelist2( POETCode* car, POETCode *cdr)
{ 
  POETList* res = ASTFactory::inst()->new_list(car, cdr);
  return res;
}

extern "C" void* make_inputlist( POETCode* car, POETCode *cdr)
{ 
  POETList* res = new POETInputList(car,cdr);
  return res;
}

extern "C" void* make_inputlist2( POETCode* car, POETCode *cdr)
{ 
  POETList* cdrlist = dynamic_cast<POETList*>(cdr);
  if (cdr != 0 && cdrlist == 0) cdrlist = new POETInputList(cdr, 0);
  POETList* res = new POETInputList(car,cdrlist);
  return res;
}

extern "C" void* make_xformList( POETCode* car, POETCode *cdr)
{ 
  if (cdr != 0) {
     POETCode* res = MakeXformList(car,cdr);
     if (res != 0) return res;
  }
  POETList* res = ASTFactory::inst()->new_list(car, cdr);
  if (yaccState == YACC_CODE)
       return evalPOET->apply_tokens(res);
  return res;
}

extern "C" POETCode* make_sourceVector(POETCode *v1, POETCode* v2)
{ if (v1 == 0) return v2;
assert(v2 != 0);
  return ASTFactory::inst()->append_tuple(v1,v2);  }
extern "C" POETCode* make_Iconst1(int val)
{ return ASTFactory::inst()->new_iconst(val); }
extern "C" POETCode* negate_Iconst(POETIconst* ival)
{
   ival->set_val(- ival->get_val());
   return ival;
}
extern "C" POETCode* make_Iconst( char* text, int len)
{
  return curfile->make_Iconst(text, len, lexState&LEX_INPUT);
}
POETCode* make_sourceIntegers( char* text, int len)
{
  char* p = text, *p1 = p; 
  POETCode* r = 0;
  ASTFactory* fac = ASTFactory::inst();
  for (int i = 0; i < len; ++p, ++i) {
     if ((*p) == ',') {
        if (r == 0) r = make_Iconst(p1,p-p1);
        else r = fac->append_tuple(r, make_Iconst(p1, p-p1)); 
        p1 = p+1;
     }
  }
  if (p1 < p) {
     if (r == 0) r = make_Iconst(p1,p-p1);
     else r = fac->append_tuple(r, make_Iconst(p1, p-p1)); 
  }
  return r;
}

extern "C" POETCode* make_sourceString( const char* text, int len)
{
  std:: string r;
  const char* p = text; 
  for (int i = 0; (len < 0 || i < len) && (*p) != 0; ++i) {
     if ((*p) == '\\' && (len < 0 || i+1 < len)) {
       switch (*(p+1)) {
       case 't': r.push_back('\t'); p+=2; ++i; continue;
       case 'r':
       case 'n': r.push_back('\n'); p+=2; ++i; continue;
       case '"': r.push_back('"'); p +=2; ++i; continue;
       case '\\': r.push_back('\\'); p += 2; ++i; continue;
      }
     }      
    r.push_back(*p);  
    ++p;
  } 
  return ASTFactory::inst()->new_string(r);
}

extern "C" POETCode* make_codeMatch(POETCode* content, POETCode* match)
{ return curfile->make_Bop(POET_OP_TYPEMATCH, content, match, yylineno); }
extern "C" POETCode* make_codeMatchQ(POETCode* content, POETCode* match)
{ return curfile->make_Bop(POET_OP_TYPEMATCH_Q, content, match, yylineno); }
extern "C" POETCode* make_annot(POETCode* parseType)
{
  return curfile->make_Uop(POET_OP_ANNOT, parseType, yylineno);
}

extern "C" POETCode* make_traceVar(POETCode* name, POETCode* inside)
{ 
  assert(name != 0); 
 try {
  POETCode* res = find_code_or_xform_var(name);
  if (res != 0) SYM_ALREADY_DEFINED(name->toString()); 
  res = find_global_var(name);
  if (res != 0) SYM_ALREADY_DEFINED(name->toString());
  LocalVar* var1 = curfile->make_traceVar(name);
  if (inside != 0) {
     LocalVar* var2 = dynamic_cast<LocalVar*>(inside);
     assert (var1 != 0 && var2 != 0);
     var1->get_entry().set_restr(var2);
  }
  return var1;
  } catch (Error err) { std::cerr << "At line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" POETCode* set_local_static(POETCode* id, POETCode* code, 
                              LocalVarType t, POETCode* restr, bool insert)
{ assert(id != 0); 
  try {
  LvarSymbolTable* local = top_scope();
  LocalVar* lvar = curfile->set_local_static(local,id, t, code, restr, insert);
  return lvar;
  }
  catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" POETCode* make_codeRef(POETCode* cvar, POETCode* arg, ParseID config)
{  return curfile->make_Bop(POET_OP_POND,cvar, arg, yylineno); }

extern "C" void set_code_inherit(CodeVar* id, POETCode* attr)
{
  try {
    LocalVar* attrVar = dynamic_cast<LocalVar*>(set_local_static(attr,0,LVAR_ATTR, 0,true));
    LocalVar* v = id->get_entry().get_inherit_var();
    if (v != 0 && v != attrVar) 
        INHERIT_MULTI_DEFINED(id,attrVar,v);
    id->get_entry().set_inherit_var(attrVar);
  }
  catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" POETCode* set_code_attr(CodeVar* id, POETCode* attr, POETCode* val)
{
  LocalVar* attrVar = dynamic_cast<LocalVar*>(set_local_static(attr,val,LVAR_ATTR, val,true));
  assert(attrVar != 0);
  id->get_entry().append_attr(attrVar);
   return attrVar;
}

extern "C" void set_code_params(CodeVar* id, POETCode* par)
{  try { if (par != 0) set_code_xform_parameters(id->get_entry(), par); }
   catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}
extern "C" void set_xform_params(XformVar* id, POETCode*  par)
{  assert(id != 0); 
   try { if (par != 0) set_code_xform_parameters(id->get_entry(), par); }
   catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" void set_code_parse(CodeVar* id, POETCode* code)
{ assert(id != 0); 
  try { id->get_entry().set_parse(code); }
  catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

extern "C" void set_code_lookahead(CodeVar* id, POETCode* code)
{ assert(id != 0); 
  try { id->get_entry().set_lookahead(AST2Int(code)); }
  catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}
extern "C" void set_code_def(CodeVar* id, POETCode* code)
{ assert(id != 0); 
  try { curfile->set_codeDef(id,code);  pop_scope(id); }
  catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}
extern "C" void set_xform_def(XformVar* id, POETCode* code)
{ assert(id != 0); 
  try { curfile->set_xformDef(id,code);  pop_scope(id); }
  catch (Error err) { std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); }
}

int initialize(int argc, char** argv)
{
  std::string arg1;
  if (argc <= 1 || ((arg1=std::string(argv[1]))=="-help") || (arg1=="-h") ) {
     std:: cerr << "Usage: ";
     std:: cerr << argv[0] << " [-bhv] {-L<dir>} {-p<name>=<val>} <poet_file1> ... <poet_filen>\n";
     std::cerr << "options:";
     std::cerr << "\n     -h:      print out help info";
     std::cerr << "\n     -v:      print out version info";
     std::cerr << "\n     -L<dir>: search for POET libraries in the specified directory";
     std::cerr << "\n     -p<name>=<val>: set POET parameter <name> to have value <val>";
     std::cerr << "\n     -dp:     print out debugging info for parsing operations";
     std::cerr << "\n     -dl:     print out debugging info for lexical analysis";
     std::cerr << "\n     -dx:     print out debugging info for xform routines"; 
     std::cerr << "\n     -dm:     print out debugging info for pattern matching"; 
     std::cerr << "\n     -md:     allow global names to be multiply-defined (overwritte)";
     std::cerr << "\n     -dt:     print out timing information for various components of the POET interpreter";
     std::cerr << "\n     -dy:   print out debugging info from yacc parsing";
     exit(1);
  }
  char* env = getenv("POET_LIB");
  if (env != 0) {
     for (char* p = env; *p != 0; ++p) { 
       while (*p == ' ') ++p; 
       char* p1 = p;
       while (*p1 != 0 && *p1 != ' ') ++p1;
       char m = *p1;
       if (m != 0)
          *p1 = 0;
       lib_dir.push_back(std::string(p) + "/");
       *p1 = m;    
     }
  }
#ifdef POET_DEST_DIR
  lib_dir.push_front(POET_DEST_DIR);
#endif
#ifdef POET_LIB_DIR
  lib_dir.push_front(POET_LIB_DIR);
#endif
  lib_dir.push_front("./");
  
  int index = 1;
  lexState= LEX_DEFAULT;
  for ( ; index < argc && argv[index][0] == '-'; ++index) {
     char* p = argv[index]+1;
     if (*p == 'L') {
          lib_dir.push_front(std::string(p+1)+"/");
     }
     else if (*p == 'd' && *(p+1) == 'p')
          debug |= DEBUG_PARSE;
     else if (*p == 'd' && *(p+1) == 'l')
          debug |= DEBUG_LEX;
     else if (*p == 'd' && *(p+1) == 'x')
          debug |= DEBUG_XFORM;
     else if (*p == 'd' && *(p+1) == 'm')
          debug |= DEBUG_PATTERN;
     else if (*p == 'd' && *(p+1) == 't')
          debug |= DEBUG_TIME;
     else if (*p == 'd' && *(p+1) == 'y')
          yydebug = 1;
     else if (*p == 'm' && *(p+1) == 'd')
          redefine_code=true;
     else if (*p == 'v')
          std::cerr << "pcg version: " << POET_VERSION << "\n";
     else if (*p == 'p') {
         std::string parName;
         char* p1 = p+1;
         while ((*p1) != 0 && (*p1) != '=') { parName.push_back(*p1); ++p1; }
         if ((*p1) == '=') 
            parmap[parName] = make_sourceString(p1+1,-1);
        else std:: cerr << "Unrecognized option: " << argv[index] << "\n";
     }
     else {
        std:: cerr << "Unrecognized option: " << argv[index] << "\n";
     }
  }
  return index;
}

int peek_input() {
     int c = 0;
     for ( c = fgetc(yyin); c == ' ' || c == '\t' || c == '\n' || c == '\r'; 
           c = fgetc(yyin))  {
         if (c == '\n')
             ++yylineno;
     } 
     ungetc(c,yyin);
     return c;
}

std:: string get_string() {
   std:: string r;
   int c;
   for ( c = fgetc(yyin); c != ' ' && c != '\t' && c != '\n' && c != '\r'; 
         c = fgetc(yyin) )  {
         r.push_back(c);
   } 
   ungetc(c,yyin);
   return r;
}

POETProgram* process_file(const char* fname)
{
  static std::map<std::string,POETProgram*,std::less<std::string> > poetfiles;
  std::list<std::string> lib_files;
  std::string filetype = (lexState&LEX_INPUT)? "POET input" : 
                          (lexState&LEX_SYNTAX)? "POET syntax" : "POET instructions";

  //POETProgram::make_macroVar(ASTFactory::inst()->new_string("INHERIT"));

  std::string name;
  if (fname == 0 || *fname==0)  {
       std::cerr << "Reading " << filetype << " from stdin\n";
       yyin = stdin;
  }
  else {
     name = std::string(fname);
     if (poetfiles.find(name) != poetfiles.end()) {
        POETProgram* res = poetfiles[name];
        return res;
     }

     yyin = fopen(fname, "r");
     if (yyin == 0) { // && !(lexState&LEX_INPUT) ) {
        for (std::list<std::string>::const_iterator pdir = lib_dir.begin();
             pdir != lib_dir.end(); ++pdir) {
           std::string fullname = (*pdir) + name;
           yyin = fopen(fullname.c_str(), "r");
           if (yyin != 0)
                break;
        }
     }
     if (yyin == 0) { INCORRECT_FNAME(name); }

     std:: cerr << "Reading " << filetype << " from file " << name << "\n";

     if (!(lexState & LEX_INPUT)) {
        unsigned index=name.find(".pi");
        if (index < name.size()) {
            std::string tmp = name.substr(0,index);
            tmp = tmp + ".pt";
            lib_files.push_back(tmp);
        }
     }
  }
  yylineno = 1;
  POETProgram* thisProgram = new POETProgram(name);
  if ( (lexState&LEX_INPUT) == 0) {
     int c = peek_input();
     while (true) {
        if (c != 'i') break;
        std:: string cur = get_string(); 
        if (cur != "include")  {
           std::cerr << " Line " << yylineno << " of file " << name << ": " << "Unrecognized commend: " << cur << "\n"; 
           exit(1);
        }
        peek_input();
        cur = get_string();
        c = peek_input();
     
        FILE* filesave = yyin;
        int linesave = yylineno;

        POETProgram* curincl = process_file(cur.c_str());
        thisProgram->insert_includeFile(curincl); 

        yyin = filesave;
        yylineno = linesave;
    }
  }
  POETProgram* save =  curfile;
  curfile = thisProgram;
  try {yyparse(); }
  catch (Error err) { 
    fclose(yyin); std::cerr << "\nAt line " << yylineno << " of file " << curfile->get_filename() << "\n"; exit(1); 
  }
  fclose(yyin);
  std:: cerr << "Done reading " << filetype << " file " << name << "\n";
  poetfiles[name] = curfile;

  for (std::list<std::string>::const_iterator plib = lib_files.begin(); 
          plib != lib_files.end(); ++ plib) { 
       std::string cur = *plib;
       if (cur != "")
          process_file(cur.c_str());
       else
         assert(false);
  }
  curfile = save;
  return thisProgram;
}


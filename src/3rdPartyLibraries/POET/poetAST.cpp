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
/****************************************************** 
implementation functions for POET AST 
*******************************************************/

#include <fstream>

#include <poetAST.h>
#include <ASTeval.h>
#include <ASTfactory.h>
#include <stdlib.h>

#define DEBUG_MAX_OUTPUT 15

int POETCode:: maxID = 0;
bool POETProgram::backtrack=true;
LvarSymbolTable POETProgram::traceDef;
LvarSymbolTable POETProgram::paramDef;
LvarSymbolTable POETProgram::macroDef;
POETString* ASTFactory::emptyString = new POETString("");
POETType* ASTFactory::intType = new POETType(TYPE_INT);
POETType* ASTFactory::stringType = new POETType(TYPE_STRING);
POETType* ASTFactory::idType = new POETType(TYPE_ID);
POETType*  ASTFactory::lvarAny = new POETType(TYPE_ANY);
ASTFactory* ASTFactory::_inst = 0;
POETProgram* curfile;
int user_debug = 0;

const char* POETTypeName[] = {"_", "INT","STRING","ID","~","...","....","|"};
const char* OpName[] = 
  { "NONE", "DEBUG", "PRINT",
    "REPLACE", "DUPLICATE", "PERMUTE", "REBUILD", 
    "TRACE", "ERASE", "COPY", "SAVE", "RESTORE", 
    "=>", "==>", "BEGIN", ":", 
    "EXP", "VAR", "CLEAR",
    "DELAY", "APPLY", 
    "SPLIT", "LEN", "CODE", "XFORM","MAP", 
    "^", "TUPLE", "LIST", "LIST1", "..",
   "#","+", "-", "-", "*", "/", "%", 
   "HEAD", "TAIL", "::", "INSERT", "&&", "||", "!", 
   "|" , "...","....", 
   "==", "<", "<=", ">", ">=", "!=", ".",
   "SEQ", "IFELSE", "CASE", 
   "FOR", "FOREACH", "FOREACHR", "CONTINUE", "BREAK", "RETURN",
    "ERROR"};

extern int user_debug;
extern bool debug_parse();

POETString* POETProgram:: make_string(const std::string& r)
          { return ASTFactory::inst()->new_string(r); }
POETIconst* POETProgram:: make_Iconst(int val)
          { return ASTFactory::inst()->new_iconst(val); }
POETOperator* POETProgram:: make_dummyOperator(POETOperatorType t)
          { return ASTFactory::inst()->new_op(t); }
POETCode* POETProgram:: make_any() 
          { return  ASTFactory::inst()->make_any(); }
POETCode* POETProgram:: make_rangeType(POETCode* lb, POETCode* ub)
          {  assert(lb!=0 && ub!=0); return new POETBop(POET_OP_RANGE, lb, ub); }
POETCode* POETProgram:: make_listType(POETCode* elemType)
          { assert(elemType!=0); return ASTFactory::inst()->new_typeUop(TYPE_LIST,elemType); }
POETCode* POETProgram:: make_atomType(POETTypeEnum t)
{ return ASTFactory::inst()->new_type(t);  }
POETCode* POETProgram:: make_pair(POETCode* r1, POETCode* r2)
          { return ASTFactory::inst()->new_pair(r1,r2); }
POETCode* POETProgram::append_tuple(POETCode* r1, POETCode* r2)
{
  ASTFactory* fac = ASTFactory::inst();
  POETCode* res = fac->append_tuple(r1, r2);
  return res;
}
POETCode* POETProgram::make_tuple3(POETCode* r1, POETCode* r2, POETCode* r3)
{
  ASTFactory* fac = ASTFactory::inst();
  POETCode* res = fac->new_pair(r1,r2);
  res = fac->append_tuple(res, r3);
  return res;
}
POETCode* POETProgram::make_tuple4(POETCode* r1, POETCode* r2, POETCode* r3, POETCode* r4)
{
  ASTFactory* fac = ASTFactory::inst();
  POETCode* res = fac->new_pair(r1,r2);
  res = fac->append_tuple(res, r3);
  res = fac->append_tuple(res, r4);
  return res;
}
POETCode* POETProgram::make_tuple5(POETCode* r1, POETCode* r2, POETCode* r3, POETCode* r4, POETCode* r5)
{
  ASTFactory* fac = ASTFactory::inst();
  POETCode* res = fac->new_pair(r1,r2);
  res = fac->append_tuple(res, r3);
  res = fac->append_tuple(res, r4);
  res = fac->append_tuple(res, r5);
  return res;
}

POETCode* POETProgram::make_tuple6(POETCode* r1, POETCode* r2, POETCode* r3, POETCode* r4, POETCode* r5, POETCode* r6)
{
  ASTFactory* fac = ASTFactory::inst();
  POETCode* res = fac->new_pair(r1,r2);
  res = fac->append_tuple(res, r3);
  res = fac->append_tuple(res, r4);
  res = fac->append_tuple(res, r5);
  res = fac->append_tuple(res, r6);
  return res;
}
POETCode* POETProgram:: make_list(POETCode* head, POETCode* tail)
             { return ASTFactory::inst()->new_list(head, tail); }
POETCode* POETProgram:: make_empty()
             { return ASTFactory::inst()->new_empty(); }

void POETProgram::insert_evalDecl(POETCode* code, int lineNo)
{ 
   POETCode* eval = new POETEvalExp(code,get_filename(), lineNo);
   define.push_back(eval); 
}

void POETProgram::insert_traceDecl(LocalVar* code, int lineno)
  { 
     POETCode* decl = new POETTraceDecl(code, get_filename(), lineno);
     define.push_back(decl); 
  }

WriteOutput* POETProgram::insert_outputDecl(int lineno)
  {
     WriteOutput* res = new WriteOutput(get_filename(), lineno);
     define.push_back( res);
     return res;
  }

void POETProgram::insert_define(LocalVar* var, POETCode* code)
{
  var->get_entry().set_code(code); 
  define.push_back(var); 
}

ParameterDecl* POETProgram::insert_parameterDecl(POETCode* par, int lineno)
{
     LocalVar* var = dynamic_cast<LocalVar*>(par);
     if (var == 0) var = make_localVar(&paramDef,par, LVAR_TUNE);
     assert (var != 0) ;
     ParameterDecl* res = new ParameterDecl(var, get_filename(), lineno);
     define.push_back(res);
     return res;
}

ReadInput* POETProgram::insert_inputDecl(int lineno)
  {
     ReadInput* res = new ReadInput(get_filename(), lineno);
     define.push_back( res);
     return res;
  }

LocalVar* POETProgram::
set_local_static(LvarSymbolTable* local, POETCode* id, 
                 LocalVarType t, POETCode* code, POETCode * restr, bool insert)
{
  assert(local != 0);
  LocalVar* lvar = local->find(id);
  POETCode* orig = 0;
  if (lvar == 0) {
     if (insert) {
       lvar = local->insert(id, t);
       lvar->get_entry().set_code(code);
       lvar->get_entry().set_restr(restr);
     }
     else SYM_UNDEFINED(id->toString()+"="+code->toString());
  }
  else if (code != 0 && (orig = lvar->get_entry().get_code()) != 0 && orig != code && !match_AST(orig, code, MATCH_AST_EQ)) {
       SYM_DIFF_DEFINED(id->toString()+"="+code->toString(), orig->toString());
   }
  else {
      if (code != 0)
         lvar->get_entry().set_code(code);
      if (restr != 0)
          lvar->get_entry().set_restr(restr);
  }
  return lvar;
}
void POETProgram::set_codeDef(CodeVar* cvar, POETCode * code)
{
   if (code != 0)  
   {
      if (codeDef[cvar].code_def != 0)
          CODE_ALREADY_DEFINED(cvar->toString());
      cvar->get_entry().set_codeDef(code);
      codeDef[cvar].code_def = code;
  }
  else if (codeDef.find(cvar) == codeDef.end())
     codeDef[cvar].code_def = code;
}

void POETProgram::set_xformDef(XformVar* xvar, POETCode * code)
{
       XvarSymbolTable::Entry e = xvar->get_entry();
       if (e.get_code() != 0) 
           SYM_ALREADY_DEFINED(e.get_name()->toString(OUTPUT_NO_DEBUG));
       e.set_codeDef(code); 
}

void POETProgram::insert_codeDecl(CodeVar* var)
  {
    define.push_back(var);
  }

XformVar* POETProgram::make_xformVar(POETCode* name)
{
  XformVar* res = ASTFactory::inst()->find_xformDef(name, true);
  LvarSymbolTable* t = res->get_entry().get_symTable();
  if (t == 0) 
      res->get_entry().set_symTable(new LvarSymbolTable());
  assert(res != 0);
  return res;
}

CodeVar* POETProgram::make_codeRef(POETCode* name, POETCode* arg)
{
  CvarSymbolTable::Entry entry = ASTFactory::inst()->insert_codeDef(name);
  if (entry.get_symTable() == 0) 
      entry.set_symTable(new LvarSymbolTable());
  CodeVar* res = ASTFactory::inst()->new_codeRef(entry,arg);
  return res;
}

XformVar* POETProgram::insert_xformDecl(POETCode* name)
 {
  XformVar* res = make_xformVar(name);
  define.push_back( res);
  return res;
  }

POETCode* POETProgram::
make_tupleAccess( POETCode* tuple, POETCode* access)
{
  POETCode* res = 0;
   if (tuple->get_enum() == SRC_XVAR)  {
       XformVar* v = static_cast<XformVar*>(tuple);
       if (v->get_config() != 0) 
          access = new POETBop(POET_OP_SEQ, v->get_config(), access);
       res = new XformVarInvoke(v->get_entry(), access, 0);
   }
   else
       res = new TupleAccess(tuple, access); 
  return res;
}

POETCode* POETProgram::
make_typeTor( POETCode* arg1, POETCode* arg2)
{ 
  POETTypeTor * tor = dynamic_cast<POETTypeTor*>(arg1);
  if (tor != 0)
     tor->push_back(arg2);
  else tor = new POETTypeTor(arg1,arg2);
  return tor;
}


POETCode* POETProgram::
make_Bop( POETOperatorType t, POETCode* arg1, POETCode* arg2, int lineno)
{
  switch (t) 
  {
     case POET_OP_MAP : return ASTFactory::inst()->new_MAP(arg1,arg2);  
     case POET_OP_LIST : case POET_OP_LIST1 : 
         return new POETParseList(t,arg1,arg2);
     case POET_OP_TYPEMATCH: case POET_OP_TYPEMATCH_Q:
     case POET_OP_ASTMATCH: case POET_OP_SPLIT: case POET_OP_POND: case POET_OP_PLUS: case POET_OP_CONCAT:
     case POET_OP_MINUS: case POET_OP_MULT: case POET_OP_DIVIDE: case POET_OP_MOD: case POET_OP_AND: 
     case POET_OP_OR: case POET_OP_EQ: case POET_OP_LT: case POET_OP_LE: case POET_OP_GT: case POET_OP_RANGE:
     case POET_OP_GE: case POET_OP_NE: case POET_OP_CONS: case POET_OP_DOT: case POET_OP_DEBUG: 
     case POET_OP_REPLACE: case POET_OP_PERMUTE: case POET_OP_INSERT:
     case POET_OP_ERASE: case POET_OP_TRACE: 
       return new POETBop(t,arg1,arg2);
     case POET_OP_CASE: 
       return (lineno < 0)? new POETBop(t, arg1, arg2) :
            (POETCode*)new POETEvalExp(new POETBop(t, arg1, arg2), get_filename(),lineno); 
     case POET_OP_SEQ: 
         return (lineno < 0)? new POETBop(t, arg1, arg2) :
            new POETBop(t, new POETEvalExp(arg1,get_filename(),lineno), arg2); 
     default: std::cerr << "unexpected Bop: " << OpName[t] << "\n"; assert(0);
  }
}

POETCode* POETProgram::
make_Uop( POETOperatorType t, POETCode* arg, int lineno)
{
  POETOperator* res = new POETUop(t, arg);
  return  res;
}

POETCode* POETProgram::
make_Top( POETOperatorType t, POETCode* arg1, POETCode* arg2, POETCode* arg3, int lineno)
{
  POETOperator* res = new POETTop(t, arg1,arg2,arg3);
  return  res;
}

POETCode* POETProgram::
make_Iconst( char* text, int len, bool parseInput)
{
  if (parseInput) {
    LocalVar* config = find_macroVar(ASTFactory::inst()->new_string("PARSE_INT"));
    POETCode* res = 0;
    if (config == 0 || (res = config->get_entry().get_code()) == 0 || !AST2Bool(res)) {
        std::string val; 
        for (int i = 0; i < len; ++i) 
           val.push_back(text[i]); 
        return ASTFactory::inst()->new_string(val);
      }
  }
  POETCode *res = string2Iconst(text, len);
  if (res == 0) {
    std::cerr << "Expecting an integer string but get : ";
    for (int i = 0; i < len; ++i)
      std::cerr << text[i];
    std::cerr << "\n";
    assert(0);
  }
  return res;
}

LocalVar* POETProgram::make_localVar(LvarSymbolTable* local,POETCode* name, LocalVarType vartype)
{
     LocalVar* lvar = local->find(name);
     if (lvar == 0)
         lvar =  local->insert(name,vartype); 
     assert(lvar != 0);
     return lvar;
}

POETCode* POETProgram::make_attrAccess(POETCode* scope, POETCode* name)
{
  assert(scope != 0);
  /*QY: we would like to postpone the evaluation of this operator for two reasons: 1) preservation of the source code for printout later. 2) use of attr names before the code template or xform routine is defined */
  return new POETBop(POET_OP_DOT,scope, name);
}

LocalVar* LvarSymbolTable::insert(POETCode* var, LocalVarType _t) 
{  
 LocalVar* res = find(var);
 if (res == 0) {
    if (_t == LVAR_UNKNOWN)  _t = LVAR_REG;
    Entry r(impl.insert(var),_t); 
    res = new LocalVar(r);
    r.set_var(res);
 }
 else if (_t != LVAR_UNKNOWN) {
    res->get_entry().set_entry_type(_t);
 }
 return res;
}

void CodeVar:: set_attr()
{
  if (attr != 0) {
      POETTuple * attrTuple = dynamic_cast<POETTuple*>(attr);
      if (attrTuple == 0) 
      {
          LocalVar* var = entry.get_attr(0);
          assert (var != 0); 
          var->get_entry().set_code(attr);
      }
      else 
      {
         unsigned size = attrTuple->size();
         for (unsigned i = 0; i < size; ++i) 
         {
            LocalVar* var = entry.get_attr(i);
            assert(var != 0);
            var->get_entry().set_code(attrTuple->get_entry(i));
         }
      }
   }
}

POETCode* CodeVar:: invoke_rebuild(POETCode* _args) 
{
    ASTFactory* fac= ASTFactory::inst();
    POETCode* res1 = invoke_func("rebuild",_args);
    if (res1 == 0) {
       POETCode* res = fac->build_codeRef(this, _args, true); 
       return res;
    }
    return res1;
}

POETCode* CodeVar::get_static_attr(const std::string& name) const
 {
      LvarSymbolTable* local = get_entry().get_symTable();
      LocalVar* b = local->find(ASTFactory::inst()->new_string(name));
      if (b != 0) 
         return b->get_entry().get_code();
      return 0;
 }

POETCode* RemoveSpaceFromList(POETCode* r)
{  
  POETList* l = dynamic_cast<POETList*>(r);
  if (l != 0) {
     POETCode* lFirst = l->get_first(), *lRest = l->get_rest();
     POETCode* r1 = RemoveSpaceFromList(lFirst);
     if (lRest == 0) return r1;
     if (r1 == EMPTY) return RemoveSpaceFromList(lRest);

     POETCode* r2 = RemoveSpaceFromList(lRest);
     if (r2 == EMPTY) return r1;
     return ASTFactory::inst()->new_list(r1,r2);
  }
  else if (r->get_enum() == SRC_STRING) {
     std::string c = static_cast<POETString*>(r)->get_content();
     if (IS_SPACE(c)) return EMPTY;
     return r;
  }
  return r;
}

POETCode* CodeVar::get_parseInfo()
{ /*QY: compute and set parsing information*/
     POETCode* res = entry.get_parse();
     if (res == 0) {
           res = entry.get_code();
           if (res == 0) CODE_SYNTAX_UNDEFINED(entry.get_name()->toString());
           res = RemoveSpaceFromList(res);
           entry.set_parse(res);
     }
     return res;
}

std:: string CodeVar:: toString(ASTOutputEnum config)
      {
       std::stringstream out;
       out << entry.get_name()->toString(OUTPUT_NO_DEBUG) ;
       if (args != 0) {
          std::string args1="",args2="";
          if (args->get_enum() == SRC_LIST) {
             args1="("; args2=")";
          }
          out  << "#" << args1 << args->toString(config) << args2;
       }
       if (attr != 0) 
          out << "#" + attr->toString(config); 
        return out.str();
      }

std:: string XformVarInvoke:: toString(ASTOutputEnum outputConfig)
    { 
      std::string res = XformVar::toString(outputConfig);
      if (config!=0) res = res + "[" + config->toString(outputConfig) + "]"; 
      if (args!=0) {
         if (args->get_enum() == SRC_TUPLE)
           res = res + args->toString(outputConfig); 
         else
           res = res + "(" + args->toString(outputConfig) + ")"; 
      }
      return res;
    }

std::string LocalVar:: toString(ASTOutputEnum config)
      { 
        std::string str = entry.get_name()->toString(OUTPUT_NO_DEBUG) ;
        if ((config & OUTPUT_VAR_VAL) 
            || ((config & OUTPUT_TRACE_VAL) && entry.get_entry_type() == LVAR_TRACE)) {
              POETCode* res = entry.get_code(); 
              if (res != 0 && res != this) {
                 entry.set_code(0);
                 str = str + "={" + res->toString(config) + "}";
                 entry.set_code(res);
              }
        }
        if (config & OUTPUT_VAR_RESTR && entry.get_restr() != 0) {
           str = "(" + str + ":" + entry.get_restr()->toString() + ")";
        }
        return str;
      }

std:: string POETList:: toString(ASTOutputEnum config)
     { 
      std::string result;
      if (config & OUTPUT_LIST_BORD) {
          result = "(";
          if (first->get_enum()==SRC_OP)
             result=result  + "("+ first->toString(config) + ")";
          else
             result=result  + first->toString(config);

          POETCode* p1 = rest;
          int num=0; 
          for (POETList* p = dynamic_cast<POETList*>(rest); p != 0; ++num) {
             if (config & OUTPUT_SHORT && num > DEBUG_MAX_OUTPUT)
                {   result = result + "...";  p1=0; break; }
             result = result + " ";
             if (p->first->get_enum()==SRC_OP)
                  result = result + "(" + p->first->toString(config) + ")";
             else
                  result = result + p->first->toString(config);
             p1 = p->rest;
             p = dynamic_cast<POETList*>(p1);
          }
          if (p1 != 0)
              result = result + " " + p1->toString(config);
          result = result + ")";
      }
      else {
         result = result + first->toString(config);
         if (rest != 0) result = result + rest->toString(config);
      }
      return result;
     }

std::string POETTuple :: toString(ASTOutputEnum config)
{ 
    std:: stringstream r;
    r << "(";
    for (unsigned i = 0; i < impl.size(); ++i) {
       POETCode* cur = impl[i];
       if (cur == 0) r << "NULL,";
       else if (cur->get_enum() == SRC_LIST) 
           r << "(" << cur->toString(config) << "),";
       else
           r << cur->toString(config) << ",";
    }
    std::string str = r.str();
    str[str.size()-1] = ')' ; 
    return str;
  }

std:: string POETMap:: toString(ASTOutputEnum config)
{ 
    std:: stringstream r;
    r << "MAP{";
    for (std::map<POETCode*,POETCode*,POETCodeLessThan>::const_iterator p=impl.begin(); 
         p != impl.end(); ++p) {
       r << (*p).first->toString(config) << "=>" << (*p).second->toString(config) << ";";
    }
    r << "}";
    std::string str = r.str();
    return str;
  }

std:: string POETOperator::toString(ASTOutputEnum config)
     {
      int num=numOfArgs();
      if (num == 0) return std::string(OpName[t]);
      std::string res = std::string(OpName[t]) + "(";
      int i = 0; 
      for ( ; i < num; ++i) 
        res = res + get_arg(i)->toString(config) + ",";
      if (i > 0) res[res.size()-1] = ')';
      else res.push_back(')');
      return res;
     }

void POETEvalExp:: visit(POETCodeVisitor* op) 
         {  try { exp->visit(op); } 
            catch (Error err) {
              std::cerr << FileInfo::toString() << "\n"; throw err;
            }
        }

std:: string POETUop:: toString(ASTOutputEnum config)
    { 
     switch (get_op())
      {
        case POET_OP_RETURN: case POET_OP_CAR: case POET_OP_CDR: case POET_OP_COPY: case POET_OP_LEN:
        case POET_OP_REBUILD: case POET_OP_RESTORE: case POET_OP_SAVE: case POET_OP_CLEAR:
            return std::string(OpName[get_op()])+"("+arg->toString(config)+")";
        case POET_OP_NOT: case POET_OP_UMINUS:
            return std::string(OpName[get_op()])+arg->toString(config);
        case POET_OP_DELAY: case POET_OP_APPLY:
            return std::string(OpName[get_op()])+"{"+arg->toString(config)+"}";
        default:return POETOperator::toString(config); 
      }
    }

std:: string POETBop:: toString(ASTOutputEnum config)
      {
       switch (get_op())
       {
        case POET_OP_SEQ: 
           return get_arg(0)->toString(config) + ";" + get_arg(1)->toString(config);
        case POET_OP_ERASE: 
             if (get_arg(0) == get_arg(1))
               return "ERASE("+get_arg(0)->toString(config) + ")";
        case POET_OP_MAP: case POET_OP_LIST: case POET_OP_LIST1: case POET_OP_REPLACE: case POET_OP_PERMUTE: case POET_OP_TRACE: case POET_OP_INSERT: case POET_OP_SPLIT:
               return POETOperator::toString(config);
        case POET_OP_DEBUG: 
              return "DEBUG["+get_arg(0)->toString(config)+"]"+"{"+get_arg(1)->toString(config)+"}";
        case POET_OP_DOT:
              return get_arg(0)->toString(config) + "." + get_arg(1)->toString(OUTPUT_NO_DEBUG);
        case POET_OP_TYPEMATCH: 
        {
        std::string opd2_string = arg2->toString(config);;
        if (config != OUTPUT_NO_DEBUG) return arg1->toString(config) + "=>" + opd2_string;
        if (arg1->get_enum() == SRC_LIST) 
           {
           POETList* opd1_list = static_cast<POETList*>(arg1);
           POETCode* opd1_head = opd1_list->get_first(), *opd1_tail=opd1_list->get_rest();
           std::string res = opd1_head->toString(config);
           if (opd1_head->get_enum() == SRC_LIST)
               res= res + " //@BEGIN(" + opd2_string + ")" + opd1_tail->toString(config);
           else if (opd1_head->get_enum() == SRC_OP)
               res= res + " BEGIN(" + opd2_string + ")" + opd1_tail->toString(config);
           else return res + opd1_tail->toString(config) + "//@ =>" + opd2_string;
           if (opd1_tail->get_enum() == SRC_OP) return res + " END(" + opd2_string + ")";
           else return res + " //@END(" + opd2_string + ")";
           }
         else if (arg1->get_enum() == SRC_OP)
           return "\n//@; BEGIN("+opd2_string+")"+arg1->toString(config)+" END("+opd2_string+")"; 
         return arg1->toString(config) + "//@ =>" + opd2_string;
        } 
        case POET_OP_CASE: return "case " + get_arg(0)->toString(config)+":"+get_arg(1)->toString(config);
        default:
            std::string opd1_string = arg1->toString(config),
                  opd2_string = arg2->toString(config);
            if (arg1->get_enum() == SRC_OP &&
                static_cast<POETOperator*>(arg1)->get_op() < get_op())
               opd1_string = "(" + opd1_string + ")";
            if (arg2->get_enum() == SRC_OP &&
                static_cast<POETOperator*>(arg2)->get_op() < get_op())
               opd2_string = "(" + opd2_string + ")";
            return opd1_string + std::string(OpName[get_op()]) + opd2_string;
       }
     }

std:: string POETTop:: toString(ASTOutputEnum config)
    {
      switch (get_op()) {
      case POET_OP_IFELSE: 
         return "("+arg1->toString(config) + ")?(" + arg2->toString(config) + "):(" + arg3->toString(config)+")";
      default: return POETOperator::toString(config);
      }
    }

std:: string POETQop:: toString(ASTOutputEnum config)
    {
      switch (get_op()) {
      case POET_OP_FOR:
         return "for ("+arg1->toString(config) + ";" + arg2->toString(config) + ";" + arg3->toString(config)+")"
               + "{" + arg4->toString(config) + "}";
      case POET_OP_FOREACH: case POET_OP_FOREACHR:
         return "foreach ("+arg1->toString(config) + ":" + arg2->toString(config) + ":" + arg3->toString(config)+")"
               + "{" + arg4->toString(config) + "}";
      default: return POETOperator::toString(config);
      }
    }

std:: string ParameterDecl:: toString(ASTOutputEnum config) 
{ 
      LvarSymbolTable::Entry e = var->get_entry();
      std::string res =  "<parameter " + e.get_name()->toString(OUTPUT_NO_DEBUG); 
      if (parse != 0) 
         res = res + " parse=" +  parse->toString(ASTOutputEnum(config | DEBUG_NO_VAR));
      if (e.get_restr() != 0) 
         res = res + " type=" + e.get_restr()->toString(ASTOutputEnum(config | DEBUG_NO_VAR));
      if (init != 0)  
         res = res + " default=" + init->toString(ASTOutputEnum(config | DEBUG_NO_VAR));
      if (msg != 0) 
         res = res + " message=" + msg->toString(ASTOutputEnum(config | DEBUG_NO_VAR));
      res = res + "/>";
      return res; 
   }

std::string ReadInput::toString(ASTOutputEnum config) 
{ 
      std::string res =  "<input"; 
      if (target != 0) 
       {
         if (target != this)
            res = res + " to=" +  target->toString(config) ;
         else
            res = res + " to=POET";
       }
      if (syntaxFiles != 0)  
       {
          if (syntaxFiles != this)
             res = res + " syntax=" + syntaxFiles->toString(DEBUG_NO_VAR);
          else
             res = res + " syntax=POET";
       }
      if (inputFiles != 0) 
         res = res + " from=(" + inputFiles->toString(DEBUG_NO_VAR) + ")";
      if (type != 0) 
         res = res + " parse=" + (type==this? "POET" : type->toString(config));
      if (annot != 0) res = res + " annot=" + annot->toString(config);
      if (cond != 0) res = res + " cond=(" + cond->toString(config) + ")";
      if (debug != 0) res = res + " debug=" + debug->toString(config);
      if (inlineInput != 0) 
         res = res + ">" + inlineInput->toString(OUTPUT_NO_DEBUG) + "</input>";
      else
         res = res + "/>";
      return res; 
   }

std:: string WriteOutput::toString(ASTOutputEnum config) 
{ 
      std::string res =  "<output"; 
      if (outputExp != 0) 
         res = res + " from=(" +  outputExp->toString(config) +")";
      if (syntaxFiles != 0)  
         res = res + " syntax=(" + syntaxFiles->toString(config) + ")" ;
      if (outputFile != 0) 
         res = res + " to=(" + outputFile->toString(config) +")";
      if (cond != 0) res = res + " cond=(" + cond->toString(config)+")";
      res = res + "/>";
      return res; 
   }

class Write_POET_Exp : public POETCodeVisitor
{
  std::ostream& out;
  unsigned align;
  int lineno;
  virtual void defaultVisit(POETCode* code) 
        { out << code->toString(DEBUG_NO_VAR); };
 
  void line_break()
     {
           out << "\n"; ++lineno;
           for (unsigned i = 0; i < align; ++i) out << " ";
     }
 public: 
   Write_POET_Exp(std::ostream& _out, unsigned _align) 
    : out(_out),align(_align), lineno(-1) {}
   int operator()(POETCode* code) { lineno=0; code->visit(this); return lineno; }
   virtual void visitAssign(POETAssign* op) 
    {
       op->get_lhs()->visit(this);
       out << " = ";
       op->get_rhs()->visit(this);
    }
   virtual void visitOperator(POETOperator *op)
    {
      switch (op->get_op()) {
      case POET_OP_SEQ: 
           op->get_arg(0)->visit(this);
           if (op->get_arg(0)->get_enum() != SRC_EVAL) out << ";"; 
           line_break();
           op->get_arg(1)->visit(this);
           break;
      case POET_OP_DELAY:
         {
           out << "DELAY{";
           unsigned align_save = align; align += 3;
           line_break();
           op->get_arg(0)->visit(this);
           align = align_save;
           line_break();
           out << "}";
           break; 
         }
      case POET_OP_CASE: 
        {
           out << "case " << op->get_arg(0)->toString(DEBUG_NO_VAR) << ":";
           unsigned align_save = align; align += 3;
           line_break();
           op->get_arg(1)->visit(this);
           align = align_save;
           line_break();
           break;
        }
      case POET_OP_IFELSE: 
         {
           out << "if (" << op->get_arg(0)->toString(DEBUG_NO_VAR) << ")  {";
           unsigned align_save = align; align += 3;
           line_break();
           op->get_arg(1)->visit(this);
           align = align_save; line_break(); out << "}";
           POETCode* arg2 = op->get_arg(2);
           if (arg2 != EMPTY) {
               out << "else {";
               align +=3; line_break(); 
               op->get_arg(2)->visit(this);
               align = align_save; line_break(); out << "}";
           }
           break;
         }
      default: 
          out << op->toString(DEBUG_NO_VAR);
      }
    }
};

class PrintNestedTraceVars : public VisitNestedTraceVars
{
  std::string res;
 public:
  virtual void preVisitTraceVar(LocalVar* v) 
    {  
       if (res.size() > 0) res = res + ",";
       res = res +  v->toString(OUTPUT_NO_DEBUG); 
    }
  std::string get_result() 
    { return res; } 
};

std:: string POETTraceDecl:: toString(ASTOutputEnum config)
{
     PrintNestedTraceVars write;
     write.apply(var);
     return "<trace "+ write.get_result() + "/>";
}

void POETProgram:: write_to_file(std::ostream& out)
{  
  int lineNo=0;
  for (std::list<POETProgram*>::const_iterator 
           pincludeFiles = includeFiles.begin();
           pincludeFiles!=includeFiles.end(); ++pincludeFiles) {
        POETProgram* cur = (*pincludeFiles);
        out << "include " << cur->get_filename() << "\n";
        ++lineNo;
  }
  for (const_iterator pdef = define.begin(); pdef != define.end(); ++pdef) {
       POETCode* cur = *pdef;
       const FileInfo* curFile = cur->get_fileinfo();
       if (curFile != 0) {
           while (lineNo < curFile->get_lineno()) {
               out << "\n"; ++lineNo;
           }
       }
       switch (cur->get_enum())
       {
       case SRC_WRITE_OUTPUT: case SRC_READ_INPUT: case SRC_PARAM_DECL:
       case SRC_TRACE_DECL:
          out << cur->toString(DEBUG_NO_VAR) << "\n"; ++lineNo; break;
       case SRC_CVAR: 
         {
          CodeVar* cvar = static_cast<CodeVar*>(cur);
          out << "<code " << cvar->toString();
          const CodeDefSave& def = codeDef[cvar];
          unsigned par_size = def.par_type.size();
          if (par_size > 0) {
             out << " pars=(";
             for (unsigned i = 0; i < par_size; ++i) 
             {
               const LocalVarSave& par_save = def.par_type[i];
               assert(par_save.lvar != 0); 
               out << par_save.lvar->get_entry().get_name();
               if (par_save.restr != 0)
                   out << ":" << par_save.restr->toString(DEBUG_NO_VAR);
               if (i < par_size) out << ",";
             } 
             out << ")";
          }
          CvarSymbolTable::Entry cvar_entry = cvar->get_entry();
          for (unsigned i = 0; i < cvar_entry.attr_size(); ++i) {
             LocalVar* v = cvar_entry.get_attr(i); 
             out << " " << v->get_entry().get_name()->toString(OUTPUT_NO_DEBUG) << "=" << v->get_entry().get_code()->toString(DEBUG_NO_VAR);
          } 
          out << "/>\n";++lineNo;  
          break;
         } 
       case SRC_LVAR:
         {
          LocalVar* cur_var = static_cast<LocalVar*>(cur);
          switch (cur_var->get_entry().get_entry_type())
          {
           case LVAR_TUNE:
              out << "<define " << cur->toString(OUTPUT_NO_DEBUG) << "/>\n";
              ++lineNo;  
              break;
           default:
                std::cerr << "Something is wrong. Unexpected var decl at global scope: " << cur->toString(DEBUG_OUTPUT) << "\n"; 
               assert(0);
          }
          break;
         }
       default: // eval command
          out << "<eval ";
          Write_POET_Exp write(out,6);
          lineNo += write(cur);
          out << "/>\n"; ++lineNo;
      }
   }
}


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
  Routines which re-implement POET evaluation extensions using ROSE AST.
********************************************************************/
//do not include the following files from rose.h
#define CFG_ROSE_H
#define CONTROLFLOWGRAPH_H
#define PRE_H
#define ASTDOTGENERATION_TEMPLATES_C
#include <sage3basic.h>
#include <unparser.h>

#include <poet_ASTeval.h>
#include <poet_ASTinterface.h>
#include <AstInterface_ROSE.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <sstream>
#include <unparser.h>

extern int debug;

extern bool debug_pattern();
extern std::list<std::string> lib_dir;
extern void code_gen(std:: ostream& out, POETCode *code, POETCode* output_invoke, POETCode* listsep, int align);
//#define DEBUG_OP 1
//#define DEBUG_UNPARSE 1
//#define DEBUG_MOD 1

static bool inside_unparse = 0;
extern SgClassDefinition* GetClassDefinition( SgNamedType *classtype);

extern SgSymbol* LookupVar( const std::string& name, SgScopeStatement* loc);


/********************************************************************
QY: Support for mapping ROSE AST to POET  
********************************************************************/

POETCode* extract_parameter_names(POETCode* pars)
{
  if (pars == 0) return 0;
  POETCode* head = get_head(pars);
  head = split_string("", head);
  POETCode* tmp = EvaluatePOET::NextToken(head, 0);
  while (tmp != EMPTY && tmp != 0)  { head = tmp; tmp = EvaluatePOET::NextToken(head,0); }
  head = EvaluatePOET::FirstToken(head);
  POETCode* tail = extract_parameter_names(get_tail(pars));
  return LIST(head, tail);
}

POETCode* get_template_parameters(SgTemplateParameterPtrList& decl)
{
/*
 size_t begin = decl.find_first_of("<"); 
 size_t end = decl.find_first_of(">"); 
 std::string pars = decl.substr(begin+1, end-begin-1);
 POETCode *tokens = extract_parameter_names(split_string(",", STRING(pars)));
*/
 POETCode* tokens=0;
 for (SgTemplateParameterPtrList::const_iterator p = decl.begin(); p != decl.end(); p++) {
    SgTemplateParameter* cur = *p;
    SgType* e = cur->get_type();
    tokens = LIST( STRING(e->unparseToString()), tokens);
 }
 return tokens;
}

template <class PtrList>
POETCode* ROSE_2_POET_list(PtrList& l, POETCode* res, 
                           SgTemplateInstantiationFunctionDecl* tmp)
{
static POETCode* tmpPars=0;
    if (l.size() == 1) { /* return the only element inside the list */
      return POETAstInterface::Ast2POET(*l.begin()); 
    }
    typename PtrList::const_reverse_iterator p = l.rbegin(); 
    if (p == l.rend()) return (res==0)? EMPTY : res;

    SgNode* prev2 = (SgNode*)*p;
    POETCode* prev = POETAstInterface::Ast2POET(prev2);
    for (++p; p != l.rend(); ++p) {
       SgNode* cur = (SgNode*)*p;
       SgPragmaDeclaration* info = isSgPragmaDeclaration(cur);
       if (info!=0) { /* replace template parameters in pragma */
           POETAstInterface::set_Ast2POET(prev2, EMPTY);
           std::string pragma_str = info->get_pragma()->get_pragma();
           if (debug_pattern()) std::cerr << "Found pragma :" << pragma_str << "\n";
           POETCode *content=STRING(pragma_str);
           POETCode* tokens = split_string("",content);
           if (tmp == 0) tmpPars = 0;
           else { 
             const SgTemplateArgumentPtrList& args= tmp->get_templateArguments();
             if (tmpPars == 0) 
               tmpPars=get_template_parameters(tmp->get_templateDeclaration()->get_templateParameters());
             POETCode* pars = tmpPars;
             for (SgTemplateArgumentPtrList::const_iterator p2 = args.begin();
                   p2 != args.end(); p2++) {
                 POETCode* from = get_head(pars); pars=get_tail(pars);
                 POETCode* to = STRING((*p2)->unparseToString());
                 tokens = eval_replace(from, to, tokens); 
              } 
           }
           prev = CODE_ACC("Pragma", PAIR(tokens, prev));
           POETAstInterface::set_Ast2POET(info,prev);
       }
       else { 
         res = LIST(prev, res);
         prev = POETAstInterface::Ast2POET((SgNode*)cur); }
    } 
    res = LIST(prev,res); 
    return res;
}

POETCode* POETAstInterface::Ast2POET(const Ast& n)
{
  static SgTemplateInstantiationFunctionDecl* tmp=0;
  SgNode* input = (SgNode*) n;
  if (input == 0) return EMPTY;
  POETCode* res = POETAstInterface::find_Ast2POET(input);
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "Looking for AST wrap for " << n << ":" << AstInterface::AstToString(n) << "\n";
#endif
  if (res != 0) return res;

  {
    SgType* t = isSgType(input);
    if (t != 0) {
       POETCode* type = 0;
       SgTemplateType *t1 = isSgTemplateType(t);
       if (t1 != 0) type = STRING(t1->get_name().str());
       else { type = new POETCode_ext(t); }
       POETAstInterface::set_Ast2POET(input, type);
       return type;
  } }
  { 
  SgExprListExp* block = isSgExprListExp(input);
  if (block != 0) {
    res=ROSE_2_POET_list(block->get_expressions(), 0, tmp);
    POETAstInterface::set_Ast2POET(input, res); 
    return res;
  } }
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "creating AST wrap for " << AstInterface::AstToString(n) << "\n";
#endif
  res = new POETCode_ext(input);
  POETAstInterface::set_Ast2POET(input,res);
  return res;
}


POETCode* EvaluatePOET::
eval_readInput_nosyntax(POETCode* inputFiles, POETCode* codeType, POETCode* inputInline)
{ 
  inputFiles = eval_AST(inputFiles);
  std::cerr << "Using ROSE Parser for " << inputFiles->toString() << "\n";
  std::vector<std::string> argvList;
  argvList.push_back("pcg_rose");

  for (std::list<std::string>::const_iterator p_lib = lib_dir.begin();
        p_lib != lib_dir.end(); ++p_lib) {
     std::string cur_dir=(*p_lib);
     argvList.push_back("-I"+cur_dir);
  }
  for (POETCode* p = inputFiles; p != 0; p = get_tail(p))
  {
     std::string curname = p->toString(OUTPUT_NO_DEBUG);
     assert(curname != "");
     if (curname.find("/") == curname.npos) {
        for (std::list<std::string>::const_iterator p_lib = lib_dir.begin();
             p_lib != lib_dir.end(); ++p_lib) {
            std::string cur_dir=(*p_lib) + "/" + curname;
            if (access(cur_dir.c_str(), R_OK) == 0) 
               { curname = cur_dir; break;}
        }
     }
     argvList.push_back(curname);
  }
  assert(argvList.size() > 1);
  SgProject *sageProject = new SgProject ( argvList);
  sageProject->set_Java_only(false);

  return POETAstInterface::Ast2POET(sageProject);
}

inline bool CHECK_NAME(const SgNode* n, const std::string& tname,  const std::string& str) { 
  if (tname == str) {
#ifdef DEBUG_OP 
      std::cerr << "matching " << n->class_name() << ":" << str << " with " << tname << "\n"; 
#endif 
      return true; 
    }
    return false;
}

#define CHECK_NAME2(n, tname,str1, str2)  (CHECK_NAME(n,tname,str1) || CHECK_NAME(n,tname,str2))
#define CHECK_NAME3(n,tname,str1, str2, str3) (CHECK_NAME2(n,tname,str1,str2) || CHECK_NAME(n,tname,str3))
#define CHECK_ARG(n,tname, t1, t2, arg)  (CHECK_NAME(n, tname, t1) && (arg==0 || CHECK_NAME2(n, arg->toString(OUTPUT_NO_DEBUG), t2,"_")))

class POETGenTypeName : public POETCodeVisitor
{
  std::vector<std::string> tname_vec;
  std::vector<POETCode*> arg_vec;
  LocalVar* lhs;

 public: 
  POETGenTypeName() : lhs(0) {}
  std::string get_tname(int i) { return tname_vec[i]; }
  unsigned num_of_types() { return tname_vec.size(); }
  POETCode* get_args(int i) { assert(i < arg_vec.size()); return arg_vec[i]; }
  LocalVar* get_lhs() { return lhs; }
  virtual void visitList(POETList* l) 
    { tname_vec.push_back("LIST"); arg_vec.push_back(l); }
  virtual void visitTuple( POETTuple* v)  { tname_vec.push_back("TUPLE"); arg_vec.push_back(v); }
  virtual void visitNULL(POETNull* l) { tname_vec.push_back("LIST"); arg_vec.push_back(0); }
  virtual void visitUnknown(POETCode_ext* ext) { 
     SgNode* input=(SgNode*)ext->get_content();
     switch (input->variantT()) {
       case V_SgInitializedName:
          tname_vec.push_back("Name"); arg_vec.push_back(STRING(static_cast<SgInitializedName*>(input)->get_name()));
#ifdef DEBUG_OP
   std::cerr << "converting name: " <<  arg_vec[arg_vec.size()-1]->toString() << "\n";
#endif
          break;
       default:
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "is unknown:" << ext->toString() << "\n";
#endif
         tname_vec.push_back("UNKNOWN"); arg_vec.push_back(ext); 
     }
   }
  virtual void visitString(POETString* s) 
   {  tname_vec.push_back("Name");  arg_vec.push_back(s);  }
  virtual void visitIconst( POETIconst* i)
   { tname_vec.push_back("INT_UL"); arg_vec.push_back(i); }
  virtual void visitType(POETType* t)
   { tname_vec.push_back(t->toString(OUTPUT_NO_DEBUG)); arg_vec.push_back(0); }
  virtual void defaultVisit(POETCode* c) { 
      std::cerr << "Seeing " << c->get_className() << ":" << c->toString() << "\n";
        assert(0); };
  virtual void visitXformVar( XformVar* v) { 
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "is xform function:" << v->toString() << "\n";
#endif
             tname_vec.push_back(v->get_entry().get_name()->toString(OUTPUT_NO_DEBUG));
             arg_vec.push_back(v->get_args());
   }
  virtual void visitOperator(POETOperator* v) { 
     switch (v->get_op()) {
     case POET_OP_POND: tname_vec.push_back(eval_AST(v->get_arg(0))->toString(OUTPUT_NO_DEBUG)); arg_vec.push_back( v->get_arg(1)); break;
     case POET_OP_VAR: tname_vec.push_back("POET__VAR"); arg_vec.push_back(0); break;
     case TYPE_LIST:
     case TYPE_LIST1:
        tname_vec.push_back("LIST"); arg_vec.push_back(v->get_arg(0));  break;
     case TYPE_TOR: 
         for (unsigned i = 0; i < v->numOfArgs(); ++i)
         {
            POETCode* cur = v->get_arg(i);
            apply(cur);
         }
         break;
     case POET_OP_CLEAR: 
         tname_vec.push_back("_"); arg_vec.push_back(v->get_arg(0));  
         lhs = dynamic_cast<LocalVar*>(eval_AST(v->get_arg(0))); assert(lhs != 0); break;
     default: 
      std::cerr << "Unexpected operator : " << v->toString() << "\n";
          assert(0);
     }
    }
  virtual void visitTupleAccess(TupleAccess* v) {assert(0); }
  virtual void visitCodeVar(CodeVar* c) 
     {
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "is code template:" << c->toString() << "\n";
#endif
             tname_vec.push_back(c->get_entry().get_name()->toString(OUTPUT_NO_DEBUG));
             arg_vec.push_back(c->get_args());
     }
  virtual void visitAssign(POETAssign* a) {  
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "is assign:" << a->toString() << "\n";
#endif
           if (lhs != 0) { 
           std::cerr << "multiple lhs: " << a->toString() << "\n"; assert(0); }
           if (a->get_lhs()->get_enum() == SRC_LVAR)
              lhs = static_cast<LocalVar*>(a->get_lhs());
           else lhs = dynamic_cast<LocalVar*>(eval_AST(a->get_lhs())); 
           assert(lhs != 0);
           a->get_rhs()->visit(this);
         }
  virtual void visitLocalVar(LocalVar* v) { 
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "is variable" << v->get_entry().get_name()->toString() << "\n";
#endif
          LocalVarType t = v->get_entry().get_entry_type();
          if (t == LVAR_ATTR) {
              std::string attr = v->get_entry().get_name()->toString(OUTPUT_NO_DEBUG);
              tname_vec.push_back(attr); arg_vec.push_back(0);  
          }  
          else {
            POETCode* cur = v->get_entry().get_code();
            if (cur == 0 || cur==ANY) {
              tname_vec.push_back("_"); arg_vec.push_back(0);  
              lhs = v;
            }
            else if (cur == EMPTY) {
              tname_vec.push_back(v->get_entry().get_name()->toString(OUTPUT_NO_DEBUG)); arg_vec.push_back(0);
            }
            else {  cur->visit(this); }
          }
    }
};

std::string POETAstInterface::AstTypeName(const Ast & n)
{
  SgNode* input = (SgNode*)n;
  return input->class_name();
}

bool CompareTypeName(const std::string& varname, POETCode* arg)
{ 
  if (get_tail(arg) == 0) arg = get_head(arg);
#ifdef DEBUG_OP
  if (!inside_unparse)
    std::cerr << "comparing name: \"" << varname << "\" vs \"" << arg->toString(OUTPUT_NO_DEBUG) << "\"\n";
#endif
  if (varname[0] == ':' && varname[1] == ':') {
    return CompareTypeName(varname.substr(2, varname.size()-2), arg);
  }
  std::string argname = arg->toString(OUTPUT_NO_DEBUG); 
  if (argname == "Bop" || argname == "Uop") 
    { switch (varname[0]) {
        case '+': case '-': case '=': case '*': case '/': case '%': case '&': case '|': case '!':return true; 
        default: return false;
      }
    }
  int p = varname.find("::");
  if (p < 0 || p >= varname.size()) {
      p = varname.find("<"); // template parameters 
      if (p < 0 || p >= varname.size()) {
          if (varname == argname) {
#ifdef DEBUG_OP
       std::cerr << "strings are the same\n";
#endif
          return true;
          }
          return false;
      }
      while (p > 0 && varname[p-1]==' ') p = p - 1;
      return CompareTypeName(varname.substr(0, p), arg);
  }
  return varname.substr(0,p) == get_head(arg)->toString(OUTPUT_NO_DEBUG) &&
         CompareTypeName(varname.substr(p+2, varname.size()-p-2), get_tail(arg));
}


bool compareSgNode(SgNode* n1, SgNode* n2)
{
  if (n1 == n2) return true;
  assert(n1!=0 && n2!=0);
  if (n1->variantT() != n2->variantT()) return false;
  std::cerr << "comparing " << n1->unparseToString() << " with " << n2->unparseToString() << "\n";
  AstInterface::AstList c1 = AstInterface::GetChildrenList(n1);
  AstInterface::AstList c2 = AstInterface::GetChildrenList(n2);
  for (AstInterface::AstList::const_iterator p1 = c1.begin(), p2 = c2.begin(); p1 != c1.end(); p1++,p2++) {
     if (!compareSgNode((SgNode*)*p1, (SgNode*)*p2)) return false;
  }
  return true;
}

POETAstInterface::Ast MatchAstWithPatternHelp(const POETAstInterface::Ast& n, POETCode* pat);

bool ClassifyAst(const POETAstInterface::Ast& n, const std::string& tname, POETCode* arg, AstInterface::AstList& c)
{
  SgNode* input = (SgNode*) n;
#ifdef DEBUG_OP
  if (!inside_unparse)
std:: cerr << "trying to match " << AstInterface::AstToString(input) << " vs " << tname << ":" << ((arg==0)? "NULL:" : arg->toString()) << "\n";
#endif
  if (tname == "_") { 
#ifdef DEBUG_OP
  if (!inside_unparse)
  std::cerr << "matching to any\n";
#endif
     return true; /* yes to anything, represented by "_" */ }
  else if (tname == "UNKNOWN") {
     return compareSgNode(input, (SgNode*)static_cast<POETCode_ext*>(arg)->get_content());
  }
  else if (tname == "EXP") { /* is an expression?*/
     assert(arg==0);
     return AstInterface::IsExpression(input) != AST_NULL;
  }
  else if (tname == "STMT") { /* is a statement?*/
     assert(arg==0);
     return AstInterface::IsStatement(input);
  }
  else if (tname == "Name" || tname == "ScopedName" || tname == "STRING") {
     std::string varname;
     switch (input->variantT()) {
     case V_SgVarRefExp: 
         varname = static_cast<SgVarRefExp*>(input)->get_symbol()->get_name().getString(); break;
     case V_SgClassType:
         varname = static_cast<SgClassType*>(input)->get_name().getString(); break;
     case V_SgInitializedName:
         varname = static_cast<SgInitializedName*>(input)->get_name().getString(); break;
     case V_SgFunctionRefExp: 
     case V_SgMemberFunctionRefExp: 
         varname = static_cast<SgFunctionRefExp*>(input)->get_symbol()->get_name().getString(); 
         if (varname.find("operator") == 0) varname = varname.substr(8,varname.size()-8);  
         break;
     case V_SgName: varname = isSgName(input)->getString(); break;
     case V_SgArrowExp: 
           if (isSgThisExp(static_cast<SgArrowExp*>(input)->get_lhs_operand()))
               return ClassifyAst(static_cast<SgArrowExp*>(input)->get_rhs_operand(), tname, arg, c);
     default: 
#ifdef DEBUG_OP
  if (!inside_unparse)
  std::cerr << "not considered a name: " << AstInterface::AstToString(input) << "\n";
#endif
         return false;
     }
     return  arg == 0 || CompareTypeName(varname, arg);
  }
  else if (tname == "get_class_defn") { /* retrieve type of n */
     SgClassType* t = isSgClassType(input); 
     if (t != 0) { c.push_back(GetClassDefinition(t)); return true; }
     else { return false; }
  }
  else if (tname == "get_symtab") { /* retrieve symbol table */
     if (isSgGlobal(input) || isSgDeclarationStatement(input) || isSgScopeStatement(input))
         { c.push_back(input); return true; }
     else { return false; }
  }
  else if (tname == "map") {
     assert(arg != 0);
     switch (arg->get_enum()) {
     case SRC_STRING: { 
           SgScopeStatement *scope = isSgScopeStatement(input);
           assert(scope != 0);
           if (scope == 0) return false;
           SgSymbol *s = LookupVar(arg->toString(OUTPUT_NO_DEBUG), scope);
assert(s != 0);
           if (s == 0) return false;
           c.push_back(s->get_type());
           return true;}
     default: assert(0);
     }  
     return false;
  }
  bool res = true;
  switch (input->variantT()) {
     case V_SgVarRefExp: 
     case V_SgFunctionRefExp: 
     case V_SgMemberFunctionRefExp: 
     case V_SgName: return false;
    case V_SgPragmaDeclaration: res = CHECK_NAME(input,tname,"Pragma");  
        if (res != 0 && arg != 0) {
std::cerr << "RECOGNIZING PRAGMA: \n";
           c.push_back(static_cast<SgPragmaDeclaration*>(input)->get_pragma());
           
           AstInterface::AstList l = AstInterface::GetBlockStmtList(input->get_parent());
           for (AstInterface::AstList::const_iterator p = l.begin(); p != l.end(); ++p) {
              SgNode* cur = (SgNode*)*p;
              if (cur == input) { 
                 while (cur->variantT() == V_SgPragmaDeclaration) {
                    ++p; 
                    cur = (SgNode*)*p;
                 }
                 c.push_back(cur); break; 
              }
           }
        }
        break;
   case V_SgIntVal:  res=(CHECK_NAME(input, tname, "INT_UL") && 
         (arg==0 || dynamic_cast<POETIconst*>(arg)->get_val() == static_cast<SgIntVal*>(input)->get_value())); break;
   case V_SgTemplateFunctionDefinition:
   case V_SgFunctionDefinition: 
             res = CHECK_NAME(input, tname, "FunctionDecl"); 
             if (res) {
                 if (arg != 0) {
                    SgFunctionDefinition * def = isSgFunctionDefinition(input); 
                    assert(def != 0);
                    c = AstInterface::GetChildrenList(def->get_declaration());
                    c[2] = def->get_body();
                    c.resize(3);
                 }
                 break;
             }
   case V_SgClassDefinition: 
   case V_SgBasicBlock: res = CHECK_NAME2(input,tname,"StmtBlock", "LIST");
         if (res && arg != 0) {
           if (tname == "LIST") c= AstInterface::GetChildrenList(input); 
           else c.push_back(input); 
         }
         break;
   case V_SgInitializedName:  res = CHECK_NAME(input,tname, "TypeInfo");
       c.push_back(isSgInitializedName(input)->get_type());
       c.push_back(input); c.push_back(isSgInitializedName(input)->get_initializer());
       if (isSgAssignInitializer((SgNode*)c[2]) != 0) c[2]=static_cast<SgAssignInitializer*>((SgNode*)c[2])->get_operand();
       break;
   case V_SgAssignInitializer: return ClassifyAst(static_cast<SgAssignInitializer*>(input)->get_operand(), tname, arg, c); 
   case V_SgVariableDeclaration:  {
      res = CHECK_NAME(input, tname, "DeclStmt"); 
      SgInitializedNamePtrList m=isSgVariableDeclaration(input)->get_variables();
      if (!res) {
         if (m.size() == 1) return ClassifyAst(m[0], tname, arg, c); 
         else   res= CHECK_NAME(input,tname, "LIST");
      }
      if (res && arg != 0) {
            for (SgInitializedNamePtrList::const_iterator p = m.begin(); p != m.end(); ++p) {
                c.push_back(*p);
            }
       }
      }
       break;
   case V_SgForInitStatement: {
       AstInterface::AstList m = AstInterface::GetChildrenList(input);
       if (m.size() == 1) res=ClassifyAst(m[0], tname, arg, c); 
       else { c = m; res= CHECK_NAME(input,tname, "LIST");}  break;
     }
   case V_SgFunctionCallExp: {
     AstInterface::AstList m = AstInterface::GetChildrenList(input);
     SgNode* op = (SgNode*)m[0];
     if (tname == "Bop" || tname == "Uop" || tname == "ArrayAccess") {
       if (arg == 0) arg = STRING(tname);
       switch (op->variantT()) {
         case V_SgDotExp: 
               res = MatchAstWithPatternHelp(isSgDotExp(op)->get_rhs_operand(), arg);
               if (res != 0) { c.push_back(isSgDotExp(op)->get_lhs_operand()); }
               break; 
         default: res = MatchAstWithPatternHelp(op, arg); 
             break;
       }
       if (res) {
          SgExpressionPtrList opds = isSgExprListExp((SgNode*)m[1])->get_expressions();
          std::cerr << "children number: " << opds.size() << "\n";
          if (opds.size() != 0) {
            for (int i = 0; i < opds.size(); ++i)
                  c.push_back(opds[i]);
          }
       }
     }
     else {
          res=CHECK_NAME(input,tname,"FunctionCall"); 
          if (res) { c.push_back(op); c.push_back(static_cast<SgFunctionCallExp*>(input)->get_args()); }
     } 
     break;
    }
   case V_SgClassType: if (!CHECK_NAME2(input,tname, "ClassType", "StructType")) return false;
      if (arg != 0) {
        c.push_back(input); c.push_back(isSgClassType(input)->get_declaration()); }break;
   case V_SgReferenceType: if (!CHECK_NAME(input,tname, "RefType")) return false;
      if (arg != 0) c.push_back(isSgReferenceType(input)->get_base_type()); 
       break;
   case V_SgPointerType: if (!CHECK_NAME(input,tname, "PtrType")) return false;
      if (arg != 0) c.push_back(isSgPointerType(input)->get_base_type()); 
      break;
   case V_SgExprStatement: res = CHECK_NAME(input,tname,"ExpStmt"); break;
   case V_SgTemplateFunctionDeclaration:
   case V_SgTemplateInstantiationFunctionDecl:
   case V_SgFunctionDeclaration: res=CHECK_NAME(input,tname,"FunctionDecl"); 
        if (arg != 0) { c=AstInterface::GetChildrenList(input); 
            assert(c.size() >= 3);
            SgFunctionDefinition *def = isSgFunctionDefinition((SgNode*)c[2]);
            if (def != 0) c[2] = def->get_body();
        }
        break;
 
   case V_SgTemplateMemberFunctionDeclaration:
   case V_SgMemberFunctionDeclaration: res=CHECK_NAME(input,tname, "MemberFunctionDecl"); 
        if (arg != 0) { c=AstInterface::GetChildrenList(input); c.push_back(isSgMemberFunctionDeclaration(input)->get_associatedClassDeclaration()->get_type()); }
        break;
   case V_SgReturnStmt: res=CHECK_NAME(input,tname,"Return"); break;
   case V_SgAssignOp:  res=CHECK_NAME(input,tname,"Assign"); break;
   case V_SgTypedefDeclaration: res=CHECK_NAME(input,tname, "TypeDef"); break;
   case V_SgPntrArrRefExp: res=CHECK_NAME(input,tname, "ArrayAccess"); break;
   case V_SgClassDeclaration: res=CHECK_NAME(input,tname,"ClassDecl"); break;
   case V_SgCastExp: res=CHECK_NAME(input,tname,"CastExp");  
        if (res && arg != 0) {
            c.push_back(static_cast<SgCastExp*>(input)->get_type());
            c.push_back(static_cast<SgCastExp*>(input)->get_originalExpressionTree());
        }
        break;
   case V_SgExprListExp: res=CHECK_NAME(input,tname, "LIST"); break;
   case V_SgSourceFile: res=CHECK_NAME(input,tname, "File"); break;
   case V_SgArrowExp: res=CHECK_NAME(input, tname, "Bop") && (arg==0 || CHECK_NAME3(input, arg->toString(OUTPUT_NO_DEBUG),"->",".","_")); break;
   case V_SgDotExp: res=CHECK_ARG(input,tname,"Bop", ".",arg); break;
   case V_SgSubtractOp: res=CHECK_ARG(input,tname,"Bop", "-",arg); break;
   case V_SgAddOp: res=CHECK_ARG(input,tname,"Bop", "+",arg); break;
   case V_SgMultiplyOp: res=CHECK_ARG(input,tname,"Bop", "*",arg); break;
   case V_SgDivideOp: res=CHECK_ARG(input,tname,"Bop", "/",arg); break;
   case V_SgModOp: res=CHECK_ARG(input,tname,"Bop", "%",arg); break;
   case V_SgLessThanOp: res=CHECK_ARG(input,tname,"Bop", "<",arg); break;
   case V_SgGreaterThanOp: res=CHECK_ARG(input,tname,"Bop", ">",arg); break;
   case V_SgGreaterOrEqualOp: res=CHECK_ARG(input,tname,"Bop", ">=",arg); break;
   case V_SgLessOrEqualOp: res=CHECK_ARG(input,tname,"Bop", "<=",arg); break;
   case V_SgEqualityOp: res=CHECK_ARG(input,tname,"Bop", "==",arg); break;
   case V_SgNotEqualOp: res=CHECK_ARG(input,tname,"Bop", "!=",arg); break;
   case V_SgAndOp:res=CHECK_ARG(input,tname,"Bop", "&&",arg); break;
   case V_SgOrOp: res=CHECK_ARG(input,tname,"Bop", "||",arg); break;
   case V_SgPlusAssignOp:  res=CHECK_ARG(input,tname,"Bop","+=",arg); break;
   case V_SgMinusOp: res=CHECK_ARG(input,tname,"Uop", "-",arg); break;
   case V_SgAddressOfOp: res=CHECK_ARG(input,tname,"Uop", "&",arg); break;
   case V_SgPointerDerefExp: res=CHECK_ARG(input,tname,"Uop", "*", arg); break;
   case V_SgPlusPlusOp: res=CHECK_ARG(input,tname, "VarRef", "++",arg) || CHECK_ARG(input,tname,"Uop","++",arg); break;
   case V_SgMinusMinusOp: res=CHECK_ARG(input,tname, "VarRef", "--",arg) || CHECK_ARG(input,tname,"Uop","--",arg); break;; 
   case V_SgWhileStmt: if (!(res=CHECK_NAME2(input,tname,"Nest","While"))) return false; 
        if (tname == "While") { c.push_back(isSgExprStatement(isSgWhileStmt(input)->get_condition())->get_expression()); }
        else { c.push_back(input); c.push_back(isSgWhileStmt(input)->get_body()); }
        break;
   case V_SgForStatement: if (! (res=CHECK_NAME2(input,tname,"Nest","For"))) return false; 
        if (tname == "For") { c.push_back(isSgForStatement(input)->get_for_init_stmt());
                        c.push_back(isSgForStatement(input)->get_test_expr());
                        c.push_back(isSgForStatement(input)->get_increment());  } 
        else { c.push_back(input); c.push_back(isSgForStatement(input)->get_loop_body()); }
        break;
   case V_SgIfStmt: if (!(res=CHECK_NAME2(input,tname, "Nest", "If"))) return false;
         if (tname == "If") {
        c.push_back(isSgExprStatement(isSgIfStmt(input)->get_conditional())->get_expression()); }
        else {
          c.push_back(input);
          c.push_back(isSgIfStmt(input)->get_true_body());
/* The false body is never 0; hack it for now
          if (isSgIfStmt(input)->get_false_body() == 0) children_number = 2; 
          else children_number=3;
*/
        }
        break;
   case V_SgBreakStmt: res=CHECK_NAME(input,tname,"Break"); break; 
   case V_SgFunctionParameterList: res=CHECK_NAME(input,tname,"LIST"); break;
   case V_SgFileList:  res=CHECK_NAME(input,tname,"LIST"); break;
   case V_SgBoolValExp: res=CHECK_NAME(input, tname, "Bool"); break;
   case V_SgDoubleVal: res = CHECK_NAME(input,tname,"FLOAT"); break;
   case V_SgTypeDouble: res = CHECK_NAME(input, tname, "double"); break;
   case V_SgGlobal: res=CHECK_NAME(input,tname,"DeclarationBlock"); break;
   case V_SgThisExp: res = CHECK_NAME(input,tname,"this"); break;
   case V_SgCtorInitializerList:
   case V_SgEnumDeclaration: case V_SgEnumVal:
   case V_SgNullStatement: V_SgPragma: case V_SgTemplateClassDeclaration: case V_SgNamespaceDefinitionStatement:
   case V_SgProject:  return 0;
   
   default: std::cerr << "does not recognize AST type:" << AstInterface::AstToString(input) << " vs. " << tname << "\n";
           return 0;
  }
  if (!res) return false;
  if (arg != 0 && c.size() == 0) 
    c= AstInterface::GetChildrenList(input);
  return true;
}


/*QY: This is used to support traversal of AST by the POET interpreter*/
POETAstInterface::Ast MatchAstTypeName(const POETAstInterface::Ast& n, const std::string& tname, POETCode* args)
{ 
  assert (n != 0 && tname != "") ;

  SgNode* input = (SgNode*) n;
  std::vector<POETCode*> arg_arr;
  if (args != 0) {
    POETTuple* t = dynamic_cast<POETTuple*>(args);
    if (t == 0) {
        POETList* l = dynamic_cast<POETList*>(args);
        if (l != 0) List2Vector(l, arg_arr);
        else arg_arr.push_back(args);
    }
    else {
      for (int i = 0; i < t->size(); ++i)  {
         assert(t->get_entry(i) != 0);
         arg_arr.push_back(t->get_entry(i));
         assert(arg_arr[i] == t->get_entry(i));
       }
    }
  }
  if (args != 0) {
    if (tname == "Bop") 
      { 
        if (arg_arr.size() < 3) { std::cerr <<  "ERROR: Wrong number of children for Uop: " << args->toString() << "\n"; assert(0); }
        args=arg_arr[0]; arg_arr[0] = arg_arr[1]; arg_arr[1] = arg_arr[2];  }
    else if (tname == "Uop") { 
      if (arg_arr.size() < 2) { std::cerr <<  "ERROR: Wrong number of children for Uop: " << args->toString() << "\n"; assert(0); }
        args=arg_arr[0]; arg_arr[0] = arg_arr[1]; 
    }
    else if (tname == "VarRef") {
      if (arg_arr.size() < 1) { std::cerr <<  "ERROR: Wrong number of children for Uop: " << args->toString() << "\n"; assert(0); }
       { args = arg_arr[1]; }
    }
  }
  if (tname == "ArrayAccess") args = STRING("[]");
  AstInterface::AstList c;
  if (!ClassifyAst(n, tname, args, c)) return 0;
  if (c.size() == 0) {
#ifdef DEBUG_OP
  if (!inside_unparse)
    std::cerr << "done matching type name from ClassifyAst " << AstInterface::AstToString(n) << "\n";
#endif
        return n; 
  }
  if (args == 0) {
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "return result from ClassifyAst " << AstInterface::AstToString(c[0]) << "\n";
#endif
     return c[0];
  } 
 
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "continue to match argument: " << args->toString() << "\n";
#endif

  unsigned children_number=c.size(), arg_num = arg_arr.size();
  if (tname == "LIST") {
     if (arg_num == children_number+1) {
        if (match_AST(EMPTY_LIST, arg_arr[arg_num-1], MATCH_AST_PATTERN) == 0) return 0;
        arg_num = children_number;
     }
     else if (arg_num > children_number) return 0;
   } 
   else if (arg_num != children_number) {
      std::cerr << "error: mismatching number of children:" << tname << ":" << args->toString() << ":" << arg_num << ".vs." << POETAstInterface::Ast2String(input) <<  ":" << children_number << "\n";  
      assert(0);
  }

  bool res = true;
  for (int i = 0, j=0; i < children_number && j < arg_num; ++i,++j) {
     if (arg_arr[j] == 0) { 
       std::cerr <<  "error in node " << tname << ":" << args->toString() <<  ": child " << j << " is 0\n";
       assert(0); }
     POETAstInterface::Ast cur = c[i];
     if (cur == 0 && tname != "LIST") res = match_AST(EMPTY, arg_arr[j], MATCH_AST_PATTERN); 
     else {
        POETCode* t = POETAstInterface::find_Ast2POET(cur);
        if (t != 0 && t->get_enum() != SRC_UNKNOWN)  {
            if (t == EMPTY && tname == "LIST") { j--; continue; }
std::cerr << "TRY MATCHING CHILD:" << t->toString() << " with " << arg_arr[j]->toString() << "\n";
            debug = 2;
            res = match_AST(t, arg_arr[j], MATCH_AST_PATTERN) != 0; 
            debug = 0;
        }
        else if (tname == "LIST" && j == arg_num-1 && i < children_number-1) {
           POETCode* r = 0;
std::cerr << "CONVERTING to POET LIST\n";
           for (int k = children_number-1; k >= i; --k) {
              r = LIST(POETAstInterface::Ast2POET(c[k]),r);
           }
           res = match_AST(r, arg_arr[j], MATCH_AST_PATTERN) != 0;
        } 
        else res = MatchAstWithPatternHelp(cur, arg_arr[j]) != 0;
     }
     if (res == false) return 0;
  }
  return n;
}

POETAstInterface::Ast MatchAstWithPatternHelp(const POETAstInterface::Ast& n, POETCode* pat)
{ 
  assert (n != 0 && pat != 0) ;
  POETGenTypeName op;
  pat->visit(&op);
  LocalVar* lhs = op.get_lhs();
  for (int i = 0; i < op.num_of_types(); ++i) {
       std::string tname = op.get_tname(i);
       POETCode* args = op.get_args(i);
       POETAstInterface::Ast res1 = MatchAstTypeName(n, tname, args);
       if (res1 != 0) {
#ifdef DEBUG_OP
    std::cerr << "Matched Ast Type " << i << ":" << tname << "\n"; 
#endif
         if (lhs != 0) {
#ifdef DEBUG_OP
    std::cerr << "Try Assign Ast To " << lhs->toString() << "\n"; 
#endif
           lhs->get_entry().set_code(POETAstInterface::Ast2POET(res1));
#ifdef DEBUG_OP
    std::cerr << "Assign Ast To " << lhs->toString() << "\n"; 
#endif
         }
         return res1;
       }
  }
  return 0;
}

POETCode* POETAstInterface::getAstAttribute(const Ast& n, POETCode* attr)
{
   std::string tname;
   switch (attr->get_enum()) {
    case SRC_LVAR: 
      tname = "get_"+static_cast<LocalVar*>(attr)->get_entry().get_name()->toString(OUTPUT_NO_DEBUG);
      break;
    default: tname="map";
   } 
   AstInterface::AstList c;
   AstInterface::Ast res = 0;
   if (ClassifyAst(n, tname, attr, c)) { assert(c.size() == 1); res = c[0]; }
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "return result from getAstAttribute " << tname << "==> " << (res == 0?"0":AstInterface::AstToString(res)) << "\n";
#endif
   if (res != 0) return Ast2POET(res);
   return 0;
}


POETCode* POETAstInterface::MatchAstWithPattern(const Ast& n, POETCode* pat)
{
   POETAstInterface::Ast res = MatchAstWithPatternHelp(n, pat);
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "return result from MatchAstWithPattern " << pat->toString() << "==> " << (res == 0?"0":AstInterface::AstToString(res)) << "\n";
#endif

   if (res != 0) return Ast2POET(res);
   return 0;
}

class POET_ROSE_Unparser;
class POETCode_ext_delegate : public UnparseDelegate
{
  Unparser* unparser; 
  std::set <SgNode*> modify;
  std::ostream *out;
  POETCode* linebreak;
  SgUnparse_Info info;
  std::set<SgNode*> replaced;
  static POETCode_ext_delegate* inst;
 public:
  POETCode_ext_delegate() : unparser(0), out(0) {
        linebreak=ASTFactory::inst()->new_string("\n");
  }
  static POETCode_ext_delegate* get_inst() { 
     if (inst == 0) 
         inst = new POETCode_ext_delegate();
     return inst;
   }
  void set_modify(SgNode* n1) { std::cerr << "MODIFY: " << n1 << "\n"; modify.insert(n1); 
    Sg_File_Info* fileInfo=n1->get_file_info();
     assert(fileInfo != 0);
     fileInfo->setTransformation();
     fileInfo->setOutputInCodeGeneration();

     SgLocatedNode* locatedNode = isSgLocatedNode(n1);
     if (locatedNode != NULL) {
         locatedNode->setTransformation();
         locatedNode->setOutputInCodeGeneration();
     }
     markTransformationsForOutput(n1); 
    }
  void set_output_if_none(std::ostream* _out) { if (out==0) out = _out; }
  void set_unparser(std::ostream& _out, std::string _filename) {
   if (out == 0)  out=&_out;
   if (unparser == 0) {
       bool UseAutoKeyword                = false;
       bool generateLineDirectives        = false;
       bool useOverloadedOperators        = false;
       bool num                           = false;
       bool _this                         = false;
       bool caststring                    = false;
       bool _debug                        = false;
       bool _class                        = true;
       bool _forced_transformation_format = true;
       bool _unparse_includes             = false; 
       Unparser_Opt roseOptions( UseAutoKeyword,
                                    generateLineDirectives,
                                    useOverloadedOperators,
                                    num,
                                    _this,
                                    caststring,
                                    _debug,
                                    _class,
                                    _forced_transformation_format,
                                    _unparse_includes );
       unparser = new Unparser(out, _filename, roseOptions,0, this);
   }
  }
  void unparse(SgNode* input, int align)
  { 
      assert(out != 0);
      if (unparser == 0) { (*out) << input->unparseToString(); return; }
#ifdef DEBUG_UNPARSE
      std::cerr << "Outside of ROSE unparsing " << input->sage_class_name() << "\n";
#endif
     switch (input->variantT()) {
      case V_SgTemplateInstantiationFunctionDecl:
      {
#ifdef DEBUG_UNPARSE
        std::cerr << "unparsing " << input->sage_class_name() << "\n";
#endif
        SgFunctionDeclaration* d = isSgFunctionDeclaration(input);
        assert(d != 0);
        std::string curname = d->get_qualified_name().getString();
        if (curname.rfind("::") == 0) {
          //roseUnparser->u_exprStmt->unparseAttachedPreprocessingInfo(d,*info,PreprocessingInfo::before);
          //out << "\n";
          //info->set_SkipFunctionDefinition();
          //roseUnparser->u_exprStmt->unparseFuncDeclStmt(d, *info);
          //info->unset_SkipFunctionDefinition();
          //out << "\n";
          if (d->get_definition() != 0)
              unparse(d->get_definition(), align); 
          //out << ";\n";
       }
        break;
      }
      case V_SgTypeDouble: (*out) << "double"; break;
      case V_SgInitializedName:
          (*out) << static_cast<SgInitializedName*>(input)->get_name().getString();
          break;
      case V_SgIntVal: (*out) << static_cast<SgIntVal*>(input)->get_value(); break;
      case V_SgClassType: (*out) << static_cast<SgClassType*>(input)->get_name().str(); break; 
/*
     case V_SgPragmaDeclaration:
      {
         out << "#pragma ";
         POETTuple* c = dynamic_cast<POETTuple*>(res);
         assert(c != 0);
         code_gen(out, c->get_entry(0)); 
         roseUnparser->cur << " "; roseUnparser->cur.insert_newline(1,align);
         code_gen(out, c->get_entry(1), 0, 0, align); 
         break;
      }
*/
     default:  
        SgStatement* s = isSgStatement(input);
        if (s != 0) {
      POETCode_ext_delegate::get_inst()->set_modify(input);
      info.unset_SkipSemiColon();
      info.unset_unparsedPartiallyUsingTokenStream();
            unparser->u_exprStmt->unparseStatement(s, info);
        }
        else  {
          SgExpression* e = isSgExpression(input);
          if (e == 0) {
             std::cerr << "Error: unexpected ROSE node:" << input->class_name() << "\n";
             assert(0);
          }
          unparser->u_exprStmt->unparseLanguageSpecificExpression(e, info);
        }
     }
 };
  virtual bool unparse_statement( SgStatement* stmt, SgUnparse_Info& _info, UnparseFormat& f) 
  {
     if (replaced.find(stmt) != replaced.end()) return false;
#ifdef DEBUG_UNPARSE
std::cerr << "checking stmt: " << stmt->sage_class_name() << " : " << stmt << "\n";
#endif
     if (stmt->variantT() == V_SgGlobal) {
         std::string filename = stmt->get_file_info()->get_filename();
         set_unparser(*f.output_stream(),filename);
// _info.set_outputCompilerGeneratedStatements();
           return false;
      }
      POETCode* t = POETAstInterface::find_Ast2POET(stmt);
      if (t != 0 && t->get_enum() != SRC_UNKNOWN) {
#ifdef DEBUG_UNPARSE
      std::cerr << "unparsing POET modification: " << t->toString() << "\n";
#endif
         replaced.insert(stmt);
         info = _info;
         switch (stmt->get_parent()->variantT()) {
          case V_SgBasicBlock:
             f.insert_newline(1, f.current_indent());
          default: break;
         }
         code_gen(*f.output_stream(), t,0, 0, f.current_indent());  
         return true; 
      }
/*
      if (t != 0) return false;
      std::string curfilename;
      if (stmt->get_file_info() != 0) curfilename = stmt->get_file_info()->get_filename(); 
      if (filename != curfilename && modify.find(stmt) == modify.end())
{
std::cerr << "SKIPPING " << stmt << "\n";
           return true;
}
*/
      return false;
  }
};
POETCode_ext_delegate* POETCode_ext_delegate::inst=0;

POETCode* POETAstInterface::visitAstChildren(const Ast& rawinput, POETCodeVisitor* op)
{
  SgNode* input = (SgNode*) rawinput;
  bool hasres = false;
  AstInterface::AstList c = AstInterface::GetChildrenList((SgNode*)input);
  for (AstInterface::AstList::const_iterator p = c.begin(); p != c.end(); ++p) {
     SgNode* n = (SgNode*) (*p);
     if (n != 0) {   
       POETCode_ext tmp(n);
       POETCode* t = POETAstInterface::find_Ast2POET(n);
       if (t == 0) t=&tmp;
       POETCode* curres = op->apply(t); 
       if (curres != 0 && curres != t) {
#ifdef DEBUG_MOD
  if (!inside_unparse)
          std::cerr << "changing AST " << n << " : " << n->class_name() << n->unparseToString() << "=>" << curres->get_className() << ":" << curres->toString() << "\n";
#endif
          set_Ast2POET(n, curres);
          hasres = true;
       }
     }
  }
  if (hasres) {
   SgNode* p = input;
   while (p != 0 && isSgFunctionDefinition(p)==0) p = p->get_parent();
   if (p != 0 && isSgFunctionDefinition(p)) {
     SgFunctionDeclaration* d = isSgFunctionDefinition(p)->get_declaration();
     if (isSgTemplateInstantiationFunctionDecl(d)) {
        POETCode_ext_delegate::get_inst()->set_modify(d); 
      }
   }
   POETCode* res = 0;
   switch (input->variantT()) {
    case V_SgCastExp:
    case V_SgAssignInitializer:
    case V_SgExprStatement: 
        res = find_Ast2POET(c[0]); 
        assert(res != 0);
        break;
    case V_SgDotExp:
     {
      POETCode* v1 = Ast2POET(c[1]);
      if (v1->toString(OUTPUT_NO_DEBUG) == "operator()") 
           res = Ast2POET(c[0]);
      else res = CODE_ACC("Bop",TUPLE3(STRING("."), Ast2POET(c[0]), v1)); 
      break;
     }
    case V_SgLessThanOp: res = CODE_ACC("Bop",TUPLE3(STRING("<"),Ast2POET(c[0]), Ast2POET(c[1]))); break;
    case V_SgSubtractOp: res = CODE_ACC("Bop",TUPLE3(STRING("-"),Ast2POET(c[0]), Ast2POET(c[1]))); break;
    case V_SgAddOp: res = CODE_ACC("Bop",TUPLE3(STRING("+"),Ast2POET(c[0]), Ast2POET(c[1]))); break;
    case V_SgMultiplyOp: res = CODE_ACC("Bop",TUPLE3(STRING("*"),Ast2POET(c[0]), Ast2POET(c[1]))); break;
    case V_SgDivideOp: res = CODE_ACC("Bop",TUPLE3(STRING("/"),Ast2POET(c[0]), Ast2POET(c[1]))); break;
    case V_SgAssignOp: res = CODE_ACC("Assign",PAIR(Ast2POET(c[0]), Ast2POET(c[1]))); break;
    case V_SgFunctionCallExp: res = CODE_ACC("FunctionCall",PAIR(Ast2POET(c[0]), Ast2POET(c[1]))); break;
    case V_SgGreaterThanOp: res = CODE_ACC("Bop",TUPLE3(STRING(">"),Ast2POET(c[0]), Ast2POET(c[1]))); break;
    case V_SgGreaterOrEqualOp: res = CODE_ACC("Bop",TUPLE3(STRING(">="),Ast2POET(c[0]), Ast2POET(c[1]))); break;
   case V_SgLessOrEqualOp: res = CODE_ACC("Bop",TUPLE3(STRING("<="),Ast2POET(c[0]), Ast2POET(c[1]))); break;
   case V_SgEqualityOp: res = CODE_ACC("Bop",TUPLE3(STRING("=="),Ast2POET(c[0]),Ast2POET(c[1]))); break;
   case V_SgNotEqualOp: res = CODE_ACC("Bop",TUPLE3(STRING("!="),Ast2POET(c[0]),Ast2POET(c[1]))); break;
   case V_SgAndOp: res = CODE_ACC("Bop",TUPLE3(STRING("&&"),Ast2POET(c[0]),Ast2POET(c[1]))); break;
   case V_SgOrOp: res = CODE_ACC("Bop",TUPLE3(STRING("||"),Ast2POET(c[0]),Ast2POET(c[1]))); break;
   case V_SgArrowExp: res = CODE_ACC("Bop",TUPLE3(STRING("->"),Ast2POET(c[0]),Ast2POET(c[1]))); break;
   case V_SgModOp: res = CODE_ACC("Bop", TUPLE3(STRING("%"),Ast2POET(c[0]),Ast2POET(c[1]))); break;
   case V_SgPointerDerefExp: res = CODE_ACC("Uop",PAIR(STRING("*"),Ast2POET(c[0]))); break;
   case V_SgMinusOp: res = CODE_ACC("Uop",PAIR(STRING("-"),Ast2POET(c[0]))); break;
   case V_SgAddressOfOp: res = CODE_ACC("Uop",PAIR(STRING("&"),Ast2POET(c[0]))); break;
   case V_SgPntrArrRefExp: res = CODE_ACC("ArrayAccess",PAIR(Ast2POET(c[0]),Ast2POET(c[1]))); break;
   case V_SgPlusPlusOp: res = CODE_ACC("VarRef",PAIR(Ast2POET(c[0]),STRING("++"))); break;
   case V_SgMinusMinusOp: res = CODE_ACC("VarRef",PAIR(Ast2POET(c[0]),STRING("--"))); break;
   case V_SgPlusAssignOp: res = CODE_ACC("Bop",TUPLE3(STRING("+="),Ast2POET(c[0]),Ast2POET(c[1]))); break;
   case V_SgBitAndOp: res = CODE_ACC("Bop",TUPLE3(STRING("&"),Ast2POET(c[0]),Ast2POET(c[1]))); break;
   case V_SgBitComplementOp: res = CODE_ACC("Uop",PAIR(STRING("~"),Ast2POET(c[0]))); break;
   default: {
      SgStatement *s = isSgStatement(input);
      if (s == 0)  {
           std::cerr << "Missing case: " << input->class_name() << ":" << input->unparseToString() << "\n";
           assert(0); // missing any non-statement case?
      }
      else 
         std::cerr << "skipping setting modification for statement:" << s->class_name() << ":" << s->unparseToString() << "\n";
    }
  }
  //if (res != 0) {
       //std::cerr << "setting modification for " << input->class_name() << ":" << input->unparseToString() << "=>" << res->toString() << "\n";
       //POETAstInterface::set_Ast2POET(input, res); 
   //  }
   return res;
  } 
  return 0;
}

void POETAstInterface::unparse(const Ast & n, std::ostream& out, int align)
{
   SgNode * input = (SgNode*)n;
  POETCode_ext_delegate *dele = POETCode_ext_delegate::get_inst();

   SgProject* sageProject=isSgProject(input); 
   if (sageProject!=0) {
#ifdef DEBUG_UNPARSE
  if (!inside_unparse)
        std::cerr << "unparsing SgProject at the top\n";
#endif
       inside_unparse=true;
       unparseProject(sageProject, 0, dele);
       inside_unparse=false;
    return;
  }
#ifdef DEBUG_UNPARSE
  if (!inside_unparse)
        std::cerr << "trying to unparse : " << AstInterface::AstToString(n) << "\n";
#endif
  dele->set_output_if_none(&out); 
  dele->unparse(input, align);
}

std::string POETAstInterface::Ast2String(const Ast & n)
  { 
    POETCode* t = POETAstInterface::find_Ast2POET(n);
    if (t != 0 && t->get_enum() != SRC_UNKNOWN)  { return t->toString(); }

    SgNode* input=(SgNode*)n;
    std::string res;

    switch (input->variantT()) {
    case V_SgPragma: res = static_cast<SgPragma*>(input)->get_pragma(); break;
    case V_SgVarRefExp: res = static_cast<SgVarRefExp*>(input)->get_symbol()->get_name().str(); break; 
    case V_SgFunctionRefExp: res = static_cast<SgFunctionRefExp*>(input)->get_symbol()->get_name().str(); break;
    case V_SgInitializedName: res = static_cast<SgInitializedName*>(input)->get_name(); break;
    case V_SgIntVal:  res = static_cast<SgIntVal*>(input)->unparseToString(); break;
    default: {
       std::string res = input->class_name() + "{";
       AstInterface::AstList c = AstInterface::GetChildrenList(n);
       for (AstInterface::AstList::const_iterator p = c.begin(); p != c.end(); ++p) {
          SgNode* cur = (SgNode*) *p;
          if (cur != 0) res = res + Ast2String(*p) + ",";
       }
       res = res + "}";
    }
   }
    return res;
  }


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

extern std::vector<std::string> extra_input;
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

void poet2vector(POETCode* args, std::vector<POETCode*>& arg_arr)
{
  if (args == 0) return;
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
  switch (input->variantT()) { 
  case V_SgIntVal:  return ICONST(static_cast<SgIntVal*>(input)->get_value()); 
  case V_SgName: return STRING(static_cast<SgName*>(input)->getString());
  case V_SgExprListExp: {
     SgExprListExp* block = isSgExprListExp(input);
     res=ROSE_2_POET_list(block->get_expressions(), 0, tmp);
      POETAstInterface::set_Ast2POET(input, res); 
      return res;
     }
  case V_SgExprStatement:
       if (isSgForStatement(input->get_parent())!=0) {
          return Ast2POET(isSgExprStatement(input)->get_expression()); 
       }
       break;
  }
    
#ifdef DEBUG_OP
  if (!inside_unparse)
std::cerr << "creating AST wrap for " << POETAstInterface::Ast2String(n) << "\n";
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
     if (curname != "")
         argvList.push_back(curname);
     for (int i = 0; i < extra_input.size();++i) {
            argvList.push_back(extra_input[i]);
     }
  }
  assert(argvList.size() > 1);
  for (int i = 0; i < argvList.size(); ++i)
      std::cerr << argvList[i] << " ";
  std::cerr << "\n";
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
#define CHECK_NAME4(n,tname,str1, str2, str3, str4) (CHECK_NAME2(n,tname,str1,str2) || CHECK_NAME2(n,tname,str3,str4))
#define CHECK_ARG(n,tname, t1, t2, arg)  (CHECK_NAME(n, tname, t1) && (arg==0 || CHECK_NAME2(n, arg->toString(OUTPUT_NO_DEBUG), t2,"_")))

class POETGenTypeName : public POETCodeVisitor
{
  std::vector<std::string> tname_vec;
  std::vector<POETCode*> arg_vec;
  std::vector<LocalVar*> lhs;

 public: 
  POETGenTypeName()  {}
  std::string get_tname(int i) { return tname_vec[i]; }
  unsigned num_of_types() { return tname_vec.size(); }
  POETCode* get_args(int i) { assert(i < arg_vec.size()); return arg_vec[i]; }
  std::vector<LocalVar*>& get_lhs() { return lhs; }
  virtual void visitList(POETList* l) 
    { tname_vec.push_back("LIST"); arg_vec.push_back(l); }
  virtual void visitTuple( POETTuple* v)  { tname_vec.push_back("TUPLE"); arg_vec.push_back(v); }
  virtual void visitNULL(POETNull* l) { tname_vec.push_back("LIST"); arg_vec.push_back(0); }
  virtual void visitUnknown(POETCode_ext* ext) { 
     SgNode* input=(SgNode*)ext->get_content();
     switch (input->variantT()) {
       case V_SgVarRefExp:
          tname_vec.push_back("Name"); arg_vec.push_back(STRING(static_cast<SgVarRefExp*>(input)->get_symbol()->get_name().str()));
          if (debug_pattern())
               std::cerr << "converting name: " <<  arg_vec[arg_vec.size()-1]->toString() << "\n";
          break;
       case V_SgInitializedName:
          tname_vec.push_back("Name"); arg_vec.push_back(STRING(static_cast<SgInitializedName*>(input)->get_name()));
          if (debug_pattern())
               std::cerr << "converting name: " <<  arg_vec[arg_vec.size()-1]->toStr
          break;
       default:
          if (debug_pattern())
               std::cerr << "unrecognized ext type: " <<  input->sage_class_name() << "\n";
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
     case POET_OP_POND:  {
         int cursize = tname_vec.size();
         POETCode* c = eval_AST(v->get_arg(0));
         c->visit(this);
         for (int i = cursize; i < tname_vec.size(); ++i) {
           arg_vec[i] = v->get_arg(1); 
         }
        }
         break;
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
         lhs.push_back(dynamic_cast<LocalVar*>(eval_AST(v->get_arg(0)))); assert(lhs.back() != 0); break;
     case POET_OP_DOT:  eval_AST(v)->visit(this); break;
     default: 
      std::cerr << "Unexpected operator : " << v->toString() << "\n";
         break;
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
           if (a->get_lhs()->get_enum() == SRC_LVAR)
              lhs.push_back(static_cast<LocalVar*>(a->get_lhs()));
           else lhs.push_back(dynamic_cast<LocalVar*>(eval_AST(a->get_lhs()))); 
           assert(lhs.back() != 0);
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
              lhs.push_back(v);
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
  if (varname[0] == ':' && varname[1] == ':') {
    return CompareTypeName(varname.substr(2, varname.size()-2), arg);
  }
  int p = varname.find("<"); // template parameters 
  int p1 = p;
  while (p1 > 0 && varname[p1-1]==' ') p1 = p1 - 1;
  while (p < varname.size() && varname[p+1]==' ') p = p + 1;
  std::string varname1=varname; 
  std::string template_arg_name = "";
  if (p > 0 && p < varname.size()) {
    int q = varname.find(">"); // template parameters 
    assert(q > p && q < varname.size());
    while (q > p && varname[q-1]==' ') q = q - 1;
    varname1=varname.substr(0,p1);
    template_arg_name = varname.substr(p+1,q-p-1);
  }
  std::vector<POETCode*> arg_arr;
  if (template_arg_name != "") {
     poet2vector(arg, arg_arr);
     arg = arg_arr[0];
  } 
  POETGenTypeName op;
  arg->visit(&op);
  std::vector<LocalVar*>& lhs = op.get_lhs();
  for (int i = 0; i < op.num_of_types(); ++i) {
     std::string tmpname = op.get_tname(i);
     if (tmpname == "_"); 
     else if (tmpname != "Name") { 
         assert(op.get_args(i) != 0);
         if (!CompareTypeName(varname1, op.get_args(i))) continue;
     }
     else {
       assert(op.get_args(i) != 0);
       std::string tmpname = op.get_args(i)->toString(OUTPUT_NO_DEBUG);
#ifdef DEBUG_OP
       std::cerr << "comparing type: \"" << varname1 << "\" vs \"" << tmpname << "\"\n";
#endif
       if (tmpname == "Bop" || tmpname == "Uop") 
      { switch (varname1[0]) {
        case '+': case '-': case '=': case '*': case '/': case '%': case '&': case '|': case '!':break; 
        case '_': if (varname1.size() == 1) break;
        default: continue;
       }
      }
      else if (varname1 != tmpname) {
#ifdef DEBUG_OP
         std::cerr << "strings are not the same\n";
#endif
         continue;
      }
     }
     if (arg_arr.size() > 1) {
       POETCode* arg1 = arg_arr[1];
       if (arg1 != 0 && ! CompareTypeName(template_arg_name, arg1)) continue;
     }
     if (lhs.size() > i && lhs[i] != 0)
     lhs[i]->get_entry().set_code(STRING(varname1));
     return true;
  }
  return false;
}

AstInterface::AstList GetAstChildrenList(SgNode* input, bool backward=false)
{
  AstInterface::AstList c = AstInterface::GetChildrenList(input);
  switch (input->variantT()) {
     case V_SgIfStmt:
     case V_SgWhileStmt: {
      if (((SgNode*)c[0])->variantT()==V_SgExprStatement)
         c[0] = static_cast<SgExprStatement*>(c[0])->get_expression();
     }
      break;
     case V_SgPragmaDeclaration: {
           c.push_back(static_cast<SgPragmaDeclaration*>(input)->get_pragma());
           AstInterface::AstList l = AstInterface::GetBlockStmtList(input->get_parent());
           for (AstInterface::AstList::const_iterator p = l.begin(); p != l.end(); ++p) {
              SgNode* cur = (SgNode*)*p;
              if (cur == input) {
                 ++p;
                 cur = (SgNode*)*p;
                 c.push_back(cur); break;
              }
           }
      }
      break;
      case V_SgBasicBlock:
           c = AstInterface::GetBlockStmtList(input);
           AstInterface::AstList::iterator p1 = c.end(), p2 = p1;
           unsigned size = 0;;
           for (AstInterface::AstList::iterator p = c.begin(); p != c.end(); ++p) {
              SgNode* cur = (SgNode*)*p;
              if (cur == 0) { continue; }
              size ++;
              if (p1 != c.end() && p1 != p) {
                 *p1 = *p; p1++; }
              p2 = p;
              while (cur->variantT() == V_SgPragmaDeclaration) {
                  p2++; cur = (SgNode*)*p2; *p2 = 0;
                  if (p1 == c.end()) { p1 = p2; }
              }
           }
           if (size < c.size()) { c.resize(size); }
           break;
  }
  if(backward) {
     AstInterface::AstList c1; int i = 0;
     for (AstInterface::AstList::const_reverse_iterator p = c.rbegin(); p != c.rend(); ++p) {
       ++i;
        c1.push_back(*p);
     }
     return c1;
  }
  return c;
}

bool compareSgNode(SgNode* n1, SgNode* n2)
{
  if (n1 == n2) return true;
  assert(n1!=0 && n2!=0);
  if (n1->variantT() != n2->variantT()) return false;
#ifdef DEBUG_OP
  std::cerr << "comparing " << n1->unparseToString() << " with " << n2->unparseToString() << "\n";
#endif
  return POETAstInterface::Ast2String(n1) == POETAstInterface::Ast2String(n2);
}

POETCode* MatchAstWithPatternHelp(const POETAstInterface::Ast& n, POETCode* pat);

bool ClassifyAst(const POETAstInterface::Ast& n, const std::string& tname, POETCode* arg, AstInterface::AstList& c, POETCode*& poet_res)
{
  SgNode* input = (SgNode*) n; 
  static SgName tmpname; // used to keep SageName content from children;
#ifdef DEBUG_OP
  if (!inside_unparse)
std:: cerr << "trying to match " << input->class_name() << ":" << POETAstInterface::Ast2String(input) << " vs " << tname << ":" << ((arg==0)? "NULL:" : arg->toString()) << "\n";
#endif
  if (tname == "UNKNOWN") {
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
  else if (tname == "Name" || tname == "GlobalName" || tname == "ScopedName" || tname == "STRING") {
     std::string varname;
     switch (input->variantT()) {
     case V_SgVarRefExp: 
         varname = static_cast<SgVarRefExp*>(input)->get_symbol()->get_name().getString(); break;
     case V_SgClassType:
         varname = static_cast<SgClassType*>(input)->get_name().getString(); break;
     case V_SgInitializedName:
         varname = static_cast<SgInitializedName*>(input)->get_name().getString(); break;
     case V_SgVariableSymbol:
         varname = static_cast<SgVariableSymbol*>(input)->get_name().getString(); break;
     case V_SgTemplateFunctionRefExp: case V_SgTemplateMemberFunctionRefExp: case V_SgMemberFunctionRefExp: 
     case V_SgFunctionRefExp: 
         varname = static_cast<SgFunctionRefExp*>(input)->get_symbol()->get_name().getString(); 
         if (varname.find("operator") == 0) varname = varname.substr(8,varname.size()-8);  
         break;
     case V_SgTemplateInstantiationFunctionDecl: 
     case V_SgTemplateFunctionDeclaration:
     case V_SgMemberFunctionDeclaration:
     case V_SgFunctionDeclaration: {
         varname = isSgFunctionDeclaration(input)->get_name().str();
         if (varname.find("operator") == 0) varname = varname.substr(8,varname.size()-8);  
         int t = varname.find(" <");
         if (t > 0 && t < varname.size()) varname = varname.substr(0,t);  
         }
         break;
     case V_SgName: varname = isSgName(input)->getString(); break;
     case V_SgNullExpression: varname = "NULL"; break;
     case V_SgArrowExp: 
           if (isSgThisExp(static_cast<SgArrowExp*>(input)->get_lhs_operand()))
               return ClassifyAst(static_cast<SgArrowExp*>(input)->get_rhs_operand(), tname, arg, c, poet_res);
     case V_SgDotExp: 
          if (ClassifyAst(static_cast<SgDotExp*>(input)->get_rhs_operand(), "Name", STRING("()"), c, poet_res)) 
              return ClassifyAst(static_cast<SgDotExp*>(input)->get_lhs_operand(), tname, arg, c, poet_res);
     default: 
#ifdef DEBUG_OP
  if (!inside_unparse)
  std::cerr << "not considered a name: " << POETAstInterface::Ast2String(input) << "\n";
#endif
         return false;
     }
     if (arg == 0 || varname == arg->toString(OUTPUT_NO_DEBUG) || CompareTypeName(varname, arg)) { poet_res = STRING(varname); return true; }
     else return false;
  }
  else if (tname == "get_type") { /* retrieve type of n */
     SgExpression* e = isSgExpression(input);
     if (e != 0) { c.push_back(e->get_type()); return true; }
     else { return false; }
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
     case SRC_UNKNOWN:
     case SRC_STRING: { 
           SgScopeStatement *scope = isSgScopeStatement(input);
           assert(scope != 0);
           if (scope == 0) return false;
           SgSymbol *s = LookupVar(arg->toString(OUTPUT_NO_DEBUG), scope);
assert(s != 0);
           if (s == 0) return false;
           c.push_back(s->get_type());
           return true;}
     default:
         std::cerr << "Error: unrecognized argument for map:" << arg->get_className() << ":" << arg->toString(DEBUG_OUTPUT) << "\n";
         assert(0);
     }  
     return false;
  }
  bool res = false;
  if (tname == "_") { 
#ifdef DEBUG_OP
  if (!inside_unparse)
  std::cerr << "matching to any\n";
#endif
     res = true; /* yes to anything, represented by "_" */ }
  switch (input->variantT()) {
     case V_SgVarRefExp: 
     case V_SgTemplateFunctionRefExp: case V_SgTemplateMemberFunctionRefExp: case V_SgMemberFunctionRefExp: 
     case V_SgFunctionRefExp: 
     case V_SgName: return res;
    case V_SgPragmaDeclaration: if (!res) res = CHECK_NAME(input,tname,"Pragma");  
        if (res != 0 && arg != 0) {
#ifdef DEBUG_OP
std::cerr << "RECOGNIZING PRAGMA: \n";
#endif
           c.push_back(static_cast<SgPragmaDeclaration*>(input)->get_pragma());
           
           AstInterface::AstList l = AstInterface::GetBlockStmtList(input->get_parent());
           for (AstInterface::AstList::const_iterator p = l.begin(); p != l.end(); ++p) {
              SgNode* cur = (SgNode*)*p;
              if (cur == input) { 
                 ++p; 
                 cur = (SgNode*)*p;
                 c.push_back(cur); break; 
              }
           }
        }
        break;
   case V_SgCharVal:
          if (!res) res=(CHECK_NAME(input, tname, "Char") &&
         (arg==0 || dynamic_cast<POETIconst*>(arg)->get_val() == static_cast<SgCharVal*>(input)->get_value()));
          break;
   case V_SgLongIntVal:
   case V_SgIntVal:  if (!res) res=(CHECK_NAME2(input, tname, "INT", "INT_UL") && 
         (arg==0 || dynamic_cast<POETIconst*>(arg)->get_val() == static_cast<SgIntVal*>(input)->get_value())); 
          break;
   case V_SgBasicBlock: if (!res) res = CHECK_NAME(input,tname,"StmtBlock");
        if (!res && arg != 0) {
          if (isSgBasicBlock(input)->get_statements().size()==1)
             c.push_back(static_cast<SgBasicBlock*>(input)->get_statements()[0]);
          else
             c.push_back(input);
        }
   case V_SgClassDefinition: 
   //case V_SgTemplateClassDefinition:
   case V_SgBasicBlock: if (!res) res = CHECK_NAME2(input,tname,"StmtBlock", "LIST");
        if (!res && static_cast<SgBasicBlock*>(input)->get_statements().size()==1)
             return ClassifyAst(static_cast<SgBasicBlock*>(input)->get_statements()[0],tname,arg,c, poet_res); 
         break;
   case V_SgInitializedName:  if (!res) res = CHECK_NAME(input,tname, "TypeInfo");
       if (res) {
         if (arg == 0) c.push_back(input);
         else {
           c.push_back(static_cast<SgInitializedName*>(input)->get_type());
           //c.push_back(input);
           tmpname = static_cast<SgInitializedName*>(input)->get_name();
           c.push_back(&tmpname);
           c.push_back(static_cast<SgInitializedName*>(input)->get_initializer());
           if (isSgAssignInitializer((SgNode*)c[2]) != 0) c[2]=static_cast<SgAssignInitializer*>((SgNode*)c[2])->get_operand();
         }
       }
       break;
   case V_SgVariableDeclaration:  {
      if (!res) res = CHECK_NAME(input, tname, "DeclStmt"); 
      SgInitializedNamePtrList m=isSgVariableDeclaration(input)->get_variables();
      if (!res) {
         if (m.size() == 1) return ClassifyAst(m[0], tname, arg, c, poet_res); 
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
       if (!res) res = CHECK_NAME(input, tname, "DeclStmt");
       if (!res) {
          AstInterface::AstList m = GetAstChildrenList(input);
          if (m.size() == 1) res=ClassifyAst(m[0], tname, arg, c, poet_res);
          else { c = m; res= CHECK_NAME(input,tname, "LIST");}
       }
       else if (arg != 0) { c = GetAstChildrenList(input); }
       break;
     }
   case V_SgExprStatement:  
       if (tname == "Bop") { 
          res = ClassifyAst(isSgExprStatement(input)->get_expression(),tname,arg,c, poet_res); 
       }
       if (!res) res = CHECK_NAME2(input,tname,"Stmt","ExpStmt");
       break;
   case V_SgFunctionCallExp: {
     AstInterface::AstList m = GetAstChildrenList(input);
     SgNode* op = (SgNode*)m[0]; unsigned argnum = m.size()-1;
     if ( (tname == "Bop" && argnum == 2)|| (tname == "Uop" && argnum==1) || (tname == "ArrayAccess" && argnum==2)) {
       POETCode* opname = (arg == 0)? STRING(tname) : arg;
       switch (op->variantT()) {
         case V_SgDotExp: 
               res = MatchAstWithPatternHelp(isSgDotExp(op)->get_rhs_operand(), arg);
               if (res != 0) { c.push_back(isSgDotExp(op)->get_lhs_operand()); }
               break; 
         default: res = MatchAstWithPatternHelp(op, arg); 
             break;
       }
     }
     else {if (!res) res=CHECK_NAME(input,tname,"FunctionCall"); 
           if (res && arg != 0) c.push_back(m[0]);
          } 
     if (res && arg != 0) {
          SgExpressionPtrList opds = isSgExprListExp((SgNode*)m[1])->get_expressions();
          if (opds.size() == 1) c.push_back(opds[0]);
          else if (tname == "FunctionCall") c.push_back(m[1]);
          else {  
             for (int i = 0; i < opds.size(); ++i)
                  c.push_back(opds[i]);
          }
     }
     break;
    }
   case V_SgClassType: if (!res) res=CHECK_NAME3(input,tname, "TypeName", "ClassType", "StructType");
      if (res && arg != 0) {
        c.push_back(input); c.push_back(isSgClassType(input)->get_declaration()); 
      }
      break;
   case V_SgReferenceType: if (!res) res = CHECK_NAME(input,tname, "RefType");
      if (res && arg != 0) c.push_back(isSgReferenceType(input)->get_base_type()); 
       break;
   case V_SgPointerType: if (!res) res=CHECK_NAME(input,tname, "PtrType");
      if (res && arg != 0) c.push_back(isSgPointerType(input)->get_base_type()); 
      break;
   case V_SgTemplateMemberFunctionDeclaration:
   case V_SgMemberFunctionDeclaration: if (!res) res=CHECK_NAME(input,tname, "MemberFunctionDecl"); 
   case V_SgTemplateFunctionDeclaration:
   case V_SgTemplateInstantiationFunctionDecl:
   case V_SgFunctionDeclaration: if (!res) res=CHECK_NAME(input,tname,"FunctionDecl"); 
        if (res && arg != 0) { c=AstInterface::GetChildrenList(input); 
            int size = c.size();
            assert(size >= 3);
            if (tname == "FunctionDecl") {
              if (size == 4) { c.resize(3);  size = 3; }
            }
            SgFunctionDefinition *def = isSgFunctionDefinition((SgNode*)c[2]);
            if (def != 0) c[2] = def->get_body();
            c.push_back(c[size-1]);
            for (int i = size; i > 0; i--) {
                  c[i-1] = c[i-2]; 
            }
            c[0] = input;
            if (tname == "MemberFunctionDecl") {
              SgMemberFunctionDeclaration* mf = isSgMemberFunctionDeclaration(input);
              assert (mf != 0);
              c.push_back(mf->get_associatedClassDeclaration()->get_type()); 
            }
        }
        break;
   case V_SgReturnStmt: if (!res) res=CHECK_NAME(input,tname,"Return"); break;
   case V_SgAssignOp:  if (!res) res=CHECK_NAME(input,tname,"Assign"); break;
   case V_SgTypedefDeclaration: if (!res) res=CHECK_NAME(input,tname, "TypeDef"); break;
   case V_SgPntrArrRefExp: if (!res) res=CHECK_NAME(input,tname, "ArrayAccess"); break;
   case V_SgTemplateClassDeclaration: 
   case V_SgClassDeclaration: if (!res) res=CHECK_NAME(input,tname,"ClassDecl"); break;
   case V_SgCastExp: if (!res) res=CHECK_NAME(input,tname,"CastExp");  
        if (res && arg != 0) {
            c.push_back(static_cast<SgCastExp*>(input)->get_type());
            c.push_back(static_cast<SgCastExp*>(input)->get_originalExpressionTree());
        }
        break;
   case V_SgExprListExp: if (!res) res=CHECK_NAME(input,tname, "LIST"); break;
   case V_SgSourceFile: if (!res) res=CHECK_NAME(input,tname, "File"); break;
   case V_SgArrowExp: if (!res) res=(tname == "PtrAccess" || CHECK_NAME(input, tname, "Bop") && (arg==0 || CHECK_NAME3(input, arg->toString(OUTPUT_NO_DEBUG),"->",".","_"))); 
            break;
   case V_SgDotExp: if (!res) res=CHECK_ARG(input,tname,"Bop", ".",arg); break;
   case V_SgLshiftOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "<<",arg); break;
   case V_SgRshiftOp: if (!res) res=CHECK_ARG(input,tname,"Bop", ">>",arg); break;
   case V_SgSubtractOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "-",arg); break;
   case V_SgAddOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "+",arg); break;
   case V_SgMultiplyOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "*",arg); break;
   case V_SgDivideOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "/",arg); break;
   case V_SgModOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "%",arg); break;
   case V_SgLessThanOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "<",arg); break;
   case V_SgGreaterThanOp: if (!res) res=CHECK_ARG(input,tname,"Bop", ">",arg); break;
   case V_SgGreaterOrEqualOp: if (!res) res=CHECK_ARG(input,tname,"Bop", ">=",arg); break;
   case V_SgLessOrEqualOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "<=",arg); break;
   case V_SgEqualityOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "==",arg); break;
   case V_SgNotEqualOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "!=",arg); break;
   case V_SgAndOp:if (!res) res=CHECK_ARG(input,tname,"Bop", "&&",arg); break;
   case V_SgOrOp: if (!res) res=CHECK_ARG(input,tname,"Bop", "||",arg); break;
   case V_SgSizeOfOp: if (!res) res=CHECK_ARG(input,tname,"FunctionCall", "sizeof",arg); break;
   case V_SgPlusAssignOp:  if (!res) res=CHECK_ARG(input,tname,"Bop","+=",arg); break;
   case V_SgMinusAssignOp:  if (!res) res=CHECK_ARG(input,tname,"Bop","-=",arg); break;
   case V_SgMinusOp: if (!res) res=CHECK_ARG(input,tname,"Uop", "-",arg); break;
   case V_SgAddressOfOp: if (!res) res=CHECK_ARG(input,tname,"Uop", "&",arg); break;
   case V_SgNotOp: if (!res) res=CHECK_ARG(input,tname,"Uop", "!",arg); break;
   case V_SgBitComplementOp: if (!res) res=CHECK_ARG(input,tname,"Uop", "~",arg); break;
   case V_SgPointerDerefExp: if (!res) res=CHECK_ARG(input,tname,"Uop", "*", arg); break;
   case V_SgPlusPlusOp: if (!res) res=CHECK_ARG(input,tname, "VarRef", "++",arg) || CHECK_ARG(input,tname,"Uop","++",arg); break;
   case V_SgMinusMinusOp: if (!res) res=CHECK_ARG(input,tname, "VarRef", "--",arg) || CHECK_ARG(input,tname,"Uop","--",arg); break;; 
   case V_SgWhileStmt: if (!res) res=CHECK_NAME2(input,tname,"Nest","While");
       if (res && arg != 0) {
        if (tname == "Nest") { c.push_back(input); c.push_back(isSgWhileStmt(input)->get_body()); }
        else { c.push_back(isSgExprStatement(isSgWhileStmt(input)->get_condition())->get_expression()); }
       }
        break;
   case V_SgForStatement: if (!res) res=CHECK_NAME2(input,tname,"Nest","For");
      if (res && arg != 0) {
        if (tname == "Nest") { c.push_back(input); c.push_back(isSgForStatement(input)->get_loop_body()); }
        else  { c.push_back(isSgForStatement(input)->get_for_init_stmt());
                        c.push_back(isSgForStatement(input)->get_test_expr());
                        c.push_back(isSgForStatement(input)->get_increment());  } 
      }
        break;
   case V_SgIfStmt: if (!res) res=CHECK_NAME2(input,tname, "Nest", "If");
       if (res) {
         if (tname == "Nest" && arg != 0) {
          c.push_back(input);
          c.push_back(isSgIfStmt(input)->get_true_body());
/* The false body is never 0; hack it for now
          if (isSgIfStmt(input)->get_false_body() == 0) children_number = 2; 
          else children_number=3;
*/
        }
        else if (arg != 0) {
        c.push_back(isSgExprStatement(isSgIfStmt(input)->get_conditional())->get_expression()); }
      }
        break;
   case V_SgTemplateInstantiationDecl:  if (!res) res = CHECK_NAME(input, tname, "TemplateInstantiation");
       if (res && arg != 0) {
         SgTemplateArgumentPtrList& args = static_cast<SgTemplateInstantiationDecl*>(input)->get_templateArguments();
         for (SgTemplateArgumentPtrList::const_iterator p = args.begin(); p != args.end(); ++p) {
           SgTemplateArgument* targ = *p;
          c.push_back(targ->get_type());
         }
         break;
       }
   case V_SgBreakStmt: if (!res) res=CHECK_NAME(input,tname,"Break"); break; 
   case V_SgFunctionParameterList: if (!res) res=CHECK_NAME(input,tname,"LIST"); break;
   case V_SgFileList:  if (!res) res=CHECK_NAME(input,tname,"LIST"); break;
   case V_SgBoolValExp: if (!res) res=CHECK_NAME(input, tname, "Bool"); break;
   case V_SgDoubleVal: if (!res) res = CHECK_NAME(input,tname,"FLOAT"); break;
   case V_SgTypeDouble: if (!res) res = CHECK_NAME(input, tname, "double"); break;
   case V_SgGlobal: if (!res) res=CHECK_NAME(input,tname,"DeclarationBlock"); break;
   case V_SgThisExp: if (!res) res = CHECK_NAME(input,tname,"this"); break;
   case V_SgStringVal: if (!res) res=CHECK_NAME(input,tname,"String"); break;
   case V_SgCtorInitializerList:
   case V_SgEnumDeclaration: case V_SgEnumVal:
   case V_SgNullStatement: case V_SgPragma: case V_SgNamespaceDefinitionStatement: case V_SgTypedefType: case V_SgTemplateInstantiationMemberFunctionDecl: case V_SgModifierType: case V_SgAssignInitializer: case V_SgConstructorInitializer: case V_SgNullExpression:
   case V_SgProject:  
   case V_SgTemplateClassDefinition: case V_SgNamespaceDeclarationStatement: case V_SgTemplateInstantiationDefn:  
   case V_SgTemplateVariableDeclaration: case V_SgConditionalExp:  case V_SgTemplateParameterVal: case V_SgUsingDirectiveStatement:
    case V_SgUsingDirectiveStatement:  case V_SgClassDefinition:
    case V_SgCatchStatementSeq: 
   break;
   
   default: 
#ifdef DEBUG_OP
     std::cerr << "does not recognize AST type:" << input->sage_class_name() << ":" << POETAstInterface::Ast2String(input) << " vs. " << tname << "\n";
#endif
           break;
  }
  if (!res) return false;
  if (arg != 0 && c.size() == 0) 
    c= GetAstChildrenList(input);
  return true;
}

/*QY: This is used to support traversal of AST by the POET interpreter*/
POETCode* MatchAstTypeName(const POETAstInterface::Ast& n, const std::string& tname, POETCode* args)
{ 
  assert (n != 0 && tname != "") ;

  SgNode* input = (SgNode*) n;
  std::vector<POETCode*> arg_arr;
  poet2vector(args, arg_arr);
  if (args != 0) {
    if (tname == "Bop") 
      { 
        if (arg_arr.size() < 3) { std::cerr <<  "ERROR: Wrong number of children for Bop: " << args->toString() << "\n"; assert(0); }
        args=arg_arr[0]; arg_arr[0] = arg_arr[1]; arg_arr[1] = arg_arr[2];  arg_arr.resize(2); }
    else if (tname == "Uop") { 
      if (arg_arr.size() < 2) { std::cerr <<  "ERROR: Wrong number of children for Uop: " << args->toString() << "\n"; assert(0); }
        args=arg_arr[0]; arg_arr[0] = arg_arr[1]; arg_arr.resize(1);  
    }
    else if (tname == "VarRef") {
      if (arg_arr.size() < 1) { std::cerr <<  "ERROR: Wrong number of children for VarRef: " << args->toString() << "\n"; assert(0); }
       { args = arg_arr[1]; arg_arr.resize(1); }
    }
  }
  AstInterface::AstList c; POETCode*  poet_res=0;
  if (!ClassifyAst(n, tname, args, c, poet_res)) return 0;
  if (c.size() == 0) {
#ifdef DEBUG_OP
    std::cerr << "done matching type name from ClassifyAst " << POETAstInterface::Ast2String(n) << "\n";
#endif
        return (poet_res==0)? POETAstInterface::Ast2POET(n) : poet_res; 
  }
  if (args == 0) {
#ifdef DEBUG_OP
std::cerr << "return result from ClassifyAst " << "\n";
#endif
    if (poet_res != 0) return poet_res;
    else { if (c.size() != 1) {
             std::cerr << "Error: expecting a single result to return but has " << c.size() << " for " << POETAstInterface::Ast2String(n) <<  "\n";
             assert(0);
           }
           return POETAstInterface::Ast2POET(c[0]); }
  } 
 
#ifdef DEBUG_OP
std::cerr << "continue to match argument: " << args->toString() << "\n";
#endif

  unsigned children_number=c.size(), arg_num = arg_arr.size();
  if (poet_res != 0) {  
     if (match_AST(poet_res, args, MATCH_AST_PATTERN)) return poet_res; 
     return 0; 
  }
  else if (tname == "LIST" || tname == "StmtBlock") {
     if (arg_num == children_number+1) {
        if (match_AST(EMPTY_LIST, arg_arr[arg_num-1], MATCH_AST_PATTERN) == 0) return 0;
        arg_num = children_number;
     }
     else if (arg_num > children_number) return 0;
   } 
   else if (arg_num != children_number) {
      std::cerr << "error: mismatching number of children:" << tname << ":" << args->toString() << ":" << arg_num << ".vs." << input->class_name() << ":" << POETAstInterface::Ast2String(input) <<  ":" << children_number << "\n";  
      assert(0);
  }

  bool res = true;
  for (int i = 0, j=0; i < children_number && j < arg_num; ++i,++j) {
     if (arg_arr[j] == 0) { 
       std::cerr <<  "error in node " << tname << ":" << args->toString() <<  ": child " << j << " is 0\n";
       assert(0); }
     POETAstInterface::Ast cur = c[i];
     if (cur == 0 && tname != "LIST") res = match_AST(EMPTY, arg_arr[j], MATCH_AST_PATTERN); 
     else if (cur == input && input->variantT() == V_SgInitializedName) {
         /* a special case handing: need to translate cur to a string*/
         POETCode* t = STRING(static_cast<SgInitializedName*>(input)->get_name().getString());
         res = match_AST( t, arg_arr[j], MATCH_AST_PATTERN) != 0;
      }
     else {
        POETCode* t = POETAstInterface::find_Ast2POET(cur);
        if (t != 0 && t->get_enum() != SRC_UNKNOWN)  {
            if (t == EMPTY && tname == "LIST") { j--; continue; }
            debug = 2;
            res = match_AST(t, arg_arr[j], MATCH_AST_PATTERN) != 0; 
            debug = 0;
        }
        else if (tname == "LIST" && j == arg_num-1 && i < children_number-1) {
           POETCode* r = 0;
           for (int k = children_number-1; k >= i; --k) {
              r = LIST(POETAstInterface::Ast2POET(c[k]),r);
           }
           res = match_AST(r, arg_arr[j], MATCH_AST_PATTERN) != 0;
        } 
        else res = MatchAstWithPatternHelp(cur, arg_arr[j]) != 0;
     }
     if (res == false) return 0;
  }
  return POETAstInterface::Ast2POET(n);
}

POETCode* MatchAstWithPatternHelp(const POETAstInterface::Ast& n, POETCode* pat)
{ 
  assert (n != 0 && pat != 0) ;
  POETGenTypeName op;
  pat->visit(&op);
  std::vector<LocalVar*>& lhs = op.get_lhs();
  for (int i = 0; i < op.num_of_types(); ++i) {
       std::string tname = op.get_tname(i);
       POETCode* args = op.get_args(i);
       POETCode* res1 = MatchAstTypeName(n, tname, args);
       if (res1 != 0) {
#ifdef DEBUG_OP
    std::cerr << "Matched Ast Type " << i << ":" << tname << "\n"; 
#endif
         for (int i = 0; i < lhs.size(); ++i) {
           lhs[i]->get_entry().set_code(res1);
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
   POETCode* poet_res=0;
   if (ClassifyAst(n, tname, attr, c, poet_res)) { 
        if (poet_res != 0) return poet_res;
        assert(c.size() == 1); 
        return Ast2POET(c[0]);
   }
   return 0;
}


POETCode* POETAstInterface::MatchAstWithPattern(const Ast& n, POETCode* pat)
{
   POETCode*  res = MatchAstWithPatternHelp(n, pat);
#ifdef DEBUG_OP
std::cerr << "return result from MatchAstWithPattern " << pat->toString() << "==> " << (res == 0?"0":res->toString()) << "\n";
#endif

   return res;
}

class POET_ROSE_Unparser;
class POETCode_ext_delegate : public UnparseDelegate
{
  Unparser* unparser; 
  std::set <SgNode*> modify;
  std::ostream *out;
  std::string filename;
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
  void set_modify(SgNode* n1) { 
    modify.insert(n1); 
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
       bool _this                         = true;
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
  std::ostream* get_output_stream() { return out; }
  std::string get_filename() { return filename; }
  void unparse(SgNode* input, int align)
  { 
      assert(out != 0);
      if (unparser == 0) { (*out) << input->unparseToString(); return; }
#ifdef DEBUG_UNPARSE
      std::cerr << "Outside of ROSE unparsing " << input->sage_class_name() << "\n";
#endif
      if (replaced.find(input) == replaced.end()) {
         POETCode* t = POETAstInterface::find_Ast2POET(input);
         if (t != 0 && t->get_enum() != SRC_UNKNOWN)  {
             switch (input->variantT()) {
             case V_SgFunctionParameterList:
             case V_SgExprListExp: 
                  t = CODE_ACC("ExpList", t);
             };
             code_gen(*out, t,0, 0, align);  
             return;
         }
      }
     switch (input->variantT()) {
      case V_SgTemplateInstantiationFunctionDecl:
      {
        SgTemplateInstantiationFunctionDecl* d = isSgTemplateInstantiationFunctionDecl(input);
        assert(d != 0);
        std::string curname = d->get_templateDeclaration()->get_name().getString();
        AstInterface::AstList c = GetAstChildrenList(input);
        assert(c.size() == 3);
        if (c[1] != 0) unparse((SgNode*)c[1], align);
        else (*out) << "void ";
        (*out) << curname << "(";
        if (c[0] != 0)
           unparse((SgNode*)c[0], align);
        (*out) << ")";
        if (c[2] != 0) { 
           SgFunctionDefinition *d = isSgFunctionDefinition((SgNode*)c[2]);
           if (d != 0) 
               unparse(d->get_body(), align);
           else unparse((SgNode*)c[2], align);
       }
       else (*out) << ";";
       }
        break;
      case V_SgFunctionParameterList: {
        SgFunctionParameterList* pars = isSgFunctionParameterList(input);
        SgInitializedNamePtrList names = pars->get_args();
        SgInitializedNamePtrList::const_iterator p=names.begin();
        SgInitializedName* cur = *p;
       unparse(cur, align);
        for ( ++p; p != names.end();++p) {
           (*out) << ","; cur = *p; unparse(cur, align);
        }
        }
        break;
/* case V_SgPragmaDeclaration:
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
      case V_SgTypeDouble: (*out) << "double"; break;
      case V_SgTypeInt:(*out) << "int"; break;
      case V_SgVariableSymbol: (*out) << (static_cast<SgVariableSymbol*>(input)->get_name().getString()); break;
      case V_SgInitializedName:
          unparse(static_cast<SgInitializedName*>(input)->get_type(), align); 
          (*out) << " " << static_cast<SgInitializedName*>(input)->get_name().getString();
          break;
      case V_SgDoubleVal:
                (*out) << static_cast<SgDoubleVal*>(input)->get_value(); break;
      case V_SgLongIntVal:
      case V_SgIntVal: (*out) << static_cast<SgIntVal*>(input)->get_value(); break;
      case V_SgClassType: (*out) << static_cast<SgClassType*>(input)->get_name().str(); break; 
      case V_SgTypedefType: (*out) << static_cast<SgTypedefType*>(input)->get_name().str(); break;
      case V_SgThisExp: (*out) << "this"; break;
      case V_SgPragma: (*out) << static_cast<SgPragma*>(input)->get_pragma(); break;
      case V_SgForInitStatement:  {
           SgUnparse_Info info1 = info;
           info1.set_SkipSemiColon();
           info1.set_inConditional();
           unparser->u_exprStmt->unparseForInitStmt(isSgForInitStatement(input), info1);
           }
           break; 
     default:  
        SgUnparse_Info info1 = info;
        info1.set_inConditional();
        SgStatement* s = isSgStatement(input);
        if (s != 0) {
            unparser->u_exprStmt->unparseStatement(s, info1);
        }
        else  {
          SgExpression* e = isSgExpression(input);
          if (e == 0) {
             std::cerr << "Error: unexpected ROSE node:" << input->class_name() << "\n";
             assert(0);
          }
          unparser->u_exprStmt->unparseLanguageSpecificExpression(e, info1);
        }
     }
 };
  virtual bool unparse_statement( SgStatement* stmt, SgUnparse_Info& _info, UnparseFormat& f) 
  {
     if (stmt->variantT() == V_SgGlobal) { 
         info=_info; 
         filename = stmt->get_file_info()->get_filename();
         set_unparser(*f.output_stream(),filename);
      }
     if (replaced.find(stmt) != replaced.end()) return false;
#ifdef DEBUG_UNPARSE
std::cerr << "checking stmt: " << stmt->sage_class_name() << " : " << stmt << "\n";
#endif
      POETCode* t = POETAstInterface::find_Ast2POET(stmt);
      if (t != 0 && (t->get_enum() != SRC_UNKNOWN || static_cast<POETCode_ext*>(t)->get_content() != stmt)) {
#ifdef DEBUG_UNPARSE
      std::cerr << "unparsing POET modification: " << t->toString() << "\n";
#endif
         replaced.insert(stmt);
         switch (stmt->get_parent()->variantT()) {
          case V_SgBasicBlock:
             f.insert_newline(1, f.current_indent());
          default: break;
         }
         code_gen(*f.output_stream(), t,0, 0, f.current_indent());  
         return true; 
      }
      if (modify.find(stmt) != modify.end()) {
         unparse(stmt, f.current_indent());
         return true;
      }
      return false;
  }
};
POETCode_ext_delegate* POETCode_ext_delegate::inst=0;

#define  WRAP_StmtBlock(c) { if (c->get_enum() != SRC_CVAR || static_cast<CodeVar*>(c)->get_entry().get_name()->toString(OUTPUT_NO_DEBUG) != "StmtBlock") c = CODE_ACC("StmtBlock", c);  } 
POETCode* mod_Ast2POET(SgNode* input, std::vector<POETCode*>& c)
{
#ifdef DEBUG_MOD
  std::cerr << "modifying statement:" << input->class_name() << ":" << input->unparseToString() << "\n";
   for (int i = 0; i < c.size(); ++i){  
     std::cerr << "Child " << i << ":";
     if (c[i] == 0) std::cerr << "NULL\n";
     else std::cerr << c[i]->toString() << "\n";
   }
#endif
   POETCode* res = 0;
   switch (input->variantT()) {
    case V_SgExprListExp:  {
         int s = c.size();
         res=c[s-1]; s = s - 1;
         while (s > 0) {
            res=LIST(c[s-1], res);
            s = s - 1;
         }  
         break;
    }
    case V_SgCastExp:
    case V_SgAssignInitializer:
        res = c[0]; 
        assert(res != 0);
        break;
    case V_SgExprStatement: 
        res=CODE_ACC("ExpStmt", c[0]); break;
    case V_SgInitializedName: 
        res = CODE_ACC("TypeInfo", TUPLE3(POETAstInterface::Ast2POET(static_cast<SgInitializedName*>(input)->get_type()),
 STRING(isSgInitializedName(input)->get_name().getString()), c[0])); break;
    case V_SgDotExp:
     {
      POETCode* v1 = c[1];
      if (v1->toString(OUTPUT_NO_DEBUG) == "operator()") 
           res = c[0];
      else res = CODE_ACC("Bop",TUPLE3(STRING("."), c[0], v1)); 
      break;
     }
    case V_SgPragmaDeclaration: res = CODE_ACC("Pragma", PAIR(c[1], c[2])); break;
    case V_SgPragma: res=STRING(static_cast<SgPragma*>(input)->get_pragma());

    case V_SgLessThanOp: res = CODE_ACC("Bop",TUPLE3(STRING("<"),c[0], c[1])); break;
    case V_SgSubtractOp: res = CODE_ACC("Bop",TUPLE3(STRING("-"),c[0], c[1])); break;
    case V_SgAddOp: res = CODE_ACC("Bop",TUPLE3(STRING("+"),c[0], c[1])); break;
    case V_SgMultiplyOp: res = CODE_ACC("Bop",TUPLE3(STRING("*"),c[0], c[1])); break;
    case V_SgDivideOp: res = CODE_ACC("Bop",TUPLE3(STRING("/"),c[0], c[1])); break;
    case V_SgAssignOp: res = CODE_ACC("Assign",PAIR(c[0], c[1])); break;
    case V_SgFunctionCallExp: res = CODE_ACC("FunctionCall",PAIR(c[0], c[1])); break;
    case V_SgGreaterThanOp: res = CODE_ACC("Bop",TUPLE3(STRING(">"),c[0], c[1])); break;
    case V_SgGreaterOrEqualOp: res = CODE_ACC("Bop",TUPLE3(STRING(">="),c[0], c[1])); break;
   case V_SgLessOrEqualOp: res = CODE_ACC("Bop",TUPLE3(STRING("<="),c[0], c[1])); break;
   case V_SgEqualityOp: res = CODE_ACC("Bop",TUPLE3(STRING("=="),c[0],c[1])); break;
   case V_SgNotEqualOp: res = CODE_ACC("Bop",TUPLE3(STRING("!="),c[0],c[1])); break;
   case V_SgAndOp: res = CODE_ACC("Bop",TUPLE3(STRING("&&"),c[0],c[1])); break;
   case V_SgOrOp: res = CODE_ACC("Bop",TUPLE3(STRING("||"),c[0],c[1])); break;
   case V_SgArrowExp: res = CODE_ACC("Bop",TUPLE3(STRING("->"),c[0],c[1])); break;
   case V_SgModOp: res = CODE_ACC("Bop", TUPLE3(STRING("%"),c[0],c[1])); break;
   case V_SgPointerDerefExp: res = CODE_ACC("Uop",PAIR(STRING("*"),c[0])); break;
   case V_SgMinusOp: res = CODE_ACC("Uop",PAIR(STRING("-"),c[0])); break;
   case V_SgAddressOfOp: res = CODE_ACC("Uop",PAIR(STRING("&"),c[0])); break;
   case V_SgPntrArrRefExp: res = CODE_ACC("ArrayAccess",PAIR(c[0],c[1])); break;
   case V_SgPlusPlusOp: res = CODE_ACC("VarRef",PAIR(c[0],STRING("++"))); break;
   case V_SgMinusMinusOp: res = CODE_ACC("VarRef",PAIR(c[0],STRING("--"))); break;
   case V_SgPlusAssignOp: res = CODE_ACC("Bop",TUPLE3(STRING("+="),c[0],c[1])); break;
   case V_SgBitAndOp: res = CODE_ACC("Bop",TUPLE3(STRING("&"),c[0],c[1])); break;
   case V_SgBitComplementOp: res = CODE_ACC("Uop",PAIR(STRING("~"),c[0])); break;
   case V_SgBasicBlock:  
       for (int i = 0; i < c.size(); ++i) 
           if (c[i] == EMPTY) c[i] = 0;
       res = Vector2List(c); 
       break;
   case V_SgForStatement: 
       res=CODE_ACC("Nest", PAIR(CODE_ACC("For", TUPLE3(c[0], c[1], c[2])), c[3])); break;
   case V_SgIfStmt:  {
        POETCode* ifcond = CODE_ACC("If",c[0]);  
        res=CODE_ACC("Nest", PAIR(ifcond, c[1])); 
        if (c.size() == 3) {
          res=CODE_ACC("StmtList",LIST(res,CODE_ACC("Nest",PAIR(CODE_ACC("Else",PAIR(EMPTY, ifcond)),c[2]))));   
        }
       break;
      }
   case V_SgReturnStmt: res = CODE_ACC("Return", c[0]); break;
   case V_SgWhileStmt: 
       res=CODE_ACC("Nest", PAIR(CODE_ACC("While",c[0]), c[1])); break;
   case V_SgFunctionDefinition: 
       WRAP_StmtBlock(c[0]);
   default: {
#ifdef DEBUG_MOD
         std::cerr << "skip setting modification for :" << input->class_name() << ":" << input->unparseToString() << "\n";
#endif
         return 0;
    }
  }
  return res;
}

POETCode* POETAstInterface::visitAstChildren(const Ast& rawinput, POETCodeVisitor* op, bool backward)
{
  SgNode* input = (SgNode*) rawinput;
  bool hasres = false;
  AstInterface::AstList c = GetAstChildrenList(input, backward);
  std::vector<POETCode*> children;
  AstInterface::AstList c = AstInterface::GetChildrenList(input);
  int v = input->variantT();
  if ( (v == V_SgIfStmt || v == V_SgWhileStmt)) {
      if (((SgNode*)c[0])->variantT()==V_SgExprStatement) 
         c[0] = static_cast<SgExprStatement*>(c[0])->get_expression();
  }
  //ClassifyAst(rawinput, "_", EMPTY, c);
  std::vector<POETCode*> children;
  for (AstInterface::AstList::const_iterator p = c.begin(); p != c.end(); ++p) {
     SgNode* n = (SgNode*) (*p);
     if (n != 0) {
       if (isSgAssignInitializer(n) != 0) n=static_cast<SgAssignInitializer*>(n)->get_operand();
       POETCode_ext tmp(n);
       POETCode* t = POETAstInterface::find_Ast2POET(n);
       if (t == 0) t=&tmp;
/*
       if (n == input) {   
        switch (n->variantT()) {
        case V_SgInitializedName: t = STRING(static_cast<SgInitializedName*>(n)->get_name().getString()); break;
        case V_SgClassType: t = STRING(static_cast<SgClassType*>(input)->get_name().getString()); break;
        default:
            std::cerr << "UNEXPECTED:" << n->class_name() << "\n";
            assert(0);
        }
       }
*/
       POETCode* curres = op->apply(t); 
       if (curres != 0 && curres != t) {
          children.push_back(curres);
          hasres = true;
       }
       else children.push_back(0);
     }
     else children.push_back(0);
  }
  if (hasres) {
   for (int i = 0; i < c.size(); ++i) {
      if (children[i] == 0) {
          children[i] = Ast2POET(c[i]);
      }
      else if (((SgNode*)c[i])->variantT() == V_SgPragmaDeclaration) {
         children[i+1] = EMPTY;
      }
   }
   POETCode* res = mod_Ast2POET(input, children);
   if (res == 0) {
     for (int i = 0; i < c.size(); ++i) {
         if (c[i] != 0) 
           set_Ast2POET(c[i], children[i]);
     }
   }
   SgNode* p = input;
   while (p != 0 && isSgFunctionDefinition(p)==0) p = p->get_parent();
   if (p != 0 && p != input && isSgFunctionDefinition(p)) {
     SgFunctionDeclaration* d = isSgFunctionDefinition(p)->get_declaration();
     if (isSgTemplateInstantiationFunctionDecl(d)) {
        POETCode_ext_delegate::get_inst()->set_modify(d); 
        SgNode* parent = d->get_parent();
        AstInterface::AstList sibs = AstInterface::GetChildrenList(parent);
        for (AstInterface::AstList::const_iterator psibs = sibs.begin(); psibs != sibs.end(); ++psibs) {
            SgNode* cur = (SgNode*)(*psibs);
            if (cur != d && isSgTemplateInstantiationFunctionDecl(cur) && static_cast<SgTemplateInstantiationFunctionDecl*>(cur)->get_qualified_name () == d->get_qualified_name()) {
                POETCode_ext_delegate::get_inst()->set_modify(cur);
                c = AstInterface::GetChildrenList(cur);
                AstInterface::AstList c1 = AstInterface::GetChildrenList(d);
                for (int i = 0; i < c.size(); ++i) {
                    if (c[i] != 0)  {
                           set_Ast2POET(c[i], Ast2POET(c1[i]));
                    }
                }
                break;
            }
        }
      }
   }
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
        std::cerr << "trying to unparse : " << POETAstInterface::Ast2String(n) << "\n";
#endif
  dele->set_output_if_none(&out); 
  dele->unparse(input, align);
}

std::string POETAstInterface::Ast2String(const Ast & n)
  { 
    POETCode* t = POETAstInterface::find_Ast2POET(n);
    if (t != 0 && t->get_enum() != SRC_UNKNOWN)  { 
       set_Ast2POET(n,0);
//std::cerr << "AST " << ((SgNode*)n)->class_name() << " MAPPED TO POET " << t->get_className() << " \n";
       std::string res =  t->toString(); 
       set_Ast2POET(n,t);
       return res;
    }

    SgNode* input=(SgNode*)n;
    std::string res;

    switch (input->variantT()) {
    case V_SgPragma: return static_cast<SgPragma*>(input)->get_pragma(); 
    case V_SgVarRefExp: return static_cast<SgVarRefExp*>(input)->get_symbol()->get_name().str(); 
    case V_SgTemplateFunctionRefExp: case V_SgTemplateMemberFunctionRefExp: case V_SgMemberFunctionRefExp: 
    case V_SgFunctionRefExp: return static_cast<SgFunctionRefExp*>(input)->get_symbol()->get_name().str(); 
    case V_SgInitializedName: return static_cast<SgInitializedName*>(input)->get_name().str(); 
    case V_SgVariableSymbol: return static_cast<SgVariableSymbol*>(input)->get_name().str(); 
    case V_SgIntVal:  return static_cast<SgIntVal*>(input)->get_valueString(); 
    case V_SgTemplateInstantiationFunctionDecl: 
    case V_SgTemplateFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
    case V_SgFunctionDeclaration:
         res=res + isSgFunctionDeclaration(input)->get_name().str();
         break;
    case V_SgFunctionDefinition:
         res = res + isSgFunctionDefinition(input)->get_declaration()->get_name().str();
         break;
     }
    if (res != "") res = res + ":" ;
    res  = res + input->class_name() + "{";
    AstInterface::AstList c = AstInterface::GetChildrenList(n);
    for (AstInterface::AstList::const_iterator p = c.begin(); p != c.end(); ++p) {
        SgNode* cur = (SgNode*) *p;
        if (cur != 0) res = res + Ast2String(cur) + ",";
        else res = res + "NULL" + ",";
     }
    res[res.size()-1] = '}';
    return res;
  }


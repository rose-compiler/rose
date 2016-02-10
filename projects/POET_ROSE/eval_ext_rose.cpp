
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

extern std::list<std::string> lib_dir;
extern void code_gen(std:: ostream& out, POETCode *code, POETCode* output_invoke, POETCode* listsep, int align);


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
#ifdef DEBUG
  std::cerr << "Found pragma :" << pragma_str << "\n";
#endif
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
           prev = new POETCode_ext(info, PAIR(tokens, prev));
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
  SgProject* sageProject=isSgProject(input); 
  if (sageProject != 0) {
    int filenum = sageProject->numberOfFiles(); 
    for (int i = 0; i < filenum; ++i) { 
      SgSourceFile* sageFile = isSgSourceFile(sageProject->get_fileList()[i]); 
      SgGlobal *root = sageFile->get_globalScope(); 
      SgDeclarationStatementPtrList declList = root->get_declarations ();
      POETCode* curfile = ROSE_2_POET_list(declList, 0, tmp);
      curfile = new POETCode_ext(sageFile, curfile);
      POETAstInterface::set_Ast2POET(sageFile, curfile);
      res=LIST(curfile, res);
    }
    POETAstInterface::set_Ast2POET(sageProject,res); 
    return res;
  } }
  { 
  SgBasicBlock* block = isSgBasicBlock(input);
  if (block != 0) {
    res=ROSE_2_POET_list(block->get_statements(), res, tmp);
    POETAstInterface::set_Ast2POET(block, res); 
    return res;
  } }
  { 
  SgExprListExp* block = isSgExprListExp(input);
  if (block != 0) {
    res=ROSE_2_POET_list(block->get_expressions(), 0, tmp);
    POETAstInterface::set_Ast2POET(block, res); 
    return res;
  } }
 {
  SgForStatement *f = isSgForStatement(input);
  if (f != 0) {
      POETCode* init = ROSE_2_POET_list(f->get_for_init_stmt()->get_init_stmt(),0,tmp);
      POETCode* ctrl = new POETCode_ext(f, TUPLE3(init,Ast2POET(f->get_test_expr()), Ast2POET(f->get_increment())));
      res = CODE_ACC("Nest", PAIR(ctrl,Ast2POET(f->get_loop_body())));  
      POETAstInterface::set_Ast2POET(input, res); 
      return res;
  }
  }
 {/*Added by Qian: 08/08/2014, for generating correct modified $while$ code*/
  SgWhileStmt *s = isSgWhileStmt(input);
  if (s != 0) {
    SgExprStatement* e = isSgExprStatement(s->get_condition());
    if (e != 0) {
      res = new POETCode_ext(s, PAIR(Ast2POET(e->get_expression()), Ast2POET(s->get_body())));
    } else {
      res = new POETCode_ext(s, PAIR(Ast2POET(s->get_condition()), Ast2POET(s->get_body())));
    }
    POETAstInterface::set_Ast2POET(input, res);
    return res;
  }
 }
 {/*Added by Qian: 08/08/2014, for generating correct modified $if$ code*/
  SgIfStmt *s = isSgIfStmt(input);
  if (s != 0) {
   SgExprStatement *e = isSgExprStatement(s->get_conditional()); 
   if ( e != 0) {
      res = new POETCode_ext(s, TUPLE3(Ast2POET(e->get_expression()), Ast2POET(s->get_true_body()), Ast2POET(s->get_false_body())));
   } else {
      res = new POETCode_ext(s, TUPLE3(Ast2POET(s->get_conditional()), Ast2POET(s->get_true_body()), Ast2POET(s->get_false_body())));
   }
    POETAstInterface::set_Ast2POET(input, res);
    return res;
  }
 }
 {/*Added by Qian: 11/08/2014, for generating correct modified $switch$ code*/
   SgSwitchStatement *s = isSgSwitchStatement(input);
   if (s != 0) {
     SgExprStatement *e = isSgExprStatement(s->get_item_selector());
     if (e != 0) {
       res = new POETCode_ext(s, PAIR(Ast2POET(e->get_expression()), Ast2POET(s->get_body())));
     } else {
       res = new POETCode_ext(s, PAIR(Ast2POET(s->get_item_selector()), Ast2POET(s->get_body())));
     }
   POETAstInterface::set_Ast2POET(input, res);
   return res;
   }
 }
 {
    SgVarRefExp * v = isSgVarRefExp(input);
    if (v != 0) {
       res = STRING(v->get_symbol()->get_name().str());
       POETAstInterface::set_Ast2POET(input, res); 
       return res;
    }
  }
 
  {
    SgMemberFunctionRefExp * v = isSgMemberFunctionRefExp(input);
    if (v != 0) {
       res = STRING(v->get_symbol()->get_name().str());
       POETAstInterface::set_Ast2POET(input, res); 
       return res;
    }
  }
  {
    SgIntVal * v = isSgIntVal(input);
    if (v != 0) {
       res = ICONST(v->get_value());
       POETAstInterface::set_Ast2POET(input, res); 
       return res;
    }
  }
  {
    SgInitializedName* var = isSgInitializedName(input);
    if (var != 0) {
      POETCode* name = STRING(var->get_name().str()); 
      POETCode* init = Ast2POET(var->get_initializer());
      res = new POETCode_ext(var, PAIR(name, init));
      POETAstInterface::set_Ast2POET(input, res); 
      return res;
    }
  }
  {
    SgFunctionRefExp *fn = isSgFunctionRefExp(input); 
    if (fn != 0) {
      res = STRING(fn->get_symbol()->get_name().str());
      POETAstInterface::set_Ast2POET(input, res);
      return res;
    }
  }
  /*
 {
  std::string fname;
  AstInterface::AstList params;
  AstNodeType returnType;
  AstNodePtr body;
  if (AstInterface :: IsFunctionDefinition( input, &fname, &params, (AstInterface::AstList*)0, &body, (AstInterface::AstTypeList*)0, &returnType)) {
if (body != AST_NULL)
 std::cerr << "body not empty:" << fname << "\n";
      POETCode* c = TUPLE4(STRING(fname),  ROSE_2_POET_list(params,0,tmp), 
                 STRING(AstInterface::GetTypeName(returnType)), 
                 Ast2POET(body.get_ptr()));
      res = new POETCode_ext(input, c);
      POETAstInterface::set_Ast2POET(input,res);
      return res;
  } }   
*/
  AstInterface::AstList c = AstInterface::GetChildrenList(input);
  switch (input->variantT()) {
    case V_SgCastExp:
    case V_SgAssignInitializer:
      res = Ast2POET(c[0]); 
      POETAstInterface::set_Ast2POET(input, res); return res; 
    case V_SgDotExp:
     {
      POETCode* v1 = Ast2POET(c[1]);
      if (v1->toString(OUTPUT_NO_DEBUG) == "operator()") 
           return Ast2POET(c[0]);
      res = CODE_ACC("Bop",TUPLE3(STRING("."), Ast2POET(c[0]), v1)); return res;
     }
    case V_SgLessThanOp:
      res = CODE_ACC("Bop",TUPLE3(STRING("<"),Ast2POET(c[0]), Ast2POET(c[1])));
      POETAstInterface::set_Ast2POET(input, res); return res;
    case V_SgSubtractOp:
      res = CODE_ACC("Bop",TUPLE3(STRING("-"),Ast2POET(c[0]), Ast2POET(c[1])));
      POETAstInterface::set_Ast2POET(input, res); return res;
    case V_SgAddOp:
      res = CODE_ACC("Bop",TUPLE3(STRING("+"),Ast2POET(c[0]), Ast2POET(c[1])));
      POETAstInterface::set_Ast2POET(input, res); return res;
    case V_SgMultiplyOp:
      res = CODE_ACC("Bop",TUPLE3(STRING("*"),Ast2POET(c[0]), Ast2POET(c[1])));
      POETAstInterface::set_Ast2POET(input, res); return res;
    case V_SgDivideOp:
      res = CODE_ACC("Bop",TUPLE3(STRING("/"),Ast2POET(c[0]), Ast2POET(c[1])));
      POETAstInterface::set_Ast2POET(input, res); return res;
    case V_SgAssignOp:
      res = CODE_ACC("Assign",PAIR(Ast2POET(c[0]), Ast2POET(c[1])));
      POETAstInterface::set_Ast2POET(input, res); return res;
    case V_SgFunctionCallExp:
      res = CODE_ACC("FunctionCall",PAIR(Ast2POET(c[0]), Ast2POET(c[1])));
      POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgGreaterThanOp:
	  res = CODE_ACC("Bop",TUPLE3(STRING(">"),Ast2POET(c[0]), Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgGreaterOrEqualOp:
	  res = CODE_ACC("Bop",TUPLE3(STRING(">="),Ast2POET(c[0]), Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgLessOrEqualOp:
	  res = CODE_ACC("Bop",TUPLE3(STRING("<="),Ast2POET(c[0]), Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgEqualityOp:
	  res = CODE_ACC("Bop",TUPLE3(STRING("=="),Ast2POET(c[0]),Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgNotEqualOp:
	  res = CODE_ACC("Bop",TUPLE3(STRING("!="),Ast2POET(c[0]),Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgAndOp:
	  res = CODE_ACC("Bop",TUPLE3(STRING("&&"),Ast2POET(c[0]),Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgOrOp:
	  res = CODE_ACC("Bop",TUPLE3(STRING("||"),Ast2POET(c[0]),Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgArrowExp:
	  res = CODE_ACC("Bop",TUPLE3(STRING("->"),Ast2POET(c[0]),Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgModOp:
	  res = CODE_ACC("Bop", TUPLE3(STRING("%"),Ast2POET(c[0]),Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgPointerDerefExp:
	  res = CODE_ACC("Uop",PAIR(STRING("*"),Ast2POET(c[0])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgMinusOp:
	  res = CODE_ACC("Uop",PAIR(STRING("-"),Ast2POET(c[0])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgAddressOfOp:
	  res = CODE_ACC("Uop",PAIR(STRING("&"),Ast2POET(c[0])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgPntrArrRefExp:
	  res = CODE_ACC("ArrayAccess",PAIR(Ast2POET(c[0]),Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgPlusPlusOp:
	  res = CODE_ACC("VarRef",PAIR(Ast2POET(c[0]),STRING("++")));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgMinusMinusOp:
	  res = CODE_ACC("VarRef",PAIR(Ast2POET(c[0]),STRING("--")));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgPlusAssignOp: /*This may wrong x+=y, should be Assign#(x, Bop#(+, x, y))*/
	  res = CODE_ACC("Bop",TUPLE3(STRING("+="),Ast2POET(c[0]),Ast2POET(c[1])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
	case V_SgBitAndOp://Added by Qian: 07/08/2014
          res = CODE_ACC("Bop",TUPLE3(STRING("&"),Ast2POET(c[0]),Ast2POET(c[1])));
          POETAstInterface::set_Ast2POET(input, res); return res;
        case V_SgBitComplementOp://Added by Qian: 07/08/2014
          res = CODE_ACC("Uop",PAIR(STRING("~"),Ast2POET(c[0])));
	  POETAstInterface::set_Ast2POET(input, res); return res;
  } 
  POETCode * c2 = 0; 
  if (tmp == 0) tmp=isSgTemplateInstantiationFunctionDecl(input);
   switch (c.size()) {
   case 0: break;
   case 1: c2 = Ast2POET(c[0]); break;
   case 2: c2 = PAIR(Ast2POET(c[0]),Ast2POET(c[1])); break;
   case 3: c2 = TUPLE3(Ast2POET(c[0]),Ast2POET(c[1]),Ast2POET(c[2])); break;
   case 4: c2 = TUPLE4(Ast2POET(c[0]),Ast2POET(c[1]),Ast2POET(c[2]),Ast2POET(c[3])); break;
   default: 
     //std::cerr << "too many children: " << c.size() << ":" << input->unparseToString() << "\n";
     c2 = EMPTY;
   }
  if (tmp == input) tmp = 0;
  res = new POETCode_ext(input, c2); 
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

std::string POETAstInterface::Ast2String(const Ast & n)
  { 
    SgNode* input = (SgNode*) n;
    return input->class_name();
  }

/*QY: This is used to support traversal of AST by the POET interpreter*/
bool POETAstInterface::MatchAstTypeName(const Ast& n, const std::string& tname)
{ 
#ifdef DEBUG
  std::cerr << "trying to match " << tname << " against " << AstInterface::AstToString(n) << "\n";
#endif
 if (tname == "EXP") {
     return AstInterface::IsExpression((SgNode*)n) != AST_NULL;
 }
 else if (tname == "ExpStmt") {
    SgExprStatement* s = isSgExprStatement((SgNode*)n);
    return (s != 0);
 }
 else if (tname == "FunctionDecl") {
     return AstInterface :: IsFunctionDefinition((SgNode*)n);
 }
 else if (tname == "While") {//Added by Qian: 06/08/2014
     return isSgWhileStmt((SgNode*)n) != 0;
 }
 else if (tname == "If") { //Added by Qian 06/08/2014
     return isSgIfStmt((SgNode*)n) != 0;
 }
 else if (tname == "Return") {//Added by Qian 07/08/2014
     return isSgReturnStmt((SgNode*)n) != 0;
 }
 else if (tname == "For") {
     return isSgForStatement((SgNode*)n) != 0;
 }
 else if (tname == "Assign") {
     return AstInterface::IsAssignment((SgNode*)n);
 }
 else if (tname == "Pragma") {
   return isSgPragmaDeclaration((SgNode*)n) != 0;
 }
 else if (tname == "VarDecl") {
     return isSgVariableDeclaration((SgNode*)n) != 0;
 }
 else if (tname == "Variable") {
    return isSgInitializedName((SgNode*)n);
 }
 else if (tname == "VarInit") {
    return isSgInitializedName((SgNode*)n);
 }
 //else if (tname == "Loop") {}
 //else std::cerr << "does not recognize type name:" << tname << "\n";
 return false;
}

class POETCode_ext_delegate : public UnparseDelegate
{
  std::ostream& out; 
  std::map<SgNode*, POETCode*> repl;
  static UnparseFormat *format;
 public:
  POETCode_ext_delegate(POETCode_ext* e, std::ostream& _out) : out(_out) 
  {
    SgNode* input = (SgNode*)e->get_content();
    POETCode* children = e->get_children();
    POETTuple* t = dynamic_cast<POETTuple*>(children);
    AstInterface::AstList c = AstInterface::GetChildrenList(input);
    if (t == 0) {
       assert(c.size() == 1); 
       SgNode* cur = (SgNode*)(c[0]);
       if (POETAstInterface::find_Ast2POET(cur) != children)
           repl[cur] = children;
    }
    else {
       int i=0;
       for (AstInterface::AstList::const_iterator p = c.begin(); p != c.end(); ++p,++i) {
          SgNode* cur = (SgNode*)(*p);
          POETCode *cur2 = t->get_entry(i);
          if (POETAstInterface::find_Ast2POET(cur) != cur2) repl[cur] = cur2; 
       }
    }
  }
  virtual bool unparse_statement( SgStatement* stmt, SgUnparse_Info& info, UnparseFormat& f) 
  {
      std::map<SgNode*, POETCode*>::const_iterator p = repl.find(stmt);
      if (p == repl.end()) return false;
      format=&f;
      POETCode* c = (*p).second;
      code_gen(out, c,0, 0, f.current_col());  
      return true;
  }
  static UnparseFormat& get_format() { assert(format != 0); return *format; }
};
UnparseFormat * POETCode_ext_delegate::format = 0;

void POETAstInterface::unparse(POETCode_ext* n, std::ostream& out, int align)
{ 
 static SgUnparse_Info info;
 static Unparser* roseUnparser = 0;
 static POETCode* linebreak=ASTFactory::inst()->new_string("\n");
 static POETCode* comma=ASTFactory::inst()->new_string(",");
 static bool template_only=false;
 static POETCode* left_over = 0;
 static const char* filename = 0;
 static bool force_output=false;

  SgNode * input = (SgNode*)n->get_content();
  POETCode* res = POETAstInterface::find_Ast2POET(input);
  if (res == n) {
      if (template_only && input->variantT() == V_SgFunctionDeclaration) 
         { left_over = (left_over==0)? n : LIST(left_over,n); }
      else {
        std::string res;
        switch (input->variantT()) {
         case V_SgVariableDeclaration:
            if (force_output) {
               res = input->unparseToString(); break;
            }
         default: 
            res = input->unparseToCompleteString();
        }
     //std::cerr << "here:" << AstInterface::AstToString(input) << " : res" << res << "\n";
        out << res;
      }
  }
  else {
     if (roseUnparser == 0) {
        /* QY/2013: copied from the global unparseFile to use a different ostream and delegate*/
       bool UseAutoKeyword                = false;
       bool generateLineDirectives        = true;
       bool useOverloadedOperators        = false;
       bool num                           = false;
       bool _this                         = true;
       bool caststring                    = false;
       bool _debug                        = false;
       bool _class                        = false;
       bool _forced_transformation_format = false;
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
       roseUnparser = new Unparser(&out, "", roseOptions);
     }
     switch (input->variantT()) {
     case V_SgSourceFile: 
      {
       SgSourceFile* f = isSgSourceFile(input);
       filename=f->get_file_info()->get_filename();
       info.set_current_scope(f->get_globalScope());
       template_only = true;
       code_gen(out, n->get_children(), 0, 0, align); 
       template_only = false; 
       if (left_over != 0) {
           code_gen(out, left_over, 0, 0, align); 
           left_over = 0;
       }
       break;
      }
     case V_SgFunctionDeclaration:
      //std::cerr << "seeing V_SgFunctionDeclaration:" << AstInterface::AstToString(input) << "\n";
      if (template_only) { 
          //std::cerr << "into leftover \n";
          left_over = (left_over==0)? left_over : LIST(left_over,n); break; 
      }
     case V_SgTemplateInstantiationFunctionDecl:
      {
        //std::cerr << "seeing V_SgTemplateInstantiationFunctionDecl:" << AstInterface::AstToString(input) << "\n";
        SgFunctionDeclaration* d = isSgFunctionDeclaration(input);
        POETTuple* c = dynamic_cast<POETTuple*>(n->get_children());
        std::string curname = d->get_qualified_name().getString();
std::cerr << curname << "\n";
        if (curname.rfind("::") == 0) {
          std::cerr << "unparsing instantiation\n";
          roseUnparser->u_exprStmt->unparseAttachedPreprocessingInfo(d,info,PreprocessingInfo::before);
          POETCode_ext_delegate repl(n, out);
          roseUnparser->repl = &repl;
          out << "\n";
          info.set_SkipFunctionDefinition();
          roseUnparser->u_exprStmt->unparseFuncDeclStmt(d, info);
          info.unset_SkipFunctionDefinition();
          force_output=true;
          out << "\n";
          code_gen(out, CODE_ACC("StmtList",c->get_entry(2)), 0, 0, align+2);
          out << ";\n";
          force_output=false;
       }
        break;
      }
     case V_SgFunctionDefinition:
      {
        //std::cerr << "seeing V_SgFunctionDefinition:" << AstInterface::AstToString(input) << "\n";
        SgStatement* d = isSgStatement(input);
        POETCode_ext_delegate repl(n, out);
        roseUnparser->repl = &repl;
        roseUnparser->u_exprStmt->unparseStatement(d, info);
        assert(n->get_children() != n);
        out << "{"; code_gen(out, linebreak, 0, 0, align + 2); 
        code_gen(out, n->get_children(), 0, linebreak, align+2); 
        code_gen(out, linebreak, 0, 0, align); out << "}"; 
        break;
      } 
     case V_SgPragmaDeclaration:
      {
         out << "#pragma ";
         POETTuple* c = dynamic_cast<POETTuple*>(n->get_children());
         assert(c != 0);
         code_gen(out, c->get_entry(0)); 
         roseUnparser->cur << " "; roseUnparser->cur.insert_newline(1,align);
         code_gen(out, c->get_entry(1), 0, 0, align); 
         break;
      }
     case V_SgForStatement:
      {
         out << "for (" ;
         POETTuple* c = dynamic_cast<POETTuple*>(n->get_children());
         assert(c != 0);
         code_gen(out, c->get_entry(0)); 
         code_gen(out, c->get_entry(1)); out << ";";
         code_gen(out, c->get_entry(2)); out << ")";
         break;
      }
      /*Begin: Added by Qian to unparse modified $While$|$If$ 2014/07/29*/
     case V_SgWhileStmt:
      {
        POETTuple* c = dynamic_cast<POETTuple*>(n->get_children());
	assert(c != 0);
	out << "while (";
	code_gen(out, c->get_entry(0)); out << ")";
	out << " {"; code_gen(out, linebreak, 0, 0, align+2);
	code_gen(out, c->get_entry(1), 0, linebreak, align+2);
	code_gen(out, linebreak, 0, 0, align);out << "}";
	break;
      }
     case V_SgIfStmt:
      {
        POETTuple* c = dynamic_cast<POETTuple*>(n->get_children());
	assert(c != 0);
	out << "if (";
	code_gen(out, c->get_entry(0)); out << ") {"; 
	code_gen(out, linebreak, 0, 0, align+2);
	code_gen(out, c->get_entry(1), 0, linebreak, align+2); 
	code_gen(out, linebreak, 0, 0, align);out << "}";
	SgIfStmt *f = isSgIfStmt(input);/*whether $if$ has the false branch*/
	if (f != 0) {
	  SgStatement *fb = isSgStatement(f->get_false_body());
	  if (fb != 0) {
		  out << " else {";
		  code_gen(out, linebreak, 0, 0, align+2);
		  code_gen(out, c->get_entry(2), 0, linebreak, align+2);
		  code_gen(out, linebreak, 0, 0, align);out << "}";
	  }
	}
	break;
      } /*End: Added by Qian*/
     case V_SgSwitchStatement:
      {/*Begin: Added by Qian to unparse modified $switch$ 11/08/2014*/
        POETTuple* c = dynamic_cast<POETTuple*>(n->get_children());
        assert(c != 0);
        out << "switch (";
        code_gen(out, c->get_entry(0)); out << ") {";
        code_gen(out, linebreak, 0, 0, align+2);
        code_gen(out, c->get_entry(1), 0, linebreak, align+2);
	code_gen(out, linebreak, 0, 0, align);out << "}";
        break;
      }
     case V_SgCaseOptionStmt:
      {
	POETTuple* c = dynamic_cast<POETTuple*>(n->get_children());
        assert(c != 0);
        out << "case ";
        code_gen(out, c->get_entry(0)); out << ":";
        code_gen(out, linebreak, 0, 0, align+2);
        code_gen(out, c->get_entry(1), 0, linebreak, align+2);
	code_gen(out, linebreak, 0, 0, align);//out << "}";
        break;
      }/*End: Added by Qian for $switch$*/
     case V_SgExprStatement:
        code_gen(out, n->get_children(), 0, 0, align); 
        out << ";";
        break;
     case V_SgTemplateInstantiationMemberFunctionDecl:  break;
     default: 
std::cerr << "Unsupported unparsing for  : " << input->class_name() << input->unparseToString() << "\n";
//assert(0); 
     }
  }
}

POETCode* POETAstInterface::ReplaceChildren(POETCode_ext* ext, POETCode* new_children)
{ 
  SgNode *n = (SgNode*)ext->get_content();
  POETCode* res = new POETCode_ext(n, new_children);
  return res; 
}


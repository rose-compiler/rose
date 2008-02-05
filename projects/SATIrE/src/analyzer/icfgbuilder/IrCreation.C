// Author: Markus Schordan, 2007

#include <iostream>
#include "IrCreation.h"

//std::string Ir::convertToUniformVariableName(std::string name) {
//  // we remove leading '::' from variable names
//  
//}


// creates a source string representation of an AST fragment. This function creates
// a dummy node to allow the ROSE unparseToString function to always succeed
std::string Ir::fragmentToString(const SgNode* node) {

  std::string s;
  // Variable Symbols cannot be unparsed using unparseToString, instead get_name must be used
  if(const SgVariableSymbol* v=dynamic_cast<const SgVariableSymbol*>(node)) {
    std::string s=v->get_name();
  } else {

  // we report what we cannot unparse as fragment
  // we'll change that to throw an exception 
  if(!dynamic_cast<const SgStatement*>(node) && !dynamic_cast<const SgExpression*>(node)) {
    return "unparse FRAGMENT: not (statement or expression)";
  }
  if(dynamic_cast<const SgFunctionDeclaration*>(node)) {
    return "unparse FRAGMENT: function declaration";
  }
  if(dynamic_cast<const SgFunctionDefinition*>(node)) {
    return "unparse FRAGMENT: function definition";
  }
  
  SgUnparse_Info* unparseInfo = new SgUnparse_Info();
  unparseInfo->set_SkipComments();    // do not generate comments
  unparseInfo->set_SkipWhitespaces(); // do not generate whitespaces to format the code
  unparseInfo->set_SkipQualifiedNames(); // skip qualified names -> this would cause a call to the EDG otherwise

  /* create a temporary AST root with SgFile and SgGlobal to allow ROSE function unparseToString to trace back */
  SgFile* file = new SgFile();
  file->set_file_info(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgGlobal* glob = new SgGlobal(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  glob->set_parent(file);
  file->set_root(glob); // do we need this one?

  // store and restore original parent pointer of 'node'
  SgNode* origParent=node->get_parent();
  (const_cast<SgNode*>(node))->set_parent(glob);
  if(const SgExpression* e=isSgExpression(node)) {
    s=globalUnparseToString(node, unparseInfo);
  }
  s=globalUnparseToString(node, unparseInfo)+"FRAGMENT";
  (const_cast<SgNode*>(node))->set_parent(origParent);
  }
  // return string representing the unparsed subtree with 'node' as root node

  return s+"_unparsedFRAGMENT";

}

std::string Ir::unparseNode(SgNode* node) {
  return "";
  // MS: this function works now for all nodes
  std::cout << "@" << node << ":";
  if(dynamic_cast<IcfgStmt*>(node)) {
    std::cout << "Unparse: " << "ICFG-NODE:" << typeid(node).name() << ":";
    if(DeclareStmt* n=dynamic_cast<DeclareStmt*>(node)) {
      std::cout << "DeclareStmt:" << Ir::fragmentToString(n) << std::endl;
      return "dummy";
    }
    if(UndeclareStmt* n=dynamic_cast<UndeclareStmt*>(node)) {
      std::cout << "UndeclareStmt:" << Ir::fragmentToString(n) << std::endl;
      return "dummy";
    }
    if(FunctionEntry* n=dynamic_cast<FunctionEntry*>(node)) {
      std::cout << "FunctionEntry:" << Ir::fragmentToString(n) << std::endl;
      return "dummy";
    }
    if(FunctionExit* n=dynamic_cast<FunctionExit*>(node)) {
      std::cout << "FunctionExit:" << Ir::fragmentToString(n) << std::endl;
      return "dummy";
    }
    if(FunctionCall* n=dynamic_cast<FunctionCall*>(node)) {
      std::cout << "FunctionCall:" << Ir::fragmentToString(n) << std::endl;
      return "dummy";
    }
    std::cout << "xxx" << std::endl;
    return "";
  } else {
    if(SgConstructorInitializer* n=dynamic_cast<SgConstructorInitializer*>(node)) {
      std::cout << "SgConstructorInitializer: not unparsed." << std::endl;
      return "";
    }
    if(SgClassType* n=dynamic_cast<SgClassType*>(node)) {
      std::cout << "SgClassType: not unparsed. Known bug." << std::endl;
      return "";
    }
    std::cout << "Unparse: " << node->sage_class_name() << " :";
  }
  if(SgSymbol* sym=isSgSymbol(node)) {
    SgName name=sym->get_name();
    std::cout << std::string("name:") << name.getString() << ":";
  }
  std::cout << Ir::fragmentToString(node);
  std::cout << std::endl;
  return "dummy";
#if 0 
  /* We may also need to construct a SgFile beforehand*/
  SgFile* file = new SgFile();
  file->set_file_info(fi);
  file->set_root(glob);
#endif

  SgUnparse_Info* unparseInfo = new SgUnparse_Info();
  unparseInfo->unset_SkipComments();    // generate comments
  unparseInfo->unset_SkipWhitespaces(); // generate all whitespaces to format the code
  unparseInfo->set_SkipQualifiedNames(); // Adrian:  skip qualified names -> this would cause a call to the EDG otherwise

  /* We also need to construct a SgFile beforehand*/
  SgGlobal* glob = new SgGlobal(createFileInfo());
  SgFile* file = new SgFile();
  file->set_file_info(createFileInfo());
  file->set_root(glob);
  SgNode* tmpParent=node->get_parent();
  node->set_parent(file);
  std::string s=globalUnparseToString(glob, unparseInfo);
  node->set_parent(tmpParent);
  return s;
}

std::string Ir::unparseExpression(SgExpression* node) {
  SgUnparse_Info* unparseInfo = new SgUnparse_Info();
  unparseInfo->unset_SkipComments();    // generate comments
  unparseInfo->unset_SkipWhitespaces(); // generate all whitespaces to format the code
  unparseInfo->set_SkipQualifiedNames(); // Adrian:  skip qualified names -> this would cause a call to the EDG otherwise
  //Unparser backend;
  std::string s; //=backend.unparseExpression(node,*unparseInfo);
  return s;
}

Sg_File_Info* Ir::createFileInfo() {
  Sg_File_Info* fi = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  fi->set_classificationBitField(fi->get_classificationBitField() 
				 | Sg_File_Info::e_compiler_generated);
  // or: fi->setCompilerGenerated()
  return fi;
}

void Ir::configLocatedNode(SgLocatedNode* n, SgNode* s1, SgNode* s2) {
  configLocatedNode(n,s1);
  s2->set_parent(n);
}

void Ir::configLocatedNode(SgLocatedNode* n, SgNode* s1) {
  configLocatedNode(n);
  s1->set_parent(n);
}

void Ir::configLocatedNode(SgLocatedNode* n) {
  n->set_startOfConstruct(createFileInfo());
  n->set_endOfConstruct(createFileInfo());
  Sg_File_Info* fi = n->get_file_info();
  if(!fi) {
    std::cout << "Setting file_info in " << n->sage_class_name() << std::endl;
    n->set_file_info(createFileInfo());
  }
  unparseNode(n);
}
void Ir::configInitializedName(SgInitializedName* n) {
  // this here reveals an inconistency in the ROSE interface:
  // InitializedName allows to set a file_info but
  // SgSupport does not (but has a get_file_info()!)
  Sg_File_Info* fi = n->get_file_info();
  if(!fi) {
    n->set_file_info(createFileInfo());
  }
  n->set_startOfConstruct(createFileInfo());
  //n->set_endOfConstruct(createFileInfo());
  // GB (2008-01-25): faking parent scope
  Sg_File_Info* fi2 = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  SgGlobal *global = new SgGlobal(fi2);
  n->set_scope(global);
}

void Ir::configSupportNode(SgSupport* n, SgNode* s1) {
  s1->set_parent(n);
  configSupportNode(n);
}
void Ir::configSupportNode(SgSupport* n) {
  //n->set_startOfConstruct(createFileInfo());
  unparseNode(n);
}

void Ir::configSymbolNode(SgSymbol* n, SgNode* s1) {
  s1->set_parent(n);
  configSymbolNode(n);
}

void Ir::configSymbolNode(SgSymbol* n) {
  //n->set_startOfConstruct(createFileInfo());
  //n->set_endOfConstruct(createFileInfo());
  //Sg_File_Info* fi = n->get_file_info();
  //if(!fi) {
  //  n->set_file_info(createFileInfo());
  //}
  unparseNode(n);
}

void Ir::configTypeNode(SgType* n) {
  unparseNode(n);
}


SgPointerType* Ir::createPointerType(SgType* type) {
  SgPointerType* n=new SgPointerType(type);
  configTypeNode(n);
  return n;
}

SgBoolValExp* Ir::createBoolValExp(bool val) {
  SgBoolValExp* n=new SgBoolValExp(createFileInfo(),val);
  configLocatedNode(n);
  return n; 
}

SgExprStatement* Ir::createExprStatement(SgExpression* e) {
  SgExprStatement* n=new SgExprStatement(e);
  configLocatedNode(n);
  assert(e->get_parent()==n);
  return n; 
}

SgAssignOp* Ir::createAssignOp(SgExpression* lhs, SgExpression* rhs) {
  SgAssignOp* n=new SgAssignOp(createFileInfo(),lhs,rhs);
  configLocatedNode(n);
  assert(lhs->get_parent()==n && rhs->get_parent()==n);
  return n; 
}

SgArrowExp* Ir::createArrowExp(SgExpression* lhs, SgExpression* rhs) {
  SgArrowExp* n=new SgArrowExp(createFileInfo(),lhs,rhs);
  configLocatedNode(n,lhs,rhs);
  assert(lhs->get_parent()==n && rhs->get_parent()==n);
  return n; 
}

SgAddressOfOp* Ir::createAddressOfOp(SgExpression* e, SgType* type) {
  SgAddressOfOp* n=new SgAddressOfOp(e,type);
  configLocatedNode(n,e);

  // following block is taken from Cxx_Grammar.h : SgAddressOfOp::get_type()
  // we need to ensure that the scope is properly set otherwise this causes a failing assert
  if(SgVarRefExp* varRefExp = isSgVarRefExp(n->get_operand()))
        {
          ROSE_ASSERT(varRefExp->get_symbol() != NULL);
          ROSE_ASSERT(varRefExp->get_symbol()->get_declaration() != NULL);
          SgInitializedName* variable = varRefExp->get_symbol()->get_declaration();
          ROSE_ASSERT(variable != NULL);
          SgScopeStatement* scope = variable->get_scope();
	  if(!scope) {
	    //std::cout << "Fixing missing SgScopeStatement in SgVarRefExp operand of AddressOfOp" << std::endl;
	    variable->set_scope(new SgScopeStatement()); // dummy, we only need it in the ICFG for ROSE 0.8.10e+
	  }
        }
  assert(e->get_parent()==n);
  return n; 
}

SgVarRefExp* Ir::createVarRefExp(std::string name, SgType* type) {
  SgVariableSymbol* variableSymbol=createVariableSymbol(name,type);
  SgVarRefExp* n=createVarRefExp(variableSymbol);
  assert(variableSymbol->get_parent()==n);
  return n;
}

SgVarRefExp* Ir::createVarRefExp(SgVariableSymbol* variableSymbol) {
  SgVarRefExp* n=new SgVarRefExp(variableSymbol);
  configLocatedNode(n,variableSymbol);
  assert(variableSymbol->get_parent()==n);
  assert(n->get_parent()==0);
  return n;
}

SgVarRefExp* Ir::createVarRefExp(SgInitializedName* initializedName) {
  SgVariableSymbol* variableSymbol=createVariableSymbol(initializedName);
  SgVarRefExp* n=new SgVarRefExp(variableSymbol);
  configLocatedNode(n,variableSymbol);
  assert(variableSymbol->get_parent()==n);
  return n;
}

SgVariableSymbol* Ir::createVariableSymbol(SgInitializedName* initializedName) {
  configInitializedName(initializedName);
  SgVariableSymbol* n=new SgVariableSymbol(initializedName);
  configSymbolNode(n,initializedName);
  return n;
}

SgVariableSymbol* Ir::createVariableSymbol(std::string name,SgType* type) {
  SgInitializedName* initializedName=createInitializedName(name,type);
  SgVariableSymbol* variableSymbol=createVariableSymbol(initializedName);
  assert(initializedName->get_parent()==variableSymbol);
  return variableSymbol;
}

SgName Ir::createName(std::string name) {
  return SgName(name);
}

SgInitializedName* Ir::createInitializedName(std::string name,SgType* type) {
  SgName sgname=createName(name);
  SgInitializedName* n=new SgInitializedName(sgname,type);
  configInitializedName(n);
  //n->set_file_info(createFileInfo());
  //n->set_startOfConstruct(createFileInfo());
  //configSupportNode(n);
  //n->set_endOfConstruct(FILEINFO); does not exist
  sgname.set_parent(n);
  configTypeNode(type);
  return n;
}

SgNullStatement* Ir::createNullStatement() {
  SgNullStatement* n=new SgNullStatement();
  configLocatedNode(n);
  return n;
}

SgExprListExp* Ir::createExprListExp() {
  SgExprListExp* n=new SgExprListExp();
  configLocatedNode(n);
  return n;
}

SgConstructorInitializer*
Ir::createConstructorInitializer(SgMemberFunctionDeclaration * mfd,SgType* type) {
  SgExprListExp* eList;
  eList=createExprListExp();
    SgConstructorInitializer* n=new SgConstructorInitializer(createFileInfo(),
							   mfd,
							   eList,
							   type,
							   false,false,
							   false /* force parenthesis */, 
							   isSgClassType(type) == NULL ? true : false);
  n->set_args(eList);
  n->post_construction_initialization();
  configLocatedNode(n,eList,mfd);
  return n;
}

SgMemberFunctionDeclaration* 
Ir::createMemberFunctionDeclaration(std::string name) {
   SgMemberFunctionDeclaration* n = new SgMemberFunctionDeclaration(createFileInfo(), name);
   configLocatedNode(n);   
   return n;
}

SgIfStmt* 
Ir::createIfStmt(SgExprStatement* expStmt) {
  // then and else are not used (connections are represented by ICFG edges)
  SgIfStmt* n=new SgIfStmt(createFileInfo(),expStmt);
  configLocatedNode(n);
  expStmt->set_parent(n);
  return n;
}

SgWhileStmt* 
Ir::createWhileStmt(SgExprStatement* expStmt) {
  // body is not used (connection to body represented by ICFG edges)
  SgWhileStmt* n=new SgWhileStmt(createFileInfo(),expStmt,NULL);
  configLocatedNode(n);
  expStmt->set_parent(n);
  return n;
}

SgDoWhileStmt* 
Ir::createDoWhileStmt(SgExprStatement* expStmt) {
  // body is not used (connection to body represented by ICFG edges)
  SgDoWhileStmt* n=new SgDoWhileStmt(createFileInfo(),NULL,expStmt);
  configLocatedNode(n);
  expStmt->set_parent(n);
  return n;
}

SgSwitchStatement*
Ir::createSwitchStatement(SgExprStatement* expStmt) {
  // body is not used (connection is represented by ICFG edge)
  SgSwitchStatement* n=new SgSwitchStatement(createFileInfo(),expStmt,NULL);
  configLocatedNode(n);
  expStmt->set_parent(n);
  return n;
}


// ICFG specific nodes that inherit from ROSE SgStatement
// have unparseToString
CallStmt*
Ir::createCallStmt(KFG_NODE_TYPE node_type, const char *name, CallBlock *parent){
  CallStmt* n=new CallStmt(node_type,name,parent);
  configLocatedNode(n);
  return n;
}

FunctionEntry*
Ir::createFunctionEntry(KFG_NODE_TYPE type, const char *func, CallBlock *parent){
  FunctionEntry* n=new FunctionEntry(type,func,parent);
  configLocatedNode(n);
  return n;
}

DeclareStmt*
Ir::createDeclareStmt(SgVariableSymbol *v, SgType *t){
  DeclareStmt* n=new DeclareStmt(v,t);
  configLocatedNode(n);
  return n;
}

UndeclareStmt*
Ir::createUndeclareStmt(std::vector<SgVariableSymbol *> *v){
  UndeclareStmt* n=new UndeclareStmt(v);
  configLocatedNode(n);
  return n;
}

ExternalCall*
Ir::createExternalCall(SgExpression *function, std::vector<SgVariableSymbol *> *params, SgType *type_){
  ExternalCall* n=new ExternalCall(function, params, type_);
  configLocatedNode(n);
  return n;
}

ConstructorCall*
Ir::createConstructorCall(const char *name_, SgType *type_){
  ConstructorCall* n=new ConstructorCall(name_,type_);
  configLocatedNode(n);
  return n;
}

DestructorCall*
Ir::createDestructorCall(const char *name_, SgType *type_){
  DestructorCall* n=new DestructorCall(name_,type_);
  configLocatedNode(n);
  return n;
}

ArgumentAssignment*
Ir::createArgumentAssignment(SgVariableSymbol *l, SgExpression *r){
  ArgumentAssignment* n=new ArgumentAssignment(l,r);
  configLocatedNode(n);
  return n;
}

ArgumentAssignment*
Ir::createArgumentAssignment(SgExpression *l, SgExpression *r){
  ArgumentAssignment* n=new ArgumentAssignment(l,r);
  configLocatedNode(n);
  return n;
}

ReturnAssignment*
Ir::createReturnAssignment(SgVariableSymbol *l, SgVariableSymbol *r){
  ReturnAssignment* n=new ReturnAssignment(l,r);
  configLocatedNode(n);
  return n;
}

ParamAssignment*
Ir::createParamAssignment(SgVariableSymbol *l, SgVariableSymbol *r){
  ParamAssignment* n=new ParamAssignment(l,r);
  configLocatedNode(n);
  return n;
}

LogicalIf*
Ir::createLogicalIf(SgExpression *e){
  LogicalIf* n=new LogicalIf(e);
  configLocatedNode(n);
  return n;
}

IfJoin*
Ir::createIfJoin(){
  IfJoin* n=new IfJoin();
  configLocatedNode(n);
  return n;
}

WhileJoin*
Ir::createWhileJoin(){
  WhileJoin* n=new WhileJoin();
  configLocatedNode(n);
  return n;
}

FunctionExit*
Ir::createFunctionExit(KFG_NODE_TYPE type, const char *func, CallBlock *parent){
  FunctionExit* n=new FunctionExit(type,func,parent);
  configLocatedNode(n);
  return n;
}

SgClassType* 
Ir::createClassType() {
  SgClassType* n=new SgClassType();
  configTypeNode(n);
  return n;
}

char*
Ir::getCharPtr(SgName& n) {
  const char* constname=Ir::getConstCharPtr(n);
  // it depends whether the string remains in memeory or not whether we need to duplicate the string
  // however, we (should) free the data structures that take the string in the end anyways
  char* nonconstname=strdup(constname);
  return nonconstname;
}

const char*
Ir::getConstCharPtr(SgName& n) {
  // returning a const char* we assume that the string stays in memory as long as the const char* is used
  const char* constname=Ir::getString(n).c_str();
  return constname;
}

std::string
Ir::getString(SgName& n) {
  std::string s;
  s=n.getString();
  return s;
}

std::string IcfgStmt::unparseToString() {
  const char* name=typeid(this).name();
  std::cout << "UnparseToString: not implemented @";
  std::cout << name << std::endl;
  assert(false);
}

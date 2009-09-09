// Author: Markus Schordan, 2007,2008

#include <iostream>
#include <cstdlib>
#include "IrCreation.h"

// GB (2008-04-01): This is the definition of the global garbage bin.
Ir::GarbageBin Ir::garbageBin;

// creates a source string representation of an AST fragment. This function creates
// a dummy node to allow the ROSE unparseToString function to always succeed
std::string Ir::fragmentToString(const SgNode* node) {

  std::string s;
  // we report what we cannot unparse as fragment
  // we'll change that to throw an exception 
  if(dynamic_cast<const SgFunctionDeclaration*>(node)) {
    return "not unparsed: FRAGMENT function declaration";
  }
  if(dynamic_cast<const SgFunctionDefinition*>(node)) {
    return "not unparsed: FRAGMENT function definition";
  }
// GB (2008-03-03): This can't hurt.
  if (node == NULL) {
      return "not unparsed: NULL FRAGMENT";
  }

  SgUnparse_Info* unparseInfo = new SgUnparse_Info();
  unparseInfo->set_SkipComments();    // do not generate comments
  unparseInfo->set_SkipWhitespaces(); // do not generate whitespaces to format the code
  unparseInfo->set_SkipQualifiedNames(); // skip qualified names -> this would cause a call to the EDG-IR otherwise
  unparseInfo->set_SkipCPPDirectives(); // GB (2008-08-04): Viktor reported a case with a #define attached to a type name
  unparseInfo->set_SkipClassDefinition(); // GB (2008-08-04): this has bugged me for a long time

  // SgType does not allow to set the parent pointer (as we need to do), an ASSERT fails, but unparsing is ok
  if(const SgType* n=isSgType(node)) {
    if (isSgTypeBool(node)) {
     // GB (2008-03-11): Implemented the case for bool manually. Otherwise,
     // if the input is a C89 file, the unparser complains like this:
     // "Warning: SgTypeBool used for C application (reserved for use in C99
     // and C++)". This is because we use type bool for temporary logical
     // variables.
        delete unparseInfo;
        return "bool";
    }
    s=n->unparseToString(unparseInfo);
    delete unparseInfo;
    return s;
  }

  if (const SgLabelStatement *l = isSgLabelStatement(node)) {
   // GB (2008-03-03): Calling unparseToString on self-made
   // SgLabelStatements doesn't work well because of a bunch of sanity
   // checks in the unparser. Let's build the string semi-manually.
      s = l->get_name().str();
      s += ":";
   // Unparse the associated statement, if any.
      if (l->get_statement() != NULL) {
          s += ' ';
          s += fragmentToString(l->get_statement());
      }
      delete unparseInfo;
      return s;
  }

  if (const SgMemberFunctionRefExp *mf = isSgMemberFunctionRefExp(node)) {
   // GB (2008-04-07): ROSE is picky about unparsing member function
   // reference expressions; the parent must be an expression, and it must
   // have a SgExprStatement predecessor. Faking of scopes is therefore
   // hard. We take the easy route and simply return the member function's
   // name.
      s = mf->get_symbol_i()->get_name().str();
      delete unparseInfo;
      return s;
  }

  /* create a temporary AST root with SgFile and SgGlobal to allow ROSE function unparseToString to trace back */
  SgSourceFile* file = new SgSourceFile();
  file->set_file_info(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
// GB (2008-03-31): It looks like ROSE 0.9.1a does not free this SgGlobal's
// File_Info when the AST is destructed. This is not our problem (and it's a
// very minor memory leak).
  SgGlobal* glob = new SgGlobal(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  glob->set_parent(file);
  // GB (2008-10-20): The set_root call appears necessary for ROSE
  // 0.9.3a-1593, it's not necessary with ROSE 0.9.3a-2261.
  file->set_globalScope(glob); // do we need this one?

  // store and restore original parent pointer of 'node'
  SgNode* origParent=node->get_parent();
  (const_cast<SgNode*>(node))->set_parent(glob);

  // the method unparseToString is not virtual therefore we need to determine the
  // concrete type before invoking unparseToString (should be changed in ROSE)
  // there are two versions of unparseToString, in the nodes that inherite in SATIrE
  // we only implement unparseToString(), but not unparseToString(unparseInfo), therefore
  // the methods are called here as they are available on the respective node
  if(const SgExprStatement* n=isSgExprStatement(node)) {
    // unparseToString does NOT work on ExprStatement in ROSE, we must take the expression explicitly
    s=n->get_expression()->unparseToString(unparseInfo); 
  } else if(const SgExpression* n=isSgExpression(node)) {
    s=n->unparseToString(unparseInfo);
  } else if(const SgVariableSymbol* v=dynamic_cast<const SgVariableSymbol*>(node)) {
    s=v->get_name();
  } else if(const IcfgStmt* n=dynamic_cast<const IcfgStmt*>(node)) {
    // MS: I've made unparseToString virtual in SATIrE, therefor this works
    s=n->unparseToString();
  } else {
    s=node->unparseToString(unparseInfo);
 // GB (2008-11-14): Sometimes this unparsed string is empty, which might be
 // due to some bug in ROSE, or some assumption that ROSE makes that we do
 // not ensure. Anyway, it looks like usually this works again if we
 // restore the original parent, so let's try that.
    if (s == "") {
      (const_cast<SgNode*>(node))->set_parent(origParent);
      s = node->unparseToString(unparseInfo);
    }
  }

  // restore original parent pointer
  (const_cast<SgNode*>(node))->set_parent(origParent);
  
  delete unparseInfo;
  // delete temporary AST nodes
  delete glob;
  delete file;

  // return string representing the unparsed subtree with 'node' as root node
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
  // GB (2008-05-26): Can't always fake parent scope: If there is an
  // existing scope, we must use that. If we break existing scopes, symbol
  // table lookups break!
  if (n->get_scope() == NULL)
  {
      Sg_File_Info* fi2 = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
      SgGlobal *global = new SgGlobal(fi2);
      n->set_scope(global);
  }
}

void Ir::configSupportNode(SgSupport* n, SgNode* s1) {
  s1->set_parent(n);
  configSupportNode(n);
}
void Ir::configSupportNode(SgSupport* n) {
  // nothing to do
}

void Ir::configSymbolNode(SgSymbol* n, SgNode* s1) {
  s1->set_parent(n);
  configSymbolNode(n);
}

void Ir::configSymbolNode(SgSymbol* n) {
  // nothing to do
}

void Ir::configTypeNode(SgType* n) {
  // nothing to do
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

  // MS: the following block is taken from Cxx_Grammar.h : SgAddressOfOp::get_type()
  // we need to ensure that the scope is properly set otherwise this causes a failing assert
  if(SgVarRefExp* varRefExp = isSgVarRefExp(n->get_operand())) {
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

SgFunctionRefExp* Ir::createFunctionRefExp(SgFunctionSymbol *functionSymbol) {
    SgFunctionType *type = isSgFunctionType(functionSymbol->get_type());
    assert(type != NULL);
    SgFunctionRefExp* n = new SgFunctionRefExp(functionSymbol, type);
    configLocatedNode(n, functionSymbol);
    assert(functionSymbol->get_parent() == n);
    return n;
}

// GB (2008-05-09): This function should only allocate a new variable symbol
// if the initialized name doesn't already have one. Symbols are supposed to
// be shared!
SgVariableSymbol* Ir::createVariableSymbol(SgInitializedName* initializedName) {
  configInitializedName(initializedName);
  SgVariableSymbol *s
      = isSgVariableSymbol(initializedName->get_symbol_from_symbol_table());
  if (s == NULL)
  {
      s = new SgVariableSymbol(initializedName);
      configSymbolNode(s,initializedName);
  }
  return s;
}

// GB (2008-05-26): This function should only be called for variables
// introduced by our code (ones containing the $ symbol or named 'this').
// All the other variables in the ROSE AST already have symbols, so use
// those. Our data structures rely on correctly shared symbol nodes! The
// $/this rule is therefore enforced here.
// GB (2008-06-27): The name rule is now tightened, all temporary variables
// must have names beginning with "$tmpvar$".
SgVariableSymbol* Ir::createVariableSymbol(std::string name,SgType* type) {
//if (name.find('$') == std::string::npos && name != "this")
  if (name != "this" && (name.size() < 8 || name.substr(0, 8) != "$tmpvar$"))
  {
      std::cerr
          << __FILE__ << ":" << __LINE__ << ": "
          << "*** internal error in IR creation: "
          << "trying to create a temporary variable with invalid name"
          << " '" << name << "'"
          << std::endl;
      std::abort();
  }
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
#if 0
// GB (2008-04-07): Do not use this function, use one of the functions for
// creating one of the specific derived classes instead. UNLESS you
// implement a switch on the node_type, like in the constructor of
// CallBlock.
CallStmt*
Ir::createCallStmt(KFG_NODE_TYPE node_type, std::string name, CallBlock *parent){
  CallStmt* n=new CallStmt(node_type,name,parent);
  configLocatedNode(n);
  return n;
}
#endif

FunctionCall*
Ir::createFunctionCall(KFG_NODE_TYPE type, std::string func, CallBlock *parent){
  FunctionCall* n=new FunctionCall(type,func,parent);
  configLocatedNode(n);
  return n;
}

FunctionReturn*
Ir::createFunctionReturn(KFG_NODE_TYPE type, std::string func, CallBlock *parent){
  FunctionReturn* n=new FunctionReturn(type,func,parent);
  configLocatedNode(n);
  return n;
}

FunctionEntry*
Ir::createFunctionEntry(KFG_NODE_TYPE type, std::string func, CallBlock *parent){
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

ExternalReturn*
Ir::createExternalReturn(SgExpression *function, std::vector<SgVariableSymbol *> *params, SgType *type_){
  ExternalReturn* n=new ExternalReturn(function, params, type_);
  configLocatedNode(n);
  return n;
}

ConstructorCall*
Ir::createConstructorCall(std::string name_, SgType *type_){
  ConstructorCall* n=new ConstructorCall(name_,type_);
  configLocatedNode(n);
  return n;
}

DestructorCall*
Ir::createDestructorCall(std::string name_, SgType *type_){
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
Ir::createFunctionExit(KFG_NODE_TYPE type, std::string func, CallBlock *parent){
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

SgThisExp *
Ir::createThisExp(SgClassSymbol *class_symbol) {
  SgThisExp* t = new SgThisExp(createFileInfo(), class_symbol);
  configLocatedNode(t);
  return t;
}

SgIntVal *
Ir::createIntVal(int value) {
  SgIntVal *i = new SgIntVal(createFileInfo(), value);
  configLocatedNode(i);
  return i;
}

// GB (2008-03-05): This function creates a null pointer expression of the
// given type by creating a cast of the integer constant 0 to the given
// type. The type should be a pointer type, of course, but this is not
// enforced.
SgCastExp *
Ir::createNullPointerExp(SgType *type) {
  SgIntVal *zero = Ir::createIntVal(0);
  SgCastExp *c = new SgCastExp(createFileInfo(), zero, type,
                               SgCastExp::e_C_style_cast);
  configLocatedNode(c, zero);
  return c;
}

char*
Ir::getCharPtr(SgName& n) {
  const char* constname=Ir::getConstCharPtr(n);
  // it depends whether the string remains in memeory or not whether we need to duplicate the string
  // however, we (should) free the data structures that take the string in the end anyways
  char* nonconstname=strdup(constname);
  garbageBin.add_cString(nonconstname);
  return nonconstname;
}

char *
Ir::getCharPtr(const std::string &s) {
    const char *conststr = s.c_str();
    char *nonconststr = strdup(conststr);
    garbageBin.add_cString(nonconststr);
    return nonconststr;
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

std::string 
Ir::getStrippedName(SgInitializedName* in) {
  /* FIXME: check for global scope '::' and use non-qualified name for global scope
   *        find a better way to deal with qualified names */
  if ((in->get_scope() != NULL) && (in->get_scope()->get_qualified_name()!="::")) {
    return in->get_qualified_name().str();
  } else {
    /* don't print the global namespace (::) */
    return in->get_name().str();
  }
  std::string s;
  return s;
}

class FragmentToTree: public AstBottomUpProcessing<std::string> {
protected:
    std::string evaluateSynthesizedAttribute(SgNode *node,
                                             SynthesizedAttributesList syn) {
        std::stringstream result;
     // Compute the class name: This is usually just the node's dynamic
     // class name without the "Sg" prefix, but there are exceptions. For
     // SgScopeStatements, we must produce stuff like
     // "ScopeStatement(IfStmt(...))".
        std::string class_name = node->class_name();
        if (class_name.substr(0, 2) == "Sg")
            class_name.erase(0, 2);
        if (isSgScopeStatement(node))
            class_name = "ScopeStatement(" + class_name;
        result << class_name << "(";

     // Now do the children. In most cases, this is the list of synthesized
     // strings interleaved with ", "; but again, there are special cases,
     // in particular for stuff that is not traversed (like names).
        if (DeclareStmt *ds = isDeclareStmt(node))
            result << variableSymbolToTree(ds->get_var()) << ", "
                   << typeToTree(ds->get_type());

        else if (UndeclareStmt *us = isUndeclareStmt(node))
            result << variableSymbolListToTree(us->get_vars());

        else if (ExternalCall *ec = isExternalCall(node))
            result << syn[0] << ", " // function
                   << variableSymbolListToTree(ec->get_params()) << ", "
                   << typeToTree(ec->get_type());

        else if (ExternalReturn *er = isExternalReturn(node))
            result << syn[0] << ", " // function
                   << variableSymbolListToTree(er->get_params()) << ", "
                   << typeToTree(er->get_type());

        else if (ConstructorCall *cc = isConstructorCall(node))
            result << stringToTree(cc->get_name()) << ", "
                   << typeToTree(cc->get_type());

        else if (DestructorCall *dc = isDestructorCall(node))
            result << stringToTree(dc->get_name()) << ", "
                   << typeToTree(dc->get_type());

        else if (ReturnAssignment *ra = isReturnAssignment(node))
            result << variableSymbolToTree(ra->get_lhs()) << ", "
                   << variableSymbolToTree(ra->get_rhs());

        else if (ParamAssignment *pa = isParamAssignment(node))
            result << variableSymbolToTree(pa->get_lhs()) << ", "
                   << variableSymbolToTree(pa->get_rhs());

        else if (FunctionEntry *fe = isFunctionEntry(node))
            result << stringToTree(fe->get_funcname());

        else if (FunctionExit *fex = isFunctionExit(node))
            result << stringToTree(fex->get_funcname()) << ", "
                   << variableSymbolListToTree(fex->parent->get_params());

        else if (FunctionCall *fc = isFunctionCall(node))
            result << stringToTree(fc->get_funcname()) << ", "
                   << variableSymbolListToTree(fc->parent->get_params());

        else if (FunctionReturn *fr = isFunctionReturn(node))
            result << stringToTree(fr->get_funcname()) << ", "
                   << variableSymbolListToTree(fr->parent->get_params());

        else if (SgIfStmt *ifs = isSgIfStmt(node))
            result << syn[0];

        else if (SgWhileStmt *whiles = isSgWhileStmt(node))
            result << syn[0];

        else if (SgDoWhileStmt *dowhiles = isSgDoWhileStmt(node))
            result << syn[0];

        else if (SgSwitchStatement *sw = isSgSwitchStatement(node))
            result << syn[0];

        else if (SgVarRefExp *varref = isSgVarRefExp(node))
            result << stringToTree(varref->get_symbol()->get_name().str());

        else if (SgStringVal *stringval = isSgStringVal(node))
            result << '"' << stringval->get_value() << '"';

        else if (SgValueExp *value = isSgValueExp(node))
            result << Ir::fragmentToString(value);

        else {
         // ArgumentAssignment and LogicalIf handled by this default
         // mechanism. The children are simply interleaved with ", ".
         // We also abuse this general case for something special: Add
         // square brackets if this is an expression list.
            if (isSgExprListExp(node))
                result << "[";
            SynthesizedAttributesList::iterator s = syn.begin();
            SynthesizedAttributesList::iterator end = syn.end();
            if (s != end)
            {
                result << *s++;
                while (s != end)
                    result << ", " << *s++;
            }
            if (isSgExprListExp(node))
                result << "]";
        }

        result << ")";
        if (isSgScopeStatement(node))
            result << ")";

        return result.str();
    }

private:
 // Some helper functions.
    static std::string variableSymbolToTree(SgVariableSymbol *sym)
    {
        std::stringstream result;
        result << "VariableSymbol(\"" << sym->get_name().str() << "\")";
        return result.str();
    }

    static std::string typeToTree(SgType *type)
    {
     // Recursively compute the type's string representation. This is always
     // a tree, and the recursion terminates, because class members are not
     // contained in our representation, so no cyclic types are possible.
        std::stringstream result;
        if (SgArrayType *at = isSgArrayType(type))
            result << "ArrayType("
                   << typeToTree(at->get_base_type()) << ")";

        else if (SgFunctionType *ft = isSgFunctionType(type))
            result << "FunctionType("
                   << typeToTree(ft->get_return_type()) << ", "
                   << typeListToTree(ft->get_arguments()) << ")";

        else if (SgNamedType *nt = isSgNamedType(type))
            result << "NamedType("
                   << stringToTree(nt->get_name().str()) << ")";

        else if (SgPointerType *pt = isSgPointerType(type))
            result << "PointerType("
                   << typeToTree(pt->get_base_type()) << ")";

        else if (SgReferenceType *rt = isSgReferenceType(type))
            result << "ReferenceType("
                   << typeToTree(rt->get_base_type()) << ")";

        else
            result << "BasicType("
                   << '"' << Ir::fragmentToString(type) << '"' << ")";

        return result.str();
    }

    static std::string typeListToTree(std::vector<SgType *> &types)
    {
        std::stringstream result;
        result << "[";
        std::vector<SgType *>::iterator t = types.begin();
        std::vector<SgType *>::iterator end = types.end();
        if (t != end)
        {
            result << typeToTree(*t++);
            while (t != end)
                result << ", " << typeToTree(*t++);
        }
        result << "]";
        return result.str();
    }

    static std::string variableSymbolListToTree(
            std::vector<SgVariableSymbol *> *syms)
    {
        std::stringstream result;
        result << "[";
        std::vector<SgVariableSymbol *>::iterator s = syms->begin();
        std::vector<SgVariableSymbol *>::iterator end = syms->end();
        if (s != end)
        {
            result << variableSymbolToTree(*s++);
            while (s != end)
                result << ", " << variableSymbolToTree(*s++);
        }
        result << "]";
        return result.str();
    }

    static std::string stringToTree(std::string s)
    {
     // Simply put " around the string.
        return '"' + s + '"';
    }
};

std::string
Ir::fragmentToTreeRepresentation(SgNode *node) {
    FragmentToTree fragmentToTree;
    return fragmentToTree.traverse(node);
}


// GB (2008-03-13): Added this function to wrap deep copying of AST
// fragments including the parent pointer. ROSE doesn't seem to be copying
// the parent pointer. I'm not sure if it ever did copy the parent pointer,
// but in any case it has started to become annoying that it isn't copied.
// Plus, declaring all those instances of SgTreeCopy should become
// unnecessary now.
SgNode *
Ir::deepCopy(SgNode *n, bool copyParentPointer /* = true */) {
    SgTreeCopy treeCopy;
    SgNode *result = n->copy(treeCopy);
    if (copyParentPointer)
        result->set_parent(n->get_parent());
    return result;
}

void **
Ir::createNodeList(SgExprListExp *e)
{
    return createNodeList(e->get_expressions());
}

// Implementation of the garbage bin follows.
void
Ir::GarbageBin::add_cString(char *str)
{
    cStrings.push_back(str);
}

void **
Ir::GarbageBin::findNodeList(void *address)
{
    std::map<void *, void **>::iterator pos;
    pos = nodeLists.find(address);
    if (pos != nodeLists.end())
        return pos->second;
    else
        return NULL;
}

void
Ir::GarbageBin::addNodeList(void *address, void **array)
{
    nodeLists[address] = array;
}

void
Ir::GarbageBin::clear()
{
    std::vector<char *>::iterator s;
    for (s = cStrings.begin(); s != cStrings.end(); ++s)
        free(*s);
    cStrings.clear();

    std::map<void *, void **>::iterator m;
    for (m = nodeLists.begin(); m != nodeLists.end(); ++m)
        delete[] m->second;
    nodeLists.clear();
}

Ir::GarbageBin::~GarbageBin()
{
    clear();
}

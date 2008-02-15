// test cases are put into tests/roseTests/astInterfaceTests
// Last modified, by Liao, Jan 10, 2008

#include "sageBuilder.h"

using namespace std;
using namespace SageInterface;
//---------------------------------------------
// scope stack interfaces
//   hide actual implementation of the stack
//---------------------------------------------

// DQ (1/18/2008): Added declaration in source file with Liao.
std::list<SgScopeStatement*> SageBuilder::ScopeStack;


void SageBuilder::pushScopeStack (SgScopeStatement* stmt)
{
  ROSE_ASSERT(stmt);
  ScopeStack.push_back(stmt); 
}

void SageBuilder::popScopeStack()
{
// we want to warning users  double freeing happens
//  if (!ScopeStack.empty())
  ScopeStack.pop_back();
}

SgScopeStatement* SageBuilder::topScopeStack()
{
  return ScopeStack.back();
}

bool SageBuilder::emptyScopeStack()
{
  return ScopeStack.empty();
}

void SageBuilder::clearScopeStack()
{
  ScopeStack.clear();
}

// *******************************************************************************
// *******************************  Build Functions  *****************************
// *******************************************************************************

// Only used to build parameter arguments for function ??
// should be transparently generated for most variable declaration builder
// deferred symbol insertion, scope setting , etc
// do them when it is actually used with the  parameterList!!
SgInitializedName *
SageBuilder::buildInitializedName \
 ( const SgName & name, SgType* type, SgScopeStatement* scope)
// ( const SgName & name, SgType* type, SgScopeStatement* scope = NULL )
{
#if 1
  // If the scope was not specified, then get it from the scope stack.
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();
//     ROSE_ASSERT(scope != NULL);
#endif 
     //foo(int);  empty name is possible here!!
     //ROSE_ASSERT(name.is_null() == false);
     ROSE_ASSERT(type != NULL);

     SgInitializedName* initializedName = new SgInitializedName(name,type);
     ROSE_ASSERT(initializedName);
#if 0
    //TODO prototype parameter has no symbol associated!!
    //TODO  scope is also different: SgFunctionDefinition or scope of SgFunctionDeclaration
    SgVariableSymbol * symbol_1 = new SgVariableSymbol(initializedName);
    ROSE_ASSERT(symbol_1);
    scope->insert_symbol(name, symbol_1);
    initializedName->set_scope(scope);
#endif 
    setSourcePositionForTransformation(initializedName);
    return initializedName;
}

//-----------------------------------------------
// could have two declarations for a same variable
// extern int i;
//  int i;
SgVariableDeclaration* 
SageBuilder::buildVariableDeclaration \
 (const SgName & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
 //(const SgName & name, SgType* type, SgInitializer * varInit= NULL, SgScopeStatement* scope = NULL)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
     ROSE_ASSERT(scope != NULL);
     ROSE_ASSERT(name.is_null() == false);
     ROSE_ASSERT(type != NULL);

  SgVariableDeclaration * varDecl = new SgVariableDeclaration(name, type, varInit);
  ROSE_ASSERT(varDecl);

  varDecl->set_firstNondefiningDeclaration(varDecl);

  SgInitializedName *initName = varDecl->get_decl_item (name);
  ROSE_ASSERT(initName);
  initName->set_scope(scope);

  SgVariableSymbol* varSymbol = scope->lookup_variable_symbol(name);
  if (varSymbol==NULL)
  {
    varSymbol = new SgVariableSymbol(initName);
  ROSE_ASSERT(varSymbol);
  scope->insert_symbol(name, varSymbol);
  }
  else
  {
//   cout<<"sageBuilder.C:112 debug: found a previous one!!...."<<endl;
    SgInitializedName* prev_decl = varSymbol->get_declaration();
    ROSE_ASSERT(prev_decl);
    initName->set_prev_decl_item(prev_decl);
  }
  // optional?
  varDecl->set_parent(scope);

#if 0
  // SgVariableDefinition should be created internally
  SgVariableDefinition * variableDefinition = new SgVariableDefinition(initName,(SgInitializer*)NULL);
  initName->set_declptr(variableDefinition);
  variableDefinition->set_parent(initName);
#endif
  ROSE_ASSERT((initName->get_declptr())!=NULL);

#if 1
  //bug 119, SgVariableDefintion's File_info is needed for deep copy to work
  // AstQuery based setSourcePositionForTransformation() cannot access all child nodes
  // have to set SgVariableDefintion explicitly
  SgVariableDefinition* variableDefinition_original = isSgVariableDefinition(initName->get_declptr());
  setOneSourcePositionForTransformation(variableDefinition_original);
  ROSE_ASSERT((variableDefinition_original->get_startOfConstruct()) !=NULL);
  ROSE_ASSERT((variableDefinition_original->get_endOfConstruct())!=NULL);
#endif
  setSourcePositionForTransformation(varDecl);
  return varDecl;
}

//-----------------------------------------------
// Assertion `definingDeclaration != __null || firstNondefiningDeclaration != __null' 
SgFunctionParameterList * 
SageBuilder::buildFunctionParameterList()
{
  SgFunctionParameterList *parameterList = new SgFunctionParameterList();
  ROSE_ASSERT (parameterList);

  parameterList->set_definingDeclaration (NULL);
  parameterList->set_firstNondefiningDeclaration (parameterList);

  setOneSourcePositionForTransformation(parameterList);
  return parameterList;
}

//-----------------------------------------------
// no type vs. void type ?
SgFunctionParameterTypeList * 
SageBuilder::buildFunctionParameterTypeList(SgFunctionParameterList * paralist)
{
  if (paralist==NULL) 
  return NULL;
  if (paralist->get_args().size()==0)
    return NULL;
  SgFunctionParameterTypeList* typePtrList = new SgFunctionParameterTypeList;
  ROSE_ASSERT(typePtrList);

  SgInitializedNamePtrList args = paralist->get_args();
  SgInitializedNamePtrList::const_iterator i;
  for(i=args.begin(); i!=args.end();i++)
    (typePtrList->get_arguments()).push_back( (*i)->get_type() );
  setSourcePositionForTransformation(typePtrList);
  return typePtrList;
  
}


SgFunctionParameterTypeList *
SageBuilder::buildFunctionParameterTypeList (SgExprListExp * expList)
{
  if (expList ==NULL) return NULL;
  SgExpressionPtrList expPtrList = expList->get_expressions();

  SgFunctionParameterTypeList* typePtrList = new SgFunctionParameterTypeList;
  ROSE_ASSERT(typePtrList);

  SgExpressionPtrList::const_iterator i;
  for (i=expPtrList.begin();i!=expPtrList.end();i++)
  {
    (typePtrList->get_arguments()).push_back( (*i)->get_type() );
  }
  setSourcePositionForTransformation(typePtrList);
  return typePtrList;

}

//-----------------------------------------------
// build function type, 
// 
// insert into symbol table when not duplicated
SgFunctionType * 
SageBuilder::buildFunctionType(SgType* return_type, SgFunctionParameterTypeList * typeList)
{
  ROSE_ASSERT(return_type);
  SgFunctionType * funcType = new SgFunctionType(return_type, false);
  ROSE_ASSERT(funcType);

  if (typeList!=NULL)
  {
    funcType->set_argument_list(typeList);
    typeList->set_parent(funcType);
  }
  SgName typeName = funcType->get_mangled_type();
  // maintain the type table 
  SgFunctionTypeTable * fTable = SgNode::get_globalFunctionTypeTable();
  ROSE_ASSERT(fTable);

  SgType* typeInTable = fTable->lookup_function_type(typeName);
  if (typeInTable==NULL)
    fTable->insert_function_type(typeName,funcType);

  return funcType;
}

//----------------- function type------------
// same function declarations (defining or nondefining) should share the same function type!
SgFunctionType *
SageBuilder::buildFunctionType(SgType* return_type, SgFunctionParameterList * argList)
//SageBuilder::buildFunctionType(SgType* return_type, SgFunctionParameterList * argList=NULL)
{
  SgFunctionParameterTypeList * typeList = buildFunctionParameterTypeList(argList);
  SgFunctionType * func_type = buildFunctionType(return_type, typeList);
  return  func_type;
}

//----------------- function declaration------------
// considering
// 1. fresh building
// 2. secondary building after another nondefining functiondeclaration
// 3. secondary building after another defining function declaration
// 4. fortran ?
SgFunctionDeclaration *
SageBuilder::buildNonDefiningFunctionDeclaration \
(const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope)
{
  // argument verification
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  // ROSE_ASSERT(scope->containsOnlyDeclarations()); 
  // this function is also called when building a function reference before the function declaration exists.  So, skip the check
  ROSE_ASSERT(name.is_null() == false);
  ROSE_ASSERT(return_type != NULL);
  ROSE_ASSERT(paralist!= NULL);

  // tentatively build a function type, since it is shared
  // by all prototype and defining declarations of a same function!
  SgFunctionType * func_type = buildFunctionType(return_type,paralist);

  // function declaration
  SgFunctionDeclaration * func;

  // search before using the function type to create the function declaration 
  // TODO only search current scope or all ancestor scope??
  SgFunctionSymbol *func_symbol = scope->lookup_function_symbol(name,func_type);
  if (func_symbol ==NULL)
  {
   //first prototype declaration
    func = new SgFunctionDeclaration (name,func_type,NULL);
    ROSE_ASSERT(func);

    // function symbol table
    func_symbol= new SgFunctionSymbol(func);
    ROSE_ASSERT(func_symbol);
    scope->insert_symbol(name, func_symbol);
    ROSE_ASSERT(scope->lookup_function_symbol(name) != NULL);

   func->set_firstNondefiningDeclaration(func);
   func->set_definingDeclaration(NULL);
  }
   else 
  { 
   //2nd, or 3rd... prototype declaration
   //reuse function type, function symbol of previous declaration

   //std::cout<<"debug:SageBuilder.C: 267: "<<"found func_symbol!"<<std::endl;
//   delete (func_type-> get_argument_list ());
//   delete func_type; // bug 189
   
   func_type = func_symbol->get_declaration()->get_type();
   func = new SgFunctionDeclaration (name,func_type,NULL);
   ROSE_ASSERT(func);

   SgFunctionDeclaration* prevDecl = func_symbol->get_declaration();
   func->set_firstNondefiningDeclaration(prevDecl->get_firstNondefiningDeclaration());
   func->set_definingDeclaration(prevDecl->get_definingDeclaration());
   }

  // parameter list
  setParameterList(func, paralist);

  SgInitializedNamePtrList argList = paralist->get_args();
  Rose_STL_Container<SgInitializedName*>::iterator argi;
  for(argi=argList.begin(); argi!=argList.end(); argi++)
  {
   // std::cout<<"patching argument's scope.... "<<std::endl;
    (*argi)->set_scope(scope);
  }

  // mark as a forward declartion
  func->setForward();

  // TODO double check if there are exceptions
   func->set_scope(scope);
   func->set_parent(scope);

  // set File_Info as transformation generated
  setSourcePositionForTransformation(func);
  return func;  
}

//----------------- function declaration------------
// a template builder for all kinds of defining SgFunctionDeclaration
// handle common chores for function type, symbol, paramter etc.

template <class actualFunction>
SgFunctionDeclaration *
SageBuilder::buildDefiningFunctionDeclaration_T \
(const SgName & name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope)
//	(const SgName & name, SgType* return_type, SgScopeStatement* scope=NULL)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  ROSE_ASSERT(scope->containsOnlyDeclarations());
  ROSE_ASSERT(name.is_null() == false);
  ROSE_ASSERT(return_type != NULL);

  // build function type, manage funtion type symbol internally
  SgFunctionType * func_type = buildFunctionType(return_type,paralist);
  SgFunctionDeclaration * func;

 //  symbol table and nondefining 
  SgFunctionSymbol *func_symbol = scope->lookup_function_symbol(name,func_type);
  if (func_symbol ==NULL)
  {
    // new defining declaration
//    func = new SgFunctionDeclaration(name,func_type,NULL);
    func = new actualFunction(name,func_type,NULL);
    ROSE_ASSERT(func);
    SgFunctionSymbol *func_symbol = new SgFunctionSymbol(func);
    scope->insert_symbol(name, func_symbol);
    func->set_firstNondefiningDeclaration(NULL);
  } else
  {
    // defining declaration after nondefining declaration
    //reuse function type, function symbol

//    delete func_type;// bug 189
    func_type = func_symbol->get_declaration()->get_type();
    //func = new SgFunctionDeclaration(name,func_type,NULL);
    func = new actualFunction(name,func_type,NULL);
    ROSE_ASSERT(func);

    func->set_firstNondefiningDeclaration\
	(func_symbol->get_declaration()->get_firstNondefiningDeclaration());

    // fix up defining declarations before current statement
    func_symbol->get_declaration()->set_definingDeclaration(func);
    //for the rare case that two or more prototype declaration exist
    // cannot do anything until append/prepend_statment() is invoked
  }

  // definingDeclaration 
  func->set_definingDeclaration(func);

  // function body and definition are created before setting argument list 
  SgBasicBlock * func_body = new SgBasicBlock();
  ROSE_ASSERT(func_body);
  SgFunctionDefinition * func_def = new SgFunctionDefinition(func,func_body);
  ROSE_ASSERT(func_def);
  func_def->set_parent(func);
  func_def->set_body(func_body);
  func_body->set_parent(func_def);

   // parameter list,
   //TODO consider the difference between C++ and Fortran
  setParameterList(func,paralist);
         // fixup the scope and symbol of arguments,
  SgInitializedNamePtrList argList = paralist->get_args();
  Rose_STL_Container<SgInitializedName*>::iterator argi;
  for(argi=argList.begin(); argi!=argList.end(); argi++)
  {
//    std::cout<<"patching defining function argument's scope and symbol.... "<<std::endl;
    (*argi)->set_scope(func_def);
    func_def->insert_symbol((*argi)->get_name(), new SgVariableSymbol(*argi) );
  }

  func->set_parent(scope);
  func->set_scope(scope);
  
  // set File_Info as transformation generated
  setSourcePositionForTransformation(func);
  return func;
}

SgFunctionDeclaration *
SageBuilder::buildDefiningFunctionDeclaration \
(const SgName & name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope)
//	(const SgName & name, SgType* return_type, SgScopeStatement* scope=NULL)
{
  SgFunctionDeclaration * func= buildDefiningFunctionDeclaration_T<SgFunctionDeclaration> \
 (name,return_type,paralist,scope);

  return func;

}

SgDoubleVal* SageBuilder::buildDoubleVal(double t)
{
  SgDoubleVal* value = new SgDoubleVal(t,"");
  ROSE_ASSERT(value);
  setOneSourcePositionForTransformation(value);
  return value;
}

SgUnsignedLongVal* SageBuilder::buildUnsignedLongVal(unsigned long v)
{
  SgUnsignedLongVal* result = new SgUnsignedLongVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);

  return result;
}

SgArrayType* SageBuilder::buildArrayType(SgType* base_type, SgExpression* index)
//SgArrayType* buildArrayType(SgType* base_type=NULL, SgExpression* index=NULL)
{
  SgArrayType* result = new SgArrayType(base_type,index);
  ROSE_ASSERT(result); 
  if (index!=NULL) index->set_parent(result); // important!
  return result;
}

SgConditionalExp* SageBuilder::buildConditionalExp(SgExpression* test, SgExpression* a, SgExpression* b)
{
  SgConditionalExp* result = new SgConditionalExp(test, a, b, NULL);
  test->set_parent(result);
  a->set_parent(result);
  b->set_parent(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgAddOp * SageBuilder::buildAddOp(SgExpression* lhs, SgExpression* rhs)
{
   return buildBinaryExpression<SgAddOp>(lhs,rhs);
}

SgPntrArrRefExp* SageBuilder::buildPntrArrRefExp(SgExpression* lhs, SgExpression* rhs)
{
   return buildBinaryExpression<SgPntrArrRefExp>(lhs,rhs);
}

SgLessThanOp * SageBuilder::buildLessThanOp(SgExpression* lhs, SgExpression* rhs)
{
  return buildBinaryExpression<SgLessThanOp>(lhs,rhs);
}

SgEqualityOp * SageBuilder::buildEqualityOp(SgExpression* lhs, SgExpression* rhs)
{
  return buildBinaryExpression<SgEqualityOp>(lhs,rhs);
}

SgPlusAssignOp * SageBuilder::buildPlusAssignOp(SgExpression* lhs, SgExpression* rhs)
{
  return buildBinaryExpression<SgPlusAssignOp>(lhs,rhs);
}

SgGreaterThanOp* SageBuilder::buildGreaterThanOp(SgExpression* lhs, SgExpression* rhs)
{
  return buildBinaryExpression<SgGreaterThanOp>(lhs,rhs);
}
SgIntVal* SageBuilder::buildIntVal(int value)
{
  //TODO does valueString matter here?
  SgIntVal* intValue= new SgIntVal(value,"");
  ROSE_ASSERT(intValue);
  setOneSourcePositionForTransformation(intValue);
  return intValue;
}
SgBoolValExp* SageBuilder::buildBoolValExp(bool value)
{
  //TODO does valueString matter here?
  SgBoolValExp* boolValue= new SgBoolValExp(value);
  ROSE_ASSERT(boolValue);
  setOneSourcePositionForTransformation(boolValue);
  return boolValue;
}

SgNullExpression* SageBuilder::buildNullExpression()
{
  SgNullExpression* ne= new SgNullExpression();
  ROSE_ASSERT(ne);
  setOneSourcePositionForTransformation(ne);
  return ne;
}

template <class T>
T* SageBuilder::buildBinaryExpression(SgExpression* lhs, SgExpression* rhs)
{
  SgExpression* mylhs, *myrhs;
  mylhs = lhs;
  myrhs = rhs;

#if 0 // Jeremiah complaine this, sometimes users just move expressions around
 // it is very tempting to reuse expressions during translation,
  // so try to catch such a mistake here
  if (lhs!=NULL)
    if (lhs->get_parent()!=NULL)
    {
      cout<<"Warning! Found an illegal attempt to reuse lhs of type "
          << lhs->class_name() <<
        " when building a binary expression . lhs is being copied."<<endl;
     ROSE_ABORT();
      mylhs = isSgExpression(deepCopy(lhs));
    }

  if (rhs!=NULL)
    if (rhs->get_parent()!=NULL)
    {
      cout<<"Warning! Found an illegal attempt to reuse rhs of type "
          << rhs->class_name() <<
        " when building a binary expression . rhs is being copied."<<endl;
     ROSE_ABORT();
      myrhs = isSgExpression(deepCopy(rhs));
    }
#endif
  T* result = new T(mylhs,myrhs, NULL);
  ROSE_ASSERT(result);
  if (mylhs!=NULL) 
  {
   mylhs->set_parent(result);
  // set lvalue
    markLhsValues(result);
  }
  if (myrhs!=NULL) myrhs->set_parent(result);
  setOneSourcePositionForTransformation(result);
  return result;

}


template <class T>
T* SageBuilder::buildUnaryExpression(SgExpression* operand)
{ 
  SgExpression* myoperand=operand;
  
#if 0
 // it is very tempting to reuse expressions during translation,
  // so try to catch such a mistake here
  if (operand!=NULL)
    if (operand->get_parent()!=NULL)
    {
      cout<<"Warning! Found an illegal attempt to reuse operand of type "
          << operand->class_name() << 
        " when building a unary expression . Operand is being copied."<<endl;
     ROSE_ABORT();// remind user the issue
      myoperand = isSgExpression(deepCopy(operand));
    }
#endif
   T* result = new T(myoperand, NULL);
  ROSE_ASSERT(result);   
  if (myoperand!=NULL) 
  { 
    myoperand->set_parent(result);
  // set lvalue, it asserts operand!=NULL 
    markLhsValues(result);
  }
  setOneSourcePositionForTransformation(result);
  return result; 
}

SgAndOp* SageBuilder::buildAndOp(SgExpression* lhs, SgExpression* rhs)
{
  return buildBinaryExpression<SgAndOp>(lhs,rhs);
}

SgOrOp* SageBuilder::buildOrOp(SgExpression* lhs, SgExpression* rhs)
{
 return buildBinaryExpression<SgOrOp>(lhs,rhs);
}

SgBitAndOp* SageBuilder::buildBitAndOp(SgExpression* lhs, SgExpression* rhs)
{
  return buildBinaryExpression<SgBitAndOp>(lhs,rhs);
}

SgBitOrOp* SageBuilder::buildBitOrOp(SgExpression* lhs, SgExpression* rhs)
{
  return buildBinaryExpression<SgBitOrOp>(lhs,rhs);
}


SgAssignInitializer * SageBuilder::buildAssignInitializer(SgExpression * operand_i /*= NULL*/)
{
  // seems to work even SgAssignInitializer is not a unary expression in SAGE III AST, should double check it later on
  return buildUnaryExpression<SgAssignInitializer>(operand_i);
}

SgPointerDerefExp* SageBuilder::buildPointerDerefExp(SgExpression* operand)
{
  return buildUnaryExpression<SgPointerDerefExp> (operand);
}

SgMinusOp* SageBuilder::buildMinusOp(SgExpression* operand)
{
  return buildUnaryExpression<SgMinusOp> (operand);
}

SgNotOp* SageBuilder::buildNotOp(SgExpression* operand)
{
  return buildUnaryExpression<SgNotOp> (operand);

}

SgExprListExp * SageBuilder::buildExprListExp()
{
  SgExprListExp* expList = new SgExprListExp();
  ROSE_ASSERT(expList);
  setOneSourcePositionForTransformation(expList);
  return expList;
}

SgVarRefExp *
SageBuilder::buildVarRefExp(SgInitializedName* initname, SgScopeStatement* scope)
{
  ROSE_ASSERT(initname);
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL); 

  SgVarRefExp *varRef = NULL;
  SgSymbol* symbol = initname->get_symbol_from_symbol_table ();
  
  if (symbol)
  {
    varRef = new SgVarRefExp(isSgVariableSymbol(symbol));
    setOneSourcePositionForTransformation(varRef);
    ROSE_ASSERT(varRef);
  }
  else
   varRef = buildVarRefExp(initname->get_name(), scope);

 return varRef;
}

SgVarRefExp *
SageBuilder::buildVarRefExp(const char* varName, SgScopeStatement* scope) 
{
   SgName name(varName);   
   return buildVarRefExp(name,scope); 
} 

SgVarRefExp *
SageBuilder::buildVarRefExp(const std::string& varName, SgScopeStatement* scope)
//SageBuilder::buildVarRefExp(std::string& varName, SgScopeStatement* scope=NULL)
{
  SgName name(varName);
  return buildVarRefExp(name,scope);
}

SgVarRefExp *
SageBuilder::buildVarRefExp(const SgName& name, SgScopeStatement* scope)
//SageBuilder::buildVarRefExp(std::string& varName, SgScopeStatement* scope=NULL)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);

  SgSymbol * symbol = lookupSymbolInParentScopes(name,scope); 
  SgVariableSymbol* varSymbol=NULL;
 if (symbol) 
    varSymbol= isSgVariableSymbol(symbol); 
  else
// if not found: put fake init name and symbol here and 
//waiting for a postProcessing phase to clean it up
// two features: no scope and unknow type for initializedName
  {
    SgInitializedName * name1 = buildInitializedName(name,SgTypeUnknown::createType());
    name1->set_scope(scope); //buildInitializedName() does not set scope for various reasons
    varSymbol= new SgVariableSymbol(name1);
  }
  ROSE_ASSERT(varSymbol); 

  SgVarRefExp *varRef = new SgVarRefExp(varSymbol);
  setOneSourcePositionForTransformation(varRef);
  ROSE_ASSERT(varRef);
  return varRef; 
}

SgVarRefExp *
SageBuilder::buildVarRefExp(SgVariableSymbol* sym)
//SageBuilder::buildVarRefExp(std::string& varName, SgScopeStatement* scope=NULL)
{
  SgVarRefExp *varRef = new SgVarRefExp(sym);
  setOneSourcePositionForTransformation(varRef);
  ROSE_ASSERT(varRef);
  return varRef; 
}

SgFunctionParameterList*
SageBuilder::buildFunctionParameterList(SgFunctionParameterTypeList * paraTypeList)
{
  SgFunctionParameterList* paraList = buildFunctionParameterList();
  if (paraTypeList==NULL) return paraList;

  SgTypePtrList typeList = paraTypeList->get_arguments();
  SgTypePtrList::iterator i;
  for (i=typeList.begin();i!=typeList.end();i++)
  {
    SgInitializedName* arg = buildInitializedName(SgName(""),(*i));
    appendArg(paraList,arg);
  }

  return paraList;
}

// lookup function symbol to create a reference to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const SgName& name,const SgType* funcType, SgScopeStatement* scope)
//SageBuilder::buildFunctionRefExp(const SgName& name,SgScopeStatement* scope=NULL)
{
  ROSE_ASSERT(funcType); // function type cannot be NULL
  SgFunctionType* func_type = isSgFunctionType(const_cast<SgType*>(funcType));
  ROSE_ASSERT(func_type);

  if (scope == NULL)
     scope = SageBuilder::topScopeStack();
   ROSE_ASSERT(scope != NULL);
  SgFunctionSymbol* symbol = lookupFunctionSymbolInParentScopes(name,func_type,scope);
  if (symbol==NULL) 
// in rare cases when function calls are inserted before prototypes exist
  {
#if 0
// MiddleLevelRewrite::insert() does not merge content of headers into current AST
    symbol = lookupFunctionSymbolInParentScopes(name,scope); //TODO relax the matching
   if (symbol==NULL) {
// we require the declaration must exist before building a reference to it, or 
// at least user should insert the header containing the prototype information first
// using MiddleLevelRewrite::insert()
      cout<<"Error! building a reference to function: "<<name.getString()<<" before it is being declared before!"<<endl;
      ROSE_ASSERT(false);
    }
#else 
// MiddleLevelRewrite::insert() might conflict this part by generating a dangling function symbol 
    SgType* return_type = func_type->get_return_type();
    SgFunctionParameterTypeList * paraTypeList = func_type->get_argument_list();
    SgFunctionParameterList *parList = buildFunctionParameterList(paraTypeList);

    SgGlobal* globalscope = getGlobalScope(scope);
    SgFunctionDeclaration * funcDecl= buildNonDefiningFunctionDeclaration(name,return_type,parList,globalscope);
     funcDecl->get_declarationModifier().get_storageModifier().setExtern();

    // This will conflict with prototype in a header
   // prepend_statement(globalscope,funcDecl);
    symbol = lookupFunctionSymbolInParentScopes(name,func_type,scope);
    ROSE_ASSERT(symbol);
#endif
  }
  SgFunctionRefExp* func_ref = new SgFunctionRefExp(symbol,func_type);
  setOneSourcePositionForTransformation(func_ref);

  ROSE_ASSERT(func_ref);
  return func_ref;
}

// no actual usage of scope argument, but put it here for consistence
SgExprStatement*
SageBuilder::buildExprStatement(SgExpression*  exp)
//SageBuilder::buildExprStatement(SgExpression*  exp = NULL)
{
  SgExprStatement* expStmt = new SgExprStatement(exp);
  ROSE_ASSERT(expStmt);
  setOneSourcePositionForTransformation(expStmt);
  return expStmt;
}

SgFunctionCallExp* 
SageBuilder::buildFunctionCallExp(const SgName& name, 
                                                SgType* return_type, 
                                                SgExprListExp* parameters, 
                                             SgScopeStatement* scope)
//                                             SgScopeStatement* scope=NULL)
{
  if (scope == NULL)    
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL); 
  SgFunctionParameterTypeList * typeList= buildFunctionParameterTypeList(parameters); 
  SgFunctionType * func_type = buildFunctionType(return_type,typeList); 
  SgFunctionRefExp* func_ref = buildFunctionRefExp(name,func_type,scope);
  SgFunctionCallExp * func_call_expr = new SgFunctionCallExp(func_ref,parameters,func_ref->get_type());
  parameters->set_parent(func_call_expr);
  setOneSourcePositionForTransformation(func_call_expr);
  ROSE_ASSERT(func_call_expr);
  return func_call_expr;  
}

SgExprStatement*
SageBuilder::buildFunctionCallStmt(const SgName& name, 
                      SgType* return_type, 
                      SgExprListExp* parameters, 
                      SgScopeStatement* scope)
//                      SgScopeStatement* scope=NULL)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  SgFunctionCallExp* func_call_expr = buildFunctionCallExp(name,return_type,parameters,scope);
  SgExprStatement * expStmt = buildExprStatement(func_call_expr);
  return expStmt;
}


SgExprStatement*
SageBuilder::buildAssignStatement(SgExpression* lhs,SgExpression* rhs)
//SageBuilder::buildAssignStatement(SgExpression* lhs,SgExpression* rhs, SgScopeStatement* scope=NULL)
{
  ROSE_ASSERT(lhs != NULL); 
  ROSE_ASSERT(rhs != NULL); 
  
  //SgAssignOp* assignOp = new SgAssignOp(lhs,rhs,lhs->get_type());
// SgBinaryOp::get_type() assume p_expression_type is not set
  SgAssignOp* assignOp = new SgAssignOp(lhs,rhs,NULL);
  ROSE_ASSERT(assignOp);
  setOneSourcePositionForTransformation(assignOp);
  lhs->set_parent(assignOp);
  rhs->set_parent(assignOp);
  
  lhs->set_lvalue (true);
  SgExprStatement* exp = new SgExprStatement(assignOp);
  ROSE_ASSERT(exp);
   // some child nodes are transparently generated, using recursive setting is safer
  setSourcePositionForTransformation(exp);
  //setOneSourcePositionForTransformation(exp);
  assignOp->set_parent(exp);
  return exp;
}


SgLabelStatement * SageBuilder::buildLabelStatement(const SgName& name,  SgStatement * stmt/*=NULL*/, SgScopeStatement* scope /*=NULL*/)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
   // should including current scope when searching for the function definition
   // since users can only pass FunctionDefinition when the function body is not yet attached
  SgFunctionDefinition * label_scope = getEnclosingFunctionDefinition(scope,true);
  SgLabelStatement * labelstmt = new SgLabelStatement(name,stmt);
  ROSE_ASSERT(labelstmt);
  setOneSourcePositionForTransformation(labelstmt);
  
 if(stmt!=NULL) 
   stmt->set_parent(labelstmt);
  
  labelstmt->set_scope(label_scope);
  SgLabelSymbol* lsymbol= new SgLabelSymbol(labelstmt);
  ROSE_ASSERT(lsymbol); 
  // TODO should we prevent duplicated insertion ?
  label_scope->insert_symbol(lsymbol->get_name(), lsymbol);

  // we don't want to set parent here yet
  // delay it until append_statement() or alike
  return labelstmt;
}

SgAddressOfOp* 
SageBuilder::buildAddressOfOp (SgExpression* operand)
{
  return buildUnaryExpression<SgAddressOfOp>(operand);
}

SgIfStmt * SageBuilder::buildIfStmt(SgStatement* conditional, SgBasicBlock * true_body, SgBasicBlock * false_body)
{
  ROSE_ASSERT(conditional);
  ROSE_ASSERT(true_body);
  ROSE_ASSERT(false_body);

  SgIfStmt *ifstmt = new SgIfStmt(conditional, true_body, false_body);
  ROSE_ASSERT(ifstmt);
  setOneSourcePositionForTransformation(ifstmt);
  conditional->set_parent(ifstmt);
  true_body->set_parent(ifstmt);
  false_body->set_parent(ifstmt);
  return ifstmt;
}

SgWhileStmt * SageBuilder::buildWhileStmt(SgStatement *  condition, SgBasicBlock *body)
{
  ROSE_ASSERT(condition);
  ROSE_ASSERT(body);
  SgWhileStmt * result = new SgWhileStmt(condition,body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  condition->set_parent(result);
  body->set_parent(result);
  return result;
}

SgBreakStmt * SageBuilder::buildBreakStmt()
{
  SgBreakStmt* result = new SgBreakStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgContinueStmt * SageBuilder::buildContinueStmt()
{
  SgContinueStmt* result = new SgContinueStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgPragmaDeclaration * SageBuilder::buildPragmaDeclaration(const string& name, SgScopeStatement* scope)
{
  if (scope == NULL)
      scope = SageBuilder::topScopeStack();
  SgPragma* pragma = new SgPragma(name);
  ROSE_ASSERT(pragma);
  setOneSourcePositionForTransformation(pragma);

  SgPragmaDeclaration* result = new SgPragmaDeclaration(pragma);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  result->set_definingDeclaration (result);
  result->set_firstNondefiningDeclaration(result);
  pragma->set_parent(result);

  return result;
}

SgBasicBlock * SageBuilder::buildBasicBlock(SgStatement * stmt)
{
  SgBasicBlock* result = new SgBasicBlock(stmt);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  if (stmt) stmt->set_parent(result);
  return result;
}

SgGotoStatement * 
SageBuilder::buildGotoStatement(SgLabelStatement *  label,SgScopeStatement* scope)
//SgGotoStatement * buildGotoStatement(SgLabelStatement *  label=NULL,SgScopeStatement* scope=NULL);
{
  SgGotoStatement* result = new SgGotoStatement(label);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);

  return result;
}

SgCaseOptionStmt * SageBuilder::buildCaseOptionStmt( SgExpression * key,SgBasicBlock *body, SgScopeStatement* scope)
{
  SgCaseOptionStmt* result = new SgCaseOptionStmt(key,body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);

  return result;
}

SgDefaultOptionStmt * SageBuilder::buildDefaultOptionStmt( SgBasicBlock *body, SgScopeStatement* scope)
{
  SgDefaultOptionStmt* result = new SgDefaultOptionStmt(body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);

  return result;
}

SgSwitchStatement* SageBuilder::buildSwitchStatement(SgStatement *item_selector,SgBasicBlock *body)
{
  SgSwitchStatement* result = new SgSwitchStatement(item_selector,body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);

  return result;
}


SgCastExp * SageBuilder::buildCastExp(SgExpression *  operand_i,
                SgType * expression_type,
                SgCastExp::cast_type_enum cast_type)
{
  SgCastExp* result = new SgCastExp(operand_i, expression_type, cast_type);

  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
   
}

SgPointerType* SageBuilder::buildPointerType(SgType * base_type /*= NULL*/)
{
  SgPointerType* result= new SgPointerType(base_type);
  ROSE_ASSERT(result);
  return result;
}

SgReferenceType* SageBuilder::buildReferenceType(SgType * base_type /*= NULL*/)
{
  SgReferenceType* result= new SgReferenceType(base_type);
  ROSE_ASSERT(result);
  return result;
}

SgTypeBool * SageBuilder::buildBoolType() { 
  SgTypeBool * result =SgTypeBool::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeChar * SageBuilder::buildCharType() 
{ 
  SgTypeChar * result =SgTypeChar::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

#if 0 // did not work, build##itemType would be expanded correctly
#define BUILD_SGTYPE_DEF(item) \
  SgType##item * SageBuilder::build##itemType() { \
  SgType##item * result =SgType##item::createType(); \
  ROSE_ASSERT(result); \
  return result; \
  }  

  BUILD_SGTYPE_DEF(Bool)
  BUILD_SGTYPE_DEF(Char)
  BUILD_SGTYPE_DEF(Double)
  BUILD_SGTYPE_DEF(Float)
  BUILD_SGTYPE_DEF(Int)
  BUILD_SGTYPE_DEF(Long)
  BUILD_SGTYPE_DEF(LongDouble)
  BUILD_SGTYPE_DEF(LongLong)
  BUILD_SGTYPE_DEF(Short)
  BUILD_SGTYPE_DEF(Void)

  BUILD_SGTYPE_DEF(Wchar)
  BUILD_SGTYPE_DEF(SignedChar)
  BUILD_SGTYPE_DEF(SignedInt)
  BUILD_SGTYPE_DEF(SignedLong)
  BUILD_SGTYPE_DEF(SignedShort)
  BUILD_SGTYPE_DEF(UnsignedChar)
  BUILD_SGTYPE_DEF(UnsignedInt)
  BUILD_SGTYPE_DEF(UnsignedLong)
  BUILD_SGTYPE_DEF(UnsignedLongLong)
  BUILD_SGTYPE_DEF(UnsignedShort)
#undef BUILD_SGTYPE_DEF
#endif 
SgTypeLongLong * SageBuilder::buildLongLongType() 
{ 
  SgTypeLongLong * result =SgTypeLongLong::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeLongDouble * SageBuilder::buildLongDoubleType() 
{ 
  SgTypeLongDouble * result =SgTypeLongDouble::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeUnsignedLongLong * SageBuilder::buildUnsignedLongLongType() 
{ 
  SgTypeUnsignedLongLong * result =SgTypeUnsignedLongLong::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeUnsignedLong * SageBuilder::buildUnsignedLongType() 
{ 
  SgTypeUnsignedLong * result =SgTypeUnsignedLong::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeUnsignedInt * SageBuilder::buildUnsignedIntType() 
{ 
  SgTypeUnsignedInt * result =SgTypeUnsignedInt::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeSignedInt * SageBuilder::buildSignedIntType() 
{ 
  SgTypeSignedInt * result =SgTypeSignedInt::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeUnsignedChar * SageBuilder::buildUnsignedCharType() 
{ 
  SgTypeUnsignedChar * result =SgTypeUnsignedChar::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeSignedLong * SageBuilder::buildSignedLongType() 
{ 
  SgTypeSignedLong * result =SgTypeSignedLong::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeWchar * SageBuilder::buildWcharType() 
{ 
  SgTypeWchar * result =SgTypeWchar::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeSignedChar * SageBuilder::buildSignedCharType() 
{ 
  SgTypeSignedChar * result =SgTypeSignedChar::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeVoid * SageBuilder::buildVoidType() 
{ 
  SgTypeVoid * result =SgTypeVoid::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeShort * SageBuilder::buildShortType() 
{ 
  SgTypeShort * result =SgTypeShort::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeLong * SageBuilder::buildLongType() 
{ 
  SgTypeLong * result =SgTypeLong::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeInt * SageBuilder::buildIntType() 
{ 
  SgTypeInt * result =SgTypeInt::createType(); 
  ROSE_ASSERT(result); 
  return result;
}
SgTypeDouble * SageBuilder::buildDoubleType() 
{ 
  SgTypeDouble * result =SgTypeDouble::createType(); 
  ROSE_ASSERT(result); 
  return result;
}
SgTypeFloat * SageBuilder::buildFloatType() 
{ 
  SgTypeFloat * result =SgTypeFloat::createType(); 
  ROSE_ASSERT(result); 
  return result;
}








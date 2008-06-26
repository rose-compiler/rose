// test cases are put into tests/roseTests/astInterfaceTests
// Last modified, by Liao, Jan 10, 2008

#include "sageBuilder.h"
#include <fstream>
#include <boost/algorithm/string/trim.hpp>
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

void SageBuilder::pushScopeStack (SgNode* node)
{
  SgScopeStatement* stmt = isSgScopeStatement(node);
  pushScopeStack(stmt);
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
SageBuilder::buildInitializedName ( const SgName & name, SgType* type)
{
#if 0
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

SgInitializedName *
SageBuilder::buildInitializedName ( const std::string & name, SgType* type)
{
  SgName var_name(name);  
  return buildInitializedName(var_name,type);
}

SgInitializedName *
SageBuilder::buildInitializedName ( const char* name, SgType* type)
{
  SgName var_name(name);  
  return buildInitializedName(var_name,type);
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
//   ROSE_ASSERT(scope != NULL); // enable bottomup construction: scope can be unknown
   ROSE_ASSERT(name.is_null() == false);
   ROSE_ASSERT(type != NULL);

  SgVariableDeclaration * varDecl = new SgVariableDeclaration(name, type, varInit);
  ROSE_ASSERT(varDecl);

  varDecl->set_firstNondefiningDeclaration(varDecl);

  if (scope!=NULL) 
  {
    fixVariableDeclaration(varDecl,scope);
  }
#if 0
  // SgVariableDefinition should be created internally
  SgVariableDefinition * variableDefinition = new SgVariableDefinition(initName,(SgInitializer*)NULL);
  initName->set_declptr(variableDefinition);
  variableDefinition->set_parent(initName);
#endif
  SgInitializedName *initName = varDecl->get_decl_item (name);   
  ROSE_ASSERT(initName); 
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

SgVariableDeclaration*
SageBuilder::buildVariableDeclaration \
 (const std::string & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
{
  SgName name2(name);
  return buildVariableDeclaration(name2,type, varInit,scope);
}

SgVariableDeclaration*
SageBuilder::buildVariableDeclaration \
 (const char* name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
{
  SgName name2(name);
  return buildVariableDeclaration(name2,type, varInit,scope);
}
//-----------------------------------------------
// Assertion `definingDeclaration != __null || firstNondefiningDeclaration != __null' 
SgFunctionParameterList * 
SageBuilder::buildFunctionParameterList(SgInitializedName* in1, SgInitializedName* in2, SgInitializedName* in3, SgInitializedName* in4, SgInitializedName* in5, SgInitializedName* in6, SgInitializedName* in7, SgInitializedName* in8, SgInitializedName* in9, SgInitializedName* in10)
{
  SgFunctionParameterList *parameterList = new SgFunctionParameterList();
  ROSE_ASSERT (parameterList);

  parameterList->set_definingDeclaration (NULL);
  parameterList->set_firstNondefiningDeclaration (parameterList);

  setOneSourcePositionForTransformation(parameterList);

  if (in1) appendArg(parameterList, in1);
  if (in2) appendArg(parameterList, in2);
  if (in3) appendArg(parameterList, in3);
  if (in4) appendArg(parameterList, in4);
  if (in5) appendArg(parameterList, in5);
  if (in6) appendArg(parameterList, in6);
  if (in7) appendArg(parameterList, in7);
  if (in8) appendArg(parameterList, in8);
  if (in9) appendArg(parameterList, in9);
  if (in10) appendArg(parameterList, in10);

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
template <class actualFunction>
actualFunction*
SageBuilder::buildNondefiningFunctionDeclaration_T \
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
  // by all prototypes and defining declarations of a same function!
  SgFunctionType * func_type = buildFunctionType(return_type,paralist);

  // function declaration
  actualFunction * func;

  // search before using the function type to create the function declaration 
  // TODO only search current scope or all ancestor scope??
   //We don't have lookup_member_function_symbol  yet
  SgFunctionSymbol *func_symbol = scope->lookup_function_symbol(name,func_type);

  if (func_symbol ==NULL)
  {
   //first prototype declaration
    func = new actualFunction (name,func_type,NULL);
    ROSE_ASSERT(func);

    // function symbol table
   if (isSgMemberFunctionDeclaration(func))
     func_symbol= new SgMemberFunctionSymbol(func);
   else
     func_symbol= new SgFunctionSymbol(func);

    ROSE_ASSERT(func_symbol);
    scope->insert_symbol(name, func_symbol);
   //  ROSE_ASSERT(scope->lookup_function_symbol(name,func_type) != NULL);
    //ROSE_ASSERT(scope->lookup_function_symbol(name) != NULL);// Did not pass for member function?

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
   func = new actualFunction(name,func_type,NULL);
   ROSE_ASSERT(func);
   
   //we don't care if it is member function or function here for a pointer
   SgFunctionDeclaration* prevDecl = NULL;
   prevDecl=func_symbol->get_declaration();
   
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
  // TODO double check if there are exceptions
   func->set_scope(scope);
   func->set_parent(scope);

  // mark as a forward declartion
  func->setForward();

  // set File_Info as transformation generated
  setSourcePositionForTransformation(func);
  return func;  
}

SgFunctionDeclaration* SageBuilder::buildNondefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope)
{
  SgFunctionDeclaration * result = buildNondefiningFunctionDeclaration_T <SgFunctionDeclaration> (name,return_type,paralist,scope);
  return result;
}

SgMemberFunctionDeclaration* SageBuilder::buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope)
{
  SgMemberFunctionDeclaration * result = buildNondefiningFunctionDeclaration_T <SgMemberFunctionDeclaration> (name,return_type,paralist,scope);
  // set definingdecl for SgCtorInitializerList
  SgCtorInitializerList * ctor= result-> get_CtorInitializerList ();
  ROSE_ASSERT(ctor);
  //required ty AstConsistencyTests.C:TestAstForProperlySetDefiningAndNondefiningDeclarations()
  ctor->set_definingDeclaration(ctor);
  ctor->set_firstNondefiningDeclaration(ctor);
  return result;
}

SgMemberFunctionDeclaration* SageBuilder::buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope)
{
  SgMemberFunctionDeclaration * result = buildDefiningFunctionDeclaration_T <SgMemberFunctionDeclaration> (name,return_type,paralist,scope);
  // set definingdecl for SgCtorInitializerList
  SgCtorInitializerList * ctor= result-> get_CtorInitializerList ();
  ROSE_ASSERT(ctor);
  //required ty AstConsistencyTests.C:TestAstForProperlySetDefiningAndNondefiningDeclarations()
  ctor->set_definingDeclaration(ctor);
  ctor->set_firstNondefiningDeclaration(ctor);
  return result;
}


//----------------- defining function declaration------------
// a template builder for all kinds of defining SgFunctionDeclaration
// handle common chores for function type, symbol, paramter etc.

template <class actualFunction>
actualFunction *
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

  // build function type, manage function type symbol internally
  SgFunctionType * func_type = buildFunctionType(return_type,paralist);
  actualFunction * func;

 //  symbol table and non-defining 
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

SgFunctionDeclaration *
SageBuilder::buildDefiningFunctionDeclaration \
(const std::string & name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope)
{
  SgName sg_name(name);
  return buildDefiningFunctionDeclaration(sg_name,return_type, paralist,scope);
}

SgFunctionDeclaration *
SageBuilder::buildDefiningFunctionDeclaration \
(const char* name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope)
{
  SgName sg_name(name);
  return buildDefiningFunctionDeclaration(sg_name,return_type, paralist,scope);
}


//------------------build value expressions -------------------
//-------------------------------------------------------------
SgBoolValExp* SageBuilder::buildBoolValExpFi(int value, Sg_File_Info* start, Sg_File_Info* end)
{
  //TODO does valueString matter here?
  SgBoolValExp* boolValue= new SgBoolValExp(value);
  ROSE_ASSERT(boolValue);
  boolValue->set_startOfConstruct(start);
  boolValue->set_startOfConstruct(end);
  boolValue->set_operatorPosition(deepCopy(start));
  return boolValue;
}
SgBoolValExp* SageBuilder::buildBoolValExp(int value /*=0*/)
{
  //TODO does valueString matter here?
  SgBoolValExp* boolValue= new SgBoolValExp(value);
  ROSE_ASSERT(boolValue);
  setOneSourcePositionForTransformation(boolValue);
  return boolValue;
}
SgBoolValExp* SageBuilder::buildBoolValExp(bool value /*=0*/)
{
  return buildBoolValExp(int(value));
}

SgCharVal* SageBuilder::buildCharValFi(char value, const string& str, Sg_File_Info* start, Sg_File_Info* end)
{
  SgCharVal* result = new SgCharVal(value, "");
  ROSE_ASSERT(result);
  result->set_valueString(str);
  result->set_startOfConstruct(start);
  result->set_startOfConstruct(end);
  result->set_operatorPosition(deepCopy(start));
  return result;
}
SgCharVal* SageBuilder::buildCharVal(char value /*= 0*/)
{
  SgCharVal* result = new SgCharVal(value, "");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgComplexVal* SageBuilder::buildComplexVal(long double real_value /*= 0.0*/, 
                              long double imaginary_value /*= 0.0*/ )
{
  SgComplexVal* result = new SgComplexVal(real_value,imaginary_value,NULL,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgDoubleVal* SageBuilder::buildDoubleVal(double t)
{
  SgDoubleVal* value = new SgDoubleVal(t,"");
  ROSE_ASSERT(value);
  setOneSourcePositionForTransformation(value);
  return value;
}

SgFloatVal* SageBuilder::buildFloatVal(float value /*= 0.0*/)
{
  SgFloatVal* result = new SgFloatVal(value,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgIntVal* SageBuilder::buildIntVal(int value)
{
  SgIntVal* intValue= new SgIntVal(value,"");
  ROSE_ASSERT(intValue);
  setOneSourcePositionForTransformation(intValue);
  return intValue;
}

SgIntVal* SageBuilder::buildIntValHex(int value)
{
  SgIntVal* intValue= new SgIntVal(value, (value >= 0 ? StringUtility::intToHex((unsigned int)value) : "-" + StringUtility::intToHex((unsigned int)(-value))));
  ROSE_ASSERT(intValue);
  setOneSourcePositionForTransformation(intValue);
  return intValue;
}

SgLongDoubleVal* SageBuilder::buildLongDoubleVal(long double value /*= 0.0*/)
{
  SgLongDoubleVal* result = new SgLongDoubleVal(value,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgStringVal* SageBuilder::buildStringVal(std::string value /*=""*/)
{
  SgStringVal* result = new SgStringVal(value);
  ROSE_ASSERT(result);   
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedCharVal* SageBuilder::buildUnsignedCharVal(unsigned char v)
{
  SgUnsignedCharVal* result = new SgUnsignedCharVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedCharVal* SageBuilder::buildUnsignedCharValHex(unsigned char v)
{
  SgUnsignedCharVal* result = new SgUnsignedCharVal(v,StringUtility::intToHex(v));
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgShortVal* SageBuilder::buildShortVal(short v)
{
  SgShortVal* result = new SgShortVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgShortVal* SageBuilder::buildShortValHex(short v)
{
  SgShortVal* result = new SgShortVal(v, (v >= 0 ? StringUtility::intToHex((unsigned int)v) : "-" + StringUtility::intToHex((unsigned int)(-v))));
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedShortVal* SageBuilder::buildUnsignedShortVal(unsigned short v)
{
  SgUnsignedShortVal* result = new SgUnsignedShortVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedShortVal* SageBuilder::buildUnsignedShortValHex(unsigned short v)
{
  SgUnsignedShortVal* result = new SgUnsignedShortVal(v,StringUtility::intToHex(v));
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedIntVal* SageBuilder::buildUnsignedIntVal(unsigned int v)
{
  SgUnsignedIntVal* result = new SgUnsignedIntVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedIntVal* SageBuilder::buildUnsignedIntValHex(unsigned int v)
{
  SgUnsignedIntVal* result = new SgUnsignedIntVal(v,StringUtility::intToHex(v) + "U");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedLongVal* SageBuilder::buildUnsignedLongVal(unsigned long v)
{
  SgUnsignedLongVal* result = new SgUnsignedLongVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedLongVal* SageBuilder::buildUnsignedLongValHex(unsigned long v)
{
  SgUnsignedLongVal* result = new SgUnsignedLongVal(v,StringUtility::intToHex(v) + "UL");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedLongLongIntVal* SageBuilder::buildUnsignedLongLongIntVal(unsigned long long v)
{
  SgUnsignedLongLongIntVal* result = new SgUnsignedLongLongIntVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedLongLongIntVal* SageBuilder::buildUnsignedLongLongIntValHex(unsigned long long v)
{
  SgUnsignedLongLongIntVal* result = new SgUnsignedLongLongIntVal(v,StringUtility::intToHex(v) + "ULL");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build UPC THREADS (integer expression)
SgUpcThreads* SageBuilder::buildUpcThreads()
{
  SgUpcThreads* result = new SgUpcThreads(0,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build UPC  MYTHREAD (integer expression)
SgUpcMythread* SageBuilder::buildUpcMythread()
{
  SgUpcMythread* result = new SgUpcMythread(0,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//----------------------build unary expressions----------------------
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

template <class T>
T* SageBuilder::buildUnaryExpressionFi(SgExpression* operand, bool needParen, Sg_File_Info* start, Sg_File_Info* end, Sg_File_Info* opPos) {
  SgExpression* myoperand=operand;
  T* result = new T(myoperand, NULL);
  ROSE_ASSERT(result);   
  if (myoperand!=NULL) 
  { 
    myoperand->set_parent(result);
  // set lvalue, it asserts operand!=NULL 
    markLhsValues(result);
  }
  result->set_startOfConstruct(start);
  result->set_endOfConstruct(end);
  result->set_operatorPosition(opPos);
  result->set_need_paren(needParen);
  return result; 
}

#define BUILD_UNARY_DEF(suffix) \
  Sg##suffix* SageBuilder::build##suffix##Fi(SgExpression* op, bool needParen, Sg_File_Info* start, Sg_File_Info* end, Sg_File_Info* opPos) \
  { \
     return buildUnaryExpressionFi<Sg##suffix>(op, needParen, start, end, opPos); \
  } \
  Sg##suffix* SageBuilder::build##suffix(SgExpression* op) \
  { \
     return buildUnaryExpression<Sg##suffix>(op); \
  }

BUILD_UNARY_DEF(AddressOfOp)
BUILD_UNARY_DEF(BitComplementOp)
BUILD_UNARY_DEF(MinusOp)
BUILD_UNARY_DEF(NotOp)
BUILD_UNARY_DEF(PointerDerefExp)
BUILD_UNARY_DEF(UnaryAddOp)
BUILD_UNARY_DEF(MinusMinusOp)
BUILD_UNARY_DEF(PlusPlusOp)

#undef BUILD_UNARY_DEF

SgCastExp * SageBuilder::buildCastExp(SgExpression *  operand_i,
                SgType * expression_type,
                SgCastExp::cast_type_enum cast_type)
{
  SgCastExp* result = new SgCastExp(operand_i, expression_type, cast_type);

  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
   
}

SgMinusMinusOp *SageBuilder::buildMinusMinusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgMinusMinusOp* result = new SgMinusMinusOp(operand_i,a_mode);
  ROSE_ASSERT(result);
  if (operand_i!=NULL)
  {
    operand_i->set_parent(result);
   // set lvalue, it asserts operand!=NULL
   markLhsValues(result);
 }
  setOneSourcePositionForTransformation(result);
  return result;
}

SgPlusPlusOp* SageBuilder::buildPlusPlusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgPlusPlusOp* result = new SgPlusPlusOp(operand_i,a_mode);
  ROSE_ASSERT(result);
  if (operand_i!=NULL)
  {
    operand_i->set_parent(result);
   // set lvalue, it asserts operand!=NULL
   markLhsValues(result);
 }
  setOneSourcePositionForTransformation(result);   return result;
}

//---------------------binary expressions-----------------------

template <class T>
T* SageBuilder::buildBinaryExpression(SgExpression* lhs, SgExpression* rhs)
{
  SgExpression* mylhs, *myrhs;
  mylhs = lhs;
  myrhs = rhs;

#if 0 // Jeremiah complained this, sometimes users just move expressions around
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
T* SageBuilder::buildBinaryExpressionFi(SgExpression* lhs, SgExpression* rhs, bool needParen, Sg_File_Info* start, Sg_File_Info* end, Sg_File_Info* opPos)
{
  SgExpression* mylhs, *myrhs;
  mylhs = lhs;
  myrhs = rhs;
  T* result = new T(mylhs,myrhs, NULL);
  ROSE_ASSERT(result);
  if (mylhs!=NULL) 
  {
   mylhs->set_parent(result);
  // set lvalue
    markLhsValues(result);
  }
  if (myrhs!=NULL) myrhs->set_parent(result);
  result->set_startOfConstruct(start);
  result->set_endOfConstruct(end);
  result->set_operatorPosition(opPos);
  result->set_need_paren(needParen);
  return result;

}
#if 0
SgAddOp * SageBuilder::buildAddOp(SgExpression* lhs, SgExpression* rhs)
{
   return buildBinaryExpression<SgAddOp>(lhs,rhs);
}
#endif
#define BUILD_BINARY_DEF(suffix) \
  Sg##suffix* SageBuilder::build##suffix##Fi(SgExpression* lhs, SgExpression* rhs, bool needParen, Sg_File_Info* start, Sg_File_Info* end, Sg_File_Info* opPos) \
  { \
     return buildBinaryExpressionFi<Sg##suffix>(lhs, rhs, needParen, start, end, opPos); \
  } \
  Sg##suffix* SageBuilder::build##suffix(SgExpression* lhs, SgExpression* rhs) \
  { \
     return buildBinaryExpression<Sg##suffix>(lhs, rhs); \
  }

BUILD_BINARY_DEF(AddOp)
BUILD_BINARY_DEF(AndAssignOp)
BUILD_BINARY_DEF(AndOp)
BUILD_BINARY_DEF(ArrowExp)
BUILD_BINARY_DEF(ArrowStarOp)
BUILD_BINARY_DEF(AssignOp)
BUILD_BINARY_DEF(BitAndOp)
BUILD_BINARY_DEF(BitOrOp)
BUILD_BINARY_DEF(BitXorOp)

BUILD_BINARY_DEF(CommaOpExp)
BUILD_BINARY_DEF(ConcatenationOp)
BUILD_BINARY_DEF(DivAssignOp)
BUILD_BINARY_DEF(DotExp)
BUILD_BINARY_DEF(EqualityOp)

BUILD_BINARY_DEF(ExponentiationOp)
BUILD_BINARY_DEF(GreaterOrEqualOp)
BUILD_BINARY_DEF(GreaterThanOp)
BUILD_BINARY_DEF(IntegerDivideOp)
BUILD_BINARY_DEF(IorAssignOp)

BUILD_BINARY_DEF(LessOrEqualOp)
BUILD_BINARY_DEF(LessThanOp)
BUILD_BINARY_DEF(LshiftAssignOp)
BUILD_BINARY_DEF(LshiftOp)

BUILD_BINARY_DEF(MinusAssignOp)
BUILD_BINARY_DEF(ModAssignOp)
BUILD_BINARY_DEF(ModOp)
BUILD_BINARY_DEF(MultAssignOp)
BUILD_BINARY_DEF(MultiplyOp)

BUILD_BINARY_DEF(NotEqualOp)
BUILD_BINARY_DEF(OrOp)
BUILD_BINARY_DEF(PlusAssignOp)
BUILD_BINARY_DEF(PntrArrRefExp)
BUILD_BINARY_DEF(RshiftAssignOp)

BUILD_BINARY_DEF(RshiftOp)
BUILD_BINARY_DEF(ScopeOp)
BUILD_BINARY_DEF(SubtractOp)
BUILD_BINARY_DEF(XorAssignOp)

#undef BUILD_BINARY_DEF



SgArrayType* SageBuilder::buildArrayType(SgType* base_type/*=NULL*/, SgExpression* index/*=NULL*/)
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

SgNullExpression* SageBuilder::buildNullExpressionFi(Sg_File_Info* start, Sg_File_Info* end)
{
  SgNullExpression* ne= new SgNullExpression();
  ROSE_ASSERT(ne);
  setOneSourcePositionForTransformation(ne);
  ne->set_startOfConstruct(start);
  ne->set_endOfConstruct(end);
  ne->set_operatorPosition(deepCopy(start));
  return ne;
}

SgNullExpression* SageBuilder::buildNullExpression() {
  return buildNullExpressionFi(Sg_File_Info::generateDefaultFileInfoForTransformationNode(), Sg_File_Info::generateDefaultFileInfoForTransformationNode());
}

SgAssignInitializer * SageBuilder::buildAssignInitializer(SgExpression * operand_i /*= NULL*/)
{
  // seems to work even SgAssignInitializer is not a unary expression in SAGE III AST, should double check it later on
  return buildUnaryExpression<SgAssignInitializer>(operand_i);
}



SgExprListExp * SageBuilder::buildExprListExp(SgExpression * expr1, SgExpression* expr2, SgExpression* expr3, SgExpression* expr4, SgExpression* expr5, SgExpression* expr6, SgExpression* expr7, SgExpression* expr8, SgExpression* expr9, SgExpression* expr10)
{
  SgExprListExp* expList = new SgExprListExp();
  ROSE_ASSERT(expList);
  setOneSourcePositionForTransformation(expList);
  if (expr1) appendExpression(expList, expr1);
  if (expr2) appendExpression(expList, expr2);
  if (expr3) appendExpression(expList, expr3);
  if (expr4) appendExpression(expList, expr4);
  if (expr5) appendExpression(expList, expr5);
  if (expr6) appendExpression(expList, expr6);
  if (expr7) appendExpression(expList, expr7);
  if (expr8) appendExpression(expList, expr8);
  if (expr9) appendExpression(expList, expr9);
  if (expr10) appendExpression(expList, expr10);
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
    SgFunctionDeclaration * funcDecl= buildNondefiningFunctionDeclaration(name,return_type,parList,globalscope);
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

// lookup function symbol to create a reference to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(SgFunctionSymbol* sym)
{
  SgFunctionRefExp* func_ref = new SgFunctionRefExp(sym, NULL);
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

SgFunctionCallExp* 
SageBuilder::buildFunctionCallExp(SgFunctionSymbol* sym, 
                                  SgExprListExp* parameters)
{
  SgFunctionRefExp* func_ref = buildFunctionRefExp(sym);
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
  ROSE_ASSERT (label_scope);
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

SgIfStmt * SageBuilder::buildIfStmt(SgStatement* conditional, SgStatement * true_body, SgStatement * false_body)
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

SgWhileStmt * SageBuilder::buildWhileStmt(SgStatement *  condition, SgStatement *body)
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

SgBasicBlock * SageBuilder::buildBasicBlock(SgStatement * stmt1, SgStatement* stmt2, SgStatement* stmt3, SgStatement* stmt4, SgStatement* stmt5, SgStatement* stmt6, SgStatement* stmt7, SgStatement* stmt8, SgStatement* stmt9, SgStatement* stmt10)
{
  SgBasicBlock* result = new SgBasicBlock();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  if (stmt1) SageInterface::appendStatement(stmt1, result);
  if (stmt2) SageInterface::appendStatement(stmt2, result);
  if (stmt3) SageInterface::appendStatement(stmt3, result);
  if (stmt4) SageInterface::appendStatement(stmt4, result);
  if (stmt5) SageInterface::appendStatement(stmt5, result);
  if (stmt6) SageInterface::appendStatement(stmt6, result);
  if (stmt7) SageInterface::appendStatement(stmt7, result);
  if (stmt8) SageInterface::appendStatement(stmt8, result);
  if (stmt9) SageInterface::appendStatement(stmt9, result);
  if (stmt10) SageInterface::appendStatement(stmt10, result);
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

SgCaseOptionStmt * SageBuilder::buildCaseOptionStmt( SgExpression * key,SgStatement *body)
{
  SgCaseOptionStmt* result = new SgCaseOptionStmt(key,body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  key->set_parent(result);
  body->set_parent(result);

  return result;
}

SgDefaultOptionStmt * SageBuilder::buildDefaultOptionStmt( SgStatement *body)
{
  SgDefaultOptionStmt* result = new SgDefaultOptionStmt(body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  body->set_parent(result);

  return result;
}

SgSwitchStatement* SageBuilder::buildSwitchStatement(SgStatement *item_selector,SgBasicBlock *body)
{
  SgSwitchStatement* result = new SgSwitchStatement(item_selector,body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  item_selector->set_parent(result);
  body->set_parent(result);

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
SgTypeString * SageBuilder::buildStringType() 
{ 
  SgTypeString * result =SgTypeString::createType(); 
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
// It is easy to forget the namespace qualifier, so use this lazy method
//----------------------------------------------------------------------
namespace SageBuilder{


  //! Build a constant type.
  SgType* buildConstType(SgType* base_type)
 {
   ROSE_ASSERT(base_type!=NULL);

   SgModifierType *result = new SgModifierType(base_type);
   ROSE_ASSERT(result!=NULL);

   result->get_typeModifier().get_constVolatileModifier().setConst();
   return result;
 }

  //! Build a volatile type.
  SgType* buildVolatileType(SgType* base_type)
 {
   ROSE_ASSERT(base_type!=NULL);

   SgModifierType *result = new SgModifierType(base_type);
   ROSE_ASSERT(result!=NULL);

   result->get_typeModifier().get_constVolatileModifier().setVolatile();
   return result;

 }
  //! Build a restrict type.
  SgType* buildRestrictType(SgType* base_type)
 {
   ROSE_ASSERT(base_type!=NULL);
   bool isPointer = isSgPointerType(base_type);
   if (!isPointer)
   {  
     printf("Base type of restrict type must be a pointer type.\n");
     ROSE_ASSERT(false);
   }
   SgModifierType *result = new SgModifierType(base_type);
   ROSE_ASSERT(result!=NULL);

   result->get_typeModifier().setRestrict();
   return result;

 }

  SgClassDefinition* buildClassDefinition(SgClassDeclaration *d/*= NULL*/)
  {
    SgClassDefinition* result = NULL;
    if (d!=NULL) // the constructor does not check for NULL d, causing segmentation fault
    {
      result = new SgClassDefinition(d);
     // result->set_parent(d); // set_declaration() == set_parent() in this case
    }
    else 
      result = new SgClassDefinition();
    
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  SgClassDeclaration * buildStructDeclaration(const SgName& name, SgScopeStatement* scope /*=NULL*/)
  {
    if (scope == NULL)
      scope = SageBuilder::topScopeStack();
     //TODO How about class type??
    // build defining declaration
    SgClassDefinition* classDef = buildClassDefinition();
   
    SgClassDeclaration* defdecl = new SgClassDeclaration 
           (name,SgClassDeclaration::e_struct,NULL,classDef);
    ROSE_ASSERT(defdecl);
    setOneSourcePositionForTransformation(defdecl);
    // constructor is side-effect free
    classDef->set_declaration(defdecl);
    defdecl->set_definingDeclaration(defdecl);

    // build the nondefining declaration
    SgClassDeclaration* nondefdecl = new SgClassDeclaration
           (name,SgClassDeclaration::e_struct,NULL,NULL);
    ROSE_ASSERT(nondefdecl);
    setOneSourcePositionForTransformation(nondefdecl);
    nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
    nondefdecl->set_definingDeclaration(defdecl);
    defdecl->set_firstNondefiningDeclaration(nondefdecl);
    nondefdecl->setForward();    

    if (scope !=NULL )  // put into fixStructDeclaration() or alike later on
    {
      fixStructDeclaration(nondefdecl,scope);
#if 0
      SgClassSymbol* mysymbol = new SgClassSymbol(nondefdecl);
      ROSE_ASSERT(mysymbol);
      scope->insert_symbol(name, mysymbol);
      defdecl->set_scope(scope);
      nondefdecl->set_scope(scope);
      defdecl->set_parent(scope);
      nondefdecl->set_parent(scope);
#endif
    }
    return defdecl;    
  }

  SgClassDeclaration * buildStructDeclaration(const string& name, SgScopeStatement* scope/*=NULL*/)
  {
    SgName myname(name);
    return buildStructDeclaration(myname, scope);
  }

  SgClassDeclaration * buildStructDeclaration(const char* name, SgScopeStatement* scope/*=NULL*/)
  {
    SgName myname(name);
    return buildStructDeclaration(myname, scope);
  }

  //! Build a SgFile node
  SgFile* buildFile(const std::string& inputFileName, const std::string& outputFileName, SgProject* project/*=NULL*/)
  {
     ROSE_ASSERT(inputFileName.size()!=0);// empty file name is not allowed.
     string sourceFilename = inputFileName, fullname;
     Rose_STL_Container<std::string> arglist;
    int nextErrorCode = 0;

     if (project==NULL)
      // SgProject is created on the fly
      // Make up an arglist in order to reuse the code inside SgFile::setupSourceFilename()
      { 
        project = new SgProject();
        ROSE_ASSERT(project);
        SgFilePtrListPtr listp = new SgFilePtrList();
        ROSE_ASSERT(listp);
        project->set_fileList(listp);

        arglist.push_back("cc"); 
        arglist.push_back("-c"); 
        project->set_originalCommandLineArgumentList (arglist);
      } 

     ifstream testfile(inputFileName.c_str());
     if (!testfile.is_open()) 
    {
      // create a temporary file if the file does not exist.
      // have to do this, otherwise StringUtility::getAbsolutePathFromRelativePath() complains
      // which is called by result->setupSourceFilename(arglist);
       testfile.close();
       ofstream outputfile(inputFileName.c_str(),ios::out); 
       //outputfile<<"// test file "<<endl;
       outputfile.close();
     }
     else // file already exists , load and parse it
     { // should not reparse all files in case their ASTs have unsaved changes, 
      // just parse the newly loaded file only.
      // use argv here, change non-existing input file later on
      // TODO add error code handling 
     }  
      testfile.close();
      arglist = project->get_originalCommandLineArgumentList ();
      arglist.push_back(sourceFilename);
      arglist.push_back("-rose:o");
      arglist.push_back(outputFileName);

      Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(arglist);
      CommandlineProcessing::removeAllFileNamesExcept(arglist,fileList,sourceFilename);
      SgFile* result = new SgFile (arglist, nextErrorCode, 0, project);
      ROSE_ASSERT(result!=NULL);
      result->set_parent(project);
      project->set_file(*result); 
      project->set_frontendErrorCode(max(project->get_frontendErrorCode(), nextErrorCode));
      // Not sure why a warning shows up from astPostProcessing.C
      // SgNode::get_globalMangledNameMap().size() != 0 size = %zu (clearing mangled name cache)
      if (result->get_globalMangledNameMap().size() != 0) 
        result->clearGlobalMangledNameMap();
      return result;
#if 0      
      // manually generate SgFile, SgGlobal, etc, Works but not elegant
      SgFile * result = new SgFile(); 
      ROSE_ASSERT(result);
      // build SgGlobal since it is not built in the default constructor
      SgGlobal* global = new SgGlobal(); //remember set SgFileInfo later
      ROSE_ASSERT(global);
      result->set_root(global);
      global->set_parent(result);

      result->set_parent(project);
      //SgFilePtrListPtr filelist = project->get_fileList();
      //filelist->push_back(result);
      project->set_file(*result); // the same as the two stmts above

      arglist = project->get_originalCommandLineArgumentList ();
//     cout<<"1. size="<<arglist.size()<<":"<< StringUtility::listToString(arglist,true)<<endl; 
      arglist.push_back(sourceFilename);
      Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(arglist);
      CommandlineProcessing::removeAllFileNamesExcept(arglist,fileList,sourceFilename);
   //cout<<"4. size="<<arglist.size()<<":"<< StringUtility::listToString(arglist,true)<<endl; 

      //arglist.push_back("-rose:o");
     // arglist.push_back(sourceFilename);
      //set the output file of unparser explicitly, somehow the patched -rose:o has no effect.
      result->set_unparse_output_filename(outputFileName);
      //result->set_unparse_output_filename(sourceFilename);

//    cout<<"sageBuilder:1438. size="<<arglist.size()<<":"<< StringUtility::listToString(arglist,true)<<endl; 
      // each SgFile has to have a arglist, otherwise the unparser will complain.
      result->set_originalCommandLineArgumentList(arglist);
      result->setupSourceFilename(arglist);
      fullname=result->get_sourceFileNameWithPath();
      ROSE_ASSERT(fullname.size()!=0);
//     cout<<"debug:sageBuilder.C:1432 full name is:"<<fullname<<endl;

      //set filename in the SgGlobal node
     setOneSourcePositionForTransformation(global);
     ROSE_ASSERT(global->get_startOfConstruct() != NULL);
     ROSE_ASSERT(global->get_endOfConstruct() == NULL);
     global->get_startOfConstruct()->set_filenameString(fullname);

     setOneSourcePositionForTransformation(result);
     result->get_file_info()->set_filenameString(fullname);

    //  SgFunctionTypeTable ?
     SgFunctionTypeTable * typetable = SgNode::get_globalFunctionTypeTable();   
     typetable->set_parent(result);
    return result;
#endif
  }// end SgFile* buildFile()

PreprocessingInfo* buildComment(SgLocatedNode* target, const std::string & content,
               PreprocessingInfo::RelativePositionType position/*=PreprocessingInfo::before*/,
   PreprocessingInfo::DirectiveType dtype
     /* = PreprocessingInfo::CpreprocessorUnknownDeclaration*/)
{
  return SageInterface::attachComment(target,content, position, dtype);  
}

//! #define xxx yyy 
  PreprocessingInfo* buildCpreprocessorDefineDeclaration(SgLocatedNode* target, 
                const std::string & content,
               PreprocessingInfo::RelativePositionType position /* =PreprocessingInfo::before*/)
  {
    ROSE_ASSERT(target != NULL); //dangling #define xxx is not allowed in the ROSE AST
    // simple input verification
    std::string content2 = content;  
    boost::algorithm::trim(content2);
    string prefix = "#define";
    string::size_type pos = content2.find(prefix, 0);
    ROSE_ASSERT (pos == 0);

    PreprocessingInfo* result = NULL;

    PreprocessingInfo::DirectiveType mytype = PreprocessingInfo::CpreprocessorDefineDeclaration;
    

    result = new PreprocessingInfo (mytype,content, "transformation-generated", 0, 0, 0,
                               position, false, true);
    ROSE_ASSERT(result);
    target->addToAttachedPreprocessingInfo(result);
    return result;
  
  }

} // end of namespace 

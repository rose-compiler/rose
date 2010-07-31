// tps (01/14/2010) : Switching from rose.h to sage3
// test cases are put into tests/roseTests/astInterfaceTests
// Last modified, by Liao, Jan 10, 2008
#include "sage3basic.h"
#include "roseAdapter.h"
#include "markLhsValues.h"
#include "sageBuilder.h"
#include <fstream>
#include <boost/algorithm/string/trim.hpp>
#include "Outliner.hh"
using namespace std;
using namespace SageInterface;
//---------------------------------------------
// scope stack interfaces
//   hide actual implementation of the stack
//---------------------------------------------

// DQ (1/18/2008): Added declaration in source file with Liao.
// std::list<SgScopeStatement*> SageBuilder::ScopeStack;
std::list<SgScopeStatement*> SageBuilder::ScopeStack(0);


void SageBuilder::pushScopeStack (SgScopeStatement* stmt)
{
  ROSE_ASSERT(stmt);

// DQ (9/28/2009): This is part of testing for GNU 4.0.x (other versions of g++ work fine).
  ROSE_ASSERT(stmt != NULL);
  if (stmt != NULL)
     {
    // Calling any member function is a way to test the pointer.
       stmt->class_name();
     }

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
// DQ (9/28/2009): Test if this is an empty stack, and if so return NULL (ScopeStack.back() should be undefined for this case).
   if (ScopeStack.empty() == true)
      return NULL;

// DQ (9/28/2009): This is part of testing for GNU 4.0.x (other versions of g++ work fine).
  SgScopeStatement* tempScope = ScopeStack.back();
  if (tempScope != NULL)
     {
       tempScope->class_name();
     }

//return ScopeStack.back();
  return tempScope;
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
SageBuilder::buildInitializedName ( const SgName & name, SgType* type, SgInitializer* init /* = NULL */)
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

     SgInitializedName* initializedName = new SgInitializedName(name,type,init);
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

SgInitializedName *
SageBuilder::buildInitializedName_nfi ( const SgName & name, SgType* type, SgInitializer* init)
{
  ROSE_ASSERT(type != NULL);
  SgInitializedName* initializedName = new SgInitializedName(name,type,init);
  ROSE_ASSERT(initializedName);
  setOneSourcePositionNull(initializedName);
  return initializedName;
}

//-----------------------------------------------
// could have two declarations for a same variable
// extern int i;
//  int i;
SgVariableDeclaration* 
SageBuilder::buildVariableDeclaration (const SgName & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
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

//-----------------------------------------------
// could have two declarations for a same variable
// extern int i;
//  int i;
SgVariableDeclaration* 
SageBuilder::buildVariableDeclaration_nfi (const SgName & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
 //(const SgName & name, SgType* type, SgInitializer * varInit= NULL, SgScopeStatement* scope = NULL)
{
  ROSE_ASSERT (scope != NULL);
   ROSE_ASSERT(type != NULL);

  SgVariableDeclaration * varDecl = new SgVariableDeclaration(name, type, varInit);
  ROSE_ASSERT(varDecl);

  varDecl->set_firstNondefiningDeclaration(varDecl);

  if (name != "") { // Anonymous bit fields should not have symbols
    fixVariableDeclaration(varDecl,scope);
  }

  SgInitializedName *initName = varDecl->get_decl_item (name);   
  ROSE_ASSERT(initName); 
  ROSE_ASSERT((initName->get_declptr())!=NULL);

#if 1
  //bug 119, SgVariableDefintion's File_info is needed for deep copy to work
  // AstQuery based setSourcePositionForTransformation() cannot access all child nodes
  // have to set SgVariableDefintion explicitly
  SgVariableDefinition* variableDefinition_original = isSgVariableDefinition(initName->get_declptr());
  setOneSourcePositionNull(variableDefinition_original);
#endif
  setOneSourcePositionNull(varDecl);

#if 0
// DQ (1/2/2010): Set the defining declaration to itself.
  if (varDecl->get_definingDeclaration() == NULL)
       varDecl->set_definingDeclaration(varDecl);
#endif

  return varDecl;
}

SgVariableDeclaration*
SageBuilder::buildVariableDeclaration(const std::string & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
{
  SgName name2(name);
  return buildVariableDeclaration(name2,type, varInit,scope);
}

SgVariableDeclaration*
SageBuilder::buildVariableDeclaration(const char* name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
{
  SgName name2(name);
  return buildVariableDeclaration(name2,type, varInit,scope);
}

//!Build a typedef declaration, such as: typedef int myint; 
SgTypedefDeclaration* 
SageBuilder::buildTypedefDeclaration(const std::string& name, SgType* base_type, SgScopeStatement* scope /*= NULL*/)
{
  SgTypedefDeclaration* type_decl = buildTypedefDeclaration_nfi(name, base_type, scope);
  setOneSourcePositionForTransformation(type_decl);

// DQ (1/2/2010): Set the defining declaration to itself.
// type_decl->set_definingDeclaration(type_decl);

  return type_decl;
}

//!Build a typedef declaration, such as: typedef int myint; 
// The side effects include: creating SgTypedefType, SgTypedefSymbol, and add SgTypedefType to the base type
SgTypedefDeclaration* 
SageBuilder::buildTypedefDeclaration_nfi(const std::string& name, SgType* base_type,  SgScopeStatement* scope /*= NULL*/)
{
  ROSE_ASSERT (base_type != NULL);
  if (scope == NULL )
    scope = SageBuilder::topScopeStack();

  // We don't yet support bottom up construction for this node yet    
  ROSE_ASSERT(scope!=NULL);

  SgDeclarationStatement * base_decl= NULL;
  if (isSgNamedType(base_type))
  {
    isSgNamedType(base_type)->get_declaration();
  }
  // SgTypedefDeclaration (Sg_File_Info *startOfConstruct, SgName name="", SgType *base_type=NULL, SgTypedefType *type=NULL, SgDeclarationStatement *declaration=NULL, SgSymbol *parent_scope=NULL)
  // SgTypedefDeclaration (SgName name="", SgType *base_type=NULL, SgTypedefType *type=NULL, SgDeclarationStatement *declaration=NULL, SgSymbol *parent_scope=NULL)
  //
  // Create the first nondefining declaration
  SgTypedefDeclaration* type_decl = new SgTypedefDeclaration(SgName(name),base_type,NULL, NULL, NULL);
  ROSE_ASSERT(type_decl);
  type_decl->set_firstNondefiningDeclaration (type_decl);
  type_decl->set_definingDeclaration(NULL);
  setOneSourcePositionNull(type_decl);

  // Symbol and SgTypedefType should be associated with the first nondefining declaration
  // Create SgTypedefType
  // This is already included in the constructor
  // SgTypedefType::createType (type_decl);
  if (scope != NULL)
  {
    SgTypedefSymbol* typedef_symbol = new SgTypedefSymbol(type_decl);
    ROSE_ASSERT(typedef_symbol);
    scope->insert_symbol(SgName(name),typedef_symbol);
    type_decl->set_scope(scope);
    type_decl->set_parent(scope);
  }

  //TODO double check when to create defining declaration
  //I tried two cases so far and the simplest typedef int MYINT will not have defining typedef declaration
  // the complex typedef struct frame {} frame; has a defining typedef declaration
  // base declaration should be associated with defining typedef declaration
  if (base_decl) 
  {
    SgTypedefDeclaration* def_type_decl = new SgTypedefDeclaration(SgName(name),base_type,type_decl->get_type(), base_decl, NULL);
    def_type_decl->set_firstNondefiningDeclaration(type_decl);
    type_decl->set_definingDeclaration(def_type_decl);
    setOneSourcePositionNull(def_type_decl);
  }

#if 0
// DQ (1/2/2010): Set the defining declaration to itself.
  if (type_decl->get_definingDeclaration() == NULL)
       type_decl->set_definingDeclaration(type_decl);
#endif

  return type_decl;
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

SgFunctionParameterList * 
SageBuilder::buildFunctionParameterList_nfi() {
  SgFunctionParameterList *parameterList = new SgFunctionParameterList();
  ROSE_ASSERT (parameterList);
  parameterList->set_definingDeclaration (NULL);
  parameterList->set_firstNondefiningDeclaration (parameterList);

  setOneSourcePositionNull(parameterList);

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

SgFunctionParameterTypeList * 
SageBuilder::buildFunctionParameterTypeList()
{
  SgFunctionParameterTypeList* typePtrList = new SgFunctionParameterTypeList;
  ROSE_ASSERT(typePtrList);
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
  // set its parent to global , deferred until the function is inserted to a scope
 // ROSE_ASSERT (fTable->get_parent() != NULL);
 //   fTable->set_parent();

  SgType* typeInTable = fTable->lookup_function_type(typeName);
  if (typeInTable==NULL)
    fTable->insert_function_type(typeName,funcType);

  return funcType;
}

#if 1
// DQ (1/4/2009): Need to finish this!!!
//-----------------------------------------------
// build member function type, 
// 
// insert into symbol table when not duplicated
SgMemberFunctionType * 
SageBuilder::buildMemberFunctionType(SgType* return_type, SgFunctionParameterTypeList* typeList, SgClassDefinition *struct_name, /* const, volatile, restrict support */ unsigned int mfunc_specifier)
   {
     ROSE_ASSERT(return_type);

  // SgMemberFunctionType (SgType *return_type=NULL, bool has_ellipses=true, SgClassDefinition *struct_name=NULL, unsigned int mfunc_specifier=0)
     SgMemberFunctionType * funcType = new SgMemberFunctionType(return_type, false);
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

  // DQ (1/21/2009): TODO: Need to mark the function type as const, volatile, 
  // or restrict (assert that none are set for now).
     ROSE_ASSERT(mfunc_specifier == 0);

     return funcType;
   }
#endif

 //----------------------------------------------------
 //! Build an opaque type with a name, useful when a type's details are unknown during transformation, especially for a runtime library's internal type.
 SgType * SageBuilder::buildOpaqueType(std::string const name, SgScopeStatement * scope)
{
  // we require users to specify a target scope
  ROSE_ASSERT(scope);
  SgTypedefDeclaration* type_decl = NULL;
  SgTypedefType* result = NULL;

  // Liao and Greg Bronevetsky , 8/27/2009
  // patch up the symbol  
  // and avoid duplicated creation
  // TODO  a function like fixTypeDeclaration() (similar to SageInterface::fixVariableDeclaration()) for this
  SgTypedefSymbol* type_symbol = scope->lookup_typedef_symbol(name);
  if (type_symbol == NULL)
  {
    type_decl =  new SgTypedefDeclaration(name,buildIntType(),NULL, NULL, NULL);
    ROSE_ASSERT(type_decl);
    type_symbol = new SgTypedefSymbol(type_decl);
    ROSE_ASSERT(type_symbol);
    SgName n = name;
    scope->get_symbol_table()->insert(n, type_symbol); 
    type_decl->set_firstNondefiningDeclaration (type_decl);
    setOneSourcePositionForTransformation(type_decl);
    prependStatement(type_decl,scope);
    // Hide it from unparser
    Sg_File_Info* file_info = type_decl->get_file_info();
    file_info->unsetOutputInCodeGeneration ();
    result = new SgTypedefType(type_decl);
  } 
  else
  {
    type_decl = type_symbol->get_declaration();
    result = type_decl->get_type();
  }
  ROSE_ASSERT(result);
  return result;
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
SageBuilder::buildNondefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, bool isMemberFunction, SgScopeStatement* scope)
   {
     if (SageInterface::is_Fortran_language() == true)
     { // We don't expect this is being called for Fortran
       cerr<<"Building nondefining function in Fortran is not allowed!"<<endl;
       ROSE_ASSERT(false);
     }
  // argument verification
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

  // printf ("Building non-defining function for scope = %p in file = %s \n",scope,TransformationSupport::getSourceFile(scope)->getFileName().c_str());

  // DQ (2/25/2009): I think I added this recently but it is overly restrictive.
  // ROSE_ASSERT(scope != NULL);

  // ROSE_ASSERT(scope->containsOnlyDeclarations()); 
  // this function is also called when building a function reference before the function declaration exists.  So, skip the check
     ROSE_ASSERT(name.is_null() == false);
     ROSE_ASSERT(return_type != NULL);
     ROSE_ASSERT(paralist != NULL);

  // tentatively build a function type, since it is shared
  // by all prototypes and defining declarations of a same function!
  // SgFunctionType * func_type = buildFunctionType(return_type,paralist);

     SgFunctionType * func_type = NULL;
     if (isMemberFunction == true)
        {
       // func_type = buildMemberFunctionType(return_type,paralist,NULL,0);
       // func_type = buildFunctionType(return_type,paralist);
          SgClassDefinition *struct_name = isSgClassDefinition(scope);
          ROSE_ASSERT(struct_name != NULL);
          SgFunctionParameterTypeList * typeList = buildFunctionParameterTypeList(paralist);
          func_type = buildMemberFunctionType(return_type,typeList,struct_name,/* const, volatile, restrict support */ 0);

       // printf ("Error: SgFunctionType built instead of SgMemberFunctionType \n");
       // ROSE_ASSERT(false);
        }
       else
        {
          func_type = buildFunctionType(return_type,paralist);
        }

  // function declaration
     actualFunction* func = NULL;

  // search before using the function type to create the function declaration 
  // TODO only search current scope or all ancestor scope??
  // We don't have lookup_member_function_symbol  yet
  // SgFunctionSymbol *func_symbol = scope->lookup_function_symbol(name,func_type);
     SgFunctionSymbol *func_symbol = NULL;
     if (scope != NULL)
          func_symbol = scope->lookup_function_symbol(name,func_type);

  // printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): scope = %p func_symbol = %p \n",scope,func_symbol);
     if (func_symbol == NULL)
        {
       // first prototype declaration
          func = new actualFunction (name,func_type,NULL);
          ROSE_ASSERT(func != NULL);

       // NOTE: we want to allow the input scope to be NULL (and even the SageBuilder::topScopeStack() == NULL)
       // so that function can be built bottom up style.  However this means that the symbol tables in the 
       // scope of the returned function declaration will have to be setup separately.
          if (scope != NULL)
             {
            // function symbol table
               if (isSgMemberFunctionDeclaration(func))
                    func_symbol = new SgMemberFunctionSymbol(func);
                 else
                    func_symbol = new SgFunctionSymbol(func);

            // printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): scope = %p func_symbol = %p = %s = %s \n",scope,func_symbol,func_symbol->class_name().c_str(),SageInterface::get_name(func_symbol).c_str());
               ROSE_ASSERT(func_symbol != NULL);

               scope->insert_symbol(name, func_symbol);

            // ROSE_ASSERT(scope->lookup_function_symbol(name,func_type) != NULL);

            // DQ (2/26/2009): uncommented assertion.
               ROSE_ASSERT(scope->lookup_function_symbol(name) != NULL); // Did not pass for member function? Should we have used the mangled name?
             }

          func->set_firstNondefiningDeclaration(func);
          func->set_definingDeclaration(NULL);

          ROSE_ASSERT(func->get_definingDeclaration() == NULL);
        }
       else 
        {
          ROSE_ASSERT(scope != NULL);

       // 2nd, or 3rd... prototype declaration
       // reuse function type, function symbol of previous declaration

       // std::cout<<"debug:SageBuilder.C: 267: "<<"found func_symbol!"<<std::endl;
       // delete (func_type-> get_argument_list ());
       // delete func_type; // bug 189
   
          func_type = func_symbol->get_declaration()->get_type();
          func = new actualFunction(name,func_type,NULL);
          ROSE_ASSERT(func);
   
       // we don't care if it is member function or function here for a pointer
          SgFunctionDeclaration* prevDecl = NULL;
          prevDecl = func_symbol->get_declaration();
          ROSE_ASSERT(prevDecl != NULL);

       // printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): prevDecl = %p \n",prevDecl);

          if (prevDecl == prevDecl->get_definingDeclaration())
             {
            // The symbol points to a defining declaration and now that we have added a non-defining 
            // declaration we should have the symbol point to the new non-defining declaration.
            // printf ("Switching declaration in functionSymbol to point to the non-defining declaration \n");

               func_symbol->set_declaration(func);
             }

       // If this is the first non-defining declaration then set the associated data member.
          SgDeclarationStatement* nondefiningDeclaration = prevDecl->get_firstNondefiningDeclaration();
          if (nondefiningDeclaration == NULL)
             {
               nondefiningDeclaration = func;
             }

          ROSE_ASSERT(nondefiningDeclaration != NULL);

       // func->set_firstNondefiningDeclaration(prevDecl->get_firstNondefiningDeclaration());
          func->set_firstNondefiningDeclaration(nondefiningDeclaration);
          func->set_definingDeclaration(prevDecl->get_definingDeclaration());
        }

  // DQ (2/24/2009): Delete the old parameter list build by the actualFunction (template argument) constructor.
     ROSE_ASSERT(func->get_parameterList() != NULL);
     delete func->get_parameterList();
     func->set_parameterList(NULL);

  // parameter list
     setParameterList(func, paralist);

     SgInitializedNamePtrList argList = paralist->get_args();
     Rose_STL_Container<SgInitializedName*>::iterator argi;
     for(argi=argList.begin(); argi!=argList.end(); argi++)
        {
       // std::cout<<"patching argument's scope.... "<<std::endl;
          (*argi)->set_scope(scope);

       // DQ (2/23/2009): Also set the declptr (to NULL)
       // (*argi)->set_declptr(NULL);
        }
  // TODO double check if there are exceptions
     func->set_scope(scope);

     // DQ (1/5/2009): This is not always true (should likely use SageBuilder::topScopeStack() instead)
     if (SageBuilder::topScopeStack()!= NULL) // This comparison only makes sense when topScopeStack() returns non-NULL value
       // since  stack scope is totally optional in SageBuilder.
       if (scope != SageBuilder::topScopeStack())
       {
         printf ("Warning: SageBuilder::buildNondefiningFunctionDeclaration_T(): scope parameter may not be the same as the topScopeStack() (e.g. for member functions) \n");
       }

     func->set_parent(scope);

  // DQ (2/21/2009): We can't assert that this is always NULL or non-NULL.
  // ROSE_ASSERT(func->get_definingDeclaration() == NULL);

  // DQ (2/21/2009): Added assertion
     ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);

  // mark as a forward declartion
     func->setForward();

  // set File_Info as transformation generated
     setSourcePositionForTransformation(func);

  // printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): generated function func = %p \n",func);

     return func;  
   }


//! Build a prototype for an existing function declaration (defining or nondefining ) 
SgFunctionDeclaration *
SageBuilder::buildNondefiningFunctionDeclaration (const SgFunctionDeclaration* funcdecl, SgScopeStatement* scope/*=NULL*/)
{
  ROSE_ASSERT(funcdecl!=NULL);
  SgName name=funcdecl->get_name(); 
  SgFunctionType * funcType = funcdecl->get_type();
  SgType* return_type = funcType->get_return_type();
  SgFunctionParameterList* paralist = deepCopy<SgFunctionParameterList>(funcdecl->get_parameterList());

// DQ (2/19/2009): Fixed to handle extern "C" state in input "funcdecl"
// return buildNondefiningFunctionDeclaration(name,return_type,paralist,scope);
  SgFunctionDeclaration* returnFunction = buildNondefiningFunctionDeclaration(name,return_type,paralist,scope);

  returnFunction->set_linkage(funcdecl->get_linkage());

  if (funcdecl->get_declarationModifier().get_storageModifier().isExtern() == true)
     {
       returnFunction->get_declarationModifier().get_storageModifier().setExtern();
     }

// DQ (2/22/2009): I think this can be null or valid
// ROSE_ASSERT(returnFunction->get_definingDeclaration() == NULL);
  ROSE_ASSERT(returnFunction->get_firstNondefiningDeclaration() != NULL);

  // Make sure that internal referneces are to the same file (else the symbol table information will not be consistant).
     if (scope != NULL)
        {
       // ROSE_ASSERT(returnFunction->get_parent() != NULL);
          ROSE_ASSERT(returnFunction->get_firstNondefiningDeclaration() != NULL);
          ROSE_ASSERT(TransformationSupport::getSourceFile(returnFunction) == TransformationSupport::getSourceFile(returnFunction->get_firstNondefiningDeclaration()));
          ROSE_ASSERT(TransformationSupport::getSourceFile(returnFunction->get_scope()) == TransformationSupport::getSourceFile(returnFunction->get_firstNondefiningDeclaration()));
        }

  return returnFunction;
}

SgFunctionDeclaration*
SageBuilder::buildNondefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope)
{
  SgFunctionDeclaration * result = buildNondefiningFunctionDeclaration_T <SgFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ false, scope);
  return result;
}

//! Build a prototype for an existing member function declaration (defining or nondefining ) 
SgMemberFunctionDeclaration *
SageBuilder::buildNondefiningMemberFunctionDeclaration (const SgMemberFunctionDeclaration* funcdecl, SgScopeStatement* scope/*=NULL*/)
{
  ROSE_ASSERT(funcdecl!=NULL);
  SgName name=funcdecl->get_name(); 
  SgFunctionType * funcType = funcdecl->get_type();
  SgType* return_type = funcType->get_return_type();
  SgFunctionParameterList* paralist = deepCopy<SgFunctionParameterList>(funcdecl->get_parameterList());

// DQ (2/19/2009): Fixed to handle extern "C" state in input "funcdecl"
// return buildNondefiningFunctionDeclaration(name,return_type,paralist,scope);
  SgMemberFunctionDeclaration* returnFunction = buildNondefiningMemberFunctionDeclaration(name,return_type,paralist,scope);

  returnFunction->set_linkage(funcdecl->get_linkage());

  if (funcdecl->get_declarationModifier().get_storageModifier().isExtern() == true)
     {
       returnFunction->get_declarationModifier().get_storageModifier().setExtern();
     }

// DQ (2/26/2009): Make this consistant with the non-member functions.
// ROSE_ASSERT(returnFunction->get_definingDeclaration() == NULL);
  ROSE_ASSERT(returnFunction->get_firstNondefiningDeclaration() != NULL);

  return returnFunction;
}

SgMemberFunctionDeclaration* SageBuilder::buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope)
{
  SgMemberFunctionDeclaration * result = buildNondefiningFunctionDeclaration_T <SgMemberFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ true,scope);
  // set definingdecl for SgCtorInitializerList
  SgCtorInitializerList * ctor= result-> get_CtorInitializerList ();
  ROSE_ASSERT(ctor != NULL);
  //required ty AstConsistencyTests.C:TestAstForProperlySetDefiningAndNondefiningDeclarations()
  ctor->set_definingDeclaration(ctor);
  ctor->set_firstNondefiningDeclaration(ctor);

// DQ (1/4/2009): Error checking
  ROSE_ASSERT(result->get_associatedClassDeclaration() != NULL);

  if (result->get_associatedClassDeclaration() == NULL)
     {
       printf ("Warning, must set the SgMemberFunctionDeclaration::associatedClassDeclaration \n");

       ROSE_ASSERT(scope != NULL);
       SgClassDefinition* classDefinition = isSgClassDefinition(scope);
       ROSE_ASSERT(classDefinition != NULL);
       SgDeclarationStatement* associatedDeclaration = classDefinition->get_declaration();
       ROSE_ASSERT(associatedDeclaration != NULL);
       SgClassDeclaration* associatedClassDeclaration = isSgClassDeclaration(associatedDeclaration);

    // DQ (1/4/2009): This needs to be set, checked in AstConsistencyTests.C!
       result->set_associatedClassDeclaration(associatedClassDeclaration);
     }

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

// DQ (1/4/2009): Error checking
  ROSE_ASSERT(result->get_associatedClassDeclaration() != NULL);

  return result;
}


//----------------- defining function declaration------------
// a template builder for all kinds of defining SgFunctionDeclaration
// handle common chores for function type, symbol, paramter etc.

template <class actualFunction>
actualFunction *
SageBuilder::buildDefiningFunctionDeclaration_T(const SgName & name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope)
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

    func->set_firstNondefiningDeclaration(func_symbol->get_declaration()->get_firstNondefiningDeclaration());

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
SageBuilder::buildDefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* paralist,
                                              SgScopeStatement* scope)
{
  SgFunctionDeclaration * func= buildDefiningFunctionDeclaration_T<SgFunctionDeclaration>(name,return_type,paralist,scope);
  return func;
}


//! Build a Fortran subroutine or procedure
SgProcedureHeaderStatement*
SageBuilder::buildProcedureHeaderStatement(const char* name, SgType* return_type, SgFunctionParameterList * paralist, SgProcedureHeaderStatement::subprogram_kind_enum kind, SgScopeStatement* scope/*=NULL*/)
{
  if (kind == SgProcedureHeaderStatement::e_subroutine_subprogram_kind)
  {
    ROSE_ASSERT(return_type == buildVoidType());
  }
   else if (kind != SgProcedureHeaderStatement::e_function_subprogram_kind)
   {
     cerr<<"unhandled subprogram kind for Fortran function unit:"<<kind<<endl;
     ROSE_ASSERT(false);
   }

   SgProcedureHeaderStatement* func= buildDefiningFunctionDeclaration_T<SgProcedureHeaderStatement> (name,return_type,paralist,scope);
  ROSE_ASSERT(func != NULL);
   func->set_subprogram_kind(kind) ;
  return func;
}

SgFunctionDeclaration *
SageBuilder::buildDefiningFunctionDeclaration(const std::string & name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope)
{
  SgName sg_name(name);
  return buildDefiningFunctionDeclaration(sg_name,return_type, paralist,scope);
}

SgFunctionDeclaration *
SageBuilder::buildDefiningFunctionDeclaration(const char* name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope)
{
  SgName sg_name(name);
  return buildDefiningFunctionDeclaration(sg_name,return_type, paralist,scope);
}


//------------------build value expressions -------------------
//-------------------------------------------------------------
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
SgBoolValExp* SageBuilder::buildBoolValExp_nfi(int value)
{
  SgBoolValExp* boolValue= new SgBoolValExp(value);
  ROSE_ASSERT(boolValue);
  setOneSourcePositionNull(boolValue);
  return boolValue;
}

SgCharVal* SageBuilder::buildCharVal(char value /*= 0*/)
{
  SgCharVal* result = new SgCharVal(value, "");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgCharVal* SageBuilder::buildCharVal_nfi(char value, const string& str)
{
  SgCharVal* result = new SgCharVal(value, str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgWcharVal* SageBuilder::buildWcharVal(wchar_t value /*= 0*/)
{
  SgWcharVal* result = new SgWcharVal(value, "");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgWcharVal* SageBuilder::buildWcharVal_nfi(wchar_t value, const string& str)
{
  SgWcharVal* result = new SgWcharVal(value, str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgComplexVal* SageBuilder::buildComplexVal(SgValueExp* real_value, SgValueExp* imaginary_value)
{
  SgComplexVal* result = new SgComplexVal(real_value,imaginary_value,imaginary_value->get_type(),"");
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  if (real_value != NULL)
       real_value->set_parent(result);

  if (imaginary_value != NULL)
       imaginary_value->set_parent(result);

  ROSE_ASSERT(real_value == NULL || real_value->get_parent() != NULL);
  ROSE_ASSERT(imaginary_value == NULL || imaginary_value->get_parent() != NULL);

  setOneSourcePositionForTransformation(result);
  return result;
}

SgComplexVal* SageBuilder::buildComplexVal_nfi(SgValueExp* real_value, SgValueExp* imaginary_value, const std::string& str)
{
  SgComplexVal* result = new SgComplexVal(real_value,imaginary_value,imaginary_value->get_type(),str);
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  if (real_value != NULL)
       real_value->set_parent(result);

  if (imaginary_value != NULL)
       imaginary_value->set_parent(result);

  ROSE_ASSERT(real_value == NULL || real_value->get_parent() != NULL);
  ROSE_ASSERT(imaginary_value == NULL || imaginary_value->get_parent() != NULL);

  setOneSourcePositionNull(result);
  return result;
}

SgComplexVal* SageBuilder::buildImaginaryVal(long double imaginary_value /*= 0.0*/ )
{
  SgComplexVal* result = new SgComplexVal(NULL,buildLongDoubleVal(imaginary_value),SgTypeLongDouble::createType(),"");
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  result->get_imaginary_value()->set_parent(result);
  ROSE_ASSERT(result->get_imaginary_value()->get_parent() != NULL);

  setOneSourcePositionForTransformation(result);
  return result;
}

SgComplexVal* SageBuilder::buildImaginaryVal(SgValueExp* imaginary_value)
{
  ROSE_ASSERT(imaginary_value != NULL);

  SgComplexVal* result = new SgComplexVal(NULL,imaginary_value,imaginary_value->get_type(),"");
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  imaginary_value->set_parent(result);
  ROSE_ASSERT(imaginary_value->get_parent() != NULL);

  setOneSourcePositionForTransformation(result);
  return result;
}

SgComplexVal* SageBuilder::buildImaginaryVal_nfi(SgValueExp* imaginary_value, const std::string& str)
{
  ROSE_ASSERT(imaginary_value != NULL);

  SgComplexVal* result = new SgComplexVal(NULL,imaginary_value,imaginary_value->get_type(),str);
  imaginary_value->set_parent(result);
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  ROSE_ASSERT(imaginary_value->get_parent() != NULL);

  setOneSourcePositionNull(result);
  return result;
}

SgDoubleVal* SageBuilder::buildDoubleVal(double t)
{
  SgDoubleVal* value = new SgDoubleVal(t,"");
  ROSE_ASSERT(value);
  setOneSourcePositionForTransformation(value);
  return value;
}

SgDoubleVal* SageBuilder::buildDoubleVal_nfi(double t, const string& str)
{
  SgDoubleVal* value = new SgDoubleVal(t,str);
  ROSE_ASSERT(value);
  setOneSourcePositionNull(value);
  return value;
}

SgFloatVal* SageBuilder::buildFloatVal(float value /*= 0.0*/)
{
  SgFloatVal* result = new SgFloatVal(value,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgFloatVal* SageBuilder::buildFloatVal_nfi(float value, const string& str)
{
  SgFloatVal* result = new SgFloatVal(value,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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

SgIntVal* SageBuilder::buildIntVal_nfi(int value, const string& str)
{
  SgIntVal* intValue= new SgIntVal(value,str);
  ROSE_ASSERT(intValue);
  setOneSourcePositionNull(intValue);
  return intValue;
}

SgLongIntVal* SageBuilder::buildLongIntVal(long value)
{
  SgLongIntVal* intValue= new SgLongIntVal(value,"");
  ROSE_ASSERT(intValue);
  setOneSourcePositionForTransformation(intValue);
  return intValue;
}

SgLongIntVal* SageBuilder::buildLongIntVal_nfi(long value, const string& str)
{
  SgLongIntVal* intValue= new SgLongIntVal(value,str);
  ROSE_ASSERT(intValue);
  setOneSourcePositionNull(intValue);
  return intValue;
}

SgLongLongIntVal* SageBuilder::buildLongLongIntVal(long long value)
{
  SgLongLongIntVal* intValue= new SgLongLongIntVal(value,"");
  ROSE_ASSERT(intValue);
  setOneSourcePositionForTransformation(intValue);
  return intValue;
}

SgLongLongIntVal* SageBuilder::buildLongLongIntVal_nfi(long long value, const string& str)
{
  SgLongLongIntVal* intValue= new SgLongLongIntVal(value,str);
  ROSE_ASSERT(intValue);
  setOneSourcePositionNull(intValue);
  return intValue;
}

SgEnumVal* SageBuilder::buildEnumVal_nfi(int value, SgEnumDeclaration* decl, SgName name)
{
  SgEnumVal* enumVal= new SgEnumVal(value,decl,name);
  ROSE_ASSERT(enumVal);
  setOneSourcePositionNull(enumVal);
  return enumVal;
}

SgLongDoubleVal* SageBuilder::buildLongDoubleVal(long double value /*= 0.0*/)
{
  SgLongDoubleVal* result = new SgLongDoubleVal(value,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgLongDoubleVal* SageBuilder::buildLongDoubleVal_nfi(long double value, const string& str)
{
  SgLongDoubleVal* result = new SgLongDoubleVal(value,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgStringVal* SageBuilder::buildStringVal(std::string value /*=""*/)
{
  SgStringVal* result = new SgStringVal(value);
  ROSE_ASSERT(result);   
  setOneSourcePositionForTransformation(result);
  return result;
}

SgStringVal* SageBuilder::buildStringVal_nfi(std::string value)
{
  SgStringVal* result = new SgStringVal(value);
  ROSE_ASSERT(result);   
  setOneSourcePositionNull(result);
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

SgUnsignedCharVal* SageBuilder::buildUnsignedCharVal_nfi(unsigned char v, const string& str)
{
  SgUnsignedCharVal* result = new SgUnsignedCharVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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

SgShortVal* SageBuilder::buildShortVal_nfi(short v, const string& str)
{
  SgShortVal* result = new SgShortVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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

SgUnsignedShortVal* SageBuilder::buildUnsignedShortVal_nfi(unsigned short v, const string& str)
{
  SgUnsignedShortVal* result = new SgUnsignedShortVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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

SgUnsignedIntVal* SageBuilder::buildUnsignedIntVal_nfi(unsigned int v, const string& str)
{
  SgUnsignedIntVal* result = new SgUnsignedIntVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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

SgUnsignedLongVal* SageBuilder::buildUnsignedLongVal_nfi(unsigned long v, const string& str)
{
  SgUnsignedLongVal* result = new SgUnsignedLongVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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

SgUnsignedLongLongIntVal* SageBuilder::buildUnsignedLongLongIntVal_nfi(unsigned long long v, const string& str)
{
  SgUnsignedLongLongIntVal* result = new SgUnsignedLongLongIntVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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

//! Build UPC THREADS (integer expression)
SgUpcThreads* SageBuilder::buildUpcThreads_nfi()
{
  SgUpcThreads* result = new SgUpcThreads(0,"");
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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

//! Build UPC  MYTHREAD (integer expression)
SgUpcMythread* SageBuilder::buildUpcMythread_nfi()
{
  SgUpcMythread* result = new SgUpcMythread(0,"");
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgThisExp* SageBuilder::buildThisExp(SgClassSymbol* sym)
{
  SgThisExp* result = new SgThisExp(sym, 0);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgThisExp* SageBuilder::buildThisExp_nfi(SgClassSymbol* sym)
{
  SgThisExp* result = new SgThisExp(sym, 0);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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
T* SageBuilder::buildUnaryExpression_nfi(SgExpression* operand) {
  SgExpression* myoperand=operand;
  T* result = new T(myoperand, NULL);
  ROSE_ASSERT(result);   
  if (myoperand!=NULL) 
  { 
    myoperand->set_parent(result);
  // set lvalue, it asserts operand!=NULL 
    markLhsValues(result);
  }
  result->set_startOfConstruct(NULL);
  result->set_endOfConstruct(NULL);
  result->set_operatorPosition(NULL);
  result->set_need_paren(false);
  return result; 
}

#define BUILD_UNARY_DEF(suffix) \
  Sg##suffix* SageBuilder::build##suffix##_nfi(SgExpression* op) \
  { \
     return SageBuilder::buildUnaryExpression_nfi<Sg##suffix>(op); \
  } \
  Sg##suffix* SageBuilder::build##suffix(SgExpression* op) \
  { \
     return SageBuilder::buildUnaryExpression<Sg##suffix>(op); \
  }

BUILD_UNARY_DEF(AddressOfOp)
BUILD_UNARY_DEF(BitComplementOp)
BUILD_UNARY_DEF(MinusOp)
BUILD_UNARY_DEF(NotOp)
BUILD_UNARY_DEF(PointerDerefExp)
BUILD_UNARY_DEF(UnaryAddOp)
BUILD_UNARY_DEF(MinusMinusOp)
BUILD_UNARY_DEF(PlusPlusOp)
BUILD_UNARY_DEF(RealPartOp)
BUILD_UNARY_DEF(ImagPartOp)
BUILD_UNARY_DEF(ConjugateOp)
BUILD_UNARY_DEF(VarArgStartOneOperandOp)
BUILD_UNARY_DEF(VarArgEndOp)

#undef BUILD_UNARY_DEF

SgCastExp * SageBuilder::buildCastExp(SgExpression *  operand_i,
                SgType * expression_type,
                SgCastExp::cast_type_enum cast_type)
{
  SgCastExp* result = new SgCastExp(operand_i, expression_type, cast_type);
  ROSE_ASSERT(result);
  if (operand_i) {operand_i->set_parent(result); markLhsValues(result);}
  setOneSourcePositionForTransformation(result);
  return result;
}

SgNewExp * SageBuilder::buildNewExp(SgType* type, 
				    SgExprListExp* exprListExp, 
				    SgConstructorInitializer* constInit, 
				    SgExpression* expr, 
				    short int val, 
				    SgFunctionDeclaration* funcDecl)
{
  SgNewExp* result = new SgNewExp(type, exprListExp, constInit, expr, val, funcDecl);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgCastExp * SageBuilder::buildCastExp_nfi(SgExpression *  operand_i,
                SgType * expression_type,
                SgCastExp::cast_type_enum cast_type)
{
  SgCastExp* result = new SgCastExp(operand_i, expression_type, cast_type);
  ROSE_ASSERT(result);
  if (operand_i) {operand_i->set_parent(result); markLhsValues(result);}
  setOneSourcePositionNull(result);
  return result;
}

SgVarArgOp * SageBuilder::buildVarArgOp_nfi(SgExpression *  operand_i, SgType * expression_type) {
  SgVarArgOp* result = new SgVarArgOp(operand_i, expression_type);
  ROSE_ASSERT(result);
  if (operand_i) {operand_i->set_parent(result); markLhsValues(result);}
  setOneSourcePositionNull(result);
  return result;
}

SgMinusMinusOp *SageBuilder::buildMinusMinusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgMinusMinusOp* result = buildUnaryExpression<SgMinusMinusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
}

SgMinusMinusOp *SageBuilder::buildMinusMinusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgMinusMinusOp* result = buildUnaryExpression_nfi<SgMinusMinusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
}

SgPlusPlusOp *SageBuilder::buildPlusPlusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgPlusPlusOp* result = buildUnaryExpression<SgPlusPlusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
}


SgPlusPlusOp *SageBuilder::buildPlusPlusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgPlusPlusOp* result = buildUnaryExpression_nfi<SgPlusPlusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
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
T* SageBuilder::buildBinaryExpression_nfi(SgExpression* lhs, SgExpression* rhs)
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
  result->set_startOfConstruct(NULL);
  result->set_endOfConstruct(NULL);
  result->set_operatorPosition(NULL);
  result->set_need_paren(false);
  return result;

}
#define BUILD_BINARY_DEF(suffix) \
  Sg##suffix* SageBuilder::build##suffix##_nfi(SgExpression* lhs, SgExpression* rhs) \
  { \
     return buildBinaryExpression_nfi<Sg##suffix>(lhs, rhs); \
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
BUILD_BINARY_DEF(DivideOp)
BUILD_BINARY_DEF(DotExp)
BUILD_BINARY_DEF(DotStarOp)
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

BUILD_BINARY_DEF(VarArgCopyOp)
BUILD_BINARY_DEF(VarArgStartOp)

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
  if (test) test->set_parent(result);
  if (a) a->set_parent(result);
  if (b) b->set_parent(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgConditionalExp* SageBuilder::buildConditionalExp_nfi(SgExpression* test, SgExpression* a, SgExpression* b, SgType* t)
{
  SgConditionalExp* result = new SgConditionalExp(test, a, b, t);
  if (test) test->set_parent(result);
  if (a) {a->set_parent(result); markLhsValues(a);}
  if (b) {b->set_parent(result); markLhsValues(b);}
  setOneSourcePositionNull(result);
  return result;
}

SgNullExpression* SageBuilder::buildNullExpression_nfi()
{
  SgNullExpression* ne= new SgNullExpression();
  ROSE_ASSERT(ne);
  setOneSourcePositionNull(ne);
  return ne;
}

SgNullExpression* SageBuilder::buildNullExpression() {
  SgNullExpression* e = buildNullExpression_nfi();
  setOneSourcePositionForTransformation(e);
  return e;
}

SgAssignInitializer * SageBuilder::buildAssignInitializer(SgExpression * operand_i /*= NULL*/, SgType * expression_type /* = NULL */)
{
  SgAssignInitializer* result = new SgAssignInitializer(operand_i, expression_type);
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

SgAssignInitializer * SageBuilder::buildAssignInitializer_nfi(SgExpression * operand_i /*= NULL*/, SgType * expression_type /* = UNLL */)
{
  SgAssignInitializer* result = new SgAssignInitializer(operand_i, expression_type);
  ROSE_ASSERT(result);   
  if (operand_i!=NULL) 
  { 
    operand_i->set_parent(result);
  // set lvalue, it asserts operand!=NULL 
    markLhsValues(result);
  }
  result->set_startOfConstruct(NULL);
  result->set_endOfConstruct(NULL);
  result->set_operatorPosition(NULL);
  result->set_need_paren(false);
  return result; 
}

//! Build an aggregate initializer
SgAggregateInitializer * SageBuilder::buildAggregateInitializer(SgExprListExp * initializers/* = NULL*/, SgType *type/* = NULL */)
{
  SgAggregateInitializer* result = new SgAggregateInitializer(initializers, type);
  ROSE_ASSERT(result);
  if (initializers!=NULL)
  {
    initializers->set_parent(result);
  }
  result->set_need_explicit_braces(true);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build an aggregate initializer
SgAggregateInitializer * SageBuilder::buildAggregateInitializer_nfi(SgExprListExp * initializers/* = NULL*/, SgType *type/* = NULL */)
{
  SgAggregateInitializer* result = new SgAggregateInitializer(initializers, type);
  ROSE_ASSERT(result);
  if (initializers!=NULL)
  {
    initializers->set_parent(result);
  }
  result->set_need_explicit_braces(true);
  setOneSourcePositionNull(result);
  return result;
}

// DQ (1/4/2009): Added support for SgConstructorInitializer
SgConstructorInitializer *
SageBuilder::buildConstructorInitializer(
   SgMemberFunctionDeclaration *declaration/* = NULL*/,
   SgExprListExp *args/* = NULL*/,
   SgType *expression_type/* = NULL*/,
   bool need_name /*= false*/,
   bool need_qualifier /*= false*/,
   bool need_parenthesis_after_name /*= false*/,
   bool associated_class_unknown /*= false*/)
   {
  // Prototype:
  // SgConstructorInitializer (SgMemberFunctionDeclaration *declaration, SgExprListExp *args, SgType *expression_type, bool need_name, bool need_qualifier, bool need_parenthesis_after_name, bool associated_class_unknown);

  // DQ (1/4/2009): Error checking
     ROSE_ASSERT(declaration->get_associatedClassDeclaration() != NULL);

     SgConstructorInitializer* result = new SgConstructorInitializer( declaration, args, expression_type, need_name, need_qualifier, need_parenthesis_after_name, associated_class_unknown );
     ROSE_ASSERT(result != NULL);
     if (args != NULL)
        {
          args->set_parent(result);
          setOneSourcePositionForTransformation(args);
        }

     setOneSourcePositionForTransformation(result);

     return result;
   }

// DQ (1/4/2009): Added support for SgConstructorInitializer
SgConstructorInitializer *
SageBuilder::buildConstructorInitializer_nfi(
   SgMemberFunctionDeclaration *declaration/* = NULL*/,
   SgExprListExp *args/* = NULL*/,
   SgType *expression_type/* = NULL*/,
   bool need_name /*= false*/,
   bool need_qualifier /*= false*/,
   bool need_parenthesis_after_name /*= false*/,
   bool associated_class_unknown /*= false*/)
   {
  // Prototype:
  // SgConstructorInitializer (SgMemberFunctionDeclaration *declaration, SgExprListExp *args, SgType *expression_type, bool need_name, bool need_qualifier, bool need_parenthesis_after_name, bool associated_class_unknown);

  // DQ (1/4/2009): Error checking
     ROSE_ASSERT(declaration->get_associatedClassDeclaration() != NULL);

     SgConstructorInitializer* result = new SgConstructorInitializer( declaration, args, expression_type, need_name, need_qualifier, need_parenthesis_after_name, associated_class_unknown );
     ROSE_ASSERT(result != NULL);
     if (args != NULL)
        {
          args->set_parent(result);
        }

     setOneSourcePositionNull(result);

     return result;
   }



//! Build sizeof() expression with an expression parameter
SgSizeOfOp* SageBuilder::buildSizeOfOp(SgExpression* exp/*= NULL*/)
{
  SgType* exp_type =NULL;
  if (exp) exp_type = exp->get_type();

  SgSizeOfOp* result = new SgSizeOfOp(exp,NULL, NULL);
  //SgSizeOfOp* result = new SgSizeOfOp(exp,NULL, exp_type);
  ROSE_ASSERT(result);
  if (exp)
  {
    exp->set_parent(result);
    markLhsValues(result);
  }
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build sizeof() expression with an expression parameter
SgSizeOfOp* SageBuilder::buildSizeOfOp_nfi(SgExpression* exp/*= NULL*/)
{
  SgType* exp_type =NULL;
  if (exp) exp_type = exp->get_type();

  SgSizeOfOp* result = new SgSizeOfOp(exp,NULL, NULL);
  //SgSizeOfOp* result = new SgSizeOfOp(exp,NULL, exp_type);
  ROSE_ASSERT(result);
  if (exp)
  {
    exp->set_parent(result);
    markLhsValues(result);
  }
  setOneSourcePositionNull(result);
  return result;
}

//! Build sizeof() expression with a type parameter
SgSizeOfOp* SageBuilder::buildSizeOfOp(SgType* type /* = NULL*/)
{
  SgSizeOfOp* result = new SgSizeOfOp((SgExpression*)NULL,type,NULL);
  //SgSizeOfOp* result = new SgSizeOfOp((SgExpression*)NULL,type,type);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build sizeof() expression with a type parameter
SgSizeOfOp* SageBuilder::buildSizeOfOp_nfi(SgType* type /* = NULL*/)
{
  SgSizeOfOp* result = new SgSizeOfOp((SgExpression*)NULL,type,NULL);
  //SgSizeOfOp* result = new SgSizeOfOp((SgExpression*)NULL,type,type);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
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

// CH (5/11/2010): Seems that this function is useful.
SgExprListExp * SageBuilder::buildExprListExp(const std::vector<SgExpression*>& exprs)
{
  SgExprListExp* expList = new SgExprListExp();
  ROSE_ASSERT(expList);
  setOneSourcePositionForTransformation(expList);
  for (size_t i = 0; i < exprs.size(); ++i) {
    appendExpression(expList, exprs[i]);
  }
  return expList;
}

SgExprListExp * SageBuilder::buildExprListExp_nfi()
{
  SgExprListExp* expList = new SgExprListExp();
  ROSE_ASSERT(expList);
  setOneSourcePositionNull(expList);
  return expList;
}

SgExprListExp * SageBuilder::buildExprListExp_nfi(const std::vector<SgExpression*>& exprs)
{
  SgExprListExp* expList = new SgExprListExp();
  ROSE_ASSERT(expList);
  setOneSourcePositionNull(expList);
  for (size_t i = 0; i < exprs.size(); ++i) {
    appendExpression(expList, exprs[i]);
  }
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
  // there is assertion for get_scope() != NULL in get_symbol_from_symbol_table()
  SgSymbol* symbol = NULL;
  if (initname->get_scope()!=NULL)
    symbol = initname->get_symbol_from_symbol_table ();
  
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
SageBuilder::buildVarRefExp(const SgName& name, SgScopeStatement* scope/*=NULL*/)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
 // ROSE_ASSERT(scope != NULL); // we allow to build a dangling ref without symbol
  SgSymbol * symbol = NULL;
  SgVariableSymbol* varSymbol=NULL;
 if (scope)
  symbol = lookupSymbolInParentScopes(name,scope); 
 if (symbol) 
    varSymbol= isSgVariableSymbol(symbol); 
  else
// if not found: put fake init name and symbol here and 
//waiting for a postProcessing phase to clean it up
// two features: no scope and unknown type for initializedName
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

//! Build a variable reference from an existing variable declaration. The assumption is a SgVariableDeclartion only declares one variable in the ROSE AST.
SgVarRefExp *
SageBuilder::buildVarRefExp(SgVariableDeclaration* vardecl)
{
  SgVariableSymbol* symbol = getFirstVarSym(vardecl);
  ROSE_ASSERT(symbol);
  return buildVarRefExp(symbol);
}


SgVarRefExp *
SageBuilder::buildVarRefExp(SgVariableSymbol* sym)
{
  SgVarRefExp *varRef = new SgVarRefExp(sym);
  setOneSourcePositionForTransformation(varRef);
  ROSE_ASSERT(varRef);
  return varRef; 
}

SgVarRefExp *
SageBuilder::buildVarRefExp_nfi(SgVariableSymbol* sym)
{
  SgVarRefExp *varRef = new SgVarRefExp(sym);
  setOneSourcePositionNull(varRef);
  ROSE_ASSERT(varRef);
  return varRef; 
}

//!Build a variable reference expression at scope to an opaque variable which has unknown information except for its name.  Used when referring to an internal variable defined in some headers of runtime libraries.(The headers are not yet inserted into the file during translation). Similar to buildOpaqueType(); 
/*! It will declare a hidden int varName  at the specified scope to cheat the AST consistence tests.
 */
SgVarRefExp*
SageBuilder::buildOpaqueVarRefExp(const std::string& name,SgScopeStatement* scope/* =NULL */)
{
  SgVarRefExp *result = NULL;

  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);

  SgSymbol * symbol = lookupSymbolInParentScopes(name,scope); 
  if (symbol)
  {
    // Can be the same opaque var ref built before
  //  cerr<<"Error: trying to build an opaque var ref when the variable is actual explicit!"<<endl;
   //    ROSE_ASSERT(false);
     ROSE_ASSERT(isSgVariableSymbol(symbol));
     result = buildVarRefExp(isSgVariableSymbol(symbol));  
  }
  else
  {
    SgVariableDeclaration* fakeVar = buildVariableDeclaration(name, buildIntType(),NULL, scope);
    Sg_File_Info* file_info = fakeVar->get_file_info();
    file_info->unsetOutputInCodeGeneration ();
    SgVariableSymbol * 	fakeSymbol = getFirstVarSym (fakeVar);   
    result = buildVarRefExp(fakeSymbol);
  } // if
  return result;
} // buildOpaqueVarRefExp()

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

SgFunctionParameterList*
SageBuilder::buildFunctionParameterList_nfi(SgFunctionParameterTypeList * paraTypeList)
{
  SgFunctionParameterList* paraList = buildFunctionParameterList();
  ROSE_ASSERT (paraList);
  SgTypePtrList typeList = paraTypeList->get_arguments();
  SgTypePtrList::iterator i;
  for (i=typeList.begin();i!=typeList.end();i++)
  {
    SgInitializedName* arg = buildInitializedName_nfi(SgName(""),(*i),NULL);
    appendArg(paraList,arg);
  }
  return paraList;
}

// lookup function symbol to create a reference to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const SgName& name,const SgType* funcType, SgScopeStatement* scope /*=NULL*/)
{
  ROSE_ASSERT(funcType); // function type cannot be NULL
  SgFunctionType* func_type = isSgFunctionType(const_cast<SgType*>(funcType));
  ROSE_ASSERT(func_type);

  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  SgFunctionSymbol* symbol = lookupFunctionSymbolInParentScopes(name,func_type,scope);
  if (symbol==NULL) 
    // in rare cases when function calls are inserted before any prototypes exist
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
    // Prepend a function prototype declaration in current scope, hide it from the unparser
    // prependStatement(funcDecl,scope);
    // Sg_File_Info* file_info = funcDecl->get_file_info();
    // file_info->unsetOutputInCodeGeneration ();

    symbol = lookupFunctionSymbolInParentScopes(name,func_type,scope);
    ROSE_ASSERT(symbol);
#endif
  }
  SgFunctionRefExp* func_ref = new SgFunctionRefExp(symbol,func_type);
  setOneSourcePositionForTransformation(func_ref);

  ROSE_ASSERT(func_ref);
  return func_ref;
}

SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const char* name,const SgType* funcType, SgScopeStatement* scope /*=NULL*/)
{
  SgName name2(name);
  return buildFunctionRefExp(name2,funcType,scope);
}

// lookup function symbol to create a reference to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const SgFunctionDeclaration* func_decl)
{
  ROSE_ASSERT(func_decl != NULL);
  SgDeclarationStatement* nondef_func = func_decl->get_firstNondefiningDeclaration ();
  ROSE_ASSERT(nondef_func!= NULL);
  SgSymbol* symbol = nondef_func->get_symbol_from_symbol_table();
  ROSE_ASSERT( symbol != NULL);
  return buildFunctionRefExp( isSgFunctionSymbol (symbol));
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

// lookup function symbol to create a reference to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp_nfi(SgFunctionSymbol* sym)
{
  SgFunctionRefExp* func_ref = new SgFunctionRefExp(sym, NULL);
  setOneSourcePositionNull(func_ref);
  ROSE_ASSERT(func_ref);
  return func_ref;
}

// lookup member function symbol to create a reference to it
SgMemberFunctionRefExp *
SageBuilder::buildMemberFunctionRefExp_nfi(SgMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier)
{
  SgMemberFunctionRefExp* func_ref = new SgMemberFunctionRefExp(sym, virtual_call, NULL, need_qualifier);
  setOneSourcePositionNull(func_ref);
  ROSE_ASSERT(func_ref);
  return func_ref;
}

// lookup member function symbol to create a reference to it
SgMemberFunctionRefExp *
SageBuilder::buildMemberFunctionRefExp(SgMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier)
{
  SgMemberFunctionRefExp* func_ref = new SgMemberFunctionRefExp(sym, virtual_call, NULL, need_qualifier);
  setOneSourcePositionForTransformation(func_ref);
  ROSE_ASSERT(func_ref);
  return func_ref;
}

// lookup class symbol to create a reference to it
SgClassNameRefExp *
SageBuilder::buildClassNameRefExp_nfi(SgClassSymbol* sym)
{
  SgClassNameRefExp* class_ref = new SgClassNameRefExp(sym);
  setOneSourcePositionNull(class_ref);
  ROSE_ASSERT(class_ref);
  return class_ref;
}

SgClassNameRefExp *
SageBuilder::buildClassNameRefExp(SgClassSymbol* sym)
{
  SgClassNameRefExp* class_ref = new SgClassNameRefExp(sym);
  setOneSourcePositionForTransformation(class_ref);
  ROSE_ASSERT(class_ref);
  return class_ref;
}

//! Lookup a C style function symbol to create a function reference expression to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const SgName& name, SgScopeStatement* scope /*=NULL*/)
{
  if (scope == NULL)
     scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  SgFunctionSymbol* symbol = lookupFunctionSymbolInParentScopes(name,scope);


  if (symbol==NULL) 
// in rare cases when function calls are inserted before any prototypes exist
  {
// assume int return type, and empty parameter list
    SgType* return_type = buildIntType();
    SgFunctionParameterList *parList = buildFunctionParameterList();

    SgGlobal* globalscope = getGlobalScope(scope);
    SgFunctionDeclaration * funcDecl= buildNondefiningFunctionDeclaration(name,return_type,parList,globalscope);
     funcDecl->get_declarationModifier().get_storageModifier().setExtern();

   
    symbol = lookupFunctionSymbolInParentScopes(name,scope);
    ROSE_ASSERT(symbol);
  }

  SgFunctionRefExp* func_ref = buildFunctionRefExp(symbol);
  setOneSourcePositionForTransformation(func_ref);

  ROSE_ASSERT(func_ref);
  return func_ref;
}

SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const char* name, SgScopeStatement* scope /*=NULL*/)
{
  SgName name2(name); 
  return buildFunctionRefExp(name2,scope);
}


SgExprStatement*
SageBuilder::buildExprStatement(SgExpression*  exp)
{
  SgExprStatement* expStmt = new SgExprStatement(exp);
  ROSE_ASSERT(expStmt);
  if (exp) exp->set_parent(expStmt);
  setOneSourcePositionForTransformation(expStmt);
  return expStmt;
}

SgExprStatement*
SageBuilder::buildExprStatement_nfi(SgExpression*  exp)
{
  SgExprStatement* expStmt = new SgExprStatement(exp);
  ROSE_ASSERT(expStmt);
  if (exp) exp->set_parent(expStmt);
  setOneSourcePositionNull(expStmt);
  return expStmt;
}

SgFunctionCallExp* 
SageBuilder::buildFunctionCallExp(const SgName& name, 
                                                SgType* return_type, 
                                                SgExprListExp* parameters/*=NULL*/, 
                                             SgScopeStatement* scope/*=NULL*/)
{
  if (scope == NULL)    
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL); 
 if (parameters == NULL)
     parameters = buildExprListExp();
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
                                  SgExprListExp* parameters/*=NULL*/)
{
  ROSE_ASSERT (sym);
  if (parameters == NULL)
    parameters = buildExprListExp();
  ROSE_ASSERT (parameters);
  SgFunctionRefExp* func_ref = buildFunctionRefExp(sym);
  SgFunctionCallExp * func_call_expr = new SgFunctionCallExp(func_ref,parameters,func_ref->get_type());
  func_ref->set_parent(func_call_expr);
  parameters->set_parent(func_call_expr);
  setOneSourcePositionForTransformation(func_call_expr);
  ROSE_ASSERT(func_call_expr);
  return func_call_expr;  
}

SgFunctionCallExp* 
SageBuilder::buildFunctionCallExp_nfi(SgExpression* f, SgExprListExp* parameters /*=NULL*/)
{
  SgFunctionCallExp * func_call_expr = new SgFunctionCallExp(f,parameters,f->get_type());
  if (f) f->set_parent(func_call_expr);
  if (parameters) parameters->set_parent(func_call_expr);
  setOneSourcePositionNull(func_call_expr);
  ROSE_ASSERT(func_call_expr);
  return func_call_expr;  
}

SgFunctionCallExp* 
SageBuilder::buildFunctionCallExp(SgExpression* f, SgExprListExp* parameters/*=NULL*/)
{
  SgFunctionCallExp * func_call_expr = new SgFunctionCallExp(f,parameters,f->get_type());
  if (f) f->set_parent(func_call_expr);
  if (parameters) parameters->set_parent(func_call_expr);
  setOneSourcePositionForTransformation(func_call_expr);
  ROSE_ASSERT(func_call_expr);
  return func_call_expr;  
}

SgExprStatement*
SageBuilder::buildFunctionCallStmt(const SgName& name, 
                      SgType* return_type, 
                      SgExprListExp* parameters /*= NULL*/, 
                      SgScopeStatement* scope /*=NULL*/)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  SgFunctionCallExp* func_call_expr = buildFunctionCallExp(name,return_type,parameters,scope);
  SgExprStatement * expStmt = buildExprStatement(func_call_expr);
  return expStmt;
}

//! Build a function call statement using function expression and argument list only, like (*funcPtr)(args);
SgExprStatement*
SageBuilder::buildFunctionCallStmt(SgExpression* function_exp, SgExprListExp* parameters/*=NULL*/)
{
  SgFunctionCallExp* func_call_expr = buildFunctionCallExp(function_exp, parameters);
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
 //  ROSE_ASSERT(scope != NULL); // We support bottom up building of label statements now
 
   // should including current scope when searching for the function definition
   // since users can only pass FunctionDefinition when the function body is not yet attached
  SgLabelStatement * labelstmt = new SgLabelStatement(name,stmt);
  ROSE_ASSERT(labelstmt);
  setOneSourcePositionForTransformation(labelstmt);
  
 if(stmt!=NULL) 
   stmt->set_parent(labelstmt);
 #if 0  // moved to fixLabelStatement()
  SgFunctionDefinition * label_scope = getEnclosingFunctionDefinition(scope,true);
  ROSE_ASSERT (label_scope);
  labelstmt->set_scope(label_scope);
  SgLabelSymbol* lsymbol= new SgLabelSymbol(labelstmt);
  ROSE_ASSERT(lsymbol); 
  // TODO should we prevent duplicated insertion ?
  label_scope->insert_symbol(lsymbol->get_name(), lsymbol);
 #endif 

  if (scope)
    fixLabelStatement(labelstmt,scope);
  // we don't want to set parent here yet
  // delay it until append_statement() or alike
  return labelstmt;
}

SgLabelStatement * SageBuilder::buildLabelStatement_nfi(const SgName& name,  SgStatement * stmt/*=NULL*/, SgScopeStatement* scope /*=NULL*/)
{
  SgLabelStatement * labelstmt = new SgLabelStatement(name,stmt);
  ROSE_ASSERT(labelstmt);
  setOneSourcePositionForTransformation(labelstmt);
  
 if(stmt!=NULL) 
   stmt->set_parent(labelstmt);
  if (scope)
    fixLabelStatement(labelstmt,scope);
  // we don't want to set parent here yet
  // delay it until append_statement() or alike
  return labelstmt;
}

SgIfStmt * SageBuilder::buildIfStmt(SgStatement* conditional, SgStatement * true_body, SgStatement * false_body)
{
  ROSE_ASSERT(conditional);
  ROSE_ASSERT(true_body);
  // ROSE_ASSERT(false_body); -- this is not required anymore
  SgIfStmt *ifstmt = new SgIfStmt(conditional, true_body, false_body);
  ROSE_ASSERT(ifstmt);
  setOneSourcePositionForTransformation(ifstmt);
  conditional->set_parent(ifstmt);
  true_body->set_parent(ifstmt);
  if (false_body != NULL) false_body->set_parent(ifstmt);
  return ifstmt;
}

SgIfStmt * SageBuilder::buildIfStmt_nfi(SgStatement* conditional, SgStatement * true_body, SgStatement * false_body)
{
  SgIfStmt *ifstmt = new SgIfStmt(conditional, true_body, false_body);
  ROSE_ASSERT(ifstmt);
  setOneSourcePositionNull(ifstmt);
  if (conditional) conditional->set_parent(ifstmt);
  if (true_body) true_body->set_parent(ifstmt);
  if (false_body) false_body->set_parent(ifstmt);
  return ifstmt;
}

//! Based on the contribution from Pradeep Srinivasa@ LANL
//Liao, 8/27/2008
SgForStatement * SageBuilder::buildForStatement(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body)
{
  SgForStatement * result = new SgForStatement(test,increment, loop_body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  if (test) 
    test->set_parent(result);
  if (loop_body) 
    loop_body->set_parent(result);
  if (increment) 
    increment->set_parent(result);

  // CH (5/13/2010): If the initialize_stmt is an object of SgForInitStatement, we can directly put it 
  // into for statement. Or else, there will be two semicolons after unparsing.
  if (SgForInitStatement* for_init_stmt = isSgForInitStatement(initialize_stmt))
  {
    result->set_for_init_stmt(for_init_stmt);
    for_init_stmt->set_parent(result);
    return result;
  }

  SgForInitStatement* init_stmt = new SgForInitStatement();
  ROSE_ASSERT(init_stmt);
  setOneSourcePositionForTransformation(init_stmt);
  result->set_for_init_stmt(init_stmt);   
  init_stmt->set_parent(result);

  if (initialize_stmt) 
  {
    init_stmt->append_init_stmt(initialize_stmt);
    // Support for "for (int i=0; )", Liao, 3/11/2009
    // The symbols are inserted into the symbol table attached to SgForStatement
    if (isSgVariableDeclaration(initialize_stmt))
    {
      fixVariableDeclaration(isSgVariableDeclaration(initialize_stmt),result);
      // fix varRefExp to the index variable used in increment, conditional expressions
      fixVariableReferences(result);
    }
  }

  return result;
}

//! Based on the contribution from Pradeep Srinivasa@ LANL
//Liao, 8/27/2008
SgForStatement * SageBuilder::buildForStatement_nfi(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body)
{
  SgForStatement * result = new SgForStatement(test,increment, loop_body);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (test) test->set_parent(result);
  if (loop_body) loop_body->set_parent(result);
  if (increment) increment->set_parent(result);

  if (initialize_stmt != NULL) {
    SgForInitStatement* init_stmt = result->get_for_init_stmt();
    ROSE_ASSERT(init_stmt);
    setOneSourcePositionNull(init_stmt);
    init_stmt->append_init_stmt(initialize_stmt);
    initialize_stmt->set_parent(init_stmt);
  }

  return result;
}

//! Based on the contribution from Pradeep Srinivasa@ LANL
//Liao, 8/27/2008
SgUpcForAllStatement * SageBuilder::buildUpcForAllStatement_nfi(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgExpression* affinity, SgStatement * loop_body)
{
  SgUpcForAllStatement * result = new SgUpcForAllStatement(test,increment, affinity, loop_body);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (test) test->set_parent(result);
  if (loop_body) loop_body->set_parent(result);
  if (increment) increment->set_parent(result);
  if (affinity) affinity->set_parent(result);

  if (initialize_stmt != NULL) {
    SgForInitStatement* init_stmt = result->get_for_init_stmt();
    ROSE_ASSERT(init_stmt);
    setOneSourcePositionNull(init_stmt);
    init_stmt->append_init_stmt(initialize_stmt);
    initialize_stmt->set_parent(init_stmt);
  }

  return result;
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

SgWhileStmt * SageBuilder::buildWhileStmt_nfi(SgStatement *  condition, SgStatement *body)
{
  SgWhileStmt * result = new SgWhileStmt(condition,body);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (condition) condition->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

SgDoWhileStmt * SageBuilder::buildDoWhileStmt(SgStatement *  body, SgStatement *condition)
{
  ROSE_ASSERT(condition);
  ROSE_ASSERT(body);
  SgDoWhileStmt * result = new SgDoWhileStmt(body, condition);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  condition->set_parent(result);
  body->set_parent(result);
  return result;
}

SgDoWhileStmt * SageBuilder::buildDoWhileStmt_nfi(SgStatement *  body, SgStatement *condition)
{
  SgDoWhileStmt * result = new SgDoWhileStmt(body, condition);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (condition) condition->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

SgBreakStmt * SageBuilder::buildBreakStmt()
{
  SgBreakStmt* result = new SgBreakStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgBreakStmt * SageBuilder::buildBreakStmt_nfi()
{
  SgBreakStmt* result = new SgBreakStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgContinueStmt * SageBuilder::buildContinueStmt()
{
  SgContinueStmt* result = new SgContinueStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgContinueStmt * SageBuilder::buildContinueStmt_nfi()
{
  SgContinueStmt* result = new SgContinueStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
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

//!Build SgPragma
SgPragma* SageBuilder::buildPragma(const std::string & name)
{
  SgPragma* result= new SgPragma(name);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
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

SgBasicBlock * SageBuilder::buildBasicBlock_nfi()
{
  SgBasicBlock* result = new SgBasicBlock();
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgBasicBlock* SageBuilder::buildBasicBlock_nfi(const vector<SgStatement*>& stmts) {
  SgBasicBlock* result = buildBasicBlock_nfi();
  appendStatementList(stmts, result);
  return result;
}

SgGotoStatement * 
SageBuilder::buildGotoStatement(SgLabelStatement *  label)
{
  SgGotoStatement* result = new SgGotoStatement(label);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgGotoStatement * 
SageBuilder::buildGotoStatement_nfi(SgLabelStatement *  label)
{
  SgGotoStatement* result = new SgGotoStatement(label);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

//! Build a return statement
SgReturnStmt* SageBuilder::buildReturnStmt(SgExpression* expression /* = NULL */)
{
  SgReturnStmt * result = new SgReturnStmt(expression);
  ROSE_ASSERT(result);
  if (expression != NULL) expression->set_parent(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build a return statement
SgReturnStmt* SageBuilder::buildReturnStmt_nfi(SgExpression* expression /* = NULL */)
{
  SgReturnStmt * result = new SgReturnStmt(expression);
  ROSE_ASSERT(result);
  if (expression != NULL) expression->set_parent(result);
  setOneSourcePositionNull(result);
  return result;
}

SgCaseOptionStmt * SageBuilder::buildCaseOptionStmt( SgExpression * key,SgStatement *body)
{
  SgCaseOptionStmt* result = new SgCaseOptionStmt(key,body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  if (key) key->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

SgCaseOptionStmt * SageBuilder::buildCaseOptionStmt_nfi( SgExpression * key,SgStatement *body)
{
  SgCaseOptionStmt* result = new SgCaseOptionStmt(key,body);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (key) key->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

SgDefaultOptionStmt * SageBuilder::buildDefaultOptionStmt( SgStatement *body)
{
  SgDefaultOptionStmt* result = new SgDefaultOptionStmt(body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  if (body) body->set_parent(result);
  return result;
}

SgDefaultOptionStmt * SageBuilder::buildDefaultOptionStmt_nfi( SgStatement *body)
{
  SgDefaultOptionStmt* result = new SgDefaultOptionStmt(body);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (body) body->set_parent(result);
  return result;
}

SgSwitchStatement* SageBuilder::buildSwitchStatement(SgStatement *item_selector,SgStatement *body)
{
  SgSwitchStatement* result = new SgSwitchStatement(item_selector,body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  if (item_selector) item_selector->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

SgSwitchStatement* SageBuilder::buildSwitchStatement_nfi(SgStatement *item_selector,SgStatement *body)
{
  SgSwitchStatement* result = new SgSwitchStatement(item_selector,body);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (item_selector) item_selector->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

//! Build a NULL statement
SgNullStatement* SageBuilder::buildNullStatement()
{
  SgNullStatement* result = NULL;
  result = new SgNullStatement();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build a NULL statement
SgNullStatement* SageBuilder::buildNullStatement_nfi()
{
  SgNullStatement* result = NULL;
  result = new SgNullStatement();
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

// DQ (4/30/2010): Added support for building asm statements.
//! Build an asm statement
SgAsmStmt* SageBuilder::buildAsmStatement( std::string s )
{
  SgAsmStmt* result = NULL;
  result = new SgAsmStmt();
  ROSE_ASSERT(result);
  result->set_assemblyCode(s);
  setOneSourcePositionForTransformation(result);
  return result;
}

// DQ (4/30/2010): Added support for building asm statements.
//! Build an asm statement
SgAsmStmt* SageBuilder::buildAsmStatement_nfi( std::string s )
{
  SgAsmStmt* result = NULL;
  result = new SgAsmStmt();
  ROSE_ASSERT(result);
  result->set_assemblyCode(s);
  setOneSourcePositionNull(result);
  return result;
}

SgAsmStmt*
SageBuilder::buildMultibyteNopStatement( int n )
   {
// Multi-byte NOP instructions.
// Note: I can't seem to get the memonic versions to work properly
#define NOP_1_BYTE_STRING "nop"
#define NOP_2_BYTE_STRING ".byte 0x66,0x90"
#define NOP_3_BYTE_STRING "nopl (%eax)"
#define NOP_4_BYTE_STRING "nopl 0x01(%eax)"
#define NOP_5_BYTE_STRING ".byte 0x0f,0x1f,0x44,0x00,0x00"
#define NOP_6_BYTE_STRING ".byte 0x66,0x0f,0x1f,0x44,0x00,0x00"
#define NOP_7_BYTE_STRING ".byte 0x0f,0x1f,0x80,0x00,0x00,0x00,0x00"
#define NOP_8_BYTE_STRING ".byte 0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x00"
#define NOP_9_BYTE_STRING ".byte 0x66,0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x00"

     ROSE_ASSERT(n > 0);

     SgAsmStmt* nopStatement = NULL;

     switch (n)
        {
          case 1: nopStatement = buildAsmStatement(NOP_1_BYTE_STRING); break;
          case 2: nopStatement = buildAsmStatement(NOP_2_BYTE_STRING); break;
          case 3: nopStatement = buildAsmStatement(NOP_3_BYTE_STRING); break;
          case 4: nopStatement = buildAsmStatement(NOP_4_BYTE_STRING); break;
          case 5: nopStatement = buildAsmStatement(NOP_5_BYTE_STRING); break;
          case 6: nopStatement = buildAsmStatement(NOP_6_BYTE_STRING); break;
          case 7: nopStatement = buildAsmStatement(NOP_7_BYTE_STRING); break;
          case 8: nopStatement = buildAsmStatement(NOP_8_BYTE_STRING); break;
          case 9: nopStatement = buildAsmStatement(NOP_9_BYTE_STRING); break;

          default:
             {
               printf ("Only supporting values of multi-byte nop's up to 9 bytes long. \n");
               ROSE_ASSERT(false);
             }
        }

     return nopStatement;
   }



//! Build a statement from an arbitrary string, used for irregular statements with macros, platform-specified attributes etc.
// This does not work properly since the global scope expects declaration statement, not just SgNullStatement
#if 0    
SgStatement* SageBuilder::buildStatementFromString(std::string str)
{ 
  SgStatement* result = NULL;
  
    return result;
     
} //buildStatementFromString()
#endif

SgPointerType* SageBuilder::buildPointerType(SgType * base_type /*= NULL*/)
   {
  // DQ (7/26/2010): This needs to call the SgPointerType::createType() function so that we can properly abstract the creation of types into the type table.
  // printf ("ERROR: This function needs to call the SgPointerType::createType() function so that we can properly abstract the creation of types into the type table. \n");
  // ROSE_ASSERT(false);

  // DQ (7/29/2010): This function needs to call the SgPointerType::createType() function to support the new type table.
  // SgPointerType* result = new SgPointerType(base_type);
     SgPointerType* result = SgPointerType::createType(base_type);
     ROSE_ASSERT(result != NULL);

     return result;
   }

SgReferenceType* SageBuilder::buildReferenceType(SgType * base_type /*= NULL*/)
   {
  // DQ (7/26/2010): This needs to call the SgReferenceType::createType() function so that we can properly abstract the creation of types into the type table.
  // printf ("ERROR: This function needs to call the SgReferenceType::createType() function so that we can properly abstract the creation of types into the type table. \n");
  // ROSE_ASSERT(false);

  // DQ (7/29/2010): This function needs to call the SgPointerType::createType() function to support the new type table.
  // SgReferenceType* result= new SgReferenceType(base_type);
     SgReferenceType* result = SgReferenceType::createType(base_type);
     ROSE_ASSERT(result != NULL);

     return result;
   }

SgModifierType* SageBuilder::buildModifierType(SgType * base_type /*= NULL*/)
   {
  // DQ (7/30/2010): Note that this is called by the outline test: tests/roseTests/astOutliningTests/moreTest3.cpp
  // DQ (7/28/2010): Now we want to make calling this function an error, the functions buildConst() will return SgModifierType objects instead.
     printf ("Error: this function SageBuilder::buildModifierType() should not be called! (call the buildConst() function (or whatever other function is required) directly \n");
  // ROSE_ASSERT(false);

  // DQ (7/26/2010): This needs to call the SgModifierType::createType() function so that we can properly abstract the creation of types into the type table.
     SgModifierType* result = new SgModifierType(base_type);
  // SgModifierType* result = SgModifierType::createType(base_type);
     ROSE_ASSERT(result != NULL);

  // DQ (7/28/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

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

SgTypeSignedShort * SageBuilder::buildSignedShortType() 
{ 
  SgTypeSignedShort * result =SgTypeSignedShort::createType(); 
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

SgTypeSignedLongLong * SageBuilder::buildSignedLongLongType() 
{ 
  SgTypeSignedLongLong * result =SgTypeSignedLongLong::createType(); 
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

SgTypeUnknown * SageBuilder::buildUnknownType() 
{ 
  SgTypeUnknown * result =SgTypeUnknown::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeShort * SageBuilder::buildShortType() 
{ 
  SgTypeShort * result =SgTypeShort::createType(); 
  ROSE_ASSERT(result); 
  return result;
}

SgTypeUnsignedShort * SageBuilder::buildUnsignedShortType()
{
  SgTypeUnsignedShort * result = SgTypeUnsignedShort::createType();
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

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a constant type.
SgModifierType* SageBuilder::buildConstType(SgType* base_type /*=NULL*/)
   {
#if 0
  // DQ (7/28/2010): Old approach before type table support.
     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);
     result->get_typeModifier().get_constVolatileModifier().setConst();
     return result;
#else
  // DQ (7/28/2010): New (similar) approach using type table support.
     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);
     result->get_typeModifier().get_constVolatileModifier().setConst();

  // DQ (7/28/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
#endif
 }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a volatile type.
SgModifierType* SageBuilder::buildVolatileType(SgType* base_type /*=NULL*/)
   {
     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_constVolatileModifier().setVolatile();

  // DQ (7/29/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a restrict type.
SgModifierType* SageBuilder::buildRestrictType(SgType* base_type)
   {
     ROSE_ASSERT(base_type!=NULL);
     if (!isSgPointerType(base_type) && !isSgReferenceType(base_type))
        {
          printf("Base type of restrict type must be a pointer or reference type.\n");
          ROSE_ASSERT(false);
        }
     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().setRestrict();

  // DQ (7/29/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC strict type.
SgModifierType* SageBuilder::buildUpcStrictType(SgType* base_type /*=NULL*/)
   {
     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_modifier(SgUPC_AccessModifier::e_upc_strict);

  // DQ (7/29/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC relaxed type.
SgModifierType* SageBuilder::buildUpcRelaxedType(SgType* base_type /*=NULL*/)
   {
     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_modifier(SgUPC_AccessModifier::e_upc_relaxed);

  // DQ (7/29/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC shared type.
SgModifierType* SageBuilder::buildUpcSharedType(SgType* base_type /*=NULL*/, long layout /*= -1*/)
   {
     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_isShared(true);

  // DQ (7/29/2010): Modified to use new input parameter.
  // result->get_typeModifier().get_upcModifier().set_layout(-1); // No layout ("shared" without a block size)
     result->get_typeModifier().get_upcModifier().set_layout(layout); // No layout ("shared" without a block size)

  // DQ (7/29/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC shared[] type.
SgModifierType* SageBuilder::buildUpcBlockIndefiniteType(SgType* base_type /*=NULL*/)
   {
     SgModifierType *result = isSgModifierType(buildUpcSharedType(base_type));
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_layout(0); // [] layout

  // DQ (7/29/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC shared[*] type.
SgModifierType* SageBuilder::buildUpcBlockStarType(SgType* base_type /*=NULL*/)
   {
     SgModifierType *result = isSgModifierType(buildUpcSharedType(base_type));
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_layout(-2); // [*] layout

  // DQ (7/29/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC shared[n] type.
SgModifierType* SageBuilder::buildUpcBlockNumberType(SgType* base_type, long block_factor)
   {
     SgModifierType *result = isSgModifierType(buildUpcSharedType(base_type));
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_layout(block_factor); // [block_factor] layout

  // DQ (7/29/2010): Insert result type into type table and return it, or 
  // replace the result type, if already available in the type table, with 
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }



  //! Build a complex type.
SgTypeComplex* SageBuilder::buildComplexType(SgType* base_type /*=NULL*/)
 {
   SgTypeComplex *result = new SgTypeComplex(base_type);
   ROSE_ASSERT(result!=NULL);
   return result;
 }

  //! Build an imaginary type.
SgTypeImaginary* SageBuilder::buildImaginaryType(SgType* base_type /*=NULL*/)
 {
   SgTypeImaginary *result = new SgTypeImaginary(base_type);
   ROSE_ASSERT(result!=NULL);
   return result;
 }

SgNamespaceDefinitionStatement* SageBuilder::buildNamespaceDefinition(SgNamespaceDeclarationStatement* d)
  {
    SgNamespaceDefinitionStatement* result = NULL;
    if (d!=NULL) // the constructor does not check for NULL d, causing segmentation fault
    {
      result = new SgNamespaceDefinitionStatement(d);
      result->set_parent(d); // set_declaration() == set_parent() in this case
    }
    else
      result = new SgNamespaceDefinitionStatement(d);
    
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }



SgClassDefinition* SageBuilder::buildClassDefinition(SgClassDeclaration *d/*= NULL*/)
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

SgClassDefinition* SageBuilder::buildClassDefinition_nfi(SgClassDeclaration *d/*= NULL*/)
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
    setOneSourcePositionNull(result);
    return result;
  }

SgClassDeclaration* SageBuilder::buildNondefiningClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope)
   {
#define REUSE_CLASS_DECLARATION_FROM_SYMBOL 0

#if (REUSE_CLASS_DECLARATION_FROM_SYMBOL == 0)
     SgClassDeclaration* nondefdecl = new SgClassDeclaration(name,kind,NULL,NULL);
     ROSE_ASSERT(nondefdecl != NULL);
     //Liao, we ask for explicit creation of SgClassType to avoid duplicated type nodes
     if (nondefdecl->get_type() == NULL)
       nondefdecl->set_type(SgClassType::createType(nondefdecl));

     printf ("SageBuilder::buildNondefiningClassDeclaration_nfi(): (and setting source position) nondefdecl = %p \n",nondefdecl);

  // The non-defining declaration asociated with a declaration does not have a 
  // source position...unless it is the position of the defining declaration.
  // setOneSourcePositionNull(nondefdecl);
     setSourcePosition(nondefdecl);

  // This is find for now, but a little later in this function (if we can find a symbol) 
  // we want to find the first non-defining declaration (using the symbol table) and use 
  // that as a paramter to "nondefdecl->set_firstNondefiningDeclaration()".
     nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
     nondefdecl->set_definingDeclaration(NULL);
     nondefdecl->setForward();

  // This is the structural parent (the logical scope can be different than the parent).
  // TPS (09/18/2009) added a condition to be able to build this properly
     if (scope==NULL)
       nondefdecl->set_parent(topScopeStack());
     else
       nondefdecl->set_parent(scope);

  // This is the logical scope...
     nondefdecl->set_scope(scope);

     ROSE_ASSERT(nondefdecl->get_parent() != NULL);
#else
     SgClassDeclaration* nondefdecl = NULL;
#endif

     SgClassDeclaration* firstNondefdecl = NULL;
     if (scope != NULL)
        {
#if 0
          SgClassSymbol* mysymbol = new SgClassSymbol(nondefdecl);
          ROSE_ASSERT(mysymbol != NULL);

       // printf ("In SageBuilder::buildNondefiningClassDeclaration(): for nondefdecl = %p built SgClassSymbol = %p \n",nondefdecl,mysymbol);

          scope->insert_symbol(name, mysymbol);
#else
       // Reuse any previously defined symbols (to avoid redundant symbols in the symbol table) 
       // and find the firstNondefiningDeclaration.
          SgClassSymbol* mysymbol = scope->lookup_class_symbol(name);
          if (mysymbol != NULL)
             {
               firstNondefdecl = isSgClassDeclaration(mysymbol->get_declaration());
               ROSE_ASSERT(firstNondefdecl != NULL);

#if (REUSE_CLASS_DECLARATION_FROM_SYMBOL == 0)
               ROSE_ASSERT(nondefdecl != NULL);
               ROSE_ASSERT(nondefdecl->get_parent() != NULL);

               nondefdecl->set_firstNondefiningDeclaration(firstNondefdecl);

            // This might be NULL if the defining declaration has not been seen yet!
               nondefdecl->set_definingDeclaration(firstNondefdecl->get_definingDeclaration());

            // Share the type!
               if (nondefdecl->get_type() != firstNondefdecl->get_type())
                  {
                 // Remove the type from the new SgClassDeclaration and set the reference to the type in the firstNondefiningDeclaration.
                    delete nondefdecl->get_type();
                    nondefdecl->set_type(firstNondefdecl->get_type());
                  }
#else
               ROSE_ASSERT(nondefdecl == NULL);
#endif
            // This function should return a new nondefining declaration each time (to support multile class prototypes!).
            // nondefdecl = firstNondefdecl;
             }
            else
             {
#if REUSE_CLASS_DECLARATION_FROM_SYMBOL
            // DQ (1/25/2009): We only want to build a new declaration if we can't reuse the existing declaration.
               nondefdecl = new SgClassDeclaration(name,kind,NULL,NULL);
               ROSE_ASSERT(nondefdecl != NULL);
               if (nondefdecl->get_type() == NULL)
                 nondefdel->set_type(SgClassType::createType(nondefdecl));

               printf ("SageBuilder::buildNondefiningClassDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

               setOneSourcePositionNull(nondefdecl);

               nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
               nondefdecl->set_definingDeclaration(NULL);
               nondefdecl->setForward();
#endif
               mysymbol = new SgClassSymbol(nondefdecl);
               firstNondefdecl = nondefdecl;

               scope->insert_symbol(name, mysymbol);
             }

          ROSE_ASSERT(mysymbol != NULL);
          ROSE_ASSERT(firstNondefdecl != NULL);
#endif
          nondefdecl->set_scope(scope);

       // DQ (1/25/2009): The scope is not the same as the parent, since the scope is logical, and the parent is structural (note that topScopeStack() is structural).
       // TPS (09/18/2009) added a condition to be able to build this properly
	  if (scope==NULL)
	    nondefdecl->set_parent(topScopeStack());
	  else
	    nondefdecl->set_parent(scope);
        }

  // The support for SgEnumDeclaration handles the type, but why not for SgClassDeclaration?
     ROSE_ASSERT(nondefdecl->get_type() != NULL);

     ROSE_ASSERT(nondefdecl->get_parent() != NULL);

     return nondefdecl;
   }

SgEnumDeclaration* SageBuilder::buildNondefiningEnumDeclaration_nfi(const SgName& name, SgScopeStatement* scope)
   {
  // The support for SgEnumDeclaration is identical to that for SgClassDeclaration (excpet for the type handleing, why is that?).

     SgEnumDeclaration* nondefdecl = new SgEnumDeclaration(name, NULL);
     ROSE_ASSERT(nondefdecl);
     setOneSourcePositionNull(nondefdecl);
     nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
     nondefdecl->set_definingDeclaration(NULL);

  // Any non-defining declaration is not always a forward declaration.
     nondefdecl->setForward();    

     if (scope != NULL)
        {
          SgEnumSymbol* mysymbol = new SgEnumSymbol(nondefdecl);
          ROSE_ASSERT(mysymbol);
          scope->insert_symbol(name, mysymbol);
          nondefdecl->set_scope(scope);

       // Can this be defined in C++ so that it is in a logical scope different from its structural scope?
          nondefdecl->set_parent(scope);
        }

  // DQ (1/25/2009): I want to check into this later, since it is not symetric with SageBuilder::buildNondefiningClassDeclaration()
     printf ("Need to check if SgEnumDeclaration constructor builds the SgEnumType already nondefdecl->get_type() = %p \n",nondefdecl->get_type());
     ROSE_ASSERT(nondefdecl->get_type() != NULL);

     SgEnumType* t = new SgEnumType(nondefdecl);
     nondefdecl->set_type(t);

     return nondefdecl;
   }

// This should take a SgClassDeclaration::class_types kind parameter!
SgClassDeclaration * SageBuilder::buildStructDeclaration(const SgName& name, SgScopeStatement* scope /*=NULL*/)
   {
#if 0
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

  // TODO How about class type??
  // build defining declaration
     SgClassDefinition* classDef = buildClassDefinition();
   
     SgClassDeclaration* defdecl = new SgClassDeclaration (name,SgClassDeclaration::e_struct,NULL,classDef);
     ROSE_ASSERT(defdecl);
     setOneSourcePositionForTransformation(defdecl);
  // constructor is side-effect free
     classDef->set_declaration(defdecl);
     defdecl->set_definingDeclaration(defdecl);

  // build the nondefining declaration
     SgClassDeclaration* nondefdecl = new SgClassDeclaration (name,SgClassDeclaration::e_struct,NULL,NULL);
     ROSE_ASSERT(nondefdecl);

     setOneSourcePositionForTransformation(nondefdecl);
     nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
     nondefdecl->set_definingDeclaration(defdecl);
     defdecl->set_firstNondefiningDeclaration(nondefdecl);
     nondefdecl->setForward();

     if (scope !=NULL )  // put into fixStructDeclaration() or alike later on
        {
          fixStructDeclaration(nondefdecl,scope);
          fixStructDeclaration(defdecl,scope);
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
#else
  // DQ (1/24/2009): Refactored to use the buildStructDeclaration_nfi function.
  // (if this work it needs to be done uniformally for the other nfi functions)
  // Also, "_nfi" is not a great name.
     SgClassDeclaration* defdecl = buildClassDeclaration_nfi(name,SgClassDeclaration::e_struct,scope,NULL);

     setOneSourcePositionForTransformation(defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != NULL);
     setOneSourcePositionForTransformation(defdecl->get_firstNondefiningDeclaration());
#endif

  // DQ (1/26/2009): I think this should be an error, but that appears it would
  // break the existing interface. Need to discuss this with Liao.
  // ROSE_ASSERT(defdecl->get_parent() != NULL);

     return defdecl;
   }


SgNamespaceDeclarationStatement * SageBuilder::buildNamespaceDeclaration(const SgName& name, SgScopeStatement* scope /*=NULL*/)
   {
     SgNamespaceDeclarationStatement* defdecl = buildNamespaceDeclaration_nfi(name,false,scope);

     setOneSourcePositionForTransformation(defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != NULL);
     setOneSourcePositionForTransformation(defdecl->get_firstNondefiningDeclaration());

     return defdecl;
   }

SgNamespaceDeclarationStatement * SageBuilder::buildNamespaceDeclaration_nfi(const SgName& name, bool unnamednamespace, SgScopeStatement* scope)
   {
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

  // TODO How about class type??
  // build defining declaration
     SgNamespaceDefinitionStatement* classDef = buildNamespaceDefinition(); //buildClassDefinition
     

#if 1
     SgNamespaceDeclarationStatement* defdecl = new SgNamespaceDeclarationStatement(name,classDef, unnamednamespace);
     ROSE_ASSERT(defdecl != NULL);
     classDef->set_parent(defdecl);
     printf ("SageBuilder::buildNamespaceDeclaration_nfi(): defdecl = %p \n",defdecl);

     setOneSourcePositionForTransformation(defdecl);
  // constructor is side-effect free
     classDef->set_namespaceDeclaration(defdecl);
     defdecl->set_definingDeclaration(defdecl);
#endif


  // Get the nondefining declaration from the symbol if it has been built (if this works, 
  // then we likely don't need the "SgClassDeclaration* nonDefiningDecl" parameter).
     SgNamespaceDeclarationStatement* nondefdecl = NULL;

  // DQ (1/26/2009): It seems that (scope == NULL) can happen in the tests/roseTests/astInterfaceTests test codes.
  // ROSE_ASSERT(scope != NULL);
     SgNamespaceSymbol* mysymbol = NULL;
     if (scope != NULL)
        {
          mysymbol = scope->lookup_namespace_symbol(name);
        }
       else
        {
       // DQ (1/26/2009): I think this should be an error, but that appears it would
       // break the existing interface. Need to discuss this with Liao.
          printf ("Warning: In SageBuilder::buildNamespaceDeclaration_nfi(): scope == NULL \n");
        }

     printf ("In SageBuilder::buildNamespaceDeclaration_nfi(): mysymbol = %p \n",mysymbol);
     if (mysymbol != NULL)
        {
          nondefdecl = isSgNamespaceDeclarationStatement(mysymbol->get_declaration());

          ROSE_ASSERT(nondefdecl != NULL);
          ROSE_ASSERT(nondefdecl->get_parent() != NULL);

          nondefdecl->set_definingDeclaration(defdecl);

          ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);
        }
       else
        {
       // DQ (1/25/2009): We only want to build a new declaration if we can't reuse the existing declaration.
          nondefdecl = new SgNamespaceDeclarationStatement(name,NULL, unnamednamespace);
          ROSE_ASSERT(nondefdecl != NULL);

          printf ("SageBuilder::buildNamespaceDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

       // The nondefining declaration will not appear in the source code, but is compiler
       // generated (so we have something about the class that we can reference; e.g in
       // types).  At the moment we make it a transformation, there might be another kind 
       // of source position that would be more precise.  FIXME.
       // setOneSourcePositionNull(nondefdecl);
          setOneSourcePositionForTransformation(nondefdecl);

          nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
          nondefdecl->set_definingDeclaration(defdecl);
          nondefdecl->setForward();


          //nondefdecl->set_parent(topScopeStack());
          nondefdecl->set_parent(scope);
		  ROSE_ASSERT(nondefdecl->get_parent());

          if (scope != NULL)
             {
               mysymbol = new SgNamespaceSymbol(name,nondefdecl); // tps: added name to constructor
               scope->insert_symbol(name, mysymbol);
             }
            else
             {
            // DQ (1/26/2009): I think this should be an error, but that appears it would
            // break the existing interface. Need to discuss this with Liao.
               printf ("Warning: no scope provided to support symbol table entry! \n");
             }
        }


     printf ("SageBuilder::buildNamespaceDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

  // setOneSourcePositionForTransformation(nondefdecl);
     setOneSourcePositionNull(nondefdecl);

  // nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
  // nondefdecl->set_definingDeclaration(defdecl);
     defdecl->set_firstNondefiningDeclaration(nondefdecl);

  // I don't think this is always a forward declaration (e.g. if it is not used in a prototype).
  // Checking the olded EDG/ROSE interface it appears that it is always marked forward (unless 
  // used in a defining declaration).
     nondefdecl->setForward();

     if (scope != NULL)  // put into fixStructDeclaration() or alike later on
        {
          fixNamespaceDeclaration(nondefdecl,scope);
          fixNamespaceDeclaration(defdecl,scope);
#if 0
          SgClassSymbol* mysymbol = new SgClassSymbol(nondefdecl);
          ROSE_ASSERT(mysymbol);
          scope->insert_symbol(name, mysymbol);
#endif
          printf ("@@@@@@@@@@@@@@ In buildNamespaceDeclaration_nfi(): setting scope of defining and non-defining declaration to scope = %s \n",scope->class_name().c_str());

          // tps namespace has no scope
          //defdecl->set_scope(scope);
          //nondefdecl->set_scope(scope);

       // defdecl->set_parent(scope);

       // DQ (1/25/2009): The scope is not the same as the parent, since the scope is logical, and the parent is structural (note that topScopeStack() is structural).
       // nondefdecl->set_parent(scope);
        //  nondefdecl->set_parent(topScopeStack());
        }

  //   defdecl->set_parent(topScopeStack());

  // DQ (1/26/2009): I think we should assert this, but it breaks the interface as defined
  // by the test code in tests/roseTests/astInterfaceTests.
  // ROSE_ASSERT(defdecl->get_parent() != NULL);

  // ROSE_ASSERT(nonDefiningDecl->get_parent() != NULL);

     ROSE_ASSERT(defdecl->get_definingDeclaration() == defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != defdecl->get_definingDeclaration());


     return defdecl;    
   }


// DQ (11/7/2009): Added more uniform support for building class declarations.
SgClassDeclaration*
SageBuilder::buildNondefiningClassDeclaration ( SgName name, SgScopeStatement* scope )
   {
     SgClassDeclaration* defdecl    = NULL;
     SgClassDeclaration* nondefdecl = NULL;

  // DQ (1/26/2009): It seems that (scope == NULL) can happen in the tests/roseTests/astInterfaceTests test codes.
  // ROSE_ASSERT(scope != NULL);
     SgClassSymbol* mysymbol = NULL;
     if (scope != NULL)
        {
          mysymbol = scope->lookup_class_symbol(name);
        }
       else
        {
       // Liao 9/2/2009: This is not an error. We support bottomup AST construction and scope can be unkown.   
       // DQ (1/26/2009): I think this should be an error, but that appears it would
       // break the existing interface. Need to discuss this with Liao.
       // printf ("Warning: In SageBuilder::buildClassDeclaration_nfi(): scope == NULL \n");
        }

  // printf ("In SageBuilder::buildClassDeclaration_nfi(): mysymbol = %p \n",mysymbol);
     if (mysymbol != NULL) // set links if nondefining declaration already exists.
        {
          nondefdecl = isSgClassDeclaration(mysymbol->get_declaration());

          ROSE_ASSERT(nondefdecl != NULL);
          ROSE_ASSERT(nondefdecl->get_parent() != NULL);

          nondefdecl->set_definingDeclaration(defdecl);

          ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);
        }
       else // build a nondefnining declaration if it does not exist
        {
       // DQ (1/25/2009): We only want to build a new declaration if we can't reuse the existing declaration.

          SgClassDeclaration::class_types kind = SgClassDeclaration::e_class;
          nondefdecl = new SgClassDeclaration(name,kind,NULL,NULL);
          ROSE_ASSERT(nondefdecl != NULL);
          if (nondefdecl->get_type() == NULL)
            nondefdecl->set_type(SgClassType::createType(nondefdecl));

 //         printf ("SageBuilder::buildClassDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

       // The nondefining declaration will not appear in the source code, but is compiler
       // generated (so we have something about the class that we can reference; e.g in
       // types).  At the moment we make it a transformation, there might be another kind 
       // of source position that would be more precise.  FIXME.
       // setOneSourcePositionNull(nondefdecl);
          setOneSourcePositionForTransformation(nondefdecl);

          nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
          nondefdecl->set_definingDeclaration(defdecl);
          nondefdecl->setForward();
       // Liao, 9/2/2009. scope stack is optional, it can be empty
      //    nondefdecl->set_parent(topScopeStack());
          nondefdecl->set_parent(scope);

          if (scope != NULL)
             {
               mysymbol = new SgClassSymbol(nondefdecl);
               scope->insert_symbol(name, mysymbol);
             }
            else
             {
            // Liao 9/2/2009: This is not an error. We support bottomup AST construction and scope can be unknown.
            // DQ (1/26/2009): I think this should be an error, but that appears it would
            // break the existing interface. Need to discuss this with Liao.
            //   printf ("Warning: no scope provided to support symbol table entry! \n");
             }
        }

     ROSE_ASSERT(nondefdecl != NULL);

     return nondefdecl;
   }

// DQ (11/7/2009): Added more uniform support for building class declarations.
SgClassDeclaration*
SageBuilder::buildDefiningClassDeclaration ( SgName name, SgScopeStatement* scope )
   {
     SgClassDeclaration* nondefiningClassDeclaration = buildNondefiningClassDeclaration(name,scope);
     ROSE_ASSERT(nondefiningClassDeclaration != NULL);

     SgClassDefinition* definingClassDefinition = buildClassDefinition();
     ROSE_ASSERT(definingClassDefinition != NULL);

     SgClassDeclaration::class_types kind = SgClassDeclaration::e_class;
     SgClassDeclaration* definingClassDeclaration = new SgClassDeclaration (name,kind,NULL,definingClassDefinition);
     ROSE_ASSERT(definingClassDeclaration != NULL);

     printf ("SageBuilder::buildDefiningClassDeclaration(): definingClassDeclaration = %p \n",definingClassDeclaration);

     setOneSourcePositionForTransformation(definingClassDeclaration);

  // constructor is side-effect free
     definingClassDefinition->set_declaration(definingClassDeclaration);
     definingClassDeclaration->set_definingDeclaration(definingClassDeclaration);
     definingClassDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);

     nondefiningClassDeclaration->set_definingDeclaration(definingClassDeclaration);

  // some error checking
     ROSE_ASSERT(nondefiningClassDeclaration->get_definingDeclaration() != NULL);
     ROSE_ASSERT(nondefiningClassDeclaration->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(definingClassDeclaration->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(definingClassDeclaration->get_definition() != NULL);

     ROSE_ASSERT(definingClassDeclaration->get_definition()->get_parent() != NULL);

     return definingClassDeclaration;
   }

// DQ (11/7/2009): Added more uniform support for building class declarations.
SgClassDeclaration*
SageBuilder::buildClassDeclaration ( SgName name, SgScopeStatement* scope )
   {
     ROSE_ASSERT(scope != NULL);
     SgClassDeclaration* definingClassDeclaration = buildDefiningClassDeclaration(name,scope);
     ROSE_ASSERT(definingClassDeclaration != NULL);

     return definingClassDeclaration;
   }

// DQ (1/24/2009): Built this "nfi" version but factored the code.
SgClassDeclaration * SageBuilder::buildClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl )
   {
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

  // step 1. Build defining declaration
     SgClassDefinition* classDef = buildClassDefinition();

#if 1
     SgClassDeclaration* defdecl = new SgClassDeclaration (name,kind,NULL,classDef);
     ROSE_ASSERT(defdecl != NULL);

//     printf ("SageBuilder::buildClassDeclaration_nfi(): defdecl = %p \n",defdecl);

     setOneSourcePositionForTransformation(defdecl);
  // constructor is side-effect free
     classDef->set_declaration(defdecl);
     defdecl->set_definingDeclaration(defdecl);
#endif

  // Step 2. build the nondefining declaration, 
  // but only if the input nonDefiningDecl pointer was NULL and it does not exist
#if 0
  // SgClassDeclaration* nondefdecl = new SgClassDeclaration (name,kind,NULL,NULL);
     SgClassDeclaration* nondefdecl = (nonDefiningDecl != NULL) ? nonDefiningDecl : new SgClassDeclaration (name,kind,NULL,NULL);
     ROSE_ASSERT(nondefdecl != NULL);
#else

#if 0
  // DQ (11/7/2009): Refactored the code to build a non-defining declaration
  // DQ (11/10/2009): This refactoring of the code below breaks a number of tests in tests/roseTests/astInterfaceTests.
  // TODO: Refactor this after we pass all of our tests.
     SgClassDeclaration* nondefdecl = buildNondefiningClassDeclaration (name,scope);
#else
  // Get the nondefining declaration from the symbol if it has been built (if this works, 
  // then we likely don't need the "SgClassDeclaration* nonDefiningDecl" parameter).
     SgClassDeclaration* nondefdecl = NULL;

  // DQ (1/26/2009): It seems that (scope == NULL) can happen in the tests/roseTests/astInterfaceTests test codes.
  // ROSE_ASSERT(scope != NULL);
     SgClassSymbol* mysymbol = NULL;
     if (scope != NULL)
        {
          mysymbol = scope->lookup_class_symbol(name);
        }
       else
        {
       // Liao 9/2/2009: This is not an error. We support bottomup AST construction and scope can be unkown.   
       // DQ (1/26/2009): I think this should be an error, but that appears it would
       // break the existing interface. Need to discuss this with Liao.
       // printf ("Warning: In SageBuilder::buildClassDeclaration_nfi(): scope == NULL \n");
        }

  //   printf ("In SageBuilder::buildClassDeclaration_nfi(): mysymbol = %p \n",mysymbol);
     if (mysymbol != NULL) // set links if nondefining declaration already exists.
        {
          nondefdecl = isSgClassDeclaration(mysymbol->get_declaration());

          ROSE_ASSERT(nondefdecl != NULL);
          ROSE_ASSERT(nondefdecl->get_parent() != NULL);

          nondefdecl->set_definingDeclaration(defdecl);

          ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);
        }
       else // build a nondefnining declaration if it does not exist
        {
       // DQ (1/25/2009): We only want to build a new declaration if we can't reuse the existing declaration.
          nondefdecl = new SgClassDeclaration(name,kind,NULL,NULL);
          ROSE_ASSERT(nondefdecl != NULL);
          if (nondefdecl->get_type() == NULL)
            nondefdecl->set_type(SgClassType::createType(nondefdecl));

 //         printf ("SageBuilder::buildClassDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

       // The nondefining declaration will not appear in the source code, but is compiler
       // generated (so we have something about the class that we can reference; e.g in
       // types).  At the moment we make it a transformation, there might be another kind 
       // of source position that would be more precise.  FIXME.
       // setOneSourcePositionNull(nondefdecl);
          setOneSourcePositionForTransformation(nondefdecl);

          nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
          nondefdecl->set_definingDeclaration(defdecl);
          nondefdecl->setForward();
       // Liao, 9/2/2009. scope stack is optional, it can be empty
      //    nondefdecl->set_parent(topScopeStack());
          nondefdecl->set_parent(scope);

          if (scope != NULL)
             {
               mysymbol = new SgClassSymbol(nondefdecl);
               scope->insert_symbol(name, mysymbol);
             }
            else
             {
       // Liao 9/2/2009: This is not an error. We support bottomup AST construction and scope can be unkown.   
            // DQ (1/26/2009): I think this should be an error, but that appears it would
            // break the existing interface. Need to discuss this with Liao.
            //   printf ("Warning: no scope provided to support symbol table entry! \n");
             }
        }
#endif
#endif

//     printf ("SageBuilder::buildClassDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

  // setOneSourcePositionForTransformation(nondefdecl);
     setOneSourcePositionNull(nondefdecl);

  // nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
  // nondefdecl->set_definingDeclaration(defdecl);
     defdecl->set_firstNondefiningDeclaration(nondefdecl);

     // Liao, 10/30/2009
     // The SgClassDeclaration constructor will automatically generate a SgClassType internally if NULL is passed for SgClassType
     // This is not desired when building a defining declaration and an inefficience in the constructor
     // Ideally, only the first nondefining class declaration should have a dedicated SgClassType and 
     // the defining class declaration (and other nondefining declaration) just shared that SgClassType.
     if (defdecl->get_type () != NULL) 
     {
       // if a defining class declaration's type is associated with a defining class.
       // This is a wrong SgClassType and has to be reset
       if (defdecl->get_type()->get_declaration() == isSgDeclarationStatement(defdecl) )
       {
         delete (defdecl->get_type ());
       }
     }
     // patch up the SgClassType for the defining class declaration
     ROSE_ASSERT (nondefdecl->get_type() != NULL); 
     ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl)); 
     defdecl->set_type(nondefdecl->get_type()); 

  // I don't think this is always a forward declaration (e.g. if it is not used in a prototype).
  // Checking the olded EDG/ROSE interface it appears that it is always marked forward (unless 
  // used in a defining declaration).
     nondefdecl->setForward();

     if (scope != NULL)  // put into fixStructDeclaration() or alike later on
        {
          fixStructDeclaration(defdecl,scope);
          fixStructDeclaration(nondefdecl,scope);
#if 0
          SgClassSymbol* mysymbol = new SgClassSymbol(nondefdecl);
          ROSE_ASSERT(mysymbol);
          scope->insert_symbol(name, mysymbol);
          printf ("@@@@@@@@@@@@@@ In buildClassDeclaration_nfi(): setting scope of defining and non-defining declaration to scope = %s \n",scope->class_name().c_str());
          defdecl->set_scope(scope);
          nondefdecl->set_scope(scope);

       // defdecl->set_parent(scope);

       // Liao, 9/2/2009. merged into fixStructDeclaration
       // DQ (1/25/2009): The scope is not the same as the parent, since the scope is logical, and the parent is structural (note that topScopeStack() is structural).
        nondefdecl->set_parent(scope);
       //   nondefdecl->set_parent(topScopeStack());
       // Liao, 9/2/2009. scope stack is optional, it can be empty
     defdecl->set_parent(scope);
     //defdecl->set_parent(topScopeStack());
#endif
        }

  // DQ (1/26/2009): I think we should assert this, but it breaks the interface as defined
  // by the test code in tests/roseTests/astInterfaceTests.
  // ROSE_ASSERT(defdecl->get_parent() != NULL);

  // ROSE_ASSERT(nonDefiningDecl->get_parent() != NULL);

     ROSE_ASSERT(defdecl->get_definingDeclaration() == defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != defdecl->get_definingDeclaration());

     return defdecl;    
   }

SgClassDeclaration * SageBuilder::buildStructDeclaration(const string& name, SgScopeStatement* scope/*=NULL*/)
   {
     SgName myname(name);
     return buildStructDeclaration(myname, scope);
   }

SgClassDeclaration * SageBuilder::buildStructDeclaration(const char* name, SgScopeStatement* scope/*=NULL*/)
   {
     SgName myname(name);
     return buildStructDeclaration(myname, scope);
   }

SgEnumDeclaration * SageBuilder::buildEnumDeclaration(const SgName& name, SgScopeStatement* scope /*=NULL*/)
  {
 // DQ (1/11/2009): This function has semantics very different from the buildEnumDeclaration_nfi() function!

    if (scope == NULL)
      scope = SageBuilder::topScopeStack();
    SgEnumDeclaration* decl = buildEnumDeclaration_nfi(name, scope);
    setOneSourcePositionForTransformation(decl);
    setOneSourcePositionForTransformation(decl->get_firstNondefiningDeclaration());
    setOneSourcePositionForTransformation(decl->get_definingDeclaration());
    return decl;    
  } //buildEnumDeclaration()

SgEnumDeclaration * SageBuilder::buildEnumDeclaration_nfi(const SgName& name, SgScopeStatement* scope)
  {
    SgEnumDeclaration* defdecl = new SgEnumDeclaration (name,NULL);
    ROSE_ASSERT(defdecl);
    setOneSourcePositionNull(defdecl);
    // constructor is side-effect free
    defdecl->set_definingDeclaration(defdecl);

    // build the nondefining declaration
    SgEnumDeclaration* nondefdecl = buildNondefiningEnumDeclaration_nfi(name, scope);
    nondefdecl->set_definingDeclaration(defdecl);
    defdecl->set_firstNondefiningDeclaration(nondefdecl);

 // DQ (1/11/2009): The buildNondefiningEnumDeclaration function builds an entry in the symbol table, and so we don't want a second one!
#if 0
    SgEnumSymbol* mysymbol = new SgEnumSymbol(nondefdecl);
    ROSE_ASSERT(mysymbol);
 // scope->print_symboltable("buildEnumDeclaration_nfi(): before inserting new SgEnumSymbol");
    scope->insert_symbol(name, mysymbol);
#endif

    defdecl->set_scope(scope);
    nondefdecl->set_scope(scope);
    defdecl->set_parent(scope);
    nondefdecl->set_parent(scope);
    return defdecl;    
  } //buildEnumDeclaration_nfi()

  //! Build a SgFile node
SgFile*
SageBuilder::buildFile(const std::string& inputFileName, const std::string& outputFileName, SgProject* project/*=NULL*/)
   {
     ROSE_ASSERT(inputFileName.size()!=0);// empty file name is not allowed.
     string sourceFilename = inputFileName, fullname;
     Rose_STL_Container<std::string> arglist;
     int nextErrorCode = 0;

     if (project == NULL)
      // SgProject is created on the fly
      // Make up an arglist in order to reuse the code inside SgFile::setupSourceFilename()
        {
          project = new SgProject();
          ROSE_ASSERT(project);
          project->get_fileList().clear();

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
       // DQ (2/6/2009): I think this comment is helpful to put into the file (helps explain why the file exists).
          outputfile<<"// Output file generated so that StringUtility::getAbsolutePathFromRelativePath() will see a vaild file ... unparsed file will have rose_ prefix "<<endl;
          outputfile.close();
        }
       else // file already exists , load and parse it
        {
       // should not reparse all files in case their ASTs have unsaved changes, 
       // just parse the newly loaded file only.
       // use argv here, change non-existing input file later on
       // TODO add error code handling 

       // DQ (2/6/2009): Avoid closing this file twice (so put this here, instead of below).
          testfile.close();
          // should remove the old one here, Liao, 5/1/2009
        }

  // DQ (2/6/2009): Avoid closing this file twice (moved to false branch above).
  // testfile.close();

  // DQ (2/6/2009): Need to add the inputFileName to the source file list in the project, 
  // because this list will be used to subtract off the source files as required to build 
  // the commandline for the backend compiler.
     project->get_sourceFileNameList().push_back(inputFileName);

     Rose_STL_Container<string> sourceFilenames = project->get_sourceFileNameList();
  // printf ("In SageBuilder::buildFile(): sourceFilenames.size() = %zu sourceFilenames = %s \n",sourceFilenames.size(),StringUtility::listToString(sourceFilenames).c_str());

     arglist = project->get_originalCommandLineArgumentList();

  // DQ (2/6/2009): We will be compiling the source code generated in the 
  // "rose_<inputFileName>" file, so we don't want this on the argument stack.
     arglist.push_back(sourceFilename);

#if 1
  // DQ (2/6/2009): Modified.
  // There is output file name specified for rose translators
     if (outputFileName.empty() == false)
        {
          arglist.push_back("-rose:o");
       // arglist.push_back("-o");
          arglist.push_back(outputFileName);
        }
#endif

  // DQ (4/15/2010): Turn on verbose mode
     arglist.push_back("-rose:verbose 2");

  // This handles the case where the original command line may have referenced multiple files.
     Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(arglist,/* binaryMode = */ false);
     CommandlineProcessing::removeAllFileNamesExcept(arglist,fileList,sourceFilename);

  // DQ (9/3/2008): Added support for SgSourceFile IR node
  // SgFile* result = new SgFile (arglist, nextErrorCode, 0, project);
  // AS(10/04/08) Because of refactoring we require the determineFileType function to be called 
  // to construct the node.
  // SgSourceFile* result = new SgSourceFile (arglist, nextErrorCode, 0, project);
  // SgSourceFile* result = isSgSourceFile(determineFileType(arglist, nextErrorCode, project));
  // TH (2009-07-15): changed to more generig isSgFile, this also supports SgBinaryComposite
     SgFile* result = determineFileType(arglist, nextErrorCode, project);
     ROSE_ASSERT(result != NULL);

#if 0
     result->display("SageBuilder::buildFile()");
#endif

     result->set_parent(project);
    if (!Outliner::use_dlopen)     
       project->set_file(*result);  // equal to push_back()
    else
    {
      // Liao, 5/1/2009, 
      // if the original command line is: gcc -c -o my.o my.c and we want to  
      // add a new file(mynew.c), the command line for the new file would become "gcc -c -o my.o mynew.c "
      // which overwrites the object file my.o from my.c and causes linking error.
      // To avoid this problem, I insert the file at the beginning and let the right object file to be the last generated one
      //
      // TODO This is not an elegant fix and it causes some strange assertion failure in addAssociatedNodes(): default case node 
      // So we only turn this on if Outliner:: use_dlopen is used for now
      // The semantics of adding a new source file can cause changes to linking phase (new object files etc.)
      // But ROSE has a long-time bug in handling combined compiling and linking command like "translator -o a.out a.c b.c"
      // It will generated two command line: "translator -o a.out a.c" and "translator -o a.out b.c", which are totally wrong.
      // This problem is very relevant to the bug.
      SgFilePtrList& flist = project->get_fileList();
      flist.insert(flist.begin(),result);
    }
     project->set_frontendErrorCode(max(project->get_frontendErrorCode(), nextErrorCode));

  // Not sure why a warning shows up from astPostProcessing.C
  // SgNode::get_globalMangledNameMap().size() != 0 size = %zu (clearing mangled name cache)
     if (result->get_globalMangledNameMap().size() != 0) 
          result->clearGlobalMangledNameMap();

     return result;
   }// end SgFile* buildFile()


PreprocessingInfo* SageBuilder::buildComment(SgLocatedNode* target, const std::string & content,PreprocessingInfo::RelativePositionType position/*=PreprocessingInfo::before*/,PreprocessingInfo::DirectiveType dtype/* = PreprocessingInfo::CpreprocessorUnknownDeclaration*/)
   {
     return SageInterface::attachComment(target,content, position, dtype);  
   }

//! #define xxx yyy 
PreprocessingInfo* SageBuilder::buildCpreprocessorDefineDeclaration(SgLocatedNode* target,const std::string & content,PreprocessingInfo::RelativePositionType position /* =PreprocessingInfo::before*/)
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

 // DQ (7/19/2008): Modified interface to PreprocessingInfo
 // result = new PreprocessingInfo (mytype,content, "transformation-generated", 0, 0, 0, position, false, true);
    result = new PreprocessingInfo (mytype,content, "transformation-generated", 0, 0, 0, position);
    ROSE_ASSERT(result);
    target->addToAttachedPreprocessingInfo(result);
    return result;
  
  }

//! Build an abstract handle from a SgNode
AbstractHandle::abstract_handle * SageBuilder::buildAbstractHandle(SgNode* n)
{
  // avoid duplicated creation
  static std::map<SgNode*, AbstractHandle::abstract_handle *> handleMap;

  ROSE_ASSERT(n != NULL);
  AbstractHandle::abstract_handle * ahandle =handleMap[n];
  if (ahandle==NULL)
  {
    AbstractHandle::abstract_node* anode = AbstractHandle::buildroseNode(n);
    ROSE_ASSERT(anode !=NULL );
    ahandle = new AbstractHandle::abstract_handle(anode);
    //TODO do we allow NULL handle to be returned?
    ROSE_ASSERT(ahandle != NULL);
  }
  return ahandle;
}

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
std::list<SgScopeStatement*> HighLevel_Sage_Builder_Interface::ScopeStack;


void HighLevel_Sage_Builder_Interface::pushScopeStack (SgScopeStatement* stmt)
{
  ROSE_ASSERT(stmt);
  ScopeStack.push_back(stmt); 
}

void HighLevel_Sage_Builder_Interface::popScopeStack()
{
// we want to warning users  double freeing happens
//  if (!ScopeStack.empty())
  ScopeStack.pop_back();
}

SgScopeStatement* HighLevel_Sage_Builder_Interface::topScopeStack()
{
  return ScopeStack.back();
}

bool HighLevel_Sage_Builder_Interface::emptyScopeStack()
{
  return ScopeStack.empty();
}

void HighLevel_Sage_Builder_Interface::clearScopeStack()
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
HighLevel_Sage_Builder_Interface::buildInitializedName \
 ( const SgName & name, SgType* type, SgScopeStatement* scope)
// ( const SgName & name, SgType* type, SgScopeStatement* scope = NULL )
{
  // If the scope was not specified, then get it from the scope stack.
     if (scope == NULL)
          scope = HighLevel_Sage_Builder_Interface::topScopeStack();
     ROSE_ASSERT(scope != NULL);
     //foo(int);  empty name is possible here!!
     //ROSE_ASSERT(name.is_null() == false);
     ROSE_ASSERT(type != NULL);

     SgInitializedName* initializedName = new SgInitializedName(name,type);
     ROSE_ASSERT(initializedName);
#if 0
    //TODO prototype parameter has no symbol associated!!
    //TODO  scope is also different: SgFunctionDefinition or scope of SgFunctionDeclaration
    // Should we hide the details of symbol table maintainence from users?
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
HighLevel_Sage_Builder_Interface::buildVariableDeclaration \
 (const SgName & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
 //(const SgName & name, SgType* type, SgInitializer * varInit= NULL, SgScopeStatement* scope = NULL)
{
  if (scope == NULL)
    scope = HighLevel_Sage_Builder_Interface::topScopeStack();
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
   cout<<"sageBuilder.C:112 debug: found a previous one!!...."<<endl;
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
SgFunctionParameterList * 
HighLevel_Sage_Builder_Interface::buildFunctionParameterList()
{
  SgFunctionParameterList *parameterList = new SgFunctionParameterList ();
  ROSE_ASSERT (parameterList);

  parameterList->set_definingDeclaration (NULL);
  parameterList->set_firstNondefiningDeclaration (parameterList);

  //TODO performance not optimal, but Sg_File_Info may be deprecated soon
  setSourcePositionForTransformation(parameterList);
  return parameterList;
}

//-----------------------------------------------
// no type vs. void type ?
SgFunctionParameterTypeList * 
HighLevel_Sage_Builder_Interface::buildFunctionParameterTypeList(SgFunctionParameterList * paralist)
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

//-----------------------------------------------
// build function type, 
// 
// insert into symbol table when not duplicated
SgFunctionType * 
HighLevel_Sage_Builder_Interface::buildFunctionType(SgType* return_type, SgFunctionParameterTypeList * typeList)
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
HighLevel_Sage_Builder_Interface::buildFunctionType(SgType* return_type, SgFunctionParameterList * argList)
//HighLevel_Sage_Builder_Interface::buildFunctionType(SgType* return_type, SgFunctionParameterList * argList=NULL)
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
HighLevel_Sage_Builder_Interface::buildNonDefiningFunctionDeclaration \
(const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope)
{
  // argument verification
  if (scope == NULL)
    scope = HighLevel_Sage_Builder_Interface::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  ROSE_ASSERT(scope->containsOnlyDeclarations());
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

   func->set_firstNondefiningDeclaration(func);
   func->set_definingDeclaration(NULL);
  }
   else 
  { 
   //2nd, or 3rd... prototype declaration
   //reuse function type, function symbol of previous declaration

   //std::cout<<"debug:HighLevel_Sage_Builder_Interface.C: 267: "<<"found func_symbol!"<<std::endl;
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
  // TODO: wrap it into a supporting function like  set_parameterList(func, paralist)
	   // fixup the scope of arguments, 
	   //no symbols for nondefining function declaration's arguments
  func->set_parameterList(paralist);
  paralist->set_parent(func);

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
HighLevel_Sage_Builder_Interface::buildDefiningFunctionDeclaration_T \
(const SgName & name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope)
//	(const SgName & name, SgType* return_type, SgScopeStatement* scope=NULL)
{
  if (scope == NULL)
    scope = HighLevel_Sage_Builder_Interface::topScopeStack();
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

   // parameter list, TODO wrap it into a supporting function
   //TODO consider the difference between C++ and Fortran
  func->set_parameterList(paralist);
  paralist->set_parent(func);
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
HighLevel_Sage_Builder_Interface::buildDefiningFunctionDeclaration \
(const SgName & name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope)
//	(const SgName & name, SgType* return_type, SgScopeStatement* scope=NULL)
{
  SgFunctionDeclaration * func= buildDefiningFunctionDeclaration_T<SgFunctionDeclaration> \
 (name,return_type,paralist,scope);

  return func;

}
//---------------------AST insertion---------------------
// handle side effects such as 
// * defining/nondefining pointers
// * preprocessing information
void HighLevel_Sage_Builder_Interface::append_statement(SgStatement *stmt)
{
  SgScopeStatement *scope = topScopeStack();
  ROSE_ASSERT(scope);

  // handle side effect for function declaration insertion. 
  // case 1: for a defining function declaration, 
  //	fix up definingDeclaration pointers from all previous decs.
  // case 2: for a nondifining function declaration, 
  //	fix up the firstNondefining pointer of a defining declaration appeared before.
  //TODO put it into a function, and consider insert in the middle later on
  if(isSgFunctionDeclaration(stmt))
  {
    ROSE_ASSERT(scope->containsOnlyDeclarations());
    SgFunctionDeclaration * func = isSgFunctionDeclaration(stmt);

    //SgStatementPtrList stmtList = scope->getStatementList();
    SgDeclarationStatementPtrList declList = scope->getDeclarationList();
    SgDeclarationStatementPtrList::iterator i;
    SgFunctionType* func_type = func->get_type();
    for (i=declList.begin();i!=declList.end();i++) 
    {
      SgFunctionDeclaration* prevDecl = isSgFunctionDeclaration(*i);
      if ((prevDecl) &&(prevDecl->get_type() == func_type)) {
        if(func->get_definingDeclaration()==func)
        { // case 1: defining declaration
//std::cout<<"debug highLevelInterface.C:445.....decl with definition!"<<std::endl;
          if(prevDecl->get_definingDeclaration()==NULL)
            prevDecl->set_definingDeclaration(func);
         } 
         else // case 2: nonefining declaration
         {  // multiple nondefining declaration may exist
//std::cout<<"debug highLevelInterface.C:450.....decl without definition!"<<std::endl;
          if(prevDecl->get_firstNondefiningDeclaration()==NULL)
              prevDecl->set_firstNondefiningDeclaration(func);
         }
      } // end if prevDecl is a function dec. with the same type
    } // end for
  } // end if functionDec
  //-----------------------
  

  //TODO: try lowlevel rewrite mechanism to handle preprocessing info.
  //several cases:
  //1. comments of empty file is attached AFTER SgGlobal, 
  //	 should move it inside and attach it BEFORE the inserted stmt
  //2. preprocessing Info attached INSIDE an empty scope
  //	 should remove from the scope stmt, and attach it before the inserted stmt  
  //3. preprocessing Info attached  after a located node: #endif,what to do? 

  //-----------------------
  // append the statement finally
  //topScopeStack ()->append_statement (stmt);
  topScopeStack ()->insertStatementInScope(stmt,false);
}

void HighLevel_Sage_Builder_Interface::append_arg(SgFunctionParameterList *paraList, SgInitializedName* initName)
{
  ROSE_ASSERT(paraList);
  ROSE_ASSERT(initName);
  paraList->append_arg(initName);
  initName->set_parent(paraList);
}


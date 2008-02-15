#ifndef ROSE_SAGE_BUILDER_INTERFACE
#define ROSE_SAGE_BUILDER_INTERFACE

#include "rose.h"

/*!
  \defgroup frontendSageHighLevelInterface High level AST builders
  \ingroup ROSE_FrontEndGroup
  \brief This namespace contains high level SAGEIII AST node and subtree builders

  Building AST trees using SgNode constructors is tedious and error-prone. It becomes 
  even more difficult with the presence of symbol tables. This namespace contains major
  AST node builders on top of the constructors. The builders also take care of symbol table
  maintenance, scope and parent pointers and so on.

  \authors Chunhua Liao
  
*/
namespace HighLevel_Sage_Builder_Interface {

#if 0
//---------------------AST creation/building/construction-----------------
//------------------------------------------------------------------------
//  builder for Sage AST nodes and subtrees
//  a wrapper for constructor, handle side effects for building edge links and
//   maintaining symbol tables, and any other side effects.
//  currently, they are put into src/frontend/SageIII/sageInterface/highLevelInterface.h
//   what name convention?
//   buildXXX

  SgExpression* buildConst(SgType* t, const string & val);
  SgExpression* buildConstInt(int val);
  SgExpression* buildUnaryOp();

  // build sth in languages, but not in SAGE AST
  // how to specify position info?  then
  PreprocessingInfo* buildComment(style, std::string& content);

 SgClassDeclaration* buildClassDeclaration(...,class_type);
  // or
 SgClassDeclaration* buildStruct(...);
 SgClassDeclaration* buildClass(...);
 SgClassDeclaration* buildUnion(...);

  // build B from dependent A
 SgTemplateInstantiationMemberFunctionDecl*  buildForwardFunctionDeclaration
     (SgTemplateInstantiationMemberFunctionDecl * memberFunctionInstantiation);

  //previous attempt: src/midend/astUtil/astInterface
  //  AstNodePtr AstInterface::CreateConst( const string& val, const string& valtype) const
  //  AstNodePtr AstInterfaceBase::CreateConstInt( int val)
  //  AstNodePtr AstInterface::CreateFunction( const string& name, const string& decl)
  //  AstNodePtr AstInterface::CreateUnaryOP( const string& op, const AstNodePtr& a0)
  //  AstNodePtr AstInterface::CreateBinaryOP( const string& op, const AstNodePtr& a0, const AstNodePtr& a1)
  //  AstNodePtr AstInterfaceBase::CreateFunctionCall( const AstNodePtr& func, AstNodeList args)

#endif

//-----------------scope stack interfaces-------------------
//Liao, Jan. 7, 2008
//  Please use the recommended operations for the stack
//  Don't use raw access functions to ScopeStack,
//  e.g. avoid ScopeStack.push_back(), using pushScopeStack() instead
//
// TODO consider complex cases:   
//	* how many scope stacks to keep? one. examine only one transparently
//	* regular: push a child scope of current scope, add symbols etc.
//	* irregular: push an arbitrary scope temporarily,  add some symbol, then pop
//	* even push a chain of scopes
//	* restore scopes
//----------------------------------------------------------
// intended to be a private member, could be changed any time

// DQ (1/18/2008): Added declaration in source file with Liao.
/*! \brief intended to be a private member, could be changed any time
*/
extern std::list<SgScopeStatement*> ScopeStack;

//public interfaces of the scope stack, should be stable
void pushScopeStack (SgScopeStatement* stmt);
void popScopeStack();
SgScopeStatement* topScopeStack() ;
bool emptyScopeStack();
void clearScopeStack();
   
//------ AST insertion(append, prepend, insert) -----------
// scope->append_statement() is not enough to handle side effect of appending statements
//e.g. preprocessing info, defining/nondefining pointers etc.
//----------------------------------------------------------
void append_statement(SgStatement *stmt);

// add side effect
void append_arg(SgFunctionParameterList *, SgInitializedName*);

//------------ AST builders--------------------------------
SgInitializedName* 
buildInitializedName(const SgName & name, SgType* type, SgScopeStatement* scope=NULL);

SgVariableDeclaration* 
buildVariableDeclaration(const SgName & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);

SgFunctionDeclaration *
buildNonDefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL);

template <class actualFunction>
SgFunctionDeclaration *
buildDefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

SgFunctionDeclaration *
buildDefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL);

SgFunctionParameterList * buildFunctionParameterList();

SgFunctionParameterTypeList * 
buildFunctionParameterTypeList(SgFunctionParameterList * paralist);

SgFunctionType * 
buildFunctionType(SgType* return_type, SgFunctionParameterTypeList * typeList=NULL);

SgFunctionType * 
buildFunctionType(SgType* return_type, SgFunctionParameterList * argList=NULL);

}
 
#endif

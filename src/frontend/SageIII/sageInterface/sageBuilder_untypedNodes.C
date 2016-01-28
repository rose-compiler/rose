// tps (01/14/2010) : Switching from rose.h to sage3
// test cases are put into tests/roseTests/astInterfaceTests
#include "sage3basic.h"

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#include "roseAdapter.h"
#include "markLhsValues.h"
#include "sageBuilder.h"
#include <fstream>
#include <boost/algorithm/string/trim.hpp>
#include "Outliner.hh"
#endif

using namespace std;
using namespace SageInterface;



SgUntypedFile*
SageBuilder::buildUntypedFile(SgUntypedGlobalScope* scope)
   {
     SgUntypedFile* returnNode = new SgUntypedFile();
     ROSE_ASSERT(returnNode != NULL);

     returnNode->set_scope(scope);

     if (scope != NULL)
        {
          scope->set_parent(returnNode);
        }

     setSourcePosition(returnNode);

     return returnNode;

   }

void setupMembers(SgUntypedScope* scopeNode,SgUntypedDeclarationStatementList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list)
   {
     ROSE_ASSERT(scopeNode != NULL);

  // Set the data members
     scopeNode->set_declaration_list(declaration_list);
     scopeNode->set_statement_list(statement_list);
     scopeNode->set_function_list(function_list);

  // Set the parents
     if (declaration_list != NULL)
        {
          declaration_list->set_parent(scopeNode);
        }

     if (statement_list != NULL)
        {
          statement_list->set_parent(scopeNode);
        }

     if (function_list != NULL)
        {
          function_list->set_parent(scopeNode);
        }

  // Not clear what to do with the source position information.
  // Since a SgUntypedNode is a SgLocatedNode we internally have a place to store source position information.
     setSourcePosition(scopeNode);
   }

SgUntypedScope*
SageBuilder::buildUntypedScope(SgUntypedDeclarationStatementList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list )
   {
     SgUntypedScope* returnNode = new SgUntypedScope();
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,declaration_list,statement_list,function_list);

     return returnNode;
   }


SgUntypedGlobalScope*
SageBuilder::buildUntypedGlobalScope(SgUntypedDeclarationStatementList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list )
   {
     SgUntypedGlobalScope* returnNode = new SgUntypedGlobalScope();
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,declaration_list,statement_list,function_list);

     return returnNode;
   }


SgUntypedFunctionScope*
SageBuilder::buildUntypedFunctionScope(SgUntypedDeclarationStatementList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list )
   {
     SgUntypedFunctionScope* returnNode = new SgUntypedFunctionScope();
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,declaration_list,statement_list,function_list);

     return returnNode;
   }

SgUntypedModuleScope*
SageBuilder::buildUntypedModuleScope(SgUntypedDeclarationStatementList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list )
   {
     SgUntypedModuleScope* returnNode = new SgUntypedModuleScope();
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,declaration_list,statement_list,function_list);

     return returnNode;
   }


void setupMembers(SgUntypedFunctionDeclaration* functionNode, SgUntypedInitializedNameList* parameters, SgUntypedType* type, SgUntypedFunctionScope* scope, SgUntypedNamedStatement* end_statement)
   {
     ROSE_ASSERT(functionNode != NULL);

  // Set the data members
     functionNode->set_parameters(parameters);
     functionNode->set_type(type);
     functionNode->set_scope(scope);
     functionNode->set_end_statement(end_statement);

  // Set the parents
     if (parameters != NULL)
        {
          parameters->set_parent(functionNode);
        }

     if (type != NULL)
        {
          type->set_parent(functionNode);
        }

     if (scope != NULL)
        {
          scope->set_parent(functionNode);
        }

     if (end_statement != NULL)
        {
          end_statement->set_parent(functionNode);
        }
     
  // Not clear what to do with the source position information.
  // Since a SgUntypedNode is a SgLocatedNode we internally have a place to store source position information.
     setSourcePosition(functionNode);
   }

SgUntypedFunctionDeclaration*
SageBuilder::buildUntypedFunctionDeclaration(std::string name, SgUntypedInitializedNameList* parameters, SgUntypedType* type, SgUntypedFunctionScope* scope, SgUntypedNamedStatement* end_statement)
   {
     SgUntypedFunctionDeclaration* returnNode = new SgUntypedFunctionDeclaration(name);
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,parameters,type,scope,end_statement);

     return returnNode;
   }

SgUntypedProgramHeaderDeclaration*
SageBuilder::buildUntypedProgramHeaderDeclaration(std::string name, SgUntypedInitializedNameList* parameters, SgUntypedType* type, SgUntypedFunctionScope* scope, SgUntypedNamedStatement* end_statement)
   {
     SgUntypedProgramHeaderDeclaration* returnNode = new SgUntypedProgramHeaderDeclaration(name);
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,parameters,type,scope,end_statement);

     return returnNode;
   }

SgUntypedSubroutineDeclaration*
SageBuilder::buildUntypedSubroutineDeclaration(std::string name, SgUntypedInitializedNameList* parameters, SgUntypedType* type, SgUntypedFunctionScope* scope, SgUntypedNamedStatement* end_statement)
   {
     SgUntypedSubroutineDeclaration* returnNode = new SgUntypedSubroutineDeclaration(name);
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,parameters,type,scope,end_statement);

     return returnNode;
   }


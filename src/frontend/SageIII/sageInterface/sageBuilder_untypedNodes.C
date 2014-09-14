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


void setupMembers(SgUntypedScope* scopeNode,SgUntypedDeclarationList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list)
   {
  // Set the data members
     scopeNode->set_declaration_list(declaration_list);
     scopeNode->set_statement_list(statement_list);
     scopeNode->set_function_list(function_list);

  // Set the parents
     declaration_list->set_parent(scopeNode);
     statement_list->set_parent(scopeNode);
     function_list->set_parent(scopeNode);

  // Not clear what to do with the source position information.
  // Since a SgUntypedNode is a SgLocatedNode we internally have a place to store source position information.
     setSourcePosition(scopeNode);
   }

SgUntypedScope*
SageBuilder::buildUntypedScope(SgUntypedDeclarationList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list )
   {
     SgUntypedScope* returnNode = new SgUntypedScope();
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,declaration_list,statement_list,function_list);

     return returnNode;
   }


SgUntypedScope*
SageBuilder::buildUntypedGlobalScope(SgUntypedDeclarationList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list )
   {
     SgUntypedGlobalScope* returnNode = new SgUntypedGlobalScope();
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,declaration_list,statement_list,function_list);

     return returnNode;
   }


SgUntypedScope*
SageBuilder::buildUntypedFunctionScope(SgUntypedDeclarationList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list )
   {
     SgUntypedFunctionScope* returnNode = new SgUntypedFunctionScope();
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,declaration_list,statement_list,function_list);

     return returnNode;
   }

SgUntypedScope*
SageBuilder::buildUntypedModuleScope(SgUntypedDeclarationList* declaration_list, SgUntypedStatementList* statement_list, SgUntypedFunctionDeclarationList* function_list )
   {
     SgUntypedModuleScope* returnNode = new SgUntypedModuleScope();
     ROSE_ASSERT(returnNode != NULL);

     setupMembers(returnNode,declaration_list,statement_list,function_list);

     return returnNode;
   }


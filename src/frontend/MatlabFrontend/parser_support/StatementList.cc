#include "sage3basic.h" 
#include "StatementList.h"

StatementList::StatementList() : SgScopeStatement()
    {
    }
	
 StatementList::StatementList(SgStatement* statement)
    {
      appendStatement(statement);
    }

void StatementList::appendStatement(SgStatement* statement)
  {
    statements.push_back(statement);
  }

SgBasicBlock* StatementList::getBasicBlock()
  {
    SgBasicBlock* block = SageBuilder::buildBasicBlock();

    SageInterface::appendStatementList(statements, block);

    return block;
  }

void StatementList::appendAll(SgScopeStatement *scope)
  {		
    SageBuilder::pushScopeStack(scope);
    SageInterface::appendStatementList(statements);
    SageBuilder::popScopeStack();
  }

void StatementList::appendAll()
  {				
    SageInterface::appendStatementList(statements);		
  }

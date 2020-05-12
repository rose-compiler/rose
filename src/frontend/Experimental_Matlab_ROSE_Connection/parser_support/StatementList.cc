#include "rose.h"
#include "StatementList.h"

/*
static
void appendAll(std::vector<Statement*>& vec)
{
  for (Statement* stmt : vec)
  {
    SageInterface::appendStatement(stmt);
  }
}

static
void appendAll(std::vector<Statement*>& vec, SgScopeStatement* scope)
{
  SageBuilder::pushScopeStack(scope);
  appendAll(vec);
  SageBuilder::popScopeStack();
}
*/

static
void dbgList(std::vector<SgStatement*>& vec)
{
  for (SgStatement* s : vec)
  {
    SgLocatedNode*   n  = s;

    if (SgExprStatement* es = isSgExprStatement(s))
      n = es->get_expression();

    std::cerr << static_cast<void*>(n) << " " << std::flush
              << typeid(*n).name() << " " << std::flush
              //~ << n->unparseToString()
              << std::endl;
  }
}


SgBasicBlock* StatementList::getBasicBlock()
  {
    dbgList(statements);
    SgBasicBlock* block = SageBuilder::buildBasicBlock();

    SageInterface::appendStatementList(statements, block);

    return block;
  }

void StatementList::appendAll(SgScopeStatement *scope)
  {
    dbgList(statements);
    SageBuilder::pushScopeStack(scope);
    SageInterface::appendStatementList(statements);
    SageBuilder::popScopeStack();
  }

void StatementList::appendAll()
  {
    dbgList(statements);
    SageInterface::appendStatementList(statements);
  }

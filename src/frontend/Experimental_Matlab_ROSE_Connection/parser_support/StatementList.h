#ifndef __STATEMENT_LIST
#define __STATEMENT_LIST

#include <vector>

/*A simple container to temporarily hold a list of statements.*/
struct StatementList : SgScopeStatement
{
  StatementList()
  : SgScopeStatement(), statements()
  {}

  explicit
  StatementList(SgStatement* stmt)
  : SgScopeStatement(), statements(1, stmt)
  {}

  void appendStatement(SgStatement* stmt)
  {
    statements.push_back(stmt);
  }

  SgBasicBlock* getBasicBlock();

  void appendAll(SgScopeStatement *scope);

  void appendAll();

 private:
   std::vector<SgStatement*> statements;
};
#endif

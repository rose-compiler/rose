#ifndef __STATEMENT_LIST
#define __STATEMENT_LIST

#include <vector>

/*A simple container to temporarily hold a list of statements.*/
class StatementList : public SgScopeStatement
{
 public:
  StatementList();
  
 StatementList(SgStatement* statement);

 void appendStatement(SgStatement* statement);

 SgBasicBlock* getBasicBlock();

 void appendAll(SgScopeStatement *scope);

 void appendAll();

 private:
  std::vector<SgStatement*> statements;
};
#endif

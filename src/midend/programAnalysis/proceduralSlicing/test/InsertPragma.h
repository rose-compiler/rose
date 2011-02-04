#ifndef INSERT_IS_DEFINED
#define INSERT_IS_DEFINED


#include "BasicBlockLocalIndex.h"
#include <string>
//#include <ASTNodeCollection.h>
//#include <midLevelRewriteInterface.h>
#include "AstRestructure.h"
//#include "highLevelRewriteInterface.h"
#include <string.h>

/*! 
This class inserts the pragma declaration around one statement, identified with a global index.
*/

class InsertPragma : public SgSimpleProcessing{

 public:
  void virtual visit(SgNode* node);
  void set_statement(int n){no_statement=n;}
  void createFunctionDeclaration(SgGlobal* global, list<SgNode*> var_list,
                                 SgFunctionDeclaration*& printf_func,
                                 SgFunctionDeclaration*& fopen_func,
                                 SgFunctionDeclaration*& fclose_func);
  void createFunctionCallprintf(SgGlobal*& root, SgFunctionDeclaration* printf_func,list<SgNode*> var_list,SgExprStatement*& func_expr);
  void createFunctionCallfopen(SgGlobal*& root, SgFunctionDeclaration* fopen_func,SgExprStatement*& func_expr);
  void createFunctionCallfclose(SgGlobal*& root, SgFunctionDeclaration* fclose_func,SgExprStatement*& func_expr);

 private:
  int no_statement;
};

#endif

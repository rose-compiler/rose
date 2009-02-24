
#include <list>


// this function identifies process-functions (main or possibly thread functions
std::vector<SgFunctionDefinition *> findProcessFunctions(SgProject* proj);
bool toConvert(SgNode *node);
SgStatement * getStatement(SgNode*node);
void markForPromelaTransformation(SgNode * node);
void forceCCodeNesting(SgNode * node);
void flattenScopes(SgFunctionDefinition * fDef);
void refactorSgVarRefExpInCBlocks(SgFunctionDefinition * fDef,SgVariableDeclaration* vDef);
void removeReturnStmts(SgFunctionDefinition * fDef);
bool isSideEffektingStmt(SgStatement * stmt);
void removeContinues(SgWhileStmt * whileStmt);
void forToWhileTransformation(SgForStatement *it);
void doWhileToWhileTransformation(SgDoWhileStmt * doWhile);

bool isPromelaCompatible(SgStatement * stmt);
bool isPromelaCompatibleVariableType(SgVariableDeclaration * node);
bool isPromelaCompatibleVariableType(SgVarRefExp * node);
void markStatementsWithCStateVariables(SgFunctionDefinition * fDef,std::set<SgInitializedName *>
variables);
void markAsPromelaStateVar(SgNode *node,bool global=false);

void revertMPIDefinitions(SgProject * proj);
void replaceSgNode(SgNode * what,SgNode * with);

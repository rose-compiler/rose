#ifndef HH__UTIL_HEADER__HH
#define HH__UTIL_HEADER__HH
#include "rose.h"
#include "DefUseAnalysis.h"
//#include <string>
//#include <vector>
//#include <algorithm>
//#include <functional>
//#include <numeric>
//#include <iostream>


class PathCollector
{
private:

    SgIncidenceDirectedGraph* graph;
    std::vector<std::vector<SgGraphNode*> > pathSet;
    StaticCFG::CFG* cfg;

public:

    PathCollector(SgIncidenceDirectedGraph* g, StaticCFG::CFG* my_cfg) {graph = g; cfg = my_cfg;}
//    PathCollector( const std::vector<Link>& links,

    ~PathCollector();
    SgIncidenceDirectedGraph* getGraph() {return graph;}
    void addPath( const std::vector<SgGraphNode*>& p );
    void clearPaths();
    CFGNode getEquivCFGNode(SgGraphNode*);
    int getEquivCFGNodeIndex(SgGraphNode*);
    StaticCFG::CFG* getStaticCFG();
    void displayPath(std::vector<SgGraphNode*> p);
    void displayAllPaths();
    std::map<SgNode*,std::string> getAssociationsForPath(std::vector<SgGraphNode*> path);
    //virtual void analyzePath(std::vector<SgGraphNode*> path) = 0;
    std::vector<std::vector<SgGraphNode*> >  getPaths() { return pathSet; }
    //void traverseAndCollect(SgGraphNode* beginning, SgGraphNode* end) 

};

namespace GraphAlgorithm {

extern SgGraphNode* startNode;
void DepthFirst(PathCollector* pathCollector, std::vector<SgGraphNode*>& visited, SgGraphNode* end, const int& max_hops, const int& min_hops);
bool containsNode(SgGraphNode*,std::vector<SgGraphNode*>);
}






std::string getWhileRule(PathCollector*,SgWhileStmt*,std::set<SgVariableSymbol*>,std::string&,std::string&);

std::vector<std::string> getWhileRules(PathCollector*,SgWhileStmt*,std::set<SgVariableSymbol*>,std::string&,std::string&);
std::string queryPragma(std::vector<std::string>,std::string);
std::string assumptionPragma(std::vector<std::string>,std::string);
std::set<SgVariableSymbol*> getVars(PathCollector*);
SgGraphNode* getWhileOutNode(SgWhileStmt*,PathCollector*);
std::string getPrelude();
SgGraphNode* getWhileEndNode(SgWhileStmt*,PathCollector*);
void collectPaths(SgGraphNode*,SgGraphNode*,PathCollector*);
std::set<SgPragmaDeclaration*> collectPragmas(SgNode*);
void getSgScopeStatement(SgScopeStatement*);
std::vector<std::string> getRules(SgNode*,PathCollector*,std::set<SgVariableSymbol*>, std::string&,std::string&);
int initializeScopeInformation(SgProject*);
void getSgFunctionDefinition(SgFunctionDefinition*);

void listStatements(SgScopeStatement* currentScopeStat);
void listBasicBlockStats(SgBasicBlock* bl);
void listBodyStatements(SgScopeStatement* scop);
void listStatementPtrList(SgStatementPtrList list);
void listSgIfStmts(SgIfStmt* ifstat);

extern std::map<std::string,std::string> origNameToSMTName;

std::string get_pragma_string(SgPragmaDeclaration*);
std::string getRHS(std::string,std::set<std::string>&);
std::string translateToS_Expr(std::string,std::set<std::string>&,std::vector<std::string>&,bool&);

std::map<SgNode*, std::string > createPathDetailsFromPath(StaticCFG::CFG*,SgIncidenceDirectedGraph* g,std::vector<SgGraphNode*> path);
void getExpressionAndValue(SgIncidenceDirectedGraph*,std::vector<SgGraphNode*>,int,std::pair<std::string,SgNode*>& expressionValue);
std::vector<std::string> evaluateBody(std::vector<SgStatement*>);
bool CoveredConditionKind(std::string);

std::string writeIfStmt(SgIfStmt* solve_if);


void initDefUseAnalysis(SgProject* proj);
void debugAnalyzeVariableReference(SgVarRefExp*);
std::vector<SgStatement*> analyzeVariableDefUse(SgVarRefExp*);

extern std::map<SgWhileStmt*, std::string> whileSMTFunctionName;
std::vector<SgStatement*> analyzeVariableDefUse(SgVarRefExp*);
std::vector<SgStatement*> getStatementList(SgStatement* node);
std::string writeWhileStatement(SgWhileStmt* whileStat);
std::string constructConditional(SgExpression* conditional);
//extern std::map<SgWhileStmt*, std::string> while_conditions;
std::string getFunctionCallExpInfo(SgFunctionCallExp* fcall);
std::string operandInfo(SgExpression* operand);

std::vector<SgStatement*> collectStatements(SgScopeStatement* currentScopeStat);
//int initializeScopeInformation(SgProject*);
std::vector<SgStatement*> collectBasicBlockStats(SgBasicBlock* bl);
std::vector<SgStatement*> collectBodyStatements(SgScopeStatement* scop);
std::vector<SgStatement*> collectStatementPtrList(SgStatementPtrList list);
void collectSgIfStmts(SgIfStmt* ifstat);

void replacePrefixPostfixOp(SgNode* n, SgScopeStatement* scop);
void replacePrefixPostfixStat(SgNode* n);
void fixAllPrefixPostfix(SgProject* proj);


extern std::map<SgSymbol*,int> SymbolToInstances;
extern std::map<SgVariableSymbol*, std::string> varExpressionValue;
extern std::set<SgVariableSymbol*> varSymbolList;
extern std::map<SgScopeStatement*,int> scopeStatementToInteger;
extern std::map<int, SgScopeStatement*> scopeIntegerToStatement;

extern std::map<SgVariableSymbol*,int> variableSymbolToInstanceIdentifier;
extern std::map<SgVariableSymbol*,std::string> symbolToConvertedString;
extern std::map<std::string,int> variablesOfNameX;
// Use this function to transform C types to SMT2 equivalents
std::string get_type_identifier(SgType* typ);
std::string getVarName(SgInitializedName* nam);
std::string getVarName(SgVarRefExp*);
std::string getVarName(SgVariableSymbol*);
void getSgDeclarationStatement(SgDeclarationStatement*);
std::string getNewVar(SgVarRefExp*);
std::string getNewVar(SgInitializedName*);


std::string convertVarRefExp(SgVarRefExp*);
std::string getCompoundAssignOp(SgCompoundAssignOp*,SgExpression*,SgExpression*);
std::string getSgStatement(SgStatement* stat);
std::string getSgBinaryOper(SgBinaryOp* op);
std::string getSgBinaryOp(SgBinaryOp*);
std::string getAssignOp(SgBinaryOp* binOp,SgExpression* lhs, SgExpression* rhs);
std::string getSgUnaryOp(SgUnaryOp* unaryOp);
std::string getSgValueExp(SgValueExp*);
void getSgBasicBlock(SgBasicBlock*);
extern std::map<std::string, std::string> enums_defined;
std::string getSExpr(SgExpression* expNode);
std::string convertAssignOp(SgBinaryOp* binOp, SgExpression* lhs, SgExpression* rhs);
std::string initializeVariable(SgInitializedName* initName);
extern std::map<SgSymbol*, std::string> SymbolToZ3;

void evaluatePath(std::vector<SgGraphNode*>, std::map<SgNode*,std::string>,SgExpression*,std::set<SgVariableSymbol*>,std::string&, std::string&, std::string&);

void listSgSwitchStatements(SgSwitchStatement* switchStat);
bool isScopeWithBasicBlock(SgScopeStatement* scop);
bool isScopeWithBody(SgScopeStatement* scop);
//void listBodyStatements(SgScopeStatement* scop);
//void listBasicBlockStats(SgBasicBlock* bl);
//void listStatementPtrList(SgStatementPtrList statList);
//void listStatements(SgScopeStatement* currentScopeStat);
//void listSgIfStmts(SgIfStmt* ifstmt);
/*
class PathCollector
{
private:

    SgIncidenceDirectedGraph* graph;
    std::vector<std::vector<SgGraphNode*> > pathSet;
    StaticCFG::CFG* cfg;

public:

    PathCollector(SgIncidenceDirectedGraph* g, StaticCFG::CFG* my_cfg) {graph = g; cfg = my_cfg;}
//    PathCollector( const std::vector<Link>& links,

    ~PathCollector();
    SgIncidenceDirectedGraph* getGraph() {return graph;}
    void addPath( const std::vector<SgGraphNode*>& p );
    void clearPaths();
    CFGNode getEquivCFGNode(SgGraphNode*);
    int getEquivCFGNodeIndex(SgGraphNode*);
    StaticCFG::CFG* getStaticCFG();
    void displayPath(std::vector<SgGraphNode*> p);
    void displayAllPaths();
    std::map<SgNode*,std::string> getAssociationsForPath(std::vector<SgGraphNode*> path);
    //virtual void analyzePath(std::vector<SgGraphNode*> path) = 0;
    std::vector<std::vector<SgGraphNode*> >  getPathSet() { return pathSet; }
    //void traverseAndCollect(SgGraphNode* beginning, SgGraphNode* end) 

};

namespace GraphAlgorithm {

extern SgGraphNode* startNode;
void DepthFirst(PathCollector* pathCollector, std::vector<SgGraphNode*>& visited, SgGraphNode* end, const int& max_hops, const int& min_hops);
bool containsNode(SgGraphNode*,std::vector<SgGraphNode*>);
}
*/

#endif

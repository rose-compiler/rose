#ifndef HH_SMTPATHLIBPRAGMAAWARE_HH
#define HH_SMTPATHLIBPRAGMAAWARE_HH

#include <iostream>
#include <fstream>
#include "rose.h"
#include <string>
#include <err.h>
#include "SgGraphTemplate.h"
#include "graphProcessing.h"
#include "staticCFG.h"
//#include "../utils/dotDebugger.h"
//#include "smtQueryLib.h"
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;
using namespace boost;
//SMTDebugger* debugger;
//SMTDebugger* initializeSMTDebugger(std::string);
void initializeDebugger();
void completeDebugger();
std::string get_pragma_string(SgPragmaDeclaration* prdecl);
std::string get_name(std::string pstring);
std::string get_position(std::string pstring);
std::vector<std::string> get_vars(std::string pstring);

std::map<std::string, std::pair<SgNode*, SgNode*> > stringToPragmas;

std::vector<std::string> variables;
std::vector<std::string> expressions;
std::vector<std::string> declarations;
int if_statement_count;
int expression_count;
std::string getSgIfStmt(SgIfStmt*, int);
std::string getSgScopeStatement(SgScopeStatement* s,int truthVal=0);
std::string getSgStatement(SgStatement*);
std::vector<SgStatement*> annotateTree(SgProject*);
std::vector<SgStatement*> stats;

void setTruthValue(SgNode*,bool);

bool getCorrespondingPathNodeTruthValue(CFGNode,CFGNode);
//bool getEdgeKindValue(EdgeConditionKind);
bool getConditionalValue(EdgeConditionKind);

//std::map<SgNode*, bool> pathNodeTruthValue;

typedef myGraph CFGforT;

myGraph* traversalMg;

SgIncidenceDirectedGraph* g;

StaticCFG::CFG* globalCFG;
#endif

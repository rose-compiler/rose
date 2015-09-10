#ifndef HH_SMTPRAGMABACKEND_HH
#define HH_SMTPRAGMABACKEND_HH
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include "rose.h"
#include "SgGraphTemplate.h"
#include "graphProcessing.h"
#include "staticCFG.h"

using namespace boost;
using namespace std;

std::vector<std::string> pragma_suffix;
typedef std::pair<SgPragmaDeclaration*,SgPragmaDeclaration*> pragma_pair;
typedef myGraph CFGforT;
StaticCFG::CFG* globalCFG;
CFGforT* orig;
myGraph* mg;
std::vector<std::string> get_vars(std::string);
void process_branches(std::vector<SgGraphNode*>);
std::string get_pragma_string(SgPragmaDeclaration*);
std::string get_name(std::string);
std::string get_position(std::string);
std::string getSgStatement(SgStatement*);
std::vector<std::string> variables;
std::vector<std::string> expressions;
std::vector<std::string> declarations;
std::string getSgStatement(SgStatement*);
void setTruthValue(SgNode*,bool);
//bool getCorrespondingPathNodeTruthValue(VirtualCFG::CFGNode,VirtualCFG::CFGNode);
bool getConditionalValue(EdgeConditionKind);

//myGraph* traversalMg;



#endif

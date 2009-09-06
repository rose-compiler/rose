#ifndef GENERATEDCOPTS_H
#define GENERATEDCOPTS_H

//#include "rose.h"
#include "x86AssemblyToC.h"
#include "filteredCFG.h"
#include <fstream>
#include <set>
#include <map>
#include <string>
#include <stdint.h>

struct CTranslationPolicy {
  // Common parts that are used in optimizations
  virtual ~CTranslationPolicy() {}
  virtual bool isMemoryWrite(SgFunctionRefExp* func) const = 0;
  virtual bool isMemoryRead(SgFunctionRefExp* func) const = 0;
  virtual bool isVolatileOperation(SgExpression* e) const = 0; // Things like system calls
  virtual SgStatement* getWhileBody() const = 0;
  virtual SgBasicBlock* getSwitchBody() const = 0;
  virtual SgVariableSymbol* getIPSymbol() const = 0;
  virtual const std::map<uint64_t, SgLabelStatement*>& getLabelsForBlocks() const = 0;
  virtual const std::set<uint64_t>& getExternallyVisibleBlocks() const = 0;
};

unsigned long long getValue(SgExpression* e);
SgValueExp* buildConstantOfType(unsigned long long val, SgType* t);

// Returns true only if top level expr was changed
bool simplifyExpression(SgExpression*& expr, bool& changed);

void simplifyAllExpressions(SgNode* top);
void removeIfConstants(SgNode* top);
SgExpression* plugInDefsForExpression(SgExpression* e, const std::map<SgInitializedName*, SgExpression*>& defs);
void plugInAllConstVarDefsForBlock(SgBasicBlock* bb, std::map<SgInitializedName*, SgExpression*>& defs, const CTranslationPolicy& conv);
void plugInAllConstVarDefs(SgNode* top, const CTranslationPolicy& conv);
void getUsedVariables(SgExpression* e, std::set<SgInitializedName*>& vars);

bool isSimple(SgExpression* e);
void addDirectJumpsToSwitchCases(const CTranslationPolicy& conv);
SgStatement* getImmediatePredecessor(SgStatement* s);
void getPredecessorsFromEnd(SgStatement* s, std::set<SgStatement*>& result, const std::map<SgLabelStatement*, std::set<SgGotoStatement*> >& gotoMap);

void getPredecessorsFromBeginning(SgStatement* s, std::set<SgStatement*>& result, const std::map<SgLabelStatement*, std::set<SgGotoStatement*> >& gotoMap);

// set<SgInitializedName*> makeAllPossibleVars(const X86AssemblyToCWithVariables& conv);

template <typename T>
bool setUnionInplace(std::set<T>& a, const std::set<T>& b) {
  bool changed = false;
  for (typename std::set<T>::const_iterator i = b.begin(); i != b.end(); ++i) {
    if (a.find(*i) == a.end()) {
      changed = true;
      a.insert(*i);
    }
  }
  return changed;
}

void getUsedVariables(SgExpression* e, std::set<SgInitializedName*>& vars);

std::ostream& operator<<(std::ostream& o, const std::set<SgInitializedName*>& s);

typedef std::map<SgLabelStatement*, std::set<SgGotoStatement*> > GotoMap;

GotoMap buildGotoMap(SgBasicBlock* bb);

#if 0
std::set<SgInitializedName*> computeLiveVars(SgStatement* stmt, const X86AssemblyToCWithVariables& conv, std::map<SgLabelStatement*, std::set<SgInitializedName*> >& liveVarsForLabels, std::set<SgInitializedName*> currentLiveVars, bool actuallyRemove);

void removeDeadStores(SgBasicBlock* switchBody, const X86AssemblyToCWithVariables& conv);
#endif

void removeEmptyBasicBlocks(SgNode* top);

SgLabelStatement* getPreviousLabel(SgStatement* s);

bool endsWithGoto(SgStatement* s);

void structureCode(SgBasicBlock* switchBody);

void flattenBlocksWithoutVariables(SgNode* top);

// void trackVariableDefs(SgStatement* top, const X86AssemblyToCWithVariables& conv); // Mini version of SSA

SgStatement* getBodyOfLabel(SgLabelStatement* l);

// void doSimpleSSA(SgBasicBlock* top, const X86AssemblyToCWithVariables& conv);

void unparseAsSExpressions(std::ostream& o, SgType* t);
void unparseAsSExpressions(std::ostream& o, SgExpression* e);
void unparseAsSExpressions(std::ostream& o, SgStatement* s);

void unparseAsSExpressionsTop(std::ostream& o, SgBasicBlock* bb);

std::set<SgInitializedName*> getVariablesUsedInExpression(SgExpression* e);

struct StatementFilter {
  bool operator()(const VirtualCFG::CFGNode& n) const {
    return (isSgExprStatement(n.getNode()) || isSgVariableDeclaration(n.getNode())) && n.getIndex() == 0;
  }
};

typedef std::map<VirtualCFG::CFGNode, std::set<SgStatement*> > ReachingStatementMap;

bool isStoppingPointStatement(SgNode* n);

void getReachingStatements(VirtualCFG::CFGNode start, ReachingStatementMap& result);

// void renumberVariableDefinitions(SgNode* top, const X86AssemblyToCWithVariables& conv);

struct InterestingStatementFilter {
  bool operator()(const VirtualCFG::CFGNode& n) const {
    // bool ok = n.isInteresting() && (isSgStatement(n.getNode())) && !isSgVariableDeclaration(n.getNode()) && !isSgPragmaDeclaration(n.getNode()) && !isSgGotoStatement(n.getNode()) && !isSgExprStatement(n.getNode());
    bool ok = n.outEdges().size() != 1;
    if (ok) 
      std::cerr << n.id() << std::endl;
    return ok;
  }
};

void moveVariableDeclarationsToTop(SgBasicBlock* top);

#endif // GENERATEDCOPTS_H

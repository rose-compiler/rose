#ifndef SPECIALIZATION_H
#define SPECIALIZATOIN_H

#include "StateRepresentations.h"
#include "ArrayElementAccessData.h"
#include "Analyzer.h"
#include "ExprAnalyzer.h"
#include "RewriteSystem.h"

using namespace std;
using namespace SPRAY;
using namespace CodeThorn;

struct EStateExprInfo {
  const EState* first;
  SgExpression* second;
  bool mark;
EStateExprInfo():first(0),second(0),mark(false){}
EStateExprInfo(const EState* estate,SgExpression* exp):first(estate),second(exp),mark(false){}
};

typedef vector<EStateExprInfo> ArrayUpdatesSequence;

enum SAR_MODE { SAR_SUBSTITUTE, SAR_SSA };

class NumberAstAttribute : public AstAttribute {
public:
  int index;
  NumberAstAttribute():index(-1){}
  NumberAstAttribute(int index):index(index){}
  string toString() {
    stringstream ss;
    ss<<index;
    return ss.str();
  }
};

class Specialization {
 public:
  void transformArrayProgram(SgProject* root, Analyzer* analyzer);
  void extractArrayUpdateOperations(Analyzer* ana,
                                    ArrayUpdatesSequence& arrayUpdates,
                                    RewriteSystem& rewriteSystem,
                                    bool useConstExprSubstRule=true
                                    );
  void printUpdateInfos(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  void writeArrayUpdatesToFile(ArrayUpdatesSequence& arrayUpdates, string filename, SAR_MODE sarMode, bool performSorting);
  void createSsaNumbering(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);

 private:
  int substituteConstArrayIndexExprsWithConst(VariableIdMapping* variableIdMapping, ExprAnalyzer* exprAnalyzer, const EState* estate, SgNode* root);
  int substituteVariablesWithConst(VariableIdMapping* variableIdMapping, const PState* pstate, SgNode *node);
  bool isAtMarker(Label lab, const EState* estate);
  SgNode* findDefAssignOfArrayElementUse(SgPntrArrRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping);
  SgNode* findDefAssignOfUse(SgVarRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping);
  void attachSsaNumberingtoDefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  void substituteArrayRefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping, SAR_MODE sarMode);
  //SgExpressionPtrList& getInitializerListOfArrayVariable(VariableId arrayVar, VariableIdMapping* variableIdMapping);
  string flattenArrayInitializer(SgVariableDeclaration* decl, VariableIdMapping* variableIdMapping);
  void transformArrayAccess(SgNode* node, VariableIdMapping* variableIdMapping);
};

#endif

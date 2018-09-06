#ifndef TYPEFORGE_ANALYSIS_LIST_H
#define TYPEFORGE_ANALYSIS_H

#include "sage3basic.h"
#include "AstTerm.h"

class TFAnalysis{
  public:
    TFAnalysis();
    int variableSetAnalysis(SgProject* project, SgType* matchType, bool base);
    void writeAnalysis(std::string fileName);
  private:
    void linkVariables(SgNode* key, SgType* type, SgExpression* exp);
    void addToMap(SgNode* originNode, SgNode* targetNode);
    std::list<std::set<SgNode*>*> listSets;
    std::map<SgNode*,std::set<SgNode*>*> setMap;
};

#endif

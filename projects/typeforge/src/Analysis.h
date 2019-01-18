#ifndef TYPEFORGE_ANALYSIS_H
#define TYPEFORGE_ANALYSIS_H

#include "sage3basic.h"
#include "AstTerm.h"

class Analysis{
  public:
    Analysis();
    int variableSetAnalysis(SgProject* project, SgType* matchType, bool base);
    void writeAnalysis(SgType* type, std::string toTypeString);
    void writeGraph(std::string fileName);
    std::set<SgNode*>* getSet(SgNode* node);
  private:
    void linkVariables(SgNode* key, SgType* type, SgExpression* exp);
    void addToMap(SgNode* originNode, SgNode* targetNode);
    std::list<std::set<SgNode*>*> listSets;
    std::map<SgNode*,std::set<SgNode*>*> setMap;
};

#endif

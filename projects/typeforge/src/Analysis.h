#ifndef TYPEFORGE_ANALYSIS_H
#define TYPEFORGE_ANALYSIS_H

#include "sage3basic.h"
#include "AstTerm.h"

namespace Typeforge {

class ToolConfig;

class Analysis {
  public:
    int variableSetAnalysis(SgProject * project, SgType * matchType);
    void toDot(std::string const & fileName) const;
    void appendAnalysis(ToolConfig * tc);

// OLD

    void writeAnalysis(SgType* type, std::string toTypeString);
    void writeGraph(std::string fileName);
    std::set<SgNode*> * getSet(SgNode * node);

  private:
    typedef std::vector<SgExpression *> stack_t;

    stack_t stack;

    void linkVariables(SgNode* key, SgType* type, SgExpression* exp);

    std::set< SgNode * > nodes;
    void addNode(SgNode * n);

    std::map< SgNode *, std::map< SgNode *, std::vector<stack_t> > > edges;
    void addEdge(SgNode * s, SgNode * t);

// OLD

    void addToMap(SgNode* originNode, SgNode* targetNode);
    std::list< std::set<SgNode*> > listSets;
    std::map< SgNode *, std::set<SgNode *> > setMap;
};

}

#endif

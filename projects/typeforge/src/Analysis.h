
#ifndef TYPEFORGE_ANALYSIS_H
#  define TYPEFORGE_ANALYSIS_H

#include "sage3basic.h"
#include "AstTerm.h"

namespace Typeforge {

class ToolConfig;

class Analysis {
  private:

    typedef std::vector<SgExpression *> stack_t;
    stack_t stack;

    // Nodes

    struct node_tuple_t {
      node_tuple_t(SgNode*);

      std::string handle;
      std::string cname;
      std::string position;

      SgScopeStatement * scope;
      SgType * type;
    };

    std::map< SgNode * , node_tuple_t > node_map;
    std::map< std::string , SgNode * > handle_map;

    std::string addNode(SgNode * n);

    // Edge

    std::map< SgNode *, std::map< SgNode *, std::vector<stack_t> > > edges;
    void addEdge(SgNode * s, SgNode * t);

    // Compute link

    void traverse(SgGlobal * g);
    void linkVariables(SgNode* key, SgType* type, SgExpression* exp);

  public:
    void initialize(SgProject * p = nullptr);

    // Accessors

    SgNode * getNode(std::string const & h) const;
    std::string getHandle(SgNode * n) const;

    std::string getClass(SgNode * n) const;
    std::string getClass(std::string const & h) const;

    std::string getPosition(SgNode * n) const;
    std::string getPosition(std::string const & h) const;

    SgType * getType(SgNode * n) const;
    SgType * getType(std::string const & h) const;

    SgScopeStatement * getScope(SgNode * n) const;
    SgScopeStatement * getScope(std::string const & h) const;

    std::vector<SgNode *> const & getEdgeIn(SgNode * n) const;

    // Generate the graph of the model

    void toDot(std::string const & fileName) const;

  friend ::Typeforge::ToolConfig;
};

extern SgProject * project;
extern Analysis typechain;

}

#endif

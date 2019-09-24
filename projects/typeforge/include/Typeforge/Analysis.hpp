
#ifndef TYPEFORGE_ANALYSIS_H
#  define TYPEFORGE_ANALYSIS_H

#include "sage3basic.h"

#include "AstTerm.h"

namespace Typeforge {

SgType * stripType(SgType * type, bool strip_std_vector = true);
bool isTypeBasedOn(SgType * type, SgType * base, bool strip_type = false);

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

      SgNode * scope;
      SgType * type;
    };

    std::map< SgNode * , node_tuple_t > node_map;
    std::map< std::string , SgNode * > handle_map;

    std::string addNode(SgNode * n);

    // Edge

    std::map< SgNode *, std::map< SgNode *, std::vector<stack_t> > > edges;
    void addEdge(SgNode * s, SgNode * t);

    // Discover nodes of interrest

    void traverse(SgGlobal * g);
    void traverseVariableDeclarations(SgGlobal * g);
    void traverseFunctionDeclarations(SgGlobal * g);
    void traverseFunctionDefinitions(SgGlobal * g);

    // Find links between nodes

    void linkVariables(SgNode * key, SgExpression * exp);

  public:
    void initialize(SgProject * p = nullptr);

    // Accessors

    SgNode * getNode(std::string const & h) const;
    std::string getHandle(SgNode * n) const;

    std::string getClass(SgNode * n) const;

    std::string getPosition(SgNode * n) const;

    SgType * getType(SgNode * n) const;

    SgNode * getScope(SgNode * n) const;

    std::vector<SgNode *> const & getEdgeIn(SgNode * n) const;

    //

    void getGlobals    ( std::vector<SgVariableDeclaration *> & decls, std::string const & location) const;
    void getLocals     ( std::vector<SgVariableDeclaration *> & decls, std::string const & location) const;
    void getFields     ( std::vector<SgVariableDeclaration *> & decls, std::string const & location) const;
    void getFunctions  ( std::vector<SgFunctionDeclaration *> & decls, std::string const & location) const;
    void getMethods    ( std::vector<SgFunctionDeclaration *> & decls, std::string const & location) const;
    void getParameters ( std::vector<SgInitializedName     *> & decls, std::string const & location) const;
    void getCallExp    ( std::vector<SgFunctionCallExp     *> & exprs, std::string const & location) const;

    // Create Clusters

    void buildChildSets(std::map<SgNode *, std::set<SgNode *> > & childsets, SgType * base) const;
    void buildClusters(std::vector<std::set<SgNode *> > & clusters, SgType * base) const;

    // Generate the graph of the model

    void toDot(std::string const & fileName, SgType * base = nullptr) const;

  friend ::Typeforge::ToolConfig;
};

extern SgProject * project;
extern Analysis typechain;

}

#endif

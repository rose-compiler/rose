#ifndef CLASS_HIERARCHY_GRAPH_H
#define CLASS_HIERARCHY_GRAPH_H

typedef std::list<SgGraphNode *> ClassHierarchyNodePtrList;


class ClassHierarchyWrapper
{
  SgNode *root;
  SgIncidenceDirectedGraph* classGraph;

  public:

  enum EdgeDirection
  {
    EdgeOut,
    EdgeIn
  } ;
  ClassHierarchyWrapper( SgNode *node );
  SgClassDefinitionPtrList getSubclasses( SgClassDefinition * );
  SgClassDefinitionPtrList getDirectSubclasses( SgClassDefinition * );
  SgClassDefinitionPtrList getAncestorClasses( SgClassDefinition * );

  void setAST( SgNode *proj );

#ifdef HAVE_SQLITE3
  ClassHierarchyWrapper(  );

  void writeHierarchyToDB ( sqlite3x::sqlite3_connection& gDB );
  std::list<std::string> getDirectSubclasses( std::string className ,  sqlite3x::sqlite3_connection& gDB );
  std::list<std::string> getSubclasses( std::string className ,  sqlite3x::sqlite3_connection& gDB );
#endif

  SgIncidenceDirectedGraph* getClassHierarchyGraph();
  SgGraphNode* findNode(SgNode*);

  private:
  SgClassDefinitionPtrList getHierarchy ( SgClassDefinition *, EdgeDirection );
};


// endif for CLASS_HIERARCHY_GRAPH_H
#endif

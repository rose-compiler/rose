#ifndef CLASS_HIERARCHY_GRAPH_H
#define CLASS_HIERARCHY_GRAPH_H

#ifdef HAVE_MYSQL
// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
   #include <GlobalDatabaseConnection.h>
#endif
#endif

class ClassHierarchyNode : public MultiGraphElem
   {
     public:
       SgClassDefinition *classDefinition;
       ClassHierarchyNode( SgClassDefinition *cls ); 
       virtual std::string toString() const; 
   };


typedef DAGCreate<ClassHierarchyNode, ClassHierarchyEdge> ClassHierarchy;

typedef std::list<ClassHierarchyNode *> ClassHierarchyNodePtrList;



class ClassHierarchyWrapper
{
    SgNode *root;
    ClassHierarchy classGraph;
    
  public:

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

    ClassHierarchy* getClassHierarchyGraph();
    ClassHierarchyNode* findNode(SgNode*);
    ClassHierarchyEdge* findEdge(ClassHierarchyNode*,ClassHierarchyNode*);
    //#endif

   private:
    //map<SgClassDefinition *, ClassHierarchyNode *> definition2node;
    SgClassDefinitionPtrList getHierarchy ( SgClassDefinition *,ClassHierarchy::EdgeDirection );
};


// endif for CLASS_HIERARCHY_GRAPH_H
#endif

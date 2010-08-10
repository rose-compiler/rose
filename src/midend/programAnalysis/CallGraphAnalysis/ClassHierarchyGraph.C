// tps : Switching from rose.h to sage3 changed size from 17,7 MB to 7,3MB
#include "sage3basic.h"

#include "CallGraph.h"

// #include <ClassHierarchyGraph.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


#ifdef HAVE_SQLITE3
#include "sqlite3x.h"
#endif

  void 
ClassHierarchyWrapper::setAST( SgNode *proj )
{
  ROSE_ASSERT ( isSgProject( proj ) );
  root = proj;
}


SgIncidenceDirectedGraph* ClassHierarchyWrapper::getClassHierarchyGraph(){
  return classGraph;

};

SgGraphNode*
ClassHierarchyWrapper::findNode(SgNode* nodeToFind){
  SgGraphNode* returnNode = NULL;

  rose_graph_integer_node_hash_map & nodes =
    classGraph->get_node_index_to_node_map ();


  for( rose_graph_integer_node_hash_map::iterator it = nodes.begin();
      it != nodes.end(); ++it )
  {
    SgNode* currentNode = it->second->get_SgNode();
    if(currentNode == nodeToFind ){

      returnNode = it->second;
      break;
    };

  }

  return returnNode;
};

#if 0
  ClassHierarchyEdge*
ClassHierarchyWrapper::findEdge ( ClassHierarchyNode* src, ClassHierarchyNode* snk)
{ 
  ROSE_ASSERT(src!=NULL);
  ROSE_ASSERT(snk!=NULL);
  // bool edge_exist = false;     
  ClassHierarchyEdge* returnEdge = NULL;
  //std::cout << "iterator1\n";
  for ( ClassHierarchy::EdgeIterator i = classGraph.GetNodeEdgeIterator(src,GraphAccess::EdgeOut); !i.ReachEnd(); ++i) 
  {
    //std::cout <<"iterator2" << endl;
    ClassHierarchyEdge* currentEdge = i.Current();
    ClassHierarchyNode* endPoint= classGraph.GetEdgeEndPoint(currentEdge, GraphAccess::EdgeIn);

    if(snk == endPoint){
      //std::cout << "NODE EXISTS" << std::endl;
      returnEdge = currentEdge;
      break;
    }else{
      //std::cout << "NODE DO NOT EXIST" << std::endl;
    }
  }



  return returnEdge; 
}
#endif

ClassHierarchyWrapper::ClassHierarchyWrapper( SgNode *node )
{
  ROSE_ASSERT ( isSgProject( node ) );
  classGraph = new SgIncidenceDirectedGraph("Class Inheritance Graph");
  root = node;

  Rose_STL_Container<SgNode *> allCls;
  allCls = NodeQuery::querySubTree ( root, V_SgClassDefinition );


  // build the class hierarchy
  // start by iterating through all the classes
  for ( Rose_STL_Container<SgNode *>::iterator it = allCls.begin(); it != allCls.end(); it++ )
  {
    SgClassDefinition *clsDescDef = isSgClassDefinition( *it );
    SgBaseClassPtrList &baseClses = clsDescDef->get_inheritances();

    // for each iterate through their parents and add parent - child relationship to the graph
    for ( SgBaseClassPtrList::iterator it = baseClses.begin(); it != baseClses.end(); it++ )
    {
      //AS (032806) Added fix to get the defining class declaration
      SgClassDeclaration *baseCls = isSgClassDeclaration(( *it )->get_base_class()->get_definingDeclaration());
      ROSE_ASSERT(baseCls!=NULL);
      SgClassDefinition *baseClsDef = baseCls->get_definition();
      ROSE_ASSERT ( baseClsDef != NULL);

      SgGraphNode *n1 = findNode(baseClsDef);
      if(n1 == NULL)
      {
        n1 = new SgGraphNode( baseClsDef->unparseToString() );
        n1->set_SgNode(baseClsDef);
        classGraph->addNode( n1 );

      }


      SgGraphNode *n2 = findNode(clsDescDef);
      if(n2==NULL)
      {
        n2 = new SgGraphNode( clsDescDef->unparseToString() );
        n2->set_SgNode(clsDescDef);
        classGraph->addNode( n2 );
      }

      SgGraphEdge *e = NULL;//findEdge(n1,n2);
      if(e == NULL)
        classGraph->addDirectedEdge(n1,n2, "inheritance");
      //classGraph.addEdge( n1, n2, e );
    }
  }
}

  SgClassDefinitionPtrList
ClassHierarchyWrapper::getSubclasses( SgClassDefinition *cls )
{
  return getHierarchy ( cls, EdgeOut );
}

  SgClassDefinitionPtrList
ClassHierarchyWrapper::getAncestorClasses( SgClassDefinition *cls )
{
  return getHierarchy ( cls, EdgeIn );
}

  SgClassDefinitionPtrList
ClassHierarchyWrapper::getHierarchy ( SgClassDefinition * cls, EdgeDirection dir )
{

  SgClassDefinitionPtrList classList;
  ROSE_ASSERT ( cls );

  //cout << "Class " << cls->get_qualified_name().str() << "\n";

  set<SgClassDefinition *> visited;
  visited.insert( cls );

  // find descendants / ascendants of class currently visited - BFS
#if 1
  rose_graph_integer_node_hash_map & nodes =
    classGraph->get_node_index_to_node_map ();


  if(dir == EdgeOut){
    //Map of all graphEdges in the graph
    rose_graph_integer_edge_hash_multimap & outEdges
      = classGraph->get_node_index_to_edge_multimap_edgesOut ();


    for( rose_graph_integer_node_hash_map::iterator i = nodes.begin();
        i != nodes.end(); ++i )
    {
      SgGraphNode *crtNode = i->second;
      SgClassDefinition *cC = isSgClassDefinition(crtNode->get_SgNode());
      ROSE_ASSERT(cC != NULL);
      if ( visited.find( cC ) != visited.end() )
        // iterate through the edges of the current node
        for( rose_graph_integer_edge_hash_multimap::const_iterator edgeIterator = outEdges.find(i->first);
            edgeIterator != outEdges.end(); ++edgeIterator )
        {
          SgDirectedGraphEdge *edge = isSgDirectedGraphEdge(edgeIterator->second);
          ROSE_ASSERT( edge != NULL );

          SgGraphNode *end = edge->get_to();
          ROSE_ASSERT ( end );
          //cout << "Descendant " << end->toString() << "\n";
          ROSE_ASSERT( isSgClassDefinition(end->get_SgNode()) != NULL );
          classList.push_back( isSgClassDefinition(end->get_SgNode()) );
          visited.insert( isSgClassDefinition(end->get_SgNode()) );
        }

    }
  }else{

    //Map of all graphEdges in the graph
    rose_graph_integer_edge_hash_multimap & inEdges
      = classGraph->get_node_index_to_edge_multimap_edgesIn ();


    for( rose_graph_integer_node_hash_map::iterator i = nodes.begin();
        i != nodes.end(); ++i )
    {
      SgGraphNode *crtNode = i->second;
      SgClassDefinition *cC = isSgClassDefinition(crtNode->get_SgNode());
      ROSE_ASSERT(cC != NULL);
      if ( visited.find( cC ) != visited.end() )
        // iterate through the edges of the current node
        for( rose_graph_integer_edge_hash_multimap::const_iterator edgeIterator = inEdges.find(i->first);
            edgeIterator != inEdges.end(); ++edgeIterator )
        {
          SgDirectedGraphEdge *edge = isSgDirectedGraphEdge(edgeIterator->second);
          ROSE_ASSERT( edge != NULL );

          SgGraphNode *end = edge->get_from();
          ROSE_ASSERT ( end );
          //cout << "Descendant " << end->toString() << "\n";
          ROSE_ASSERT( isSgClassDefinition(end->get_SgNode()) != NULL );
          classList.push_back( isSgClassDefinition(end->get_SgNode()) );
          visited.insert( isSgClassDefinition(end->get_SgNode()) );
        }

    }
  }

#else
  for ( nodeIterator = classGraph.GetNodeIterator(); !nodeIterator.ReachEnd(); nodeIterator++ )
  {
    ClassHierarchyNode *crtNode = nodeIterator.Current();
    SgClassDefinition *cC = crtNode->classDefinition;
    if ( visited.find( cC ) != visited.end() )
      // iterate through the edges of the current node
      for ( edgeIterator = classGraph.GetNodeEdgeIterator( crtNode, dir ); !edgeIterator.ReachEnd();
          edgeIterator++ )
      {
        ClassHierarchyEdge *edge = edgeIterator.Current();
        ClassHierarchyNode *end =
          dynamic_cast<ClassHierarchyNode *>( classGraph.GetEdgeEndPoint( edge, dir_rev ) );
        ROSE_ASSERT ( end );
        //cout << "Descendant " << end->toString() << "\n";
        classList.push_back( end->classDefinition );
        visited.insert( end->classDefinition );
      }
  }
#endif
  return classList;
}

  SgClassDefinitionPtrList
ClassHierarchyWrapper::getDirectSubclasses ( SgClassDefinition * cls )
{
  SgClassDefinitionPtrList classList;
  ROSE_ASSERT ( cls );

  //cout << "Class " << cls->get_qualified_name().str() << "\n";

  // find descendants / ascendants of class currently visited - BFS
  rose_graph_integer_node_hash_map & nodes =
    classGraph->get_node_index_to_node_map ();

  //Map of all graphEdges in the graph
  rose_graph_integer_edge_hash_multimap & outEdges
    = classGraph->get_node_index_to_edge_multimap_edgesOut ();


  for( rose_graph_integer_node_hash_map::iterator i = nodes.begin();
      i != nodes.end(); ++i )
  {
    SgGraphNode *crtNode = i->second;
    SgClassDefinition *cC = isSgClassDefinition(crtNode->get_SgNode());
    ROSE_ASSERT( cC != NULL );
    // iterate through the edges of the current node
    for( rose_graph_integer_edge_hash_multimap::const_iterator edgeIterator = outEdges.find(i->first);
        edgeIterator != outEdges.end(); ++edgeIterator )
    {
      SgDirectedGraphEdge *edge = isSgDirectedGraphEdge(edgeIterator->second);
      ROSE_ASSERT( edge != NULL );

      SgGraphNode *end = edge->get_to();
      ROSE_ASSERT ( end );
      //cout << "Descendant " << end->toString() << "\n";
      ROSE_ASSERT( isSgClassDefinition(end->get_SgNode()) != NULL );
      classList.push_back( isSgClassDefinition(end->get_SgNode()) );
    }

  }

#if 0

  for ( nodeIterator = classGraph.GetNodeIterator(); !nodeIterator.ReachEnd(); nodeIterator++ )
  {
    ClassHierarchyNode *crtNode = nodeIterator.Current();
    SgClassDefinition *cC = crtNode->classDefinition;
    if ( cC == cls )
      // iterate through the edges of the current node
      for ( edgeIterator = classGraph.GetNodeEdgeIterator( crtNode, ClassHierarchy::EdgeOut );
          !edgeIterator.ReachEnd(); edgeIterator++ )
      {
        ClassHierarchyEdge *edge = edgeIterator.Current();
        ClassHierarchyNode *end =
          dynamic_cast<ClassHierarchyNode *>( classGraph.GetEdgeEndPoint( edge, ClassHierarchy::EdgeIn ) );
        ROSE_ASSERT ( end );
        //cout << "Descendant " << end->toString() << "\n";
        classList.push_back( end->classDefinition );
      }
  }
#endif
  return classList;
}


#ifdef HAVE_SQLITE3
// generates the class hierarchy shema ( not the classes themselves )

ClassHierarchyWrapper::ClassHierarchyWrapper(  ){};

  std::list<string>
ClassHierarchyWrapper::getDirectSubclasses( string className,  sqlite3x::sqlite3_connection& gDB )
{
  std::list<string> retList;


  string command = "SELECT subclass from Hierarchy WHERE class = \"" + className + "\";";

  sqlite3x::sqlite3_command cmd(gDB,command.c_str());
  sqlite3x::sqlite3_reader r = cmd.executereader();


  while( r.read() )
  {
    string cls = r.getstring(0);
    retList.push_back( cls );
  }
  return retList;
}

  std::list<string>
ClassHierarchyWrapper::getSubclasses( string className ,  sqlite3x::sqlite3_connection& gDB )
{
  std::list<string> retList;
  std::list<string> toVisit;
  toVisit.push_back( className );

  while ( !toVisit.empty() )
  {
    string crt = toVisit.front();
    toVisit.pop_front();
    std::list<string> temp = getDirectSubclasses( crt,gDB );
    for ( std::list<string>::iterator i = temp.begin(); i != temp.end(); i++ )
    {
      bool alreadyExists = false;
      for ( std::list<string>::iterator j = retList.begin(); j != retList.end(); j++ )
        if ( *j == *i )
        {
          alreadyExists = true;
          break;
        }
      if ( !alreadyExists )
      {
        retList.push_back( *i );
        toVisit.push_back( *i );
      }
    }
  }
  return retList;
}

// writes the class hierarchy to the specified database
  void
ClassHierarchyWrapper::writeHierarchyToDB ( sqlite3x::sqlite3_connection& gDB )
{

  // find descendants of class currently visited - BFS

  rose_graph_integer_node_hash_map & nodes =
    classGraph->get_node_index_to_node_map ();

  //Map of all graphEdges in the graph
  rose_graph_integer_edge_hash_multimap & outEdges
    = classGraph->get_node_index_to_edge_multimap_edgesOut ();


  for( rose_graph_integer_node_hash_map::iterator i = nodes.begin();
      i != nodes.end(); ++i )
  {
    SgGraphNode *crtNode = i->second;
    SgClassDefinition *cC = isSgClassDefinition(crtNode->get_SgNode());
    ROSE_ASSERT(cC != NULL);
    // iterate through the edges of the current node
    for( rose_graph_integer_edge_hash_multimap::const_iterator edgeIterator = outEdges.find(i->first);
        edgeIterator != outEdges.end(); ++edgeIterator )
    {
      SgDirectedGraphEdge *edge = isSgDirectedGraphEdge(edgeIterator->second);
      ROSE_ASSERT( edge != NULL );

      SgGraphNode *end = edge->get_to();
      ROSE_ASSERT ( end );

      ROSE_ASSERT ( end );
      SgClassDefinition *dC = isSgClassDefinition(end->get_SgNode());
      ROSE_ASSERT(dC != NULL);

      std::ostringstream existQuery;
      existQuery << "select count(Class) from Hierarchy where Class=\""<<cC->get_qualified_name().getString() 
        <<"\" AND Subclass=\""<<dC->get_qualified_name().getString()<< "\" limit 1";

      if( sqlite3x::sqlite3_command(gDB, existQuery.str().c_str()).executeint() == 0 )
      {

        string command = "INSERT INTO Hierarchy VALUES (\"" + cC->get_qualified_name().getString() + "\", \""
          + dC->get_qualified_name().getString() + "\", \"" + cC->get_file_info()->get_filename() + "\", \""
          + dC->get_file_info()->get_filename() + "\");";
        cout << "Executing: " << command << "\n";
        //q->execute();
        gDB.executenonquery(command.c_str());
      }
    }
  }

#if 0

  for ( nodeIterator = classGraph.GetNodeIterator(); !nodeIterator.ReachEnd(); nodeIterator++ )
  {
    ClassHierarchyNode *crtNode = nodeIterator.Current();
    SgClassDefinition *cC = crtNode->classDefinition;
    for ( edgeIterator = classGraph.GetNodeEdgeIterator( crtNode, ClassHierarchy::EdgeOut );
        !edgeIterator.ReachEnd(); edgeIterator++ )
    {
      ClassHierarchyEdge *edge = edgeIterator.Current();
      ClassHierarchyNode *end =
        dynamic_cast<ClassHierarchyNode *>( classGraph.GetEdgeEndPoint( edge, ClassHierarchy::EdgeIn ) );
      ROSE_ASSERT ( end );
      SgClassDefinition *dC = end->classDefinition;
      //int k;

      std::ostringstream existQuery;
      existQuery << "select count(Class) from Hierarchy where Class=\""<<cC->get_qualified_name().getString() 
        <<"\" AND Subclass=\""<<dC->get_qualified_name().getString()<< "\" limit 1";

      if( sqlite3x::sqlite3_command(gDB, existQuery.str().c_str()).executeint() == 0 )
      {

        command = "INSERT INTO Hierarchy VALUES (\"" + cC->get_qualified_name().getString() + "\", \""
          + dC->get_qualified_name().getString() + "\", \"" + cC->get_file_info()->get_filename() + "\", \""
          + dC->get_file_info()->get_filename() + "\");";
        cout << "Executing: " << command << "\n";
        //q->execute();
        gDB.executenonquery(command.c_str());
      }
    }
  }
#endif
}
#endif

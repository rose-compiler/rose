// tps : Switching from rose.h to sage3 changed size from 17,7 MB to 7,3MB
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "CallGraph.h"
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#define foreach BOOST_FOREACH
using namespace std;
using namespace boost;

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

ClassHierarchyWrapper::ClassHierarchyWrapper(SgNode *node)
{
    ROSE_ASSERT(isSgProject(node));
    ROSE_ASSERT(node != NULL);
    classGraph = new SgIncidenceDirectedGraph("Class Inheritance Graph");
    root = node;

    Rose_STL_Container<SgNode *> allCls;
    allCls = NodeQuery::querySubTree(root, V_SgClassDefinition);


    // build the class hierarchy
    // start by iterating through all the classes
    for (Rose_STL_Container<SgNode *>::iterator it = allCls.begin(); it != allCls.end(); it++)
    {
        SgClassDefinition *clsDescDef = isSgClassDefinition(*it);
        SgBaseClassPtrList &baseClses = clsDescDef->get_inheritances();

        // for each iterate through their parents and add parent - child relationship to the graph
        for (SgBaseClassPtrList::iterator it = baseClses.begin(); it != baseClses.end(); it++)
        {
            //AS (032806) Added fix to get the defining class declaration
            SgClassDeclaration *baseCls = isSgClassDeclaration((*it)->get_base_class()->get_definingDeclaration());
            ROSE_ASSERT(baseCls != NULL);
            SgClassDefinition *baseClsDef = baseCls->get_definition();
            ROSE_ASSERT(baseClsDef != NULL);

            SgGraphNode *n1 = findNode(baseClsDef);
            if (n1 == NULL)
            {
                n1 = new SgGraphNode();
                n1->set_SgNode(baseClsDef);
                classGraph->addNode(n1);
            }

            SgGraphNode *n2 = findNode(clsDescDef);
            if (n2 == NULL)
            {
                n2 = new SgGraphNode();
                n2->set_SgNode(clsDescDef);
                classGraph->addNode(n2);
            }

           classGraph->addDirectedEdge(n1, n2);
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

  return classList;
}

  
ClassHierarchyWrapper::~ClassHierarchyWrapper()
{
    //Delete all the edges we allocated
    rose_graph_integer_edge_hash_multimap & outEdges = classGraph->get_node_index_to_edge_multimap_edgesOut();
    int id;
    SgGraphEdge* edge;
    
    foreach (tie(id, edge), outEdges)
    {
        delete edge;
    }
    
    rose_graph_integer_node_hash_map& nodes = classGraph->get_node_index_to_node_map();
    SgGraphNode* node;
    foreach (tie(id, node), nodes)
    {
        delete node;
    }
    
    delete classGraph;
}


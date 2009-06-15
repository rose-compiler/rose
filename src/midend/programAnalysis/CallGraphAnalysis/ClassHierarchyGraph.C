
#include <rose.h>

#include "CallGraph.h"

// #include <ClassHierarchyGraph.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


ClassHierarchyNode::ClassHierarchyNode( SgClassDefinition *cls ) : MultiGraphElem( NULL ), classDefinition( cls ) 
 {}

std::string 
ClassHierarchyNode::toString() const 
 { 
  return classDefinition->get_qualified_name().getString(); 
 }


void 
ClassHierarchyWrapper::setAST( SgNode *proj )
 {
  ROSE_ASSERT ( isSgProject( proj ) );
  root = proj;
 }


ClassHierarchy* ClassHierarchyWrapper::getClassHierarchyGraph(){
            return &classGraph;

};

ClassHierarchyNode*
ClassHierarchyWrapper::findNode(SgNode* nodeToFind){
     ClassHierarchyNode* returnNode = NULL;
     for (ClassHierarchy::NodeIterator i = classGraph.GetNodeIterator(); !i.ReachEnd(); ++i) 
        {
           ClassHierarchyNode* node = i.Current();
	   SgNode* currentNode = node->classDefinition;
	   if(currentNode == nodeToFind ){

               returnNode = node;
	       break;
	   };

        }

    return returnNode;
};

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

ClassHierarchyWrapper::ClassHierarchyWrapper( SgNode *node )
{
  ROSE_ASSERT ( isSgProject( node ) );
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

	  ClassHierarchyNode *n1 = findNode(baseClsDef);
	  if(n1 == NULL)
	  {
	       n1 = new ClassHierarchyNode( baseClsDef );
           classGraph.addNode( n1 );

	  }


	  ClassHierarchyNode *n2 = findNode(clsDescDef);
	  if(n2==NULL)
	  {
	       n2 = new ClassHierarchyNode( clsDescDef );
     	   classGraph.addNode( n2 );
	  }

	  ClassHierarchyEdge *e = NULL;//findEdge(n1,n2);
	  if(e == NULL)
	       e = new ClassHierarchyEdge( "inheritance" );
	  classGraph.addEdge( n1, n2, e );
	}
    }
}

SgClassDefinitionPtrList
ClassHierarchyWrapper::getSubclasses( SgClassDefinition *cls )
{
  return getHierarchy ( cls, ClassHierarchy::EdgeOut );
}

SgClassDefinitionPtrList
ClassHierarchyWrapper::getAncestorClasses( SgClassDefinition *cls )
{
  return getHierarchy ( cls, ClassHierarchy::EdgeIn );
}

SgClassDefinitionPtrList
ClassHierarchyWrapper::getHierarchy ( SgClassDefinition * cls, ClassHierarchy::EdgeDirection dir )
{

  SgClassDefinitionPtrList classList;
  ROSE_ASSERT ( cls );

  //cout << "Class " << cls->get_qualified_name().str() << "\n";
  ClassHierarchy::EdgeIterator edgeIterator;
  ClassHierarchy::NodeIterator nodeIterator;
  ClassHierarchy::EdgeDirection dir_rev =
    ( dir == ClassHierarchy::EdgeOut ? ClassHierarchy::EdgeIn : ClassHierarchy::EdgeOut );

  set<SgClassDefinition *> visited;
  visited.insert( cls );

  // find descendants / ascendants of class currently visited - BFS
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
  return classList;
}

SgClassDefinitionPtrList
ClassHierarchyWrapper::getDirectSubclasses ( SgClassDefinition * cls )
{
  SgClassDefinitionPtrList classList;
  ROSE_ASSERT ( cls );

  //cout << "Class " << cls->get_qualified_name().str() << "\n";
  ClassHierarchy::EdgeIterator edgeIterator;
  ClassHierarchy::NodeIterator nodeIterator;

  // find descendants / ascendants of class currently visited - BFS
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

  string command;

  ClassHierarchy::EdgeIterator edgeIterator;
  ClassHierarchy::NodeIterator nodeIterator;

  // find descendants of class currently visited - BFS
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
}
#endif


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

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
//#ifdef HAVE_MYSQL

ClassHierarchyWrapper::ClassHierarchyWrapper( std::string db ) 
 { 
  dbName = db; 
 }

void 
ClassHierarchyWrapper::setDBName( std::string db ) 
 { 
  dbName = db; 
 }

#else

ClassHierarchy* ClassHierarchyWrapper::getClassHierarchyGraph(){
            return &classGraph;

};

#endif

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

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
  //#ifdef HAVE_MYSQL
  dbName = "__defaultClassHierarchy";
#endif

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


// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
//#ifdef HAVE_MYSQL
// generates the class hierarchy shema ( not the classes themselves )
void
ClassHierarchyWrapper::createHierarchySchema ()
{
  GlobalDatabaseConnection *gDB;
  gDB = new GlobalDatabaseConnection( dbName.c_str() );
  gDB->initialize();
  string command = "";
  command = command + "CREATE TABLE Hierarchy ( Class TEXT, Subclass TEXT, ClassFile TEXT, SubclassFile TEXT, "
    + "PRIMARY KEY ( Class, Subclass ) );";
  Query *q = gDB->getQuery();
  q->set( command );
  q->execute();
  if ( q->success() != 0 )
    cout << "Error creating schema: " << q->error() << "\n";
  gDB->shutdown();
}
#endif

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
//#ifdef HAVE_MYSQL
Rose_STL_Container<string>
ClassHierarchyWrapper::getDirectSubclasses( string className )
{
  Rose_STL_Container<string> retList;
  GlobalDatabaseConnection *gDB;
  gDB = new GlobalDatabaseConnection( dbName.c_str() );
  gDB->initialize();
  string command;
  Query *q = gDB->getQuery();
  
  command = "SELECT subclass from Hierarchy WHERE class = \"" + className + "\";";
  q->set( command );
  cout << "Executing: " << q->preview() << "\n";
  Result *res = gDB->select();
  if ( q->success() != 0 )
    cout << "Error reading values: " << q->error() << "\n";
  else
    res->showResult();

  for ( Result::iterator i = res->begin(); i != res->end(); i++ )
    {
      string cls = (*i)[0].get_string();
      retList.push_back( cls );
    }
  gDB->shutdown();
  return retList;
}
#endif

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
//#ifdef HAVE_MYSQL
Rose_STL_Container<string>
ClassHierarchyWrapper::getSubclasses( string className )
{
  Rose_STL_Container<string> retList;
  Rose_STL_Container<string> toVisit;
  toVisit.push_back( className );

  while ( !toVisit.empty() )
    {
      string crt = toVisit.front();
      toVisit.pop_front();
      Rose_STL_Container<string> temp = getDirectSubclasses( crt );
      for ( Rose_STL_Container<string>::iterator i = temp.begin(); i != temp.end(); i++ )
	{
	  bool alreadyExists = false;
	  for ( Rose_STL_Container<string>::iterator j = retList.begin(); j != retList.end(); j++ )
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
#endif

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
//#ifdef HAVE_MYSQL
// writes the class hierarchy to the specified database
void
ClassHierarchyWrapper::writeHierarchyToDB ()
{
  GlobalDatabaseConnection *gDB;
  gDB = new GlobalDatabaseConnection( dbName.c_str() );
  gDB->initialize();
  string command;
  Query *q = gDB->getQuery();
  
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
	  command = "INSERT INTO Hierarchy VALUES (\"" + cC->get_qualified_name().getString() + "\", \""
	    + dC->get_qualified_name().getString() + "\", \"" + cC->get_file_info()->get_filename() + "\", \""
	    + dC->get_file_info()->get_filename() + "\");";
	  q->set( command );
	  cout << "Executing: " << q->preview() << "\n";
	  q->execute();
	  if ( q->success() != 0 )
	    cout << "Error inserting values: " << q->error() << "\n";  
	}
    }
  gDB->shutdown();
}
#endif

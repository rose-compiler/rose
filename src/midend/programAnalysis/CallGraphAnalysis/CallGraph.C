
#include <rose.h>
#include <boost/lexical_cast.hpp>
#include "CallGraph.h"

#ifdef HAVE_SQLITE3
using namespace sqlite3x;
#endif

bool var_SOLVE_FUNCTION_CALLS_IN_DB = false;

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

namespace OutputDot
{
  std::string Translate( std::string r1) 
  {
    std::string r2 = "";
    for (unsigned int i = 0; i < r1.size(); ++i) {
      char c = r1[i];
      if (c == '\"')
        r2 = r2 + "\\\"";
      else if (c == '\n') {
        r2 = r2 + "\\n";
      }
      else
        r2 = r2 + c;
    }
    return r2;
  };
  //! get the name of a vertex
  std::string getVertexName(SgGraphNode* v) 
  { 
    std::string r1 = v->get_name();
    std::string r2 = Translate(r1); 
    return r2;
  };

  std::string getEdgeLabel(SgDirectedGraphEdge* e) 
  {
    std::string r1 = e->get_name();
    return Translate(r1);
  };

  void 
  writeToDOTFile(SgIncidenceDirectedGraph* graph,  const std::string& filename, const std::string& graphname)
  {
    bool debug = false;
    if(debug) std::cerr << " dot output to " << filename << std::endl; 
    std::ofstream dotfile(filename.c_str());

    //Liao, add "" to enclose the graphname,otherwise syntax error for .dot file. 2/22/2008
    dotfile <<  "digraph \"" << graphname <<"\""<< " {\n";

    rose_graph_integer_node_hash_map & nodes =
      graph->get_node_index_to_node_map ();


    for( rose_graph_integer_node_hash_map::iterator it = nodes.begin();
        it != nodes.end(); ++it )
    {
      SgGraphNode* node = it->second;
      dotfile << ((long)node) << "[label=\"" << getVertexName(node) << "\" ];" << std::endl;
    }

    if(debug) std::cerr << " finished add node" << std::endl; // debug

    rose_graph_integer_edge_hash_multimap & outEdges
      = graph->get_node_index_to_edge_multimap_edgesOut ();


    for( rose_graph_integer_edge_hash_multimap::const_iterator outEdgeIt = outEdges.begin();
        outEdgeIt != outEdges.end(); ++outEdgeIt )
    {
      if(debug) std::cerr << " add edge from node ... " << std::endl; // debug
      SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(outEdgeIt->second);
      ROSE_ASSERT(graphEdge!=NULL);

      dotfile << ((long)graphEdge->get_from()) << " -> " << ((long)graphEdge->get_to())
        << "[label=\"" << getEdgeLabel(graphEdge) << "\"];" << std::endl;
    }
    if(debug) std::cerr << " writing content to " << filename << std::endl; // debug
    dotfile <<  "}\n";
  };
};

#ifdef HAVE_SQLITE3

// remove nodes and edges of functions defined in the files of a specified directory
void
filterNodesByDirectory (  sqlite3x::sqlite3_connection& gDB, string directory )
{
  cout << "Filtering system calls...\n";

  string command;

  command = "CREATE TEMP TABLE nb AS SELECT nid FROM Nodes n where n.filename LIKE \"" + directory + "%\";";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Nodes WHERE nid IN (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Edges WHERE nid1 in (SELECT nb.nid FROM nb) OR nid2 in (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  cout << "Done filtering\n";
}



// remove nodes and edges of functions defined in the specified file
void
filterNodesByFilename (  sqlite3x::sqlite3_connection& gDB , string filterFile )
{
  cout << "Filtering system calls...\n";
  string command;

  command = "CREATE TEMP TABLE IF NOT EXISTS nb AS SELECT nid Nodes filename = " + filterFile + ";";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Nodes WHERE nid IN (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Edges WHERE nid1 in (SELECT nb.nid FROM nb) OR nid2 in (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  cout << "Done filtering\n";
}

// remove nodes and edges of functions with a specific name
void
filterNodesByFunction ( sqlite3x::sqlite3_connection& gDB , SgFunctionDeclaration *function )
{
  ROSE_ASSERT ( function );
  cout << "Filtering system calls...\n";
  string functionName = function->get_qualified_name().getString() + function->get_mangled_name().getString();

  string command;

  command = "DELETE FROM Nodes WHERE nid = " + functionName + ";";
  gDB.executenonquery(command.c_str());

  command =  "DELETE FROM Edges WHERE nid1 = " + functionName + "  OR nid2 = " + functionName + ";";
  gDB.executenonquery(command.c_str());

  cout << "Done filtering\n";
}
// DQ (7/28/2005): Don't include the data base





void
filterNodesKeepPaths( sqlite3x::sqlite3_connection& gDB, std::vector<std::string> keepDirs )
{
  cout << "Filtering system calls by path, keeping only files in the correct path...\n";

  gDB.executenonquery("CREATE TEMP TABLE IF NOT EXISTS DELETE_NID(nid TEXT) ;");

  for( unsigned int i = 0; i < keepDirs.size(); i++ )
  {
    std::cout << "Keeping path " << keepDirs[i] << std::endl;
    gDB.executenonquery("INSERT INTO DELETE_NID  select nid from Nodes where filename like \""+keepDirs[i]+"\"");

  }
  gDB.executenonquery("DELETE from Edges where nid1 not in DELETE_NID or nid2 not in DELETE_NID" );
  gDB.executenonquery("DELETE from Nodes where nid  not in DELETE_NID" );
  cout << "Done filtering\n";
}

void filterNodesByFunctionName( sqlite3x::sqlite3_connection& gDB, std::vector<std::string> removeFunctions )
{
  for( unsigned int i = 0; i < removeFunctions.size(); i++ )
  {
    gDB.executenonquery("DELETE from Nodes where nid  like  \""+removeFunctions[i]+"\"");
    gDB.executenonquery("DELETE from Edges where nid1 like  \""+removeFunctions[i]+"\" OR nid2 like \"" +removeFunctions[i]+"\"");

  }
};

// creates a db and tables for storage of graphs
void
createSchema ( sqlite3x::sqlite3_connection& gDB, string dbName )
{
  
  //Query query = (*gDB)->getQuery();
  //gDB.executenonquery("CREATE TABLE IF NOT EXISTS Graph (gid, pgid, filename TEXT);");
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS Nodes (nid TEXT, filename TEXT, label TEXT, def INTEGER, type TEXT, scope TEXT, PRIMARY KEY (nid)););");
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS Edges  (nid1 TEXT, nid2 TEXT, label TEXT, type TEXT, objClass TEXT);");
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS Hierarchy ( Class TEXT, Subclass TEXT, ClassFile TEXT, SubclassFile TEXT, PRIMARY KEY ( Class, Subclass ) );");

  cout << "Tables created\n";
}

sqlite3x::sqlite3_connection* open_db(std::string dbName  )
{
  sqlite3x::sqlite3_connection* gDB = new sqlite3x::sqlite3_connection(dbName.c_str());
  createSchema(*gDB, dbName);
  return gDB;
};


// generate a graph from the DB
SgIncidenceDirectedGraph*
loadCallGraphFromDB (   sqlite3x::sqlite3_connection& gDB)
{
  cout << "Loading...\n";
  SgIncidenceDirectedGraph* graph = new SgIncidenceDirectedGraph("CallGraph");

  string loadNodes = "SELECT nid,label,def,type,scope FROM Nodes;",
//    loadGraph = "SELECT * FROM Graph;",
    loadEdges = "SELECT nid1,nid2,label FROM Edges;";

  Rose_STL_Container<SgGraphNode *> nodeList;

  {
    sqlite3_command cmd(gDB, loadNodes.c_str());
    sqlite3x::sqlite3_reader r = cmd.executereader();


  
    while(r.read()) 
    {
      string nid   = r.getstring(0); //nid is a unique identifier for a function in the schema
      string label = r.getstring(1);
      int is_def   = boost::lexical_cast<int>(r.getstring(2));
      string typeF = r.getstring(3);
      string scope = r.getstring(4);

      ROSE_ASSERT(findNode(graph,nid)== NULL);

      SgGraphNode* n = new SgGraphNode(nid);
      n->addNewAttribute("Properties",new Properties(nid,label,typeF,scope,is_def,false,false) );
      //CallGraphNode *n = new CallGraphNode( nid, label,typeF,scope,is_def, false, false );

      graph->addNode(n);
           //cout << "Node pushed: " << nid << "\t" << n << "\n";
      nodeList.push_back(n);
    }
  }



  //ADDING EDGES
  sqlite3_command cmd(gDB, loadEdges.c_str());
  sqlite3x::sqlite3_reader r = cmd.executereader();


  while(r.read())
    {
      string nid1 = r.getstring(0),
	nid2  = r.getstring(1),
	label = r.getstring(2);

      // find end points of edges, by their name
    //      std::cout << "Calling findNode in outer loop loc7 " << nid1 << " " << nid2 << " " 
     //       << label << std::endl;

      SgGraphNode* startingNode = findNode(graph, nid1);

      SgGraphNode* endNode      = findNode(graph, nid2);

      assert(startingNode);

      if (endNode)
	{
	  //cout << "Nodes retrieved: " << nid1 << "\t" << startingNode << "\t"
	  //   << nid2 << "\t" << endNode << "\n";
          if(findEdge(graph,startingNode,endNode)==NULL)
          {
	    graph->addDirectedEdge(startingNode, endNode, label);
          }
	}
    }

  cout << "After recreating the graph\n";
  return graph;
}



#endif


bool 
dummyFilter::operator() (SgFunctionDeclaration* node) const{
  return true;
};




bool 
CallGraphNode::isDefined () 
 { 
   return hasDefinition; 
 }

void 
CallGraphNode::Dump() const 
{ 
  
   printf ("NODE: function declaration = %p label = %s \n", (var_SOLVE_FUNCTION_CALLS_IN_DB == true ? properties->functionDeclaration: functionDeclaration ) ,label.c_str());

}

std::string 
CallGraphNode::toString() const 
 { 
   return label;
 }

std::string 
CallGraphEdge::toString() const 
 { 
   return label;
 }

void 
CallGraphEdge::Dump() const 
 { 
   printf ("EDGE: label = %s \n",label.c_str()); 
 }

CallGraphEdge::CallGraphEdge ( std::string label ) : MultiGraphElem( NULL ), label( label )
 {
   if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
     properties = new Properties();

 }


bool 
FunctionData::isDefined () 
 { 
   return hasDefinition; 
 }


//Only used when SOLVE_FUNCTION_CALLS_IN_DB is defined
Properties::Properties(){
  functionType = NULL;
  functionDeclaration = NULL;
  invokedClass = NULL;
  isPolymorphic = isPointer = false;
  nid=label=type=scope=functionName="not-set";
};

Properties::Properties(SgFunctionDeclaration* inputFunctionDeclaration){
     functionDeclaration = inputFunctionDeclaration;
     ROSE_ASSERT( functionDeclaration != NULL );

     functionType = inputFunctionDeclaration->get_type()->findBaseType();
     invokedClass = NULL;
     isPointer = isPolymorphic = false;
     nid=label=type=scope=functionName="not-set";
}

//Only used when SOLVE_FUNCTION_CALLS_IN_DB is defined
Properties::Properties(Properties* prop){
    isPointer=prop->isPointer;
    isPolymorphic=prop->isPolymorphic;
    invokedClass=prop->invokedClass;
    functionDeclaration=prop->functionDeclaration;
    functionType=prop->functionType;

    nid=prop->nid;;
    label=prop->label;;
    type=prop->type;
    scope=prop->scope;
    functionName=prop->functionName;

    hasDef=prop->hasDef;
    isPtr=prop->isPtr;
    isPoly=prop->isPoly;
};

Properties::Properties(std::string p_nid, std::string p_label, std::string p_type, std::string p_scope,
     bool p_hasDef, bool p_isPtr, bool p_isPoly)
        : nid(p_nid), label(p_label), type(p_type), scope(p_scope),hasDef(p_hasDef), isPtr(p_isPtr),
        isPoly(p_isPoly)
{
  functionType = NULL;
  functionDeclaration = NULL;
  invokedClass = NULL;
  isPolymorphic = isPointer = false;

  //Filter out the parameters from the nid to get the function name
  functionName = nid.substr(0,nid.size()-label.size());
};


CallGraphNode::CallGraphNode ( std::string nid, std::string dbLabel, std::string typeF, std::string scope,
    bool hasDef, bool isPtr, bool isPoly )
   : MultiGraphElem( NULL ), label( nid )
{
  hasDefinition = hasDef;
  properties = new Properties();
  properties->isPointer = isPtr;;
  properties->isPolymorphic = isPoly;
  properties->invokedClass = NULL;
  if ( properties->isPolymorphic )
    cout << "IsPolymorphic is set to true on "
         << properties->invokedClass->get_qualified_name().getString() << "\n";
  properties->functionDeclaration = NULL;
  functionDeclaration = NULL;
  properties->functionType = NULL;

  properties->label = dbLabel;
  properties->type  = typeF;
  properties->scope = scope;
  properties->nid   = nid;
  properties->functionName = nid.substr(0,nid.size()-properties->label.size());

};


CallGraphNode::CallGraphNode ( std::string label, SgFunctionDeclaration* fctDeclaration, SgType *ty,
	        bool hasDef, bool isPtr, bool isPoly, SgClassDefinition *invokedCls )
	   : MultiGraphElem( NULL ), label( label )
  {
  //ROSE_ASSERT(invokedCls != NULL);
 // ROSE_ASSERT(fctDeclaration != NULL);

  functionDeclaration = NULL;

  hasDefinition = hasDef;
  properties = new Properties();
  properties->isPointer = isPtr;;
  properties->isPolymorphic = isPoly;
  properties->invokedClass = invokedCls;
  if ( properties->isPolymorphic )
    cout << "IsPolymorphic is set to true on "
         << properties->invokedClass->get_qualified_name().getString() << "\n";
  properties->functionDeclaration = fctDeclaration;
  properties->functionType = ty;
  if ( properties->functionDeclaration != NULL )
    {
       ROSE_ASSERT ( isSgFunctionDeclaration( properties->functionDeclaration ) );
       ROSE_ASSERT ( ty && ty == properties->functionDeclaration->get_type()->findBaseType() );
     }
 }

CallGraphNode::CallGraphNode ( std::string lbl, Properties *fctProps, bool hasDef ) : MultiGraphElem ( NULL )
                                                                                               , label(lbl)

 {
   properties = fctProps;
   hasDefinition = hasDef;
 }



CallGraphNode::CallGraphNode ( std::string lbl, SgFunctionDeclaration* fctDeclaration, bool hasDef )
	   : MultiGraphElem(NULL), label(lbl), functionDeclaration(fctDeclaration) {
   hasDefinition = hasDef;

 }



CallGraphBuilder::CallGraphBuilder( SgProject *proj )
  {
    project = proj;
    graph = NULL;
  }

SgIncidenceDirectedGraph*
CallGraphBuilder::getGraph() 
  { 
    return graph; 
  }

#if 0
CallGraphDotOutput::CallGraphDotOutput( SgIncidenceDirectedGraph & graph ) : GraphDotOutput<SgIncidenceDirectedGraph>(graph), callGraph(graph) 
  {}
#endif

namespace CallTargetSet
{


	Rose_STL_Container<SgFunctionDeclaration*> solveFunctionPointerCallsFunctional(SgNode* node, SgFunctionType* functionType ) 
	{ 
		Rose_STL_Container<SgFunctionDeclaration*> functionList;

		SgFunctionDeclaration* fctDecl = isSgFunctionDeclaration(node);
		ROSE_ASSERT( fctDecl != NULL );
		//if ( functionType == fctDecl->get_type() )
		//Find all function declarations which is both first non-defining declaration and
		//has a mangled name which is equal to the mangled name of 'functionType'
		if( functionType->get_mangled().getString() == fctDecl->get_type()->get_mangled().getString() )
		{
			//ROSE_ASSERT( functionType->get_mangled().getString() == fctDecl->get_mangled().getString() );

			SgFunctionDeclaration *nonDefDecl =
				isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );

			//The ROSE AST normalizes functions so that there should be a nondef function decl for
			//every function
			//ROSE_ASSERT( nonDefDecl != NULL );
			if( fctDecl == nonDefDecl )
				functionList.push_back( nonDefDecl );
			else
			    functionList.push_back( fctDecl );
		}//else
		//ROSE_ASSERT( functionType->get_mangled().getString() != fctDecl->get_type()->get_mangled().getString() );

		return functionList; 
	}



	SgFunctionDeclarationPtrList
		solveFunctionPointerCall( SgPointerDerefExp *pointerDerefExp, SgProject *project )
		{
			SgFunctionDeclarationPtrList functionList;

			SgFunctionType *fctType = isSgFunctionType( pointerDerefExp->get_type()->findBaseType() );
			ROSE_ASSERT ( fctType );
			ROSE_ASSERT ( project );
			// SgUnparse_Info ui;
			// string type1str = fctType->get_mangled( ui ).str();
			string type1str = fctType->get_mangled().str();
			cout << "Return type of function pointer " << type1str << "\n";

			cout << " Line: " << pointerDerefExp->get_file_info()->get_filenameString() <<  
				" l" << pointerDerefExp->get_file_info()->get_line() << 
				" c" << pointerDerefExp->get_file_info()->get_col()  << std::endl;
			// getting all possible functions with the same type
			// DQ (1/31/2006): Changed name and made global function type symbol table a static data member.
			// SgType *ty = Sgfunc_type_table.lookup_function_type( fctType->get_mangled( ui ) );
			ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
			// SgType *ty = SgNode::get_globalFunctionTypeTable()->lookup_function_type( fctType->get_mangled( ui ) );
			// ROSE_ASSERT ( ty->get_mangled( ui ) == type1str );

			// if there are multiple forward declarations of the same function
			// there will be multiple nodes in the AST containing them
			// but just one link in the call graph
			//  list<SgNode *> fctDeclarationList = NodeQuery::querySubTree( project, V_SgFunctionDeclaration );
			//AS (09/23/06) Query the memory pool instead of subtree of project
			//AS (10/2/06)  Modified query to only query for functions or function templates
			//VariantVector vv = V_SgFunctionDeclaration;
			VariantVector vv;
			vv.push_back(V_SgFunctionDeclaration);
			vv.push_back(V_SgTemplateInstantiationFunctionDecl);

			functionList =  AstQueryNamespace::queryMemoryPool(std::bind2nd(std::ptr_fun(solveFunctionPointerCallsFunctional), fctType), &vv );
			std::cout << "The size of the list: " << functionList.size() << std::endl;
			return functionList;

		}
}


namespace CallTargetSet
{
SgFunctionDeclarationPtrList
solveMemberFunctionPointerCall ( SgExpression *functionExp, ClassHierarchyWrapper *classHierarchy )
   {
     SgBinaryOp *binaryExp = isSgBinaryOp( functionExp );
     ROSE_ASSERT ( isSgArrowStarOp( binaryExp ) || isSgDotStarOp( binaryExp ) );//|| isSgArrowExp ( binaryExp ) || isSgDotExp ( binaryExp ) );

     SgExpression *left = NULL, *right = NULL;
     SgClassType *classType = NULL;
     SgClassDefinition *classDefinition = NULL;
     SgFunctionDeclarationPtrList functionList;
     string type1str;
     SgMemberFunctionType *memberFunctionType = NULL;

     left  = binaryExp->get_lhs_operand();
     right = binaryExp->get_rhs_operand();

     printf ("binaryExp = %p = %s \n",binaryExp,binaryExp->class_name().c_str());
     printf ("left  = %p = %s \n",left,left->class_name().c_str());
     printf ("right = %p = %s \n",right,right->class_name().c_str());

  // left side of the expression should have class type
     classType = isSgClassType( left->get_type()->findBaseType() );
     ROSE_ASSERT ( classType != NULL );
     printf ("classType->get_declaration() = %p = %s \n",classType->get_declaration(),classType->get_declaration()->class_name().c_str());

  // DQ (2/23/2006): bug fix
  // classDefinition = isSgClassDeclaration( classType->get_declaration() )->get_definition();
     ROSE_ASSERT ( classType->get_declaration() != NULL );
     ROSE_ASSERT ( classType->get_declaration()->get_definingDeclaration() != NULL );
     SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classType->get_declaration()->get_definingDeclaration());
     ROSE_ASSERT ( definingClassDeclaration != NULL );
     classDefinition = definingClassDeclaration->get_definition();
     ROSE_ASSERT ( classDefinition != NULL);

  // right side of the expression should have member function type
     memberFunctionType = isSgMemberFunctionType( right->get_type()->findBaseType() );
     ROSE_ASSERT( memberFunctionType );
     type1str = memberFunctionType->get_mangled().getString();

     SgDeclarationStatementPtrList &allMembers = classDefinition->get_members();
     for ( SgDeclarationStatementPtrList::iterator it = allMembers.begin(); it != allMembers.end(); it++ )
        {
          SgMemberFunctionDeclaration *memberFunctionDeclaration = isSgMemberFunctionDeclaration( *it );
          if ( memberFunctionDeclaration )
             {
               SgMemberFunctionDeclaration *nonDefDecl = isSgMemberFunctionDeclaration( memberFunctionDeclaration->get_firstNondefiningDeclaration() );
               if ( nonDefDecl )
                    memberFunctionDeclaration = nonDefDecl;

               SgType *possibleType = memberFunctionDeclaration->get_type();
               string type2str = possibleType->get_mangled().getString();
               if ( type1str == type2str )
                  {
                    if ( !(  memberFunctionDeclaration->get_functionModifier().isPureVirtual() ) )
                         functionList.push_back( memberFunctionDeclaration );
                 // cout << "PUSHING " << memberFunctionDeclaration << "\n";
                  }

            // for virtual functions in polymorphic calls, we need to search down in the hierarchy of classes
            // and retrieve all declarations of member functions with the same type
               if ( ( memberFunctionDeclaration->get_functionModifier().isVirtual() ||
                      memberFunctionDeclaration->get_functionModifier().isPureVirtual() ) && !isSgThisExp( left ) )
                  {
                    SgClassDefinitionPtrList subclasses = classHierarchy->getSubclasses( classDefinition );
                    cout << "Virtual function " << memberFunctionDeclaration->get_mangled_name().str() << "\n";
                    string name2 = memberFunctionDeclaration->get_qualified_name().getString() +
                    memberFunctionDeclaration->get_mangled_name().getString();
                    for ( SgClassDefinitionPtrList::iterator it_cls = subclasses.begin(); it_cls != subclasses.end(); it_cls++ )
                       {
                         SgClassDefinition *cls = isSgClassDefinition( *it_cls );
                         SgDeclarationStatementPtrList &clsMembers = cls->get_members();
                         for ( SgDeclarationStatementPtrList::iterator it_cls_mb = clsMembers.begin(); it_cls_mb != clsMembers.end(); it_cls_mb++ )
                            {
                              SgMemberFunctionDeclaration *cls_mb_decl = isSgMemberFunctionDeclaration( *it_cls_mb );
                              string name3 = cls_mb_decl->get_qualified_name().getString() +
                              cls_mb_decl->get_mangled_name().getString();
                              string type3str = cls_mb_decl->get_type()->get_mangled().getString();
                              if ( name2 == name3 )
                                 {
                                   SgMemberFunctionDeclaration *nonDefDecl = isSgMemberFunctionDeclaration( cls_mb_decl->get_firstNondefiningDeclaration() );
                                   SgMemberFunctionDeclaration *defDecl = isSgMemberFunctionDeclaration( cls_mb_decl->get_definingDeclaration() );
                                   ROSE_ASSERT ( (!nonDefDecl && defDecl == cls_mb_decl) || (nonDefDecl == cls_mb_decl && nonDefDecl) );

                                   if ( nonDefDecl )
                                      {
                                        if ( !( nonDefDecl->get_functionModifier().isPureVirtual() ) && nonDefDecl->get_functionModifier().isVirtual() )
                                             functionList.push_back( nonDefDecl );
                                      }
                                     else
                                        if ( !( defDecl->get_functionModifier().isPureVirtual() ) && defDecl->get_functionModifier().isVirtual() )
                                             functionList.push_back( defDecl ); // == cls_mb_decl
                                 }
                           }
                       }
                  }
             }
        }

     cout << "Function list size: " << functionList.size() << "\n";
     return functionList;
   }

SgFunctionDeclarationPtrList
solveMemberFunctionCall( SgClassType *crtClass, ClassHierarchyWrapper *classHierarchy,
						  SgMemberFunctionDeclaration *memberFunctionDeclaration, bool polymorphic )
{
  SgFunctionDeclarationPtrList functionList;
  ROSE_ASSERT ( memberFunctionDeclaration && classHierarchy );
  //  memberFunctionDeclaration->get_file_info()->display( "Member function we are considering" );

  SgDeclarationStatement *nonDefDeclInClass = NULL;
  nonDefDeclInClass = memberFunctionDeclaration->get_firstNondefiningDeclaration();
  SgMemberFunctionDeclaration *functionDeclarationInClass = NULL;
	   
  // memberFunctionDeclaration is outside the class
  if ( nonDefDeclInClass )
    {
      //      nonDefDeclInClass->get_file_info()->display( "found nondefining" );
      functionDeclarationInClass = isSgMemberFunctionDeclaration( nonDefDeclInClass );
    }
  // in class declaration, since there is no non-defining declaration
  else
    {
      functionDeclarationInClass = memberFunctionDeclaration;
      //      functionDeclarationInClass->get_file_info()->display("declaration in class already");
    }

  ROSE_ASSERT ( functionDeclarationInClass );
  // we need the inclass declaration so we can determine if it is a virtual function
  if ( functionDeclarationInClass->get_functionModifier().isVirtual() && polymorphic )
    {
      SgFunctionDefinition *functionDefinition = memberFunctionDeclaration->get_definition();

      // if it's not pure virtual then
      // the current function declaration is a candidate function to be called
      if ( functionDefinition )
	functionList.push_back( functionDeclarationInClass );
      else
	functionDeclarationInClass->get_file_info()->display( "Pure virtual function found" );

      // search down the class hierarchy to get
      // all redeclarations of the current member function
      // which may be the ones being called via polymorphism
      SgClassDefinition *crtClsDef = NULL;
      // selecting the root of the hierarchy
      if ( crtClass )
	{
	  SgClassDeclaration *tmp = isSgClassDeclaration( crtClass->get_declaration() );
	  ROSE_ASSERT ( tmp );
	  SgClassDeclaration* tmp2 = isSgClassDeclaration(tmp->get_definingDeclaration());
	  ROSE_ASSERT (tmp2);
	  crtClsDef = tmp2->get_definition();
	  ROSE_ASSERT ( crtClsDef );
	}
      else
	{
	  crtClsDef = isSgClassDefinition( memberFunctionDeclaration->get_scope() );
	  ROSE_ASSERT ( crtClsDef );
	}

      // for virtual functions, we need to search down in the hierarchy of classes
      // and retrieve all declarations of member functions with the same type
      SgClassDefinitionPtrList subclasses = classHierarchy->getSubclasses( crtClsDef );
      functionDeclarationInClass = NULL;
      string f1 = memberFunctionDeclaration->get_mangled_name().str();
      string f2;
      for ( SgClassDefinitionPtrList::iterator it_cls = subclasses.begin(); it_cls != subclasses.end(); it_cls++ )
	{
	  SgClassDefinition *cls = isSgClassDefinition( *it_cls );
	  SgDeclarationStatementPtrList &clsMembers = cls->get_members();
	  for ( SgDeclarationStatementPtrList::iterator it_cls_mb = clsMembers.begin(); it_cls_mb != clsMembers.end(); it_cls_mb++ )
	    {
	      SgMemberFunctionDeclaration *cls_mb_decl = isSgMemberFunctionDeclaration( *it_cls_mb );

	      f2 = cls_mb_decl->get_mangled_name().str();
	      if ( f1 == f2 )
		{
		  SgMemberFunctionDeclaration *nonDefDecl =
		    isSgMemberFunctionDeclaration( cls_mb_decl->get_firstNondefiningDeclaration() );
		  SgMemberFunctionDeclaration *defDecl =
		    isSgMemberFunctionDeclaration( cls_mb_decl->get_definingDeclaration() );
		  ROSE_ASSERT ( (!nonDefDecl && defDecl == cls_mb_decl) || (nonDefDecl == cls_mb_decl && nonDefDecl) );
		  if ( nonDefDecl )
		    functionDeclarationInClass = nonDefDecl;
		  else
		    functionDeclarationInClass = defDecl;
		  ROSE_ASSERT ( functionDeclarationInClass );
		  if ( !( functionDeclarationInClass->get_functionModifier().isPureVirtual() ) )
		    functionList.push_back( functionDeclarationInClass );
		}
	    }
	}
    } // end if virtual
  // non virtual (standard) member function or call not polymorphic (or both)
  else
    if ( functionDeclarationInClass->get_declarationModifier().get_storageModifier().isStatic() )
      {
	cout << "Found static function declaration called as member function " << functionDeclarationInClass << "\n";
	exit( 1 );
      }
    else
      {
	// always pushing the in-class declaration, so we need to find that one
	SgDeclarationStatement *nonDefDeclInClass = NULL;
	nonDefDeclInClass = memberFunctionDeclaration->get_firstNondefiningDeclaration();
	SgMemberFunctionDeclaration *functionDeclarationInClass = NULL;
	
	// memberFunctionDeclaration is outside the class
	if ( nonDefDeclInClass )
	  functionDeclarationInClass = isSgMemberFunctionDeclaration( nonDefDeclInClass );
	// in class declaration, since there is no non-defining declaration
	else
	  functionDeclarationInClass = memberFunctionDeclaration;
	
	ROSE_ASSERT ( functionDeclarationInClass );
	//      			   cout << "Pushing non-virtual function declaration for function "
	//<< functionDeclarationInClass->get_name().str() << "   " << functionDeclarationInClass << "\n";
	functionList.push_back ( functionDeclarationInClass );
      }
  return functionList;
}
}
	


Rose_STL_Container<SgFunctionDeclaration*> solveFunctionPointerCallsFunctional(SgNode* node, SgFunctionType* functionType ) 
   { 
     Rose_STL_Container<SgFunctionDeclaration*> functionList;

     SgFunctionDeclaration* fctDecl = isSgFunctionDeclaration(node);
     ROSE_ASSERT( fctDecl != NULL );
     //if ( functionType == fctDecl->get_type() )
     //Find all function declarations which is both first non-defining declaration and
     //has a mangled name which is equal to the mangled name of 'functionType'
     if( functionType->get_mangled().getString() == fctDecl->get_type()->get_mangled().getString() )
	{
       //ROSE_ASSERT( functionType->get_mangled().getString() == fctDecl->get_mangled().getString() );

	  SgFunctionDeclaration *nonDefDecl =
		  isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );

       //The ROSE AST normalizes functions so that there should be a nondef function decl for
       //every function
	  ROSE_ASSERT( nonDefDecl != NULL );
	  if( fctDecl == nonDefDecl )
	       functionList.push_back( nonDefDecl );
	}//else
           //ROSE_ASSERT( functionType->get_mangled().getString() != fctDecl->get_type()->get_mangled().getString() );

     return functionList; 
   }



FunctionData::FunctionData ( SgFunctionDeclaration* inputFunctionDeclaration,
			     SgProject *project, ClassHierarchyWrapper *classHierarchy )
   {
     hasDefinition = false;
     properties = new Properties(inputFunctionDeclaration);
     SgFunctionDeclaration *defDecl =
       isSgFunctionDeclaration( properties->functionDeclaration->get_definingDeclaration() );

     //cout << "!!!" << inputFunctionDeclaration->get_name().str() << " has definition " << defDecl << "\n";
     //     cout << "Input declaration: " << inputFunctionDeclaration << " as opposed to " << functionDeclaration << "\n"; 

     // Test for a forward declaration (declaration without a definition)
     if ( defDecl )
       {
	 SgFunctionDefinition* functionDefinition = defDecl->get_definition();
	 ROSE_ASSERT ( functionDefinition != NULL );
	 hasDefinition = true;
	 Rose_STL_Container<SgNode*> functionCallExpList;
	 functionCallExpList = NodeQuery::querySubTree ( functionDefinition, V_SgFunctionCallExp );
	 
	 // printf ("functionCallExpList.size() = %zu \n",functionCallExpList.size());
	 
	 // list<SgFunctionDeclaration*> functionListDB;
	 Rose_STL_Container<SgNode*>::iterator i = functionCallExpList.begin();

	  // for all functions getting called in the body of the current function
	  // we need to get their declarations, or the set of declarations for
	  // function pointers and virtual functions
          while (i != functionCallExpList.end())
             {
	       Properties *fctProps = new Properties();
	       fctProps->functionDeclaration = NULL;
	       fctProps->functionType = NULL;
	       fctProps->invokedClass = NULL;
	       fctProps->isPointer = fctProps->isPolymorphic = false;

               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(*i);
               ROSE_ASSERT ( functionCallExp != NULL );

               SgExpression* functionExp = functionCallExp->get_function();
               ROSE_ASSERT ( functionExp != NULL );

	       switch ( functionExp->variantT() )
		 {
		 case V_SgArrowStarOp:
		 case V_SgDotStarOp:
		   {
		     /*
		     SgFunctionDeclarationPtrList fD =
		       CallGraphFunctionSolver::solveMemberFunctionPointerCall( functionExp, classHierarchy );
		     for ( SgFunctionDeclarationPtrList::iterator it = fD.begin(); it != fD.end(); it++ )
		       {
			 ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
			 functionListDB.push_back( *it );
		       }
		     */
		     SgBinaryOp *binaryExp = isSgBinaryOp( functionExp );
		     ROSE_ASSERT ( isSgArrowStarOp( binaryExp ) || isSgDotStarOp( binaryExp ) );//|| isSgArrowExp ( binaryExp ) || isSgDotExp ( binaryExp ) );

		     SgExpression *left, *right;
		     SgClassType *classType;
 
		     left = binaryExp->get_lhs_operand();
		     right = binaryExp->get_rhs_operand();
		     // left side of the expression should have class type
		     classType = isSgClassType( left->get_type()->findBaseType() );
		     ROSE_ASSERT ( classType );
		     if ( !isSgThisExp( left ) ||
			  isSgPointerDerefExp( left ) && isSgThisExp( isSgPointerDerefExp( left )->get_operand() ))
		       {
			 fctProps->invokedClass = isSgClassDeclaration( classType->get_declaration()->get_definingDeclaration() )->get_definition();

			 fctProps->isPolymorphic = true;
                         if(fctProps->invokedClass==NULL)
                         {
                           std::cout << "A " << classType->unparseToString() << " " << classType->class_name() <<
                             " " <<  classType->get_declaration()->class_name() <<
                             classType->get_declaration()->unparseToString() << left->unparseToString() << std::endl;
                         }
                           ;

                         ROSE_ASSERT(fctProps->invokedClass!=NULL);

		       }
		     else
		       {
			 fctProps->invokedClass = NULL;
			 fctProps->isPolymorphic = false;
		       }
		     fctProps->functionDeclaration = NULL;
		     fctProps->functionType = isSgMemberFunctionType( right->get_type()->findBaseType() );
		     ROSE_ASSERT ( fctProps->functionType );
		     fctProps->isPointer = true;
		     functionListDB.push_back( fctProps );
		   }
		   break;
		 case V_SgDotExp:
		 case V_SgArrowExp:
		   {
		     SgMemberFunctionDeclaration *memberFunctionDeclaration = NULL;
		     SgClassType *crtClass = NULL;
		     fctProps->isPointer = false;
		     ROSE_ASSERT ( isSgBinaryOp( functionExp ) );

		     SgExpression *leftSide = isSgBinaryOp( functionExp )->get_lhs_operand();
		     SgType *leftType = leftSide->get_type()->findBaseType();
		     crtClass = isSgClassType( leftType );

		     SgMemberFunctionRefExp *memberFunctionRefExp =
		       isSgMemberFunctionRefExp( isSgBinaryOp( functionExp )->get_rhs_operand() );
		     memberFunctionDeclaration =
		       isSgMemberFunctionDeclaration( memberFunctionRefExp->get_symbol()->get_declaration() );
		     ROSE_ASSERT ( memberFunctionDeclaration && crtClass );
		     SgMemberFunctionDeclaration *nonDefDecl =
		       isSgMemberFunctionDeclaration( memberFunctionDeclaration->get_firstNondefiningDeclaration() );
		     if ( nonDefDecl )
		       memberFunctionDeclaration = nonDefDecl;

		     fctProps->functionDeclaration = memberFunctionDeclaration;
		     ROSE_ASSERT ( isSgFunctionDeclaration( memberFunctionDeclaration ) );
		     fctProps->functionType = fctProps->functionDeclaration->get_type()->findBaseType();
		     ROSE_ASSERT ( fctProps->functionType );
		     if ( isSgThisExp( leftSide ) || !( memberFunctionDeclaration->get_functionModifier().isVirtual() ) )
		       {
			 fctProps->isPolymorphic = false;
			 fctProps->invokedClass = NULL;
		       }
		     else
		       {
			 fctProps->isPolymorphic = true;
			 fctProps->invokedClass = isSgClassDeclaration( crtClass->get_declaration()->get_definingDeclaration() )->get_definition();
                         ROSE_ASSERT(fctProps->invokedClass!=NULL);
			 cout << "SET polymorphic on class " << fctProps->invokedClass->get_qualified_name().getString()
			      << "\t" << fctProps << "\n";
		       }
                     ROSE_ASSERT(fctProps->functionDeclaration != NULL);
		     functionListDB.push_back( fctProps );
		   }
		   break;
		 case V_SgPointerDerefExp:
		   {
		     fctProps->isPointer = true;
		     fctProps->isPolymorphic = false;
		     fctProps->invokedClass = NULL;
		     fctProps->functionDeclaration = NULL; 
		     fctProps->functionType =
		       isSgFunctionType( isSgPointerDerefExp( functionExp )->get_type()->findBaseType() );
		     ROSE_ASSERT ( fctProps->functionType );
		     functionListDB.push_back( fctProps );
		   }
		   break;
		 case V_SgMemberFunctionRefExp:
		   {
		     SgMemberFunctionDeclaration *mbFctDecl =
		       isSgMemberFunctionRefExp( functionExp )->get_symbol()->get_declaration();
		     ROSE_ASSERT ( mbFctDecl );
		     SgMemberFunctionDeclaration *nonDefDecl =
		       isSgMemberFunctionDeclaration( mbFctDecl->get_firstNondefiningDeclaration() );
		     if ( nonDefDecl )
		       mbFctDecl = nonDefDecl;
		     ROSE_ASSERT( mbFctDecl );
		     fctProps->functionDeclaration = mbFctDecl;
		     fctProps->functionType = mbFctDecl->get_type()->findBaseType();
		     ROSE_ASSERT ( fctProps->functionType );
		     fctProps->isPointer = false;
		     fctProps->isPolymorphic = false;
		     fctProps->invokedClass = NULL;
		     functionListDB.push_back( fctProps );
                     ROSE_ASSERT(fctProps->functionDeclaration != NULL);


		     /*
		     functionListDB.push_back( mbFctDecl );
		     */
		   }
		   break;
		 case V_SgFunctionRefExp:
		   {
		     SgFunctionDeclaration *fctDecl =
		       isSgFunctionDeclaration( isSgFunctionRefExp( functionExp )->get_symbol()->get_declaration() );
		     ROSE_ASSERT ( fctDecl );
		     SgFunctionDeclaration *nonDefDecl =
		       isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );
		     if ( nonDefDecl )
		       fctProps->functionDeclaration = nonDefDecl;
		     else
		       fctProps->functionDeclaration = fctDecl;
		     ROSE_ASSERT ( isSgFunctionDeclaration( fctProps->functionDeclaration ) );
		     fctProps->functionType = fctProps->functionDeclaration->get_type()->findBaseType();
		     ROSE_ASSERT ( fctProps->functionType );
		     fctProps->isPointer = false;
		     fctProps->isPolymorphic = false;
		     fctProps->invokedClass = NULL;
		     functionListDB.push_back( fctProps );
                     ROSE_ASSERT(fctProps->functionDeclaration != NULL);

		   }
		   break;
		 default:
		   {
		     cout << "Error, unexpected type of functionRefExp: " << functionExp->sage_class_name() << "!!!\n";
		     ROSE_ASSERT ( false );
		   }
		 }
	       ROSE_ASSERT ( !( fctProps->functionDeclaration ) || isSgFunctionDeclaration( fctProps->functionDeclaration ) );
	       ROSE_ASSERT ( isSgType( fctProps->functionType ) );
               i++;
             }
	}
   }


FunctionData::FunctionData ( SgFunctionDeclaration* inputFunctionDeclaration, bool hasDef,
			     SgProject *project, ClassHierarchyWrapper *classHierarchy )
{
  hasDefinition = hasDef;
  functionDeclaration = inputFunctionDeclaration;
  properties = new Properties(inputFunctionDeclaration);

  ROSE_ASSERT( functionDeclaration != NULL );
  SgFunctionDeclaration *defDecl =
    isSgFunctionDeclaration( functionDeclaration->get_definingDeclaration() );
  
  //cout << " " << functionDeclaration->get_name().str() << " has definition " << functionDefinition << "\n";
  // cout << "Input declaration: " << inputFunctionDeclaration << " as opposed to " << functionDeclaration << "\n";
  
  // Test for a forward declaration (declaration without a definition)
  if ( defDecl )
    {
      SgFunctionDefinition* functionDefinition = defDecl->get_definition();
      ROSE_ASSERT ( functionDefinition != NULL );
      Rose_STL_Container<SgNode*> functionCallExpList;
      functionCallExpList = NodeQuery::querySubTree ( functionDefinition, V_SgFunctionCallExp );

      // printf ("functionCallExpList.size() = %zu \n",functionCallExpList.size());

      // list<SgFunctionDeclaration*> functionList;
      Rose_STL_Container<SgNode*>::iterator i = functionCallExpList.begin();
      
      // for all functions getting called in the body of the current function
      // we need to get their declarations, or the set of declarations for
      // function pointers and virtual functions
      while (i != functionCallExpList.end())
	{
	  SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(*i);
	  ROSE_ASSERT ( functionCallExp != NULL );
	  
	  SgExpression* functionExp = functionCallExp->get_function();
	  ROSE_ASSERT ( functionExp != NULL );
	  //cout << "Function expression " << functionExp->sage_class_name() << "\n";
	  
	  switch ( functionExp->variantT() )
	    {
	    case V_SgArrowStarOp:
	    case V_SgDotStarOp:
	      {
		SgFunctionDeclarationPtrList fD =
		  CallTargetSet::solveMemberFunctionPointerCall( functionExp, classHierarchy );
		for ( SgFunctionDeclarationPtrList::iterator it = fD.begin(); it != fD.end(); it++ )
		  {
		    ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
		    functionList.push_back( *it );
		  }
	      }
	      break;
	    case V_SgDotExp:
	    case V_SgArrowExp:
	      {
		SgMemberFunctionDeclaration *memberFunctionDeclaration = NULL;
		SgClassType *crtClass = NULL;
		bool polymorphic = false;
		
		ROSE_ASSERT ( isSgBinaryOp( functionExp ) );
		SgExpression *leftSide = isSgBinaryOp( functionExp )->get_lhs_operand();
		if ( isSgThisExp( leftSide ) )
		  polymorphic = false;
		else
		  polymorphic = true;
		
		SgMemberFunctionRefExp *memberFunctionRefExp =
		  isSgMemberFunctionRefExp( isSgBinaryOp( functionExp )->get_rhs_operand() );
		SgType *leftType = leftSide->get_type();
		leftType = leftType->findBaseType();

		crtClass = isSgClassType( leftType );
                //AS(122805) In the case of a constructor initializer it is possible that a call to a constructor initializer may
                //return a type corresponding to an operator some-type() declared within the constructed class. An example is:
                //   struct Foo {
                //      operator  bool () const
                //          { return true; }
                //   };
                //
                //   struct Bar {
                //      bool foobar()
                //          { return Foo (); }
                //   };
                //where the call to the constructor of the class Foo will cause a call to the operator bool(), where bool corresponds
                //type of the member function foobar declared within Bar.
                if(isSgConstructorInitializer(leftSide)!= NULL){
                     SgClassDeclaration* constInit = isSgConstructorInitializer(leftSide)->get_class_decl();

                     //ROSE_ASSERT(constInit!=NULL);
                     if(constInit)
                        crtClass = constInit->get_type();
                     else{
                        //AS(010306) A compiler constructed SgConstructorInitializer may wrap a function call which return a class type.
                        //In an dot or arrow expression this returned class type may be used as an expression left hand side. To handle
                        //this case the returned class type must be extracted from the expression list. An example demonstrating this is:
                        //class Vector3d {
                        //   public:
                        //    Vector3d(){};
                        //    Vector3d(const Vector3d &vector3d){};
                        //   Vector3d     cross() const
                        //        { return Vector3d();};
                        //   void   GetZ(){};
                        //};
                        //void foo(){
                        //  Vector3d vn1;
                        //  (vn1.cross()).GetZ();
                        //}
 
                        SgExprListExp* expLst = isSgExprListExp(isSgConstructorInitializer(leftSide)->get_args());
                        ROSE_ASSERT(expLst!=NULL);
                        ROSE_ASSERT(expLst->get_expressions().size()==1);
                        SgClassType* lhsClassType = isSgClassType(isSgFunctionCallExp(*expLst->get_expressions().begin())->get_type());
                        
                        crtClass = lhsClassType;
                        }

                        
                        ROSE_ASSERT(crtClass!=NULL);
                }
		memberFunctionDeclaration =
		  isSgMemberFunctionDeclaration( memberFunctionRefExp->get_symbol()->get_declaration() );
		ROSE_ASSERT ( memberFunctionDeclaration && crtClass );
                ROSE_ASSERT ( crtClass != NULL );
	
		// returns the list of all in-class declarations of functions potentially called
		// ( may be several because of polymorphism )
		SgFunctionDeclarationPtrList fD =
		  CallTargetSet::solveMemberFunctionCall( crtClass, classHierarchy,
								    memberFunctionDeclaration, polymorphic );
		for ( SgFunctionDeclarationPtrList::iterator it = fD.begin(); it != fD.end(); it++ )
		  {
		    ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
		    functionList.push_back( *it );
		  }
	      }
	      break;
	    case V_SgPointerDerefExp:
	      // function pointer:
	      // a. can be returned by another function
	      // b. can be pointed to by a variable
	      {
		SgFunctionDeclarationPtrList fD =
		  CallTargetSet::solveFunctionPointerCall( isSgPointerDerefExp( functionExp ), project );
		for ( SgFunctionDeclarationPtrList::iterator it = fD.begin(); it != fD.end(); it++ )
		  {
		    ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
		    functionList.push_back( *it );
		  }
	      }
	      break;
	    case V_SgMemberFunctionRefExp:
	      {
		SgMemberFunctionDeclaration *mbFctDecl =
		  isSgMemberFunctionRefExp( functionExp )->get_symbol()->get_declaration();
		ROSE_ASSERT ( mbFctDecl );
		SgMemberFunctionDeclaration *nonDefDecl =
		  isSgMemberFunctionDeclaration( mbFctDecl->get_firstNondefiningDeclaration() );
		if ( nonDefDecl )
		  mbFctDecl = nonDefDecl;
		functionList.push_back( mbFctDecl );
	      }
	      break;
	    case V_SgFunctionRefExp:
	      {
		//cout << "Member fct or fct ref\n";
		SgFunctionDeclaration *fctDecl =
		  isSgFunctionDeclaration( isSgFunctionRefExp( functionExp )->get_symbol()->get_declaration() );
		ROSE_ASSERT ( fctDecl );
		SgFunctionDeclaration *nonDefDecl =
		  isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );
		if ( nonDefDecl )
		  functionList.push_back( nonDefDecl );
		else
		  functionList.push_back( fctDecl );
	      }
	      break;
	    default:
	      {
		cout << "Error, unexpected type of functionRefExp: " << functionExp->sage_class_name() << "!!!\n";
		ROSE_ASSERT ( false );
	      }
	    }
	  i++;
	}
    }
}


SgGraphNode*
findNode(SgGraph* graph, std::string nid)
{
  const rose_graph_string_integer_hash_multimap& nidToInt =
    graph->get_string_to_node_index_multimap();
  const rose_graph_integer_node_hash_map & intToNode =
    graph->get_node_index_to_node_map ();

  rose_graph_string_integer_hash_multimap::const_iterator iItr = nidToInt.find(nid);


  if( iItr != nidToInt.end() )
  {
    int n = iItr->second;
#if 1

    iItr++;

    int i = 2;
    for(;iItr != nidToInt.end(); ++iItr )
    {
          i++;
    };

    //The nid should match an unique int
    if( iItr != nidToInt.end() )
    {
      std::cout << "Error: nid " << nid << " occurs more than once. " << i << std::endl; 
    };

    ROSE_ASSERT( iItr == nidToInt.end() );
#endif

    return intToNode.find(n)->second;
  }else
    return NULL;
};


//Iterate over all edges in graph until an edge from->to is found. If not such edge
//exsists return NULL
SgGraphEdge*
findEdge (SgIncidenceDirectedGraph* graph, SgGraphNode* from, SgGraphNode* to)
{
  const rose_graph_integer_edge_hash_multimap & outEdges
    = graph->get_node_index_to_edge_multimap_edgesOut ();

   rose_graph_integer_edge_hash_multimap::const_iterator it = outEdges.find(from->get_index());

   for (;it!=outEdges.end();++it) {
     SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(it->second);
     ROSE_ASSERT(graphEdge!=NULL);
     if(graphEdge->get_from() == from && graphEdge->get_to() == to)
       return graphEdge;
   }

  
   return NULL;
};


SgGraphNode*
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, SgFunctionDeclaration* functionDeclaration )
   {
     Rose_STL_Container<SgGraphNode*>::iterator k = nodeList.begin();

     SgGraphNode* returnNode = NULL;

     bool found = false;
     while ( !found && (k != nodeList.end()) )
        {

          if ( ( var_SOLVE_FUNCTION_CALLS_IN_DB == true &&
                 (*k)->get_SgNode() == functionDeclaration ) 
               ||
               ( var_SOLVE_FUNCTION_CALLS_IN_DB == false && 
                 (*k)->get_SgNode() == functionDeclaration) )
             {
               returnNode = *k;
               found = true;
             }
          k++;
        }

     if ( !returnNode )
       cout << "NO node found for " << functionDeclaration->get_name().str() << " " << functionDeclaration << "\n";
     // ROSE_ASSERT (returnNode != NULL);
     return returnNode;
   }

SgGraphNode*
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, string name )
   {
     Rose_STL_Container<SgGraphNode*>::iterator k = nodeList.begin();

     SgGraphNode* returnNode = NULL;

     bool found = false;
     while ( !found && (k != nodeList.end()) )
        {
          if ((*k)->get_name() == name)
             {
               returnNode = *k;
               found = true;
             }
          k++;
        }

     //ROSE_ASSERT (returnNode != NULL);
     if ( !returnNode )
       cout << "No node found for " << name << "\n";
     return returnNode;
   }


void
CallGraphBuilder::buildCallGraph (){
    buildCallGraph(dummyFilter());
}


#if 0
template<typename Predicate>
void
CallGraphBuilder::buildCallGraph (Predicate pred)
   {
     Rose_STL_Container<FunctionData *> callGraphData;

  //AS (09/23/06) Query the memory pool instead of subtree of project
     VariantVector vv( V_SgFunctionDeclaration );
     AstQueryNamespace::DefaultNodeFunctional defFunc;
    Rose_STL_Container<SgNode *> functionList = NodeQuery::queryMemoryPool(defFunc, &vv );


  //   list<SgNode *> functionList = NodeQuery::querySubTree ( project, V_SgFunctionDeclaration );
     
     ClassHierarchyWrapper classHierarchy( project );
     Rose_STL_Container<SgNode *>::iterator i = functionList.begin();

     printf ("Inside of buildCallGraph functionList.size() = %zu \n",functionList.size());

     while ( i != functionList.end() )
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration( *i );
          ROSE_ASSERT ( functionDeclaration != NULL );


          bool hasDef = false;
      

          if ( var_SOLVE_FUNCTION_CALLS_IN_DB == true && functionDeclaration->get_definition() != NULL )
	   {
	     // printf ("Insert function declaration containing function definition \n");
	     hasDef = true;
           }

	  // determining the in-class declaration
	  if ( isSgMemberFunctionDeclaration( functionDeclaration ) )
	    {
	      // always saving the in-class declaration, so we need to find that one
	      SgDeclarationStatement *nonDefDeclInClass =
		isSgMemberFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
	      // functionDeclaration is outside the class (so it must have a definition)
	      if ( nonDefDeclInClass )
		  functionDeclaration = isSgMemberFunctionDeclaration( nonDefDeclInClass );
	    }
	  else
	    {
	      // we need to have only one declaration for regular functions as well
	      SgFunctionDeclaration *nonDefDecl =
		isSgFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
	      if ( nonDefDecl )
		functionDeclaration = nonDefDecl;
	    }
	  FunctionData* functionData = var_SOLVE_FUNCTION_CALLS_IN_DB ? new FunctionData( functionDeclaration, project, &classHierarchy )
            : new FunctionData( functionDeclaration, hasDef, project, &classHierarchy ) ;

          if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
	     *i = functionDeclaration;

          //AS(032806) Filter out functions baced on criteria in predicate
          if(pred(functionDeclaration)==true) 
             callGraphData.push_back( functionData );
          i++;
        }

     // Build the graph
     CallGraphCreate *returnGraph = new CallGraphCreate();
     ROSE_ASSERT (returnGraph != NULL);

     Rose_STL_Container<FunctionData *>::iterator j = callGraphData.begin();

     printf ("Build the node list callGraphData.size() = %zu \n",callGraphData.size());

     Rose_STL_Container<CallGraphNode *> nodeList;
     while ( j != callGraphData.end() )
        {
          string functionName;
          SgFunctionDeclaration* id;
          if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
          {
	  ROSE_ASSERT ( (*j)->properties->functionDeclaration );
	  functionName = (*j)->properties->functionDeclaration->get_mangled_name().getString();
          id = (*j)->properties->functionDeclaration;

          }else
          {
             functionName = (*j)->functionDeclaration->get_name().str();
             id = (*j)->functionDeclaration;

	  cout << "Function: "
	       << (*j)->properties->functionDeclaration->get_scope()->get_qualified_name().getString() +
	    (*j)->properties->functionDeclaration->get_mangled_name().getString()
	       << " has declaration " << (*j)->isDefined() << "\n";

          }

	  // Generate a unique name to test against later
	  SgDeclarationStatement *nonDefDeclInClass =
	    isSgMemberFunctionDeclaration( id->get_firstNondefiningDeclaration() );
	  if ( nonDefDeclInClass )
	    ROSE_ASSERT ( id == nonDefDeclInClass );
          CallGraphNode* node = (var_SOLVE_FUNCTION_CALLS_IN_DB == true ? new CallGraphNode( functionName, (*j)->properties, (*j)->isDefined() )
             : new CallGraphNode(functionName, id, (*j)->isDefined())  );

          if(var_SOLVE_FUNCTION_CALLS_IN_DB == true )
              ROSE_ASSERT ( node->properties->functionType );

          nodeList.push_back( node );
	  /*
	  // show graph
	  cout << "Function " << functionName << "   " << id << " has pointers to:\n";
	  list <Properties *> &fL = (*j)->functionList;
          list<Properties *>::iterator k = fL.begin();
 	  while (k != fL.end())
	    {
	      cout << "\tfunction: " << *k << "\n";
	      k++;
	    }
	  */
          returnGraph->addNode( node );
          j++;
        }

     j = callGraphData.begin();
     cout << "NodeList size: " << nodeList.size() << "\n";
     int totEdges = 0;
     while (j != callGraphData.end())
        {
//          std::cout << "Calling findNode in outer loop loc1" << std::endl;
	  //printf ("Calling findNode in outer loop (*j)->functionDeclaration->get_name() = %s \n",(*j)->properties->functionDeclaration->get_name().str());
          CallGraphNode* startingNode = ( var_SOLVE_FUNCTION_CALLS_IN_DB == true ? findNode( nodeList, (*j)->properties->functionDeclaration ) :
             findNode(nodeList,(*j)->functionDeclaration) );
          ROSE_ASSERT (startingNode != NULL);

          Rose_STL_Container<Properties *> & functionList = ( var_SOLVE_FUNCTION_CALLS_IN_DB == true ? (*j)->functionListDB :
              (*j)->functionList);
          Rose_STL_Container<Properties *>::iterator k = functionList.begin();

          if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
          {

            while ( k != functionList.end() )
            {
              ROSE_ASSERT ( (*k)->functionType );
              string label = "POINTER";

              CallGraphEdge* edge = new CallGraphEdge( " " );
              if ( (*k)->functionDeclaration )
                edge->label = (*k)->functionDeclaration->get_mangled_name().getString();
              ROSE_ASSERT ( edge != NULL );
              edge->properties = *k;
              // if we have a pointer (no function declaration) or a virtual function, create dummy node
              if ( !( (*k)->functionDeclaration ) || (*k)->isPolymorphic )
              {
                SgGraphNode *dummy;
                if ( (*k)->functionDeclaration && (*k)->functionDeclaration->get_definingDeclaration() )
                  dummy = new SgGraphNode( "DUMMY", *k, true );
                else
                  dummy = new SgGraphNode( "DUMMY", *k, false );
                returnGraph->addNode( dummy );

                returnGraph->addEdge( startingNode, dummy, edge );
              }
              else
              {

                SgGraphNode *endNode = findNode( nodeList, ( *k )->functionDeclaration );
                ROSE_ASSERT ( endNode );
                if(returnGraph->edgeExist(startingNode,endNode)==false)
                  returnGraph->addEdge( startingNode, endNode, edge );
                cout << "\tEndNode "
                  << (*k)->functionDeclaration->get_name().str() << "\t" << endNode->isDefined() << "\n";
              }
              totEdges++;
              k++;
            }
          }else{

            while (k != functionList.end())
            {

              SgGraphNode* endingNode = findNode( nodeList, *k );
              /*
                 if ( !endingNode )
                 endingNode = findNode( nodeList,
                 ( *k )->get_qualified_name().getString() +
                 ( *k )->get_mangled_name().getString(), 1 );
               */
              if ( endingNode )
              {
                ROSE_ASSERT (endingNode != NULL);

                CallGraphEdge* edge = new CallGraphEdge(endingNode->functionDeclaration->get_name().getString());
                ROSE_ASSERT (edge != NULL);
                if(returnGraph->edgeExist(startingNode,endingNode)==false)
                  returnGraph->addEdge(startingNode,endingNode,edge);
                totEdges++;
              }
              else
              {
                cout << "COULDN'T FIND: " << ( *k )->functionDeclaration->get_qualified_name().str() << "\n";
                //isSgFunctionDeclaration( *k )->get_file_info()->display("AVOIDED CALL");
              }
              k++;
            }
          }
	  j++;
        }

     cout << "Total number of edges: " << totEdges << "\n";
     // printf ("Return graph \n");

     graph = returnGraph;
   }
#endif

GetOneFuncDeclarationPerFunction::result_type 
GetOneFuncDeclarationPerFunction::operator()(SgNode* node )
    {
      result_type returnType;
      SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node);
      if(funcDecl != NULL)
      {
        if( funcDecl->get_definingDeclaration () != NULL && node == funcDecl->get_definingDeclaration ())
          returnType.push_back(node);
        if( funcDecl->get_definingDeclaration () == NULL && node == funcDecl->get_firstNondefiningDeclaration () )
          returnType.push_back(node);
      }
      return returnType;
    }








void
GenerateDotGraph ( SgIncidenceDirectedGraph *graph, string fileName )
   {
     ROSE_ASSERT(graph != NULL);

     //     printf ("Building the GraphDotOutput object ... \n");
     std::cerr << "Error: Outputing to DOT file not implemented yet" << std::endl;
     OutputDot::writeToDOTFile(graph,fileName, "Call Graph" );
     //CallGraphDotOutput output(*graph);
     //output.writeToDOTFile(fileName, "Call Graph");
   }



#ifdef HAVE_SQLITE3
// creates a db and tables for storage of graphs



// writes the subgraph, edge, and node info to DB
void
writeSubgraphToDB( sqlite3x::sqlite3_connection& gDB, SgIncidenceDirectedGraph* callGraph )
{
  // writing the Graph table
  cout << "Writing graph...\n";

  cout << "Writing nodes and edges...\n";
  //----------------------------------------------------------
  // writing the Nodes & Edges tables
  rose_graph_integer_node_hash_map & nodes =
    callGraph->get_node_index_to_node_map ();

  //Map of all graphEdges in the graph
  rose_graph_integer_edge_hash_multimap & outEdges
    = callGraph->get_node_index_to_edge_multimap_edgesOut ();

  try{
  sqlite3_transaction trans2(gDB);

  for( rose_graph_integer_node_hash_map::iterator i = nodes.begin();
      i != nodes.end(); ++i )
  {
    SgGraphNode *node = i->second;//.Current();

    Properties* cur_property = dynamic_cast<Properties*>(node->getAttribute("Properties"));

    ROSE_ASSERT(cur_property != NULL);
    SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node->get_SgNode());

    ROSE_ASSERT ( node );
    //Write Current node to DB
    if (  funcDecl  ) //Currently only writing graph Created with SgNodes is supported
    {
      string filename = "POINTER_FUNCTION", mnglName = "POINTER", scope = "NULL";

      //Get current filename
      if( funcDecl->get_file_info()->isCompilerGenerated() || funcDecl->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() )
      {
        if ( funcDecl->get_file_info()->isCompilerGenerated() )
          filename = "./__compilerGenerated";
        if ( funcDecl->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() )
          filename = "./__compilerGenearatedToBeUnparsed";

      }else{
        filename = funcDecl->get_file_info()->get_filename();
      }

      mnglName = funcDecl->get_qualified_name().getString() +
        funcDecl->get_mangled_name().getString();
      scope = funcDecl->get_scope()->get_qualified_name().getString();

      ostringstream st;
      ROSE_ASSERT ( cur_property->functionType );
      string mnglType = cur_property->functionType->get_mangled().getString();

      int isDef = cur_property->hasDef ? 1 : 0;

      if ( isDef )
      {
        gDB.executenonquery(  "DELETE FROM Nodes WHERE nid = \"" + mnglName + "\";");

      }

      string lbl = "POINTER_LABEL";

      if ( funcDecl )
        lbl = funcDecl->get_mangled_name().getString();

      string command = "INSERT INTO Nodes VALUES (?,?,?,?,?,?);";

      std::ostringstream existQuery;
      /*      existQuery << "select count(nid) from Nodes where nid=\""<<mnglName<<"\" AND gid="<<nV<<" AND label=\"" << lbl << "\" AND def="
              << isDef << " AND type=\"" << mnglType << "\" AND scope=\"" << scope << "\" limit 1";
       */
      existQuery << "select count(nid) from Nodes where nid=\""<<mnglName<<"\" limit 1";

      if( sqlite3_command(gDB, existQuery.str().c_str()).executeint() == 0 )
      {
        sqlite3_command cmd(gDB, command.c_str());

        cmd.bind(1, mnglName);
        cmd.bind(2, filename);
        cmd.bind(3, lbl);
        cmd.bind(4, isDef );
        cmd.bind(5, mnglType);
        cmd.bind(6, scope);

        std::ostringstream ost;
        ost << "INSERT INTO Nodes VALUES (\""<<mnglName<<"\","<<filename<<",\"" << lbl << "\","
          << isDef << ",\"" << mnglType << "\",\"" << scope << "\");" << std::endl;

        //        std::cout << ost.str() << std::endl;

        cmd.executenonquery();
      }

      //cout << command << "\n";

      //std::cout << "Finding edges " << i->first << std::endl;
      for( rose_graph_integer_edge_hash_multimap::const_iterator ei = outEdges.find(i->first);
          ei != outEdges.end(); ++ei )
      {
        SgDirectedGraphEdge *edge = isSgDirectedGraphEdge(ei->second);
        ROSE_ASSERT( edge != NULL );
        SgGraphNode *end = edge->get_to();
        Properties* to_property = dynamic_cast<Properties*>(end->getAttribute("Properties"));
        ROSE_ASSERT(to_property != NULL);

        ostringstream st;

        ROSE_ASSERT ( end );
        string n2mnglName = "POINTER";

        string typeF = "", cls = "", nid2 = "NULL";
        if ( to_property->isPolymorphic )
        {
          ROSE_ASSERT ( isSgClassDefinition( to_property->invokedClass ) );
          cls = to_property->invokedClass->get_qualified_name().getString();
        }

        ROSE_ASSERT ( to_property->functionType );
        typeF = to_property->functionType->get_mangled().getString();
        if ( !( to_property->isPointer ) )
        {
          ROSE_ASSERT ( to_property->functionDeclaration &&
              to_property->functionDeclaration == end->get_SgNode() );
          n2mnglName = to_property->functionDeclaration->get_qualified_name().getString() +
            to_property->functionDeclaration->get_mangled_name().getString();
        }

        //std::cout << "Creating edge between " << mnglName << " " << n2mnglName << std::endl;
        st << "INSERT INTO Edges VALUES (\"" << mnglName << "\", \"" << n2mnglName << "\", \"" << edge->get_name()
          << "\", \"" << to_property->functionType->get_mangled( ).getString() << "\", \"" << cls << "\");";
        //cout << st.str() << "\n";
        gDB.executenonquery(st.str().c_str());

      }
    }
  }

	trans2.commit();
  }
  catch(exception &ex) {
	cerr << "Exception Occured: " << ex.what() << endl;
  }

  cout << "Done writing to DB\n";
}

// solve function pointers based on type
// TODO: virtual function pointers are not properly solved ( they are treated as regular member function pointers )
void
solveFunctionPointers( sqlite3x::sqlite3_connection& gDB )
{
  cout << "Solving function pointers...\n";

  string command = "";
  command = "INSERT INTO Edges SELECT e.nid1, nid, n1.label, e.type, e.objClass from Nodes n1, Edges e WHERE "
    "e.nid2 = \"POINTER\" AND e.type = n1.type;";// AND e.objClass = n1.scope;";
  gDB.executenonquery(command.c_str());
 
  command = "DELETE FROM Edges WHERE nid2 = \"POINTER\" AND objClass = \"\";";
  gDB.executenonquery(command.c_str());

  cout << command << "\t" << "CLEANUP\n";

  cout << "Done with function pointers\n";
}

// DQ (7/28/2005): Don't include the data base
// solve virtual function calls ( but not via pointers )
void
solveVirtualFunctions( sqlite3x::sqlite3_connection& gDB, string dbHierarchy )
{
  cout << "Solving virtual function calls...\n";

  string command = "";
  // determine descendants of edges.objClass
  {
   

    cout << "Classes with virtual functions called:\n";

    //rows->showResult();

    std::vector<std::vector<std::string> > objClassEdges;
  
    {
      command = "SELECT * from Edges WHERE objClass <> \"\";"; // AND nid2 <> \"POINTER\";";
      sqlite3_command cmd(gDB, command.c_str());

      sqlite3x::sqlite3_reader r = cmd.executereader();

      while( r.read() )
      {
        std::vector<std::string> objClassEdgesElem;
        string nid1     = r.getstring(0); objClassEdgesElem.push_back(nid1);
        string nid2     = r.getstring(1); objClassEdgesElem.push_back(nid2);
        string lbl2     = r.getstring(2); objClassEdgesElem.push_back(lbl2);
        string fType    = r.getstring(3); objClassEdgesElem.push_back(fType);
        string objClass = r.getstring(4); objClassEdgesElem.push_back(objClass);

        objClassEdges.push_back(objClassEdgesElem);
      }
    }

    command = "DELETE FROM Edges WHERE objClass <> \"\";";// AND nid2 <> \"POINTER\";";
    gDB.executenonquery( command.c_str() );

    command = "DELETE FROM Nodes WHERE nid = \"DUMMY\";";
    gDB.executenonquery( command.c_str() );


    for(unsigned int i = 0 ; i < objClassEdges.size(); i++)
    {

      string nid1     = objClassEdges[i][0];
      string nid2     = objClassEdges[i][1];
      string lbl2     = objClassEdges[i][2];
      string fType    = objClassEdges[i][3];
      string objClass = objClassEdges[i][4];

//      ClassHierarchyWrapper clsHierarchy ( "ClassHierarchy" );
      ClassHierarchyWrapper clsHierarchy;

      std::list<string> subclasses = clsHierarchy.getSubclasses( objClass,gDB );
      subclasses.push_back( objClass );
      for ( std::list<string>::iterator it = subclasses.begin(); it != subclasses.end(); it++ )
      {
        if ( nid2 == "POINTER" )
          command = "SELECT * FROM Nodes WHERE scope = \"" + *it + "\" AND type = \"" + fType + "\";";
        else
          command = "SELECT * FROM Nodes WHERE scope = \"" + *it + "\" AND label = \"" + lbl2 + "\";";// AND def = 1;";

        sqlite3_command cmd(gDB, command.c_str());

        sqlite3x::sqlite3_reader r = cmd.executereader();

        cout << "Now executing: " << command;

        std::vector<string> inserts;
        while( r.read() )
        {
          string nnid  = r.getstring(0);
          //	      int gid = (*itr)[1];
          string lbl   = r.getstring(2);
          //	      int is_def = (*itr)[3];
          string fType = r.getstring(4) ;
          string scope = r.getstring(3);
          command = "INSERT INTO Edges VALUES ( \"" + nid1 + "\", \"" + nnid + "\", \"" + lbl + "\", \"" + fType +
            "\", \"\" );";
          inserts.push_back(command);
        }

        for(unsigned int i = 0 ; i < inserts.size() ; i++)
          gDB.executenonquery(inserts[i].c_str());
      }
    }
  }


  cout << "Done with virtual functions\n";
}




//AS(5/1/2009): Largely a code copy of the other writeSubgraphToDB(..) function.
//Need to experiment to see if the other one will do the job.
#if 0
// DQ (7/28/2005): Don't include the data base
// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
// writes the subgraph, edge, and node info to DB
void
CallGraphDotOutput::writeSubgraphToDB( GlobalDatabaseConnection *gDB )
{
  // writing the Graph table
  cout << "Writing graph...\n";
  int crtSubgraph = GetCurrentMaxSubgraph(gDB);
  for (unsigned int i=0; i < callGraph.subGraphNames.size(); i++)
    {
      string filename = callGraph.subGraphNames[i];
      string filenameWithoutPath = basename(filename.c_str());
      string subGraphName = filenameWithoutPath;
      ostringstream st;
      // TODO: define a hierarchical structure, don't use -1 as a default parent
      st << "INSERT INTO Graph VALUES (" << i + crtSubgraph << ", " << -1 << ", " << "\"" << filename << "\");";
      string sts = st.str();
      gDB->execute(sts.c_str());
    }

  cout << "Writing nodes and edges...\n";
  //----------------------------------------------------------
  // writing the Nodes & Edges tables
  for (CallGraphCreate::NodeIterator i = callGraph.GetNodeIterator(); !i.ReachEnd(); ++i)
    {
      SgGraphNode *node = i.Current();
      assert(node);
      string filename = node->functionDeclaration->get_file_info()->get_filename();
      
      int nV = getVertexSubgraphId(*node);
      
      ostringstream st;
      string clsName = "";
      SgClassDefinition *clsDef = isSgClassDefinition( node->functionDeclaration->get_scope() );

      // get_scope returns NULL when it shouldn't....
      if ( clsDef )
	{
	  clsName = clsDef->get_qualified_name().getString();
	  //cout << "CLSCLSCLS: " << clsName << "\n";
	}
      
      /*get_mangled_name()*/
      string mnglName = node->functionDeclaration->get_qualified_name().getString() +
	node->functionDeclaration->get_mangled_name().getString();
      int isDef = node->isDefined() ? 1 : 0;
      string command;
      
      if ( isDef )
	{
	  command = "DELETE FROM Nodes WHERE nid = \"" + mnglName + "\";";
	  Query *q = gDB->getQuery();
	  q->set(command.c_str());
	  gDB->execute();

	  st << "INSERT INTO Nodes VALUES (\"" << mnglName  << "\", " << nV << ", \""
	     << node->toString() << "\", \"" << 1 << "\");";
	  command = st.str();
	}
      else
	{
	  st << "INSERT INTO Nodes VALUES (\"" << mnglName  << "\", " << nV << ", \""
	     << node->toString() << "\", \"" << 0 << ", \");";
	  command = st.str();
	}

      Query *q = gDB->getQuery();
      q->set( command.c_str() );
      cout << command << "\n";
      gDB->execute();
      
      CallGraphCreate::EdgeIterator ei;
      CallGraphCreate::EdgeDirection dir = CallGraphCreate::EdgeIn;
      
      for (ei = callGraph.GetNodeEdgeIterator(node, dir) ; !ei.ReachEnd(); ++ei)
	{
	  CallGraphEdge *edge = ei.Current();
	  CallGraphNode *end;
	  ostringstream st;

	  // string n1mnglName = node->functionDeclaration->get_mangled_name().getString();
	  string clsName = "";
	  assert ( node->functionDeclaration );
	  SgClassDefinition *clsDef = isSgClassDefinition( node->functionDeclaration->get_scope() );
	  
	  // get_scope returns NULL... is it NOT virtual???
	  if ( clsDef )
	    clsName = clsDef->get_qualified_name().getString();
	  
	  end = dynamic_cast<CallGraphNode *>(getTargetVertex(*edge));
	  assert(end);
	  string n2mnglName = end->functionDeclaration->get_qualified_name().getString() +
	    end->functionDeclaration->get_mangled_name().getString();
	  
	  st << "INSERT INTO Edges VALUES (\"" << mnglName << "\", \"" <<
	    n2mnglName << "\", \"" << edge->label << "\");";
	  gDB->execute(st.str().c_str());
	}
    }
  cout << "Done writing to DB\n";
}
// DQ (7/28/2005): Don't include the data base
#endif

#endif


#if 0
void
CallGraphBuilder::classifyCallGraph ()
{
  ROSE_ASSERT(graph != NULL);

  // printf ("Output the graph information! (number of nodes = %zu) \n",graph->size());

  int counter = 0;

  // This iteration over the graph verifies that we have a valid graph!
  rose_graph_integer_node_hash_map & nodes =
    graph->get_node_index_to_node_map ();


  for( rose_graph_integer_node_hash_map::iterator nodeIterator = nodes.begin();
      nodeIterator != nodes.end(); ++nodeIterator )

  {
    // printf ("In loop using node iterator ... \n");
    // DAGBaseNodeImpl* nodeImpl = nodeIterator.Current();
    SgGraphNode* node = *nodeIterator;
    Properties* cur_property = dynamic_cast<Properties*>(node->getAttribute("Properties"));
    ROSE_ASSERT(cur_property != NULL);
    ROSE_ASSERT (node != NULL);

      string filename;
      if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
      {
        filename = "./__pointers";
        if ( cur_property->functionDeclaration )
        {
          ROSE_ASSERT (cur_property->functionDeclaration->get_file_info() != NULL);
          ROSE_ASSERT (cur_property->functionDeclaration->get_file_info()->get_filename() != NULL);
          string tmp = cur_property->functionDeclaration->get_file_info()->get_filename();
          filename = tmp;
          if ( cur_property->functionDeclaration->get_file_info()->isCompilerGenerated() )
            filename = "/__compilerGenerated";
          if ( cur_property->functionDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() )
            filename = "./__compilerGenearatedToBeUnparsed";
        }
      }else{

        ROSE_ASSERT (node->get_SgNode() != NULL);
        ROSE_ASSERT (node->get_SgNode()->get_file_info() != NULL);
        // ROSE_ASSERT (node->functionDeclaration->get_file_info()->getCurrentFilename() != NULL);
        // string filename = node->functionDeclaration->get_file_info()->getCurrentFilename();
        ROSE_ASSERT (node->get_SgNode()->get_file_info()->get_filename() != NULL);
        filename = node->get_SgNode()->get_file_info()->get_filename();
        if ( node->get_SgNode()->get_file_info()->isCompilerGenerated() )
          filename = "/__compilerGenerated";
        if ( node->get_SgNode()->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() )
          filename = "./__compilerGenearatedToBeUnparsed";
        // printf ("function location = %s \n",filename.c_str());
      }
      

      // If not in the sub graph map then add it and increment the counter!
      if (graph->getSubGraphMap().find(filename) == graph->getSubGraphMap().end())
	{
            // This must be done on the DOT graph
            // graph->addSubgraph(filename);
	  graph->getSubGraphMap()[filename] = counter;
	  graph->subGraphNames[counter] = filename;
	  counter++;
	}
    }
  
  ROSE_ASSERT (graph->getSubGraphMap().size() == graph->subGraphNames.size());
  for (unsigned int i = 0; i < graph->getSubGraphMap().size(); i++)
    {
      // printf ("subgraphMap[%d] = %s \n",i,graph->subGraphNames[i].c_str());
      
      // Make sure that the filename are correct
      ROSE_ASSERT(graph->subGraphNames[i] == graph->subGraphNames[graph->getSubGraphMap()[graph->subGraphNames[i]]]);
      ROSE_ASSERT(graph->getSubGraphMap()[graph->subGraphNames[i]] == (int) i);
    }
}
#endif



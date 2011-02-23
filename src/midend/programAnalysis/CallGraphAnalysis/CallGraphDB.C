// tps : Switching from rose.h to sage3 changed size from 17,4 MB to 7MB
#include <sage3basic.h>

// DQ (10/16/2010): This is needed to use the HAVE_SQLITE3 macro.
// DQ (10/14/2010): This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include <boost/lexical_cast.hpp>
#include "CallGraph.h"

#ifdef HAVE_SQLITE3
#include "sqlite3x.h"
using namespace sqlite3x;
#endif

using namespace std;

#ifdef HAVE_SQLITE3



// creates a db and tables for storage of graphs
void
createSchema ( sqlite3x::sqlite3_connection& gDB, string dbName )
{

  //Query query = (*gDB)->getQuery();
  //gDB.executenonquery("CREATE TABLE IF NOT EXISTS Graph (gid, pgid, filename TEXT);");
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS Nodes (nid TEXT, filename TEXT, label TEXT, def INTEGER, type TEXT, scope TEXT, PRIMARY KEY (nid)););");
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS Edges  (nid1 TEXT, nid2 TEXT, label TEXT, type TEXT, objClass TEXT);");
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS Hierarchy ( Class TEXT, Subclass TEXT, ClassFile TEXT, SubclassFile TEXT, PRIMARY KEY ( Class, Subclass ) );");

  if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
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


  if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
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

  return graph;
}

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
            ei != outEdges.end() && ei->first == i->first; ++ei )
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

          if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
            std::cout << "Creating edge between " << mnglName << " " << n2mnglName << " " << to_property->functionType->get_mangled( ).getString() << std::endl;

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

  if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
    cout << "Solving function pointers...\n";

  string command = "";
  command = "INSERT INTO Edges SELECT e.nid1, nid, n1.label, e.type, e.objClass from Nodes n1, Edges e WHERE "
    "e.nid2 = \"POINTER\" AND e.type = n1.type;";// AND e.objClass = n1.scope;";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Edges WHERE nid2 = \"POINTER\" AND objClass = \"\";";
  gDB.executenonquery(command.c_str());

  if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
  {
    cout << command << "\t" << "CLEANUP\n";

    cout << "Done with function pointers\n";
  }
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


    if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
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

        if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Now executing: " << command;

        std::vector<string> inserts;
        while( r.read() )
        {
          string nnid  = r.getstring(0);
          //          int gid = (*itr)[1];
          string lbl   = r.getstring(2);
          //          int is_def = (*itr)[3];
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


  if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
    cout << "Done with virtual functions\n";
}


#endif


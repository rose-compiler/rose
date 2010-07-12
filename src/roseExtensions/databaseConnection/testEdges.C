#include "GlobalDatabaseConnectionMYSQL.h" 
#include "TableDefinitions.h" 

DEFINE_TABLE_PROJECTS(); 
DEFINE_TABLE_GRAPHDATA(); 
DEFINE_TABLE_GRAPHNODE(); 
DEFINE_TABLE_GRAPHEDGE(); 
CREATE_TABLE_2( simpleFuncTable,  int,projectId, string,functionName); 
DEFINE_TABLE_2( simpleFuncTable,  int,projectId, string,functionName); 
CREATE_TABLE_5( callEdge,         int,projectId, string,site,   string,actual, 
		int,scope, int,ordinal); 
DEFINE_TABLE_5( callEdge,         int,projectId, string,site,   string,actual, 
		int,scope, int,ordinal); 

#define TABLES_DEFINED 1 
 
#include "DatabaseGraph.h" 

#include <boost/config.hpp> 
#include <assert.h> 
#include <iostream> 
 
#include <vector> 
#include <algorithm> 
#include <utility> 

using namespace boost;

// simpleFuncTableRowdata is the node data
// callEdgeRowdata is the edge data
typedef DatabaseGraph<simpleFuncTableRowdata, callEdgeRowdata,  
                      boost::vecS, boost::vecS, boost::bidirectionalS> CallGraph;

int main(int argc, char** argv)
{
  GlobalDatabaseConnection db;
  int drop = 1;

  int initOk = db.initialize(drop);
  assert(initOk == 0);

  CREATE_TABLE(db, projects);
  CREATE_TABLE(db, graphdata); 
  CREATE_TABLE(db, graphnode); 
  CREATE_TABLE(db, graphedge); 

  string projectName = "testProject";  
  projectsRowdata prow( UNKNOWNID, projectName, UNKNOWNID ); 
  projects.retrieveCreateByColumn( &prow, "name",  
                                   projectName ); 
  long projectId = prow.get_id(); 

  TableAccess< simpleFuncTableRowdata > simpleFunc(&db);
  simpleFunc.initialize();

  TableAccess< callEdgeRowdata > callEdge(&db); 
  callEdge.initialize(); 

  CallGraph *callgraph = new CallGraph(projectId, GTYPE_SIMPLECALLGRAPH, &db);

  // create two nodes 

  // create a row for node v1 with entries for the projectId and name "v1"
  simpleFuncTableRowdata v1(UNKNOWNID, projectId, "v1");

  // Look up the name "v1" (accessed via v1.get_functionName(), a method of simpleFuncTableRowdata)
  // under the column "functionName" in the simpleFunc table.  If the row exists, put the row
  // entries in the first argument.  If it does not, create the row from the data in the
  // first argument.
  simpleFunc.retrieveCreateByColumn(&v1, "functionName", v1.get_functionName(), v1.get_projectId());

  // repeat for node "v2"
  simpleFuncTableRowdata v2(UNKNOWNID, projectId, "v2");
  simpleFunc.retrieveCreateByColumn(&v2, "functionName", v2.get_functionName(), v2.get_projectId());

  // create the edge
  // an edge row consists of 4 entries-- a call site (string), an actual argument (string),
  // an integer representing the scope of the call site), and an integer representing
  // the argument number.
  callEdgeRowdata edge(UNKNOWNID, projectId, "site-value", "actual-value", 3, 5);

  // create two arrays-- one representing the column names and the second their
  // respective values.
  string edgeColumns[2]; 
  string edgeValues[2]; 
  edgeColumns[0] = "site"; 
  edgeValues[0]   = edge.get_site(); 
  edgeColumns[1] = "actual"; 
  edgeValues[1]   = edge.get_actual(); 

  // Determine if a row entry exists with the given column headings and associated
  // values (namely, a column with name "site" and value "site-value" and a 
  // second column with name "actual" and value "actual-value".  If it does
  // exist, read it from the database and put the values in edge.  If it does
  // not create a row with the data from the first argument, edge.
  callEdge.retrieveCreateByColumns(&edge,  
				   edgeColumns,  
				   edgeValues,  
				   2, 
				   edge.get_projectId());

  // insert the first node twice
  callgraph->insertVertex(v1, v1.get_functionName());
  // this should not have any affect
  callgraph->insertVertex(v1, v1.get_functionName());

  // insert the second node
  callgraph->insertVertex(v2, v2.get_functionName());

  // insert the edge twice
  callgraph->insertEdge(v1, v2, edge);
  // this should not have any affect
  callgraph->insertEdge(v1, v2, edge);

  // save the call graph
  callgraph->writeToDatabase();
  
  // read it back
  CallGraph *restoredCallgraph = new CallGraph(projectId, GTYPE_SIMPLECALLGRAPH, &db);
  restoredCallgraph->loadFromDatabase();

  assert(num_edges(*restoredCallgraph) == 1);

  typedef graph_traits<CallGraph>::edge_iterator edge_iter; 
  pair<edge_iter, edge_iter> ep; 
       
  // loop over the edges in the callgraph 
  for (ep = edges(*restoredCallgraph); ep.first != ep.second; ++ep.first) { 
         
    simpleFuncTableRowdata src( get( vertex_dbg_data, *restoredCallgraph,  
				     boost::source( *(ep.first), *restoredCallgraph) ) );

    simpleFuncTableRowdata tar( get( vertex_dbg_data, *restoredCallgraph,  
				     boost::target( *(ep.first), *restoredCallgraph) ) );
 
    assert( (!strcmp(src.get_functionName().c_str(), v1.get_functionName().c_str()) &&
	     !strcmp(tar.get_functionName().c_str(), v2.get_functionName().c_str())) ||
	    (!strcmp(src.get_functionName().c_str(), v2.get_functionName().c_str()) &&
	     !strcmp(tar.get_functionName().c_str(), v1.get_functionName().c_str())) );
	     

    callEdgeRowdata restoredEdge( get( edge_dbg_data, *restoredCallgraph, *ep.first ) );

    assert( !strcmp(edge.get_site().c_str(), restoredEdge.get_site().c_str()) );
    assert( !strcmp(edge.get_actual().c_str(), restoredEdge.get_actual().c_str()) );
    assert( edge.get_ordinal() == restoredEdge.get_ordinal() );
    assert( edge.get_scope() == restoredEdge.get_scope() );

  }

  printf ("\n\n");
  printf ("*****************************\n");
  printf ("Program Terminated Normally! \n");
  printf ("*****************************\n");
  printf ("\n\n");

  return 0;
}

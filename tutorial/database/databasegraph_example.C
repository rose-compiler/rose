#include <iostream>
#include "GlobalDatabaseConnectionMYSQL.h"
#include "TableDefinitions.h"
DEFINE_TABLE_PROJECTS();
DEFINE_TABLE_GRAPHDATA();
DEFINE_TABLE_GRAPHNODE();
DEFINE_TABLE_GRAPHEDGE();
CREATE_TABLE_2( testtable,  string,name,  double,number );
DEFINE_TABLE_2( testtable,  string,name,  double,number );
#define TABLES_DEFINED 1
#include "DatabaseGraph.h"

//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  GlobalDatabaseConnection db;
  int initOk =  db.initialize();
  assert( initOk==0 );

  CREATE_TABLE(db, projects);
  CREATE_TABLE(db, graphdata);
  CREATE_TABLE(db, graphnode);
  CREATE_TABLE(db, graphedge);

  TableAccess< testtableRowdata > testtable( &db );
  testtable.initialize();

  // initialize project
  string projectName = "testProject";  // this should be given at the command line
  projectsRowdata prow( UNKNOWNID ,projectName, UNKNOWNID );
  projects.retrieveCreateByColumn( &prow, "name", projectName );
  long projectId   = prow.get_id();

  // init graph
  DatabaseGraph<testtableRowdata, EdgeTypeEmpty> *graph = 
    new DatabaseGraph<testtableRowdata, EdgeTypeEmpty>( projectId, GTYPE_TESTGRAPH, &db );
  graph->loadFromDatabase( );

  // the graph is now ready for use..., add some example nodes and edges
  testtableRowdata testrow1( UNKNOWNID, "dgb_exrow1", 0.1 );
	std::ostringstream whereClause;
  whereClause << " graphId='" << graph->getGraphId() <<"' ";
  vector<graphnodeRowdata> result = graphnode.select( whereClause.str() );
  if(result.size() > 0) {
    // select a random node from the graph
    int index = rand() % result.size();
    // the node will be identified by the id, so the other fields are unimportant here
    testrow1.set_id( result[index].get_nodeId() );
  } else {
    // there are no nodes yet, create a new root node
    testtable.insert( &testrow1 );
    graph->insertVertex( testrow1, testrow1.get_name() );
  }

  // add three other nodes...
  testtableRowdata testrow2( UNKNOWNID, "dgb_exrow2", 0.2 );
  testtable.insert( &testrow2 );
  testtableRowdata testrow3( UNKNOWNID, "dgb_exrow3", 0.3 );
  testtable.insert( &testrow3 );
  testtableRowdata testrow4( UNKNOWNID, "dgb_exrow4", 0.4 );
  testtable.insert( &testrow4 );

  graph->insertVertex( testrow2, testrow2.get_name() );
  graph->insertVertex( testrow3, testrow3.get_name() );
  graph->insertVertex( testrow4, testrow4.get_name() );

  // and edges to them from the first node
  graph->insertEdge( testrow1, testrow2 );
  graph->insertEdge( testrow1, testrow3 );
  graph->insertEdge( testrow1, testrow4 );

  // save graph to dot file, and to database
  graph->writeToDOTFile( "databasegraph_example.dot" );
  graph->writeToDatabase( );
  delete graph;
  db.shutdown();
  return( 0 );
}


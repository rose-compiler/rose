
#include <iostream>
#include "GlobalDatabaseConnectionMYSQL.h"
#include "TableDefinitions.h"
DEFINE_TABLE_PROJECTS();
DEFINE_TABLE_FILES();
DEFINE_TABLE_GRAPHDATA();
DEFINE_TABLE_GRAPHNODE();
DEFINE_TABLE_GRAPHEDGE();
CREATE_TABLE_2( testtable,  string,name,  double,number );
DEFINE_TABLE_2( testtable,  string,name,  double,number );
#define TABLES_DEFINED 1

#include "DatabaseGraph.h"


//-----------------------------------------------------------------------------
// define traversal classes NYI
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {

	GlobalDatabaseConnection db;
	int initOk =  db.initialize();
	assert( initOk==0 );

	CREATE_TABLE(db, projects);
	CREATE_TABLE(db, files);
	CREATE_TABLE(db, graphdata);
	CREATE_TABLE(db, graphnode);
	CREATE_TABLE(db, graphedge);

	TableAccess< testtableRowdata > testtable( &db );
	testtable.initialize();

	// initialize project
	string projectName = "testProject";  // this should be given at the command line
	string fileName    = "testFile.C";   // this should be retrieved from a SgFile node

	projectsRowdata prow( UNKNOWNID ,projectName, UNKNOWNID );
	projects.retrieveCreateByColumn( &prow, "name", 
			projectName );
	long projectId 	= prow.get_id();

	// get id of this file
	filesRowdata frow( UNKNOWNID, projectId , fileName );
	files.retrieveCreateByColumn( &frow, "fileName", 
			fileName, frow.get_projectId() );
	long fileId	= frow.get_id();

	// init graph
	DatabaseGraph<testtableRowdata, EdgeTypeEmpty> *callgraph = 
		new DatabaseGraph<testtableRowdata, EdgeTypeEmpty>( projectId, GTYPE_SIMPLECALLGRAPH, &db );
	callgraph->loadFromDatabase( );

	// traverse... NYI

	// save graph to dot file, and to database
	graph->writeToDOTFile( "simplecallgraph_example.dot" );
	graph->writeToDatabase( );
	delete callgraph;

	db.shutdown();
	return( 0 );
}


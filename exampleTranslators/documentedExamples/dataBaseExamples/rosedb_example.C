#include <iostream>

// database access
#include "GlobalDatabaseConnectionMYSQL.h"
#include "TableDefinitions.h"
DEFINE_TABLE_PROJECTS();
DEFINE_TABLE_FILES();

//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {

	GlobalDatabaseConnection db;
	int initOk =  db.initialize();
	assert( initOk==0 );

	CREATE_TABLE(db, projects);
	CREATE_TABLE(db, files);

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

	// do some work...
	std::cout << "Project ID:"<< projectId <<" , File ID:" << fileId << std::endl;

	db.shutdown();
	return( 0 );
}


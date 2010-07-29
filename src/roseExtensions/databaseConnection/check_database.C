
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
  if( initOk!=0 ) {
		std::cerr << "ROSE Database check: unable to connect to database!" << endl;
		return 1;
	}

	// try to init tables
  CREATE_TABLE(db, projects);
  CREATE_TABLE(db, files);

  db.shutdown();

	std::cout << "ROSE Database check: ok" << endl;
  return( 0 );
}


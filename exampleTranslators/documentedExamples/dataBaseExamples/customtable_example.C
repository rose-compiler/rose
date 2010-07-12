
// database access
#include "GlobalDatabaseConnectionMYSQL.h"
#include "TableAccess.h"

CREATE_TABLE_2( testtable,  string,name,  double,number );
DEFINE_TABLE_2( testtable,  string,name,  double,number );

//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {

	GlobalDatabaseConnection db;
	int initOk =  db.initialize();
	assert( initOk==0 );

	TableAccess< testtableRowdata > testtable( &db );
	testtable.initialize();

	// add a row
	testtableRowdata testrow( UNKNOWNID, "name", 1.0 );
	testtable.insert( &testrow );

	// select & modify
	vector<testtableRowdata> results = testtable.select( 
			" number=1.0 " );
	assert( results.size() > 0 );
	results[0].set_name( string("newname") );
	results[0].set_number( 2.0 );
	testtable.modify( &results[0] ); // this uses the ID of the row

	// remove entry
	testtable.remove( &results[0] );

	// add again for next run...
	testtable.insert( &results[0] );

	db.shutdown();
	return( 0 );
}


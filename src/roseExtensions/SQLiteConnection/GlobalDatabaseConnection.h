/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * class handling the DB connection to SQLite
 *
 *****************************************************************************/
#ifndef GLOBALDATBASECONNECTION_H
#define GLOBALDATBASECONNECTION_H

#include <sstream>
#include <iostream>
#include <sqlite3.h>
#include <dbheaders.h>
#include <Result.h>
#include <Query.h>
#include <Connection.h>

#include <assert.h>

//#include "TableAccess.h"
//#include "TableAccessMacros.h"

//! if this define equals one, the whole database is dropped during initialization (for debugging)
#define DB_DROPDATABASE 0

//! if this define equals one, all tables are dropped before creation (for debugging)
#define DB_DROPTABLES 0

//! class handling the DB connection to MySQL
class GlobalDatabaseConnection {

public:
	//! constructor, default dbname is "default.db"
	GlobalDatabaseConnection( void );
	
	//! init constructor
	GlobalDatabaseConnection(const char *dbname);

	//! destructor
	~GlobalDatabaseConnection( );


	//! initialize assuming a default project name
	int initialize( int drop = 0 );

	//! close connection to datbase
	int shutdown( void );

	//! execute a SQL query and check for success
	int execute();
	
	//! execute a SQL query and check for success, returning the resulting rows
	Result *select();

	//! execute a SQL query and check for success
	int execute(const char *query);
	
	//! execute a SQL query and check for success, returning the resulting rows
	Result *select(const char *query);

	//! get a query object for the database
	Query *getQuery( void ) { assert(connection); return connection->query(); };

	//! execute the insert_id command
	int getInsertId() { assert(connection); return connection->insert_id(); };
	
	// debugging functions

	//! DEBUGGING - print contents of all tables to stdout
	void DEBUG_dump( void );

private:


	//! is there a connection to the database?
	bool connected;

	//! mysql++ db connection object
	Connection *connection;

	//! database name for database login
	string dbName;

private:
  //! avoid copying of the database class
	GlobalDatabaseConnection &operator= ( const GlobalDatabaseConnection & db ){ assert( false ); };

};


#endif


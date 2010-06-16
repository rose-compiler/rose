/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * class handling the DB connection to MySQL
 *
 *****************************************************************************/
#ifndef GLOBALDATBASECONNECTION_H
#define GLOBALDATBASECONNECTION_H


// mysql interface
#include <mysql++.h>

#include "TableAccessMacros.h"

//! if this define equals one, the whole database is dropped during initialization (for debugging)
#define DB_DROPDATABASE 0

//! if this define equals one, all tables are dropped before creation (for debugging)
#define DB_DROPTABLES 0

//! class handling the DB connection to MySQL
class GlobalDatabaseConnection {

public:
	//! constructor
	GlobalDatabaseConnection( void );
	
	//! init constructor
	GlobalDatabaseConnection(char *host, char *username, char *pwd, char *dbname);

	//! init constructor that exposes additional sql options, such as port number and socket name
	GlobalDatabaseConnection(char *host, char *username, char *pwd, char *dbname, unsigned int port, bool compress = 0, unsigned int connect_timeout = 60, bool te = true, const char *socket_name = "", unsigned int client_flag = 0);

	//! destructor
	~GlobalDatabaseConnection( );


	//! initialize assuming a default project name
	int initialize( int drop = 0 );

	//! close connection to datbase
	int shutdown( void );

	//! set parameters for database connection, pass NULL for any to leave it unchanged
	void setDatabaseParameters(char *host, char *username, char *pwd, char *dbname);

	//! set the full suite of parameters for database connection, pass NULL for any to leave it unchanged.
	void setDatabaseParameters(char *host, char *username, char *pwd, char *dbname, unsigned int *port, bool *compress, unsigned int *connect_timeout, bool *te, const char *socket_name, unsigned int *client_flag);

	//! execute a SQL query and check for success
	int execute(const char *query);
	
	//! execute a SQL query and check for success, returning the resulting rows
    mysqlpp::StoreQueryResult *select(const char *query);

	//! get a query object for the database
    mysqlpp::Query getQuery( void ) { return connection.query(); };

	//! execute the insert_id command
	int getInsertId() { return connection.query().insert_id(); };
	
	// debugging functions

	//! DEBUGGING - print contents of all tables to stdout
	void DEBUG_dump( void );

private:


	//! is there a connection to the database?
	bool connected;

	//! mysql++ db connection object
    mysqlpp::Connection connection;

	//! database name for database login
	std::string dbName;

	//! hostname for database login
	std::string dbHost;

	//! username for database login
	std::string dbUsername;

	//! password for database login
	std::string dbPwd;

	//! port for database login
	unsigned int dbPort;

	//! compress flag for database login
	bool dbCompress;

	//! connection timeout for database login
	unsigned int dbTimeout;

	//! throw exception flag for database login
	bool dbTe;

	//! socket name for database connection
	std::string dbSock;

	//! client flag for database connection
    //! Deprecated in MySQL 5.0+
	unsigned int dbFlag;

private:
  //! avoid copying of the database class
	GlobalDatabaseConnection &operator= ( const GlobalDatabaseConnection & db ){ assert( false ); };

};


#endif


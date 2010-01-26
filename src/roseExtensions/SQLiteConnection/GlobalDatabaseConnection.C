#include "sage3basic.h"

#include <GlobalDatabaseConnection.h>

using namespace std;

//-----------------------------------------------------------------------------
// constructor
GlobalDatabaseConnection::GlobalDatabaseConnection( void ) :
	connection()
{
       connected = false;;
       dbName = "default.db";
       connection = new Connection(dbName);

	// set default database access params
	 //	setDatabaseParameters(,&myTimeout, &myFlag);
}
	
//-----------------------------------------------------------------------------
// init contructors 
GlobalDatabaseConnection::GlobalDatabaseConnection(const char *dbname)
{
	connected = false;
	ostringstream os;
	os << dbname;
	dbName = os.str();
	connection = new Connection(dbname);
	//setDatabaseParameters(dbname, &myTimeout, &myFlag);
}

	
//-----------------------------------------------------------------------------
// assume default name
int GlobalDatabaseConnection::initialize( int drop ) {
  int rc;

        assert(connection);

        if (drop)
          connection->drop_db( dbName );

	if (dbName == "") {
	  cerr << "db creation / open failed, missing dbName " << endl;
	  return 2;
	}

	rc = connection->open();
	if (rc) {
	  Query *q = getQuery();
	  cerr << "Can't open database: " << q->error();
	  return 2;
	}

	connected = true;
	//DEBUG_dump();
	return 0;
}


//-----------------------------------------------------------------------------
// close connection to datbase
int GlobalDatabaseConnection::shutdown( void )
{
  assert(connection);
	if(connected) {
		// disconnect from db
	  int rc = connection->close();
	  connected = false;
	  return rc;
	}
	return 0;
}
//-----------------------------------------------------------------------------
// execute a SQL query and check for success
int GlobalDatabaseConnection::execute(const char *command)
{
  assert(connection);
  Query *query = connection->query();
  query->set(command);
  int r = query->execute();
	//cout << "\nRETURNED FROM QUERY EXECUTE with " << r << "\n";
  if(r != 0)
    {
      //cout << "SQL execute failed: " << query->error() << endl << "Original query: " << command << endl; // debug
      //assert( false );
      return -1;
    }
  else
    return 0;
}

//-----------------------------------------------------------------------------
// execute a SQL query and check for success
int GlobalDatabaseConnection::execute()
{
  assert(connection);
  Query *query = connection->query();
  int r = query->execute();
  if(r != 0)
    {
      //cout << "SQL execute failed: " << query->error() << endl << "Original query: " << query->preview() << endl; // debug
      return -1;
    }
  else
    return 0;
}	
	
//-----------------------------------------------------------------------------
// execute a SQL query and check for success, returning the resulting rows
Result * GlobalDatabaseConnection::select(const char *command)
{
  assert(connection);
  Query *query = connection->query();
  query->set(command);
  Result *res = query->store();
  assert(res);
  if (query->success() != 0) {
    //cout << "SQL execute failed: " << query->error() << endl << "Original query: " << command << endl; // debug
    return NULL;
  }
  // everything ok
  return res;
}

//-----------------------------------------------------------------------------
// execute a SQL query and check for success, returning the resulting rows
Result * GlobalDatabaseConnection::select()
{
  assert(connection);
  Query *query = connection->query();
  Result *res = query->store();
  assert(res);
  if (query->success() != 0) {
    //cout << "SQL execute failed: " << query->error() << endl << "Original query: " << query->preview() << endl; // debug
    return NULL;
  }
  // everything ok
  return res;
}


/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * class handling a single table
 *
 *****************************************************************************/
#ifndef TABLEACCESS_H
#define TABLEACCESS_H

#include <iostream>
#include <cstdio>
#include <sstream>
#include <mysql++.h>
#include <cstring>

#include "GlobalDatabaseConnectionMYSQL.h"
//! automated table rowdata object creation macros
#include "TableAccessMacros.h"

//! if defined as 1 -> print SQL command
#define DEBUG_SQL 0

class GlobalDatabaseConnection;

//! standard ID value for DB insertion, or if ID is unknown (0 will never be assigned as ID)
#define UNKNOWNID 0

//! class to access tables in the database
template<class Rowdata>
class TableAccess {

public:
	//! constructor
	TableAccess( void );
	
	//! init constructor
	TableAccess( GlobalDatabaseConnection *set );
	
	//! init constructor
	TableAccess(char *name, char *comment , char *add);

	//! destructor
	~TableAccess( );


	//! check if table exists, create it if not
	int initialize( void );

	//! remove table
	int drop( void );

	//! get a row by id, returns !=0 upon failure
	int selectById(long id, Rowdata *ret);

	//! get row(s) with given requirements, returns empty vector upon failure
	vector<Rowdata> select(string cond);

	//! insert a given row
	int insert(Rowdata *data);

	//! modify a given row
	int modify(Rowdata *data);

	//! delete a given row
	int remove(Rowdata *data);
	

	//! higher level function to retrieve a table entry by name, or create it, if it does not exist
	long retrieveCreateByColumn(Rowdata *data, string column, string name, long projId = 0);

	//! higher level function to retrieve by name a table entry that spans multiple columns, or create it, if it does not exist
	long retrieveCreateByColumns(Rowdata *data, string columns[], string names[], int numcols, long projId = 0);

	// access functions

	//! set table parameters like name and table comment
	void setTableParameters(char *name, char *comment, char *add);

	//! add a column to the column list
	void addColumn(string colname);
	//! add a column to the column list (converting from C char array)
	void addColumn(char *colname);

	//! get table name
	string getName( void ){ return tableName; }
	//! set table name
	void setName(string set){ tableName = set; }

	//! get additional commands
	string getAdditionalCommands( void ){ return additionalCommands; }
	//! set additional commands
	void setAdditionalCommands(string set){ additionalCommands = set; }

	//! get DB connection object
	GlobalDatabaseConnection *getGlobalDatabaseConnection( void ){ return gdbConnection; }
	//! set DB connection object
	void setGlobalDatabaseConnection( GlobalDatabaseConnection *set ){ gdbConnection = set; }
	
private:

	//! has the table been initliazed?
	bool initialized;

	//! connection to database
	GlobalDatabaseConnection *gdbConnection;

	//! table name of this table
	string tableName;

	//! table comment
	string tableComment;

	//! other special SQL commands like index creation
	string additionalCommands;

	//! hostname for database login
	vector<string> columns;

};



//-----------------------------------------------------------------------------
// type definitions

// a table that does not use the rowdata interface (so it has to be initialized
// by calls to addColumn, and accessed with standard SQL queries)
typedef TableAccess<customTableRowdata> customTableAccess;




//-----------------------------------------------------------------------------
// constructor
template<class Rowdata>
TableAccess<Rowdata>::TableAccess( void )
{
	initialized = false;
	gdbConnection = NULL;
	Rowdata data;
	setTableParameters( data.table(), "", "" );
}
	
//-----------------------------------------------------------------------------
// init constructor
template< class Rowdata >
TableAccess<Rowdata>::TableAccess(GlobalDatabaseConnection *set)
{
	initialized = false;
	gdbConnection = set;
	Rowdata data;
	setTableParameters( data.table(), "", "" );
}

template< class Rowdata >
TableAccess<Rowdata>::TableAccess(char *name, char *comment, char *add)
{
	initialized = false;
	gdbConnection = NULL;
	setTableParameters( name, comment, add );
}

//-----------------------------------------------------------------------------
// destructor
template< class Rowdata >
TableAccess<Rowdata>::~TableAccess( )
{
}


//-----------------------------------------------------------------------------
// check if table exists, create it if not
template< class Rowdata >
int TableAccess<Rowdata>::initialize( void )
{
	string firstCol; 		// first column (for primary key)
	Rowdata rowdata; 		// rowdat object for custom column init

	// init columns from rowdata information, that means it's not a custom table
	if(!rowdata.custom() ) {
		//TableAccess<TableRowdataInterface> *ta = 
			//new TableAccess<TableRowdataInterface>( rowdata->getTableName(), "", "" );

		setTableParameters( rowdata.getTableName(), "" , "" );
		vector<string> cols = rowdata.getColumnDatatypes();
		vector<string> names = rowdata.getColumnNames();

		vector<string>::iterator c=cols.begin();
		vector<string>::iterator n=names.begin();
		for( ; c!=cols.end(), n!=names.end(); c++, n++ ) {
			string colDataType = (*c);
			string colSqlType = "UNDEFINED";
			// unecessary? for(int l=0;l<colDataType.length();l++) colDataType[l] = tolower( colDataType[l] );

			// translate C/C++ datatypes to default SQL datatypes
			if( colDataType == "bool" )				colSqlType = "BOOL";
			if( colDataType == "short" )			colSqlType = "SMALLINT";
			if( colDataType == "int" ) 				colSqlType = "INT";
			if( colDataType == "long" ) 			colSqlType = "INT";
			if( colDataType == "long long" )	colSqlType = "BIGINT";
			if( colDataType == "float" )			colSqlType = "FLOAT";
			if( colDataType == "double" )			colSqlType = "DOUBLE";
			if( colDataType == "string" ) 		colSqlType = "VARCHAR(255)";
			if( colDataType == "textString" )	colSqlType = "TEXT";

			string coldesc = (*n)+" "+ colSqlType;
			if(c==cols.begin()) {
				// add autoincrement to first column (id)
				coldesc += " AUTO_INCREMENT ";
			}
			addColumn( coldesc  );
		}
	}
	
	// add table fo gdb con
	//addTable( ta );

	// get all table names
    mysqlpp::StoreQueryResult *res = gdbConnection->select("SHOW TABLES;");
	bool found = false;
	for(mysqlpp::StoreQueryResult::iterator i= res->begin(); i!=res->end(); i++ ) {
        mysqlpp::Row row = *i;
	  //std::cout << " checktabss " << row[0] << " against " << tableName.c_str() << std::endl; // debug
		if( std::strcmp(tableName.c_str(),row[0]) == 0 ) {
#if DB_DROPTABLES==1
			string dropcmd = "DROP TABLE " + tableName + ";";
			std::cout << " dropping " << tableName << std::endl; // debug
			gdbConnection->execute( dropcmd.c_str() );
			found = false;
#else
			i = (res->end()-1);
			found = true;
#endif
	
		}
	}

	// creation necessary?
	if(!found) {
		//std::cout << " not found " << tableName << std::endl; // debug
		string cmd = "CREATE TABLE " + tableName + " ( ";

		// add column definitions
		for(vector<string>::iterator i= columns.begin(); i!=columns.end(); i++ ) {
			cmd += *i;
			if( (i+1) != columns.end() ) cmd += " , ";
			// remember name of first column for primary key
			if(i==columns.begin()) {
				string blank = " ";
				int pos = (*i).find(blank,0);
				assert( pos != (int)string::npos );
				firstCol = (*i).substr(0,pos);
			}
		}

		// add non-vital commands
		if( additionalCommands.length() == 0 ){
			// create primary key for first column by default
			additionalCommands = " PRIMARY KEY (" + firstCol + ") ";
		}
		cmd += " , " + additionalCommands;

		cmd += " ) ";
		if( tableComment.length() > 0 ){
			cmd += " COMMENT='"+tableComment+"' ";
		}
		
		cmd += " ;";
		//std::cout << "TableAccess::init executing: " << cmd << std::endl; // debug
		gdbConnection->execute( cmd.c_str() );
	}

	
	initialized = true;
	return 0;
}


//-----------------------------------------------------------------------------
// remove table
template< class Rowdata >
int TableAccess<Rowdata>::drop( void )
{
	return 0;
}




//-----------------------------------------------------------------------------
// get a row by id
template< class Rowdata >
int TableAccess<Rowdata>::selectById(long id, Rowdata *data)
{
	if(!gdbConnection) {
		std::cout << "Error TableAccess<T>::selectById : database connection not initialized" << std::endl; // debug
		assert( false );
		return 1;
	}

	mysqlpp::Query query =  gdbConnection->getQuery();
	query << "SELECT " << data->field_list() <<" FROM "<<  data->table() << " WHERE id='" << id << "' ;";
#if DEBUG_SQL==1
	std::cout << "TableAccess<T>::selectById : " << query.preview() << std::endl; // debug
#endif
    mysqlpp::StoreQueryResult res = query.store();
	if(! res ) {
		std::cout << "Error TableAccess<T>::selectById : " << query.error() << std::endl; // debug
		assert( false );
		return 2;
	}
	// set rowdata object
	data->set( res[0] );
	return 0;
}


//-----------------------------------------------------------------------------
// get row(s) with given requirements
template< class Rowdata >
vector<Rowdata> TableAccess<Rowdata>::select(string cond)
{
	vector<Rowdata> ret;
	Rowdata dummyData;
	if(!gdbConnection) {
		std::cout << "Error TableAccess<T>::select: database connection not initialized" << std::endl; // debug
		assert( false );
		return ret;
	}

	mysqlpp::Query query =  gdbConnection->getQuery();
	query << "SELECT " << dummyData.field_list() <<" FROM "<<  dummyData.table() << " WHERE " << cond << ";";
#if DEBUG_SQL==1
	std::cout << "TableAccess<T>::select: " << query.preview() << std::endl; // debug
#endif
    mysqlpp::StoreQueryResult res = query.store();
	if(! res ) {
		std::cout << "Error TableAccess<T>::select: " << query.error() << std::endl; // debug
		assert( false );
		return ret;
	}
	// copy results into a vector
	for(mysqlpp::StoreQueryResult::iterator i= res.begin(); i!=res.end(); i++ ) {
        mysqlpp::Row row = *i;
		Rowdata rd( row );
		ret.push_back(rd);
	}
	return ret;
}


//-----------------------------------------------------------------------------
// insert a given row
template< class Rowdata >
int TableAccess<Rowdata>::insert(Rowdata *data)
{
	if(!gdbConnection) {
		std::cout << "Error TableAccess<T>::insert : database connection not initialized" << std::endl; // debug
		assert( false );
		return -2;
	}

	mysqlpp::Query query =  gdbConnection->getQuery();
	query.insert( *data );
#if DEBUG_SQL==1
	std::cout << "TableAccess<T>::insert : " << query.preview() << std::endl; // debug
#endif
    mysqlpp::SimpleResult res = query.execute();
	if(! res ) {
		std::cout << "Error TableAccess<T>::insert : " << query.error() << std::endl; // debug
		assert( false );
		return -1;
	}
	int id = gdbConnection->getInsertId();
	data->set_id( id );
	return id;
}


//-----------------------------------------------------------------------------
// modify a given row
template< class Rowdata >
int TableAccess<Rowdata>::modify(Rowdata *data)
{
	long id = data->get_id();
	if(!gdbConnection) {
		std::cout << "Error TableAccess<T>::modify : database connection not initialized" << std::endl; // debug
		assert( false );
		return -2;
	}

	mysqlpp::Query query = gdbConnection->getQuery();
	query << "UPDATE " << data->table() << " SET " << data->equal_list() << 
		" WHERE id='" << id << "' ; ";
#if DEBUG_SQL==1
	std::cout << "TableAccess<T>::modify : " << query.preview() << std::endl; // debug
#endif
    mysqlpp::SimpleResult res = query.execute();
	if(! res ) {
		std::cout << "Error TableAccess<T>::modify : " << query.error() << std::endl; // debug
		assert( false );
		return -1;
	}
	return 0;
}


//-----------------------------------------------------------------------------
// delete a given row
template< class Rowdata >
int TableAccess<Rowdata>::remove(Rowdata *data)
{
	if(!gdbConnection) {
		std::cout << "Error TableAccess<T>::remove : database connection not initialized" << std::endl; // debug
		assert( false );
		return -2;
	}

	char cmdstr[256];
  snprintf(cmdstr, 256,"DELETE FROM %s WHERE id='%d';", data->table(), data->get_id() );
#if DEBUG_SQL==1
	std::cout << "TableAccess<T>::remove : " << cmdstr << std::endl; // debug
#endif
	gdbConnection->execute( cmdstr );
	return 0;
}


//-----------------------------------------------------------------------------
// set table parameters like name and table comment
template< class Rowdata >
void TableAccess<Rowdata>::setTableParameters(char *name, char *comment, char *add )
{
	if(name) 		tableName = name;
	if(comment) tableComment = comment;
	if(add) 		additionalCommands = add;
}


//-----------------------------------------------------------------------------
// add a column to the column list
template< class Rowdata >
void TableAccess<Rowdata>::addColumn( string colname )
{ 
	assert(!initialized); 
	columns.push_back( colname ); 
}

// add a column to the column list (converting from C char array)
template< class Rowdata >
void TableAccess<Rowdata>::addColumn(char *colname)
{ 
	string col = colname;
	addColumn( col );
}


//-----------------------------------------------------------------------------
// higher level function to retrieve a table entry by name, or create it, if it does not exist
// optional "colid" - specify index of name column, if != 1
// optional "projId" - specify additional condition for checking project ID (only used if >0)
template< class Rowdata >
//long TableAccess<Rowdata>::retrieveCreateByName(Rowdata *data,string name, int colid = 1, long projId = 0)
long TableAccess<Rowdata>::retrieveCreateByColumn(Rowdata *data, string column, string name, long projId)
{
	// initialize project
	long id = 0;               // as of yet undefined - this _HAS_ to be '0' first AUTO_INCREMENT to work!
	//char *name = "testProj"; // test name
	vector<string> columnNames = data->getColumnNames();
	bool found = false;
	//string columnName;
	//int n=0;
	for(vector<string>::iterator i=columnNames.begin();
			i!=columnNames.end(); i++) {
		//if(n==colid) columnName = *i;
		//n++;
		if( (*i) == column ) found=true;
	}
	assert( found );

	std::ostringstream cmd;
	cmd	<< column <<"='" << name << "'";
	if( projId > 0 ) {
		// add additional project id condition if id>0
		cmd << " AND projectId='" << projId << "' ";
	}

	vector<Rowdata> results = select( cmd.str() );
	if(results.size() == 0) {
		// insert new entry, and retrieve id
		insert( data );
		results = select( cmd.str() );
		if(results.size() != 1) {
			// insert failed?? -> fatal
			assert(false);
			return -1;
		}
		*data = results[0];
		// FIXME, user insert_id ?
		//std::cout << "IDinsert" <<": "<< results[0].equal_list() << std::endl; // debug
		//std::cout << "IDinsert" <<": "<< data->equal_list() << std::endl; // debug
	}
	else if(results.size() > 1) {
		// more than one project with this name?? -> fatal
		assert(false);
		return -1;
	} else {
		// project entry found
		*data = results[0];
		//std::cout << "IDfound" <<": "<< results[0].equal_list() << std::endl; // debug
		//std::cout << "IDfound" <<": "<< data->equal_list() << std::endl; // debug
	}
	id = data->get_id();
	//std::cout << " rcID " << id << std::endl; // debug
	return id;
}

template< class Rowdata >
long TableAccess<Rowdata>::retrieveCreateByColumns(Rowdata *data, string columns[], string names[], int numcols, long projId)
{
	// initialize project
	long id = 0;               // as of yet undefined - this _HAS_ to be '0' first AUTO_INCREMENT to work!
	//char *name = "testProj"; // test name
	vector<string> columnNames = data->getColumnNames();
	bool found = false;
	//string columnName;
	//int n=0;

	for(int cur = 0; cur < numcols; cur++) {
	  for(vector<string>::iterator i=columnNames.begin();
	      i!=columnNames.end(); i++) {
	    //if(n==colid) columnName = *i;
	    //n++;
	    if( (*i) == columns[cur] ) { found=true; break; }
	  }
	  assert( found );
	}

	std::ostringstream cmd;
	for(int cur = 0; cur < numcols; cur++) {
	  if (cur != (numcols - 1))
	    cmd	<< columns[cur] <<"='" << names[cur] << "' AND ";
	  else 
	    cmd	<< columns[cur] <<"='" << names[cur] << "'";
	}
	if( projId > 0 ) {
		// add additional project id condition if id>0
		cmd << " AND projectId='" << projId << "' ";
	}

	vector<Rowdata> results = select( cmd.str() );
	if(results.size() == 0) {
		// insert new entry, and retrieve id
		insert( data );
		results = select( cmd.str() );
		if(results.size() != 1) {
			// insert failed?? -> fatal
			assert(false);
			return -1;
		}
		*data = results[0];
		// FIXME, user insert_id ?
		//std::cout << "IDinsert" <<": "<< results[0].equal_list() << std::endl; // debug
		//std::cout << "IDinsert" <<": "<< data->equal_list() << std::endl; // debug
	}
	else if(results.size() > 1) {
		// more than one project with this name?? -> fatal
		assert(false);
		return -1;
	} else {
		// project entry found
		*data = results[0];
		//std::cout << "IDfound" <<": "<< results[0].equal_list() << std::endl; // debug
		//std::cout << "IDfound" <<": "<< data->equal_list() << std::endl; // debug
	}
	id = data->get_id();
	//std::cout << " rcID " << id << std::endl; // debug
	return id;
}

#endif


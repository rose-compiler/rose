
//-----------------------------------------------------------------------------
//! class handling the DB connection to MySQL
class GlobalDatabaseConnection {
public:
	//! constructor
	GlobalDatabaseConnection( void );
	
	//! init constructor
	GlobalDatabaseConnection(char *host, char *username, char *pwd, char *dbname);

	//! destructor
	~GlobalDatabaseConnection( );


	//! initialize assuming a default project name
	int initialize( void );

	//! close connection to datbase
	int shutdown( void );

	//! set parameters for database connection, pass NULL for any to leave it unchanged
	void setDatabaseParameters(char *host, char *username, char *pwd, char *dbname);

	//! execute a SQL query and check for success
	int execute(const char *query);
	
	//! execute a SQL query and check for success, returning the resulting rows
	Result *select(const char *query);

	//! get a query object for the database
	Query getQuery( void ) { return connection.query(); };
};



//-----------------------------------------------------------------------------
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
	vector<Rowdata> selectCustom(string cond);

	//! insert a given row
	int insert(Rowdata *data);

	//! modify a given row
	int modify(Rowdata *data);

	//! delete a given row
	int remove(Rowdata *data);
	
	//! higher level function to retrieve a table entry by name, 
	//  or create it, if it does not exist
	long retrieveCreateByColumn(Rowdata *data, string column, 
			string name, long projId = 0);


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

};


//-----------------------------------------------------------------------------
//! example of a rowdata class generated with the SQL_CREATE macros
class <rowdataClass> : public TableRowdataInterface {
public: 


	// functions inherited from TableRowdataInterface
	//! a virtual desctructor is required
	virtual ~<rowdataClass>();

	//! is it a custom table? 
	virtual bool custom( void ) = 0;

	//! init data from row
	virtual void set(const MysqlRow &row) = 0;

	//! get the name of the table
	char *getTableName() = 0;

	//! get a vector of strings of the column datatypes
	virtual vector<string> getColumnDatatypes( void ) = 0;

	//! get a vector of strings of the column names
	virtual vector<string> getColumnNames( void ) = 0;


	// useful functions from the mysql++ implementation
	<rowdataClass> (const <columnType1>&p1, const <columnType2> &p2, ...);
	void set (const <columnType1> &p1, const <columnType2> &p2, ... ); 

	// comparison operators working with column 'id'
	bool operator == (const <rowdataClass> &other) const;
	bool operator != (const <rowdataClass> &other) const; 
	bool operator >  (const <rowdataClass> &other) const; 
	bool operator >= (const <rowdataClass> &other) const; 
	...

	// return a string of the concatenated column values/names, 
	// useful for creation of SQL commands
	string field_list( void );
	string value_list( void );


	// access function for all columns:
	<columnType1> get_<columnName1>( void );
	void set_<columnName1>( <columnType1> set );
	
	<columnType2> get_<columnName2>( void );
	void set_<columnName2>( <columnType2> set );
	
	<columnType3> get_<columnName3>( void );
	void set_<columnName3>( <columnType3> set );

	...

private:
		
	// column storage
	<columnType1> <columnName1>;
	<columnType2> <columnName2>;
	<columnType3> <columnName3>;
	...

};
	

//-----------------------------------------------------------------------------
//! a node in the DB graph
template<class NodeType>
class GraphNode {
public:
	//! constructor
	GraphNode( NodeType node, string setname){ name = setname; content = node; };

	//! destructor
	~GraphNode() { /* nothing to do */ };

	//! the node type object
	NodeType content;

	//! name of this node (for display of the graph)
	string name;

	//! children/successor nodes of the current one
	vector<GraphNode *> children;
};

//-----------------------------------------------------------------------------
//! graph database storage class 
template<class NodeType>
class DatabaseGraph {
public:
	//! constructor
	DatabaseGraph( long pid, long type, 
			short namecol, GlobalDatabaseConnection *gdb );
	
	//! destructor
	~DatabaseGraph( );

	//! add a node without children (if it's not already there)
	int addNode(NodeType node, string name);

	//! add a node as child of another node
	int addEdge(NodeType parent, NodeType child, 
			bool allowMultiple = true );

	//! remove all children pointers from a node
	int clearChildren(NodeType parent);

	//! remove node from the tree
	int removeNode( NodeType node );

	//! check if there is an edge in the graph pointing to this node
	int nodeIsUsed(NodeType node);

	//! write DOT graph to filename
	void writeToFile(string filename);

	//! load graph from datbase
	int loadFromDatabase( void );

	//! store graph in datbase
	int writeToDatabase( void );

};




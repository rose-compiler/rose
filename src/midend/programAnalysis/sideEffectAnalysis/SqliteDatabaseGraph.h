/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * graph database storage class definition
 *
 *****************************************************************************/

#ifndef SQLITE_DATABASEGRAPH_H
#define SQLITE_DATABASEGRAPH_H

// for graph output as .dot file
#include <cstring>
#include <algorithm>

// use the boost graph files
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"
#include "boost/graph/exception.hpp"
#include "boost/graph/graphviz.hpp"

#include <rose.h>
#include "sqlite3x.h"

//! the different graph types, used for graphdata.graphType field
#define GTYPE_CALLGRAPH 				1
#define GTYPE_CLASSHIERARCHY		2
#define GTYPE_TESTGRAPH					3 		// for database graph example
#define GTYPE_SIMPLECALLGRAPH		4 		// for simple callgraph example
#define GTYPE_USERDEFINED				100 	// user defined graphs should start with this id


//--------------------------------------------------------------------
// Schema Setup
//--------------------------------------------------------------------
#define PROJECTSTBL   "projects"
#define GRAPHDATATBL  "graphdata"
#define GRAPHNODETBL  "graphnode"
#define GRAPHEDGETBL  "graphedge"

// VertexType
class dbTable
{
  public:
    dbTable(string& table)
    {
      name = table;
    }

    string table() { return name; }
    vector<string> field_list() { return columnNames; }
    vector<string> field_types() { return columnTypesl }

    string name;
    vector<string> columnNames;
    vector<string> columnTypes;
}

// EdgetType
class dbRow : dbTable
{
  public:
    dbRow(string& table) : dbTable(table) {}
    dbRow(sqlite3x::sqlite3_reader r) { load(r); }

    void set_id(int id) { rowid = id; }
    int get_id() { return rowid; }

    virtual bool load(sqlite3x::sqlite3_reader r)=0;
    virtual bool insert(sqlite3x::sqlite3_connection db)=0;

    int rowid;
}

class cgdata : dbRow
{
  public:

    cgData(int project, int type): dbRow("graphdata")
    {
      columnNames.push_back("id");
      columnNames.push_back("projectid");
      columnNames.push_back("graphType");

      columnTypes.push_back("int");
      columnTypes.push_back("int");
      columnTypes.push_back("int");

      projectId = project;
      graphType = type;
    }
    cgData(sqlite3x::sqlite3_reader r) { load(r); }

    // Load from database
    bool load(sqlite3::sqlite3_reader r)
    {
      set_id( r.getint(0) );
      projectid = r.getint(1)
      graphType = r.getint(2);
      return true;
    }

    // Insert into database
    // NOTE: does not check for duplicates
    bool insert(sqlite3x::sqlite3_connection db)
    {
      sqlite3x::sqlite3_comment insertcmd(db,
        "INSERT INTO graphdata (\"projectid\",\"graphType\")
        VALUES (?,?);");
      insertcmd.bind(0,projectId);
      insertcmd.bind(1,graphType);
      insertcmd.executeonquery();

      return insertcmd.read();
    }

    void set_graphid(int id) { gid = id; }
    int  get_graphid() { return gid; }

    void set_projectid(int id) { projectId = id; }
    int  get_projectid() { return projectId; }

    void set_graphtype(int type) { graphType = type; }
    int  get_graphtype() { return graphType; }

    int gid;
    int projectId;
    int graphType;
}

class cgNode : dbRow
{
  public:
    cgNode(int graphid, int node, string& name): dbRow("graphnode")
  {
    columnNames.push_back("id");
    columnNames.push_back("graphid");
    columnNames.push_back("nodeId");
    columnNames.push_back("name");

    columnTypes.push_back("int");
    columnTypes.push_back("int");
    columnTypes.push_back("int");
    columnTypes.push_back("string");

    gid = graphid;
    nodeId = node;
    nodeName = name;
  }
    cgNode(sqlite3x::sqlite3_reader r) { load(r); }

    bool load(sqlite3::sqlite3_reader r)
    {
      rowid = r.getint(0);
      gid = r.getint(1);
      nodedid = r.getint(2);
      nodeName = r.getstring(3);
    }

    bool insert(sqlite3x::sqlite3_connection db)
    {
      sqlite3x::sqlite3_comment insertcmd(db,
        "INSERT INTO graphnode (\"graphid\",\"nodeid\",\"name\")
        VALUES (?,?,?);");
      insertcmd.bind(0,gid);
      insertcmd.bind(1,nodeId);
      insertcmd.bind(2,nodeName);
      insertcmd.executeonquery();

      return insertcmd.read();
    }

    void set_graphid(int id) { gid = id; }
    int  get_graphid() { return gid; }

    void set_nodeid(int id) { nodeId = id; }
    int  get_nodeid() { return nodeId; }

    void set_nodename(string& name) { nodeName = name; }
    string get_nodename() { return nodeName; }

    int gid;
    int nodeId;
    string nodeName;
}

class cgEdge : dbRow {
  public:
    cgEdge(int graphid, int edge, int source, int target): dbRow("graphedge")
    {
      columnNames.push_back("id");
      columnNames.push_back("graphid");
      columnNames.push_back("edgeid");
      columnNames.push_back("sourceId");
      columnNames.push_back("targetId");

      columnTypes.push_back("int");
      columnTypes.push_back("int");
      columnTypes.push_back("int");
      columnTypes.push_back("int");
      columnTypes.push_back("int");

      gid = graphid;
      edgeId = edge;
      sourceId = source;
      targetId = target;
    }
    cgEdge(sqlite3x::sqlite3_reader r) { load(r); }

    bool string(sqlite3::sqlite3_reader r)
    {
      rowid = r.getint(0);
      gid = r.getint(1);
      edgeid = r.getint(2);
      sourceId = r.getint(3);
      targetId = r.getint(4);
    }

    bool insert(sqlite3x::sqlite3_connection db)
    {
      sqlite3x::sqlite3_comment insertcmd(db,
        "INSERT INTO graphedge (\"graphid\",\"edgeid\",\"sourceId\",
        \"targetId\") VALUES (?,?,?,?);");
      insertcmd.bind(0,gid);
      insertcmd.bind(1,edgeId);
      insertcmd.bind(2,sourceId);
      insertcmd.bind(3,targetId);
      insertcmd.executeonquery();

      return insertcmd.read();
    }

    void set_graphid(int id) { gid = id; }
    int  get_graphid() { return gid; }

    void set_edgeid(int id) { edgeId = id; }
    int  get_edgeid() { return edgeId; }

    int set_sourceId(int source) { sourceId = source; }
    int get_sourceId() { return sourceId; }

    int set_targetId(int target) { targetId = target; }
    int get_targetId() { return targetId; }

    int gid;
    int edgeId;
    int sourceId;
    int targetId;
}
    


bool
define_schema(sqlite3x::sqlite3_connection& gDB)
{
  gDB.executeonquery("CREATE TABLE IF NOT EXISTS projects
      (id INTEGER PRIMARY KEY, name TEXT, callgraphRootId INTEGER)");
  gDB.executeonquery("CREATE TABLE IF NOT EXISTS graphdata
      (id INTEGER PRIMARY KEY, projectId INTEGER, graphType INTEGER)");
  gDB.executeonquery("CREATE TABLE IF NOT EXISTS graphnode
      (id INTEGER PRIMARY KEY, graphId INTEGER, nodeId INTEGER, name TEXT)");
  gDB.executeonquery("CREATE TABLE IF NOT EXISTS graphedge
      (id INTEGER PRIMARY KEY, graphId INTEGER, edgeId INTEGER,
       sourceId INTEGER, targetID INTEGER)");
}


// install boost database graph property map types
namespace boost {
	enum vertex_dbg_data_t { vertex_dbg_data };
	BOOST_INSTALL_PROPERTY( vertex, dbg_data );

	enum edge_dbg_data_t { edge_dbg_data };
	BOOST_INSTALL_PROPERTY( edge, dbg_data );

	enum graph_dbg_data_t { graph_dbg_data };
	BOOST_INSTALL_PROPERTY( graph, dbg_data );
};

//-----------------------------------------------------------------------------
//! graph database storage class 
template<class VertexType, class EdgeType, 
	class BoostVertexList = boost::vecS, class BoostEdgeList = boost::vecS, class BoostDirection = boost::bidirectionalS,
	class BoostVertexProperty = boost::GraphvizVertexProperty, class BoostEdgeProperty = boost::GraphvizEdgeProperty, class BoostGraphProperty = boost::GraphvizGraphProperty >
class DatabaseGraph : 
	public boost::adjacency_list<
		BoostVertexList, BoostEdgeList, BoostDirection,
		boost::property< boost::vertex_index1_t, std::size_t,
            boost::property<boost::vertex_name_t, std::string,
            boost::property<boost::vertex_color_t, boost::default_color_type,
			boost::property<boost::vertex_dbg_data_t, VertexType,
            BoostVertexProperty > > > >,
		boost::property<boost::edge_dbg_data_t, EdgeType, BoostEdgeProperty>,
		boost::property<boost::graph_dbg_data_t, int, BoostGraphProperty>
	>
{

public:
	
	//! boost graph typedefs
        typedef boost::adjacency_list<
		//boost::vecS, boost::vecS, bidirectionalS,
		BoostVertexList, BoostEdgeList, BoostDirection,
		boost::property< boost::vertex_index1_t, std::size_t,
        boost::property<boost::vertex_name_t, std::string,
        boost::property<boost::vertex_color_t, boost::default_color_type,
			boost::property<boost::vertex_dbg_data_t, VertexType,
            BoostVertexProperty > > > >,
		boost::property<boost::edge_dbg_data_t, 	EdgeType,
        BoostEdgeProperty>,
		boost::property<boost::graph_dbg_data_t, 	int, BoostGraphProperty>
	> dbgType;
	typedef typename boost::graph_traits< dbgType >::vertex_descriptor dbgVertex;
	typedef typename boost::graph_traits< dbgType >::vertex_iterator 	dbgVertexIterator;
	typedef typename boost::graph_traits< dbgType >::edge_descriptor  	dbgEdge;
	typedef typename boost::graph_traits< dbgType >::edge_iterator  		dbgEdgeIterator;
	typedef typename std::pair<bool, dbgEdge>										dbgEdgeReturn;

	//! constructor
	DatabaseGraph( long pid, long type, sqlite3x::sqlite3_connection& *gdb );
	
	//! destructor
	~DatabaseGraph( );

	//! add a vertex to the graph, returns the boost vertex descriptor
	dbgVertex insertVertex(VertexType& e1, string name);
	//! add a vertex with subgraph information - subgraphs are currently not supported!!!
	dbgVertex insertVertex(VertexType& e, string name, int subgraphId, string subgraphName);
	//! insert an edge between e1 and e2
	dbgEdgeReturn insertEdge(VertexType& e1, VertexType& e2, EdgeType& value);
	//! insert an edge between e1 and e2, using the empty edge data class
        dbgEdgeReturn insertEdge(VertexType& e1, VertexType& e2);

        //! search for a specific edge in the graph, using operator= on the id, returns false if not found 
        bool searchEdge(EdgeType &edge, dbgEdge &edesc) const; 

	// helper functions

	//! set subgraph initialization information
	void setSubgraphInit(int refcol, TableRowdataInterface *rowdata, int namecol);

	//! remove all successor pointers from a node
	int clearSuccessors(VertexType& parent);

	//! remove node from the tree
	int removeNode( VertexType& node );

	//! check if there is an edge in the graph pointing to this node
	int nodeIsUsed(VertexType& node);

	//! get graph id (this function creates an graphdata entry, if the id is not retrieved by another function yet)
	int getGraphId();

	//! set the column number of the VertexType column to take the node names from
	void setNodeNameColumn( int set ){ mNameColumn = set; }


	// file functions

	//! load graph from datbase
	int loadFromDatabase( void );

	//! store graph in datbase
	int writeToDatabase( void );
		
	//! write DOT graph to filename
	void writeToDOTFile(string filename);
	
	//! write the adjacency matrix to a file (only integer matrix entries, raw file format)
	void writeAdjacencyMatrixToFileRaw(string filename) { };
		
	//! write the adjacency matrix to a file (in MCL raw file format for mcxassemble)
	void writeAdjacencyMatrixToFileMcl(string filename) { };
		

private:

	//! search for a specific node in the graph, using operator=, returns NULL if not found
	bool searchVertex(VertexType &node, dbgVertex &vdesc) const;


	// member variables

	//! project id for this graph
	long mProjectId;

	//! graph id from the graphdata table
	long mGraphId;

	//! graph type
	long mType;

	//! column index of the column used for the node names
	short mNameColumn;

	//! database connection pointer
	sqlite3x::sqlite3_connection *mpGDB;

	//! initialize subgraph information from another table?
	bool mInitSubgraphs;

	//! column id for subgraph table id's
	int mSubgraphReferenceColumn;

	//! table rowdata object for selection of subgraph names
	TableRowdataInterface *mpSubgraphNameRowdata;

	//! index of subgraph name table column 
	int mSubgraphNameColumn;
};



// for shorter function definitions...
#define DBG_TEMPLATE_DEF template<class VertexType, class EdgeType,\
	class BoostVertexList, class BoostEdgeList, class BoostDirection,\
	class BoostVertexProperty, class BoostEdgeProperty, class BoostGraphProperty>
#define DBG_TEMPLATE_CLASSES VertexType, EdgeType, BoostVertexList, BoostEdgeList, BoostDirection, BoostVertexProperty, BoostEdgeProperty, BoostGraphProperty


//-----------------------------------------------------------------------------
// constructor
DBG_TEMPLATE_DEF 
DatabaseGraph<DBG_TEMPLATE_CLASSES>::DatabaseGraph( long pid, long type, sqlite3x::sqlite3_connection *gdb )
{
	mProjectId = pid;
	mType      = type;
	mpGDB      = gdb;
	mNameColumn = -1;
	mInitSubgraphs = false;
	mGraphId = -1;
}

DBG_TEMPLATE_DEF 
typename DatabaseGraph<DBG_TEMPLATE_CLASSES>::dbgVertex 
DatabaseGraph<DBG_TEMPLATE_CLASSES>::insertVertex(VertexType& e1, string name) 
{ 
	dbgVertex vdesc;
	if(searchVertex(e1, vdesc) ) {
		// already there... , only set name etc.
	} else {
		vdesc = add_vertex( *this );
		put( boost::vertex_dbg_data, *this, vdesc, e1 );
		put( boost::vertex_index1, *this, vdesc, e1.get_id() );
	}
	put( boost::vertex_name, *this, vdesc, name );
	// WARNING - dont use get( attr, G, vdesc)["label"] !!! , that's not for writing
	get( boost::vertex_attribute, *this )[vdesc]["label"] = name;
	//cerr << " added vertex || " << get(vertex_index1, *this, vdesc) << " " << name<< endl; // debug
	//return e1.get_id();
	return vdesc;
}

DBG_TEMPLATE_DEF 
typename DatabaseGraph<DBG_TEMPLATE_CLASSES>::dbgVertex 
DatabaseGraph<DBG_TEMPLATE_CLASSES>::insertVertex(VertexType& e1, string name, int subgraphId, string subgraphName) 
{ 
	// TODO init subgraphs
	dbgVertex vdesc;
	if(searchVertex(e1, vdesc) ) {
		// already there... , only set name etc.
	} else {
		vdesc = add_vertex( *this );
		put( boost::vertex_dbg_data, *this, vdesc, e1 );
		put( boost::vertex_index1, *this, vdesc, e1.get_id() );
	}
	//vdesc = add_vertex( *this );
	put( boost::vertex_name, *this, vdesc, name );
	get( boost::vertex_attribute, *this )[vdesc]["label"] = name;
	//cerr << " added vertex sg || " << get(vertex_index1, *this, vdesc) << " " << name<< endl; // debug
	//return e1.get_id();
	return vdesc;
}

//-----------------------------------------------------------------------------
// insert an edge between e1 and e2
DBG_TEMPLATE_DEF 
typename DatabaseGraph<DBG_TEMPLATE_CLASSES>::dbgEdgeReturn 
DatabaseGraph<DBG_TEMPLATE_CLASSES>::insertEdge(VertexType& e1, VertexType& e2, EdgeType& value) 
{ 
	dbgEdgeReturn ret;
	ret.first = false;
	dbgVertex vi1;
	if(! searchVertex(e1, vi1) ) return ret;
	dbgVertex vi2;
	if(! searchVertex(e2, vi2) ) return ret;

	dbgEdge ei;
        EdgeType dummy;
	// the original code did not search for duplicate empty edges
	// so we don't either when dealing with edge edges.
	// we need to know whether there is data associated with this
	// edge ... declare an edge of the given template parameter
	// and see if we can cast it to an empty edge.  this
	// probably should use specialization instead.  BSW
        if ( ( dynamic_cast<EdgeTypeEmpty *>( &dummy ) != NULL ) ||
	     ( !searchEdge(value, ei) ) ) {
	  pair<dbgEdge, bool> eres = add_edge( vi1, vi2,  *this );
	  if(!eres.second) {
	    cerr << " add edge || failed!!! " << endl;
	    return ret;
	  }
	  put( boost::edge_dbg_data, *this, eres.first, value );
	  ei = eres.first;
	  //cerr << " added edge || " << get(vertex_index1, *this, boost::target(ei,*this))<<","<< get(vertex_index1, *this, boost::source(ei,*this)) << endl; // debug
	  //cerr << "   for vertex |1 " << get(vertex_index1, *this, boost::target(ei,*this)) << " " << get(vertex_attribute, *this, boost::target(ei,*this))["label"] << endl; // debug
	  //cerr << "   for vertex |2 " << get(vertex_index1, *this, boost::source(ei,*this)) << " " << get(vertex_attribute, *this, boost::source(ei,*this))["label"] << endl; // debug
	}
	ret.first = true;
	ret.second = ei;
	return ret;
}

//-----------------------------------------------------------------------------
// insert an edge between e1 and e2, using the empty edge data class
DBG_TEMPLATE_DEF 
typename DatabaseGraph<DBG_TEMPLATE_CLASSES>::dbgEdgeReturn 
DatabaseGraph<DBG_TEMPLATE_CLASSES>::insertEdge(VertexType& e1, VertexType& e2)
{
	EdgeTypeEmpty empty;
	return insertEdge(e1,e2, empty);
}




//-----------------------------------------------------------------------------
// destructor
DBG_TEMPLATE_DEF DatabaseGraph<DBG_TEMPLATE_CLASSES>::~DatabaseGraph( )
{
	// FIXME delete all nodes?
}


//-----------------------------------------------------------------------------
// set subgraph initialization information
DBG_TEMPLATE_DEF void DatabaseGraph<DBG_TEMPLATE_CLASSES>::setSubgraphInit(int refcol, TableRowdataInterface *rowdata, int namecol)
{
	mInitSubgraphs = true;
	mSubgraphReferenceColumn = refcol;
	mpSubgraphNameRowdata = rowdata;
	mSubgraphNameColumn = namecol;
}




//-----------------------------------------------------------------------------
// remove all successor pointers from a node
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::clearSuccessors(VertexType &node)
{
	dbgVertex srcVert;
	if(!searchVertex( node, srcVert )) return false;	
	remove_out_edge_if( srcVert, always_true_pred(srcVert, *this), *this );
	return true;
}

//! helper predicate for clearSuccessors function, always returns true
template <typename Vertex, typename Graph>
	struct always_true_predicate {
		always_true_predicate(Vertex u, const Graph& g) { }
		template <class Edge>
			bool operator()(const Edge& e) const {
				return true;
			}
	};
template <typename Vertex, typename Graph>
inline always_true_predicate<Vertex, Graph>
always_true_pred(Vertex u, const Graph& g) {
	return always_true_predicate<Vertex, Graph>(u, g);
}
	


//-----------------------------------------------------------------------------
// write DOT graph to filename
DBG_TEMPLATE_DEF void DatabaseGraph<DBG_TEMPLATE_CLASSES>::writeToDOTFile(string filename)
{
	std::ofstream fileout( filename.c_str() );

	dbgType *tG = dynamic_cast<dbgType *>( this );
	write_graphviz( fileout, *this, 
			make_vertex_attributes_writer(*tG),
			make_edge_attributes_writer(*tG),
			make_graph_attributes_writer(*tG) );
	
	fileout.close();
}


//-----------------------------------------------------------------------------
// search for a specific node in the graph, using operator=, returns NULL if not found

//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF bool DatabaseGraph<DBG_TEMPLATE_CLASSES>::searchVertex(VertexType &node, dbgVertex &vdesc) const
{
	typename boost::graph_traits< dbgType >::vertex_iterator vi,vend;
	tie(vi,vend) = vertices( *this );
	for(; vi!=vend; vi++) {
		if( get( boost::vertex_dbg_data,  *this , *vi).get_id() == node.get_id() ) {
			vdesc = *vi;
			return true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------
// search for a specific edge in the graph, using operator= on the id, returns NULL if not found

//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF bool DatabaseGraph<DBG_TEMPLATE_CLASSES>::searchEdge(EdgeType &edge, dbgEdge &edesc) const
{
  

	typename boost::graph_traits< dbgType >::edge_iterator ei,eend;
	tie(ei,eend) = edges( *this );
	// milki (06/23/2010) edge_iterators use preincrement
	for(; ei!=eend; ++ei) {
		if( get( boost::edge_dbg_data,  *this , *ei).get_id() == edge.get_id() ) {
			edesc = *ei;
			return true;
		}
	}
	return false;
}



//-----------------------------------------------------------------------------
// check if there is an edge in the graph pointing to this node
//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::nodeIsUsed(VertexType &node)
{
	dbgVertex vi;
	if(! searchVertex(node, vi) ) return false;
	
	typename boost::graph_traits< dbgType >::in_edge_iterator ii,iend;
	tie(ii,iend) = in_edges(vi, *this );
	if( ii == iend ) return false;
	return true;
}


//-----------------------------------------------------------------------------
// check if there is an edge in the graph pointing to this node
//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::removeNode(VertexType& node)
{
	dbgVertex vi;
	if(! searchVertex(node, vi) ) return false;

	//remove_edge_if( remove_node_pred(srcVert, *this), *this );
	clear_vertex( vi, *this );
	remove_vertex( vi, *this );
	return true;
}

//! helper predicate for removeNode function
/*template <typename Vertex, typename Graph>
	struct remove_node_predicate {
		remove_node_predicate(Vertex u, const Graph& g) : srcVert(u), G(g) { }
		template <class Edge>
			bool operator()(const Edge& e) const {
				if( boost::source( e, G ) == srcVert ) return true;
				if( boost::target( e, G ) == srcVert ) return true;
			}
		Vertex srcVert;
		Graph G;
	};
template <typename Vertex, typename Graph>
inline always_true_predicate<Vertex, Graph>
remove_nod_pred(Vertex u, const Graph& g) {
	return always_true_predicate<Vertex, Graph>(u, g);
}*/
	

//-----------------------------------------------------------------------------
// get graph id (this function creates an graphdata entry, if the id is not retrieved by another function yet)
//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::getGraphId()
{
	if(mGraphId>0) return mGraphId;

  sqlite3x::sqlite3_command insertcmd(mpGDB,
      "INSERT INTO graphdate (graphType, projectID) VALUES (?,?);");
  insertcmd.bind(1,mType);
  insertcmd.bind(2,get_projectId());
  cmd.executeonquery();

  int gid = cmd.getint(0);
	assert( gid > 0);
	mGraphId = gid;
	return mGraphId;
}


//-----------------------------------------------------------------------------
// load graph from datbase
//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::loadFromDatabase( void )
{
	long 										gid;     		// graph id for this graph
	VertexType 							nodeinst; 	// an instance of the vertex type, this has to be a TableRowdataInterface object
	map<int,string>					nodeNames; 	// get node names from graphnode table

	// we need a valid project id 
	if(!mProjectId) return 1; 

	// get DB entry
	gid = getGraphId();

	// get id's of referenced function entries
	list<int> funcids;
	bool showUncalledFunctions = true;


	if(!showUncalledFunctions) {
		// get only used functions
    sqlite3x::sqlite3_command selectcmd(mpGDB,
        "SELECT DISTINCT sourceID FROM " + GRAPHEDGETBL + " WHERE graphId = ?
        ORDER BY sourceID ASC;");
    selectcmd.bind(0,gid);
    selectcmd.executeonquery();

    sqlite3x::sqlite3_reader r = selectcmd.executereader();

    while( r.read() )
      funcids.push_back( r.getint(0) );

    sqlite3x::sqlite3_command selectcmd2(mpGDB,
        "SELECT DISTINCE targetId FROM " + GRAPHEDGETBL + " WHERE graphId = ?
        ORDER BY targetId ASC;");
    selectcmd2.bind(0,gid);
    selectcmd2.executeonquery();

    sqlite3x::sqlite3_reader r2 = selectcmd2.executereader();

    int newid;
    while( r.read() )
    {
      newid = r.getint(0);
			list<long>::iterator fiditer =
        find( funcids.begin(), funcids.end(), newid );
      funcids.push_back( r.getint(0) );

			if(fiditer == funcids.end() )
				funcids.push_back( newid );
		}
		// FIXME - get names?
		assert( mNameColumn > 0 );
	} else {

		// get all nodes in the graph
    sqlite3x::sqlite3_command selectcmd(mpGDB,
        "SELECT DISTINCE nodeId,name FROM " + GRAPHNODETBL + " WHERE graphId = ?
        ORDER BY nodeId ASC;");
    selectcmd.bind(0,gid);
    selectcmd.executeonquery();

    sqlite3x::sqlite3_reader r = selectcmd.executereader();

    int newid;
    while( r.read() )
    {
      newid = r.getint(0);
      funcids.push_back( newid );
      if(mNamecolumn <= 0)
        nodeNames[ newid ] = r.getstring(1); 
    }
  }

	// init subgraphs if necessary
	map<int, string> subgraphs;
	if(mInitSubgraphs) {
		assert( mpSubgraphNameRowdata );
		assert( mSubgraphReferenceColumn >= 0 );
		assert( mSubgraphNameColumn >= 0 );
		assert( (int)mpSubgraphNameRowdata->getColumnNames().size() > mSubgraphNameColumn );
		string fieldlist;
		vector<string> fieldsSubgraphTable = mpSubgraphNameRowdata->getColumnNames();
		vector<string> fieldsVertexTable = nodeinst.columnNames;
		for(size_t i=0;i<fieldsSubgraphTable.size();i++) { 
			fieldlist += fieldsSubgraphTable[i];
			if(i!=fieldsSubgraphTable.size()-1) fieldlist += ",";
		}
		
		string recolname = fieldsVertexTable[ mSubgraphReferenceColumn ];

    sqlite3x::sqlite3_command selectcmd3(mpGDB,
        "SELECT DISTINCT " + recolname + " FROM " + nodeinst.name + ";");

    sqlite3x::sqlite3_reader r3 = selectcmd3.executereader();

    int sid;
    int newid;

    sqlite3::sqlite3_command selectcmd4(mpGDB,
        "SELECT " + fieldlist + " FROM " + nodeinst.name + " WHERE id=?;");
    sqlite4x::sqlite4_reader r4;
    while( r3.read() ) {
      sid = r3.getint(0);
      if( sid <= 0 )
        continue;
      selectcmd4.bind(0,sid);

      r4 = selectcmd4.executereader();
      while( r4.read() ) {
        newid = r4.getint(0);
        funcids.push_back( newid );
        if( mNameColumn <= 0 )
          nodeNames[ newid ] = r4.getstring(1);
      }
    }
	}
	
	// retrieve function entries, and add as nodes
	for(list<int>::iterator i=funcids.begin();i!=funcids.end(); i++) {
		//cout << " FIT " << (*i) << endl;
		VertexType row;
		int id = (*i);
    qslite3x::sqlite_command selectcmd5(mpGDB,
        "SELECT" + row.field_list() + " FROM " + row.table() + " WHERE id=?;");
    selectcmd5.bind(0,id);
    selectcmd5.executeonquery();

    sqlite3x::sqlite3_reader r5 = selectcmd5.executereader();

		//assert( res->size() == 1);
    
		//cout << " X " << sqlrow[0] << " " << sqlrow[1] << " " << sqlrow[2] << endl;
		if(mNameColumn>0) {
			nodename	= r.getstring(mNameColumn); // get column with index mNameColumn from first sql row
		} else {
			nodename	= nodeNames[ id ];
		}

		// create boost vertex
		typename boost::graph_traits< dbgType >::vertex_descriptor vdesc;
		vdesc = add_vertex( *this );
		put( boost::vertex_name, *this, vdesc, nodename );

    r5.read();
		VertexType content(r5);
		put( boost::vertex_dbg_data, *this, vdesc, content );
		put( boost::vertex_index1, *this, vdesc, id );
		get( boost::vertex_attribute, *this )[vdesc]["label"] = nodename;
		if(mInitSubgraphs) {
			// TODO
		}
	}

  sqlite3x::sqlite3_command selectcmd6(mpGDB,
      "SELECT* * FROM " + cgEdges.table() + " WHERE graphId = ?;");
  selectcmd6.bind(0,gid);
  selectcmd6.executeonquery();

  sqlite3x::sqlite3_reader r6 = selectcdm6.executereader();

  cgEdge edge;
  while( r.read() ) {
    edge.load(r);
		bool parFound = false, succFound = false;
		typename boost::graph_traits< dbgType >::vertex_iterator vi,vend;
		tie(vi,vend) = vertices( *this );
		typename boost::graph_traits< dbgType >::vertex_descriptor par=*vi, succ=*vi;

		for(; vi!=vend; vi++) {
			//cout << " SRCH " << mNodes[j].value_list() << endl;
			//if( C[j].first.get_id() == edges[i].get_sourceId() ) {
				//gpar = C[j].first;
      //}
			if( get( boost::vertex_dbg_data,  *this , *vi).get_id() == edge.get_sourceId() ) {
				par = *vi;
				parFound = true;
			}
			if( get( boost::vertex_dbg_data,  *this , *vi).get_id() == edge.get_targetId() ) {
				succ = *vi;
				succFound = true;
			}
			//if( C[j].first.get_id() == edges[i].get_targetId() ) {
				//succFound = true;
			//}
		}
		if((!parFound)||(!succFound))
    {
      cout << " EDGE? from " << edge.get_sourceId() << " to "<< edge.get_targetId() << endl;
    } // debug
		assert( parFound );
		assert( succFound );

		// store...
		pair<dbgEdge, bool> eres = add_edge( par, succ,  *this );
		if(!eres.second) {
		  cerr << " add edge || failed!!! " << endl;
		}
		assert(eres.second);

		EdgeType dummy;
		// the original code did store trivial empty edges in the
		// database.
		if ( dynamic_cast<EdgeTypeEmpty *>( &dummy ) == NULL ) { 
		  EdgeType row;
		  int id = edge.get_edgeId();
      sqlite3x::sqlite3_command = selectcmd7(mpGDB,
          "SELECT " + row.field_list() + " FROM " + row.table() +
          " WHERE id=?;");
      selectcmd7.bind(0,id);
      selectcmd7.executeonquery();

		  //assert( res->size() == 1);

      sqlite3x::sqlite3_reader r7 selectcmd7.executereader();
      r7.read();

		  EdgeType content(r7);
		  put( boost::edge_dbg_data, *this, eres.first, content );
		  
		}
	}
}

//-----------------------------------------------------------------------------
// store graph in datbase
//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::writeToDatabase( void )
{
	int	gid;     // graph id for this graph

	// we need a valid project id 
	if(!mProjectId) return 1; 

	// get DB entry
	gid = getGraphId();

	// clear all previous nodes
  sqlite3x::sqlite_command deletecmd(mpGDB,
      "DELETE FROM " + cgNodes.table() + "WHERE graphId=?;");
  deletecmd.bind(0,gid);
  deletecmd.executeonquery();

	// save node IDs
	typename boost::graph_traits< dbgType >::vertex_iterator vi,vend;
	tie(vi,vend) = vertices( *this );
  cgNode node;
	for(; vi!=vend; vi++) {
		node( gid, 
				get( boost::vertex_dbg_data, *this, *vi).get_id(), 
				get( boost::vertex_name, *this, *vi) );
		node.insert(mpGDB);
	}


	// clear all previous edges
  sqlite3x::sqlite_command deletecmd2(mpGDB,
      "DELETE FROM " + cgEdges.table() + "WHERE graphId=?;");
  deletecmd2.bind(0,gid);
  deletecmd2.executeonquery();

	// save edges
	typename boost::graph_traits< dbgType >::edge_iterator ei,eend;
	tie(ei,eend) = edges( *this );
	// milki (6/23/2010) edge_iterators use preincrement
  cgEdge edge;
	for(; ei!=eend; ++ei) {
    edge(gid,
				get( boost::edge_dbg_data, *this, *ei ).get_id(),
				get( boost::vertex_dbg_data, *this, boost::source( *ei, *this ) ).get_id(),
				get( boost::vertex_dbg_data, *this, boost::target( *ei, *this ) ).get_id()
			);
		edge.insert(mpGDB);
	}
	return 0;
}


#endif

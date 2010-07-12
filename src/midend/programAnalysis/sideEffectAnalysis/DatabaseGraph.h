/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * graph database storage class definition
 *
 *****************************************************************************/


#ifndef DATABASEGRAPH_H
#define DATABASEGRAPH_H

// for graph output as .dot file
#include "rose_config.h"
#include <algorithm>

// database access, dont redefine tables 
#include "GlobalDatabaseConnectionMYSQL.h"
#ifndef TABLES_DEFINED
#include "TableDefinitions.h"
USE_TABLE_GRAPHDATA();
USE_TABLE_GRAPHNODE();
USE_TABLE_GRAPHEDGE();
USE_TABLE_METATABLE();
#endif

// use the boost graph files
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"
#include "boost/graph/exception.hpp"
#include "boost/graph/graphviz.hpp"

//! the different graph types, used for graphdata.graphType field
#define GTYPE_CALLGRAPH 				1
#define GTYPE_CLASSHIERARCHY		2
#define GTYPE_TESTGRAPH					3 		// for database graph example
#define GTYPE_SIMPLECALLGRAPH		4 		// for simple callgraph example
#define GTYPE_USERDEFINED				100 	// user defined graphs should start with this id


//-----------------------------------------------------------------------------
//! empty edge class, for graphs that arent storing any edge data
#if 0
class EdgeTypeEmpty {
	public:
		EdgeTypeEmpty() { };

		//! a comparison operator is needed for e.g. find algorithms
		bool operator== (const EdgeTypeEmpty &e) const { return true; };
};
#endif

CREATE_TABLE_1( EdgeTypeEmpty,  int,projectId );
DEFINE_TABLE_1( EdgeTypeEmpty,  int,projectId );
typedef EdgeTypeEmptyRowdata EdgeTypeEmpty;

// install boost database graph property map types
namespace boost {
	enum vertex_dbg_data_t { vertex_dbg_data };
	BOOST_INSTALL_PROPERTY( vertex, dbg_data );

	enum edge_dbg_data_t { edge_dbg_data };
	BOOST_INSTALL_PROPERTY( edge, dbg_data );

	enum graph_dbg_data_t { graph_dbg_data };
	BOOST_INSTALL_PROPERTY( graph, dbg_data );
};

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace boost;


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
	//typedef boost::property<boost::vertex_dbg_data_t, VertexType, boost::GraphvizVertexProperty> 	vertexDbgData;
	//typedef boost::property<boost::edge_dbg_data_t, 	EdgeType, 	boost::GraphvizEdgeProperty> 		edgeDbgData;
	//typedef boost::property<boost::graph_dbg_data_t, 	int, boost::GraphvizGraphProperty> 	graphDbgData;


	//! constructor
	DatabaseGraph( long pid, long type, GlobalDatabaseConnection *gdb );
	
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
	GlobalDatabaseConnection *mpGDB;

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
DatabaseGraph<DBG_TEMPLATE_CLASSES>::DatabaseGraph( long pid, long type, GlobalDatabaseConnection *gdb )
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

	char typestr[8]; // convert type to string
	snprintf( typestr, 8, "%ld", mType );
	graphdataTableAccess	cgData( mpGDB );
	graphdataRowdata cgrow( UNKNOWNID, mProjectId, mType );
	int gid = cgData.retrieveCreateByColumn( &cgrow, "graphType", typestr , cgrow.get_projectId() );
	assert( gid > 0);
	mGraphId = gid;
	return mGraphId;
}



//-----------------------------------------------------------------------------
// load graph from datbase
//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::loadFromDatabase( void )
{
	graphdataTableAccess 		cgData( mpGDB );  // graph data table
	graphedgeTableAccess 		cgEdges( mpGDB ); // table for graph edges
	graphnodeTableAccess 		cgNodes( mpGDB ); // table for graph nodes
	long 										gid;     		// graph id for this graph
	VertexType 							nodeinst; 	// an instance of the vertex type, this has to be a TableRowdataInterface object
	map<int,string>					nodeNames; 	// get node names from graphnode table

	// we need a valid project id 
	if(!mProjectId) return 1; 
	// just to make sure, init tables...
	cgData.initialize();
	cgEdges.initialize();
	cgNodes.initialize();

	// get DB entry
	gid = getGraphId();

	// get id's of referenced function entries
	list<long> funcids;
	bool showUncalledFunctions = true;

	if(!showUncalledFunctions) {
		// get only used functions
		Query query1 = mpGDB->getQuery();
		query1 << "SELECT DISTINCT sourceId FROM " << cgEdges.getName() << " WHERE graphId='" << gid <<"' ORDER BY sourceId ASC; ";
		StoreQueryResult *res = mpGDB->select( query1.str().c_str() );
		assert( res );
		for(StoreQueryResult::iterator i=res->begin(); i!= res->end(); i++) {
			funcids.push_back( (*i)[0] );
		}

		Query query2 = mpGDB->getQuery();
		query2 << "SELECT DISTINCT targetId FROM " << cgEdges.getName() << " WHERE graphId='" << gid <<"' ORDER BY targetId ASC ; ";
		res = mpGDB->select( query2.str().c_str() );
		assert( res );
		for(StoreQueryResult::iterator i=res->begin(); i!= res->end(); i++) {
			long newid = (*i)[0];
			list<long>::iterator fiditer = find( funcids.begin(), funcids.end(), newid );
			if(fiditer == funcids.end() ) {
				funcids.push_back( newid );
			}
		}
		// FIXME - get names?
		assert( mNameColumn > 0 );
	} else {
		// get all nodes in the graph
		Query query1 = mpGDB->getQuery();
		query1 << "SELECT DISTINCT nodeId,name FROM " << cgNodes.getName() << " WHERE graphId='" << gid <<"' ORDER BY nodeId ASC; ";
		StoreQueryResult *res = mpGDB->select( query1.str().c_str() );
		assert( res );
		for(StoreQueryResult::iterator i=res->begin(); i!= res->end(); i++) {
			funcids.push_back( (*i)[0] );
			if(mNameColumn <= 0) {
				nodeNames[ (int)((*i)[0]) ] = (std::string)(*i)[1];
			}
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
		vector<string> fieldsVertexTable = nodeinst.getColumnNames();
		for(size_t i=0;i<fieldsSubgraphTable.size();i++) { 
			fieldlist += fieldsSubgraphTable[i];
			if(i!=fieldsSubgraphTable.size()-1) fieldlist += ",";
		}
		
		string recolname = fieldsVertexTable[ mSubgraphReferenceColumn ];
		Query query1 = mpGDB->getQuery();
		query1 << "SELECT DISTINCT "<< recolname << " FROM " << nodeinst.table() << " ; ";
		//cout << " D1 "<< query1.str() << endl; // debug
		StoreQueryResult *res = mpGDB->select( query1.str().c_str() );
		assert( res );
		for(StoreQueryResult::iterator i=res->begin(); i!= res->end(); i++) {
			// retrieve each entry, and add it to the subgraphs map
			int sid = (*i)[0];
			// id have to be >0
			if(sid <= 0) continue;
			
			Query query2 = mpGDB->getQuery();
			query2 << "SELECT "<< fieldlist << " FROM " << mpSubgraphNameRowdata->getTableName() << " WHERE id="<< sid << "; ";
			//cout << " D2 "<< query1.str() << endl; // debug
			StoreQueryResult *res = mpGDB->select( query2.str().c_str() );
			assert( res );
			Row sqlrow = (*res->begin());
			std::ostringstream subgraphName;
		 	subgraphName << sqlrow[ mSubgraphNameColumn ];
			subgraphs[ sid ] = subgraphName.str();
		}
	}

	
	// retrieve function entries, and add as nodes
	TableAccess<VertexType> table( mpGDB );
	for(list<long>::iterator i=funcids.begin();i!=funcids.end(); i++) {
		//cout << " FIT " << (*i) << endl;
		VertexType row;
		long id = (*i);
		std::ostringstream select;
		select << "SELECT "<< row.field_list() <<" FROM " << row.table() << " WHERE id='" << id <<"' ; ";
		StoreQueryResult *res = mpGDB->select( select.str().c_str() );
		assert( res->size() == 1);
		string nodename;
		Row sqlrow = (*res->begin());
		//cout << " X " << sqlrow[0] << " " << sqlrow[1] << " " << sqlrow[2] << endl;
		if(mNameColumn>0) {
			nodename	= (std::string)sqlrow[ mNameColumn ]; // get column with index mNameColumn from first sql row
		} else {
			nodename	= nodeNames[ id ];
		}

		// create boost vertex
		typename boost::graph_traits< dbgType >::vertex_descriptor vdesc;
		vdesc = add_vertex( *this );
		put( boost::vertex_name, *this, vdesc, nodename );
		VertexType content(sqlrow);
		put( boost::vertex_dbg_data, *this, vdesc, content );
		put( boost::vertex_index1, *this, vdesc, id );
		get( boost::vertex_attribute, *this )[vdesc]["label"] = nodename;
		if(mInitSubgraphs) {
			// TODO
		}
	}

	std::ostringstream cmd;
	cmd << " graphId='" << gid <<"' ";
	vector<graphedgeRowdata> edges = cgEdges.select( cmd.str() );
	for(unsigned int i=0;i<edges.size();i++) {
		// add boost edges
		{
		bool parFound = false, succFound = false;
		typename boost::graph_traits< dbgType >::vertex_iterator vi,vend;
		tie(vi,vend) = vertices( *this );
		typename boost::graph_traits< dbgType >::vertex_descriptor par=*vi, succ=*vi;

		for(; vi!=vend; vi++) {
			//cout << " SRCH " << mNodes[j].value_list() << endl;
			//if( C[j].first.get_id() == edges[i].get_sourceId() ) {
				//gpar = C[j].first;
				//gsuccessor = C[j].first;
			if( get( boost::vertex_dbg_data,  *this , *vi).get_id() == edges[i].get_sourceId() ) {
				par = *vi;
				parFound = true;
			}
			if( get( boost::vertex_dbg_data,  *this , *vi).get_id() == edges[i].get_targetId() ) {
				succ = *vi;
				succFound = true;
			}
			//if( C[j].first.get_id() == edges[i].get_targetId() ) {
				//succFound = true;
			//}
		}
		if((!parFound)||(!succFound)) { cout << " EDGE? from " << edges[i].get_sourceId() << " to "<< edges[i].get_targetId() << endl; } // debug
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
		  long id = edges[i].get_edgeId();
		  std::ostringstream select;
		  select << "SELECT "<< row.field_list() <<" FROM " << row.table() << " WHERE id='" << id <<"' ; ";
		  StoreQueryResult *res = mpGDB->select( select.str().c_str() );
		  assert( res->size() == 1);
		  Row sqlrow = (*res->begin());
		  
		  EdgeType content(sqlrow);
		  put( boost::edge_dbg_data, *this, eres.first, content );
		  
		}
		}
	}

	{
		// TOPO TEST
		/*typedef graph_traits<boostGraph>::vertex_descriptor boostVert;

		cerr << " BOOST TTT topo start " << endl;
		//typedef adjacency_list<boost::vecS, boost::vecS, directedS, property<vertex_color_t, default_color_type> > Graph;
		//typedef boostGraph Graph;

		typedef boost::graph_traits<boostGraph>::vertex_descriptor Vertex;
		typedef std::pair<std::size_t,std::size_t> Pair;
		Pair edges[6] = { Pair(0,1), Pair(2,4), Pair(2,5), Pair(0,3), Pair(1,4), Pair(4,3) };

		// VC++ can't handle the iterator constructor
		boostGraph G2(6);
		for (std::size_t j = 0; j < 6; ++j)
			add_edge(edges[j].first, edges[j].second, G2);
		*/
		/*{graph_traits<boostGraph>::vertex_iterator vi,vend;
		tie(vi,vend) = vertices(G2);
		for(; vi!=vend; vi++) {
			cerr << " BOOST2 v i"<< get(vertex_index,G2,*vi)<< " i1" << get(vertex_index1, G2, *vi)<<","<< get(vertex_name, G2, *vi) << endl;
			graph_traits<boostGraph>::out_edge_iterator ei,eend;
			tie(ei,eend) = out_edges(*vi, G2);
			for(; ei!=eend; ei++) {
				graph_traits<boostGraph>::vertex_descriptor tar = boost::target(*ei,G2);
				cerr << " BOOST2 v >>> " << get(vertex_index1, G2, tar )<<","<< get(vertex_name, G2, tar ) << endl;
			}
			graph_traits<boostGraph>::in_edge_iterator ii,iend;
			tie(ii,iend) = in_edges(*vi, G2);
			for(; ii!=iend; ii++) {
				cerr << " BOOST2 v <<< " << get(vertex_index1, G2, boost::target(*ii,G2) )<<","<< get(vertex_name, G2, boost::target(*ii,G2) ) << endl;
			}
		}}*/

		//boost::property_map<Graph, vertex_index_t>::type id = get(vertex_index, G2);
		/*typedef std::deque< boostVert > container;
		std::deque< boostVert > torder2;
		try {
			topological_sort(G2, std::back_inserter(torder2));
		} catch(boost::not_a_dag) {
			cerr << " BOOST NOT A DAG!!!!! " << endl;
		}
		cerr << " BOOST TTT topo end " << endl;
		for( std::deque<boostVert>::iterator i=torder2.begin(); i!=torder2.end(); i++) {
				//cerr << " v" << get(vertex_index1, G2, *i )<<","<< get(vertex_name, G2, *i );
				cerr << " v" << get(vertex_index, G2, *i );
		}
		cerr << " BOOST TTT topo end " << endl;
		cerr << endl;*/



		// output boost graph
		/*
		cerr << " BOOST LOADED " << endl;
		graph_traits< dbgType >::vertex_iterator vi,vend;
		
		tie(vi,vend) = vertices( *this );
		for(; vi!=vend; vi++) {
			cerr << " BOOST v i"<< get(vertex_index, *this ,*vi)<< " i1" << get(vertex_dbg_data,  *this , *vi).get_id() <<","<< get(vertex_name,  *this , *vi) << endl;
			//get( vertex_attribute, *this )[*vi]["color"] = string("red");

			//cerr << " BOOST v " << get(vertex_index1,  *this , *vi)<<"," << endl;
			// iterate over edges
			graph_traits<dbgType>::out_edge_iterator ei,eend;
			tie(ei,eend) = out_edges(*vi, *this);
			for(; ei!=eend; ei++) {
				graph_traits<dbgType>::vertex_descriptor tar = boost::target(*ei,*this);
				cerr << " BOOST v >>> " << get(vertex_index1, *this, tar )<<","<< get(vertex_name, *this, tar ) << endl;
			}
			graph_traits<dbgType>::in_edge_iterator ii,iend;
			tie(ii,iend) = in_edges(*vi, *this);
			for(; ii!=iend; ii++) {
				cerr << " BOOST v <<< " << get(vertex_index1, *this, boost::target(*ii,*this) )<<","<< get(vertex_name, *this, boost::target(*ii,*this) ) << endl;
			}
		
		}
	 	//get_property( *this , graph_vertex_attribute)["shape"] = "box";
	 	//get_property( *this , graph_graph_attribute)["bgcolor"] = "lightgrey";
	 	set_property( *this , graph_name, string("test_graph_name") );
		*/
	}

	// BOOST TEST
	return 0;
}



//-----------------------------------------------------------------------------
// store graph in datbase
//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::writeToDatabase( void )
{
	graphdataTableAccess 		cgData( mpGDB );  // graph data table
	graphedgeTableAccess 		cgEdges( mpGDB ); // table for graph edges
	graphnodeTableAccess 		cgNodes( mpGDB ); // table for graph nodes
	long 										gid;     // graph id for this graph

	// we need a valid project id 
	if(!mProjectId) return 1; 

	// get DB entry
	gid = getGraphId();

	// clear all previous nodes
	std::ostringstream delnodes;
	delnodes << "DELETE FROM " << cgNodes.getName() << " WHERE graphId='" << gid << "' ; " ;
	//cout << " DEL " <<deledges.str().c_str()<< endl;
	mpGDB->execute( delnodes.str().c_str() );
	
	// save node IDs
	typename boost::graph_traits< dbgType >::vertex_iterator vi,vend;
	tie(vi,vend) = vertices( *this );
	for(; vi!=vend; vi++) {
		graphnodeRowdata node( UNKNOWNID, gid, 
				get( boost::vertex_dbg_data, *this, *vi).get_id(), 
				get( boost::vertex_name, *this, *vi) );
		cgNodes.insert( &node ); 
	}


	// clear all previous edges
	std::ostringstream deledges;
	deledges << "DELETE FROM " << cgEdges.getName() << " WHERE graphId='" << gid << "' ; " ;
	//cout << " DEL " <<deledges.str().c_str()<< endl;
	mpGDB->execute( deledges.str().c_str() );
	
	// save edges
	typename boost::graph_traits< dbgType >::edge_iterator ei,eend;
	tie(ei,eend) = edges( *this );
	// milki (6/23/2010) edge_iterators use preincrement
	for(; ei!=eend; ++ei) {
		graphedgeRowdata edge( UNKNOWNID, gid, 
				get( boost::edge_dbg_data, *this, *ei ).get_id(),
				get( boost::vertex_dbg_data, *this, boost::source( *ei, *this ) ).get_id(),
				get( boost::vertex_dbg_data, *this, boost::target( *ei, *this ) ).get_id()
			);
		cgEdges.insert( &edge ); 
	}
	return 0;
}


#endif



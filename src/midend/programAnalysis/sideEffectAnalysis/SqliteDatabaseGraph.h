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
#define GTYPE_CALLGRAPH                                 1
#define GTYPE_CLASSHIERARCHY            2
#define GTYPE_TESTGRAPH                                 3               // for database graph example
#define GTYPE_SIMPLECALLGRAPH           4               // for simple callgraph example
#define GTYPE_USERDEFINED                               100     // user defined graphs should start with this id


//--------------------------------------------------------------------
// Schema Setup
//--------------------------------------------------------------------
#define PROJECTSTBL   "projects"
#define GRAPHDATATBL  "graphData"
#define GRAPHNODETBL  "graphNode"
#define GRAPHEDGETBL  "graphEdge"

// VertexType
class dbTable
{
  public:
    dbTable(const char* table) { name = table; }

    std::string table() { return name; }
    std::string field_list()
    {
      std::string list;
      for( std::vector<std::string>::iterator it = columnNames.begin();
          it != columnNames.end() ; it++ )
        list += (*it) + ",";
      return list.substr(0, list.size() - 2);
    }

    std::vector<std::string> field_types() { return columnTypes; }

    std::vector<std::string> getColumnNames() { return columnNames; }

    std::string name;
    std::vector<std::string> columnNames;
    std::vector<std::string> columnTypes;
};

// EdgetType
class dbRow : public dbTable
{
  public:
    dbRow(const char* table) : dbTable(table) {}

    void set_id(long id) { rowid = id; }
    long get_id() const { return rowid; }

    virtual void load(sqlite3x::sqlite3_reader& r)=0;
    virtual void insert(sqlite3x::sqlite3_connection* db)=0;

    long rowid;
};

class projectsRow: public dbRow
{
  public:
    projectsRow(std::string project): dbRow(PROJECTSTBL)
  {
      columnNames.push_back("id");
      columnNames.push_back("name");
      columnNames.push_back("CallgraphRootID");

      columnTypes.push_back("integer");
      columnTypes.push_back("string");
      columnTypes.push_back("integer");

      name = project;
  }

    // Load from database
    void load(sqlite3x::sqlite3_reader& r)
    {
      rowid = r.getint(0);
      name = r.getstring(1);
      rootid = r.getint(2);
    }

    // Insert into database
    // NOTE: does not check for duplicates
    void insert(sqlite3x::sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + std::string(PROJECTSTBL) + " WHERE name=? AND CallgraphRootID=?;");
      selectcmd.bind(1,name);
      selectcmd.bind(2,rootid);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3x::sqlite3_command insertcmd(*db,
        "INSERT INTO " + std::string(PROJECTSTBL) + " (name,CallgraphRootID) VALUES (?,?);");
      insertcmd.bind(1,name);
      insertcmd.bind(2,rootid);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    std::string name;
    int rootid;
};

class cgData : public dbRow
{
  public:
    cgData(int project, int type): dbRow(GRAPHDATATBL)
    {
      columnNames.push_back("id");
      columnNames.push_back("projectid");
      columnNames.push_back("graphType");

      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("integer");

      projectId = project;
      graphType = type;
    }
    cgData(sqlite3x::sqlite3_reader& r): dbRow(GRAPHDATATBL) { load(r); }

    // Load from database
    void load(sqlite3x::sqlite3_reader& r)
    {
      rowid = r.getint(0);
      projectId = r.getint(1);
      graphType = r.getint(2);
    }

    // Insert into database
    // NOTE: does not check for duplicates
    void insert(sqlite3x::sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + std::string(GRAPHDATATBL) + " WHERE projectid=? AND graphType=?;");
      selectcmd.bind(1,projectId);
      selectcmd.bind(2,graphType);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3x::sqlite3_command insertcmd(*db,
        "INSERT INTO " + std::string(GRAPHDATATBL) + " (projectid,graphType) VALUES (?,?);");
      insertcmd.bind(1,projectId);
      insertcmd.bind(2,graphType);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    void set_graphid(int id) { projectId = id; }
    int  get_graphid() { return projectId; }

    void set_projectid(int id) { projectId = id; }
    int  get_projectid() { return projectId; }

    void set_graphtype(int type) { graphType = type; }
    int  get_graphtype() { return graphType; }

    int projectId;
    int graphType;
};

class cgNode : public dbRow
{
  public:
    cgNode(int graphid, int node, std::string& name): dbRow(GRAPHNODETBL)
  {
    columnNames.push_back("id");
    columnNames.push_back("graphId");
    columnNames.push_back("nodeId");
    columnNames.push_back("name");

    columnTypes.push_back("integer");
    columnTypes.push_back("integer");
    columnTypes.push_back("integer");
    columnTypes.push_back("text");

    gid = graphid;
    nodeId = node;
    nodeName = name;
  }
    cgNode(sqlite3x::sqlite3_reader& r) : dbRow(GRAPHNODETBL) { load(r); }

    void load(sqlite3x::sqlite3_reader& r)
    {
      rowid = r.getint(0);
      gid = r.getint(1);
      nodeId = r.getint(2);
      nodeName = r.getstring(3);
    }

    void insert(sqlite3x::sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + std::string(GRAPHNODETBL) + " WHERE graphId=? AND nodeId=? AND name=?;");
      selectcmd.bind(1,gid);
      selectcmd.bind(2,nodeId);
      selectcmd.bind(3,nodeName);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3x::sqlite3_command insertcmd(*db,
        "INSERT INTO " + std::string(GRAPHNODETBL) + " (graphId,nodeId,name) VALUES (?,?,?);");
      insertcmd.bind(1,gid);
      insertcmd.bind(2,nodeId);
      insertcmd.bind(3,nodeName);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    void set_graphid(int id) { gid = id; }
    int  get_graphid() { return gid; }

    void set_nodeid(int id) { nodeId = id; }
    int  get_nodeid() { return nodeId; }

    void set_nodename(std::string& name) { nodeName = name; }
    std::string get_nodename() { return nodeName; }

    int gid;
    int nodeId;
    std::string nodeName;
};

class cgEdge : public dbRow
{
  public:
    cgEdge(): dbRow(GRAPHEDGETBL) {}

    cgEdge(int graphid, int edge, int source, int target): dbRow(GRAPHEDGETBL)
    {
      columnNames.push_back("id");
      columnNames.push_back("graphId");
      columnNames.push_back("edgeId");
      columnNames.push_back("sourceId");
      columnNames.push_back("targetId");

      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("integer");

      gid = graphid;
      edgeId = edge;
      sourceId = source;
      targetId = target;
    }
    cgEdge(sqlite3x::sqlite3_reader& r): dbRow(GRAPHEDGETBL) { load(r); }

    void load(sqlite3x::sqlite3_reader& r)
    {
      rowid = r.getint(0);
      gid = r.getint(1);
      edgeId = r.getint(2);
      sourceId = r.getint(3);
      targetId = r.getint(4);
    }

    void insert(sqlite3x::sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + std::string(GRAPHEDGETBL) + " WHERE graphId=? AND edgeId=? AND sourceId=? AND targetId=?;");
      selectcmd.bind(1,gid);
      selectcmd.bind(2,edgeId);
      selectcmd.bind(3,sourceId);
      selectcmd.bind(4,targetId);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3x::sqlite3_command insertcmd(*db,
        "INSERT INTO " + std::string(GRAPHEDGETBL) + " (graphId,edgeId,sourceId,targetId) VALUES (?,?,?,?);");
      insertcmd.bind(1,gid);
      insertcmd.bind(2,edgeId);
      insertcmd.bind(3,sourceId);
      insertcmd.bind(4,targetId);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    void set_graphid(int id) { gid = id; }
    int  get_graphid() { return gid; }

    void set_edgeId(int id) { edgeId = id; }
    int  get_edgeId() { return edgeId; }

    void set_sourceId(int source) { sourceId = source; }
    int get_sourceId() { return sourceId; }

    void set_targetId(int target) { targetId = target; }
    int get_targetId() { return targetId; }

    int gid;
    int edgeId;
    int sourceId;
    int targetId;
};

bool
define_schema(sqlite3x::sqlite3_connection& gDB)
{
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS " + std::string(PROJECTSTBL) + " (id INTEGER PRIMARY KEY, name TEXT, callgraphRootId INTEGER)");
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS " + std::string(GRAPHDATATBL) + " (id INTEGER PRIMARY KEY, projectId INTEGER, graphType INTEGER)");
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS " + std::string(GRAPHNODETBL) + " (id INTEGER PRIMARY KEY, graphId INTEGER, nodeId INTEGER, name TEXT)");
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS " + std::string(GRAPHEDGETBL) + " (id INTEGER PRIMARY KEY, graphId INTEGER, edgeId INTEGER, sourceId INTEGER, targetID INTEGER)");

  return true;
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
                boost::property<boost::edge_dbg_data_t,         EdgeType,
        BoostEdgeProperty>,
                boost::property<boost::graph_dbg_data_t,        int, BoostGraphProperty>
        > dbgType;
        typedef typename boost::graph_traits< dbgType >::vertex_descriptor dbgVertex;
        typedef typename boost::graph_traits< dbgType >::vertex_iterator        dbgVertexIterator;
        typedef typename boost::graph_traits< dbgType >::edge_descriptor        dbgEdge;
        typedef typename boost::graph_traits< dbgType >::edge_iterator                  dbgEdgeIterator;
        typedef typename std::pair<bool, dbgEdge>                                                                               dbgEdgeReturn;

        //! constructor
        DatabaseGraph( long pid, long type, sqlite3x::sqlite3_connection *gdb );
        
        //! destructor
        ~DatabaseGraph( );

        //! add a vertex to the graph, returns the boost vertex descriptor
        dbgVertex insertVertex(VertexType& e1, std::string name);
        //! add a vertex with subgraph information - subgraphs are currently not supported!!!
        dbgVertex insertVertex(VertexType& e, std::string name, int subgraphId, std::string subgraphName);
        //! insert an edge between e1 and e2
        dbgEdgeReturn insertEdge(VertexType& e1, VertexType& e2, EdgeType& value);
        //! insert an edge between e1 and e2, using the empty edge data class
        dbgEdgeReturn insertEdge(VertexType& e1, VertexType& e2);

        //! search for a specific edge in the graph, using operator= on the id, returns false if not found 
        bool searchEdge(EdgeType &edge, dbgEdge &edesc) const; 

        // helper functions

        //! set subgraph initialization information
        void setSubgraphInit(int refcol, dbRow *rowdata, int namecol);

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
        void writeToDOTFile(std::string filename);
        
        //! write the adjacency matrix to a file (only integer matrix entries, raw file format)
        void writeAdjacencyMatrixToFileRaw(std::string filename) { };
                
        //! write the adjacency matrix to a file (in MCL raw file format for mcxassemble)
        void writeAdjacencyMatrixToFileMcl(std::string filename) { };
                

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
        dbRow *mpSubgraphNameRowdata;

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
DatabaseGraph<DBG_TEMPLATE_CLASSES>::insertVertex(VertexType& e1, std::string name) 
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
DatabaseGraph<DBG_TEMPLATE_CLASSES>::insertVertex(VertexType& e1, std::string name, int subgraphId, std::string subgraphName) 
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
  if ( ( !searchEdge(value, ei) ) ) {
    std::pair<dbgEdge, bool> eres = add_edge( vi1, vi2,  *this );
          if(!eres.second) {
      std::cerr << " add edge || failed!!! " << std::endl;
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
// destructor
DBG_TEMPLATE_DEF DatabaseGraph<DBG_TEMPLATE_CLASSES>::~DatabaseGraph( )
{
        // FIXME delete all nodes?
}


//-----------------------------------------------------------------------------
// set subgraph initialization information
DBG_TEMPLATE_DEF void DatabaseGraph<DBG_TEMPLATE_CLASSES>::setSubgraphInit(int refcol, dbRow *rowdata, int namecol)
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
DBG_TEMPLATE_DEF void DatabaseGraph<DBG_TEMPLATE_CLASSES>::writeToDOTFile(std::string filename)
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

  cgData dbg(mProjectId,mType);
  dbg.insert(mpGDB);

  int gid = dbg.get_graphid();
        assert( gid > 0);
        mGraphId = gid;
        return mGraphId;
}


//-----------------------------------------------------------------------------
// load graph from datbase
//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::loadFromDatabase( void )
{
        int                                                                             gid;                    // graph id for this graph
        VertexType                                                      nodeinst;       // an instance of the vertex type, this has to be a dbRow object
  std::map<int,std::string>                                     nodeNames;      // get node names from graphnode table

        // we need a valid project id 
        if(!mProjectId) return 1; 

        // get DB entry
        gid = getGraphId();

        // get id's of referenced function entries
  std::list<int> funcids;
        bool showUncalledFunctions = true;


        if(!showUncalledFunctions) {
                // get only used functions
    sqlite3x::sqlite3_command selectcmd(*mpGDB,
        "SELECT DISTINCT sourceID FROM " + std::string(GRAPHEDGETBL) + " WHERE graphId = ? ORDER BY sourceID ASC;");
    selectcmd.bind(1,gid);

    sqlite3x::sqlite3_reader r = selectcmd.executereader();

    while( r.read() )
      funcids.push_back( r.getint(0) );

    sqlite3x::sqlite3_command selectcmd2(*mpGDB,
        "SELECT DISTINCT targetId FROM " + std::string(GRAPHEDGETBL) + " WHERE graphId = ? ORDER BY targetId ASC;");
    selectcmd2.bind(1,gid);

    sqlite3x::sqlite3_reader r2 = selectcmd2.executereader();

    int newid;
    while( r.read() )
    {
      newid = r.getint(0);
      std::list<int>::iterator fiditer =
        std::find( funcids.begin(), funcids.end(), newid );
      funcids.push_back( r.getint(0) );

                        if(fiditer == funcids.end() )
                                funcids.push_back( newid );
                }
                // FIXME - get names?
                assert( mNameColumn > 0 );
        } else {

                // get all nodes in the graph
    sqlite3x::sqlite3_command selectcmd(*mpGDB,
        "SELECT DISTINCT nodeId,name FROM " + std::string(GRAPHNODETBL) + " WHERE graphId = ? ORDER BY nodeId ASC;");
    selectcmd.bind(1,gid);

    sqlite3x::sqlite3_reader r = selectcmd.executereader();

    int newid;
    while( r.read() )
    {
      newid = r.getint(0);
      funcids.push_back( newid );
      if(mNameColumn <= 0)
        nodeNames[ newid ] = r.getstring(1); 
    }
  }

        // init subgraphs if necessary
  std::map<int, std::string> subgraphs;
        if(mInitSubgraphs) {
                assert( mpSubgraphNameRowdata );
                assert( mSubgraphReferenceColumn >= 0 );
                assert( mSubgraphNameColumn >= 0 );
                assert( (int)mpSubgraphNameRowdata->getColumnNames().size() > mSubgraphNameColumn );
                std::string fieldlist;
    std::vector<std::string> fieldsSubgraphTable = mpSubgraphNameRowdata->getColumnNames();
                std::vector<std::string> fieldsVertexTable = nodeinst.columnNames;
                for(size_t i=0;i<fieldsSubgraphTable.size();i++) { 
                        fieldlist += fieldsSubgraphTable[i];
                        if(i!=fieldsSubgraphTable.size()-1) fieldlist += ",";
                }
                
                std::string recolname = fieldsVertexTable[ mSubgraphReferenceColumn ];

    sqlite3x::sqlite3_command selectcmd3(*mpGDB,
        "SELECT DISTINCT " + recolname + " FROM " + nodeinst.name + ";");

    sqlite3x::sqlite3_reader r3 = selectcmd3.executereader();

    int sid;
    int newid;

    sqlite3x::sqlite3_command selectcmd4(*mpGDB,
        "SELECT " + fieldlist + " FROM " + nodeinst.name + " WHERE id=?;");
    sqlite3x::sqlite3_reader r4;
    while( r3.read() ) {
      sid = r3.getint(0);
      if( sid <= 0 )
        continue;
      selectcmd4.bind(1,sid);

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
        for(std::list<int>::iterator i=funcids.begin();i!=funcids.end(); i++) {
                //cout << " FIT " << (*i) << endl;
                VertexType row;
                int id = (*i);
    sqlite3x::sqlite3_command selectcmd5(*mpGDB,
        "SELECT " + row.field_list() + " FROM " + row.table() + " WHERE id=?;");
    selectcmd5.bind(1,id);

    sqlite3x::sqlite3_reader r5 = selectcmd5.executereader();

                //assert( res->size() == 1);
    
                //cout << " X " << sqlrow[0] << " " << sqlrow[1] << " " << sqlrow[2] << endl;
    std::string nodename;
                if(mNameColumn>0) {
                        nodename        = r5.getstring(mNameColumn); // get column with index mNameColumn from first sql row
                } else {
                        nodename        = nodeNames[ id ];
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

  sqlite3x::sqlite3_command selectcmd6(*mpGDB,
      "SELECT * FROM " + std::string(GRAPHEDGETBL) + " WHERE graphId = ?;");
  selectcmd6.bind(1,gid);

  sqlite3x::sqlite3_reader r6 = selectcmd6.executereader();

  cgEdge edge;
  while( r6.read() ) {
    edge.load(r6);
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
      std::cout << " EDGE? from " << edge.get_sourceId() << " to "<< edge.get_targetId() << std::endl;
    } // debug
                assert( parFound );
                assert( succFound );

                // store...
    std::pair<dbgEdge, bool> eres = add_edge( par, succ,  *this );
                if(!eres.second) {
      std::cerr << " add edge || failed!!! " << std::endl;
                }
                assert(eres.second);

                EdgeType row;
                int id = edge.get_edgeId();
    sqlite3x::sqlite3_command selectcmd7(*mpGDB,
        "SELECT " + row.field_list() + " FROM " + row.table() +
        " WHERE id=?;");
    selectcmd7.bind(1,id);

                //assert( res->size() == 1);

    sqlite3x::sqlite3_reader r7 = selectcmd7.executereader();
    r7.read();

                EdgeType content(r7);
                put( boost::edge_dbg_data, *this, eres.first, content );
        }
  return 0;
}

//-----------------------------------------------------------------------------
// store graph in datbase
//template<class VertexType, class EdgeType>
DBG_TEMPLATE_DEF int DatabaseGraph<DBG_TEMPLATE_CLASSES>::writeToDatabase( void )
{
        int     gid;     // graph id for this graph

        // we need a valid project id 
        if(!mProjectId) return 1; 

        // get DB entry
        gid = getGraphId();

        // clear all previous nodes
  sqlite3x::sqlite3_command deletecmd(*mpGDB,
      "DELETE FROM " + std::string(GRAPHNODETBL) + " WHERE graphId = ?;");
  deletecmd.bind(1,gid);
  deletecmd.executenonquery();

        // save node IDs
        typename boost::graph_traits< dbgType >::vertex_iterator vi,vend;
        tie(vi,vend) = vertices( *this );
        for(; vi!=vend; vi++) {
                cgNode node( gid, 
                                get( boost::vertex_dbg_data, *this, *vi).get_id(), 
                                get( boost::vertex_name, *this, *vi) );
                node.insert(mpGDB);
        }


        // clear all previous edges
  sqlite3x::sqlite3_command deletecmd2(*mpGDB,
      "DELETE FROM " + std::string(GRAPHEDGETBL) + " WHERE graphId = ?;");
  deletecmd2.bind(1,gid);
  deletecmd2.executenonquery();

        // save edges
        typename boost::graph_traits< dbgType >::edge_iterator ei,eend;
        tie(ei,eend) = edges( *this );
        // milki (6/23/2010) edge_iterators use preincrement
        for(; ei!=eend; ++ei) {
    cgEdge edge(gid,
                                get( boost::edge_dbg_data, *this, *ei ).get_id(),
                                get( boost::vertex_dbg_data, *this, boost::source( *ei, *this ) ).get_id(),
                                get( boost::vertex_dbg_data, *this, boost::target( *ei, *this ) ).get_id()
                        );
                edge.insert(mpGDB);
        }
        return 0;
}


#endif

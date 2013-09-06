#include <cerrno>
#include "sage3basic.h"
#include "CloneDetectionLib.h"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/pending/disjoint_sets.hpp>

std::string argv0;

static SqlDatabase::TransactionPtr transaction;



static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE \n"
              <<"This command computes the API Call similarity.\n"
              <<"\n"
              <<"  These switches control how api call traces are compared:\n"
              <<"    --call-depth=-1|0|..|MAXINT\n"
              <<"            Controls which API Calls of the trace is considered part of the similarity computation.\n"
              <<"            Options are: -1 is all calls, 0 is only direct calls, or a custom depth from 1\n"
              <<"            to some arbitrary depth.\n"
              <<"    --ignore-inline-candidates\n"
              <<"            Ignore functions present in only one of the traces that has a semantic equivalent in both\n"
              <<"            functions compilation unit.\n"
              <<"    --ignore-no-compares\n"
              <<"            Ignore functions that in the semantic clone detection never succeeded for any test.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

//map from function id to SemanticCalls

void 
create_func_replacement()
{
  /*
  
  all_pairs = "select func1_id, func2_id from semantic_funcsim";

  std::map<int,int> replace_func_with;

  for each f in all_pairs
    it = replace_func_with.find(f.func1_id) 
    if it != replace_func_with.end() && it->second.func2_id > f.func2_id
       it->second = f.func2_id;
    else
        replace_func_with.insert(<>(f.1, f.2))

  lookup(){
    with = last match in chain in replace_func_with
    replace replace_func_with entry with with
    return with
  }

  */


}

struct SemanticCalls{

  int func_id;
  int igroup_id;
  int caller_id;
  int callee_id;
  int pos;
  int ncalls;
};

typedef std::vector<int> CallVec;


CallVec* 
load_api_calls_for(int func_id, int igroup_id, bool ignore_no_compares, int call_depth, bool expand_ncalls)
{
  SqlDatabase::StatementPtr stmt = transaction->statement(
     "select fio.callee_id, fio.ncalls from semantic_fio_calls as fio"
     +std::string( ignore_no_compares ? " join tmp_tested_funcs as f1 on f1.func_id = fio.callee_id" : "") // filter out functions with no compares
     +" where fio.func_id = ? AND fio.igroup_id = ?" // filter on current parameters
     +std::string(call_depth > 0 ? " AND fio.caller_id = ?" : "") // filter out function not called directly
     +" ORDER BY fio.pos"
     );
 
  stmt->bind(0, func_id);
  stmt->bind(1, igroup_id);
  stmt->bind(2, func_id);

  CallVec* call_vec = new CallVec; 
  for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
    int callee_id = row.get<int>(0);
    int ncalls    = row.get<int>(1);

    if(expand_ncalls){
      for(int i = 0 ; i < ncalls ; i++)
        call_vec->push_back(callee_id);
    }else
      call_vec->push_back(callee_id);
  }

  return call_vec;
}

typedef std::vector< std::pair<int,int> > SemanticPairVec;

using namespace boost;

void
find_semantic_pairs_between(int func1_id, int func2_id, int igroup_id, double similarity)
{
  std::string _query("select sem.func1_id, sem.func2_id from semantic_funcsim as sem"
      "join tmp_called_functions as tcf_2 on sem.func1_id = tcf_1.callee_id AND ( tcf.func_id IN (?,?)) AND (igroup_id = ?)"
      "join tmp_called_functions as tcf_1 on sem.func2_id = tcf_2.callee_id AND ( tcf.func_id IN (?,?)) AND (igroup_id = ?)"
      "where similarity >= ? ORDER BY sem.func1_id, sem.func2_id");



  //Count how many vertices we have for boost graph
  SqlDatabase::StatementPtr count_stmt = transaction->statement( _query );
  count_stmt->bind(0, func1_id);
  count_stmt->bind(1, func2_id);
  count_stmt->bind(3, igroup_id);
  count_stmt->bind(4, func1_id);
  count_stmt->bind(5, func2_id);
  count_stmt->bind(6, igroup_id);
  count_stmt->bind(7, similarity);

  int VERTEX_COUNT = count_stmt->execute_int();

  //Get all vetexes and find the union 
  SqlDatabase::StatementPtr stmt = transaction->statement( _query );

  stmt->bind(0, func1_id);
  stmt->bind(1, func2_id);
  stmt->bind(3, igroup_id);
  stmt->bind(4, func1_id);
  stmt->bind(5, func2_id);
  stmt->bind(6, igroup_id);
  stmt->bind(7, similarity);

  SemanticPairVec* cvec = new SemanticPairVec;


  typedef adjacency_list <vecS, vecS, undirectedS> Graph;
  typedef graph_traits<Graph>::vertex_descriptor Vertex;
  typedef graph_traits<Graph>::vertices_size_type VertexIndex;

  Graph graph(VERTEX_COUNT);

  std::vector<VertexIndex> rank(num_vertices(graph));
  std::vector<Vertex> parent(num_vertices(graph));

  typedef VertexIndex* Rank;
  typedef Vertex* Parent;

  disjoint_sets<Rank, Parent> ds(&rank[0], &parent[0]);

  initialize_incremental_components(graph, ds);
  incremental_components(graph, ds);

  graph_traits<Graph>::edge_descriptor edge;
  bool flag;

  for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
    int func1 = row.get<int>(0);
    int func2 = row.get<int>(1);
    boost::tie(edge, flag) = add_edge(func1, func2, graph);
    ds.union_set(func1,func2);

  }

  typedef component_index<VertexIndex> Components;

  // NOTE: Because we're using vecS for the graph type, we're
  // effectively using identity_property_map for a vertex index map.
  // If we were to use listS instead, the index map would need to be
  // explicitly passed to the component_index constructor.
  Components components(parent.begin(), parent.end());

  // Iterate through the component indices
  BOOST_FOREACH(VertexIndex current_index, components) {
    std::cout << "component " << current_index << " contains: ";

    int foo = components[current_index].first();
    // Iterate through the child vertex indices for [current_index]
    BOOST_FOREACH(VertexIndex child_index,
        components[current_index]) {
      std::cout << child_index << " ";
    }

    std::cout << std::endl;
  }


}



void 
load_api_calls(int func1_id, int func2_id, int igroup_id, CallVec& call_vec, double similarity, bool ignore_no_compares, int call_depth, bool expand_ncalls )
{
 call_vec.clear();

 CallVec* func1_vec = load_api_calls_for(func1_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);
 CallVec* func2_vec = load_api_calls_for(func2_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);


 find_semantic_pairs_between(func1_id, func2_id, igroup_id, similarity);






 //Find all connected components

/*
  //
 
  vec = vector of trace
  for clone in all_clones
    vec = create vector sorted by pos where callee id is replaced with lookup

  sort vec by pos

  vec_new = all callee_id from vec
  
    

  //Find the functions that are similar between the two 

  "create temporary table tmp_tested_funcs as select func1_id, func2_id from semantic_funcsim"
  " where (func1_id = ? OR func1_id = ?) AND (func2_id = ? OR func2_id = ?) ";


 
 transaction->execute("create temporary table tmp_tested_funcs as select distinct func_id from semantic_fio");
 
 //functions from A that is semantically similar to functions in B


SqlDatabase::StatementPtr stmt = tx->statement("select distinct f1.func_id as func1_id, f2.func_id as func2_id"
     " from tmp_tested_funcs as f1"
     " join tmp_tested_funcs as f2 on f1.func_id < f2.func_id");

 transaction->execute("create temporary table tmp_tested_funcs as select distinct func_id from semantic_fio");

  for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
    SemanticCalls sem_calls;
    sem_calls.func_id   = func_id;
    sem_calls.igroup_id = igroup_id;
    sem_calls.caller_id = row.get<int>(0);
    sem_calls.callee_id = row.get<int>(1);
    sem_calls.pos       = row.get<int>(2);
    sem_calls.ncalls    = row.get<int>(3);

    call_vec.push_back(sem_calls);
  }
*/

};

  
int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    bool ignore_inline_candidates = false;
    bool ignore_no_compares = false;
    int  call_depth = 0;

    

    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")){
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--ignore-inline-candidates")) {
            ignore_inline_candidates = true;
        } else if (!strcmp(argv[argno], "--ignore-no-compares")) {
          ignore_no_compares = false;
        } else if (!strncmp(argv[argno], "--call-depth=",13)) {
          call_depth = strtol(argv[argno]+13, NULL, 0);
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno+1!=argc)
        usage(1);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    transaction = conn->transaction();

    //table of tested functions. Criteria is that it needs to pass at least one test.
    transaction->execute("create temporary table tmp_tested_funcs as select distinct func_id from semantic_fio");
    
    //table of called fuctions. 
    transaction->execute("create temporary table tmp_called_functions as select distinct igroup_id, func_id, callee_id from semantic_fio_calls");
 
    return 0;
} 

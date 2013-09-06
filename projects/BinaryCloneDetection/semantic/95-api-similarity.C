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

using namespace boost;

void
normalize_call_trace(int func1_id, int func2_id, int igroup_id, double similarity, CallVec* func1_vec, CallVec* func2_vec)
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

  Components components(parent.begin(), parent.end());

  // Iterate through the component indices
  BOOST_FOREACH(VertexIndex current_index, components) {
    std::cout << "component " << current_index << " contains: ";

    std::vector<int> component_funcs;

    // Iterate through the child vertex indices for [current_index]
    BOOST_FOREACH(VertexIndex child_index,
        components[current_index]) {
      component_funcs.push_back(child_index);
      std::cout << child_index << " ";
    }


    if (component_funcs.size() > 0){
      for(CallVec::iterator it = func1_vec->begin(); it != func1_vec->end(); ++it )
        for(std::vector<int>::iterator comp_it; comp_it != component_funcs.end(); ++comp_it)
          if(*it == *comp_it) 
            *comp_it = component_funcs[0];

      for(CallVec::iterator it = func2_vec->begin(); it != func2_vec->end(); ++it )
        for(std::vector<int>::iterator comp_it; comp_it != component_funcs.end(); ++comp_it)
          if(*it == *comp_it) 
            *comp_it = component_funcs[0];


    }


    std::cout << std::endl;
  }


}



size_t
similarity(int func1_id, int func2_id, int igroup_id, CallVec& call_vec, double similarity, bool ignore_no_compares, int call_depth, bool expand_ncalls )
{
 call_vec.clear();

 CallVec* func1_vec = load_api_calls_for(func1_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);
 CallVec* func2_vec = load_api_calls_for(func2_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);

 //Detect and normalize similar function calls
 normalize_call_trace(func1_id, func2_id, igroup_id, similarity, func1_vec, func2_vec);

 size_t dl = Combinatorics::damerau_levenshtein_distance(*func1_vec, *func2_vec);
 
 delete func1_vec;
 delete func2_vec;

 return dl;
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

#include <cerrno>
#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "rose_getline.h"
#include "Combinatorics.h"

#include <cerrno>


#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/pending/disjoint_sets.hpp>

#include <boost/graph/breadth_first_search.hpp> 
#include <boost/graph/visitors.hpp> 
#include <vector> 

#include <algorithm>
#include <set>
#include <iterator>

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
              <<"  Other switches and arguments:\n"
              <<"    --file=NAME\n"
              <<"            Read pairs from this file instead of standard input.\n"
              <<"    --verbose\n"
              <<"            Show lots of diagnostics.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

typedef WorkList< std::pair<int/*func1_id*/, int/*func2_id*/> > FunctionPairs;

static FunctionPairs
load_worklist(const std::string &input_name, FILE *f)
{
    FunctionPairs worklist;
    char *line = NULL;
    size_t line_sz = 0, line_num = 0;
    while (rose_getline(&line, &line_sz, f)>0) {
        ++line_num;
        if (char *c = strchr(line, '#'))
            *c = '\0';
        char *s = line + strspn(line, " \t\r\n"), *rest;
        if (!*s)
            continue; // blank line

        errno = 0;
        int func1_id = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": " <<input_name <<":" <<line_num <<": syntax error: func1_id expected\n";
            exit(1);
        }
        s = rest;

        errno = 0;
        int func2_id = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": " <<input_name <<":" <<line_num <<": syntax error: func2_id expected\n";
            exit(1);
        }
        s = rest;

        while (isspace(*s)) ++s;
        if (*s) {
            std::cerr <<argv0 <<": " <<input_name <<":" <<line_num <<": syntax error: extra text after func2_id\n";
            exit(1);
        }

        worklist.push(std::make_pair(std::min(func1_id, func2_id), std::max(func1_id, func2_id)));
    }
    return worklist;
}




typedef std::vector<int> CallVec;


CallVec* 
load_api_calls_for(int func_id, int igroup_id, bool ignore_no_compares, int call_depth, bool expand_ncalls)
{
  SqlDatabase::StatementPtr stmt = transaction->statement(
     "select distinct fio.pos, fio.callee_id, fio.ncalls from semantic_fio_calls as fio"
     " join tmp_interesting_funcs as f1 on f1.func_id = fio.callee_id"  // filter out functions with no compares
     " where fio.func_id = ? AND fio.igroup_id = ?" // filter on current parameters
     +std::string(call_depth >= 0 ? " AND fio.caller_id = ?" : "") // filter out function not called directly

     +" ORDER BY fio.pos"
     );
 
  stmt->bind(0, func_id);
  stmt->bind(1, igroup_id);

  if( call_depth >= 0 )
    stmt->bind(2, func_id);

  CallVec* call_vec = new CallVec; 
  for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
    int callee_id = row.get<int>(1);
    int ncalls    = row.get<int>(2);

    if(expand_ncalls){
      for(int i = 0 ; i < ncalls ; i++)
        call_vec->push_back(callee_id);
    }else
      call_vec->push_back(callee_id);
  }

  return call_vec;
}

CallVec* 
load_function_api_calls_for(int func_id, bool reachability_graph)
{
  SqlDatabase::StatementPtr stmt = transaction->statement(
     "select distinct scg.callee from "  
     + std::string(reachability_graph ? "semantic_rg" : "semantic_cg " ) + " as scg "  
     //" join tmp_interesting_funcs as tif on tif.func_id = scg.callee "
     " where scg.caller=? ORDER BY scg.callee"
     );
 
  stmt->bind(0, func_id);


  CallVec* call_vec = new CallVec; 
  for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
    int callee_id = row.get<int>(0);

    call_vec->push_back(callee_id);
  }

  return call_vec;
}


using namespace boost;


void
normalize_func_to_id(int func1_id, int func2_id, double similarity_threshold, std::map<int,int>& norm_map, bool reachability_graph)
{
  std::string _query_condition(
      //all function pairs that are semantically similar
      " select distinct func1_id, func2_id from ("
      "   select sem.func1_id, sem.func2_id"
      "   from semantic_funcsim as sem"
      "   join tmp_called_functions as tcf1 on sem.func1_id = tcf1.callee_id"
      "   join tmp_called_functions as tcf2 on sem.func2_id = tcf2.callee_id"
      //                          0                    1                    2
      "   where sem.similarity >= ? and sem.func1_id = ? and sem.func2_id = ?"

      " UNION"

      //all library call pairs that has the same name in the call trace
      "   select sem.func_id as func1_id, sem2.func_id as func2_id"
      "   from tmp_library_funcs as sem"
      "   join tmp_library_funcs sem2 on sem.name = sem2.name"
      "   join tmp_called_functions as tcf1 on sem.func_id  = tcf1.callee_id"
      "   join tmp_called_functions as tcf2 on sem2.func_id = tcf2.callee_id"
      //                         3                   4                       5                   6
      "   where (tcf1.func_id  = ? OR tcf1.func_id = ?) AND (tcf2.func_id  = ? OR tcf2.func_id = ?) "

      " UNION"

      //all library calls that has the same name as a tested function
      "   select tplt.func_id as func1_id, tlib.func_id as func2_id"
      "   from tmp_plt_func_names as tplt" 
      "   join tmp_library_funcs as tlib on tplt.name = tlib.name"
      "   join " + std::string(reachability_graph ? "semantic_rg" : "semantic_cg " ) + " as scg on tplt.func_id  = scg.callee"
      //                       7 8
      "   where scg.caller IN (?,?)"

      " UNION "

      //all library call pairs that has the same name in the static cg
      "   select sem.func_id as func1_id, sem2.func_id as func2_id"
      "   from tmp_library_funcs as sem" 
      "   join tmp_library_funcs sem2 on sem.name = sem2.name"
      "   join " + std::string(reachability_graph ? "semantic_rg" : "semantic_cg " ) + " as tcf1 on sem.func_id  = tcf1.callee"
      "   join " + std::string(reachability_graph ? "semantic_rg" : "semantic_cg " ) + " as tcf2 on sem2.func_id = tcf2.callee"
      //                        9 10                     11 12
      "   where tcf1.caller IN (?,?) AND tcf2.caller IN (?,?)"

      ") as functions_to_normalize"
      );


  //Get all vetexes and find the union 

  SqlDatabase::StatementPtr stmt = transaction->statement(_query_condition);

  stmt->bind(0, similarity_threshold);
  stmt->bind(1, std::min(func1_id, func2_id));
  stmt->bind(2, std::max(func1_id, func2_id));
  stmt->bind(3, func1_id);
  stmt->bind(4, func2_id);
  stmt->bind(5, func1_id);
  stmt->bind(6, func2_id);
  stmt->bind(7, func1_id);
  stmt->bind(8, func2_id);
  stmt->bind(9, func1_id);
  stmt->bind(10, func2_id);
  stmt->bind(11, func1_id);
  stmt->bind(12, func2_id);

  if(stmt->begin() == stmt->end())
    return;

  //Count how many vertices we have for boost graph

  int VERTEX_COUNT = transaction->statement("select count(*) from semantic_functions")->execute_int();

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

    int first_value = -1;

    // Iterate through the child vertex indices for [current_index]
    BOOST_FOREACH(VertexIndex child_index,
        components[current_index]) {

      if(first_value >= 0) 
        norm_map[child_index] = first_value;
      else
        first_value = child_index;

    }

  }


}


/* Remove the functions from the compilation unit that is only available in one of the traces.
 *   - criteria complement of the functions from the files of the caller functions in the 
 *     call trace is removed.
 */
std::pair<CallVec*, CallVec*> 
remove_compilation_unit_complement(int func1_id, int func2_id, int igroup_id, int similarity, CallVec* func1_vec, CallVec* func2_vec)
{
  CallVec* new_func1_vec = new CallVec;
  CallVec* new_func2_vec = new CallVec;


  if( func1_vec->size() > 0 || func2_vec->size() > 0  ){

    //find the set complement of functions called by the two functions
    // - we are not interested in functions called by both 

    std::set<int> func1_vec_set;
    std::set<int> func2_vec_set;

    for(CallVec::iterator it = func1_vec->begin(); it != func1_vec->end(); ++it)
    {
      func1_vec_set.insert(*it);
    }
 
    for(CallVec::iterator it = func2_vec->begin(); it != func2_vec->end(); ++it)
    {
      func2_vec_set.insert(*it);
    }

    std::set<int> func1_func2_complement;

    std::set_difference(func1_vec_set.begin(), func1_vec_set.end(), func2_vec_set.begin(), func2_vec_set.end(), std::inserter(func1_func2_complement, func1_func2_complement.end()) );
    
    //find the compilation units in question. A compilation unit is in our case a file.
    SqlDatabase::StatementPtr func1_file_stmt = transaction->statement( "select file_id from semantic_functions where id = ?" );
    func1_file_stmt->bind(0, func1_id);

    int func1_file_id = func1_file_stmt->execute_int(); 


    SqlDatabase::StatementPtr func2_file_stmt = transaction->statement( "select file_id from semantic_functions where id = ?" );
    func2_file_stmt->bind(0, func2_id);

    int func2_file_id = func2_file_stmt->execute_int(); 


    //find the functions that needs to be removed
    //  - all functions that has a clone in between the files
    SqlDatabase::StatementPtr stmt = transaction->statement(
        "select sem.func1_id, sem.func2_id from semantic_funcsim as sem"
        " join semantic_functions as sf1 on sem.func1_id = sf1.id"
        " join semantic_functions as sf2 on sem.func2_id = sf2.id"
        " where similarity >= ? AND sf1.file_id IN (?,?) AND sf2.file_id IN (?, ?) AND sf1.file_id != sf2.file_id"
        );


    stmt->bind(0, similarity);
    stmt->bind(1, func1_file_id);
    stmt->bind(2, func2_file_id);
    stmt->bind(3, func1_file_id);
    stmt->bind(4, func2_file_id);

    std::set<int> complement_functions;
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
      int clone_func1 = row.get<int>(0);
      int clone_func2 = row.get<int>(1);

      complement_functions.insert(clone_func1);
      complement_functions.insert(clone_func2);
    }

    //find the functions we want to remove
    //  - functions present with clones in between the files that is not part of both traces
    std::set<int> remove_these;

    std::set_intersection(complement_functions.begin(), complement_functions.end(), func1_func2_complement.begin(), func1_func2_complement.end(), std::inserter(remove_these, remove_these.end()) );

    //prune functions to remove away from the call trace into new vectors
    for(CallVec::iterator it = func1_vec->begin(); it != func1_vec->end(); ++it)
      if ( remove_these.find(*it) == remove_these.end()) 
        new_func1_vec->push_back(*it);

    for(CallVec::iterator it = func2_vec->begin(); it != func2_vec->end(); ++it)
      if( remove_these.find(*it) == remove_these.end())
        new_func2_vec->push_back(*it);

  }

  return std::pair<CallVec*, CallVec*>(new_func1_vec, new_func2_vec);
}



double
similarity(int func1_id, int func2_id, int igroup_id, double similarity, bool ignore_inline_candidates, bool ignore_no_compares, int call_depth, bool expand_ncalls, 
    std::map<int,int>& norm_map)
{



 CallVec* func1_vec = load_api_calls_for(func1_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);
 CallVec* func2_vec = load_api_calls_for(func2_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);


 //Detect and normalize similar function calls

 if( (func1_vec->size() == 0) & (func2_vec->size() == 0) )
   return -1;

 for(CallVec::iterator it = func1_vec->begin(); it != func1_vec->end(); ++it )
 {

   std::map<int,int>::iterator located_it = norm_map.find(*it);

   if(located_it != norm_map.end())
     *it = located_it->second;
 }

 for(CallVec::iterator it = func2_vec->begin(); it != func2_vec->end(); ++it )
 {

   std::map<int,int>::iterator located_it = norm_map.find(*it);

   if(located_it != norm_map.end())
     *it = located_it->second;
 }




 //remove possible inlined functions from the traces

 if( ignore_inline_candidates ){
   std::pair<CallVec*, CallVec*> removed_complement = remove_compilation_unit_complement(func1_id, func2_id, igroup_id, similarity, func1_vec, func2_vec);

   delete func1_vec;
   delete func2_vec;
   func1_vec = removed_complement.first;
   func2_vec = removed_complement.second;
 }


if( ( func1_vec->size() == 0 ) & ( func2_vec->size() == 0 ) )
   return -1;

 
 size_t dl_max = std::max(func1_vec->size(), func2_vec->size());

 double dl_similarity = 1.0;

 if ( dl_max > 0 ){

   size_t dl = Combinatorics::damerau_levenshtein_distance(*func1_vec, *func2_vec);

   dl_similarity = 1.0 - (double)dl / dl_max;

 }


 delete func1_vec;
 delete func2_vec;




 return dl_similarity;
};

double 
whole_function_similarity(int func1_id, int func2_id, std::map<int,int>& norm_map, bool reachability_graph)
{


  CallVec* func1_vec = load_function_api_calls_for(func1_id, reachability_graph);
  CallVec* func2_vec = load_function_api_calls_for(func2_id, reachability_graph);

  //normalize functions


  for(CallVec::iterator it = func1_vec->begin(); it != func1_vec->end(); ++it )
  {

    std::map<int,int>::iterator located_it = norm_map.find(*it);

    if(located_it != norm_map.end())
      *it = located_it->second;
  }

  for(CallVec::iterator it = func2_vec->begin(); it != func2_vec->end(); ++it )
  {

    std::map<int,int>::iterator located_it = norm_map.find(*it);

    if(located_it != norm_map.end())
      *it = located_it->second;
  }




  //compute similarity
  size_t dl_max = std::max(func1_vec->size(), func2_vec->size());

  double dl_similarity = 1.0;

  if ( dl_max > 0 ){

    size_t dl = Combinatorics::damerau_levenshtein_distance(*func1_vec, *func2_vec);

    dl_similarity = 1.0 - (double)dl / dl_max;

  }


  delete func1_vec;
  delete func2_vec;

  return dl_similarity;

};


class FunctionPair{

  public:

  int func1_id;
  int func2_id;

  FunctionPair(int _func1_id, int _func2_id) :  func1_id(_func1_id), func2_id(_func2_id) {};
};

typedef std::vector<FunctionPair*> FunctionPairVec;


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
    int  call_depth = -1;

 
    bool ignore_faults = true;
    double semantic_similarity_threshold = 0.70;

    bool expand_ncalls = false;

    bool reachability_graph = true;

    bool show_progress = false;

    bool verbose = false;

    std::string input_file_name;

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
        } else if (!strcmp(argv[argno], "--progress")) {
          show_progress = true;
        } else if (!strcmp(argv[argno], "--no-expand-ncalls")) {
          expand_ncalls = false;
        } else if (!strncmp(argv[argno], "--file=", 7)) {
            input_file_name = argv[argno]+7;
        } else if (!strcmp(argv[argno], "--verbose")) {
            verbose = true;
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
    int64_t cmd_id = CloneDetection::start_command(transaction, argc, argv, "calculating api similarity");


    // Read function pairs from standard input or the file
    FunctionPairs worklist;
    if (input_file_name.empty()) {
        std::cerr <<argv0 <<": reading function pairs worklist from stdin...\n";
        worklist = load_worklist("stdin", stdin);
    } else {
        FILE *in = fopen(input_file_name.c_str(), "r");
        if (NULL==in) {
            std::cerr <<argv0 <<": " <<strerror(errno) <<": " << input_file_name <<"\n";
            exit(1);
        }
        worklist = load_worklist(input_file_name, in);
        fclose(in);
    }
    size_t npairs = worklist.size();
    std::cerr <<argv0 <<": work list has " <<npairs <<" function pair" <<(1==npairs?"":"s") <<"\n";

    // Process each function pair
    CloneDetection::Progress progress(npairs);
    progress.force_output(show_progress);



    //Creat list of functions and igroups to analyze

    SqlDatabase::StatementPtr insert_stmt = transaction->statement("insert into api_call_similarity"
                                                            // 0        1         2           3          4
                                                            "(func1_id, func2_id, max_similarity, min_similarity, ave_similarity, cg_similarity )"
                                                            " values (?, ?, ?, ?, ?, ?)");
 
    while (!worklist.empty()) {
      ++progress;
      int func1_id, func2_id;
      boost::tie(func1_id, func2_id) = worklist.shift();
      if (verbose)
        std::cerr <<argv0 <<": func1_id=" <<func1_id <<" func2_id=" <<func2_id <<"\n";


      SqlDatabase::StatementPtr igroup_stmt = transaction->statement("select distinct sem1.igroup_id from semantic_fio as sem1 "
          " join semantic_fio as sem2 ON sem2.igroup_id = sem1.igroup_id AND sem2.func_id = ?"
          " where sem1.func_id = ?  "+
          std::string(ignore_faults?" and sem1.status = 0 and sem2.status = 0":"") 
          + "ORDER BY sem1.igroup_id");
      igroup_stmt->bind(0, func2_id);
      igroup_stmt->bind(1, func1_id);

      int ncompares = 0;

      double max_api_similarity = 0;
      double min_api_similarity = INT_MAX;
      double ave_api_similarity = 0;



      std::map<int,int> norm_map;
      normalize_func_to_id(func1_id, func2_id, semantic_similarity_threshold, norm_map, reachability_graph);


      for (SqlDatabase::Statement::iterator row=igroup_stmt->begin(); row!=igroup_stmt->end(); ++row) {
        int igroup_id = row.get<int>(0);

        double api_similarity = similarity(func1_id, func2_id, igroup_id, semantic_similarity_threshold, ignore_inline_candidates, 
            ignore_no_compares, call_depth, expand_ncalls, norm_map  );

        if( api_similarity < 0)
          continue;

        max_api_similarity = std::max(api_similarity, max_api_similarity);
        min_api_similarity = std::min(api_similarity, min_api_similarity);

        ave_api_similarity += api_similarity;

        ncompares++;
      }


      if( ncompares == 0)
      {
        ave_api_similarity = 1.0;
        max_api_similarity = 1.0;
        min_api_similarity = 1.0;

      }else{
      ave_api_similarity = ave_api_similarity/ncompares;


      }


      //find call similarity between functions

      double cg_similarity = whole_function_similarity(func1_id, func2_id, norm_map, reachability_graph);


      insert_stmt->bind(0, func1_id);
      insert_stmt->bind(1, func2_id);
      insert_stmt->bind(2, max_api_similarity);
      insert_stmt->bind(3, min_api_similarity);
      insert_stmt->bind(4, ave_api_similarity);
      insert_stmt->bind(5, cg_similarity);

      insert_stmt->execute();

    }
    
    progress.message("committing changes");
    std::string mesg = "calculated api similarity for "+
                       StringUtility::numberToString(npairs)+" function pair"+(1==npairs?"":"s");
    CloneDetection::finish_command(transaction, cmd_id, mesg);


    transaction->commit();

    progress.clear();

    return 0;
} 

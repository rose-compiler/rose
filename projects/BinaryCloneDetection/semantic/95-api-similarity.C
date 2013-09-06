#include <cerrno>
#include "sage3basic.h"
#include "CloneDetectionLib.h"

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
 transaction->execute("create temporary table tmp_tested_funcs as select distinct func_id from semantic_fio");
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

void 
load_api_calls(int func1_id, int func2_id, int igroup_id, CallVec& call_vec, bool ignore_no_compares, int call_depth, bool expand_ncalls )
{
 call_vec.clear();

 CallVec* func1_vec = load_api_calls_for(func1_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);
 CallVec* func2_vec = load_api_calls_for(func2_id, igroup_id, ignore_no_compares, call_depth, expand_ncalls);



 //Find clones

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

    return 0;
} 

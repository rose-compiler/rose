// Adds functions to the database.

#include "sage3basic.h"
#include "CloneDetectionLib.h"

#include <vector>
#include <boost/lexical_cast.hpp>

static SqlDatabase::TransactionPtr transaction;

using namespace boost;

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE SPECIMEN\n"
              <<"  This command takes data from a results database and computes rates (false positives, false negatives)"
              <<"  and produces graphs and tables. \n"
              <<"\n"
              <<"    --sem-threshold=0.0|..|1.0\n"
              <<"            Similarity measure for semantic similarity between 0 and 1.\n"
              <<"    --path-threshold=0.0|..|1.0\n"
              <<"            Path sensitive similarity threshold between 0 and 1.\n"
              <<"    --cg-threshold=0.0|..|1.0\n"
              <<"            Call graph similarity threshold between 0 and 1.\n"
              <<"    --file-regex=*\n"
              <<"            regular expression for file-name we are interested in.\n"
              <<"    --nisnsn=0|..|MAX_INT\n"
              <<"            only consider functions with at least this number of instructions.\n"
              <<"    --num-funcs=0|..|MAX_INT\n"
              <<"            Number of functions to consider.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

void
create_tables_and_indexes(std::string regex_of_targets, int ninsns, double path_threshold, double cg_threshold,
                          double sem_threshold)
{
    transaction->execute("drop table IF EXISTS tmp_tested_funcs");
    transaction->execute("drop table IF EXISTS tmp_plt_func_names");
    transaction->execute("drop table IF EXISTS tmp_library_funcs");
    transaction->execute("drop table IF EXISTS tmp_interesting_funcs");
    transaction->execute("drop table IF EXISTS tmp_called_functions");
    transaction->execute("drop table IF EXISTS tmp_random_function_stats");
    transaction->execute("drop table IF EXISTS tmp_clone_pairs");
    transaction->execute("drop table IF EXISTS tmp_duplicate_funcs");

    //table of tested functions. Criteria is that it needs to pass at least one test.
    //Looking for functions that are:
    //  1) tested
    //  2) is not a dynamic linking stub function
    //  3) has more instructions that ninsns
    transaction->execute("create table tmp_duplicate_funcs as select name from semantic_functions"
                         "group by name having count(*) > 1");
    transaction->execute("create table tmp_tested_funcs as select distinct func_id from semantic_fio where status = 0");
    transaction->execute("create table tmp_library_funcs  as ( select distinct id as func_id, name from semantic_functions"
                         " where name like '%@plt')");
    transaction->execute("create table tmp_interesting_funcs as ( select ttf.func_id from tmp_tested_funcs ttf"
                         " join semantic_functions as sf on sf.id = ttf.func_id "
                         " join semantic_files as sem_file on sem_file.id = sf.file_id "
                         " join tmp_duplicate_funcs as tdf on tdf.name = sf.name "
                         " where sem_file.name like '%"+ regex_of_targets  + "%'"
                         " and sf.ninsns >= "+ boost::lexical_cast<std::string>(ninsns) +
                         " and coalesce(sf.name,'') != '' and sf.name not like '\\_%' "
                         " except select func_id from tmp_library_funcs)");
    transaction->execute("create table tmp_clone_pairs as "
                         " select sim.func1_id, sim.func2_id, sf1.name as func1_name, sf2.name as func2_name "
                         "    from semantic_funcsim sim "
                         "    join api_call_similarity api_sem"
                         "       on api_sem.func1_id = sim.func1_id and api_sem.func2_id = sim.func2_id"
                         "    join semantic_functions as sf1 on sim.func1_id = sf1.id "
                         "    join semantic_functions as sf2 on sim.func2_id = sf2.id "
                         "    where sim.similarity >= "       + boost::lexical_cast<std::string>(sem_threshold)  +
                         "    and api_sem.ave_similarity >= " + boost::lexical_cast<std::string>(path_threshold) +
                         "    and api_sem.cg_similarity  >= " + boost::lexical_cast<std::string>(cg_threshold));
    transaction->execute("create table tmp_random_function_stats ("
                         " func_id integer, total_matches integer, positives integer, negatives integer)");
}

void
evaluate_random_function(int random_fid)
{
    int total_matches = transaction->statement("select count(*) from tmp_clone_pairs"
                                               " where func1_id = " + boost::lexical_cast<std::string>(random_fid) +
                                               " or func2_id = " + boost::lexical_cast<std::string>(random_fid))->execute_int();
    int matches_with_same_name = transaction->statement("select count(*) from tmp_clone_pairs"
                                                        " where (func1_id = " + boost::lexical_cast<std::string>(random_fid) +
                                                        " or func2_id = " + boost::lexical_cast<std::string>(random_fid) + ")" +
                                                        " and func1_name = func2_name")->execute_int();
    SqlDatabase::StatementPtr insert_stmt = transaction->statement("insert into tmp_random_function_stats("
                                                                   " func_id, total_matches, positives, negatives)"
                                                                   " values(?,?,?,?)");
    insert_stmt->bind(0,random_fid);
    insert_stmt->bind(1,total_matches);
    insert_stmt->bind(2,matches_with_same_name);
    insert_stmt->bind(3,total_matches-matches_with_same_name);
    insert_stmt->execute();
}

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

    int argno = 1;
    std::string regex_of_targets;
    int ninsns = 0;
    double path_threshold = 0.0;
    double cg_threshold   = 0.0;
    double sem_threshold  = 0.0;
    int num_funcs = 0;
    std::string file_regex = "";
    std::vector<std::string> signature_components;

    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        std::cout << argv[argno] << std::endl;
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--sem-threshold=",16)) {
            sem_threshold = boost::lexical_cast<double>(argv[argno]+16);
        } else if (!strncmp(argv[argno], "--path-threshold=",17)) {
            path_threshold = boost::lexical_cast<double>(argv[argno]+17);
        } else if (!strncmp(argv[argno], "--cg-threshold=",15)) {
            cg_threshold = boost::lexical_cast<double>(argv[argno]+15);
        } else if (!strncmp(argv[argno], "--num-funcs=",12)) {
            num_funcs = boost::lexical_cast<int>(argv[argno]+12);
        } else if (!strncmp(argv[argno], "--ninsns=",9)) {
            ninsns = boost::lexical_cast<int>(argv[argno]+9);
        } else if (!strncmp(argv[argno], "--file-regex=",13)) {
            file_regex = boost::lexical_cast<std::string>(argv[argno]+13);

        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);

    std::string db_name(argv[argno++]);
    std::cout << "Connecting to db:" << db_name << std::endl;
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(db_name);
    transaction = conn->transaction();

    create_tables_and_indexes(file_regex, ninsns, path_threshold, cg_threshold, sem_threshold);
    SqlDatabase::StatementPtr stmt2 = transaction->statement("select func_id from tmp_interesting_funcs"
                                                             " order by random() limit ?;");
    stmt2->bind(0, num_funcs);

    for (SqlDatabase::Statement::iterator row=stmt2->begin(); row!=stmt2->end(); ++row) {
        int random_fid = row.get<int>(0);
        evaluate_random_function(random_fid);
    }

    transaction->commit();
    return 0;
}

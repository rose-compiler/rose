// Generates a list of function pairs whose similarity needs to be computed.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include <cerrno>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>

static SqlDatabase::TransactionPtr transaction;

using namespace boost;

typedef boost::adjacency_list< boost::vecS,
                               boost::vecS,
                               boost::directedS > DirectedGraph;

void
insert_specialized_funtions()
{
    transaction->execute("drop table IF EXISTS compiler_specializations;");
    transaction->execute("create table compiler_specializations(func_id integer, equivalent_func_name text);");

    SqlDatabase::StatementPtr insert_stmt = transaction->statement("insert into compiler_specializations"
                                                                   "(func_id, equivalent_func_name)"
                                                                   " values (?, ?)");
    std::cerr << "Compute equivalent functions";

    SqlDatabase::StatementPtr stmt = transaction->statement("select id,name from semantic_functions where name like "
                                                            "'__intel%' and id NOT IN ( select func_id as id from "
                                                            "tmp_uninteresting_funcs)");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int func_id = row.get<int>(0);
        std::string name = row.get<std::string>(1);
        boost::replace_first(name, "_P3", "");

        size_t left_word_boundary  = name.find_last_of("_");
        size_t right_word_boundary = name.find_last_of("@");
        if (right_word_boundary == std::string::npos)
            right_word_boundary = name.find_last_of(".");

        std::string equivalent_function = name.substr(left_word_boundary+1, right_word_boundary == std::string::npos ?
                                                      std::string::npos : right_word_boundary-1-left_word_boundary);

        std::cerr << "Equivalent function: " << equivalent_function << std::endl;
        insert_stmt->bind(0, func_id);
        insert_stmt->bind(1, equivalent_function);
        insert_stmt->execute();

        insert_stmt->bind(0, func_id);
        insert_stmt->bind(1, equivalent_function+"@plt");
        insert_stmt->execute();
    }
}

void
compute_eqivalence_classes(double similarity_threshold, bool reachability_graph)
{

    transaction->execute("delete from equivalent_classes");
    SqlDatabase::StatementPtr insert_stmt = transaction->statement("insert into equivalent_classes"
                                                                   "(func_id, equivalent_func_id)"
                                                                   " values (?, ?)");

    std::string _query_condition(//all function pairs that are semantically similar
                                 " select distinct func1_id, func2_id from ("
                                 "   select sem.func1_id, sem.func2_id"
                                 "   from semantic_funcsim as sem"
                                 "   where sem.similarity >= ? "

                                 " UNION"

                                 //all library call pairs that has the same name in the call trace
                                 "   select sem.func_id as func1_id, sem2.func_id as func2_id"
                                 "   from tmp_library_funcs as sem"
                                 "   join tmp_library_funcs sem2 on sem.name = sem2.name"
                                 "   join tmp_called_functions as tcf1 on sem.func_id  = tcf1.callee_id"
                                 "   join tmp_called_functions as tcf2 on sem2.func_id = tcf2.callee_id"

                                 " UNION"

                                 //all library calls that has the same name as a tested function
                                 "   select tplt.func_id as func1_id, tlib.func_id as func2_id"
                                 "   from tmp_plt_func_names as tplt"
                                 "   join tmp_library_funcs as tlib on tplt.name = tlib.name"
                                 "   join " + std::string(reachability_graph ? "semantic_rg" : "semantic_cg ") +
                                 "     as scg on tplt.func_id  = scg.callee"

                                 " UNION "

                                 //all library call pairs that has the same name in the static cg
                                 "   select sem.func_id as func1_id, sem2.func_id as func2_id"
                                 "   from tmp_library_funcs as sem"
                                 "   join tmp_library_funcs sem2 on sem.name = sem2.name"
                                 "   join " + std::string(reachability_graph ? "semantic_rg" : "semantic_cg ") +
                                 "     as tcf1 on sem.func_id  = tcf1.callee"
                                 "   join " + std::string(reachability_graph ? "semantic_rg" : "semantic_cg ") +
                                 "     as tcf2 on sem2.func_id = tcf2.callee"

                                 " UNION "

                                 " select cs.func_id as func1_id, sf.id as func2_id from compiler_specializations as cs "
                                 " join semantic_functions as sf on sf.name=cs.equivalent_func_name "

                                 ") as functions_to_normalize");

    // Get all vetexes and find the union
    SqlDatabase::StatementPtr stmt = transaction->statement(_query_condition);
    stmt->bind(0, similarity_threshold);
    if (stmt->begin() == stmt->end())
        return;

    // Count how many vertices we have for boost graph
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
        BOOST_FOREACH(VertexIndex child_index, components[current_index]) {
            if (first_value >= 0) {
                insert_stmt->bind(0, child_index);
                insert_stmt->bind(1, first_value);
                insert_stmt->execute();
            } else {
                first_value = child_index;
            }
        }
    }
}

void
create_reachable_node_graph()
{
    int VERTEX_COUNT = transaction->statement("select count(*) from semantic_functions")->execute_int();

    // Delete from the database all call graph edges that correspond to one of our specimen files.
    transaction->execute("delete from semantic_rg");

    SqlDatabase::StatementPtr insert_stmt = transaction->statement("insert into semantic_rg"
                                                                   // 0        1         2
                                                                   "(caller, callee, file_id)"
                                                                   " values (?, ?, ?)");

    SqlDatabase::StatementPtr stmt = transaction->statement("select id from semantic_files");

    // Create the new reachability graph
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!= stmt->end(); ++row) {
        DirectedGraph G(VERTEX_COUNT);

        // Add all edges from functions in file
        int file_id = row.get<int>(0);
        SqlDatabase::StatementPtr func_in_file_stmt = transaction->statement("select scg.caller, scg.callee"
                                                                             " from semantic_cg as scg"
                                                                             " join semantic_functions as sf"
                                                                             "   on scg.caller = sf.id"
                                                                             " join tmp_interesting_funcs as tif"
                                                                             "   on tif.func_id = scg.callee"
                                                                             " where sf.file_id = ?");
        func_in_file_stmt->bind(0,file_id);

        std::set<int> callers_in_file;

        // Insert edges into graph
        for (SqlDatabase::Statement::iterator func_row = func_in_file_stmt->begin();
             func_row != func_in_file_stmt->end();
             ++func_row) {
            int caller = func_row.get<int>(0);
            int callee = func_row.get<int>(1);
            boost::add_edge(caller, callee, G);
            callers_in_file.insert(caller);
        }

        typedef graph_traits<DirectedGraph>::vertex_iterator vertex_iter;
        std::pair<vertex_iter, vertex_iter> vp;

        // Iterate over each vertex of the graph, find all reachable nodes, and insert them into the db
        for (vp = boost::vertices(G); vp.first != vp.second; ++vp.first) {
            int caller = *vp.first;

            // If the vertex is a function in the file, analyze it, otherwise skip.
            if (callers_in_file.find(caller) == callers_in_file.end())
                continue;

            // Find all reachable nodes
            typedef DirectedGraph::vertex_descriptor Vertex;
            std::vector<Vertex> reachable;

            boost::breadth_first_search(G, *vp.first,
                                        boost::visitor(boost::make_bfs_visitor(boost::write_property(boost::identity_property_map(),
                                                                                                     std::back_inserter(reachable),
                                                                                                     boost::on_discover_vertex()))));


            //insert all callees by function into db
            for (std::vector<Vertex>::iterator r_it = reachable.begin(); r_it != reachable.end(); ++r_it) {
                int callee = *r_it;
                if (caller == callee)
                    continue;

                insert_stmt->bind(0, caller);
                insert_stmt->bind(1, callee);
                insert_stmt->bind(2, file_id);

                insert_stmt->execute();
            }
        }
    }
}

void
create_tables_and_indexes(int call_depth)
{
    transaction->execute("drop table IF EXISTS tmp_tested_funcs");
    transaction->execute("drop table IF EXISTS tmp_plt_func_names");
    transaction->execute("drop table IF EXISTS tmp_library_funcs");
    transaction->execute("drop table IF EXISTS tmp_interesting_funcs");
    transaction->execute("drop table IF EXISTS tmp_called_functions");
    transaction->execute("drop table IF EXISTS tmp_uninteresting_funcs");

    // Table of tested functions. Criteria is that it needs to pass at least one test.
    transaction->execute("create table tmp_tested_funcs as select distinct func_id from semantic_fio where status = 0");
    transaction->execute("create table tmp_plt_func_names as ( select distinct name||'@plt' as name, id as func_id from "
                         "semantic_functions where name NOT LIKE '%@plt')");
    transaction->execute("create table tmp_library_funcs  as ( select distinct id as func_id, name from semantic_functions where "
                         "name LIKE '%@plt')");
    transaction->execute("create table tmp_interesting_funcs as ( select func_id from tmp_tested_funcs UNION select func_id "
                         "from tmp_library_funcs)");

    // The __x86.get_pc_thunk.bx isn't a normal function. It's just GCC's way of loading the EIP into EBX in preparation for
    // finding thing like the PLT in position independent code.  I would expect that ICC uses the exact same instructions even
    // if it doesn't give it a function symbol.

    // tolower() is a real function in ICC rather than macros that reference
    // arrays of character characteristics.

    transaction->execute("create table tmp_uninteresting_funcs as (select id as func_id from semantic_functions where name LIKE"
                         " '__x86.get_pc_thunk.%' OR "
                         " name IN ( 'tolower@plt', '__intel_cpu_features_init_x', '__intel_cpu_features_init',"
                         " '__intel_new_feature_proc_init', '__intel_proc_init_ftzdazule', '__intel_cpu_features_init_body',"
                         " '__intel_f2int', '__intel_cpu_indicator_init'))");
    transaction->execute("delete from tmp_interesting_funcs where func_id IN (select func_id from tmp_uninteresting_funcs)");

    insert_specialized_funtions();

    // Table of called fuctions.  Do this in two steps so we're not doing a join on the large semantic_fio_calls table. Also,
    // we don't need to have the igroup_id column since it's never used. Note that this table is named "functions" not "funcs"
    // like the rest.
    transaction->execute("create table tmp_all_called_funcs as"
                         "  select distinct caller_id as func_id, callee_id"
                         "  from semantic_fio_calls");
    transaction->execute("create table tmp_called_functions as"
                         "  select f1.*"
                         "  from tmp_all_called_funcs as f1"
                         "  join tmp_interesting_funcs as f2"
                         "    on f1.callee_id = f2.func_id");
    transaction->execute("drop table tmp_all_called_funcs");

    transaction->execute("drop index IF EXISTS fr_call_index");
    transaction->execute("drop index IF EXISTS fr_tmp_called_index");
    transaction->execute("drop index IF EXISTS fr_tmp_interesting_funcs_index");

    if (call_depth >= 0) {
        transaction->execute("create index fr_call_index on semantic_fio_calls(func_id, igroup_id, caller_id)");
    } else {
        transaction->execute("create index fr_call_index on semantic_fio_calls(func_id, igroup_id)");
    }

    transaction->execute("create index fr_tmp_called_index on tmp_called_functions(callee_id)");
    transaction->execute("create index fr_tmp_interesting_funcs_index on tmp_interesting_funcs(func_id)");
    transaction->execute("create index fr_tmp_library_funcs_index on tmp_library_funcs(func_id)");
    transaction->execute("create index fr_tmp_library_funcs_name_index on tmp_library_funcs(name)");
}

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE\n"
              <<"  This command generates a list of function pairs whose similarity value needs to be computed.  The list\n"
              <<"  is emitted on the standard output stream one pair at a time.  Each line is either a comment (first\n"
              <<"  character is a '#'), blank, or a pair containing two function IDs separated by white space.  The output\n"
              <<"  from this command is typically partitioned into multiple sets of lines and fed into 32-func-similarity\n"
              <<"  commands running in parallel.\n"
              <<"\n"
              <<"  These switches control which functions are compared:\n"
              <<"    --sem-threshold=0.0|..|1.0\n"
              <<"            Similarity measure for semantic similarity between 0 and 1.\n"
              <<"    --min-insns=0|..|MAX_INT\n"
              <<"            Minimum number of instructions in candidate functions.\n"
              <<"    --call-depth=-1|0|..|MAX_INT\n"
              <<"            Depth of callgraph to consider. -1 is all. \n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

static struct Switches {
    Switches()
        : delete_old_data(false), relation_id(0) {}
    bool delete_old_data;
    std::string exclude_functions_table;
    int relation_id;
} opt;

int
main(int argc, char *argv[])
{
    int call_depth = -1;
    int min_insns  = 2;
    double semantic_similarity_threshold = 0.70;

    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    int argno = 1;
    opt.delete_old_data = true;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--call-depth=",13)) {
            call_depth = strtol(argv[argno]+13, NULL, 0);
        } else if (!strncmp(argv[argno], "--min-insns=",12)) {
            min_insns = strtol(argv[argno]+12, NULL, 0);
	} else if (!strncmp(argv[argno], "--sem-threshold=",16)) {
            semantic_similarity_threshold = boost::lexical_cast<double>(argv[argno]+16);
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);
    time_t start_time = time(NULL);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    transaction = conn->transaction();

    // Save ourself in the history if we're modifying the database.
    int64_t cmd_id=-1;
    if (opt.delete_old_data)
        cmd_id = CloneDetection::start_command(transaction, argc, argv, "clearing funcsim data for relation #"+
                                               StringUtility::numberToString(opt.relation_id), start_time);

    //delete old data
    transaction->execute("delete from api_call_similarity");

    //the data can get huge and we need helper tables and indexes for quick processing. We can
    //such as well create these here when we are running serially.
    create_tables_and_indexes(call_depth);

    //create reachability graph
    create_reachable_node_graph();

    //compute equivalence classes
    compute_eqivalence_classes(0.7, true);

    // Create indexes for normalize_func_to_id()
    transaction->execute("drop index if exists semantic_funcsim_similarity");
    transaction->execute("create index semantic_funcsim_similarity on semantic_funcsim(similarity)");
    transaction->execute("drop index if exists semantic_funcsim_func1_id");
    transaction->execute("create index semantic_funcsim_func1_id on semantic_funcsim(func1_id)");
    transaction->execute("drop index if exists semantic_funcsim_func2_id");
    transaction->execute("create index semantic_funcsim_func2_id on semantic_funcsim(func2_id)");
    transaction->execute("drop index if exists tmp_called_functions_func_id");
    transaction->execute("create index tmp_called_functions_func_id on tmp_called_functions(func_id)");

    // Create pairs of function IDs for those functions which have been tested and for which no similarity measurement has been
    // computed.
    std::cerr <<argv0 <<": creating work list\n";
    SqlDatabase::StatementPtr stmt2 = transaction->statement("select sf.func1_id, sf.func2_id from semantic_funcsim as sf "
                                                             " join semantic_functions as sf1 on sf1.id=sf.func1_id "
                                                             " join semantic_functions as sf2 on sf2.id=sf.func2_id "
                                                             " where similarity >= ? and "
                                                             " sf1.ninsns >= ? and sf2.ninsns >= ? ");
    stmt2->bind(0, semantic_similarity_threshold);
    stmt2->bind(1, min_insns);
    stmt2->bind(2, min_insns);

    for (SqlDatabase::Statement::iterator row=stmt2->begin(); row!=stmt2->end(); ++row)
        std::cout <<row.get<int>(0) <<"\t" <<row.get<int>(1) <<"\n";

    if (cmd_id>=0)
        CloneDetection::finish_command(transaction, cmd_id, "cleared funcsim table for relation #"+
                                       StringUtility::numberToString(opt.relation_id));

    transaction->commit();
    return 0;
}

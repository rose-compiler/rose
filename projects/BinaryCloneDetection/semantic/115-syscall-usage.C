// Adds functions to the database.

#include "sage3basic.h"
#include "CloneDetectionLib.h"

//to create the reachability grpah
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>

#include <vector>

//For finding the syscall numbers
#include "AsmFunctionIndex.h"
#include "AsmUnparser.h"
#include "BinaryLoader.h"
#include "PartialSymbolicSemantics.h"
#include "SMTSolver.h"
#include "BinaryControlFlow.h"
#include "BinaryFunctionCall.h"
#include "BinaryDominance.h"

static SqlDatabase::TransactionPtr transaction;

static
unsigned int ids_reserved_for_syscalls = 1000;

using namespace boost;
using namespace rose;

typedef boost::adjacency_list< boost::vecS,
                               boost::vecS,
                               boost::directedS > DirectedGraph;

using namespace CloneDetection;
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE SPECIMEN\n"
              <<"  This command parses and disassembles the specimen executable, selects functions that calls a system call\n"
              <<"  and adds those functions to the database.\n"
              <<"\n"
              <<"    --[no-]link\n"
              <<"            Perform dynamic linking and also consider functions that exist in the linked-in libraries. The\n"
              <<"            default is to not perform dynamic linking.\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n"
              <<"    SPECIMENS\n"
              <<"            Zero or more binary specimen names.\n";
    exit(exit_status);
}

/** Add edges to graph from functions that call system calls to system calls.
 *
 *  The first 1000 vertexes (0 to 999) in the graph is reserved for system calls, which is many more than the actual system
 *  calls in linux.  */
void
add_syscall_edges(DirectedGraph* G, std::vector<SgAsmFunction*>& all_functions)
{
    // Detect all system calls and add an edge from the function to the function to the system call
    for (unsigned int caller_id = 0; caller_id < all_functions.size(); ++caller_id) {
        SgAsmFunction *func = all_functions[caller_id];

        std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(func);

        for (std::vector<SgAsmInstruction*>::iterator inst_it = insns.begin(); inst_it != insns.end(); ++inst_it) {
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(*inst_it);
            if (insn == NULL)
                continue;

            SgAsmBlock *block = SageInterface::getEnclosingNode<SgAsmBlock>(insn);

            // On linux system calls are always interrups and all interrupts are system calls
            if (insn && block && insn->get_kind()==x86_int) {

                const SgAsmExpressionPtrList &opand_list = insn->get_operandList()->get_operands();
                SgAsmExpression *expr = opand_list.size()==1 ? opand_list[0] : NULL;

                //semantically execute the basic block to find out which sytem call was called
                if (expr && expr->variantT()==V_SgAsmIntegerValueExpression &&
                    0x80==isSgAsmIntegerValueExpression(expr)->get_value()) {

                    const SgAsmStatementPtrList &stmts = block->get_statementList();
                    size_t int_n;
                    for (int_n=0; int_n<stmts.size(); int_n++) {
                        if (isSgAsmInstruction(stmts[int_n])==insn)
                            break;
                    }

                    typedef PartialSymbolicSemantics::Policy<PartialSymbolicSemantics::State,
                                                             PartialSymbolicSemantics::ValueType> Policy;
                    typedef X86InstructionSemantics<Policy, PartialSymbolicSemantics::ValueType> Semantics;
                    Policy policy;
                    Semantics semantics(policy);

                    try {
                        semantics.processBlock(stmts, 0, int_n);
                        if (policy.readRegister<32>("eax").is_known()) {
                            int nr = policy.readRegister<32>("eax").known_value();
                            boost::add_edge(caller_id, nr, *G);
                        }
                    } catch (const Semantics::Exception&) {
                    } catch (const Policy::Exception&) {
                    }
                }
            }
        }
    }
}

/** Add edges from functions with the comilation unit to other functions within the compilation unit.
 *
 * The vertex number in the graph corresponding to a function is the index within all_functions+1000, since the first 1000
 * vertexes in the graph is reserved for system calls.
 *
 * The purpose of adding call edges is to detect when a function calls a function that calls a system call. */
void
add_call_edges_within_compilation_unit(DirectedGraph* G, std::vector<SgAsmFunction*>& all_functions, SgAsmInterpretation *interp)
{
    //Create a helper data structure for locating id in all_functions from a function pointer
    std::map<SgAsmFunction*, int> funcToId;
    for (unsigned int i = 0; i < all_functions.size(); ++i) {
        SgAsmFunction *func = all_functions[i];
        funcToId[func] = i;
    }

    //construct callgraph and add all edges
    CG cg = BinaryAnalysis::FunctionCall().build_cg_from_ast<CG>(interp);
    boost::graph_traits<CG>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(cg); ei!=ei_end; ++ei) {
        CG_Vertex caller_v = source(*ei, cg);
        CG_Vertex callee_v = target(*ei, cg);
        SgAsmFunction *caller = get(boost::vertex_name, cg, caller_v);
        SgAsmFunction *callee = get(boost::vertex_name, cg, callee_v);

        int caller_id = funcToId[caller];
        int callee_id = funcToId[callee];

        add_edge(caller_id + ids_reserved_for_syscalls, callee_id + ids_reserved_for_syscalls, *G);
    }
}

DirectedGraph*
create_reachability_graph(std::vector<SgAsmFunction*>& all_functions, SgAsmInterpretation *interp)
{
    // size of graph is the size of all functions + ids reserved for sytem calls
    DirectedGraph* G = new DirectedGraph(ids_reserved_for_syscalls+all_functions.size());
    add_syscall_edges(G, all_functions);
    add_call_edges_within_compilation_unit(G, all_functions, interp);
    return G;
}

void
add_calls_to_syscalls_to_db(SqlDatabase::TransactionPtr tx, DirectedGraph* G, std::vector<SgAsmFunction*> all_functions)
{
    // load the functions in db into memory
    std::map<std::string, std::set<int> > symbolToId;
    SqlDatabase::StatementPtr cmd3 = tx->statement("select id, name  from semantic_functions");
    for (SqlDatabase::Statement::iterator r=cmd3->begin(); r!=cmd3->end(); ++r) {
        int func_id           = r.get<int64_t>(0);
        std::string func_name = r.get<std::string>(1);

        if (func_name.size() == 0)
            continue;

        std::map<std::string, std::set<int> >::iterator fit = symbolToId.find(func_name);
        if (fit == symbolToId.end()) {
            std::set<int> function_ids;
            function_ids.insert(func_id);
            symbolToId[func_name] = function_ids;
        } else {
            fit->second.insert(func_id);
        }
    }

    DirectedGraph& graph = *G;
    SqlDatabase::StatementPtr stmt = tx->statement("insert into syscalls_made(caller, syscall_id, syscall_name) values(?,?,?)");

    // Iterate over all components of the reachability graph
    typedef graph_traits<DirectedGraph>::vertex_descriptor Vertex;
    graph_traits<DirectedGraph>::vertex_iterator i, end;
    for (tie(i, end) = vertices(graph); i != end; ++i) {
        if (*i < ids_reserved_for_syscalls)
            continue;

        std::set<int> syscalls;

        // Iterate through the child vertex indices for [current_index]
        std::vector<Vertex> reachable;
        boost::breadth_first_search(graph, *i,
                                    boost::visitor(boost::make_bfs_visitor(boost::write_property(boost::identity_property_map(),
                                                                                                 std::back_inserter(reachable),
                                                                                                 boost::on_discover_vertex()))));
        for (std::vector<Vertex>::iterator it = reachable.begin(); it != reachable.end(); ++it) {
            if (*it < ids_reserved_for_syscalls)
                syscalls.insert(*it);
        }

        int caller_id = *i - ids_reserved_for_syscalls;
        ROSE_ASSERT(caller_id >= 0);
        SgAsmFunction* caller = all_functions[caller_id];
        ROSE_ASSERT(isSgAsmFunction(caller) != NULL);

        std::string func_name = caller->get_name();
        if (func_name.length() == 0)
            continue;

        std::map<std::string, std::set<int> >::iterator equivalent_ids = symbolToId.find(func_name);
        if (equivalent_ids == symbolToId.end())
            equivalent_ids = symbolToId.find(func_name+"@plt");

        if (syscalls.size() > 0 && equivalent_ids != symbolToId.end()) {
            for (std::set<int>::iterator sit = syscalls.begin(); sit != syscalls.end(); ++sit) {
                int syscall_callee_id = *sit;
                extern std::map<int, std::string> linux32_syscalls; // defined in linux_syscalls.C
                const std::string &syscall_name = linux32_syscalls[syscall_callee_id];
                for (std::set<int>::iterator equivalent_id = equivalent_ids->second.begin();
                     equivalent_id != equivalent_ids->second.end(); ++ equivalent_id) {
                    stmt->bind(0, *equivalent_id);
                    stmt->bind(1, syscall_callee_id);
                    stmt->bind(2, syscall_name);
                    stmt->execute();
                }
            }
        }
    }
}

void
analyze_data(SqlDatabase::TransactionPtr tx)
{
    transaction->execute("drop table IF EXISTS functions_cg_accumulate;");
    transaction->execute("create table functions_cg_accumulate as select sm.caller, count(sm.callee) as num_calls,"
                         " sf.ninsns, sf.name from semantic_rg as sm join semantic_functions as sf on sf.id=sm.caller "
                         " group by sm.caller, sf.ninsns, sf.name;");

    transaction->execute("drop table IF EXISTS functions_rg_accumulate;");
    transaction->execute("create table functions_rg_accumulate as select sm.caller, count(sm.callee) as num_calls,"
                         " sf.ninsns, sf.name from semantic_cg as sm join semantic_functions as sf on sf.id=sm.caller "
                         " group by sm.caller, sf.ninsns, sf.name;");

    transaction->execute("drop table IF EXISTS syscalls_cg_accumulate;");
    transaction->execute("create table syscalls_cg_accumulate as select sm.caller, count(sm.callee) as num_calls,"
                         " sf.ninsns, sf.name from semantic_cg as sm join semantic_functions as sf on sf.id=sm.caller "
                         " join syscalls_made as sysm on sysm.caller=sm.callee "
                         " group by sm.caller, sf.ninsns, sf.name;");

    transaction->execute("drop table IF EXISTS syscalls_rg_accumulate;");
    transaction->execute("create table syscalls_rg_accumulate as select sm.caller, count(sm.callee) as num_calls,"
                         " sf.ninsns, sf.name from semantic_rg as sm join semantic_functions as sf on sf.id=sm.caller "
                         " join syscalls_made as sysm on sysm.caller=sm.callee "
                         " group by sm.caller, sf.ninsns, sf.name;");

    transaction->execute("drop table IF EXISTS syscalls_fio_accumulate");
    transaction->execute("create table syscalls_fio_accumulate as  "
                         " select fio.caller_id, count(fio.callee_id) as num_calls, sf.ninsns, sf.name from syscalls_made as sm "
                         " join semantic_fio_calls as fio on fio.callee_id = sm.caller "
                         " join semantic_functions as sf on sf.id=fio.caller_id "
                         " group by fio.caller_id, sf.ninsns, sf.name");
    {
        //all functions that is not a stub function for a dynamic library call
        std::cout << "\n\n\n################# COMPARING ALL FUNCTIONS \n\n";
	
        int num_functions    = tx->statement("select count(*) from functions_rg_accumulate where name NOT LIKE '%@plt'")
                               ->execute_int();
        int num_cg_syscalls  = tx->statement("select count(*) from syscalls_cg_accumulate where name NOT LIKE '%@plt'; ")
                               ->execute_int();
        int num_rg_syscalls  = tx->statement("select count(*) from syscalls_rg_accumulate where name NOT LIKE '%@plt'; ")
                               ->execute_int();
        int path_calls       = tx->statement("select count(distinct fio.caller_id) from syscalls_made as sm"
                                             " join semantic_fio_calls as fio on fio.callee_id = sm.caller"
                                             " join semantic_functions as sf on fio.caller_id=sf.id"
                                             " where sf.name not like '%@plt'")
                               ->execute_int();
        int total_num_functions = tx->statement("select count(*) from semantic_functions where name not like '%@plt'")
                                  ->execute_int();

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "num functions with calls:        " << num_functions   << std::endl;
        std::cout << "num functions:                   " << total_num_functions   << std::endl;

        std::cout << "num callgraph syscalls:          " << num_cg_syscalls
                  << " fraction " << 100*((double) num_cg_syscalls/num_functions) << std::endl;
        std::cout << "num reachability graph syscalls: " << num_rg_syscalls
                  << " fraction " << 100*((double) num_rg_syscalls/num_functions) << std::endl;
        std::cout << "path calls:                      " << path_calls
                  << " fraction " << 100*((double) path_calls/num_functions)      << std::endl;
    }

    {
        std::cout << "\n\n\n################# COMPARING FUNCTIONS WITH CALLS\n\n";
        int num_functions    = tx->statement("select count(distinct caller)"
                                             " from functions_rg_accumulate where name not like '%@plt'")->execute_int();
        int num_cg_syscalls  = tx->statement("select count(distinct caller) from syscalls_cg_accumulate"
                                             " where name not like '%@plt'; ")->execute_int();
        int num_rg_syscalls  = tx->statement("select count(distinct caller) from syscalls_rg_accumulate"
                                             " where name not like '%@plt'; ")->execute_int();
        int path_calls       = tx->statement("select count(distinct fio.caller_id) from semantic_fio_calls as fio"
                                             " join syscalls_made as sm on fio.callee_id = sm.caller"
                                             " join semantic_functions as sf on sf.id=fio.caller_id"
                                             " where sf.name not like '%@plt'")->execute_int();
        int total_num_functions = tx->statement("select count(*) from semantic_functions"
                                                " where name not like '%@plt'")->execute_int();

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "num functions with calls:         " << num_functions   << std::endl;
        std::cout << "num functions:                    " << total_num_functions   << std::endl;
        std::cout << "fraction of functions with calls: " << 100.0*num_functions/total_num_functions << std::endl;
        std::cout << "num callgraph syscalls:           " << num_cg_syscalls
                  << " fraction " << 100*((double) num_cg_syscalls/num_functions) << std::endl;
        std::cout << "num reachability graph syscalls:  "  << num_rg_syscalls
                  << " fraction " << 100*((double) num_rg_syscalls/num_functions) << std::endl;
        std::cout << "path calls:                       " << path_calls
                  << " fraction " << 100*((double) path_calls/num_functions)      << std::endl;
    }

    {
        std::cout << "\n\n\n################# COMPARING FUNCTIONS WITH CALLS WITH MORE THAN 100 INSTRUCTIONS\n\n";
        int num_functions    = tx->statement("select count(distinct fr.caller) from functions_rg_accumulate as fr"
                                             " join semantic_functions as sf on sf.id=fr.caller"
                                             " where sf.name not like '%@plt' and sf.ninsns >=100")->execute_int();
        int num_cg_syscalls  = tx->statement("select count(distinct sc.caller) from syscalls_cg_accumulate as sc"
                                             " join semantic_functions as sf on sf.id=sc.caller"
                                             " where sf.name not like '%@plt' and sf.ninsns >=100")->execute_int();
        int num_rg_syscalls  = tx->statement("select count(distinct sr.caller) from syscalls_rg_accumulate as sr"
                                             " join semantic_functions as sf on sf.id=sr.caller"
                                             " where sf.name NOT LIKE '%@plt' and sf.ninsns >= 100")->execute_int();
        int path_calls       = tx->statement("select count(distinct fio.caller_id) from semantic_fio_calls as fio"
                                             " join syscalls_made as sm on fio.callee_id = sm.caller"
                                             " join semantic_functions as sf on sf.id=fio.caller_id"
                                             " where sf.name not like '%@plt' and sf.ninsns >= 100")->execute_int();
        int total_num_functions = tx->statement("select count(*) from semantic_functions"
                                                " where ninsns >= 100 and name not like '%@plt'")->execute_int();

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "num functions with calls:         " << num_functions   << std::endl;
        std::cout << "num functions:                    " << total_num_functions   << std::endl;
        std::cout << "fraction of functions with calls: " << 100.0*num_functions/total_num_functions << std::endl;
        std::cout << "num callgraph syscalls:           " << num_cg_syscalls
                  << " fraction " << 100*((double) num_cg_syscalls/num_functions) << std::endl;
        std::cout << "num reachability graph syscalls:  " << num_rg_syscalls
                  << " fraction " << 100*((double) num_rg_syscalls/num_functions) << std::endl;
        std::cout << "path calls:                       " << path_calls
                  << " fraction " << 100*((double) path_calls/num_functions)      << std::endl;
    }

    tx->statement("drop table IF EXISTS syscall_statistics;");
    tx->statement("create table syscall_statistics as select distinct rg.caller, sm.syscall_id, sm.syscall_name"
                  " from syscalls_made as sm"
                  " join semantic_rg as rg on rg.callee = sm.caller");
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
    bool link = false;
    std::vector<std::string> signature_components;

    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        std::cout << argv[argno] << std::endl;
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--link")) {
            link = true;
        } else if (!strcmp(argv[argno], "--no-link")) {
            link = false;
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+2!=argc)
        usage(1);

    std::string db_name(argv[argno++]);
    std::cout << "Connecting to db:" << db_name << std::endl;
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(db_name);
    transaction = conn->transaction();

    transaction->execute("drop table if exists syscalls_made;");
    transaction->execute("create table syscalls_made (caller integer references semantic_functions(id),"
                         " syscall_id integer, syscall_name text)");

    std::cout << "database name is : " << std::string(argv[argno]) << std::endl;
    std::string specimen_name = argv[argno++];

    // Parse the binary specimen
    SgAsmInterpretation *interp = open_specimen(specimen_name, argv0, link);
    assert(interp!=NULL);

    // Figure out what functions need to be added to the database.
    std::vector<SgAsmFunction*> all_functions = SageInterface::querySubTree<SgAsmFunction>(interp);
    DirectedGraph* G = create_reachability_graph(all_functions, interp);
    add_calls_to_syscalls_to_db(transaction, G, all_functions);
    analyze_data(transaction);
    transaction->commit();
    return 0;
}

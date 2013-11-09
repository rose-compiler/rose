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

typedef boost::adjacency_list< boost::vecS, 
        boost::vecS, 
        boost::directedS > DirectedGraph; 

using namespace CloneDetection;
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE SPECIMEN\n"
              <<"  This command parses and disassembles the specimen executable, selects functions that calls a system call \n"
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


/*******************************************************************************************************
  * Add edges to graph from functions that call system calls to system calls.
  * The first 1000 vertexes (0 to 999) in the graph is reserved for system calls, which is many more
  * than the actual system calls in linux.
  */
void
add_syscall_edges(DirectedGraph* G, std::vector<SgAsmFunction*>& all_functions)
{
  //Detect all system calls and add an edge from the function to the function to the system call

  for (unsigned int caller_id = 0; caller_id < all_functions.size(); ++caller_id) {
    SgAsmFunction *func = all_functions[caller_id];

    std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(func);

    for(std::vector<SgAsmInstruction*>::iterator inst_it; inst_it != insns.end(); ++inst_it)
    {
      SgAsmx86Instruction *insn = isSgAsmx86Instruction(*inst_it);

      SgAsmBlock *block = SageInterface::getEnclosingNode<SgAsmBlock>(insn);

      //On linux system calls are always interrups and all interrupts are system calls
      if (insn && block && insn->get_kind()==x86_int) {

        const SgAsmExpressionPtrList &opand_list = insn->get_operandList()->get_operands();
        SgAsmExpression *expr = opand_list.size()==1 ? opand_list[0] : NULL;

        //semantically execute the basic block to find out which sytem call was called
        if (expr && expr->variantT()==V_SgAsmByteValueExpression &&
            0x80==SageInterface::getAsmConstant(isSgAsmValueExpression(expr))) {

          const SgAsmStatementPtrList &stmts = block->get_statementList();
          size_t int_n;
          for (int_n=0; int_n<stmts.size(); int_n++)
            if (isSgAsmInstruction(stmts[int_n])==insn)
              break;

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


/*******************************************************************************************************
  * Add edges from functions with the comilation unit to other functions within the compilation unit.
  * The vertex number in the graph corresponding to a function is the index within all_functions+1000, since
  * the first 1000 vertexes in the graph is reserved for system calls.
  *
  * The purpose of adding call edges is to detect when a function calls a function that calls a system call.
  */
void
add_call_edges_within_compilation_unit( DirectedGraph* G, 
    std::vector<SgAsmFunction*>& all_functions, SgAsmInterpretation *interp )
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

    add_edge(caller_id, callee_id, *G);

  }

}

DirectedGraph* 
create_reachability_graph( std::vector<SgAsmFunction*>& all_functions, SgAsmInterpretation *interp )
{
  //size of graph is the size of all functions + ids reserved for sytem calls
  DirectedGraph* G = new DirectedGraph( ids_reserved_for_syscalls+all_functions.size() );

  add_syscall_edges(G, all_functions);
  add_call_edges_within_compilation_unit(G, all_functions, interp);

  return G;
}

void
add_calls_to_syscalls_to_db(SqlDatabase::TransactionPtr tx, DirectedGraph* G, std::vector<SgAsmFunction*> all_functions)
{
  DirectedGraph& graph = *G;
  //Create a helper data structure for locating id in all_functions from a function pointer
  std::map<SgAsmFunction*, int> funcToId;

  for (unsigned int i = 0; i < all_functions.size(); ++i) {
    SgAsmFunction *func = all_functions[i];
    funcToId[func] = i;
  } 

  SqlDatabase::StatementPtr stmt = tx->statement("insert into syscalls_made(caller_name, syscall_id, syscall_name) values(?,?,?)");

  //Iterate over all components of the reachability graph

  typedef graph_traits<DirectedGraph>::vertex_descriptor Vertex;


  graph_traits<DirectedGraph>::vertex_iterator i, end;
  for(tie(i, end) = vertices(graph); i != end; ++i) {

    std::set<int> syscalls;
    std::set<int> func_calls;

    // Iterate through the child vertex indices for [current_index]

    std::vector<Vertex> reachable; 

    boost::breadth_first_search(graph, *i, 
        boost::visitor( 
          boost::make_bfs_visitor( 
            boost::write_property( 
              boost::identity_property_map(), 
              std::back_inserter(reachable), 
              boost::on_discover_vertex())))); 

    for(std::vector<Vertex>::iterator it = reachable.begin(); it != reachable.end(); ++it ){
      if(*it < ids_reserved_for_syscalls)
        syscalls.insert(*i);
      else
        func_calls.insert(*i);
    }

    if(func_calls.size() > 0 && syscalls.size() > 0 )
    {
      for(std::set<int>::iterator fit = func_calls.begin(); fit != func_calls.end(); ++fit)
      {
        for(std::set<int>::iterator sit = syscalls.begin(); sit != syscalls.end(); ++sit )
        {
          int caller_id = *fit;
          SgAsmFunction* caller = all_functions[caller_id];

          int syscall_callee_id = *sit;
          extern std::map<int, std::string> linux32_syscalls; // defined in linux_syscalls.C

          const std::string &syscall_name = linux32_syscalls[syscall_callee_id];

          stmt->bind(0, caller->get_name() /* symbol name for the caller */);
          stmt->bind(1, syscall_callee_id);
          stmt->bind(2, syscall_name);

          stmt->execute();

        }

      }

    }

  }
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

    transaction->execute("drop table IF EXISTS syscalls_made;");
    transaction->execute("create table syscalls_made( caller_name text, syscall_id integer, syscall_name text );");


    std::cout << "database name is : " << std::string(argv[argno]) << std::endl;
    std::string specimen_name = argv[argno++];

    // Parse the binary specimen
    SgAsmInterpretation *interp = open_specimen(specimen_name, argv0, link);
    assert(interp!=NULL);

    // Figure out what functions need to be added to the database.
    std::vector<SgAsmFunction*> all_functions = SageInterface::querySubTree<SgAsmFunction>(interp);

    DirectedGraph* G = create_reachability_graph( all_functions, interp );

    add_calls_to_syscalls_to_db(transaction, G, all_functions);

    transaction->commit();
    return 0;
}

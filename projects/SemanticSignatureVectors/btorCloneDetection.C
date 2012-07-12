/* Reads a binary file and disassembles according to command-line switches */

#include "rose.h"
#include "x86InstructionSemantics.h"
#include "x86AssemblyToBtor.h"
#include "BtorFunctionPolicy.h"
#include "integerOps.h"
#include <cassert>
#include <cstdio>
#include <boost/static_assert.hpp>
#include "btorProblem.h"
#include "x86AssemblyToNetlist.h" // Mostly for BMCError and numBmcErrors

using namespace std;
using namespace IntegerOps;
using boost::array;


struct HLTHooks: public BtorTranslationHooks {
  void hlt(BtorTranslationPolicy* policy) {
    policy->registerMap.errorFlag[bmc_error_program_failure] = policy->problem.build_op_redor(policy->readGPR(x86_gpr_bx));
  }
  void startInstruction(BtorTranslationPolicy* policy, SgAsmx86Instruction* insn) {
  }
  void finishInstruction(BtorTranslationPolicy* policy, SgAsmx86Instruction* insn) {
  }
};

#undef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "constructCFG.h"




std::string btorTranslateFunction(BtorTranslationPolicy& policy, SgAsmNode* func, FILE* outfile, bool initialConditionsAreUnknown, bool bogusIpIsError, SgAsmGenericHeader* header) {
  rose_addr_t entryPoint = header->get_entry_rva() + header->get_base_va();

  X86InstructionSemantics<BtorTranslationPolicy, BtorWordType> t(policy);
  std::vector<SgNode*> instructions = NodeQuery::querySubTree(func, V_SgAsmx86Instruction);

  for (size_t i = 0; i < instructions.size(); ++i) {
    SgAsmx86Instruction* insn = isSgAsmx86Instruction(instructions[i]);
    ROSE_ASSERT (insn);
    string s = unparseInstructionWithAddress(insn);
    fprintf(outfile, "\n%s\n", s.c_str());

    try {
        t.processInstruction(insn);
    } catch (const X86InstructionSemantics<BtorTranslationPolicy, BtorWordType>::Exception &e) {
        fprintf(stderr, "%s: %s\n", e.mesg.c_str(), unparseInstructionWithAddress(e.insn).c_str());
    }
  }
  policy.setInitialState(entryPoint, initialConditionsAreUnknown);
  policy.addNexts();

  return policy.problem.unparse();
}

string btorOpToString(BtorOperator op) {
  assert ((size_t)op < sizeof(btorOperators) / sizeof(*btorOperators));
  assert (btorOperators[(size_t)op].op == op);
  return btorOperators[(size_t)op].name;
}
/*********************************
 * Create a DDG for the operations side-effecting memory and registers
 * in the subtree of a BtorComputation 
************************************/
SgGraphNode* createDDG( SgIncidenceDirectedGraph* ddg, const BtorComputationPtr& bPtr, map< boost::shared_ptr<BtorComputation>, SgGraphNode* >& existingNodes)
{
  boost::shared_ptr<BtorComputation> p = bPtr.p;


  //Find or create node for current BtorComputation
  SgGraphNode* from_node; 

  map< boost::shared_ptr<BtorComputation>, SgGraphNode* >::iterator nodeItr =
    existingNodes.find(p);
  if( nodeItr == existingNodes.end()  )
  {
    from_node = new SgGraphNode( btorOpToString(p->op) );

    ddg->addNode(from_node);

    existingNodes[p] = from_node;

  }else
    return nodeItr->second;

  //Create edges for the current graph
  for (size_t k = 0; k < p->operands.size(); ++k)
  {
    BtorComputationPtr& n_bPtr = p->operands[k];


    //Create or find node for current BtorComputation
    map< boost::shared_ptr<BtorComputation>, SgGraphNode* >::iterator nodeItr =
      existingNodes.find(n_bPtr.p);

    //Create graph for subgraph recursively
    SgGraphNode* to_node = createDDG(ddg, n_bPtr, existingNodes );

    //Add edge to graph
    ddg->addDirectedEdge( from_node, to_node  , "");
  }

  return from_node;


};

/*********************************
 * Given a policy create a DDG for all operations that has
 * side effected the registers
 ********************************/
SgIncidenceDirectedGraph* createDDG(BtorFunctionPolicy& policy)
{
  SgIncidenceDirectedGraph* ddg = new SgIncidenceDirectedGraph();

  BtorProblem& problem = policy.problem;

  map< boost::shared_ptr<BtorComputation>, SgGraphNode* > existingNodes;

  //Add the subgraph of all registers
  for (vector<BtorComputationPtr>::const_iterator j = problem.computations.begin(); j != problem.computations.end(); ++j) {
    //std::cout << j->p.get();
    createDDG(ddg, *j, existingNodes );
    //   std::cout << std::endl;
  }



  return ddg;

};



int
main(int argc, char *argv[]) 
{
    unsigned search = Disassembler::SEARCH_DEFAULT;
    bool show_bad = false;
    bool do_debug = false;
    bool do_reassemble = false;
    int exit_status = 0;

    char **new_argv = (char**)calloc(argc+2, sizeof(char*));
    int new_argc=0;
    new_argv[new_argc++] = argv[0];
    new_argv[new_argc++] = "-rose:read_executable_file_format_only";
    for (int i=1; i<argc; i++) {
        if (!strcmp(argv[i], "--search-following")) {
            search |= Disassembler::SEARCH_FOLLOWING;
        } else if (!strcmp(argv[i], "--no-search-following")) {
            search &= ~Disassembler::SEARCH_FOLLOWING;
        } else if (!strcmp(argv[i], "--search-immediate")) {
            search |= Disassembler::SEARCH_IMMEDIATE;
        } else if (!strcmp(argv[i], "--no-search-immediate")) {
            search &= ~Disassembler::SEARCH_IMMEDIATE;
        } else if (!strcmp(argv[i], "--search-words")) {
            search |= Disassembler::SEARCH_WORDS;
        } else if (!strcmp(argv[i], "--no-search-words")) {
            search &= ~Disassembler::SEARCH_WORDS;
        } else if (!strcmp(argv[i], "--search-allbytes")) {
            search |= Disassembler::SEARCH_ALLBYTES;
        } else if (!strcmp(argv[i], "--no-search-allbytes")) {
            search &= ~Disassembler::SEARCH_ALLBYTES;
        } else if (!strcmp(argv[i], "--search-unused")) {
            search |= Disassembler::SEARCH_UNUSED;
        } else if (!strcmp(argv[i], "--no-search-unused")) {
            search &= ~Disassembler::SEARCH_UNUSED;
        } else if (!strcmp(argv[i], "--search-nonexe")) {
            search |= Disassembler::SEARCH_NONEXE;
        } else if (!strcmp(argv[i], "--no-search-nonexe")) {
            search &= ~Disassembler::SEARCH_NONEXE;
        } else if (!strcmp(argv[i], "--search-deadend")) {
            search |= Disassembler::SEARCH_DEADEND;
        } else if (!strcmp(argv[i], "--no-search-deadend")) {
            search &= ~Disassembler::SEARCH_DEADEND;
        } else if (!strcmp(argv[i], "--search-unknown")) {
            search |= Disassembler::SEARCH_UNKNOWN;
        } else if (!strcmp(argv[i], "--no-search-unknown")) {
            search &= ~Disassembler::SEARCH_UNKNOWN;
        } else if (!strcmp(argv[i], "--show-bad")) {
            show_bad = true;
        } else if (!strcmp(argv[i], "--reassemble")) {
            do_reassemble = true;
        } else if (!strcmp(argv[i], "--debug")) {
            do_debug = true;
        } else {
            new_argv[new_argc++] = argv[i];
        }
    }
    
    /* The -rose:read_executable_file_format_only causes a prominent warning to be displayed:
     *   >
     *   >WARNING: Skipping instruction disassembly
     *   >
     */
    SgProject *project = frontend(new_argc, new_argv);
    printf("No, please ignore the previous warning; Rest assured, we're doing disassembly!\n\n");


    /* Process each interpretation individually */
    std::vector<SgNode*> interps = NodeQuery::querySubTree(project, V_SgAsmInterpretation);
    assert(interps.size()>0);
    for (size_t i=0; i<interps.size(); i++) {
        SgAsmInterpretation *interp = isSgAsmInterpretation(interps[i]);

        /* Disassemble and partitioner instructions into basic blocks and functions. The partitioner analyses the control flow
         * graph (CFG) in order to discover functions and we find it convenient to use the same machinery to produce a
         * SgIncidenceDirectedGraph below. So we'll provide our own partitioner object (rather than have the disassembler
         * create a temporary one) so we can obtain its CFG after the disassembler is finished. */
        Disassembler *d = Disassembler::lookup(interp)->clone(); /*cloned so we can make local changes*/
        if (do_debug)
            d->set_debug(stderr);
        d->set_search(search);
        Partitioner *part = new Partitioner;
        d->set_partitioner(part);
        Disassembler::InstructionMap instMap = d->disassembleInterp(interp);
        

        /* Construct the SgIncidenceDirectedGraph CFG */
        Partitioner::BasicBlockStarts bb_starts = part->detectBasicBlocks(instMap); /*DEPRECATED*/
        Partitioner::FunctionStarts func_starts = part->detectFunctions(interp, instMap, bb_starts); /*DEPRECATED*/
        SgAsmBlock *blocks = part->build_ast();
        SgIncidenceDirectedGraph* cfg = constructCFG_BB(blocks, bb_starts, instMap);




        AstDOTGeneration dotgen;

        dotgen.writeIncidenceGraphToDOTFile(cfg, "cfg_x86.dot");


        FILE* f = fopen("foo.btor", "w");

        for( unsigned int i = 0 ; i < blocks->get_statementList().size(); i++   )
        {
          SgAsmFunction* funcDecl = isSgAsmFunction(blocks->get_statementList()[i]);


          SgIncidenceDirectedGraph* intraProceduralCFG = constructCFG_BB(funcDecl, bb_starts, instMap);
          std::vector<SgGraphNode*> roots = findGraphRoots(intraProceduralCFG);

          {
            AstDOTGeneration dotgen2;

            dotgen2.writeIncidenceGraphToDOTFile(intraProceduralCFG, "cfg_x86_"+ funcDecl->get_name()+ ".dot");

          }

          std::set< std::vector<SgAsmx86Instruction*> > staticTraces;

          
          {
           std::vector<SgNode*> nodesInSubTree = NodeQuery::querySubTree( funcDecl, V_SgAsmx86Instruction );

            staticTraces = generateStaticTraces(intraProceduralCFG,  nodesInSubTree.size() );


            int count =0;
            for( std::set< std::vector<SgAsmx86Instruction*> >::iterator traceItr = staticTraces.begin();
                traceItr != staticTraces.end(); ++traceItr )
            {
              count++;


              BtorFunctionPolicy policy;

              string s = btorTranslate(policy, *traceItr);

              //Create data dependency graph
              SgIncidenceDirectedGraph* ddg = createDDG( policy );

              AstDOTGeneration dotgen2;

              dotgen2.writeIncidenceGraphToDOTFile(ddg, "ddg_x86_"+ funcDecl->get_name()+"_" + boost::lexical_cast<std::string>(count) + ".dot");

              { // Output x86
                FILE* fTrace = fopen( string("ddg_x86_"+ funcDecl->get_name()+"_" + boost::lexical_cast<std::string>(count) + ".out").c_str() , "w");

                const std::vector<SgAsmx86Instruction*>& curTrace = *traceItr;

                for(std::vector<SgAsmx86Instruction*>::const_iterator iItr = curTrace.begin(); iItr != curTrace.end(); ++iItr )
                  fprintf(fTrace, "%s\n", unparseInstructionWithAddress(*iItr).c_str() );

                fclose(fTrace);

              }

              { // Output BTOR
                FILE* fTrace = fopen( string("ddg_x86_"+ funcDecl->get_name()+"_" + boost::lexical_cast<std::string>(count) + ".btor.out").c_str() , "w");
                fprintf(fTrace, "%s\n", s.c_str() );
                fclose(fTrace);
              }

            }
          }

          ROSE_ASSERT( funcDecl != NULL );
          //std::cout << "The function:" <<  funcDecl->get_name() << std::endl; 
          fprintf(f, "\n\n%s\n", string("The function" + funcDecl->get_name()).c_str() );
          assert (f);
          HLTHooks hooks;
          BtorFunctionPolicy policy;


          //string s = btorTranslateFunction(policy, funcDecl, f, initialConditionsAreUnknown, true,
          //    interp->get_header());

          std::vector<SgNode*> instructions = NodeQuery::querySubTree(funcDecl, V_SgAsmx86Instruction);

          std::vector<SgAsmx86Instruction*> insts; 
          for(unsigned int k = 0 ;  k < instructions.size() ; k++ )
          {
            insts.push_back(isSgAsmx86Instruction(instructions[k]) );

            std::cout << "The class name " << instructions[k]->get_parent()->class_name()  << " " <<  instructions[k]->get_parent()->get_parent()->class_name() 
                      << " " << ( isSgAsmBlock(instructions[k]->get_parent()) ?  isSgAsmBlock(instructions[k]->get_parent())->get_address() : 0 ) << std::endl;
            ROSE_ASSERT(   isSgAsmx86Instruction(instructions[k]) != NULL );
          }


          fprintf(f, " we have %d instructions",(int) insts.size());

          string s = btorTranslate(policy, insts);

          fprintf(f, "%s", s.c_str());


          BtorProblem& problem = policy.problem;


          for (vector<BtorComputationPtr>::const_iterator j = problem.computations.begin(); j != problem.computations.end(); ++j) {
            std::cout << j->p.get();
            for (size_t k = 0; k < j->p->operands.size(); ++k) {
              BtorComputationPtr& op = j->p->operands[k];

              std::cout << " " << op.p.get();
            }
            std::cout << std::endl;
          }

#if 0
          for( unsigned int j = 0 ; j < funcDecl->get_statementList().size() ; j++ )
          {
            std::cout << "  block:" <<  funcDecl->get_statementList()[j]->class_name() << std::endl; 
          };
#endif


        }
        fclose(f);


    }

    printf("running back end...\n");
    int ecode = backend(project);
    return ecode>0 ? ecode : exit_status;
}

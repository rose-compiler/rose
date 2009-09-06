/* Reads a binary file and disassembles according to command-line switches */

#include "rose.h"

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

    t.processInstruction(insn);

  }
  policy.setInitialState(entryPoint, initialConditionsAreUnknown);
  policy.addNexts();

  return policy.problem.unparse();
}



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

        /* Build the disassembler */
        Disassembler *d = Disassembler::create(interp);
        if (do_debug)
            d->set_debug(stderr);
        d->set_search(search);

        /* RPM: you don't actually need to pass "bad", which returns the addresses/error messages for instructions that
         *      couldn't be disassembled.  Just d->disassembleInterp(interp). */
        /* Disassemble instructions, linking them into the interpretation */
        Disassembler::BadMap bad;
        Disassembler::InstructionMap instMap = d->disassembleInterp(interp, NULL, &bad);


        /* Create the CFG */
        Partitioner part; 

        /* RPM: BasicBlockStarts is contains the callers, not the callees.  It's a map where the keys (first element of each
         *      pair) are the address of the first instruction of each basic block and the values (second element of each
         *      pair) are the virtual addresses of instructions that branch to that basic block.  For instance, if you have
         *          0x3000: JMP 0x5000
         *                  ...
         *          0x4000: CALL 0x5000
         *                  ...
         *          0x5000: NOP
         *      then the BasicBlockStarts will contain the pair (0x5000, [0x3000, 0x4000]). */
        //BasicBlockStarts is a basic block together with a set of know callees

        
        Partitioner::BasicBlockStarts bb_starts = part.detectBasicBlocks(instMap); //The CFG 
        Partitioner::FunctionStarts func_starts = part.detectFunctions(interp->get_header(), instMap, bb_starts);
        

       // SgAsmBlock* blocks = part.partition(interp->get_header(), instMap);

#if 0
        { // Create cg
        SgIncidenceDirectedGraph* cg = constructCallGraph(func_starts,bb_starts,instMap);

        AstDOTGeneration dotgen;
        dotgen.writeIncidenceGraphToDOTFile(cg, "cg_x86.dot");
        }


        { // Create cfg
        SgIncidenceDirectedGraph* cfg = constructCFG(func_starts, bb_starts,instMap);

        AstDOTGeneration dotgen;
        dotgen.writeIncidenceGraphToDOTFile(cfg, "cfg_x86.dot");
        }
#endif

        SgAsmBlock* blocks = part.buildTree(instMap, bb_starts, func_starts);
        //SgAsmBlock* blocks = part.partition(interp->get_header(),instMap);
        //frontend/Disassemblers/Partitioner.h:    typedef std::map<rose_addr_t, std::set<rose_addr_t> > BasicBlockStarts;
        SgIncidenceDirectedGraph* cfg = constructCFG_BB(blocks, bb_starts,instMap);

        AstDOTGeneration dotgen;

        dotgen.writeIncidenceGraphToDOTFile(cfg, "cfg_x86.dot");


        FILE* f = fopen("foo.btor", "w");

        for( unsigned int i = 0 ; i < blocks->get_statementList().size(); i++   )
        {
          SgAsmFunctionDeclaration* funcDecl = isSgAsmFunctionDeclaration(blocks->get_statementList()[i]);


          SgIncidenceDirectedGraph* intraProceduralCFG = constructCFG_BB(funcDecl, bb_starts, instMap);
          std::vector<SgGraphNode*> roots = findGraphRoots(intraProceduralCFG);


          std::set< std::vector<SgAsmx86Instruction*> > staticTraces;

          rose_graph_integer_node_hash_map & nodes =
          intraProceduralCFG->get_node_index_to_node_map ();

          {
            std::vector<SgNode*> nodesInSubTree = NodeQuery::querySubTree( funcDecl, V_SgAsmx86Instruction );

            staticTraces = generateStaticTraces(intraProceduralCFG,  nodesInSubTree.size() );

            for( std::set< std::vector<SgAsmx86Instruction*> >::iterator traceItr = staticTraces.begin();
                traceItr != staticTraces.end(); ++traceItr )
            {

              BtorFunctionPolicy policy;

              string s = btorTranslate(policy, *traceItr);





            }
          }

          ROSE_ASSERT( funcDecl != NULL );
          //std::cout << "The function:" <<  funcDecl->get_name() << std::endl; 
          fprintf(f, "\n\n%s\n", string("The function" + funcDecl->get_name()).c_str() );
          assert (f);
          HLTHooks hooks;
          BtorFunctionPolicy policy;


          bool initialConditionsAreUnknown = false;  // Start of execution
          //string s = btorTranslateFunction(policy, funcDecl, f, initialConditionsAreUnknown, true,
          //    interp->get_header());

          std::vector<SgNode*> instructions = NodeQuery::querySubTree(funcDecl, V_SgAsmx86Instruction);

          std::vector<SgAsmx86Instruction*> insts; 
          for(unsigned int k = 0 ;  k < instructions.size() ; k++ )
          {
            insts.push_back(isSgAsmx86Instruction(instructions[k]) );

            std::cout << "The class name " << instructions[k]->get_parent()->class_name()  << " " <<  instructions[k]->get_parent()->get_parent()->class_name() 
                      << " " << ( isSgAsmBlock(instructions[k]->get_parent()) ?  isSgAsmBlock(instructions[k]->get_parent())->get_address() : NULL ) << std::endl;
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

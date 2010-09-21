/* Reads a binary file and disassembles according to command-line switches */

#include "rose.h"
#include <cstdio>

#include "binary_analysis.h"

using namespace std;



int
main(int argc, char *argv[]) 
{

    char **new_argv = (char**)calloc(argc+2, sizeof(char*));
    int new_argc=0;
    new_argv[new_argc++] = argv[0];
    new_argv[new_argc++] = strdup("-rose:read_executable_file_format_only");
    for (int i=1; i<argc; i++) {
      new_argv[new_argc++] = argv[i];
    }
    
    SgProject *project = frontend(new_argc, new_argv);

    /* Process each interpretation individually */
    std::vector<SgNode*> interps = NodeQuery::querySubTree(project, V_SgAsmInterpretation);
    assert(interps.size()>0);
    for (size_t i=0; i<interps.size(); i++) {
        SgAsmInterpretation *interp = isSgAsmInterpretation(interps[i]);
        { // Create cg
        SgIncidenceDirectedGraph* cg = constructCallGraph(interp);

        AstDOTGeneration dotgen;
        dotgen.writeIncidenceGraphToDOTFile(cg, "cg_x86.dot");
        }
#if 0


        { // Create cfg
        SgIncidenceDirectedGraph* cfg = constructCFG(func_starts, bb_starts,instMap);

        AstDOTGeneration dotgen;
        dotgen.writeIncidenceGraphToDOTFile(cfg, "cfg_x86.dot");
        }
#endif


    }

    printf("running back end...\n");
    return backend(project);
}

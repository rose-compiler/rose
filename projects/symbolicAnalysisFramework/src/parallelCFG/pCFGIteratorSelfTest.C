#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

// dataflow analysis includes
#include "genericDataflowCommon.h"
#include "variables.h"
#include "analysisCommon.h"
#include "functionState.h"
#include "latticeFull.h"
#include "analysis.h"
#include "dataflow.h"
#include "divAnalysis.h"
#include "ConstrGraphAnalysis.h"

// pcfg includes
#include "pCFG.h"
#include "MPIDepAnalysis.h"
#include "pCFGAnnotations.h"
#include "pCFGIterator.h"


int main(int argc, char* argv[])
{
    printf("================ S T A R T =================\n");

    SgProject* project = frontend(argc, argv);
    initAnalysis(project);
    Dbg::init("PCFGIterator",".","index.html");

    PragmaParse pragmaparser;
    pragmaparser.traverseInputFiles(project, preorder);

    // PragmaParseTest pragmaparsetest;
    // pragmaparsetest.traverseInputFiles(project, preorder);

    analysisDebugLevel = 2;

    LiveDeadVarsAnalysis ldva(project);
    UnstructuredPassInterDataflow upid_ldva(&ldva);
    upid_ldva.runAnalysis();

    MPIDepAnalysis mda(&ldva);
    UnstructuredPassInterDataflow upid_mda(&mda);
    upid_mda.runAnalysis();

    pCFGIterator pcfgiterator(&ldva, &mda);
    UnstructuredPassInterDataflow upid_pcfg(&pcfgiterator);
    upid_pcfg.runAnalysis();
    ostringstream filename;
    filename << (project->get_sourceFileNameList())[0] << "_pcfg.dot";    
    pcfgiterator.writeToDot(filename.str());
    
    return 0;
}

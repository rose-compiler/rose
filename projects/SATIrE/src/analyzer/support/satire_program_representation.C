#include "satire.h"

namespace SATIrE
{

Program::Program(AnalyzerOptions *o)
  : options(o), astRoot(createRoseAst(o)), icfg(NULL), prologTerm(NULL)
{
    analysisScheduler.registerProgram(this);
 // Run any provided analyzers that were specified on the command line.
 // Logically, this is part of program construction because the user asked
 // for an annotated program.
    const std::vector<DataFlowAnalysis *> &analyzers
        = o->getDataFlowAnalyzers();
    std::vector<DataFlowAnalysis *>::const_iterator a;
    for (a = analyzers.begin(); a != analyzers.end(); ++a)
        (*a)->run(this);
}

Program::~Program()
{
    analysisScheduler.unregisterProgram(this);
}

}

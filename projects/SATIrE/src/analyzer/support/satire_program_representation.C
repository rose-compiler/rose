#include "satire.h"

#include "o_Location.h"

namespace SATIrE
{

Program::Program(AnalyzerOptions *o)
  : options(o), astRoot(createRoseAst(o)), icfg(NULL), prologTerm(NULL)
{
    analysisScheduler.registerProgram(this);

    if (options->buildIcfg())
        icfg = createICFG(astRoot, options);

 // Run any provided analyzers that were specified on the command line.
 // Logically, this is part of program construction because the user asked
 // for an annotated program.
    const std::vector<DataFlowAnalysis *> &analyzers
        = o->getDataFlowAnalyzers();
    std::vector<DataFlowAnalysis *>::const_iterator a;
    for (a = analyzers.begin() ; a != analyzers.end(); ++a)
    {
      if (*a != NULL /* empty analyzer */) {
        (*a)->run(this);
        (*a)->processResultsWithPrefix(this, false);
      }
    }
}

Program::~Program()
{
    analysisScheduler.unregisterProgram(this);
}

}

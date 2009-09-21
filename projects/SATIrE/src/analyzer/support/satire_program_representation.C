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
    std::vector<DataFlowAnalysis *>::const_iterator a = analyzers.begin();
 // The first analyzer is special: It allows us to compute context
 // information and run the context-specific points-to analysis.
    if (a != analyzers.end() && (*a) != NULL /* empty analyzer */)
    {
        (*a)->run(this);
        a++;

#if HAVE_PAG
        icfg->contextInformation = new ContextInformation(icfg);
        if (o->runPointsToAnalysis())
        {
            icfg->contextSensitivePointsToAnalysis->run(icfg);
            if (o->outputPointsToGraph())
            {
                icfg->contextSensitivePointsToAnalysis->doDot(
                        o->getPointsToGraphName());
            }
            o_Location_power = icfg->contextSensitivePointsToAnalysis
                                    ->get_locations().size();
        }
#endif
    }
    for ( ; a != analyzers.end(); ++a)
    {
      if (*a != NULL /* empty analyzer */) {
        (*a)->run(this);
        (*a)->processResultsWithPrefix(this);
      }
    }
}

Program::~Program()
{
    analysisScheduler.unregisterProgram(this);
}

}

#include "satire.h"

#include "set_pag_options.h"

namespace SATIrE
{

// ---- Auxiliary function
static void
setPrefixedPagOptions(AnalyzerOptions *options,
                      DataFlowAnalysis::Implementation *p_impl)
{
    p_impl->setDebugStat(options->statistics());
    if (options->retFuncUsed())
        p_impl->setGlobalRetfunc(1);
}

// ---- DataFlowAnalysis methods
std::string
DataFlowAnalysis::identifier() const
{
    return p_impl->identifier();
}

std::string
DataFlowAnalysis::description() const
{
    return p_impl->description();
}

void DataFlowAnalysis::run(Program *program)
{
    AnalyzerOptions *options = program->options;
    bool verbose = options->verbose();

    /* Set the PAG options as specified on the command line. */
    setPagOptions(*options);
    setPrefixedPagOptions(options, p_impl);

    /* Build the program's ICFG if necessary. */
    if (program->icfg == NULL)
        program->icfg = createICFG(program->astRoot, options);

    /* Run this analysis. */
    if (verbose)
    {
        std::cout
            << "performing analysis " << identifier() << " ... "
            << std::flush;
    }
    TimingPerformance *nestedTimer
        = new TimingPerformance("Actual data-flow analysis "
                                + identifier() + ":");
    p_impl->analysisDoit(program->icfg);
    delete nestedTimer;
    if (verbose) std::cout << "done" << std::endl;

    /* Make results persistent. We always do this (by default) to avoid
     * problems with garbage collected results. */
    p_impl->makePersistent();
}

void
DataFlowAnalysis::processResults(Program *program)
{
    /* Output visual representations of the analysis results. */
    p_impl->outputAnalysisVisualization(program);

    /* Annotate the progam with analysis information according to the
     * command line options. */
    p_impl->annotateProgram(program);

    /* Print the program as annotated text or term if requested. */
    p_impl->outputAnnotatedProgram(program);
}

DataFlowAnalysis::DataFlowAnalysis(
        DataFlowAnalysis::Implementation *implementation)
  : p_impl(implementation)
{
}

DataFlowAnalysis::~DataFlowAnalysis()
{
}

}

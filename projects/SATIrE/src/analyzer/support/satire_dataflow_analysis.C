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

#if HAVE_PAG
    /* If requested, compute call strings from PAG's call string data, and
     * store them in the ICFG. */
    if (options->computeCallStrings())
        computeCallStrings(program);
#endif
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

#if HAVE_PAG
    /* Print call strings and other context data if requested. */
    outputContextData(program);
#endif
}

DataFlowAnalysis::DataFlowAnalysis(
        DataFlowAnalysis::Implementation *implementation)
  : p_impl(implementation)
{
}

DataFlowAnalysis::~DataFlowAnalysis()
{
}

// ---- private DataFlowAnalysis methods
#if HAVE_PAG
void
DataFlowAnalysis::computeCallStrings(Program *program) const
{
    CFG *icfg = program->icfg;
    if (icfg->contextInformation == NULL)
        icfg->contextInformation = new ContextInformation(icfg);
}


void
DataFlowAnalysis::outputContextData(Program *program) const
{
    CFG *icfg = program->icfg;
    if (program->options->outputCallStrings())
    {
        if (icfg->contextInformation == NULL)
        {
            std::cerr
                << "*** error: can't output context data, no data available!"
                << std::endl;
            std::abort();
        }

        icfg->contextInformation->print(std::cout);
    }
    if (program->options->outputContextGraph())
    {
        if (icfg->contextInformation == NULL)
        {
            std::cerr
                << "*** error: can't output context data, no data available!"
                << std::endl;
            std::abort();
        }

        std::ofstream outfile(
                program->options->getContextGraphFileName().c_str());
        if (!outfile)
        {
            std::cerr
                << "*** error: couldn't open output file "
                << program->options->getContextGraphFileName()
                << std::endl;
            std::abort();
        }
        icfg->contextInformation->printContextGraph(outfile);
    }
}
#endif

}

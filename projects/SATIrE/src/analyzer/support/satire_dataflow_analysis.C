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

    /* Print call strings if requested. */
    outputCallStrings(program);
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
void
DataFlowAnalysis::outputCallStrings(Program *program) const
{
    if (!program->options->outputCallStrings())
        return;

    CFG *icfg = program->icfg;
    int *callstring;
    int length;

 // sel_mapping is a global variable used by PAG; for now, we do not attempt
 // to interpret VIVU stuff, nor infinite callstrings.
    if (!(sel_mapping == PAG_MAPPING_CALLSTRING_0
       || sel_mapping == PAG_MAPPING_CALLSTRING_1
       || sel_mapping == PAG_MAPPING_CALLSTRING_N))
    {
        std::cerr
            << "*** SATIrE call string output error: unsupported mapping '"
            << sel_mapping << "'" << std::endl
            << "*** only finite-length non-VIVU call strings are supported"
            << std::endl;
        std::abort();
    }

 // Iterate over all procedures in the ICFG.
    int procs = kfg_num_procs(icfg);
    for (int p = 0; p < procs; p++)
    {
        char *procname = kfg_proc_name(icfg, p);
        std::cout
            << "----- call strings for procedure " << procname << " -----"
            << std::endl;
     // Get procedure's entry node as a representative node for the
     // procedure; we assume that all nodes in the procedure have the same
     // contexts as the entry, which should be a reasonable assumption.
        KFG_NODE entry = kfg_numproc(icfg, p);
        int arity = kfg_arity_id(kfg_get_id(icfg, entry));
        for (int pos = 0; pos < arity; pos++)
        {
            mapping_get_callstring(icfg, p, pos, &callstring, &length);
            std::cout << "position " << pos << ": ";
            outputCallString(program, callstring, length);
        }

     // Empty separator line for maximal prettiness.
        if (p+1 < procs)
            std::cout << std::endl;
    }
}

void
DataFlowAnalysis::outputCallString(
        Program *program, int *callstring, int length) const
{
    CFG *icfg = program->icfg;

    if (length > 0)
    {
        for (int i = 0; i < length; i++)
        {
            int nodeid = callstring[i];
            KFG_NODE node = kfg_get_node(icfg, nodeid);
            int procid = kfg_procnum(icfg, nodeid);
            char *procname = kfg_proc_name(icfg, procid);
            std::cout << procname << "/" << nodeid;
            if (i+1 < length)
                std::cout << "  ->  ";
        }
        std::cout << std::endl;
    }
    else if (length == 0)
    {
        std::cout << "spontaneous" << std::endl;
    }
    else
    {
        std::cerr
            << "error: negative callstring length!" << std::endl;
        std::abort();
    }
}

}

#include "satire.h"
#include "termite.h"

namespace SATIrE
{

AnalysisDriver::AnalysisDriver(int argc, char **argv)
  : options(extractOptions(argc, argv))
{
  init_termite(argc, argv);
}

AnalysisDriver::~AnalysisDriver()
{
}

AnalyzerOptions *
AnalysisDriver::getOptions() const
{
    return options;
}

void
AnalysisDriver::run(Analysis *analysis)
{
    TimingPerformance *timer
        = new TimingPerformance("Complete run of SATIrE analysis:");

    /* Build a program representation according to the command line. */
    Program *program = new Program(options);

    /* Perform the actual analysis on the AST. */
    TimingPerformance *nestedTimer
        = new TimingPerformance("Run of analysis " + analysis->identifier()
                              + " (with dependencies):");
    analysisScheduler.runAnalysisWithDependencies(analysis, program);
    delete nestedTimer;

    /* Perform post-processing according to the command line options:
     * program annotation, visualization, transformation, etc. This is a
     * virtual method that can be customized by overwriting it in a
     * subclass. */
    nestedTimer = new TimingPerformance("Postprocessing of analysis results:");
    processResults(analysis, program);
    delete nestedTimer;

    /* Output the program according to the command line options. This is a
     * virtual method that can be customized by overwriting it in a
     * subclass. */
    outputProgram(program);

    /* Clean up. */
    delete program;

    /* Report timing statistics if requested by the user. */
    delete timer;
    if (options->statistics())
        TimingPerformance::generateReport();
}

void
AnalysisDriver::processResults(Analysis *analysis, Program *program)
{
    /* By default, simply invoke the post-processing phase of the top level
     * analysis and let it take care of annotation, visualization etc.
     * according to its needs. */
    analysis->processResults(program);
}

void
AnalysisDriver::outputProgram(Program *program)
{
    /* By default, call the general output function. */
    outputProgramRepresentation(program, program->options);
}

}

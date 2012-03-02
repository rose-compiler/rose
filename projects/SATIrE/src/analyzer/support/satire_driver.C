// This is a small driver program for simple command-line access to many of
// the features of the SATIrE library.

#include <satire.h>

using namespace SATIrE;

int main(int argc, char **argv)
{
    AnalyzerOptions *options = extractOptions(argc, argv);
    Program *program = new Program(options);
    outputProgramRepresentation(program, options);
    if (options->statistics())
        TimingPerformance::generateReport();
}

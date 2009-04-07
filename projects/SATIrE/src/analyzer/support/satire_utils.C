#include "satire.h"

// Here we want to use the TermPrinter, but pretend that it is being used
// without PAG (regardless of whether PAG is actually supported locally)
// because then it insists on using the DFI printer, but we have no DFI to
// print here. So... we pretend that we are building without PAG.
#ifdef HAVE_PAG
#  define SATIRE_UTILS_HAVE_PAG_TMP HAVE_PAG
#  undef HAVE_PAG
#endif
#include "TermPrinter.h"
#ifdef SATIRE_UTILS_HAVE_PAG_TMP
#  define HAVE_PAG SATIRE_UTILS_HAVE_PAG_TMP
#  undef SATIRE_UTILS_HAVE_PAG_TMP
#endif

namespace SATIrE {

AnalyzerOptions *extractOptions(int argc, char **argv)
{
    AnalyzerOptions *options = new AnalyzerOptions();
    CommandLineParser clp;
    clp.parse(options, argc, argv);
    init_termite(argc, argv);
    return options;
}

SgProject *createRoseAst(AnalyzerOptions *options)
{
    SgProject *astRoot;

    /* Run frontend or binary AST input mechanism, depending on options. */
    if (!options->inputBinaryAst())
    {
        astRoot = frontend(options->getCommandLineNum(),
                           options->getCommandLineCarray());
    }
    else
    {
        astRoot = AST_FILE_IO::readASTFromFile(
                           options->getInputBinaryAstFileName());
    }

    /* Perform sanity checks on the AST if requested. */
    if (options->checkRoseAst())
        AstTests::runAllTests(astRoot);

    return astRoot;
}

CFG *createICFG(SgProject *astRoot, AnalyzerOptions *options)
{
    TimingPerformance *nestedTimer;
    bool verbose = options->verbose();

    /* Collect procedures. */
    nestedTimer = new TimingPerformance("SATIrE ICFG construction:");
    if (verbose) std::cout << "collecting functions ... " << std::flush;
    ProcTraversal s;
    s.setPrintCollectedFunctionNames(options->printCollectedFunctionNames());
    s.traverse(astRoot, preorder);
    if (verbose) std::cout << "done" << std::endl;

    /* Create the ICFG itself. */
    if (verbose) std::cout << "generating cfg ... " << std::flush;
    CFGTraversal t(s, options);
    if (!options->numberExpressions())
        t.numberExpressions(false);
    t.traverse(astRoot, preorder);
    if (verbose) std::cout << "done" << std::endl;
    delete nestedTimer;

    CFG *cfg = t.getCFG();

    /* Run ICFG checks if requested. */
    if (options->checkIcfg())
    {
        if (verbose) std::cout << "testing cfg ... " << std::flush;
        nestedTimer = new TimingPerformance("CFG consistency check:");
        int testResult = kfg_testit(cfg, 1 /* quiet */);
        delete nestedTimer;
        if (verbose)
        {
            if (testResult == 0)
                std::cout << "failed" << std::endl;
            else
                std::cout << "done" << std::endl;
        }
     // This warning should be printed regardless of the user's choice of
     // verboseness.
        if (testResult == 0)
        {
            std::cerr << std::endl
                << "Warning: PAG reports: There are problems in the ICFG."
                << std::endl;
        }
    }

    /* Generate an external representation of the ICFG if requested. */
    if (options->outputIcfg())
    {
        TimingPerformance timer("ICFG visualization:");
        if (verbose)
        {
            std::cout
                << "generating icfg visualization "
                << options->getOutputIcfgFileName()
                << " ... " << std::flush;
        }
        outputIcfg(cfg, options->getOutputIcfgFileName().c_str());
        if (verbose) std::cout << "done" << std::endl;
    }

    return cfg;
}

void outputRoseAst(SgProject *astRoot, AnalyzerOptions *options)
{
    /* Handle command line option --output-source. */
    if (options->outputSource())
    {
        int numberOfFiles = astRoot->numberOfFiles();
        if (numberOfFiles == 1)
        {
            SgFile &file = astRoot->get_file(0);
            TimingPerformance timer("Output (single) source file:");
            file.set_unparse_output_filename(
                    options->getOutputSourceFileName());
            unparseFile(&file, 0, 0);
        }
        else if (numberOfFiles >= 1)
        {
            TimingPerformance timer("Output all source files:");
            for (int i = 0; i < numberOfFiles; i++)
            {
                SgFile &file = astRoot->get_file(i);
                std::string filename = file.get_sourceFileNameWithoutPath();
                file.set_unparse_output_filename(
                        options->getOutputFilePrefix() + filename);
                unparseFile(&file, 0, 0);
            }
        }
    }

    /* Handle command line option --output-binary-ast. */
    if (options->outputBinaryAst())
    {
        AST_FILE_IO::startUp(astRoot);
        AST_FILE_IO::writeASTToFile(options->getOutputBinaryAstFileName());
    }
}

void outputProgramRepresentation(Program *program, AnalyzerOptions *options)
{
    /* This function handles various forms of AST output, including terms. */
    outputRoseAst(program->astRoot, options);

    if (options->outputIcfg())
    {
        /* If the outputIcfg flag is set, ICFG construction generates
         * appropriate output. That is, the ICFG is output iff it is
         * constructed; in other words, all we need to do here is to ensure
         * that the ICFG exists. */
        if (program->icfg == NULL)
            program->icfg = createICFG(program->astRoot, options);
    }

    /* Handle command line option --output-term. */
    if (options->outputTerm())
    {
        if (program->prologTerm == NULL)
        {
            TimingPerformance timer("Output Prolog term:");
            TermPrinter<void *> tp(NULL, program->icfg);
            if (options->analysisWholeProgram())
                tp.traverse(program->astRoot);
            else
                tp.traverseInputFiles(program->astRoot);
            std::ofstream termfile;
            std::string filename = program->options->getOutputTermFileName();
            termfile.open(filename.c_str());
            termfile << "% Termite term representation" << std::endl;
            termfile << tp.getTerm()->getRepresentation() << "." << std::endl;
            termfile.close();
            program->prologTerm = tp.getTerm();
        }
    }
}

}

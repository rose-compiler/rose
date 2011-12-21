#include "satire.h"

#include "aslanalysis.h"

// We want to use the TermPrinter, but without the DFI printer because we
// have no DFI to print here.
#define DO_NOT_USE_DFIPRINTER 1
#include "TermPrinter.h"
#undef DO_NOT_USE_DFIPRINTER

// This is the ARAL input file pointer, which is global because the ARAL
// parser uses advanced late 1970s parsing technology.
extern FILE *aralin;

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
    if (options->inputBinaryAst())
    {
        astRoot = AST_FILE_IO::readASTFromFile(
                           options->getInputBinaryAstFileName());
    }
    else if (options->inputTermiteAst())
    {
        TermToRose conv;
        astRoot = dynamic_cast<SgProject*>(
                conv.toRose(options->getInputTermiteAstFileName().c_str()));
	ROSE_ASSERT(astRoot);
    }
    else 
    {
        astRoot = frontend(options->getCommandLineNum(),
                           options->getCommandLineCarray());
    }


    /* Perform sanity checks on the AST if requested. */
    if (options->checkRoseAst())
        AstTests::runAllTests(astRoot);

    return astRoot;
}

CFG *createICFG(Program *program, AnalyzerOptions *options)
{
    TimingPerformance *nestedTimer;
    bool verbose = options->verbose();

    /* Collect procedures. */
    nestedTimer = new TimingPerformance("SATIrE ICFG construction:");
    if (verbose) std::cout << "collecting functions ... " << std::flush;
    ProcTraversal s(program);
    s.setPrintCollectedFunctionNames(options->printCollectedFunctionNames());
    s.traverse(program->astRoot, preorder);
    if (verbose) std::cout << "done" << std::endl;

    /* Create the ICFG itself. */
    if (verbose) std::cout << "generating cfg ... " << std::flush;
    CFGTraversal t(s, program, options);
    if (!options->numberExpressions())
        t.numberExpressions(false);
    t.traverse(program->astRoot, preorder);
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
	if(options->getOutputGdlIcfgFileName()!="") { 
	  if (verbose) {
            std::cout
	      << "generating gdl-icfg visualization "
	      << options->getOutputGdlIcfgFileName()
	      << " ... " << std::flush;
	  }
	  outputGdlIcfg(cfg, options->getOutputGdlIcfgFileName().c_str());
	  if (verbose) std::cout << "done" << std::endl;
	}
	if(options->getOutputDotIcfgFileName()!="") { 
	  if (verbose) {
            std::cout
	      << "generating dot-icfg visualization "
	      << options->getOutputDotIcfgFileName()
	      << " ... " << std::flush;
	  }
	  outputDotIcfg(cfg, options->getOutputDotIcfgFileName().c_str());
	  if (verbose) std::cout << "done" << std::endl;
	}
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
            std::string filename = options->getOutputSourceFileName();
            if (filename == "")
            {
                filename = options->getOutputFilePrefix()
                         + file.get_sourceFileNameWithoutPath();
            }
            file.set_unparse_output_filename(filename);
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
            program->icfg = createICFG(program, options);
    }

    /* Handle command line option --output-term. */
    if (options->outputTerm())
    {
        if (program->prologTerm == NULL)
        {
            SATIrE::Analyses::ASLAnalysis aslAnalyzer;
            aslAnalyzer.run(program->astRoot);
            TimingPerformance timer("Output Prolog term:");
            TermPrinter<void *> tp(NULL, "", program->icfg, program);
            if (options->analysisWholeProgram())
                tp.traverse(program->astRoot);
            else
                tp.traverseInputFiles(program->astRoot);
            std::ofstream termfile;
            std::string filename = program->options->getOutputTermFileName();
            if (!openFileForWriting(termfile, filename))
            {
                std::cerr
                    << "*** error: could not open term file " << filename
                    << std::endl;
                std::exit(EXIT_FAILURE);
            }
            termfile << "% Termite term representation" << std::endl;
            termfile << tp.getTerm()->getRepresentation() << "." << std::endl;
            termfile.close();
            program->prologTerm = tp.getTerm();
        }
    }
}

// helper function
static void addAralResults(Program *program, Aral::ResultSection *results);

void attachAralInformation(Program *program, AnalyzerOptions *options)
{
    std::string aralFileName = options->getAralInputFileName();
    FILE *aralFile = std::fopen(aralFileName.c_str(), "r");
    {{{ if (aralFile == NULL)
    {
        std::cerr
            << "** ERROR: could not open file '"
            << aralFileName
            << "' for reading"
            << std::endl;
        std::exit(EXIT_FAILURE);
    } }}}

    aralin = aralFile;
    Aral::AralFile *aralRoot = Aral::Translator::frontEnd();
    aralin = NULL;
    fclose(aralFile);
    {{{ if (aralRoot == NULL)
    {
        std::cerr
            << "** ERROR: ARAL parser failure"
            << std::endl;
    } }}}

    Aral::ResultSectionList *results = aralRoot->getResultSectionList();
    Aral::List::iterator r;
    for (r = results->begin(); r != results->end(); ++r)
    {
        Aral::ResultSection *result
            = dynamic_cast<Aral::ResultSection *>(*r);
        assert(result != NULL);
        addAralResults(program, result);
    }
}



// MS: Mapping of ARAL 0.9.15 to ROSE-AST Nodes, November 2009.
static void addAralResults(Program* program, Aral::ResultSection* resultSection)
{
 // MS:TODO: utilize flow-specifier, implement file/function/program loc-specs and test

  int label=0;
  std::string analysisName=resultSection->getName();
  AstAralAttribute* attribute=new AstAralAttribute();
  Aral::AnnotationDataList* annotDataList=resultSection->getAnnotationDataList();
  for(Aral::AnnotationDataList::iterator i=annotDataList->begin();
      i!=annotDataList->end();
      ++i) {
    Aral::AnnotationData* data=dynamic_cast<Aral::AnnotationData*>(*i);
    assert(data);
    // the data ist attached to the ROSE-AST according to the information
    // in the LocationSpecifier.
    SgNode* node=0;


    Aral::LocationSpecifier* locSpec=data->getLocationSpecifier();
    switch(locSpec->getSpecifier()) {
    case Aral::LocationSpecifier::E_PROGRAM_LOCSPEC:
      // we ignore it for now
      break;
    case Aral::LocationSpecifier::E_FUNCTION_LOCSPEC:
      // we ignore it for now
      break;
    case Aral::LocationSpecifier::E_FILE_LOCSPEC:
      // SgFile* file=traceBackToFileNode(node);
      // we ignore it for now
      break;
    case Aral::LocationSpecifier::E_LABEL_LOCSPEC:
      Aral::Label* aralLabel=locSpec->getLabel();
      long labelNumber=aralLabel->getNumber();
      node=program->icfg->labeledStatement(labelNumber);
      Aral::FlowSpecifier flowSpec=data->getFlowSpecifier();
      Aral::InfoElementList* infoElemList=data->getInfoElementList();
      // if the analysis is context-sensitive we may have more than one element,
      // otherwise we have exactly one InfoElement in the list with context=0.
      // we put the InfoElementList in the Attribute, and let the Attribute
      // provide a richer interface.
      attribute->setInfoElementList(infoElemList);
      node->addNewAttribute(analysisName,attribute);
    }
  }
}

SgFile *traceBackToFileNode(SgNode *node)
{
    while (node != NULL && !isSgFile(node))
        node = node->get_parent();
    return isSgFile(node);
}

}

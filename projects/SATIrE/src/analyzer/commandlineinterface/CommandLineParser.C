// Copyright 2005,2006,2007,2008 Markus Schordan, Gergo Barany, Adrian Prantl
// $Id: CommandLineParser.C,v 1.28 2008-11-14 17:11:10 gergo Exp $

// #include <config.h>

#include <stdlib.h>
#include <string.h>
#include <sstream>
#include "CommandLineParser.h"

namespace SATIrE {
  DataFlowAnalysis *makeProvidedAnalyzer(const char *name);
}

using namespace std;

void CommandLineParser::exitError(string msg)
{
  if (msg != "") cout << "**ERROR: " << msg << endl;
  cout << "(For a complete list of options, invoke with --help)" << endl;
  exit(EXIT_FAILURE);
}

void CommandLineParser::parse(AnalyzerOptions *cl, int argc, char**argv) {

  cl->setProgramName(string(argv[0]));
  cl->clearCommandLine();
  cl->appendCommandLine(string(argv[0]));

  for (int i=1; i < argc; i += handleOption(cl, i, argc, argv));

  // handle edg warnings by modifying the command line that is passed to ROSE
  if(!cl->frontendWarnings()) 
    cl->appendCommandLine("--edg:no_warnings");

  // post-processing of parsed values
  if(cl->helpMessageRequested()) {
    cout << cl->getOptionsInfo() << endl;
    exit(EXIT_SUCCESS);
  }
  if(cl->optionsError()) {
    exitError(cl->getOptionsErrorMessage());
   }
  if (cl->getNumberOfInputFiles() == 0 &&
      !cl->inputBinaryAst() && 
      !cl->inputTermiteAst()) {
    exitError("no input files to analyze");
  } else if (cl->getNumberOfInputFiles() != 0 && cl->inputBinaryAst()) {
    exitError("both source and binary input files");
  }

  /* extend command line with ROSE options for front end language selection */
  switch(cl->getLanguage()) {
  case AnalyzerOptions::Language_C89: cl->appendCommandLine(string("-rose:C_only ")); break;
  case AnalyzerOptions::Language_C99: cl->appendCommandLine(string("-rose:C99_only ")); break;
  case AnalyzerOptions::Language_CPP: /* default ROSE mode */ break;
  default: {
    cout << "UNKOWN LANGUAGE SELECTED: " << cl->getLanguage() << endl;
    failed(cl);
  }
  }
  //cl->appendCommandLine(cl->getInputFileName());
}

int CommandLineParser::handleOption(AnalyzerOptions* cl, int i, int argc, char **argv) {
  if (i >= argc) 
    return 0;

  int old_i = i;

  if (optionMatch(argv[i], "--statistics")) {
    cl->statisticsOn();
  } else if (optionMatch(argv[i], "--no-statistics")) {
    cl->statisticsOff();
#if HAVE_PAG
  } else if (optionMatchPrefix(argv[i], "--cfgordering=")) {
    cl->setCfgOrdering(atoi(argv[i]+prefixLength));
#endif
  } else if (optionMatch(argv[i], "--no-result")) {
    cl->resultGenerationOff();
  } else if (optionMatch(argv[i], "--no_anim")) {
    cout << "SATIrE commandline option: --no_anim is deprecated (has no effect)." << endl;
    //cl->outputGdlAnimOff();
#if HAVE_PAG
  } else if (optionMatchPrefix(argv[i], "--callstringlength=")) {
    cl->setCallStringLength(atoi(argv[i]+prefixLength));
  } else if (optionMatch(argv[i], "--callstringinfinite")) {
    cl->setCallStringLength(-1);
#endif
  } else if (optionMatch(argv[i], "--verbose")) {
    cl->verboseOn();
    cl->quietOff();
  } else if (optionMatch(argv[i], "--no-verbose")) {
    cl->verboseOff();
    cl->quietOn();
#if HAVE_PAG
  } else if (optionMatchPrefix(argv[i], "--pag-memsize-mb=")) {
    if (cl->memsizeMBSet() || cl->memsizePercSet()) {
      cl->setOptionsErrorMessage("only one --pag-memsize-mb or --pag-memsize-perc flag is allowed");
      return 1;
    }
    cl->memsizeMBSetOn();
    cl->setMemsizeMB(atoi(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--pag-memsize-perc=")) {
    if (cl->memsizeMBSet() || cl->memsizePercSet()) {
      cl->setOptionsErrorMessage("only one --pag-memsize-mb or --pag-memsize-perc flag is allowed");
      return 1;
    }
    cl->memsizePercSetOn();
    cl->setMemsizePerc(atoi(argv[i]+prefixLength));
// GB (2008-06-27): Renamed highperc to a more descriptive external name.
//} else if (optionMatchPrefix(argv[i], "--pag-gc-highperc=")) {
  } else if (optionMatchPrefix(argv[i], "--pag-memsize-grow=")) {
    cl->setGcHigh(atoi(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--pag-gc-lowperc=")) {
    cl->setGcLow(atoi(argv[i]+prefixLength));
  } else if (optionMatch(argv[i], "--pag-startbank")) {
    cl->setStartBank(atoi(argv[i]+prefixLength));
  } else if (optionMatch(argv[i], "--pag-sharemin")) {
    cl->setShareMin(atoi(argv[i]+prefixLength));
  } else if (optionMatch(argv[i], "--pag-sharenum")) {
    cl->setShareNum(atoi(argv[i]+prefixLength));
#endif
#if HAVE_PAG
  } else if (optionMatch(argv[i], "--gdl-preinfo")) {
    cl->preInfoOn();
  } else if (optionMatch(argv[i], "--no-gdl-preinfo")) {
    cl->preInfoOff();
  } else if (optionMatch(argv[i], "--gdl-postinfo")) {
    cl->postInfoOn();
  } else if (optionMatch(argv[i], "--no-gdl-postinfo")) {
    cl->postInfoOff();
  } else if (optionMatch(argv[i], "--gdl-proceduresubgraphs")) {
    cl->gdlProcedureSubgraphsOn();
  } else if (optionMatch(argv[i], "--no-gdl-proceduresubgraphs")) {
    cl->gdlProcedureSubgraphsOff();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=varid")) {
    cl->nodeFormatVarIdOn();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=no-varid")) {
    cl->nodeFormatVarIdOff();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=varname")) {
    cl->nodeFormatVarNameOn();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=no-varname")) {
    cl->nodeFormatVarNameOff();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=exprid")) {
    cl->nodeFormatExprIdOn();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=no-exprid")) {
    cl->nodeFormatExprIdOff();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=exprsource")) {
    cl->nodeFormatExprSourceOn();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=no-exprsource")) {
    cl->nodeFormatExprSourceOff();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=asttext")) {
    cl->nodeFormatAstTextOn();
  } else if (optionMatch(argv[i], "--gdl-nodeformat=no-asttext")) {
    cl->nodeFormatAstTextOff();
#endif
  } else if (optionMatchPrefix(argv[i], "--analysis=")) {
    /* see if we have an analyzer with the given name */
    SATIrE::DataFlowAnalysis *analyzer
        = SATIrE::makeProvidedAnalyzer(argv[i]+prefixLength);
    if (analyzer == NULL) {
      cl->setOptionsErrorMessage("no such provided analysis: "
                                 + std::string(argv[i]+prefixLength));
      return 1;
    }
    cl->appendDataFlowAnalysis(analyzer);
  } else if (optionMatch(argv[i], "--analysis-files=all")) {
    cl->analysisWholeProgramOn();
  } else if (optionMatch(argv[i], "--analysis-files=cl")) {
    cl->analysisWholeProgramOff();
  } else if (optionMatch(argv[i], "--analysis-annotation")) {
    cl->analysisAnnotationOn();
  } else if (optionMatch(argv[i], "--no-analysis-annotation")) {
    cl->analysisAnnotationOff();
  } else if (optionMatch(argv[i], "--number-expressions")) {
    cl->numberExpressionsOn();
  } else if (optionMatch(argv[i], "--no-number-expressions")) {
    cl->numberExpressionsOff();
  } else if (optionMatch(argv[i], "--output-collectedfuncs")) {
    cl->printCollectedFunctionNamesOn();
  } else if (optionMatch(argv[i], "--no-output-collectedfuncs")) {
    cl->printCollectedFunctionNamesOff();
  } else if (optionMatch(argv[i], "--output-text")) {
    cl->outputTextOn();
  } else if (optionMatch(argv[i], "--no-output-text")) {
    cl->outputTextOff();
  } else if (optionMatch(argv[i], "--check-ast")) {
    cl->checkRoseAstOn();
  } else if (optionMatch(argv[i], "--no-check-ast")) {
    cl->checkRoseAstOff();
  } else if (optionMatch(argv[i], "--check-icfg")) {
    cl->checkIcfgOn();
  } else if (optionMatch(argv[i], "--no-check-icfg")) {
    cl->checkIcfgOff();
  } else if (optionMatchPrefix(argv[i], "--output-source=")) {
    if (strlen(argv[i]+prefixLength) == 0) {
      cl->setOptionsErrorMessage("empty output file name");
      return 1;
    }
    cl->outputSourceOn();
    cl->setOutputSourceFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-icfg=")) {
    cl->outputIcfgOn();
    cl->setOutputIcfgFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-sourceprefix=")) {
    if (strlen(argv[i]+prefixLength) == 0) {
      cl->setOptionsErrorMessage("empty output file name prefix");
      return 1;
    }
    cl->outputSourceOn();
    cl->setOutputFilePrefix(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-term=")) {
    cl->outputTermOn();
    cl->setOutputTermFileName(strdup(argv[i]+prefixLength));
#if HAVE_PAG
  } else if (optionMatchPrefix(argv[i], "--output-gdl=")) {
    cl->outputGdlOn();
    cl->setOutputGdlFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-gdlanim=")) {
    cl->outputGdlAnimOn();
    cl->setOutputGdlAnimDirName(strdup(argv[i]+prefixLength));
#endif
  } else if (optionMatchPrefix(argv[i], "--input-binary-ast=")) {
    if (cl->inputBinaryAst()) {
      cl->setOptionsErrorMessage("--input-binary-ast specified more than once");
      return 1;
    }
    cl->inputBinaryAstOn();
    cl->setInputBinaryAstFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-binary-ast=")) {
    if (cl->outputBinaryAst()) {
      cl->setOptionsErrorMessage("--output-binary-ast specified more than once");
      return 1;
    }
    cl->outputBinaryAstOn();
    cl->setOutputBinaryAstFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--input-termite-ast=")) {
    if (cl->inputTermiteAst()) {
      cl->setOptionsErrorMessage("--input-termite-ast specified more than once");
      return 1;
    }
    if (cl->inputBinaryAst()) {
      cl->setOptionsErrorMessage("--input-termite-ast and --input-binary-ast specified at the same time");
      return 1;
    }
    cl->inputTermiteAstOn();
    cl->setInputTermiteAstFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--gnu:")) {
    /* process gnu options: pass through without '--gnu:' */
    cl->appendCommandLine(string(argv[i]+6));
  } else if (optionMatchPrefix(argv[i], "--edg:")) {
    /* pass edg-options as required by ROSE through */
    cl->appendCommandLine(string(argv[i]));
  } else if (optionMatchPrefix(argv[i], "-edg:")) {
    /* ROSE requires to specify all EDG options starting
     * with one '-' as -edg: */
    cl->appendCommandLine(string(argv[i]));
  } else if (optionMatchPrefix(argv[i], "--rose:")) {
    /* all ROSE options start with '-rose:', to make this uniform
     * with SATIrE command line options we use '--rose:' but pass
     * all options through unchanged as '-rose:' */
    cl->appendCommandLine(string(argv[i]+1));
  } else if (optionMatch(argv[i], "--help-rose")) {
    cl->appendCommandLine("--help");
  } else if (optionMatch(argv[i], "--frontend-warnings")) {
    cl->frontendWarningsOn();
  } else if (optionMatch(argv[i], "--no-frontend-warnings")) {
    cl->frontendWarningsOff();
  } else if (optionMatch(argv[i], "--warn-deprecated")) {
    cl->deprecatedWarningsOn();
  } else if (optionMatch(argv[i], "--no-warn-deprecated")) {
    cl->deprecatedWarningsOff();
#if HAVE_PAG
  } else if (optionMatch(argv[i], "--pag-vivu")) {
    cl->vivuOn();
  } else if (optionMatchPrefix(argv[i], "--pag-vivuLoopUnrolling=")) {
    cl->setVivuLoopUnrolling(atoi(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--pag-vivu4MaxUnrolling=")) {
    cl->setVivu4MaxUnrolling(atoi(argv[i]+prefixLength));
#endif
  } else if (optionMatch(argv[i], "--help")) {
    cl->helpMessageRequestedOn();
  } else if (!strncmp(argv[i], "-I",2)) {
    /* include path option is passed to ROSE as is */
    cl->appendCommandLine(string(argv[i]));
  } else if (optionMatch(argv[i], "--language=c++")) {
    cl->setLanguage(AnalyzerOptions::Language_CPP);
  } else if (optionMatch(argv[i], "--language=c89")) {
    cl->setLanguage(AnalyzerOptions::Language_C89);
  } else if (optionMatch(argv[i], "--language=c99")) {
    cl->setLanguage(AnalyzerOptions::Language_C99);
  } else if (optionMatch(argv[i], "--run-pointsto-analysis")) {
    cl->runPointsToAnalysisOn();
  } else if (optionMatch(argv[i], "--no-run-pointsto-analysis")) {
    cl->runPointsToAnalysisOff();
  } else if (optionMatch(argv[i], "--resolve-funcptr-calls")) {
    cl->resolveFuncPtrCallsOn();
  } else if (optionMatch(argv[i], "--no-resolve-funcptr-calls")) {
    cl->resolveFuncPtrCallsOff();
  } else if (optionMatchPrefix(argv[i], "--output-pointsto-graph=")) {
    cl->outputPointsToGraphOn();
    cl->setPointsToGraphName(argv[i]+prefixLength);
#if HAVE_PAG
  } else if (optionMatch(argv[i], "--compute-call-strings")) {
    cl->computeCallStringsOn();
  } else if (optionMatch(argv[i], "--no-compute-call-strings")) {
    cl->computeCallStringsOff();
  } else if (optionMatch(argv[i], "--output-call-strings")) {
    cl->outputCallStringsOn();
  } else if (optionMatch(argv[i], "--no-output-call-strings")) {
    cl->outputCallStringsOff();
  } else if (optionMatchPrefix(argv[i], "--output-context-graph=")) {
    cl->outputContextGraphOn();
    cl->setContextGraphFileName(argv[i]+prefixLength);
#endif
  } else if (optionMatch(argv[i], "--verbatim-args")) {
    cl->verbatimArgsOn();
  } else if ((!optionMatchPrefix(argv[i], "-") && !optionMatchPrefix(argv[i],"--")) ) {
    /* handle as filename, pass filenames through */
    cout << "Found input file name '" << argv[i] << "'." << endl;
 // GB (2008-06-02): Using appendInputFile here; this method sets the input
 // file name, appends the name to the command line, and increments the file
 // name counter.
    cl->appendInputFile(argv[i]);
  } else if (cl->verbatimArgs()) {
    cl->appendCommandLine(string(argv[i]));
  } else {
    stringstream s;
    s << "unrecognized option: " << argv[i] << endl;
    cl->setOptionsErrorMessage(s.str());
    return 1;
  }
  return i+1-old_i;
}

/* Support functions for handling the analyzer command line and PAG options */
bool CommandLineParser::fileExists(const string& fileName)
{
  fstream fin;
  fin.open(fileName.c_str(),ios::in);
  if( fin.is_open() )
  {
    fin.close();
    return true;
  }
  fin.close();
  return false;
}

void CommandLineParser::failed(AnalyzerOptions *opt) {
  cout << opt->getOptionsInfo() << endl;
  exit(1);
}

bool CommandLineParser::optionMatch(const char* s1, const char* s2) {
  bool match=!strcmp(s1,s2);
  //if(match) cout << "INFO: found option: " << s1 << endl;
  return match;
}

bool CommandLineParser::optionMatchPrefix(const char* s, const char* prefix) {
  prefixLength=strlen(prefix);
  return !strncmp(s, prefix,prefixLength);
}

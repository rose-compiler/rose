// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany, Adrian Prantl
// $Id: CommandLineParser.C,v 1.18 2008-05-29 08:03:52 gergo Exp $

#include <config.h>

#include <sstream>
#include "CommandLineParser.h"

void CommandLineParser::parse(AnalyzerOptions *cl, int argc, char**argv) {

  cl->setProgramName(std::string(argv[0]));
  cl->clearCommandLine();
  cl->appendCommandLine(std::string(argv[0]));

  for (int i=1; i < argc; i += handleOption(cl, i, argc, argv));

  // handle edg warnings by modifying the command line that is passed to ROSE
  if(!cl->frontendWarnings()) 
    cl->appendCommandLine("--edg:no_warnings");

  // post-processing of parsed values
  if(cl->helpMessageRequested()) {
    std::cout << cl->getOptionsInfo() << std::endl;
    exit(0);
  }
  if(cl->optionsError()) {
    std::cout << cl->getOptionsErrorMessage() << std::endl;
    exit(1);
  }

  /* extend command line with ROSE options for front end language selection */
  switch(cl->getLanguage()) {
  case AnalyzerOptions::Language_C89: cl->appendCommandLine(std::string("-rose:C_only ")); break;
  case AnalyzerOptions::Language_C99: cl->appendCommandLine(std::string("-rose:C99_only ")); break;
  case AnalyzerOptions::Language_CPP: /* default ROSE mode */ break;
  default: {
    std::cout << "UNKOWN LANGUAGE SELECTED: " << cl->getLanguage() << std::endl;
    failed(cl);
  }
  }
  //cl->appendCommandLine(cl->getInputFileName());
}

int CommandLineParser::handleOption(AnalyzerOptions* cl, int i, int argc, char **argv) {
  if (i >= argc) 
    return 0;

  int old_i = i;

  if (optionMatchPrefix(argv[i], "--cfgordering=")) {
    cl->setCfgOrdering(atoi(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--pag-gc-lowperc=")) {
    cl->setGcLow(atoi(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--pag-gc-highperc=")) {
    cl->setGcHigh(atoi(argv[i]+prefixLength));
  } else if (optionMatch(argv[i], "--statistics")) {
    cl->statisticsOn();
  } else if (optionMatch(argv[i], "--no-statistics")) {
    cl->statisticsOff();
  } else if (optionMatch(argv[i], "--no-result")) {
    cl->resultGenerationOff();
  } else if (optionMatch(argv[i], "--no_anim")) {
    std::cout << "SATIrE commandline option: --no_anim is deprecated (has no effect)." << std::endl;
    //cl->outputGdlAnimOff();
  } else if (optionMatchPrefix(argv[i], "--callstringlength=")) {
    cl->setCallStringLength(atoi(argv[i]+prefixLength));
  } else if (optionMatch(argv[i], "--callstringinfinite")) {
    cl->setCallStringLength(-1);
  } else if (optionMatch(argv[i], "--verbose")) {
    cl->verboseOn();
    cl->quietOff();
  } else if (optionMatch(argv[i], "--no-verbose")) {
    cl->verboseOff();
    cl->quietOn();
  } else if (optionMatch(argv[i], "--pag-startbank")) {
    cl->setStartBank(atoi(argv[i]+prefixLength));
  } else if (optionMatch(argv[i], "--pag-sharemin")) {
    cl->setShareMin(atoi(argv[i]+prefixLength));
  } else if (optionMatch(argv[i], "--pag-sharenum")) {
    cl->setShareNum(atoi(argv[i]+prefixLength));
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
    cl->outputSourceOn();
    cl->setOutputSourceFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-icfg=")) {
    cl->outputIcfgOn();
    cl->setOutputIcfgFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-sourceprefix=")) {
    cl->outputSourceOn();
    cl->setOutputFilePrefix(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-term=")) {
    cl->outputTermOn();
    cl->setOutputTermFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-gdl=")) {
    cl->outputGdlOn();
    cl->setOutputGdlFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-gdlanim=")) {
    cl->outputGdlAnimOn();
    cl->setOutputGdlAnimDirName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--input-binary-ast=")) {
    cl->inputBinaryAstOn();
    cl->setInputBinaryAstFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--output-binary-ast=")) {
    cl->outputBinaryAstOn();
    cl->setOutputBinaryAstFileName(strdup(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--gnu:")) {
    /* process gnu options: pass through without '--gnu:' */
    cl->appendCommandLine(std::string(argv[i]+6));
  } else if (optionMatchPrefix(argv[i], "--edg:")) {
    /* pass edg-options as required by ROSE through */
    cl->appendCommandLine(std::string(argv[i]));
  } else if (optionMatchPrefix(argv[i], "-edg:")) {
    /* ROSE requires to specify all EDG options starting
     * with one '-' as -edg: */
    cl->appendCommandLine(std::string(argv[i]));
  } else if (optionMatchPrefix(argv[i], "--rose:")) {
    /* all ROSE options start with '-rose:', to make this uniform
     * with SATIrE command line options we use '--rose:' but pass
     * all options through unchanged as '-rose:' */
    cl->appendCommandLine(std::string(argv[i]+1));
  } else if (optionMatch(argv[i], "--help-rose")) {
    cl->appendCommandLine("--help");
  } else if (optionMatch(argv[i], "--frontend-warnings")) {
    cl->frontendWarningsOn();
  } else if (optionMatch(argv[i], "--no-frontend-warnings")) {
    cl->frontendWarningsOff();
  } else if (optionMatch(argv[i], "--pag-vivu")) {
    cl->vivuOn();
  } else if (optionMatchPrefix(argv[i], "--pag-vivuLoopUnrolling=")) {
    cl->setVivuLoopUnrolling(atoi(argv[i]+prefixLength));
  } else if (optionMatchPrefix(argv[i], "--pag-vivu4MaxUnrolling=")) {
    cl->setVivu4MaxUnrolling(atoi(argv[i]+prefixLength));
  } else if (optionMatch(argv[i], "--help")) {
    cl->helpMessageRequestedOn();
  } else if (!strncmp(argv[i], "-I",2)) {
    /* include path option is passed to ROSE as is */
    cl->appendCommandLine(std::string(argv[i]));
  } else if (optionMatch(argv[i], "--language=c++")) {
    cl->setLanguage(AnalyzerOptions::Language_CPP);
  } else if (optionMatch(argv[i], "--language=c89")) {
    cl->setLanguage(AnalyzerOptions::Language_C89);
  } else if (optionMatch(argv[i], "--language=c99")) {
    cl->setLanguage(AnalyzerOptions::Language_C99);
  } else if ((!optionMatchPrefix(argv[i], "-") && !optionMatchPrefix(argv[i],"--")) ) {
    /* handle as filename, pass filenames through */
    std::cout << "Found Input filename." << std::endl;
    cl->setInputFileName(argv[i]);
    cl->appendCommandLine(argv[i]);
  } else {
    std::stringstream s;
    s << "ERROR: unrecognized option: " << argv[i] << std::endl;
    cl->setOptionsErrorMessage(s.str());
    return 1;
  }
  return i+1-old_i;
}

/* Support functions for handling the analyzer command line and PAG options */
bool CommandLineParser::fileExists(const std::string& fileName)
{
  std::fstream fin;
  fin.open(fileName.c_str(),std::ios::in);
  if( fin.is_open() )
  {
    fin.close();
    return true;
  }
  fin.close();
  return false;
}

void CommandLineParser::failed(AnalyzerOptions *opt) {
  std::cout << opt->getOptionsInfo() << std::endl;
  exit(1);
}

bool CommandLineParser::optionMatch(char* s1, char* s2) {
  bool match=!strcmp(s1,s2);
  //if(match) std::cout << "INFO: found option: " << s1 << std::endl;
  return match;
}

bool CommandLineParser::optionMatchPrefix(char* s, char* prefix) {
  prefixLength=strlen(prefix);
  return !strncmp(s, prefix,prefixLength);
}

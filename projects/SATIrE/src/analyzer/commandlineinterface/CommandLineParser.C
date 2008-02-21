// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany, Adrian Prantl
// $Id: CommandLineParser.C,v 1.12 2008-02-21 14:31:17 adrian Exp $

#include <config.h>

#include <sstream>
#include "CommandLineParser.h"

void CommandLineParser::parse(AnalyzerOptions *cl, int argc, char**argv) {

  cl->setProgramName(std::string(argv[0]));
  cl->clearCommandLine();
  cl->appendCommandLine(std::string(argv[0]));

  int n_consumed;
  for (int i=1; i < argc;) {
    n_consumed = handleOption(cl, i, argc, argv);
    i += n_consumed;
  }
  if(cl->frontendWarnings()) 
    cl->appendCommandLine("--edg:no_warnings");

  //cl->setAnimationDirectoryName("anim-out");
  //cl->setGdlFileName(cl->getProgramName()+"_result.gdl");

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

  if (!strcmp(argv[i], "--cfgordering")) {
    cl->setCfgOrdering(atoi(argv[++i]));
  } else if (optionMatch(argv[i], "--pag-gc-lowperc")) {
    cl->setGcLow(atoi(argv[++i]));
  } else if (optionMatch(argv[i], "--pag-gc-highperc")) {
    cl->setGcHigh(atoi(argv[++i]));
  } else if (optionMatch(argv[i], "--statistics=yes")) {
    cl->statisticsOn();
  } else if (optionMatch(argv[i], "--statistics=no")) {
    cl->statisticsOff();
  } else if (optionMatch(argv[i], "--no_result")) {
    cl->resultGenerationOff();
  } else if (optionMatch(argv[i], "--no_anim")) {
    std::cout << "SATIrE commandline option: --no_anim is deprecated (has no effect)." << std::endl;
    //cl->outputGdlAnimOff();
  } else if (optionMatch(argv[i], "--callstringlength")) {
    if(i+1>=argc) { failed(cl); }
    cl->setCallStringLength(atoi(argv[++i]));
  } else if (optionMatch(argv[i], "--callstringinfinite")) {
    cl->setCallStringLength(-1);
  } else if (optionMatch(argv[i], "--verbose=yes")) {
    cl->verboseOn();
  } else if (optionMatch(argv[i], "--verbose=no")) {
    cl->verboseOff();
  } else if (optionMatch(argv[i], "--quiet=yes")) {
    cl->quietOn();
  } else if (optionMatch(argv[i], "--quiet=no")) {
    cl->quietOff();
  } else if (optionMatch(argv[i], "--pag-startbank")) {
    cl->setStartBank(atoi(argv[++i]));
  } else if (optionMatch(argv[i], "--pag-sharemin")) {
    cl->setShareMin(atoi(argv[++i]));
  } else if (optionMatch(argv[i], "--pag-sharenum")) {
    cl->setShareNum(atoi(argv[++i]));
  } else if (optionMatch(argv[i], "--gdl-preinfo=yes")) {
    cl->preInfoOn();
  } else if (optionMatch(argv[i], "--gdl-preinfo=no")) {
    cl->preInfoOff();
  } else if (optionMatch(argv[i], "--gdl-postinfo=yes")) {
    cl->postInfoOn();
  } else if (optionMatch(argv[i], "--gdl-postinfo=no")) {
    cl->postInfoOff();
  } else if (optionMatch(argv[i], "--gdl-proceduresubgraphs=yes")) {
    cl->gdlProcedureSubgraphsOn();
  } else if (optionMatch(argv[i], "--gdl-proceduresubgraphs=no")) {
    cl->gdlProcedureSubgraphsOff();
  } else if (optionMatch(argv[i], "--analysis-files=all")) {
    cl->analysisWholeProgramOn();
  } else if (optionMatch(argv[i], "--analysis-files=cl")) {
    cl->analysisWholeProgramOff();
  } else if (optionMatch(argv[i], "--analysis-annotation=yes")) {
    cl->analysisAnnotationOn();
  } else if (optionMatch(argv[i], "--analysis-annotation=no")) {
    cl->analysisAnnotationOff();
  } else if (optionMatch(argv[i], "--output-collectedfuncs=yes")) {
    cl->printCollectedFunctionNamesOn();
  } else if (optionMatch(argv[i], "--output-collectedfuncs=no")) {
    cl->printCollectedFunctionNamesOff();
  } else if (optionMatch(argv[i], "--output-text=yes")) {
    cl->outputTextOn();
  } else if (optionMatch(argv[i], "--output-text=no")) {
    cl->outputTextOff();
  } else if (optionMatch(argv[i], "--check-ast=yes")) {
    cl->checkRoseAstOn();
  } else if (optionMatch(argv[i], "--check-ast=no")) {
    cl->checkRoseAstOff();
  } else if (optionMatch(argv[i], "--output-source=yes")) {
    cl->outputSourceOn();
  } else if (optionMatch(argv[i], "--output-source=no")) {
    cl->outputSourceOff();
  } else if (optionMatch(argv[i], "--output-sourcefile")) {
    cl->outputSourceOn();
    cl->setOutputSourceFileName(strdup(argv[++i]));
  } else if (optionMatch(argv[i], "--output-icfgfile")) {
    cl->outputIcfgOn();
    cl->setOutputIcfgFileName(strdup(argv[++i]));
  } else if (optionMatch(argv[i], "--output-fileprefix")) {
    cl->outputSourceOn();
    cl->setOutputFilePrefix(strdup(argv[++i]));
  } else if (optionMatch(argv[i], "--output-termfile")) {
    cl->outputTermOn();
    cl->setOutputTermFileName(strdup(argv[++i]));
  } else if (optionMatch(argv[i], "--output-gdlfile")) {
    cl->outputGdlOn();
    cl->setOutputGdlFileName(strdup(argv[++i]));
  } else if (optionMatch(argv[i], "--output-gdlanimdir")) {
    cl->outputGdlAnimOn();
    cl->setOutputGdlAnimDirName(strdup(argv[++i]));
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
  } else if (optionMatch(argv[i], "--rose-help")) {
    cl->appendCommandLine("--help");
  } else if (optionMatch(argv[i], "--frontend-warnings=yes")) {
    cl->frontendWarningsOn();
  } else if (optionMatch(argv[i], "--frontend-warnings=no")) {
    cl->frontendWarningsOff();
  } else if (optionMatch(argv[i], "--pag-vivu")) {
    cl->vivuOn();
  } else if (optionMatch(argv[i], "--pag-vivuLoopUnrolling")) {
    if(i+1>=argc) { failed(cl); }
    cl->setVivuLoopUnrolling(atoi(argv[++i]));
  } else if (optionMatch(argv[i], "--pag-vivu4MaxUnrolling")) {
    if(i+1>=argc) { failed(cl); }
    cl->setVivu4MaxUnrolling(atoi(argv[++i]));
  } else if (optionMatch(argv[i], "--help")) {
    cl->helpMessageRequestedOn();
  } else if (!strncmp(argv[i], "-I",2)) {
    /* include path option is passed to ROSE as is */
    cl->appendCommandLine(std::string(argv[i]));
  } else if (optionMatch(argv[i], "--language")) {
    //check for language
    if(optionMatch(argv[i+1], "c++")) {
      cl->setLanguage(AnalyzerOptions::Language_CPP);
    } else if (optionMatch(argv[i+1], "c89")) {
      std::cout << "c89 option recognized\n";
      cl->setLanguage(AnalyzerOptions::Language_C89);
    } else if (optionMatch(argv[i+1], "c99")) {
      std::cout << "c99 option recognized\n";
      cl->setLanguage(AnalyzerOptions::Language_C99);
    } else {
      failed(cl);
    }
    i++;
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
  if(match) std::cout << "INFO: found option: " << s1 << std::endl;
  return match;
}

bool CommandLineParser::optionMatchPrefix(char* s, char* prefix) {
  return !strncmp(s, prefix,strlen(prefix));
}

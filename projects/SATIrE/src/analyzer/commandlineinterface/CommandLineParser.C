// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: CommandLineParser.C,v 1.4 2007-05-22 20:25:22 markus Exp $

#include <config.h>

#include "CommandLineParser.h"

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

void CommandLineParser::failed(AnalyzerOptions opt) {
  std::cout << opt.getOptionsInfo() << std::endl;
  exit(1);
}

AnalyzerOptions CommandLineParser::parse(int argc, char**argv) {
  AnalyzerOptions cl;

  char inName[100];
  inName[0] = 0;

  cl.setAnimationDirectoryName("anim-out");
  cl.setProgramName(std::string(argv[0]));
  cl.setGdlFileName(cl.getProgramName()+"_result.gdl");
  cl.clearCommandLine();
  cl.appendCommandLine(std::string(argv[0]));

  for (int i=1; i < argc; i++) {
    if (!strcmp(argv[i], "--cfgordering")) {
      cl.setCfgOrdering(atoi(argv[++i]));
    } else if (!strcmp(argv[i], "--gc_lowperc")) {
      cl.setGcLow(atoi(argv[++i]));
    } else if (!strcmp(argv[i], "--gc_highperc")) {
      cl.setGcHigh(atoi(argv[++i]));
    } else if (!strcmp(argv[i], "--gdl")) {
      cl.setGdlFileName(strdup(argv[++i]));
    } else if (!strcmp(argv[i], "--statistics")) {
      cl.statisticsOn();
    } else if (!strcmp(argv[i], "--no_result")) {
      cl.resultGenerationOff();
    } else if (!strcmp(argv[i], "--animdir")) {
      cl.setAnimationDirectoryName(strdup(argv[++i]));
    } else if (!strcmp(argv[i], "--no_anim")) {
      cl.animationGenerationOff();
    } else if (!strcmp(argv[i], "--callstringlength")) {
      if(i+1>=argc) { failed(cl); }
      cl.setCallStringLength(atoi(argv[++i]));
    } else if (!strcmp(argv[i], "--callstringinfinite")) {
      cl.setCallStringLength(-1);
    } else if (!strcmp(argv[i], "--pagverbose")) {
      cl.pagVerboseOn();
    } else if (!strcmp(argv[i], "--startbank")) {
      cl.setStartBank(atoi(argv[++i]));
    } else if (!strcmp(argv[i], "--sharemin")) {
      cl.setShareMin(atoi(argv[++i]));
    } else if (!strcmp(argv[i], "--sharenum")) {
      cl.setShareNum(atoi(argv[++i]));
    } else if (!strcmp(argv[i], "--preinfo")) {
      cl.preInfoOn();
    } else if (!strcmp(argv[i], "--postinfo")) {
      cl.postInfoOn();
    } else if (!strcmp(argv[i], "--no_preinfo")) {
      cl.preInfoOff();
    } else if (!strcmp(argv[i], "--no_postinfo")) {
      cl.postInfoOff();
    } else if (!strcmp(argv[i], "--no_proceduresubgraphs")) {
      cl.gdlProcedureSubgraphsOff();
    } else if (!strcmp(argv[i], "--textoutput")) {
      cl.analysisResultsTextOutputOn();
    } else if (!strcmp(argv[i], "--sourceoutput")) {
      cl.analysisResultsSourceOutputOn();
    } else if (!strcmp(argv[i], "--vivu")) {
      cl.vivuOn();
    } else if (!strcmp(argv[i], "--vivuLoopUnrolling")) {
      if(i+1>=argc) { failed(cl); }
      cl.setVivuLoopUnrolling(atoi(argv[++i]));
    } else if (!strcmp(argv[i], "--vivu4MaxUnrolling")) {
      if(i+1>=argc) { failed(cl); }
      cl.setVivu4MaxUnrolling(atoi(argv[++i]));
    } else if (!strcmp(argv[i], "--help")) {
      cl.helpMessageRequestedOn();
    } else if (!strncmp(argv[i], "-I",2)) {
      /* include path option is passed to ROSE as is */
      cl.appendCommandLine(std::string(argv[i]));
    } else if (!strcmp(argv[i], "--language")) {
      //check for language
      if(!strcmp(argv[i+1], "c++")) {
	cl.setLanguage(AnalyzerOptions::Language_CPP);
      } else if (!strcmp(argv[i+1], "c89")) {
	std::cout << "c89 option recognized\n";
	cl.setLanguage(AnalyzerOptions::Language_C89);
      } else if (!strcmp(argv[i+1], "c99")) {
	std::cout << "c99 option recognized\n";
	cl.setLanguage(AnalyzerOptions::Language_C99);
      } else {
	failed(cl);
      }
      i++;
    } else {
      if(inName[0]==0) {
	strcpy(inName, argv[i]);
	cl.appendCommandLine(std::string(argv[i]));
	if(!fileExists(std::string(inName))) {
	  std::cout << "Error: File "<< inName << " not found." << std::endl;
	  exit(1);
	}
      } else {
	failed(cl);
      }
    }

  }
  
  // post-processing of parsed values
  if(cl.helpMessageRequested()) {
    std::cout << cl.getOptionsInfo() << std::endl;
    exit(0);
  }
  if(cl.optionsError()) {
    std::cout << cl.getOptionsErrorMessage() << std::endl;
    exit(1);
  }

  if (inName[0]==0) {
    failed(cl);
  }
  cl.setInputFileName(inName);

  /* extend command line with ROSE options for front end language selection */
  switch(cl.getLanguage()) {
  case AnalyzerOptions::Language_C89: cl.appendCommandLine(std::string("-rose:C_only")); break;
  case AnalyzerOptions::Language_C99: cl.appendCommandLine(std::string("-rose:C99_only")); break;
  case AnalyzerOptions::Language_CPP: /* default ROSE mode */ break;
  default: {
    std::cout << "UNKOWN LANGUAGE SELECTED: " << cl.getLanguage() << std::endl;
    failed(cl);
  }
  };
  
  return cl;
}

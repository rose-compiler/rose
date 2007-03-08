// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: AnalyzerOptions.C,v 1.3 2007-03-08 15:36:48 markus Exp $

#include "AnalyzerOptions.h"

#include <iostream>
AnalyzerOptions::AnalyzerOptions(): _optionsErrorMessage(""),_optionsInfo("") {
  setCfgOrdering(1);
  setCallStringLength(0);
  setGdlFileName("analysis_result.gdl");
  setGcLow(30);
  setGcHigh(30);
  resultGenerationOn();
  animationGenerationOn();
  helpMessageRequestedOff();
  pagVerboseOff();
  preInfoOff();
  postInfoOn();
  statisticsOff();
  gdlProcedureSubgraphsOn();
  //setStartBank(?);
  //setShareMin(?);
  //setShareNum(?);
  analysisResultsTextOutputOff();
  analysisResultsSourceOutputOff();
  vivuOff();
  setVivuLoopUnrolling(2); // default
  setVivu4MaxUnrolling(-1); // default
  
  clearCommandLine();
  setCommandLineNum(0);

  std::string s=
    "Analysis options:\n"
    "   --callstringlength <num>     set callstring length to <num> [default:0]\n"
    "   --callstringinfinite         select infinite callstring (for non-resursive programs only)\n"
    "   --cfgordering <num>          set ordering that is used by the iteration algorithm where\n"
    "                                <num> = 1 : dfs preorder [default]\n"
    "                                        2 : bfs preorder\n"
    "                                        3 : reversed dfs postorder\n"
    "                                        4 : bfs postorder\n"
    "                                        5 : topsort scc dfs preorder\n"
    "                                        6 : topsort scc bfs preorder\n"
    "                                        7 : topsort scc reversed bfs dfs postorder\n"
    "                                        8 : topsort scc bfs postorder\n"
    "\n"
    " Output options:\n"
    "   --statistics                  output analyzer statistics on stdout\n"
    "   --pagverbose                  output analyzer debug info on stdout\n"
    "   --textoutput                  output the analysis results for each statement on stdout\n"
    "   --sourceoutput                generate source file with annotated analysis results for each statement\n"
    "                                 (only useful with callstringlength 0)\n"
    "   --help                        output this help message on stdout\n"
    "\n"
    " GDL output options:\n"
    "   --preinfo                     output analysis info before cfg nodes \n"
    "   --no_preinfo                  do not output preinfo [default]\n"
    "   --postinfo                    output analysis info after cfg nodes [default]\n"
    "   --no_postinfo                 do not output postinfo\n"
    "   --gdl <filename>              generate gdl graph in file <filename>\n"
    "   --animdir <dirname>           generate animation gdl files in directory <dirname>\n"
    "                                 [default: --animdir anim-out]\n"
    "   --no_anim                     do not generate an animation\n"
    "\n";

    /*
    " PAG garbage collection options:\n"
    "   --gc_lowperc <num>            the value <num> [0..99] gives the percentage of free heap,\n"
    "                                 at which GC is started\n"
    "   --gc_highperc <num>           if after a GC less than <num>% [0..99] of space in the heap\n"
    "                                 is free, a new bank is allocated and added to the heap\n"
    " VIVU options:\n"
    "   --vivu                        turns on vivu computation\n"
    "                                 use --callstringlength to set vivu chop size\n"
    "   --vivuLoopUnrolling <num>     if set to 2 it is distinguished between first and other executions\n"
    "                                 [default 2]\n"
    "   --vivu4MaxUnrolling <num>     maximal unrolling for VIVU4 mapping [default: -1]\n"
    "\n";
    */

  setOptionsInfo(s);
}
std::string AnalyzerOptions::toString() {
  return "InputFileName: "+getInputFileName()+"\n"
    + "Program Name: "+getProgramName()+"\n"
    + "GDL Filename: "+getGdlFileName()+"\n"
    + "Animation DirName: "+getAnimationDirectoryName()+"\n";
}
std::string AnalyzerOptions::getAnimationDirectoryName() { return _animationDirectoryName; }
void AnalyzerOptions::setAnimationDirectoryName(std::string animDir) { _animationDirectoryName=animDir; }
std::string AnalyzerOptions::getProgramName() { return _programName; }
void AnalyzerOptions::setProgramName(std::string progName) { _programName=progName; }
std::string AnalyzerOptions::getInputFileName() { return _inputFileName; }
void AnalyzerOptions::setInputFileName(std::string fileName) { _inputFileName=fileName; }
void AnalyzerOptions::setCfgOrdering(int ordering) { 
  if(ordering <1 || ordering>8) {
    optionsError("Cfg ordering must be a value between 1 to 8.");
  } else {
    _cfgOrdering=ordering; 
  }
}
int AnalyzerOptions::getCfgOrdering() { return _cfgOrdering; }
void AnalyzerOptions::setCallStringLength(int length) { _callStringLength=length; }
int AnalyzerOptions::getCallStringLength() { return _callStringLength; }
void AnalyzerOptions::setGdlFileName(std::string fileName) { _gdlFileName=fileName; }
std::string AnalyzerOptions::getGdlFileName() { return _gdlFileName; }
void AnalyzerOptions::setOptionsInfo(std::string info) { _optionsInfo=info; }
std::string AnalyzerOptions::getOptionsInfo() { return "\n Usage: "+getProgramName()+" [OPTION]... <filename>\n\n "+_optionsInfo; }
void AnalyzerOptions::setGcLow(int perc) { 
  if(perc < 0 || perc>99) {
    _gcLow=30; 
    optionsError("GC Low-percentage of garbage collection must be within 0..99.");
  } else {
    _gcLow=perc;
  }
}
void AnalyzerOptions::setGcHigh(int perc) { 
  if(perc < 0 || perc>99) {
    _gcHigh=30; 
    optionsError("GC High-percentage of garbage collection must be within 0..99.");
  } else {
    _gcHigh=perc;
  }
}

int AnalyzerOptions::getGcLow() { return _gcLow; }
int AnalyzerOptions::getGcHigh() { return _gcHigh; }

void AnalyzerOptions::setStartBank(int start) { _startBank=start; }
int AnalyzerOptions::getStartBank() { return _startBank; }
void AnalyzerOptions::setShareMin(int min) { _shareMin=min; }
int AnalyzerOptions::getShareMin() { return _shareMin; }
void AnalyzerOptions::setShareNum(int num) { _shareNum=num; }
int AnalyzerOptions::getShareNum() { return _shareNum; }

void AnalyzerOptions::animationGenerationOn() { _animationGeneration=true; }
void AnalyzerOptions::animationGenerationOff() { _animationGeneration=false; }
bool AnalyzerOptions::animationGeneration() { return _animationGeneration; }

void AnalyzerOptions::resultGenerationOn() { _resultGeneration=true; }
void AnalyzerOptions::resultGenerationOff() { _resultGeneration=false; }
bool AnalyzerOptions::resultGeneration() { return _resultGeneration; }

void AnalyzerOptions::helpMessageRequestedOn() { _helpMessageRequested=true; }
void AnalyzerOptions::helpMessageRequestedOff() { _helpMessageRequested=false; }
bool AnalyzerOptions::helpMessageRequested() { return _helpMessageRequested; }

void AnalyzerOptions::pagVerboseOn() { _pagVerbose=true; }
void AnalyzerOptions::pagVerboseOff() { _pagVerbose=false; }
bool AnalyzerOptions::pagVerbose() { return _pagVerbose; }

void AnalyzerOptions::preInfoOn() { _preInfo=true; }
void AnalyzerOptions::preInfoOff() { _preInfo=false; }
bool AnalyzerOptions::preInfo() { return _preInfo; }

void AnalyzerOptions::postInfoOn() { _postInfo=true; }
void AnalyzerOptions::postInfoOff() { _postInfo=false; }
bool AnalyzerOptions::postInfo() { return _postInfo; }

void AnalyzerOptions::statisticsOn() { _statistics=true; }
void AnalyzerOptions::statisticsOff() { _statistics=false; }
bool AnalyzerOptions::statistics() { return _statistics; }

void AnalyzerOptions::gdlProcedureSubgraphsOn() { _gdlProcedureSubgraphs=true; }
void AnalyzerOptions::gdlProcedureSubgraphsOff() { _gdlProcedureSubgraphs=false; }
bool AnalyzerOptions::gdlProcedureSubgraphs() { return _gdlProcedureSubgraphs; }

void AnalyzerOptions::analysisResultsTextOutputOn() { _analysisResultsTextOutput=true; }
void AnalyzerOptions::analysisResultsTextOutputOff() { _analysisResultsTextOutput=false; }
bool AnalyzerOptions::analysisResultsTextOutput() { return _analysisResultsTextOutput; }

void AnalyzerOptions::analysisResultsSourceOutputOn() { _analysisResultsSourceOutput=true; }
void AnalyzerOptions::analysisResultsSourceOutputOff() { _analysisResultsSourceOutput=false; }
bool AnalyzerOptions::analysisResultsSourceOutput() { return _analysisResultsSourceOutput; }

void AnalyzerOptions::vivuOn() { _vivu=true; }
void AnalyzerOptions::vivuOff() { _vivu=false; }
bool AnalyzerOptions::vivu() { return _vivu; }

void AnalyzerOptions::setVivuLoopUnrolling(int start) { _vivuLoopUnrolling=start; }
int AnalyzerOptions::getVivuLoopUnrolling() { return _vivuLoopUnrolling; }

void AnalyzerOptions::setVivu4MaxUnrolling(int start) { _vivu4MaxUnrolling=start; }
int AnalyzerOptions::getVivu4MaxUnrolling() { return _vivu4MaxUnrolling; }

bool AnalyzerOptions::optionsError() { return _optionsErrorMessage!=""; }
void AnalyzerOptions::optionsError(std::string message) {
  _optionsErrorMessage=message;
}
std::string AnalyzerOptions::getOptionsErrorMessage() {
  return _optionsErrorMessage;
}

void AnalyzerOptions::clearCommandLine() { _commandLine.clear(); }
void AnalyzerOptions::appendCommandLine(std::string cl) { _commandLine.push_back(cl); addCommandLineNum(1);}
std::list<std::string> AnalyzerOptions::getCommandLineList() { return _commandLine; }
char** AnalyzerOptions::getCommandLineCarray() { 
  int argc=getCommandLineNum();
  char** argv=new char*[argc]; // we need to create the same DS as in C for argv
  int j=0;
  for(std::list<std::string>::iterator i=_commandLine.begin(); i!=_commandLine.end(); i++) {
    argv[j++]=strdup(const_cast<char*>((*i).c_str()));
  }
  return argv;
}

std::string AnalyzerOptions::getCommandLine() { 
  std::string s;
  for(std::list<std::string>::iterator i=_commandLine.begin(); i!=_commandLine.end(); i++) {
    s+=*i+" ";
  }
  return s;
}


void AnalyzerOptions::setCommandLineNum(int cl) { _commandLineNum=cl; }
void AnalyzerOptions::addCommandLineNum(int cl) { _commandLineNum+=cl; }
int AnalyzerOptions::getCommandLineNum() { return _commandLineNum; }

bool AnalyzerOptions::retFuncUsed() { return true; }

// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany, Adrian Prantl, Viktor Pavlu
// $Id: AnalyzerOptions.C,v 1.30 2008-11-14 17:11:10 gergo Exp $

// todo: inheritance mechanism for help text (w/ automagic "[default]" labelling)

#include "AnalyzerOptions.h"
#include <iostream>
#include <cstring>

AnalyzerOptions::~AnalyzerOptions() {}
AnalyzerOptions::AnalyzerOptions(): _optionsErrorMessage(""),_optionsInfo(""),_numberOfInputFiles(0) {
  
  // set default values
  setLanguage(Language_C89);
  #define STRING_ATTR(attrname,defaultval) \
  set##attrname(defaultval);
  #define INT_ATTR(attrname,defaultval) \
  set##attrname(defaultval);
  #define INT_ATTR_NOSTUB(attrname,defaultval) \
  set##attrname(defaultval);
  #define BOOL_ATTR(attrname,defaultval) \
  attrname##defaultval();
  #include "attributes"
  #undef STRING_ATTR
  #undef INT_ATTR
  #undef INT_ATTR_NOSTUB
  #undef BOOL_ATTR
  
  clearCommandLine();
  setCommandLineNum(0);

  std::string s=
    " Frond End options:\n"
    "   --language=c++|c99|c89   select input language [default=c89]\n"
    "   --frontend-warnings      show Front End warnings when parsing file(s)\n"
    "   --no-frontend-warnings   do not show Front End warnings when parsing\n"
    "                            file(s) [default]\n"
    "   -I<path>                 specify path for include files\n"
    "   --input-binary-ast=<FILENAME> read AST from binary file instead of a\n"
    "                            source file\n"
    "   --verbatim-args          trailing arguments passed to front end verbatim\n"
    "\n"
    " General analysis options:\n"
    "   --check-ast              run all ROSE tests for checking\n"
    "                            whether ROSE-AST is correct\n"
    "   --no-check-ast           do not run ROSE AST tests [default]\n"
    "   --check-icfg             run PAG's ICFG consistency checks\n"
    "   --no-check-icfg          do not run ICFG checks [default]\n"
    "   --analysis-files=all|cl  analyse all source files or only those\n"
    "                            specified on the command line [default=cl]\n"
    "   --analysis-annotation    annotate analysis results in AST and output\n"
    "                            [default]\n"
    "   --no-analysis-annotation do not annotate analysis results in AST\n"
    "   --number-expressions     number expressions and types in the ICFG [default]\n"
    "   --no-number-expressions  do not number expressions and types in the ICFG\n"
    "   --run-pointsto-analysis  run a points-to analysis on the ICFG\n"
    "   --resolve-funcptr-calls  resolve indirect calls using pointer analysis\n"
    "   --output-pointsto-graph=<name>  create <name>.dot and <name>.eps files\n"
    "                            showing points-to analysis results (requires DOT)\n"
    "   --analysis=<identifier>  run SATIrE's analysis <identifier> on the ICFG\n"
    "\n"
#if HAVE_PAG
    " PAG-specific analysis options:\n"
    "   --callstringlength=<num> set callstring length to <num> [default=0]\n"
    "   --callstringinfinite     select infinite callstring (for non-recursive\n"
    "                            programs only)\n"
    "   --compute-call-strings   compute representation of call strings [default]\n"
    "   --no-compute-call-strings do not compute call strings\n"
    "   --output-call-strings    experimental: print call strings used by PAG\n"
    "   --no-output-call-strings do not attempt to output call strings [default]\n"
    "                            programs only)\n"
    "   --output-context-graph=<FILENAME> output DOT graph of calling contexts\n"
    "   --cfgordering=<num>      set ordering that is used by the iteration\n"
    "                            algorithm where\n"
    "                                  <num> = 1 : dfs preorder [default]\n"
    "                                          2 : bfs preorder\n"
    "                                          3 : reversed dfs postorder\n"
    "                                          4 : bfs postorder\n"
    "                                          5 : topsort scc dfs preorder\n"
    "                                          6 : topsort scc bfs preorder\n"
    "                                          7 : topsort scc reversed bfs\n"
    "                                              dfs postorder\n"
    "                                          8 : topsort scc bfs postorder\n"
    "   --pag-memsize-mb=<num>   allocate <num> MB of memory for PAG analysis\n"
    "   --pag-memsize-perc=<num> allocate <num>% of system memory (autodetected)\n"
    "   --pag-memsize-grow=<num> grow memory if less than <num>% are free after GC\n"
    "\n"
    " Default PAG options:\n"
    "       --pag-memsize-mb=5 --pag-memsize-grow=30\n"
    "\n"
#endif
    " Output options:\n"
    "   --statistics             output analyzer statistics on stdout\n"
    "   --no-statistics          do not show analyzer statistics on stdout\n"
    "   --verbose                output analyzer info on stdout\n"
    "   --no-verbose             do not print analyzer info on stdout\n"
    "   --output-text            print analysis results for each statement\n"
    "   --output-collectedfuncs  print all functions that are collected for\n"
    "                            the icfg generation\n"
    "   --output-source=<FILENAME> generate source file with annotated \n"
    "                            analysis results for each statement\n"
    "   --output-term=<FILENAME> generate Termite term representation of input\n"
    "                            program AST\n"
    "   --output-icfg=<FILENAME> output icfg of input program\n"
    "   --output-binary-ast=<FILENAME> write AST to binary file\n"
    "   --warn-deprecated        warn about the use of deprecated features\n"
    "   --no-warn-deprecated     do not warn about the use of deprecated features\n"
    "   --help                   print this help message on stdout\n"
    "   --help-rose              print the ROSE help message on stdout\n"
    "\n"
    " Default output options:\n"
    "       --no-statistics --verbose --warn-deprecated\n"
    "\n"
    " Multiple input/output files options:\n"
    "   --output-sourceprefix=<PREFIX> generate for each input file one output file\n"
    "                            with prefixed name\n"
    "\n"
#if HAVE_PAG
    " GDL output options:\n"
    "   --gdl-preinfo            output analysis info before cfg nodes\n"
    "   --no-gdl-preinfo         do not output analysis info before cfg nodes\n"
    "   --gdl-postinfo           output analysis info after cfg nodes\n"
    "   --no-gdl-postinfo        do not output analysis info after cfg nodes\n"
    "   --gdl-nodeformat=FORMAT  where FORMAT=varid|varname|exprid|exprsource\n"
    "                                        |asttext|no-varid|no-varname\n"
    "                                        |no-exprid|no-exprsource|no-asttext\n"
    "                            the format can be specified multiple times to\n"
    "                            have different formats printed at the same node\n"
    "                            The output is only affected if VariableId\n" 
    "                            and/or ExpressionId is used in the carrier type\n"
    "   --output-gdl=<FILENAME>  output program as gdl graph\n"
    "   --output-gdlanim=<DIRNAME> output animation gdl files in \n"
    "                            directory <dirname>\n"
    "\n"
    " Default GDL options:\n"
    "       --no-gdl-preinfo --gdl-postinfo\n"
    "       --gdl-nodeformat=no-asttext\n"
    "       --gdl-nodeformat=no-varid --gdl-nodeformat=varname\n"
    "       --gdl-nodeformat=no-exprid --gdl-nodeformat=exprsource\n"
#endif
    /*
    " PAG garbage collection options:\n"
    "   --pag-gc-lowperc <num>            the value <num> [0..99] gives the percentage of free heap,\n"
    "                                 at which GC is started\n"
    "   --pag-gc-highperc <num>           if after a GC less than <num>% [0..99] of space in the heap\n"
    "                                 is free, a new bank is allocated and added to the heap\n"
    " VIVU options:\n"
    "   --pag:vivu                        turns on vivu computation\n"
    "                                 use --callstringlength to set vivu chop size\n"
    "   --pag-vivuLoopUnrolling <num>     if set to 2 it is distinguished between first and other executions\n"
    "                                 [default 2]\n"
    "   --pag-vivu4MaxUnrolling <num>     maximal unrolling for VIVU4 mapping [default: -1]\n"
    "\n";
    */
    ;
  setHelpOptionsText(s);
}

std::string AnalyzerOptions::toString() {
  return "Program Name: "+getProgramName()+"\n"
    + "Input FileName: "+getInputFileName()+"\n"
    + "Output Source Filename: "+getOutputSourceFileName()+"\n"
    + "Output Term Filename: "+getOutputTermFileName()+"\n"
#if HAVE_PAG
    + "Output GDL Filename: "+getOutputGdlFileName()+"\n"
    + "Output GDL Anim DirName: "+getOutputGdlAnimDirName()+"\n"
#endif
    + "ROSE Command Line: "+getCommandLine()+"\n"
    ;
}

std::string AnalyzerOptions::getOptionsInfo() {
  return "\n Usage: "+getProgramName()+" [OPTION]... <filename1> <filename2> ... \n\n "+getHelpOptionsText();
}

#define STRING_ATTR(attrname,defaultval) \
  std::string AnalyzerOptions::get##attrname() { return _##attrname; } \
  void AnalyzerOptions::set##attrname(std::string s) { _##attrname=s; }
#define INT_ATTR(attrname,defaultval) \
  int AnalyzerOptions::get##attrname() { return _##attrname; } \
  void AnalyzerOptions::set##attrname(int i) { _##attrname=i; }
#define INT_ATTR_NOSTUB(attrname,defaultval) \
  int AnalyzerOptions::get##attrname() { return _##attrname; }
#define BOOL_ATTR(attrname,defaultval) \
  void AnalyzerOptions::attrname##On()  { _##attrname=true;  } \
  void AnalyzerOptions::attrname##Off() { _##attrname=false; } \
  bool AnalyzerOptions::attrname() { return _##attrname; }
#include "attributes"
#undef STRING_ATTR
#undef INT_ATTR
#undef INT_ATTR_NOSTUB
#undef BOOL_ATTR

#if HAVE_PAG
void AnalyzerOptions::setCfgOrdering(int ordering) { 
  if(ordering <1 || ordering>8) {
    setOptionsErrorMessage("ERROR: Cfg ordering must be a value between 1 to 8.");
  } else {
    _CfgOrdering=ordering; 
  }
}
void AnalyzerOptions::setGcLow(int perc) { 
  if(perc < 0 || perc>99) {
    _GcLow=30; 
    setOptionsErrorMessage("ERROR: GC Low-percentage of garbage collection must be within 0..99.");
  } else {
    _GcLow=perc;
  }
}
void AnalyzerOptions::setGcHigh(int perc) { 
  if(perc < 0 || perc>99) {
    _GcHigh=30; 
    setOptionsErrorMessage("ERROR: GC High-percentage of garbage collection must be within 0..99.");
  } else {
    _GcHigh=perc;
  }
}
void AnalyzerOptions::setMemsizeMB(int size) {
  if (size < 0) {
    _MemsizeMB = 5;
    setOptionsErrorMessage("ERROR: Analyzer memory size must be positive");
  } else {
    _MemsizeMB = size;
  }
}
void AnalyzerOptions::setMemsizePerc(int perc) {
  if (perc < 1 || perc > 99) {
    _MemsizePerc = 1;
    setOptionsErrorMessage("ERROR: Analyzer memory percentage must be within 1..99");
  } else {
    _MemsizePerc = perc;
  }
}
#endif

AnalyzerOptions::Language AnalyzerOptions::getLanguage() { return _language; }
void AnalyzerOptions::setLanguage(AnalyzerOptions::Language language) { _language=language; }


bool AnalyzerOptions::optionsError() {
  return _optionsErrorMessage!="";
}
void AnalyzerOptions::setOptionsErrorMessage(std::string message) {
  _optionsErrorMessage=message;
}
std::string AnalyzerOptions::getOptionsErrorMessage() {
  return _optionsErrorMessage;
}

void AnalyzerOptions::clearCommandLine() { _commandLine.clear(); }
void AnalyzerOptions::appendCommandLine(std::string cl) { _commandLine.push_back(cl); addCommandLineNum(1);}
std::vector<std::string> AnalyzerOptions::getCommandLineList() { return _commandLine; }
char** AnalyzerOptions::getCommandLineCarray() { 
  int argc=getCommandLineNum();
  char** argv=new char*[argc]; // we need to create the same DS as in C for argv
  int j=0;
  for(std::vector<std::string>::iterator i=_commandLine.begin(); i!=_commandLine.end(); i++) {
    argv[j++]=strdup(const_cast<char*>((*i).c_str()));
  }
  return argv;
}

void AnalyzerOptions::appendInputFile(std::string name) {
    appendCommandLine(name);
    setInputFileName(name);
    ++_numberOfInputFiles;
}

std::string AnalyzerOptions::getCommandLine() { 
  std::string s;
  for(std::vector<std::string>::iterator i=_commandLine.begin(); i!=_commandLine.end(); i++) {
    s+=*i+" ";
  }
  return s;
}

void AnalyzerOptions::setCommandLineNum(int cl) { _commandLineNum=cl; }
void AnalyzerOptions::addCommandLineNum(int cl) { _commandLineNum+=cl; }
int AnalyzerOptions::getCommandLineNum() { return _commandLineNum; }

bool AnalyzerOptions::retFuncUsed() { return true; }

int AnalyzerOptions::getNumberOfInputFiles() { return _numberOfInputFiles; }

void AnalyzerOptions::appendDataFlowAnalysis(SATIrE::DataFlowAnalysis *a)
{
  dataFlowAnalyzers.push_back(a);
}

const std::vector<SATIrE::DataFlowAnalysis *> &
AnalyzerOptions::getDataFlowAnalyzers() const
{
  return dataFlowAnalyzers;
}

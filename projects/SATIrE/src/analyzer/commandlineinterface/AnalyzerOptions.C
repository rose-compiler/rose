// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany, Adrian Prantl, Viktor Pavlu
// $Id: AnalyzerOptions.C,v 1.15 2008-02-21 17:00:24 markus Exp $

// todo: inheritance mechanism for help text (w/ automagic "[default]" labelling)

#include "AnalyzerOptions.h"
#include <iostream>

AnalyzerOptions::~AnalyzerOptions() {}
AnalyzerOptions::AnalyzerOptions(): _optionsErrorMessage(""),_optionsInfo("") {
  
  // set default values
  setLanguage(Language_CPP);
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
    "   --language c++ [default]\n"
    "   --language c99\n"
    "   --language c89\n"
    "   --frontend-warnings=yes|no     show Front End warnings when parsing file(s) [default=no]\n"
    "\n"
    " Analysis options:\n"
    "   --callstringlength <num>       set callstring length to <num> [default:0]\n"
    "   --callstringinfinite           select infinite callstring (for non-recursive programs only)\n"
    "   --cfgordering <num>            set ordering that is used by the iteration algorithm where\n"
    "                                  <num> = 1 : dfs preorder [default]\n"
    "                                          2 : bfs preorder\n"
    "                                          3 : reversed dfs postorder\n"
    "                                          4 : bfs postorder\n"
    "                                          5 : topsort scc dfs preorder\n"
    "                                          6 : topsort scc bfs preorder\n"
    "                                          7 : topsort scc reversed bfs dfs postorder\n"
    "                                          8 : topsort scc bfs postorder\n"
    "   --check-ast=yes|no             run all ROSE specific test for checking whether ROSE-AST is correct [default=no]\n"
    "   --analysis-files=all|cl        analyse all source files [default=all]\n"
    "   --analysis-annotation=yes|no   annotate analysis results in AST and output [default=yes]\n"
    "\n"
    " Output options:\n"
    "   --statistics=yes|no            output analyzer statistics on stdout [default=no]\n"
    "   --verbose=yes|no               output analyzer debug info on stdout [default=yes]\n"
    "   --output-text=yes|no           output analysis results for each statement on stdout [default=no]\n"
    "   --output-collectedfuncs=yes|no output all functions that are collected for the icfg generation [default=no]\n"
    "   --output-sourcefile <FILENAME> generate source file with annotated analysis results for each statement\n"
    "   --output-termfile <FILENAME>   generate Prolog term representation of input program AST\n"
    "   --output-icfgfile <FILENAME>   output icfg of input program (shows forward and backward edges)\n"
    "   --help                         output this help message on stdout\n"
    "\n"
    " Handling of multiple input and output files\n"
    "   --output-source=yes|no         output source files (only useful with option 'output-fileprefix') [default=no]\n"
    "   --output-fileprefix <PREFIX>   generate for each input file one output file with prefixed name\n"
    "                                  (only applied to source files in this version)                 \n"
    "\n"
    " GDL output options:\n"
    "   --gdl-preinfo=yes|no           output analysis info before cfg nodes [default=no]\n"
    "   --gdl-postinfo=yes|no          output analysis info after cfg nodes [default=yes]\n"
    "   --output-gdlfile <filename>    output program as gdl graph\n"
    "   --output-gdlanimdir <dirname>  output animation gdl files in directory <dirname>\n"
    "\n"
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
    + "Output GDL Filename: "+getOutputGdlFileName()+"\n"
    + "Output GDL Anim DirName: "+getOutputGdlAnimDirName()+"\n"
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

void AnalyzerOptions::setCfgOrdering(int ordering) { 
  if(ordering <1 || ordering>8) {
    setOptionsErrorMessage("Cfg ordering must be a value between 1 to 8.");
  } else {
    _CfgOrdering=ordering; 
  }
}
void AnalyzerOptions::setGcLow(int perc) { 
  if(perc < 0 || perc>99) {
    _GcLow=30; 
    setOptionsErrorMessage("GC Low-percentage of garbage collection must be within 0..99.");
  } else {
    _GcLow=perc;
  }
}
void AnalyzerOptions::setGcHigh(int perc) { 
  if(perc < 0 || perc>99) {
    _GcHigh=30; 
    setOptionsErrorMessage("GC High-percentage of garbage collection must be within 0..99.");
  } else {
    _GcHigh=perc;
  }
}

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

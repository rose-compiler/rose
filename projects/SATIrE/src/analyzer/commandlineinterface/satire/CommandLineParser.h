// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: CommandLineParser.h,v 1.7 2008-10-13 13:50:27 gergo Exp $

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <string>
#include <fstream>
#include <iostream>

#include "AnalyzerOptions.h"
#include "set_pag_options.h"

class CommandLineParser {
public:
  virtual ~CommandLineParser(){}
  void parse(AnalyzerOptions *cl, int argc, char** argv);
  virtual int handleOption(AnalyzerOptions *cl, int i, int argc, char **argv);
protected:
  bool fileExists(const std::string& fileName);
  void failed(AnalyzerOptions *opt);
  bool optionMatch(const char* s1, const char* s2);
  bool optionMatchPrefix(const char* s, const char* prefix);
  void exitError(std::string message = "");
  int prefixLength;
};


#endif

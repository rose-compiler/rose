// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: CommandLineParser.h,v 1.1 2007-09-20 09:25:32 adrian Exp $

#ifndef COMMANDLINE
#define COMMANDLINE

#include <string>
#include <fstream>
#include <iostream>

#include "AnalyzerOptions.h"
#include "set_pag_options.h"

class CommandLineParser {
public:
  AnalyzerOptions parse(int argc, char** argv);
private:
  bool fileExists(const std::string& fileName);
  void failed(AnalyzerOptions opt);
};


#endif

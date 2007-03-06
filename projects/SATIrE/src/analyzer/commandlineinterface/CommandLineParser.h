// Author: Markus Schordan, 2006

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

#ifndef COMMAND_LINE_OPTIONS_H
#define COMMAND_LINE_OPTIONS_H

#include <string>
#include <map>
#include <cstdio>
#include <cstring>
#include <boost/program_options.hpp>

using namespace std;

//namespace po = boost::program_options;

class BoolOptions {
public:
  BoolOptions();
  BoolOptions(int argc, char* argv[]);
  void init(int argc0, char* argv0[]);
  void registerOption(string name, bool defaultval);
  void processOptions();
  bool operator[](string option);
  string toString();
private:
  int argc;
  char** argv;
  map<string,bool> mapping;
};

enum ResultsFormat { RF_UNKNOWN, RF_RERS2012, RF_RERS2013 };

extern BoolOptions boolOptions; // defined in CommandLineOptions.C
extern boost::program_options::variables_map args; // defined in CommandLineOptions.C
extern int option_debug_mode;
extern ResultsFormat resultsFormat;
#endif

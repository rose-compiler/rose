#ifndef COMMAND_LINE_OPTIONS_H
#define COMMAND_LINE_OPTIONS_H

#include <string>
#include <map>
#include <cstdio>
#include <cstring>
#include <boost/program_options.hpp>

//namespace po = boost::program_options;

class BoolOptions {
public:
  BoolOptions();
  BoolOptions(int argc, char* argv[]);
  void init(int argc0, char* argv0[]);
  void registerOption(std::string name, bool defaultval);
  void processOptions();
  bool operator[](std::string option);
  std::string toString();
private:
  int argc;
  char** argv;
  std::map<std::string,bool> mapping;
};

extern BoolOptions boolOptions; // defined in CommandLineOptions.C
extern boost::program_options::variables_map args; // defined in CommandLineOptions.C
extern int option_debug_mode;
#endif

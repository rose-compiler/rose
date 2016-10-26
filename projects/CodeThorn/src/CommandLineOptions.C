#define USE_SAWYER_COMMANDLINE
#include "CommandLineOptions.h"
#include <sstream>
#include <iostream>

using namespace std;
/////////////////////////////////////////////////
// Command line processing global options
/////////////////////////////////////////////////
#ifdef USE_SAWYER_COMMANDLINE
Sawyer::CommandLine::Boost::variables_map args;
#else
boost::program_options::variables_map args;
#endif

BoolOptions boolOptions;

/////////////////////////////////////////////////

BoolOptions::BoolOptions():argc(0),argv(0){
}

BoolOptions::BoolOptions(int argc, char* argv[]):argc(argc),argv(argv) {
}

void BoolOptions::init(int argc0, char* argv0[]){
  argc=argc0;
  argv=argv0;
}

bool BoolOptions::operator[](string option) {
  // check that we are only accessing options which have been registered
  map<string,bool>::iterator i=mapping.find(option);
  if(i==mapping.end()) {
    std::cerr<<"Error: access to non-registered command line option "<<option<<endl;
    exit(1);
  }
  return mapping[option];
}

void BoolOptions::registerOption(string name, bool defaultval) {
  if(mapping.find(name) == mapping.end()) {
    mapping[name]=defaultval;
  } else {
    cerr<<"Error: command line option '"<<name<<"' already registered with value: "<<mapping[name]
        <<". Value "<<defaultval<<" not registered."<<endl;
    exit(1);
  }
}

void BoolOptions::setOption(string name, bool val) {
  if(mapping.find(name) != mapping.end()) {
    mapping[name]=val;
  } else {
    cerr<<"Error: attempted to set unregistered command line option '"<<name<<"' to value '"<<val<<"'."<<endl;
    exit(1);
  }
}

void BoolOptions::processZeroArgumentsOption(string name) {
  if(args.count(name)>0) {
    setOption(name,true);
  }
  // check for "no-" name
  string no_name="no-"+name;
  if(args.count(no_name)>0) {
    setOption(name,false);
  }
}

void BoolOptions::processOptions() {
  for(map<string,bool>::iterator i=mapping.begin();i!=mapping.end();++i) {
    string option=(*i).first;
    if (args.count(option)) {
      string x= args[option].as<string>();
      if(x=="yes")
        mapping[option]=true;
      else {
        if(x=="no")
          mapping[option]=false;
        else {
          std::cout<< "Wrong option: "<<option<<"="<<x<<". Only yes or no is allowed."<<endl;
          exit(1);
        }
      }
      for (int i=1; i<argc; ++i) {
        if (string(argv[i]) == option) {
          // do not confuse ROSE frontend
          argv[i] = strdup("");
          assert(i+1<argc);
          argv[i+1] = strdup("");
        }
      }
    }
  }
}

string BoolOptions::toString() {
  stringstream options;
  options<<"Options:"<<endl;
  for(map<string,bool>::iterator i=mapping.begin();i!=mapping.end();++i) {
    options<<(*i).first<<":"<<(*i).second<<endl;
  }
  return options.str();
}


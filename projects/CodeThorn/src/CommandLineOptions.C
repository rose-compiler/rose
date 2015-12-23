#include "CommandLineOptions.h"
#include <sstream>
#include <iostream>

using namespace std;
/////////////////////////////////////////////////
// Command line processing global options
/////////////////////////////////////////////////
boost::program_options::variables_map args;
BoolOptions boolOptions;
ResultsFormat resultsFormat=RF_RERS2012;

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
  mapping[name]=defaultval;
}

string BoolOptions::toString() {
  stringstream options;
  options<<"Options:"<<endl;
  for(map<string,bool>::iterator i=mapping.begin();i!=mapping.end();++i) {
    options<<(*i).first<<":"<<(*i).second<<endl;
  }
  return options.str();
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

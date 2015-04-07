// Author: Markus Schordan, 2013.

#include <boost/program_options.hpp>

#include "rose.h"

#include "VariableIdMapping.h"
#include "CommandLineOptions.h"
#include <iostream>

using std::cout;
using std::endl;
using namespace SPRAY;
using namespace std;

namespace po = boost::program_options;

int main( int argc, char *argv[] ) {
  try {
  // Command line option handling.
  po::options_description desc
    ("varwatch 0.1\n"
     "Written by Markus Schordan 2013\n"
     "Supported options");
  desc.add_options()
    ("help,h", "produce this help message")
    ("rose-help", "show help for compiler frontend options")
    ("version,v", "display the version")
    ;

  po::store(po::command_line_parser(argc, argv).
            options(desc).allow_unregistered().run(), args);
  po::notify(args);
  
  if (args.count("version")) {
    cout << "varwatch 1.0\n";
    return 0;
  }
  if (args.count("help")) {
    cout << desc << "\n";
    return 0;
  }
  if (args.count("rose-help")) {
    argv[1] = strdup("--help");
  }

  // Build the AST used by ROSE
  cout << "INIT: Parsing and creating AST: started."<<endl;
  SgProject* astRoot = frontend(argc,argv);
  cout << "INIT: Parsing and creating AST: finished."<<endl;
  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(astRoot);
#if 0
  bool check=variableIdMapping.isUniqueVariableSymbolMapping();
  cout << "VariableIdMapping is unique variable symbol mapping: ";
  check? cout << "YES":cout<<"NO";
  cout << endl;
  if(!check)
    variableIdMapping.reportUniqueVariableSymbolMappingViolations();
  //  else
  {
    cout<<"MAPPING:\n";
    variableIdMapping.toStream(cout);
    cout<<"-------------------------------------- OK --------------------------------------"<<endl;
  }
#endif

  variableIdMapping.generateDot("vidmapping.dot",astRoot);

  } catch(char* str) {
    cerr << "*Exception raised: " << str << endl;
    return 1;
  } catch(const char* str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  } catch(string str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
 }

  return 0;

}

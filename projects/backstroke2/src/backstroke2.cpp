// Author: Markus Schordan, 2013, 2014.

#include "rose.h"

#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <cmath>

#include "SgNodeHelper.h"
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include "Timer.h"

#include "limits.h"
#include "assert.h"

#include "CodeGenerator.h"
#include "Utility.h"

using namespace std;
using namespace Backstroke;

string option_prefix;

//#include "CommandLineOptions.h"
boost::program_options::variables_map args;

int main(int argc, char* argv[]) {
  try {
    if(argc==1) {
      cout << "Error: wrong command line options."<<endl;
      exit(1);
    }
    // Command line option handling.
    namespace po = boost::program_options;
    boost::program_options::options_description desc
      ("supported options");
  
    desc.add_options()
      ("help,h", "produce this help message.")
      ("rose-help", "show help for compiler frontend options.")
      ("show-file-node-info", "show project and file node dumps (using display()).")
      ("version,v", "display the version.")
      ;

    po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), args);
    po::notify(args);

    if (args.count("help")) {
      cout << "backstroke2 <filename> [OPTIONS]"<<endl;
      cout << desc << "\n";
      return 0;
    }
    if (args.count("rose-help")) {
      argv[1] = strdup("--help");
    }

    if (args.count("version")) {
      cout << "version 2.0 alpha\n";
      cout << "Written by Markus Schordan 2014\n";
      return 0;
    }

    // clean up string-options in argv
    for (int i=1; i<argc; ++i) {
      if (string(argv[i]) == "--prefix" 
          ) {
        // do not confuse ROSE frontend
        argv[i] = strdup("");
        assert(i+1<argc);
        argv[i+1] = strdup("");
      }
    }

  cout << "INIT: Parsing and creating AST."<<endl;
  SgProject* root = frontend(argc,argv);
  //  AstTests::runAllTests(root);
  // inline all functions

  if(args.count("show-file-node-info")) {
    Utility::printRoseInfo(root);
  }

  Backstroke::CodeGenerator g;
  g.generateCode(root);

  cout<< "STATUS: finished."<<endl;

  // main function try-catch
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

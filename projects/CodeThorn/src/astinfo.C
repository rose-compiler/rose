// Author: Markus Schordan, 2013.

#include <boost/program_options.hpp>
#include <iostream>

#include "rose.h"

#include "VariableIdMapping.h"
#include "LineColInfo.h"

#ifdef USE_SAWYER_COMMANDLINE
#include "Sawyer/CommandLineBoost.h"
#else
#include <boost/program_options.hpp>
#endif

#ifdef USE_SAWYER_COMMANDLINE
Sawyer::CommandLine::Boost::variables_map args;
#else
boost::program_options::variables_map args;
#endif

using std::cout;
using std::endl;
using namespace SPRAY;
using namespace std;

namespace po = boost::program_options;

void printLineColInfo(SgNode* astRoot) {
  RoseAst ast(astRoot);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgLocatedNode* loc=isSgLocatedNode(*i)) {
      if(!isSgInitializedName(loc)&&!loc->isCompilerGenerated()&&!isSgGlobal(loc)) {
        LineColInfo li(loc);
        cout<<li.toString()<<": "<<(*i)->class_name()<<" #"<<(*i)->unparseToString()<<"#"<<endl;
      }
    }
  }
}

void printClassName(SgNode* astRoot) {
  RoseAst ast(astRoot);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgClassDeclaration* classDecl=isSgClassDeclaration(*i)) {
      //      SgClassDeclaration* classDecl=classDefinition->get_declaration();
      std::string name=classDecl->get_name();
      LineColInfo li(classDecl);
      cout<<li.toString()<<": "<<(*i)->class_name()<<" #"<<name<<"#"<<endl;
    }
  }
}

int main( int argc, char *argv[] ) {
  try {
    // Command line option handling.
    po::options_description desc
      ("astinfo 0.1\n"
       "Written by Markus Schordan 2017\n"
       "Supported options");
    desc.add_options()
      ("help,h", "produce this help message")
      ("rose-help", "show help for compiler frontend options")
      ("varidmapping", "print variable-id mapping")
      ("linecol", "print line:column information for SgLocated nodes")
      ("classname", "print class name SgClass nodes")
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
    //cout << "INIT: Parsing and creating AST: started."<<endl;
    SgProject* astRoot = frontend(argc,argv);
    //cout << "INIT: Parsing and creating AST: finished."<<endl;
    VariableIdMapping variableIdMapping;
    variableIdMapping.computeVariableSymbolMapping(astRoot);

    if(args.count("varidmapping")) {
      cout<<"MAPPING:\n";
      variableIdMapping.toStream(cout);
      cout<<"-------------------------------------- OK --------------------------------------"<<endl;
      //variableIdMapping.generateDot("vidmapping.dot",astRoot);
    }
    if(args.count("linecol")) {
      printLineColInfo(astRoot);
    }
    if(args.count("classname")) {
      printClassName(astRoot);
    }
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

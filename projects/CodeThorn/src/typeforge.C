
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <map>

#include "rose.h"
#include "AstTerm.h"
#include "SgNodeHelper.h"
#include "Timer.h"
#include "CommandLineOptions.h"
#include "AstProcessing.h"
#include "AstMatching.h"
#include "Sawyer/Graph.h"
#include "TypeTransformer.h"

//preparation for using the Sawyer command line parser
//#define USE_SAWYER_COMMANDLINE
#ifdef USE_SAWYER_COMMANDLINE
#include "Sawyer/CommandLineBoost.h"
#else
#include <boost/program_options.hpp>
#endif

#include "CastStats.h"
#include "CastTransformer.h"
#include "CastGraphVis.h"
#include "CppStdUtilities.h"
#include <utility>
#include <functional>

using namespace std;

class TestTraversal : public AstSimpleProcessing {
public:
  virtual void visit(SgNode* node) { /* do nothing */ };
};


string toolName="typeforge";

int main (int argc, char* argv[])
{
  ROSE_INITIALIZE;
  Rose::global_options.set_frontend_notes(false);
  Rose::global_options.set_frontend_warnings(false);
  Rose::global_options.set_backend_warnings(false);

  // Command line option handling.
#ifdef USE_SAWYER_COMMANDLINE
    namespace po = Sawyer::CommandLine::Boost;
#else
    namespace po = boost::program_options;
#endif

  po::options_description desc
    ("Supported options");

  desc.add_options()
    ("help,h", "produce this help message.")
    ("version,v", "display the version.")
    ("annotate", "annotate implicit casts as comments.")
    ("explicit", "make all imlicit casts explicit.")
    ("stats", "print statistics on casts of built-in floating point types.")
    ("trace", "print cast operations as they are performed.")
    ("dot-type-graph", "generate typegraph in dot file 'typegraph.dot'.")
    ("command-file", po::value< string >()," name of file where each line specifies how to change a variable's type: type-name function-name var-name.")
    ("float-var", po::value< string >()," change type of var [arg] to float.")
    ("double-var", po::value< string >()," change type of var [arg] to double.")
    ("long-double-var", po::value< string >()," change type of var [arg] to long double.")
    ;

  po::store(po::command_line_parser(argc, argv).
            options(desc).allow_unregistered().run(), args);
  po::notify(args);

  if (args.count("help")) {
    cout << toolName <<" <filename> [OPTIONS]"<<endl;
    cout << desc << "\n";
    exit(0);
  }

  if(args.isUserProvided("version")) {
    cout<<toolName<<" version 0.2.0"<<endl;
    return 0;
  }

  for (int i=1; i<argc; ++i) {
    if (string(argv[i]) == "--float-var"
        || string(argv[i]) == "--double-var"
	|| string(argv[i]) == "--long-double-var"
        ) {
      argv[i] = strdup("");
      assert(i+1<argc);
      argv[i+1] = strdup("");
    }
  }
  vector<string> argvList(argv, argv+argc);
  argvList.push_back("-rose:skipfinalCompileStep");
  SgProject* sageProject=frontend (argvList); 
  TypeTransformer tt;

  if(args.isUserProvided("explicit")) {
    tt.makeAllCastsExplicit(sageProject);
    cout<<"Converted all implicit casts to explicit casts."<<endl;
  }

  if(args.isUserProvided("stats")) {
    CastStats castStats;
    castStats.computeStats(sageProject);
    cout<<castStats.toString();
    return 0;
  }

  if(args.isUserProvided("annotate")) {
    tt.annotateImplicitCastsAsComments(sageProject);
    cout<<"Annotated program with comments."<<endl;
    backend(sageProject);
    return 0;
  }
  
  if(args.isUserProvided("dot-type-graph")) {
    string dotFileName="typegraph.dot";
    if(generateTypeGraph(sageProject,dotFileName)) {
      cout<<"Generated file "<<dotFileName<<endl;
    } else {
      cerr<<"Error: could not write file "<<dotFileName<<endl;
    }
    return 0;
  }

  if(args.isUserProvided("trace")) {
    tt.setTraceFlag(true);
  }

  if(args.isUserProvided("command-file")) {
    string changeFileName=args.getString("command-file");
    CppStdUtilities::DataFileVector lines;
    bool fileOK=CppStdUtilities::readDataFile(changeFileName,lines);
    if(fileOK) {
      TypeTransformer::VarTypeVarNameTupleList list;
      int lineNr=0;
      for (auto line : lines) {
        lineNr++;
        std::vector<std::string> splitLine=CppStdUtilities::splitByComma(line);
        string functionName,varName,typeName;
        if(splitLine.size()>=2) {
          functionName=splitLine[0];
          varName=splitLine[1];
        } 
        if(splitLine.size()==3) {
          typeName=splitLine[2];
        } else {
          typeName="float";
        }
        if(splitLine.size()>3) {
          cerr<<"Error: wrong input format in file "<<changeFileName<<". Wrong number of entries in line "<<lineNr<<"."<<endl;
          return 1;
        }
        if(functionName.size()>=2&&functionName[0]=='/' && functionName[1]=='/') {
          // line is commented out (skip)
          cout<<"Skipping "<<functionName<<","<<varName<<","<<typeName<<endl;
        } else {
          RoseAst completeast(sageProject);
          SgFunctionDefinition* funDef=completeast.findFunctionByName(functionName);
          if(typeName=="float") {
            tt.addToTransformationList(list,SageBuilder::buildFloatType(),funDef,varName);
          } else if(typeName=="double") {
            tt.addToTransformationList(list,SageBuilder::buildDoubleType(),funDef,varName);
          } else if(typeName=="long double") {
            tt.addToTransformationList(list,SageBuilder::buildLongDoubleType(),funDef,varName);
          } else {
            cerr<<"Error: unknown type "<<typeName<<" in file "<<changeFileName<<" in line "<<lineNr<<"."<<endl;
          }
        }
      } 
      tt.transformCommandLineFiles(sageProject,list);
      backend(sageProject);
      return 0;
    } else {
      cerr<<"Error: could not access file "<<changeFileName<<endl;
      return 1;
    }
  }

  if(args.isUserProvided("float-var")||args.isUserProvided("double-var")||args.isUserProvided("long-double-var")) {
    TypeTransformer::VarTypeVarNameTupleList list;
    SgFunctionDefinition* funDef=nullptr;
    if(args.isUserProvided("float-var")) {
      string varNames=args.getString("float-var");
      tt.addToTransformationList(list,SageBuilder::buildFloatType(),funDef,varNames);
    }
    if(args.isUserProvided("double-var")) {
      string varNames=args.getString("double-var");
      tt.addToTransformationList(list,SageBuilder::buildDoubleType(),funDef,varNames);
    } 
    if(args.isUserProvided("long-double-var")) {
      string varNames=args.getString("long-double-var");
      tt.addToTransformationList(list,SageBuilder::buildLongDoubleType(),funDef,varNames);
    }
    tt.transformCommandLineFiles(sageProject,list);
    backend(sageProject);
  }
  
  return 0;
}

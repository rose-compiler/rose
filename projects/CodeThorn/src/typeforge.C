
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

bool isComment(string s) {
  return s.size()>=2 && s[0]=='/' && s[1]=='/';
}

SgType* buildTypeFromStringSpec(string typeName,SgFunctionDefinition* funDef) {
  SgType* newType=nullptr;
  if(typeName=="float") {
    newType=SageBuilder::buildFloatType();
  } else if(typeName=="double") {
    newType=SageBuilder::buildDoubleType();
  } else if(typeName=="long double") {
    newType=SageBuilder::buildLongDoubleType();
  } else if(typeName=="AD_real") {
    string typeName="AD_real";
    SgScopeStatement* scope=funDef->get_scope();
    newType=SageBuilder::buildOpaqueType(typeName, scope);
  } else {
    cerr<<"Error: unsupported type: "<<typeName<<"."<<endl;
    exit(1);
  }
  return newType;
}


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
    cout<<toolName<<" version 0.3.0"<<endl;
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
    backend(sageProject);
    return 0;
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
    string commandFileName=args.getString("command-file");
    CppStdUtilities::DataFileVector lines;
    bool fileOK=CppStdUtilities::readDataFile(commandFileName,lines);
    if(fileOK) {
      TypeTransformer::VarTypeVarNameTupleList list;
      int lineNr=0;
      for (auto line : lines) {
        lineNr++;
        std::vector<std::string> splitLine=CppStdUtilities::splitByComma(line);
        string commandName,functionName,varName,typeName;
        size_t numEntries=splitLine.size();
        if(numEntries<=2 || numEntries>=5) {
          cerr<<"Error: wrong input format in file "<<commandFileName<<". Wrong number of entries in line "<<lineNr<<"."<<endl;
          return 1;
        }
        commandName=splitLine[0];
        functionName=splitLine[1];
        if(isComment(commandName)) {
          // line is commented out (skip)
          cout<<"Skipping line "<<lineNr<<endl;
          continue;
        }
        if(commandName=="replace_vartype") {
          varName=splitLine[2];
          if(numEntries==4) {
            typeName=splitLine[3];
          } else {
            typeName="float";
          }
          RoseAst completeast(sageProject);
          SgFunctionDefinition* funDef=completeast.findFunctionByName(functionName);
          if(funDef==0) {
            cerr<<"Error: function "<<functionName<<" does not exist in file."<<endl;
            return 1;
          }
          SgType* newType=buildTypeFromStringSpec(typeName,funDef);
          if(newType==nullptr) {
            cerr<<"Error: unknown type "<<typeName<<" in command file "<<commandFileName<<" in line "<<lineNr<<"."<<endl;
            return 1;
          } else {
            tt.addToTransformationList(list,newType,funDef,varName);
          }
        } else if(commandName=="replace_type") {
          cout<<"INFO: replace_type mode: "<< "in line "<<lineNr<<"."<<endl;
        } else {
          cerr<<"Error: unknown command "<<commandName<<" in line "<<lineNr<<"."<<endl;
          return 1;
        }
      }
      tt.transformCommandLineFiles(sageProject,list);
      backend(sageProject);
      return 0;
    } else {
      cerr<<"Error: could not access file "<<commandFileName<<endl;
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


#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <map>

#include "rose.h"
#include "AstTerm.h"
#include "SgNodeHelper.h"
#include "CommandLineOptions.h"
#include "AstProcessing.h"
#include "AstMatching.h"
#include "Sawyer/Graph.h"
#include "TFTypeTransformer.h"
#include "TFSpecFrontEnd.h"
#include "TFAnalysis.h"

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
#include <regex>
#include <algorithm>
#include <list>
#include "TFTransformation.h"

using namespace std;

class TestTraversal : public AstSimpleProcessing {
public:
  virtual void visit(SgNode* node) { /* do nothing */ };
};


string toolName="typeforge";

#define EXPLICIT_VAR_FORGE
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
  po::options_description all_desc("Supported Options");

  po::options_description desc("Supported Options");
  
  po::options_description hidden_desc("Hidden Options");

  desc.add_options()
    ("help,h", "Produce this help message.")
    ("version,v", "Display the version of Typeforge.")
    ("compile", "Run back end compiler.")
    //("annotate", "annotate implicit casts as comments.")
    ("explicit", "Make all implicit casts explicit.")
    ("stats", "Print statistics on casts of built-in floating point types.")
    ("trace", "Print program transformation operations as they are performed.")
    ("set-analysis", "Perform set analysis to determine which variables must be changed together.")
    //    ("dot-type-graph", "generate typegraph in dot file 'typegraph.dot'.")
    ("spec-file", po::value<vector<string> >(),"Name of Typeforge specification file.")
    ("csv-stats-file", po::value< string >(),"Generate file [args] with transformation statistics.")
#ifdef EXPLICIT_VAR_FORGE
    ("float-var", po::value< string >(),"Change type of var [arg] to float.")
    ("double-var", po::value< string >(),"Change type of var [arg] to double.")
    ("long-double-var", po::value< string >(),"Change type of var [arg] to long double.")
#endif
    ;

  hidden_desc.add_options()
    ("source-file", po::value<vector<string> >(),"Name of source files.");

  all_desc.add(desc).add(hidden_desc);

  po::positional_options_description pos;
  pos.add("source-file", -1);
  po::parsed_options parsed = po::command_line_parser(argc, argv).options(all_desc).positional(pos).allow_unregistered().run();
  po::store(parsed, args);
  po::notify(args);

  if (args.count("help")) {
    cout << toolName <<" <filename> [OPTIONS]"<<endl;
    cout << desc << "\n";
    exit(0);
  }

  if(args.isUserProvided("version")) {
    cout<<toolName<<" version 0.4.0"<<endl;
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

  bool objectFiles = false;
  for(auto file : args["source-file"].as< vector<string> >()){
    boost::filesystem::path pathObj(file);
    if(pathObj.has_extension()){
      if(pathObj.extension().string() == ".o"){
        objectFiles = true;
      }
    }
  }  
 
  vector<string> argvList = po::collect_unrecognized(parsed.options, po::include_positional); 
  argvList.insert(argvList.begin(), "rose");
  if(!args.count("compile")) argvList.push_back("-rose:skipfinalCompileStep");
  //for(auto str : argvList) cout<<str<<"\n";
  SgProject* sageProject=frontend (argvList); 
  TFTypeTransformer tt;

  if(args.isUserProvided("set-analysis")){
    TFAnalysis analysis;
    analysis.variableSetAnalysis(sageProject);
    analysis.writeAnalysis("");    
  }

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

#if 0  
  if(args.isUserProvided("dot-type-graph")) {
    string dotFileName="typegraph.dot";
    if(generateTypeGraph(sageProject,dotFileName)) {
      cout<<"Generated file "<<dotFileName<<endl;
    } else {
      cerr<<"Error: could not write file "<<dotFileName<<endl;
    }
    return 0;
  }
#endif

  if(args.isUserProvided("trace")) {
    tt.setTraceFlag(true);
  }
  if(args.isUserProvided("spec-file") && !objectFiles) {
    //Setup phase
    //string commandFileName=args.getString("spec-file");
    TFTransformation tfTransformation;
    tfTransformation.trace=tt.getTraceFlag();
    TFSpecFrontEnd typeforgeSpecFrontEnd;
    for(auto commandFileName : args["spec-file"].as<vector<string>>()){
      bool error=typeforgeSpecFrontEnd.parse(commandFileName);
      if(error) {
        exit(1);
      }
    }
    typeforgeSpecFrontEnd.run(sageProject, tt, tfTransformation);
    auto list=typeforgeSpecFrontEnd.getTransformationList();
    //Analysis Phase
    tt.analyzeTransformations(sageProject,list);
    tfTransformation.transformationAnalyze(sageProject);
    //Execution Phase
    tt.executeTransformations(sageProject);
    tfTransformation.transformationExecution();
    //Output Phase
    if(args.isUserProvided("csv-stats-file")) {
      string csvFileName=args.getString("csv-stats-file");
      tt.generateCsvTransformationStats(csvFileName,
					typeforgeSpecFrontEnd.getNumTypeReplace(),
					tt,
					tfTransformation);
    }
    tt.printTransformationStats(typeforgeSpecFrontEnd.getNumTypeReplace(),
				tt,
				tfTransformation);
    backend(sageProject);
    return 0;
  }
  else{
    backend(sageProject);
    return 0;
  }

#ifdef EXPLICIT_VAR_FORGE
  if(args.isUserProvided("float-var")||args.isUserProvided("double-var")||args.isUserProvided("long-double-var")) {
    TFTypeTransformer::VarTypeVarNameTupleList list;
    SgFunctionDefinition* funDef=nullptr;
    if(args.isUserProvided("float-var")) {
      string varNames=args.getString("float-var");
      tt.addNameTransformationToList(list,SageBuilder::buildFloatType(),funDef,varNames);
    }
    if(args.isUserProvided("double-var")) {
      string varNames=args.getString("double-var");
      tt.addNameTransformationToList(list,SageBuilder::buildDoubleType(),funDef,varNames);
    } 
    if(args.isUserProvided("long-double-var")) {
      string varNames=args.getString("long-double-var");
      tt.addNameTransformationToList(list,SageBuilder::buildLongDoubleType(),funDef,varNames);
    }
    tt.transformCommandLineFiles(sageProject,list);
    backend(sageProject);
  }
#endif  
  return 0;
}

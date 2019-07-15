
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
#include "SpecFrontEnd.h"
#include "Analysis.h"
#include "TFToolConfig.h"

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
using namespace Typeforge;

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
    ("cast-stats", "Print statistics on casts of built-in floating point types.")
    ("trace", "Print program transformation operations as they are performed.")
    ("plugin", po::value<vector<string> >(),"Name of Typeforge plugin files.")
    //    ("dot-type-graph", "generate typegraph in dot file 'typegraph.dot'.")
    ("csv-stats-file", po::value< string >(),"Generate file [args] with transformation statistics.")
    ("typeforge-out", po::value< string >(),"File to store output inside of JSON.")
    ("stats", "Print statistics on performed changes to the program.")
    ;

  hidden_desc.add_options()
    ("source-file", po::value<vector<string> >(),"Name of source files.")
    ("set-analysis", "Perform set analysis to determine which variables must be changed together.")
    ("spec-file", po::value<vector<string> >(),"Name of Typeforge specification file.")
    ;

  all_desc.add(desc).add(hidden_desc);

  po::positional_options_description pos;
  pos.add("source-file", -1);
  po::parsed_options parsed = po::command_line_parser(argc, argv).options(all_desc).positional(pos).allow_unregistered().run();
  po::store(parsed, args);
  po::notify(args);

  if (argc == 1) {
    exit(0);
  }

  if (args.count("help")) {
    cout << toolName <<" <filename> [OPTIONS]"<<endl;
    cout << desc << "\n";
    exit(0);
  }

  if(args.isUserProvided("version")) {
    cout<<toolName<<" version 0.8.4"<<endl;
    return 0;
  }

  //check if given object files for linking allow 1 to account for -o to gcc
  int objectFiles = 0;
  for(auto file : args["source-file"].as< vector<string> >()){
    boost::filesystem::path pathObj(file);
    if(pathObj.has_extension()){
      if(pathObj.extension().string() == ".o"){
        objectFiles += 1;
      }
    }
  }  
  if(objectFiles <=1) objectFiles = 0;
 
  vector<string> argvList = po::collect_unrecognized(parsed.options, po::include_positional); 
  argvList.insert(argvList.begin(), "rose");
  if(!args.count("compile")) argvList.push_back("-rose:skipfinalCompileStep");
  //for(auto str : argvList) cout<<str<<"\n";
  SgProject* sageProject=frontend (argvList); 
  TFTypeTransformer tt;

  if(args.isUserProvided("typeforge-out")){
    TFToolConfig::open(args["typeforge-out"].as<string>());
  }

  if(args.isUserProvided("set-analysis")){
    Analysis analysis;
    analysis.variableSetAnalysis(sageProject, SageBuilder::buildDoubleType(), true);
    analysis.writeAnalysis(SageBuilder::buildDoubleType(), "float");    
    analysis.writeGraph("dotGraph.gv");
    TFToolConfig::write();    
  }

  if(args.isUserProvided("explicit")) {
    tt.makeAllCastsExplicit(sageProject);
    cout<<"Converted all implicit casts to explicit casts."<<endl;
    backend(sageProject);
    return 0;
  }

  if(args.isUserProvided("cast-stats")) {
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
  
  vector<string> plugins;
  if(args.isUserProvided("plugin")){
    vector<string> plugin = args["plugin"].as<vector<string>>();
    plugins.insert(plugins.end(), plugin.begin(), plugin.end());
  }

  if(args.isUserProvided("spec-file")){
    vector<string> spec = args["spec-file"].as<vector<string>>();
    plugins.insert(plugins.end(), spec.begin(), spec.end());
  }

  if(plugins.size() > 0 && !objectFiles) {
    //Setup phase
    TFTransformation tfTransformation;
    tfTransformation.trace=tt.getTraceFlag();
    SpecFrontEnd typeforgeSpecFrontEnd;
    for(auto commandFileName : plugins){
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
    if(args.isUserProvided("stats")) {
      tt.printTransformationStats(typeforgeSpecFrontEnd.getNumTypeReplace(),
                                  tt,
                                  tfTransformation);
    }
  
    TFToolConfig::write();    
    return backend(sageProject);
  }
  else{
    return backend(sageProject);
  }

  return 0;
}

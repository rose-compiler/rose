// Author: Markus Schordan, 2013.

#include "rose.h"

#include "inliner.h"

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "AstAnnotator.h"
#include "Miscellaneous.h"
#include "ProgramStats.h"
#include "CommandLineOptions.h"
#include "AnalysisAbstractionLayer.h"
#include "AType.h"
#include "SgNodeHelper.h"
#include "FIConstAnalysis.h"
#include "TrivialInlining.h"
#include "Threadification.h"
#include "RewriteSystem.h"

#include <vector>
#include <set>
#include <list>
#include <string>

#include "limits.h"
#include <cmath>
#include "assert.h"

using namespace std;
using namespace CodeThorn;
using namespace AType;

#include "PropertyValueTable.h"
#include "DeadCodeElimination.h"
#include "ReachabilityAnalysis.h"

#include "ConversionFunctionsGenerator.h"

//static  VariableIdSet variablesOfInterest;
static bool detailedOutput=0;
const char* csvAssertFileName=0;
const char* csvConstResultFileName=0;
bool global_option_multiconstanalysis=false;

#if 0
bool isVariableOfInterest(VariableId varId) {
  return variablesOfInterest.find(varId)!=variablesOfInterest.end();
}
#endif

size_t numberOfFunctions(SgNode* node) {
  RoseAst ast(node);
  size_t num=0;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
    if(isSgFunctionDefinition(*i))
      num++;
  }
  return num;
}

void printResult(VariableIdMapping& variableIdMapping, VarConstSetMap& map) {
  cout<<"Result:"<<endl;
  VariableConstInfo vci(&variableIdMapping, &map);
  for(VarConstSetMap::iterator i=map.begin();i!=map.end();++i) {
    VariableId varId=(*i).first;
    //string variableName=variableIdMapping.uniqueShortVariableName(varId);
    string variableName=variableIdMapping.variableName(varId);
    set<CppCapsuleConstIntLattice> valueSet=(*i).second;
    stringstream setstr;
    setstr<<"{";
    for(set<CppCapsuleConstIntLattice>::iterator i=valueSet.begin();i!=valueSet.end();++i) {
      if(i!=valueSet.begin())
        setstr<<",";
      setstr<<(*i).getValue().toString();
    }
    setstr<<"}";
    cout<<variableName<<"="<<setstr.str()<<";";
#if 1
    cout<<"Range:"<<VariableConstInfo::createVariableValueRangeInfo(varId,map).toString();
    cout<<" width: "<<VariableConstInfo::createVariableValueRangeInfo(varId,map).width().toString();
    cout<<" top: "<<VariableConstInfo::createVariableValueRangeInfo(varId,map).isTop();
    cout<<endl;
#endif
    cout<<" isAny:"<<vci.isAny(varId)
        <<" isUniqueConst:"<<vci.isUniqueConst(varId)
        <<" isMultiConst:"<<vci.isMultiConst(varId);
    if(vci.isUniqueConst(varId)||vci.isMultiConst(varId)) {
      cout<<" width:"<<vci.width(varId);
    } else {
      cout<<" width:unknown";
    }
    cout<<" Test34:"<<vci.isInConstSet(varId,34);
    cout<<endl;
  }
  cout<<"---------------------"<<endl;
}


void printCodeStatistics(SgNode* root) {
  SgProject* project=isSgProject(root);
  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(project);
  VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,&variableIdMapping);
  DeadCodeElimination dce;
  cout<<"----------------------------------------------------------------------"<<endl;
  cout<<"Statistics:"<<endl;
  cout<<"Number of empty if-statements: "<<dce.listOfEmptyIfStmts(root).size()<<endl;
  cout<<"Number of functions          : "<<SgNodeHelper::listOfFunctionDefinitions(project).size()<<endl;
  cout<<"Number of global variables   : "<<SgNodeHelper::listOfGlobalVars(project).size()<<endl;
  cout<<"Number of used variables     : "<<setOfUsedVars.size()<<endl;
  cout<<"----------------------------------------------------------------------"<<endl;
  cout<<"VariableIdMapping-size       : "<<variableIdMapping.getVariableIdSet().size()<<endl;
  cout<<"----------------------------------------------------------------------"<<endl;
}

int main(int argc, char* argv[]) {
  try {
    if(argc==1) {
      cout << "Error: wrong command line options."<<endl;
      exit(1);
    }
#if 0
    if(argc==3) {
      csvAssertFileName=argv[2];
      argc=2; // don't confuse ROSE command line
      cout<< "INIT: CSV-output file: "<<csvAssertFileName<<endl;
    }
#endif
  // Command line option handling.
    namespace po = boost::program_options;
  po::options_description desc
    ("Woodpecker V0.1\n"
     "Written by Markus Schordan\n"
     "Supported options");
  
  desc.add_options()
    ("help,h", "produce this help message.")
    ("rose-help", "show help for compiler frontend options.")
    ("version,v", "display the version.")
    ("stats", "display code statistics.")
    ("normalize", po::value< string >(), "normalize code (eliminate compound assignment operators).")
    ("inline",po::value< string >(), "perform inlining ([yes]|no).")
    ("eliminate-empty-if",po::value< string >(), "eliminate if-statements with empty branches in main function ([yes]/no).")
    ("eliminate-dead-code",po::value< string >(), "eliminate dead code (variables and expressions) ([yes]|no).")
    ("csv-const-result",po::value< string >(), "generate csv-file [arg] with const-analysis data.")
    ("generate-transformed-code",po::value< string >(), "generate transformed code with prefix rose_ ([yes]|no).")
    ("verbose",po::value< string >(), "print detailed output during analysis and transformation (yes|[no]).")
    ("generate-conversion-functions","generate code for conversion functions between variable names and variable addresses.")
    ("csv-assert",po::value< string >(), "name of csv file with reachability assert results'")
    ("enable-multi-const-analysis",po::value< string >(), "enable multi-const analysis.")
    ("transform-thread-variable", "transform code to use additional thread variable.")
    ;
  //    ("int-option",po::value< int >(),"option info")


  po::store(po::command_line_parser(argc, argv).
            options(desc).allow_unregistered().run(), args);
  po::notify(args);

  if (args.count("help")) {
    cout << "woodpecker <filename> [OPTIONS]"<<endl;
    cout << desc << "\n";
    return 0;
  }
  if (args.count("rose-help")) {
    argv[1] = strdup("--help");
  }

  if (args.count("version")) {
    cout << "Woodpecker version 0.1\n";
    cout << "Written by Markus Schordan 2013\n";
    return 0;
  }
  if (args.count("csv-assert")) {
    csvAssertFileName=args["csv-assert"].as<string>().c_str();
  }
  if (args.count("csv-const-result")) {
    csvConstResultFileName=args["csv-const-result"].as<string>().c_str();
  }
  
  boolOptions.init(argc,argv);
  // temporary fake optinos
  boolOptions.registerOption("arith-top",false); // temporary
  boolOptions.registerOption("semantic-fold",false); // temporary
  boolOptions.registerOption("post-semantic-fold",false); // temporary
  // regular options
  boolOptions.registerOption("normalize",false);
  boolOptions.registerOption("inline",true);
  boolOptions.registerOption("eliminate-empty-if",true);
  boolOptions.registerOption("eliminate-dead-code",true);
  boolOptions.registerOption("generate-transformed-code",true);
  boolOptions.registerOption("enable-multi-const-analysis",false);
  boolOptions.registerOption("verbose",false);
  boolOptions.processOptions();

  if(boolOptions["verbose"])
    detailedOutput=1;

  // clean up string-options in argv
  for (int i=1; i<argc; ++i) {
    if (string(argv[i]) == "--csv-assert" 
        || string(argv[i]) == "--csv-const-result"
        ) {
      // do not confuse ROSE frontend
      argv[i] = strdup("");
      assert(i+1<argc);
        argv[i+1] = strdup("");
    }
  }

  global_option_multiconstanalysis=boolOptions["enable-multi-const-analysis"];
#if 0
  if(global_option_multiconstanalysis) {
    cout<<"INFO: Using flow-insensitive multi-const-analysis."<<endl;
  } else {
    cout<<"INFO: Using flow-insensitive unique-const-analysis."<<endl;
  }
#endif

  cout << "INIT: Parsing and creating AST started."<<endl;
  SgProject* root = frontend(argc,argv);
  //  AstTests::runAllTests(root);
  // inline all functions
  cout << "INIT: Parsing and creating AST finished."<<endl;

  if(args.count("stats")) {
    printCodeStatistics(root);
    exit(0);
  }

  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(root);

  if(args.count("transform-thread-variable")) {
    Threadification* threadTransformation=new Threadification(&variableIdMapping);
    threadTransformation->transform(root);
    root->unparse(0,0);
    delete threadTransformation;
    cout<<"STATUS: generated program with introduced thread-variable."<<endl;
    exit(0);
  }

  SgFunctionDefinition* mainFunctionRoot=0;
  if(boolOptions["inline"]) {
    cout<<"STATUS: eliminating non-called trivial functions."<<endl;
    // inline functions
    TrivialInlining tin;
    tin.setDetailedOutput(detailedOutput);
    tin.inlineFunctions(root);
    DeadCodeElimination dce;
    // eliminate non called functions
    int numEliminatedFunctions=dce.eliminateNonCalledTrivialFunctions(root);
    cout<<"STATUS: eliminated "<<numEliminatedFunctions<<" functions."<<endl;
  } else {
    cout<<"INFO: Inlining: turned off."<<endl;
  }

  if(boolOptions["eliminate-empty-if"]) {
    DeadCodeElimination dce;
    cout<<"STATUS: Eliminating empty if-statements."<<endl;
    size_t num=0;
    size_t numTotal=num;
    do {
      num=dce.eliminateEmptyIfStmts(root);
      cout<<"INFO: Number of if-statements eliminated: "<<num<<endl;
      numTotal+=num;
    } while(num>0);
    cout<<"STATUS: Total number of empty if-statements eliminated: "<<numTotal<<endl;
  }

  if(boolOptions["normalize"]) {
    cout <<"STATUS: Normalization started."<<endl;
    RewriteSystem rewriteSystem;
    rewriteSystem.resetStatistics();
    rewriteSystem.rewriteCompoundAssignmentsInAst(root,&variableIdMapping);
    cout <<"STATUS: Normalization finished."<<endl;

  }
 
  cout<<"STATUS: performing flow-insensitive const analysis."<<endl;
  VarConstSetMap varConstSetMap;
  VariableIdSet variablesOfInterest;
  FIConstAnalysis fiConstAnalysis(&variableIdMapping);
  fiConstAnalysis.setOptionMultiConstAnalysis(global_option_multiconstanalysis);
  fiConstAnalysis.setDetailedOutput(detailedOutput);
  fiConstAnalysis.runAnalysis(root, mainFunctionRoot);
  variablesOfInterest=fiConstAnalysis.determinedConstantVariables();
  cout<<"INFO: variables of interest: "<<variablesOfInterest.size()<<endl;
  if(detailedOutput)
    printResult(variableIdMapping,varConstSetMap);

  if(csvConstResultFileName) {
    VariableIdSet setOfUsedVarsInFunctions=AnalysisAbstractionLayer::usedVariablesInsideFunctions(root,&variableIdMapping);
    VariableIdSet setOfUsedVarsGlobalInit=AnalysisAbstractionLayer::usedVariablesInGlobalVariableInitializers(root,&variableIdMapping);
    VariableIdSet setOfAllUsedVars = setOfUsedVarsInFunctions;
    setOfAllUsedVars.insert(setOfUsedVarsGlobalInit.begin(), setOfUsedVarsGlobalInit.end());
    cout<<"INFO: number of used vars inside functions: "<<setOfUsedVarsInFunctions.size()<<endl;
    cout<<"INFO: number of used vars in global initializations: "<<setOfUsedVarsGlobalInit.size()<<endl;
    cout<<"INFO: number of vars inside functions or in global inititializations: "<<setOfAllUsedVars.size()<<endl;
    fiConstAnalysis.filterVariables(setOfAllUsedVars);
    fiConstAnalysis.writeCvsConstResult(variableIdMapping, string(csvConstResultFileName));
  }

  if(args.count("generate-conversion-functions")) {
    string conversionFunctionsFileName="conversionFunctions.C";
    ConversionFunctionsGenerator gen;
    set<string> varNameSet;
    std::list<SgVariableDeclaration*> globalVarDeclList=SgNodeHelper::listOfGlobalVars(root);
    for(std::list<SgVariableDeclaration*>::iterator i=globalVarDeclList.begin();i!=globalVarDeclList.end();++i) {
      SgInitializedNamePtrList& initNamePtrList=(*i)->get_variables();
      for(SgInitializedNamePtrList::iterator j=initNamePtrList.begin();j!=initNamePtrList.end();++j) {
	      SgInitializedName* initName=*j;
        if ( true || isSgArrayType(initName->get_type()) ) {  // optional filter (array variables only)
	        SgName varName=initName->get_name();
	        string varNameString=varName; // implicit conversion
	        varNameSet.insert(varNameString);
        }
      }
    }
    string code=gen.generateCodeForGlobalVarAdressMaps(varNameSet);
    ofstream myfile;
    myfile.open(conversionFunctionsFileName.c_str());
    myfile<<code;
    myfile.close();
  }

  VariableConstInfo vci=*(fiConstAnalysis.getVariableConstInfo());
  DeadCodeElimination dce;
  if(boolOptions["eliminate-dead-code"]) {
    cout<<"STATUS: performing dead code elimination."<<endl;
    dce.setDetailedOutput(detailedOutput);
    dce.setVariablesOfInterest(variablesOfInterest);
    dce.eliminateDeadCodePhase1(root,&variableIdMapping,vci);
    cout<<"STATUS: Eliminated "<<dce.numElimVars()<<" variable declarations."<<endl;
    cout<<"STATUS: Eliminated "<<dce.numElimAssignments()<<" variable assignments."<<endl;
    cout<<"STATUS: Replaced "<<dce.numElimVarUses()<<" uses of variables with constant."<<endl;
    cout<<"STATUS: Eliminated "<<dce.numElimVars()<<" dead variables."<<endl;
    cout<<"STATUS: Dead code elimination phase 1: finished."<<endl;
    cout<<"STATUS: Performing condition const analysis."<<endl;
  } else {
    cout<<"STATUS: Dead code elimination: turned off."<<endl;
  }
  if(csvAssertFileName) {
    cout<<"STATUS: performing flow-insensensitive condition-const analysis."<<endl;
    Labeler labeler(root);
    fiConstAnalysis.performConditionConstAnalysis(&labeler);
    cout<<"INFO: Number of true-conditions     : "<<fiConstAnalysis.getTrueConditions().size()<<endl;
    cout<<"INFO: Number of false-conditions    : "<<fiConstAnalysis.getFalseConditions().size()<<endl;
    cout<<"INFO: Number of non-const-conditions: "<<fiConstAnalysis.getNonConstConditions().size()<<endl;
    cout<<"STATUS: performing flow-insensensitive reachability analysis."<<endl;
    ReachabilityAnalysis ra;
    PropertyValueTable reachabilityResults=ra.fiReachabilityAnalysis(labeler, fiConstAnalysis);
    cout<<"STATUS: generating file "<<csvAssertFileName<<endl;
    reachabilityResults.writeFile(csvAssertFileName,true);
  }
#if 0
  rdAnalyzer->determineExtremalLabels(startFunRoot);
  rdAnalyzer->run();
#endif
  cout << "INFO: Remaining functions in program: "<<numberOfFunctions(root)<<endl;
  if(boolOptions["generate-transformed-code"]) {
    cout << "STATUS: generating transformed source code."<<endl;
    root->unparse(0,0);
  }

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

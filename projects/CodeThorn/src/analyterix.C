// Author: Markus Schordan, 2013, 2014.

#include "rose.h"

#include "inliner.h"
#include "CommandLineOptions.h"
#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "WorkList.h"
#include "CFAnalysis.h"
#include "RDLattice.h"
#include "DFAnalysisBase.h"
#include "RDAnalysis.h"
#include "RoseRDAnalysis.h"
#include "LVAnalysis.h"
#include "IntervalAnalysis.h"
#include "RDAstAttribute.h"
#include "AstAnnotator.h"
#include "AnalysisAstAnnotator.h"
#include "DataDependenceVisualizer.h"
#include "Miscellaneous.h"
#include "ProgramStats.h"
#include "AnalysisAbstractionLayer.h"
#include "AType.h"
#include "SgNodeHelper.h"
#include "DFAstAttributeConversion.h"
#include "FIConstAnalysis.h"
#include "FIPointerAnalysis.h"
#include <boost/foreach.hpp>

#include "addressTakenAnalysis.h"
#include "defUseQuery.h"
#include "Timer.h"
#include "AnalysisAbstractionLayer.h"
#include "AliasAnalysis.h"

#include "AstTerm.h"

#include <vector>
#include <set>
#include <list>
#include <string>

#include "limits.h"
#include <cmath>
#include "assert.h"

// ROSE analyses
#include "VariableRenaming.h"

// temporary
#include "IntervalTransferFunctions.h"

using namespace std;
using namespace CodeThorn;
using namespace AType;
using namespace DFAstAttributeConversion;
using namespace AnalysisAbstractionLayer;

#include "PropertyValueTable.h"

string option_prefix;
bool option_stats=false;
bool option_generalanalysis=false;
bool option_rose_rd_analysis=false;
bool option_fi_constanalysis=false;
const char* csvConstResultFileName=0;
bool option_rd_analysis=false;
bool option_ud_analysis=false;
bool option_lv_analysis=false;
bool option_interval_analysis=false;
bool option_check_static_array_bounds=false;
bool option_at_analysis=false;
bool option_trace=false;
bool option_optimize_icfg=false;

//boost::program_options::variables_map args;

void writeFile(std::string filename, std::string data) {
  std::ofstream myfile;
  myfile.open(filename.c_str(),std::ios::out);
  myfile << data;
  myfile.close();
}

void generateRessourceUsageVis(RDAnalysis* rdAnalyzer) {
  cout << "INFO: computing program statistics."<<endl;
  SPRAY::ProgramStatistics ps(rdAnalyzer->getVariableIdMapping(),
                       rdAnalyzer->getLabeler(), 
                       rdAnalyzer->getFlow(),
                       "ud-analysis");
  ps.computeStatistics();
  //ps.printStatistics();
  cout << "INFO: generating resource usage visualization."<<endl;
  ps.setGenerateWithSource(false);
  ps.generateResourceUsageICFGDotFile("resourceusageicfg.dot");
  rdAnalyzer->getFlow()->resetDotOptions();
}

void checkStaticArrayBounds(SgProject* root, SPRAY::IntervalAnalysis* intervalAnalysis) {
  cout<<"STATUS: checking static array bounds."<<endl;
  SPRAY::Labeler* labeler=intervalAnalysis->getLabeler();
  for(Labeler::iterator j=labeler->begin();j!=labeler->end();++j) {
    SgNode* node=labeler->getNode(*j);
    std::string lineCol=SgNodeHelper::sourceLineColumnToString(node);
    if(isSgStatement(node)&&!isSgFunctionDefinition(node)&&!isSgBasicBlock(node)) {
      RoseAst ast(node);
      for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
        if(SgPntrArrRefExp* arrRefExp=isSgPntrArrRefExp(*i)) {
          SgNode* lhs=SgNodeHelper::getLhs(arrRefExp);
          SgNode* rhs=SgNodeHelper::getRhs(arrRefExp);
          // go to the right most expression of "x->y->...->z[i]"
          while(isSgArrowExp(lhs)) {
            lhs=SgNodeHelper::getRhs(isSgArrowExp(lhs));
          }
          SgVarRefExp* arrayVar=isSgVarRefExp(lhs);
          SgVarRefExp* indexVar=isSgVarRefExp(rhs);
          if(arrayVar&&indexVar) {
            VariableIdMapping* variableIdMapping=intervalAnalysis->getVariableIdMapping();
            VariableId arrayVarId=variableIdMapping->variableId(arrayVar);
            VariableId indexVarId=variableIdMapping->variableId(indexVar);
            IntervalPropertyState* intervalPropertyState=dynamic_cast<IntervalPropertyState*>(intervalAnalysis->getPreInfo(*j));
            ROSE_ASSERT(intervalPropertyState);
            if(!variableIdMapping->hasArrayType(arrayVarId)) {
              cerr<<"Internal error: determined array variable, but it is not registered as array variable."<<endl;
              exit(1);
            }
            size_t arraySize=variableIdMapping->getSize(arrayVarId);
            if(intervalPropertyState->variableExists(indexVarId)) {
              NumberIntervalLattice indexVariableInterval=intervalPropertyState->getVariable(indexVarId);
              if(indexVariableInterval.isTop()
                 ||indexVariableInterval.getLow()<0
                 ||indexVariableInterval.getHigh()>(arraySize-1)) {
                cout<<"DETECTED: array out of bounds access: "<<lineCol
                    <<": "<<node->unparseToString()
                    <<" ("
                    <<variableIdMapping->variableName(indexVarId)
                    <<" in "<<indexVariableInterval.toString()
                    <<" accessing array '"<<variableIdMapping->variableName(arrayVarId)<<"'"
                    <<" of size "<<arraySize
                    <<")"
                    <<endl;
              }
            } else if(intervalPropertyState->isBot()) {
              cout<<"ANALYSIS: not reachable: "<<node->unparseToString()<<endl;
              // nothing to do
            } else {
              cout<<"Error: variable "<<indexVarId.toString()<<" does not exist in property state."<<endl;
              exit(1);
            }
          } else {
            cerr<<"WARNING: Unsupported array access expression: ";
            cerr<<SPRAY::AstTerm::astTermWithNullValuesToString(arrRefExp)<<endl;
          }
        }
      }
    }
  }
}

void runAnalyses(SgProject* root, Labeler* labeler, VariableIdMapping* variableIdMapping) {

  SPRAY::DFAnalysisBase::normalizeProgram(root);

  if(option_fi_constanalysis) {
    VarConstSetMap varConstSetMap;
    FIConstAnalysis fiConstAnalysis(variableIdMapping);
    fiConstAnalysis.runAnalysis(root);
    fiConstAnalysis.attachAstAttributes(labeler,"const-analysis-inout"); // not iolabeler
    if(csvConstResultFileName) {
      cout<<"INFO: generating const CSV file "<<option_prefix+csvConstResultFileName<<endl;
      fiConstAnalysis.writeCvsConstResult(*variableIdMapping, option_prefix+csvConstResultFileName);
    }
    cout << "INFO: annotating analysis results as comments."<<endl;
    AstAnnotator ara(labeler);
    ara.annotateAstAttributesAsCommentsBeforeStatements(root, "const-analysis-inout");
    ara.annotateAstAttributesAsCommentsAfterStatements(root, "const-analysis-inout");
  }

  if(option_at_analysis) {
    cout<<"STATUS: running address taken analysis."<<endl;
    // compute variableId mappings
    VariableIdMapping variableIdMapping;
    variableIdMapping.computeVariableSymbolMapping(root);
    SPRAY::FIPointerAnalysis fipa(&variableIdMapping,root);
    fipa.initialize();
    fipa.run();
#if 0
    VariableIdSet vidset=fipa.getModByPointer();
    cout<<"mod-set: "<<SPRAY::VariableIdSetPrettyPrint::str(vidset,variableIdMapping)<<endl;
#endif
  }
  
  if(option_interval_analysis) {
    cout << "STATUS: creating interval analyzer."<<endl;
    SPRAY::IntervalAnalysis* intervalAnalyzer=new SPRAY::IntervalAnalysis();
    cout << "STATUS: initializing interval analyzer."<<endl;
    intervalAnalyzer->initialize(root);
    cout << "STATUS: running pointer analysis."<<endl;
    ROSE_ASSERT(intervalAnalyzer->getVariableIdMapping());
    SPRAY::FIPointerAnalysis* fipa=new FIPointerAnalysis(intervalAnalyzer->getVariableIdMapping(),root);
    fipa->initialize();
    fipa->run();
    intervalAnalyzer->setPointerAnalysis(fipa);
    cout << "STATUS: initializing interval transfer functions."<<endl;
    intervalAnalyzer->initializeTransferFunctions();
    cout << "STATUS: initializing interval global variables."<<endl;
    intervalAnalyzer->initializeGlobalVariables(root);
      
    intervalAnalyzer->setSolverTrace(option_trace);
    std::string funtofind="main";
    RoseAst completeast(root);
    SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
    intervalAnalyzer->determineExtremalLabels(startFunRoot);
    intervalAnalyzer->run();

#if 0
    intervalAnalyzer->attachInInfoToAst("iv-analysis-in");
    intervalAnalyzer->attachOutInfoToAst("iv-analysis-out");
    AstAnnotator ara(intervalAnalyzer->getLabeler(),intervalAnalyzer->getVariableIdMapping());
    ara.annotateAstAttributesAsCommentsBeforeStatements(root, "iv-analysis-in");
    ara.annotateAstAttributesAsCommentsAfterStatements(root, "iv-analysis-out");
#else
    AnalysisAstAnnotator ara(intervalAnalyzer->getLabeler(),intervalAnalyzer->getVariableIdMapping());
    ara.annotateAnalysisPrePostInfoAsComments(root,"iv-analysis",intervalAnalyzer);
#endif
    if(option_check_static_array_bounds) {
      checkStaticArrayBounds(root,intervalAnalyzer);
    }

    delete fipa;
  }

  if(option_lv_analysis) {
    cout << "STATUS: creating LV analysis."<<endl;
    SPRAY::LVAnalysis* lvAnalysis=new SPRAY::LVAnalysis();
    cout << "STATUS: initializing LV analysis."<<endl;
    lvAnalysis->setBackwardAnalysis();
    lvAnalysis->initialize(root);
    cout << "STATUS: initializing LV transfer functions."<<endl;
    lvAnalysis->initializeTransferFunctions();
    cout << "STATUS: initializing LV global variables."<<endl;
    lvAnalysis->initializeGlobalVariables(root);
    std::string funtofind="main";
    RoseAst completeast(root);
    SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
    cout << "generating icfg_backward.dot."<<endl;
    write_file("icfg_backward.dot", lvAnalysis->getFlow()->toDot(lvAnalysis->getLabeler()));

    lvAnalysis->determineExtremalLabels(startFunRoot);
    lvAnalysis->run();
    cout << "INFO: attaching LV-data to AST."<<endl;
#if 0
    lvAnalysis->attachInInfoToAst("lv-analysis-in");
    lvAnalysis->attachOutInfoToAst("lv-analysis-out");
    AstAnnotator ara(lvAnalysis->getLabeler(),lvAnalysis->getVariableIdMapping());
    ara.annotateAstAttributesAsCommentsBeforeStatements(root, "lv-analysis-in");
    ara.annotateAstAttributesAsCommentsAfterStatements(root, "lv-analysis-out");
#else
    AnalysisAstAnnotator ara(lvAnalysis->getLabeler(),lvAnalysis->getVariableIdMapping());
    ara.annotateAnalysisPrePostInfoAsComments(root,"lv-analysis",lvAnalysis);
#endif
    delete lvAnalysis;
  }

  if(option_rd_analysis) {
      cout << "STATUS: creating RD analyzer."<<endl;
      SPRAY::RDAnalysis* rdAnalysis=new SPRAY::RDAnalysis();
      cout << "STATUS: initializing RD analyzer."<<endl;
      rdAnalysis->initialize(root);
      cout << "STATUS: initializing RD transfer functions."<<endl;
      rdAnalysis->initializeTransferFunctions();
      cout << "STATUS: initializing RD global variables."<<endl;
      rdAnalysis->initializeGlobalVariables(root);
      
      cout << "generating icfg_forward.dot."<<endl;
      write_file("icfg_forward.dot", rdAnalysis->getFlow()->toDot(rdAnalysis->getLabeler()));
    
      std::string funtofind="main";
      RoseAst completeast(root);
      SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
      rdAnalysis->determineExtremalLabels(startFunRoot);
      rdAnalysis->run();
    
      cout << "INFO: attaching RD-data to AST."<<endl;
      rdAnalysis->attachInInfoToAst("rd-analysis-in");
      rdAnalysis->attachOutInfoToAst("rd-analysis-out");
      //printAttributes<RDAstAttribute>(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping(),"rd-analysis-in");
      cout << "INFO: annotating analysis results as comments."<<endl;
      ROSE_ASSERT(rdAnalysis->getVariableIdMapping());
#if 0
      AstAnnotator ara(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping());
      ara.annotateAstAttributesAsCommentsBeforeStatements(root, "rd-analysis-in");
      ara.annotateAstAttributesAsCommentsAfterStatements(root, "rd-analysis-out");
#else
      AnalysisAstAnnotator ara(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping());
      ara.annotateAnalysisPrePostInfoAsComments(root,"rd-analysis",rdAnalysis);
#endif

#if 0
      cout << "INFO: substituting uses with rhs of defs."<<endl;
      substituteUsesWithAvailableExpRhsOfDef("ud-analysis", root, rdAnalysis->getLabeler(), rdAnalysis->getVariableIdMapping());
#endif
      if(option_ud_analysis) {
        ROSE_ASSERT(option_rd_analysis);
        cout << "INFO: generating and attaching UD-data to AST."<<endl;
        createUDAstAttributeFromRDAttribute(rdAnalysis->getLabeler(),"rd-analysis-in", "ud-analysis");
        Flow* flow=rdAnalysis->getFlow();
        cout<<"Flow label-set size: "<<flow->nodeLabels().size()<<endl;
        CFAnalysis* cfAnalyzer0=rdAnalysis->getCFAnalyzer();
        int red=cfAnalyzer0->reduceBlockBeginNodes(*flow);
        cout<<"INFO: eliminated "<<red<<" block-begin nodes in ICFG."<<endl;
        
#if 0
        cout << "INFO: computing program statistics."<<endl;
        ProgramStatistics ps(rdAnalysis->getVariableIdMapping(),
                             rdAnalysis->getLabeler(), 
                             rdAnalysis->getFlow(),
                             "ud-analysis");
        ps.computeStatistics();
        //ps.printStatistics();
        cout << "INFO: generating resource usage visualization."<<endl;
        ps.setGenerateWithSource(false);
        ps.generateResourceUsageICFGDotFile("resourceusageicfg.dot");
        flow->resetDotOptions();
#endif
        cout << "INFO: generating visualization data."<<endl;
        // generate ICFG visualization
        cout << "generating icfg.dot."<<endl;
        write_file("icfg.dot", flow->toDot(rdAnalysis->getLabeler()));
        
        //  cout << "INFO: generating control dependence graph."<<endl;
        //Flow cdg=rdAnalysis->getCFAnalyzer()->controlDependenceGraph(*flow);

        cout << "generating datadependencegraph.dot."<<endl;
        DataDependenceVisualizer ddvis0(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        //printAttributes<UDAstAttribute>(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping(),"ud-analysis");
        //ddvis._showSourceCode=false; // for large programs
        ddvis0.generateDefUseDotGraph(root,"datadependencegraph.dot");
        flow->resetDotOptions();
        
        cout << "generating icfgdatadependencegraph.dot."<<endl;
        DataDependenceVisualizer ddvis1(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        ddvis1.includeFlowGraphEdges(flow);
        ddvis1.generateDefUseDotGraph(root,"icfgdatadependencegraph.dot");
        flow->resetDotOptions();
        
        cout << "generating icfgdatadependencegraph_clustered.dot."<<endl;
        DataDependenceVisualizer ddvis2(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        ddvis2.generateDotFunctionClusters(root,rdAnalysis->getCFAnalyzer(),"icfgdatadependencegraph_clustered.dot",true);
        
        cout << "generating icfg_clustered.dot."<<endl;
        DataDependenceVisualizer ddvis3(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        ddvis3.generateDotFunctionClusters(root,rdAnalysis->getCFAnalyzer(),"icfg_clustered.dot",false);
        
      }
    }
}

int main(int argc, char* argv[]) {
  try {
    if(argc==1) {
      cout << "Error: wrong command line options."<<endl;
      exit(1);
    }
     // Command line option handling.
    namespace po = boost::program_options;
    po::options_description desc
      ("analyterix V0.2\n"
       "Written by Markus Schordan\n"
       "Supported options");
  
    desc.add_options()
      ("help,h", "produce this help message.")
      ("rose-help", "show help for compiler frontend options.")
      ("version,v", "display the version.")
      ("stats", "display code statistics.")
      ("fi-constanalysis", "perform flow-insensitive constant analysis.")
      ("csv-fi-constanalysis",po::value< string >(), "generate csv-file [arg] with const-analysis data.")
      ("rd-analysis", "perform reaching definitions analysis.")
      ("rose-rd-analysis", "perform rose reaching definitions analysis.")
      ("lv-analysis", "perform live variables analysis.")
      ("ud-analysis", "use-def analysis.")
      ("at-analysis", "address-taken analysis.")
      ("icfg-dot", "generates the ICFG as dot file.")
      ("optimize-icfg", "prunes conditions with empty blocks, block begin, and block end icfg nodes.")
      ("no-optmize-icfg", "does not optimize icfg.")
      ("interval-analysis", "perform interval analysis.")
      ("trace", "show operations as performed by selected solver.")
      ("check-static-array-bounds", "check static array bounds (uses interval analysis).")
      ("print-varid-mapping", "prints variableIdMapping")
      ("print-varid-mapping-array", "prints variableIdMapping with array element varids.")
      ("print-label-mapping", "prints mapping of labels to statements")
      ("prefix",po::value< string >(), "set prefix for all generated files.")
      ;
  //    ("int-option",po::value< int >(),"option info")

    po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), args);
    po::notify(args);

    if (args.count("help")) {
      cout << "analyterix <filename> [OPTIONS]"<<endl;
      cout << desc << "\n";
      return 0;
    }
    if (args.count("version")) {
      cout << "analyterix version 0.1\n";
      cout << "Written by Markus Schordan 2014\n";
      return 0;
    }
    if (args.count("rose-help")) {
      argv[1] = strdup("--help");
    }
    if (args.count("prefix")) {
      option_prefix=args["prefix"].as<string>().c_str();
    }

    if (args.count("optimize-icfg")) {
      option_optimize_icfg=true;
    }
    if (args.count("no-optimize-icfg")) {
      option_optimize_icfg=false;
    }
    if (args.count("trace")) {
      option_trace=true;
    }
    if(args.count("stats")) {
      option_stats=true;
    }
    if(args.count("rd-analysis")) {
      option_rd_analysis=true;
    }
    if(args.count("lv-analysis")) {
      option_lv_analysis=true;
    }
    if(args.count("interval-analysis")) {
      option_interval_analysis=true;
    }
    if(args.count("check-static-array-bounds")) {
      option_interval_analysis=true;
      option_check_static_array_bounds=true;
    }
    if(args.count("ud-analysis")) {
      option_rd_analysis=true; // required
      option_ud_analysis=true;
    }
    if(args.count("rose-rd-analysis")) {
      option_rose_rd_analysis=true;
    }
    if(args.count("fi-constanalysis")) {
      option_fi_constanalysis=true;
    }
    if (args.count("csv-fi-constanalysis")) {
      csvConstResultFileName=args["csv-fi-constanalysis"].as<string>().c_str();
      option_fi_constanalysis=true;
    }
    if(args.count("at-analysis")) {
      option_at_analysis=true;
    }
    // clean up string-options in argv
    for (int i=1; i<argc; ++i) {
      if (string(argv[i]) == "--prefix" 
          || string(argv[i]) == "--csv-const-result"
          ) {
        // do not confuse ROSE frontend
        argv[i] = strdup("");
        assert(i+1<argc);
        argv[i+1] = strdup("");
      }
    }

    cout << "INIT: Parsing and creating AST."<<endl;
    boolOptions.registerOption("semantic-fold",false); // temporary
    boolOptions.registerOption("post-semantic-fold",false); // temporary
    SgProject* root = frontend(argc,argv);
    //  AstTests::runAllTests(root);

   if(option_stats) {
      SPRAY::ProgramStatistics::printBasicCodeInfo(root);
    }

  cout<<"STATUS: computing variableid mapping"<<endl;
  VariableIdMapping variableIdMapping;
  if (args.count("print-varid-mapping-array")) {
    variableIdMapping.setModeVariableIdForEachArrayElement(true);
  }

  variableIdMapping.computeVariableSymbolMapping(root);
  cout<<"VariableIdMapping size: "<<variableIdMapping.getVariableIdSet().size()<<endl;
  Labeler* labeler=new Labeler(root);
  //cout<<"Labelling:\n"<<labeler->toString()<<endl;

#if 0
  IOLabeler* iolabeler=new IOLabeler(root,&variableIdMapping);
  //cout<<"IOLabelling:\n"<<iolabeler->toString()<<endl;
#endif

  if (args.count("print-varid-mapping")||args.count("print-varid-mapping-array")) {
    variableIdMapping.toStream(cout);
    return 0;
  }

  if(args.count("print-label-mapping")) {
    cout<<labeler->toString();
    return 0;
  }

  if(args.count("icfg-dot")) {
    CFAnalysis* cfAnalysis=new CFAnalysis(labeler);
    Flow flow=cfAnalysis->flow(root);
    if(option_optimize_icfg) {
      cfAnalysis->optimizeFlow(flow);
    }
    InterFlow interFlow=cfAnalysis->interFlow(flow);
    cfAnalysis->intraInterFlow(flow,interFlow);
    string dotString=flow.toDot(labeler);
    writeFile("icfg.dot",dotString);
    delete cfAnalysis;
    exit(0);
  }
  runAnalyses(root, labeler, &variableIdMapping);

  cout << "INFO: generating annotated source code."<<endl;
  root->unparse(0,0);

  if(option_rose_rd_analysis) {
    Experimental::RoseRDAnalysis::generateRoseRDDotFiles(labeler,root);
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

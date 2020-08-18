/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "rose.h"

//#include "rose_config.h"

#include "codethorn.h"
#include "SgNodeHelper.h"
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "EState.h"
#include "TimeMeasurement.h"
#include <cstdio>
#include <cstring>
#include <map>

#include "CodeThornCommandLineOptions.h"
#include "RewriteSystem.h"

#include "InternalChecks.h"
#include "AstAnnotator.h"
#include "AstTerm.h"
#include "AbstractValue.h"
#include "AstMatching.h"
#include "ltlthorn-lib/SpotConnection.h"
#include "ltlthorn-lib/CounterexampleAnalyzer.h"
#include "AstUtility.h"
#include "ArrayElementAccessData.h"
#include "PragmaHandler.h"
#include "Miscellaneous2.h"
#include "FIConstAnalysis.h"
#include "ReachabilityAnalysis.h"
#include "EquivalenceChecking.h"
#include "Solver5.h"
#include "Solver8.h"
#include "ltlthorn-lib/Solver10.h"
#include "ltlthorn-lib/Solver11.h"
#include "ltlthorn-lib/Solver12.h"
#include "ReadWriteAnalyzer.h"
#include "AnalysisParameters.h"
#include "CodeThornException.h"
#include "CodeThornException.h"
#include "ProgramInfo.h"
#include "FunctionCallMapping.h"
#include "AstStatistics.h"

#include "DataRaceDetection.h"
#include "AstTermRepresentation.h"
#include "Normalization.h"
#include "DataDependenceVisualizer.h" // also used for clustered ICFG
#include "Evaluator.h" // CppConstExprEvaluator
#include "CtxCallStrings.h" // for setting call string options
#include "AnalysisReporting.h"

// Z3-based analyser / SSA 
#include "z3-prover-connection/SSAGenerator.h"
#include "z3-prover-connection/ReachabilityAnalyzerZ3.h"

#if defined(__unix__) || defined(__unix) || defined(unix)
#include <sys/resource.h>
#endif

#include "CodeThornLib.h"
#include "LTLThornLib.h"
#include "CppStdUtilities.h"

//BOOST includes
#include "boost/lexical_cast.hpp"

using namespace std;

using namespace CodeThorn;
using namespace boost;

#include "Diagnostics.h"
using namespace Sawyer::Message;

// experimental
#include "IOSequenceGenerator.C"

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "Solver5.h"
#include "Solver8.h"
#include "ltlthorn-lib/Solver10.h"
#include "ltlthorn-lib/Solver11.h"
#include "ltlthorn-lib/Solver12.h"

const std::string versionString="1.12.12";

struct TimingCollector {
  TimeMeasurement timer;
  double frontEndRunTime=0.0;
  double normalizationRunTime=0.0;
  double determinePrefixDepthTime=0.0;
  double extractAssertionTracesTime=0.0;
  double totalInputTracesTime=0.0;
  double initRunTime=0.0;
  double analysisRunTime=0.0;
  void startTimer() { timer.start(); }
  void stopTimer() { timer.stop(); }
  void stopFrontEndTimer() { frontEndRunTime=timer.getTimeDurationAndStop().milliSeconds(); }
  void stopNormalizationTimer() { normalizationRunTime=timer.getTimeDurationAndStop().milliSeconds(); }
};

void configureRersSpecialization() {
#ifdef RERS_SPECIALIZATION
  // only included in hybrid RERS analyzers.
  // Init external function pointers for generated property state
  // marshalling functions (5 function pointers named:
  // RERS_Problem::...FP, are initialized in the following external
  // function.
  // An implementation of this function is linked with the hybrid analyzer
  extern void RERS_Problem_FunctionPointerInit();
  RERS_Problem_FunctionPointerInit();
#endif
}

void exprEvalTest(int argc, char* argv[],CodeThornOptions& ctOpt) {
  cout << "------------------------------------------"<<endl;
  cout << "RUNNING CHECKS FOR EXPR ANALYZER:"<<endl;
  cout << "------------------------------------------"<<endl;
  SgProject* sageProject=frontend(argc,argv);
  Normalization normalization;
  if(ctOpt.normalizeAll) {
    if(ctOpt.quiet==false) {
      cout<<"STATUS: normalizing program."<<endl;
    }
    normalization.normalizeAst(sageProject,2);
  }
  ExprAnalyzer* exprAnalyzer=new ExprAnalyzer();
  VariableIdMappingExtended* vid=new VariableIdMappingExtended();
  AbstractValue::setVariableIdMapping(vid);
  RoseAst ast(sageProject);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    // match on expr stmts and test the expression
    SgExpression* expr=0;
    
    // TEMPLATESKIP this skips all templates
    if(Normalization::isTemplateNode(*i)) {
      i.skipChildrenOnForward();
      continue;
    }
    if(SgExprStatement* exprStmt=isSgExprStatement(*i)) {
      if(!SgNodeHelper::isCond(exprStmt)) {
        expr=exprStmt->get_expression();
      }
    } else if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      expr=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(varDecl);
    }
    if(expr) {
      cout<<"Testing expr eval with empty state: "<<expr->unparseToString();
      ExprAnalyzer::EvalMode evalMode=ExprAnalyzer::MODE_EMPTY_STATE;
      AbstractValue aVal=exprAnalyzer->evaluateExpressionWithEmptyState(expr);
      cout<<" => result value: "<<aVal.toString()<<" "<<endl;
    }
  }
  AbstractValue::setVariableIdMapping(nullptr);
  delete vid;
  delete exprAnalyzer;
}

void optionallyRunExprEvalTestAndExit(CodeThornOptions& ctOpt,int argc, char * argv[]) {
  if(ctOpt.exprEvalTest) {
    exprEvalTest(argc,argv,ctOpt);
    exit(0);
  }
}

void processCtOptGenerateAssertions(CodeThornOptions& ctOpt, Analyzer* analyzer, SgProject* root) {
  if (ctOpt.generateAssertions) {
    AssertionExtractor assertionExtractor(analyzer);
    assertionExtractor.computeLabelVectorOfEStates();
    assertionExtractor.annotateAst();
    AstAnnotator ara(analyzer->getLabeler());
    ara.annotateAstAttributesAsCommentsBeforeStatements  (root,"ctgen-pre-condition");
    SAWYER_MESG(logger[TRACE]) << "STATUS: Generated assertions."<<endl;
  }
}

Solver* createSolver(CodeThornOptions& ctOpt) {
  Solver* solver = nullptr;
  // solver "factory"
  switch(ctOpt.solver) {
  case 5 :  {  
    solver = new Solver5(); break;
  }
  case 8 :  {  
    solver = new Solver8(); break;
  }
  case 10 :  {  
    solver = new Solver10(); break;
  }
  case 11 :  {  
    solver = new Solver11(); break;
  }
  case 12 :  {  
    solver = new Solver12(); break;
  }
  default :  { 
    logger[ERROR] <<"Unknown solver ID: "<<ctOpt.solver<<endl;
    exit(1);
  }
  }
  return solver;
}

IOAnalyzer* createAnalyzer(CodeThornOptions& ctOpt, LTLOptions& ltlOpt) {
  IOAnalyzer* analyzer;
  if(ctOpt.dr.checkShuffleAlgorithm) {
    analyzer = new ReadWriteAnalyzer();
  } else {
    analyzer = new IOAnalyzer();
  }
  analyzer->setOptions(ctOpt);
  analyzer->setLtlOptions(ltlOpt);
  return analyzer;
}

void optionallyRunInternalChecks(CodeThornOptions& ctOpt, int argc, char * argv[]) {
  if(ctOpt.internalChecks) {
    if(CodeThorn::internalChecks(argc,argv)==false) {
      mfacilities.shutdown();
      exit(1);
    } else {
      mfacilities.shutdown();
      exit(0);
    }
  }
}


void optionallyRunInliner(CodeThornOptions& ctOpt, Normalization& normalization, SgProject* sageProject) {
  if(ctOpt.inlineFunctions) {
    InlinerBase* inliner=normalization.getInliner();
    if(RoseInliner* roseInliner=dynamic_cast<CodeThorn::RoseInliner*>(inliner)) {
      roseInliner->inlineDepth=ctOpt.inlineFunctionsDepth;
    }
    inliner->inlineFunctions(sageProject);
    size_t numInlined=inliner->getNumInlinedFunctions();
    SAWYER_MESG(logger[TRACE])<<"inlined "<<numInlined<<" functions"<<endl;
  }
}

void optionallyRunVisualizer(CodeThornOptions& ctOpt, Analyzer* analyzer, SgNode* root) {
  Visualizer visualizer(analyzer->getLabeler(),analyzer->getVariableIdMapping(),analyzer->getFlow(),analyzer->getPStateSet(),analyzer->getEStateSet(),analyzer->getTransitionGraph());
  if (ctOpt.visualization.icfgFileName.size()>0) {
    string cfgFileName=ctOpt.visualization.icfgFileName;
    DataDependenceVisualizer ddvis(analyzer->getLabeler(),analyzer->getVariableIdMapping(),"none");
    ddvis.setDotGraphName("CFG");
    ddvis.generateDotFunctionClusters(root,analyzer->getCFAnalyzer(),cfgFileName,false);
    cout << "generated "<<cfgFileName<<endl;
  }
  if(ctOpt.visualization.viz) {
    cout << "generating graphviz files:"<<endl;
    visualizer.setOptionMemorySubGraphs(ctOpt.visualization.tg1EStateMemorySubgraphs);
    string dotFile="digraph G {\n";
    dotFile+=visualizer.transitionGraphToDot();
    dotFile+="}\n";
    write_file("transitiongraph1.dot", dotFile);
    cout << "generated transitiongraph1.dot."<<endl;
    string dotFile3=visualizer.foldedTransitionGraphToDot();
    write_file("transitiongraph2.dot", dotFile3);
    cout << "generated transitiongraph2.dot."<<endl;

    string datFile1=(analyzer->getTransitionGraph())->toString(analyzer->getVariableIdMapping());
    write_file("transitiongraph1.dat", datFile1);
    cout << "generated transitiongraph1.dat."<<endl;

    assert(analyzer->startFunRoot);
    //analyzer->generateAstNodeInfo(analyzer->startFunRoot);
    //dotFile=astTermWithNullValuesToDot(analyzer->startFunRoot);
    SAWYER_MESG(logger[TRACE]) << "Option VIZ: generate ast node info."<<endl;
    analyzer->generateAstNodeInfo(root);
    cout << "generating AST node info ... "<<endl;
    dotFile=AstTerm::functionAstTermsWithNullValuesToDot(root);
    write_file("ast.dot", dotFile);
    cout << "generated ast.dot."<<endl;

    SAWYER_MESG(logger[TRACE]) << "Option VIZ: generating cfg dot file ..."<<endl;
    write_file("cfg_non_clustered.dot", analyzer->getFlow()->toDot(analyzer->getCFAnalyzer()->getLabeler()));
    DataDependenceVisualizer ddvis(analyzer->getLabeler(),analyzer->getVariableIdMapping(),"none");
    ddvis.generateDotFunctionClusters(root,analyzer->getCFAnalyzer(),"cfg.dot",false);
    cout << "generated cfg.dot, cfg_non_clustered.dot"<<endl;
    cout << "=============================================================="<<endl;
  }
  if(ctOpt.visualization.vizTg2) {
    string dotFile3=visualizer.foldedTransitionGraphToDot();
    write_file("transitiongraph2.dot", dotFile3);
    cout << "generated transitiongraph2.dot."<<endl;
  }

  if (ctOpt.visualization. dotIOStg.size()>0) {
    string filename=ctOpt.visualization. dotIOStg;
    cout << "generating dot IO graph file:"<<filename<<endl;
    string dotFile="digraph G {\n";
    dotFile+=visualizer.transitionGraphWithIOToDot();
    dotFile+="}\n";
    write_file(filename, dotFile);
    cout << "=============================================================="<<endl;
  }

  if (ctOpt.visualization.dotIOStgForcedTop.size()>0) {
    string filename=ctOpt.visualization.dotIOStgForcedTop;
    cout << "generating dot IO graph file for an abstract STG:"<<filename<<endl;
    string dotFile="digraph G {\n";
    dotFile+=visualizer.abstractTransitionGraphToDot();
    dotFile+="}\n";
    write_file(filename, dotFile);
    cout << "=============================================================="<<endl;
  }
}

void optionallyGenerateExternalFunctionsFile(CodeThornOptions& ctOpt, SgProject* sageProject) {
  bool unknownFunctionsFile=ctOpt.externalFunctionsCSVFileName.size()>0;
  bool showProgramStats=ctOpt.programStats;
  bool showProgramStatsOnly=ctOpt.programStatsOnly;
  if(unknownFunctionsFile||showProgramStats||showProgramStatsOnly) {
    ProgramInfo programInfo(sageProject);
    programInfo.compute();
    if(unknownFunctionsFile) {
      programInfo.writeFunctionCallNodesToFile(ctOpt.externalFunctionsCSVFileName);
    }
    if(showProgramStats||showProgramStatsOnly) {
      programInfo.printDetailed();
    }
    if(showProgramStatsOnly) {
      exit(0);
    }
  }
}

void optionallyGenerateAstStatistics(CodeThornOptions& ctOpt, SgProject* sageProject) {
  if(ctOpt.info.printAstNodeStats||ctOpt.info.astNodeStatsCSVFileName.size()>0) {
    // from: src/midend/astDiagnostics/AstStatistics.C
    if(ctOpt.info.printAstNodeStats) {
      ROSE_Statistics::AstNodeTraversalStatistics astStats;
      string s=astStats.toString(sageProject);
      cout<<s; // output includes newline at the end
    }
    if(ctOpt.info.astNodeStatsCSVFileName.size()>0) {
      ROSE_Statistics::AstNodeTraversalCSVStatistics astCSVStats;
      string fileName=ctOpt.info.astNodeStatsCSVFileName;
      astCSVStats.setMinCountToShow(1); // default value is 1
      if(!CppStdUtilities::writeFile(fileName, astCSVStats.toString(sageProject))) {
        cerr<<"Error: cannot write AST node statistics to CSV file "<<fileName<<endl;
        exit(1);
      }
    }
  }
}

void optionallyGenerateSourceProgramAndExit(CodeThornOptions& ctOpt, SgProject* sageProject) {
  if(ctOpt.unparse) {
    sageProject->unparse(0,0);
    exit(0);
  }
}

void optionallyGenerateTraversalInfoAndExit(CodeThornOptions& ctOpt, SgProject* sageProject) {
  if(ctOpt.info.astTraversalCSVFileName.size()>0) {
    RoseAst ast(sageProject);
    std::ofstream myfile;
    myfile.open(ctOpt.info.astTraversalCSVFileName.c_str(),std::ios::out);
    for(auto n : ast) {
      myfile<<n->class_name();
      if(ctOpt.info.astTraversalCSVMode>=2) {
        myfile<<","<<SgNodeHelper::sourceFilenameLineColumnToString(n);
      }
      myfile<<endl;
    }
    myfile.close();
    exit(0);
  }
}

void optionallyRunRoseAstChecksAndExit(CodeThornOptions& ctOpt, SgProject* sageProject) {
  if(ctOpt.runRoseAstChecks) {
    cout << "ROSE tests started."<<endl;
    // Run internal consistency tests on AST
    AstTests::runAllTests(sageProject);

    // test: constant expressions
    {
      SAWYER_MESG(logger[TRACE]) <<"STATUS: testing constant expressions."<<endl;
      CppConstExprEvaluator* evaluator=new CppConstExprEvaluator();
      list<SgExpression*> exprList=AstUtility::exprRootList(sageProject);
      logger[INFO] <<"found "<<exprList.size()<<" expressions."<<endl;
      for(list<SgExpression*>::iterator i=exprList.begin();i!=exprList.end();++i) {
        EvalResult r=evaluator->traverse(*i);
        if(r.isConst()) {
          SAWYER_MESG(logger[TRACE])<<"Found constant expression: "<<(*i)->unparseToString()<<" eq "<<r.constValue()<<endl;
        }
      }
      delete evaluator;
    }
    cout << "ROSE tests finished."<<endl; 
    mfacilities.shutdown();
    exit(0);
  }
}

void optionallyRunIOSequenceGenerator(CodeThornOptions& ctOpt, IOAnalyzer* analyzer) {
  if(ctOpt.rers.iSeqFile.size()>0) {
    int iseqLen=0;
    if(ctOpt.rers.iSeqLength!=-1) {
      iseqLen=ctOpt.rers.iSeqLength;
    } else {
      logger[ERROR] <<"input-sequence file specified, but no sequence length."<<endl;
      exit(1);
    }
    string fileName=ctOpt.rers.iSeqFile;
    SAWYER_MESG(logger[TRACE]) <<"STATUS: computing input sequences of length "<<iseqLen<<endl;
    IOSequenceGenerator iosgen;
    if(ctOpt.rers.iSeqRandomNum!=-1) {
      int randomNum=ctOpt.rers.iSeqRandomNum;
      SAWYER_MESG(logger[TRACE]) <<"STATUS: reducing input sequence set to "<<randomNum<<" random elements."<<endl;
      iosgen.computeRandomInputPathSet(iseqLen,*analyzer->getTransitionGraph(),randomNum);
    } else {
      iosgen.computeInputPathSet(iseqLen,*analyzer->getTransitionGraph());
    }
    SAWYER_MESG(logger[TRACE]) <<"STATUS: generating input sequence file "<<fileName<<endl;
    iosgen.generateFile(fileName);
  } else {
    if(ctOpt.rers.iSeqLength!=-1) {
      logger[ERROR] <<"input sequence length specified without also providing a file name (use option --iseq-file)."<<endl;
      exit(1);
    }
  }
}

void optionallyAnnotateTermsAndUnparse(CodeThornOptions& ctOpt, SgProject* sageProject, Analyzer* analyzer) {
  if (ctOpt.annotateTerms) {
    // TODO: it might be useful to be able to select certain analysis results to be annotated only
    logger[INFO] << "Annotating term representations."<<endl;
    AstTermRepresentationAttribute::attachAstTermRepresentationAttributes(sageProject);
    AstAnnotator ara(analyzer->getLabeler());
    ara.annotateAstAttributesAsCommentsBeforeStatements(sageProject,"codethorn-term-representation");
  }

  if (ctOpt.annotateTerms||ctOpt.generateAssertions) {
    logger[INFO] << "Generating annotated program."<<endl;
    //backend(sageProject);
    sageProject->unparse(0,0);
  }
}

void optionallyRunDataRaceDetection(CodeThornOptions& ctOpt, Analyzer* analyzer) {
  // parse command line options for data race detection
  DataRaceDetection dataRaceDetection;
  dataRaceDetection.setOptions(ctOpt);
  dataRaceDetection.handleCommandLineOptions(*analyzer);
  dataRaceDetection.setVisualizeReadWriteAccesses(ctOpt.visualization.visualizeRWSets); 
  if(dataRaceDetection.run(*analyzer)) {
    exit(0);
  }
}

SgProject* runRoseFrontEnd(int argc, char * argv[], CodeThornOptions& ctOpt, TimingCollector& timingCollector) {
  timingCollector.startTimer();
  vector<string> argvList(argv,argv+argc);
  if(ctOpt.ompAst||ctOpt.dr.detection) {
    SAWYER_MESG(logger[TRACE])<<"selected OpenMP AST."<<endl;
    argvList.push_back("-rose:OpenMP:ast_only");
  }
  if(ctOpt.roseAstReadFileName.size()>0) {
    // add ROSE option as required non-standard single dash long option
    argvList.push_back("-rose:ast:read");
    argvList.push_back(ctOpt.roseAstReadFileName);
  }
  SgProject* project=frontend(argvList);
  timingCollector.stopFrontEndTimer();
  return project;
}

void optionallyPrintProgramInfos(CodeThornOptions& ctOpt, Analyzer* analyzer) {
  if(ctOpt.info.printVariableIdMapping) {
    analyzer->getVariableIdMapping()->toStream(cout);
  }
  
  if(ctOpt.info.printTypeSizeMapping||ctOpt.info.typeSizeMappingCSVFileName.size()>0) {
    // from: src/midend/astDiagnostics/AstStatistics.C
    string s=analyzer->typeSizeMappingToString();
    if(ctOpt.info.printTypeSizeMapping) {
      cout<<"Type size mapping:"<<endl;
      cout<<s; // output includes newline at the end
    }
    if(ctOpt.info.typeSizeMappingCSVFileName.size()>0) {
      string fileName=ctOpt.info.typeSizeMappingCSVFileName;
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Error: cannot write type-size mapping to CSV file "<<fileName<<endl;
        exit(1);
      }
    }
  }
}

void optionallyRunNormalization(CodeThornOptions& ctOpt,SgProject* sageProject, TimingCollector& timingCollector) {
  timingCollector.startTimer();
  Normalization normalization;
  normalization.options.printPhaseInfo=ctOpt.normalizePhaseInfo;
  if(ctOpt.normalizeFCalls) {
    normalization.normalizeAst(sageProject,1);
    SAWYER_MESG(logger[TRACE])<<"STATUS: normalized expressions with fcalls (if not a condition)"<<endl;
  }
  
  if(ctOpt.normalizeAll) {
    if(ctOpt.quiet==false) {
      cout<<"STATUS: normalizing program."<<endl;
    }
    //SAWYER_MESG(logger[INFO])<<"STATUS: normalizing program."<<endl;
    normalization.normalizeAst(sageProject,2);
  }
  timingCollector.stopNormalizationTimer();
  optionallyRunInliner(ctOpt,normalization, sageProject);
}

void setAssertConditionVariablesInAnalyzer(SgNode* root,Analyzer* analyzer) {
  AbstractValueSet varsInAssertConditions=AstUtility::determineVarsInAssertConditions(root,analyzer->getVariableIdMapping());
  SAWYER_MESG(logger[TRACE])<<"STATUS: determined "<<varsInAssertConditions.size()<< " variables in (guarding) assert conditions."<<endl;
  analyzer->setAssertCondVarsSet(varsInAssertConditions);
}

void optionallyEliminateCompoundStatements(CodeThornOptions& ctOpt, Analyzer* analyzer, SgNode* root) {
  if(ctOpt.eliminateCompoundStatements) {
    RewriteSystem rewriteSystem;
    SAWYER_MESG(logger[TRACE])<<"STATUS: Elimination of compound assignments started."<<endl;
    set<AbstractValue> compoundIncVarsSet=AstUtility::determineSetOfCompoundIncVars(analyzer->getVariableIdMapping(),root);
    analyzer->setCompoundIncVarsSet(compoundIncVarsSet);
    SAWYER_MESG(logger[TRACE])<<"STATUS: determined "<<compoundIncVarsSet.size()<<" compound inc/dec variables before normalization."<<endl;
    rewriteSystem.resetStatistics();
    rewriteSystem.rewriteCompoundAssignmentsInAst(root,analyzer->getVariableIdMapping());
    SAWYER_MESG(logger[TRACE])<<"STATUS: Elimination of compound assignments finished."<<endl;
  }
}

void optionallyEliminateRersArraysAndExit(CodeThornOptions& ctOpt, SgProject* sageProject, Analyzer* analyzer) {
  if(ctOpt.rers.eliminateArrays) {
    Specialization speci;
    speci.transformArrayProgram(sageProject, analyzer);
    sageProject->unparse(0,0);
    exit(0);
  }
}

void optionallyPrintFunctionIdMapping(CodeThornOptions& ctOpt,Analyzer* analyzer) {
  // function-id-mapping is initialized with initializeSolver function.
  if(ctOpt.info.printFunctionIdMapping) {
    ROSE_ASSERT(analyzer->getCFAnalyzer());
    ROSE_ASSERT(analyzer->getCFAnalyzer()->getFunctionIdMapping());
    analyzer->getCFAnalyzer()->getFunctionIdMapping()->toStream(cout);
  }
}

void optionallyInitializePatternSearchSolver(CodeThornOptions& ctOpt,IOAnalyzer* analyzer,TimingCollector& timingCollector) {
    // pattern search: requires that exploration mode is set,
    // otherwise no pattern search is performed
    if(ctOpt.patSearch.explorationMode.size()>0) {
      logger[INFO] << "Pattern search exploration mode was set. Choosing solver 10." << endl;
      analyzer->setSolver(new Solver10());
      analyzer->setStartPState(*analyzer->popWorkList()->pstate());
    }
}

void optionallyWriteSVCompWitnessFile(CodeThornOptions& ctOpt, Analyzer* analyzer) {
  if (ctOpt.svcomp.svcompMode && ctOpt.svcomp.witnessFileName.size()>0) {
    analyzer->writeWitnessToFile(ctOpt.svcomp.witnessFileName);
  }
}

void optionallyAnalyzeAssertions(CodeThornOptions& ctOpt, LTLOptions& ltlOpt, IOAnalyzer* analyzer, TimingCollector& tc) {
  TimeMeasurement& timer=tc.timer;
  bool withCe=ltlOpt.withCounterExamples || ltlOpt.withAssertCounterExamples;
  if(withCe) {
    SAWYER_MESG(logger[TRACE]) << "STATUS: extracting assertion traces (this may take some time)"<<endl;
    timer.start();
    analyzer->extractRersIOAssertionTraces();
    tc.extractAssertionTracesTime = timer.getTimeDurationAndStop().milliSeconds();
  }
  
  tc.determinePrefixDepthTime= 0; // MJ: Determination of prefix depth currently deactivated.
  //int inputSeqLengthCovered = -1;
  tc.totalInputTracesTime = tc.extractAssertionTracesTime + tc.determinePrefixDepthTime;

  if(ctOpt.status) {
    analyzer->printStatusMessageLine("==============================================================");
    analyzer->reachabilityResults.printResults("YES (REACHABLE)", "NO (UNREACHABLE)", "error_", withCe);
  }
  if (ctOpt.rers.assertResultsOutputFileName.size()>0) {
    analyzer->reachabilityResults.writeFile(ctOpt.rers.assertResultsOutputFileName.c_str(),
                                            false, 0, withCe);
    if(ctOpt.status) {
      cout << "Reachability results written to file \""<<ctOpt.rers.assertResultsOutputFileName<<"\"." <<endl;
      cout << "=============================================================="<<endl;
    }
  }
  // deprecated?
  if(ctOpt.eliminateSTGBackEdges) {
    int numElim=analyzer->getTransitionGraph()->eliminateBackEdges();
    SAWYER_MESG(logger[TRACE])<<"STATUS: eliminated "<<numElim<<" STG back edges."<<endl;
  }

  if(ctOpt.status) {
    analyzer->reachabilityResults.printResultsStatistics();
    analyzer->printStatusMessageLine("==============================================================");
  }
}

void optionallyRunZ3AndExit(CodeThornOptions& ctOpt,Analyzer* analyzer) {
#ifdef HAVE_Z3
  if(ctOpt.z3BasedReachabilityAnalysis)
    {
      assert(ctOpt.z3UpperInputBound!=-1 && ctOpt.z3VerifierErrorNumber!=-1);	
      int RERSUpperBoundForInput=ctOpt.z3UpperInputBound;
      int RERSVerifierErrorNumber=ctOpt.z3VerifierErrorNumber;
      cout << "generateSSAForm()" << endl;
      ReachabilityAnalyzerZ3* reachAnalyzer = new ReachabilityAnalyzerZ3(RERSUpperBoundForInput, RERSVerifierErrorNumber, analyzer, &logger);	
      cout << "checkReachability()" << endl;
      reachAnalyzer->checkReachability();

      exit(0);
    }
#endif	
}
void optionallyRunSSAGeneratorAndExit(CodeThornOptions& ctOpt, Analyzer* analyzer) {
  if(ctOpt.ssa) {
    SSAGenerator* ssaGen = new SSAGenerator(analyzer, &logger);
    ssaGen->generateSSAForm();
    exit(0);
  }
}

void optionallyGenerateVerificationReports(CodeThornOptions& ctOpt,Analyzer* analyzer) {
  if(ctOpt.analysisList().size()>0) {
    const bool reportDetectedErrorLines=true;
    AnalysisReporting::generateVerificationReports(ctOpt,analyzer,reportDetectedErrorLines);
    AnalysisReporting::generateAnalysisStatsRawData(ctOpt,analyzer);
    AnalysisReporting::generateAnalyzedFunctionsAndFilesReports(ctOpt,analyzer);
  }
}

void initializeSolverWithStartFunction(CodeThornOptions& ctOpt,Analyzer* analyzer,SgNode* root, TimingCollector& tc) {
  tc.startTimer();
  SAWYER_MESG(logger[TRACE])<< "INIT: initializing solver with start function "<<analyzer->getSolver()->getId()<<"."<<endl;
  {
    string option_start_function="main";
    if(ctOpt.startFunctionName.size()>0) {
      option_start_function = ctOpt.startFunctionName;
    }
    // if main function exists, start with main-function
    // if a single function exist, use this function
    // in all other cases exit with error.
    RoseAst completeAst(root);
    string startFunction=option_start_function;
    SgNode* startFunRoot=completeAst.findFunctionByName(startFunction);
    if(startFunRoot==0) {
      // no main function exists. check if a single function exists in the translation unit
      SgProject* project=isSgProject(root);
      ROSE_ASSERT(project);
      std::list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(project);
      if(funDefs.size()==1) {
        // found exactly one function. Analyse this function.
        SgFunctionDefinition* functionDef=*funDefs.begin();
        startFunction=SgNodeHelper::getFunctionName(functionDef);
      } else if(funDefs.size()>1) {
        cerr<<"Error: no main function and more than one function in translation unit."<<endl;
        exit(1);
      } else if(funDefs.size()==0) {
        cerr<<"Error: no function in translation unit."<<endl;
        exit(1);
      }
    }
    ROSE_ASSERT(startFunction!="");
    analyzer->initializeSolver(startFunction,root,false);
  }
  tc.initRunTime=tc.timer.getTimeDurationAndStop().milliSeconds();
}

void runSolver(CodeThornOptions& ctOpt,Analyzer* analyzer, SgProject* sageProject,TimingCollector& tc) {
  tc.startTimer();
  analyzer->printStatusMessageLine("==============================================================");
  if(!analyzer->getModeLTLDriven() && ctOpt.z3BasedReachabilityAnalysis==false && ctOpt.ssa==false) {
    switch(ctOpt.abstractionMode) {
    case 0:
    case 1:
      analyzer->runSolver();
      break;
    case 2:
      cout<<"INFO: PA framework: initialization."<<endl;
      analyzer->initialize(sageProject);
      cout<<"INFO: running PA Framework solver."<<endl;
      analyzer->run();
      cout<<"INFO: PA framework: finished."<<endl;
      exit(0);
    }
  }
  tc.analysisRunTime=tc.timer.getTimeDurationAndStop().milliSeconds();
}

void runLTLAnalysis(CodeThornOptions& ctOpt, LTLOptions& ltlOpt,IOAnalyzer* analyzer, TimingCollector& tc) {
  long pstateSetSize=analyzer->getPStateSet()->size();
  long pstateSetBytes=analyzer->getPStateSet()->memorySize();
  long pstateSetMaxCollisions=analyzer->getPStateSet()->maxCollisions();
  long pstateSetLoadFactor=analyzer->getPStateSet()->loadFactor();
  long eStateSetSize=analyzer->getEStateSet()->size();
  long eStateSetBytes=analyzer->getEStateSet()->memorySize();
  long eStateSetMaxCollisions=analyzer->getEStateSet()->maxCollisions();
  double eStateSetLoadFactor=analyzer->getEStateSet()->loadFactor();
  long transitionGraphSize=analyzer->getTransitionGraph()->size();
  long transitionGraphBytes=transitionGraphSize*sizeof(Transition);
  long numOfconstraintSets=analyzer->getConstraintSetMaintainer()->numberOf();
  long constraintSetsBytes=analyzer->getConstraintSetMaintainer()->memorySize();
  long constraintSetsMaxCollisions=analyzer->getConstraintSetMaintainer()->maxCollisions();
  double constraintSetsLoadFactor=analyzer->getConstraintSetMaintainer()->loadFactor();
  long numOfStdinEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR));
  long numOfStdoutVarEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR));
  long numOfStdoutConstEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_CONST));
  long numOfStderrEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR));
  long numOfFailedAssertEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT));
  long numOfConstEStates=0;//(analyzer->getEStateSet()->numberOfConstEStates(analyzer->getVariableIdMapping()));
  long numOfStdoutEStates=numOfStdoutVarEStates+numOfStdoutConstEStates;

#if defined(__unix__) || defined(__unix) || defined(unix)
  // Unix-specific solution to finding the peak phyisical memory consumption (rss).
  // Not necessarily supported by every OS.
  struct rusage resourceUsage;
  getrusage(RUSAGE_SELF, &resourceUsage);
  long totalMemory=resourceUsage.ru_maxrss * 1024;
#else
  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;
#endif

  double totalRunTime=tc.frontEndRunTime+tc.initRunTime+tc.analysisRunTime;

  long pstateSetSizeInf = 0;
  long eStateSetSizeInf = 0;
  long transitionGraphSizeInf = 0;
  long eStateSetSizeStgInf = 0;
  double infPathsOnlyTime = 0;
  double stdIoOnlyTime = 0;

 
  TimeMeasurement& timer=tc.timer;
  if(ltlOpt.stdIOOnly) {
    SAWYER_MESG(logger[TRACE]) << "STATUS: bypassing all non standard I/O states. (P2)"<<endl;
    timer.start();
    if (ltlOpt.keepErrorStates) {
      analyzer->reduceStgToInOutAssertStates();
    } else {
      analyzer->reduceStgToInOutStates();
    }
    if(ltlOpt.inifinitePathsOnly) {
      analyzer->pruneLeaves();
    }
    stdIoOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
  } else {
    if(ltlOpt.inifinitePathsOnly) {
      assert (!ltlOpt.keepErrorStates);
      cout << "recursively removing all leaves (1)."<<endl;
      timer.start();
      analyzer->pruneLeaves();
      infPathsOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
      pstateSetSizeInf=analyzer->getPStateSet()->size();
      eStateSetSizeInf = analyzer->getEStateSet()->size();
      transitionGraphSizeInf = analyzer->getTransitionGraph()->size();
      eStateSetSizeStgInf = (analyzer->getTransitionGraph())->estateSet().size();
    }
  }

  long eStateSetSizeIoOnly = 0;
  long transitionGraphSizeIoOnly = 0;
  double spotLtlAnalysisTime = 0;

  stringstream statisticsSizeAndLtl;
  stringstream statisticsCegpra;

  if (ltlOpt.ltlFormulaeFile.size()>0) {
    logger[INFO] <<"STG size: "<<analyzer->getTransitionGraph()->size()<<endl;
    string ltl_filename = ltlOpt.ltlFormulaeFile;
    if(ctOpt.rers.rersMode) {  //reduce the graph accordingly, if not already done
      if (!ltlOpt.inifinitePathsOnly
          && !ltlOpt.keepErrorStates
          &&!analyzer->getModeLTLDriven()) {
        cout<< "STATUS: recursively removing all leaves (due to RERS-mode (2))."<<endl;
        timer.start();
        analyzer->pruneLeaves();
        infPathsOnlyTime = timer.getTimeDurationAndStop().milliSeconds();

        pstateSetSizeInf=analyzer->getPStateSet()->size();
        eStateSetSizeInf = analyzer->getEStateSet()->size();
        transitionGraphSizeInf = analyzer->getTransitionGraph()->size();
        eStateSetSizeStgInf = (analyzer->getTransitionGraph())->estateSet().size();
      }
      if (!ltlOpt.stdIOOnly &&!analyzer->getModeLTLDriven()) {
        cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode) (P1)."<<endl;
        timer.start();
        analyzer->getTransitionGraph()->printStgSize("before reducing non-I/O states");
        if (ltlOpt.keepErrorStates) {
          analyzer->reduceStgToInOutAssertStates();
        } else {
          analyzer->reduceStgToInOutStates();
        }
        stdIoOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
        analyzer->getTransitionGraph()->printStgSize("after reducing non-I/O states");
      }
    }
    if(ltlOpt.noInputInputTransitions) {  //delete transitions that indicate two input states without an output in between
      analyzer->removeInputInputTransitions();
      analyzer->getTransitionGraph()->printStgSize("after reducing input->input transitions");
    }
    bool withCounterexample = false;
    if(ltlOpt.withCounterExamples || ltlOpt.withLTLCounterExamples) {  //output a counter-example input sequence for falsified formulae
      withCounterexample = true;
    }

    timer.start();
    std::set<int> ltlInAlphabet;// = analyzer->getInputVarValues();
    //take fixed ltl input alphabet if specified, instead of the input values used for stg computation
    if (ltlOpt.ltlInAlphabet.size()>0) {
      string setstring=ltlOpt.ltlInAlphabet;
      ltlInAlphabet=Parse::integerSet(setstring);
      SAWYER_MESG(logger[TRACE]) << "LTL input alphabet explicitly selected: "<< setstring << endl;
    }
    //take ltl output alphabet if specifically described, otherwise take the old RERS specific 21...26 (a.k.a. oU...oZ)
    std::set<int> ltlOutAlphabet;// = Parse::integerSet("{21,22,23,24,25,26}");
    if (ltlOpt.ltlOutAlphabet.size()>0) {
      string setstring=ltlOpt.ltlOutAlphabet;
      ltlOutAlphabet=Parse::integerSet(setstring);
      SAWYER_MESG(logger[TRACE]) << "LTL output alphabet explicitly selected: "<< setstring << endl;
    } else {
      // TODO: fail, if no output alphabet is provided
    }
    PropertyValueTable* ltlResults=nullptr;
    SpotConnection spotConnection(ltl_filename);
    spotConnection.setModeLTLDriven(analyzer->getModeLTLDriven());
    if (analyzer->getModeLTLDriven()) {
      analyzer->setSpotConnection(&spotConnection);
    }

    SAWYER_MESG(logger[TRACE]) << "STATUS: generating LTL results"<<endl;
    bool spuriousNoAnswers = false;
    SAWYER_MESG(logger[TRACE]) << "LTL: check properties."<<endl;
    if (ltlOpt.propertyNrToCheck!=-1) {
      int propertyNum = ltlOpt.propertyNrToCheck;
      spotConnection.checkSingleProperty(propertyNum, *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
    } else {
      spotConnection.checkLtlProperties( *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
    }
    spotLtlAnalysisTime=timer.getTimeDurationAndStop().milliSeconds();
    SAWYER_MESG(logger[TRACE]) << "LTL: get results from spot connection."<<endl;
    ltlResults = spotConnection.getLtlResults();
    SAWYER_MESG(logger[TRACE]) << "LTL: results computed."<<endl;

    if (ltlOpt.cegpra.ltlPropertyNr!=-1 || ltlOpt.cegpra.checkAllProperties) {
      if (ltlOpt.cegpra.csvStatsFileName.size()>0) {
        statisticsCegpra << "init,";
        analyzer->getTransitionGraph()->printStgSize("initial abstract model");
        analyzer->getTransitionGraph()->csvToStream(statisticsCegpra);
        statisticsCegpra << ",na,na";
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
      }
      CounterexampleAnalyzer ceAnalyzer(analyzer, &statisticsCegpra);
      if (ltlOpt.cegpra.maxIterations!=-1) {
        ceAnalyzer.setMaxCounterexamples(ltlOpt.cegpra.maxIterations);
      }
      if (ltlOpt.cegpra.checkAllProperties) {
        ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(spotConnection, ltlInAlphabet, ltlOutAlphabet);
      } else {
        // cegpra for single LTL property
        //ROSE_ASSERT(ltlOpt.cegpra.ltlPropertyNr!=-1);
        int property = ltlOpt.cegpra.ltlPropertyNr;
        ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(property, spotConnection, ltlInAlphabet, ltlOutAlphabet);
      }
    }

    if(ctOpt.status) {
      ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexample);
      analyzer->printStatusMessageLine("==============================================================");
      ltlResults->printResultsStatistics();
      analyzer->printStatusMessageLine("==============================================================");
    }
    if (ltlOpt.spotVerificationResultsCSVFileName.size()>0) {  //write results to a file instead of displaying them directly
      std::string csv_filename = ltlOpt.spotVerificationResultsCSVFileName;
      SAWYER_MESG(logger[TRACE]) << "STATUS: writing ltl results to file: " << csv_filename << endl;
      ltlResults->writeFile(csv_filename.c_str(), false, 0, withCounterexample);
    }
    if (ltlOpt.ltlStatisticsCSVFileName.size()>0) {
      analyzer->getTransitionGraph()->printStgSize("final model");
      analyzer->getTransitionGraph()->csvToStream(statisticsSizeAndLtl);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
    }
#if 0
    if(ltlResults) {
      delete ltlResults;
      ltlResults = NULL;
    }
#endif
    //temporaryTotalRunTime = totalRunTime + infPathsOnlyTime + stdIoOnlyTime + spotLtlAnalysisTime;
    //printAnalyzerStatistics(analyzer, temporaryTotalRunTime, "LTL check complete. Reduced transition system:");
  }
  double totalLtlRunTime =  infPathsOnlyTime + stdIoOnlyTime + spotLtlAnalysisTime;

  double arrayUpdateExtractionRunTime=0.0;
  double arrayUpdateSsaNumberingRunTime=0.0;
  double sortingAndIORunTime=0.0;
  double verifyUpdateSequenceRaceConditionRunTime=0.0;

  int verifyUpdateSequenceRaceConditionsResult=-1;
  int verifyUpdateSequenceRaceConditionsTotalLoopNum=-1;
  int verifyUpdateSequenceRaceConditionsParLoopNum=-1;

  /* Data race detection */
  optionallyRunDataRaceDetection(ctOpt,analyzer);

  double overallTime=totalRunTime + tc.totalInputTracesTime + totalLtlRunTime;
  analyzer->printAnalyzerStatistics(totalRunTime, "STG generation and assertion analysis complete");

  if(ctOpt.csvStatsFileName.size()>0) {
    string filename=ctOpt.csvStatsFileName;
    stringstream text;
    text<<"Sizes,"<<pstateSetSize<<", "
        <<eStateSetSize<<", "
        <<transitionGraphSize<<", "
        <<numOfconstraintSets<<", "
        << numOfStdinEStates<<", "
        << numOfStdoutEStates<<", "
        << numOfStderrEStates<<", "
        << numOfFailedAssertEStates<<", "
        << numOfConstEStates<<endl;
    text<<"Memory,"<<pstateSetBytes<<", "
        <<eStateSetBytes<<", "
        <<transitionGraphBytes<<", "
        <<constraintSetsBytes<<", "
        <<totalMemory<<endl;
    text<<"Runtime(readable),"
        <<CodeThorn::readableruntime(tc.frontEndRunTime)<<", "
        <<CodeThorn::readableruntime(tc.initRunTime)<<", "
        <<CodeThorn::readableruntime(tc.normalizationRunTime)<<", "
        <<CodeThorn::readableruntime(tc.analysisRunTime)<<", "
        <<CodeThorn::readableruntime(verifyUpdateSequenceRaceConditionRunTime)<<", "
        <<CodeThorn::readableruntime(arrayUpdateExtractionRunTime)<<", "
        <<CodeThorn::readableruntime(arrayUpdateSsaNumberingRunTime)<<", "
        <<CodeThorn::readableruntime(sortingAndIORunTime)<<", "
        <<CodeThorn::readableruntime(totalRunTime)<<", "
        <<CodeThorn::readableruntime(tc.extractAssertionTracesTime)<<", "
        <<CodeThorn::readableruntime(tc.determinePrefixDepthTime)<<", "
        <<CodeThorn::readableruntime(tc.totalInputTracesTime)<<", "
        <<CodeThorn::readableruntime(infPathsOnlyTime)<<", "
        <<CodeThorn::readableruntime(stdIoOnlyTime)<<", "
        <<CodeThorn::readableruntime(spotLtlAnalysisTime)<<", "
        <<CodeThorn::readableruntime(totalLtlRunTime)<<", "
        <<CodeThorn::readableruntime(overallTime)<<endl;
    text<<"Runtime(ms),"
        <<tc.frontEndRunTime<<", "
        <<tc.initRunTime<<", "
        <<tc.normalizationRunTime<<", "
        <<tc.analysisRunTime<<", "
        <<verifyUpdateSequenceRaceConditionRunTime<<", "
        <<arrayUpdateExtractionRunTime<<", "
        <<arrayUpdateSsaNumberingRunTime<<", "
        <<sortingAndIORunTime<<", "
        <<totalRunTime<<", "
        <<tc.extractAssertionTracesTime<<", "
        <<tc.determinePrefixDepthTime<<", "
        <<tc.totalInputTracesTime<<", "
        <<infPathsOnlyTime<<", "
        <<stdIoOnlyTime<<", "
        <<spotLtlAnalysisTime<<", "
        <<totalLtlRunTime<<", "
        <<overallTime<<endl;
    text<<"hashset-collisions,"
        <<pstateSetMaxCollisions<<", "
        <<eStateSetMaxCollisions<<", "
        <<constraintSetsMaxCollisions<<endl;
    text<<"hashset-loadfactors,"
        <<pstateSetLoadFactor<<", "
        <<eStateSetLoadFactor<<", "
        <<constraintSetsLoadFactor<<endl;
    text<<"threads,"<<analyzer->getNumberOfThreadsToUse()<<endl;
    //    text<<"abstract-and-const-states,"
    //    <<"";

    // iterations (currently only supported for sequential analysis)
    text<<"iterations,";
    if(analyzer->getNumberOfThreadsToUse()==1 && analyzer->getSolver()->getId()==5 && analyzer->getExplorationMode()==EXPL_LOOP_AWARE)
      text<<analyzer->getIterations()<<","<<analyzer->getApproximatedIterations();
    else
      text<<"-1,-1";
    text<<endl;

    // -1: test not performed, 0 (no race conditions), >0: race conditions exist
    text<<"parallelism-stats,";
    if(verifyUpdateSequenceRaceConditionsResult==-1) {
      text<<"sequential";
    } else if(verifyUpdateSequenceRaceConditionsResult==0) {
      text<<"pass";
    } else {
      text<<"fail";
    }
    text<<","<<verifyUpdateSequenceRaceConditionsResult;
    text<<","<<verifyUpdateSequenceRaceConditionsParLoopNum;
    text<<","<<verifyUpdateSequenceRaceConditionsTotalLoopNum;
    text<<endl;

    text<<"infinite-paths-size,"<<pstateSetSizeInf<<", "
        <<eStateSetSizeInf<<", "
        <<transitionGraphSizeInf<<", "
        <<eStateSetSizeStgInf<<endl;
    //<<numOfconstraintSetsInf<<", "
    //<< numOfStdinEStatesInf<<", "
    //<< numOfStdoutEStatesInf<<", "
    //<< numOfStderrEStatesInf<<", "
    //<< numOfFailedAssertEStatesInf<<", "
    //<< numOfConstEStatesInf<<endl;
    text<<"states & transitions after only-I/O-reduction,"
        <<eStateSetSizeIoOnly<<", "
        <<transitionGraphSizeIoOnly<<endl;
    //  text<<"input length coverage"
    // <<inputSeqLengthCovered<<endl; // MS 2020: this value is not computed

    write_file(filename,text.str());
    cout << "generated "<<filename<<endl;
  }

  if(ltlOpt.ltlStatisticsCSVFileName.size()>0) {
    // content of a line in the .csv file:
    // <#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
    string filename = ltlOpt.ltlStatisticsCSVFileName;
    write_file(filename,statisticsSizeAndLtl.str());
    cout << "generated "<<filename<<endl;
  }

  if (ltlOpt.cegpra.csvStatsFileName.size()>0) {
    // content of a line in the .csv file:
    // <analyzed_property>,<#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,
    // <#analyzed_counterexamples>,<analysis_result(y/n/?)>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
    string filename = ltlOpt.cegpra.csvStatsFileName;
    write_file(filename,statisticsCegpra.str());
    cout << "generated "<<filename<<endl;
  }
}

int main( int argc, char * argv[] ) {
  try {
    ROSE_INITIALIZE;
    CodeThorn::configureRose();
    configureRersSpecialization();
    CodeThorn::initDiagnosticsLTL();

    TimingCollector tc;
    TimeMeasurement timer;

    tc.startTimer();
    CodeThornOptions ctOpt;
    LTLOptions ltlOpt; // to be moved into separate tool
    ParProOptions parProOpt; // options only available in parprothorn
    parseCommandLine(argc, argv, logger,versionString,ctOpt,ltlOpt,parProOpt);
    mfacilities.control(ctOpt.logLevel); SAWYER_MESG(logger[TRACE]) << "Log level is " << ctOpt.logLevel << endl;
    IOAnalyzer* analyzer=createAnalyzer(ctOpt,ltlOpt);
    optionallyRunInternalChecks(ctOpt,argc,argv);
    optionallyRunExprEvalTestAndExit(ctOpt,argc,argv);
    analyzer->configureOptions(ctOpt,ltlOpt,parProOpt);
    analyzer->setSolver(createSolver(ctOpt));
    analyzer->setOptionContextSensitiveAnalysis(ctOpt.contextSensitive);
    tc.stopTimer();

    SgProject* sageProject=runRoseFrontEnd(argc,argv,ctOpt,tc);
    if(ctOpt.status) cout << "STATUS: Parsing and creating AST finished."<<endl;
    optionallyRunNormalization(ctOpt,sageProject,tc);
    optionallyGenerateExternalFunctionsFile(ctOpt, sageProject);
    optionallyGenerateAstStatistics(ctOpt, sageProject);
    optionallyGenerateTraversalInfoAndExit(ctOpt, sageProject);
    optionallyGenerateSourceProgramAndExit(ctOpt, sageProject);
    if(ctOpt.status) cout<<"STATUS: analysis started."<<endl;
    analyzer->initializeVariableIdMapping(sageProject);
    logger[INFO]<<"registered string literals: "<<analyzer->getVariableIdMapping()->numberOfRegisteredStringLiterals()<<endl;
    optionallyPrintProgramInfos(ctOpt, analyzer);
    optionallyRunRoseAstChecksAndExit(ctOpt, sageProject);
    SgNode* root=sageProject;ROSE_ASSERT(root);
    setAssertConditionVariablesInAnalyzer(root,analyzer);
    optionallyEliminateCompoundStatements(ctOpt, analyzer, root);
    optionallyEliminateRersArraysAndExit(ctOpt,sageProject,analyzer);
    initializeSolverWithStartFunction(ctOpt,analyzer,root,tc);
    analyzer->initLabeledAssertNodes(sageProject);
    optionallyPrintFunctionIdMapping(ctOpt,analyzer);
    optionallyInitializePatternSearchSolver(ctOpt,analyzer,tc);
    runSolver(ctOpt,analyzer,sageProject,tc);
    analyzer->printStatusMessageLine("==============================================================");
    optionallyWriteSVCompWitnessFile(ctOpt, analyzer);
    optionallyAnalyzeAssertions(ctOpt, ltlOpt, analyzer, tc);
    optionallyRunZ3AndExit(ctOpt,analyzer);
    optionallyGenerateVerificationReports(ctOpt,analyzer);
    runLTLAnalysis(ctOpt,ltlOpt,analyzer,tc);
    processCtOptGenerateAssertions(ctOpt, analyzer, sageProject);
    optionallyRunVisualizer(ctOpt,analyzer,root);
    optionallyRunIOSequenceGenerator(ctOpt, analyzer);
    optionallyAnnotateTermsAndUnparse(ctOpt, sageProject, analyzer);
    if(ctOpt.status) cout<<color("normal")<<"done."<<endl;

    // main function try-catch
  } catch(const CodeThorn::Exception& e) {
    cerr << "Error: " << e.what() << endl;
    mfacilities.shutdown();
    return 1;
  } catch(const std::exception& e) {
    cerr<< "Error: " << e.what() << endl;
    mfacilities.shutdown();
    return 1;
  } catch(char const* str) {
    cerr<< "Error: " << str << endl;
    mfacilities.shutdown();
    return 1;
  } catch(string str) {
    cerr<< "Error: " << str << endl;
    mfacilities.shutdown();
    return 1;
  } catch(...) {
    cerr<< "Error: Unknown exception raised." << endl;
    mfacilities.shutdown();
    return 1;
  }
  mfacilities.shutdown();
  return 0;
}


#include "sage3basic.h"
#include "Analyzer.h"
#include "RewriteSystem.h"
#include "Specialization.h"
#include "Normalization.h"
#include "FunctionIdMapping.h"
#include "FunctionCallMapping.h"
#include "Diagnostics.h"
#include "CodeThornLib.h"
#include "LTLThornLib.h"
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
#include "LTLOptions.h"
#include "LTLThornLib.h"
#include "CppStdUtilities.h"

//BOOST includes
#include "boost/lexical_cast.hpp"

// required only for ROSE AST Consistency tests
#include "AstConsistencyTests.h"

#include "IOSequenceGenerator.h"

using namespace std;

using namespace CodeThorn;
using namespace boost;

#include "Diagnostics.h"
using namespace Sawyer::Message;

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "Solver5.h"
#include "Solver8.h"
#include "ltlthorn-lib/Solver10.h"
#include "ltlthorn-lib/Solver11.h"
#include "ltlthorn-lib/Solver12.h"

// handler
#include <signal.h>
#include <execinfo.h>

using namespace Sawyer::Message;

// handler for generating backtrace
void codethornBackTraceHandler(int sig) {
  void *array[10];
  size_t size;

  size = backtrace (array, 10);
  printf ("Obtained %zd stack frames.\n", size);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

void CodeThorn::initDiagnostics() {
  Rose::Diagnostics::initialize();
  // general logger for CodeThorn library functions
  Rose::Diagnostics::initAndRegister(&CodeThorn::logger, "CodeThorn");
  // class specific loggers for CodeThorn library functions
  Analyzer::initDiagnostics();
  ExprAnalyzer::initDiagnostics();
  RewriteSystem::initDiagnostics();
  Specialization::initDiagnostics();
  Normalization::initDiagnostics();
  FunctionIdMapping::initDiagnostics();
  FunctionCallMapping::initDiagnostics();
  EStateTransferFunctions::initDiagnostics();
}

Sawyer::Message::Facility CodeThorn::logger;

namespace CodeThorn {
void turnOffRoseWarnings() {
  string turnOffRoseWarnings=string("Rose(none,>=error),Rose::EditDistance(none,>=error),Rose::FixupAstDeclarationScope(none,>=error),")
    +"Rose::FixupAstSymbolTablesToSupportAliasedSymbols(none,>=error),"
    +"Rose::EditDistance(none,>=error),"
    +"Rose::TestChildPointersInMemoryPool(none,>=error),Rose::UnparseLanguageIndependentConstructs(none,>=error),"
    +"rose_ir_node(none,>=error)";
  // result string must be checked
  string result=Rose::Diagnostics::mfacilities.control(turnOffRoseWarnings); 
  if(result!="") {
    cerr<<result<<endl;
    cerr<<"Error in logger initialization."<<endl;
    exit(1);
  }
  
  // see class Options in src/roseSupport/utility_functions.h
  Rose::global_options.set_frontend_notes(false);
  Rose::global_options.set_frontend_warnings(false);
  Rose::global_options.set_backend_warnings(false);
}

void configureRose() {
  CodeThorn::initDiagnostics();

  Rose::Diagnostics::mprefix->showProgramName(false);
  Rose::Diagnostics::mprefix->showThreadId(false);
  Rose::Diagnostics::mprefix->showElapsedTime(false);

  turnOffRoseWarnings();
  signal(SIGSEGV, codethornBackTraceHandler);   // install handler for backtrace
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

void optionallySetRersMapping(CodeThornOptions ctOpt,LTLOptions ltlOpt,IOAnalyzer* analyzer) {
  // guard to check if LTL checking is activated
  if(ltlOpt.ltlRersMappingFileName.size()==0 && ltlOpt.ltlInAlphabet.size()==0 && ltlOpt.ltlOutAlphabet.size()==0)
    return;
  
  LtlRersMapping ltlRersMapping;
  if (ltlOpt.ltlInAlphabet.size()>0) {
    ltlRersMapping.addInputAsciiValueSetWithOffsetA(ltlOpt.ltlInAlphabet);
  }
  if (ltlOpt.ltlOutAlphabet.size()>0) {
    ltlRersMapping.addOutputAsciiValueSetWithOffsetA(ltlOpt.ltlOutAlphabet);
  }
  if(ltlOpt.ltlRersMappingFileName.size()>0) {
    // load and parse file into ltlInAlphabet and ltlOutAlphabet
    // input/output alphabet
    if(!readAndParseLTLRersMappingFile(ltlOpt.ltlRersMappingFileName,ltlRersMapping)) {
      cerr<<"Error: could not open RERS mapping file "<<ltlOpt.ltlRersMappingFileName<<endl;
      exit(1);
    }
  }
  
  {
    std::set<int> ltlInAlphabet;// = analyzer->getInputVarValues();
    std::set<int> ltlOutAlphabet;
    ltlInAlphabet=ltlRersMapping.getInputValueSet();
    ltlOutAlphabet=ltlRersMapping.getOutputValueSet();
    if(ltlInAlphabet.size()==0) {
      cerr<<"Error: no LTL input alphabet provided."<<endl;
      exit(1);
    }
    if(ltlOutAlphabet.size()==0) {
      cerr<<"Error: no LTL output alphabet provided."<<endl;
      exit(1);
    }
    /* now also set the input values. This also checks the
       inputalphabet setting does not conflict with input values
       settings */
    std::set<int> inputValues=analyzer->getInputVarValues();
    if(inputValues.size()>0) {
      if(inputValues.size()!=ltlInAlphabet.size()) {
        cerr<<"Error: input alphabet with "<<ltlInAlphabet.size()<<" values specified in ltl-rers-mapping but "<<inputValues.size()<<" input values."<<endl;
        exit(1);
      } else {
        // check that input alphabet matches input values
        for(auto inputVal : inputValues) {
          if(ltlInAlphabet.find(inputVal)==ltlInAlphabet.end()) {
            // sets don't match
            cerr<<"Error: input alphabet is different to specified input values (only input alphabet is necessary)."<<endl;
            exit(1);
          }
        }
        cout<<"INFO: RersMapping: input values match input alphabet. Option --input-values is superfluous."<<endl;
      }
    } else {
      // set input values to input alphabet
      for (auto val : ltlInAlphabet) {
        analyzer->insertInputVarValue(val);
      }
      cout<<"INFO: RersMapping: setting input values to input alphabet."<<endl;
    }
  }
  analyzer->setLtlRersMapping(ltlRersMapping);
}

bool readAndParseLTLRersMappingFile(string ltlRersMappingFileName, CodeThorn::LtlRersMapping& rersLtlMapping) {
  CppStdUtilities::DataFileVector dataFileVector;
  bool readStatus=CppStdUtilities::readDataFile(ltlRersMappingFileName,dataFileVector);
  if(readStatus==false)
    return readStatus;
  int lineNr=1;
  for(std::string line : dataFileVector) {
    std::vector<std::string> lineEntries=CppStdUtilities::splitByRegex(line,",|\\t|\\s+");
    if(lineEntries.size()!=2) {
      cerr<<"Error: format error in rers mapping file. Not exactly two entries in line "<<lineNr<<endl;
      exit(1);
    }
    string ioString=lineEntries[0];
    int value=std::stoi(lineEntries[1]);
    cout<<"INFO: mapping: line "<<lineNr<<": "<<ioString<<" <=> "<<value<<endl;
    if(ioString.size()==2&&ioString[0]=='i') {
      rersLtlMapping.addInput(ioString[1],value);
    } else if(ioString.size()==2&&ioString[0]=='o') {
      rersLtlMapping.addOutput(ioString[1],value);
    } else {
      cout<<"WARNING: unknown entry in rers mapping file line "<<lineNr<<": "<<ioString<<" (ignoring it)"<<endl;
    }
    lineNr++;
  }
  return true;
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
    CodeThorn::IOSequenceGenerator iosgen;
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
  if(ctOpt.roseAstWrite) {
    // add ROSE option as required non-standard single dash long option
    argvList.push_back("-rose:ast:write");
  }
  if(ctOpt.roseAstMerge) {
    // add ROSE option as required non-standard single dash long option
    argvList.push_back("-rose:ast:merge");
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
  CodeThorn::optionallyRunInliner(ctOpt,normalization, sageProject);
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

void optionallyGenerateVerificationReports(CodeThornOptions& ctOpt,Analyzer* analyzer) {
  if(ctOpt.analysisList().size()>0) {
    const bool reportDetectedErrorLines=true;
    AnalysisReporting::generateVerificationReports(ctOpt,analyzer,reportDetectedErrorLines); // also generates verification call graph
    AnalysisReporting::generateAnalysisStatsRawData(ctOpt,analyzer);
    AnalysisReporting::generateAnalyzedFunctionsAndFilesReports(ctOpt,analyzer);
  }
}
void optionallyGenerateCallGraphDotFile(CodeThornOptions& ctOpt,Analyzer* analyzer) {
  std::string fileName=ctOpt.visualization.callGraphFileName;
  if(fileName.size()>0) {
    InterFlow::LabelToFunctionMap map=analyzer->getCFAnalyzer()->labelToFunctionMap(*analyzer->getFlow());
    cout<<"DEBUG: labeltofunctionmap size:"<<map.size()<<endl;
    std::string dotFileString=analyzer->getInterFlow()->dotCallGraph(map);
    cout<<"DEBUG: interflow size:"<<analyzer->getInterFlow()->size()<<endl;
    if(!CppStdUtilities::writeFile(fileName, dotFileString)) {
      cerr<<"Error: could not generate callgraph dot file "<<fileName<<endl;
      exit(1);
    } else {
      cout<<"Generated call graph dot file "<<fileName<<endl;
    }
  }
}

  void initializeSolverWithStartFunction(CodeThornOptions& ctOpt,Analyzer* analyzer,SgNode* root, TimingCollector& tc) {
  tc.startTimer();
  SAWYER_MESG(logger[INFO])<< "Iinitializing solver "<<analyzer->getSolver()->getId()<<" started"<<endl;
  string startFunctionName;
  if(ctOpt.startFunctionName.size()>0) {
    startFunctionName = ctOpt.startFunctionName;
  } else {
    startFunctionName = "main";
  }
  analyzer->initializeSolver(startFunctionName,root,false);
  SAWYER_MESG(logger[INFO])<< "Initializing solver "<<analyzer->getSolver()->getId()<<" finished"<<endl;
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
} // end of namespace CodeThorn

#include "sage3basic.h"
#include "SgNodeHelper.h"
#include "CTAnalysis.h"
#include "RewriteSystem.h"
#include "Specialization.h"
#include "Normalization.h"
#include "FunctionIdMapping.h"
#include "FunctionCallMapping.h"
#include "Rose/Diagnostics.h"
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
#include "AnalysisParameters.h"
#include "CodeThornException.h"
#include "ProgramInfo.h"
#include "FunctionCallMapping.h"
#include "AstStatistics.h"

#include "Normalization.h"
#include "DataDependenceVisualizer.h" // also used for clustered ICFG
#include "Evaluator.h" // CppConstExprEvaluator
#include "CtxCallStrings.h" // for setting call string options
#include "AnalysisReporting.h"
#include "CTAnalysis.h"

#if HAVE_Z3
// Z3-based analyser / SSA
#include "z3-prover-connection/SSAGenerator.h"
#include "z3-prover-connection/ReachabilityAnalyzerZ3.h"
#endif

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

// required for createSolver function
#include "Solver5.h"
#include "Solver16.h"
#include "Solver17.h"
#include "Solver18.h"
#include "Solver19.h"
#include "Solver8.h"

using namespace std;

using namespace CodeThorn;
using namespace boost;

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

static std::string CodeThornLibraryVersion="1.13.72";

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
  CTAnalysis::initDiagnostics();
  RewriteSystem::initDiagnostics();
  Specialization::initDiagnostics();
  Normalization::initDiagnostics();
  FunctionIdMapping::initDiagnostics();
  FunctionCallMapping::initDiagnostics();
  EStateTransferFunctions::initDiagnostics();
  Solver18::initDiagnostics();
}

Sawyer::Message::Facility CodeThorn::logger;

// deprecated, wrapper for CodeThornLib::evaluateExpressionWithEmptyState
AbstractValue CodeThorn::evaluateExpressionWithEmptyState(SgExpression* expr) {
  return CodeThorn::CodeThornLib::evaluateExpressionWithEmptyState(expr);
}

namespace CodeThorn {
  namespace CodeThornLib {

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

    Solver* createSolver(CodeThornOptions& ctOpt) {
      Solver* solver = nullptr;
      // solver "factory"
      switch(ctOpt.solver) {
      case 5 :  {
        solver = new Solver5(); break;
      }
      case 16 :  {
        solver = new Solver16(); break; // variant of solver5
      }
      case 17 :  {
        solver = new Solver17(); break; // does not create a TS
      }
      case 18 :  {
        Solver18* solver18= new Solver18();
        solver18->setAbstractionConsistencyCheckFlag(ctOpt.abstractionConsistencyCheck);
        solver18->setPassThroughOptimizationFlag(ctOpt.passThroughOptimization);
        solver=solver18;
        break; // does not create a TS
      }
      case 19 :  {
        solver = new Solver19(); break; // does not create a TS
      }
      case 8 :  {
        solver = new Solver8(); break;
      }
      default :  {
        logger[ERROR] <<"Unknown solver ID: "<<ctOpt.solver<<endl;
        exit(1);
      }
      }
      return solver;
    }

    AbstractValue evaluateExpressionWithEmptyState(SgExpression* expr) {
      CTAnalysis* analyzer=new CTAnalysis();
      Solver* solver=new Solver16();
      analyzer->setSolver(solver); // solver is required
      EStateTransferFunctions* exprAnalyzer=new EStateTransferFunctions();
      exprAnalyzer->setAnalyzer(analyzer);
      VariableIdMappingExtended* vid=new VariableIdMappingExtended();  // only empty vim required
      VariableIdMappingExtended* oldVID=AbstractValue::_variableIdMapping;
      AbstractValue::setVariableIdMapping(vid);
      AbstractValue aVal=exprAnalyzer->evaluateExpressionWithEmptyState(expr);
      AbstractValue::_variableIdMapping=oldVID;

      delete vid;
      delete exprAnalyzer;
      delete analyzer;

      return aVal;
    }


    void exprEvalTest(int argc, char* argv[],CodeThornOptions& ctOpt) {
      cout << "------------------------------------------"<<endl;
      cout << "RUNNING CHECKS FOR EXPR ANALYZER:"<<endl;
      cout << "------------------------------------------"<<endl;
      SgProject* sageProject=frontend(argc,argv);
      Normalization normalization;
      if(ctOpt.normalizeLevel>0) {
        if(ctOpt.quiet==false) {
          cout<<"STATUS: normalizing program."<<endl;
        }
        // normalization.options.normalizeCplusplus=ctOpt.extendedNormalizedCppFunctionCalls; // PP(8/13/21)
        normalization.normalizeAst(sageProject,ctOpt.normalizeLevel);
      }
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
          AbstractValue aVal=evaluateExpressionWithEmptyState(expr);
          cout<<" => result value: "<<aVal.toString()<<" "<<endl;
        }
      }
    }

    void optionallyRunExprEvalTestAndExit(CodeThornOptions& ctOpt,int argc, char * argv[]) {
      if(ctOpt.exprEvalTest) {
        exprEvalTest(argc,argv,ctOpt);
        exit(0);
      }
    }

    void optionallySetRersMapping(CodeThornOptions /*ctOpt*/,LTLOptions ltlOpt,IOAnalyzer* analyzer) {
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

    void processCtOptGenerateAssertions(CodeThornOptions& ctOpt, CTAnalysis* analyzer, SgProject* root) {
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
      IOAnalyzer* analyzer = new IOAnalyzer();
      analyzer->setOptions(ctOpt);
      analyzer->setLtlOptions(ltlOpt);
      if(ctOpt.sharedPStates==true) {
        cerr<<"CodeThornLib::createAnalyzer: shared PStates not supported anymore. Exiting."<<endl;
        exit(1);
      }
      EState::sharedPStates=ctOpt.sharedPStates;
      EState::fastPointerHashing=ctOpt.fastPointerHashing;
      AbstractValue::pointerSetsEnabled=ctOpt.pointerSetsEnabled;
      AbstractValue::domainAbstractionVariant=ctOpt.domainAbstractionVariant;
      return analyzer;
    }

    IOAnalyzer* createEStateAnalyzer(CodeThornOptions& ctOpt, LTLOptions& ltlOpt, Labeler*, VariableIdMappingExtended*, CFAnalysis*, Solver*) {
      IOAnalyzer* ioAnalyzer=CodeThornLib::createAnalyzer(ctOpt,ltlOpt);
      return ioAnalyzer;
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

    void optionallyRunVisualizer(CodeThornOptions& ctOpt, CTAnalysis* analyzer, SgNode* root) {
      ROSE_ASSERT(analyzer->getTransitionGraph());
      ROSE_ASSERT(analyzer->getEStateSet());
      Visualizer visualizer(analyzer);
      if(ctOpt.visualization.vis) {
	cout << "generating graphvis files:"<<endl;
	visualizer.setOptionMemorySubGraphs(ctOpt.visualization.tg1EStateMemorySubgraphs);
	string dotFile="digraph G {\n";
	dotFile+=visualizer.transitionGraphToDot();
	dotFile+="}\n";
        string tg1DotFileName=ctOpt.reportFilePath+"/"+"transitiongraph1.dot";
	if(write_file(tg1DotFileName, dotFile)) {
          cout << "generated "<<tg1DotFileName<<endl;
        }
	string tg2DotFileData=visualizer.foldedTransitionGraphToDot();
        string tg2DotFileName=ctOpt.reportFilePath+"/"+"transitiongraph2.dot";
	if(write_file(tg2DotFileName, tg2DotFileData)) {
          cout << "generated "<<tg2DotFileName<<endl;
        }

	string datFile1=(analyzer->getTransitionGraph())->toString(analyzer->getVariableIdMapping());
        string tg1DatFileName=ctOpt.reportFilePath+"/"+"transitiongraph1.dat";
	if(write_file(tg1DatFileName, datFile1)) {
          cout << "generated "<<tg1DatFileName<<endl;
        }

	//analyzer->generateAstNodeInfo(analyzer->startFunRoot);
	//dotFile=astTermWithNullValuesToDot(analyzer->startFunRoot);
	SAWYER_MESG(logger[TRACE]) << "Option VIS: generate ast node info."<<endl;
	cout << "generating AST node info ... "<<endl;
	analyzer->generateAstNodeInfo(root);
	dotFile=AstTerm::functionAstTermsWithNullValuesToDot(root);
        string astDotFileName=ctOpt.reportFilePath+"/"+"ast.dot";
	if(write_file(astDotFileName, dotFile)) {
          cout << "generated AST file "<<astDotFileName<<endl;
        } else {
          cerr << "Error: failed to generate AST file "<<astDotFileName<<endl;
        }
	SAWYER_MESG(logger[TRACE]) << "Option VIS: generating icfg dot file ..."<<endl;

        //string icfgNonClusteredFileName=ctOpt.reportFilePath+"/"+"icfg_non_clustered.dot";
	//write_file(icfgNonClusteredFileName, analyzer->getFlow()->toDot(analyzer->getCFAnalyzer()->getLabeler(),analyzer->getTopologicalSort()));
        string icfgFileName=ctOpt.reportFilePath+"/"+"icfg.dot";
	DataDependenceVisualizer ddvis(analyzer->getLabeler(),analyzer->getVariableIdMapping(),"none");
	ddvis.generateDotFunctionClusters(root,analyzer->getCFAnalyzer(),icfgFileName,analyzer->getTopologicalSort());
        cout<<"Generated ICFG dot file "<<icfgFileName<<endl;

	cout << "=============================================================="<<endl;
      }
      if (ctOpt.visualization.icfgFileName.size()>0 && !ctOpt.visualization.vis) {
	string icfgFileName=ctOpt.visualization.icfgFileName;
	//DataDependenceVisualizer ddvis(analyzer->getLabeler(),analyzer->getVariableIdMapping(),"none");
	//ddvis.setDotGraphName("CFG");
	//ddvis.generateDotFunctionClusters(root,analyzer->getCFAnalyzer(),icfgFileName,analyzer->getTopologicalSort(),false);
        analyzer->getCFAnalyzer()->generateIcfgDotFile(ctOpt.visualization.icfgFileName,analyzer->getTopologicalSort());
	cout << "generated "<<icfgFileName<<" (top sort: "<<(analyzer->getTopologicalSort()!=0)<<")"<<endl;
      }


      if(ctOpt.visualization.visTg2) {
        string dotFile3=visualizer.foldedTransitionGraphToDot();
        write_file("transitiongraph2.dot", dotFile3);
        cout << "generated transitiongraph2.dot."<<endl;
      }

      if (ctOpt.visualization.dotIOStg.size()>0) {
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

    void optionallyGenerateExternalFunctionsFile(CodeThornOptions& ctOpt, FunctionCallMapping* funCallMapping) {
      if(ctOpt.generateReports) {
        std::string fileName=ctOpt.reportFilePath+"/"+ctOpt.externalFunctionsCSVFileName;
        if(fileName.size()>0) {
          if(!ctOpt.quiet)
            cout<<"Generated list of external functions in file "<<fileName<<endl;
          FunctionCallMapping::ExternalFunctionNameContainerType fnList=funCallMapping->getExternalFunctionNames();
          std::list<string> sList;
          for(auto fn : fnList)
            sList.push_back(fn);
          sList.sort(CppStdUtilities::compareCaseInsensitively);
          stringstream csvList;
          for(auto fn : sList) {
            csvList<<fn<<endl;
          }
          if(!CppStdUtilities::writeFile(fileName, csvList.str())) {
            cerr<<"Error: cannot write list of external functions to CSV file "<<fileName<<endl;
            exit(1);
          }
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
	if(ctOpt.status) cout<<"STATUS: Unparsing source code and exiting."<<endl;
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

    void runRoseAstChecks(SgProject* sageProject) {
      // Run internal consistency tests on AST
      AstTests::runAllTests(sageProject);
    }

    void optionallyRunRoseAstChecks(CodeThornOptions& ctOpt, SgProject* sageProject) {
      if(ctOpt.runRoseAstChecks) {
	if(ctOpt.status) cout<< "STATUS: ROSE AST checks started."<<endl;
	runRoseAstChecks(sageProject);
	if(ctOpt.status) cout << "STATUS: ROSE AST checks finished."<<endl;

        // test: constant expressions
        {
          if(ctOpt.status) cout <<"STATUS: testing constant expressions started."<<endl;
          CppConstExprEvaluator* evaluator=new CppConstExprEvaluator();
          list<SgExpression*> exprList=AstUtility::exprRootList(sageProject);
          SAWYER_MESG(logger[INFO]) <<"found "<<exprList.size()<<" expressions."<<endl;
          for(list<SgExpression*>::iterator i=exprList.begin();i!=exprList.end();++i) {
            EvalResult r=evaluator->traverse(*i);
            if(r.isConst()) {
              SAWYER_MESG(logger[TRACE])<<"Found constant expression: "<<(*i)->unparseToString()<<" eq "<<r.constValue()<<endl;
            }
          }
          delete evaluator;
	  if(ctOpt.status) cout << "STATUS: testing constant expressions finished."<<endl;
        }
      }
    }

    void optionallyRunRoseAstChecksAndExit(CodeThornOptions& ctOpt, SgProject* sageProject) {
      optionallyRunRoseAstChecks(ctOpt,sageProject);
      if(ctOpt.runRoseAstChecks) {
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

    SgProject* runRoseFrontEnd(int argc, char * argv[], CodeThornOptions& ctOpt, TimingCollector& timingCollector) {
      vector<string> argvList(argv,argv+argc);
      return runRoseFrontEnd(argvList, ctOpt, timingCollector);
    }

    SgProject* runRoseFrontEnd(vector<string>& argvList, CodeThornOptions& ctOpt, TimingCollector& timingCollector) {
      timingCollector.startTimer();
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
      timingCollector.stopTimer(TimingCollector::frontEnd);
      return project;
    }

    void optionallyPrintProgramInfos(CodeThornOptions& ctOpt, CTAnalysis* analyzer) {

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

      if(ctOpt.info.dumpFunctionCallMapping) {
        ROSE_ASSERT(analyzer->getCFAnalyzer());
        auto cfAnalyzer=analyzer->getCFAnalyzer();
        ROSE_ASSERT(cfAnalyzer->getFunctionCallMapping());
        auto fcMapping=cfAnalyzer->getFunctionCallMapping();
        fcMapping->dumpFunctionCallMapping();
      }

      // exit if any of above output is requested
      if(ctOpt.info.printVariableIdMapping||ctOpt.info.printTypeSizeMapping||ctOpt.info.typeSizeMappingCSVFileName.size()>0||ctOpt.info.dumpFunctionCallMapping) {
        exit(0);
      }

    }

    void optionallyRunNormalization(CodeThornOptions& ctOpt,SgProject* sageProject, TimingCollector& timingCollector) {
      timingCollector.startTimer();
      Normalization normalization;
      normalization.options.printPhaseInfo=ctOpt.normalizePhaseInfo;
      normalization.options.normalizeCplusplus=ctOpt.extendedNormalizedCppFunctionCalls; // PP(8/13/21)
      if(ctOpt.normalizeLevel>0) {
        if(ctOpt.quiet==false) {
          cout<<"STATUS: normalizing program (level "<<ctOpt.normalizeLevel<<")"<<endl;
        }
        //SAWYER_MESG(logger[INFO])<<"STATUS: normalizing program."<<endl;
        normalization.normalizeAst(sageProject,ctOpt.normalizeLevel);
      }
      timingCollector.stopTimer(TimingCollector::normalization);
      optionallyRunInliner(ctOpt,normalization, sageProject);
    }

    void setAssertConditionVariablesInAnalyzer(SgNode* root,CTAnalysis* analyzer) {
      SAWYER_MESG(logger[TRACE])<<"setAssertConditionVariablesInAnalyzer started"<<endl;
      ROSE_ASSERT(analyzer->getVariableIdMapping());
      AbstractValueSet varsInAssertConditions=AstUtility::determineVarsInAssertConditions(root,analyzer->getVariableIdMapping());
      SAWYER_MESG(logger[TRACE])<<"STATUS: determined "<<varsInAssertConditions.size()<< " variables in (guarding) assert conditions."<<endl;
      analyzer->setAssertCondVarsSet(varsInAssertConditions);
    }

    void optionallyEliminateRersArraysAndExit(CodeThornOptions& ctOpt, SgProject* sageProject, CTAnalysis* analyzer) {
      if(ctOpt.rers.eliminateArrays) {
        Specialization speci;
        speci.transformArrayProgram(sageProject, analyzer);
        sageProject->unparse(0,0);
        exit(0);
      }
    }

    void optionallyWriteSVCompWitnessFile(CodeThornOptions& ctOpt, CTAnalysis* analyzer) {
      if (ctOpt.svcomp.svcompMode && ctOpt.svcomp.witnessFileName.size()>0) {
        analyzer->writeWitnessToFile(ctOpt.svcomp.witnessFileName);
      }
    }

    void optionallyAnalyzeAssertions(CodeThornOptions& ctOpt, LTLOptions& ltlOpt, IOAnalyzer* analyzer, TimingCollector& tc) {
      tc.startTimer();
      bool withCe=ltlOpt.withCounterExamples || ltlOpt.withAssertCounterExamples;
      if(withCe) {
        SAWYER_MESG(logger[TRACE]) << "STATUS: extracting assertion traces (this may take some time)"<<endl;
        analyzer->extractRersIOAssertionTraces();
      }
      tc.stopTimer(TimingCollector::extractAssertionTraces);

      //tc.determinePrefixDepthTime= 0; // MJ: Determination of prefix depth currently deactivated.
      //int inputSeqLengthCovered = -1;

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

    void optionallyGenerateVerificationReports(CodeThornOptions& ctOpt,CTAnalysis* analyzer) {
      if(ctOpt.generateReports) {
        if(ctOpt.analysisList().size()>0) {
          if(ctOpt.status) cout<<"STATUS: generating verification reports: started."<<endl;
          const bool reportDetectedErrorLines=true;
          AnalysisReporting anaRep;

          anaRep.generateVerificationReports(ctOpt,analyzer,reportDetectedErrorLines); // also generates verification call graph
          anaRep.generateAnalyzedFunctionsAndFilesReports(ctOpt,analyzer);
	  anaRep.generateInternalAnalysisReport(ctOpt,analyzer);
	  anaRep.generateUnusedVariablesReport(ctOpt,analyzer);
          if(ctOpt.status) cout<<"STATUS: generating verification reports: finished."<<endl;
        } else {
          if(ctOpt.status) cout<<"STATUS: no analysis reports generated (no analysis selected)."<<endl;
        }
      }
    }

    void optionallyGenerateCallGraphDotFile(CodeThornOptions& ctOpt,CTAnalysis* analyzer) {
      std::string fileName=ctOpt.visualization.callGraphFileName;
      if(fileName.size()>0) {
        InterFlow::LabelToFunctionMap map=analyzer->getCFAnalyzer()->labelToFunctionMap(*analyzer->getFlow());
        //cout<<"DEBUG: labeltofunctionmap size:"<<map.size()<<endl;
        std::string dotFileString=analyzer->getInterFlow()->dotCallGraph(map);
        //cout<<"DEBUG: interflow size:"<<analyzer->getInterFlow()->size()<<endl;
        if(!CppStdUtilities::writeFile(fileName, dotFileString)) {
          cerr<<"Error: could not generate callgraph dot file "<<fileName<<endl;
          exit(1);
        } else {
          cout<<"Generated call graph dot file "<<fileName<<endl;
        }
      }

      {
        // new call graph (to replace above)
        std::string fileName=ctOpt.visualization.callGraphFileName2;
        if(fileName.size()>0) {
          string dotFileString=analyzer->getCFAnalyzer()->getCallGraph()->toDot(analyzer->getLabeler(),analyzer->getTopologicalSort());
          if(!CppStdUtilities::writeFile(fileName, dotFileString)) {
            cerr<<"Error: could not generate callgraph dot file "<<fileName<<endl;
            exit(1);
          } else {
            cout<<"Generated call graph dot file "<<fileName<<endl;
          }
        }
      }
    }
    /*
      void runSolver(CodeThornOptions& ctOpt,CTAnalysis* analyzer, SgProject* sageProject,TimingCollector& tc) {
      tc.startTimer();
      analyzer->printStatusMessageLine("==============================================================");
      if(!analyzer->getModeLTLDriven() && ctOpt.z3BasedReachabilityAnalysis==false && ctOpt.ssa==false) {
      switch(ctOpt.abstractionMode) {
      case 0:
      case 1:
      analyzer->runSolver();
      break;
      default:
      cout<<"Error: unknown abstraction mode "<<ctOpt.abstractionMode<<endl;
      exit(1);
      }
      }
      tc.stopTimer(TimingCollector::transitionSystemAnalysis);
      }
    */

    void normalizationPass(CodeThornOptions& ctOpt, SgProject* project) {
      CodeThorn::Normalization normalization;
      normalization.options.printPhaseInfo=ctOpt.normalizePhaseInfo;
      normalization.setInliningOption(ctOpt.inlineFunctions);
      normalization.options.normalizeCplusplus=ctOpt.extendedNormalizedCppFunctionCalls; // PP(8/13/21)
      normalization.normalizeAst(project,ctOpt.normalizeLevel);
    }

    VariableIdMappingExtended* createVariableIdMapping(CodeThornOptions& ctOpt, SgProject* project) {
      VariableIdMappingExtended* variableIdMapping=new VariableIdMappingExtended(); // createvid
      variableIdMapping->setAstSymbolCheckFlag(ctOpt.astSymbolCheckFlag);
      variableIdMapping->setArrayAbstractionIndex(ctOpt.arrayAbstractionIndex);
      if(ctOpt.vimReportFileName.size()>0)
	variableIdMapping->setErrorReportFileName(ctOpt.reportFilePath+"/"+ctOpt.vimReportFileName);
      variableIdMapping->setStatusFlag(ctOpt.status);
      variableIdMapping->computeVariableSymbolMapping(project);
      return variableIdMapping;
    }

    namespace
    {
      bool matchCxxCall(SgNode* n)
      {
        return SgNodeHelper::matchExtendedNormalizedCall(n);
      }
    };


    Labeler* createLabeler(SgProject* project, VariableIdMappingExtended* variableIdMapping, bool withCplusplus) {
      CTIOLabeler* res = new CTIOLabeler(variableIdMapping);
      if (withCplusplus)
        res->setIsFunctionCallFn(matchCxxCall);
      res->initialize(project);
      return res;
    }

#if 0
    CFAnalysis* createControlFlowGraph(CodeThornOptions& ctOpt, SgProject* project, Labeler* labeler) {
      CFAnalysis* cfAnalysis=new CFAnalysis(labeler);
      FunctionCallMapping2* functionCallMapping2=new FunctionCallMapping2();
      ClassHierarchyWrapper* classHierarchy=new ClassHierarchyWrapper(project);
      functionCallMapping2->setLabeler(labeler);
      functionCallMapping2->setClassHierarchy(classHierarchy);
      functionCallMapping2->computeFunctionCallMapping(project);
      cfAnalysis->setFunctionCallMapping2(functionCallMapping2);
      cfAnalysis->createICFG(project);
      return cfAnalysis;
    }
#endif

    string getRunTimeAndMemoryUsageReport(CodeThornOptions& /*ctOpt*/,TimingCollector& tc) {
      stringstream ss;
      ss<<tc.toString();
      ss<<"Total memory                   : "<<CodeThorn::getPhysicalMemorySize()/(1024*1024) <<" MiB"<<endl;
      ss<<"Total states allocation history: "<<EState::allocationHistoryToString()<<endl;
      ss<<"Total states alloc/dealloced   : "<<EState::allocationStatsToString()<<endl;
      return ss.str();
    }

    void optionallyPrintRunTimeAndMemoryUsageReport(CodeThornOptions& ctOpt,TimingCollector& tc) {
      if(ctOpt.status) cout<<getRunTimeAndMemoryUsageReport(ctOpt,tc);
    }

    void generateRunTimeAndMemoryUsageReport(CodeThornOptions& ctOpt,TimingCollector& tc) {
      if(ctOpt.generateReports) {
        string reportPathAndFile=ctOpt.reportFilePath+"/"+"runtime-memory-report.txt";
        write_file(reportPathAndFile, getRunTimeAndMemoryUsageReport(ctOpt,tc));
        if(ctOpt.status) cout<<"Generated runtime and memory usage report "<<reportPathAndFile<<endl;
      }
    }

    std::string programStatsToString(ProgramInfo* progInfo, VariableIdMappingExtended* vim) {
      stringstream ss;
      ss<<"========================="<<endl;
      ss<<"PROGRAM STATISTICS REPORT"<<endl;
      ss<<"========================="<<endl;
      ss<<progInfo->toStringDetailed();
      vim->typeSizeOverviewtoStream(ss);
      return ss.str();
    }

    std::string programStatsToString(ProgramInfo* progInfo1, ProgramInfo* progInfo2, VariableIdMappingExtended* vim) {
      stringstream ss;
      ss<<progInfo1->toStringCompared(progInfo2);
      vim->typeSizeOverviewtoStream(ss);
      return ss.str();
    }

    void generateProgramStats(CodeThornOptions& ctOpt, ProgramInfo* progInfo1, ProgramInfo* progInfo2, VariableIdMappingExtended* vim) {
      if(ctOpt.generateReports) {
        string reportPathAndFile=ctOpt.reportFilePath+"/"+"program-statistics-report.txt";
        write_file(reportPathAndFile, programStatsToString(progInfo1,progInfo2,vim));
        if(ctOpt.status) cout<<"Generated program statistics report "<<reportPathAndFile<<endl;
      }
    }

    bool astSymbolPointerCheck(CodeThornOptions& ctOpt, SgProject* node) {
      stringstream report;
      string fileName=ctOpt.reportFilePath+"/"+ctOpt.info.astSymbolPointerCheckReportFileName;
      uint8_t memoryPoolFillByte=0xdd;
      uint32_t memoryPoolFillPrefixToCheck=memoryPoolFillByte<<24|memoryPoolFillByte<<16|memoryPoolFillByte<<8|memoryPoolFillByte;
      uint32_t numSymZero=0;
      uint32_t numDeleted=0;
      uint32_t numChecked=0;
      if(fileName.size()>0) {
        stringstream lineColSeq;
        std::list<SgFunctionDefinition*> fdList=SgNodeHelper::listOfFunctionDefinitions(node);
        for(auto fd : fdList) {
          RoseAst ast(fd);
          list<string> lcList;
          for(auto n : ast) {
            if(SgVarRefExp* varRefExp=isSgVarRefExp(n)) {
	      // check symbol
	      SgSymbol* sym=varRefExp->get_symbol();
	      numChecked++;
	      if(sym==0) {
		report<<"symbol_is_zero: "<<SgNodeHelper::locationToString(varRefExp)<<endl;
		numSymZero++;
	      } else {
		if((uint32_t)(((uint64_t)sym)>>32) == memoryPoolFillPrefixToCheck) {
		  report<<"symbol_in_deleted_SgVarRefExp: "<<SgNodeHelper::locationToString(varRefExp)<<endl;
		  numDeleted++;
		}
	      }
	    }
	  }
	}
	report<<"Number of symbols in VarRefExp == 0: "<<numSymZero<<endl;
	report<<"Number of deleted VarRefExp        : "<<numDeleted<<endl;
	report<<"Number of checked VarRefExp        : "<<numChecked<<endl;
	if(ctOpt.status) cout<<"Generated ast symbol pointer report in file "<<fileName<<endl;
	write_file(fileName,report.str());
      }
      return numSymZero==0 && numDeleted==0;
    }


    void optionallyGenerateLineColumnCsv(CodeThornOptions& ctOpt, SgProject* node) {
      string fileName=ctOpt.info.astTraversalLineColumnCSVFileName;
      if(fileName.size()>0) {
        stringstream lineColSeq;
        std::list<SgFunctionDefinition*> fdList=SgNodeHelper::listOfFunctionDefinitions(node);
        for(auto fd : fdList) {
          RoseAst ast(fd);
          list<string> lcList;
          for(auto n : ast) {
            if(isSgExpression(n)) {
              string lc=CodeThorn::ProgramLocationsReport::findOriginalProgramLocationOfNode(n);

              // strip off column "...:xxxx"
              size_t pos = lc.rfind(":");
              if(pos != std::string::npos) {
                lc.erase(pos,lc.size()-pos);
              }

              // print source
              //lc+=(":"+SgNodeHelper::nodeToString(n));

              // add to result
              lcList.push_back(lc);
            }
          }
          lcList.unique(); // remove duplicates (e.g.  1,3,3,4,3,3,5 -> 1,3,4,3,5)
          for(auto lc : lcList)
            lineColSeq<<lc<<endl;
        }
        write_file(fileName,lineColSeq.str());
        if(ctOpt.status) cout<<"Generated line-column CSV info in file "<<fileName<<endl;
      }
    }

    string getCodeThornLibraryVersionNumber() {
      return CodeThornLibraryVersion;
    }

  } // end of namespace CodeThornLib

} // end of namespace CodeThorn

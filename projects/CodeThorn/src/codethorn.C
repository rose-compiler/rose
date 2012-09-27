/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "codethorn.h"
#include "SgNodeHelper.h"
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "StateRepresentation.h"
#include "Analyzer.h"
#include "LanguageRestrictor.h"
#include "Timer.h"
#include "LTL.h"
#include "LTLChecker.h"
#include <cstdio>
#include <cstring>
#include <boost/program_options.hpp>
#include <map>
#include "InternalChecks.h"
#include "Miscellaneous.h"

namespace po = boost::program_options;


bool CodeThornLanguageRestrictor::checkIfAstIsAllowed(SgNode* node) {
  MyAst ast(node);
  for(MyAst::iterator i=ast.begin();i!=ast.end();++i) {
	if(!isAllowedAstNode(*i)) {
	  cerr << "Error: Unsupported language construct found: " << (*i)->sage_class_name() << endl;
	  // report first error and return
	  switch((*i)->variantT()) {
	  V_SgDoWhileStmt: cerr << "Error info: cfg construction for do-while-statement not supported yet."<<endl; break;
	  V_SgForStatement: cerr << "Error info: cfg construction for for-statement not supported yet."<<endl; break;
	  V_SgContinueStmt: cerr << "Error info: cfg construction for continue-statement not supported yet."<<endl; break;
	  }
	  return false;
	}
  }
}

void checkProgram(SgNode* root) {
  LanguageRestrictor lr;
  lr.allowAstNodesRequiredForEmptyProgram();
  LanguageRestrictor::VariantSet vs;
  vs.insert(V_SgIntVal);
  vs.insert(V_SgAssignOp);
  vs.insert(V_SgCastExp);
  vs.insert(V_SgVarRefExp);
  vs.insert(V_SgExprStatement);
  vs.insert(V_SgIfStmt);
  vs.insert(V_SgWhileStmt);
  vs.insert(V_SgBreakStmt);
  vs.insert(V_SgAndOp);
  vs.insert(V_SgOrOp);
  vs.insert(V_SgNotOp);
  vs.insert(V_SgNotEqualOp);
  vs.insert(V_SgEqualityOp);
  vs.insert(V_SgIntVal);
  vs.insert(V_SgVariableDeclaration);
  vs.insert(V_SgAddOp);
  vs.insert(V_SgReturnStmt);
  vs.insert(V_SgAssignInitializer);
  vs.insert(V_SgBoolValExp);
  vs.insert(V_SgLabelStatement);
  vs.insert(V_SgNullStatement);
  vs.insert(V_SgConditionalExp); // TODO (assignments not handled!)
  vs.insert(V_SgMinusOp);

  // inter-procedural
  vs.insert(V_SgFunctionCallExp);
  vs.insert(V_SgFunctionRefExp);
  vs.insert(V_SgExprListExp);

  // rers Problems
  vs.insert(V_SgTypedefDeclaration);
  vs.insert(V_SgClassDeclaration);
  vs.insert(V_SgClassDefinition);
  vs.insert(V_SgEnumDeclaration);
  vs.insert(V_SgStringVal);
  vs.insert(V_SgAddressOfOp);

  // rers Problems 10-13
  // arithmetic operators
  vs.insert(V_SgSubtractOp);
  vs.insert(V_SgMultiplyOp);
  vs.insert(V_SgDivideOp);
  vs.insert(V_SgModOp);
  lr.setAstNodeVariantSet(vs,true);
  cout << "INIT: Running CodeThorn language restrictor."<<endl;
  bool valid=lr.checkIfAstIsAllowed(root);
  if(!valid) {
	cout << "INIT FAILED: Input program not valid. No analysis performed."<<endl;
	exit(1);
  }
}

void generateAssertsCsvFile(Analyzer& analyzer, SgProject* sageProject, string filename) {
  ofstream* csv = NULL;
  csv = new ofstream();
  // use binary and \r\n tp enforce DOS line endings
  // http://tools.ietf.org/html/rfc4180
  csv->open(filename.c_str(), ios::trunc|ios::binary);
  *csv << "Index;\"Assert Error Label\";ReachabilityResult;Confidence\r\n";
  
  LabelSet lset=analyzer.getTransitionGraph()->labelSetOfIoOperations(InputOutput::FAILED_ASSERT);
  list<pair<SgLabelStatement*,SgNode*> > assertNodes=analyzer.listOfLabeledAssertNodes(sageProject);
  int cnt=1;
  for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=assertNodes.begin();i!=assertNodes.end();++i) {
	*csv << cnt 
		 << ";"
		 << SgNodeHelper::getLabelName((*i).first)
		 <<";"
	  ;
	Label lab=analyzer.getLabeler()->getLabel((*i).second);
	if(lset.find(lab)!=lset.end()) {
	  *csv << "YES;9";
	} else {
	  *csv << "NO;9";
	}
	*csv << "\r\n";
	cnt++;
  }
  if (csv) delete csv;
}

void printAsserts(Analyzer& analyzer, SgProject* sageProject) {
  LabelSet lset=analyzer.getTransitionGraph()->labelSetOfIoOperations(InputOutput::FAILED_ASSERT);
  list<pair<SgLabelStatement*,SgNode*> > assertNodes=analyzer.listOfLabeledAssertNodes(sageProject);
  for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=assertNodes.begin();i!=assertNodes.end();++i) {
	cout << "assert: "
		 << SgNodeHelper::getLabelName((*i).first)
	  //	 << SgNodeHelper::nodeToString((*i).second)<< " : "
	  ;
	cout << ": ";
	Label lab=analyzer.getLabeler()->getLabel((*i).second);
	if(lset.find(lab)!=lset.end()) {
	  cout << color("green")<<"YES (REACHABLE)"<<color("normal");
	}
	else {
	  cout << color("cyan")<<"NO (UNREACHABLE)"<<color("normal");
	}
	cout << endl;
  }
}

void printAssertStatistics(Analyzer& analyzer, SgProject* sageProject) {
  LabelSet lset=analyzer.getTransitionGraph()->labelSetOfIoOperations(InputOutput::FAILED_ASSERT);
  list<pair<SgLabelStatement*,SgNode*> > assertNodes=analyzer.listOfLabeledAssertNodes(sageProject);
  int reachable=0;
  int unreachable=0;
  for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=assertNodes.begin();i!=assertNodes.end();++i) {
	Label lab=analyzer.getLabeler()->getLabel((*i).second);
	if(lset.find(lab)!=lset.end())
	  reachable++;
	else
	  unreachable++;
  }
  int n=assertNodes.size();
  assert(reachable+unreachable == n);
  cout<<"Assert reachability statistics: "
	  <<reachable          <<"/"<<n<<color("green")  <<" YES"       <<color("normal")<<", " 	
	  <<unreachable        <<"/"<<n<<color("cyan")   <<" NO "       <<color("normal") 	
	  <<endl
	;
}

void generateLTLOutput(Analyzer& analyzer, string ltl_file) {
  //
  // Verification
  //
  int n = 0;
  int n_yes = 0;
  int n_no = 0;
  int n_undecided = 0;
  int n_failed = 0;

  if (ltl_file.size()) {
    LTL::Checker checker(*analyzer.getEStateSet(),
			 *analyzer.getTransitionGraph());
    ltl_input = fopen(ltl_file.c_str(), "r");

    ofstream* csv = NULL;
    if (args.count("csv-ltl")) {
      csv = new ofstream();
      // use binary and \r\n tp enforce DOS line endings
      // http://tools.ietf.org/html/rfc4180
      csv->open(args["csv-ltl"].as<string>().c_str(), ios::trunc|ios::binary);
      *csv << "Index;\"LTL formula\";Result;Confidence\r\n";
    }

    while ( !ltl_eof) {
      try { 
	ltl_label = 0;
        if (ltl_parse()) {
          cerr<<color("red")<< "Syntax error" <<color("normal")<<endl;
	  ++n;
	  ++n_failed;
	  continue;
        }
        if (ltl_val == NULL) {
	  // empty line
	  continue;
	}
      } catch(const char* s) {
        if (ltl_val) cout<<color("normal")<<string(*ltl_val)<<endl;
        cout<< s<<endl<<color("red")<< "Grammar Error" <<color("normal")<<endl;
	++n;
	++n_failed;
	continue;
      } catch(...) {
	cout<<color("red")<< "Parser exception" << endl;
	++n;
	++n_failed;
	continue;
      }  
	  
      ++n;
      string formula = *ltl_val;
      cout<<endl<<"Verifying formula "<<color("white")<<formula<<color("normal")<<"."<<endl;
      if (csv) *csv << n <<";\"" <<formula<<"\";";
      try {
	AType::BoolLattice result = checker.verify(*ltl_val);
	if (result.isTrue()) {
	  ++n_yes;
	  cout<<color("green")<<"YES"<<color("normal")<<endl;
	  if (csv) *csv << "YES;9\r\n";
	} else if (result.isFalse()) {
	  ++n_no;
	  cout<<color("cyan")<<"NO"<<color("normal")<<endl;
	  if (csv) *csv << "NO;9\r\n";
	} else {
	  ++n_undecided;
	  cout<<color("magenta")<<"UNKNOWN"<<color("normal")<<endl;
	  if (csv) *csv << "UNKNOWN;0\r\n";
	}
      } catch(const char* str) {
	++n_failed;
	cerr << "Exception raised: " << str << endl;
	cout<<color("red")<<"ERROR"<<color("normal")<<endl;
	if (csv) *csv << "ERROR;0\r\n";
      } catch(string str) {
	++n_failed;
	cerr << "Exception raised: " << str << endl;
	cout<<color("red")<<"ERROR"<<color("normal")<<endl;
	if (csv) *csv << "ERROR;0\r\n";
      } catch(...) {
	++n_failed;
	cout<<color("red")<<"ERROR"<<color("normal")<<endl;
	if (csv) *csv << "ERROR;0\r\n";
      }  
    }
    if (csv) delete csv;
    fclose(ltl_input);
    assert(n_yes+n_no+n_undecided+n_failed == n);
    cout<<"\nStatistics "<<endl
        <<"========== "<<endl
	<<n_yes      <<"/"<<n<<color("green")  <<" YES, "       <<color("normal") 	
	<<n_no       <<"/"<<n<<color("cyan")   <<" NO, "        <<color("normal") 	
	<<n_undecided<<"/"<<n<<color("magenta")<<" UNKNOWN, "   <<color("normal") 	
	<<n_failed   <<"/"<<n<<color("red")    <<" ERROR"       <<color("normal") 	
	<<endl;

  } 
}


int main( int argc, char * argv[] ) {
  string ltl_file;
  try {
	Timer timer;
	timer.start();

  // Command line option handling.
  po::options_description desc
    ("CodeThorn 1.1 [RC1]\n"
     "Supported options");

  desc.add_options()
    ("help,h", "produce this help message")
    ("rose-help", "show help for compiler frontend options")
    ("version,v", "display the version")
    ("internal-checks", "run internal consistency checks (without input program)")
    ("verify", po::value< string >(), "verify all LTL formulae in the file [arg]")
    ("csv-ltl", po::value< string >(), "output LTL verification results into a CSV file [arg]")
    ("csv-assert", po::value< string >(), "output assert reachability results into a CSV file [arg]")
    ("tg1-estate-address", po::value< string >(), "transition graph 1: visualize address [=yes|no]")
    ("tg1-estate-id", po::value< string >(), "transition graph 1: visualize estate-id [=yes|no]")
    ("tg1-estate-properties", po::value< string >(), 
     "transition graph 1: visualize all estate-properties [=yes|no]")
    ("tg2-estate-address", po::value< string >(), "transition graph 2: visualize address [=yes|no]")
    ("tg2-estate-id", po::value< string >(), "transition graph 2: visualize estate-id [=yes|no]")
    ("tg2-estate-properties", po::value< string >(),
     "transition graph 2: visualize all estate-properties [=yes|no]")
    ("colors",po::value< string >(),"use colors in output [=yes|no]")
    ("report-stdout",po::value< string >(),"report stdout estates during analysis [=yes|no]")
    ("report-failed-assert",po::value< string >(),
     "report failed assert estates during analysis [=yes|no]")
    ("precision-equality-constraints",po::value< string >(),
     "(experimental) use constraints for determining estate equality [=yes|no]")
    ("precision-equality-io",po::value< string >(),
     "(experimental) use constraints for determining estate equality [=yes|no]")
    ("precision-bool",po::value< string >(),
     "use precise top with bool-(and/or) operators (used in LTL)")
    ("precision-intbool",po::value< string >(),
     "use precise top with intbool-(and/or) operators (used in int-analyzer)")
    ("precision-exact-constraints",po::value< string >(),
     "use precise constraint extraction (experimental)")
    ("tg-ltl-reduced",po::value< string >(),"compute LTL-reduced transition graph")
    ;

  po::store(po::command_line_parser(argc, argv).
	    options(desc).allow_unregistered().run(), args);
  po::notify(args);

  if (args.count("help")) {
    cout << desc << "\n";
    return 0;
  }

  if (args.count("rose-help")) {
    argv[1] = strdup("--help");
  }

  if (args.count("version")) {
    cout << "CodeThorn version 1.1 [RC1]\n";
    cout << "Written by Markus Schordan and Adrian Prantl\n";
    cout << "2012\n";
    return 0;
  }

  boolOptions.init(argc,argv);
  boolOptions.registerOption("tg1-estate-address",false);
  boolOptions.registerOption("tg1-estate-id",false);
  boolOptions.registerOption("tg1-estate-properties",true);
  boolOptions.registerOption("tg2-estate-address",false);
  boolOptions.registerOption("tg2-estate-id",true);
  boolOptions.registerOption("tg2-estate-properties",false);
  boolOptions.registerOption("colors",true);
  boolOptions.registerOption("report-stdout",false);
  boolOptions.registerOption("report-failed-assert",false);
  boolOptions.registerOption("precision-equality-constraints",true);
  boolOptions.registerOption("precision-equality-io",true);
  boolOptions.registerOption("precision-bool",true);
  boolOptions.registerOption("precision-intbool",true);
  boolOptions.registerOption("precision-exact-constraints",false);
  boolOptions.registerOption("tg-ltl-reduced",false);
  boolOptions.processOptions();
  cout<<boolOptions.toString();

  if (args.count("internal-checks")) {
	if(internalChecks(argc,argv)==false)
	  return 1;
	else
	  return 0;
  }
  
  // clean up verify and csv-ltl option in argv
  if (args.count("verify")) {
    ltl_file = args["verify"].as<string>();
	for (int i=1; i<argc; ++i) {
	  if ((string(argv[i]) == "--verify") || 
	      (string(argv[i]) == "--csv-ltl")) {
		// do not confuse ROSE frontend
		argv[i] = strdup("");
		assert(i+1<argc);
		argv[i+1] = strdup("");
	  }
	}
  }

  // clean up csv-assert option in argv
  for (int i=1; i<argc; ++i) {
	if (string(argv[i]) == "--csv-assert") {
	  // do not confuse ROSE frontend
	  argv[i] = strdup("");
	  assert(i+1<argc);
		argv[i+1] = strdup("");
	}
  }

  // print version information
#ifdef STATESET_REF
  cout << "INFO: CodeThorn 1.1. (RC1): Slow reference implementation (using STATESET_REF)."<<endl;
#else
  cout << "INFO: CodeThorn 1.1 (RC1): Fast sequential implementation (STATE_SET)."<<endl;
#endif
#ifdef STATESET_REF
  cout << "INFO: CodeThorn 1.1. (RC1): Slow reference implementation (using ESTATESET_REF)."<<endl;
#else
  cout << "INFO: CodeThorn 1.1 (RC1): Fast sequential implementation (ESTATE_SET)."<<endl;
#endif
  // Build the AST used by ROSE
  cout << "INIT: Parsing and creating AST."<<endl;
  SgProject* sageProject = frontend(argc,argv);
  
  cout << "INIT: Running ROSE AST tests."<<endl;
  // Run internal consistency tests on AST
  AstTests::runAllTests(sageProject);

  SgNode* root=sageProject;
  checkProgram(root);

  cout << "INIT: Running variable<->symbol mapping check."<<endl;

  VariableIdMapping varIdMap;
  varIdMap.computeVariableSymbolMapping(sageProject);
  cout << "STATUS: Variable<->Symbol mapping created."<<endl;
  if(!varIdMap.isUniqueVariableSymbolMapping()) {
	cerr << "WARNING: Variable<->Symbol mapping not bijective."<<endl;
	//varIdMap.reportUniqueVariableSymbolMappingViolations();
  }
  cout << "INIT: creating solver."<<endl;
  Analyzer analyzer;
  analyzer.initializeSolver1("main",root);

  cout << "=============================================================="<<endl;
  analyzer.runSolver1();
  timer.stop();
  //  cout << analyzer.stateSetToString(final);
  cout << "=============================================================="<<endl;
  printAsserts(analyzer,sageProject);
  if (args.count("csv-assert")) {
	string filename=args["csv-assert"].as<string>().c_str();
	generateAssertsCsvFile(analyzer,sageProject,filename);
  }
  cout << "=============================================================="<<endl;
  if (ltl_file.size()) {
	generateLTLOutput(analyzer,ltl_file);
	cout << "=============================================================="<<endl;
  }
  printAssertStatistics(analyzer,sageProject);
  cout << "=============================================================="<<endl;

  double totalRunTime=timer.getElapsedTimeInMilliSec();

  long stateSetSize=analyzer.getStateSet()->size();
  long stateSetBytes=analyzer.getStateSet()->memorySize();
  long eStateSetSize=analyzer.getEStateSet()->size();
  long eStateSetBytes=analyzer.getEStateSet()->memorySize();
  long transitionGraphSize=analyzer.getTransitionGraph()->size();
  long transitionGraphBytes=transitionGraphSize*sizeof(Transition);
  long numOfconstraintSets=analyzer.getConstraintSetMaintainer()->numberOfConstraintSets();
  long constraintSetsBytes=analyzer.getConstraintSetMaintainer()->memorySize();

  cout << "Number of stdin-estates        : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR))<<color("white")<<endl;
  cout << "Number of stdout-estates       : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR))<<color("white")<<endl;
  cout << "Number of stderr-estates       : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR))<<color("white")<<endl;
  cout << "Number of failed-assert-estates: "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT))<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout << "Number of states               : "<<color("magenta")<<stateSetSize<<color("white")<<" (memory: "<<color("magenta")<<stateSetBytes<<color("white")<<" bytes)"<<endl;
  cout << "Number of estates              : "<<color("cyan")<<eStateSetSize<<color("white")<<" (memory: "<<color("cyan")<<eStateSetBytes<<color("white")<<" bytes)"<<endl;
  cout << "Number of transitions          : "<<color("blue")<<transitionGraphSize<<color("white")<<" (memory: "<<color("blue")<<transitionGraphBytes<<color("white")<<" bytes)"<<endl;
  cout << "Number of constraint sets      : "<<color("yellow")<<numOfconstraintSets<<color("white")<<" (memory: "<<color("yellow")<<constraintSetsBytes<<color("white")<<" bytes)"<<endl;
  cout << "=============================================================="<<endl;
  cout << "Memory total         : "<<color("green")<<stateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes<<" bytes"<<color("normal")<<endl;
  if(totalRunTime<1000.0) 
	cout << "Time total           : "<<color("green")<<totalRunTime<<" ms"<<color("normal")<<endl;
  else
	cout << "Time total           : "<<color("green")<<totalRunTime/1000.0<<" seconds"<<color("normal")<<endl;
  cout << "=============================================================="<<endl;

  if(eStateSetSize>2500) {
	cout << "Number of eStates > 2500. Not generating visualization."<<endl;
  } else {
    Visualizer visualizer(analyzer.getLabeler(),analyzer.getFlow(),analyzer.getStateSet(),analyzer.getEStateSet(),analyzer.getTransitionGraph());
    string dotFile="digraph G {\n";
    dotFile+=visualizer.transitionGraphToDot();
    dotFile+="}\n";
    write_file("transitiongraph1.dot", dotFile);
    cout << "generated transitiongraph1.dot."<<endl;
    string dotFile3=visualizer.foldedTransitionGraphToDot();
    write_file("transitiongraph2.dot", dotFile3);
    cout << "generated transitiongraph2.dot."<<endl;

	string datFile1=(analyzer.getTransitionGraph())->toString();
	write_file("transitiongraph1.dat", datFile1);
    cout << "generated transitiongraph1.dat."<<endl;

    assert(analyzer.startFunRoot);
    //analyzer.generateAstNodeInfo(analyzer.startFunRoot);
    //dotFile=astTermWithNullValuesToDot(analyzer.startFunRoot);
    analyzer.generateAstNodeInfo(sageProject);
    dotFile=functionAstTermsWithNullValuesToDot(sageProject);
    write_file("ast.dot", dotFile);
    cout << "generated ast.dot."<<endl;
    
    write_file("cfg.dot", analyzer.flow.toDot(analyzer.cfanalyzer->getLabeler()));
    cout << "generated cfg.dot."<<endl;

  }

#if 0
  {
    cout << "MAP:"<<endl;
    cout << analyzer.getLabeler()->toString();
  }
#endif

#if 0
  // check output var to be constant in transition graph
  TransitionGraph* tg=analyzer.getTransitionGraph();
  for(TransitionGraph::iterator i=tg->begin();i!=tg->end();++i) {
	const EState* es1=(*i).source;
	assert(es1->io.op==InputOutput::STDOUT_VAR && es1->state->varIsConst(es1->io.var));
  }
#endif


  // reset terminal
  cout<<color("normal")<<"done."<<endl;
  
  } catch(char* str) {
	cerr << "*Exception raised: " << str << endl;
  } catch(const char* str) {
	cerr << "Exception raised: " << str << endl;
  } catch(string str) {
	cerr << "Exception raised: " << str << endl;
 }
  return 0;
}


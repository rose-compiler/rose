/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "codethorn.h"
#include "SgNodeHelper.h"
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "StateRepresentations.h"
#include "Analyzer.h"
#include "LanguageRestrictor.h"
#include "Timer.h"
#include "LTLCheckerFixpoint.h"
#include "LTLCheckerUnified.h"
#include <cstdio>
#include <cstring>
#include <boost/program_options.hpp>
#include <map>
#include "InternalChecks.h"
#include "Miscellaneous.h"

namespace po = boost::program_options;
using namespace CodeThorn;

bool CodeThornLanguageRestrictor::checkIfAstIsAllowed(SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	if(!isAllowedAstNode(*i)) {
	  cerr << "Language-Restrictor: excluded language construct found: " << (*i)->sage_class_name() << endl;
	  // report first error and return
	  if((*i)->variantT()==V_SgContinueStmt) {
		cerr << "cfg construction for continue-statement not supported yet."<<endl; break;
	  }
	  return false;
	}
  }
  return true;
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
  vs.insert(V_SgDoWhileStmt);

  vs.insert(V_SgForInitStatement);
  vs.insert(V_SgForStatement);

  vs.insert(V_SgBreakStmt);
  vs.insert(V_SgAndOp);
  vs.insert(V_SgOrOp);
  vs.insert(V_SgNotOp);
  vs.insert(V_SgNotEqualOp);
  vs.insert(V_SgEqualityOp);
  vs.insert(V_SgIntVal);
  vs.insert(V_SgVariableDeclaration);
  vs.insert(V_SgReturnStmt);
  vs.insert(V_SgAssignInitializer);
  vs.insert(V_SgBoolValExp);
  vs.insert(V_SgLabelStatement);
  vs.insert(V_SgNullStatement);
  vs.insert(V_SgConditionalExp); // TODO: case if inside expressions
  vs.insert(V_SgMinusOp);

  vs.insert(V_SgPlusPlusOp);
  vs.insert(V_SgMinusMinusOp);

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
  vs.insert(V_SgAddOp);
  vs.insert(V_SgSubtractOp);
  vs.insert(V_SgMultiplyOp);
  vs.insert(V_SgDivideOp);
  vs.insert(V_SgModOp);
  vs.insert(V_SgGreaterOrEqualOp);
  vs.insert(V_SgLessThanOp);
  vs.insert(V_SgGreaterThanOp);
  vs.insert(V_SgLessOrEqualOp);

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
  //*csv << "Index;\"Assert Error Label\";ReachabilityResult;Confidence\r\n";
  
  LabelSet lset=analyzer.getTransitionGraph()->labelSetOfIoOperations(InputOutput::FAILED_ASSERT);
  list<pair<SgLabelStatement*,SgNode*> > assertNodes=analyzer.listOfLabeledAssertNodes(sageProject);
  if(boolOptions["rers-binary"]) {
	for(int i=0;i<62;i++) {
	  *csv << i<<",";
	  if(analyzer.binaryBindingAssert[i]) {
		*csv << "yes,9";
	  } else {
		*csv << "no,9";
	  }
	  *csv << "\n";
	}
  } else {
	for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=assertNodes.begin();i!=assertNodes.end();++i) {
	  string name=SgNodeHelper::getLabelName((*i).first);
	  if(name=="globalError")
		name="error_60";
	  name=name.substr(6,name.size()-6);
	  *csv << name
		   <<","
		;
	  Label lab=analyzer.getLabeler()->getLabel((*i).second);
	  if(lset.find(lab)!=lset.end()) {
		*csv << "yes,9";
	  } else {
		*csv << "no,9";
	  }
	  *csv << "\n";
	}
  }
  if (csv) delete csv;
}

void printAsserts(Analyzer& analyzer, SgProject* sageProject) {
  if(boolOptions["rers-binary"]) {
	for(int i=0;i<62;i++) {
	  cout << "assert: error_"<<i<<": ";
	  if(analyzer.binaryBindingAssert[i]) {
		cout << color("green")<<"YES (REACHABLE)"<<color("normal");
	  } else {
		cout << color("cyan")<<"NO (UNREACHABLE)"<<color("normal");
	  }
	  cout << endl;
	}
	return;
  }
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

  if(boolOptions["rers-binary"]) {
	reachable=0;
	unreachable=0;
	for(int i=0;i<62;i++) {
	  if(analyzer.binaryBindingAssert[i])
		reachable++;
	  else
		unreachable++;
	}
  }
  cout<<"Assert reachability statistics: "
	  <<color("normal")<<"YES: "<<color("green")<<reachable
	  <<color("normal")<<", NO: " <<color("cyan")<<unreachable
	  <<color("normal")<<", TOTAL: " <<n
	  <<endl
	;
}

void generateLTLOutput(Analyzer& analyzer, string ltl_file) {
  extern CodeThorn::LTL::Formula* ltl_val;
  //
  // Verification
  //
  int n = 0;
  int n_yes = 0;
  int n_no = 0;
  int n_undecided = 0;
  int n_failed = 0;

  assert(analyzer.getEStateSet());
  assert(analyzer.getTransitionGraph());
  if (ltl_file.size()) {
    CodeThorn::FixpointLTL::Checker* checker1 = 0; 
    CodeThorn::UnifiedLTL::UChecker* checker2 = 0; 
    switch(analyzer.getLTLVerifier()) {
    case 1: 
      checker1 = new CodeThorn::FixpointLTL::Checker(*analyzer.getEStateSet(), 
						     *analyzer.getTransitionGraph());
      break;
    case 2:
      checker2 = new CodeThorn::UnifiedLTL::UChecker(*analyzer.getEStateSet(),
						     *analyzer.getTransitionGraph());
      break;
    default: 
      cerr << "Error: unknown ltl-verifier specified with ltl-verifier option."<<endl;
      exit(1);
    }
    ltl_input = fopen(ltl_file.c_str(), "r");
    if (ltl_input == NULL)
      cerr<<"Error: could not open file "<<ltl_file.c_str()<<endl;
    assert(ltl_input);

    ofstream* csv = NULL;
    if (args.count("csv-ltl")) {
      csv = new ofstream();
      // use binary and \r\n tp enforce DOS line endings
      // http://tools.ietf.org/html/rfc4180
      csv->open(args["csv-ltl"].as<string>().c_str(), ios::trunc|ios::binary);
      //*csv << "Index,\"LTL formula\",Result,Confidence\r\n";
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
      //if (csv) *csv << n <<";\"" <<formula<<"\";";
      if (csv) *csv << n+60 <<",";
      try {
	AType::BoolLattice result;
	if (checker1) result = checker1->verify(*ltl_val);
	if (checker2) result = checker2->verify(*ltl_val);

	if (result.isTrue()) {
	  ++n_yes;
	  cout<<color("green")<<"YES"<<color("normal")<<endl;
	  if (csv) *csv << "yes,9\r\n";
	} else if (result.isFalse()) {
	  ++n_no;
	  cout<<color("cyan")<<"NO"<<color("normal")<<endl;
	  if (csv) *csv << "no,9\r\n";
	} else {
	  ++n_undecided;
	  cout<<color("magenta")<<"UNKNOWN"<<color("normal")<<endl;
	  if (csv) *csv << "unknown,0\r\n";
	}
      } catch(const char* str) {
	++n_failed;
	cerr << "Exception raised: " << str << endl;
	cout<<color("red")<<"ERROR"<<color("normal")<<endl;
	if (csv) *csv << "error,0\r\n";
      } catch(string str) {
	++n_failed;
	cerr << "Exception raised: " << str << endl;
	cout<<color("red")<<"ERROR"<<color("normal")<<endl;
	if (csv) *csv << "error,0\r\n";
      } catch(...) {
	++n_failed;
	cout<<color("red")<<"ERROR"<<color("normal")<<endl;
	if (csv) *csv << "error,0\r\n";
      }  
    }
    fclose(ltl_input);
    if (csv) delete csv;
    if (checker1) delete checker1;
    if (checker2) delete checker2;
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

string readableruntime(double time) {
  stringstream s;
  if(time<1000.0) {
	s<<time<<" ms";
	return s.str();
  } else {
	time=time/1000;
  }
  if(time<60) {
	s<<time<<" secs"; 
	return s.str();
  } else {
	time=time/60;
  }
  if(time<60) {
	s<<time<<" mins"; 
	return s.str();
  } else {
	time=time/60;
  }
  if(time<24) {
	s<<time<<" hours"; 
	return s.str();
  } else {
	time=time/24;
  }
  if(time<31) {
	s<<time<<" days"; 
	return s.str();
  } else {
	time=time/(((double)(365*3+366))/12*4);
  }
  s<<time<<" months"; 
  return s.str();
}

int main( int argc, char * argv[] ) {
  string ltl_file;
  try {
	Timer timer;
	timer.start();

  // Command line option handling.
  po::options_description desc
    ("CodeThorn V1.2\n"
     "Written by Markus Schordan and Adrian Prantl 2012\n"
     "Supported options");

  desc.add_options()
    ("help,h", "produce this help message")
    ("rose-help", "show help for compiler frontend options")
    ("version,v", "display the version")
    ("internal-checks", "run internal consistency checks (without input program)")
    ("verify", po::value< string >(), "verify all LTL formulae in the file [arg]")
    ("ltl-verifier",po::value< int >(),"specify which ltl-verifier to use [=1|2]")
    ("debug-mode",po::value< int >(),"set debug mode [arg]")
    ("csv-ltl", po::value< string >(), "output LTL verification results into a CSV file [arg]")
    ("csv-assert", po::value< string >(), "output assert reachability results into a CSV file [arg]")
    ("csv-assert-live", po::value< string >(), "output assert reachability results during analysis into a CSV file [arg]")
    ("csv-stats",po::value< string >(),"output statistics into a CSV file [arg]")
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
    ("report-stderr",po::value< string >(),"report stderr estates during analysis [=yes|no]")
    ("report-failed-assert",po::value< string >(),
     "report failed assert estates during analysis [=yes|no]")
    ("precision-intbool",po::value< string >(),
     "use precise top with intbool-(and/or) operators (used in int-analyzer) [=yes|no]")
    ("precision-exact-constraints",po::value< string >(),
     "(experimental) use precise constraint extraction [=yes|no]")
    ("tg-ltl-reduced",po::value< string >(),"(experimental) compute LTL-reduced transition graph based on a subset of computed estates [=yes|no]")
    ("semantic-fold",po::value< string >(),"compute semantically folded state transition graph [=yes|no]")
    ("post-semantic-fold",po::value< string >(),"compute semantically folded state transition graph only after the complete transition graph has been computed. [=yes|no]")
    ("report-semantic-fold",po::value< string >(),"report each folding operation with the respective number of estates. [=yes|no]")
    ("semantic-fold-threshold",po::value< int >(),"Set threshold with <arg> for semantic fold operation (experimental)")
    ("post-collapse-stg",po::value< string >(),"compute collapsed state transition graph after the complete transition graph has been computed. [=yes|no]")
    ("viz",po::value< string >(),"generate visualizations (dot) outputs [=yes|no]")
    ("update-input-var",po::value< string >(),"For testing purposes only. Default is Yes. [=yes|no]")
    ("run-rose-tests",po::value< string >(),"Run ROSE AST tests. [=yes|no]")
    ("reduce-cfg",po::value< string >(),"Reduce CFG nodes which are not relevant for the analysis. [=yes|no]")
    ("threads",po::value< int >(),"Run analyzer in parallel using <arg> threads (experimental)")
    ("display-diff",po::value< int >(),"Print statistics every <arg> computed estates.")
	("ltl-output-dot",po::value< string >(),"LTL visualization: generate dot output.")
	("ltl-show-derivation",po::value< string >(),"LTL visualization: show derivation in dot output.")
	("ltl-show-node-detail",po::value< string >(),"LTL visualization: show node detail in dot output.")
	("ltl-collapsed-graph",po::value< string >(),"LTL visualization: show collapsed graph in dot output.")
	("input-var-values",po::value< string >(),"specify a set of input values (e.g. \"{1,2,3}\")")
    ("input-var-values-as-constraints",po::value<string >(),"represent input var values as constraints (otherwise as constants in PState)")
    ("arith-top",po::value< string >(),"Arithmetic operations +,-,*,/,% always evaluate to top [=yes|no]")
    ("abstract-interpreter",po::value< string >(),"Run analyzer in abstract interpreter mode. Use [=yes|no]")
    ("rers-binary",po::value< string >(),"Call rers binary functions in analysis. Use [=yes|no]")
	("print-all-options",po::value< string >(),"print all yes/no command line options.")
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
    cout << "CodeThorn version 1.2\n";
    cout << "Written by Markus Schordan and Adrian Prantl 2012\n";
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
  boolOptions.registerOption("report-stderr",false);
  boolOptions.registerOption("report-failed-assert",false);
  boolOptions.registerOption("precision-intbool",true);
  boolOptions.registerOption("precision-exact-constraints",false);
  boolOptions.registerOption("tg-ltl-reduced",false);
  boolOptions.registerOption("semantic-fold",false);
  boolOptions.registerOption("post-semantic-fold",false);
  boolOptions.registerOption("report-semantic-fold",false);
  boolOptions.registerOption("post-collapse-stg",true);

  boolOptions.registerOption("viz",false);
  boolOptions.registerOption("update-input-var",true);
  boolOptions.registerOption("run-rose-tests",false);
  boolOptions.registerOption("reduce-cfg",true);
  boolOptions.registerOption("print-all-options",false);

  boolOptions.registerOption("ltl-output-dot",false);
  boolOptions.registerOption("ltl-show-derivation",true);
  boolOptions.registerOption("ltl-show-node-detail",true);
  boolOptions.registerOption("ltl-collapsed-graph",false);
  boolOptions.registerOption("input-var-values-as-constraints",false);

  boolOptions.registerOption("arith-top",false);
  boolOptions.registerOption("abstract-interpreter",false);
  boolOptions.registerOption("rers-binary",false);
  boolOptions.registerOption("relop-constraints",false); // not accessible on command line yet

  boolOptions.processOptions();

  if(boolOptions["print-all-options"]) {
	cout<<boolOptions.toString(); // prints all bool options
  }
  
  if (args.count("internal-checks")) {
	if(CodeThorn::internalChecks(argc,argv)==false)
	  return 1;
	else
	  return 0;
  }

  Analyzer analyzer;
  
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
  if(args.count("csv-assert-live")) {
	analyzer._csv_assert_live_file=args["csv-assert-live"].as<string>();
  }

  if(args.count("input-var-values")) {
	string setstring=args["input-var-values"].as<string>();
	cout << "STATUS: input-var-values="<<setstring<<endl;
	stringstream ss(setstring);
	if(ss.peek()=='{')
	  ss.ignore();
	else
	  throw "Error: option input-var-values: wrong input format (at start).";
	int i;
	while(ss>>i) {
	  //cout << "DEBUG: input-var-string:i:"<<i<<" peek:"<<ss.peek()<<endl;	
	  analyzer.insertInputVarValue(i);
	  if(ss.peek()==','||ss.peek()==' ')
		ss.ignore();
	}
#if 0
	if(ss.peek()=='}')
	  ss.ignore();
	else
	  throw "Error: option input-var-values: wrong input format (at end).";
#endif
  }

  int numberOfThreadsToUse=1;
  if(args.count("threads")) {
	numberOfThreadsToUse=args["threads"].as<int>();
  }
  analyzer.setNumberOfThreadsToUse(numberOfThreadsToUse);

  if(args.count("semantic-fold-threshold")) {
	int semanticFoldThreshold=args["semantic-fold-threshold"].as<int>();
	analyzer.setSemanticFoldThreshold(semanticFoldThreshold);
  }
  if(args.count("display-diff")) {
	int displayDiff=args["display-diff"].as<int>();
	analyzer.setDisplayDiff(displayDiff);
  }
  if(args.count("ltl-verifier")) {
	int ltlVerifier=args["ltl-verifier"].as<int>();
	analyzer.setLTLVerifier(ltlVerifier);
  }
  if(args.count("debug-mode")) {
	option_debug_mode=args["debug-mode"].as<int>();
  }

  // clean up string-options in argv
  for (int i=1; i<argc; ++i) {
	if (string(argv[i]) == "--csv-assert" 
		|| string(argv[i])=="--csv-stats" 
		|| string(argv[i])=="--csv-assert-live"
		|| string(argv[i])=="--threads" 
		|| string(argv[i])=="--display-diff"
		|| string(argv[i])=="--input-var-values"
		|| string(argv[i])=="--ltl-verifier"
		) {
	  // do not confuse ROSE frontend
	  argv[i] = strdup("");
	  assert(i+1<argc);
		argv[i+1] = strdup("");
	}
  }

  // Build the AST used by ROSE
  cout << "INIT: Parsing and creating AST."<<endl;
  SgProject* sageProject = frontend(argc,argv);
  double frontEndRunTime=timer.getElapsedTimeInMilliSec();
  
  if(boolOptions["run-rose-tests"]) {
	cout << "INIT: Running ROSE AST tests."<<endl;
	// Run internal consistency tests on AST
	AstTests::runAllTests(sageProject);
  }

  SgNode* root=sageProject;
  checkProgram(root);
  timer.start();

  cout << "INIT: Running variable<->symbol mapping check."<<endl;
  //VariableIdMapping varIdMap;
  analyzer.getVariableIdMapping()->computeVariableSymbolMapping(sageProject);
  cout << "STATUS: Variable<->Symbol mapping created."<<endl;
  if(!analyzer.getVariableIdMapping()->isUniqueVariableSymbolMapping()) {
	cerr << "WARNING: Variable<->Symbol mapping not bijective."<<endl;
	//varIdMap.reportUniqueVariableSymbolMappingViolations();
  }
#if 0
  analyzer.getVariableIdMapping()->toStream(cout);
#endif

  cout << "INIT: creating solver."<<endl;
  analyzer.initializeSolver1("main",root);
  analyzer.initLabeledAssertNodes(sageProject);
  double initRunTime=timer.getElapsedTimeInMilliSec();

  timer.start();
  cout << "=============================================================="<<endl;
  if(boolOptions["semantic-fold"]) {
	analyzer.runSolver2();
  } else {
	analyzer.runSolver1();
  }

  if(boolOptions["post-semantic-fold"]) {
	cout << "Performing post semantic folding (this may take some time):"<<endl;
	analyzer.semanticFoldingOfTransitionGraph();
  }
  double analysisRunTime=timer.getElapsedTimeInMilliSec();

  // since CT1.2 the ADT TransitionGraph ensures that no duplicates can exist
#if 0
  long removed=analyzer.getTransitionGraph()->removeDuplicates();
  cout << "Transitions reduced: "<<removed<<endl;
#endif

  cout << "=============================================================="<<endl;
  // TODO: reachability in presence of semantic folding
  if(!boolOptions["semantic-fold"] && !boolOptions["post-semantic-fold"]) {
	printAsserts(analyzer,sageProject);
  }
  if (args.count("csv-assert")) {
	string filename=args["csv-assert"].as<string>().c_str();
	generateAssertsCsvFile(analyzer,sageProject,filename);
	cout << "=============================================================="<<endl;
  }
  if(boolOptions["tg-ltl-reduced"]) {
	cout << "(Experimental) Reducing transition graph ..."<<endl;
	set<const EState*> xestates=analyzer.nonLTLRelevantEStates();
	cout << "Size of transition graph before reduction: "<<analyzer.getTransitionGraph()->size()<<endl;
	cout << "Number of EStates to be reduced: "<<xestates.size()<<endl;
	analyzer.getTransitionGraph()->reduceEStates(xestates);
	cout << "Size of transition graph after reduction : "<<analyzer.getTransitionGraph()->size()<<endl;
	cout << "=============================================================="<<endl;
  }
  timer.start();
  if (ltl_file.size()) {
	generateLTLOutput(analyzer,ltl_file);
	cout << "=============================================================="<<endl;
  }
  double ltlRunTime=timer.getElapsedTimeInMilliSec();
  // TODO: reachability in presence of semantic folding
  if(boolOptions["semantic-fold"] || boolOptions["post-semantic-fold"]) {
	  cout << "NOTE: no reachability results with semantic folding (not implemented yet)."<<endl;
	} else {
	  printAssertStatistics(analyzer,sageProject);
	}
  cout << "=============================================================="<<endl;

  double totalRunTime=frontEndRunTime+initRunTime+ analysisRunTime+ltlRunTime;

  long pstateSetSize=analyzer.getPStateSet()->size();
  long pstateSetBytes=analyzer.getPStateSet()->memorySize();
  long pstateSetMaxCollisions=analyzer.getPStateSet()->maxCollisions();
  long pstateSetLoadFactor=analyzer.getPStateSet()->loadFactor();
  long eStateSetSize=analyzer.getEStateSet()->size();
  long eStateSetBytes=analyzer.getEStateSet()->memorySize();
  long eStateSetMaxCollisions=analyzer.getEStateSet()->maxCollisions();
  double eStateSetLoadFactor=analyzer.getEStateSet()->loadFactor();
  long transitionGraphSize=analyzer.getTransitionGraph()->size();
  long transitionGraphBytes=transitionGraphSize*sizeof(Transition);
  long numOfconstraintSets=analyzer.getConstraintSetMaintainer()->numberOf();
  long constraintSetsBytes=analyzer.getConstraintSetMaintainer()->memorySize();
  long constraintSetsMaxCollisions=analyzer.getConstraintSetMaintainer()->maxCollisions();
  double constraintSetsLoadFactor=analyzer.getConstraintSetMaintainer()->loadFactor();

  cout << "Number of stdin-estates        : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR))<<color("white")<<endl;
  cout << "Number of stdout-estates       : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR))<<color("white")<<endl;
  cout << "Number of stderr-estates       : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR))<<color("white")<<endl;
  cout << "Number of failed-assert-estates: "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT))<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout << "Number of pstates              : "<<color("magenta")<<pstateSetSize<<color("white")<<" (memory: "<<color("magenta")<<pstateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<pstateSetLoadFactor<<  "/"<<pstateSetMaxCollisions<<")"<<endl;
  cout << "Number of estates              : "<<color("cyan")<<eStateSetSize<<color("white")<<" (memory: "<<color("cyan")<<eStateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<eStateSetLoadFactor<<  "/"<<eStateSetMaxCollisions<<")"<<endl;
  cout << "Number of transitions          : "<<color("blue")<<transitionGraphSize<<color("white")<<" (memory: "<<color("blue")<<transitionGraphBytes<<color("white")<<" bytes)"<<endl;
  cout << "Number of constraint sets      : "<<color("yellow")<<numOfconstraintSets<<color("white")<<" (memory: "<<color("yellow")<<constraintSetsBytes<<color("white")<<" bytes)"<<" ("<<""<<constraintSetsLoadFactor<<  "/"<<constraintSetsMaxCollisions<<")"<<endl;
  cout << "=============================================================="<<endl;
  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;
  cout << "Memory total         : "<<color("green")<<totalMemory<<" bytes"<<color("normal")<<endl;
  cout << "Time total           : "<<color("green")<<readableruntime(totalRunTime)<<color("normal")<<endl;
  cout << "=============================================================="<<endl;

  if(args.count("csv-stats")) {
	string filename=args["csv-stats"].as<string>().c_str();
	stringstream text;
	text<<"Sizes,"<<pstateSetSize<<", "
		<<eStateSetSize<<", "
		<<transitionGraphSize<<", "
		<<numOfconstraintSets<<endl;
	text<<"Memory,"<<pstateSetBytes<<", "
		<<eStateSetBytes<<", "
		<<transitionGraphBytes<<", "
		<<constraintSetsBytes<<", "
		<<totalMemory<<endl;
	text<<"Runtime(readable),"
		<<readableruntime(frontEndRunTime)<<", "
		<<readableruntime(initRunTime)<<", "
		<<readableruntime(analysisRunTime)<<", "
		<<readableruntime(ltlRunTime)<<", "
		<<readableruntime(totalRunTime)<<endl;
	text<<"Runtime(ms),"
		<<frontEndRunTime<<", "
		<<initRunTime<<", "
		<<analysisRunTime<<", "
		<<ltlRunTime<<", "
		<<totalRunTime<<endl;
	text<<"hashset-collisions,"
		<<pstateSetMaxCollisions<<", "
		<<eStateSetMaxCollisions<<", "
		<<constraintSetsMaxCollisions<<endl;
	text<<"hashset-loadfactors,"
		<<pstateSetLoadFactor<<", "
		<<eStateSetLoadFactor<<", "
		<<constraintSetsLoadFactor<<endl;
	text<<"threads,"<<numberOfThreadsToUse<<endl;
	write_file(filename,text.str());
    cout << "generated "<<filename<<endl;
  }
  

  if(boolOptions["viz"]) {
    Visualizer visualizer(analyzer.getLabeler(),analyzer.getVariableIdMapping(),analyzer.getFlow(),analyzer.getPStateSet(),analyzer.getEStateSet(),analyzer.getTransitionGraph());
	cout << "generating graphviz files:"<<endl;
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
	cout << "generated node info."<<endl;
    dotFile=functionAstTermsWithNullValuesToDot(sageProject);
    write_file("ast.dot", dotFile);
    cout << "generated ast.dot."<<endl;
    
    write_file("cfg.dot", analyzer.flow.toDot(analyzer.cfanalyzer->getLabeler()));
    cout << "generated cfg.dot."<<endl;
  }

#if 0
  {
	cout << "EStateSet:\n"<<analyzer.getEStateSet()->toString()<<endl;
  }
#endif

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
	InputOutput myio=es1->io;
	assert(myio.op==InputOutput::STDOUT_VAR 
		   && 
		   es1->pstate->varIsConst(es1->io.var)
		   );
  }
#endif


  // reset terminal
  cout<<color("normal")<<"done."<<endl;
  
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


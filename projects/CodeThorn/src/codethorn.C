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

namespace po = boost::program_options;

void write_file(std::string filename, std::string data) {
  std::ofstream myfile;
  myfile.open(filename.c_str(),std::ios::out);
  myfile << data;
  myfile.close();
}

string int_to_string(int x) {
  stringstream ss;
  ss << x;
  return ss.str();
}

string color(string name) {
  if(!boolOptions["colors"]) 
	return "";
  string c="\33[";
  if(name=="normal") return c+"0m";
  if(name=="bold") return c+"1m";
  if(name=="bold-off") return c+"22m";
  if(name=="blink") return c+"5m";
  if(name=="blink-off") return c+"25m";
  if(name=="underline") return c+"4m";
  if(name=="default-text-color") return c+"39m";
  if(name=="default-bg-color") return c+"49m";
  bool bgcolor=false;
  string prefix="bg-";
  size_t pos=name.find(prefix);
  if(pos==0) {
	bgcolor=true;
	name=name.substr(prefix.size(),name.size()-prefix.size());
  }
  string colors[]={"black","red","green","yellow","blue","magenta","cyan","white"};
  int i;
  for(i=0;i<8;i++) {
	if(name==colors[i]) {
	  break;
	}
  }
  if(i<8) {
	if(bgcolor)
	  return c+"4"+int_to_string(i)+"m";
	else
	  return c+"3"+int_to_string(i)+"m";
  }
  else
	throw "Error: unknown color code.";
}

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

  lr.setAstNodeVariantSet(vs,true);
  cout << "INIT: Running CodeThorn language restrictor."<<endl;
  bool valid=lr.checkIfAstIsAllowed(root);
  if(!valid) {
	cout << "INIT FAILED: Input program not valid. No analysis performed."<<endl;
	exit(1);
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
    ("help", "produce this help message")
    ("rose-help", "show help for compiler frontend options")
    ("version", "display the version")
    ("internal-checks", "run internal consistency checks (without input program)")
    ("verify", po::value< string >(), "verify all LTL formulae in the file [arg]")
    ("tg1-estate-address", po::value< string >(), "transition graph 1: visualize address [=yes|no]")
    ("tg1-estate-id", po::value< string >(), "transition graph 1: visualize estate-id [=yes|no]")
	("tg1-estate-properties", po::value< string >(), "transition graph 1: visualize all estate-properties [=yes|no]")
    ("tg2-estate-address", po::value< string >(), "transition graph 2: visualize address [=yes|no]")
    ("tg2-estate-id", po::value< string >(), "transition graph 2: visualize estate-id [=yes|no]")
    ("tg2-estate-properties", po::value< string >(),"transition graph 2: visualize all estate-properties [=yes|no]")
	("colors",po::value< string >(),"use colors in output [=yes|no]")
	("report-stdout",po::value< string >(),"report stdout estates during analysis [=yes|no]")
	("report-failed-assert",po::value< string >(),"report failed assert estates during analysis [=yes|no]")
	("precision-equality-constraints",po::value< string >(),"(experimental) use constraints for determining estate equality [=yes|no]")
	("precision-equality-io",po::value< string >(),"(experimental) use constraints for determining estate equality [=yes|no]")
	("precision-bool",po::value< string >(),"use precise top with bool-(and/or) operators (used in LTL)")
	("precision-intbool",po::value< string >(),"use precise top with intbool-(and/or) operators (used in int-analyzer)")
	("precision-exact-constraints",po::value< string >(),"use precise constraint extraction (experimental)")
	;

  po::store(po::command_line_parser(argc, argv).
	    options(desc).allow_unregistered().run(), args);
  po::notify(args);

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
  boolOptions.processOptions();
  cout<<boolOptions.toString();

  if (args.count("internal-checks")) {
	if(internalChecks(argc,argv)==false)
	  return 1;
	else
	  return 0;
  }
  
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

  if (args.count("verify")) {
    ltl_file = args["verify"].as<string>();
	for (int i=1; i<argc; ++i) {
	  if (string(argv[i]) == "--verify") {
		// do not confuse ROSE frontend
		argv[i] = strdup("");
		assert(i+1<argc);
		argv[i+1] = strdup("");
	  }
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
  //  cout << analyzer.stateSetToString(final);
  cout << "=============================================================="<<endl;
  timer.stop();
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
  // we only generate a visualization if #estates<=1000
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
      cout<<endl<<"Verifying formula "<<color("white")<<string(*ltl_val)<<color("normal")<<"."<<endl;
      try {
	AType::BoolLattice result = checker.verify(*ltl_val);
	if (result.isTrue()) {
	  ++n_yes;
	  cout<<color("green")<<"YES"<<color("normal")<<endl;
	} else if (result.isFalse()) {
	  ++n_no;
	  cout<<color("cyan")<<"NO"<<color("normal")<<endl;
	} else {
	  ++n_undecided;
	  cout<<color("magenta")<<"UNDECIDED"<<color("normal")<<endl;
	}
      } catch(const char* str) {
	++n_failed;
	cerr << "Exception raised: " << str << endl;
	cout<<color("red")<<"FAILED"<<color("normal")<<endl;
      } catch(string str) {
	++n_failed;
	cerr << "Exception raised: " << str << endl;
	cout<<color("red")<<"FAILED"<<color("normal")<<endl;
      } catch(...) {
	++n_failed;
	cout<<color("red")<<"FAILED"<<color("normal")<<endl;
      }  
    }
    fclose(ltl_input);
    assert(n_yes+n_no+n_undecided+n_failed == n);
    cout<<"\nStatistics "<<endl
        <<"========== "<<endl
	<<n_yes      <<"/"<<n<<color("green")  <<" YES, "       <<color("normal") 	
	<<n_no       <<"/"<<n<<color("cyan")   <<" NO, "	       <<color("normal") 	
	<<n_undecided<<"/"<<n<<color("magenta")<<" UNDECIDED, " <<color("normal") 	
	<<n_failed   <<"/"<<n<<color("red")    <<" FAILED"      <<color("normal") 	
	<<endl;

  } 

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


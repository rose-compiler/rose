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
    ;

  po::store(po::command_line_parser(argc, argv).
	    options(desc).allow_unregistered().run(), args);
  po::notify(args);

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


  boolOptions.init(argc,argv);
  boolOptions.registerOption("tg1-estate-address",false);
  boolOptions.registerOption("tg1-estate-id",false);
  boolOptions.registerOption("tg1-estate-properties",true);
  boolOptions.registerOption("tg2-estate-address",false);
  boolOptions.registerOption("tg2-estate-id",true);
  boolOptions.registerOption("tg2-estate-properties",false);
  boolOptions.processOptions();
  cout<<boolOptions.toString();

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
  long stateSetBytes=stateSetSize*sizeof(State);
  long eStateSetSize=analyzer.getEStateSet()->size();
  long eStateSetBytes=eStateSetSize*sizeof(EState);
  long transitionGraphSize=analyzer.getTransitionGraph()->size();
  long transitionGraphBytes=transitionGraphSize*sizeof(Transition);

  const string csi = "\33[";
  const string white = csi+"37m";
  const string green = csi+"32m";
  const string red = csi+"31m";
  const string magenta = csi+"35m";
  const string cyan = csi+"36m";
  const string blue = csi+"34m";
  const string bold_on = csi+"1m";
  const string bold_off = csi+"22m";
  const string normal = csi+"0m";

  cout << "Number of stdin-estates : "<<cyan<<(analyzer.getEStateSet()->numberOfIoTypeStates(InputOutput::STDIN_VAR))<<white<<endl;
  cout << "Number of stdout-estates: "<<cyan<<(analyzer.getEStateSet()->numberOfIoTypeStates(InputOutput::STDOUT_VAR))<<white<<endl;
  cout << "Number of stderr-estates: "<<cyan<<(analyzer.getEStateSet()->numberOfIoTypeStates(InputOutput::STDERR_VAR))<<white<<endl;
  cout << "Number of states     : "<<magenta<<stateSetSize<<white<<" (memory: "<<magenta<<stateSetBytes<<white<<" bytes)"<<endl;
  cout << "Number of estates    : "<<cyan<<eStateSetSize<<white<<" (memory: "<<cyan<<eStateSetBytes<<white<<" bytes)"<<endl;
  cout << "Number of transitions: "<<blue<<transitionGraphSize<<white<<" (memory: "<<blue<<transitionGraphBytes<<white<<" bytes)"<<endl;
  cout << "Memory total         : "<<green<<stateSetBytes+eStateSetBytes+transitionGraphBytes<<" bytes"<<normal<<endl;
  if(totalRunTime<1000.0) 
	cout << "Time total           : "<<green<<totalRunTime<<" ms"<<normal<<endl;
  else
	cout << "Time total           : "<<green<<totalRunTime/1000.0<<" seconds"<<normal<<endl;
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
  if (ltl_file.size()) {
    LTL::Checker checker(*analyzer.getEStateSet(),
			 *analyzer.getTransitionGraph());
    ltl_input = fopen(ltl_file.c_str(), "r");
    while ( !ltl_eof) {
      try { 
        if (ltl_parse() != 0) {
          cerr<<red<< "Syntax error" << endl;
	  continue;
        }
        if (ltl_val == NULL) {
	  // empty line
	  continue;
	}
      } catch(const char* s) {
        if (ltl_val) cout<<normal<<string(*ltl_val)<<endl;
        cout<< s<<endl<<red<< "Grammar Error" << endl;
	continue;
      } catch(...) {
	cout<<red<< "Parser exception" << endl;
	continue;
      }  
	  
      cout<<endl<<"Verifying formula "<<white<<string(*ltl_val)<<normal<<"."<<endl;
      try {
	if (checker.verify(*ltl_val))
	  cout<<green<<"YES"<<normal<<endl;
	else
	  cout<<cyan<<"NO"<<normal<<endl;
      } catch(const char* str) {
	cerr << "Exception raised: " << str << endl;
	cout<<red<<"FAILED"<<normal<<endl;
      } catch(string str) {
	cerr << "Exception raised: " << str << endl;
	cout<<red<<"FAILED"<<normal<<endl;
      } catch(...) {
	cout<<red<<"FAILED"<<normal<<endl;
      }  
    }
    fclose(ltl_input);
  } 

  // reset terminal
  cout<<normal<<"done."<<endl;
  
  } catch(char* str) {
	cerr << "*Exception raised: " << str << endl;
  } catch(const char* str) {
	cerr << "Exception raised: " << str << endl;
  } catch(string str) {
	cerr << "Exception raised: " << str << endl;
 }
  return 0;
}


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

void checkTypes() {
  VariableIdMapping variableIdMapping;
  State s1;
  cout << "RUNNING CHECKS: START"<<endl;
  VariableId var_x=variableIdMapping.createUniqueTemporaryVariableId("x");
  if(!s1.varExists(var_x))
	cout << "PASS: x does not exist."<<endl;
  else
	cout << "FAIL: x does not exist."<<endl;
  s1[var_x]=5;
  if(s1.varExists(var_x))
	cout << "PASS: x does exist."<<endl;
  else
	cout << "FAIL: x does exist."<<endl;
  cout<<"x value: " <<s1.varValueToString(var_x)<<endl;
  cout<<"State: "<<s1.toString()<<endl;
  VariableId var_y=variableIdMapping.createUniqueTemporaryVariableId("y");
  s1[var_y]=7;
  cout<<"State: "<<s1.toString()<<endl;
  s1[var_x]=6;
  cout<<"State: "<<s1.toString()<<endl;
  s1[var_x]=ANALYZER_INT_TOP;
  cout<<"State: "<<s1.toString()<<endl;
  s1.erase(var_x);
  cout<<"State: "<<s1.toString()<<endl;
  cout<<"- - - - - - - - - - - - - - - - - - -"<<endl;
  StateSet ss1;
  cout<<"StateSet-1: "<<ss1.toString()<<endl;
  ss1.insert(s1);
  cout<<"StateSet-2: "<<ss1.toString()<<endl;
  ss1.insert(s1);
  cout<<"StateSet-3: "<<ss1.toString()<<endl;
  State s2=s1;
  ss1.insert(s2);
  cout<<"StateSet-4: "<<ss1.toString()<<endl;
  s2[var_y]=10;
  cout<<"StateSet-5: "<<ss1.toString()<<endl;
  ss1.insert(s2);
  cout<<"StateSet-6: "<<ss1.toString()<<endl;
  if(ss1.size()==2) cout << "TEST: PASS"<<endl;
  else cout << "TEST: FAIL"<<endl;

  cout << endl;
  {
	cout << "RUNNING CHECKS FOR BOOLLATTICE TYPE: START"<<endl;
	AType::BoolLattice a;
	a=true;
	cout << "a: "<<a.toString()<<endl;
	AType::BoolLattice b;
	b=false;
	cout << "b: "<<b.toString()<<endl;
	AType::BoolLattice c=a||b;
	cout << "c=a||b: "<<c.toString()<<endl;
	AType::Top e;
	AType::BoolLattice d;
	d=e;
	cout << "d: "<<d<<endl;
	c=c||d;
	cout << "c=c||d: "<<c<<endl;
	AType::BoolLattice f=AType::Bot();
	d=AType::Bot();
	cout << "d: "<<d<<endl;
	cout << "f: "<<d<<endl;
	a=d&&f;
	cout << "a=d&&f: "<<a<<endl;
	f=false;
	cout << "f: "<<f<<endl;
	a=d&&f;
	cout << "a=d&&f: "<<a<<endl;
	cout << "RUNNING CHECKS FOR BOOL LATTICE TYPE: END"<<endl;
  }
  cout << "RUNNING CHECKS: END"<<endl;

  cout << endl;
  {
	cout << "RUNNING CHECKS FOR CONSTINT LATTICE TYPE: START"<<endl;
	AType::ConstIntLattice a;
	a=true;
	cout << "a: "<<a.toString();
	if(a.isTrue()) cout << "(true)";
	if(a.isFalse()) cout << "(false)";
	cout<<endl;
	AType::ConstIntLattice b;
	b=false;
	cout << "b: "<<b.toString();
	if(b.isTrue()) cout << "(true)";
	if(b.isFalse()) cout << "(false)";
	cout<<endl;

	AType::ConstIntLattice c=a||b;
	cout << "c=a||b: "<<c.toString()<<endl;
	AType::Top e;
	AType::ConstIntLattice d;
	d=e;
	cout << "d: "<<d<<endl;
	c=c||d;
	cout << "c=c||d: "<<c<<endl;
	AType::ConstIntLattice f=AType::Bot();
	d=AType::Bot();
	cout << "d: "<<d<<endl;
	cout << "f: "<<d<<endl;
	a=d&&f;
	cout << "a=d&&f: "<<a<<endl;
	f=false;
	cout << "f: "<<f<<endl;
	a=d&&f;
	cout << "a=d&&f: "<<a<<endl;
	a=5;
	cout << "a=5: "<<a;
	if(a.isTrue()) cout << "(true)";
	if(a.isFalse()) cout << "(false)";
	cout<<endl;
	a=0;
	cout << "a=0: "<<a;
	if(a.isTrue()) cout << "(true)";
	if(a.isFalse()) cout << "(false)";
	cout<<endl;
	//	a=a+1;
	//cout << "a=a+1: "<<a<<endl;
	cout << "RUNNING CHECKS FOR CONST INT LATTICE TYPE: END"<<endl;
  }
  cout << endl;
  {
	cout << "RUNNING CHECKS FOR CONSTRAINT TYPE: START"<<endl;
	VariableId var_x=variableIdMapping.createUniqueTemporaryVariableId("x");
	VariableId var_y=variableIdMapping.createUniqueTemporaryVariableId("y");
	VariableId var_z=variableIdMapping.createUniqueTemporaryVariableId("z");

	Constraint c1(Constraint::EQ_VAR_CONST,var_x,1);
	Constraint c2(Constraint::NEQ_VAR_CONST,var_y,2);
	Constraint c3(Constraint::DEQ_VAR_CONST,var_z,1);
	cout<<"c1:"<<c1.toString()<<endl;
	cout<<"c2:"<<c2.toString()<<endl;
	cout<<"c3:"<<c3.toString()<<endl;
	ConstraintSet cs;
	cs.insert(c1);
	cs.insert(c2);
	cs.insert(c3);
	cout<<"ConstraintSet cs:"<<cs.toString()<<endl;
	if(cs.constraintExists(Constraint::EQ_VAR_CONST,var_x,1))
	  cout << "constraintExists(EQ_Var_Const,x,1): true (TEST:PASS)"<<endl;
	else
	  cout << "constraintExists(EQ_Var_Const,x,1): false (TEST:FAIL)"<<endl;

	if(cs.constraintExists(c2))
	  cout << "constraintExists(c2): true (TEST:PASS)"<<endl;
	else
	  cout << "constraintExists(c2): false (TEST:FAIL)"<<endl;
	cout << "RUNNING CHECKS FOR CONSTRAINT TYPE: END"<<endl;
  }
  {
	cout << "RUNNING CHECKS FOR COMBINED TYPES: START"<<endl;
	EState es1;
	EState es2;
	cout << "EState created. "<<endl;
	cout << "empty EState: "<<es1.toString()<<endl;
	es1.label=1;
	VariableId var_x=variableIdMapping.createUniqueTemporaryVariableId("x");
	es1.constraints.insert(Constraint(Constraint::EQ_VAR_CONST,var_x,1));
	es2.label=1;
	es2.constraints.insert(Constraint(Constraint::NEQ_VAR_CONST,var_x,1));
	cout << "empty EState with label and constraint es1: "<<es1.toString()<<endl;
	cout << "empty EState with label and constraint es2: "<<es2.toString()<<endl;
	State s;
	es1.state=&s;
	es2.state=&s;
	cout << "empty EState with label, empty state, and constraint es1: "<<es1.toString()<<endl;
	cout << "empty EState with label, empty state, and constraint es2: "<<es2.toString()<<endl;
	bool testres=(es1==es2);
	if(testres)
	  cout << "es1==es2: "<<testres<< "(not as expected: FAIL)"<<endl;
	else
	  cout << "es1==es2: "<<testres<< "(as expected: PASS)"<<endl;
	cout << "RUNNING CHECKS FOR COMBINED TYPES: END"<<endl;
  }
  cout << "RUNNING CHECKS: END"<<endl;
}

void checkLanguageRestrictor(int argc, char *argv[]) {
  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);
  LanguageRestrictor lr;
  LanguageRestrictor::VariantSet vs= lr.computeVariantSetOfProvidedAst(sageProject);
  for(LanguageRestrictor::VariantSet::iterator i=vs.begin();i!=vs.end();++i) {
	cout << "VARIANT: "<<lr.variantToString(*i)<<endl;
  }
  cout <<endl;
  lr.allowAstNodesRequiredForEmptyProgram();
  vs=lr.getAllowedAstNodeVariantSet();
  for(LanguageRestrictor::VariantSet::iterator i=vs.begin();i!=vs.end();++i) {
	cout << "VARIANT: "<<lr.variantToString(*i)<<endl;
  }
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
#if 0
  try {
  checkTypes();
  //checkLanguageRestrictor(argc,argv);
  } catch(char* str) {
	cerr << "*Exception raised: " << str << endl;
  } catch(const char* str) {
	cerr << "Exception raised: " << str << endl;
  } catch(string str) {
	cerr << "Exception raised: " << str << endl;
 }
  return 0;
#else
  try {
	Timer timer;
	timer.start();

  // Command line option handling.
  po::options_description desc
    ("CodeThorn 1.0\n"
     "Supported options");

  desc.add_options()
    ("help", "produce this help message")
    ("rose-help", "show help for compiler frontend options")
    ("version", "display the version")
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

  if (args.count("help")) {
    cout << desc << "\n";
    return 0;
  }

  if (args.count("rose-help")) {
    argv[1] = strdup("--help");
  }

  if (args.count("version")) {
    cout << "CodeThorn version 1.0\n";
    return 0;
  }

  if (args.count("verify")) {
    ltl_file = args["verify"].as<string>();
    for (int i=1; i<argc; ++i)
      if (string(argv[i]) == "--verify") {
	// do not confuse ROSE frontend
	argv[i] = strdup("");
	assert(i+1<argc);
	argv[i+1] = strdup("");
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

  cout << "NOTE: We are ignoring operator '?' (not implemented yet)"<<endl;
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
        cout<< s<<endl<<red<< "FAILED" << endl;
	continue;
      } catch(...) {
	cout<<red<< "Parser exception" << endl;
	continue;
      }  
	  
      cout<<normal<<"Verifying formula "<<magenta<< string(*ltl_val) <<normal<<"."<<endl;
      try {
	if (checker.verify(*ltl_val))
	  cout<<green<<"YES"<<normal<<endl;
	else
	  cout<<green<<"NO"<<normal<<endl;
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
#endif
}


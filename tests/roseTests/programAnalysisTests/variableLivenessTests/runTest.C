/******************************************
 * Category: DFA
 * Application to test the Def-Use Analysis
 * created by tps in Feb 2007
 *****************************************/

#include "rose.h"
#include "DefUseAnalysis.h"
#include "LivenessAnalysis.h"
#include <string>
#include <iostream>
using namespace std;

void testOneFunctionDEFUSE( std::string funcParamName,
			    vector<string> argvList,
			    bool debug, int nrOfNodes,
			    multimap <string, int> results,
			    multimap <string, int> useresults) {
  cout << " \n\n------------------------------------------\nrunning (defuse)... " << argvList[1] << endl;
  // Build the AST used by ROSE
  SgProject* project = frontend(argvList);
  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  int val = defuse->run(debug);
  std::cout << "Analysis run is : " << (val ?  "failure" : "success" ) << " " << val << std::endl;
  ROSE_ASSERT(val!=1);
  if (val==1) exit(1);

  if (debug==false)
    defuse->dfaToDOT();

  //std::list<SgNode*> vars = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
  //std::list<SgNode*>::const_iterator i = vars.begin();
  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
  NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
  for (; i!=vars.end();++i) {
    SgFunctionDefinition* func = isSgFunctionDefinition(*i);
    std::string name = func->class_name();
    string funcName = func->get_declaration()->get_qualified_name().str();
    int maxNodes = defuse->getDefSize();
    int nodeNr = defuse->getIntForSgNode(func);
    if (nodeNr == -1)
      continue;
    //cout << " checking function : " << funcName << endl;
    if (funcName!=funcParamName)
      continue;

    cout << "\n------------------------\nchecking for " << name << " -- " << funcName << " -- " << nodeNr << endl;
    if (maxNodes!=nrOfNodes) {
      cout << " Error: Test should have " << nrOfNodes << " nodes. found: " << maxNodes << endl;
      abort();
    }
    cout << " Test has nodes:  " << nrOfNodes <<  endl;

    cout <<"\nChecking all definitions ... " << endl;
    // check nodes in multimap
    std::vector <std::pair < SgInitializedName*, SgNode*> > map = defuse->getDefMultiMapFor(func);
    if (map.size()>0) {
      std::vector < std::pair <SgInitializedName*, SgNode*> >::const_iterator j = map.begin();
      unsigned int hit=0;
      for (;j!=map.end();++j) {
	SgInitializedName* in_node = j->first;
	SgNode* node = j->second;
	string name= in_node->get_qualified_name().str();
	cout << " ... checking :  " << name << endl;
	multimap <string, int>::const_iterator k =results.begin();
	for (;k!=results.end();++k) {
	  string resName = k->first;
	  int resNr = k->second;
	  int tableNr = defuse->getIntForSgNode(node);
	  if (name==resName)
	    cout << " ... defNr: " << resNr << "  inTable: " << tableNr <<  endl;
	  if (name==resName && tableNr==resNr) {
	    hit++;
	    cout << " Hit " << hit << "/" << results.size() << " - (" << name << "," << resNr << ")" << endl;
	  }
	}

      }
      if (hit!=results.size()) {
	cout << " Error: No hit! ... DFA values of node " << nrOfNodes << " are not correct! " << endl;
	exit(1);
      }
    } else {
      if (results.size()!=0) {
	cout << " Error: Test node " << defuse->getIntForSgNode(func) << " should have a multimap. " << endl;
	exit(1);
      }
    }

    cout <<"\nChecking all uses ... " << endl;
    // check nodes in multimap
    map = defuse->getUseMultiMapFor(func);
    if (map.size()>0) {
      std::vector <std::pair <SgInitializedName*, SgNode*> >::const_iterator j = map.begin();
      size_t hit=0;
      for (;j!=map.end();++j) {
	SgInitializedName* in_node = j->first;
	SgNode* node = j->second;
	string name= in_node->get_qualified_name().str();
	cout << " ... checking :  " << name << endl;
	multimap <string, int>::const_iterator k =useresults.begin();
	for (;k!=useresults.end();++k) {
	  string resName = k->first;
	  int resNr = k->second;
	  int tableNr = defuse->getIntForSgNode(node);
	  if (name==resName)
	    cout << " ... defNr: " << resNr << "  inTable: " << tableNr <<  endl;
	  if (name==resName && tableNr==resNr) {
	    hit++;
	    cout << " Hit " << hit << "/" << useresults.size() << " - (" << name << "," << resNr << ")" << endl;
	  }
	}

      }
      if (hit!=useresults.size()) {
	cout << " Error: No hit! ... DFA values of node " << nrOfNodes << " are not correct! " << endl;
	exit(1);
      }
    } // if
  }
  std::cout << "Analysis test is success." << std::endl;
}


void testOneFunction( std::string funcParamName,
		      vector<string> argvList,
		      bool debug, int nrOfNodes,
		      multimap <int, vector<string> >  resultsIn,
		      multimap <int, vector<string> > resultsOut) {
  cout << " \n\n------------------------------------------\nrunning (variable)... " << argvList[1] << endl;

  // Build the AST used by ROSE
  SgProject* project = frontend(argvList);
  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  int val = defuse->run(debug);
  if (debug)
    std::cerr << ">Analysis run is : " << (val ?  "failure" : "success" ) << " " << val << std::endl;
  if (val==1) exit(1);

  if (debug==false)
    defuse->dfaToDOT();


  LivenessAnalysis* liv = new LivenessAnalysis(debug,(DefUseAnalysis*)defuse);

  std::vector <FilteredCFGNode < IsDFAFilter > > dfaFunctions;
  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
  NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
  bool abortme=false;
  int hitIn=0;
  int hitOut=0;
  for (; i!=vars.end();++i) {
    SgFunctionDefinition* func = isSgFunctionDefinition(*i);
    std::string name = func->class_name();
    string funcName = func->get_declaration()->get_qualified_name().str();
    if (debug)
      cerr << " .. running live analysis for func : " << funcName << endl;
    FilteredCFGNode <IsDFAFilter> rem_source = liv->run(func,abortme);
    if (abortme)
      break;
    if (funcName!=funcParamName) {
      if (debug)
        cerr << "    .. skipping live analysis check for func : " << funcName << endl;
      continue;
    }
    if (rem_source.getNode()!=NULL)
      dfaFunctions.push_back(rem_source);

    NodeQuerySynthesizedAttributeType nodes = NodeQuery::querySubTree(func, V_SgNode);
    SgFunctionDeclaration* decl = isSgFunctionDeclaration(func->get_declaration());
    ROSE_ASSERT(decl);
    Rose_STL_Container<SgInitializedName*> args = decl->get_parameterList()->get_args();
    if (debug)
      cerr <<"Found args : " << args.size() << endl;
    Rose_STL_Container<SgInitializedName*>::const_iterator it = args.begin();
    for (;it!=args.end();++it) {
      nodes.push_back(*it);
    }
    if((int)nodes.size()-1!=nrOfNodes) {
      cerr << "Error :: Number of nodes = " << nodes.size()-1 << "  should be : " << nrOfNodes << endl;
      exit(1);
    } else {
      if (debug)
    	cerr << "Investigating nodes : " << nodes.size() << endl;
    }
    NodeQuerySynthesizedAttributeType::const_iterator nodesIt = nodes.begin();
    for (; nodesIt!=nodes.end();++nodesIt) {
      SgNode* node = *nodesIt;
      ROSE_ASSERT(node);
      int tableNr = defuse->getIntForSgNode(node);
      std::vector<SgInitializedName*> in = liv->getIn(node);
      std::vector<SgInitializedName*> out = liv->getOut(node);

      std::vector<string> inName;
      std::vector<string> outName;
      std::vector<SgInitializedName*>::const_iterator itv = in.begin();
      for (;itv!=in.end();++itv) {
	SgInitializedName* init = *itv;
	string name = init->get_name();
	inName.push_back(name);
      }
      itv = out.begin();
      for (;itv!=out.end();++itv) {
	SgInitializedName* init = *itv;
	string name = init->get_name();
	outName.push_back(name);
      }
      std::sort(inName.begin(), inName.end());
      std::sort(outName.begin(), outName.end());

      multimap <int, vector<string> >::const_iterator k =resultsIn.begin();
      for (;k!=resultsIn.end();++k) {
	int resNr = k->first;
	vector<string> results = k->second;
	if (debug)
      	  cerr << "   ... containing nodes : " << results.size() << " node: " << node->class_name() << "   tableNr : " << tableNr <<
	    "  resNr : " << resNr << endl;
	if (tableNr==resNr) {
	  std::sort(results.begin(), results.end());
	  if (results==inName) {
	    if (debug)
	      cerr <<"Contents in IN vector is correct! " << endl;
	  } else {
	    if (debug) {
	      cerr << " >>>>>>>>>> Problem with contents for IN ! " << endl;
	      cerr << " >>>>>>>>>> RESULT ... " << endl;
	    }
	    std::vector<string>::const_iterator itv = inName.begin();
	    for (;itv!=inName.end();++itv) {
	      string name = *itv;
	      if (debug)		cerr << name << " " ;
	    }
	    if (debug) {
	      cerr << endl;
	      cerr << " >>>>>>>>>> USER ... " << endl;
	    }
	    itv = results.begin();
	    for (;itv!=results.end();++itv) {
	      string name = *itv;
	      if (debug)	cerr << name << " " ;
	    }
	    if (debug) {
	      cerr << endl;
	    }
	    exit(1);
	  }
	  if (results.size()==in.size()) {
	    hitIn++;
	  }
	  if (debug)
	    cout << " nodeNr: " << tableNr << ".  ResultSize IN should be:" << results.size() << " -  resultSize is: " << in.size()
                 << "  foundMatches == : " << hitIn << "/" << resultsIn.size() << endl;
        }
      }
      k =resultsOut.begin();
      for (;k!=resultsOut.end();++k) {
	int resNr = k->first;
	vector<string> results = k->second;
	//    	  cerr << "   ... containing nodes : " << results.size() << "   tableNr : " << tableNr <<
	//			  "  resNr : " << resNr << endl;
	if (tableNr==resNr) {
	  std::sort(results.begin(), results.end());
	  if (results==outName) {
	    if (debug)
	      cerr <<"Contents in OUT vector is correct! " << endl;
	  } 	     else {
	    if (debug) {
	      cerr << " >>>>>>>>>> Problem with contents for OUT ! " << endl;
	      cerr << " >>>>>>>>>> RESULT ... " << endl;
	    }
	    std::vector<string>::const_iterator itv = outName.begin();
	    for (;itv!=outName.end();++itv) {
	      string name = *itv;
	      if (debug)		cerr << name << " " ;
	    }
	    if (debug) {
	      cerr << endl;
	      cerr << " >>>>>>>>>> USER ... " << endl;
	    }
	    itv = results.begin();
	    for (;itv!=results.end();++itv) {
	      string name = *itv;
	      if (debug)					
		cerr << name << " " ;
	    }
	    if (debug)
	      cerr << endl;
	    exit(1);
	  }

	  if (results.size()==out.size()) {
	    hitOut++;
	  }
	  if (debug)
	    cout << " nodeNr: " << tableNr << ".  ResultSize OUT should be:" << results.size() << " -  resultSize is: " << out.size()
                 << "  foundMatches == : " << hitOut << "/" << resultsOut.size() << endl;
        }
      }
      if (hitIn==0 && hitOut==0) {
	if (debug)
	  cout << " nodeNr: " << tableNr << " IN: " << hitIn << "/" << resultsIn.size() <<
	    "        Out:" << hitOut << "/" << resultsOut.size() << endl;
      }
    }
    if (hitIn!=(int)resultsIn.size() || hitOut!=(int)resultsOut.size()) {
      cout << " Error: No hit! ... DFA values of node " << nrOfNodes << " are not correct! " << endl;
      exit(1);
    }

  }
  if (debug)
    cerr << "Writing out to var.dot... " << endl;
  std::ofstream f2("var.dot");
  dfaToDot(f2, string("var"), dfaFunctions,
           (DefUseAnalysis*)defuse, liv);
  f2.close();

  if (abortme) {
    cerr<<"ABORTING ." << endl;
    ROSE_ASSERT(false);
    //    exit(1);
  }

  // iterate again and write second var.dot file
  i = vars.begin();
  abortme=false;
  std::vector <FilteredCFGNode < IsDFAFilter > > dfaFunctions2;
  for (; i!=vars.end();++i) {
    SgFunctionDefinition* func = isSgFunctionDefinition(*i);
    std::string name = func->class_name();
    string funcName = func->get_declaration()->get_qualified_name().str();
    if (debug)
      cerr << " .. running live analysis for func (fixupStatementsINOUT): " << funcName << endl;
    FilteredCFGNode <IsDFAFilter> rem_source = liv->run(func,abortme);
    liv->fixupStatementsINOUT(func);
    if (rem_source.getNode()!=NULL)
      dfaFunctions2.push_back(rem_source);

  }
  if (debug)
    cerr << "Writing out to varFix.dot... " << endl;
  std::ofstream f3("varFix.dot");
  dfaToDot(f3, string("varFix"), dfaFunctions2,
           (DefUseAnalysis*)defuse, liv);
  f3.close();

  if (debug)
    std::cout << "Analysis test is success." << std::endl;
}


void runCurrentFile(vector<string> &argvList, bool debug, bool debug_map) {
  // Build the AST used by ROSE

  if (debug)
    std::cout << ">>>> Starting ROSE frontend ... " << endl;
  SgProject* project = frontend(argvList);
  if (debug)
    std::cout << ">>>> generate PDF " << endl;
  generatePDF ( *project );
  if (debug)
    std::cout << ">>>> start def-use analysis ... " << endl;

  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  int val = defuse->run(debug);
  if (debug)
    std::cout << "Analysis is : " << (val ?  "failure" : "success" ) << " " << val <<std::endl;
  if (val==1) exit(1);
  if (debug==false)
    defuse->dfaToDOT();

  LivenessAnalysis* liv = new LivenessAnalysis(true,(DefUseAnalysis*)defuse);

  //example usage
  // testing
  std::vector <FilteredCFGNode < IsDFAFilter > > dfaFunctions;
  bool abortme=false;
  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
  NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
  for (; i!=vars.end();++i) {
    SgFunctionDefinition* func = isSgFunctionDefinition(*i);
    string funcName = func->get_declaration()->get_qualified_name().str();
    if (debug)
      cerr << " running live analysis for func : " << funcName << endl;
    FilteredCFGNode <IsDFAFilter> rem_source = liv->run(func,abortme);
    if (rem_source.getNode()!=NULL)
      dfaFunctions.push_back(rem_source);
    if (abortme)
      break;
  }


  std::ofstream f2("var.dot");
  dfaToDot(f2, string("var"), dfaFunctions,
           (DefUseAnalysis*)defuse, liv);
  f2.close();
  if (abortme) {
    cerr<<"ABORTING ." << endl;
    exit(1);
  }


  delete project;
  delete defuse;
}

void usage() {
  cout << " Usage: " << endl;
  cout << "   runTest all [startNr]   --- to test all testcases" << endl;
  cout << "   runTest [file]          --- to test one file" << endl;
  exit(1);
}

int strToInt(string str) {
  int b;
  std::stringstream ss(str);
  ss >> b;
  return b;
}

int main( int argc, char * argv[] )
{
  if (argc==1) {
    usage();
  }
  string pass = argv[1];
  if (pass=="--help") {
    usage();
  }
  bool allTests=false;
  if (pass=="all") {
    allTests=true;
  }

  vector<string> argvList(argv, argv + argc);


  if (allTests==false) {
    // change here if you want debug
    bool debug_map = false;
    bool debug = false;
    runCurrentFile(argvList, debug, debug_map);
  }

  if (allTests==true) {
    bool debug =false;
    string startNr = "";
    if (argc>2)
      startNr = argv[2];
    bool testAll = false;
    int startNrInt = 0;
    if (startNr=="")
      testAll=true;
    else  {
      startNrInt = strToInt(startNr);
      testAll=false;
    }
    if (debug)
      cout << "start: " << startNrInt << "  all: " << testAll << endl;

    argvList.resize(2);
    argvList[0]=argv[0];
    multimap <int,  vector<string> >results;
    multimap <int,  vector<string> > outputResults;

    char* srcdirVar = getenv("SRCDIR");
    ROSE_ASSERT (srcdirVar);
    std::string srcdir = srcdirVar;
    srcdir += "/";

    if (debug)
      cerr << " RUNNING VARIABLE ANALYSIS (DEFUSE) TESTS -- NR: " << startNrInt << endl;

    // -------------------------------------- use-def tests
    {
      multimap <string, int> useresults;
      multimap <string, int> resultsMe;

      if (startNrInt==1 || testAll) {
	// ------------------------------ TESTCASE 1 -----------------------------------------
	argvList[1]=srcdir+"tests/test1.C";
	resultsMe.clear();      useresults.clear();
	resultsMe.insert(pair<string,int>("a", 8));
	resultsMe.insert(pair<string,int>("a", 17));
	resultsMe.insert(pair<string,int>("c", 18));
	testOneFunctionDEFUSE("::main",argvList, debug, 21, resultsMe,useresults);
      }

      if (startNrInt==2 || testAll) {
	// ------------------------------ TESTCASE 2 -----------------------------------------
	argvList[1]=srcdir+"tests/test2.C";
	resultsMe.clear();      useresults.clear();
	resultsMe.insert(pair<string,int>("a", 8));
	resultsMe.insert(pair<string,int>("a", 17));
	resultsMe.insert(pair<string,int>("c", 18));
	testOneFunctionDEFUSE("::main",argvList, debug, 19, resultsMe,useresults);
      }

      if (startNrInt==3 || testAll) {
	// ------------------------------ TESTCASE 3 -----------------------------------------
	argvList[1]=srcdir+"tests/test3.C";
	resultsMe.clear();       useresults.clear();
	resultsMe.insert(pair<string,int>("d", 15));
	testOneFunctionDEFUSE("::main", argvList, debug, 17, resultsMe,useresults);
      }

      if (startNrInt==4 || testAll) {
	// ------------------------------ TESTCASE 4 -----------------------------------------
	argvList[1]=srcdir+"tests/test4.C";
	resultsMe.clear();       useresults.clear();
	resultsMe.insert(pair<string,int>("::globalvar", 12));
	testOneFunctionDEFUSE("::main", argvList, debug, 14, resultsMe, useresults);
      }

      if (startNrInt==5 || testAll) {
	// ------------------------------ TESTCASE 5 -----------------------------------------
	argvList[1]=srcdir+"tests/test5.C";
	resultsMe.clear();       useresults.clear();
	resultsMe.insert(pair<string,int>("x", 10));
	resultsMe.insert(pair<string,int>("y", 16));
	useresults.insert(pair<string,int>("x", 13));
	testOneFunctionDEFUSE("::main", argvList, debug, 18, resultsMe, useresults);
      }

      if (startNrInt==6 || testAll) {
	// ------------------------------ TESTCASE 6 -----------------------------------------
	argvList[1]=srcdir+"tests/test6.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("x", 14));
	resultsMe.insert(pair<string,int>("x", 25));
	resultsMe.insert(pair<string,int>("z", 26));
	resultsMe.insert(pair<string,int>("z", 10));
	useresults.insert(pair<string,int>("x", 16));
	useresults.insert(pair<string,int>("z", 22));
	testOneFunctionDEFUSE("::main", argvList, debug, 26, resultsMe,useresults);
      }

      if (startNrInt==7 || testAll) {
	// ------------------------------ TESTCASE 7 -----------------------------------------
	argvList[1]=srcdir+"tests/test7.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("i", 6));
	resultsMe.insert(pair<string,int>("i", 15));
	useresults.insert(pair<string,int>("i", 8));
	useresults.insert(pair<string,int>("i", 14));
	testOneFunctionDEFUSE("::main", argvList, debug, 16, resultsMe,useresults);
      }

      if (startNrInt==8 || testAll) {
	// ------------------------------ TESTCASE 8 -----------------------------------------
	argvList[1]=srcdir+"tests/test8.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("x", 24));
	resultsMe.insert(pair<string,int>("i", 12));
	resultsMe.insert(pair<string,int>("i", 32));
	useresults.insert(pair<string,int>("i", 15));
	useresults.insert(pair<string,int>("p", 26));
	useresults.insert(pair<string,int>("x", 28));
	testOneFunctionDEFUSE("::main", argvList, debug, 31, resultsMe,useresults);
      }

      if (startNrInt==9 || testAll) {
	// ------------------------------ TESTCASE 9 -----------------------------------------
	argvList[1]=srcdir+"tests/test9.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("array", 4));
	resultsMe.insert(pair<string,int>("i", 12));
	resultsMe.insert(pair<string,int>("i", 31));
	useresults.insert(pair<string,int>("i", 15));
	// useresults.insert(pair<string,int>("array", 20)); // tps: fixed this
	testOneFunctionDEFUSE("::main", argvList, debug, 30, resultsMe,useresults);
      }

      if (startNrInt==10 || testAll) {
	// ------------------------------ TESTCASE 10 -----------------------------------------
	argvList[1]=srcdir+"tests/test10.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("z", 13));
	resultsMe.insert(pair<string,int>("z", 22));
	resultsMe.insert(pair<string,int>("res", 23));
	testOneFunctionDEFUSE("::main", argvList, debug, 25, resultsMe,useresults);
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("x", 3));
	resultsMe.insert(pair<string,int>("y", 4));
	resultsMe.insert(pair<string,int>("z", 5));
	testOneFunctionDEFUSE("::f2", argvList, debug, 25, resultsMe,useresults);
      }

      if (startNrInt==11 || testAll) {
	// ------------------------------ TESTCASE 11 -----------------------------------------
	argvList[1]=srcdir+"tests/test11.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("z", 19));
	resultsMe.insert(pair<string,int>("z", 63));
	resultsMe.insert(pair<string,int>("z", 73));
	resultsMe.insert(pair<string,int>("p", 24));
	resultsMe.insert(pair<string,int>("i", 28));
	resultsMe.insert(pair<string,int>("i", 56));
	resultsMe.insert(pair<string,int>("j", 40)); //41
	resultsMe.insert(pair<string,int>("j", 77));
	//      resultsMe.insert(pair<string,int>("res", 42));
	testOneFunctionDEFUSE("::main", argvList, debug, 76, resultsMe,useresults);
	resultsMe.clear();  useresults.clear();
	testOneFunctionDEFUSE("::f1", argvList, debug, 76, resultsMe,useresults);
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("x", 7));
	resultsMe.insert(pair<string,int>("y", 8));
	resultsMe.insert(pair<string,int>("z", 9));
	testOneFunctionDEFUSE("::f2", argvList, debug, 76, resultsMe,useresults);
      }

      if (startNrInt==13 || testAll) {
	// ------------------------------ TESTCASE 13 -----------------------------------------
	argvList[1]=srcdir+"tests/test13.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("::global", 7));
	testOneFunctionDEFUSE("::main", argvList, debug, 9, resultsMe,useresults);
      }

      if (startNrInt==14 || testAll) {
	// ------------------------------ TESTCASE 14 -----------------------------------------
	argvList[1]=srcdir+"tests/test14.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("a", 12));
	resultsMe.insert(pair<string,int>("b", 6));
	testOneFunctionDEFUSE("::main", argvList, debug, 14, resultsMe,useresults);
      }

      if (startNrInt==15 || testAll) {
	// ------------------------------ TESTCASE 15 -----------------------------------------
	argvList[1]=srcdir+"tests/test15.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("i", 12));
	resultsMe.insert(pair<string,int>("i", 26));
	resultsMe.insert(pair<string,int>("index", 26));
	resultsMe.insert(pair<string,int>("index", 8));
	testOneFunctionDEFUSE("::main", argvList, debug, 26, resultsMe,useresults);
      }

      if (startNrInt==18 || testAll) {
	// ------------------------------ TESTCASE 18 -----------------------------------------
	argvList[1]=srcdir+"tests/test18.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("i", 12));
	resultsMe.insert(pair<string,int>("i", 49));
	resultsMe.insert(pair<string,int>("p", 8));
	resultsMe.insert(pair<string,int>("p", 46));
	resultsMe.insert(pair<string,int>("x", 24));
	resultsMe.insert(pair<string,int>("z", 39));
	resultsMe.insert(pair<string,int>("e", 41));
	useresults.insert(pair<string,int>("i", 15));
	useresults.insert(pair<string,int>("x", 28));
	testOneFunctionDEFUSE("::main", argvList, debug, 48, resultsMe,useresults);
      }

      if (startNrInt==19 || testAll) {
	// ------------------------------ TESTCASE 19 -----------------------------------------
	argvList[1]=srcdir+"tests/test19.C";
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("::global", 2));
	resultsMe.insert(pair<string,int>("::global", 8));
	resultsMe.insert(pair<string,int>("::global2", 28));
	resultsMe.insert(pair<string,int>("a", 24));
	testOneFunctionDEFUSE("::main", argvList, debug, 30, resultsMe,useresults);
	resultsMe.clear();  useresults.clear();
	resultsMe.insert(pair<string,int>("::global", 8));
	resultsMe.insert(pair<string,int>("::global2", 12));
	testOneFunctionDEFUSE("::setMe", argvList, debug, 30, resultsMe,useresults);
      }

      if (startNrInt==20 || testAll) {
	// ------------------------------ TESTCASE 1 -----------------------------------------
	argvList[1]=srcdir+"tests/test20.C";
	resultsMe.clear();      useresults.clear();
	testOneFunctionDEFUSE("::bar",argvList, debug, 5, resultsMe,useresults);
      }

      if (startNrInt==21 || testAll) {
	// ------------------------------ TESTCASE 1 -----------------------------------------
	argvList[1]=srcdir+"tests/test21.C";
	resultsMe.clear();      useresults.clear();
	testOneFunctionDEFUSE("::func",argvList, debug, 9, resultsMe,useresults);
      }

      if (startNrInt==22 || testAll) {
	// ------------------------------ TESTCASE 1 -----------------------------------------
	argvList[1]=srcdir+"tests/test22.C";
	resultsMe.clear();      useresults.clear();
	resultsMe.insert(pair<string,int>("b", 7));
	testOneFunctionDEFUSE("::func",argvList, debug, 12, resultsMe,useresults);
      }

      if (startNrInt==23 || testAll) {
	// ------------------------------ TESTCASE 1 -----------------------------------------
	argvList[1]=srcdir+"tests/test23.C";
	resultsMe.clear();      useresults.clear();
	resultsMe.insert(pair<string,int>("a",11));
	testOneFunctionDEFUSE("::func",argvList, debug, 30, resultsMe,useresults);
      }

      if (startNrInt==24 || testAll) {
	// ------------------------------ TESTCASE 24 -----------------------------------------
	argvList[1]=srcdir+"tests/inputlivenessAnalysis.C";
	resultsMe.clear();      useresults.clear();
	testOneFunctionDEFUSE("::func",argvList, debug, 30, resultsMe,useresults);
      }

      if (startNrInt==25 || testAll) {
	// ------------------------------ TESTCASE 24 -----------------------------------------
	argvList[1]=srcdir+"tests/jacobi_seq.C";
	resultsMe.clear();  useresults.clear();
	testOneFunctionDEFUSE("::jacobi", argvList, debug, 548, resultsMe,useresults);
	testOneFunctionDEFUSE("::main", argvList, debug, 548, resultsMe,useresults);
      }

    }

    // -------------------------------------- use-def tests












    // -------------------------------------- variable live tests


    if (startNrInt==1 || testAll) {
      // ------------------------------ TESTCASE 1 -----------------------------------------
      argvList[1]=srcdir+"tests/test1.C";
      results.clear();      outputResults.clear();
      vector<string> as;
      results.insert(pair<int,  vector<string> >( make_pair(8, as )));
      testOneFunction("::main",argvList, debug, 25, results,outputResults);
    }

    if (startNrInt==2 || testAll) {
      // ------------------------------ TESTCASE 2 -----------------------------------------
      argvList[1]=srcdir+"tests/test2.C";
      results.clear();      outputResults.clear();
      vector<string> as;
      results.insert(pair<int,  vector<string> >( make_pair(8, as )));
      testOneFunction("::main",argvList, debug, 23, results,outputResults);
    }

    if (startNrInt==3 || testAll) {
      // ------------------------------ TESTCASE 3 -----------------------------------------
      argvList[1]=srcdir+"tests/test3.C";
      results.clear();       outputResults.clear();
#if 1
      vector<string> in1;
      results.insert(pair<int,  vector<string> >( make_pair(6, in1 )));
      vector<string> in10;
      outputResults.insert(pair<int,  vector<string> >( make_pair(10, in10 )));
      string out10[] = {"d"};
      vector<string> out10v(out10,out10+1);
      results.insert(pair<int,  vector<string> >( make_pair(10, out10v )));
      string arrin[] = {"d"};
      vector<string> in(arrin,arrin+1);
      results.insert(pair<int,  vector<string> >( make_pair(8, in )));
      string arrout[] = {"d"};
      vector<string> out(arrout,arrout+1);
      outputResults.insert(pair<int,  vector<string> >( make_pair(8, out )));
#endif
      testOneFunction("::main", argvList, debug, 18, results,outputResults);
    }

    if (startNrInt==4 || testAll) {
      // ------------------------------ TESTCASE 4 -----------------------------------------
      argvList[1]=srcdir+"tests/test4.C";
      results.clear();       outputResults.clear();
      vector<string> in4;
      results.insert(pair<int,  vector<string> >( make_pair(4, in4 )));
      string out4[] = {"argc"};
      vector<string> out4v(out4,out4+1);
      outputResults.insert(pair<int,  vector<string> >( make_pair(4, out4v )));

      string in7[] = {"argc"};
      vector<string> in7v(in7,in7+1);
      results.insert(pair<int,  vector<string> >( make_pair(7, in7v )));
      vector<string> out7;
      outputResults.insert(pair<int,  vector<string> >( make_pair(7, out7 )));
      testOneFunction("::main", argvList, debug, 12, results, outputResults);
    }


    if (startNrInt==5 || testAll) {
      // ------------------------------ TESTCASE 5 -----------------------------------------
      argvList[1]=srcdir+"tests/test5.C";
      results.clear();       outputResults.clear();
      vector<string> in4;
      results.insert(pair<int,  vector<string> >( make_pair(10, in4 )));
      string out4[] = {"x"};
      vector<string> out4v(out4,out4+1);
      outputResults.insert(pair<int,  vector<string> >( make_pair(10, out4v )));

      vector<string> in14;
      results.insert(pair<int,  vector<string> >( make_pair(14, in14 )));
      vector<string> out14;
      outputResults.insert(pair<int,  vector<string> >( make_pair(14, in14 )));

      testOneFunction("::main", argvList, debug, 20, results, outputResults);
    }

    if (startNrInt==6 || testAll) {
      // ------------------------------ TESTCASE 6 -----------------------------------------
      argvList[1]=srcdir+"tests/test6.C";
      results.clear();  outputResults.clear();
      string in22[] = {"z"};
      vector<string> in22v(in22,in22+1);
      results.insert(pair<int,  vector<string> >( make_pair(22, in22v )));
      vector<string> out22;
      outputResults.insert(pair<int,  vector<string> >( make_pair(22, out22 )));

      testOneFunction("::main", argvList, debug, 29, results,outputResults);
    }

#if 1

    if (startNrInt==7 || testAll) {
      // ------------------------------ TESTCASE 7 -----------------------------------------
      argvList[1]=srcdir+"tests/test7.C";
      results.clear();  outputResults.clear();
      string in8[] = {"i"};
      vector<string> in8v(in8,in8+1);
      results.insert(pair<int,  vector<string> >( make_pair(8, in8v )));
      string out12[] = {"i"};
      vector<string> out12v(out12,out12+1);
      outputResults.insert(pair<int,  vector<string> >( make_pair(12, out12v )));
      testOneFunction("::main", argvList, debug, 17, results,outputResults);
    }

    if (startNrInt==8 || testAll) {
      // ------------------------------ TESTCASE 8 -----------------------------------------
      argvList[1]=srcdir+"tests/test8.C";
      results.clear();  outputResults.clear();
      string out12[] = {"i","p","x"};
      vector<string> out12v(out12,out12+3);
      outputResults.insert(pair<int,  vector<string> >( make_pair(20, out12v )));
      testOneFunction("::main", argvList, debug, 36, results,outputResults);
    }

    if (startNrInt==9 || testAll) {
      // ------------------------------ TESTCASE 9 -----------------------------------------
      argvList[1]=srcdir+"tests/test9.C";
      results.clear();  outputResults.clear();

      vector<string> out9;
      outputResults.insert(pair<int,  vector<string> >( make_pair(9, out9 )));

      vector<string> in12;
      results.insert(pair<int,  vector<string> >( make_pair(12, in12 )));

      string out12[] = {"i"};
      vector<string> out12v(out12,out12+1);
      outputResults.insert(pair<int,  vector<string> >( make_pair(12, out12v )));

      testOneFunction("::main", argvList, debug, 33, results,outputResults);
    }

    if (startNrInt==10 || testAll) {
      // ------------------------------ TESTCASE 10 -----------------------------------------
      argvList[1]=srcdir+"tests/test10.C";
      results.clear();  outputResults.clear();
      testOneFunction("::f2", argvList, debug, 6, results,outputResults);
    }

    if (startNrInt==11 || testAll) {
      // ------------------------------ TESTCASE 11 -----------------------------------------
      argvList[1]=srcdir+"tests/test11.C";
      results.clear();  outputResults.clear();
      testOneFunction("::f2", argvList, debug, 6, results,outputResults);
    }

    if (startNrInt==13 || testAll) {
      // ------------------------------ TESTCASE 13 -----------------------------------------
      argvList[1]=srcdir+"tests/test13.C";
      results.clear();  outputResults.clear();
      testOneFunction("::main", argvList, debug, 7, results,outputResults);
    }

    if (startNrInt==14 || testAll) {
      // ------------------------------ TESTCASE 14 -----------------------------------------
      argvList[1]=srcdir+"tests/test14.C";
      results.clear();  outputResults.clear();
      testOneFunction("::main", argvList, debug, 15, results,outputResults);
    }

    if (startNrInt==15 || testAll) {
      // ------------------------------ TESTCASE 15 -----------------------------------------
      argvList[1]=srcdir+"tests/test15.C";
      results.clear();  outputResults.clear();
      testOneFunction("::main", argvList, debug, 27, results,outputResults);
    }

    if (startNrInt==18 || testAll) {
      // ------------------------------ TESTCASE 18 -----------------------------------------
      argvList[1]=srcdir+"tests/test18.C";
      results.clear();  outputResults.clear();
      testOneFunction("::main", argvList, debug, 56, results,outputResults);
    }

    if (startNrInt==19 || testAll) {
      // ------------------------------ TESTCASE 19 -----------------------------------------
      argvList[1]=srcdir+"tests/test19.C";
      results.clear();  outputResults.clear();
      testOneFunction("::setMe", argvList, debug, 9, results,outputResults);
    }

    if (startNrInt==20 || testAll) {
      // ------------------------------ TESTCASE 20 -----------------------------------------
      argvList[1]=srcdir+"tests/test20.C";
      results.clear();      outputResults.clear();
      testOneFunction("::bar",argvList, debug, 8, results,outputResults);
    }

    if (startNrInt==21 || testAll) {
      // ------------------------------ TESTCASE 21 -----------------------------------------
      argvList[1]=srcdir+"tests/test21.C";
      results.clear();      outputResults.clear();
      testOneFunction("::func",argvList, debug, 11, results,outputResults);
    }

    if (startNrInt==22 || testAll) {
      // ------------------------------ TESTCASE 22 -----------------------------------------
      argvList[1]=srcdir+"tests/test22.C";
      results.clear();      outputResults.clear();
      testOneFunction("::func",argvList, debug, 18, results,outputResults);
    }

    if (startNrInt==23 || testAll) {
      // ------------------------------ TESTCASE 23 -----------------------------------------
      argvList[1]=srcdir+"tests/test23.C";
      results.clear();      outputResults.clear();
      testOneFunction("::func",argvList, debug, 37, results,outputResults);
    }
#endif

    if (startNrInt==24 || testAll) {
      // ------------------------------ TESTCASE 24 -----------------------------------------
      argvList[1]=srcdir+"tests/inputlivenessAnalysis.C";
      results.clear();  outputResults.clear();
      testOneFunction("::main", argvList, debug, 71, results,outputResults);
    }

    if (startNrInt==25 || testAll) {
      // ------------------------------ TESTCASE 25 -----------------------------------------
      argvList[1]=srcdir+"tests/jacobi_seq.C";
      results.clear();  outputResults.clear();
      testOneFunction("::jacobi", argvList, debug, 264, results,outputResults);
      testOneFunction("::main", argvList, debug, 24, results,outputResults);
    }
  }
  argvList.clear();
  return 0;
}

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

void testOneFunction( std::string funcParamName,
		      vector<string> argvList,
		      bool debug, int nrOfNodes,
		      multimap <int, vector<string> >  resultsIn,
		      multimap <int, vector<string> > resultsOut) {
  cout << " \n\n------------------------------------------\nrunning ... " << argvList[1] << endl;

  // Build the AST used by ROSE
  SgProject* project = frontend(argvList);
  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  int val = defuse->run(debug);
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
    cerr << " .. running live analysis for func : " << funcName << endl;
    FilteredCFGNode <IsDFAFilter> rem_source = liv->run(func,abortme);
    if (rem_source.getNode()!=NULL)
      dfaFunctions.push_back(rem_source);
    if (abortme)
      break;

    NodeQuerySynthesizedAttributeType nodes = NodeQuery::querySubTree(func, V_SgNode);
    SgFunctionDeclaration* decl = isSgFunctionDeclaration(func->get_declaration());
	ROSE_ASSERT(decl);
	Rose_STL_Container<SgInitializedName*> args = decl->get_parameterList()->get_args();
	cerr <<"Found args : " << args.size() << endl;
	Rose_STL_Container<SgInitializedName*>::const_iterator it = args.begin();
	for (;it!=args.end();++it) {
		nodes.push_back(*it);
	}
    if((int)nodes.size()-1!=nrOfNodes) {
  	  cerr << "Error :: Number of nodes = " << nodes.size()-1 << "  should be : " << nrOfNodes << endl;
  	  exit(1);
    } else {
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
      	  cerr << "   ... containing nodes : " << results.size() << " node: " << node->class_name() << "   tableNr : " << tableNr <<
		  			  "  resNr : " << resNr << endl;
		  if (tableNr==resNr) {
		 	     std::sort(results.begin(), results.end());
		 	     if (results==inName)
				    cerr <<"Contents in IN vector is correct! " << endl;
		 	     else {
					cerr << " >>>>>>>>>> Problem with contents ! " << endl;
					cerr << " >>>>>>>>>> RESULT ... " << endl;
				    std::vector<string>::const_iterator itv = inName.begin();
				    for (;itv!=inName.end();++itv) {
				    	string name = *itv;
						cerr << name << " " ;
				    }
					cerr << endl;
					cerr << " >>>>>>>>>> USER ... " << endl;
				    itv = results.begin();
				    for (;itv!=results.end();++itv) {
				    	string name = *itv;
						cerr << name << " " ;
				    }
					cerr << endl;
					exit(1);
		 	     }
		    if (results.size()==in.size()) {
			  hitIn++;
		    }
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
		 	     if (results==outName)
				    cerr <<"Contents in IN vector is correct! " << endl;
		 	     else {
					cerr << " >>>>>>>>>> Problem with contents ! " << endl;
					cerr << " >>>>>>>>>> RESULT ... " << endl;
				    std::vector<string>::const_iterator itv = outName.begin();
				    for (;itv!=outName.end();++itv) {
				    	string name = *itv;
						cerr << name << " " ;
				    }
					cerr << endl;
					cerr << " >>>>>>>>>> USER ... " << endl;
				    itv = results.begin();
				    for (;itv!=results.end();++itv) {
				    	string name = *itv;
						cerr << name << " " ;
				    }
					cerr << endl;
					exit(1);
		 	     }

		    if (results.size()==out.size()) {
			  hitOut++;
		    }
	        cout << " nodeNr: " << tableNr << ".  ResultSize OUT should be:" << results.size() << " -  resultSize is: " << out.size()
			  << "  foundMatches == : " << hitOut << "/" << resultsOut.size() << endl;
		  }
		}
		if (hitIn==0 && hitOut==0) {
        cout << " nodeNr: " << tableNr << " IN: " << hitIn << "/" << resultsIn.size() <<
			"        Out:" << hitOut << "/" << resultsOut.size() << endl;
		}
    }
    if (hitIn!=(int)resultsIn.size() || hitOut!=(int)resultsOut.size()) {
	  cout << " Error: No hit! ... DFA values of node " << nrOfNodes << " are not correct! " << endl;
	  exit(1);
    }

  }
  cerr << "Writing out to var.dot... " << endl;
  std::ofstream f2("var.dot");
  dfaToDot(f2, string("var"), dfaFunctions,
	   (DefUseAnalysis*)defuse, liv);
  f2.close();

  if (abortme) {
    cerr<<"ABORTING ." << endl;
    exit(1);
  }
  std::cout << "Analysis test is success." << std::endl;
}


void runCurrentFile(vector<string> &argvList, bool debug, bool debug_map) {
  // Build the AST used by ROSE
  std::cout << ">>>> Starting ROSE frontend ... " << endl;
  SgProject* project = frontend(argvList);
  std::cout << ">>>> generate PDF " << endl;
  generatePDF ( *project );
  std::cout << ">>>> start def-use analysis ... " << endl;

  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  int val = defuse->run(debug);
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
    cout << "start: " << startNrInt << "  all: " << testAll << endl;

    argvList.resize(2);
    argvList[0]=argv[0];
    multimap <int,  vector<string> >results;
    multimap <int,  vector<string> > outputResults;

    char* srcdirVar = getenv("SRCDIR");
    ROSE_ASSERT (srcdirVar);
    std::string srcdir = srcdirVar;
    srcdir += "/";

    cerr << " RUNNING VARIABLE ANALYSIS (DEFUSE) TESTS -- NR: " << startNrInt << endl;


    if (startNrInt==1 || testAll) {
      // ------------------------------ TESTCASE 1 -----------------------------------------
      argvList[1]=srcdir+"tests/test1.C";
      results.clear();      outputResults.clear();
	  vector<string> as;
      results.insert(pair<int,  vector<string> >( make_pair(8, as )));
      testOneFunction("::main",argvList, debug, 23, results,outputResults);
    }

    if (startNrInt==2 || testAll) {
      // ------------------------------ TESTCASE 2 -----------------------------------------
      argvList[1]=srcdir+"tests/test2.C";
      results.clear();      outputResults.clear();
	  vector<string> as;
      results.insert(pair<int,  vector<string> >( make_pair(8, as )));
      testOneFunction("::main",argvList, debug, 21, results,outputResults);
    }

    if (startNrInt==3 || testAll) {
      // ------------------------------ TESTCASE 3 -----------------------------------------
      argvList[1]=srcdir+"tests/test3.C";
      results.clear();       outputResults.clear();
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
      testOneFunction("::main", argvList, debug, 17, results,outputResults);
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

    #if 0

    if (startNrInt==5 || testAll) {
      // ------------------------------ TESTCASE 5 -----------------------------------------
      argvList[1]=srcdir+"tests/test5.C";
      results.clear();       outputResults.clear();
      results.insert(pair<string,int>("x", 10));
      results.insert(pair<string,int>("y", 16));
      outputResults.insert(pair<string,int>("x", 13));
      testOneFunction("::main", argvList, debug, 18, results, outputResults);
    }

    if (startNrInt==6 || testAll) {
      // ------------------------------ TESTCASE 6 -----------------------------------------
      argvList[1]=srcdir+"tests/test6.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("x", 14));
      results.insert(pair<string,int>("x", 25));
      results.insert(pair<string,int>("z", 26));
      results.insert(pair<string,int>("z", 10));
      outputResults.insert(pair<string,int>("x", 16));
      outputResults.insert(pair<string,int>("z", 22));
      testOneFunction("::main", argvList, debug, 26, results,outputResults);
    }

    if (startNrInt==7 || testAll) {
      // ------------------------------ TESTCASE 7 -----------------------------------------
      argvList[1]=srcdir+"tests/test7.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("i", 6));
      results.insert(pair<string,int>("i", 15));
      outputResults.insert(pair<string,int>("i", 8));
      outputResults.insert(pair<string,int>("i", 14));
      testOneFunction("::main", argvList, debug, 16, results,outputResults);
    }

    if (startNrInt==8 || testAll) {
      // ------------------------------ TESTCASE 8 -----------------------------------------
      argvList[1]=srcdir+"tests/test8.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("x", 24));
      results.insert(pair<string,int>("i", 12));
      results.insert(pair<string,int>("i", 32));
      outputResults.insert(pair<string,int>("i", 15));
      outputResults.insert(pair<string,int>("p", 26));
      outputResults.insert(pair<string,int>("x", 28));
      testOneFunction("::main", argvList, debug, 31, results,outputResults);
    }

    if (startNrInt==9 || testAll) {
      // ------------------------------ TESTCASE 9 -----------------------------------------
      argvList[1]=srcdir+"tests/test9.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("array", 4));
      results.insert(pair<string,int>("i", 12));
      results.insert(pair<string,int>("i", 31));
      outputResults.insert(pair<string,int>("i", 15));
      outputResults.insert(pair<string,int>("array", 21));
      testOneFunction("::main", argvList, debug, 30, results,outputResults);
    }

    if (startNrInt==10 || testAll) {
      // ------------------------------ TESTCASE 10 -----------------------------------------
      argvList[1]=srcdir+"tests/test10.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("z", 13));
      results.insert(pair<string,int>("z", 22));
      results.insert(pair<string,int>("res", 23));
      testOneFunction("::main", argvList, debug, 25, results,outputResults);
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("x", 3));
      results.insert(pair<string,int>("y", 4));
      results.insert(pair<string,int>("z", 5));
      testOneFunction("::f2", argvList, debug, 25, results,outputResults);
    }

    if (startNrInt==11 || testAll) {
      // ------------------------------ TESTCASE 11 -----------------------------------------
      argvList[1]=srcdir+"tests/test11.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("z", 19));
      results.insert(pair<string,int>("z", 63));
      results.insert(pair<string,int>("z", 73));
      results.insert(pair<string,int>("p", 24));
      results.insert(pair<string,int>("i", 28));
      results.insert(pair<string,int>("i", 56));
      results.insert(pair<string,int>("j", 41));
      results.insert(pair<string,int>("j", 77));
      results.insert(pair<string,int>("res", 42));
      testOneFunction("::main", argvList, debug, 76, results,outputResults);
      results.clear();  outputResults.clear();
      testOneFunction("::f1", argvList, debug, 76, results,outputResults);
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("x", 7));
      results.insert(pair<string,int>("y", 8));
      results.insert(pair<string,int>("z", 9));
      testOneFunction("::f2", argvList, debug, 76, results,outputResults);
    }

    if (startNrInt==13 || testAll) {
      // ------------------------------ TESTCASE 13 -----------------------------------------
      argvList[1]=srcdir+"tests/test13.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("::global", 7));
      testOneFunction("::main", argvList, debug, 9, results,outputResults);
    }

    if (startNrInt==14 || testAll) {
      // ------------------------------ TESTCASE 14 -----------------------------------------
      argvList[1]=srcdir+"tests/test14.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("a", 12));
      results.insert(pair<string,int>("b", 6));
      testOneFunction("::main", argvList, debug, 14, results,outputResults);
    }

    if (startNrInt==15 || testAll) {
      // ------------------------------ TESTCASE 15 -----------------------------------------
      argvList[1]=srcdir+"tests/test15.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("i", 12));
      results.insert(pair<string,int>("i", 26));
      results.insert(pair<string,int>("index", 26));
      results.insert(pair<string,int>("index", 8));
      testOneFunction("::main", argvList, debug, 26, results,outputResults);
    }

    if (startNrInt==18 || testAll) {
      // ------------------------------ TESTCASE 18 -----------------------------------------
      argvList[1]=srcdir+"tests/test18.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("i", 12));
      results.insert(pair<string,int>("i", 49));
      results.insert(pair<string,int>("p", 8));
      results.insert(pair<string,int>("p", 46));
      results.insert(pair<string,int>("x", 24));
      results.insert(pair<string,int>("z", 39));
      results.insert(pair<string,int>("e", 41));
      outputResults.insert(pair<string,int>("i", 15));
      outputResults.insert(pair<string,int>("x", 28));
      testOneFunction("::main", argvList, debug, 48, results,outputResults);
    }

    if (startNrInt==19 || testAll) {
      // ------------------------------ TESTCASE 19 -----------------------------------------
      argvList[1]=srcdir+"tests/test19.C";
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("::global", 2));
      results.insert(pair<string,int>("::global", 8));
      results.insert(pair<string,int>("::global2", 28));
      results.insert(pair<string,int>("a", 24));
      testOneFunction("::main", argvList, debug, 30, results,outputResults);
      results.clear();  outputResults.clear();
      results.insert(pair<string,int>("::global", 8));
      results.insert(pair<string,int>("::global2", 12));
      testOneFunction("::setMe", argvList, debug, 30, results,outputResults);
    }

    if (startNrInt==20 || testAll) {
      // ------------------------------ TESTCASE 1 -----------------------------------------
      argvList[1]=srcdir+"tests/test20.C";
      results.clear();      outputResults.clear();
      testOneFunction("::bar",argvList, debug, 5, results,outputResults);
    }

    if (startNrInt==21 || testAll) {
      // ------------------------------ TESTCASE 1 -----------------------------------------
      argvList[1]=srcdir+"tests/test21.C";
      results.clear();      outputResults.clear();
      testOneFunction("::func",argvList, debug, 9, results,outputResults);
    }

    if (startNrInt==22 || testAll) {
      // ------------------------------ TESTCASE 1 -----------------------------------------
      argvList[1]=srcdir+"tests/test22.C";
      results.clear();      outputResults.clear();
      results.insert(pair<string,int>("b", 7));
      testOneFunction("::func",argvList, debug, 12, results,outputResults);
    }

    if (startNrInt==23 || testAll) {
      // ------------------------------ TESTCASE 1 -----------------------------------------
      argvList[1]=srcdir+"tests/test23.C";
      results.clear();      outputResults.clear();
      results.insert(pair<string,int>("a",11));
      testOneFunction("::func",argvList, debug, 30, results,outputResults);
    }
#endif
  }

  cout << ">> TEST END ... " << endl;
  argvList.clear();
  return 0;
}

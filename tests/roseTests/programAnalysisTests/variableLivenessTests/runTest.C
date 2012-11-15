/******************************************
 * Category: Variable liveness analysis
 * Application to test the Variable liveness analysis
 * Created by tps in Feb 2007
 * Apparently largely cut-n-pasted from ../defUseAnalysisTests/runTest.C
 *****************************************/

#include "rose.h"
#include "DefUseAnalysis.h"
#include "LivenessAnalysis.h"
#include <string>
#include <iostream>
#if 1 /*DEBUGGING [Robb Matzke 2012-11-07]*/
#include "stringify.h"
#endif
using namespace std;

static bool
is_type_node(SgNode *node) {
    return isSgType(node)!=NULL;
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

    // Edg3 mistakenly adds SgType nodes to the AST; Edg4 adds some also, but fewer.  So we just remove them all. They
    // make no difference in the variable-liveness analysis anyway.
    nodes.erase(std::remove_if(nodes.begin(), nodes.end(), is_type_node), nodes.end());

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
      	  cerr << "   ... containing nodes : " << results.size() << " node: " << node->class_name()
               << "   tableNr : " << tableNr
               << "  resNr : " << resNr << endl;
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
	    cout << " nodeNr: " << tableNr << ".  ResultSize IN should be:" << results.size()
                 << " -  resultSize is: " << in.size()
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
	    cout << " nodeNr: " << tableNr << ".  ResultSize OUT should be:" << results.size()
                 << " -  resultSize is: " << out.size()
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
    bool debug =true;
    size_t startNr = 0;
    size_t stopNr = (size_t)(-1);
    if (argc>2) {
        startNr = strtol(argv[2], NULL, 0);
        if (argc>3) {
            stopNr = strtol(argv[3], NULL, 0);
        }
    }

    argvList.resize(2);
    argvList[0]=argv[0];
    multimap <int,  vector<string> >results;
    multimap <int,  vector<string> > outputResults;

    char* srcdirVar = getenv("SRCDIR");
    ROSE_ASSERT (srcdirVar);
    std::string srcdir = srcdirVar;
    srcdir += "/";

    // -------------------------------------- use-def tests
    // Removed because they've already been tested by ../defUseAnalysisTests. [Robb Matzke 2012-11-06]
    // -------------------------------------- use-def tests

    // -------------------------------------- variable live tests
    if (startNr<=1 && 1<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 1 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test1.C";
      results.clear();      outputResults.clear();
      vector<string> as;
      results.insert(pair<int,  vector<string> >( make_pair(8, as )));
      testOneFunction("::foo",argvList, debug, 21, results,outputResults);
    }

    if (startNr<=2 && 2<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 2 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test2.C";
      results.clear();      outputResults.clear();
      vector<string> as;
      results.insert(pair<int,  vector<string> >( make_pair(8, as )));
      testOneFunction("::foo",argvList, debug, 21, results,outputResults);
    }

    if (startNr<=3 && 3<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 3 -----------------------------------------\n";
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

      testOneFunction("::foo", argvList, debug, 16, results,outputResults);
    }

    if (startNr<=4 && 4<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 4 -----------------------------------------\n";
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
      testOneFunction("::foo", argvList, debug, 12, results, outputResults);
    }


    if (startNr<=5 && 5<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 5 -----------------------------------------\n";
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

      testOneFunction("::foo", argvList, debug, 17, results, outputResults);
    }

    if (startNr<=6 && 6<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 6 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test6.C";
      results.clear();  outputResults.clear();
      string in22[] = {"z"};
      vector<string> in22v(in22,in22+1);
      results.insert(pair<int,  vector<string> >( make_pair(22, in22v )));
      vector<string> out22;
      outputResults.insert(pair<int,  vector<string> >( make_pair(22, out22 )));

      testOneFunction("::foo", argvList, debug, 26, results,outputResults);
    }


    if (startNr<=7 && 7<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 7 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test7.C";
      results.clear();  outputResults.clear();
      string in8[] = {"i"};
      vector<string> in8v(in8,in8+1);
      results.insert(pair<int,  vector<string> >( make_pair(8, in8v )));
      string out12[] = {"i"};
      vector<string> out12v(out12,out12+1);
      outputResults.insert(pair<int,  vector<string> >( make_pair(12, out12v )));
      testOneFunction("::foo", argvList, debug, 16, results,outputResults);
    }

    if (startNr<=8 && 8<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 8 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test8.C";
      results.clear();  outputResults.clear();
      string out12[] = {"i","p","x"};
      vector<string> out12v(out12,out12+3);
      outputResults.insert(pair<int,  vector<string> >( make_pair(20, out12v )));
      testOneFunction("::foo", argvList, debug, 31, results,outputResults);
    }

    if (startNr<=9 && 9<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 9 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test9.C";
      results.clear();  outputResults.clear();

      vector<string> out9;
      outputResults.insert(pair<int,  vector<string> >( make_pair(9, out9 )));

      vector<string> in12;
      results.insert(pair<int,  vector<string> >( make_pair(12, in12 )));

      string out12[] = {"i"};
      vector<string> out12v(out12,out12+1);
      outputResults.insert(pair<int,  vector<string> >( make_pair(12, out12v )));
      testOneFunction("::foo", argvList, debug, 30, results,outputResults);
    }

    if (startNr<=10 && 10<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 10 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test10.C";
      results.clear();  outputResults.clear();
      testOneFunction("::f2", argvList, debug, 6, results,outputResults);
    }

    if (startNr<=11 && 11<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 11 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test11.C";
      results.clear();  outputResults.clear();
      testOneFunction("::f2", argvList, debug, 6, results,outputResults);
    }

    if (startNr<=13 && 13<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 13 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test13.C";
      results.clear();  outputResults.clear();
      testOneFunction("::foo", argvList, debug, 7, results,outputResults);
    }

    if (startNr<=14 && 14<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 14 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test14.C";
      results.clear();  outputResults.clear();
      testOneFunction("::f", argvList, debug, 11, results,outputResults);
    }

    if (startNr<=15 && 15<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 15 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test15.C";
      results.clear();  outputResults.clear();
      testOneFunction("::foo", argvList, debug, 22, results,outputResults);
    }

    if (startNr<=18 && 18<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 18 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test18.C";
      results.clear();  outputResults.clear();
      testOneFunction("::foo", argvList, debug, 48, results,outputResults);
    }

    if (startNr<=19 && 19<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 19 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test19.C";
      results.clear();  outputResults.clear();
      testOneFunction("::setMe", argvList, debug, 9, results,outputResults);
    }

    if (startNr<=20 && 20<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 20 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test20.C";
      results.clear();      outputResults.clear();
      testOneFunction("::bar",argvList, debug, 7, results,outputResults);
    }

    if (startNr<=21 && 21<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 21 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test21.C";
      results.clear();      outputResults.clear();
      testOneFunction("::func",argvList, debug, 12, results,outputResults);
    }

    if (startNr<=22 && 22<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 22 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test22.C";
      results.clear();      outputResults.clear();
      testOneFunction("::func",argvList, debug, 18, results,outputResults);
    }

    if (startNr<=23 && 23<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 23 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test23.C";
      results.clear();      outputResults.clear();
      testOneFunction("::func",argvList, debug, 44, results,outputResults);
    }

    if (startNr<=24 && 24<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 24 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/inputlivenessAnalysis.C";
      results.clear();  outputResults.clear();
      testOneFunction("::foo", argvList, debug, 73, results,outputResults);
    }

    if (startNr<=25 && 25<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 25 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/jacobi_seq.C";
      results.clear();  outputResults.clear();
      testOneFunction("::jacobi", argvList, debug, 248, results,outputResults);
      testOneFunction("::foo", argvList, debug, 24, results,outputResults);
    }
  }
  argvList.clear();
  return 0;
}

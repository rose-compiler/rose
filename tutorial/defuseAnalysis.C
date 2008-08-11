/****************************************** 
 * Category: DFA
 * Application to test the Def-Use Analysis
 * created by tps in Feb 2007
 *****************************************/

#include "rose.h"
#include "DefUseAnalysis.h"
#include <string>
#include <iostream>
using namespace std;

void testOneFunction( std::string funcParamName, 
		      vector<string> argvList,
		      bool debug, int nrOfNodes, 
		      multimap <string, int> results,
		      multimap <string, int> useresults) {
  cout << " \n\n------------------------------------------\nrunning ... " << argvList[1] << endl;
  // Build the AST used by ROSE
  SgProject* project = frontend(argvList);
  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  int val = defuse->run(debug);
  std::cout << "Analysis run is : " << (val ? "success" : "failure") << std::endl;

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
      exit(0);
    }
    cout << " Test has nodes:  " << nrOfNodes <<  endl;

    cout <<"\nChecking all definitions ... " << endl;
    // check nodes in multimap
    std::vector < std::pair < SgInitializedName*, SgNode* > > map = defuse->getDefMultiMapFor(func);
    if (map.size()>0) {
      std::vector < std::pair < SgInitializedName*, SgNode*> >::const_iterator j = map.begin();
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
	exit(0);
      }
    } else {
      if (results.size()!=0) {
	cout << " Error: Test node " << defuse->getIntForSgNode(func) << " should have a multimap. " << endl;
	exit(0);
      }
    }
  
    cout <<"\nChecking all uses ... " << endl;
    // check nodes in multimap
    map = defuse->getUseMultiMapFor(func);
    if (map.size()>0) {
      std::vector <std::pair < SgInitializedName*, SgNode* > >::const_iterator j = map.begin();
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
	exit(0);
      }
    } // if
  }
  std::cout << "Analysis test is success." << std::endl;
}


void runCurrentFile(vector<string> argvList) {
  // Build the AST used by ROSE
  std::cout << ">>>> Starting ROSE frontend ... " << endl;
  SgProject* project = frontend(argvList);
  std::cout << ">>>> generate PDF " << endl;
  generatePDF ( *project );
  std::cout << ">>>> start def-use analysis ... " << endl;

  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  bool debug = true;
  int val = defuse->run(debug);
  std::cout << "Analysis is : " << (val ? "success" : "failure") << std::endl;
  if (debug==false)
    defuse->dfaToDOT();
  
  //example usage
  // testing
//std::list<SgNode*> vars = NodeQuery::querySubTree(project, V_SgInitializedName); 
//std::list<SgNode*>::const_iterator i = vars.begin();
  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(project, V_SgInitializedName); 
  NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
  for (; i!=vars.end();++i) {
    SgInitializedName* initName = isSgInitializedName(*i);
    std::string name = initName->get_qualified_name().str();
  
    vector<SgNode* > vec = defuse->getDefFor(initName, initName);
    if (vec.size()>0)
      std::cout << "  DEF>> Vector entries for " << name <<  " ( " << 
	initName << " ) : " << vec.size() << std::endl;

  }

  // testing
  vars = NodeQuery::querySubTree(project, V_SgReturnStmt); 
  i = vars.begin();
  for (; i!=vars.end();++i) {
    SgReturnStmt* ret = isSgReturnStmt(*i);
    ROSE_ASSERT(ret);
    typedef std::vector <std::pair < SgInitializedName*, SgNode* > > mymap; 
    mymap map = defuse->getUseMultiMapFor(ret);    
    mymap::const_iterator j;
    j = map.begin();
    for (; j!=map.end();++j) {
      SgInitializedName* initName = isSgInitializedName(j->first);
      ROSE_ASSERT(initName);
      std::string name = initName->get_qualified_name().str();
      
      vector<SgNode* > vec = defuse->getUseFor(ret, initName);
      if (vec.size()>0)
	std::cout << "  USE>> Vector entries for " << name <<  " ( " << 
	  ret << " ) : " << vec.size() << std::endl;

    }
  }
  // print resulting table
  cout << "\nDEFMAP" << endl;
  defuse->printDefMap();
  cout << "\nUSEMAP" << endl;
  defuse->printUseMap();
}

void usage() {
  cout << " Usage: " << endl;
  cout << "   runTest all [startNr]   --- to test all testcases" << endl;
  cout << "   runTest [file]          --- to test one file" << endl;
  exit(0);
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
    runCurrentFile(argvList);
  }  
  
  if (allTests==true) {
    bool debug = false;
    string startNr = "";
    if (argc>2)
      startNr = argv[2];
    bool testAll = false;
    int startNrInt = 0;
    if (startNr=="")
      testAll=true;
    else 
      startNrInt = strToInt(startNr);
    //cout << "start: " << startNrInt << "  all: " << testAll << endl;

    argvList.resize(2);
    argvList[0]="runTest";
    multimap <string, int> results;
    multimap <string, int> useresults;

    char* srcdirVar = getenv("SRCDIR");
    ROSE_ASSERT (srcdirVar);
    std::string srcdir = srcdirVar;
    srcdir += "/";

    if (startNrInt<=1 || testAll) {
      // ------------------------------ TESTCASE 1 -----------------------------------------
      argvList[1]=srcdir+"tests/test1.C";
      results.clear();      useresults.clear();
      results.insert(pair<string,int>("a", 7));
      results.insert(pair<string,int>("a", 16));    
      results.insert(pair<string,int>("c", 17));
      testOneFunction("::main",argvList, debug, 21, results,useresults);
    }

    if (startNrInt<=2 || testAll) {
      // ------------------------------ TESTCASE 2 -----------------------------------------
      argvList[1]=srcdir+"tests/test2.C";
      results.clear();      useresults.clear();
      results.insert(pair<string,int>("a", 7));
      results.insert(pair<string,int>("a", 16));    
      results.insert(pair<string,int>("c", 17));
      testOneFunction("::main",argvList, debug, 19, results,useresults);
    }

    if (startNrInt<=3 || testAll) {
      // ------------------------------ TESTCASE 3 -----------------------------------------
      argvList[1]=srcdir+"tests/test3.C";
      results.clear();       useresults.clear();
      results.insert(pair<string,int>("d", 14));
      testOneFunction("::main", argvList, debug, 17, results,useresults);
    }

    if (startNrInt<=4 || testAll) {
      // ------------------------------ TESTCASE 4 -----------------------------------------
      argvList[1]=srcdir+"tests/test4.C";
      results.clear();       useresults.clear();
      results.insert(pair<string,int>("::globalvar", 11));
      testOneFunction("::main", argvList, debug, 14, results, useresults);
    }

    if (startNrInt<=5 || testAll) {
      // ------------------------------ TESTCASE 5 -----------------------------------------
      argvList[1]=srcdir+"tests/test5.C";
      results.clear();       useresults.clear();
      results.insert(pair<string,int>("x", 9));
      results.insert(pair<string,int>("y", 15));    
      useresults.insert(pair<string,int>("x", 12));    
      testOneFunction("::main", argvList, debug, 18, results, useresults);
    }

    if (startNrInt<=6 || testAll) {
      // ------------------------------ TESTCASE 6 -----------------------------------------
      argvList[1]=srcdir+"tests/test6.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 13));
      results.insert(pair<string,int>("x", 24));
      results.insert(pair<string,int>("z", 25));    
      results.insert(pair<string,int>("z", 9));    
      useresults.insert(pair<string,int>("x", 15));    
      useresults.insert(pair<string,int>("z", 21));    
      testOneFunction("::main", argvList, debug, 26, results,useresults);
    }

    if (startNrInt<=7 || testAll) {
      // ------------------------------ TESTCASE 7 -----------------------------------------
      argvList[1]=srcdir+"tests/test7.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("i", 5));
      results.insert(pair<string,int>("i", 14));
      useresults.insert(pair<string,int>("i", 7));    
      useresults.insert(pair<string,int>("i", 13));    
      testOneFunction("::main", argvList, debug, 16, results,useresults);
    }

    if (startNrInt<=8 || testAll) {
      // ------------------------------ TESTCASE 8 -----------------------------------------
      argvList[1]=srcdir+"tests/test8.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 23));
      results.insert(pair<string,int>("i", 11));
      results.insert(pair<string,int>("i", 31));
      useresults.insert(pair<string,int>("i", 14));    
      useresults.insert(pair<string,int>("p", 25));    
      useresults.insert(pair<string,int>("x", 27));    
      testOneFunction("::main", argvList, debug, 31, results,useresults);
    }

    if (startNrInt<=9 || testAll) {
      // ------------------------------ TESTCASE 9 -----------------------------------------
      argvList[1]=srcdir+"tests/test9.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("array", 3));
      results.insert(pair<string,int>("i", 11));
      results.insert(pair<string,int>("i", 30));
      useresults.insert(pair<string,int>("i", 14));    
      useresults.insert(pair<string,int>("array", 20));    
      testOneFunction("::main", argvList, debug, 30, results,useresults);
    }

    if (startNrInt<=10 || testAll) {
      // ------------------------------ TESTCASE 10 -----------------------------------------
      argvList[1]=srcdir+"tests/test10.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("z", 12));
      results.insert(pair<string,int>("z", 21));
      results.insert(pair<string,int>("res", 22));
      testOneFunction("::main", argvList, debug, 25, results,useresults);
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 2));
      results.insert(pair<string,int>("y", 3));
      results.insert(pair<string,int>("z", 4));
      testOneFunction("::f2", argvList, debug, 25, results,useresults);
    }

    if (startNrInt<=11 || testAll) {
      // ------------------------------ TESTCASE 11 -----------------------------------------
      argvList[1]=srcdir+"tests/test11.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("z", 18));
      results.insert(pair<string,int>("z", 62));
      results.insert(pair<string,int>("z", 72));
      results.insert(pair<string,int>("p", 23));
      results.insert(pair<string,int>("i", 27));
      results.insert(pair<string,int>("i", 55));
      results.insert(pair<string,int>("j", 40));
      results.insert(pair<string,int>("j", 76));
      results.insert(pair<string,int>("res", 41));
      testOneFunction("::main", argvList, debug, 76, results,useresults);
      results.clear();  useresults.clear();
      testOneFunction("::f1", argvList, debug, 76, results,useresults);
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 6));
      results.insert(pair<string,int>("y", 7));
      results.insert(pair<string,int>("z", 8));
      testOneFunction("::f2", argvList, debug, 76, results,useresults);
    }

    if (startNrInt<=13 || testAll) {
      // ------------------------------ TESTCASE 13 -----------------------------------------
      argvList[1]=srcdir+"tests/test13.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("::global", 6));
      testOneFunction("::main", argvList, debug, 9, results,useresults);
    }

    if (startNrInt<=14 || testAll) {
      // ------------------------------ TESTCASE 14 -----------------------------------------
      argvList[1]=srcdir+"tests/test14.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("a", 11));
      results.insert(pair<string,int>("b", 5));
      testOneFunction("::main", argvList, debug, 14, results,useresults);
    }

    if (startNrInt<=15 || testAll) {
      // ------------------------------ TESTCASE 15 -----------------------------------------
      argvList[1]=srcdir+"tests/test15.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("i", 11));
      results.insert(pair<string,int>("i", 25));
      results.insert(pair<string,int>("index", 25));
      results.insert(pair<string,int>("index", 7));
      testOneFunction("::main", argvList, debug, 26, results,useresults);
    }

    if (startNrInt<=18 || testAll) {
      // ------------------------------ TESTCASE 18 -----------------------------------------
      argvList[1]=srcdir+"tests/test18.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("i", 11));
      results.insert(pair<string,int>("i", 48));
      results.insert(pair<string,int>("p", 7));
      results.insert(pair<string,int>("p", 45));
      results.insert(pair<string,int>("x", 23));
      results.insert(pair<string,int>("z", 38));
      results.insert(pair<string,int>("e", 40));
      useresults.insert(pair<string,int>("i", 14));    
      useresults.insert(pair<string,int>("x", 27));    
      testOneFunction("::main", argvList, debug, 48, results,useresults);
    }

    if (startNrInt<=19 || testAll) {
      // ------------------------------ TESTCASE 19 -----------------------------------------
      argvList[1]=srcdir+"tests/test19.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("::global", 1));
      results.insert(pair<string,int>("::global", 7));
      results.insert(pair<string,int>("::global2", 27));
      results.insert(pair<string,int>("a", 23));
      testOneFunction("::main", argvList, debug, 30, results,useresults);
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("::global", 7));
      results.insert(pair<string,int>("::global2", 11));
      testOneFunction("::setMe", argvList, debug, 30, results,useresults);
    }

  }

  return 0;
}

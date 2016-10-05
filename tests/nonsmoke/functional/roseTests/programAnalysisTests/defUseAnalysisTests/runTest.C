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
  if (debug)
    cout <<"\n\n------------------------------------------\ntesting ... " << argvList[1] << endl;
  // Build the AST used by ROSE
  SgProject* project = frontend(argvList);
  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  int val = defuse->run(debug);
  if (debug)
    std::cout << "Analysis run is : " << (val ?  "failure" : "success" ) << " " << val << std::endl;
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

    if (debug)
      cout << "\n------------------------\nchecking for " << name << " -- " << funcName << " -- " << nodeNr << endl;
    if (maxNodes!=nrOfNodes) {
	cerr << " Error: Test should have " << nrOfNodes << " nodes. found: " << maxNodes << endl;
      abort();
    }
    if (debug)
      cout << " Test has nodes:  " << nrOfNodes <<  endl;
    if (debug)
      cout <<"\nChecking all definitions ... " << endl;
    // check nodes in multimap
    std::vector <std::pair < SgInitializedName*, SgNode*> > map = defuse->getDefMultiMapFor(func);
    if (map.size()>0) {
      std::vector < std::pair <SgInitializedName*, SgNode*> >::const_iterator j = map.begin();
      unsigned int hit=0;
      SgNode* node = NULL;
      string name="";
      for (;j!=map.end();++j) {
	SgInitializedName* in_node = j->first;
	node = j->second;
	name= in_node->get_qualified_name().str();
	if (debug)
	  cout << " ... checking :  " << name << endl;
	multimap <string, int>::const_iterator k =results.begin();
	for (;k!=results.end();++k) {
	  string resName = k->first;
	  int resNr = k->second;
	  int tableNr = defuse->getIntForSgNode(node);
	  if (name==resName)
	    if (debug)
	      cout << " ... defNr: " << resNr << "  inTable: " << tableNr <<  endl; 
	  if (name==resName && tableNr==resNr) {
	    hit++;
	    if (debug)
	      cout << " Hit " << hit << "/" << results.size() << " - (" << name << "," << resNr << ")" << endl;
	  }
	}

      }
      if (hit!=results.size()) {
	cerr << " Error: No hit! ... DFA values of node " << nodeNr << " are not correct! " << endl;
	exit(1);
      }
    } else {
      if (results.size()!=0) {
	cerr << " Error: Test node " << defuse->getIntForSgNode(func) << " should have a multimap. " << endl;
	exit(1);
      }
    }
  
    if (debug)
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
	if (debug)
	  cout << " ... checking :  " << name << endl;
	multimap <string, int>::const_iterator k =useresults.begin();
	for (;k!=useresults.end();++k) {
	  string resName = k->first;
	  int resNr = k->second;
	  int tableNr = defuse->getIntForSgNode(node);
	  if (name==resName)
	    if (debug)
	      cout << " ... defNr: " << resNr << "  inTable: " << tableNr <<  endl; 
	  if (name==resName && tableNr==resNr) {
	    hit++;
	    if (debug)
	      cout << " Hit " << hit << "/" << useresults.size() << " - (" << name << "," << resNr << ")" << endl;
	  }
	}

      }
      if (hit!=useresults.size()) {
	cerr << " Error: No hit! ... DFA values of node " << nrOfNodes << " are not correct! " << endl;
	exit(1);
      }
    } // if
  }
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
  
  //example usage
  // testing
  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(project, V_SgInitializedName); 
  NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
  for (; i!=vars.end();++i) {
    SgInitializedName* initName = isSgInitializedName(*i);
    std::string name = initName->get_qualified_name().str();
  
    vector<SgNode* > vec = defuse->getDefFor(initName, initName);
    if (vec.size()>0)
      if (debug)
	std::cout << "  DEF>> Vector entries for " << name <<  " ( " << 
	  initName << " ) : " << vec.size() << std::endl;

  }

  // testing
  vars = NodeQuery::querySubTree(project, V_SgReturnStmt); 
  i = vars.begin();
  typedef std::vector <std::pair <SgInitializedName*, SgNode* > > maptype; 
  for (; i!=vars.end();++i) {
    SgReturnStmt* ret = isSgReturnStmt(*i);
    ROSE_ASSERT(ret);
    
    maptype map = defuse->getUseMultiMapFor(ret);    
    maptype::const_iterator j;
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
  if (debug_map) {
      cout << "\nDEFMAP" << endl;
    defuse->printDefMap();
      cout << "\nUSEMAP" << endl;
    defuse->printUseMap();
  }
  delete project;
  delete defuse;
}

void usage() {
  cout << " Usage: " << endl;
  cout << "   runTest all [startNr [stopNr]]   --- to test all testcases between startNr and stopNr, inclusive" << endl;
  cout << "   runTest [file]                   --- to test one file" << endl;
  exit(1);
}

int strToInt(string str) {
  int b;
  std::stringstream ss(str);
  ss >> b;
  if (!ss) return 0;
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
    bool debug = true;
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
    multimap <string, int> results;
    multimap <string, int> useresults;

    char* srcdirVar = getenv("SRCDIR");
    ROSE_ASSERT (srcdirVar);
    std::string srcdir = srcdirVar;
    srcdir += "/";

    if (startNr<=1 && 1<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 1 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test1.C";
      results.clear();      useresults.clear();
      results.insert(pair<string,int>("a", 8));
      results.insert(pair<string,int>("a", 17));    
      results.insert(pair<string,int>("c", 18));
      testOneFunction("::foo",argvList, debug, 21, results,useresults);
    }

    if (startNr<=2 && 2<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 2 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test2.C";
      results.clear();      useresults.clear();
      results.insert(pair<string,int>("a", 8));
      results.insert(pair<string,int>("a", 17));    
      results.insert(pair<string,int>("c", 18));
      testOneFunction("::foo",argvList, debug, 21, results,useresults);
    }

    if (startNr<=3 && 3<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 3 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test3.C";
      results.clear();       useresults.clear();
      results.insert(pair<string,int>("d", 15));
      testOneFunction("::foo", argvList, debug, 17, results,useresults);
    }

    if (startNr<=4 && 4<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 4 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test4.C";
      results.clear();       useresults.clear();
      results.insert(pair<string,int>("::globalvar", 12));
      testOneFunction("::foo", argvList, debug, 14, results, useresults);
    }

    if (startNr<=5 && 5<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 5 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test5.C";
      results.clear();       useresults.clear();
      results.insert(pair<string,int>("x", 10));
      results.insert(pair<string,int>("y", 16));    
      useresults.insert(pair<string,int>("x", 13));    
      testOneFunction("::foo", argvList, debug, 18, results, useresults);
    }

    if (startNr<=6 && 6<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 6 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test6.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 14));
      results.insert(pair<string,int>("x", 25));
      results.insert(pair<string,int>("z", 26));    
      results.insert(pair<string,int>("z", 10));    
      useresults.insert(pair<string,int>("x", 16));    
      useresults.insert(pair<string,int>("z", 22));    
      testOneFunction("::foo", argvList, debug, 26, results,useresults);
    }

    if (startNr<=7 && 7<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 7 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test7.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("i", 6));
      results.insert(pair<string,int>("i", 15));
      useresults.insert(pair<string,int>("i", 8));    
      useresults.insert(pair<string,int>("i", 14));    
      testOneFunction("::foo", argvList, debug, 16, results,useresults);
    }

    if (startNr<=8 && 8<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 8 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test8.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 24));
      results.insert(pair<string,int>("i", 12));
      results.insert(pair<string,int>("i", 32));
      useresults.insert(pair<string,int>("i", 15));    
      useresults.insert(pair<string,int>("p", 26));    
      useresults.insert(pair<string,int>("x", 28));    
      testOneFunction("::foo", argvList, debug, 31, results,useresults);
    }

    if (startNr<=9 && 9<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 9 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test9.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("array", 4));
      results.insert(pair<string,int>("i", 12));
      results.insert(pair<string,int>("i", 31));
      useresults.insert(pair<string,int>("i", 15));    
      // useresults.insert(pair<string,int>("array", 20)); // tps: fixed this    
      testOneFunction("::foo", argvList, debug, 30, results,useresults);
    }

    if (startNr<=10 && 10<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 10 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test10.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("z", 13));
      results.insert(pair<string,int>("z", 22));
      results.insert(pair<string,int>("res", 23));
      testOneFunction("::foo", argvList, debug, 25, results,useresults);
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 3));
      results.insert(pair<string,int>("y", 4));
      results.insert(pair<string,int>("z", 5));
      testOneFunction("::f2", argvList, debug, 25, results,useresults);
    }

    if (startNr<=11 && 11<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 11 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test11.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("z", 19));
      results.insert(pair<string,int>("z", 63));
      results.insert(pair<string,int>("z", 73));
      results.insert(pair<string,int>("p", 24));
      results.insert(pair<string,int>("i", 28));
      results.insert(pair<string,int>("i", 56));
      results.insert(pair<string,int>("j", 40)); //41
      results.insert(pair<string,int>("j", 77));
      //      results.insert(pair<string,int>("res", 42));
      testOneFunction("::foo", argvList, debug, 76, results,useresults);
      results.clear();  useresults.clear();
      testOneFunction("::f1", argvList, debug, 76, results,useresults);
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 7));
      results.insert(pair<string,int>("y", 8));
      results.insert(pair<string,int>("z", 9));
      testOneFunction("::f2", argvList, debug, 76, results,useresults);
    }

    if (startNr<=13 && 13<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 13 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test13.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("::global", 7));
      testOneFunction("::foo", argvList, debug, 9, results,useresults);
    }

    if (startNr<=14 && 14<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 14 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test14.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("a", 12));
      results.insert(pair<string,int>("b", 6));
      testOneFunction("::f", argvList, debug, 14, results,useresults);
    }

    if (startNr<=15 && 15<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 15 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test15.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("i", 12));
      results.insert(pair<string,int>("i", 26));
      results.insert(pair<string,int>("index", 26));
      results.insert(pair<string,int>("index", 8));
      testOneFunction("::foo", argvList, debug, 26, results,useresults);
    }

    if (startNr<=18 && 18<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 18 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test18.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("i", 12));
      results.insert(pair<string,int>("i", 49));
      results.insert(pair<string,int>("p", 8));
      results.insert(pair<string,int>("p", 46));
      results.insert(pair<string,int>("x", 24));
      results.insert(pair<string,int>("z", 39));
      results.insert(pair<string,int>("e", 41));
      useresults.insert(pair<string,int>("i", 15));    
      useresults.insert(pair<string,int>("x", 28));    
      testOneFunction("::foo", argvList, debug, 48, results,useresults);
    }

    if (startNr<=19 && 19<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 19 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test19.C";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("::global", 2));
      results.insert(pair<string,int>("::global", 8));
      results.insert(pair<string,int>("::global2", 28));
      results.insert(pair<string,int>("a", 24));
      testOneFunction("::foo", argvList, debug, 30, results,useresults);
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("::global", 8));
      results.insert(pair<string,int>("::global2", 12));
      testOneFunction("::setMe", argvList, debug, 30, results,useresults);
    }

    if (startNr<=20 && 20<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 20 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test20.C";
      results.clear();      useresults.clear();
      testOneFunction("::bar", argvList, debug, 8, results, useresults);
    }

    if (startNr<=21 && 21<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 21 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test21.C";
      results.clear();      useresults.clear();
      testOneFunction("::func",argvList, debug, 13, results,useresults);
    }

    if (startNr<=22 && 22<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 22 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test22.C";
      results.clear();      useresults.clear();
      results.insert(pair<string,int>("b", 9));
      testOneFunction("::func",argvList, debug, 19, results,useresults);
    }

    if (startNr<=23 && 23<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 23 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test23.C";
      results.clear();      useresults.clear();
      results.insert(pair<string,int>("a",16));
      testOneFunction("::func",argvList, debug, 44, results,useresults);
    }

    if (startNr<=24 && 24<=stopNr) {
      std::cout <<"------------------------------ TESTCASE 24 -----------------------------------------\n";
      argvList[1]=srcdir+"tests/test24.C";
      results.clear();      useresults.clear();
      results.insert(pair<string,int>("t",19));
      //      results.insert(pair<string,int>("t",22));
      testOneFunction("::func",argvList, debug, 26, results,useresults);
    }

  }
  
  argvList.clear();
  return 0;
}

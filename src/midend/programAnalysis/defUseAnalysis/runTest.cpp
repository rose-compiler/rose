/****************************************** 
 * Category: DFA
 * Application to test the Def-Use Analysis
 * created by tps in Feb 2007
 *****************************************/

#include "DefUseAnalysis.h"
#include <string>
#include <iostream>
using namespace std;

void testOneFunction( std::string funcParamName, 
                      int argc, char * argv[], 
                      bool debug, int nrOfNodes, 
                      multimap <string, int> results,
                      multimap <string, int> useresults) {
  if (debug)
    cout << " \n\n------------------------------------------\nrunning ... " << argv[1] << endl;
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  int val = defuse->run(debug);
  if (debug)
    std::cout << "Analysis run is : " << (val ? "success" : "failure") << std::endl;

  if (debug==false)
    defuse->dfaToDOT();

  Rose_STL_Container<SgNode*> vars = NodeQuery::querySubTree(project, V_SgFunctionDefinition); 
  Rose_STL_Container<SgNode*>::const_iterator i = vars.begin();
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
      cout << " Error: Test should have " << nrOfNodes << " nodes. found: " << maxNodes << endl;
      exit(0);
    }
    if (debug)
      cout << " Test has nodes:  " << nrOfNodes <<  endl;

    if (debug)
      cout <<"\nChecking all definitions ... " << endl;
    // check nodes in multimap
    std::multimap <SgInitializedName*, SgNode* > map = defuse->getDefMultiMapFor(func);
    if (map.size()>0) {
      std::multimap <SgInitializedName*, SgNode*>::const_iterator j = map.begin();
      int hit=0;
      for (;j!=map.end();++j) {
        SgInitializedName* in_node = j->first;
        SgNode* node = j->second;
        string name= in_node->get_qualified_name().str();
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
      if (hit!=(int)results.size()) {
        cout << " Error: No hit! ... DFA values of node " << nrOfNodes << " are not correct! " << endl;
        exit(0);
      }
    } else {
      if (results.size()!=0) {
        cout << " Error: Test node " << defuse->getIntForSgNode(func) << " should have a multimap. " << endl;
        exit(0);
      }
    }
  
    if (debug)
      cout <<"\nChecking all uses ... " << endl;
    // check nodes in multimap
    map = defuse->getUseMultiMapFor(func);
    if (map.size()>0) {
      std::multimap <SgInitializedName*, SgNode*>::const_iterator j = map.begin();
      int hit=0;
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
      if (hit!=(int)useresults.size()) {
        cout << " Error: No hit! ... DFA values of node " << nrOfNodes << " are not correct! " << endl;
        exit(0);
      }
    } // if
  }
  if (debug)
    std::cout << "Analysis test is success." << std::endl;
}


void runCurrentFile(int argc, char * argv[]) {
  // Build the AST used by ROSE
  if (debug)
    std::cout << ">>>> Starting ROSE frontend ... " << endl;
  SgProject* project = frontend(argc,argv);
  if (debug)
    std::cout << ">>>> generate PDF " << endl;
  generatePDF ( *project );
  if (debug)
    std::cout << ">>>> start def-use analysis ... " << endl;

  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  bool debug = false;
  int val = defuse->run(debug);
  if (debug)
    std::cout << "Analysis is : " << (val ? "success" : "failure") << std::endl;
  if (debug==false)
    defuse->dfaToDOT();
  
  //example usage
  // testing
  Rose_STL_Container<SgNode*> vars = NodeQuery::querySubTree(project, V_SgInitializedName); 
  Rose_STL_Container<SgNode*>::const_iterator i = vars.begin();
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
  for (; i!=vars.end();++i) {
    SgReturnStmt* ret = isSgReturnStmt(*i);
    ROSE_ASSERT(ret);
    std::multimap <SgInitializedName*, SgNode* > 
      map = defuse->getUseMultiMapFor(ret);    
    std::multimap <SgInitializedName*, SgNode*>::const_iterator j;
    j = map.begin();
    for (; j!=map.end();++j) {
      SgInitializedName* initName = isSgInitializedName(j->first);
      ROSE_ASSERT(initName);
      std::string name = initName->get_qualified_name().str();
      
      vector<SgNode* > vec = defuse->getUseFor(ret, initName);
      if (vec.size()>0)
        if (debug)
          std::cout << "  USE>> Vector entries for " << name <<  " ( " << 
            ret << " ) : " << vec.size() << std::endl;

    }
  }
  // print resulting table
  if (debug) {
    cout << "\nDEFMAP" << endl;
    defuse->printDefMap();
    cout << "\nUSEMAP" << endl;
    defuse->printUseMap();
  }
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

  if (allTests==false) {
    runCurrentFile(argc, argv);
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

    argc=2;  
    argv[0]="runTest";
    multimap <string, int> results;
    multimap <string, int> useresults;

    if (startNrInt<=1 || testAll) {
      // ------------------------------ TESTCASE 1 -----------------------------------------
      argv[1]="tests/test1.c";
      results.clear();      useresults.clear();
      results.insert(pair<string,int>("a", 7));
      results.insert(pair<string,int>("a", 16));    
      results.insert(pair<string,int>("c", 17));
      testOneFunction("::main",argc, argv, debug, 21, results,useresults);
    }

    if (startNrInt<=2 || testAll) {
      // ------------------------------ TESTCASE 2 -----------------------------------------
      argv[1]="tests/test2.c";
      results.clear();      useresults.clear();
      results.insert(pair<string,int>("a", 7));
      results.insert(pair<string,int>("a", 16));    
      results.insert(pair<string,int>("c", 17));
      testOneFunction("::main",argc, argv, debug, 19, results,useresults);
    }

    if (startNrInt<=3 || testAll) {
      // ------------------------------ TESTCASE 3 -----------------------------------------
      argv[1]="tests/test3.c";
      results.clear();       useresults.clear();
      results.insert(pair<string,int>("d", 14));
      testOneFunction("::main", argc, argv, debug, 17, results,useresults);
    }

    if (startNrInt<=4 || testAll) {
      // ------------------------------ TESTCASE 4 -----------------------------------------
      argv[1]="tests/test4.c";
      results.clear();       useresults.clear();
      results.insert(pair<string,int>("::globalvar", 11));
      testOneFunction("::main", argc, argv, debug, 14, results, useresults);
    }

    if (startNrInt<=5 || testAll) {
      // ------------------------------ TESTCASE 5 -----------------------------------------
      argv[1]="tests/test5.c";
      results.clear();       useresults.clear();
      results.insert(pair<string,int>("x", 9));
      results.insert(pair<string,int>("y", 15));    
      useresults.insert(pair<string,int>("x", 12));    
      testOneFunction("::main", argc, argv, debug, 18, results, useresults);
    }

    if (startNrInt<=6 || testAll) {
      // ------------------------------ TESTCASE 6 -----------------------------------------
      argv[1]="tests/test6.c";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 13));
      results.insert(pair<string,int>("x", 24));
      results.insert(pair<string,int>("z", 25));    
      results.insert(pair<string,int>("z", 9));    
      useresults.insert(pair<string,int>("x", 15));    
      useresults.insert(pair<string,int>("z", 21));    
      testOneFunction("::main", argc, argv, debug, 26, results,useresults);
    }

    if (startNrInt<=7 || testAll) {
      // ------------------------------ TESTCASE 7 -----------------------------------------
      argv[1]="tests/test7.c";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("i", 5));
      results.insert(pair<string,int>("i", 14));
      useresults.insert(pair<string,int>("i", 7));    
      useresults.insert(pair<string,int>("i", 13));    
      testOneFunction("::main", argc, argv, debug, 16, results,useresults);
    }

    if (startNrInt<=8 || testAll) {
      // ------------------------------ TESTCASE 8 -----------------------------------------
      argv[1]="tests/test8.c";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 23));
      results.insert(pair<string,int>("i", 11));
      results.insert(pair<string,int>("i", 31));
      useresults.insert(pair<string,int>("i", 14));    
      useresults.insert(pair<string,int>("p", 25));    
      useresults.insert(pair<string,int>("x", 27));    
      testOneFunction("::main", argc, argv, debug, 31, results,useresults);
    }

    if (startNrInt<=9 || testAll) {
      // ------------------------------ TESTCASE 9 -----------------------------------------
      argv[1]="tests/test9.c";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("array", 3));
      results.insert(pair<string,int>("i", 11));
      results.insert(pair<string,int>("i", 30));
      useresults.insert(pair<string,int>("i", 14));    
      useresults.insert(pair<string,int>("array", 20));    
      testOneFunction("::main", argc, argv, debug, 30, results,useresults);
    }

    if (startNrInt<=10 || testAll) {
      // ------------------------------ TESTCASE 10 -----------------------------------------
      argv[1]="tests/test10.c";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("z", 12));
      results.insert(pair<string,int>("z", 21));
      results.insert(pair<string,int>("res", 22));
      testOneFunction("::main", argc, argv, debug, 25, results,useresults);
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 2));
      results.insert(pair<string,int>("y", 3));
      results.insert(pair<string,int>("z", 4));
      testOneFunction("::f2", argc, argv, debug, 25, results,useresults);
    }

    if (startNrInt<=11 || testAll) {
      // ------------------------------ TESTCASE 11 -----------------------------------------
      argv[1]="tests/test11.c";
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
      testOneFunction("::main", argc, argv, debug, 76, results,useresults);
      results.clear();  useresults.clear();
      testOneFunction("::f1", argc, argv, debug, 76, results,useresults);
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("x", 6));
      results.insert(pair<string,int>("y", 7));
      results.insert(pair<string,int>("z", 8));
      testOneFunction("::f2", argc, argv, debug, 76, results,useresults);
    }

    if (startNrInt<=13 || testAll) {
      // ------------------------------ TESTCASE 13 -----------------------------------------
      argv[1]="tests/test13.c";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("::global", 6));
      testOneFunction("::main", argc, argv, debug, 9, results,useresults);
    }

    if (startNrInt<=14 || testAll) {
      // ------------------------------ TESTCASE 14 -----------------------------------------
      argv[1]="tests/test14.c";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("a", 11));
      results.insert(pair<string,int>("b", 5));
      testOneFunction("::main", argc, argv, debug, 14, results,useresults);
    }

    if (startNrInt<=15 || testAll) {
      // ------------------------------ TESTCASE 15 -----------------------------------------
      argv[1]="tests/test15.c";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("i", 11));
      results.insert(pair<string,int>("i", 25));
      results.insert(pair<string,int>("index", 25));
      results.insert(pair<string,int>("index", 7));
      testOneFunction("::main", argc, argv, debug, 26, results,useresults);
    }

    if (startNrInt<=18 || testAll) {
      // ------------------------------ TESTCASE 18 -----------------------------------------
      argv[1]="tests/test18.c";
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
      testOneFunction("::main", argc, argv, debug, 48, results,useresults);
    }

    if (startNrInt<=19 || testAll) {
      // ------------------------------ TESTCASE 19 -----------------------------------------
      argv[1]="tests/test19.c";
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("::global", 1));
      results.insert(pair<string,int>("::global", 7));
      results.insert(pair<string,int>("::global2", 27));
      results.insert(pair<string,int>("a", 23));
      testOneFunction("::main", argc, argv, debug, 30, results,useresults);
      results.clear();  useresults.clear();
      results.insert(pair<string,int>("::global", 7));
      results.insert(pair<string,int>("::global2", 11));
      testOneFunction("::setMe", argc, argv, debug, 30, results,useresults);
    }

  }

  return 1;
}


#include <rose.h>

#include <AstInterface.h>
#include <CFGImpl.h>
#include <string>
#include <iostream>
#include <GraphDotOutput.h>
#include <CommandOptions.h>
#include <GraphIO.h>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
  cerr << "-dot :generate DOT output \n";
  cerr << "-oa : use OpenAnalysis CFG \n";
}

bool GenerateDOT( int argc,  char * argv[] )
{
  for (int i = 1; i < argc; ++i) {
    if ( argv[i] != 0 && !strcmp(argv[i], "-dot"))
        return true;
  }
  return false;
}

bool UseOA( int argc,  char * argv[] )
{
  for (int i = 1; i < argc; ++i) {
    if ( argv[i] != 0 && !strcmp(argv[i], "-oa"))
        return true;
  }
  return false;
}

class TestCFGWrap
{
  int t;
 protected:
  DefaultCFGImpl graph;
 public:
  typedef enum {ROSE, OA} AnalysisDomain; 
  TestCFGWrap( AnalysisDomain _t) : t(_t) {}

  void operator()(SgNode* head)
  { 
     AstInterface fa(head);
     switch (t) {
     case ROSE: 
        ROSE_Analysis::BuildCFG(fa, head, graph);
        break;
     case OA:
        OpenAnalysis::BuildCFG(fa, head, graph);
        break;
     default:
       assert(false);
     }
  }
};

class TestCFGWrap_Text : public TestCFGWrap
{
   fstream f;
 public:
  TestCFGWrap_Text( AnalysisDomain _t, const string& fname) : TestCFGWrap(_t) 
   { 
     f.open(fname.c_str(),ios_base::out);
   }
  ~TestCFGWrap_Text() { f.close(); }

  void operator()(SgNode* head)
  {
     TestCFGWrap::operator()(head);
     f << GraphToString(graph);
  }
};

class TestCFGWrap_DOT : public TestCFGWrap
{
 public:
  TestCFGWrap_DOT( AnalysisDomain _t) : TestCFGWrap(_t) {}

  void operator()(SgNode* head, string fname)
  {
     TestCFGWrap::operator()(head);
     GraphDotOutput output(graph);
     output.writeToDOTFile(fname); 
  }
};


int
main ( int argc,  char * argv[] )
   {

     if (argc <= 1) {
         PrintUsage(argv[0]);
         return -1;
     }

     vector<string> argvList(argv, argv + argc);

     SgProject sageProject ( argvList);

    CmdOptions::GetInstance()->SetOptions(argvList);

   int filenum = sageProject.numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgFile &sageFile = sageProject.get_file(i);
     SgGlobal *root = sageFile.get_root();
     SgDeclarationStatementPtrList& declList = root->get_declarations ();
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) {
          SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
          if (func == 0)
             continue;
          SgFunctionDefinition *defn = func->get_definition();
          if (defn == 0)
             continue;
          SgNode* stmts = defn;
          TestCFGWrap::AnalysisDomain t = UseOA(argc, argv)? TestCFGWrap::OA : TestCFGWrap::ROSE;
          if (GenerateDOT(argc, argv)) {
             string name = string(strrchr(sageFile.getFileName().c_str(),'/')+1) + ".dot";
             TestCFGWrap_DOT op(t);
             op(stmts, name);
          }
          else {
             const char *p = sageFile.getFileName().c_str();
	     const char *p1 = strrchr(p, '/');
             if (p1 == 0)
                 p1 = p;
             else
                 ++p1;
             string name = string(p1) + ".out";
             TestCFGWrap_Text op(t,name);
             op(stmts);
          }
     }
   }

  return 0;
}


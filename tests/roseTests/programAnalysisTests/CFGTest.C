
#include <sage3.h>

#include <AstInterface_ROSE.h>
#include <string>
#include <iostream>
#include <CommandOptions.h>
#include <GraphIO.h>
#include <GraphDotOutput.h>
#include <CFGImpl.h>
//#define TEMPLATE_ONLY
//#include <CFG_OA.C>
//#undef TEMPLATE_ONLY

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
     AstInterfaceImpl scope(head);
     AstInterface fa(&scope);
     switch (t) {
     case ROSE: 
        ROSE_Analysis::BuildCFG(fa, AstNodePtrImpl(head), graph);
        break;
/*
     case OA:
        OpenAnalysis::BuildCFG(fa, AstNodePtrImpl(head), graph);
        break;
*/
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
     write_graph(graph, f, "edge");
     f << "\n";
  }
};

class TestCFGWrap_DOT : public TestCFGWrap
{
 public:
  TestCFGWrap_DOT( AnalysisDomain _t) : TestCFGWrap(_t) {}

  void operator()(SgNode* head, string fname)
  {
     TestCFGWrap::operator()(head);
     GraphDotOutput<DefaultCFGImpl> output(graph);
     output.writeToDOTFile(fname, "CFGGraph"); 
  }
};


int
main ( int argc,  char * argv[] )
   {

     if (argc <= 1) {
         PrintUsage(argv[0]);
         return -1;
     }

     SgProject sageProject ( (int)argc,argv);

     SageInterface::changeAllLoopBodiesToBlocks(&sageProject);

    CmdOptions::GetInstance()->SetOptions(argc, argv);

   int filenum = sageProject.numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
      SgSourceFile* sageFile = isSgSourceFile(sageProject.get_fileList()[i]);
      ROSE_ASSERT(sageFile != NULL);

      TestCFGWrap::AnalysisDomain t = UseOA(argc, argv)? TestCFGWrap::OA : TestCFGWrap::ROSE;
      //string txtname = string(strrchr(sageFile.getFileName(),'/')+1) + ".outx";
      string filename = sageFile->getFileName();
      string txtname = filename.substr(filename.rfind('/')+1) + ".outx"; 
      TestCFGWrap_Text txtop(t,txtname);
      //string dotname = string(strrchr(sageFile.getFileName(),'/')+1) + ".dot";
      string dotname = filename.substr(filename.rfind('/')+1) + ".dot";
      TestCFGWrap_DOT dotop(t);
     SgGlobal *root = sageFile->get_globalScope();
     SgDeclarationStatementPtrList& declList = root->get_declarations ();
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) {
          SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
          if (func == 0)
             continue;
          SgFunctionDefinition *defn = func->get_definition();
          if (defn == 0)
             continue;
          SgNode* stmts = defn;
          if (GenerateDOT(argc, argv)) {
             dotop(stmts, dotname);
          }
          else {
             txtop(stmts);
          }
     }
   }

  return 0;
}


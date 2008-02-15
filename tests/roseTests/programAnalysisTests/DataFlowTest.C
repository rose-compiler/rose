
#include <rose.h>
#include <general.h>

#include <GraphDotOutput.h>
#include <StmtInfoCollect.h>
#include <AstInterface.h>

#include <DefUseChain.h>
#include <string>
#include <iostream>
#include <CommandOptions.h>
#include <GraphIO.h>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
  cerr << "-dot :generate DOT output \n";
}

bool GenerateDOT( int argc,  char * argv[] )
{
  for (int i = 1; i < argc; ++i) {
    if ( argv[i] != 0 && !strcmp(argv[i], "-dot"))
        return true;
  }
  return false;
}

class TestDUWrap
{
  AliasAnalysisInterface& alias;
 protected:
  DefaultDUchain graph;
 public:
  TestDUWrap( AliasAnalysisInterface& a) : alias(a) {}
  void operator()(AstInterface& fa, SgNode* head)
  {
    ReachingDefinitionAnalysis r;
    r(fa, head);
    graph.build(fa, r, alias);
  }
};

class TestDUWrap_Text : public TestDUWrap
{
  fstream f;
 public:
  TestDUWrap_Text( AliasAnalysisInterface& a, const string& fname) : TestDUWrap(a) 
   {
     f.open(fname.c_str(),ios_base::out);
   }
   ~TestDUWrap_Text() { f.close(); }

  void operator()(AstInterface& fa, SgNode* head)
  {
     TestDUWrap::operator()(fa, head);
     f << GraphToString(graph);
  }
};

class TestDUWrap_DOT : public TestDUWrap
{
 public:
  TestDUWrap_DOT( AliasAnalysisInterface& a) : TestDUWrap(a) {}

  void operator()(AstInterface& fa, SgNode* head, string fname)
  {
     TestDUWrap::operator()(fa, head);
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
     SgProject sageProject (argvList);
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
          SgBasicBlock *stmts = defn->get_body();  
          AstInterface fa(stmts);
          StmtVarAliasCollect alias;
          alias(fa, defn);
          if (GenerateDOT(argc, argv)) {
             string name = string(strrchr(sageFile.getFileName().c_str(),'/')+1) + ".dot";
             TestDUWrap_DOT op(alias);
             op(fa, defn, name);
          }
          else {
             string name = string(strrchr(sageFile.getFileName().c_str(),'/')+1) + ".out";
             TestDUWrap_Text op(alias,name);
             op(fa, defn);
          }
     }
   }

  return 0;
}

#define TEMPLATE_ONLY
#include <DefUseChain.C>

// DQ (1/15/2007): This is an error on the Intel compiler!
// template class DefUseChain<DefUseChainNode>;

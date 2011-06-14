
#include <sage3.h>

#include <StmtInfoCollect.h>
#include <AstInterface_ROSE.h>

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
    r(fa, AstNodePtrImpl(head));
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
     write_graph(graph, f, "edge");
  }
};

class TestDUWrap_DOT : public TestDUWrap
{
 public:
  TestDUWrap_DOT( AliasAnalysisInterface& a) : TestDUWrap(a) {}

  void operator()(AstInterface& fa, SgNode* head, string fname)
  {
/*
     TestDUWrap::operator()(fa, head);
     GraphDotOutput output(graph);
     output.writeToDOTFile(fname); 
*/
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
     SgGlobal *root = sageFile->get_globalScope();
     SgDeclarationStatementPtrList& declList = root->get_declarations ();
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) {
          SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
          if (func == 0)
             continue;
          SgFunctionDefinition *defn = func->get_definition();
          if (defn == 0)
             continue;
          SgBasicBlock *stmts = defn->get_body();  
          AstInterfaceImpl scope(stmts);
          AstInterface fa(&scope);
          StmtVarAliasCollect alias;
          alias(fa, AstNodePtrImpl(defn));
          if (GenerateDOT(argc, argv)) {
             string name = string(strrchr(sageFile->getFileName().c_str(),'/')+1) + ".dot";
             TestDUWrap_DOT op(alias);
             op(fa, defn, name);
          }
          else {
             string name = string(strrchr(sageFile->getFileName().c_str(),'/')+1) + ".outx";
             TestDUWrap_Text op(alias,name);
             op(fa, defn);
          }
     }
   }

  return 0;
}


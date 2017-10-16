
#include <StmtInfoCollect.h>
#include <AstInterface_ROSE.h>

#include <SteensgaardPtrAnal.h>
#include <string>
#include <iostream>
#include <CommandOptions.h>
#include <GraphIO.h>
//do not include the following files from rose.h
#define CFG_ROSE_H
#define CONTROLFLOWGRAPH_H
#define PRE_H
#define ASTDOTGENERATION_TEMPLATES_C
#include <sage3.h>


void PrintUsage( char* name)
{
  std::cerr << name << "<program name>" << "\n";
}

class TestPtrAnal
{
  SteensgaardPtrAnal  op;
 public:
  void operator()(AstInterface& fa, SgNode* head)
  {
    op(fa, AstNodePtrImpl(head));
  }
  void output() { op.output(std::cout); }
};

int
main ( int argc,  char * argv[] )
   {

     if (argc <= 1) {
         PrintUsage(argv[0]);
         return -1;
     }

    SgProject sageProject ( argc,argv);
    CmdOptions::GetInstance()->SetOptions(argc, argv);


   TestPtrAnal op;
   int filenum = sageProject.numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgSourceFile* sageFile = isSgSourceFile(sageProject.get_fileList()[i]);
     ROSE_ASSERT(sageFile != NULL);
     SgGlobal *root = sageFile->get_globalScope();
     AstInterfaceImpl scope(root);
     AstInterface fa(&scope);
     SgDeclarationStatementPtrList& declList = root->get_declarations ();
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) {
          SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
          if (func == 0)
             continue;
          SgFunctionDefinition *defn = func->get_definition();
          if (defn == 0)
             continue;
          op(fa, defn);
     }
   }
  op.output();

  return 0;
}



#include <StmtInfoCollect.h>
#include <AstInterface_ROSE.h>

#include <PtrAnal.h>
#include <string>
#include <iostream>
#include <CommandOptions.h>
#include <GraphIO.h>
#include "ptr_anal_icfg_creator.h"
//do not include the following files from rose.h
#define CFG_ROSE_H
#define CONTROLFLOWGRAPH_H
#define PRE_H
#define ASTDOTGENERATION_TEMPLATES_C
#include <sage3.h>


extern	int DEBUG_ICFG;
extern	int DEBUG_STMT;

void PrintUsage( char* name)
{
  std::cerr << name << "<program name>" << "\n";
}

class PrintPtrAnalMap : public ProcessAstNode
{
   PtrAnal& m;
  public:
   PrintPtrAnalMap(PtrAnal& _m) : m(_m) {}
   virtual bool Traverse( AstInterface &fa, const AstNodePtr& _n,
                             AstInterface::TraversalVisitType t) 
  {
       AstNodePtr n = fa.IsExpression(_n); 
       if (n != AST_NULL) {
          PtrAnal::VarRef p = m.translate_exp(n);
          if (p.name != "") {
            std::cout << AstInterface::AstToString(n) << ":" << 
             ((long) p.stmt) << p.name << "\n"; 
          }
      }
      else if (fa.IsStatement(_n)) {
          PtrAnal::StmtRef p = m.translate_stmt(_n);
          if (p.size()) {
            std::cout << AstInterface::AstToString(_n) << ":" << 
             ((long) p.front()) << "->" << ((long)p.back()) << "\n"; 
          }
      }
       return true;
  }
};



int
main ( int argc,  char * argv[] )
   {
     if (argc <= 1) {
         PrintUsage(argv[0]);
         return -1;
     }
	DEBUG_ICFG = 1;
	DEBUG_STMT = 0;

    SgProject sageProject ( argc,argv);
    SageInterface::changeAllLoopBodiesToBlocks(&sageProject);
    CmdOptions::GetInstance()->SetOptions(argc, argv);


  // TestPtrAnal op;
  //
   ptr_Anal_ICFG_Creator op;
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
   op.draw("out.jpg");
  return 0;
}


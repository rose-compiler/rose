
#include <string>
#include <AstInterface_ROSE.h>
#include <StmtInfoCollect.h>
#include <CommandOptions.h>
//do not include the following files from rose.h
#define CFG_ROSE_H
#define CONTROLFLOWGRAPH_H
#define PRE_H
#define ASTDOTGENERATION_TEMPLATES_C
#include "sage3.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
}

class TestVarRefCollect : public CollectObject< pair<AstNodePtr,AstNodePtr> >
{
  string refs;
 public:
   virtual bool operator()( const pair<AstNodePtr,AstNodePtr>& var) 
      { refs = refs + " " + AstNodePtrImpl(var.first)->unparseToString(); return true;}
   void DumpOut( ostream& out) 
     { out << refs;  }
   void Clear() { refs = ""; }
};

class TestStmtModRef : public ProcessAstTree
{
  TestVarRefCollect mod, use, kill;
  StmtSideEffectCollect op;
  void Clear() { mod.Clear(); use.Clear(); kill.Clear(); }
 public:
  bool ProcessTree( AstInterface &fa, const AstNodePtr& s,
                       AstInterface::TraversalVisitType t)
  {
     if (t == AstInterface::PreVisit && fa.IsExecutableStmt(s)) {
         std::cout << AstNodePtrImpl(s)->unparseToString();
         std::cout << "\n";
         bool r = op ( fa, s, &mod, &use, &kill);
         std::cout << "modref: ";
         mod.DumpOut(std::cout);
         std::cout << " ;  readref: ";
         use.DumpOut(std::cout); 
         std::cout << " ;  killref: ";
         kill.DumpOut(std::cout); 
         std::cout << "\n";
         if (!r)
            std::cout << "Unknown \n";
         Clear();
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

     SgProject *sageProject = new SgProject( argc,argv);
     SageInterface::changeAllLoopBodiesToBlocks(sageProject);
    CmdOptions::GetInstance()->SetOptions(argc, argv);


   int filenum = sageProject->numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgSourceFile* sageFile = isSgSourceFile(sageProject->get_fileList()[i]);
     ROSE_ASSERT(sageFile != NULL);

     SgGlobal *root = sageFile->get_globalScope();
     TestStmtModRef op;
     AstInterfaceImpl scope(root);
     AstInterface fa(&scope);
     op( fa, AstNodePtrImpl(sageProject));
   }

  return 0;
}


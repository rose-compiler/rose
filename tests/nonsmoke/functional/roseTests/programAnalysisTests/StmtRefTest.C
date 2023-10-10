
#include "sage3.h"
#include <string>
#include <AstInterface_ROSE.h>
#include <StmtInfoCollect.h>
#include <CommandOptions.h>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
}

class TestVarRefCollect 
{
  string refs;
 public:
   virtual bool operator()( AstNodePtr var_first, AstNodePtr var_second) 
      { refs = refs + " " + AstNodePtrImpl(var_first)->unparseToString(); return true;}
   void DumpOut( ostream& out) 
     { out << refs;  }
   void Clear() { refs = ""; }
};

class TestStmtModRef : public ProcessAstTree<AstNodePtr>
{
  TestVarRefCollect mod, use, kill;
  StmtSideEffectCollect<AstNodePtr> op;
  void Clear() { mod.Clear(); use.Clear(); kill.Clear(); }
 public:
  TestStmtModRef(AstInterface& fa) : op(fa) {}
  bool ProcessTree( AstInterface &fa, const AstNodePtr& s,
                       AstInterface::TraversalVisitType t)
  {
     if (t == AstInterface::PreVisit && fa.IsExecutableStmt(s)) {
         std::function<bool(AstNodePtr,AstNodePtr)> mod_f(mod), use_f(use), kill_f(kill);
         std::cout << AstNodePtrImpl(s)->unparseToString();
         std::cout << "\n";
         bool r = op ( s, &mod_f, &use_f, &kill_f);
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

     // pmp 09JUN05
     //   gcc 3.4 does not allow cast from int to unsigned& anymore
     //   a surrugoate variable is introduced and used instead of argc.
     //   was: SgProject sageProject ( argc,argv);
     //        CmdOptions::GetInstance()->SetOptions((unsigned)argc, argv);
      
     SgProject sageProject ( argc,argv);
     SageInterface::changeAllBodiesToBlocks(&sageProject);
    CmdOptions::GetInstance()->SetOptions(argc, argv);


   int filenum = sageProject.numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgSourceFile* sageFile = isSgSourceFile(sageProject.get_fileList()[i]);
     ROSE_ASSERT(sageFile != NULL);

     SgGlobal *root = sageFile->get_globalScope();
     AstInterfaceImpl scope(root);
     AstInterface fa(&scope);
     TestStmtModRef op(fa);
     op( fa, AstNodePtrImpl(&sageProject));
   }

  return 0;
}


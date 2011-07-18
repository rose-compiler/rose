
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
  fstream out;
  void Clear() { mod.Clear(); use.Clear(); kill.Clear(); }
 public:
  TestStmtModRef(const string& fname)  
  {
     out.open(fname.c_str(),ios_base::out);
  }
  ~TestStmtModRef() { out.close(); }
  bool ProcessTree( AstInterface &fa, const AstNodePtr& s,
                       AstInterface::TraversalVisitType t)
  {
     if (t == AstInterface::PreVisit && fa.IsExecutableStmt(s)) {
         out << AstNodePtrImpl(s)->unparseToString();
         out << "\n";
         bool r = op ( fa, s, &mod, &use, &kill);
         out << "modref: ";
         mod.DumpOut(out);
         out << " ;  readref: ";
         use.DumpOut(out); 
         out << " ;  killref: ";
         kill.DumpOut(out); 
         out << "\n";
         if (!r)
            out << "Unknown \n";
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
     SageInterface::changeAllLoopBodiesToBlocks(&sageProject);
    CmdOptions::GetInstance()->SetOptions(argc, argv);


   int filenum = sageProject.numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgSourceFile* sageFile = isSgSourceFile(sageProject.get_fileList()[i]);
     ROSE_ASSERT(sageFile != NULL);

     SgGlobal *root = sageFile->get_globalScope();
     string name = string(strrchr(sageFile->getFileName().c_str(),'/')+1) + ".outx";
     TestStmtModRef op(name);
     AstInterfaceImpl scope(root);
     AstInterface fa(&scope);
     op( fa, AstNodePtrImpl(&sageProject));
   }

  return 0;
}


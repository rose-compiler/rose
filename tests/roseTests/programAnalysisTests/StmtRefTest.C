
#include <string>
#include "rose.h"
#include <StmtInfoCollect.h>
#include <CommandOptions.h>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#define Boolean int

void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
}

class TestVarRefCollect : public CollectObject< pair<AstNodePtr,AstNodePtr> >
{
  string refs;
 public:
   virtual Boolean operator()( const pair<AstNodePtr,AstNodePtr>& var) 
      { refs = refs + " " + var.first->unparseToString(); return true;}
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
  Boolean ProcessTree( AstInterface &fa, const AstNodePtr& s,
                       AstInterface::TraversalVisitType t)
  {
     if (t == AstInterface::PreVisit && fa.IsExecutableStmt(s)) {
         out << s->unparseToString();
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

     vector<string> argvList(argv, argv + argc);
      
     SgProject sageProject ( argvList);
    CmdOptions::GetInstance()->SetOptions(argvList);


   int filenum = sageProject.numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgFile &sageFile = sageProject.get_file(i);
     SgGlobal *root = sageFile.get_root();
     string name = string(strrchr(sageFile.getFileName().c_str(),'/')+1) + ".out";
     TestStmtModRef op(name);
     AstInterface fa(root);
     op( fa, &sageProject);
   }

  return 0;
}


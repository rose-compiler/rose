
#include <sage3.h>
#include <AstInterface_ROSE.h>
#include <CPPAstInterface.h>
#include <TestParallelLoop.h>
#include <string>
#include <iostream>
#include <CommandOptions.h>

using namespace std;

void PrintUsage( char* name)
{
  STD cerr << name << " <options> " << "<program name>" << "\n";
  TestParallelLoop::DumpOptions(STD cerr);
}

class TestParallelLoopWrap : public TransformAstTree
{
  TestParallelLoop &op;
  fstream f;
 public:
  TestParallelLoopWrap( TestParallelLoop &_op, const string& fname) : op(_op) 
   {
     f.open(fname.c_str(), ios_base::out);
   }
  ~TestParallelLoopWrap() { f.close(); }
  bool operator()(AstInterface& _fa,const AstNodePtr& _head, AstNodePtr& result)
  { 
    CPPAstInterface& fa = static_cast<CPPAstInterface&>(_fa);
    SgNode* head = AstNodePtrImpl(_head).get_ptr();
    if (head->variantT() == V_SgForStatement) {
       if (op.LoopParallelizable(fa, _head) )
          f << "parallelize loop " << head->unparseToString() << endl;
    }
    return false;
 }
};

int
main ( unsigned argc,  char * argv[] )
   {

     if (argc <= 1) {
         PrintUsage(argv[0]);
         return -1;
     }
     CmdOptions::GetInstance()->SetOptions(argc,argv);
     TestParallelLoop test(argc, argv);

     SgProject sageProject ( (int)argc,argv);

   int filenum = sageProject.numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgFile &sageFile = sageProject.get_file(i);
     string name = string(strrchr(sageFile.getFileName().c_str(),'/')+1) + ".out";
     TestParallelLoopWrap testWrap(test,name);

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
          AstInterfaceImpl scope(stmts);
          CPPAstInterface fa(&scope);
          TransformAstTraverse( fa, AstNodePtrImpl(stmts), testWrap);
     }
   }

  return 0;
}


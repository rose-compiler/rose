
#include <general.h>

#include "pre.h"
#include "finiteDifferencing.h"
#include "rose.h"

// DQ (1/2/2008): I think this is no longer used!
// #include "copy_unparser.h"

#include "rewrite.h"
#include <CommandOptions.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>

using namespace std;

void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
  cerr << "-gobj: generate object file\n";
  cerr << "-orig: copy non-modified statements from original file\n";
  cerr << "-splitloop: applying loop splitting to remove conditionals inside loops\n";
  cerr << ReadAnnotation::get_inst()->OptionString() << endl;
  cerr << "-pre:  apply partial redundancy elimination\n";
  cerr << "-fd:  apply finite differencing to array index expressions\n";
  PrintLoopTransformUsage( cerr );
}

bool GenerateObj()
{
  return CmdOptions::GetInstance()->HasOption("-gobj");
}

class AssumeNoAlias : public AliasAnalysisInterface
{
 public:
  virtual bool
     may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2)
   { return false; }
};


int
main ( int argc,  char * argv[] )
{

  if (argc <= 1) {
      PrintUsage(argv[0]);
      return -1;
  }
  vector<string> argvList(argv, argv + argc);
  CmdOptions::GetInstance()->SetOptions(argvList);
  SetLoopTransformOptions(argvList);

  OperatorSideEffectAnnotation *funcInfo = 
         OperatorSideEffectAnnotation::get_inst();
  funcInfo->register_annot();
  ReadAnnotation::get_inst()->read();
  AssumeNoAlias aliasInfo;

  SgProject sageProject ( argvList);

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
          AstInterface *fa = new AstInterface(stmts);
          LoopTransformTraverse( *fa, stmts, aliasInfo, funcInfo);
       // JJW 10-29-2007 Adjust for iterator invalidation and possible changes to declList
          p = std::find(declList.begin(), declList.end(), func);
          assert (p != declList.end());
     }
   }

   if (CmdOptions::GetInstance()->HasOption("-fd")) {
       simpleIndexFiniteDifferencing(&sageProject);
   }

   if (CmdOptions::GetInstance()->HasOption("-pre")) {
       PRE::partialRedundancyElimination(&sageProject);
   }

   unparseProject(&sageProject);
   if (GenerateObj())
      return sageProject.compileOutput();
   return 0;
}


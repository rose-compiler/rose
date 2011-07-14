#include "rose.h"
#include <general.h>

#include "pre.h"
#include "finiteDifferencing.h"


// DQ (1/2/2008): I think this is no longer used!
// #include "copy_unparser.h"

#include "rewrite.h"
#include <CommandOptions.h>
#include <AstInterface_ROSE.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>

using namespace std;

#ifdef USE_OMEGA
#include <DepTestStatistics.h>

extern DepTestStatistics DepStats;
#endif

extern bool DebugAnnot();
extern void FixFileInfo(SgNode* n);
class UnparseFormatHelp;
class UnparseDelegate;
void unparseProject( SgProject* project, UnparseFormatHelp* unparseHelp /*= NULL*/, UnparseDelegate *repl  /*= NULL */);

void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
  cerr << "-gobj: generate object file\n";
  cerr << "-orig: copy non-modified statements from original file\n";
  cerr << "-splitloop: applying loop splitting to remove conditionals inside loops\n";
  cerr << ReadAnnotation::get_inst()->OptionString() << endl;
  cerr << "-pre:  apply partial redundancy elimination\n";
  cerr << "-fd:  apply finite differencing to array index expressions\n";
  LoopTransformInterface::PrintTransformUsage( cerr );
}

bool GenerateObj()
{
  return CmdOptions::GetInstance()->HasOption("-gobj");
}


int
main ( int argc,  char * argv[] )
{

  if (argc <= 1) {
      PrintUsage(argv[0]);
      return -1;
  }
  vector<string> argvList(argv, argv + argc);
  CmdOptions::GetInstance()->SetOptions(argvList);
  AssumeNoAlias aliasInfo;
  LoopTransformInterface::cmdline_configure(argvList);
  LoopTransformInterface::set_aliasInfo(&aliasInfo);

#ifdef USE_OMEGA
  DepStats.SetFileName(buffer.str());
#endif

  OperatorSideEffectAnnotation *funcInfo = 
         OperatorSideEffectAnnotation::get_inst();
  funcInfo->register_annot();
  ReadAnnotation::get_inst()->read();
  if (DebugAnnot())
     funcInfo->Dump();
  LoopTransformInterface::set_sideEffectInfo(funcInfo);
  SgProject *project = new SgProject ( argvList);

   int filenum = project->numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
  // SgFile &sageFile = sageProject->get_file(i);
  // SgGlobal *root = sageFile.get_root();
     SgSourceFile* file = isSgSourceFile(project->get_fileList()[i]);
     SgGlobal *root = file->get_globalScope();
     SgDeclarationStatementPtrList& declList = root->get_declarations ();
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) {
          SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
          if (func == 0)
             continue;
          SgFunctionDefinition *defn = func->get_definition();
          if (defn == 0)
             continue;
          SgBasicBlock *stmts = defn->get_body();  
          AstInterfaceImpl faImpl = AstInterfaceImpl(stmts);
          LoopTransformInterface::TransformTraverse(faImpl, AstNodePtrImpl(stmts));

       // JJW 10-29-2007 Adjust for iterator invalidation and possible changes to declList
          p = std::find(declList.begin(), declList.end(), func);
          assert (p != declList.end());
     }
   }

   if (CmdOptions::GetInstance()->HasOption("-fd")) {
       simpleIndexFiniteDifferencing(project);
   }

   if (CmdOptions::GetInstance()->HasOption("-pre")) {
       PRE::partialRedundancyElimination(project);
   }
#ifdef USE_OMEGA
     DepStats.SetDepChoice(0x1 | 0x2 | 0x4);
     DepStats.PrintResults();
#endif
  //Qing's loop transformations are not robust enough to pass all tests.
   //AstTests::runAllTests(sageProject);
   unparseProject(project);
   if (GenerateObj())
      return project->compileOutput();
   return 0;
}


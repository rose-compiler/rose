
#include "sage3basic.h"
#include "unparser.h"
#include <CommandOptions.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <AstInterface_ROSE.h>
#include <AutoTuningInterface.h>

using namespace std;
extern bool DebugAnnot();

#ifdef USE_OMEGA
#include <DepTestStatistics.h>

extern DepTestStatistics DepStats;
#endif

extern void FixFileInfo(SgNode* n);
class UnparseFormatHelp;
class UnparseDelegate;
void unparseProject( SgProject* project, UnparseFormatHelp* unparseHelp /*= NULL*/, UnparseDelegate *repl  /*= NULL */);

void PrintUsage( const string& name)
{
  std::cerr << name << " <options> " << "<program name>" << "\n";
  std::cerr << "-orig: copy non-modified statements from original file\n";
  std::cerr << "-splitloop: applying loop splitting to remove conditionals inside loops\n";
  std::cerr << ReadAnnotation::get_inst()->OptionString() << std::endl;
//  std::cerr << "-inline: applying loop inlining for annotated functions\n";
//  std::cerr << "-pre:  apply partial redundancy elimination\n";
//  std::cerr << "-fd:  apply finite differencing to array index expressions\n";
  LoopTransformInterface::PrintTransformUsage( std::cerr );
}

int
main ( int argc,  char * argv[] )
{
  //init_poet();  // initialize poet

  if (argc <= 1) {
      PrintUsage(argv[0]);
      return -1;
  }

#ifdef USE_OMEGA
  std::stringstream buffer;
  buffer << argv[argc-1] << std::endl;
	
  DepStats.SetFileName(buffer.str());
#endif

  vector<string> argvList(argv, argv + argc);
  CmdOptions::GetInstance()->SetOptions(argvList);

  OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
  funcAnnot->register_annot();
  LoopTransformInterface::set_sideEffectInfo(funcAnnot);

  ReadAnnotation::get_inst()->read();
  if (DebugAnnot()) {
    funcAnnot->Dump();
  }

  AssumeNoAlias aliasInfo;
  LoopTransformInterface::set_aliasInfo(&aliasInfo);

  LoopTransformInterface::cmdline_configure(argvList);

  SgProject *sageProject = new SgProject ( argvList);
  FixFileInfo(sageProject);

  int filenum = sageProject->numberOfFiles();
  for (int i = 0; i < filenum; ++i) {
    SgSourceFile* sageFile = isSgSourceFile(sageProject->get_fileList()[i]);
    ROSE_ASSERT(sageFile != NULL);
    std::string fname = sageFile->get_file_info()->get_raw_filename();
    fname=fname.substr(fname.find_last_of('/')+1);
    AutoTuningInterface tuning(fname);
    LoopTransformInterface::set_tuningInterface(&tuning);
    SgGlobal *root = sageFile->get_globalScope();
    ROSE_ASSERT(root != NULL);

    SgDeclarationStatementPtrList declList = root->get_declarations ();

    for (SgDeclarationStatementPtrList::iterator p = declList.begin(); 
         p != declList.end(); ++p) {
      SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
      if (func == 0) continue;
      SgFunctionDefinition *defn = func->get_definition();
      if (defn == 0) continue;
      SgBasicBlock *stmts = defn->get_body();  
      AstInterfaceImpl scope(stmts);
      LoopTransformInterface::TransformTraverse(scope,AstNodePtrImpl(stmts));
    }
    tuning.GenOutput();
  }

//   if (CmdOptions::GetInstance()->HasOption("-fd")) {
//       simpleIndexFiniteDifferencing(sageProject);
//   }
//   if (CmdOptions::GetInstance()->HasOption("-pre")) {
//       partialRedundancyElimination(sageProject);
//   }

     unparseProject(sageProject);
   //backend(sageProject);

#ifdef USE_OMEGA
     DepStats.SetDepChoice(0x1 | 0x2 | 0x4);
     DepStats.PrintResults();
#endif

  return 0;
}


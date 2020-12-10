
#include <CommandOptions.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <ArrayAnnot.h>
#include <AstInterface_ROSE.h>
#include <ArrayInterface.h>
#include <AutoTuningInterface.h>
//do not include the following files from rose.h
#define CFG_ROSE_H
#define CONTROLFLOWGRAPH_H
#define PRE_H
#define ASTDOTGENERATION_TEMPLATES_C
#include "rose.h"
#include "unparser.h"

#include "AstDiagnostics.h"

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
  std::cerr << "-conserv: use conservative aliasing analysis instaed of assuming no aliasing\n";
  std::cerr << "-very_conserv: use conservative aliasing analysis and assume pointer arrays can overlap\n";
  std::cerr << ReadAnnotation::get_inst()->OptionString() << std::endl;
//  std::cerr << "-inline: applying loop inlining for annotated functions\n";
  LoopTransformInterface::PrintTransformUsage( std::cerr );
}

bool be_conservative() {
  CmdOptions *p = CmdOptions::GetInstance();
  return p->HasOption("-conserv") || p->HasOption("-very_conserv");
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

  ArrayAnnotation* array_annot = ArrayAnnotation::get_inst();
  array_annot->register_annot();

  //OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
  //funcAnnot->register_annot();
  LoopTransformInterface::set_sideEffectInfo(array_annot);

  ArrayInterface anal(*array_annot);
  LoopTransformInterface::set_arrayInfo(&anal);

  ReadAnnotation::get_inst()->read();
  if (DebugAnnot()) {
   // funcAnnot->Dump();
    array_annot->Dump();
  }

  AssumeNoAlias aliasInfo;
  if (be_conservative()) 
     LoopTransformInterface::set_aliasInfo(&anal);
  else 
     LoopTransformInterface::set_aliasInfo(&aliasInfo);

  LoopTransformInterface::cmdline_configure(argvList);

  SgProject *sageProject = new SgProject ( argvList);
  FixFileInfo(sageProject);

  // DQ (11/19/2013): Added AST consistency tests.
     AstTests::runAllTests(sageProject);

  int filenum = sageProject->numberOfFiles();
  for (int i = 0; i < filenum; ++i) {

  // DQ (11/19/2013): Added AST consistency tests.
     AstTests::runAllTests(sageProject);

    SgSourceFile* sageFile = isSgSourceFile(sageProject->get_fileList()[i]);
    ROSE_ASSERT(sageFile != NULL);

    std::string fname = sageFile->get_file_info()->get_raw_filename();
    fname=fname.substr(fname.find_last_of('/')+1);

    AutoTuningInterface tuning(fname);

    LoopTransformInterface::set_tuningInterface(&tuning);
    SgGlobal *root = sageFile->get_globalScope();
    ROSE_ASSERT(root != NULL);

    SgDeclarationStatementPtrList declList = root->get_declarations ();

 // DQ (11/19/2013): Added AST consistency tests.
    AstTests::runAllTests(sageProject);

    for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) 
    {
      SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
      if (func == 0) continue;
      SgFunctionDefinition *defn = func->get_definition();
      if (defn == 0) continue;
      AstNodePtrImpl head(defn);
      AstInterfaceImpl scope(defn);
      AstInterface fa(&scope);
      if (be_conservative())  anal.analyze(fa, head);
      SgBasicBlock *stmts = defn->get_body();  

   // DQ (11/19/2013): Added AST consistency tests.
      AstTests::runAllTests(sageProject);

      LoopTransformInterface::TransformTraverse(scope,AstNodePtrImpl(stmts));

#if 0
   // DQ (11/19/2013): Added AST consistency tests (this fails).
      AstTests::runAllTests(sageProject);
#endif
    }
    tuning.GenOutput();

#if 0
  // DQ (11/19/2013): Added AST consistency tests (this fails).
     AstTests::runAllTests(sageProject);
#endif
  }

//   if (CmdOptions::GetInstance()->HasOption("-fd")) {
//       simpleIndexFiniteDifferencing(sageProject);
//   }
//   if (CmdOptions::GetInstance()->HasOption("-pre")) {
//       partialRedundancyElimination(sageProject);
//   }

#if 0
  // DQ (11/19/2013): Added AST consistency tests (this fails).
     AstTests::runAllTests(sageProject);
#endif

     unparseProject(sageProject);
   //backend(sageProject);

#ifdef USE_OMEGA
     DepStats.SetDepChoice(0x1 | 0x2 | 0x4);
     DepStats.PrintResults();
#endif

  return 0;
}



//#include "pre.h"
//#include "finiteDifferencing.h"
#include "sage3.h"
#include <CommandOptions.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <AstInterface_ROSE.h>

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
  STD cerr << name << " <options> " << "<program name>" << "\n";
  STD cerr << "-gobj: generate object file\n";
  STD cerr << "-orig: copy non-modified statements from original file\n";
  STD cerr << "-splitloop: applying loop splitting to remove conditionals inside loops\n";
  STD cerr << ReadAnnotation::get_inst()->OptionString() << STD endl;
//  STD cerr << "-pre:  apply partial redundancy elimination\n";
//  STD cerr << "-fd:  apply finite differencing to array index expressions\n";
  PrintLoopTransformUsage( STD cerr );
}

bool GenerateObj()
{
  return CmdOptions::GetInstance()->HasOption("-gobj");
}

int
main ( int argc,  char * argv[] )
{
	int i;

	std::stringstream buffer;
	buffer << argv[argc-1] << std::endl;
	
  if (argc <= 1) {
      PrintUsage(argv[0]);
      return -1;
  }

  CmdOptions::GetInstance()->SetOptions(argc, argv);
  argc = SetLoopTransformOptions(argc, argv);

#ifdef USE_OMEGA
  DepStats.SetFileName(buffer.str());
#endif

  OperatorSideEffectAnnotation *funcInfo = 
         OperatorSideEffectAnnotation::get_inst();
  funcInfo->register_annot();
  ReadAnnotation::get_inst()->read();
  if (DebugAnnot())
     funcInfo->Dump();
  AssumeNoAlias aliasInfo;

  SgProject *sageProject = new SgProject ( argc,argv);
  FixFileInfo(sageProject);

   int filenum = sageProject->numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgFile &sageFile = sageProject->get_file(i);
     SgGlobal *root = sageFile.get_root();
     SgDeclarationStatementPtrList declList = root->get_declarations ();
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) {
          SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
          if (func == 0)
             continue;
          SgFunctionDefinition *defn = func->get_definition();
          if (defn == 0)
             continue;
          SgBasicBlock *stmts = defn->get_body();  
          AstInterfaceImpl scope(stmts);
          AstInterface fa(&scope);
          NormalizeForLoop(fa, AstNodePtrImpl(stmts));
          LoopTransformTraverse( fa, AstNodePtrImpl(stmts), aliasInfo, funcInfo);
     }
   }

//   if (CmdOptions::GetInstance()->HasOption("-fd")) {
//       simpleIndexFiniteDifferencing(sageProject);
//   }
//   if (CmdOptions::GetInstance()->HasOption("-pre")) {
//       partialRedundancyElimination(sageProject);
//   }

  //   unparseProject(sageProject);
   backend(sageProject);

#ifdef USE_OMEGA
     DepStats.SetDepChoice(0x1 | 0x2 | 0x4);
     DepStats.PrintResults();
#endif

//  if (GenerateObj())
 //    return sageProject->compileOutput();
  return 0;
}


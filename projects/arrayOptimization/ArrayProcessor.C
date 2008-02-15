
#include <sage3.h>
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>
#include <string>
#include <iostream>
#include <LoopTransformInterface.h>
#include <CommandOptions.h>
#include <AstInterface_ROSE.h>

void PrintUsage( char* name)
{
  STD cerr << name << " <options> " << "<program name>" << "\n";
  STD cerr << "-toarrayonly : rewrite to array form \n";
  STD cerr << ReadAnnotation::OptionString();
  PrintLoopTransformUsage(STD cerr);
}

bool GenerateObj()
{
  return CmdOptions::GetInstance()->HasOption("-gobj");
}

extern bool DebugAnnot();

bool ToArrayOnly( int argc,  char * argv[] )
{
  for (int i = 1; i < argc; ++i) {
    if ( argv[i] != 0 && !strcmp(argv[i], "-toarrayonly"))
        return true;
  }
  return false;
}

int
main ( unsigned argc,  char * argv[] )
   {

     if (argc <= 1) {
         PrintUsage(argv[0]);
         return -1;
     }
    CmdOptions::GetInstance()->SetOptions(argc, argv);
    argc = SetLoopTransformOptions( argc, argv);
    bool toOnly = ToArrayOnly(argc, argv);

    ArrayAnnotation* annot = ArrayAnnotation::get_inst();
    annot->register_annot();
    ReadAnnotation::get_inst()->read();
    if (DebugAnnot())
        annot->Dump();
    if (CmdOptions::GetInstance()->HasOption("-dumpannot"))
       annot->Dump();

    SgProject sageProject ( (int)argc,argv);
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

         SgBasicBlock* body = defn->get_body();
         AstInterfaceImpl scope(body);
         CPPAstInterface fa(&scope);

         OperatorInlineRewrite()( fa, AstNodePtrImpl(body));

         ArrayInterface anal(*annot);
         anal.initialize(fa, AstNodePtrImpl(defn));
         anal.observe(fa);

         RewriteToArrayAst toArray( anal);
         RewriteFromArrayAst fromArray( anal); 
         AstNodePtr r = TransformAstTraverse( fa, AstNodePtrImpl(body), toArray);
         fa.SetRoot(r);
         r = LoopTransformTraverse( fa, r, anal, annot, &anal);
         fa.SetRoot(r);
         if (!toOnly) {
             TransformAstTraverse( fa, AstNodePtrImpl(defn), fromArray);
          }
     }
   }

  // Generate the final C++ source code from the potentially modified SAGE AST
  if (!GenerateObj())
     sageProject.unparse();
  else
     return backend(&sageProject);
  return 0;
}


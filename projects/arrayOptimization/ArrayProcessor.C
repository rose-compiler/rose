
#include <sage3.h>
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>
#include <string>
#include <iostream>
#include <LoopTransformInterface.h>
#include <CommandOptions.h>
#include <AstInterface_ROSE.h>

using namespace std;

void PrintUsage( const string& name)
{
  std::cerr << name << " <options> " << "<program name>" << "\n";
  std::cerr << "-toarrayonly : rewrite to array form \n";
  std::cerr << ReadAnnotation::OptionString();
  PrintLoopTransformUsage(std::cerr);
}

bool GenerateObj()
{
  return CmdOptions::GetInstance()->HasOption("-gobj");
}

extern bool DebugAnnot();

bool ToArrayOnly( const vector<string>& argv )
{
  for (size_t i = 1; i < argv.size(); ++i) {
    if ( argv[i] == "-toarrayonly")
        return true;
  }
  return false;
}

int
main ( int argc,  char * argv[] )
   {

     if (argc <= 1) {
         PrintUsage(argv[0]);
         return 1;
     }
    vector<string> argvList(argv, argv + argc);
    SetLoopTransformOptions( argvList);
    CmdOptions::GetInstance()->SetOptions(argvList);
    bool toOnly = ToArrayOnly(argvList);

    ArrayAnnotation* annot = ArrayAnnotation::get_inst();
    annot->register_annot();
    ReadAnnotation::get_inst()->read();
    if (DebugAnnot())
        annot->Dump();
    if (CmdOptions::GetInstance()->HasOption("-dumpannot"))
       annot->Dump();

    SgProject* sageProject = new SgProject( argvList);
    int filenum = sageProject->numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
  // SgSourceFile &sageFile = isSgSourceFile(sageProject->get_fileList()[i]);
  // SgGlobal *root = sageFile.get_root();
     SgSourceFile* sageFile = isSgSourceFile(sageProject->get_fileList()[i]);
     SgGlobal *root = sageFile->get_globalScope();
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
     sageProject->unparse();
  else
     return backend(sageProject);
  return 0;
}


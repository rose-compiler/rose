
#include <rose.h>

// DQ (3/13/2006): If we have to have this at lease it is 
// not in the header files seen by ALL ROSE applications.
#define Boolean int

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include <general.h>
#include <AstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>
#include <string>
#include <iostream>
#include <LoopTransformInterface.h>
#include <CommandOptions.h>

void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
  cerr << "-toarrayonly : rewrite to array form \n";
  cerr << ReadAnnotation::OptionString();
  PrintLoopTransformUsage(cerr);
}

bool GenerateObj()
{
  return CmdOptions::GetInstance()->HasOption("-gobj");
}

bool ToArrayOnly( const vector<string>& argvList )
{
  for (unsigned int i = 1; i < argvList.size(); ++i) {
    if ( argvList[i] == "-toarrayonly")
        return true;
  }
  return false;
}

int
main ( int argc,  char * argv[] )
   {

     if (argc <= 1) {
         PrintUsage(argv[0]);
         return -1;
     }
    vector<string> argvList(argv, argv + argc);
    SetLoopTransformOptions( argvList);
    CmdOptions::GetInstance()->SetOptions(argvList);
    bool toOnly = ToArrayOnly(argvList);

    ArrayAnnotation* annot = ArrayAnnotation::get_inst();
    annot->register_annot();
    ReadAnnotation::get_inst()->read();

    if (CmdOptions::GetInstance()->HasOption("-dumpannot"))
       annot->Dump();

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

         SgBasicBlock* body = defn->get_body();
         AstInterface fa(body);

         OperatorInlineRewrite()( fa, body);

         ArrayInterface anal(*annot);
         anal.initialize(fa, defn);
         anal.observe(fa);

         RewriteToArrayAst toArray( anal);
         RewriteFromArrayAst fromArray( anal); 
         AstNodePtr r = TransformAstTraverse( fa, body, toArray);
         fa.SetRoot(r);
         r = LoopTransformTraverse( fa, r, anal, annot, &anal);
         fa.SetRoot(r);
         if (!toOnly) {
             TransformAstTraverse( fa, defn, fromArray);
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


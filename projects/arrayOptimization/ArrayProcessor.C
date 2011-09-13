#include "sage3basic.h"
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>
#include <string>
#include <iostream>
#include <LoopTransformInterface.h>
#include <CommandOptions.h>
#include <AstInterface_ROSE.h>

using namespace std;
using namespace CommandlineProcessing;

void PrintUsage( const string& name)
{
  std::cerr << name << " <options> " << "<program name>" << "\n";
  std::cerr << "-dumpannot: dump all annotations read from files \n";
  std::cerr << "-norawarray : don't rewrite to C-style raw array form \n";
  std::cerr << "-noobj: don't enable the backend compiler \n";
  std::cerr << ReadAnnotation::OptionString();
  LoopTransformInterface::PrintTransformUsage(std::cerr);
}

int
main ( int argc,  char * argv[] )
   {
     if (argc <= 1) {
         PrintUsage(argv[0]);
         return 1;
     }
    vector<string> argvList(argv, argv + argc);
   // Read and set loop transformation specific options, such as -fs2 
    CmdOptions::GetInstance()->SetOptions(argvList);
    // Handle and remove other custom options 
    bool noRawArray= isOption(argvList,"","-norawarray",true);
    bool dumpAnnot = isOption(argvList,"","-dumpannot",true);

    // Read into all annotation files 
    ArrayAnnotation* annot = ArrayAnnotation::get_inst();
    annot->register_annot();
    ReadAnnotation::get_inst()->read();
    if (dumpAnnot)    
       annot->Dump();
    LoopTransformInterface::set_sideEffectInfo(annot);
    LoopTransformInterface::cmdline_configure(argvList);

    // ROSE part
    SgProject* sageProject = new SgProject( argvList);
    int filenum = sageProject->numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgSourceFile* sageFile = isSgSourceFile(sageProject->get_fileList()[i]);
     SgGlobal *root = sageFile->get_globalScope();
     SgDeclarationStatementPtrList declList = root->get_declarations ();
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
         LoopTransformInterface::set_astInterface(fa);
 
         // Replace operators with their equivalent counterparts defined 
         // in "inline" annotations
         OperatorInlineRewrite()( fa, AstNodePtrImpl(body));
         
	 // Pass annotations to arrayInterface  
         ArrayInterface anal(*annot);
         anal.initialize(fa, AstNodePtrImpl(defn));
         anal.observe(fa);

         LoopTransformInterface::set_aliasInfo(&anal);
         LoopTransformInterface::set_arrayInfo(&anal);
 
	 // Write collective operations into explicit loops with 
	 // array element accesses using element access member functions
         RewriteToArrayAst toArray( anal);
         RewriteFromArrayAst fromArray( anal); 

         AstNodePtr r = TransformAstTraverse( fa, AstNodePtrImpl(body),toArray);
         fa.SetRoot(r);
         
	 /* Conduct loop transformation as requested */
         r = LoopTransformInterface::TransformTraverse(scope, r);
         fa.SetRoot(r);

         // Replace high level array class object reference with 
         // equivalent C-style raw array accesses
         if (!noRawArray) {
            RewriteFromArrayAst fromArray( anal); 
            TransformAstTraverse( fa, AstNodePtrImpl(defn), fromArray);
          }
     }
   }
  // Generate the final C++ source code from the potentially modified SAGE AST
  sageProject->unparse();
  return 0;
}


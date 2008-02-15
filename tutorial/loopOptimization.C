// LoopProcessor:
//   Assume no aliasing
//   apply loop opt to the bodies of all function definitions

// =====================================

#include "rose.h"

#include <AstInterface_ROSE.h>
#include "LoopTransformInterface.h"
#include "CommandOptions.h"

using namespace std;

int
main ( int argc,  char * argv[] )
   {
     CmdOptions::GetInstance()->SetOptions(argc, argv);
     SetLoopTransformOptions(argc, argv);
     AssumeNoAlias aliasInfo;

     vector<string> argvList(argv, argv + argc);
     SgProject project ( argvList);

  // Loop over the number of files in the project
     int filenum = project.numberOfFiles();
     for (int i = 0; i < filenum; ++i)
        {
          SgFile & file = project.get_file(i);
          SgGlobal *root = file.get_root();
          SgDeclarationStatementPtrList& declList = root->get_declarations ();

       // Loop over the declaration in the global scope of each file
          for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p)
             {
               SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
               if (func == NULL)
                    continue;
               SgFunctionDefinition *defn = func->get_definition();
               if (defn == NULL)
                    continue;

               SgBasicBlock *stmts = defn->get_body();
               AstInterfaceImpl faImpl(stmts);
               AstInterface fa(&faImpl);

            // This will do as much fusion as possible (finer grained 
            // control over loop optimizations uses a different interface).
               LoopTransformTraverse( fa, AstNodePtrImpl(stmts), aliasInfo);

            // JJW 10-29-2007 Adjust for iterator invalidation and possible
            // inserted statements
               p = std::find(declList.begin(), declList.end(), func);
               assert (p != declList.end());
             }
        }

  // Generate source code from AST and call the vendor's compiler
     return backend(&project);
   }


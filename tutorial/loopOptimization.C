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
     vector<string> argvList(argv, argv + argc);
     CmdOptions::GetInstance()->SetOptions(argvList);
     AssumeNoAlias aliasInfo;
     LoopTransformInterface::cmdline_configure(argvList);
     LoopTransformInterface::set_aliasInfo(&aliasInfo);

     SgProject* project = new SgProject(argvList);

  // Loop over the number of files in the project
     int filenum = project->numberOfFiles();
     for (int i = 0; i < filenum; ++i)
        {
          SgSourceFile* file = isSgSourceFile(project->get_fileList()[i]);
          SgGlobal *root = file->get_globalScope();
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

            // This will do as much fusion as possible (finer grained 
            // control over loop optimizations uses a different interface).
               LoopTransformInterface::TransformTraverse(faImpl, AstNodePtrImpl(stmts));

            // JJW 10-29-2007 Adjust for iterator invalidation and possible
            // inserted statements
               p = std::find(declList.begin(), declList.end(), func);
               assert (p != declList.end());
             }
        }

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }


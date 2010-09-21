// DQ (9/16/2010): This test code demonstrate how to insert a statement
// before and after a function in a file.  Important to this test code 
// is that we correctly handle the and CPP directives that might be 
// attached to the first function declaration.

#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int
main (int argc, char *argv[])
   {
     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

     SgGlobal* globalScope = getFirstGlobalScope (project);

     Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgFunctionDeclaration);

     Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin();
     while (i != functionDeclarationList.end())
        {
          SgFunctionDeclaration* functionDelcaration = isSgFunctionDeclaration(*i);
          SgName functionName = functionDelcaration->get_name();

#if 1
       // DQ (9/19/2010): Turn this off so that I can checkin UPC/UPC++ work.
          if (functionName == "removeThisFunctionToTestAttachedInfoBeforeStatement")
             {
               SageInterface::removeStatement(functionDelcaration);
             }
#endif
#if 0
       // DQ (9/19/2010): Turn this off so that I can checkin UPC/UPC++ work.
          if (functionName == "removeThisFunctionToTestAttachedInfoAfterStatement")
             {
               SageInterface::removeStatement(functionDelcaration);
             }
#endif

          i++;
        }

     return backend (project);
   }

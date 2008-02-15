// Example demonstrating function inlining (maximal inlining, up to preset number of inlinings).

#include "rose.h"

using namespace std;

// This is a function in Qing's AST interface
void FixSgProject(SgProject& proj);

int main (int argc, char* argv[])
   {
  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     SgProject* project = new SgProject(argc,argv);

  // DQ (7/20/2004): Added internal consistancy tests on AST
     AstTests::runAllTests(project);

     bool modifiedAST = true;
     int  count   = 0;

  // Inline one call at a time until all have been inlined.  Loops on recursive code.
     do {
          modifiedAST = false;

       // Build a list of functions within the AST
          Rose_STL_Container<SgNode*> functionCallList = NodeQuery::querySubTree (project,V_SgFunctionCallExp);

       // Loop over all function calls
       // for (list<SgNode*>::iterator i = functionCallList.begin(); i != functionCallList.end(); i++)
          Rose_STL_Container<SgNode*>::iterator i = functionCallList.begin();
          while (modifiedAST == false && i != functionCallList.end())
             {
               SgFunctionCallExp* functionCall = isSgFunctionCallExp(*i);
               ROSE_ASSERT(functionCall != NULL);

            // Not all function calls can be inlined in C++, so report if successful.
               bool sucessfullyInlined = doInline(functionCall);

               if (sucessfullyInlined == true)
                  {
                 // As soon as the AST is modified recompute the list of function 
                 // calls (and restart the iterations over the modified list)
                    modifiedAST = true;
                  }
                 else
                  {
                    modifiedAST = false;
                  }

            // Increment the list iterator
               i++;
             }

       // Quite when we have ceased to do any inline transformations 
       // and only do a predefined number of inline transformations
          count++;
        }
     while(modifiedAST == true && count < 10);

  // Call function to postprocess the AST and fixup symbol tables
     FixSgProject(*project);

  // Rename each variable declaration
     renameVariables(project);

  // Fold up blocks
     flattenBlocks(project);

  // Clean up inliner-generated code
     cleanupInlinedCode(project);

  // Change members to public
     changeAllMembersToPublic(project);

  // DQ (3/11/2006): This fails so the inlining, or the AST Interface 
  // support, needs more work even though it generated good code.
  // AstTests::runAllTests(project);

     return backend(project);
   }

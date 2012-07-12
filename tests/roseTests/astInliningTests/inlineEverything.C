#include "rose.h"

/* Visitor to find all function call expressions in the program */
std::vector<SgFunctionCallExp*> calls_to_inline;

class FindCallsVisitor: public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n)
             {
#if 0
            // Debugging code
               if (!isSgInitializedName(n))
                    n->unparseToString();
#endif
               SgFunctionCallExp* n2 = isSgFunctionCallExp(n);
               if (n2)
                  {
                    calls_to_inline.push_back(n2);
                  }
             }
   };

int main (int argc, char* argv[])
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     SgProject* sageProject = frontend(argc,argv);

  // DQ (7/20/2004): Added internal consistancy tests on AST
     AstTests::runAllTests(sageProject);

  // This is not needed here
  // FixSgProject(sageProject);

     bool changed = true;
     int  count   = 0;

  /* Inline one call at a time until all have been inlined.  Loops on recursive code. */
     while (changed)
        {
          changed = false;
          calls_to_inline.clear();
          FindCallsVisitor().traverseInputFiles(sageProject, preorder);

          for (std::vector<SgFunctionCallExp*>::iterator i = calls_to_inline.begin(); i != calls_to_inline.end(); ++i)
             {
            // cout << (*i)->unparseToString() << endl;
            // generateAstGraph(sageProject, 400000);
               if (doInline(*i))
                  {
                    changed = true;
                 // AstTests::runAllTests(sageProject);
                    break;
                  }
             }
          ++count;
#if 0
          sageProject.unparse();
#endif
       // To prevent infinite loops
          if (count == 10)
             {
               break;
             }
        }

#if 1

  // Rename each variable declaration
     renameVariables(sageProject);

#if 1
  // Fold up blocks
     flattenBlocks(sageProject);

  // Clean up inliner-generated code
     cleanupInlinedCode(sageProject);
#endif

  // Change members to public
     changeAllMembersToPublic(sageProject);

  // AstPDFGeneration().generateInputFiles(sageProject);
  // AstDOTGeneration().generateInputFiles(sageProject);
#endif

     AstTests::runAllTests(sageProject);

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
     generateDOT (*sageProject );
     // generateAstGraph(project, 2000);
#endif

#if 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(sageProject,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif


     return backend(sageProject);
   }

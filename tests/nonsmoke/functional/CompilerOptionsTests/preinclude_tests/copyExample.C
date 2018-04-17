
#include "rose.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

SgNode*
copyAST ( SgNode* node )
   {
     ROSE_ASSERT(node != NULL);

  // This is a better implementation using a derived class from SgCopyHelp to control the 
  // copying process (skipping the copy of any function definition).  This is a variable 
  // declaration with an explicitly declared class type.
     class RestrictedCopyType : public SgCopyHelp
        {
       // DQ (9/26/2005): This class demonstrates the use of the copy mechanism 
       // within Sage III (originally designed and implemented by Qing Yi).
       // One problem with it is that there is no context information permitted.

          public:
               virtual SgNode *copyAst(const SgNode *n)
                  {
                    SgNode *returnValue = n->copy(*this);
#if 0
                    switch(n->variantT())
                       {
                         case V_SgFunctionDefinition:
                            {
                              printf ("Skip copying the function definition if it is present \n");
                              returnValue = false;
                              break;
                            }
                         default:
                            {
                           // Nothing to do here
                            }
                       }
#endif
                    return returnValue;
                  }
        } restrictedCopyType;

     SgNode* copyOfNode = node->copy(restrictedCopyType);

     ROSE_ASSERT(copyOfNode != NULL);
     return copyOfNode;
   }


int
main ( int argc, char* argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

     if (SgProject::get_verbose() > 0)
          printf ("In preprocessor.C: main() \n");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#if 0
  // This fails for test2005_63.C but Rich has fixed this
  // by updating the pdf library we are using within ROSE.
     printf ("Generate the pdf output of the SAGE III AST \n");
     generatePDF ( *project );
#endif

#if 0
     printf ("Generate the dot output of the SAGE III AST \n");
     generateDOT ( *project );
#endif

#if 1
  // DQ (2/6/2004): These tests fail in Coco for test2004_14.C
     AstTests::runAllTests(const_cast<SgProject*>(project));
#else
     printf ("Skipped agressive (slow) internal consistancy tests! \n");
#endif

     if (project->get_verbose() > 0)
          printf ("Calling the AST copy mechanism \n");

  // Demonstrate the copying of the whole AST
     SgProject* newProject = static_cast<SgProject*>(copyAST(project));
     ROSE_ASSERT(newProject != NULL);

     printf ("Running tests on copy of AST \n");
     AstTests::runAllTests(newProject);

#if 1
  // DQ (8/17/2006): This is a test that causes parent's of types to be set 
  // (and there is an assertion against this in the set_parent() member 
  // function which fails).
     ROSE_ASSERT(project != NULL);
     list<SgNode*> declList = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
  // SgFunctionType* func_type2=isSgFunctionType(isSgFunctionDeclaration(*(declList.begin()))->get_type()->copy(treeCopy));
     SgTreeCopy treeCopy;
     list<SgNode*>::iterator i = declList.begin();
     while ( i != declList.end() )
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT(functionDeclaration != NULL);
          ROSE_ASSERT(functionDeclaration->get_type() != NULL);
          SgFunctionType* func_type2 = isSgFunctionType(functionDeclaration->get_type()->copy(treeCopy));
          i++;
        }
#endif

     if (project->get_verbose() > 0)
          printf ("Calling the backend() \n");

     int errorCode = 0;
     errorCode = backend(project);

  // DQ (7/7/2005): Only output the performance report if verbose is set (greater than zero)
     if (project->get_verbose() > 0)
        {
       // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
          AstPerformance::generateReport();
        }

  // printf ("Exiting with errorCode = %d \n",errorCode);
     return errorCode;
   }


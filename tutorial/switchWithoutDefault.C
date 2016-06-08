// Example ROSE Analysis to identify switch statments 
// without default case: used within ROSE/tutorial

#include "rose.h"

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversal::visit(SgNode* n)
   {
     SgSwitchStatement* switchStatement = isSgSwitchStatement(n);
     if (switchStatement != NULL)
        {
          SgStatementPtrList & cases = SageInterface::ensureBasicBlockAsBodyOfSwitch(switchStatement)->get_statements();
          bool switchHasDefault = false;

       // The default statement could be at any position in the list of cases.
          SgStatementPtrList::iterator i = cases.begin();
          while (i != cases.end())
             {
               SgDefaultOptionStmt* defaultCase = isSgDefaultOptionStmt(*i);
               if (defaultCase != NULL)
                  switchHasDefault = true;
               i++;
             }

          if (switchHasDefault == false)
             {
               printf ("Error (source code locations of switch without default case): \n");
               switchStatement->get_startOfConstruct()->display("Error: switch without default case");
             }
        }
   }

void
findSwitchWithoutDefault(SgProject* project)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST check for switch without default:");

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST check Main:");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

     findSwitchWithoutDefault(project);

  // if (project->get_verbose() > 0)
  //      printf ("Calling the backend() \n");

     int errorCode = 0;
  // errorCode = backend(project);

  // Just set the project, the report will be generated upon calling the destructor for "timer"
     timer.set_project(project);

     if (project->get_verbose() > 0)
        {
       // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
          AstPerformance::generateReport();
        }

  // return 0;
  // printf ("Exiting with errorCode = %d \n",errorCode);
     return errorCode;
   }



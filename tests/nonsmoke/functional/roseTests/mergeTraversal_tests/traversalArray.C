// This code test the use of traversals in series and as
// nested traversals.  It is also testing the ROSE timer mechanism.
// where they are nested and where they are in series.

#include "rose.h"

void
visit_node(SgNode* n)
   {
  // Function representing some trivial work on each IR node of the AST.
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
visit_initializedNames(SgNode* n)
   {
  // Function representing some trivial work on each IR node of the AST.
     SgInitializedName* initializedName = isSgInitializedName(n);
     if (initializedName != NULL)
        {
          static int length = 0;
          length += initializedName->get_name().get_length();
        }
   }


class visitorTraversal_1 : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void
visitorTraversal_1::visit(SgNode* n)
   {
     visit_node(n);
   }

void
test_1(SgProject* project)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST check #1:");

  // Build the traversal object
     visitorTraversal_1 exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);
   }

class visitorTraversal_2 : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversal_2::visit(SgNode* n)
   {
  // visit_node(n);
     visit_initializedNames(n);
   }

void
test_2(SgProject* project)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST check #2:");

  // Build the traversal object
     visitorTraversal_2 exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);
   }

class visitorTraversal_combined : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversal_combined::visit(SgNode* n)
   {
     visit_node(n);
  // visit_node(n);
     visit_initializedNames(n);
   }

void
test_combined(SgProject* project)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST check combined:");

  // Build the traversal object
     visitorTraversal_combined exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);
   }

void
test_repeated(SgProject* project)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST check repeated:");

  // Build the traversal object
     visitorTraversal_1 exampleTraversal_1;
     visitorTraversal_2 exampleTraversal_2;

  // Call the traversal starting at the project node of the AST
     exampleTraversal_1.traverseInputFiles(project,preorder);
     exampleTraversal_2.traverseInputFiles(project,preorder);
   }

int
main ( int argc, char* argv[] )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST check Main:");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Repeat these a few times to make sure that the times are representative
     test_1(project);
     test_2(project);
     test_1(project);
     test_2(project);

  // Repeat these a few times to make sure that the times are representative
     test_repeated(project);
     test_combined(project);
     test_repeated(project);
     test_combined(project);
     test_repeated(project);
     test_combined(project);

     int errorCode = 0;

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



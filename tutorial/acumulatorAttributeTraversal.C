// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

// Build an accumulator attribute, fancy name for what is essentially a global variable :-).
class AccumulatorAttribute
   {
     public:
          int loopCounter;

       // Specific constructors are optional
          AccumulatorAttribute () : loopCounter(0) {}
          AccumulatorAttribute ( const AccumulatorAttribute & X ) {}
          AccumulatorAttribute & operator= ( const AccumulatorAttribute & X ) { return *this; }
   };

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          static AccumulatorAttribute accumulatorAttribute;
          virtual void visit(SgNode* n);
   };

// declaration required for static data member
AccumulatorAttribute avisitorTraversal::accumulatorAttribute;

void visitorTraversal::visit(SgNode* n)
   {
     if (isSgForStatement(n) != NULL)
        {
          printf ("Found a for loop ... \n");
          accumulatorAttribute.loopCounter++;
        }
   }

int
main ( int argc, char* argv[] )
   {
     // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST 
  // can be specified to be preorder or postorder).
     exampleTraversal.traverseInputFiles(project,preorder);

     printf ("Number of loops = %d \n",exampleTraversal.accumulatorAttribute.loopCounter);

     return 0;
   }


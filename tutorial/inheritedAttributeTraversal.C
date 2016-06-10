// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class InheritedAttribute
   {
     public:
      // Depth in AST
         int depth;
         int maxLinesOfOutput;

       // Specific constructors are required
          InheritedAttribute (int x) : depth(x), maxLinesOfOutput(20) {};
          InheritedAttribute ( const InheritedAttribute & X ) : depth(X.depth), maxLinesOfOutput(20){};
   };

class visitorTraversal : public AstTopDownProcessing<InheritedAttribute>
   {
     public:
       // virtual function must be defined
          virtual InheritedAttribute evaluateInheritedAttribute(SgNode* n, InheritedAttribute inheritedAttribute);
   };

InheritedAttribute
visitorTraversal::evaluateInheritedAttribute(SgNode* n, InheritedAttribute inheritedAttribute)
   {
     static int linesOfOutput = 0;
     if (linesOfOutput++ < inheritedAttribute.maxLinesOfOutput)
          printf ("Depth in AST at %s = %d \n",n->sage_class_name(),inheritedAttribute.depth);

     return InheritedAttribute(inheritedAttribute.depth+1);
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // DQ (1/18/2006): Part of debugging
     SgFile & localFile = project->get_file(0);
     localFile.get_file_info()->display("localFile information");

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute(0);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,inheritedAttribute);

  // Or the traversal over all AST IR nodes can be called!
     exampleTraversal.traverse(project,inheritedAttribute);

     return 0;
   }


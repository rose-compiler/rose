// This example demonstrates the seeding of a specific type
// of bug (buffer overflow) into any existing application to 
// test bug finding tools.

#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

namespace SeedBugsArrayIndexing {

class InheritedAttribute
   {
     public:
          bool isLoop;
          bool isVulnerability;
          InheritedAttribute() : isLoop(false),isVulnerability(false) {}
          InheritedAttribute(const InheritedAttribute & X) : isLoop(X.isLoop),isVulnerability(X.isVulnerability) {}
   };

class BugSeeding : public SgTopDownProcessing<InheritedAttribute>
   {
     public:
          InheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode, 
             InheritedAttribute inheritedAttribute );
   };

InheritedAttribute
BugSeeding::evaluateInheritedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute )
   {
  // Use this if we only want to seed bugs in loops
     bool isLoop = inheritedAttribute.isLoop           ||
                   (isSgForStatement(astNode) != NULL) ||
                   (isSgWhileStmt(astNode) != NULL)    ||
                   (isSgDoWhileStmt(astNode) != NULL);
  // Add Fortran support
     isLoop = isLoop || (isSgFortranDo(astNode) != NULL);

  // Mark future noes in this subtree as being part of a loop
     inheritedAttribute.isLoop = isLoop;

  // To test this on simple codes, optionally allow it to be applied everywhere
     bool applyEveryWhere = true;

     if (isLoop == true || applyEveryWhere == true)
        {
       // The inherited attribute is true iff we are inside a loop and this is a SgPntrArrRefExp.
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);
          if (arrayReference != NULL)
             {
            // Mark as a vulnerability
               inheritedAttribute.isVulnerability = true;

            // Now change the array index (to seed the buffer overflow bug)
               SgVarRefExp* arrayVarRef = isSgVarRefExp(arrayReference->get_lhs_operand());
               ROSE_ASSERT(arrayVarRef != NULL);
               ROSE_ASSERT(arrayVarRef->get_symbol() != NULL);
               SgInitializedName* arrayName = isSgInitializedName(arrayVarRef->get_symbol()->get_declaration());
               ROSE_ASSERT(arrayName != NULL);
               SgArrayType* arrayType = isSgArrayType(arrayName->get_type());
               ROSE_ASSERT(arrayType != NULL);
               SgExpression* arraySize = arrayType->get_index();

               SgTreeCopy copyHelp;
            // Make a copy of the expression used to hold the array size in the array declaration.
               SgExpression* arraySizeCopy = isSgExpression(arraySize->copy(copyHelp));
               ROSE_ASSERT(arraySizeCopy != NULL);

            // This is the existing index expression
               SgExpression* indexExpression = arrayReference->get_rhs_operand();
               ROSE_ASSERT(indexExpression != NULL);

            // Build a new expression: "array[n]" --> "array[n+arraySizeCopy]", where the arraySizeCopy is a size of "array"
               SgExpression* newIndexExpression = buildAddOp(indexExpression,arraySizeCopy);

            // Substitute the new expression for the old expression
               arrayReference->set_rhs_operand(newIndexExpression);
             }
        }

     return inheritedAttribute;
   }
}

int
main (int argc, char *argv[])
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

     SeedBugsArrayIndexing::BugSeeding treeTraversal;
     SeedBugsArrayIndexing::InheritedAttribute inheritedAttribute;

     treeTraversal.traverseInputFiles (project,inheritedAttribute);

  // Running internal tests (optional)
     AstTests::runAllTests (project);

  // Output the new code seeded with a specific form of bug.
     return backend (project);
   }

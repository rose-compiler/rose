// This example demonstrates the seeding of a specific type
// of bug into an existing application to test bug finding 
// tools.

#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

// typedef bool InheritedAttribute;
// typedef bool SynthesizedAttribute;

class InheritedAttribute
   {
     public:
          bool isLoop;
          bool isVulnerability;

          InheritedAttribute() : isLoop(false),isVulnerability(false) {}
          InheritedAttribute(const InheritedAttribute & X) : isLoop(X.isLoop),isVulnerability(X.isVulnerability) {}
   };

#if 0
class SynthesizedAttribute
   {
  // Unclear what we want to save about the vulnerability, set
  // of SgFileNode objects, set of pointers to SgPntrArrRefExp
     public:
          bool isLoop;
          bool isVulnerability;

          SynthesizedAttribute() : isLoop(false),isVulnerability(false) {}
          SynthesizedAttribute(const SynthesizedAttribute & X) : isLoop(X.isLoop),isVulnerability(X.isVulnerability) {}
   };
#endif

class BugSeeding : public SgTopDownProcessing<InheritedAttribute>
// class BugSeeding : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
// class BugSeeding : public SgSimpleProcessing
   {
     public:
       // void visit (SgNode * astNode);
          InheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode, 
             InheritedAttribute inheritedAttribute );
#if 0
          SynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             InheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
#endif
   };

InheritedAttribute
BugSeeding::evaluateInheritedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute )
   {
     bool isLoop = inheritedAttribute.isLoop           ||
                   (isSgForStatement(astNode) != NULL) ||
                   (isSgWhileStmt(astNode) != NULL)    ||
                   (isSgDoWhileStmt(astNode) != NULL);

  // Add Fortran support
  // isLoop = isLoop || (isSgDoStatement(astNode) != NULL);

     inheritedAttribute.isLoop = isLoop;

     if (isLoop == true || true)
        {
       // The inherited attribute is true iff we are inside a loop and this is a SgPntrArrRefExp.
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);
          if (arrayReference != NULL)
             {
            // Mark as a vulnerability
               inheritedAttribute.isVulnerability = true;

            // Now change the array index (to seed the buffer overflow bug)
               SgExpression* lhs = arrayReference->get_rhs_operand();
               ROSE_ASSERT(lhs != NULL);
               SgVarRefExp* arrayVarRef = isSgVarRefExp(arrayReference->get_lhs_operand());
               ROSE_ASSERT(arrayVarRef != NULL);
               ROSE_ASSERT(arrayVarRef->get_symbol() != NULL);
               SgInitializedName* arrayName = isSgInitializedName(arrayVarRef->get_symbol()->get_declaration());
               ROSE_ASSERT(arrayName != NULL);
               SgArrayType* arrayType = isSgArrayType(arrayName->get_type());
               ROSE_ASSERT(arrayType != NULL);
               SgExpression* arraySize = arrayType->get_index();

               SgTreeCopy copyHelp;
               SgExpression* arraySizeCopy = isSgExpression(arraySize->copy(copyHelp));
               ROSE_ASSERT(arraySizeCopy != NULL);

            // SgExpression* rhs = buildIntVal(1);
               SgExpression* rhs = arraySizeCopy;
            // SgExpression* newIndexExpression = buildAddOp(arraySizeCopy,rhs);
               SgExpression* newIndexExpression = buildAddOp(lhs,rhs);
               arrayReference->set_rhs_operand(newIndexExpression);
             }
        }

     return inheritedAttribute;
   }

#if 0
SynthesizedAttribute
BugSeeding::evaluateSynthesizedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute,
     SynthesizedAttributesList childAttributes )
   {
     SynthesizedAttribute synthesizedAttribute;

     if (inheritedAttribute.isVulnerability == true)
        {
       // Fold up the list of child attributes using logical or, i.e. the local
       // result will be true iff one of the child attributes is true.
       // SynthesizedAttribute localResult = std::accumulate(childAttributes.begin(), childAttributes.end(),false, std::logical_or<bool>());

          bool isVulnerability = false;
          for (unsigned long i=0; i < childAttributes.size(); i++)
             {
               isVulnerability = isVulnerability || childAttributes[i].isVulnerability;
             }

          if (isVulnerability == true)
             {
               printf ("Found a subtree with  vulnerability ...\n");
             }
        }

     return synthesizedAttribute;
   }
#endif

int
main (int argc, char *argv[])
   {
     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

     BugSeeding treeTraversal;
     InheritedAttribute inheritedAttribute;

     treeTraversal.traverseInputFiles (project,inheritedAttribute);

  // Running interlan tests (optional)
     AstTests::runAllTests (project);

  // Output the new code seeded with a specific form of bug.
     return backend (project);
   }

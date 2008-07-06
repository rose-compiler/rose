// This is the source file just for the BufferOverFlowSecurityFlaw,
// other source files would be defined, one for each security flaw.

#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

#include "bugSeeding.h"


// **********************************************************************
//                      BufferOverFlowSecurityFlaw
// **********************************************************************

BufferOverFlowSecurityFlaw::BufferOverFlowSecurityFlaw()
   {
  // Build all the different types of vulnerabilities for this security flaw (could be many)
  // E.g. an array access in a loop body might be one, and an array access in an index 
  // expression (indirect addressing) might be another.  The point is that there can be many.
  // vulnerabilityKindList.push_back(new BufferOverflowVulnerability());
  // SecurityFlaw::Vulnerability* vulnerability = new BufferOverflowVulnerability_InLoop();
  // vulnerabilityKindList.push_back(vulnerability);

     vulnerabilityKindList.push_back(new BufferOverflowVulnerability_InLoop());
     vulnerabilityKindList.push_back(new BufferOverflowVulnerability_ExternalToLoop());

  // Build all the different types of seeding techniques for this security flaw (could be many)
  // Not that the SeedSecurityFlaw object contain information and functionality required to
  // construct the clones.  Each clone is built at a specific level of grainularity as required
  // to support the security flaw seeding transformation.  E.g. a buffer overflow that appears
  // as an array write in a loop might be seeded by adjusting the loop bounds, to support this
  // the grainularity of the cloning operation must be at least that of the loop level grainularity
  // or greater.  Alternatively, to seed the buffer overflow via an array subscript manipulation
  // we only need to clone the array expression (the finest possible level of grainularity).
  // seedKindList.push_back(new SeedBufferOverflowSecurityFlaw());
  // SecurityFlaw::SeedSecurityFlaw* flaw = new SeedBufferOverflowSecurityFlaw();
  // seedKindList.push_back(flaw);

     seedKindList.push_back(new SeedBufferOverflowSecurityFlaw_ModifyArrayIndex());
#if 1
     seedKindList.push_back(new SeedBufferOverflowSecurityFlaw_AlterLoopBounds());
#endif
   }

BufferOverFlowSecurityFlaw::~BufferOverFlowSecurityFlaw()
   {
  // Nothing to do here!
   }

// **********************************************************************
//              BufferOverFlowSecurityFlaw_InLoop::Vulnerability
// **********************************************************************


void
BufferOverFlowSecurityFlaw::BufferOverflowVulnerability_InLoop::detector( SgProject *project )
   {
     ROSE_ASSERT (project != NULL);

  // Build an AST traversal object
     Traversal treeTraversal(this);

  // Build the initial inherited attribute
     InheritedAttribute inheritedAttribute;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
     treeTraversal.traverseInputFiles (project,inheritedAttribute);
   }

BufferOverFlowSecurityFlaw::BufferOverflowVulnerability_InLoop::Traversal::Traversal( SecurityFlaw::Vulnerability* vulnerabilityPointer )
   : vulnerabilityPointer(vulnerabilityPointer)
   {
   }

BufferOverFlowSecurityFlaw::BufferOverflowVulnerability_InLoop::InheritedAttribute
BufferOverFlowSecurityFlaw::BufferOverflowVulnerability_InLoop::Traversal::evaluateInheritedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute )
   {
  // This is the function that detects the vulnerability

  // Use this if we only want to seed bugs in loops
     bool isLoop = inheritedAttribute.isLoop           ||
                   (isSgForStatement(astNode) != NULL) ||
                   (isSgWhileStmt(astNode) != NULL)    ||
                   (isSgDoWhileStmt(astNode) != NULL);

  // Added Fortran support (additional language support is easy)
     isLoop = isLoop || (isSgFortranDo(astNode) != NULL);

  // Mark future nodes in this subtree as being part of a loop
     inheritedAttribute.isLoop = isLoop;

     if (inheritedAttribute.isLoop == true)
        {
       // The inherited attribute is true iff we are inside a loop and this is a SgPntrArrRefExp.
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);
          if (arrayReference != NULL)
             {
            // Place an attribute at this IR node to mark it as a vulnerability.
               printf ("Found a buffer overflow vulnerability in a loop \n");

            // Build an attribute (on the heap)
               AstAttribute* newAttribute = new SecurityVulnerabilityAttribute(astNode,vulnerabilityPointer);
               ROSE_ASSERT(newAttribute != NULL);

            // We need to name the attributes, but all the VulnerabilityAttributes can all have the same name.
            // It is easier to distinquish them by value (stored internally in the attribute). The use of
            // names for attributes permits other forms of analysis to use attributes in the same AST and
            // for all the forms of analysis to co-exist (so long as the select unique names).
               astNode->addNewAttribute("SecurityVulnerabilityAttribute",newAttribute);
             }
        }

     return inheritedAttribute;
   }




// **********************************************************************
//              BufferOverFlowSecurityFlaw_ExternalToLoop::Vulnerability
// **********************************************************************

void
BufferOverFlowSecurityFlaw::BufferOverflowVulnerability_ExternalToLoop::detector( SgProject *project )
   {
     ROSE_ASSERT (project != NULL);

  // Build an AST traversal object
     Traversal treeTraversal(this);

  // Build the initial inherited attribute
     InheritedAttribute inheritedAttribute;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
     treeTraversal.traverseInputFiles (project,inheritedAttribute);
   }

BufferOverFlowSecurityFlaw::BufferOverflowVulnerability_ExternalToLoop::Traversal::Traversal( SecurityFlaw::Vulnerability* vulnerabilityPointer )
   : vulnerabilityPointer(vulnerabilityPointer)
   {
   }

BufferOverFlowSecurityFlaw::BufferOverflowVulnerability_ExternalToLoop::InheritedAttribute
BufferOverFlowSecurityFlaw::BufferOverflowVulnerability_ExternalToLoop::Traversal::evaluateInheritedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute )
   {
  // This is the function that detects the vulnerability

  // Use this if we only want to seed bugs in loops
     bool isLoop = inheritedAttribute.isLoop           ||
                   (isSgForStatement(astNode) != NULL) ||
                   (isSgWhileStmt(astNode) != NULL)    ||
                   (isSgDoWhileStmt(astNode) != NULL);

  // Added Fortran support (additional language support is easy)
     isLoop = isLoop || (isSgFortranDo(astNode) != NULL);

  // Mark future nodes in this subtree as being part of a loop
     inheritedAttribute.isLoop = isLoop;

     if (inheritedAttribute.isLoop == false)
        {
       // The inherited attribute is true iff we are inside a loop and this is a SgPntrArrRefExp.
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);
          if (arrayReference != NULL)
             {
            // Place an attribute at this IR node to mark it as a vulnerability.
               printf ("Found a buffer overflow vulnerability external to a loop \n");

            // Build an attribute (on the heap)
               AstAttribute* newAttribute = new SecurityVulnerabilityAttribute(astNode,vulnerabilityPointer);
               ROSE_ASSERT(newAttribute != NULL);

            // We need to name the attributes, but all the VulnerabilityAttributes can all have the same name.
            // It is easier to distinquish them by value (stored internally in the attribute). The use of
            // names for attributes permits other forms of analysis to use attributes in the same AST and
            // for all the forms of analysis to co-exist (so long as the select unique names).
               astNode->addNewAttribute("SecurityVulnerabilityAttribute",newAttribute);
             }
        }

     return inheritedAttribute;
   }




// ***************************************************************************
// BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_ModifyArrayIndex
// ***************************************************************************

BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_ModifyArrayIndex::SeedBufferOverflowSecurityFlaw_ModifyArrayIndex()
   {
   }

BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_ModifyArrayIndex::~SeedBufferOverflowSecurityFlaw_ModifyArrayIndex()
   {
   }


void
BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_ModifyArrayIndex::seed( SgNode *astNode )
   {
     ROSE_ASSERT (astNode != NULL);

  // Build an AST traversal object
     SeedTraversal treeTraversal;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
     treeTraversal.traverse(astNode,preorder);
   }

void
BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_ModifyArrayIndex::SeedTraversal::visit( SgNode* astNode )
   {
  // This function is most likely to be called on a copy, in which case it should find the 
  // SecurityVulnerabilityAttribute (copied by the AST copy mechanism) and then implement
  // the transformation.  This is the reason for the recent change to ROSE to have the
  // AST copy mechanism be fixed to also copy AST persistant attributes.

     ROSE_ASSERT(astNode != NULL);

     AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
     SecurityVulnerabilityAttribute* securityVulnerabilityAttribute = dynamic_cast<SecurityVulnerabilityAttribute*>(existingAttribute);

  // if (securityVulnerabilityAttribute != NULL && securityVulnerabilityAttribute->get_value() == 5)
     if (securityVulnerabilityAttribute != NULL)
        {
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);

       // We need to recurse down to where the lhs is finally a SgVarRefExp
          std::vector<SgPntrArrRefExp*> arrayReferenceStack;
          SgArrayType* arrayType = NULL;
          while (arrayReference != NULL && arrayType == NULL)
             {
               arrayReferenceStack.push_back(arrayReference);

               SgVarRefExp* arrayVarRef = isSgVarRefExp(arrayReference->get_lhs_operand());
               if (arrayVarRef != NULL)
                  {
                    ROSE_ASSERT(arrayVarRef->get_symbol() != NULL);
                    SgInitializedName* arrayName = isSgInitializedName(arrayVarRef->get_symbol()->get_declaration());
                    ROSE_ASSERT(arrayName != NULL);
                 // printf ("arrayName->get_name() = %s \n",arrayName->get_name().str());
                    arrayType = isSgArrayType(arrayName->get_type());
                    ROSE_ASSERT(arrayType != NULL);
                  }
                 else
                  {
                 // printf ("Recursing one more level into the array indexing... \n");
                    arrayReference = isSgPntrArrRefExp(arrayReference->get_lhs_operand());
                    ROSE_ASSERT(arrayReference != NULL);
                  }
             }

          ROSE_ASSERT(arrayType != NULL);
          while (arrayReferenceStack.empty() == false)
             {
               ROSE_ASSERT(arrayType->get_base_type() != NULL);
            // printf ("arrayReferenceStack.size() = %ld arrayType->get_base_type() = %p = %s \n",arrayReferenceStack.size(),arrayType->get_base_type(),arrayType->get_base_type()->class_name().c_str());

               arrayReference = arrayReferenceStack.back();

               if (arrayType != NULL && arrayReference == isSgPntrArrRefExp(astNode))
                  {
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

                 // Subsitute the new expression for the old expression
                    arrayReference->set_rhs_operand(newIndexExpression);
                  }

               ROSE_ASSERT(arrayType->get_base_type() != NULL);
               arrayType = isSgArrayType(arrayType->get_base_type());

               arrayReferenceStack.pop_back();
             }

       // printf ("At base of BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw::SeedTraversal::visit()! \n");

          addComment (astNode,"// *** NOTE Seeded Security Flaw: BufferOverFlowSecurityFlaw ");

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
   }

// **************************************************************************
// BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_AlterLoopBounds
// **************************************************************************

BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_AlterLoopBounds::SeedBufferOverflowSecurityFlaw_AlterLoopBounds()
   {
   }

BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_AlterLoopBounds::~SeedBufferOverflowSecurityFlaw_AlterLoopBounds()
   {
   }


void
BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_AlterLoopBounds::seed( SgNode *astNode )
   {
     ROSE_ASSERT (astNode != NULL);

  // Build an AST traversal object
     SeedTraversal treeTraversal;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
     treeTraversal.traverse(astNode,preorder);
   }

void
BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_AlterLoopBounds::SeedTraversal::visit( SgNode* astNode )
   {
  // This function is most likely to be called on a copy, in which case it should find the 
  // SecurityVulnerabilityAttribute (copied by the AST copy mechanism) and then implement
  // the transformation.  This is the reason for the recent change to ROSE to have the
  // AST copy mechanism be fixed to also copy AST persistant attributes.

     ROSE_ASSERT(astNode != NULL);

     AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
     SecurityVulnerabilityAttribute* securityVulnerabilityAttribute = dynamic_cast<SecurityVulnerabilityAttribute*>(existingAttribute);

  // if (securityVulnerabilityAttribute != NULL && securityVulnerabilityAttribute->get_value() == 5)
     if (securityVulnerabilityAttribute != NULL)
        {
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);

       // We need to recurse down to where the lhs is finally a SgVarRefExp
          std::vector<SgPntrArrRefExp*> arrayReferenceStack;
          SgArrayType* arrayType = NULL;
          while (arrayReference != NULL && arrayType == NULL)
             {
               arrayReferenceStack.push_back(arrayReference);

               SgVarRefExp* arrayVarRef = isSgVarRefExp(arrayReference->get_lhs_operand());
               if (arrayVarRef != NULL)
                  {
                    ROSE_ASSERT(arrayVarRef->get_symbol() != NULL);
                    SgInitializedName* arrayName = isSgInitializedName(arrayVarRef->get_symbol()->get_declaration());
                    ROSE_ASSERT(arrayName != NULL);
                 // printf ("arrayName->get_name() = %s \n",arrayName->get_name().str());
                    arrayType = isSgArrayType(arrayName->get_type());
                    ROSE_ASSERT(arrayType != NULL);
                  }
                 else
                  {
                 // printf ("Recursing one more level into the array indexing... \n");
                    arrayReference = isSgPntrArrRefExp(arrayReference->get_lhs_operand());
                    ROSE_ASSERT(arrayReference != NULL);
                  }
             }

          ROSE_ASSERT(arrayType != NULL);
          while (arrayReferenceStack.empty() == false)
             {
               ROSE_ASSERT(arrayType->get_base_type() != NULL);
            // printf ("arrayReferenceStack.size() = %ld arrayType->get_base_type() = %p = %s \n",arrayReferenceStack.size(),arrayType->get_base_type(),arrayType->get_base_type()->class_name().c_str());

               arrayReference = arrayReferenceStack.back();

               if (arrayType != NULL && arrayReference == isSgPntrArrRefExp(astNode))
                  {
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

                 // Subsitute the new expression for the old expression
                    arrayReference->set_rhs_operand(newIndexExpression);
                  }

               ROSE_ASSERT(arrayType->get_base_type() != NULL);
               arrayType = isSgArrayType(arrayType->get_base_type());

               arrayReferenceStack.pop_back();
             }

       // printf ("At base of BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw::SeedTraversal::visit()! \n");

          addComment (astNode,"// *** NOTE Seeded Security Flaw: BufferOverFlowSecurityFlaw ");

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
   }


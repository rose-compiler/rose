// This is the source file just for the BufferOverFlowSecurityFlaw,
// other source files would be defined, one for each security flaw.

#include "rose.h"

using namespace rose;
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

  // Seeding approaches
     SeedBufferOverflowSecurityFlaw_ModifyArrayIndex* seedModifyIndex     = new SeedBufferOverflowSecurityFlaw_ModifyArrayIndex();
     SeedBufferOverflowSecurityFlaw_AlterLoopBounds* seedModifyLoopBounds = new SeedBufferOverflowSecurityFlaw_AlterLoopBounds();

     seedKindList.push_back(seedModifyIndex);
     seedKindList.push_back(seedModifyLoopBounds);

  // Vulnerability #1
     BufferOverflowVulnerability_ExternalToLoop* vulnerabilityExternalToLoop = new BufferOverflowVulnerability_ExternalToLoop();
     vulnerabilityKindList.push_back(vulnerabilityExternalToLoop);

  // Vulerability #1 can use seeding apprached #1
     vulnerabilityExternalToLoop->associateSeeding(seedModifyIndex);

#if 1
  // To simplify the debugging, we can comment this part out

  // Vulnerability #2
     BufferOverflowVulnerability_InLoop* vulnerabilityInLoop = new BufferOverflowVulnerability_InLoop();
     vulnerabilityKindList.push_back(vulnerabilityInLoop);

  // Vulnerability #2 can use seeding apprached #1 and #2
     vulnerabilityInLoop->associateSeeding(seedModifyIndex);
     vulnerabilityInLoop->associateSeeding(seedModifyLoopBounds);
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
            // printf ("Found a buffer overflow vulnerability in a loop \n");

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

BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_ModifyArrayIndex::SeedTraversal::SeedTraversal(SecurityFlaw::SeedSecurityFlaw* seedingSecurityFlaw )
   : seedingSecurityFlaw(seedingSecurityFlaw)
   {
   }

void
BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_ModifyArrayIndex::seed( SgNode *astNode )
   {
     ROSE_ASSERT (astNode != NULL);

  // Build an AST traversal object
     SeedTraversal treeTraversal(this);

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

     AstAttribute* existingAttribute = astNode->getAttribute("PrimarySecurityVulnerabilityForCloneAttribute");
     PrimarySecurityVulnerabilityForCloneAttribute* securityVulnerabilityAttribute = dynamic_cast<PrimarySecurityVulnerabilityForCloneAttribute*>(existingAttribute);

  // Make sure this is the set of security flaws that is intended to use this seeding approach.
     ROSE_ASSERT(securityVulnerabilityAttribute == NULL || securityVulnerabilityAttribute->vulnerabilityPointer != NULL);
     bool isAnAssociatedVulnerability = securityVulnerabilityAttribute != NULL && 
          ( securityVulnerabilityAttribute->vulnerabilityPointer->associatedSeedingTechniques.find(seedingSecurityFlaw) != 
            securityVulnerabilityAttribute->vulnerabilityPointer->associatedSeedingTechniques.end());

     if (isAnAssociatedVulnerability == true)
        {
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);

       // The following code is unexpectedly complex because we have to handle multi-dimensional arrays.

       // We need to recurse down to where the lhs is finally a SgVarRefExp (so that we can get the 
       // array name, so that we can get the type information for the array (then we will travrse the 
       // type information to get the array bounds for each dimension.
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
                 // printf ("Recursing one more level into the multi-dimensional array indexing... \n");
                    arrayReference = isSgPntrArrRefExp(arrayReference->get_lhs_operand());
                    ROSE_ASSERT(arrayReference != NULL);
                  }
             }

          ROSE_ASSERT(arrayType != NULL);

       // Now recurse back up the stack to do the actual transformations 
          while (arrayReferenceStack.empty() == false)
             {
               ROSE_ASSERT(arrayType->get_base_type() != NULL);
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
#if 1
       // printf ("Adding comment: NOTE Seeded Security Flaw: BufferOverFlowSecurityFlaw_ModifyArrayIndex to node %p \n",astNode);
          addComment (astNode,std::string("*** NOTE Seeded Security Flaw: BufferOverFlowSecurityFlaw_ModifyArrayIndex : ") + StringUtility::numberToString(astNode) + " : " + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );
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

BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_AlterLoopBounds::SeedTraversal::SeedTraversal(SecurityFlaw::SeedSecurityFlaw* seedingSecurityFlaw )
   : seedingSecurityFlaw(seedingSecurityFlaw)
   {
   }

void
BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw_AlterLoopBounds::seed( SgNode *astNode )
   {
     ROSE_ASSERT (astNode != NULL);

  // Build an AST traversal object
     SeedTraversal treeTraversal(this);

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

     AstAttribute* existingAttribute = astNode->getAttribute("PrimarySecurityVulnerabilityForCloneAttribute");
     PrimarySecurityVulnerabilityForCloneAttribute* securityVulnerabilityAttribute = dynamic_cast<PrimarySecurityVulnerabilityForCloneAttribute*>(existingAttribute);

  // Make sure this is the set of security flaws that is intended to use this seeding approach.
     ROSE_ASSERT(securityVulnerabilityAttribute == NULL || securityVulnerabilityAttribute->vulnerabilityPointer != NULL);
     bool isAnAssociatedVulnerability = securityVulnerabilityAttribute != NULL &&
          ( securityVulnerabilityAttribute->vulnerabilityPointer->associatedSeedingTechniques.find(seedingSecurityFlaw) != 
            securityVulnerabilityAttribute->vulnerabilityPointer->associatedSeedingTechniques.end() );

     if (isAnAssociatedVulnerability == true)
        {
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);

       // printf ("Transforming array expression on line = %d \n",arrayReference->get_file_info()->get_line());
       // printf ("vulnerability name = %s \n",securityVulnerabilityAttribute->vulnerabilityPointer->get_name().c_str());

          SgStatement* associatedStatement = TransformationSupport::getStatement(astNode);

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
               arrayReference = arrayReferenceStack.back();

            // Look only for the SgPntrArrRefExp that matches the IR node that we are visiting!
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

                 // Now look for the test in the loop so that it can be transformed.
                 //    1) get the list of variables in the index expression
                    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree ( indexExpression, V_SgVarRefExp );
                    int nodeListSize = nodeList.size();
                    if (nodeListSize == 1)
                       {
                      // printf ("This dimension of the array access uses a single variable \n");
                         SgVarRefExp* varRefExp = isSgVarRefExp(nodeList[0]);
                         ROSE_ASSERT(varRefExp != NULL);
                         bool isLoop = false;
                         SgScopeStatement* outerScope = associatedStatement->get_scope();
                         while (isLoop == false)
                            {
                              isLoop = (isSgForStatement(outerScope) != NULL) ||
                                       (isSgWhileStmt(outerScope) != NULL)    ||
                                       (isSgDoWhileStmt(outerScope) != NULL);

                           // Added Fortran support (additional language support is easy)
                              isLoop = isLoop || (isSgFortranDo(outerScope) != NULL);

                              if (isLoop == false)
                                   outerScope = outerScope->get_scope();

                           // If we reach the global scope then there has been some sort of error!
                              ROSE_ASSERT(isSgGlobal(outerScope) == NULL);
                            }

                         SgForStatement* forStatement = isSgForStatement(outerScope);
                         ROSE_ASSERT(forStatement != NULL);

                         SgStatement* forLoopTest = forStatement->get_test();
                         SgExprStatement* expressionStatement = isSgExprStatement(forLoopTest);
                         ROSE_ASSERT(expressionStatement != NULL);
                         SgExpression* expr = expressionStatement->get_expression();
                         ROSE_ASSERT(expr != NULL);
                         SgBinaryOp* binaryOperator = isSgBinaryOp(expr);
                         ROSE_ASSERT(binaryOperator != NULL);
                         SgExpression* rhs = binaryOperator->get_rhs_operand();
                         ROSE_ASSERT(rhs != NULL);

                      // Build a new expression: "lhs < rhs + arraySizeCopy", where the arraySizeCopy is a size of "array"
                         SgExpression* newIndexExpression = buildAddOp(rhs,arraySizeCopy);

                      // Subsitute the new expression for the old expression
                         binaryOperator->set_rhs_operand(newIndexExpression);
                       }
                      else
                       {
                         printf ("This dimension of the array access does not use a variable or uses more than one nodeListSize = %d \n",nodeListSize);

                         printf ("Error: treat this as an error for now! \n");
                         ROSE_ASSERT(false);
                       }
                  }

               ROSE_ASSERT(arrayType->get_base_type() != NULL);
               arrayType = isSgArrayType(arrayType->get_base_type());

               arrayReferenceStack.pop_back();
             }

#if 1
          addComment (astNode,std::string("*** NOTE Seeded Security Flaw: BufferOverFlowSecurityFlaw_AlterLoopBounds ") + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );
#endif
        }
   }


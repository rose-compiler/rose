// This is where we put the suppprting code for bug seeding.

#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

#include "bugSeedingSupport.h"

void
SecurityFlaw::detectVunerabilities( SgProject *project )
   {
  // This is a pure virtual function in the bae class, so it should not be called.
     printf ("Error: Base class function called: SecurityFlaw::detectVunerabilities() \n");
     ROSE_ASSERT(false);
   }

void
SecurityFlaw::defineSearchSpace()
   {
     printf ("Base class function called: SecurityFlaw::defineSearchSpace() \n");
   }

void
SecurityFlaw::seedSecurityFlaws( SgProject *project )
   {
     printf ("Base class function called: SecurityFlaw::seedSecurityFlaws() \n");
   }


void
BufferOverFlowSecurityFlaw::Vulnerability::detector( SgProject *project )
   {
     ROSE_ASSERT (project != NULL);

  // Build an AST traversal object
     Traversal treeTraversal;

  // Build the initial inherited attribute
     InheritedAttribute inheritedAttribute;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles".
     treeTraversal.traverseInputFiles (project,inheritedAttribute);
   }


BufferOverFlowSecurityFlaw::Vulnerability::InheritedAttribute
BufferOverFlowSecurityFlaw::Vulnerability::Traversal::evaluateInheritedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute )
   {
  // This is the function that detects the vulnerability

  // Use this if we only want to seed bugs in loops
     bool isLoop = inheritedAttribute.isLoop           ||
                   (isSgForStatement(astNode) != NULL) ||
                   (isSgWhileStmt(astNode) != NULL)    ||
                   (isSgDoWhileStmt(astNode) != NULL);

  // Added Fortran support
     isLoop = isLoop || (isSgFortranDo(astNode) != NULL);

  // Mark future noes in this subtree as being part of a loop
     inheritedAttribute.isLoop = isLoop;

     if (inheritedAttribute.isLoop == true)
        {
       // The inherited attribute is true iff we are inside a loop and this is a SgPntrArrRefExp.
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);
          if (arrayReference != NULL)
             {
            // Place an attribute at this IR node to mark it as a vulnerability.

               printf ("Found a buffer overflow vulnerability \n");

            // Build an attribute (on the heap)
               AstAttribute* newAttribute = new SecurityVulnerabilityAttribute(5);
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


void BufferOverFlowSecurityFlaw::detectVunerabilities( SgProject *project )
   {
     ROSE_ASSERT(project != NULL);

  // Build all the different types of vulnerabilities for this security flaw (could be many)
     vulnerabilityKindList.push_back(new Vulnerability());

  // Now iterate over the list
     std::vector<Vulnerability*>::iterator i = vulnerabilityKindList.begin();
     while (i != vulnerabilityKindList.end())
        {
          (*i)->detector(project);
          i++;
        }
   }


void
BufferOverFlowSecurityFlaw::SeedTraversal::visit( SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
     SecurityVulnerabilityAttribute* securityVulnerabilityAttribute = dynamic_cast<SecurityVulnerabilityAttribute*>(existingAttribute);

     if (securityVulnerabilityAttribute != NULL && securityVulnerabilityAttribute->get_value() == 5)
        {
       // The inherited attribute is true iff we are inside a loop and this is a SgPntrArrRefExp.
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);
          ROSE_ASSERT(arrayReference != NULL);

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

       // Subsitute the new expression for the old expression
          arrayReference->set_rhs_operand(newIndexExpression);

       // Now add a comment to make clear that this is a location of a seeded security flaw
          std::string comment = "// *** NOTE Seeded Security Flaw: BufferOverFlowSecurityFlaw ";
          PreprocessingInfo* commentInfo = new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, 
               comment,"user-generated",0, 0, 0, PreprocessingInfo::before, false, true);
          SgStatement* associatedStatement = TransformationSupport::getStatement(astNode);
          associatedStatement->addToAttachedPreprocessingInfo(commentInfo);
        }
   }


void
BufferOverFlowSecurityFlaw::seedSecurityFlaws( SgProject *project )
   {
     ROSE_ASSERT(project != NULL);

  // Build an AST traversal object
  // BufferOverFlowSecurityFlaw::SeedTraversal treeTraversal();
  // BufferOverFlowSecurityFlaw::SeedTraversal treeTraversal;
     SeedTraversal treeTraversal;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles".
     treeTraversal.traverseInputFiles (project,preorder);
   }



















// Initial idea ...
#if 0
using namespace VulnerabilityDetection;

InheritedAttribute
VulnerabilityDetection::Traversal::evaluateInheritedAttribute (
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

            // Place an attribute at this IR node to mark it as a vulnerability.


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

            // Subsitute the new expression for the old expression
               arrayReference->set_rhs_operand(newIndexExpression);
             }
        }

     return inheritedAttribute;
   }

#endif

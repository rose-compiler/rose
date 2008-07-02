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
   }

BufferOverFlowSecurityFlaw::~BufferOverFlowSecurityFlaw()
   {
   }

void BufferOverFlowSecurityFlaw::detectVunerabilities( SgProject *project )
   {
  // Build the list of ways to detect this security flaw in source code and 
  // execute the "detector()" member function on each one.

     ROSE_ASSERT(project != NULL);

  // Build all the different types of vulnerabilities for this security flaw (could be many)
  // E.g. an array access in a loop body might be one, and an array access in an index 
  // expression (indirect addressing) might be another.  The point is that there can be many.
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
BufferOverFlowSecurityFlaw::seedWithGrainularity( SgProject *project )
   {
     ROSE_ASSERT (project != NULL);

  // Build an AST traversal object
     CloneVulnerabilityTraversal treeTraversal;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
     treeTraversal.traverseInputFiles (project,preorder);

  // Build all the different types of seeding techniques for this security flaw (could be many)
     seedKindList.push_back(new SeedSecurityFlaw());

  // Now iterate over the list
     std::vector<SeedSecurityFlaw*>::iterator j = seedKindList.begin();
     while (j != seedKindList.end())
        {
       // Transform the new statment (the copy)
          (*j)->seed(project);
          j++;
        }
   }

#if 0
void
BufferOverFlowSecurityFlaw::seedConedTrees( SgProject *project )
   {
     ROSE_ASSERT (project != NULL);

  // Build all the different types of seeding techniques for this security flaw (could be many)
     seedKindList.push_back(new SeedSecurityFlaw());

  // Now iterate over the list
     std::vector<SeedSecurityFlaw*>::iterator j = seedKindList.begin();
     while (j != seedKindList.end())
        {
       // Transform the new statment (the copy)
          (*j)->seed(project);
          j++;
        }
   }
#endif


// **********************************************************************
//              BufferOverFlowSecurityFlaw::Vulnerability
// **********************************************************************

void
BufferOverFlowSecurityFlaw::Vulnerability::detector( SgProject *project )
   {
     ROSE_ASSERT (project != NULL);

  // Build an AST traversal object
     Traversal treeTraversal;

  // Build the initial inherited attribute
     InheritedAttribute inheritedAttribute;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
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


// **********************************************************************
//       BufferOverFlowSecurityFlaw::CloneVulnerabilityTraversal
// **********************************************************************


void
BufferOverFlowSecurityFlaw::CloneVulnerabilityTraversal::visit( SgNode* astNode )
   {
   // This function calls the seeding traversal on the AST formed by:
   //    1) Finding the SecurityVulnerabilityAttribute marker 
   //    2) Backing up the AST to a specific level of grainularity
   //    3) Then calling each kind of SeedSecurityFlaw (likely this should be called on different copies; current limitation)

     ROSE_ASSERT(astNode != NULL);

#if 1
     printf ("Returning without generating a clone! \n");
     return;
#endif

     AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
     SecurityVulnerabilityAttribute* securityVulnerabilityAttribute = dynamic_cast<SecurityVulnerabilityAttribute*>(existingAttribute);

     if (securityVulnerabilityAttribute != NULL && securityVulnerabilityAttribute->get_value() == 5)
        {
       // This is a marked security flaw, now backup to a position from which to build a copy so  
       // that we can introduce a version with the seeded security flaw.  This is the grainularity 
       // option for the seeding.

          std::vector<SgNode*> grainularityAxis = SeedSecurityFlaw::grainularityOfSeededCode(astNode);

          std::vector<SgNode*>::reverse_iterator i = grainularityAxis.rbegin();
          while (i != grainularityAxis.rend())
             {
            // Put code here to build AST copy and transform the copy!
               SgNode* subtree = *i;

               printf ("subtree         = %p = %s \n",subtree,subtree->class_name().c_str());

               SgStatement* subTreeStatement = isSgStatement(subtree);
               ROSE_ASSERT(subTreeStatement != NULL);

            // Make a copy of the expression used to hold the array size in the array declaration.
               SgTreeCopy subTreeCopyHelp;
               SgStatement* nearestWholeStatementCopy = isSgStatement(subtree->copy(subTreeCopyHelp));
               ROSE_ASSERT(nearestWholeStatementCopy != NULL);

            // Insert the new statement after the statement with the security vulnerability (at the defined level of grainularity)
               subTreeStatement->get_scope()->insert_statement(subTreeStatement,nearestWholeStatementCopy,false);

            // Note that SgFunctionDeclaration IR nodes have the additional detail of requiring symbols to be added.
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(nearestWholeStatementCopy);
               if (functionDeclaration != NULL)
                  {
                    SgName functionName = functionDeclaration->get_name();
                    functionName += std::string("_SecurityFlawSeeded_function_") + StringUtility::numberToString(uniqueValue());
                    functionDeclaration->set_name(functionName);

                    printf ("functionName = %s \n",functionName.str());

                    ROSE_ASSERT(functionDeclaration->get_scope() != NULL);
                    SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(functionDeclaration);
                    SgName mangledName = functionDeclaration->get_mangled_name();
                    functionDeclaration->get_scope()->insert_symbol(functionName,functionSymbol);
                 // functionDeclaration->get_scope()->insert_symbol(mangledName,functionSymbol);
                    functionSymbol->set_parent(functionDeclaration->get_scope()->get_symbol_table());

                    addComment (nearestWholeStatementCopy,"// *** NOTE Function Containing Seeded Security Flaw: BufferOverFlowSecurityFlaw ");

                 // error checking
                    SgSymbol* local_symbol = functionDeclaration->get_symbol_from_symbol_table();
                    ROSE_ASSERT(local_symbol != NULL);
                  }

               i++;
             }

       // Now that the clones have been made remove the original attribute so that we will not
       // seed the original version of the code that was marked for seeding.  This leaves a
       // single version of the original code in place in the generated source code.
          astNode->removeAttribute("SecurityVulnerabilityAttribute");
          addComment (astNode,"// *** NOTE Original Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ");

       // Remove the finest grainularity statement (the original statement, since it has already been added and transformed).
          ROSE_ASSERT(grainularityAxis.empty() == false);
          SgStatement* subTreeStatement = isSgStatement(*(grainularityAxis.rbegin()));
          ROSE_ASSERT(subTreeStatement != NULL);
#if 0
       // This is an error to call it on itself (subTreeStatement), so call it from the scope.
          subTreeStatement->get_scope()->remove_statement(subTreeStatement);
#endif
        }
   }


// **********************************************************************
//            BufferOverFlowSecurityFlaw::SeedSecurityFlaw
// **********************************************************************

void
BufferOverFlowSecurityFlaw::SeedSecurityFlaw::seed( SgNode *astNode )
   {
     ROSE_ASSERT (astNode != NULL);

  // Build an AST traversal object
     SeedTraversal treeTraversal;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
     treeTraversal.traverse(astNode,preorder);
   }

void
BufferOverFlowSecurityFlaw::SeedSecurityFlaw::SeedTraversal::visit( SgNode* astNode )
   {
  // This function is most likely to be called on a copy, in which case it should find the 
  // SecurityVulnerabilityAttribute (copied by the AST copy mechanism) and then implement
  // the transformation.  This is the reason for the recent change to ROSE to have the
  // AST copy mechanism be fixed to also copy AST persistant attributes.

     ROSE_ASSERT(astNode != NULL);

     AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
     SecurityVulnerabilityAttribute* securityVulnerabilityAttribute = dynamic_cast<SecurityVulnerabilityAttribute*>(existingAttribute);

     if (securityVulnerabilityAttribute != NULL && securityVulnerabilityAttribute->get_value() == 5)
        {
#if 0
       // The inherited attribute is true iff we are inside a loop and this is a SgPntrArrRefExp.
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);
          ROSE_ASSERT(arrayReference != NULL);

       // Now change the array index (to seed the buffer overflow bug)
          SgVarRefExp* arrayVarRef = isSgVarRefExp(arrayReference->get_lhs_operand());
#if 0
          if (arrayVarRef != NULL)
             {
             }
#endif
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

          addComment (astNode,"// *** NOTE Seeded Security Flaw: BufferOverFlowSecurityFlaw ");
#else

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
                    printf ("arrayName->get_name() = %s \n",arrayName->get_name().str());
                    arrayType = isSgArrayType(arrayName->get_type());
                    ROSE_ASSERT(arrayType != NULL);
                  }
                 else
                  {
                    printf ("Recursing one more level into the array indexing... \n");
                    arrayReference = isSgPntrArrRefExp(arrayReference->get_lhs_operand());
                    ROSE_ASSERT(arrayReference != NULL);
                  }
             }

          ROSE_ASSERT(arrayType != NULL);
          while (arrayReferenceStack.empty() == false)
             {
               ROSE_ASSERT(arrayType->get_base_type() != NULL);
               printf ("arrayReferenceStack.size() = %ld arrayType->get_base_type() = %p = %s \n",arrayReferenceStack.size(),arrayType->get_base_type(),arrayType->get_base_type()->class_name().c_str());

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

          printf ("At base of BufferOverFlowSecurityFlaw::SeedSecurityFlaw::SeedTraversal::visit()! \n");

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
#endif
        }
   }

std::vector<SgNode*>
BufferOverFlowSecurityFlaw::SeedSecurityFlaw::grainularityOfSeededCode( SgNode* astNode )
   {
  // Loop through the parents of the input node to gather the locations of possible subtree that we will copy and 
  // transform (to seed security flaws into).

     std::vector<SgNode*> returnVector;

     bool atRoot = false;
     while ( atRoot == false )
        {
          SgStatement* statement = isSgStatement(astNode);
          if (statement != NULL)
             {
               printf ("statement = %p = %s \n",statement,statement->class_name().c_str());

            // Make sure this is not a statement which we can't insert as a subtree.  For example, it is useless to
            // clone a body of a for loop since we can't insert it into the for loop (because a for loop can have 
            // only a single body).
               if ( isSgBasicBlock(statement) == NULL && isSgFunctionDefinition(statement) == NULL )
                  {
                    printf ("Adding this statement to the vector of possible subtrees: statement = %p = %s \n",statement,statement->class_name().c_str());
                    returnVector.push_back(statement);

                 // Use this to just execute a single case!
                    break;
                  }
             }

          astNode = astNode->get_parent();
          ROSE_ASSERT(astNode != NULL);

       // Stop when we get to the global scope.
          atRoot = (isSgGlobal(astNode) != NULL);
        }

     return returnVector;
   }


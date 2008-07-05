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

// void BufferOverFlowSecurityFlaw::seedWithGrainularity( SgProject *project )
void
BufferOverFlowSecurityFlaw::codeCloneGeneration( SgProject *project )
   {
     ROSE_ASSERT (project != NULL);

  // Build all the different types of seeding techniques for this security flaw (could be many)
  // Not that the SeedSecurityFlaw object contain information and functionality required to
  // construct the clones.  Each clone is built at a specific level of grainularity as required
  // to support the security flaw seeding transformation.  E.g. a buffer overflow that appears
  // as an array write in a loop might be seeded by adjusting the loop bounds, to support this
  // the grainularity of the cloning operation must be at least that of the loop level grainularity
  // or greater.  Alternatively, to seed the buffer overflow via an array subscript manipulation
  // we only need to clone the array expression (the finest possible level of grainularity).
  // seedKindList.push_back(new SeedBufferOverflowSecurityFlaw());
     SecurityFlaw::SeedSecurityFlaw* flaw = new SeedBufferOverflowSecurityFlaw();
     seedKindList.push_back(flaw);

  // Now iterate over the list
     std::vector<SecurityFlaw::SeedSecurityFlaw*>::iterator j = seedKindList.begin();
     while (j != seedKindList.end())
        {
       // How we clone subtrees depends upon which seeding appraoch is being used, so first
       // we build the SeedSecurityFlaw object and then build any clones if required.

       // User option to permit seeding of original code or a separate code fragement and a selected 
       // level of grainularity (e.g. alternate statement, enclosing statement, function, class, file, etc.).
          if ((*j)->get_seedOriginalCode() == false)
             {
            // Make all the required clones to support the security flaw seeding for each specific SeedSecurityFlaw
               CloneVulnerability::makeClones(project,*j);

               MarkClones::markVulnerabilitiesInClones(project,*j);
             }

          j++;
        }
   }

void
BufferOverFlowSecurityFlaw::seedSecurityFlaws( SgProject *project )
   {
  // Note that the container of SeedSecurityFlaw was built in BufferOverFlowSecurityFlaw::codeCloneGeneration()

     ROSE_ASSERT (project != NULL);

     printf ("In BufferOverFlowSecurityFlaw::seedSecurityFlaws() \n");

     ROSE_ASSERT(seedKindList.empty() == false);

  // Now iterate over the list
     std::vector<SecurityFlaw::SeedSecurityFlaw*>::iterator j = seedKindList.begin();
     while (j != seedKindList.end())
        {
       // Transform the new statment (the copy)
          (*j)->seed(project);
          j++;
        }
   }


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

#if 1
     bool markAllBuffers = true;
#endif

     if (inheritedAttribute.isLoop == true || markAllBuffers)
        {
       // The inherited attribute is true iff we are inside a loop and this is a SgPntrArrRefExp.
          SgPntrArrRefExp *arrayReference = isSgPntrArrRefExp(astNode);
          if (arrayReference != NULL)
             {
            // Place an attribute at this IR node to mark it as a vulnerability.

               printf ("Found a buffer overflow vulnerability \n");

            // Build an attribute (on the heap)
               AstAttribute* newAttribute = new SecurityVulnerabilityAttribute(astNode);
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

#if 0
// **********************************************************************
//       BufferOverFlowSecurityFlaw::CloneVulnerabilityTraversal
// **********************************************************************

BufferOverFlowSecurityFlaw::CloneVulnerability::CloneVulnerabilityTraversal::CloneVulnerabilityTraversal( SecurityFlaw::SeedSecurityFlaw* Xptr )
   {
     ROSE_ASSERT(Xptr != NULL);
     associtedSeedSecurityFlaw = Xptr;
   }

BufferOverFlowSecurityFlaw::CloneVulnerability::CloneVulnerabilityTraversal::~CloneVulnerabilityTraversal()
   {
     associtedSeedSecurityFlaw = NULL;
   }

#if 0
// This code does not appear to compile, I don't know why.  I have alternatively
// worked around the problem by specifying the code in the header file since it is trivial.
BufferOverFlowSecurityFlaw::CloneVulnerability::PrimaryVulnerabilityTraversal(SgNode* node)
   {
     xNode = node;
   }

BufferOverFlowSecurityFlaw::CloneVulnerability::~PrimaryVulnerabilityTraversal()
   {
     xNode = NULL;
   }
#endif

void
BufferOverFlowSecurityFlaw::CloneVulnerability::makeClones( SgProject* project, SecurityFlaw::SeedSecurityFlaw* flaw )
   {
  // Build an AST traversal object
     CloneVulnerabilityTraversal treeTraversal(flaw);

     InheritedAttribute inheritedAttribute;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
  // treeTraversal.traverseInputFiles (project,preorder);
     treeTraversal.traverseInputFiles (project,inheritedAttribute);
   }

void
BufferOverFlowSecurityFlaw::CloneVulnerability::markPrimaryCloneVulnerability( SgNode* primaryNodeInClonedCode, SgNode* primaryNodeInOriginalCode, SgNode* rootOfClone )
   {
  // Build an AST traversal object
     PrimaryVulnerabilityTraversal treeTraversal(primaryNodeInClonedCode,primaryNodeInOriginalCode,rootOfClone);

     ROSE_ASSERT(primaryNodeInClonedCode == NULL);
  // printf ("primaryNodeInClonedCode   = %p = %s \n",primaryNodeInClonedCode,primaryNodeInClonedCode->class_name().c_str());
     printf ("primaryNodeInOriginalCode = %p = %s \n",primaryNodeInOriginalCode,primaryNodeInOriginalCode->class_name().c_str());
     printf ("rootOfClone               = %p = %s \n",rootOfClone,rootOfClone->class_name().c_str());

     printf ("treeTraversal.primaryVulnerabilityNodeInOriginalCode = %p = %s \n",treeTraversal.primaryVulnerabilityNodeInOriginalCode,treeTraversal.primaryVulnerabilityNodeInOriginalCode->class_name().c_str());
     printf ("treeTraversal.rootOfClone                            = %p = %s \n",treeTraversal.rootOfClone,treeTraversal.rootOfClone->class_name().c_str());

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
     treeTraversal.traverse (rootOfClone,preorder);
   }


void
BufferOverFlowSecurityFlaw::CloneVulnerability::PrimaryVulnerabilityTraversal::visit( SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     AstAttribute* attributeInClonedCode = astNode->getAttribute("SecurityVulnerabilityAttribute");
     SecurityVulnerabilityAttribute* securityVulnerabilityAttributeInClonedCode = dynamic_cast<SecurityVulnerabilityAttribute*>(attributeInClonedCode);

  // Use the value to code specific types of vulnerabilities
  // if (securityVulnerabilityAttributeInClonedCode != NULL && securityVulnerabilityAttributeInClonedCode->get_value() == 5)
  // if (securityVulnerabilityAttributeInClonedCode != NULL && securityVulnerabilityAttributeInClonedCode->get_securityVulnerabilityNode() == astNode)
     if (securityVulnerabilityAttributeInClonedCode != NULL)
        {
       // SgNode* primaryNodeInOriginalCode = primaryVulnerabilityNodeInOriginalCode;
          ROSE_ASSERT(primaryVulnerabilityNodeInOriginalCode != NULL);

          ROSE_ASSERT(securityVulnerabilityAttributeInClonedCode->get_securityVulnerabilityNode() != astNode);

          printf ("primaryVulnerabilityNodeInOriginalCode = %p = %s \n",primaryVulnerabilityNodeInOriginalCode,primaryVulnerabilityNodeInOriginalCode->class_name().c_str());

       // Now get the AstAttribute on the primaryNodeInOriginalCode and see if it is the same, if so then we have identified the primary vulnerability in this clone.
          AstAttribute* attributeInOriginalCode = primaryVulnerabilityNodeInOriginalCode->getAttribute("SecurityVulnerabilityAttribute");
          SecurityVulnerabilityAttribute* securityVulnerabilityAttributeInOriginalCode = dynamic_cast<SecurityVulnerabilityAttribute*>(attributeInOriginalCode);
          ROSE_ASSERT(securityVulnerabilityAttributeInOriginalCode != NULL);

       // Since AstAttribute are presently shared this is a way of verifying that we have 
       // linked the location in the clone with the location in the original source code.
          if (securityVulnerabilityAttributeInOriginalCode == securityVulnerabilityAttributeInClonedCode)
             {
               printf ("***** Found primary vulnerability in clone ***** astNode = %p = %s rootOfClone = %p = %s \n",astNode,astNode->class_name().c_str(),rootOfClone,rootOfClone->class_name().c_str());
            // SgStatement* statement = isSgStatement(astNode);
            // ROSE_ASSERT(statement != NULL);
               addComment (astNode,"// *** NOTE Primary Node for clone: BufferOverFlowSecurityFlaw ");

            // Add pointer to root of clone for this security vulnerability to the list
            // securityVulnerabilityAttributeInOriginalCode->set_associatedClones(rootOfClone);

            // Remove the SecurityVulnerabilityAttribute attribute (this should not call the descructor for the AstAttribute object)
               astNode->removeAttribute("SecurityVulnerabilityAttribute");

               if (astNode->attributeExists("PrimarySecurityVulnerabilityForCloneAttribute") == false)
                  {
                 // AstAttribute* primaryVulnerabilityAttribute = new PrimarySecurityVulnerabilityForCloneAttribute(astNode);
                    PrimarySecurityVulnerabilityForCloneAttribute* primaryVulnerabilityAttribute = new PrimarySecurityVulnerabilityForCloneAttribute(astNode,rootOfClone);
                    ROSE_ASSERT(primaryVulnerabilityAttribute != NULL);

                    astNode->addNewAttribute("SeededSecurityFlawCloneAttribute",primaryVulnerabilityAttribute);

                    ROSE_ASSERT(securityVulnerabilityAttributeInOriginalCode->get_securityVulnerabilityNode() != NULL);
                    primaryVulnerabilityAttribute->set_primaryVulnerabilityInOriginalCode(securityVulnerabilityAttributeInOriginalCode->get_securityVulnerabilityNode());
                  }
                 else
                  {
                 // We may have to deal with a single IR node having several PrimarySecurityVulnerabilityForCloneAttribute 
                 // associated with different security flaws.  But for now I want to make this an error!
                    printf ("Error: this IR nodes already has a PrimarySecurityVulnerabilityForCloneAttribute \n");
                    ROSE_ASSERT(false);
                  }
             }
        }
   }


// void BufferOverFlowSecurityFlaw::CloneVulnerabilityTraversal::visit( SgNode* astNode )
BufferOverFlowSecurityFlaw::CloneVulnerability::InheritedAttribute
BufferOverFlowSecurityFlaw::CloneVulnerability::CloneVulnerabilityTraversal::evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute )
   {
   // This function calls the seeding traversal on the AST formed by:
   //    1) Finding the SecurityVulnerabilityAttribute marker 
   //    2) Backing up the AST to a specific level of grainularity
   //    3) Then removing any clones from subtrees of the original code.

     ROSE_ASSERT(astNode != NULL);

     AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
     SecurityVulnerabilityAttribute* securityVulnerabilityAttribute = dynamic_cast<SecurityVulnerabilityAttribute*>(existingAttribute);

  // Use the value to code specific types of vulnerabilities
  // if (securityVulnerabilityAttribute != NULL && securityVulnerabilityAttribute->get_value() == 5)
     if (securityVulnerabilityAttribute != NULL)
        {
       // This is a marked security flaw, now backup to a position from which to build a copy so  
       // that we can introduce a version with the seeded security flaw.  This is the grainularity 
       // option for the seeding.  We can support many different levels of grainularity, but the 
       // results appear more useful if one one is selected.  It is unclear if more that one will
       // ever be useful (more than one level of grainularity gets messy).

          ROSE_ASSERT(securityVulnerabilityAttribute->get_securityVulnerabilityNode() == astNode);

          ROSE_ASSERT(associtedSeedSecurityFlaw != NULL);
          std::vector<SgNode*> grainularityAxis = associtedSeedSecurityFlaw->grainularityOfSeededCode(astNode);

       // Iterate from finest level of grainularity (e.g. SgExpression or SgStatement) to largest 
       // level of grainularity (e.g. SgFunctionDeclaration or SgFile).  This is most often a 
       // container of size == 1.
          printf ("grainularityAxis.size() = %lu \n",grainularityAxis.size());

#if 1
       // I think this is the better choice but I am not certain.
          std::vector<SgNode*>::reverse_iterator i = grainularityAxis.rbegin();
          while (i != grainularityAxis.rend())
#else
          std::vector<SgNode*>::iterator i = grainularityAxis.begin();
          while (i != grainularityAxis.end())
#endif
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

            // Mark the original subtree that is being copied as the original (so that we can optionally permit not seeding the original code).
            // Note that we may be visiting this IR node for a second time so it might already have an existing SecurityFlawOriginalSubtreeAttribute.
               AstAttribute* originalSubtreeAttribute = new SecurityFlawOriginalSubtreeAttribute(0);
               ROSE_ASSERT(originalSubtreeAttribute != NULL);
            // AstAttribute* previousOriginalSubtreeAttribute = subTreeStatement->getAttribute("SecurityFlawOriginalSubtreeAttribute");
            // if (previousOriginalSubtreeAttribute == NULL)
               if (subTreeStatement->attributeExists("SecurityFlawOriginalSubtreeAttribute") == false)
                  {
                    subTreeStatement->addNewAttribute("SecurityFlawOriginalSubtreeAttribute",originalSubtreeAttribute);
                  }

            // Mark the copy as a copy built only for seeding security flaws (record the primary flaw in the original AST).
            // AstAttribute* cloneSubtreeAttribute = new SeededSecurityFlawCloneAttribute(astNode,subtree);
               AstAttribute* cloneSubtreeAttribute = new SeededSecurityFlawCloneAttribute(nearestWholeStatementCopy,subtree);
               ROSE_ASSERT(cloneSubtreeAttribute != NULL);
            // nearestWholeStatementCopy->addNewAttribute("SeededSecurityFlawCloneAttribute",cloneSubtreeAttribute);
               if (nearestWholeStatementCopy->attributeExists("SeededSecurityFlawCloneAttribute") == false)
                  {
                 // If the subTreeStatement has been copied previously then it had a SecurityFlawOriginalSubtreeAttribute 
                 // attribute added and we want to remove it from the copy just make.
                    if (nearestWholeStatementCopy->attributeExists("SecurityFlawOriginalSubtreeAttribute") == true)
                       {
                         nearestWholeStatementCopy->removeAttribute("SecurityFlawOriginalSubtreeAttribute");
                       }
                    nearestWholeStatementCopy->addNewAttribute("SeededSecurityFlawCloneAttribute",cloneSubtreeAttribute);

                 // Link from SecurityVulnerabilityAttribute to the clone that was generated to support its seeding.
                 // securityVulnerabilityAttribute->set_associatedClone(nearestWholeStatementCopy);

                 // Mark the clone's primary vulnerability (i.e. the reason why we bothered to make the clone).
                    SgNode* locationOfPrimaryVulnerabilityInClone = NULL;
                    markPrimaryCloneVulnerability(locationOfPrimaryVulnerabilityInClone,astNode,nearestWholeStatementCopy);
                  }

            // Insert the new statement after the statement with the security vulnerability (at the defined level of grainularity)
               if (isSgFunctionDeclaration(nearestWholeStatementCopy) != NULL)
                  {
                 // Insert functions at the base of the current scope
                    subTreeStatement->get_scope()->insertStatementInScope(nearestWholeStatementCopy,false);
                  }
                 else
                  {
                 // Insert statements after the current statement
                    subTreeStatement->get_scope()->insert_statement(subTreeStatement,nearestWholeStatementCopy,false);
                  }

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

                 // addComment (nearestWholeStatementCopy,"// *** NOTE Function Containing Seeded Security Flaw: BufferOverFlowSecurityFlaw ");

                 // error checking
                    SgSymbol* local_symbol = functionDeclaration->get_symbol_from_symbol_table();
                    ROSE_ASSERT(local_symbol != NULL);
                  }

               i++;
             }

#if 0
       // Now that the clones have been made remove the original attribute so that we will not
       // seed the original version of the code that was marked for seeding.  This leaves a
       // single version of the original code in place in the generated source code.
       // astNode->removeAttribute("SecurityVulnerabilityAttribute");
       // addComment (astNode,"// *** NOTE Original Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ");

       // Remove the finest grainularity statement (the original statement, since it has already been added and transformed).
          ROSE_ASSERT(grainularityAxis.empty() == false);
          SgStatement* subTreeStatement = isSgStatement(*(grainularityAxis.rbegin()));
          ROSE_ASSERT(subTreeStatement != NULL);

       // This is an error to call it on itself (subTreeStatement), so call it from the scope.
          subTreeStatement->get_scope()->remove_statement(subTreeStatement);
#endif
        }

     return inheritedAttribute;
   }
#endif

#if 0
// **********************************************************************
//           BufferOverFlowSecurityFlaw::MarkClonesTraversal
// **********************************************************************


BufferOverFlowSecurityFlaw::MarkClones::MarkClonesTraversal::MarkClonesTraversal( SecurityFlaw::SeedSecurityFlaw* Xptr )
   {
     ROSE_ASSERT(Xptr != NULL);
     associtedSeedSecurityFlaw = Xptr;
   }

BufferOverFlowSecurityFlaw::MarkClones::MarkClonesTraversal::~MarkClonesTraversal()
   {
     associtedSeedSecurityFlaw = NULL;
   }


void
BufferOverFlowSecurityFlaw::MarkClones::markVulnerabilitiesInClones( SgProject* project, SecurityFlaw::SeedSecurityFlaw* flaw )
   {
  // Build an AST traversal object
     MarkClonesTraversal treeTraversal(flaw);

     InheritedAttribute inheritedAttribute;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
  // treeTraversal.traverseInputFiles (project,preorder);
     treeTraversal.traverseInputFiles (project,inheritedAttribute);
   }


// void BufferOverFlowSecurityFlaw::CloneVulnerabilityTraversal::visit( SgNode* astNode )
BufferOverFlowSecurityFlaw::MarkClones::InheritedAttribute
BufferOverFlowSecurityFlaw::MarkClones::MarkClonesTraversal::evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute )
   {
   // This function calls the seeding traversal on the AST formed by:
   //    1) Finding the SecurityVulnerabilityAttribute marker 
   //    2) Backing up the AST to a specific level of grainularity
   //    3) Then removing any clones from subtrees of the original code.

     ROSE_ASSERT(astNode != NULL);

#if 0
     printf ("astNode                           = %p = %s \n",astNode,astNode->class_name().c_str());
     printf ("inheritedAttribute.inOriginalCode = %s \n",inheritedAttribute.inOriginalCode ? "true" : "false");
     printf ("inheritedAttribute.inClonedCode   = %s \n",inheritedAttribute.inClonedCode   ? "true" : "false");
#endif

     if (astNode->attributeExists("SecurityFlawOriginalSubtreeAttribute") == true)
        {
          printf ("Found a SecurityFlawOriginalSubtreeAttribute \n");
          inheritedAttribute.inOriginalCode = true;
          ROSE_ASSERT(inheritedAttribute.inClonedCode == false);
        }

     if (astNode->attributeExists("SeededSecurityFlawCloneAttribute") == true)
        {
          printf ("Found a SeededSecurityFlawCloneAttribute \n");
          inheritedAttribute.inClonedCode = true;
          ROSE_ASSERT(inheritedAttribute.inOriginalCode == false);
        }

     AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
     SecurityVulnerabilityAttribute* securityVulnerabilityAttribute = dynamic_cast<SecurityVulnerabilityAttribute*>(existingAttribute);

  // Use the value to code specific types of vulnerabilities
  // if (securityVulnerabilityAttribute != NULL && securityVulnerabilityAttribute->get_value() == 5)
     if (securityVulnerabilityAttribute != NULL)
        {
          if (inheritedAttribute.inOriginalCode == true)
             {
               ROSE_ASSERT(securityVulnerabilityAttribute->get_securityVulnerabilityNode() == astNode);

               addComment (astNode,"// *** NOTE Original Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ");
             }
            else
             {
               ROSE_ASSERT(securityVulnerabilityAttribute->get_securityVulnerabilityNode() != astNode);

               if (inheritedAttribute.inClonedCode == true)
                  {
                    addComment (astNode,"// *** NOTE Cloned Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ");
                  }
                 else
                  {
                    printf ("Outside of of both original code and cloned code! astNode = %p = %s \n",astNode,astNode->class_name().c_str());
                  }
             }
        }

     return inheritedAttribute;
   }
#endif


// **********************************************************
// BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw
// **********************************************************

BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw::SeedBufferOverflowSecurityFlaw()
   {
   }

BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw::~SeedBufferOverflowSecurityFlaw()
   {
   }


void
BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw::seed( SgNode *astNode )
   {
     ROSE_ASSERT (astNode != NULL);

  // Build an AST traversal object
     SeedTraversal treeTraversal;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
     treeTraversal.traverse(astNode,preorder);
   }

void
BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw::SeedTraversal::visit( SgNode* astNode )
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

          printf ("At base of BufferOverFlowSecurityFlaw::SeedBufferOverflowSecurityFlaw::SeedTraversal::visit()! \n");

          addComment (astNode,"// *** NOTE Seeded Security Flaw: BufferOverFlowSecurityFlaw ");

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
   }


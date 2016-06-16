// This is where we put the supporting code for bug seeding.

#include "rose.h"

using namespace std;
using namespace rose;
using namespace SageBuilder;
using namespace SageInterface;

#include "bugSeeding.h"


SecurityFlaw::SecurityFlaw()
   {
  // I think that we will discover that seeding the original input application without 
  // cloning code fragements will be messy and unanalizable by static analysis tools.
  // So the value for seedOriginalCode is FALSE by default.
  // seedOriginalCode = false;
   }

SecurityFlaw::~SecurityFlaw()
   {
  // Nothing to do here!
   }


// ************************
// Virtual Member Functions
// ************************

// This is a virtual member function
void
SecurityFlaw::detectVunerabilities( SgProject *project )
   {
  // This is a pure virtual function in the bae class, so it should not be called.
     printf ("Error: Base class function called: SecurityFlaw::detectVunerabilities() \n");

     ROSE_ASSERT(vulnerabilityKindList.empty() == false);

     printf ("In SecurityFlaw::detectVunerabilities(): vulnerabilityKindList.size() = %zu \n",vulnerabilityKindList.size());

  // Now iterate over the list
     vector<SecurityFlaw::Vulnerability*>::iterator i = vulnerabilityKindList.begin();
     while (i != vulnerabilityKindList.end())
        {
          (*i)->detector(project);
          i++;
        }
   }

// This is a virtual member function
void
SecurityFlaw::defineSearchSpace()
   {
     printf ("Base class function called: SecurityFlaw::defineSearchSpace() \n");

     printf ("For now make this an error... \n");
     ROSE_ASSERT(false);
   }

// This is a virtual member function
void
SecurityFlaw::codeCloneGeneration( SgProject *project )
   {
     printf ("Base class function called: SecurityFlaw::codeCloneGeneration() \n");

     ROSE_ASSERT(seedKindList.empty() == false);

  // These should only be called once, not in a loop over all SecurityFlaw::SeedSecurityFlaw objects.

  // For now provide a pointer to the first SecurityFlaw::SeedSecurityFlaw base class.
  // But these functions should only be called once to generate all clones on a per
  // identified security flaw basis.  Any more AST copies shouldbe done to support the
  // specific requirements of each SecurityFlaw::SeedSecurityFlaw object (defined mechanism).
     vector<SecurityFlaw::SeedSecurityFlaw*>::iterator j = seedKindList.begin();

  // Generate clones for security flaw vulnerabilities
     CloneVulnerability::makeClones(project,*j);

  // This marks all the clones with attributes to associate them to one another.
     MarkClones::markVulnerabilitiesInClones(project,*j);

     printf ("In SecurityFlaw::codeCloneGeneration(): seedKindList.size() = %zu \n",seedKindList.size());

  // Reset the counters used to generate uniquely named functions and variables.
     uniqueValueSeeding(true);

  // DQ (7/26/2008): Added loop over the list of different seeding approaches.
  // while (j != seedKindList.end())
        {
       // Generate clones for seeding methodologies (additional cloning on each clone generated 
       // for a security flaw vulnerabilities)
          CloneSeedLocation::makeClones(project,*j);

          j++;
        }

  // DQ (7/26/2008): Add comments to the clones so that we can identify them in the generated code.
     CommentClones::commentClones(project);
   }

// This is a virtual member function
void
SecurityFlaw::seedSecurityFlaws( SgProject *project )
   {
     printf ("Base class function called: SecurityFlaw::seedSecurityFlaws() \n");

  // Note that the container of SeedSecurityFlaw was built in BufferOverFlowSecurityFlaw::codeCloneGeneration()

     ROSE_ASSERT (project != NULL);

     printf ("In BufferOverFlowSecurityFlaw::seedSecurityFlaws() \n");

     ROSE_ASSERT(seedKindList.empty() == false);

  // Now iterate over the list
     vector<SecurityFlaw::SeedSecurityFlaw*>::iterator j = seedKindList.begin();
     while (j != seedKindList.end())
        {
       // Transform the new statment (the copy)
          printf ("In SecurityFlaw::seedSecurityFlaws(): SecurityFlaw::SeedSecurityFlaw = %s \n",(*j)->get_name().c_str());

          (*j)->seed(project);
          j++;
        }
   }



// ******************
// Static Member Data
// ******************

// Declaration of static data member (collection of all security flaws).
vector<SecurityFlaw*> SecurityFlaw::securityFlawCollection;



// ***********************
// Static Member Functions
// ***********************

// This is a static member function
void
SecurityFlaw::initialize()
   {
  // When there is support for many different kinds of security flaws the list will be assembled here!

  // Build a BufferOverFlowSecurityFlaw object
     BufferOverFlowSecurityFlaw* bufferOverFlowSecurityFlaw = new BufferOverFlowSecurityFlaw();

  // Call initialize function
  // bufferOverFlowSecurityFlaw->initialize ();

     securityFlawCollection.push_back(bufferOverFlowSecurityFlaw);
   }

// This is a static member function
void
SecurityFlaw::defineAllSearchSpaces()
   {
  // Not clear no how to implement this part of the plan.
  // For now I am leaving the search space specification out of the implementation.
   }

// This is a static member function
void
SecurityFlaw::detectAllVunerabilities( SgProject *project )
   {
  // Call the member function to annotate the AST where each security flaw vulnerabilities exists.

     printf ("In SecurityFlaw::detectAllVunerabilities(): securityFlawCollection.size() = %zu \n",securityFlawCollection.size());

     vector<SecurityFlaw*>::iterator i = securityFlawCollection.begin();
     while (i != securityFlawCollection.end())
        {
          (*i)->detectVunerabilities(project);
          i++;
        }
   }

// This is a static member function
void
SecurityFlaw::generationAllClones( SgProject *project )
   {
  // Reset the counters used to generate uniquely named functions and variables.
  // uniqueValueVulnerability(true);
  // uniqueValueSeeding(true);

     printf ("In BufferOverFlowSecurityFlaw::seedSecurityFlaws() \n");

     printf ("In SecurityFlaw::generationAllClones(): securityFlawCollection.size() = %zu \n",securityFlawCollection.size());

     vector<SecurityFlaw*>::iterator i = securityFlawCollection.begin();
     while (i != securityFlawCollection.end())
        {
       // Reset the counters used to generate uniquely named functions and variables.
          uniqueValueVulnerability(true);

       // It is redundant to reset this but reset them (both static functions to generate unique values) together anyway.
          uniqueValueSeeding(true);

          (*i)->codeCloneGeneration(project);
          i++;
        }
   }

void
SecurityFlaw::seedAllSecurityFlaws( SgProject *project )
   {
     vector<SecurityFlaw*>::iterator i = securityFlawCollection.begin();
     while (i != securityFlawCollection.end())
        {
          (*i)->seedSecurityFlaws(project);
          i++;
        }
   }

void
SecurityFlaw::resetAttributesAfterASTCloning( SgNode* root )
   {
     printf ("Calling the traversal to reset the attributes on the AST (root = %p = %s) \n",root,root->class_name().c_str());

     ResetSecurityFlawAttributesTraversal treeTraversal;
     treeTraversal.traverse (root,preorder);
   }



// ***************************
// Supporting Member Functions
// ***************************

void
SecurityFlaw::addComment( SgNode* astNode, string comment )
   {
  // This function adds a comment before the statement contained by the input IR node's associated statement IR node.
     SgStatement* associatedStatement = TransformationSupport::getStatement(astNode);

  // printf ("In SecurityFlaw::addComment(astNode = %p): Adding comment to SgStatement = %p \n",astNode,associatedStatement);
     SageInterface::attachComment(associatedStatement,comment);
   }

int
SecurityFlaw::uniqueValueVulnerability( bool reset )
   {
  // This function retruns a unique integer value and is used to build names of functions, 
  // variable, etc. to avoid name collisions.

     static int i = 0;

  // i++;
     if (reset == true)
          i = 0;
       else
          i++;

     return i;
   }


int
SecurityFlaw::uniqueValueSeeding( bool reset )
   {
  // This function retruns a unique integer value and is used to build names of functions, 
  // variable, etc. to avoid name collisions.

     static int i = 0;

  // i++;
     if (reset == true)
          i = 0;
       else
          i++;

     return i;
   }


// **********************************************************************
//             SecurityFlaw::Vulnerability
// **********************************************************************

SecurityFlaw::Vulnerability::Vulnerability()
   {
   }

SecurityFlaw::Vulnerability::~Vulnerability()
   {
   }

string
SecurityFlaw::Vulnerability::get_name()
   {
     return "SecurityFlaw::Vulnerability";
   }

string
SecurityFlaw::Vulnerability::get_color()
   {
     return "blue";
   }

void
SecurityFlaw::Vulnerability::associateSeeding ( SecurityFlaw::SeedSecurityFlaw* seedingApproach )
   {
     associatedSeedingTechniques.insert(seedingApproach);
   }


// **********************************************************************
//             SecurityFlaw::CloneVulnerabilityTraversal
// **********************************************************************

SecurityFlaw::CloneVulnerability::CloneVulnerabilityTraversal::CloneVulnerabilityTraversal( SeedSecurityFlaw* Xptr )
   {
     ROSE_ASSERT(Xptr != NULL);
     associtedSeedSecurityFlaw = Xptr;
   }

SecurityFlaw::CloneVulnerability::CloneVulnerabilityTraversal::~CloneVulnerabilityTraversal()
   {
     associtedSeedSecurityFlaw = NULL;
   }

#if 0
// This code does not appear to compile, I don't know why.  I have alternatively
// worked around the problem by specifying the code in the header file since it is trivial.
SecurityFlaw::CloneVulnerability::PrimaryVulnerabilityTraversal(SgNode* node)
   {
     xNode = node;
   }

SecurityFlaw::CloneVulnerability::~PrimaryVulnerabilityTraversal()
   {
     xNode = NULL;
   }
#endif

void
SecurityFlaw::CloneVulnerability::makeClones( SgProject* project, SeedSecurityFlaw* flaw )
   {
  // Build an AST traversal object
     CloneVulnerabilityTraversal treeTraversal(flaw);

     InheritedAttribute inheritedAttribute;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
  // treeTraversal.traverseInputFiles (project,preorder);
     treeTraversal.traverseInputFiles (project,inheritedAttribute);
   }

// void BufferOverFlowSecurityFlaw::CloneVulnerabilityTraversal::visit( SgNode* astNode )
SecurityFlaw::CloneVulnerability::InheritedAttribute
SecurityFlaw::CloneVulnerability::CloneVulnerabilityTraversal::evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute )
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
          vector<SgNode*> grainularityAxis = associtedSeedSecurityFlaw->grainularityOfSeededCode(astNode);

       // Iterate from finest level of grainularity (e.g. SgExpression or SgStatement) to largest 
       // level of grainularity (e.g. SgFunctionDeclaration or SgFile).  This is most often a 
       // container of size == 1.
          printf ("grainularityAxis.size() = %lu \n",grainularityAxis.size());

#if 1
       // I think this is the better choice but I am not certain.
          vector<SgNode*>::reverse_iterator i = grainularityAxis.rbegin();
          while (i != grainularityAxis.rend())
#else
          vector<SgNode*>::iterator i = grainularityAxis.begin();
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

               printf ("Calling resetAttributesAfterASTCloning... \n");
               resetAttributesAfterASTCloning(nearestWholeStatementCopy);
               printf ("DONE: Calling resetAttributesAfterASTCloning... \n");

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
                    functionName += string("_SecurityFlawVulnerability_function_") + StringUtility::numberToString(uniqueValueVulnerability());
                    functionDeclaration->set_name(functionName);

                    printf ("functionName = %s \n",functionName.str());

                    ROSE_ASSERT(functionDeclaration->get_scope() != NULL);
                    SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(functionDeclaration);
                    SgName mangledName = functionDeclaration->get_mangled_name();
                    functionDeclaration->get_scope()->insert_symbol(functionName,functionSymbol);
                 // functionDeclaration->get_scope()->insert_symbol(mangledName,functionSymbol);
                    functionSymbol->set_parent(functionDeclaration->get_scope()->get_symbol_table());

                 // addComment (nearestWholeStatementCopy,"*** NOTE Function Containing Seeded Security Flaw: BufferOverFlowSecurityFlaw ");

                 // error checking
                    SgSymbol* local_symbol = functionDeclaration->get_symbol_from_symbol_table();
                    ROSE_ASSERT(local_symbol != NULL);
                  }

               i++;
             }
        }

     return inheritedAttribute;
   }

void
SecurityFlaw::ResetSecurityFlawAttributesTraversal::visit( SgNode* astNode )
   {
  // When a clone is made, any attributes that it has (e.g. to mark vulnerabilities found
  // in earlier stages) are copied, but as a result their pointer values need to be reset.

     ROSE_ASSERT(astNode != NULL);

     SecurityVulnerabilityAttribute* securityVulnerabilityAttributeInClonedCode = dynamic_cast<SecurityVulnerabilityAttribute*>(astNode->getAttribute("SecurityVulnerabilityAttribute"));
     if (securityVulnerabilityAttributeInClonedCode != NULL)
        {
          ROSE_ASSERT(securityVulnerabilityAttributeInClonedCode->get_securityVulnerabilityNode() != astNode);

       // Note that the securityVulnerabilityNodeInOriginalCode is stored in the SecurityVulnerabilityAttribute, 
       // so we have a record of what is the AST node in the original AST, here we reset what the parent node is 
       // that contains the attribute.
          securityVulnerabilityAttributeInClonedCode->set_securityVulnerabilityNode(astNode);

       // At this point these are not different.
          ROSE_ASSERT(securityVulnerabilityAttributeInClonedCode->get_securityVulnerabilityNode() != securityVulnerabilityAttributeInClonedCode->get_securityVulnerabilityNodeInOriginalCode());
        }

     PrimarySecurityVulnerabilityForCloneAttribute* primarySecurityVulnerabilityAttributeInClonedCode = dynamic_cast<PrimarySecurityVulnerabilityForCloneAttribute*>(astNode->getAttribute("PrimarySecurityVulnerabilityForCloneAttribute"));
     if (primarySecurityVulnerabilityAttributeInClonedCode != NULL)
        {
          ROSE_ASSERT(primarySecurityVulnerabilityAttributeInClonedCode->get_primarySecurityFlawInClone() != astNode);

          primarySecurityVulnerabilityAttributeInClonedCode->set_primarySecurityFlawInClone(astNode);

       // printf ("Are there any of these in the AST? \n");
       // ROSE_ASSERT(false);
        }
   }


void
SecurityFlaw::CloneVulnerability::markPrimaryCloneVulnerability( SgNode* primaryNodeInClonedCode, SgNode* primaryNodeInOriginalCode, SgNode* rootOfClone )
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
SecurityFlaw::CloneVulnerability::PrimaryVulnerabilityTraversal::visit( SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     AstAttribute* attributeInClonedCode = astNode->getAttribute("SecurityVulnerabilityAttribute");
     SecurityVulnerabilityAttribute* securityVulnerabilityAttributeInClonedCode = dynamic_cast<SecurityVulnerabilityAttribute*>(attributeInClonedCode);

  // Use the value to code specific types of vulnerabilities
     if (securityVulnerabilityAttributeInClonedCode != NULL)
        {
       // SgNode* primaryNodeInOriginalCode = primaryVulnerabilityNodeInOriginalCode;
          ROSE_ASSERT(primaryVulnerabilityNodeInOriginalCode != NULL);

       // DQ (7/27/2008): Now that we reset the attributes this is false!
       // ROSE_ASSERT(securityVulnerabilityAttributeInClonedCode->get_securityVulnerabilityNode() != astNode);

          printf ("primaryVulnerabilityNodeInOriginalCode = %p = %s \n",primaryVulnerabilityNodeInOriginalCode,primaryVulnerabilityNodeInOriginalCode->class_name().c_str());

       // Now get the AstAttribute on the primaryNodeInOriginalCode and see if it is the same, if so then we have identified the primary vulnerability in this clone.
          AstAttribute* attributeInOriginalCode = primaryVulnerabilityNodeInOriginalCode->getAttribute("SecurityVulnerabilityAttribute");
          SecurityVulnerabilityAttribute* securityVulnerabilityAttributeInOriginalCode = dynamic_cast<SecurityVulnerabilityAttribute*>(attributeInOriginalCode);
          ROSE_ASSERT(securityVulnerabilityAttributeInOriginalCode != NULL);

       // Since AstAttribute are presently shared this is a way of verifying that we have 
       // linked the location in the clone with the location in the original source code.
       // if (securityVulnerabilityAttributeInOriginalCode == securityVulnerabilityAttributeInClonedCode)

       // printf ("Can't test for equality between these now, since they are no longer shared! \n");
       // ROSE_ASSERT(false);

          if (securityVulnerabilityAttributeInOriginalCode->get_securityVulnerabilityNodeInOriginalCode() == securityVulnerabilityAttributeInClonedCode->get_securityVulnerabilityNodeInOriginalCode())
             {
               printf ("***** Found primary vulnerability in clone ***** astNode = %p = %s rootOfClone = %p = %s \n",astNode,astNode->class_name().c_str(),rootOfClone,rootOfClone->class_name().c_str());
            // SgStatement* statement = isSgStatement(astNode);
            // ROSE_ASSERT(statement != NULL);
            // if (astNode == primaryVulnerabilityNodeInClone)
            //      addComment (astNode,std::string("*** NOTE Primary Node for clone: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttributeInClonedCode->vulnerabilityPointer->get_name() );

            // Add pointer to root of clone for this security vulnerability to the list
            // securityVulnerabilityAttributeInOriginalCode->set_associatedClones(rootOfClone);

            // Remove the SecurityVulnerabilityAttribute attribute (this should not call the descructor for the AstAttribute object)
               astNode->removeAttribute("SecurityVulnerabilityAttribute");

               if (astNode->attributeExists("PrimarySecurityVulnerabilityForCloneAttribute") == false)
                  {
                 // AstAttribute* primaryVulnerabilityAttribute = new PrimarySecurityVulnerabilityForCloneAttribute(astNode);
                    ROSE_ASSERT(securityVulnerabilityAttributeInClonedCode->vulnerabilityPointer != NULL);
                    PrimarySecurityVulnerabilityForCloneAttribute* primaryVulnerabilityAttribute =
                         new PrimarySecurityVulnerabilityForCloneAttribute(astNode,rootOfClone,securityVulnerabilityAttributeInClonedCode->vulnerabilityPointer);
                    ROSE_ASSERT(primaryVulnerabilityAttribute != NULL);

                 // astNode->addNewAttribute("SeededSecurityFlawCloneAttribute",primaryVulnerabilityAttribute);
                    astNode->addNewAttribute("PrimarySecurityVulnerabilityForCloneAttribute",primaryVulnerabilityAttribute);

                 // Note that this could be added to the constructor arguments to avoid having it to be set explicitly 
                 // (it is now set properly in securityVulnerabilityAttributeInOriginalCode->get_securityVulnerabilityNodeInOriginalCode() ).
                    ROSE_ASSERT(securityVulnerabilityAttributeInOriginalCode->get_securityVulnerabilityNode() != NULL);
                    primaryVulnerabilityAttribute->set_primaryVulnerabilityInOriginalCode(securityVulnerabilityAttributeInOriginalCode->get_securityVulnerabilityNode());

                 // if (astNode == primaryVulnerabilityNodeInClone)
                 // addComment (astNode,std::string("*** NOTE Primary Node for clone: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttributeInClonedCode->vulnerabilityPointer->get_name() );
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


// **********************************************************************
//             SecurityFlaw::CloneSeedLocationTraversal
// **********************************************************************

SecurityFlaw::CloneSeedLocation::CloneSeedLocationTraversal::CloneSeedLocationTraversal( SeedSecurityFlaw* Xptr )
   {
     ROSE_ASSERT(Xptr != NULL);
     associtedSeedSecurityFlaw = Xptr;
   }

SecurityFlaw::CloneSeedLocation::CloneSeedLocationTraversal::~CloneSeedLocationTraversal()
   {
     associtedSeedSecurityFlaw = NULL;
   }

void
SecurityFlaw::CloneSeedLocation::makeClones( SgProject* project, SeedSecurityFlaw* flaw )
   {
  // Build an AST traversal object
     CloneSeedLocationTraversal treeTraversal(flaw);

     InheritedAttribute inheritedAttribute;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
  // treeTraversal.traverseInputFiles (project,preorder);
     treeTraversal.traverseInputFiles (project,inheritedAttribute);
   }

void
SecurityFlaw::CloneSeedLocation::markPrimarySeedLocations( SgNode* primaryNodeInClonedCode, SgNode* primaryNodeInOriginalCode, SgNode* rootOfClone )
   {
  // Build an AST traversal object
     PrimarySeedLocationTraversal treeTraversal(primaryNodeInClonedCode,primaryNodeInOriginalCode,rootOfClone);

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
SecurityFlaw::CloneSeedLocation::PrimarySeedLocationTraversal::visit( SgNode* astNode )
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

          printf ("Can't test for equality between these now, since they are no longer shared! \n");
          ROSE_ASSERT(false);

          if (securityVulnerabilityAttributeInOriginalCode == securityVulnerabilityAttributeInClonedCode)
             {
               printf ("***** Found primary vulnerability in clone ***** astNode = %p = %s rootOfClone = %p = %s \n",astNode,astNode->class_name().c_str(),rootOfClone,rootOfClone->class_name().c_str());
            // SgStatement* statement = isSgStatement(astNode);
            // ROSE_ASSERT(statement != NULL);
            // if (astNode == primaryVulnerabilityNodeInClone)
            //      addComment (astNode,std::string("*** NOTE Primary Node for clone: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttributeInClonedCode->vulnerabilityPointer->get_name() );

            // Add pointer to root of clone for this security vulnerability to the list
            // securityVulnerabilityAttributeInOriginalCode->set_associatedClones(rootOfClone);

            // Remove the SecurityVulnerabilityAttribute attribute (this should not call the descructor for the AstAttribute object)
               astNode->removeAttribute("SecurityVulnerabilityAttribute");

               if (astNode->attributeExists("PrimarySecurityVulnerabilityForCloneAttribute") == false)
                  {
                 // AstAttribute* primaryVulnerabilityAttribute = new PrimarySecurityVulnerabilityForCloneAttribute(astNode);
                    ROSE_ASSERT(securityVulnerabilityAttributeInClonedCode->vulnerabilityPointer != NULL);
                    PrimarySecurityVulnerabilityForCloneAttribute* primaryVulnerabilityAttribute =
                         new PrimarySecurityVulnerabilityForCloneAttribute(astNode,rootOfClone,securityVulnerabilityAttributeInClonedCode->vulnerabilityPointer);
                    ROSE_ASSERT(primaryVulnerabilityAttribute != NULL);

                 // astNode->addNewAttribute("SeededSecurityFlawCloneAttribute",primaryVulnerabilityAttribute);
                    astNode->addNewAttribute("PrimarySecurityVulnerabilityForCloneAttribute",primaryVulnerabilityAttribute);

                    ROSE_ASSERT(securityVulnerabilityAttributeInOriginalCode->get_securityVulnerabilityNode() != NULL);
                    primaryVulnerabilityAttribute->set_primaryVulnerabilityInOriginalCode(securityVulnerabilityAttributeInOriginalCode->get_securityVulnerabilityNode());

                 // if (astNode == primaryVulnerabilityNodeInClone)
                 // addComment (astNode,std::string("*** NOTE Primary Node for clone: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttributeInClonedCode->vulnerabilityPointer->get_name() );
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
SecurityFlaw::CloneSeedLocation::InheritedAttribute
SecurityFlaw::CloneSeedLocation::CloneSeedLocationTraversal::evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute )
   {
     ROSE_ASSERT(astNode != NULL);

  // At this level were we are clonig to introduce different seeding methodologoes, we want to take actions 
  // at the PrimarySecurityVulnerabilityForCloneAttribute objects previously left in the AST from previous 
  // cloning and marking operations.
     AstAttribute* existingAttribute = astNode->getAttribute("PrimarySecurityVulnerabilityForCloneAttribute");
     PrimarySecurityVulnerabilityForCloneAttribute* securityVulnerabilityAttribute = dynamic_cast<PrimarySecurityVulnerabilityForCloneAttribute*>(existingAttribute);

     if (securityVulnerabilityAttribute != NULL)
        {
       // ROSE_ASSERT(securityVulnerabilityAttribute->get_securityVulnerabilityNode() == astNode);

          ROSE_ASSERT(associtedSeedSecurityFlaw != NULL);
          vector<SgNode*> grainularityAxis = associtedSeedSecurityFlaw->grainularityOfSeededCode(astNode);

       // Iterate from finest level of grainularity (e.g. SgExpression or SgStatement) to largest 
       // level of grainularity (e.g. SgFunctionDeclaration or SgFile).  This is most often a 
       // container of size == 1.
          printf ("grainularityAxis.size() = %lu \n",grainularityAxis.size());

#if 1
       // I think this is the better choice but I am not certain.
          vector<SgNode*>::reverse_iterator i = grainularityAxis.rbegin();
          while (i != grainularityAxis.rend())
#else
          vector<SgNode*>::iterator i = grainularityAxis.begin();
          while (i != grainularityAxis.end())
#endif
             {
            // Put code here to build AST copy and transform the copy!
               SgNode* subtree = *i;

               printf ("subtree         = %p = %s \n",subtree,subtree->class_name().c_str());

               SgStatement* subTreeStatement = isSgStatement(subtree);
               ROSE_ASSERT(subTreeStatement != NULL);

               ROSE_ASSERT(securityVulnerabilityAttribute->vulnerabilityPointer != NULL);
               printf ("XXXXXXX In SecurityFlaw::codeCloneGeneration(): associatedSeedingTechniques.size() = %zu \n",securityVulnerabilityAttribute->vulnerabilityPointer->associatedSeedingTechniques.size());

               std::set<SeedSecurityFlaw*>::iterator j = securityVulnerabilityAttribute->vulnerabilityPointer->associatedSeedingTechniques.begin();
               while (j != securityVulnerabilityAttribute->vulnerabilityPointer->associatedSeedingTechniques.end())
                  {
// ******************************
            // Make a copy of the expression used to hold the array size in the array declaration.
               SgTreeCopy subTreeCopyHelp;
               SgStatement* nearestWholeStatementCopy = isSgStatement(subtree->copy(subTreeCopyHelp));
               ROSE_ASSERT(nearestWholeStatementCopy != NULL);

               printf ("Calling resetAttributesAfterASTCloning... \n");
               resetAttributesAfterASTCloning(nearestWholeStatementCopy);
               printf ("DONE: Calling resetAttributesAfterASTCloning... \n");

            // DQ (7/26/2008): This is likely the most significant change from where this code was copied from the SecurityFlaw::CloneVulnerability class

            // Mark the original subtree that is being copied as the original (so that we can optionally permit not seeding the original code).
            // Note that we may be visiting this IR node for a second time so it might already have an existing SecurityFlawOriginalSubtreeAttribute.
            // AstAttribute* originalSubtreeAttribute = new SeedMethodologyCloneAttribute(nearestWholeStatementCopy,subtree,associtedSeedSecurityFlaw);
               AstAttribute* originalSubtreeAttribute = new SeedMethodologyCloneAttribute(nearestWholeStatementCopy,subtree,*j);
               ROSE_ASSERT(originalSubtreeAttribute != NULL);
               if (nearestWholeStatementCopy->attributeExists("SeedMethodologyCloneAttribute") == false)
                  {
                    nearestWholeStatementCopy->addNewAttribute("SeedMethodologyCloneAttribute",originalSubtreeAttribute);

                 // Also remove the copy that was make of the SecurityFlawOriginalSubtreeAttribute (of the grainularity of the cloning was the same).
                    if (nearestWholeStatementCopy->attributeExists("SeededSecurityFlawCloneAttribute") == true)
                       {
                         nearestWholeStatementCopy->removeAttribute("SeededSecurityFlawCloneAttribute");
                       }
                  }


#if 0
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
#endif

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
                    functionName += string("_SecurityFlawSeeded_function_") + StringUtility::numberToString(uniqueValueSeeding());
                    functionDeclaration->set_name(functionName);

                    printf ("functionName = %s \n",functionName.str());

                    ROSE_ASSERT(functionDeclaration->get_scope() != NULL);
                    SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(functionDeclaration);
                    SgName mangledName = functionDeclaration->get_mangled_name();
                    functionDeclaration->get_scope()->insert_symbol(functionName,functionSymbol);
                 // functionDeclaration->get_scope()->insert_symbol(mangledName,functionSymbol);
                    functionSymbol->set_parent(functionDeclaration->get_scope()->get_symbol_table());

                 // addComment (nearestWholeStatementCopy,"*** NOTE Function Containing Seeded Security Flaw: BufferOverFlowSecurityFlaw ");

                 // error checking
                    SgSymbol* local_symbol = functionDeclaration->get_symbol_from_symbol_table();
                    ROSE_ASSERT(local_symbol != NULL);
                  }

// ******************************
                    j++;
                  }

               i++;
             }
        }

     return inheritedAttribute;
   }


// **********************************************************************
//                    SecurityFlaw::MarkClonesTraversal
// **********************************************************************

SecurityFlaw::MarkClones::MarkClonesTraversal::MarkClonesTraversal( SecurityFlaw::SeedSecurityFlaw* Xptr )
   {
     ROSE_ASSERT(Xptr != NULL);
     associtedSeedSecurityFlaw = Xptr;
   }

SecurityFlaw::MarkClones::MarkClonesTraversal::~MarkClonesTraversal()
   {
     associtedSeedSecurityFlaw = NULL;
   }


void
SecurityFlaw::MarkClones::markVulnerabilitiesInClones( SgProject* project, SecurityFlaw::SeedSecurityFlaw* flaw )
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
SecurityFlaw::MarkClones::InheritedAttribute
SecurityFlaw::MarkClones::MarkClonesTraversal::evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute )
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

           // addComment (astNode,string("*** NOTE Original Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );
             }
            else
             {
            // DQ (7/27/2008): Now that we support deep copy on attributes this is false
            // ROSE_ASSERT(securityVulnerabilityAttribute->get_securityVulnerabilityNode() != astNode);

               if (inheritedAttribute.inClonedCode == true)
                  {
                 // addComment (astNode,std::string("*** NOTE Cloned Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );

#if 0
                    astNode->removeAttribute("SecurityVulnerabilityAttribute");

                 // Build a new SecurityVulnerabilityAttribute attribute to replace the one that was shared 
                 // with the origial AST (copy of AST attributes is not deep).
                    AstAttribute* newAttribute = new SecurityVulnerabilityAttribute(astNode,securityVulnerabilityAttribute->vulnerabilityPointer);
                    ROSE_ASSERT(newAttribute != NULL);

                 // We need to name the attributes, but all the VulnerabilityAttributes can all have the same name.
                 // It is easier to distinquish them by value (stored internally in the attribute). The use of
                 // names for attributes permits other forms of analysis to use attributes in the same AST and
                 // for all the forms of analysis to co-exist (so long as the select unique names).
                    astNode->addNewAttribute("SecurityVulnerabilityAttribute",newAttribute);
#else
                 // DQ (7/27/2008): Now that we have fixed this bug in ROSE we can do less work!
                    printf ("Because we now have deep copies on AST attributes, this code (building new SecurityVulnerabilityAttribute objects) is not required. \n");
#endif

                  }
                 else
                  {
                    printf ("Outside of of both original code and cloned code! astNode = %p = %s \n",astNode,astNode->class_name().c_str());
                  }
             }
        }

     return inheritedAttribute;
   }


// **********************************************************************
//                    SecurityFlaw::CommentClones
// **********************************************************************

void
SecurityFlaw::CommentClones::commentClones( SgProject* project )
   {
  // Build an AST traversal object
     CommentClonesTraversal treeTraversal;

  // This traverses only the input source file (to traverse all header file 
  // and the source file call "traverse" instead of "traverseInputFiles").
  // treeTraversal.traverseInputFiles (project,preorder);
     treeTraversal.traverseInputFiles (project,preorder);
   }


// void BufferOverFlowSecurityFlaw::CloneVulnerabilityTraversal::visit( SgNode* astNode )
void
SecurityFlaw::CommentClones::CommentClonesTraversal::visit ( SgNode* astNode )
   {
  // This traversal adds comments into the generated source code to indicate where security 
  // flaws were identified and where clones were made and where flaws were seeded.

     ROSE_ASSERT(astNode != NULL);
  // printf ("In CommentClonesTraversal::visit(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());

     if (astNode->attributeExists("SecurityFlawOriginalSubtreeAttribute") == true)
        {
       // printf ("Found a SecurityFlawOriginalSubtreeAttribute \n");
#if 0
          AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
          SecurityVulnerabilityAttribute* securityVulnerabilityAttribute = dynamic_cast<SecurityVulnerabilityAttribute*>(existingAttribute);

       // addComment(astNode,"This is a SecurityFlawOriginalSubtreeAttribute");
          ROSE_ASSERT(securityVulnerabilityAttribute != NULL);
          ROSE_ASSERT(securityVulnerabilityAttribute->vulnerabilityPointer != NULL);
          addComment (astNode,string("*** NOTE Original Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );
#else
          addComment (astNode,"This is a SecurityFlawOriginalSubtreeAttribute ");
#endif
        }

     if (astNode->attributeExists("SeededSecurityFlawCloneAttribute") == true)
        {
          printf ("Found a SeededSecurityFlawCloneAttribute \n");
#if 0
          AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
          SeededSecurityFlawCloneAttribute* securityVulnerabilityAttribute = dynamic_cast<SeededSecurityFlawCloneAttribute*>(existingAttribute);
#endif
       // addComment(astNode,"This is a SeededSecurityFlawCloneAttribute");
       // addComment (astNode,string("*** NOTE Cloned Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );
          addComment (astNode,string("*** NOTE Cloned Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ") );
        }

     if (astNode->attributeExists("PrimarySecurityVulnerabilityForCloneAttribute") == true)
        {
          printf ("Found a PrimarySecurityVulnerabilityForCloneAttribute \n");
          AstAttribute* existingAttribute = astNode->getAttribute("PrimarySecurityVulnerabilityForCloneAttribute");
          PrimarySecurityVulnerabilityForCloneAttribute* securityVulnerabilityAttribute = dynamic_cast<PrimarySecurityVulnerabilityForCloneAttribute*>(existingAttribute);

       // addComment(astNode,"This is a SeededSecurityFlawCloneAttribute");
          addComment (astNode,std::string("*** NOTE Primary Node for clone: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );
        }
   }


// **********************************************************************
//                    SecurityFlaw::SeedSecurityFlaw
// **********************************************************************

SecurityFlaw::SeedSecurityFlaw::SeedSecurityFlaw()
   {
     seedOriginalCode = false;
   }

SecurityFlaw::SeedSecurityFlaw::~SeedSecurityFlaw()
   {
  // Nothing to do here
   }

bool
SecurityFlaw::SeedSecurityFlaw::get_seedOriginalCode()
   {
     return seedOriginalCode;
   }

void
SecurityFlaw::SeedSecurityFlaw::set_seedOriginalCode( bool t )
   {
     seedOriginalCode = t;
   }

vector<SgNode*>
SecurityFlaw::SeedSecurityFlaw::grainularityOfSeededCode ( SgNode* astNode )
   {
  // Loop through the parents of the input node to gather the locations of possible subtrees 
  // that we will copy and transform (to seed security flaws into).  It could be all subtrees,
  // it perhaps only the expressions or statements where the security flaw vulnerability is
  // defined.

     vector<SgNode*> returnVector;

     printf ("seedGrainulatity.get_testAllLevels()     = %s \n",seedGrainulatity.get_testAllLevels()     ? "true" : "false");
     printf ("seedGrainulatity.get_grainularityLevel() = %d \n",seedGrainulatity.get_grainularityLevel());

     bool atRoot = false;
     while ( atRoot == false )
        {
          SgStatement* statement = isSgStatement(astNode);
          if (statement != NULL)
             {
               printf ("seed statement = %p = %s \n",statement,statement->class_name().c_str());

            // Make sure this is a statement which we can insert as a subtree.  For example, it is useless to
            // clone a body of a "for" loop since we can't insert it into the for loop (because a for loop can 
            // have only a single body).
               if ( isSgBasicBlock(statement) == NULL && isSgFunctionDefinition(statement) == NULL )
                  {
                    if ( seedGrainulatity.get_testAllLevels() == true)
                       {
                      // This option results in too many clones and vulnerabilities being introduces (generates messy code).
                         printf ("Adding this statement to the vector of possible subtrees: statement = %p = %s \n",statement,statement->class_name().c_str());
                         returnVector.push_back(statement);
                       }
                      else
                       {
                      // Check if this is a function declaration and the grainularity specified ask for the function level
                         if ( seedGrainulatity.get_grainularityLevel() == GrainularitySpecification::e_function && isSgFunctionDeclaration(statement) != NULL )
                            {
                           // This may be the more useful option.  But to obtain the same control flow in the problem 
                           // call sites to the function cloned may have to be transformed to call each of the generated 
                           // cloned functions as well.
                              printf ("Adding this statement (SgFunctionDeclaration) to the vector of possible subtrees: statement = %p = %s \n",statement,statement->class_name().c_str());
                              returnVector.push_back(statement);
                            }
                           else
                            {
                              if ( seedGrainulatity.get_grainularityLevel() == GrainularitySpecification::e_statement && isSgStatement(statement) != NULL )
                                 {
                                // Unclear if this is useful, since it make the generated code overly complex.
                                   printf ("Adding this statement to the vector of possible subtrees: statement = %p = %s \n",statement,statement->class_name().c_str());
                                   returnVector.push_back(statement);
                                 }
                                else
                                 {
                                // nothing to do
                                   printf ("This case for where to support levels of grainulatity is not implemented! statement = %p = %s \n",statement,statement->class_name().c_str());
                                 }
                            }
                       }

                 // Use this to just execute a single case!
                 // break;
                  }
             }

          astNode = astNode->get_parent();
          ROSE_ASSERT(astNode != NULL);

       // Stop when we get to the global scope.
          atRoot = (isSgGlobal(astNode) != NULL);
        }

#if 0
     printf ("Exiting at base of grainularity generator \n");
     ROSE_ASSERT(false);
#endif

     return returnVector;
   }


// ***********************************************************
//                 GrainularitySpecification
// ***********************************************************

GrainularitySpecification::GrainularitySpecification()
   {
  // Select a default (not all are implemented):
  //    e_unknown
  //    e_expression
  //    e_statement
  //    e_function
  //    e_class
  //    e_file

  // I think this should be the default, since it is the most useful
     grainularityLevel   = e_function;

  // This option generates messy code in general (default == false).
     testAllLevels       = false;

  // This option only makes sense for e_expression, e_statement, and maybe e_class (where classes are nested)
     enclosingScopeDepth = 0;
   }

GrainularitySpecification::~GrainularitySpecification()
   {
  // Nothing to do here!
   }


void
GrainularitySpecification::set_enclosingScopeDepth( int n ) 
   {
     enclosingScopeDepth = n;
   }

int
GrainularitySpecification::get_enclosingScopeDepth() 
   {
     return enclosingScopeDepth;
   }

void
GrainularitySpecification::set_testAllLevels( bool t ) 
   {
     testAllLevels = t;
   }

bool
GrainularitySpecification::get_testAllLevels() 
   {
     return testAllLevels;
   }

void
GrainularitySpecification::set_grainularityLevel( GrainularitySpecification::GranularityLevelEnum t ) 
   {
     testAllLevels = t;
   }

GrainularitySpecification::GranularityLevelEnum
GrainularitySpecification::get_grainularityLevel() 
   {
     return grainularityLevel;
   }



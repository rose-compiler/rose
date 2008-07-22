// This is where we put the suppprting code for bug seeding.

#include "rose.h"

using namespace std;
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
     CloneVulnerability::makeClones(project,*j);
     MarkClones::markVulnerabilitiesInClones(project,*j);
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
     vector<SecurityFlaw*>::iterator i = securityFlawCollection.begin();
     while (i != securityFlawCollection.end())
        {
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


// ***************************
// Supporting Member Functions
// ***************************

void
SecurityFlaw::addComment( SgNode* astNode, string comment )
   {
  // This function adds a comment before the statement contained by the input IR node's associated statement IR node.
     SgStatement* associatedStatement = TransformationSupport::getStatement(astNode);

     SageInterface::attachComment(associatedStatement,comment);
   }

int
SecurityFlaw::uniqueValue()
   {
  // This function retruns a unique integer value and is used to build names of functions, 
  // variable, etc. to avoid name collisions.

     static int i = 0;
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
                    addComment (astNode,std::string("*** NOTE Primary Node for clone: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttributeInClonedCode->vulnerabilityPointer->get_name() );
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
                    functionName += string("_SecurityFlawSeeded_function_") + StringUtility::numberToString(uniqueValue());
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

               addComment (astNode,string("*** NOTE Original Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );
             }
            else
             {
               ROSE_ASSERT(securityVulnerabilityAttribute->get_securityVulnerabilityNode() != astNode);

               if (inheritedAttribute.inClonedCode == true)
                  {
                    addComment (astNode,std::string("*** NOTE Cloned Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );

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
     ROSE_ASSERT(astNode != NULL);

#if 0
     printf ("astNode                           = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

     if (astNode->attributeExists("SecurityFlawOriginalSubtreeAttribute") == true)
        {
          printf ("Found a SecurityFlawOriginalSubtreeAttribute \n");
          AstAttribute* existingAttribute = astNode->getAttribute("SecurityVulnerabilityAttribute");
          SecurityVulnerabilityAttribute* securityVulnerabilityAttribute = dynamic_cast<SecurityVulnerabilityAttribute*>(existingAttribute);

       // addComment(astNode,"This is a SecurityFlawOriginalSubtreeAttribute");
          addComment (astNode,string("*** NOTE Original Security Flaw Vulnerability: BufferOverFlowSecurityFlaw ") + securityVulnerabilityAttribute->vulnerabilityPointer->get_name() );
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
                         printf ("Adding this statement to the vector of possible subtrees: statement = %p = %s \n",statement,statement->class_name().c_str());
                         returnVector.push_back(statement);
                       }
                      else
                       {
                      // Check if this is a function declaration and the grainularity specified ask for the function level
                         if ( seedGrainulatity.get_grainularityLevel() == GrainularitySpecification::e_function && isSgFunctionDeclaration(statement) != NULL )
                            {
                              printf ("Adding this statement (SgFunctionDeclaration) to the vector of possible subtrees: statement = %p = %s \n",statement,statement->class_name().c_str());
                              returnVector.push_back(statement);
                            }
                           else
                            {
                              if ( seedGrainulatity.get_grainularityLevel() == GrainularitySpecification::e_statement && isSgStatement(statement) != NULL )
                                 {
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



void
SecurityVulnerabilityAttribute::set_associatedClones(SgNode* node)
   {
     ROSE_ASSERT(node != NULL);
     printf ("Inside of SecurityVulnerabilityAttribute::set_associatedClones node = %p \n",node);
     if (associtedClones.find(node) == associtedClones.end())
        {
          printf ("Adding node = %p to associtedClones set \n",node);
          associtedClones.insert(node);
        }
   }

SecurityVulnerabilityAttribute::SecurityVulnerabilityAttribute (SgNode* securityVulnerabilityNode, SecurityFlaw::Vulnerability* vulnerabilityPointer)
   : securityVulnerabilityNode(securityVulnerabilityNode), vulnerabilityPointer(vulnerabilityPointer)
   {
   }

SgNode*
SecurityVulnerabilityAttribute::get_securityVulnerabilityNode()
   {
     return securityVulnerabilityNode;
   }

void
SecurityVulnerabilityAttribute::set_securityVulnerabilityNode(SgNode* node)
   {
     securityVulnerabilityNode = node;
   }

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
SecurityVulnerabilityAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     set<SgNode*>::iterator i = associtedClones.begin();
     while ( i != associtedClones.end() )
        {
          ROSE_ASSERT(securityVulnerabilityNode != NULL);
          printf ("Adding an edge from %p = %s to %p = %s \n",securityVulnerabilityNode,securityVulnerabilityNode->class_name().c_str(),*i,(*i)->class_name().c_str());
          AstAttribute::AttributeEdgeInfo edge (securityVulnerabilityNode,*i,"associated vulnerabilities","");

          v.push_back(edge);

          i++;
        }

  // string vulnerabilityName = "SecurityVulnerabilityAttribute" + vulnerabilityPointer->get_name();
     string vulnerabilityName = vulnerabilityPointer->get_name();
     string vulnerabilityColor = vulnerabilityPointer->get_color();

  // Note that we need the trailing " "
     string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=" + vulnerabilityColor + " ";
  // string options = " constraint=false color=deepskyblue4";
  // string options = " constraint=false color=deepskyblue4 ";
  // printf ("vulnerabilityOptions = %s \n",vulnerabilityOptions.c_str());

  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute"," minlen=1 splines=\"false\" arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=deepskyblue4 ");
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute"," maxlen=\"1\" arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=deepskyblue4 ");
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute"," maxlen=\"1\" arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=" + color );
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute",options);
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute","");
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute",options);
     AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,vulnerabilityName,vulnerabilityOptions);
     v.push_back(additional_edge);

#if 0
     printf ("Exiting at base of SecurityVulnerabilityAttribute::additionalEdgeInfo() \n");
     ROSE_ASSERT(false);
#endif
     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
SecurityVulnerabilityAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     string vulnerabilityName    = " " + vulnerabilityPointer->get_name();
     string vulnerabilityColor   = vulnerabilityPointer->get_color();
     string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false fillcolor=" + vulnerabilityColor + ",style=filled ";

  // AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, "SecurityVulnerabilityAttribute"," fillcolor=\"red\",style=filled ");
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, vulnerabilityName, vulnerabilityOptions);
     v.push_back(vulnerabilityNode);

     return v;
   }















// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
SecurityFlawOriginalSubtreeAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
SecurityFlawOriginalSubtreeAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;
#if 0
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) this, "SecurityFlawOriginalSubtreeAttribute"," fillcolor=\"darkorange\",style=filled ");
     v.push_back(vulnerabilityNode);
#endif
     return v;
   }







SeededSecurityFlawCloneAttribute::SeededSecurityFlawCloneAttribute (SgNode* primarySecurityFlawInClone, SgNode* rootOfCloneInOriginalCode)
   : primarySecurityFlawInClone(primarySecurityFlawInClone), 
     rootOfCloneInOriginalCode(rootOfCloneInOriginalCode)
   {
   }

SgNode*
SeededSecurityFlawCloneAttribute::get_primarySecurityFlawInClone()
   {
     return primarySecurityFlawInClone;
   }

void
SeededSecurityFlawCloneAttribute::set_primarySecurityFlawInClone(SgNode* node)
   {
     primarySecurityFlawInClone = node;
   }


SgNode*
SeededSecurityFlawCloneAttribute::get_rootOfCloneInOriginalCode()
   {
     return rootOfCloneInOriginalCode;
   }

void
SeededSecurityFlawCloneAttribute::set_rootOfCloneInOriginalCode(SgNode* node)
   {
     rootOfCloneInOriginalCode = node;
   }

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
SeededSecurityFlawCloneAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(rootOfCloneInOriginalCode != NULL);

     AstAttribute::AttributeEdgeInfo edgeToRootOfCloneInOriginalCode (primarySecurityFlawInClone,rootOfCloneInOriginalCode,"root of clone in original code"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" color=orange ");

     v.push_back(edgeToRootOfCloneInOriginalCode);

     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
SeededSecurityFlawCloneAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(rootOfCloneInOriginalCode != NULL);
#if 0
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) this, "SeededSecurityFlawCloneAttribute"," fillcolor=\"springgreen\",style=filled ");
     v.push_back(vulnerabilityNode);
#endif
     return v;
   }


















PrimarySecurityVulnerabilityForCloneAttribute::PrimarySecurityVulnerabilityForCloneAttribute(SgNode* primarySecurityFlawInClone, SgNode* rootOfClone, SecurityFlaw::Vulnerability* vulnerabilityPointer)
   : primarySecurityFlawInClone(primarySecurityFlawInClone), rootOfClone(rootOfClone), vulnerabilityPointer(vulnerabilityPointer)
   {
     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(rootOfClone != NULL);
     ROSE_ASSERT(vulnerabilityPointer != NULL);
   }

SgNode*
PrimarySecurityVulnerabilityForCloneAttribute::get_primarySecurityFlawInClone()
   {
     return primarySecurityFlawInClone;
   }

void
PrimarySecurityVulnerabilityForCloneAttribute::set_primaryVulnerabilityInOriginalCode(SgNode* node)
   {
     primaryVulnerabilityInOriginalCode = node;
   }

SgNode*
PrimarySecurityVulnerabilityForCloneAttribute::get_primaryVulnerabilityInOriginalCode()
   {
     return primaryVulnerabilityInOriginalCode;
   }

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
PrimarySecurityVulnerabilityForCloneAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(primaryVulnerabilityInOriginalCode != NULL);

     AstAttribute::AttributeEdgeInfo edgeToOriginalCode (primarySecurityFlawInClone,primaryVulnerabilityInOriginalCode,"vulnerabilities in original code"," arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=red ");
  // AstAttribute::AttributeEdgeInfo edgeToOriginalCode (primarySecurityFlawInClone,primaryVulnerabilityInOriginalCode,"vulnerabilities in original code"," arrowsize=7.0 style=\"setlinewidth(7)\" color=red ");
  // AstAttribute::AttributeEdgeInfo edgeToRootOfClone (primarySecurityFlawInClone,rootOfClone,"root of clone"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" color=springgreen ");
     AstAttribute::AttributeEdgeInfo edgeToRootOfClone (primarySecurityFlawInClone,rootOfClone,"root of clone"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" color=firebrick ");

     v.push_back(edgeToOriginalCode);
     v.push_back(edgeToRootOfClone);

     ROSE_ASSERT(vulnerabilityPointer != NULL);
     string vulnerabilityName  = vulnerabilityPointer->get_name();
     string vulnerabilityColor = vulnerabilityPointer->get_color();

  // Note that we need the trailing " ", plus forcing constraint=true places the node nearest the original node
     string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=true color=" + vulnerabilityColor + " ";
#if 1
     AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,primarySecurityFlawInClone,vulnerabilityName,vulnerabilityOptions);
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,primaryVulnerabilityInOriginalCode,vulnerabilityName,vulnerabilityOptions);
     v.push_back(additional_edge);
#endif
     return v;
   }


std::vector<AstAttribute::AttributeNodeInfo>
PrimarySecurityVulnerabilityForCloneAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(primaryVulnerabilityInOriginalCode != NULL);

#if 0
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) this, "PrimarySecurityVulnerabilityForCloneAttribute"," fillcolor=\"purple\",style=filled ");
     v.push_back(vulnerabilityNode);
#endif

     return v;
   }



/* Colors: (from http://www.graphviz.org/doc/info/colors.html)
aquamarine aliceblue 	antiquewhite 	antiquewhite1 	antiquewhite2 	antiquewhite3
antiquewhite4 	aquamarine 	aquamarine1 	aquamarine2 	aquamarine3
aquamarine4 	azure 	azure1 	azure2 	azure3
azure4 	beige 	bisque 	bisque1 	bisque2
bisque3 	bisque4 	black 	blanchedalmond 	   blue   
blue1 	blue2 	blue3 	blue4 	blueviolet
brown 	brown1 	brown2 	brown3 	brown4
burlywood 	burlywood1 	burlywood2 	burlywood3 	burlywood4
cadetblue 	cadetblue1 	cadetblue2 	cadetblue3 	cadetblue4
chartreuse 	chartreuse1 	chartreuse2 	chartreuse3 	chartreuse4
chocolate 	chocolate1 	chocolate2 	chocolate3 	chocolate4
coral 	coral1 	coral2 	coral3 	coral4
cornflowerblue 	cornsilk 	cornsilk1 	cornsilk2 	cornsilk3
cornsilk4 	crimson 	   cyan    	cyan1 	cyan2
cyan3 	cyan4 	darkgoldenrod 	darkgoldenrod1 	darkgoldenrod2
darkgoldenrod3 	darkgoldenrod4 	darkgreen 	darkkhaki 	darkolivegreen
darkolivegreen1 	darkolivegreen2 	darkolivegreen3 	darkolivegreen4 	darkorange
darkorange1 	darkorange2 	darkorange3 	darkorange4 	darkorchid
darkorchid1 	darkorchid2 	darkorchid3 	darkorchid4 	darksalmon
darkseagreen 	darkseagreen1 	darkseagreen2 	darkseagreen3 	darkseagreen4
darkslateblue 	darkslategray 	darkslategray1 	darkslategray2 	darkslategray3
darkslategray4 	darkslategrey 	darkturquoise 	darkviolet 	deeppink
deeppink1 	deeppink2 	deeppink3 	deeppink4 	deepskyblue
deepskyblue1 	deepskyblue2 	deepskyblue3 	deepskyblue4 	dimgray
dimgrey 	dodgerblue 	dodgerblue1 	dodgerblue2 	dodgerblue3
dodgerblue4 	firebrick 	firebrick1 	firebrick2 	firebrick3
firebrick4 	floralwhite 	forestgreen 	gainsboro 	ghostwhite
gold    	gold1 	gold2 	gold3 	gold4
goldenrod 	goldenrod1 	goldenrod2 	goldenrod3 	goldenrod4
gray
*/

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
SecurityVulnerabilityAttribute::additionalNodeOptions()
   {
     return "fillcolor=\"red\",style=filled";
   }

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
SecurityFlawOriginalSubtreeAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"deepskyblue\",style=filled";
     return "fillcolor=\"orange\",style=filled";
   }

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
SeededSecurityFlawCloneAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"yellow\",style=filled";
  // return "fillcolor=\"springgreen\",style=filled";
  // return "fillcolor=\"firebrick\",style=filled";
     return "fillcolor=\"cyan\",style=filled";
   }

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
PrimarySecurityVulnerabilityForCloneAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"greenyellow\",style=filled";
  // return "fillcolor=\"magenta\",style=filled";
  // return "fillcolor=\"aquamarine\",style=filled";
  // return "fillcolor=\"cyan\",style=filled";
     return "fillcolor=\"firebrick\",style=filled";
   }


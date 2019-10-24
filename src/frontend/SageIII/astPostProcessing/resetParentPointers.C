// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "rose_config.h"

#include "resetParentPointers.h"
#include "astPostProcessing.h"
// tps (01/14/2009): Had to define this locally as it is not part of sage3 but rose.h
#include "AstDiagnostics.h"

#define DEBUG_PARENT_INITIALIZATION 0

// DQ (9/24/2007): This is Gergo's fix for the AST islands that were previously not traversed in the AST.
#define FIXED_ISLAND_TRAVERSAL

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/6/2017): Added support for message logging to control output from ROSE tools.
#undef mprintf
#define mprintf Rose::Diagnostics::mfprintf(Rose::ir_node_mlog[Rose::Diagnostics::DEBUG])

// [DQ]
// Declaration matching static member data
// list<string> ResetParentPointers::modifiedNodeInformationList;

#if 0
// [DQ]
// Allocation of space for listOfTraversedTypes declared in dqTraversal
list<SgNode*> ResetParentPointers::listOfTraversedTypes;
#endif


// [DQ]
void
ResetParentPointers::traceBackToRoot ( SgNode* node )
   {
  // DQ (9/24/2007): Put this back since it is insifnicant to the performance.
  // DQ (9/24/2007): Comment out to check the performance, this test should really be done in the AST consistancy tests.
#if 1
  // Trace the current node back as far as possible (should be able to reach SgGlobal)
     SgNode* parentNode = node;

  // printf ("Starting at parentNode->sage_class_name() = %s \n",parentNode->sage_class_name());
     int counter = 0;
     while (parentNode->get_parent() != NULL)
        {
          parentNode = parentNode->get_parent();
       // printf ("     parentNode->sage_class_name() = %s \n",parentNode->sage_class_name());
          if (counter > 1000)
             {
            // There is likely an error so limit path lengths back to the AST root to this arbitrary distance
               printf ("Error: ResetParentPointers::traceBackToRoot path to root length (1000) exceeded \n");
               ROSE_ASSERT(parentNode != NULL);
               ROSE_ASSERT(parentNode->get_parent() != NULL);
               printf ("   starting node           = %p = %s \n",node,node->sage_class_name());
               printf ("   (current node)          = %p = %s \n",parentNode,parentNode->sage_class_name());
               printf ("   (current node's parent) = %p = %s \n",parentNode->get_parent(),parentNode->get_parent()->sage_class_name());

               ROSE_ASSERT(parentNode->get_parent()->get_parent() != NULL);
               printf ("   (parent node's parent)  = %p = %s \n",
                    parentNode->get_parent()->get_parent(),parentNode->get_parent()->get_parent()->sage_class_name());

               ROSE_ASSERT(parentNode->get_parent()->get_parent()->get_parent() != NULL);
               printf ("   (parent node's parent 2)  = %p = %s \n",
                    parentNode->get_parent()->get_parent()->get_parent(),parentNode->get_parent()->get_parent()->get_parent()->sage_class_name());

               if (node->get_file_info() != NULL)
                    node->get_file_info()->display("node");
               if (parentNode->get_file_info() != NULL)
                    parentNode->get_file_info()->display("parentNode");
               if (parentNode->get_parent()->get_file_info() != NULL)
                    parentNode->get_parent()->get_file_info()->display("parentNode->get_parent()");

               ROSE_ASSERT(false);
             }

          counter++;
        }

  // DQ (2/26/2004): Fixed in Sage to pass SgParent to SgFile constructor so that SgProject 
  //                 could be know earily in the construction of the SgFile to support template 
  //                 instantiation and evaluation of SgProject commandLine.
  // Check to see if we made it back to the root (current root is SgFile, later it will be SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).

  // DQ (5/13/2004): The rot can sometimes be a SgFile node (as in the case of the rewrite mechanism)
  // if ( !( (isSgFile(parentNode) != NULL) || (isSgProject(parentNode) != NULL) ) )
  // Appling DeMorgan's Rule to the previous statement we get the simpler form
     if ( (isSgFile(parentNode) == NULL) && (isSgProject(parentNode) == NULL) )
        {
       // DQ (10/21/2004): This is relaxed to allow setting of parent pointers from manually constructed code!
#if STRICT_ERROR_CHECKING
       // Need to make this optional in some way so that this output is not disturbing to users (but useful for debugging)!
          printf ("Errors: could not trace back to SgFile node, path taken: \n");
          printf ("Starting at AST node->sage_class_name() = %s \n",node->sage_class_name());
          SgLocatedNode* locatedNode = isSgLocatedNode(node);
          if (locatedNode != NULL)
             {
               printf ("initial node in failing path to root = %p = %s \n",locatedNode,locatedNode->sage_class_name());
               locatedNode->get_file_info()->display("problem AST node");
             }
          SgNode* parentNode = node;
          while (parentNode->get_parent() != NULL)
             {
               parentNode = parentNode->get_parent();
               printf ("     ParentNode->sage_class_name() = %s \n",parentNode->sage_class_name());
               SgLocatedNode* locatedNode = isSgLocatedNode(parentNode);
               if (locatedNode != NULL)
                  {
                    printf ("initial node in failing path to root = %p = %s \n",locatedNode,locatedNode->sage_class_name());
                    locatedNode->get_file_info()->display("problem AST node");
                  }
             }
#else
       // This is not an issue when AST post processing is done within construction of AST fragements (e.g. loop processing)
       // printf ("Relaxed Error Checking: Commented out debugging output in ResetParentPointers::traceBackToRoot(%s) traced to parent = %s \n",
       //      node->sage_class_name(),parentNode->sage_class_name());
#endif
       // DQ (10/21/2004): This is relaxed to allow setting of parent pointers from manually constructed code!
       // ROSE_ASSERT(false);
        }
#endif
   }

void 
ResetParentPointers::resetParentPointersInDeclaration (SgDeclarationStatement* declaration, SgNode* inputParent )
   {
  // This function makes the value in the parent consistent across the defining and non-defining declarations

  // DQ (5/21/2006): Set the Sg_File_Info so that they can be traced (later we might 
  // want to remove the parent pointer since it is not really required, except that 
  // it is helpful for debugging). This allows declaration in islands to be set!
  // declaration->get_startOfConstruct()->set_parent(declaration);
  // declaration->get_endOfConstruct()->set_parent(declaration);
     Sg_File_Info* fileInfoStart = declaration->get_startOfConstruct();
     if (fileInfoStart != NULL)
        {
          if (fileInfoStart->get_parent() == NULL)
             {
               fileInfoStart->set_parent(declaration);
             }
        }
     Sg_File_Info* fileInfoEnd = declaration->get_endOfConstruct();
     if (fileInfoEnd != NULL)
        {
          if (fileInfoEnd->get_parent() == NULL)
             {
               fileInfoEnd->set_parent(declaration);
             }
        }

  // DQ (10/12/2004): Refactored common code for setting parents in defining and non-defining declarations
     ROSE_ASSERT(declaration != NULL);
     SgDeclarationStatement* nondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
     SgDeclarationStatement* definingDeclaration    = declaration->get_definingDeclaration();

     SgNode* parent = NULL;
     if (definingDeclaration != NULL)
          parent = definingDeclaration->get_parent();
     if (parent == NULL && nondefiningDeclaration != NULL)
          parent = nondefiningDeclaration->get_parent();

     ROSE_ASSERT (definingDeclaration != NULL || nondefiningDeclaration != NULL);
#if 0
     printf ("In ResetParentPointers::resetParentPointersInDeclaration(): declaration = %p = %s definingDeclaration = %p nondefiningDeclaration = %p \n",
          declaration,declaration->class_name().c_str(),definingDeclaration,nondefiningDeclaration);
#endif

     if (definingDeclaration != NULL || nondefiningDeclaration != NULL)
        {
          if (parent == NULL)
             {
            // DQ (10/12/2004): If neither the defining nor non-defining declaration has its parent 
            // set then this is likely the first time the parent is being set for any associated 
            // declaration (for the current object declared).  In this case set the parent to be 
            // the parent passed in as a parameter to this function.

               printf ("In ResetParentPointers::resetParentPointersInDeclaration(): using the inputParent = %p = %s = %s as a parent for declaration \n",inputParent,inputParent->class_name().c_str(),SageInterface::get_name(inputParent).c_str());
               parent = inputParent;
             }

          ROSE_ASSERT(parent != NULL);
#if DEBUG_PARENT_INITIALIZATION || 0
          printf ("parent of declaration = %p = %s = %s (defining or non-defining) is %p = %s \n",
               declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str(),
               parent,parent->class_name().c_str());
#endif
       // DQ (10/9/2004): Avoid resetting any parents that are set to scope 
       // statements (these are likely already correct, either a forward 
       // declaration or a normal defining class declaration).
          if (definingDeclaration != NULL && definingDeclaration->get_parent() != NULL)
             {
            // SgNode* currentParent = definingDeclaration->get_parent();
            // printf ("AST Fixup warning: definingDeclaration already has parent = %p = %s \n",currentParent,currentParent->class_name().c_str());
             }

       // DQ (5/21/2006): Set the parent of the defining declaration (can be a non SgScopeStatement)
       // if (definingDeclaration != NULL && isSgScopeStatement(definingDeclaration->get_parent()) == NULL)
          if (definingDeclaration != NULL && definingDeclaration->get_parent() == NULL)
             {
#if DEBUG_PARENT_INITIALIZATION
               printf ("AST Fixup: Setting parent of definingDeclaration = %p = %s (to parent = %p = %s) \n",
                    definingDeclaration,definingDeclaration->class_name().c_str(),
                    parent,parent->class_name().c_str());
#endif
               if (definingDeclaration->get_parent() != NULL)
                  {
                    printf ("Existing parent is already set to = %p = %s \n",
                         definingDeclaration->get_parent(),definingDeclaration->get_parent()->class_name().c_str());
                  }
               ROSE_ASSERT(definingDeclaration->get_parent() == NULL);
               definingDeclaration->set_parent(parent);
             }
          ROSE_ASSERT(definingDeclaration == NULL || definingDeclaration->get_parent() != NULL);

          if (nondefiningDeclaration != NULL && nondefiningDeclaration->get_parent() != NULL)
             {
            // SgNode* currentParent = nondefiningDeclaration->get_parent();
            // printf ("AST Fixup warning: nondefiningDeclaration already has parent = %p = %s \n",currentParent,currentParent->class_name().c_str());
             }
       // DQ (5/21/2006): Set the parent of the defining declaration (can be a non SgScopeStatement)
       // if (nondefiningDeclaration != NULL && isSgScopeStatement(nondefiningDeclaration->get_parent()) == NULL)
          if (nondefiningDeclaration != NULL && nondefiningDeclaration->get_parent() == NULL)
             {
#if DEBUG_PARENT_INITIALIZATION
               printf ("AST Fixup: Setting parent of nondefiningDeclaration = %p = %s (to parent = %p = %s) \n",
                    nondefiningDeclaration,nondefiningDeclaration->class_name().c_str(),
                    parent,parent->class_name().c_str());
#endif
               if (nondefiningDeclaration->get_parent() != NULL)
                  {
#if DEBUG_PARENT_INITIALIZATION
                    printf ("Existing parent is already set to = %p = %s \n",
                         nondefiningDeclaration->get_parent(),nondefiningDeclaration->get_parent()->class_name().c_str());
                    nondefiningDeclaration->get_file_info()->display("Called from reset parent: nondefiningDeclaration");
                    isSgLocatedNode(nondefiningDeclaration->get_parent())->get_file_info()->display("Called from reset parent: parent");
#endif
                  }

            // DQ (5/21/2006): We can uncomment this now (reasserted constraint)
               ROSE_ASSERT(nondefiningDeclaration->get_parent() == NULL);
               nondefiningDeclaration->set_parent(parent);
             }
          ROSE_ASSERT(nondefiningDeclaration == NULL || nondefiningDeclaration->get_parent() != NULL);
        }

  // DQ (10/12/2004): Check and see if this is always true!
     ROSE_ASSERT(declaration->get_parent() != NULL);

  // Now  set the current parent (if not already set) to the same parent pointer 
     if (declaration->get_parent() == NULL)
        {
#if DEBUG_PARENT_INITIALIZATION
          printf ("AST Fixup: Setting parent of declaration = %p = %s (to parent = %p = %s) \n",
               declaration,declaration->class_name().c_str(),
               parent,parent->class_name().c_str());
#endif
          declaration->set_parent(parent);
        }
     ROSE_ASSERT(declaration->get_parent() != NULL);

  // DQ (10/17/2004): Added assertions
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
     if (classDeclaration != NULL)
        {
       // DQ (10/22/2005): Changed semantics to make forward declaration have a null pointer!
       // DQ (older comment) Note that all classDeclarations have a valid pointer to their definition independent 
       // of if they are defining or non-defining declarations, this is different from functions which only have 
       // a valid pointer to their definition if they are a defining declaration.  I like that this removed 
       // opportunities for NULL pointers in the AST, but it can make it confusing for users who might only check 
       // for a definition and assume it is a defining declaration if the definition is found.
       // ROSE_ASSERT(classDeclaration->get_definition() != NULL);
#if 0
          if (classDeclaration->isForward() == true)
               ROSE_ASSERT(classDeclaration->get_definition() == NULL);
#endif

       // DQ (10/22/2005): This is now conditional on having a valid definition
          if (classDeclaration->get_definition() != NULL)
             {
            // DQ (10/17/2004): We have to set the definition uniformally so that even definitions that would not be 
            // traversed (such as those associated with hidden declarations (e.g. in typedefs or variable declarations) 
            // will get their parent set).  It is reset by the normal mechanism to a value consistant with the traversal 
            // if it is not set correctly.
               if (classDeclaration->get_definition()->get_parent() == NULL)
                  {
#if DEBUG_PARENT_INITIALIZATION
                    printf ("Setting the parent of the class definition, since it has not yet been set. \n");
#endif
                    classDeclaration->get_definition()->set_parent(classDeclaration);
                  }
               ROSE_ASSERT(classDeclaration->get_definition()->get_parent() != NULL);
             }
        }
   }

void 
ResetParentPointers::resetParentPointersInType (SgType* typeNode, SgNode* previousNode)
   {
  // DQ (10/7/2004): Refactored common code for traversing islands within the AST which require a nested traversal

#if 0
  // DQ (10/12/2007): This is now called for the case of a forward enum in a typedef (e.g. "typedef a b;", at least for the case of C if not C++).
  // DQ (9/24/2007): This function should not be called now that Gergo's fix the the AST islands not being traversed is in place!
     printf ("Error: this function should not be called and has been eliminated by the fix to the AST island problems \n");
     ROSE_ASSERT(false);
#endif

  // This strips off and pointer types or reference types array types and modifier types that might hide the base type
  // printf ("Initial value of typeNode = %p = %s \n",typeNode,typeNode->sage_class_name());
     typeNode = typeNode->findBaseType();
  // printf ("after call to findBaseType(): typeNode = %p = %s \n",typeNode,typeNode->sage_class_name());

     switch(typeNode->variantT())
        {
          case V_SgClassType:
             {
            // Find any code (inside of a SgClassDefinition) that is buried within a SgClassType.
            // Examples of such code is: "struct X { int x; } Xvar;" or "typedef struct X { int x; } Xtype;".
               SgClassType* classType = isSgClassType (typeNode);
               SgDeclarationStatement* declarationStatement = classType->get_declaration();
               ROSE_ASSERT(declarationStatement != NULL);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
               ROSE_ASSERT(classDeclaration != NULL);
#if 0
               printf ("classDeclaration = %p isForward() = %s \n",
                    classDeclaration,(classDeclaration->isForward() == true) ? "true" : "false");
#endif
            // DQ (10/22/2005): The definition of a forward declaration is now NULL
            // ROSE_ASSERT(classDeclaration->get_definition() != NULL);
               if (classDeclaration->get_definition() != NULL)
                  {
                    SgClassDeclaration* definingClassDeclaration = classDeclaration->get_definition()->get_declaration();
                    ROSE_ASSERT(definingClassDeclaration != NULL);
#if 0
                    printf ("definingClassDeclaration = %p isForward() = %s \n",
                         definingClassDeclaration,(definingClassDeclaration->isForward() == true) ? "true" : "false");
#endif
                    if (definingClassDeclaration->isForward() == false)
                       {
#if 0
                         printf ("Calling resetParentPointers on class declaration found in SgTypedefDeclaration (island) \n");
#endif
                         SgNode* existingParent = definingClassDeclaration->get_parent();
                         if (existingParent != NULL)
                            {
                           // printf ("This defining class declaration has been set previously so reset to existing parent! \n");
                              resetParentPointers (definingClassDeclaration,existingParent);
                            }
                           else
                            {
                           // printf ("This defining class declaration has not been set previously (set to typedefDeclaration) \n");
                              resetParentPointers (definingClassDeclaration,previousNode);
                            }
                       }
                  }

               if (declarationStatement->get_parent() == NULL)
                  {
#if DEBUG_PARENT_INITIALIZATION
                    printf ("AST Fixup: in declarationStatement = %p = %s parent unset, set parent = %p = %s \n",
                              declarationStatement,declarationStatement->class_name().c_str(),
                              previousNode,previousNode->class_name().c_str());
#endif
                    declarationStatement->set_parent(previousNode);
                  }
               ROSE_ASSERT(declarationStatement->get_parent() != NULL);

            // DQ (10/22/2005): The definition of a forward declaration is now NULL
            // ROSE_ASSERT(classDeclaration->get_definition() != NULL);
               if (classDeclaration->get_definition() != NULL)
                  {
                 // DQ (10/17/2004): Added assertions
                    ROSE_ASSERT(classDeclaration->get_definition() != NULL);
                    SgClassDefinition* classDefinition = classDeclaration->get_definition();

                 // Since the defintion is shared only set it if it is not already set!
                    if (classDefinition->get_parent() == NULL)
                       {
#ifndef _MSC_VER
// DQ (11/28/2009): fatal error C1017: invalid integer constant expression
#if PRINT_SIDE_EFFECT_WARNINGS || DEBUG_PARENT_INITIALIZATION
                         printf ("Note: It would be better to set the parent of the class definition in the EDG/Sage connection (I think) \n");
#endif
#endif
                         if (classDeclaration->get_definingDeclaration() != NULL)
                              classDefinition->set_parent(classDeclaration->get_definingDeclaration());
                         else
                              classDefinition->set_parent(classDeclaration);
                       }
                    ROSE_ASSERT(classDeclaration->get_definition()->get_parent() != NULL);
                  }
               break;
             }

          case V_SgEnumType:
             {
            // Find any code (inside of a SgEnumDeclaration) that is buried within a SgEnumType.
            // Examples of such code is: "enum X { value = 0; } Xvar;" or "typedef enum X { value = 0; } Xtype;".
            // This is not a significant island of untraversed code except that the parent pointers 
            // are not set and we want to set them all!
               SgEnumType* enumType = isSgEnumType (typeNode);
               SgDeclarationStatement* declarationStatement = enumType->get_declaration();
               ROSE_ASSERT(declarationStatement != NULL);
               SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declarationStatement);
               ROSE_ASSERT(enumDeclaration != NULL);

            // printf ("In ResetParentPointers::resetParentPointersInType(): found enum declaration \n");
            // enumDeclaration->get_file_info()->display("found enum declaration");
#if 0
               if (enumDeclaration->get_parent() == NULL)
                  {
                    resetParentPointers (enumDeclaration,previousNode);
                  }
#else
            // DQ (5/31/2006): Added to handle enum fields and there parents
               resetParentPointers (enumDeclaration,previousNode);
#endif
               ROSE_ASSERT(enumDeclaration->get_parent() != NULL);

               break;
             }

          case V_SgTypedefType:
             {
            // DQ (11/14/2004): Not certain that we require this case, but Kull triggered the default case 
            // for a SgTypedefType so I have implemented it as part of testing.

            // Find any code (inside of a SgTypedefDeclaration) that is buried within a SgTypedefType.
            // Examples of such code is: "typedef X Xtype;" where X is a typedef type with a declaration.
            // This is not a significant island of untraversed code except that the parent pointers 
            // are not set and we want to set them all!
               SgTypedefType* typedefType = isSgTypedefType (typeNode);
               SgDeclarationStatement* declarationStatement = typedefType->get_declaration();
               ROSE_ASSERT(declarationStatement != NULL);
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(declarationStatement);
               ROSE_ASSERT(typedefDeclaration != NULL);

            // DQ (11/14/2004): If this is set to SgGlobal then reset it
            // if ( (typedefDeclaration->get_parent() == NULL) || (isSgGlobal(typedefDeclaration->get_parent()) != NULL) )
               if (typedefDeclaration->get_parent() == NULL)
                  {
                    resetParentPointers (typedefDeclaration,previousNode);
                  }
               ROSE_ASSERT(declarationStatement->get_parent() != NULL);
               break;
             }

          default:
             {
               printf ("Default reached in ResetParentPointers::resetParentPointersInType(%s) \n",typeNode->sage_class_name());
               ROSE_ASSERT(false);
               break;
             }
        }
   }


void 
ResetParentPointers::resetParentPointersInTemplateArgumentList ( const SgTemplateArgumentPtrList& templateArgListPtr )
   {
  // DQ (10/15/2004): It would be helpful if this were a part of the standard traversal, but it is not and it might be
  // that the SgTemplateArgumentPtrList would have to be made a new IR node to allow it to be traversed.
  // for now we will set the parents of any declarations hidden in any types explicitly (so that the resetTemplateName()
  // function will work (which requires the parent pointers)).  The reason why the parent pointers are required to be 
  // set for template arguments is that the template arguments can require qualified names and the name qualification
  // requires that the parents pointers be traversed to trace back through the scopes and collect the names of all the 
  // scopes.  This is handled in the get_scope() function which is called by the get_qualified_name() function.

  // ROSE_ASSERT(templateArgListPtr != NULL);
  // printf ("### In resetParentPointersInTemplateArgumentList(): templateArgListPtr->size() = %" PRIuPTR " ### \n",templateArgListPtr->size());
     SgTemplateArgumentPtrList::const_iterator i = templateArgListPtr.begin();
     while (i != templateArgListPtr.end())
        {
       // printf ("### In resetParentPointersInTemplateArgumentList(): templateArgList element *i = %s \n",(*i)->sage_class_name());
          switch((*i)->get_argumentType())
             {
               case SgTemplateArgument::argument_undefined:
                  {
                    printf ("Error: SgTemplateArgument::argument_undefined not allowed \n");
                    ROSE_ASSERT(false);
                    break;
                  }

               case SgTemplateArgument::type_argument:
                  {
                    ROSE_ASSERT((*i)->get_type() != NULL);
                    SgType* argumentType = (*i)->get_type();
                 // printf ("SgTemplateArgument::type_argument: argumentType = %p = %s \n",argumentType,argumentType->sage_class_name());

                    SgNamedType* namedType = isSgNamedType(argumentType);
                 // printf ("### In resetParentPointersInTemplateArgumentList(): namedType = %p \n",namedType);
                    if (namedType != NULL)
                       {
#if 0
                         printf ("In resetParentPointersInTemplateArgumentList(): ### namedType = %p = %s name = %s \n",namedType,namedType->sage_class_name(),namedType->get_name().str());
#endif
                         SgDeclarationStatement* declaration = namedType->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
#if 0
                         printf ("In resetParentPointersInTemplateArgumentList(): declaration = %p = %s parent = %p = %s \n",
                                 declaration,declaration->class_name().c_str(),declaration->get_parent(),
                                 (declaration->get_parent() != NULL) ? declaration->get_parent()->class_name().c_str() : "NULL_PARENT");
#endif
#if 0
                         printf ("In resetParentPointersInTemplateArgumentList(): ### namedType = %p = %s name = %s declaration = %p = %s parent = %p = %s \n",
                                 namedType,namedType->class_name().c_str(),namedType->get_name().str(),
                                 declaration,declaration->class_name().c_str(),declaration->get_parent(),
                                 (declaration->get_parent() != NULL) ? declaration->get_parent()->class_name().c_str() : "NULL_PARENT");
#endif
                         if (declaration->get_parent() == NULL)
                            {
                           // It should be possible to find an existing parent since defining declarations can't appear
                           // within template arguments.  At least I hope not!
                              SgNode* existingParent = NULL;
                           // Note that the defining declaration is not required to exist ("typedef struct X Y; X* xptr;" for example)
                           // printf ("declaration->get_definingDeclaration() = %p \n",declaration->get_definingDeclaration());
                              if (declaration->get_definingDeclaration() != NULL)
                                 {
                                // Hopefully this is set by now, but likely it is not required to be
                                   existingParent = declaration->get_definingDeclaration()->get_parent();
                                 }
                           // If still not found then look at the firstNondefiningDeclaration
                           // printf ("existingParent = %p \n",existingParent);
                              if (existingParent == NULL)
                                 {
                                   ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() != NULL);
                                   existingParent = declaration->get_firstNondefiningDeclaration()->get_parent();
                                 }

                              SgTypedefType* typedefType = isSgTypedefType(namedType);
                           // printf ("typedefType = %p \n",typedefType);
                              if ( (existingParent == NULL) && (typedefType != NULL) )
                                 {
                                   SgSymbol* symbol = typedefType->get_parent_scope();
                                   ROSE_ASSERT(symbol != NULL);
                                   switch(symbol->variantT())
                                      {
                                        case V_SgClassSymbol:
                                           {
                                          // printf ("In case V_SgClassSymbol: symbol = %p = %s \n",symbol,symbol->sage_class_name());
                                             SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                                             ROSE_ASSERT(classSymbol != NULL);
                                             SgDeclarationStatement* declaration = classSymbol->get_declaration();
                                             ROSE_ASSERT(declaration != NULL);
                                             SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                                             ROSE_ASSERT(classDeclaration != NULL);
                                             existingParent = classDeclaration->get_definition();
                                             ROSE_ASSERT(existingParent != NULL);
                                             break;
                                           }

                                        default:
                                           {
                                             printf ("Error: default reached symbol = %p = %s \n",symbol,symbol->sage_class_name());
                                             ROSE_ASSERT(false);
                                           }
                                      }
#if 0
                                   SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                                   if (typedefSymbol == NULL)
                                      {
                                        printf ("declaration = %p = %s \n",declaration,declaration->sage_class_name());
                                        declaration->get_file_info()->display("Error at this node!");
                                        printf ("symbol = %p = %s \n",symbol,symbol->sage_class_name());
                                      }
                                   ROSE_ASSERT(typedefSymbol != NULL);
                                   SgTypedefDeclaration* typedefDeclaration = typedefSymbol->get_declaration();
                                   ROSE_ASSERT(typedefDeclaration != NULL);
                                   existingParent = typedefDeclaration->get_parent();
#endif
                                   ROSE_ASSERT(existingParent != NULL);
                                 }

                              if (existingParent == NULL)
                                 {
                                   printf ("namedType   = %p = %s \n",namedType,namedType->class_name().c_str());
                                   printf ("declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                                 }

                           // DQ (3/27/2012): I think we wqant to allow declarations built to support types and hidden behind types to have NULL parents.
                           // ROSE_ASSERT(existingParent != NULL);
                              if (existingParent != NULL)
                                 {
#if DEBUG_PARENT_INITIALIZATION
                                   printf ("Setting parent of %p = %s to %p = %s \n",
                                        declaration,declaration->class_name().c_str(),
                                        existingParent,existingParent->class_name().c_str());
#endif
                                   declaration->set_parent(existingParent);

                                // DQ (10/17/2004): Added assertions
                                   SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                                   if (classDeclaration != NULL)
                                      {
                                     // DQ (1/30/2013): Commented out assertion that appears to be only an issue for ROSE compiling ROSE (part of testing).
                                     // ROSE_ASSERT(classDeclaration->get_definition() != NULL);
                                     // ROSE_ASSERT(classDeclaration->get_definition()->get_parent() != NULL);
                                        if (classDeclaration->get_definition() == NULL)
                                           {
// DQ (9/12/2014): Added more control over output of messages for release versions of ROSE.
#if PRINT_DEVELOPER_WARNINGS
                                             printf ("WARNING: In resetParentPointersInTemplateArgumentList(): commented out to compile ROSE using ROSE: assertion failing for: classDeclaration->get_definition() != NULL \n");
                                             printf ("--- classDeclaration = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
                                          // classDeclaration->get_file_info()->display("assertion failing for: classDeclaration->get_definition() != NULL: debug");
#endif
                                           }
                                          else
                                           {
                                             ROSE_ASSERT(classDeclaration->get_definition()->get_parent() != NULL);
                                           }
                                      }
                                 }
                                else
                                 {
                                   printf ("WARNING: In new EDG 4.x support I want to allow some paraents to be NULL. \n");
                                 }
                            }

                      // DQ (10/13/2004): If this is a template declaration then we might have to reset its name
                         SgClassType* classType = isSgClassType(namedType);
                         if (classType != NULL)
                            {
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                              ROSE_ASSERT(classDeclaration != NULL);
                              SgTemplateInstantiationDecl* templateClassDeclaration = isSgTemplateInstantiationDecl(classDeclaration);
                              if (templateClassDeclaration != NULL)
                                 {
                                // printf ("Found a template instantiation declaration  (call resetParentPointersInTemplateArgumentList) ... \n");
                                   resetParentPointersInTemplateArgumentList(templateClassDeclaration->get_templateArguments());
                                 }
                            }
                       }
                    break;
                  }

               case SgTemplateArgument::nontype_argument:
                  {
                 // These can be boolean or integer values, for example.

                 // DQ (8/13/2013): Added support for nontype template arguments to be either an expression
                 // (SgExpression) or a variable declaration (SgInitializedName)
                 // ROSE_ASSERT((*i)->get_expression() != NULL);
                    if ((*i)->get_expression() != NULL)
                       {
                         ROSE_ASSERT((*i)->get_initializedName() == NULL);
                         SgExpression* argumentExpression = (*i)->get_expression();
#if 0
                         printf ("SgTemplateArgument::nontype_argument: argumentExpression = %p = %s \n",argumentExpression,argumentExpression->class_name().c_str());
#endif
                         if (argumentExpression->get_parent() == NULL)
                            {
#if DEBUG_PARENT_INITIALIZATION
                               printf ("Setting parent in SgTemplateArgument::nontype_argument = %p = %s \n",argumentExpression,argumentExpression->class_name().c_str());
#endif
                              argumentExpression->set_parent(*i);
                            }
                       }
                      else
                       {
                         ROSE_ASSERT((*i)->get_initializedName() != NULL);
                         SgInitializedName* argumentInitializedName = (*i)->get_initializedName();

                         if (argumentInitializedName->get_parent() == NULL)
                            {
#if DEBUG_PARENT_INITIALIZATION
                               printf ("Setting parent in SgTemplateArgument::nontype_argument = %p = %s \n",argumentInitializedName,argumentInitializedName->class_name().c_str());
#endif
                              argumentInitializedName->set_parent(*i);
                            }
                       }

                 // printf ("Error: SgTemplateArgument::nontype_argument not implemented \n");
                 // ROSE_ASSERT(false);

                    break;
                  }

               case SgTemplateArgument::template_template_argument:
                  {
                 // DQ (8/24/2006): We don't want to reset the parent of a reference
                 // to a shared SgTemplateDeclaration. So there is nothing to do here.

                 // printf ("Error: resetParentPointersInTemplateArgumentList() SgTemplateArgument::template_template_argument case not implemented \n");
                 // ROSE_ASSERT(false);
                    break;
                  }

            // DQ (2/10/2014): Added this case to avoid compiler warning (I think there is nothing to do here).
               case SgTemplateArgument::start_of_pack_expansion_argument:
                  {
                 // printf ("Error: resetParentPointersInTemplateArgumentList() SgTemplateArgument::start_of_pack_expansion_argument case not implemented \n");
                 // ROSE_ASSERT(false);
                    break;
                  }
             }

       // Increment to next template argument
          i++;
        }

  // printf ("### Leaving resetParentPointersInTemplateArgumentList(): templateArgListPtr->size() = %" PRIuPTR " ### \n",templateArgListPtr->size());
   }




// [DQ]
ResetParentPointersInheritedAttribute
ResetParentPointers::evaluateInheritedAttribute (
   SgNode* node,
   ResetParentPointersInheritedAttribute inheritedAttribute )
   {
     ROSE_ASSERT(node != NULL);
  // cerr << "reset parent for node " << node->unparseToString();

#if 0
  // DQ (7/18/2019): Debugginf readFileTwice with test2019_501.C.
  // printf ("##### ResetParentPointers::evaluateInheritedAttribute(node = %p = %s) \n",node,node->class_name().c_str());
  // printf ("##### ResetParentPointers::evaluateInheritedAttribute(node = %p = %s) parent = %p \n",node,node->class_name().c_str(),node->get_parent());
     printf ("##### ResetParentPointers::evaluateInheritedAttribute(node = %p = %s) parent = %p = %s \n",
          node,node->class_name().c_str(),node->get_parent(),node->get_parent() != NULL ? node->get_parent()->class_name().c_str() : "null");
     printf (" --- inheritedAttribute.parentNode = %p = %s \n",inheritedAttribute.parentNode,inheritedAttribute.parentNode != NULL ? inheritedAttribute.parentNode->class_name().c_str() : "null");
#endif

#if 0
     vector <SgNode*> children = node->get_traversalSuccessorContainer();
     printf (" --- Output the children at this node (children.size() = %zu): \n",children.size());
     for (size_t i = 0; i < children.size(); i++)
        {
          printf (" --- --- children[%zu] = %p = %s \n",i,children[i],children[i] != NULL ? children[i]->class_name().c_str() : "null");
        }
#endif

#if 0
     if (node->get_parent() != NULL)
        {
          printf (" --- node->get_parent() = %p = %s \n",node->get_parent(),node->get_parent()->class_name().c_str());
        }
#endif
#if 0
  // ROSE_ASSERT(node->get_file_info() != NULL);
     if (node->get_file_info() != NULL)
        {
          node->get_file_info()->display("ResetParentPointers::evaluateInheritedAttribute: debug");
        }
#endif
#if 0
     if (isSgTryStmt(node) != NULL)
        {
          printf ("\n\n################ FOUND TRY STATEMENT ##################### \n\n\n");
        }
#endif

#if 0
  // DQ: Useful for output of debugging information (tracing through AST)
     printf ("\n\n##### node = %p = %s #####\n",node,node->sage_class_name());
     Sg_File_Info* traceSourcePosition = node->get_file_info();
     if (traceSourcePosition != NULL)
          traceSourcePosition->display("Location in ResetParentPointers");
     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(node);
     if (declarationStatement != NULL)
        {
          printf ("declarationStatement->isForward() = %s \n",(declarationStatement->isForward() == true) ? "true" : "false");
          SgDeclarationStatement* definingDeclaration = isSgDeclarationStatement(declarationStatement->get_definingDeclaration());
          printf ("definingDeclaration = %p \n",definingDeclaration);
          if (definingDeclaration != NULL)
               definingDeclaration->get_file_info()->display("Location of definingDeclaration");
          SgDeclarationStatement* nondefiningDeclaration = isSgDeclarationStatement(declarationStatement->get_firstNondefiningDeclaration());
          printf ("nondefiningDeclaration = %p \n",nondefiningDeclaration);
          if (nondefiningDeclaration != NULL)
               nondefiningDeclaration->get_file_info()->display("Location of nondefiningDeclaration");
        }
     printf ("----- node = %p = %s -----\n\n\n",node,node->sage_class_name());
#endif

  // Fix the parent pointer in the subtree (can't fix root node parent pointer so 
  // this shoud be called from above any node that requires an update (safe nodes 
  // would be the SgProject node) fix only made if parent == NULL)

  // DQ (5/10/2006): Set the Sg_File_Info so that they can be traced (later we might 
  // want to remove the parent pointer since it is not really required, except that 
  // it is helpful for debugging).
  // node->get_startOfConstruct()->set_parent(declaration);
  // node->get_endOfConstruct()->set_parent(declaration);
  // Sg_File_Info* fileInfoStart = node->get_startOfConstruct();
     Sg_File_Info* fileInfoStart = node->get_file_info();
     if (fileInfoStart != NULL)
        {
          if (fileInfoStart->get_parent() == NULL)
             {
               fileInfoStart->set_parent(node);
             }
        }
     Sg_File_Info* fileInfoEnd = node->get_endOfConstruct();
     if (fileInfoEnd != NULL)
        {
          if (fileInfoEnd->get_parent() == NULL)
             {
               fileInfoEnd->set_parent(node);
             }
        }

  // Handle the end of construct
     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
          Sg_File_Info* localFileInfoEndOfConstruct = node->get_endOfConstruct();
          if (localFileInfoEndOfConstruct != NULL)
             {
               if (localFileInfoEndOfConstruct->get_parent() == NULL)
                  {
                    localFileInfoEndOfConstruct->set_parent(node);
                  }
                 else
                  {
                 // printf ("Error: parent of localFileInfo is already set \n");
                 // localFileInfoEndOfConstruct->display("parent of localFileInfo is already set");
                  }
             }
        }

     SgInitializedName* initializedName = isSgInitializedName(node);
     if (initializedName != NULL)
        {
       // printf ("ResetParentPointers: initializedName = %p \n",initializedName);
          SgStorageModifier* modifier = &(initializedName->get_storageModifier());
          if (modifier != NULL)
             {
               if (modifier->get_parent() == NULL)
                  {
                    modifier->set_parent(initializedName);
                  }
             }
        }

// #ifdef REMOVE_SET_PARENT_FUNCTION
#if 0
#error "Dead Code!"

  // DQ (9/26/2004): Since we set both defining and non-defining declarations it can happen that we over 
  // aggressively set the parent of some nodes further along in the source sequence so I have disabled 
  // the testing of declarations (enforcing that they have a NULL parent pointer).
  // if (node->get_parent() != NULL)
     if (node->get_parent() != NULL && (dynamic_cast<SgFile*>(node) == NULL) && (dynamic_cast<SgDeclarationStatement*>(node) == NULL))
        {
       // printf ("Note: While in ResetParentPointers AST Fixup node = %p = %s already has parent \n",node,node->sage_class_name());
#if 0
          Sg_File_Info* sourcePosition = node->get_file_info();
          if (sourcePosition != NULL)
               sourcePosition->display("Warning: this IR node already has its parent set!");
#endif
        }
#if 0
  // DQ (9/26/2004): I think this may be too restrictive given that SgTemplateInstantiationDecl are shared
  // ROSE_ASSERT (node->get_parent() == NULL);
  // ROSE_ASSERT (node->get_parent() == NULL || (dynamic_cast<SgFile*>(node) != NULL) );
     ROSE_ASSERT ( (node->get_parent() == NULL) || 
                   (dynamic_cast<SgFile*>(node) != NULL) || 
                   (dynamic_cast<SgDeclarationStatement*>(node) != NULL) );
#endif
#endif

  // Note: SgType and SgSymbol nodes are not fixed (currently) (SgType nodes 
  //       and SgSymbol nodes are shared))
  // Set all nodes except SgSymbol and SgType nodes (even if they have been set previously set)
     if ( dynamic_cast<SgType*>(node) == NULL && dynamic_cast<SgSymbol*>(node) == NULL )
        {
       // Handle the part of the tree that is not hidden in the islands
          if ( inheritedAttribute.parentNode != NULL )
             {
            // set the parent on the current node to the one saved in the inherited attribute (for this traversal)
#if 0
               printf ("Valid parentNode = %p = %s at node = %p = %s \n",
                       inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str(),node,node->class_name().c_str());
#endif
               ROSE_ASSERT(node != NULL);
               if (node->get_parent() != inheritedAttribute.parentNode)
                  {
                 // DQ (8/1/2019): Output information where the node->get_parent() != inheritedAttribute.parentNode
                 // ROSE_ASSERT(inheritedAttribute.parentNode != NULL);
                 // ROSE_ASSERT(node->get_parent() != NULL);
#if 0
                    printf ("Note: ResetParentPointers::evaluateInheritedAttribute(): (not reset) node->get_parent() != inheritedAttribute.parentNode: node = %p = %s node->get_parent() = %p = %s inheritedAttribute.parentNode = %p = %s \n",
                         node,node->class_name().c_str(),
                         node->get_parent(),node->get_parent() != NULL ? node->get_parent()->class_name().c_str() : "null",
                         inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str());
#endif
#if 0
                  // DQ (9/24/2007): This feature is not used and should be removed, as a performance improvement!
                     string currentNodeString       = node->sage_class_name();
                     string currentNodeParentString = (node->get_parent() != NULL) ?
                                                       node->get_parent()->sage_class_name() : "NULL";
                     string futureNodeParentString  = inheritedAttribute.parentNode->sage_class_name();
                  // printf ("On node (%s): parent node currently set to %s being reset to %s \n",
                  //      currentNodeString.c_str(),currentNodeParentString.c_str(),futureNodeParentString.c_str());
                     string saveString = "On node (" + currentNodeString + 
                                         "): parent node currently set to "+ currentNodeParentString + 
                                         " being reset to " + futureNodeParentString;

                  // Accumulate the string into a list and remove redundent entries
                     modifiedNodeInformationList.push_back(saveString);
                     modifiedNodeInformationList.sort();
                     modifiedNodeInformationList.unique();
#endif
                  }

            // Set the parent node to the parent saved in the inherited attribute

            // DQ (11/1/2005): Only reset parents that are already NULL
#if 0
               if (node->get_parent() == NULL)
                  {
#if DEBUG_PARENT_INITIALIZATION
                    printf ("AST Fixup: Setting parent of node = %p = %s (to parent = %p = %s) \n",
                         node,node->class_name().c_str(),
                         inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str());
#endif

                 // Make sure it is NULL before resetting it!
                    ROSE_ASSERT (node->get_parent() == NULL);
                    node->set_parent(inheritedAttribute.parentNode);
                  }
                 else
                  {
                    if (node->get_parent() != inheritedAttribute.parentNode)
                       {
                         printf ("Would have reset valid parent pointer at node %p = %s to %p = %s \n",
                              node,node->class_name().c_str(),
                              inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str());
                       }
                  }
#else
#if DEBUG_PARENT_INITIALIZATION
               printf ("AST Fixup: Setting parent of node = %p = %s (to parent = %p = %s) \n",
                    node,node->class_name().c_str(),
                    inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str());
#endif

            // DQ (6/2/2006): This can lead to the parent of a SgClassDeclaration for a type
            // in a function parameter to be set to the SgFunctionParameterList (by mistake).
            // node->set_parent(inheritedAttribute.parentNode);
               if (node->get_parent() == NULL)
                  {
#if 0
                    printf ("AST Fixup: Setting unset parent of node = %p = %s from %p to %p = %s \n",
                         node,node->class_name().c_str(),node->get_parent(),
                         inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str());
#endif
                    node->set_parent(inheritedAttribute.parentNode);
                  }
#endif
             }
            else
             {
            // printf ("On node->sage_class_name() = %s inheritedAttribute.parentNode == NULL (not set) \n",
            //      node->sage_class_name());
               if (!dynamic_cast<SgProject*>(node) && !dynamic_cast<SgFile*>(node) )
                  {
                 // DQ (10/21/2004): This is relaxed to allow setting of parent pointers from manually constructed code!
#if STRICT_ERROR_CHECKING
                 // Only SgProject and SgFile can be root nodes after EDG->SAGE translation
                    printf ("Warning: only SgProject and SgFile can be root nodes after EDG->SAGE translation \n");
                    ROSE_ABORT();
#endif
                  }
             }

       // Test chain of parents back to the root node of the AST (test on non SgType and non SgSymbol nodes)
          traceBackToRoot(node);

#if 0
          if (isSgVariableDeclaration(node) != NULL)
             {
               printf ("AST fixup: found variable declaration = %p \n",node);
             }
#endif

       // Handle possible islands in the AST (generally typedef statements where the types need to be traversed)
          switch(node->variantT())
             {
               case V_SgTemplateInstantiationDecl:
                  {
                    SgTemplateInstantiationDecl* templateInstantiation = isSgTemplateInstantiationDecl(node);
                    ROSE_ASSERT(templateInstantiation != NULL);
                    resetParentPointersInDeclaration (templateInstantiation, inheritedAttribute.parentNode);

                 // TV (05/29/2018): possible if it is non-real
                 // ROSE_ASSERT(templateInstantiation->get_templateDeclaration() != NULL);

                 // DQ (10/15/2004): Now we have to reset the parents of any declarations appearing in 
                 // the template argument list!  Unless we should define the traversal to traverse that list!
                 // Could there be cycles introduced this way???  For now maybe we should just visit them 
                 // explicitly (and check with Markus).
#if 0
                    printf ("\n\n##### Setting parents of declarations within the template arguments not implemented! ##### \n\n");
#endif
                 // ROSE_ASSERT(templateInstantiation->get_templateArguments() != NULL);
                    resetParentPointersInTemplateArgumentList(templateInstantiation->get_templateArguments());
                    break;
                  }

            // DQ (8/18/2005): Added case of template member function so that we could set the SgTemplateDeclaration
               case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
                    SgTemplateInstantiationMemberFunctionDecl* templateInstantiation = isSgTemplateInstantiationMemberFunctionDecl(node);
                    ROSE_ASSERT(templateInstantiation != NULL);

                 // this is likely redundant
                    resetParentPointersInDeclaration (templateInstantiation, inheritedAttribute.parentNode);

                    SgDeclarationStatement* templateDeclaration = templateInstantiation->get_templateDeclaration();
                    if (templateDeclaration == NULL)
                       {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                         printf ("WARNING: templateInstantiation->get_templateDeclaration() == NULL templateInstantiation = %p = %s \n",templateInstantiation,templateInstantiation->class_name().c_str());
#endif
                       }
                 // DQ (5/3/2012): commented out for the new EDG 4.3 support.
                 // ROSE_ASSERT(templateDeclaration != NULL);

#if 0
                 // DQ (8/18/2005): There is too much variablity in how this is set, 
                 // it is better to have it be set in the EDG/Sage III translation!
                    printf ("##### AST Fixup: Calling resetParentPointersInDeclaration for templateDeclaration = %p = %s using parent = %p = %s \n",
                         templateDeclaration,templateDeclaration->class_name().c_str(),
                         inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str());
                    resetParentPointersInDeclaration (templateDeclaration, inheritedAttribute.parentNode);
#endif

                 // DQ (10/15/2004): Now we have to reset the parents of any declarations appearing in 
                 // the template argument list!  Unless we should define the traversal to traverse that list!
                 // Could there be cycles introduced this way???  For now maybe we should just visit them 
                 // explicitly (and check with Markus).
#if 0
                    printf ("\n\n##### Setting parents of declarations within the template arguments not implemented! ##### \n\n");
#endif
                 // ROSE_ASSERT(templateInstantiation->get_templateArguments() != NULL);
                    resetParentPointersInTemplateArgumentList(templateInstantiation->get_templateArguments());
                    break;
                  }

            // DQ (10/9/2004): We would like to set all declarations to have parents
            // based on their defining or first non-defining declarations.
               case V_SgClassDeclaration:
                  {
                 // Set any of the defining or nondefining declaration parent pointers
                 // these will be used to set parents of other class declarations 
                 // referenced within types.
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
                    ROSE_ASSERT(classDeclaration != NULL);
                    resetParentPointersInDeclaration (classDeclaration, inheritedAttribute.parentNode);
                    break;
                  }

               case V_SgVarRefExp:
                  {
                 // DQ (5/1/2005): some SgInitializedName are not traversed and so there parents are not set!
                 // Note that hidden in the SgVarRefExp is a SgVariableSymbol with a SgInitializedName 
                 // which will not have its parent set since we don't traverse the SgVariableSymbol within 
                 // the SgVarRefExp to find the hidden SgInitializedName object.  Why?  Well, in general
                 // symbols are shared and we can't guarentee uniqueness of the visit of many symbols.
                 // The SgInitializedName is in the SgVariableSymbol and so it is never visited!

                 // Other IR nodes where the containing symbols which are not traversed include:
                 //      SgClassNameRefExp (contains a SgClassSymbol)
                 //      SgFunctionRefExp (contains a SgFunctionSymbol)
                 //      MemberFunctionRefExp (contains a SgMemberFunctionSymbol)
                 //      ThisExp (contains a SgClassSymbol)
                 // But only the SgVariableSymbol's declaration is a SgInitializedName, so 
                 // no other IR nodes must be specially handled (I think).

                    SgVarRefExp *variableRefExpression = isSgVarRefExp(node);
                    ROSE_ASSERT(variableRefExpression != NULL);

                    SgVariableSymbol *variableSymbol = variableRefExpression->get_symbol();

                    if (variableSymbol == NULL)
                       {
                         printf ("WARNING: variableSymbol == NULL: variableRefExpression = %p \n",variableRefExpression);
                       }
                    ROSE_ASSERT(variableSymbol != NULL);

                 // DQ (1/1/2014): I think we may have to allow this for cases such as that in test2014_01.c
                 // But I would prefer to have a sysmbol always built so that ROSE had a consistant representation.
                 // Initially we want to allow this so that we can get the graph of the AST so that I can understand the problem better.
                    if (variableSymbol != NULL)
                       {
                 // This is bit confusing since what is returned is the SgInitializedName and NOT a declaration!
                    SgInitializedName *initializedName = variableSymbol->get_declaration();
                    ROSE_ASSERT(initializedName != NULL);

                 // printf ("AST fixup: In a SgVarRefExp found a SgInitializedName = %p \n",initializedName);

                    if (initializedName->get_parent() == NULL)
                       {
                      // Set the parent to be the SgVarRefExp (since setting it to the
                      // symbol would not productive, because symbols can be shared!)
#if DEBUG_PARENT_INITIALIZATION
                         printf ("Setting parent of %p = %s to %p = %s \n",
                              initializedName,initializedName->class_name().c_str(),
                              variableRefExpression,variableRefExpression->class_name().c_str());
#endif
                         initializedName->set_parent(variableRefExpression);
                       }
                    ROSE_ASSERT(initializedName->get_parent() != NULL);
                       }

                    break;
                  }

               case V_SgInitializedName:
                  {
                 // Find the types within the function declaration and set the parents of any 
                 // declarations that are contained in the types.
                    SgInitializedName* initializedName = isSgInitializedName(node);
                    ROSE_ASSERT(initializedName != NULL);
#if 0
                    printf ("AST fixup: Found a SgInitializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif
                 // DQ (9/6/2005): Set the parents of SgInitializedName objects
                 // This problem shows up in the loop processor test codes, 
                 // not clear if it is a real problem or not!
                    if (initializedName->get_parent() == NULL)
                       {
#if DEBUG_PARENT_INITIALIZATION
                         printf ("Warning Resetting the parent (previously NULL) of a SgInitializedName object! \n");
#endif
                         initializedName->set_parent(inheritedAttribute.parentNode);
                       }
                 // ROSE_ASSERT(initializedName->get_parent() != NULL);

                    SgType* type = initializedName->get_type();
                    if (type != NULL)
                       {
                         type = type->findBaseType();
                         SgClassType* classType = isSgClassType(type);
                         if (classType != NULL)
                            {
                              SgDeclarationStatement* declaration = classType->get_declaration();
                              ROSE_ASSERT(declaration != NULL);
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                              ROSE_ASSERT(classDeclaration != NULL);
#if 0
                              printf ("Calling resetParentPointers(): classDeclaration = %p = %s parent = %p = %s inheritedAttribute.parentNode = %p = %s \n",
                                   classDeclaration,classDeclaration->class_name().c_str(),
                                   classDeclaration->get_parent(),classDeclaration->get_parent() != NULL ? classDeclaration->get_parent()->class_name().c_str() : "null",
                                   inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str());
                              SgNode* parentBeforeReset = classDeclaration->get_parent();
#endif
                              resetParentPointers (classDeclaration,inheritedAttribute.parentNode);
#if 0
                              SgNode* parentAfterReset = classDeclaration->get_parent();
                              printf ("DONE: Calling resetParentPointers(): classDeclaration = %p = %s inheritedAttribute.parentNode = %p = %s \n",
                                   classDeclaration,classDeclaration->class_name().c_str(),inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str());
#endif
#if 0
                           // DQ (8/3/2019): Check the value of the parent to see if it was reset.
                              printf ("DONE: Calling resetParentPointers(): classDeclaration = %p = %s parent = %p = %s inheritedAttribute.parentNode = %p = %s \n",
                                   classDeclaration,classDeclaration->class_name().c_str(),
                                   classDeclaration->get_parent(),classDeclaration->get_parent() != NULL ? classDeclaration->get_parent()->class_name().c_str() : "null",
                                   inheritedAttribute.parentNode,inheritedAttribute.parentNode->class_name().c_str());
#endif
#if 0
                           // DQ (8/3/2019): This code demonstrates that the resetParentPointers function is called (rarely), test2016_90.C is an example of where it is required.
                              if (parentBeforeReset != parentAfterReset)
                                 {
                                   printf ("Note: in type of SgInitializedName: parent of classDeclaration = %p = %s was changed by the resetParentPointers() function \n",
                                        classDeclaration,classDeclaration->class_name().c_str());

                                // ROSE_ASSERT(parentBeforeReset != NULL);
                                   ROSE_ASSERT(parentAfterReset != NULL);

                                   printf (" --- parentBeforeReset = %p = %s \n",parentBeforeReset,parentBeforeReset != NULL ? parentBeforeReset->class_name().c_str() : "null");
                                   printf (" --- parentAfterReset  = %p = %s \n",parentAfterReset,parentAfterReset->class_name().c_str());
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
#endif
                            }
                       }

                    SgInitializedName* previousInitializedName = initializedName->get_prev_decl_item();
                    if (previousInitializedName != NULL)
                       {
                      // This can sometimes have a null parent (test2005_67.C) (for non-static member)
                         if (previousInitializedName->get_parent() == NULL)
                            {
                              printf ("Warning (previousInitializedName->get_parent() == NULL): initializedName = %p previousInitializedName = %p get_name() = %s \n",
                                      initializedName,previousInitializedName,previousInitializedName->get_name().str());
                              ROSE_ASSERT(previousInitializedName->get_scope() != NULL);
                              printf ("--- previousInitializedName->get_scope() = %p = %s \n",
                                      previousInitializedName->get_scope(),previousInitializedName->get_scope()->class_name().c_str());
                            }
                      // DQ (2/12/2011): Commented out to support generation of graph to debug test2011_08.C, this test codes 
                      // demonstrates that the SgInitializedName build first might only be to support a symbol and not have a
                      // proper parent.
                      // ROSE_ASSERT(previousInitializedName->get_parent() != NULL);
#if 1
                      // DQ (6/5/2011): Commented out as part of name qualification testing...
                         if (previousInitializedName->get_prev_decl_item() != NULL)
                              ROSE_ASSERT(previousInitializedName->get_parent() != NULL);
#else
                         printf ("Commented out test as part of name qualification testing. previousInitializedName = %p = %s \n",previousInitializedName,previousInitializedName->get_name().str());
#endif
                       }

#if STRICT_ERROR_CHECKING
                    SgDeclarationStatement* declarationStatement = initializedName->get_declaration();
                    ROSE_ASSERT(declarationStatement != NULL);
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
                 // ROSE_ASSERT(classDeclaration != NULL);
                    if (classDeclaration != NULL && classDeclaration->get_parent() == NULL)
                       {
#if DEBUG_PARENT_INITIALIZATION
                         printf ("Setting parent of class declaration = %p found in SgInitializedName \n",classDeclaration);
#endif
                         ROSE_ASSERT(classDeclaration->get_firstNondefiningDeclaration() != NULL);
                         ROSE_ASSERT(classDeclaration->get_firstNondefiningDeclaration()->get_parent() != NULL);
                         SgNode* existingParent = classDeclaration->get_firstNondefiningDeclaration()->get_parent();
                         ROSE_ASSERT(existingParent != NULL);
                         classDeclaration->set_parent(existingParent);
                         ROSE_ASSERT(classDeclaration->get_parent() != NULL);
                       }
#endif
#if 0
                    printf ("AST fixup: BOTTOM OF CASE: Found a SgInitializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif
                    break;
                  }

            // DQ (10/9/2004): function declarations have types that can have declarations
            // so find the types and set the parents of all the declarations.
               case V_SgFunctionDeclaration:
                  {
                 // Find the types within the function declaration and set the parents of any 
                 // declarations that are contained in the types.
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
                    ROSE_ASSERT(functionDeclaration != NULL);
                    SgType* returnType = functionDeclaration->get_orig_return_type();
#if STRICT_ERROR_CHECKING
                    ROSE_ASSERT(returnType != NULL);
#endif
                    if (returnType != NULL)
                       {
                         returnType = returnType->findBaseType();
                         SgClassType* classType = isSgClassType(returnType);
                         if (classType != NULL)
                            {
                              SgDeclarationStatement* declaration = classType->get_declaration();
                              ROSE_ASSERT(declaration != NULL);
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                              ROSE_ASSERT(classDeclaration != NULL);
                              if (classDeclaration->get_parent() == NULL)
                                 {
#if DEBUG_PARENT_INITIALIZATION
                                   printf ("Setting parent of class declaration found in return type of function \n");
#endif
                                   classDeclaration->set_parent(functionDeclaration);
                                 }
                           // DQ (10/17/2004): Modified this to not enforce assertion that defining 
                           // declaration existed (see Python specific testcode: test2004_92.C).
                           // Implemented the same fix for the firstNondefiningDeclaration as well.

                           // ROSE_ASSERT(classDeclaration->get_definingDeclaration() != NULL);
                              if (classDeclaration->get_definingDeclaration() != NULL)
                                 {
                                // DQ (1/30/2013): Commented out assertion that appears to be only an issue for ROSE compiling ROSE (part of testing).
                                   if (classDeclaration->get_definingDeclaration()->get_parent() == NULL)
                                      {
// DQ (9/12/2014): Added more control over output of messages for release versions of ROSE.
#if PRINT_DEVELOPER_WARNINGS
                                        printf ("WARNING: In resetParentPointersInTemplateArgumentList(): commented out to compile ROSE using ROSE: assertion failing for: classDeclaration->get_definingDeclaration()->get_parent() != NULL \n");
                                        printf ("--- classDeclaration = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
                                     // classDeclaration->get_file_info()->display("assertion failing for: classDeclaration->get_definingDeclaration()->get_parent() != NULL: debug");
#endif
                                      }
                                // ROSE_ASSERT(classDeclaration->get_definingDeclaration()->get_parent() != NULL);
                                 }

                           // ROSE_ASSERT(classDeclaration->get_firstNondefiningDeclaration() != NULL);
                              if (classDeclaration->get_firstNondefiningDeclaration() != NULL)
                                 {
                                   ROSE_ASSERT(classDeclaration->get_firstNondefiningDeclaration()->get_parent() != NULL);
                                 }
                            }
                       }
                    break;
                  }

               case V_SgTypedefDeclaration:
                  {
                 // printf ("Found a SgTypedefDeclaration = %p looking for islands of untraversed AST ... \n",node);
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
                    ROSE_ASSERT(typedefDeclaration != NULL);

                 // typedefDeclaration->get_file_info()->display("case V_SgTypedefDeclaration: typedefDeclaration");

#ifndef FIXED_ISLAND_TRAVERSAL
                    bool islandFound = typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration();
                 // printf ("islandFound = %s \n",(islandFound == true) ? "true" : "false");
                    if (islandFound == true)
                       {
                      // We only want to traverse the base type (since all "*" and "&" are associated
                      // with the variables in this variable list, e.g. list of SgInitializedName objects)

                      // DQ (5/21/2006): Set the parent of the declaration in the typedef if it exists 
                      // (it should if "islandFound == true").
                         ROSE_ASSERT(typedefDeclaration->get_declaration() != NULL);
                      // DQ (5/21/2006): Not true for enum declaration in typedef.
                      // ROSE_ASSERT(typedefDeclaration->get_declaration()->get_parent() != NULL);
                         if (typedefDeclaration->get_declaration()->get_parent() != typedefDeclaration)
                            {
                           // printf ("Reset the parent of embedded declaration in typedef to the typedef \n");
                              typedefDeclaration->get_declaration()->set_parent(typedefDeclaration);
                            }
                      // DQ (5/21/2006): reset the parent points in the island represented by the declaration
                         resetParentPointersInDeclaration(typedefDeclaration->get_declaration(),typedefDeclaration);

                         SgType* baseType = typedefDeclaration->get_base_type();
                         ROSE_ASSERT(baseType != NULL);
                         resetParentPointersInType(baseType,typedefDeclaration);
                       }
#endif

                 // DQ (10/12/2007): A forward enum will not have its parent set, see test2007_92.C.  E.g. "typedef enum zero number;"
                 // so we need to test this independent of the island problem (which is now fixed).
// #ifndef FIXED_ISLAND_TRAVERSAL
                 // DQ (10/14/2004): Even if this is not an island it might be a typedef of a template which 
                 // needs its name to be reset.  If it is a typedef that is later used then it would be reset
                 // where the type of the variable declaration would be seen, but if it is not used then we will 
                 // only see it here and thus we have to find and reset the name of the template declaration.
                    SgType* baseType = typedefDeclaration->get_base_type();
                    ROSE_ASSERT(baseType != NULL);
                 // printf ("baseType = %p = %s \n",baseType,baseType->sage_class_name());
                    SgNamedType* namedType = isSgNamedType(baseType);

                 // DQ (10/16/2004): Need to set this since typedef types are used in cases where they
                 // must be provided with qualified names and so there parents must be setup properly!
                    if (namedType != NULL)
                       {
                         SgDeclarationStatement* declaration = namedType->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
#if 0
                         printf ("namedType = %p = %s declaration->get_parent() = %p = %s \n",
                                 namedType,namedType->get_name().str(),
                                 declaration->get_parent(),
                                 (declaration->get_parent() != NULL) ? declaration->get_parent()->sage_class_name() : "NULL");
#endif
                         if (declaration->get_parent() == NULL)
                            {
#if 0
                              printf ("Error: Null Parent Pointer Found - declaration = %p = %s \n",declaration,declaration->sage_class_name());
                              declaration->get_file_info()->display("Error: Null Parent Pointer Found!");
#endif
                           // DQ (10/16/2004): reset pointers locared within declarations held within types
                           // printf ("$$$ Reset pointers locared within declarations held within types $$$ \n");
                              resetParentPointersInType(namedType,typedefDeclaration);
                           // printf ("$$$ DONE with reset pointers locared within declarations held within types $$$ \n");
                            }
                         ROSE_ASSERT(declaration->get_parent() != NULL);
                       }
// #endif
                    break;
                  }

               case V_SgVariableDeclaration:
                  {
                 // printf ("Found a SgVariableDeclaration = %p looking for islands of untraversed AST ... \n",node);
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
                    ROSE_ASSERT(variableDeclaration != NULL);

#ifndef FIXED_ISLAND_TRAVERSAL
                    bool islandFound = variableDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration();
#if 1
                    printf ("variableDeclaration                                    = %p \n",variableDeclaration);
                    printf ("variableDeclaration->get_definingDeclaration()         = %p \n",variableDeclaration->get_definingDeclaration());
                    printf ("variableDeclaration->get_firstNondefiningDeclaration() = %p \n",variableDeclaration->get_firstNondefiningDeclaration());
                    variableDeclaration->get_startOfConstruct()->display("setup parents within SgVariableDeclaration");

                    bool isSameAsDefiningDeclaration = (variableDeclaration == variableDeclaration->get_definingDeclaration());
                    printf ("isSameAsDefiningDeclaration = %s \n",isSameAsDefiningDeclaration ? "true" : "false");
                    bool isSameAsFirstNondefiningDeclaration = (variableDeclaration == variableDeclaration->get_firstNondefiningDeclaration());
                    printf ("isSameAsFirstNondefiningDeclaration = %s \n",isSameAsFirstNondefiningDeclaration ? "true" : "false");
#endif
#if 1
                    printf ("islandFound = %s \n",(islandFound == true) ? "true" : "false");
#endif
                    if (islandFound == true)
                       {
                      // We only want to traverse the base type (since all "*" and "&" are associated
                      // with the variables in this variable list, e.g. list of SgInitializedName objects)
                         SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
                         ROSE_ASSERT(variableList.size() > 0);
                         SgInitializedName* firstVariable = *(variableList.begin());
                         ROSE_ASSERT(firstVariable != NULL);
#if 1
                      // DQ (5/21/2006): Set the parent of the declaration in the typedef if it exists 
                      // (it should if "islandFound == true").
                      // SgDeclarationStatement* declaration = NULL;
                         SgType* variableType = firstVariable->get_typeptr();
                         ROSE_ASSERT(variableType != NULL);
                         SgNamedType* namedType = isSgNamedType(variableType);

                      // DQ (6/21/2006): Handle case of indirection in SgNamedType variables, 
                      // for example SgArrayType (e.g. "struct { int x; } ArrayVar [100];").
                         if (namedType == NULL)
                            {
                              SgType* baseType = variableType->stripType();
                              ROSE_ASSERT(baseType != NULL);
#if 0
                              printf ("In reset parent pointers for island in SgVariableDeclaration: variableType = %p = %s \n",
                                   variableType,variableType->class_name().c_str());
                              printf ("In reset parent pointers for island in SgVariableDeclaration: After calling stripType() namedType = %p = %s \n",
                                   baseType,baseType->class_name().c_str());
#endif
                              namedType = isSgNamedType(baseType);
                              ROSE_ASSERT(namedType != NULL);
                            }

                         ROSE_ASSERT(namedType != NULL);
                         ROSE_ASSERT(namedType->get_declaration() != NULL);
                      // DQ (5/21/2006): Not true for enum declaration in typedef.
                      // ROSE_ASSERT(namedType->get_declaration()->get_parent() != NULL);
                         if (namedType->get_declaration()->get_parent() != variableDeclaration)
                            {
                           // printf ("Reset the parent of embedded declaration in variable declaration to the SgVariableDeclaration \n");
                              namedType->get_declaration()->set_parent(variableDeclaration);
                              ROSE_ASSERT(namedType->get_declaration()->get_definingDeclaration() != NULL);
                              namedType->get_declaration()->get_definingDeclaration()->set_parent(variableDeclaration);
                              if (namedType->get_declaration()->get_firstNondefiningDeclaration() != NULL)
                                   namedType->get_declaration()->get_firstNondefiningDeclaration()->set_parent(variableDeclaration);
                            }
                      // DQ (5/21/2006): reset the parent points in the island represented by the declaration
                         resetParentPointersInDeclaration(namedType->get_declaration(),variableDeclaration);
#endif
                         SgType* baseType = firstVariable->get_typeptr();
                         ROSE_ASSERT(baseType != NULL);
                         resetParentPointersInType(baseType,variableDeclaration);
                       }
#endif
                    break;
                  }

            // DQ (9/24/2005): This need to be traversed since the declaration is not traversed as part of the traversal (though it could be)
            // DQ (4/16/2005): Added support for explicit template instantation directives
               case V_SgTemplateInstantiationDirectiveStatement:
                  {
                 // At the moment we don't traverse the decalaration hidden in a SgTemplateInstantiationDirectiveStatement
                 // printf ("Found a SgTemplateInstantiationDirectiveStatement = %p ... \n",node);
                    SgTemplateInstantiationDirectiveStatement* directive = isSgTemplateInstantiationDirectiveStatement(node);
                    ROSE_ASSERT(directive != NULL);
                    SgDeclarationStatement* declaration = directive->get_declaration();
                    ROSE_ASSERT(declaration != NULL);
                    if (declaration->get_parent() == NULL)
                       {
#if DEBUG_PARENT_INITIALIZATION
                         printf ("Setting parent of %p = %s to %p = %s \n",
                              declaration,declaration->class_name().c_str(),
                              directive,directive->class_name().c_str());
#endif
                         declaration->set_parent(directive);
                       }
                      else
                       {
                      // DQ (2/9/2014): This was an error, but it only shows up in the use of the GNU 4.6 header files 
                      // (and it is not clear that it should be an error).  So output a message as we debug this issue.
                         if (declaration->get_parent() != directive)
                            {
#if 0
                              printf ("Error: In reset parent pointers for island in case V_SgTemplateInstantiationDirectiveStatement: declaration->get_parent() != directive \n");
                              printf ("directive = %p = %s \n",directive,directive->class_name().c_str());
                              printf ("Error: declaration = %p = %s  (declaration->get_parent() = %p = %s) \n",declaration,declaration->class_name().c_str(),declaration->get_parent(),declaration->get_parent()->class_name().c_str());
                              declaration->get_file_info()->display("location of problem code: declaration: debug");
                              directive->get_file_info()->display("location of problem code: directive: debug");
#else
#if 0
                           // DQ (5/19/2014): Make this a warning for now; it does not appear to be a problem.
                              printf ("Warning: In reset parent pointers for island in case V_SgTemplateInstantiationDirectiveStatement: declaration->get_parent() != directive \n");
#endif
#endif
                            }

                      // DQ (3/15/2006): Why is it an error to have this be a valid pointer?  The parent should be the directive, I think.
                      // ROSE_ASSERT(declaration->get_parent() == directive);
#if 0
                         printf ("directive = %p = %s \n",directive,directive->class_name().c_str());
                         printf ("Error: declaration = %p = %s  (declaration->get_parent() = %p = %s) \n",declaration,declaration->class_name().c_str(),declaration->get_parent(),declaration->get_parent()->class_name().c_str());
                         declaration->get_file_info()->display("location of problem code");
                         ROSE_ASSERT(false);
#endif
                       }
#if 0
                    printf ("reset parents: Found a SgTemplateInstantiationDirectiveStatement = %p: declaration->get_parent() = %p = %s \n",
                         directive,declaration->get_parent(),declaration->get_parent()->class_name().c_str());
                    printf ("reset parents: Found a SgTemplateInstantiationDirectiveStatement = %p: declaration->get_scope() = %p = %s \n",
                         directive,declaration->get_scope(),declaration->get_scope()->class_name().c_str());
#endif
                    SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declaration);
                    if (memberFunctionDeclaration != NULL)
                       {
                         SgCtorInitializerList* ctors = memberFunctionDeclaration->get_CtorInitializerList();
                         ROSE_ASSERT(ctors != NULL);
                         ROSE_ASSERT(ctors->get_parent() != NULL);
#if 0
                         printf ("ctors = %p scope = %p = %s \n",ctors,ctors->get_scope(),ctors->get_scope()->class_name().c_str());
#endif
                       }
                    break;
                  }

               default:
                  {
                 // Only trap out typedefs and variable declarations
#if 0
                    printf ("default reached in reset parent pointers: node = %p = %s \n",node,node != NULL ? node->class_name().c_str() : "null");
#endif
                    break;
                  }
             }
        }
       else
        {
       // DQ (2/14/2015): Comment out to debug C++11 data member initialization (See C++11 test2015_13.C).
       // This was a problem because of a stored SgType pointer that was traversed as part of the AST, this is fixed now.

       // Since we don't traverse types this branch is never executed!
          printf ("Found a type or symbol while resetting parents \n");
          printf ("$$$$$ In evaluateInheritedAttribute() \n");
          printf ("   --- astNode->class_name() = %s \n",node->class_name().c_str());
          ROSE_ASSERT(false);
        }

  // I/O useful for debugging
  // if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
     if ( SgProject::get_verbose() >= AST_POST_PROCESSING_VERBOSE_LEVEL )
        {
          if ( node->get_parent() != NULL )
             {
            // Test if we have a parent that we would expect to have (given the traversal that we are using)
               SgNode* suggestedNode = inheritedAttribute.parentNode;
               SgNode* parentNode = node->get_parent();
               if ( inheritedAttribute.parentNode != parentNode )
                  {
                 // Interesting node 
                    string currentNodeString  = node->class_name();
                    string expectedNodeString = (suggestedNode) ? suggestedNode->class_name() : "NULL POINTER";
                    string recordedNodeString = (parentNode)    ? parentNode->class_name()    : "NULL POINTER";
                    printf ("Note: On %s node expected parent (%s) didn't match recorded parent (%s) \n",currentNodeString.c_str(),expectedNodeString.c_str(),recordedNodeString.c_str());
                  }
             }
        }

#if 0
     printf (" --- Setting inheritedAttribute.parentNode to node = %p = %s \n",node,node != NULL ? node->class_name().c_str() : "null");
#endif

  // Always set the parent node point in the inherited attribute
     inheritedAttribute.parentNode = node;

     return inheritedAttribute;
   }

// DQ (9/25/2004): build another function that has a better interface and really works with any SgNode
void resetParentPointers ( SgNode* node, SgNode* parent )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
  // TimingPerformance timer ("(reset parent pointers) time (sec) = ");

  // printf ("Resetting the parent pointers ... (starting at node = %s) \n",node->sage_class_name());
     ResetParentPointersInheritedAttribute inheritedAttribute;

     inheritedAttribute.parentNode = parent;

     ResetParentPointers setParentPointerTraversal;
     setParentPointerTraversal.traverse(node,inheritedAttribute);
   }


void ResetParentPointersOfClassAndNamespaceDeclarations::visit(SgNode* node)
   {
  // DQ (11/1/2005): Reset parent pointers of and data members 
  // or namespace members to the class or namespace.

     ROSE_ASSERT(node != NULL);

     switch(node->variantT())
        {
          case V_SgClassDefinition:
          case V_SgTemplateInstantiationDefn:
             {
               SgClassDefinition* classDefinition = isSgClassDefinition(node);
               ROSE_ASSERT(classDefinition != NULL);
               SgDeclarationStatementPtrList & memberList = classDefinition->get_members();
               SgDeclarationStatementPtrList::iterator i = memberList.begin();
               while ( i != memberList.end() )
                  {
                    if ( (*i)->get_parent() != classDefinition )
                       {
                      // Then we need to reset the parent!
#if DEBUG_PARENT_INITIALIZATION
                         printf ("Resetting parent of data member of SgClassDefinition or SgTemplateInstantiationDefn \n");
#endif
                         (*i)->set_parent(classDefinition);
                       }
                    i++;
                  }
               break;
             }

          case V_SgNamespaceDefinitionStatement:
             {
               SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(node);
               ROSE_ASSERT(namespaceDefinition != NULL);
               SgDeclarationStatementPtrList & declarationList = namespaceDefinition->get_declarations();
               SgDeclarationStatementPtrList::iterator i = declarationList.begin();
               while ( i != declarationList.end() )
                  {
                    if ( (*i)->get_parent() != namespaceDefinition )
                       {
                      // Then we need to reset the parent!
#if DEBUG_PARENT_INITIALIZATION
                         printf ("Resetting parent of data member of SgNamespaceDefinitionStatement \n");
#endif
                         (*i)->set_parent(namespaceDefinition);
                       }
                    i++;
                  }
               break;
             }

          case V_SgGlobal:
             {
               SgGlobal* globalScope = isSgGlobal(node);
               ROSE_ASSERT(globalScope != NULL);
               SgDeclarationStatementPtrList & declarationList = globalScope->get_declarations();
               SgDeclarationStatementPtrList::iterator i = declarationList.begin();
               while ( i != declarationList.end() )
                  {
                    if ( (*i)->get_parent() != globalScope )
                       {
                      // Then we need to reset the parent!
#if DEBUG_PARENT_INITIALIZATION
                         printf ("Resetting parent of data member of SgGlobal \n");
#endif
                         (*i)->set_parent(globalScope);
                       }
                    i++;
                  }
               break;
             }

          case V_SgBasicBlock:
             {
               SgBasicBlock* blockScope = isSgBasicBlock(node);
               ROSE_ASSERT(blockScope != NULL);
               SgStatementPtrList & statementList = blockScope->get_statements();
               SgStatementPtrList::iterator i = statementList.begin();
               while ( i != statementList.end() )
                  {
                    if ( (*i)->get_parent() != blockScope )
                       {
                      // Then we need to reset the parent!
#if DEBUG_PARENT_INITIALIZATION
                         printf ("Resetting parent of data member of SgBasicBlock \n");
#endif
                         (*i)->set_parent(blockScope);
                       }
                    i++;
                  }
               break;
             }

          default:
             {
            // Nothing else to do here!
             }
        }
   }


// DQ (9/25/2004): build another function that has a better interface and really works with any SgNode
void resetParentPointersOfClassOrNamespaceDeclarations ( SgNode* node )
   {
  // DQ (11/1/2005): In a separate pass set the data members of any class or 
  // namespace to have a parent pointing at the class definition or the namespace.

  // printf ("Resetting the parent pointers ... (starting at node = %s) \n",node->sage_class_name());
     ResetParentPointersOfClassAndNamespaceDeclarations setParentPointerTraversal;
     setParentPointerTraversal.traverse(node,preorder);
   }


void topLevelResetParentPointer ( SgNode* node )
   {
  // Put this in a local scope so that the timing can be easily controled
  // this is required because the resetParentPointers() function is called 
  // recursively.

  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance resetParentPointerTimer ("Reset parent pointers:");

  // reset the parent pointers
     resetParentPointers(node,node->get_parent());

  // DQ (9/24/2007): This might not be required now that AST islands are fixed.  This IS required!
#if 1
  // reset the parent pointers in any class definitions, namespace definitions or global scope
     resetParentPointersOfClassOrNamespaceDeclarations(node);
#endif
   }


void
resetFileInfoParentPointersInMemoryPool()
   {
     ResetFileInfoParentPointersInMemoryPool t;
     t.traverseMemoryPool();
   }
     
void
ResetFileInfoParentPointersInMemoryPool::visit(SgNode* node)
   {
#if 0
     printf ("##### ResetFileInfoParentPointersInMemoryPool::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     SgSupport*     support     = isSgSupport(node);
     SgUntypedNode* untypedNode = isSgUntypedNode(node);

  // DQ (9/15/2014): Skip checking of parent pointers here for SgUntypedNode IR nodes (handle this case seperately).
  // All types should have NULL parent pointers (because types can be shared)
  // if (locatedNode != NULL)
     if (locatedNode != NULL && untypedNode == NULL)
        {
          if (locatedNode->get_startOfConstruct() == NULL)
             {
               printf ("Error: locatedNode->get_startOfConstruct() == NULL (locatedNode = %p = %s) \n",locatedNode,locatedNode->class_name().c_str());
               if (isSgFunctionParameterList(node) != NULL)
                  {
                 // DQ (9/13/2011): Reported as possible NULL value in static analysis of ROSE code.
                    ROSE_ASSERT(locatedNode->get_parent() != NULL);

                    printf ("     This is a SgFunctionParameterList, so look at the parent = %p = %s \n",locatedNode->get_parent(),locatedNode->get_parent()->class_name().c_str());
                  }

               if (locatedNode->get_parent() == NULL)
                  {
                    printf ("     locatedNode->get_parent() locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
                  }
#if 0
               ROSE_ASSERT(locatedNode->get_parent() != NULL);
               if (locatedNode->get_parent()->get_startOfConstruct() != NULL)
                  {
                    printf ("     This is a ???, so look at the parent = %p = %s \n",locatedNode->get_parent(),locatedNode->get_parent()->class_name().c_str());
                    locatedNode->get_parent()->get_startOfConstruct()->display("Error: locatedNode->get_parent()->get_startOfConstruct() == NULL");
                  }
#else
#if 1
            // DQ (2/12/2012): Refactoring disagnostic support for detecting where we are when something fails.
               SageInterface::whereAmI(locatedNode);
#else
            // DQ (2/12/2012): Added better support for tracing back trhough the AST (better diagnostics).
               SgNode* parent = locatedNode->get_parent();
               while (parent != NULL)
                  {
                    printf ("Tracing back through parent = %p = %s \n",parent,parent->class_name().c_str());
                    if (parent->get_startOfConstruct() != NULL)
                       {
                         parent->get_startOfConstruct()->display("Error: parent->get_startOfConstruct() == NULL");
                       }

                    parent = parent->get_parent();
                  }
#endif
#endif
            // ROSE_ASSERT(locatedNode->get_file_info() != NULL);
            // locatedNode->get_parent()->get_file_info()->display("Error: locatedNode->get_startOfConstruct() == NULL");
             }
          ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);

          if (locatedNode->get_startOfConstruct()->get_parent() == NULL)
             {
               locatedNode->get_startOfConstruct()->set_parent(locatedNode);
             }
          ROSE_ASSERT(locatedNode->get_startOfConstruct()->get_parent() != NULL);

          if (locatedNode->get_endOfConstruct() != NULL)
             {
               if (locatedNode->get_endOfConstruct()->get_parent() == NULL)
                  {
                    locatedNode->get_endOfConstruct()->set_parent(locatedNode);
                  }
               ROSE_ASSERT(locatedNode->get_endOfConstruct()->get_parent() != NULL);
             }
        }

     if (support != NULL)
        {
          switch (support->variantT())
             {
            // These are the only SgSupport IR nodes that have a Sg_File_Info object pointer.
               case V_SgRenamePair:
               case V_SgPragma:
                  {
                    if (support->get_file_info() == NULL)
                         printf ("support node = %p = %s \n",support,support->class_name().c_str());

                    ROSE_ASSERT(support->get_file_info() != NULL);
                    if (support->get_file_info()->get_parent() == NULL)
                       {
                         support->get_file_info()->set_parent(support);
                       }
                    ROSE_ASSERT(support->get_file_info()->get_parent() != NULL);
                    break;
                  }
#if 0 // Liao 11/5/2010, this should be handled by SgLocatedNode now
               case V_SgInitializedName:
                  {
                    SgInitializedName* initializedName = isSgInitializedName(support);
                    if (support->get_file_info() == NULL)
                         printf ("support node = %p = %s = %s \n",support,support->class_name().c_str(),initializedName->get_name().str());

                    ROSE_ASSERT(support->get_file_info() != NULL);
                    if (support->get_file_info()->get_parent() == NULL)
                       {
                         support->get_file_info()->set_parent(support);
                       }
                    ROSE_ASSERT(support->get_file_info()->get_parent() != NULL);
                    break;
                  }
#endif
            // case V_SgFile:
               case V_SgSourceFile:
               case V_SgBinaryComposite:
               case V_SgUnknownFile:
                  {
                    ROSE_ASSERT(support->get_file_info() != NULL);
                    ROSE_ASSERT(support->get_file_info()->get_parent() != NULL);
                    break;
                  }

#if 0
            // It still might be that these will be set properly 
            // (so we can't set them here, set then in the resetParentPointersInMemoryPool (next))
               case V_Sg_File_Info:
                  {
                    Sg_File_Info* fileInfo = isSg_File_Info(support);
                    ROSE_ASSERT(fileInfo->get_parent() != NULL);
                    break;
                  }
#endif
               default:
                  {
                 // All other SgSupport should call teh SgNode virtual base class function and return NULL
                    if (support->get_file_info() != NULL)
                       {
                         printf ("Error: support->get_file_info() != NULL for support = %p = %s \n",support,support->class_name().c_str());
                       }
                    ROSE_ASSERT(support->get_file_info() == NULL);
                    break;
                  }
             }
        }

  // DQ (9/15/2014): Specific checking of parent pointers here for SgUntypedNode IR nodes (skipped handle this case with all SgLocatedNode IR nodes above).
     if (untypedNode != NULL)
        {
          if (untypedNode->get_startOfConstruct() == NULL)
             {
            // Rasmussen (2/27/2017): Turning off warning temporarily as OFP/Stratego does not yet provide
            //                        location information.
            // printf ("Warning: untypedNode->get_startOfConstruct() == NULL (untypedNode = %p = %s) \n",untypedNode,untypedNode->class_name().c_str());
             }
        }
   }




// DQ (8/23/2012): Modified to take a SgNode so that we could compute the global scope for use in setting 
// parents of template instantiations that have not be placed into the AST but exist in the memory pool.
// This is called directly from void postProcessingSupport (SgNode* node).
// void resetParentPointersInMemoryPool()
void
resetParentPointersInMemoryPool(SgNode* node)
   {
  // There are two traversals here, these could be combined since they both operate on the memory pool.

     TimingPerformance timer ("Reset parent pointers in memory pool:");

     ROSE_ASSERT(node != NULL);

#if 0
     printf ("In resetParentPointersInMemoryPool(): node = %p = %s \n",node,node->class_name().c_str());
#endif

     SgGlobal* globalScope = NULL;
     SgProject* project = isSgProject(node);
     if (project != NULL)
        {
          SgFile* file = (*project)[0];

          SgSourceFile* sourceFile = isSgSourceFile(file);

       // DQ (10/9/2012): Robb points out that this is a problem for the binary analysis.
       // ROSE_ASSERT(sourceFile != NULL);
          if (sourceFile != NULL)
             {
               globalScope = sourceFile->get_globalScope();
             }

          ROSE_ASSERT(globalScope != NULL || isSgBinaryComposite(file) != NULL);
        }
       else
        {
          SgSourceFile* sourceFile = isSgSourceFile(node);
          if (sourceFile != NULL)
             {
               globalScope = sourceFile->get_globalScope();
             }
            else
             {
            // DQ (8/5/2019): It is a bit of a problem that we have to allow this to work when not using a SgProject or SgSourceFile, but this
            // is required for the inlining tests.  Note that these inlined tests will not have the resetFileInfoParentPointersInMemoryPool() be called.

            // DQ (8/2/2019): This function is only meaningful to call with the SgProject node, and will do nothing otherwise.
            // This may be the source of the problem in using the SageBuilder::buildFile() API in testing test2019_501.C).
            // If so then this function should be fixed to allow a SgFile to be used alternatively.
            // printf ("In resetParentPointersInMemoryPool(): This function can't be called using anything but the SgProject: node = %p = %s \n",node,node->class_name().c_str());
            // ROSE_ASSERT(false);

            // DQ (8/6/2019): Alternatively, let's use the existing parent pointers to serach for the associated global scope.
               bool includingSelf = true;
               globalScope = SageInterface::getEnclosingNode<SgGlobal>(node,includingSelf);

               if (globalScope == NULL)
                  {
                 // DQ (8/6/2019): Make it an error to not have found an associated global scope from the input node.
                    printf ("Error: In resetParentPointersInMemoryPool(): Could not locate global scope in search upward through the AST from this node = %p = %s \n",node,node->class_name().c_str());
                  }
                 else
                  {
#if 0
                    printf ("In resetParentPointersInMemoryPool(): Found global scope in search upward through the AST from this node = %p = %s \n",node,node->class_name().c_str());
#endif
                  }
               ROSE_ASSERT(globalScope != NULL);
             }
        }

  // ROSE_ASSERT(globalScope != NULL);

  // DQ (10/9/2012): Make this conditional upon having found a valid SgGlobal (not the case for a binary file).
     if (globalScope != NULL)
        {
          ResetParentPointersInMemoryPool t(globalScope);

          ROSE_ASSERT(t.globalScope != NULL);

          t.traverseMemoryPool();

       // JJW: Moved this down because it requires that some non-Sg_File_Info
       // parent pointers have been set
       // Reset parents of any remaining unset Sg_File_Info object first
          resetFileInfoParentPointersInMemoryPool();
        }
       else
        {
       // DQ (8/5/2019): This fails for the binary analysis, where there is no SgGlobal found.
       // DQ (8/2/2019): This function is only meaningful to call with the SgProject node, and will do nothing otherwise.
       // printf ("In resetParentPointersInMemoryPool(): This function is not doing anything when called using: node = %p = %s \n",node,node->class_name().c_str());
       // ROSE_ASSERT(false);
        }
   }

     
void
ResetParentPointersInMemoryPool::visit(SgNode* node)
   {
#if 0
     SgLocatedNode* temp_locatedNode = isSgLocatedNode(node);
     if ( (temp_locatedNode != NULL) && (temp_locatedNode->get_file_info()->isFrontendSpecific() == false) )
        {

#error "DEAD CODE!"

       // Only do I/O on nodes not in the header of predefined functions.
          printf ("##### ResetParentPointersInMemoryPool::visit(node = %p = %s) \n",node,node->class_name().c_str());
        }
#endif

#if 0
     printf ("##### ResetParentPointersInMemoryPool::visit(node = %p = %s) \n",node,node->class_name().c_str());
#endif

  // I built a pointer to global scope so that we could use it for this case.
     ROSE_ASSERT(globalScope != NULL);

     SgType*        type        = isSgType(node);
     SgSymbol*      symbol      = isSgSymbol(node);
     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     SgSupport*     support     = isSgSupport(node);

  // All types should have NULL parent pointers (because types can be shared)
     if (type != NULL)
        {
       // Note that the SgNode::get_parent() function is forced to return NULL for the case of a SgType IR node
          ROSE_ASSERT(type->get_parent() == NULL);
        }

  // Symbols can be shared within a single file but are not yet shared across files in the AST merge
     if (symbol != NULL)
        {
       // Should point to the associated SgSymbolTable
          if (symbol->get_parent() == NULL)
             {
            // printf ("In ResetParentPointersInMemoryPool::visit(): symbol = %p = %s get_parent() == NULL \n",symbol,symbol->class_name().c_str());

               switch(symbol->variantT())
                  {
                    case V_SgFunctionSymbol:
                       {
                         SgFunctionSymbol* tempSymbol = isSgFunctionSymbol(symbol);
                         ROSE_ASSERT(tempSymbol != NULL);
                         SgFunctionDeclaration* declaration = tempSymbol->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
                         SgScopeStatement* scope = declaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         ROSE_ASSERT(scope->get_symbol_table() != NULL);
                         symbol->set_parent(scope->get_symbol_table());
                         break;
                       }

                    case V_SgMemberFunctionSymbol:
                       {
                         SgMemberFunctionSymbol* tempSymbol = isSgMemberFunctionSymbol(symbol);
                         ROSE_ASSERT(tempSymbol != NULL);
                         SgFunctionDeclaration* declaration = tempSymbol->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
                         SgScopeStatement* scope = declaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         ROSE_ASSERT(scope->get_symbol_table() != NULL);
                         symbol->set_parent(scope->get_symbol_table());
                         break;
                       }

                    case V_SgVariableSymbol:
                       {
                         SgVariableSymbol* tempSymbol = isSgVariableSymbol(symbol);
                         ROSE_ASSERT(tempSymbol != NULL);
                         SgInitializedName* declaration = tempSymbol->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
                         SgScopeStatement* scope = declaration->get_scope();

                      // DQ (6/24/2006): There should be a test that detects this scope problem!
                         if (scope == NULL)
                            {
                              printf ("Looking for the scope in the SgVariableSymbol through the definition (declaration = %p = %s = %s) \n",
                                   declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
                              ROSE_ASSERT(declaration->get_definition() != NULL);

                              SgDeclarationStatement* declarationStatement = declaration->get_definition();
                              ROSE_ASSERT(declarationStatement != NULL);
                              ROSE_ASSERT(declarationStatement->get_scope() != NULL);
                              printf ("Looking for the scope in the SgVariableSymbol: declarationStatement = %p = %s \n",
                                   declarationStatement,declarationStatement->class_name().c_str());
                              scope = declarationStatement->get_scope();
                              ROSE_ASSERT(scope != NULL);
                            }

                         ROSE_ASSERT(scope != NULL);
                         ROSE_ASSERT(scope->get_symbol_table() != NULL);
                         symbol->set_parent(scope->get_symbol_table());
                         break;
                       }

                    case V_SgTemplateSymbol:
                       {
                         SgTemplateSymbol* tempSymbol = isSgTemplateSymbol(symbol);
                         ROSE_ASSERT(tempSymbol != NULL);
                         SgTemplateDeclaration* declaration = tempSymbol->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
                         SgScopeStatement* scope = declaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         ROSE_ASSERT(scope->get_symbol_table() != NULL);
                         symbol->set_parent(scope->get_symbol_table());
                         break;
                       }

                    case V_SgTypedefSymbol:
                       {
                         SgTypedefSymbol* tempSymbol = isSgTypedefSymbol(symbol);
                         ROSE_ASSERT(tempSymbol != NULL);
                         SgTypedefDeclaration* declaration = tempSymbol->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
                         SgScopeStatement* scope = declaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         ROSE_ASSERT(scope->get_symbol_table() != NULL);
                         symbol->set_parent(scope->get_symbol_table());
                         break;
                       }

                    case V_SgLabelSymbol:
                       {
                         SgLabelSymbol* tempSymbol = isSgLabelSymbol(symbol);
                         ROSE_ASSERT(tempSymbol != NULL);
                         SgLabelStatement* declaration = tempSymbol->get_declaration();

                      // DQ (12/9/2007): Added support for fortran labels
                      // ROSE_ASSERT(declaration != NULL);
                         if(declaration != NULL)
                            {
                              SgScopeStatement* scope = declaration->get_scope();
                              ROSE_ASSERT(scope != NULL);
                              ROSE_ASSERT(scope->get_symbol_table() != NULL);
                              symbol->set_parent(scope->get_symbol_table());
                            }
                           else
                            {
                              printf ("Support for testing fortran lables might be incomplete! \n");
                              SgStatement* fortranStatement = tempSymbol->get_fortran_statement();
                              ROSE_ASSERT(fortranStatement != NULL);
                            }
                         
                         break;
                       }

                    case V_SgClassSymbol:
                       {
                         SgClassSymbol* tempSymbol = isSgClassSymbol(symbol);
                         ROSE_ASSERT(tempSymbol != NULL);
                         SgClassDeclaration* declaration = tempSymbol->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
                         SgScopeStatement* scope = declaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         ROSE_ASSERT(scope->get_symbol_table() != NULL);
                         symbol->set_parent(scope->get_symbol_table());
                         break;
                       }

                    case V_SgEnumSymbol:
                       {
                         SgEnumSymbol* tempSymbol = isSgEnumSymbol(symbol);
                         ROSE_ASSERT(tempSymbol != NULL);
                         SgEnumDeclaration* declaration = tempSymbol->get_declaration();
                         ROSE_ASSERT(declaration != NULL);
                         SgScopeStatement* scope = declaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         ROSE_ASSERT(scope->get_symbol_table() != NULL);
                         symbol->set_parent(scope->get_symbol_table());
                         break;
                       }

                 // DQ (2/28/2015): Added support for SgAliasSymbol case.
                    case V_SgAliasSymbol:
                       {
                         SgAliasSymbol* tempSymbol = isSgAliasSymbol(symbol);
                         ROSE_ASSERT(tempSymbol != NULL);

                      // DQ (2/28/2015): I think this is not possible to fix here, so we need to report the error and exit.
                         printf ("ERROR: parent for SgAliasSymbol not set (can't be fixed up here) \n");
                         ROSE_ASSERT(false);

                         break;
                       }

                    default:
                       {
                         printf ("Error: default reached in switch(symbol->variantT()) symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }                  
             }
          ROSE_ASSERT(symbol->get_parent() != NULL);
        }
#if 0
     if (isSgExpression(locatedNode) != NULL)
        {
          printf ("ResetParentPointersInMemoryPool::visit(): Skipping SgExpression IR Node \n");
        }
#endif
  // Skip SgExpression object for now!
     locatedNode = isSgStatement(locatedNode);

  // SgStatement and SgExpression IR nodes should always have a valid parent (except for the SgProject)
     if (locatedNode != NULL)
        {
          switch (locatedNode->variantT())
             {
               case V_SgClassDeclaration:
               case V_SgTemplateInstantiationDecl:
                  {
                 // At this point the AST traversal has been used to set the parents and we can use information 
                 // from defining and non-defining declaration to set parents of extrainious non-defining 
                 // declarations accessible only from the memory pool.  We only reset NULL pointers.
                    SgClassDeclaration* declaration = isSgClassDeclaration(locatedNode);
                    if (declaration != NULL && declaration->get_parent() == NULL)
                       {
                         SgDeclarationStatement* definingDeclaration    = declaration->get_definingDeclaration();
                         SgDeclarationStatement* nondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
#if 0
                      // DQ (8/2/2019): Debugging nondefining template instantiation if global scope (test2019_501.C).
                         printf ("ResetParentPointersInMemoryPool::visit(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                         if (definingDeclaration != NULL)
                            {
                              printf (" --- definingDeclaration = %p = %s \n",definingDeclaration,definingDeclaration->class_name().c_str());
                            }
                         if (nondefiningDeclaration != NULL)
                            {
                              printf (" --- nondefiningDeclaration = %p = %s \n",nondefiningDeclaration,nondefiningDeclaration->class_name().c_str());
                            }
#endif
                         SgNode* parentOfRelatedDeclaration = NULL;
                         if (definingDeclaration != NULL)
                            {
                              parentOfRelatedDeclaration = definingDeclaration->get_parent();
                            }
                         if (parentOfRelatedDeclaration == NULL && nondefiningDeclaration != NULL)
                            {
                              parentOfRelatedDeclaration = nondefiningDeclaration->get_parent();
                            }

                         if (parentOfRelatedDeclaration != NULL)
                            {
#if 0
                              printf ("Note: declaration->set_parent() using parentOfRelatedDeclaration = %p = %s \n",
                                   parentOfRelatedDeclaration,parentOfRelatedDeclaration->class_name().c_str());
#endif
                              declaration->set_parent(parentOfRelatedDeclaration);
                            }
#if 1
                         if (declaration->get_parent() == NULL)
                            {
                            // DQ (3/6/2017): Converted to use message logging.
                            // mprintf ("##### ResetParentPointersInMemoryPool::visit(declaration = %p = %s) declaration->get_parent() == NULL \n",node,node->class_name().c_str());
#if 0
                               printf ("##### ResetParentPointersInMemoryPool::visit(declaration = %p = %s) declaration->get_parent() == NULL \n",node,node->class_name().c_str());
#endif
                            // DQ (8/23/2012): For remaining template instantiationsthat only have a non-definng declaration, set the parent to the global scope (since they don't appear to be connected to anything else).
                               SgTemplateInstantiationDecl* templateInstantiation = isSgTemplateInstantiationDecl(nondefiningDeclaration);
                               if (templateInstantiation != NULL)
                                  {
                                    printf ("WARNING: This is a case of a template class instantiation that does not appear in the AST but exists in the memory pool as part of the new refined disambiguation of template instantations using template arguments. \n");

                                 // I built a pointer to global scope so that we could use it for this case.
                                    ROSE_ASSERT(globalScope != NULL);
                                    templateInstantiation->set_parent(globalScope);
                                  }
                            }
#endif
                      // DQ (6/10/2007): Test for null parents before the call to resetTemplateNames()
                      // DQ (6/22/2006): Commented out temporarily for debugging use of glob.h

#if 0
                      // DQ (3/3/2012): I think we might not need to have this test (or at least I want to debug this case using the AST Whole AST dot graph).
                      // This is a problem (failing test) for test2006_01.C.
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                         printf ("WARNING: In ResetParentPointersInMemoryPool::visit(declaration = %p = %s): test commented out for declaration->get_parent() != NULL \n",node,node->class_name().c_str());
#endif
                      // ROSE_ASSERT(declaration->get_parent() != NULL);
#else
                      // DQ (8/3/2019): This assertion is a problem (failing C language test) for test2012_47.c (and about 27 other C language tests).
                      // DQ (8/2/2019): reintroduce this asseretion.
                      // ROSE_ASSERT(declaration->get_parent() != NULL);
#endif
                       }

                 // DQ (6/22/2006): Commented out temporarily for debugging use of glob.h
                 // ROSE_ASSERT(locatedNode->get_parent() != NULL);
                    break;
                  }

               case V_SgFunctionDeclaration:
               case V_SgMemberFunctionDeclaration:
                  {
                    SgNode* parent = locatedNode->get_parent();
                    if (parent == NULL)
                       {
#if 0
                         printf ("Case of locatedNode = %p = %s parent = %p = %s \n",
                              locatedNode,locatedNode->class_name().c_str(),parent,(parent != NULL) ? parent->class_name().c_str() : "Null");

                         printf ("Error: NULL parent found \n");
                         locatedNode->get_file_info()->display("Error: NULL parent found: debug");
#endif
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(locatedNode);
                         ROSE_ASSERT(functionDeclaration != NULL);
                         SgDeclarationStatement* definingDeclaration    = functionDeclaration->get_definingDeclaration();
                         SgDeclarationStatement* nondefiningDeclaration = functionDeclaration->get_firstNondefiningDeclaration();

                         if (nondefiningDeclaration == NULL)
                            {
                           // DQ (9/13/2011): Reported as possible NULL value in static analysis of ROSE code.
                              ROSE_ASSERT(functionDeclaration->get_parent() != NULL);

                              printf ("Error: nondefiningDeclaration == NULL for functionDeclaration = %p = %s \n",functionDeclaration,SageInterface::get_name(functionDeclaration).c_str());
                              printf ("   definingDeclaration = %p \n",definingDeclaration);
                              printf ("   functionDeclaration->get_parent() = %p = %s \n",functionDeclaration->get_parent(),functionDeclaration->get_parent()->class_name().c_str());
                            }
                         ROSE_ASSERT(nondefiningDeclaration != NULL);
#if PRINT_DEVELOPER_WARNINGS
                         printf ("Warning from ResetParentPointersInMemoryPool::visit(): parent == NULL for function name = %s definingDeclaration = %p nondefiningDeclaration = %p parent = %p \n",
                              functionDeclaration->get_name().str(),definingDeclaration,nondefiningDeclaration,nondefiningDeclaration->get_parent());
#endif
                      // ROSE_ASSERT(nondefiningDeclaration->get_parent() != NULL);
                         if (definingDeclaration != NULL)
                            {
                              ROSE_ASSERT(definingDeclaration->get_parent() != NULL);

                           // Make the parent the same for both the defining and nondefining declarations
                              if (nondefiningDeclaration->get_parent() == NULL)
                                 {
                                // This happens in the case where a member function is used before it is declared (a case where the parent was not set in the EDG/SageIII translation).
#if PRINT_DEVELOPER_WARNINGS
                                   printf ("Setting the nondefiningDeclaration->get_parent() == NULL using definingDeclaration->get_parent() = %p \n",definingDeclaration->get_parent());
#endif
                                   ROSE_ASSERT(definingDeclaration->get_parent() != NULL);
                                   nondefiningDeclaration->set_parent(definingDeclaration->get_parent());
                                 }
                              ROSE_ASSERT(nondefiningDeclaration->get_parent() != NULL);
                            }
                       }

                  if (locatedNode->get_parent()==NULL) 
                  {
                    SageInterface::dumpInfo(locatedNode,"ResetParentPointersInMemoryPool::visit() error: found a func dec without defining declaration and its non-defining declaration has no scope info. ");
                    ROSE_ASSERT(locatedNode->get_parent() != NULL);
                  }
                    break;
                  }
#if 0
            // DQ (6/26/2006): If we don't reset the parent pointer then this code is redundant with the AST Consistancy testing
               case V_SgVariableDeclaration:
                  {
                    SgNode* parent = locatedNode->get_parent();
                    if (parent == NULL)
                       {
                         printf ("Case of locatedNode = %p = %s parent = %p = %s \n",
                              locatedNode,locatedNode->class_name().c_str(),parent,(parent != NULL) ? parent->class_name().c_str() : "Null");

                         printf ("Error: NULL parent found \n");
                         locatedNode->get_file_info()->display("Error: NULL parent found: debug");
                       }
                 // ROSE_ASSERT(locatedNode->get_parent() != NULL);
                    break;
                  }
#endif
#if 0
            // DQ (6/26/2006): If we don't reset the parent pointer then this code is redundant with the AST Consistancy testing
               case V_SgBasicBlock:
                  {
                    SgNode* parent = locatedNode->get_parent();
                    if (parent == NULL)
                       {
                         printf ("Error: case V_SgBasicBlock: NULL parent found \n");
                         locatedNode->get_file_info()->display("Error: NULL parent found: debug");
                       }
                 // DQ (6/23/2006): Commented out temporarily for debugging use of STL!
                 // ROSE_ASSERT(locatedNode->get_parent() != NULL);
                    break;
                  }
#endif
               case V_SgInitializedName:
               default:
                  {
#if 0
                 // DQ (6/26/2006): If we don't reset the parent pointer then this code is redundant with the AST Consistancy testing
                    if (locatedNode->get_parent() == NULL)
                       {
                         printf ("OK if root of subtree: locatedNode->get_parent() == NULL for support = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
                         locatedNode->get_file_info()->display("Error: locatedNode->get_parent() == NULL");
                       }
                 // DQ (6/24/2006): When building tree fragments and setting the parent, the parent of the 
                 // root node of the subtree can't be set until it is attached to the AST, so is has to be 
                 // OK to find an IR node with a NULL parent!
                 // ROSE_ASSERT(locatedNode->get_parent() != NULL);
#endif
                    break;
                  }
             }
        }

  // Some SgSupport IR nodes have a valid parent
     if (support != NULL)
        {
#if 1
       // DQ (6/26/2006): Set the parent pointer to a type to collect them for visualization.
          Sg_File_Info* fileInfo = isSg_File_Info(support);
          if (fileInfo != NULL && fileInfo->get_parent() == NULL)
             {
            // This is a detached Sg_File_Info object (else it would have been set 
            // properly in the ResetFileInfoParentPointersInMemoryPool traversal).
            // by default we set it to the get_globalFunctionTypeTable() (somewhat 
            // arbitrarily this helps me figure out what Sg_File_Info objects are 
            // built redundantly).
            // support->set_parent(SgTypeShort::createType()get_globalFunctionTypeTable());
#if 0
               fileInfo->set_parent(SgTypeShort::createType());
               ROSE_ASSERT(fileInfo->get_parent() != NULL);
#endif

            // DQ (12/23/2006): Sg_File_Info objects are used in non-SgNode objects to record source position information, 
            // when this is done we add extra flags to the classification so that we can expect when the parent pointer 
            // will be NULL.  In these cases the parent pointer can't be used to point to the object using the Sg_File_Info
            // object since it is not dirived from SgNode.  This is a diesn issue and may be addressed differently in the 
            // future.  We want to skip setting the parent in this case and avoid considering it to be an error.
               if (fileInfo->isCommentOrDirective() == false && fileInfo->isToken() == false)
                  {
#if PRINT_DEVELOPER_WARNINGS
                    printf ("ResetParentPointersInMemoryPool::visit(): Valid fileInfo = %p has parent == NULL \n",fileInfo);
                 // fileInfo->display("ResetParentPointersInMemoryPool::visit(): fileInfo->get_parent() == NULL");
#endif
                  }

            // printf ("Make this an error now to have a Sg_File_Info object with a NULL parent \n");
            // ROSE_ASSERT(false);
             }
#else
       // DQ (6/26/2006): If we don't reset the parent pointer then this code is redundant with the AST Consistancy testing
          switch (support->variantT())
             {
            // DQ (6/26/2006): General testing in now done in the AST Consistancy tests
               case V_SgProject:
                  {
                    ROSE_ASSERT(support->get_parent() == NULL);
                    break;
                  }

#error "DEAD CODE!"

               case V_SgStorageModifier:
            // case V_SgInitializedName:
               case V_SgSymbolTable:
            // case V_SgFile:
               case V_SgSourceFile:
               case V_SgBinaryComposite:
               case V_SgTypedefSeq:
               case V_SgFunctionParameterTypeList:
               case V_SgPragma:
               case V_SgBaseClass:
                  {
                    SgNode* parent = support->get_parent();
                    if (parent == NULL)
                       {
#if 0
                         printf ("Case of SgSupport support = %p = %s parent = %p = %s \n",
                              support,support->class_name().c_str(),parent,(parent != NULL) ? parent->class_name().c_str() : "Null");
#endif
                      // printf ("Error: NULL parent found \n");
                      // ROSE_ASSERT(support->get_file_info() != NULL);
                      // support->get_file_info()->display("Error: NULL parent found");
                       }
                 // ROSE_ASSERT(support->get_parent() != NULL);
                    break;
                  }

#error "DEAD CODE!"

               case V_Sg_File_Info:
                  {
                    if (support->get_parent() == NULL)
                       {
                      // This is a detached Sg_File_Info object (else it would have been set 
                      // properly in the ResetFileInfoParentPointersInMemoryPool traversal).
                      // by default we set it to the get_globalFunctionTypeTable() (somewhat 
                      // arbitrarily this helps me figure out what Sg_File_Info objects are 
                      // built redundantly).
                      // support->set_parent(SgTypeShort::createType()get_globalFunctionTypeTable());
                         support->set_parent(SgTypeShort::createType());
                       }
                    ROSE_ASSERT(support->get_parent() != NULL);
                    break;
                  }

#error "DEAD CODE!"

               default:
                  {
                    if (support->get_parent() != NULL)
                       {
                         printf ("##### ResetParentPointersInMemoryPool::visit(node = %p = %s) support->get_parent() != NULL \n",node,node->sage_class_name());
                       }
                    ROSE_ASSERT(support->get_parent() == NULL);
                    break;
                  }
             }
#endif

        }

#if 0
  // DQ (6/10/2007): Test for null parents before the call to resetTemplateNames()
     if (node->get_parent() != NULL)
        {

#error "DEAD CODE!"

          printf ("##### ResetParentPointersInMemoryPool::visit(node = %p = %s) node->get_parent() != NULL \n",node,node->sage_class_name());
        }
     ROSE_ASSERT(node->get_parent() == NULL);
#endif
   }



#include "sage3basic.h"
#include "newHiddenList.h"

using namespace std;


// *******************************************************
// Main calling function to support hidden list evaluation
// *******************************************************

void
newBuildHiddenTypeAndDeclarationLists( SgNode* node )
   {
  // DQ (8/1/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("New hidden type and hidden declaration list generation:");

     SgSourceFile* project = isSgSourceFile(node);

     if (project == NULL)
        {
          printf ("Can't process buildHiddenTypeAndDeclarationLists for AST fragment = %p = %s \n",node,node->class_name().c_str());
          return;
        }
     ROSE_ASSERT(project != NULL);

     HiddenListTraversal t;
     HiddenListInheritedAttribute ih;

  // Call the traversal.
     t.traverse(node,ih);
   }


// *******************
// Inherited Attribute
// *******************

HiddenListInheritedAttribute::HiddenListInheritedAttribute()
   {
  // Default constructor
   }

HiddenListInheritedAttribute::HiddenListInheritedAttribute ( const HiddenListInheritedAttribute & X )
   {
  // Copy constructor.
   }



// *********************
// Synthesized Attribute
// *********************

HiddenListSynthesizedAttribute::HiddenListSynthesizedAttribute()
   {
  // Default constructor
   }


HiddenListSynthesizedAttribute::HiddenListSynthesizedAttribute ( const HiddenListSynthesizedAttribute & X )
   {
  // Copy constructor.
   }


// *******************
// HiddenListTraversal
// *******************

HiddenListTraversal::HiddenListTraversal()
   {
  // Default constructor
   }

#if 0
bool
HiddenListTraversal::isANamedConstruct(SgNode* n)
   {
  // If this is a declaration of a type (e.g. SgclassDeclaration) then return true.
     return false;
   }
#endif


SgDeclarationStatement*
HiddenListTraversal::associatedDeclaration(SgScopeStatement* scope)
   {
     SgDeclarationStatement* return_declaration = NULL;
     switch (scope->variantT())
        {
          case V_SgClassDefinition:
             {
               SgClassDefinition* definition = isSgClassDefinition(scope);
               ROSE_ASSERT(definition != NULL);

               SgClassDeclaration* declaration = definition->get_declaration();
               ROSE_ASSERT(declaration != NULL);

               return_declaration = declaration;
               break;
             }

          case V_SgNamespaceDefinitionStatement:
             {
               SgNamespaceDefinitionStatement* definition = isSgNamespaceDefinitionStatement(scope);
               ROSE_ASSERT(definition != NULL);

               SgNamespaceDeclarationStatement* declaration = definition->get_namespaceDeclaration();
               ROSE_ASSERT(declaration != NULL);

               return_declaration = declaration;
               break;
             }

          case V_SgTemplateInstantiationDefn:
             {
               SgTemplateInstantiationDefn* definition = isSgTemplateInstantiationDefn(scope);
               ROSE_ASSERT(definition != NULL);

               SgTemplateInstantiationDecl* declaration = isSgTemplateInstantiationDecl(definition->get_declaration());
               ROSE_ASSERT(declaration != NULL);

               return_declaration = declaration;
               break;
             }

       // Some scopes don't have an associated declaration (return NULL in these cases).
       // Also missing some of the Fortran specific scopes.
          case V_SgGlobal:
          case V_SgIfStmt:
          case V_SgWhileStmt:
          case V_SgDoWhileStmt:
          case V_SgForStatement:
          case V_SgForAllStatement:
          case V_SgBasicBlock:
          case V_SgSwitchStatement:
          case V_SgCatchOptionStmt:
          case V_SgUpcForAllStatement:
             {
               return_declaration = NULL;
               break;
             }

       // Catch anything that migh have been missed (and exit so it can be identified and fixed).
          default:
             {
               printf ("Default reached in HiddenListTraversal::associatedDeclaration() scope = %s \n",scope->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

     return return_declaration;
   }

SgDeclarationStatement*
HiddenListTraversal::associatedDeclaration(SgType* type)
   {
     SgDeclarationStatement* return_declaration = NULL;

  // We want to strip away all by typedef types.
     SgType* strippedType = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
     ROSE_ASSERT(strippedType != NULL);

  // switch (type->variantT())
     switch (strippedType->variantT())
        {
          case V_SgClassType:
             {
               SgClassType* classType = isSgClassType(strippedType);
               ROSE_ASSERT(classType != NULL);

               SgClassDeclaration* declaration = isSgClassDeclaration(classType->get_declaration());
               ROSE_ASSERT(declaration != NULL);

               return_declaration = declaration;
               break;
             }

          case V_SgTypedefType:
             {
               SgTypedefType* typedefType = isSgTypedefType(strippedType);
               ROSE_ASSERT(typedefType != NULL);

               SgTypedefDeclaration* declaration = isSgTypedefDeclaration(typedefType->get_declaration());
               ROSE_ASSERT(declaration != NULL);

               return_declaration = declaration;
               break;
             }

          case V_SgEnumType:
             {
               SgEnumType* enumType = isSgEnumType(strippedType);
               ROSE_ASSERT(enumType != NULL);

               SgEnumDeclaration* declaration = isSgEnumDeclaration(enumType->get_declaration());
               ROSE_ASSERT(declaration != NULL);

               return_declaration = declaration;
               break;
             }

          case V_SgFunctionType:
          case V_SgMemberFunctionType:
             {
            // Not clear if I have to resolve declarations associated with function types.
               printf ("In HiddenListTraversal::associatedDeclaration(): Case of SgFunctionType not implemented strippedType = %s \n",strippedType->class_name().c_str());

               return_declaration = NULL;
               break;
             }

       // Some scopes don't have an associated declaration (return NULL in these cases).
       // Also missing some of the Fortran specific scopes.
          case V_SgTypeInt:
          case V_SgTypeUnsignedLong:
          case V_SgTypeUnsignedChar:
          case V_SgTypeUnsignedShort:
          case V_SgTypeUnsignedInt:
          case V_SgTypeSignedChar:
          case V_SgTypeShort:
          case V_SgTypeLong:
          case V_SgTypeLongLong:
          case V_SgTypeVoid:
          case V_SgTypeChar:
          case V_SgTypeUnsignedLongLong:
          case V_SgTypeFloat:
          case V_SgTypeDouble:
          case V_SgTypeBool:
          case V_SgTypeWchar:
             {
               return_declaration = NULL;
               break;
             }

       // Catch anything that migh have been missed (and exit so it can be identified and fixed).
          default:
             {
               printf ("Default reached in HiddenListTraversal::associatedDeclaration() type = %s strippedType = %s \n",type->class_name().c_str(),strippedType->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

     return return_declaration;
   }


int
HiddenListTraversal::nameQualificationDepthOfParent(SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // Now resolve how much name qualification is required.
     int qualificationDepth = 0;

     printf ("***** Inside of HiddenListTraversal::nameQualificationDepthOfParent() ***** \n");
     printf ("   declaration  = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
     printf ("   currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());

  // qualificationDepth++;
  // SgDeclaration* classOrNamespaceDefinition = classDefinition->get_declaration()->get_scope();
     SgScopeStatement* parentScope = declaration->get_scope();
  // SgName parentName = associatedName(parentScope);

  // qualificationDepth = nameQualificationDepth(parentName,parentScope,positionStatement) + 1;
     SgGlobal* globalScope = isSgGlobal(parentScope);
     if (globalScope != NULL)
        {
       // There is no declaration associated with global scope so we have to process the case of a null pointer...
          printf ("parentDeclaration == NULL: parentScope = %p = %s \n",parentScope,parentScope->class_name().c_str());
#if 0
          printf ("This case is not handled yet! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
       // Now ask the same question recursively using the parent declaration and the same currentScope (is it visible from the same point in the code).
          SgDeclarationStatement* parentDeclaration = associatedDeclaration(parentScope);

       // In some cases the declaration can be in a scope with is not associated with a declaration (e.g. SgBasicBlock or SgForStatement).
          if (parentDeclaration != NULL)
             {
            // qualificationDepth = nameQualificationDepth(parentDeclaration,parentScope,positionStatement);
               qualificationDepth = nameQualificationDepth(parentDeclaration,currentScope,positionStatement);
             }
        }

     return qualificationDepth;
   }

bool
HiddenListTraversal::requiresTypeElaboration(SgSymbol* symbol)
   {
  // DQ (5/14/2011): type elaboration only works between non-types and types.  Different types must be distinquished using name qualification.
  // If this is a type then since all types are given equal weight we need more name qualification to distinquish them.
  // However, if this is a non-type then type elaboration is sufficent to distinquish the type (e.g. from a variable name).
     bool typeElaborationRequired = false;

     ROSE_ASSERT(symbol != NULL);
     switch (symbol->variantT())
        {
          case V_SgMemberFunctionSymbol:
          case V_SgVariableSymbol:
               typeElaborationRequired = true;
               break;

          case V_SgTemplateSymbol: // Note sure about this!!!
          case V_SgTypedefSymbol:
               typeElaborationRequired = false;
               break;

          default:
             {
               printf ("Default reached in HiddenListTraversal::requiresTypeElaboration(): symbol = %p = %s \n",symbol,symbol->class_name().c_str());
               ROSE_ASSERT(false);
             }           
        }

     return typeElaborationRequired;
   }



// int HiddenListTraversal::nameQualificationDepth ( SgScopeStatement* classOrNamespaceDefinition )
int 
HiddenListTraversal::nameQualificationDepth ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement, bool forceMoreNameQualification )
   {
  // Note that the input must be a declaration because it can include enums (SgDeclarationStatement IR nodes) 
  // that don't have a corresponding definition (SgScopeStatement IR nodes).

  // This function computes the number of qualified names required to uniquely qualify and input reference.
  // It evaluates how much name qualification is required (typically 0 (no qualification), but sometimes 
  // the depth of the nesting of scopes plus 1 (full qualification with global scoping operator)).

  // The positionStatement is the position of the associated reference to the declaration.
  // It is used when "using declarations" are not at the top of the scope.  Initially
  // we will assume that the such "using declarations" are at the top of the scope.

  // Also this function may have to be able to take a SgInitializedName, but at the moment it takes a 
  // SgVariable declaration to handle the equivalent case.  Later when we support multiple SgInitializedName 
  // IR nodes in a single SgVariableDeclaration this will have to be changed.

     ROSE_ASSERT(declaration  != NULL);
     ROSE_ASSERT(currentScope != NULL);

     int  qualificationDepth        = 0;
     bool typeElaborationIsRequired = false;
  // bool globalQualifierIsRequired = false;

     printf ("##### Inside of HiddenListTraversal::nameQualificationDepth() ##### \n");
     printf ("declaration  = %p = %s = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str(),SageInterface::generateUniqueName(declaration,true).c_str());
     printf ("currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());

     SgClassDeclaration*    classDeclaration    = isSgClassDeclaration(declaration);
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declaration);
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
     SgTypedefDeclaration*  typedefDeclaration  = isSgTypedefDeclaration(declaration);
     SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(declaration);
     SgEnumDeclaration*     enumDeclaration     = isSgEnumDeclaration(declaration);
     SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(declaration);

#if 0
  // SgClassDefinition*  classDefinition  = classDeclaration != NULL ? classDeclaration->get_definition() : NULL;
  // ROSE_ASSERT(classDeclaration != NULL);
     SgClassDefinition*  classDefinition  = classDeclaration != NULL ? isSgClassDeclaration(classDeclaration->get_definingDeclaration())->get_definition() : NULL;

     if (classDefinition != NULL)
          printf ("classDefinition = %p = %s = %s \n",classDefinition,classDefinition->class_name().c_str(),SageInterface::get_name(classDefinition).c_str());
       else
          printf ("classDefinition == NULL \n");

  // SgNamespaceDefinitionStatement*  namespaceDefinition  = namespaceDeclaration != NULL ? namespaceDeclaration->get_definition() : NULL;
#endif

  // Make sure that the definitions and declarations are consistant.
  // ROSE_ASSERT(classDefinition != NULL || namespaceDefinition != NULL);
  // ROSE_ASSERT((classDefinition != NULL && classDeclaration != NULL) || (namespaceDefinition != NULL && namespaceDeclaration != NULL));
  // ROSE_ASSERT(classDeclaration != NULL || namespaceDeclaration != NULL);
     ROSE_ASSERT(classDeclaration != NULL || namespaceDeclaration != NULL || variableDeclaration != NULL || functionDeclaration != NULL || typedefDeclaration != NULL || templateDeclaration != NULL || enumDeclaration != NULL );

  // ROSE_ASSERT((classDeclaration != NULL && classDefinition != NULL) || (namespaceDeclaration != NULL && namespaceDefinition != NULL) || variableDeclaration != NULL);

  // SgName name = (classDeclaration != NULL) ? classDeclaration->get_name() : ((namespaceDeclaration != NULL) ? namespaceDeclaration->get_name() : "unknown");
     SgName name = (classDeclaration     != NULL) ? classDeclaration->get_name()     : 
                   (namespaceDeclaration != NULL) ? namespaceDeclaration->get_name() : 
                   (variableDeclaration  != NULL) ? SageInterface::getFirstInitializedName(variableDeclaration)->get_name() : 
                   (functionDeclaration  != NULL) ? functionDeclaration->get_name()  : 
                   (typedefDeclaration   != NULL) ? typedefDeclaration->get_name()   :
                   (templateDeclaration  != NULL) ? templateDeclaration->get_name()  : 
                   (enumDeclaration      != NULL) ? enumDeclaration->get_name()      : "unknown_name";

     printf ("name = %s \n",name.str());
     ROSE_ASSERT(name.is_null() == false);

#if 0
  // DQ (5/8/2011): I think we don't need to handle this special case.

  // The global scope is the root of the tree of scopes so we can't ask for the parent of this scope and thus it is a special case.
     SgGlobal* globalScope = isSgGlobal(currentScope);
     if (globalScope != NULL)
        {
       // We could have a using declaration include another declaration with the same name into the current 
       // scope.  If there is more than one then name qalification is required to dismbiguate them.
          printf ("We still need to check for more than one name in currentScope = %s \n",currentScope->class_name().c_str());

       // Make sure that there is no ambiguity (should be only one symbol with this name).
          size_t numberOfSymbols = globalScope->count_symbol(name);
          if (numberOfSymbols > 1)
             {
               printf ("Found a case of ambiguity in the global scope (trigger global name qualifier). \n");
               qualificationDepth++;

            // I think that this means that global qualification is required.
               globalQualifierIsRequired = true;
             }

       // To debug this for now, just handle the case of a single symbol with this name.
       // ROSE_ASSERT(numberOfSymbols == 1);
        }
       else
#endif
        {
       // Note that there can be more than one symbol if the name is hidden in a base class scope (and thus there are SgAliasSymbols using the same name).
          SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope);

       // This is used to count the number of symbols of the same type in a single scope.
       // size_t numberOfSymbols = 0;
       // bool forceMoreNameQualification = false;

          if (symbol != NULL)
             {
               printf ("Lookup symbol based on name only: symbol = %p = %s \n",symbol,symbol->class_name().c_str());

               SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);
               if (aliasSymbol != NULL)
                  {
                    symbol = aliasSymbol->get_alias();
                    ROSE_ASSERT(symbol != NULL);
                  }

            // We have to check the kind of declaration against the kind of symbol found. A local variable (for example) 
            // could hide the same name used for the declaration.  This if we find symbol inconsistant with the declaration 
            // then we need some form of qualification (sometimes just type elaboration).
               switch (declaration->variantT())
                  {
                    case V_SgClassDeclaration:
                       {
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                         ROSE_ASSERT(classDeclaration != NULL);

                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                      // ROSE_ASSERT(classSymbol != NULL);
                         if (classSymbol == NULL)
                            {
                           // This is only type elaboration if it is a variable that is the conflict, if it is a typedef then more qualification is required. (see test2011_37.C).
                           // printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                           // typeElaborationIsRequired = true;
                              if (requiresTypeElaboration(symbol) == true)
                                 {
                                   printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                                   typeElaborationIsRequired = true;
                                 }
                                else
                                 {
                                // I think we have to force an extra level of name qualification.
                                   forceMoreNameQualification = true;
                                   printf ("Forcing an extra level of name qualification forceMoreNameQualification = %s \n",forceMoreNameQualification ? "true" : "false");
#if 0
                                   printf ("I think we have to force an extra level of name qualification (not implemented) \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                              
                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope);
                           // ROSE_ASSERT(symbol != NULL);
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In HiddenListTraversal::nameQualificationDepth(): classSymbol == NULL \n");
                                 }
                            }

                         break;
                       }

                    case V_SgNamespaceDeclarationStatement:
                       {
                      // There is no type elaboration for a reference to a namespace, so I am not sure what to do here.
                         SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(declaration);
                         ROSE_ASSERT(namespaceDeclaration != NULL);

                         SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(symbol);
                         ROSE_ASSERT(namespaceSymbol != NULL);

                         break;
                       }

                    case V_SgTemplateInstantiationMemberFunctionDecl:
                    case V_SgMemberFunctionDeclaration:
                    case V_SgFunctionDeclaration:
                       {
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
                         ROSE_ASSERT(functionDeclaration != NULL);

                         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                      // ROSE_ASSERT(classSymbol != NULL);
                         if (functionSymbol == NULL)
                            {
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                              typeElaborationIsRequired = true;

                              printf ("WARNING: Present implementation of symbol table will not find alias sysmbols of SgFunctionSymbol \n");

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,currentScope);

                           // ROSE_ASSERT(symbol != NULL);
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In HiddenListTraversal::nameQualificationDepth(): functionSymbol == NULL \n");
                                 }
                            }

                      // numberOfSymbols = currentScope->count_symbol(name);
                         break;
                       }

#if 0
                    case V_SgMemberFunctionDeclaration:
                       {
                         SgMemberFunctionDeclaration* functionDeclaration = isSgMemberFunctionDeclaration(declaration);
                         ROSE_ASSERT(functionDeclaration != NULL);

                         SgMemberFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                      // ROSE_ASSERT(classSymbol != NULL);
                         if (functionSymbol == NULL)
                            {
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration (this uses the same interface as for .
                              symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,currentScope);
                              ROSE_ASSERT(symbol != NULL);
                              printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                            }

                         break;
                       }
#endif
                    case V_SgTypedefDeclaration:
                       {
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(declaration);
                         ROSE_ASSERT(typedefDeclaration != NULL);

                         SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                         if (typedefSymbol == NULL)
                            {
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                              typeElaborationIsRequired = true;

                           // printf ("WARNING: Present implementation of symbol table will not find alias sysmbols of SgTypedefSymbol \n");

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupTypedefSymbolInParentScopes(name,currentScope);
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In HiddenListTraversal::nameQualificationDepth(): typedefSymbol == NULL \n");
                                 }
                              ROSE_ASSERT(symbol != NULL);
                              printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                            }

                         break;
                       }

                    case V_SgTemplateDeclaration:
                       {
                         SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(declaration);
                         ROSE_ASSERT(templateDeclaration != NULL);

                         SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
                         if (templateSymbol == NULL)
                            {
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);
                              ROSE_ASSERT(symbol != NULL);
                              printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                            }

                         break;
                       }

                    case V_SgTemplateInstantiationFunctionDecl:
                       {
                         SgTemplateInstantiationFunctionDecl* templateFunction = isSgTemplateInstantiationFunctionDecl(declaration);
                         ROSE_ASSERT(templateFunction != NULL);

                         SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
                         if (templateSymbol == NULL)
                            {
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,currentScope);
                              ROSE_ASSERT(symbol != NULL);
                              printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                            }

                         break;
                       }

                    case V_SgEnumDeclaration:
                       {
                         SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declaration);
                         ROSE_ASSERT(enumDeclaration != NULL);

                         SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                         if (enumSymbol == NULL)
                            {
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupEnumSymbolInParentScopes(name,currentScope);
                              ROSE_ASSERT(symbol != NULL);
                              printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                            }

                         break;
                       }

                    case V_SgTemplateInstantiationDecl:
                       {
                         SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
                         ROSE_ASSERT(templateInstantiationDeclaration != NULL);

                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                         if (classSymbol == NULL)
                            {
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope);
                              ROSE_ASSERT(symbol != NULL);
                              printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                            }

                         break;
                       }

                    case V_SgVariableDeclaration:
                       {
                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declaration);
                         ROSE_ASSERT(variableDeclaration != NULL);

                         SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);
                         printf ("variableSymbol = %p = %s \n",variableSymbol,symbol->class_name().c_str());
                         if (variableSymbol == NULL)
                            {
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupVariableSymbolInParentScopes(name,currentScope);
                              ROSE_ASSERT(symbol != NULL);
                              printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                            }

                         break;
                       }

                    default:
                       {
                      // Handle cases are we work through specific example codes.
                         printf ("default reached symbol = %s \n",symbol->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }
             }

       // At this point if there was any ambiguity in the first matching symbol that was found, then
       // we have resolved this to the correct type of symbol (SgClassSymbol, SgFunctionSymbol, etc.).
          if (symbol != NULL)
             {
            // DQ (5/6/2011): Now we have fixed derived class symbol tables to inject there base classes symbols into the derived class.
               SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);

            // Make sure that there is no ambiguity (should be only one symbol with this name).
               size_t numberOfSymbols      = currentScope->count_symbol(name); 
               size_t numberOfAliasSymbols = currentScope->count_alias_symbol(name); 

               printf ("symbol = %p = %s aliasSymbol = %p numberOfSymbols = %zu \n",symbol,symbol->class_name().c_str(),aliasSymbol,numberOfSymbols);

               printf ("numberOfSymbols = %zu numberOfAliasSymbols = %zu \n",numberOfSymbols,numberOfAliasSymbols);
               numberOfSymbols = numberOfSymbols - numberOfAliasSymbols;
               printf ("Corrected numberOfSymbols = %zu \n",numberOfSymbols);

            // if (numberOfSymbols > 1)
               if (numberOfSymbols > 1 || forceMoreNameQualification == true)
                  {
                 // If there is more than one symbol with the same name then name qualification is required to distinguish between them.
                 // The exception to this is overloaded member functions.  But might also be where type evaluation is required.
                    printf ("Found a case of ambiguity of declaration = %s in the currentScope = %p = %s = %s (trigger additional name qualifier). \n",
                         declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
#if 1
                    switch(declaration->variantT())
                       {
                         case V_SgFunctionDeclaration:
                            {
                           // See test2011_48.C for an example of this (the overloaded functions cause numberOfSymbols > 1.
#if 0
                           // Don't qualify function defined in their associated class.
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
                              SgScopeStatement* structurallyAssociatedScope = isSgScopeStatement(functionDeclaration->get_parent());
                              ROSE_ASSERT(structurallyAssociatedScope != NULL);

                           // Note that structurallyAssociatedDeclaration could be NULL if the function declaration is in global scope.
                              SgDeclarationStatement* structurallyAssociatedDeclaration = associatedDeclaration(structurallyAssociatedScope);
                           // SgDeclarationStatement* semanticallyAssociatedDeclaration = functionDeclaration->get_associatedClassDeclaration();
                              SgDeclarationStatement* semanticallyAssociatedDeclaration = functionDeclaration;

                              ROSE_ASSERT(semanticallyAssociatedDeclaration != NULL);

                              if (structurallyAssociatedDeclaration != semanticallyAssociatedDeclaration)
                                 {
                                // The associated class for the member function does not match its structural location so we require name qualification.
                                   printf ("The associated class for the member function does not match its structural location so we require name qualification \n");
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                                 }
#else
                              printf ("Error: Skipping name qualification for SgFunctionDeclaration (sorry, not implemented) \n");
#endif
                              break;
                            }

                         case V_SgMemberFunctionDeclaration:
                            {
                           // Don't qualify member function defined in their associated class.
                              SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declaration);
                              SgScopeStatement* structurallyAssociatedScope = isSgScopeStatement(memberFunctionDeclaration->get_parent());
                              ROSE_ASSERT(structurallyAssociatedScope != NULL);

                           // Note that structurallyAssociatedDeclaration could be NULL if the function declaration is in global scope.
                              SgDeclarationStatement* structurallyAssociatedDeclaration = associatedDeclaration(structurallyAssociatedScope);
                              SgDeclarationStatement* semanticallyAssociatedDeclaration = memberFunctionDeclaration->get_associatedClassDeclaration();

                              ROSE_ASSERT(semanticallyAssociatedDeclaration != NULL);

                              if (structurallyAssociatedDeclaration != semanticallyAssociatedDeclaration)
                                 {
                                // The associated class for the member function does not match its structural location so we require name qualification.
                                   printf ("The associated class for the member function does not match its structural location so we require name qualification \n");
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                                 }

                              break;
                            }

                         case V_SgEnumDeclaration:
                            {
                           // An Enum can have a tag and it will be the it scope and trigger unwanted name qualification.
                           // SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declaration);

                           // I think what we want to do is recognize when there enum declaration is declared directly in the typedef.
                           // We now make sure that name qualification is not called in this case, so we should not reach this point!
                              printf ("Error: Skipping name qualification for enum types (sorry, not implemented) \n");

                           // We do reach this point in test2004_105.C
                           // ROSE_ASSERT(false);

                              break;
                            }

                         default:
                            {
                           // But we have to keep moving up the chain of scopes to see if the parent might also require qualification.
                              qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                            }
                       }
#else
                 // Don't qualify member function defined in their associated class.
                    SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declaration);
                    if (memberFunctionDeclaration != NULL)
                       {
                         SgScopeStatement* structurallyAssociatedScope = isSgScopeStatement(memberFunctionDeclaration->get_parent());
                         ROSE_ASSERT(structurallyAssociatedScope != NULL);

                      // Note that structurallyAssociatedDeclaration could be NULL if the function declaration is in global scope.
                         SgDeclarationStatement* structurallyAssociatedDeclaration = associatedDeclaration(structurallyAssociatedScope);
                         SgDeclarationStatement* semanticallyAssociatedDeclaration = memberFunctionDeclaration->get_associatedClassDeclaration();

                         ROSE_ASSERT(semanticallyAssociatedDeclaration != NULL);

                         if (structurallyAssociatedDeclaration != semanticallyAssociatedDeclaration)
                            {
                           // The associated class for the member function does not match its structural location so we require name qualification.
                              printf ("The associated class for the member function does not match its structural location so we require name qualification \n");
                              qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                            }
                       }
                      else
                       {
                      // But we have to keep moving up the chain of scopes to see if the parent might also require qualification.
                         qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                       }
#endif
                  }
                 else
                  {
                 // The numberOfSymbols can be zero or one, because the symbol might not be the the current scope.  
                 // If it is zero then it just means that the name is visible 
                 // from the current scope by is not located in the current scope.  If it is one, then there is a 
                 // symbol matching the name and we need to check if it is associated with the same declaration or not.

                 // However, since symbol != NULL, the numberOfSymbols should be non-zero.
                 // ROSE_ASSERT(numberOfSymbols > 0);

                 // Not clear if we want to resolve this to another scope since the alias sysmbols scope 
                 // is want might have to be qualified (not the scope of the aliased declaration).
                    if (aliasSymbol != NULL)
                       {
                         printf ("Resetting the symbol to that stored in the SgAliasSymbol \n");
                         symbol = aliasSymbol->get_alias();
                       }

                    switch (symbol->variantT())
                       {
                         case V_SgClassSymbol:
                            {
                              SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                              ROSE_ASSERT(classSymbol != NULL);

                           // This is a class symbol, check if the declaration is the same.
                           // SgClassDeclaration* associatedClassDeclaration = baseClass->get_base_class();
                              SgClassDeclaration* associatedClassDeclaration = classSymbol->get_declaration();

                              ROSE_ASSERT(classDeclaration != NULL);
                              ROSE_ASSERT(associatedClassDeclaration != NULL);

                              if (associatedClassDeclaration->get_firstNondefiningDeclaration() == classDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This class is visible from where it is referenced.
                                   printf ("This class IS visible from where it is referenced \n");
                                 }
                                else
                                 {
                                // The name does not match, so the associatedClassDeclaration is hidding the base class declaration.
                                   printf ("This class is NOT visible from where it is referenced (declaration with same name does not match) \n");

                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
#if 0
                                   printf ("Exiting for unimplemented case (class) \n");
                                   ROSE_ASSERT(false);
#endif
                                 }

                              break;
                            }

                         case V_SgNamespaceSymbol:
                            {
                              SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(symbol);
                              ROSE_ASSERT(namespaceSymbol != NULL);
#if 0
                              printf ("This is the case of a namespace (sorry, not implemented) \n");
                              ROSE_ASSERT(false);
#endif
                              SgNamespaceDeclarationStatement* associatedNamespaceDeclaration = namespaceSymbol->get_declaration();

                              ROSE_ASSERT(namespaceDeclaration != NULL);
                              ROSE_ASSERT(associatedNamespaceDeclaration != NULL);

                              if (associatedNamespaceDeclaration->get_firstNondefiningDeclaration() == namespaceDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This class is visible from where it is referenced.
                                   printf ("This namespace IS visible from where it is referenced \n");
                                 }
                                else
                                 {
                                // The name does not match, so the associatedClassDeclaration is hidding the base class declaration.
                                   printf ("This namespace is NOT visible from where it is referenced (declaration with same name does not match) \n");

                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
#if 0
                                   printf ("Exiting for unimplemented case (namespace) \n");
                                   ROSE_ASSERT(false);
#endif
                                 }

                              break;
                            }

                         case V_SgVariableSymbol:
                            {
                              SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);
                              ROSE_ASSERT(variableSymbol != NULL);

                           // This is a variable symbol, check if the declaration is the same.
                           // SgVariableDeclaration* associatedVariableDeclaration = variableSymbol->get_declaration();
                              SgInitializedName* associatedInitializedName = variableSymbol->get_declaration();

                              ROSE_ASSERT(variableDeclaration != NULL);
                              ROSE_ASSERT(associatedInitializedName != NULL);

                           // if (associatedInitializedName->get_firstNondefiningDeclaration() == variableDeclaration->get_firstNondefiningDeclaration())
                              if (associatedInitializedName == SageInterface::getFirstInitializedName(variableDeclaration))
                                 {
                                // This class is visible from where it is referenced.
                                   printf ("This variable IS visible from where it is referenced \n");
                                 }
                                else
                                 {
                                // The name does not match, so the associatedClassDeclaration is hidding the base class declaration.
                                   printf ("This variable is NOT visible from where it is referenced (declaration with same name does not match) \n");

                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
#if 0
                                   printf ("Exiting for unimplemented case (class) \n");
                                   ROSE_ASSERT(false);
#endif
                                 }

#if 0
                              printf ("This is the case of a variable (sorry, not implemented) \n");
                              ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case V_SgMemberFunctionSymbol:
                         case V_SgFunctionSymbol:
                            {
                              SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                              ROSE_ASSERT(functionSymbol != NULL);

                           // This is a function symbol, check if the declaration is the same.
                              SgFunctionDeclaration* associatedFunctionDeclaration = functionSymbol->get_declaration();

                              ROSE_ASSERT(functionDeclaration != NULL);
                              ROSE_ASSERT(associatedFunctionDeclaration != NULL);

                              if (associatedFunctionDeclaration->get_firstNondefiningDeclaration() == functionDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This function is visible from where it is referenced.
                                   printf ("This function or member function IS visible from where it is referenced \n");
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the base class declaration.
                                   printf ("This function or member function is NOT visible from where it is referenced (declaration with same name does not match) \n");

                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
#if 0
                                   printf ("Exiting for unimplemented case (function) \n");
                                   ROSE_ASSERT(false);
#endif
                                 }

                              break;
                            }

                         case V_SgTypedefSymbol:
                            {
                              SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                              ROSE_ASSERT(typedefSymbol != NULL);

                           // This is a typdef symbol, check if the declaration is the same.
                              SgTypedefDeclaration* associatedTypedefDeclaration = typedefSymbol->get_declaration();

                              printf ("associatedTypedefDeclaration = %p = %s \n",associatedTypedefDeclaration,associatedTypedefDeclaration->get_name().str());

                              ROSE_ASSERT(typedefDeclaration != NULL);
                              ROSE_ASSERT(associatedTypedefDeclaration != NULL);

                              printf ("associatedTypedefDeclaration->get_firstNondefiningDeclaration() = %p \n",associatedTypedefDeclaration->get_firstNondefiningDeclaration());
                              printf ("typedefDeclaration->get_firstNondefiningDeclaration()           = %p \n",typedefDeclaration->get_firstNondefiningDeclaration());
                              printf ("currentScope                                                    = %p = %s \n",currentScope,currentScope->class_name().c_str());

                              if (associatedTypedefDeclaration->get_firstNondefiningDeclaration() == typedefDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This typedef is visible from where it is referenced.
                                   printf ("This typedef IS visible from where it is referenced \n");
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the base class declaration.
                                   printf ("This typedef is NOT visible from where it is referenced (declaration with same name does not match) \n");

                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
#if 0
                                   printf ("Exiting for unimplemented case (typedef) \n");
                                   ROSE_ASSERT(false);
#endif
                                 }

                              break;
                            }

                         case V_SgTemplateSymbol:
                            {
                              SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
                              ROSE_ASSERT(templateSymbol != NULL);

#if 0
                           // This is a template symbol, check if the declaration is the same.
                              SgTemplateDeclaration* associatedTemplateDeclaration = templateSymbol->get_declaration();

                           // Comment this out for now...
                              ROSE_ASSERT(templateDeclaration != NULL);
                              ROSE_ASSERT(associatedTemplateDeclaration != NULL);

                              if (associatedTemplateDeclaration->get_firstNondefiningDeclaration() == templateDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This class is visible from where it is referenced.
                                   printf ("This template IS visible from where it is referenced \n");
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the base class declaration.
                                   printf ("This template is NOT visible from where it is referenced (declaration with same name does not match) \n");

                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
#if 0
                                   printf ("Exiting for unimplemented case (typedef) \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
#else
                              printf ("WARNING: Support for name qualification for SgTemplateInstantiationFunctionDecl is not implemented yet \n");
#endif
                              break;
                            }

                         case V_SgEnumSymbol:
                            {
                              SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                              ROSE_ASSERT(enumSymbol != NULL);

                           // This is a typdef symbol, check if the declaration is the same.
                              SgEnumDeclaration* associatedEnumDeclaration = enumSymbol->get_declaration();

                              ROSE_ASSERT(enumDeclaration != NULL);
                              ROSE_ASSERT(associatedEnumDeclaration != NULL);

                              if (associatedEnumDeclaration->get_firstNondefiningDeclaration() == enumDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This class is visible from where it is referenced.
                                   printf ("This enum IS visible from where it is referenced \n");
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the base class declaration.
                                   printf ("This enum is NOT visible from where it is referenced (declaration with same name does not match) \n");

                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
#if 0
                                   printf ("Exiting for unimplemented case (typedef) \n");
                                   ROSE_ASSERT(false);
#endif
                                 }

                              break;
                            }

                         default:
                            {
                           // Handle cases are we work through specific example codes.
                              printf ("default reached symbol = %s \n",symbol->class_name().c_str());
                              ROSE_ASSERT(false);
                            }
                       }
                  }
             }
            else
             {
            // This class is visible from where it is referenced.
               printf ("This class is NOT visible from where it is referenced (no declaration with same name) \n");

               qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
             }
        }

     printf ("Report type elaboration: typeElaborationIsRequired = %s \n",(typeElaborationIsRequired == true) ? "true" : "false");

     return qualificationDepth;
   }



SgDeclarationStatement* 
HiddenListTraversal::getDeclarationAssociatedWithType( SgType* type )
   {
     ROSE_ASSERT(type != NULL);

     printf ("In getDeclarationAssociatedWithType(): type = %s \n",type->class_name().c_str());

#if 1
     SgDeclarationStatement* declaration = type->getAssociatedDeclaration();
#else
     SgDeclarationStatement* declaration = NULL;

  // Resolve to the base type (but not past typedef types).  Note that a subtle point is that 
  // we want to avoid expressing typedefs that are private in functions that are public.
  // SgType* strippedType = type->stripType();
     SgType* strippedType = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
     ROSE_ASSERT(strippedType != NULL);

     printf ("In nameQualificationDepthForType(): strippedType = %s \n",strippedType->class_name().c_str());

  // Check if this is a named type (either SgClassType, SgEnumType, or SgTypedefType).
     SgNamedType* namedType = isSgNamedType(strippedType);
     if (namedType != NULL)
        {
       // Look for the declaration and evaluate if it required qualification.
          declaration = namedType->get_declaration();
          ROSE_ASSERT(declaration != NULL);

          printf ("In HiddenListTraversal::getDeclarationAssociatedWithType(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
        }
#endif

     return declaration;
   }

int
HiddenListTraversal::nameQualificationDepthForType ( SgInitializedName* initializedName, SgStatement* positionStatement )
   {
     int amountOfNameQualificationRequired = 0;

#if 1
     printf ("In nameQualificationDepthForType(): initializedName = %s type = %s \n",initializedName->get_name().str(),initializedName->get_type()->class_name().c_str());

     SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());
     if (declaration != NULL)
        {
          SgScopeStatement* currentScope = initializedName->get_scope();

       // Check the visability and unambiguity of this declaration.
          amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement);
          printf ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

       // This is where the template argument hide, they must be evaluated for name qualification requirements.
          SgTemplateInstantiationDecl* templateInstatiationDecl = isSgTemplateInstantiationDecl(declaration);
          if (templateInstatiationDecl != NULL)
             {
               SgTemplateArgumentPtrList & templateArgumentList = templateInstatiationDecl->get_templateArguments();
               SgTemplateArgumentPtrList::iterator i = templateArgumentList.begin();
               while (i != templateArgumentList.end())
                  {
                    SgTemplateArgument* templateArgument = *i;
                    ROSE_ASSERT(templateArgument != NULL);
                    SgType* type = templateArgument->get_type();
                    if (type != NULL)
                       {
                         SgNamedType* namedType = isSgNamedType(type);
                         if (namedType != NULL)
                            {
                           // This could be a type that requires name qualification (reference to a declaration).

                              SgDeclarationStatement* templateArgumentTypeDeclaration = getDeclarationAssociatedWithType(type);
                              if (templateArgumentTypeDeclaration != NULL)
                                 {
                                // Check the visability and unambiguity of this declaration.
                                   int amountOfNameQualificationRequiredForTemplateArgument = nameQualificationDepth(templateArgumentTypeDeclaration,currentScope,positionStatement);
                                   printf ("amountOfNameQualificationRequiredForTemplateArgument = %d \n",amountOfNameQualificationRequiredForTemplateArgument);

                                   printf ("Must call a function to set the name qualification data in the SgTemplateArgument = %p \n",templateArgument);

                                   setNameQualification(templateArgument,templateArgumentTypeDeclaration,amountOfNameQualificationRequiredForTemplateArgument);
#if 0
                                   printf ("Found a SgTemplateArgument using a type that DOES have a declaration: now inspect the declaration \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
#if 0
                              printf ("Found a SgTemplateArgument using a type that could have a declaration: now inspect the template arguments \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }

                    i++;
                  }
#if 0
               printf ("Found a SgTemplateInstantiationDecl: need inspect the template arguments \n");
               ROSE_ASSERT(false);
#endif
             }
        }
#else
     SgType* type = initializedName->get_type();
     ROSE_ASSERT(type != NULL);

     printf ("In nameQualificationDepthForType(): initializedName = %s type = %s \n",initializedName->get_name().str(),type->class_name().c_str());

  // Resolve to the base type (but not past typedef types).  Note that a subtle point is that 
  // we want to avoid expressing typedefs that are private in functions that are public.
  // SgType* strippedType = type->stripType();
     SgType* strippedType = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
     ROSE_ASSERT(strippedType != NULL);

     printf ("In nameQualificationDepthForType(): initializedName = %s strippedType = %s \n",initializedName->get_name().str(),strippedType->class_name().c_str());

  // Check if this is a named type (either SgClassType, SgEnumType, or SgTypedefType).
     SgNamedType* namedType = isSgNamedType(strippedType);
     if (namedType != NULL)
        {
       // Look for the declaration and evaluate if it required qualification.

          SgDeclarationStatement* declaration = namedType->get_declaration();
          ROSE_ASSERT(declaration != NULL);

          SgScopeStatement* currentScope = initializedName->get_scope();

       // Check the visability and unambiguity of this declaration.
          amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement);
          printf ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#if 0
          printf ("Found a named type \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

     return amountOfNameQualificationRequired;
   }


#if 1
int
HiddenListTraversal::nameQualificationDepth ( SgInitializedName* initializedName, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
     int amountOfNameQualificationRequired = 0;

     ROSE_ASSERT(initializedName != NULL);
  // SgScopeStatement* currentScope = initializedName->get_scope();
     ROSE_ASSERT(currentScope != NULL);
#if 0
  // If this is global scope then don't try to take it's scope.
     if (isSgGlobal(currentScope) == NULL)
          currentScope = currentScope->get_scope();
     ROSE_ASSERT(currentScope != NULL);
#endif

     SgName name = initializedName->get_name();

     SgDeclarationStatement* declaration = associatedDeclaration(initializedName->get_scope());
  // ROSE_ASSERT(declaration != NULL);

     SgVariableSymbol* variableSymbol = NULL;
     SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope);

     printf ("In HiddenListTraversal::nameQualificationDepth(SgInitializedName*): symbol = %p \n",symbol);
     if (symbol != NULL)
        {
          printf ("Lookup symbol based on name only: symbol = %p = %s \n",symbol,symbol->class_name().c_str());

          variableSymbol = isSgVariableSymbol(symbol);
          if (variableSymbol == NULL)
             {
               variableSymbol = SageInterface::lookupVariableSymbolInParentScopes(name,currentScope);

            // ROSE_ASSERT(variableSymbol != NULL);
               if (variableSymbol != NULL)
                  {
                    printf ("Lookup symbol based symbol type: variableSymbol = %p = %s \n",variableSymbol,variableSymbol->class_name().c_str());
                  }
                 else
                  {
                    printf ("In HiddenListTraversal::nameQualificationDepth(SgInitializedName*,SgScopeStatement*,SgStatement*): variableSymbol == NULL \n");
                  }

            // amountOfNameQualificationRequired = nameQualificationDepth(associatedDeclaration(initializedName->get_scope()),currentScope,positionStatement) + 1;
            // SgDeclarationStatement* declaration = associatedDeclaration(initializedName->get_scope());
               ROSE_ASSERT(declaration != NULL);
               amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
             }
        }
       else
        {
       // Symbol for the SgInitializedName is not in the current scope or those of parent scopes.  So some name qualification is required.
       // amountOfNameQualificationRequired = nameQualificationDepth(associatedDeclaration(initializedName->get_scope()),currentScope,positionStatement) + 1;
       // SgDeclarationStatement* declaration = associatedDeclaration(initializedName->get_scope());
       // amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
       // ROSE_ASSERT(declaration != NULL);

       // See test2004_34.C for an example of where declaration == NULL
          if (declaration != NULL)
             {
               amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
             }
        }

  // amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement);
     printf ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

     return amountOfNameQualificationRequired;
   }
#endif


HiddenListInheritedAttribute
HiddenListTraversal::evaluateInheritedAttribute(SgNode* n, HiddenListInheritedAttribute inheritedAttribute)
   {
     ROSE_ASSERT(n != NULL);
     printf ("Inside of HiddenListTraversal::evaluateInheritedAttribute(): node = %p = %s \n",n,n->class_name().c_str());

  // Locations where name qualified references can exist:
  //   1) Base class names
  //   2) Variable names in declarations (see test2011_30.C)
  //   3) Types referenced by variables
  //   4) Types referenced in function parameter lists
  //   5) Return types referenced by functions
  //   6) References to functions thrown by functions
  //   7) Namespaces referenced by SgUsingDirectiveStatement IR nodes
  //   8) Variables and declarations reference from SgUsingDeclarationStatement IR nodes
  //   9) Functions reference by SgFunctionRefExp IR nodes
  //  10) Functions reference by SgMemberFunctionRefExp IR nodes
  //  11) Variable reference by SgVarRefExp IR nodes
  //  12) Template arguments
  //  13) Template parameters?
  //  14) Function declarations
  //  15) Member function declarations
  //  16) Typedef declarations
  //  17)
  //  18)
  //  19)
  //  20)

     SgClassDefinition* classDefinition = isSgClassDefinition(n);
     if (classDefinition != NULL)
        {
       // Add all of the named types from this class into the set that have already been seen.
       // Note that this should not include nested classes (I think).

          SgBaseClassPtrList & baseClassList = classDefinition->get_inheritances();

          printf ("!!!!! Evaluate the derived classes: are they visible --- baseClassList.size() = %zu \n",baseClassList.size());

          SgBaseClassPtrList::iterator i = baseClassList.begin();
          while (i != baseClassList.end())
             {
            // Check each base class.
               SgBaseClass* baseClass = *i;
               ROSE_ASSERT(baseClass != NULL);

               SgClassDeclaration* classDeclaration = baseClass->get_base_class();
               SgScopeStatement*   currentScope     = classDefinition->get_scope();
               ROSE_ASSERT(currentScope != NULL);

               printf ("Calling nameQualificationDepth() for classDeclaration name = %s \n",classDeclaration->get_name().str());

               int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,classDefinition);

               printf ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

               setNameQualification(baseClass,classDeclaration,amountOfNameQualificationRequired);

               i++;
             }
#if 0
          printf ("Exiting after evaluation of derived classes: are they visible: \n");
          ROSE_ASSERT(false);
#endif
        }

  // Handle the types used in variable declarations...
  // A problem with this implementation is that it relies on there being one SgInitializedName per SgVariableDeclaration.
  // This is currently the case for C++, but we would like to fix this.  It is not clear if the SgInitializedName should carry its
  // own qualification or not (this violates the idea that the IR node that has the reference stored the name qualification data).
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(n);
     if (variableDeclaration != NULL)
        {
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);
          ROSE_ASSERT(initializedName != NULL);
          int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,variableDeclaration);
          printf ("SgVariableDeclaration's type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForType);

          SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());
          if (declaration != NULL)
             {
               printf ("Putting the name qualification for the type into the SgInitializedName = %p = %s \n",initializedName,initializedName->get_name().str());
               setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType);
             }
            else
             {
               printf ("declaration == NULL: could not put name qualification for the type into the SgInitializedName = %p = %s \n",initializedName,initializedName->get_name().str());
             }

          SgScopeStatement* currentScope = SageInterface::getScope(variableDeclaration);
          int amountOfNameQualificationRequiredForName = nameQualificationDepth(initializedName,currentScope,variableDeclaration);
          printf ("SgVariableDeclaration's variable name: amountOfNameQualificationRequiredForName = %d \n",amountOfNameQualificationRequiredForName);

       // If there is a previous declaration then that is the original declaration (see test2011_30.C for an example of this case).
          SgInitializedName* originalInitializedName = initializedName->get_prev_decl_item();
          if (originalInitializedName != NULL)
             {
               ROSE_ASSERT(originalInitializedName->get_parent() != NULL);
               SgVariableDeclaration* originalVariableDeclaration = isSgVariableDeclaration(originalInitializedName->get_parent());
               ROSE_ASSERT(originalVariableDeclaration != NULL);
               setNameQualification(variableDeclaration,originalVariableDeclaration,amountOfNameQualificationRequiredForName);
             }
        }

  // Handle SgType name qualification where SgInitializedName's appear outside of SgVariableDeclaration's (e.g. in function parameter declarations).
     SgInitializedName* initializedName = isSgInitializedName(n);
     if (initializedName != NULL)
        {
          ROSE_ASSERT(initializedName->get_parent() != NULL);
          SgFunctionParameterList* functionParamterList = isSgFunctionParameterList(initializedName->get_parent());
          if (functionParamterList != NULL)
             {
               SgStatement* statement = isSgStatement(initializedName->get_parent());
               int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,statement);
               printf ("SgInitializedName's (%s) type: amountOfNameQualificationRequiredForType = %d \n",initializedName->get_name().str(),amountOfNameQualificationRequiredForType);

            // SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionParamterList->get_parent());
               SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());
               if (declaration != NULL)
                  {
                    setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType);
                  }
             }
            else
             {
            // Check for different places where SgInitializedName are used to make sure that we are not missing more opportunities for name qualification.
               printf ("Note: Found SgInitializedName (%s) in initializedName->get_parent() = %s \n",initializedName->get_name().str(),initializedName->get_parent()->class_name().c_str());
             }
        }

  // Handle references to SgFunctionDeclaration...
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
     if (functionDeclaration != NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

       // We need the structural location in scope (not the semantic one).
          SgScopeStatement* currentScope = isSgScopeStatement(functionDeclaration->get_parent());
          ROSE_ASSERT(currentScope != NULL);

          int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
          printf ("SgMemberFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

          setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
        }

  // Handle references to SgMemberFunctionDeclaration...
     SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(n);
     if (memberFunctionDeclaration != NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

       // We need the structural location in scope (not the semantic one).
          SgScopeStatement* currentScope = isSgScopeStatement(memberFunctionDeclaration->get_parent());
          ROSE_ASSERT(currentScope != NULL);

          int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,memberFunctionDeclaration);
          printf ("SgMemberFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

          setNameQualification(memberFunctionDeclaration,amountOfNameQualificationRequired);
        }

  // DQ (5/14/2011): Added support for the name qualification of the base type used in typedefs.
  // Handle references to SgTypedefDeclaration...
     SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(n);
     if (typedefDeclaration != NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

       // We need the structural location in scope (not the semantic one).
       // SgScopeStatement* currentScope = isSgScopeStatement(typedefDeclaration->get_parent());
          SgScopeStatement* currentScope = typedefDeclaration->get_scope();
          ROSE_ASSERT(currentScope != NULL);

          SgType* baseType = typedefDeclaration->get_base_type();
          ROSE_ASSERT(baseType != NULL);
          SgDeclarationStatement* baseTypeDeclaration = associatedDeclaration(baseType);

       // If the base type is defined in the typedef directly then it should need no name qualification by definition.
          printf ("typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() = %s \n",typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() ? "true" : "false");

       // This is NULL if the base type is not associated with a declaration (e.g. not a SgNamedType).
       // ROSE_ASSERT(baseTypeDeclaration != NULL);
       // if (baseTypeDeclaration != NULL)
          if ( (baseTypeDeclaration != NULL) && (typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() == false) )
             {
               int amountOfNameQualificationRequiredForBaseType = nameQualificationDepth(baseTypeDeclaration,currentScope,typedefDeclaration);
               printf ("SgTypedefDeclaration: amountOfNameQualificationRequiredForBaseType = %d \n",amountOfNameQualificationRequiredForBaseType);

               ROSE_ASSERT(baseTypeDeclaration != NULL);
               setNameQualification(typedefDeclaration,baseTypeDeclaration,amountOfNameQualificationRequiredForBaseType);
             }
        }

  // Handle references in SgUsingDirectiveStatement...
     SgUsingDirectiveStatement* usingDirective = isSgUsingDirectiveStatement(n);
     if (usingDirective != NULL)
        {
          SgNamespaceDeclarationStatement* namespaceDeclaration = usingDirective->get_namespaceDeclaration();
          ROSE_ASSERT(namespaceDeclaration != NULL);
          SgScopeStatement* currentScope = usingDirective->get_scope();
          ROSE_ASSERT(currentScope != NULL);

          int amountOfNameQualificationRequired = nameQualificationDepth(namespaceDeclaration,currentScope,usingDirective);
          printf ("SgUsingDirectiveStatement's SgNamespaceDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

          setNameQualification(usingDirective,namespaceDeclaration,amountOfNameQualificationRequired);
        }

     SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(n);
     if (usingDeclaration != NULL)
        {
          SgDeclarationStatement* associatedDeclaration     = usingDeclaration->get_declaration();
          SgInitializedName*      associatedInitializedName = usingDeclaration->get_initializedName();

          printf ("In case for SgUsingDeclarationStatement: associatedDeclaration = %p associatedInitializedName = %p \n",associatedDeclaration,associatedInitializedName);
          if (associatedDeclaration != NULL)
             printf ("associatedDeclaration = %p = %s = %s = %s \n",associatedDeclaration,associatedDeclaration->class_name().c_str(),SageInterface::get_name(associatedDeclaration).c_str(),SageInterface::generateUniqueName(associatedDeclaration,true).c_str());
          if (associatedInitializedName != NULL)
             printf ("associatedInitializedName = %p = %s = %s = %s \n",associatedInitializedName,associatedInitializedName->class_name().c_str(),SageInterface::get_name(associatedInitializedName).c_str(),SageInterface::generateUniqueName(associatedInitializedName,true).c_str());

          SgScopeStatement* currentScope = usingDeclaration->get_scope();
          ROSE_ASSERT(currentScope != NULL);

          int amountOfNameQualificationRequired = 0;
          if (associatedDeclaration != NULL)
             {
            // DQ (5/14/2011): For the case of test2001_46.C I think we need to force name qualification.
            // In general any reference using a SgUsingDeclarationStatement should likely be qualified (at least one level, via the parent of the associatedDeclaration).
            // amountOfNameQualificationRequired = nameQualificationDepth(associatedDeclaration,currentScope,usingDeclaration);
               amountOfNameQualificationRequired = nameQualificationDepth(associatedDeclaration,currentScope,usingDeclaration, /* forceMoreNameQualification = */ true);

               setNameQualification(usingDeclaration,associatedDeclaration,amountOfNameQualificationRequired);
             }
            else
             {
               ROSE_ASSERT(associatedInitializedName != NULL);
               amountOfNameQualificationRequired = nameQualificationDepth(associatedInitializedName,currentScope,usingDeclaration);

               setNameQualification(usingDeclaration,associatedInitializedName,amountOfNameQualificationRequired);
#if 0
            // We want to debug this case later!
               printf ("Exiting in unimplemented case of SgInitializedName in SgUsingDeclarationStatement \n");
               ROSE_ASSERT(false);
#endif
             }

          printf ("SgUsingDeclarationStatement's SgVarRefExp: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
        }

  // DQ (5/12/2011): We want to located name qualification information about referenced functions 
  // at the SgFunctionRefExp and SgMemberFunctionRefExp IR node instead of the SgFunctionCallExp IR node.
     SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(n);
     if (functionRefExp != NULL)
        {
          SgFunctionDeclaration* functionDeclaration = functionRefExp->getAssociatedFunctionDeclaration();
       // ROSE_ASSERT(functionDeclaration != NULL);
          if (functionDeclaration != NULL)
             {
               SgStatement* currentStatement = TransformationSupport::getStatement(functionRefExp);
               ROSE_ASSERT(currentStatement != NULL);

               SgScopeStatement* currentScope = currentStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);

               int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,currentStatement);
               printf ("SgFunctionCallExp's function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

               setNameQualification(functionRefExp,functionDeclaration,amountOfNameQualificationRequired);
             }
            else
             {
               printf ("WARNING: functionDeclaration == NULL in SgFunctionCallExp for name qualification support! \n");
             }
        }

     SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(n);
     if (memberFunctionRefExp != NULL)
        {
          SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionRefExp->getAssociatedMemberFunctionDeclaration();
       // ROSE_ASSERT(functionDeclaration != NULL);
          if (memberFunctionDeclaration != NULL)
             {
               SgStatement* currentStatement = TransformationSupport::getStatement(memberFunctionRefExp);
               ROSE_ASSERT(currentStatement != NULL);

               SgScopeStatement* currentScope = currentStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);

               int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,currentStatement);
               printf ("SgMemberFunctionCallExp's member function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

               setNameQualification(memberFunctionRefExp,memberFunctionDeclaration,amountOfNameQualificationRequired);
             }
            else
             {
               printf ("WARNING: memberFunctionDeclaration == NULL in SgFunctionCallExp for name qualification support! \n");
             }
        }

     SgVarRefExp* varRefExp = isSgVarRefExp(n);
     if (varRefExp != NULL)
        {
       // We need to store the information about the required name qualification in the SgVarRefExp IR node.

          SgStatement* currentStatement = TransformationSupport::getStatement(varRefExp);
          ROSE_ASSERT(currentStatement != NULL);

          SgScopeStatement* currentScope = currentStatement->get_scope();
          ROSE_ASSERT(currentScope != NULL);

          SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
          ROSE_ASSERT(variableSymbol != NULL);
          SgInitializedName* initializedName = variableSymbol->get_declaration();
          ROSE_ASSERT(initializedName != NULL);

          SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initializedName->get_parent());
       // ROSE_ASSERT(variableDeclaration != NULL);
          if (variableDeclaration == NULL)
             {
            // This is the special case for the compiler generated variable "__PRETTY_FUNCTION__".
               if (initializedName->get_name() == "__PRETTY_FUNCTION__" ||  initializedName->get_name() == "__func__")
                  {
                 // Skip these cases...
                  }
                 else
                  {
                 // If this is a SgInitializedName from a function parameter list then it does not need qualification.
                    SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(initializedName->get_parent());
                    if (functionParameterList != NULL)
                       {
                         printf ("Names from function parameter list can not be name qualified: name = %s \n",initializedName->get_name().str());
                       }
                      else
                       {
                         printf ("varRefExp's initialized name = %s is not associated with a SgVariableDeclaration \n",initializedName->get_name().str());
                         initializedName->get_file_info()->display("This SgInitializedName is not associated with a SgVariableDeclaration");
                         ROSE_ASSERT(variableDeclaration != NULL);
                       }
                  }
             }
            else
             {
               int amountOfNameQualificationRequired = nameQualificationDepth(variableDeclaration,currentScope,currentStatement);
               printf ("SgVarRefExp's SgDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

               setNameQualification(varRefExp,variableDeclaration,amountOfNameQualificationRequired);
             }
        }

#if 0
  // Template arguments don't hide here!!!

     SgTemplateArgument* templateArgument = isSgTemplateArgument(n);
     if (templateArgument != NULL)
        {
#if 0
          SgDeclaration* declaration = templateArgument->getAssociatedFunctionDeclaration();
          if (declaration != NULL)
             {
               SgStatement* currentStatement = TransformationSupport::getStatement(templateArgument);
               ROSE_ASSERT(currentStatement != NULL);

               SgScopeStatement* currentScope = currentStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);

               int amountOfNameQualificationRequired = nameQualificationDepth(templateArgument,currentScope,currentStatement);
               printf ("SgTemplateArgument's declaration name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

               setNameQualification(templateArgument,declaration,amountOfNameQualificationRequired);
             }
            else
             {
               printf ("WARNING: declaration == NULL in SgTemplateArgument for name qualification support! \n");
             }
#else
          printf ("Sorry not implemented, name qualification for SgTemplateArgument \n");
          ROSE_ASSERT(false);
#endif
        }
#endif


  // ******************************************************************************
  // Now that this declaration is pocessed, mark it as being seen (place into set).
  // ******************************************************************************

     SgDeclarationStatement* declaration = isSgDeclarationStatement(n);
     if (declaration != NULL)
        {
       // If this is a declaration of something that has a name then we need to mark it as having been seen.

       // In some cases of C++ name qualification depending on if the defining declaration (or a forward 
       // declaration is in a scope that would define the declaration to a scope where the declaration could be 
       // present).  This detail is handled by reporting if such a declaration has been seen yet.  Since the 
       // preorder traversal is the same as the traversal used in the unparsing it is sufficient to record
       // the order of the processing here and not complicate the unparser directly.  Note that the use of
       // function declarations follow these rules and so are a problem when the prototype is defined in a
       // function (where it does not communicate the defining declarations location) instead of in a global 
       // scope or namespace scope (where it does appear to communicate its position.

          SgDeclarationStatement* firstNondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
          if (referencedNameSet.find(firstNondefiningDeclaration) == referencedNameSet.end())
             {
            // printf ("Adding declaration to set of visited declarations \n");
               referencedNameSet.insert(firstNondefiningDeclaration);
             }
        }

     return HiddenListInheritedAttribute(inheritedAttribute);
   }


HiddenListSynthesizedAttribute
HiddenListTraversal::evaluateSynthesizedAttribute(SgNode* n, HiddenListInheritedAttribute inheritedAttribute, SynthesizedAttributesList synthesizedAttributeList)
   {
  // This is not used now but will likely be used later.
     HiddenListSynthesizedAttribute returnAttribute;

     return returnAttribute;
   }


void
HiddenListTraversal::setNameQualification(SgVarRefExp* varRefExp, SgVariableDeclaration* variableDeclaration, int amountOfNameQualificationRequired)
   {
  // This is where we hide the details of translating the intepretation of the amountOfNameQualificationRequired
  // which can be greater than the number of nested scopes to a representation that is bounded by the number of 
  // nested scopes and sets the global qualification to be true. If I decide I don't like this here, then we
  // might find a way to handling this point more directly later. This at least gets it set properly in the AST.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     setNameQualificationSupport(variableDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     varRefExp->set_global_qualification_required(outputGlobalQualification);
     varRefExp->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     varRefExp->set_type_elaboration_required(outputTypeEvaluation);

     printf ("In HiddenListTraversal::setNameQualification(): varRefExp->get_name_qualification_length()     = %d \n",varRefExp->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): varRefExp->get_type_elaboration_required()     = %s \n",varRefExp->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): varRefExp->get_global_qualification_required() = %s \n",varRefExp->get_global_qualification_required() ? "true" : "false");
   }


void
HiddenListTraversal::setNameQualification(SgFunctionRefExp* functionRefExp, SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
   {
  // This is where we hide the details of translating the intepretation of the amountOfNameQualificationRequired
  // which can be greater than the number of nested scopes to a representation that is bounded by the number of 
  // nested scopes and sets the global qualification to be true. If I decide I don't like this here, then we
  // might find a way to handling this point more directly later. This at least gets it set properly in the AST.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     functionRefExp->set_global_qualification_required(outputGlobalQualification);
     functionRefExp->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     functionRefExp->set_type_elaboration_required(outputTypeEvaluation);

     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_name_qualification_length()     = %d \n",functionRefExp->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_type_elaboration_required()     = %s \n",functionRefExp->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_global_qualification_required() = %s \n",functionRefExp->get_global_qualification_required() ? "true" : "false");
   }

void
HiddenListTraversal::setNameQualification(SgMemberFunctionRefExp* functionRefExp, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
   {
  // This is where we hide the details of translating the intepretation of the amountOfNameQualificationRequired
  // which can be greater than the number of nested scopes to a representation that is bounded by the number of 
  // nested scopes and sets the global qualification to be true. If I decide I don't like this here, then we
  // might find a way to handling this point more directly later. This at least gets it set properly in the AST.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     functionRefExp->set_global_qualification_required(outputGlobalQualification);
     functionRefExp->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     functionRefExp->set_type_elaboration_required(outputTypeEvaluation);

     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_name_qualification_length()     = %d \n",functionRefExp->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_type_elaboration_required()     = %s \n",functionRefExp->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_global_qualification_required() = %s \n",functionRefExp->get_global_qualification_required() ? "true" : "false");
   }


void
HiddenListTraversal::setNameQualification ( SgBaseClass* baseClass, SgClassDeclaration* classDeclaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     setNameQualificationSupport(classDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     baseClass->set_global_qualification_required(outputGlobalQualification);
     baseClass->set_name_qualification_length(outputNameQualificationLength);
     baseClass->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): baseClass->get_name_qualification_length()     = %d \n",baseClass->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): baseClass->get_type_elaboration_required()     = %s \n",baseClass->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): baseClass->get_global_qualification_required() = %s \n",baseClass->get_global_qualification_required() ? "true" : "false");
#endif
   }

#if 0
void
HiddenListTraversal::setNameQualification ( SgMemberFunctionDeclaration* memberFunctionDeclaration, int amountOfNameQualificationRequired )
   {
  // This takes only a SgMemberFunctionDeclaration since it is where we locate the name qualification information AND
  // is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     setNameQualificationSupport(memberFunctionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     memberFunctionDeclaration->set_global_qualification_required(outputGlobalQualification);
     memberFunctionDeclaration->set_name_qualification_length(outputNameQualificationLength);
     memberFunctionDeclaration->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): memberFunctionDeclaration->get_name_qualification_length()     = %d \n",memberFunctionDeclaration->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): memberFunctionDeclaration->get_type_elaboration_required()     = %s \n",memberFunctionDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): memberFunctionDeclaration->get_global_qualification_required() = %s \n",memberFunctionDeclaration->get_global_qualification_required() ? "true" : "false");
#endif
   }
#else
void
HiddenListTraversal::setNameQualification ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired )
   {
  // This takes only a SgFunctionDeclaration since it is where we locate the name qualification information AND
  // is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     functionDeclaration->set_global_qualification_required(outputGlobalQualification);
     functionDeclaration->set_name_qualification_length(outputNameQualificationLength);
     functionDeclaration->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): functionDeclaration->get_name_qualification_length()     = %d \n",functionDeclaration->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): functionDeclaration->get_type_elaboration_required()     = %s \n",functionDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): functionDeclaration->get_global_qualification_required() = %s \n",functionDeclaration->get_global_qualification_required() ? "true" : "false");
#endif
   }
#endif

void
HiddenListTraversal::setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     usingDeclaration->set_global_qualification_required(outputGlobalQualification);
     usingDeclaration->set_name_qualification_length(outputNameQualificationLength);
     usingDeclaration->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): usingDeclaration->get_name_qualification_length()     = %d \n",usingDeclaration->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): usingDeclaration->get_type_elaboration_required()     = %s \n",usingDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): usingDeclaration->get_global_qualification_required() = %s \n",usingDeclaration->get_global_qualification_required() ? "true" : "false");
#endif
   }


void
HiddenListTraversal::setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgInitializedName* associatedInitializedName, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     setNameQualificationSupport(associatedInitializedName->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     usingDeclaration->set_global_qualification_required(outputGlobalQualification);
     usingDeclaration->set_name_qualification_length(outputNameQualificationLength);
     usingDeclaration->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): usingDeclaration->get_name_qualification_length()     = %d \n",usingDeclaration->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): usingDeclaration->get_type_elaboration_required()     = %s \n",usingDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): usingDeclaration->get_global_qualification_required() = %s \n",usingDeclaration->get_global_qualification_required() ? "true" : "false");
#endif
   }

void
HiddenListTraversal::setNameQualification ( SgUsingDirectiveStatement* usingDirective, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     usingDirective->set_global_qualification_required(outputGlobalQualification);
     usingDirective->set_name_qualification_length(outputNameQualificationLength);
     usingDirective->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): usingDirective->get_name_qualification_length()     = %d \n",usingDirective->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): usingDirective->get_type_elaboration_required()     = %s \n",usingDirective->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): usingDirective->get_global_qualification_required() = %s \n",usingDirective->get_global_qualification_required() ? "true" : "false");
#endif
   }


// void HiddenListTraversal::setNameQualification(SgInitializedName* initializedName,SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
void
HiddenListTraversal::setNameQualification(SgInitializedName* initializedName,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     initializedName->set_global_qualification_required_for_type(outputGlobalQualification);
     initializedName->set_name_qualification_length_for_type(outputNameQualificationLength);
     initializedName->set_type_elaboration_required_for_type(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): initializedName->get_name_qualification_length_for_type()     = %d \n",initializedName->get_name_qualification_length_for_type());
     printf ("In HiddenListTraversal::setNameQualification(): initializedName->get_type_elaboration_required_for_type()     = %s \n",initializedName->get_type_elaboration_required_for_type() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): initializedName->get_global_qualification_required_for_type() = %s \n",initializedName->get_global_qualification_required_for_type() ? "true" : "false");
#endif
   }

void
HiddenListTraversal::setNameQualification(SgVariableDeclaration* variableDeclaration,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     variableDeclaration->set_global_qualification_required(outputGlobalQualification);
     variableDeclaration->set_name_qualification_length(outputNameQualificationLength);
     variableDeclaration->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): variableDeclaration->get_name_qualification_length()     = %d \n",variableDeclaration->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): variableDeclaration->get_type_elaboration_required()     = %s \n",variableDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): variableDeclaration->get_global_qualification_required() = %s \n",variableDeclaration->get_global_qualification_required() ? "true" : "false");
#endif
   }

void
HiddenListTraversal::setNameQualification(SgTypedefDeclaration* typedefDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     typedefDeclaration->set_global_qualification_required_for_base_type(outputGlobalQualification);
     typedefDeclaration->set_name_qualification_length_for_base_type(outputNameQualificationLength);
     typedefDeclaration->set_type_elaboration_required_for_base_type(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): typedefDeclaration->get_name_qualification_length_for_base_type()     = %d \n",typedefDeclaration->get_name_qualification_length_for_base_type());
     printf ("In HiddenListTraversal::setNameQualification(): typedefDeclaration->get_type_elaboration_required_for_base_type()     = %s \n",typedefDeclaration->get_type_elaboration_required_for_base_type() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): typedefDeclaration->get_global_qualification_required_for_base_type() = %s \n",typedefDeclaration->get_global_qualification_required_for_base_type() ? "true" : "false");
#endif
   }


void
HiddenListTraversal::setNameQualification(SgTemplateArgument* templateArgument, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     templateArgument->set_global_qualification_required(outputGlobalQualification);
     templateArgument->set_name_qualification_length(outputNameQualificationLength);
     templateArgument->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): templateArgument->get_name_qualification_length()     = %d \n",templateArgument->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): templateArgument->get_type_elaboration_required()     = %s \n",templateArgument->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): templateArgument->get_global_qualification_required() = %s \n",templateArgument->get_global_qualification_required() ? "true" : "false");
#endif
   }


void
HiddenListTraversal::setNameQualificationSupport(SgScopeStatement* scope, const int inputNameQualificationLength, int & output_amountOfNameQualificationRequired , bool & outputGlobalQualification, bool & outputTypeEvaluation )
   {
  // This is lower level support for the different overloaded setNameQualification() functions.

     output_amountOfNameQualificationRequired = inputNameQualificationLength;
     outputGlobalQualification                = false;
     outputTypeEvaluation                     = false;

     printf ("In HiddenListTraversal::setNameQualificationSupport(): scope = %p = %s = %s inputNameQualificationLength = %d \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str(),inputNameQualificationLength);

     for (int i = 0; i < inputNameQualificationLength; i++)
        {
          printf ("   --- In loop: scope = %p = %s = %s \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
          SgGlobal* globalScope = isSgGlobal(scope);
          if (globalScope != NULL)
             {
            // If we have iterated beyond the number of nested scopes, then set the global 
            // qualification and reduce the name_qualification_length correspondingly by one.

               printf ("!!!!! We have iterated beyond the number of nested scopes: setting outputGlobalQualification == true \n");

               outputGlobalQualification = true;
               output_amountOfNameQualificationRequired = inputNameQualificationLength-1;
             }

       // We have to loop over scopes that are not named scopes!
          scope = scope->get_scope();
        }

     printf ("In HiddenListTraversal::setNameQualificationSupport(): outputGlobalQualification = %s output_amountOfNameQualificationRequired = %d \n",outputGlobalQualification ? "true" : "false",output_amountOfNameQualificationRequired);
   }


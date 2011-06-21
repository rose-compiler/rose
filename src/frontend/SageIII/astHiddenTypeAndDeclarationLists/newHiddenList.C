#include "sage3basic.h"
#include "newHiddenList.h"

using namespace std;


// *******************************************************
// Main calling function to support hidden list evaluation
// *******************************************************

// Notes:
//   1) This name HiddenListTraversal should be changed to something that 
//      makes it more clear that it is associated with Name Qualification support.
//   2) The filename for this source code should also be changed.

void
newBuildHiddenTypeAndDeclarationLists( SgNode* node )
   {
  // This function is the top level API for Name Qualification support.
  // This is the only function that need be seen by ROSE.  This function 
  // is called in the function:
  //      Unparser::unparseFile(SgSourceFile* file, SgUnparse_Info& info )
  // in the unparser.C file.  Thus the name qualification is computed
  // as we start to process a file and the computed values saved into the 
  // SgNode static data member maps. Two maps are used: 
  //    one to support qualification of IR nodes that are named, and 
  //    one to support name qualification of types.
  // These are passed by reference and references are stored to them in 
  // the HiddenListTraversal class.

  // DQ (8/1/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("New hidden type and hidden declaration list generation:");

#if 0
  // SgSourceFile* project = isSgSourceFile(node);
     SgProject* project = isSgProject(node);

     if (project == NULL)
        {
          printf ("Can't process buildHiddenTypeAndDeclarationLists for AST fragment = %p = %s \n",node,node->class_name().c_str());
          return;
        }
     ROSE_ASSERT(project != NULL);
#endif

  // DQ (5/28/2011): Initialize the local maps to the static maps in SgNode.  This is requires so the
  // types used in template arguments can call the unparser to support there gneration of name qualified 
  // nested types.
  // HiddenListTraversal t;
     HiddenListTraversal t(SgNode::get_globalQualifiedNameMapForNames(),SgNode::get_globalQualifiedNameMapForTypes(),SgNode::get_globalTypeNameMap());

     HiddenListInheritedAttribute ih;

  // Call the traversal.
     t.traverse(node,ih);

  // This does not reset the template names.
  // reset the names of template class declarations
  // resetTemplateNames(node);

#if 0
  // The qualified name lists should not have been setup previously, or at least I have not debugged such a case yet.
     ROSE_ASSERT(SgNode::get_globalQualifiedNameMapForNames().size() == 0);
     ROSE_ASSERT(SgNode::get_globalQualifiedNameMapForTypes().size() == 0);

  // We locate the qualified name list in a static data member in SgNode.
  // this is better than locating it in a global variable and I think better than 
  // placing it in the Sg_Unparse_Info object (though it could be moved there in 
  // the future).  The rational for placing it in the SgNode is that it might be 
  // useful separate from the unparser.
  // This is a deep copy and might be inifficient...we should evaluate this later.
     SgNode::set_globalQualifiedNameMapForNames(t.get_qualifiedNameMapForNames());
     SgNode::set_globalQualifiedNameMapForTypes(t.get_qualifiedNameMapForTypes());
#endif
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

#if 0
HiddenListTraversal::HiddenListTraversal()
   {
  // Default constructor
   }
#else
HiddenListTraversal::HiddenListTraversal(std::map<SgNode*,std::string> & input_qualifiedNameMapForNames, std::map<SgNode*,std::string> & input_qualifiedNameMapForTypes,std::map<SgNode*,std::string> & input_typeNameMap)
   : qualifiedNameMapForNames(input_qualifiedNameMapForNames),
     qualifiedNameMapForTypes(input_qualifiedNameMapForTypes),
     typeNameMap(input_typeNameMap)
   {
  // Nothing to do here.
   }
#endif

#if 0
bool
HiddenListTraversal::isANamedConstruct(SgNode* n)
   {
  // If this is a declaration of a type (e.g. SgclassDeclaration) then return true.
     return false;
   }
#endif

#if 0
bool
HiddenListTraversal::containsPossibleNameQualification(SgType* type)
   {
  // DQ (6/3/2011): This is part of name qualification support for types.
  // This function returns true if any part of the type COULD be name qualified.
  // This function traverses the SgType IR node.

  // How can I traverse a type...?





     return false;
   }
#endif


// DQ (5/28/2011): Added support to set the static global qualified name map in SgNode.
const std::map<SgNode*,std::string> &
HiddenListTraversal::get_qualifiedNameMapForNames() const
   {
     return qualifiedNameMapForNames;
   }

// DQ (5/28/2011): Added support to set the static global qualified name map in SgNode.
const std::map<SgNode*,std::string> &
HiddenListTraversal::get_qualifiedNameMapForTypes() const
   {
     return qualifiedNameMapForTypes;
   }


int
numberOfSymbolsWithName( const SgName & name, SgScopeStatement* scope )
   {
  // DQ (6/20/2011): This function counts the number of symbols associated with the same name.
  // This function should be put into the SgScopeStatement for more general use.

  // We might have to have separate functions specific to functions, variables, etc.
  // This function addresses a requirement associated with a bug demonstrated by test2011_84.C.

     ROSE_ASSERT(scope != NULL);
     SgSymbol* symbol = scope->lookup_function_symbol(name);

     int count = 0;

     printf ("In numberOfSymbolsWithName(): symbol = %p scope = %p = %s \n",symbol,scope,scope->class_name().c_str());

     while (symbol != NULL)
        {
          printf ("     In loop: symbol = %p = %s \n",symbol,symbol->class_name().c_str());
          count++;
          symbol = scope->next_any_symbol();
        }

     printf ("In numberOfSymbolsWithName(): count = %d \n",count);
     return count;
   }

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
          case V_SgTypeLongDouble:
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


void
HiddenListTraversal::evaluateTemplateInstantiationDeclaration ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // DQ (6/1/2011): Added support for template arguments.
     switch (declaration->variantT())
        {
          case V_SgTemplateInstantiationDecl:
             {
               SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
               ROSE_ASSERT(templateInstantiationDeclaration != NULL);
               printf ("$$$$$$$$$ --- templateInstantiationDeclaration = %p \n",templateInstantiationDeclaration);

            // Evaluate all template arguments.
               evaluateNameQualificationForTemplateArgumentList (templateInstantiationDeclaration->get_templateArguments(),currentScope,positionStatement);
               break;
             }

          case V_SgTemplateInstantiationFunctionDecl:
             {
               SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(declaration);
               ROSE_ASSERT(templateInstantiationFunctionDeclaration != NULL);
               printf ("$$$$$$$$$ --- templateInstantiationFunctionDeclaration = %p \n",templateInstantiationFunctionDeclaration);

            // Evaluate all template arguments.
               evaluateNameQualificationForTemplateArgumentList (templateInstantiationFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
               break;
             }

          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
               SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(declaration);
               printf ("$$$$$$$$$ --- templateInstantiationMemberFunctionDeclaration = %p \n",templateInstantiationMemberFunctionDeclaration);
               ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration != NULL);

            // Evaluate all template arguments.
            // evaluateNameQualificationForTemplateArgumentList (templateInstantiationMemberFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
               SgTemplateArgumentPtrList & templateArgumentList = templateInstantiationMemberFunctionDeclaration->get_templateArguments();
               evaluateNameQualificationForTemplateArgumentList(templateArgumentList,currentScope,positionStatement);
               break;
             }

          default:
             {
               printf ("This IR node does not contain template arguments to process. \n");
             }
        }
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

          case V_SgNamespaceSymbol: // Note sure about this!!!
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

  // The use of SageInterface::generateUniqueName() can cause the unparser to be called and triggers the name 
  // qualification recursively but only for template declaration (SgTemplateInstantiationDecl, I think).
  // printf ("declaration  = %p = %s = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str(),SageInterface::generateUniqueName(declaration,true).c_str());
     printf ("declaration  = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
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

  // ROSE_ASSERT(name.is_null() == false);
     if (name.is_null() == true)
        {
       // This name is empty so we need to keep going to evaluate the qualified name (see test2006_121.C).
          printf ("CASE OF EMPTY NAME: declaration = %p = %s but has no valid name (it has and empty name), thus we have to recurse to the next level \n",declaration,declaration->class_name().c_str());

          qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
        }

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
            // printf ("Lookup symbol based on name only: symbol = %p = %s \n",symbol,symbol->class_name().c_str());
               printf ("Lookup symbol based on name only (via parents starting at currentScope = %p = %s: name = %s symbol = %p = %s) \n",currentScope,currentScope->class_name().c_str(),name.str(),symbol,symbol->class_name().c_str());

               SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);
               if (aliasSymbol != NULL)
                  {
                    symbol = aliasSymbol->get_alias();
                    ROSE_ASSERT(symbol != NULL);
                  }

            // We have to check the kind of declaration against the kind of symbol found. A local variable (for example) 
            // could hide the same name used for the declaration.  This if we find symbol inconsistant with the declaration 
            // then we need some form of qualification (sometimes just type elaboration).
               printf ("Targeting a declaration = %p = %s \n",declaration,declaration->class_name().c_str());
               printf ("     declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
               printf ("     declaration->get_definingDeclaration()         = %p \n",declaration->get_definingDeclaration());

               switch (declaration->variantT())
                  {
                    case V_SgClassDeclaration:
                       {
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                         ROSE_ASSERT(classDeclaration != NULL);

                         printf ("classDeclaration name = %s \n",classDeclaration->get_name().str());

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
                           else
                            {
                              printf ("Symbol matches the class declaration (classDeclaration->get_firstNondefiningDeclaration()) classSymbol->get_declaration() = %p \n",classSymbol->get_declaration());

                           // DQ (6/9/2011): I would prefer to have this be true and it might work if it is not, but I would like to have this be a warning for now!
                           // ROSE_ASSERT(classDeclaration->get_firstNondefiningDeclaration() == classSymbol->get_declaration());
                              if (classDeclaration->get_firstNondefiningDeclaration() != classSymbol->get_declaration())
                                 {
                                   printf ("WARNING: classDeclaration->get_firstNondefiningDeclaration() != classSymbol->get_declaration() \n");
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

                      // DQ (6/5/2011): Added support for case where namespaceSymbol == NULL.
                      // ROSE_ASSERT(namespaceSymbol != NULL);
                         if (namespaceSymbol == NULL)
                            {
                           // This is the case of test2011_72.C (where there is a function with a name matching the name of the namespace).
                           // There is no such think a namespace elaboration, but if there was it might be required at this point.

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupNamespaceSymbolInParentScopes(name,currentScope);

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
                           else
                            {
                           // This is the typical case.
                            }
                         
                         break;
                       }

                 // DQ (6/1/2011): Added case for SgTemplateInstantiationFunctionDecl.
                 // case V_SgTemplateInstantiationFunctionDecl:
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
#if 0
                      // DQ (6/1/2011): Added support for template arguments.
                         SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(declaration);
                         printf ("$$$$$$$$$ --- templateInstantiationMemberFunctionDeclaration = %p \n",templateInstantiationMemberFunctionDeclaration);
                         if (templateInstantiationMemberFunctionDeclaration != NULL)
                            {
                           // Evaluate all template arguments.
                              evaluateNameQualificationForTemplateArgumentList (templateInstantiationMemberFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
                            }
#endif
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
#if 0
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In HiddenListTraversal::nameQualificationDepth(): typedefSymbol == NULL \n");
                                 }
#else
                           // DQ (5/15/2011): Added this to support where symbol after moving name qualification 
                           // support to the astPostProcessing phase instead of calling it in the unparser.
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In HiddenListTraversal::nameQualificationDepth(): typedefSymbol == NULL \n");
                                // Look for a template symbol
                                   symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);
                                   ROSE_ASSERT(symbol != NULL);
                                 }
#endif
#if 1
                              ROSE_ASSERT(symbol != NULL);
                              printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
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

                 // DQ (6/1/2011): Not clear if we need a special case for the case of SgTemplateInstantiationMemberFunctionDecl.
                 // I think we need to call: evaluateNameQualificationForTemplateArgumentList()
                 // to evaluate template arguments for both SgTemplateInstantiationFunctionDecl and SgTemplateInstantiationMemberFunctionDecl.

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
#if 0
                      // Evaluate all template arguments.
                         evaluateNameQualificationForTemplateArgumentList (templateFunction->get_templateArguments(),currentScope,positionStatement);
#endif
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

                         printf ("Found a case of declaration == SgTemplateInstantiationDecl \n");

                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                         if (classSymbol == NULL)
                            {
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope);

                           // DQ (5/15/2011): Added this to support where symbol after moving name qualification 
                           // support to the astPostProcessing phase instead of calling it in the unparser.
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In HiddenListTraversal::nameQualificationDepth(): classSymbol == NULL \n");
                                // Look for a template symbol
                                   symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);

                                // DQ (5/15/2011): This fails for test2004_77.C)...
                                // ROSE_ASSERT(symbol != NULL);
                                 }
#if 0
                              ROSE_ASSERT(symbol != NULL);
                              printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
                            }
                           else
                            {
                              printf ("Need to dig deeper into this symbol! \n");
                              SgDeclarationStatement* nestedDeclaration = classSymbol->get_declaration();
                              ROSE_ASSERT(nestedDeclaration != NULL);
                              printf ("nestedDeclaration = %p = %s \n",nestedDeclaration,nestedDeclaration->class_name().c_str());
                              SgTemplateInstantiationDecl* nestedTemplateDeclaration = isSgTemplateInstantiationDecl(nestedDeclaration);
                              if (nestedTemplateDeclaration != NULL)
                                 {
                                   printf ("nestedTemplateDeclaration = %p = %s \n",nestedTemplateDeclaration,nestedTemplateDeclaration->get_name().str());
#if 1
                                // DQ (6/1/2011): This is now done below; for all IR nodes contianing template parameters.
                                // evaluateNameQualificationForTemplateArgumentList (nestedTemplateDeclaration->get_templateArguments(),currentScope,positionStatement);
#else
#if 1
                                   SgType* templateClassType = nestedTemplateDeclaration->get_type();
                                   ROSE_ASSERT(templateClassType != NULL);
                                   int depth = nameQualificationDepth(templateClassType,currentScope,positionStatement);
#else
                                // This nested class could also have template arguments that require name qualification.
                                // But the returned value will be combuted in the current call to this function (so we can ignore the return value).
                                   int depth = nameQualificationDepth(nestedTemplateDeclaration,currentScope,positionStatement,/* forceMoreNameQualification = */ false);
#endif
                                   printf ("Ignoring the value returned from the recursive call to nameQualificationDepth() dept = %d \n",depth);
#endif
                                 }
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
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In HiddenListTraversal::nameQualificationDepth(): variableSymbol == NULL \n");
                                // Look for a template symbol
                                   symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);

                                // DQ (6/5/2011): This assert fails for test2005_107.C (this is OK, the referenced symbol is not visible from the current scope).
                                // ROSE_ASSERT(symbol != NULL);
#if 0
                                   if (symbol != NULL)
                                      {
                                      }
                                     else
                                      {
                                      }
#endif
                                 }
                           // ROSE_ASSERT(symbol != NULL);
                           // printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
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
            else
             {
               printf ("SageInterface::lookupSymbolInParentScopes(name,currentScope) returned NULL \n");
             }

#if 1
       // Refactored this code to another member function so that it could also support evaluation of declarations found in types (more generally).
          evaluateTemplateInstantiationDeclaration(declaration,currentScope,positionStatement);
#else
       // DQ (6/1/2011): Added support for template arguments.
          switch (declaration->variantT())
             {
               case V_SgTemplateInstantiationDecl:
                  {
                    SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
                    ROSE_ASSERT(templateInstantiationDeclaration != NULL);
                    printf ("$$$$$$$$$ --- templateInstantiationDeclaration = %p \n",templateInstantiationDeclaration);

                 // Evaluate all template arguments.
                    evaluateNameQualificationForTemplateArgumentList (templateInstantiationDeclaration->get_templateArguments(),currentScope,positionStatement);
                    break;
                  }

               case V_SgTemplateInstantiationFunctionDecl:
                  {
                    SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(declaration);
                    ROSE_ASSERT(templateInstantiationFunctionDeclaration != NULL);
                    printf ("$$$$$$$$$ --- templateInstantiationFunctionDeclaration = %p \n",templateInstantiationFunctionDeclaration);

                 // Evaluate all template arguments.
                    evaluateNameQualificationForTemplateArgumentList (templateInstantiationFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
                    break;
                  }

               case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
                    SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(declaration);
                    printf ("$$$$$$$$$ --- templateInstantiationMemberFunctionDeclaration = %p \n",templateInstantiationMemberFunctionDeclaration);
                    ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration != NULL);

                 // Evaluate all template arguments.
                 // evaluateNameQualificationForTemplateArgumentList (templateInstantiationMemberFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
                    SgTemplateArgumentPtrList & templateArgumentList = templateInstantiationMemberFunctionDeclaration->get_templateArguments();
                    evaluateNameQualificationForTemplateArgumentList(templateArgumentList,currentScope,positionStatement);
                    break;
                  }

               default:
                  {
                    printf ("This IR node does not contain template arguments to process. \n");
                  }
                
             }
#endif

       // At this point if there was any ambiguity in the first matching symbol that was found, then
       // we have resolved this to the correct type of symbol (SgClassSymbol, SgFunctionSymbol, etc.).
       // Now we want to resolve it to the exact symbol that matches the declaration.
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
            // if (numberOfSymbols > 1 || forceMoreNameQualification == true)
               if (numberOfSymbols > 1000 || forceMoreNameQualification == true)
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

                                // However, if this is a templated class then we have to dig deeper to identify if the template arguments require name qualification.
                                   printf ("symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#if 0
                                   printf ("I think this is associated with a template, need to stop here! \n");
                                   ROSE_ASSERT(false);
#endif
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

                              printf ("associatedFunctionDeclaration->get_firstNondefiningDeclaration() = %p \n",associatedFunctionDeclaration->get_firstNondefiningDeclaration());
                              printf ("functionDeclaration->get_firstNondefiningDeclaration()           = %p \n",functionDeclaration->get_firstNondefiningDeclaration());
                              if (associatedFunctionDeclaration->get_firstNondefiningDeclaration() == functionDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // DQ (6/20/2011): But we don't check for if there was another declaration that might be a problem (overloaded functions don't count!)...
                                // This function is visible from where it is referenced. 
                                   printf ("This function or member function IS visible from where it is referenced (but there could still be ambiguity if this was just the first of several symbols found in the current scope) \n");

                                // But we need to check if there is another such symbol in the same scope that would trigger qualification.
                                // SgScopeStatement* associatedScope = associatedFunctionDeclaration->get_scope();
                                // ROSE_ASSERT(associatedScope != NULL);
                                // printf ("Searching associatedScope = %p = %s \n",associatedScope,associatedScope->class_name().c_str());
                                   SgClassDefinition* classDefinition = isSgClassDefinition(functionDeclaration->get_parent());
                                   if (classDefinition != NULL)
                                      {
                                        printf ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                        printf ("Searching classDefinition = %p \n",classDefinition);

                                     // int numberOfSymbolsWithMatchingName = numberOfSymbolsWithName(name,associatedScope);
                                        int numberOfSymbolsWithMatchingName = numberOfSymbolsWithName(name,classDefinition);
                                        printf ("numberOfSymbolsWithMatchingName = %d \n",numberOfSymbolsWithMatchingName);

                                     // ROSE_ASSERT(numberOfSymbolsWithMatchingName == 1);
                                      }
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
#if 0
                           // DQ (6/1/2011): Added support for template arguments.
                              SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(associatedFunctionDeclaration);
                              printf ("$$$$$$$$$ --- templateInstantiationMemberFunctionDeclaration = %p \n",templateInstantiationMemberFunctionDeclaration);
                              if (templateInstantiationMemberFunctionDeclaration != NULL)
                                 {
                                // Evaluate all template arguments.
                                   evaluateNameQualificationForTemplateArgumentList (templateInstantiationMemberFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
                                 }
#endif
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

     printf ("In HiddenListTraversal::nameQualificationDepth(): Report type elaboration: typeElaborationIsRequired = %s \n",(typeElaborationIsRequired == true) ? "true" : "false");

     return qualificationDepth;
   }



SgDeclarationStatement* 
HiddenListTraversal::getDeclarationAssociatedWithType( SgType* type )
   {
  // Note that this function could be eliminated since it only wraps another function.

     ROSE_ASSERT(type != NULL);

  // printf ("In getDeclarationAssociatedWithType(): type = %s \n",type->class_name().c_str());

     SgDeclarationStatement* declaration = type->getAssociatedDeclaration();

  // Primative types will not have an asociated declaration...
  // ROSE_ASSERT(declaration != NULL);
     if (declaration == NULL)
        {
          printf ("In getDeclarationAssociatedWithType(): declaration == NULL type = %s \n",type->class_name().c_str());
        }
       else
        {
          printf ("In getDeclarationAssociatedWithType(): declaration = %p \n",declaration);
          printf ("In getDeclarationAssociatedWithType(): declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
          printf ("In getDeclarationAssociatedWithType(): declaration->get_definingDeclaration()         = %p \n",declaration->get_definingDeclaration());

          ROSE_ASSERT(declaration == declaration->get_firstNondefiningDeclaration());
        }

     return declaration;
   }

// void evaluateNameQualificationForTemplateArgumentList (SgTemplateArgumentPtrList & templateArgumentList, SgScopeStatement* currentScope, SgStatement* positionStatement);
void
HiddenListTraversal::evaluateNameQualificationForTemplateArgumentList (SgTemplateArgumentPtrList & templateArgumentList, SgScopeStatement* currentScope, SgStatement* positionStatement)
   {
  // DQ (6/4/2011): Note that test2005_73.C demonstrate where the Template arguments are shared between template instantiations.

  // Used for debugging...
     int counter = 0;

     printf ("\n\n*********************************************************************************************************************\n");
     printf ("Inside of HiddenListTraversal::evaluateNameQualificationForTemplateArgumentList(): templateArgumentList.size() = %zu \n",templateArgumentList.size());
     printf ("*********************************************************************************************************************\n");

     SgTemplateArgumentPtrList::iterator i = templateArgumentList.begin();
     while (i != templateArgumentList.end())
        {
          SgTemplateArgument* templateArgument = *i;
          ROSE_ASSERT(templateArgument != NULL);

          printf ("*** Processing template argument #%d \n",counter++);

          SgType* type = templateArgument->get_type();
          if (type != NULL)
             {
            // Reduce the type to the base type stripping off wrappers that would hide the fundamental type inside.
               SgType* strippedType = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
               ROSE_ASSERT(strippedType != NULL);

            // SgNamedType* namedType = isSgNamedType(type);
               SgNamedType* namedType = isSgNamedType(strippedType);
               if (namedType != NULL)
                  {
                 // This could be a type that requires name qualification (reference to a declaration).

                    SgDeclarationStatement* templateArgumentTypeDeclaration = getDeclarationAssociatedWithType(type);
                    if (templateArgumentTypeDeclaration != NULL)
                       {
                      // Check the visability and unambiguity of this declaration.
                      // Note that since the recursion happens before we set the names, all qualified name are set first 
                      // at the nested types and then used in the setting of qualified names at the higher level types 
                      // (less nested types).
#if 1
                      // DQ (5/15/2011): Added recursive handling of template arguments which can require name qualification.
                         printf ("xxxxxx --- Making a RECURSIVE call to nameQualificationDepth() on the template argument \n");
                         int amountOfNameQualificationRequiredForTemplateArgument = nameQualificationDepth(namedType,currentScope,positionStatement);
                         printf ("xxxxxx --- amountOfNameQualificationRequiredForTemplateArgument = %d \n",amountOfNameQualificationRequiredForTemplateArgument);

                         printf ("xxxxxx --- Must call a function to set the name qualification data in the SgTemplateArgument = %p \n",templateArgument);
                         setNameQualification(templateArgument,templateArgumentTypeDeclaration,amountOfNameQualificationRequiredForTemplateArgument);
#else
                         int amountOfNameQualificationRequiredForTemplateArgument = nameQualificationDepth(templateArgumentTypeDeclaration,currentScope,positionStatement);
                         printf ("amountOfNameQualificationRequiredForTemplateArgument = %d \n",amountOfNameQualificationRequiredForTemplateArgument);

                         printf ("Must call a function to set the name qualification data in the SgTemplateArgument = %p \n",templateArgument);
                         setNameQualification(templateArgument,templateArgumentTypeDeclaration,amountOfNameQualificationRequiredForTemplateArgument);
#endif
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

     printf ("*********************************************************************************************************************\n");
     printf ("Leaving HiddenListTraversal::evaluateNameQualificationForTemplateArgumentList(): templateArgumentList.size() = %zu \n",templateArgumentList.size());
     printf ("*********************************************************************************************************************\n\n");
   }

int
HiddenListTraversal::nameQualificationDepth ( SgType* type, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
     int amountOfNameQualificationRequired = 0;

     ROSE_ASSERT(type != NULL);
     printf ("In nameQualificationDepth(SgType*): type = %s \n",type->class_name().c_str());

     SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(type);
     if (declaration != NULL)
        {
       // SgScopeStatement* currentScope = initializedName->get_scope();

       // Check the visability and unambiguity of this declaration.
          amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement);
          printf ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

       // This is where the template argument hide, they must be evaluated for name qualification requirements.
          SgTemplateInstantiationDecl* templateInstatiationDecl = isSgTemplateInstantiationDecl(declaration);
          if (templateInstatiationDecl != NULL)
             {
#if 1
#if 0
                printf ("@@@@@@@@@@@@@@@@@ This is redundant template argument handling @@@@@@@@@@@@@@@@@@@@ \n");
                evaluateNameQualificationForTemplateArgumentList (templateInstatiationDecl->get_templateArguments(),currentScope,positionStatement);
#endif
#else
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
#if 1
                                // DQ (5/15/2011): Added recursive handling of template arguments which can require name qualification.
                                   int amountOfNameQualificationRequiredForTemplateArgument = nameQualificationDepth(namedType,currentScope,positionStatement);
                                   printf ("amountOfNameQualificationRequiredForTemplateArgument = %d \n",amountOfNameQualificationRequiredForTemplateArgument);

                                   printf ("Must call a function to set the name qualification data in the SgTemplateArgument = %p \n",templateArgument);
                                   setNameQualification(templateArgument,templateArgumentTypeDeclaration,amountOfNameQualificationRequiredForTemplateArgument);
#else
                                   int amountOfNameQualificationRequiredForTemplateArgument = nameQualificationDepth(templateArgumentTypeDeclaration,currentScope,positionStatement);
                                   printf ("amountOfNameQualificationRequiredForTemplateArgument = %d \n",amountOfNameQualificationRequiredForTemplateArgument);

                                   printf ("Must call a function to set the name qualification data in the SgTemplateArgument = %p \n",templateArgument);
                                   setNameQualification(templateArgument,templateArgumentTypeDeclaration,amountOfNameQualificationRequiredForTemplateArgument);
#endif
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
#endif
#if 0
               printf ("Found a SgTemplateInstantiationDecl: need inspect the template arguments \n");
               ROSE_ASSERT(false);
#endif
             }
        }

     return amountOfNameQualificationRequired;
   }

int
HiddenListTraversal::nameQualificationDepthForType ( SgInitializedName* initializedName, SgStatement* positionStatement )
   {
     ROSE_ASSERT(initializedName   != NULL);
     ROSE_ASSERT(positionStatement != NULL);

     printf ("In nameQualificationDepthForType(): initializedName = %s type = %s \n",initializedName->get_name().str(),initializedName->get_type()->class_name().c_str());

     return nameQualificationDepth(initializedName->get_type(),initializedName->get_scope(),positionStatement);
   }


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

  // DQ (6/5/2011): Test if this has a valid name (if not then it need not be qualified).
  // Examples of tests codes: test2005_114.C and test2011_73.C.
     if (name.is_null() == true)
        {
       // An empty name implies that no name qualification would make sense.
          return 0;
        }

     SgDeclarationStatement* declaration = associatedDeclaration(initializedName->get_scope());
  // ROSE_ASSERT(declaration != NULL);

     SgVariableSymbol* variableSymbol = NULL;
     SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope);

     printf ("In HiddenListTraversal::nameQualificationDepth(SgInitializedName* = %p): symbol = %p \n",initializedName,symbol);
     if (symbol != NULL)
        {
          printf ("Lookup symbol based on name only (via parents starting at currentScope = %p = %s: name = %s symbol = %p = %s) \n",currentScope,currentScope->class_name().c_str(),name.str(),symbol,symbol->class_name().c_str());

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

            // DQ (6/5/2011): This assertion fails for test2005_114.C.
               ROSE_ASSERT(declaration != NULL);
               amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
             }
            else
             {
               printf ("initializedName->get_prev_decl_item() = %p \n",initializedName->get_prev_decl_item());
            // DQ (6/4/2011): Get the associated symbol so that we can avoid matching on name only; and not the actual SgVariableSymbol symbols.
#if 1
               SgVariableSymbol* targetInitializedNameSymbol = isSgVariableSymbol(initializedName->search_for_symbol_from_symbol_table());
#else
               ROSE_ASSERT(initializedName->get_symbol_from_symbol_table() != NULL);
               SgVariableSymbol* targetInitializedNameSymbol = isSgVariableSymbol(initializedName->get_symbol_from_symbol_table());
#endif
               ROSE_ASSERT(targetInitializedNameSymbol != NULL);

            // DQ (6/4/2011): Make sure we have the correct symbol, else we have detected a collision which will require name qualification to resolve.
               if (variableSymbol == targetInitializedNameSymbol)
                  {
                 // Found the correct symbol.
                    printf ("Found the correct SgVariableSymbol \n");
                  }
                 else
                  {
                 // This is not the correct symbol, even though the unqualified names match.
                    printf ("These symbols only match based on name and is not the targetInitializedNameSymbol. \n");

                    ROSE_ASSERT(declaration != NULL);
                    amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
                  }
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
            else
             {
            // This can be the case of ??? "catch (Overflow)" (see test2004_43.C) instead of "catch (Overflow xxx)" (see test2011_71.C).
               printf ("In HiddenListTraversal::nameQualificationDepth(SgInitializedName*): declaration == NULL, why is this? initializedName->get_scope() = %p = %s \n",initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());
            // ROSE_ASSERT(false);
             }
        }

  // amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement);
     printf ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

     return amountOfNameQualificationRequired;
   }

# if 0
class TestTraversal : public AstSimpleProcessing
   {
  // This traversal is designed to traverse types.

     private:
          SgScopeStatement* currentScope;
          SgStatement* positionStatement;

          std::map<SgNode*,std::string> & qualifiedNameMapForNames;
          std::map<SgNode*,std::string> & qualifiedNameMapForTypes;

     public:
       // TestTraversal (SgScopeStatement* currentScope, SgStatement* positionStatement );
          TestTraversal (std::map<SgNode*,std::string> nameMap, std::map<SgNode*,std::string> typeMap );
          virtual void visit(SgNode* n);
   };
#endif

#if 0
void 
traverseTemplateArgumentPtrList (SgTemplateArgumentPtrList & templateArgumentList, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
     string template_name;

     printf ("Inside of traverseTemplateArgumentPtrList() \n");

     evaluateNameQualificationForTemplateArgumentList(templateArgumentList,currentScope,positionStatement);

     printf ("Leaving traverseTemplateArgumentPtrList() \n");
   }
#endif

#if 0
TestTraversal::TestTraversal (SgScopeStatement* input_currentScope, SgStatement* input_positionStatement )
   : currentScope(input_currentScope), positionStatement(input_positionStatement)
   {
   }
#endif

#if 0
TestTraversal::TestTraversal ( std::map<SgNode*,std::string> nameMap, std::map<SgNode*,std::string> typeMap )
   : qualifiedNameMapForNames(nameMap), qualifiedNameMapForTypes(typeMap)
   {
   }
#endif

#if 0
void
TestTraversal::visit(SgNode* n)
   {
  // Visit the data members in the type.

     ROSE_ASSERT(n != NULL);
     printf (">>>>>>>>>>>>>>>>>>>>>>>>> Type traversal: n = %p = %s \n",n,n->class_name().c_str());

     switch (n->variantT())
        {
          case V_SgTemplateInstantiationDecl:
             {
            // Lookup any template arguments
               SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(n);
               ROSE_ASSERT(templateInstantiationDeclaration != NULL);

               SgTemplateArgumentPtrList & templateArgumentList = templateInstantiationDeclaration->get_templateArguments();
            // traverseTemplateArgumentPtrList(templateArgumentList,currentScope,positionStatement);
            // evaluateNameQualificationForTemplateArgumentList(templateArgumentList,currentScope,positionStatement);
             }

          default:
             {
            // These IR nodes can't contain types
             }
        }
   }
#endif



void
HiddenListTraversal::traverseType ( SgType* type, SgNode* nodeReferenceToType, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // The type can contain subtypes (e.g. template arguments) and when the subtypes need to be name qualificed the name of the encompassing type 
  // has a name that depends upon its location in the source code (and could vary depending on the positon in a single basic block, I think).


     printf ("<<<<< Starting traversal of type: type = %p = %s \n",type,type->class_name().c_str());

     ROSE_ASSERT(nodeReferenceToType != NULL);

  // Some type IR nodes are difficult to save as a string and reuse. So for now we will skip supporting 
  // some type IR nodes with generated name qualification specific to where they are used.
     bool skipThisType = false;
     if (isSgPointerMemberType(type) != NULL)
        {
          skipThisType = true;
        }

     if (skipThisType == false)
        {
          SgDeclarationStatement* declaration = associatedDeclaration(type);
          if (declaration != NULL)
             {
               evaluateTemplateInstantiationDeclaration(declaration,currentScope,positionStatement);
             }

#if 1
          SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
          ROSE_ASSERT (unparseInfoPointer != NULL);
          unparseInfoPointer->set_outputCompilerGeneratedStatements();

       // Avoid unpasing the class definition when unparseing the type.
          unparseInfoPointer->set_SkipClassDefinition();
#endif
#if 1
       // Associate the unparsing of this type with the statement or scope where it occures.
       // This is the key to use in the lookup of the qualified name. But this is the correct key....
       // unparseInfoPointer->set_reference_node_for_qualification(positionStatement);
       // unparseInfoPointer->set_reference_node_for_qualification(currentScope);
          unparseInfoPointer->set_reference_node_for_qualification(nodeReferenceToType);

          string typeNameString = globalUnparseToString(type,unparseInfoPointer);
          printf ("++++++++++++++++ typeNameString (globalUnparseToString()) = %s \n",typeNameString.c_str());

       // This is symptematic of an error which causes the whole class to be included with the class 
       // definition.  This was fixed by calling unparseInfoPointer->set_SkipClassDefinition() above.
          if (typeNameString.length() > 2000)
             {
               printf ("Error: type names should not be this long... typeNameString.length() = %zu \n",typeNameString.length());
               ROSE_ASSERT(false);
            }
#endif

#if 1
          bool isTemplateName = (typeNameString.find('<') != string::npos) && (typeNameString.find("::") != string::npos);
          if (isTemplateName == true)
             {
               if (typeNameMap.find(nodeReferenceToType) == typeNameMap.end())
                  {
                    printf ("============== Inserting qualifier for name = %s into list at IR node = %p = %s \n",typeNameString.c_str(),nodeReferenceToType,nodeReferenceToType->class_name().c_str());
                    typeNameMap.insert(std::pair<SgNode*,std::string>(nodeReferenceToType,typeNameString));
                  }
                 else
                  {
                 // If it already existes then overwrite the existing information.
                    std::map<SgNode*,std::string>::iterator i = typeNameMap.find(nodeReferenceToType);
                    ROSE_ASSERT (i != typeNameMap.end());

                    string previousTypeName = i->second.c_str();
                    printf ("WARNING: replacing previousTypeName = %s with new typeNameString = %s for nodeReferenceToType = %p = %s \n",previousTypeName.c_str(),typeNameString.c_str(),nodeReferenceToType,nodeReferenceToType->class_name().c_str());

                    if (i->second != typeNameString)
                       {
                         i->second = typeNameString;

#if 1
                         printf ("Error: name in qualifiedNameMapForNames already exists and is different... nodeReferenceToType = %p = %s \n",nodeReferenceToType,nodeReferenceToType->class_name().c_str());
                         ROSE_ASSERT(false);
#endif
                       }
#if 0
                    printf ("Error: name in qualifiedNameMapForNames already exists... nodeReferenceToType = %p = %s \n",nodeReferenceToType,nodeReferenceToType->class_name().c_str());
                    ROSE_ASSERT(false);
#endif
                  }
             }
#endif
        }
       else
        {
       // Output a message when we cheat on this IR node (even if this is a clue for George).
          printf ("Skipping precompuation of string for name qualified type = %p = %s \n",type,type->class_name().c_str());
        }

     printf ("<<<<< Ending traversal of type: type = %p = %s \n",type,type->class_name().c_str());
   }


bool
HiddenListTraversal::skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(SgDeclarationStatement* declaration)
   {
  // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in 
  // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
  // definition could not live in the SgBasicBlock.

     bool skipNameQualification = false;
     if (referencedNameSet.find(declaration->get_firstNondefiningDeclaration()) == referencedNameSet.end())
        {
          printf ("$$$$$$$$$$ NOT Found: declaration %p = %s in referencedNameSet \n",declaration,declaration->class_name().c_str());
          skipNameQualification = true;
        }
       else
        {
          printf ("$$$$$$$$$$ FOUND: declaration %p = %s in referencedNameSet \n",declaration,declaration->class_name().c_str());
        }

     return skipNameQualification;
   }


HiddenListInheritedAttribute
HiddenListTraversal::evaluateInheritedAttribute(SgNode* n, HiddenListInheritedAttribute inheritedAttribute)
   {
     ROSE_ASSERT(n != NULL);

  // printf ("Inside of HiddenListTraversal::evaluateInheritedAttribute(): node = %p = %s \n",n,n->class_name().c_str());

  // Extra information about the location of the current node.
     Sg_File_Info* fileInfo = n->get_file_info();
     if (fileInfo != NULL)
        {
           printf ("HiddenListTraversal: --- n = %p = %s line %d col = %d file = %s \n",n,n->class_name().c_str(),fileInfo->get_line(),fileInfo->get_col(),fileInfo->get_filenameString().c_str());
        }

  // Locations where name qualified references can exist:
  //   1) Base class names
  //   2) Variable names in declarations (see test2011_30.C)
  //   3) Types referenced by variables
  //   4) Types referenced in function parameter lists
  //   5) Return types referenced by functions (including covariant types for member functions)
  //   6) References to functions thrown by functions
  //   7) Namespaces referenced by SgUsingDirectiveStatement IR nodes
  //   8) Variables and declarations reference from SgUsingDeclarationStatement IR nodes
  //   9) Functions reference by SgFunctionRefExp IR nodes
  //  10) Functions reference by SgMemberFunctionRefExp IR nodes
  //  11) Variable reference by SgVarRefExp IR nodes
  //  12) Template arguments (and default template parameter specifications)
  //  13) Template parameters?
  //  14) Function declarations
  //  15) Member function declarations
  //  16) Typedef declarations
  //  17) Throw exception lists
  //  18) A number of expressions (listed below)
  //         SgVarRefExp
  //         SgFunctionRefExp
  //         SgMemberFunctionRefExp
  //         SgConstructorInitializer
  //         SgNewExp
  //         SgCastExp
  //         SgSizeOfOp
  //         SgTypeIdOp
  //  19)
  //  20)

  // The use of name qualification in types is a complicated because types are shared and the same type can 
  // have it's template arguments qualified differently depending on where it is referenced.  This is an 
  // issue for all references to types containing template arguments and not just where SgInitializedName 
  // are used. Since name qualification of the same type can only vary at most from statement to statement 
  // in some cases likely only from scope to scope) we need only associate names to statements (see note 1).  
  // I would like to for now use scopes as the finest level of resolution.
  // The solution:
  //    1) Support a test for which types are effected.  a member function of SgType will evaluate if
  //       a type uses template arguments or subtypes using template arguments and if these could require 
  //       name qualification. 
  //    2) A map will be created in each scope (or maybe statement) for types used in that scope (or statement)
  //       which will store the computed name of the type (evaluated as part of the name qualification support;
  //       called immediately at the start of the unparsing of each SgFile).  The SgType pointer will be used
  //       as the key into the map of SgType to names (stored as strings).
  //    3) The unparser will check for entries in the associated map and use the stringified type names if they
  //       are available.  This can be done at the top level of the unparseType() function.

  // Note 1: A forward declaraion of a function (and maybe a class) can happen in a scope that does not allow a defining
  // declaration and when this happens the name qualification of that function is undefined.  However after a
  // forward declaration in a scope permitting a defining declaration, the function name must be qualified as per
  // usual name qualification rules.

  // DQ (6/11/2011): This is a new IR nodes, but the use of it causes a few problems (test2004_109.C) 
  // because the source position is not computed correctly (I think).
     SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(n);
     if (templateClassDefinition != NULL)
        {
          printf ("Name qualification of SgTemplateClassDefinition = %p not implemented \n",templateClassDefinition);
          ROSE_ASSERT(false);
        }

     SgClassDefinition* classDefinition = isSgClassDefinition(n);
  // if (classDefinition != NULL && templateClassDefinition == NULL)
     if (classDefinition != NULL)
        {
       // Add all of the named types from this class into the set that have already been seen.
       // Note that this should not include nested classes (I think).

          SgBaseClassPtrList & baseClassList = classDefinition->get_inheritances();

          printf ("!!!!! Evaluate the derived classes: are they visible --- baseClassList.size() = %zu \n",baseClassList.size());

#if 0
          string declaration_name = classDefinition->get_declaration()->unparseToString();
          printf ("test: declaration_name (unparseToString()) = %s \n",declaration_name.c_str());
#endif

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

  // Handle references to SgMemberFunctionDeclaration...
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(n);
     if (classDeclaration != NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

       // We need the structural location in scope (not the semantic one).
          SgScopeStatement* currentScope = isSgScopeStatement(classDeclaration->get_parent());

       // ROSE_ASSERT(currentScope != NULL);
          if (currentScope != NULL)
             {
            // Only use name qualification where the scopes of the declaration's use (currentScope) is not the same 
            // as the scope of the class declaration.  However, the analysis should work and determin that the 
            // required name qualification length is zero.
               if (currentScope != classDeclaration->get_scope())
                  {
                    int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,classDeclaration);
                    printf ("SgClassDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

                    setNameQualification(classDeclaration,amountOfNameQualificationRequired);
                  }
                 else
                  {
                 // Don't know what test code exercises this case (see test2011_62.C).
                    printf ("WARNING: SgClassDeclaration -- currentScope is not available through predicate (currentScope != classDeclaration->get_scope()), not clear why! \n");

                    ROSE_ASSERT(classDeclaration->get_parent() == classDeclaration->get_scope());
                    printf ("name qualification for classDeclaration->get_scope()  = %p = %s \n",classDeclaration->get_scope(),classDeclaration->get_scope()->class_name().c_str());
                    printf ("classDeclaration->get_parent()                        = %p = %s \n",classDeclaration->get_parent(),classDeclaration->get_parent()->class_name().c_str());

                 // ROSE_ASSERT(false);
                  }
             }
            else
             {
               printf ("WARNING: SgClassDeclaration -- currentScope is not available, not clear why! \n");
            // ROSE_ASSERT(false);
             }
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

          printf ("================ Calling nameQualificationDepthForType to evaluate the type \n");
       // Compute the depth of name qualification from the current statement:  variableDeclaration.
          int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,variableDeclaration);
          printf ("SgVariableDeclaration's type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForType);

       // Not all types have an associated declaration, but some do: examples include classes, typedefs, and enums.
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

       // This is not always the correct current scope (see test2011_70.C for an example).
          SgScopeStatement* currentScope = SageInterface::getScope(variableDeclaration);
       // SgScopeStatement* currentScope = isSgScopeStatement(variableDeclaration->get_parent());
          ROSE_ASSERT(currentScope != NULL);

          printf ("\n++++++++++++++++ Calling nameQualificationDepth to evaluate the name currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
          int amountOfNameQualificationRequiredForName = nameQualificationDepth(initializedName,currentScope,variableDeclaration);
          printf ("SgVariableDeclaration's variable name: amountOfNameQualificationRequiredForName = %d \n",amountOfNameQualificationRequiredForName);

#if 1
       // DQ (6/5/2011): Debugging test2011_75.C ...

       // If there is a previous declaration then that is the original declaration (see test2011_30.C for an example of this case).
          SgInitializedName* originalInitializedName = initializedName->get_prev_decl_item(); 
       // SgInitializedName* originalInitializedName = initializedName;
          if (originalInitializedName != NULL)
             {
               printf ("originalInitializedName = %p = %s \n",originalInitializedName,originalInitializedName->get_name().str());

            // DQ (6/5/2011): Handle case where originalInitializedName->get_parent() == NULL
            // ROSE_ASSERT(originalInitializedName->get_parent() != NULL);
               if (originalInitializedName->get_parent() != NULL)
                  {
                    SgVariableDeclaration* originalVariableDeclaration = isSgVariableDeclaration(originalInitializedName->get_parent());

                 // DQ (6/4/2011): test2005_68.C is an example that fails this test.
                 // ROSE_ASSERT(originalVariableDeclaration != NULL);
                 // setNameQualification(variableDeclaration,originalVariableDeclaration,amountOfNameQualificationRequiredForName);
                    if (originalVariableDeclaration != NULL)
                       {
                      // Accumulate names of scopes starting at the originalVariableDeclaration (amountOfNameQualificationRequiredForName times).
                         setNameQualification(variableDeclaration,originalVariableDeclaration,amountOfNameQualificationRequiredForName);
                       }
                      else
                       {
                      // DQ (6/4/2011): test2005_68.C is an example that fails this test (also test2011_70.C).  If the SgInitializedName 
                      // is referenced before it is declared then it can appear associated via the symbol table through a non variable 
                      // declaration (SgVariableDeclaration) IR node.
                         printf ("WARNING: variableDeclaration case of name qualification originalVariableDeclaration == NULL (building alternativeDecaration) \n");
                         SgFunctionDeclaration* alternativeDecaration = TransformationSupport::getFunctionDeclaration(originalInitializedName->get_parent());
                         ROSE_ASSERT(alternativeDecaration != NULL);
                         setNameQualification(variableDeclaration,alternativeDecaration,amountOfNameQualificationRequiredForName);
                       }
                  }
                 else
                  {
                 // This is demonstrated by test2011_72.C (and maybe test2005_103.C).  I can't really fix this in the AST 
                 // until I can get all of the test codes back to passing with the new name qualification support, so this 
                 // will have to wait.  We output an error message for now.
                    printf ("Error: originalInitializedName->get_parent() == NULL (skipping any required name qualification, I think this is a bug in the AST) \n");   
                  }
             }
#endif
          printf ("++++++++++++++++ DONE: Calling nameQualificationDepth to evaluate the name \n\n");
        }

  // Handle SgType name qualification where SgInitializedName's appear outside of SgVariableDeclaration's (e.g. in function parameter declarations).
     SgInitializedName* initializedName = isSgInitializedName(n);
     if (initializedName != NULL)
        {
#if 0
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

            // The SgInitializedName can be used in the variable list of the SgVariableDeclaration, if so then the SgInitializedName stores the name of the qualifier for the type.
            // However, we can't always recompute the name of the type correctly if it contains template parameters since this is position dependent.  So we have to save the 
            // type as a string and store it with the SgInitializedName for use if template arguments are used in the type.  This is complicated because types are shared and
            // the same type can have it's template arguments qulified differently depending on where it is referenced.  This is an issue for all references to types containing
            // template arguments and not just where SgInitializedName are used. 
             }
#else
       // We want to handle types from every where a SgInitializedName might be used.
          SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());
          if (declaration != NULL)
             {
               SgStatement* currentStatement = TransformationSupport::getStatement(initializedName);
               ROSE_ASSERT(currentStatement != NULL);

               SgScopeStatement* currentScope = currentStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);

            // int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentScope,currentStatement);
               int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentStatement);
               printf ("SgInitializedName's (%s) type: amountOfNameQualificationRequiredForType = %d \n",initializedName->get_name().str(),amountOfNameQualificationRequiredForType);

               setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType);

            // This can be inside of the case where (declaration != NULL)
            // Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
               traverseType(initializedName->get_type(),initializedName,currentScope,currentStatement);
             }
#endif

        }

  // Handle references to SgFunctionDeclaration...
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);

  // DQ (6/4/2011): Avoid processing as both member and non-member function...
  // if (functionDeclaration != NULL)
     if (functionDeclaration != NULL && isSgMemberFunctionDeclaration(n) == NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

       // We need the structural location in scope (not the semantic one).
          SgScopeStatement* currentScope = isSgScopeStatement(functionDeclaration->get_parent());
       // SgScopeStatement* currentScope = TransformationSupport::getScope(functionDeclaration);
       // SgScopeStatement* currentScope = functionDeclaration->get_scope();

       // SgStatement* currentStatement = TransformationSupport::getStatement(functionDeclaration->get_parent());
       // ROSE_ASSERT(currentStatement != NULL);

       // Make sure these are the same. test2005_57.C presents what might be a relevant test code.
       // ROSE_ASSERT(currentScope == TransformationSupport::getScope(currentStatement));
       // ROSE_ASSERT(currentScope == currentStatement->get_scope());

       // ROSE_ASSERT(currentScope != NULL);
          if (currentScope != NULL)
             {
#if 1
       // Handle the function return type...
          ROSE_ASSERT(functionDeclaration->get_orig_return_type() != NULL);
          ROSE_ASSERT(functionDeclaration->get_type() != NULL);
          ROSE_ASSERT(functionDeclaration->get_type()->get_return_type() != NULL);
          SgType* returnType = functionDeclaration->get_type()->get_return_type();
          ROSE_ASSERT(returnType != NULL);
#if 0
          int amountOfNameQualificationRequiredForReturnType = nameQualificationDepth(returnType,currentScope,functionDeclaration);
          printf ("SgFunctionDeclaration's return type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForReturnType);

          printf ("Putting the name qualification for the type into the return type of SgFunctionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->get_name().str());
          setNameQualificationReturnType(functionDeclaration,amountOfNameQualificationRequiredForReturnType);
#else
          SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(returnType);
          if (declaration != NULL)
             {
               int amountOfNameQualificationRequiredForReturnType = nameQualificationDepth(declaration,currentScope,functionDeclaration);
               printf ("SgFunctionDeclaration's return type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForReturnType);

               printf ("Putting the name qualification for the type into the return type of SgFunctionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->get_name().str());
            // setNameQualificationReturnType(functionDeclaration,amountOfNameQualificationRequiredForReturnType);
               setNameQualificationReturnType(functionDeclaration,declaration,amountOfNameQualificationRequiredForReturnType);
             }
            else
             {
            // This case is common for builtin functions such as: __builtin_powi
#if 1
               printf ("declaration == NULL: could not put name qualification for the type into the return type of SgFunctionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->get_name().str());
#endif
             }
#endif
#endif

       // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
          traverseType(returnType,functionDeclaration,currentScope,functionDeclaration);

       // Handle the function name...
       // DQ (6/20/2011): Friend function can be qualified.
       // if (functionDeclaration->get_declarationModifier().isFriend() == true || functionDeclaration->get_specialFunctionModifier().isOperator() == true)
          if (functionDeclaration->get_specialFunctionModifier().isOperator() == true)
             {
            // DQ (6/19/2011): We sometimes have to qualify friends if it is to avoid ambiguity (see test2006_159.C) (but we never qualify an operator, I think).
            // Maybe a friend declaration should add an SgAliasSymbol to the class definition scope's symbol table.
            // Then simpler rules (no special case) would cause the name qualification to be generated properly.

            // DQ (6/20/2011): Friend function can be qualified and a fix to add a SgAliasSymbol to the class definition scope's symbol table 
            // should allow it to be handled with greater precission.
            // Never use name qualification for friend functions or operators. I am more sure of the case of friend functions than operators.
            // Friend functions will have a global scope (though this might change in the future; google "friend global scope injection").
            // printf ("Detected a friend or operator function, these are not provided with name qualification. \n");
               printf ("Detected a operator function, these are not provided with name qualification. \n");
             }
            else
             {
            // Only use name qualification where the scopes of the declaration's use (currentScope) is not the same 
            // as the scope of the function declaration.  However, the analysis should work and determin that the 
            // required name qualification length is zero.
               printf ("I would like to not have to have this SgFunctionDeclaration logic, we should get the name qualification correct more directly. \n");
               if (currentScope != functionDeclaration->get_scope())
                  {
                    int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
                    printf ("SgFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#if 1
                 // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in 
                 // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
                 // definition could not live in the SgBasicBlock.
                    bool skipNameQualification = skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(functionDeclaration);
                    if (skipNameQualification == false)
                         setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
#else
                 // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in 
                 // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
                 // definition could not live in the SgBasicBlock.
                    if (referencedNameSet.find(functionDeclaration->get_firstNondefiningDeclaration()) == referencedNameSet.end())
                       {
                         printf ("$$$$$$$$$$ NOT Found: declaration %p = %s in referencedNameSet \n",functionDeclaration,functionDeclaration->get_name().str());
                         amountOfNameQualificationRequired = 0;
                       }
                      else
                       {
                         printf ("$$$$$$$$$$ FOUND: declaration %p = %s in referencedNameSet \n",functionDeclaration,functionDeclaration->get_name().str());
                       }

                    setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
#endif
                  }
             }

             }
            else
             {
            // Note that test2005_57.C presents an example that triggers this case and so might be a relevant 
            // test code.  Example: "template<typename T> void foobar (T x){ }".
               printf ("WARNING: SgFunctionDeclaration -- currentScope is not available, not clear why! \n");
            // ROSE_ASSERT(false);
             }
        }

  // Handle references to SgMemberFunctionDeclaration...
     SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(n);
     if (memberFunctionDeclaration != NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

       // We need the structural location in scope (not the semantic one).
          SgScopeStatement* currentScope = isSgScopeStatement(memberFunctionDeclaration->get_parent());

       // ROSE_ASSERT(currentScope != NULL);
          if (currentScope != NULL)
             {

#if 1
            // Handle the function return type...
               ROSE_ASSERT(memberFunctionDeclaration->get_orig_return_type() != NULL);
               ROSE_ASSERT(memberFunctionDeclaration->get_type() != NULL);
               ROSE_ASSERT(memberFunctionDeclaration->get_type()->get_return_type() != NULL);
               SgType* returnType = memberFunctionDeclaration->get_type()->get_return_type();
               ROSE_ASSERT(returnType != NULL);
#if 0
               int amountOfNameQualificationRequiredForReturnType = nameQualificationDepth(returnType,currentScope,memberFunctionDeclaration);
               printf ("SgFunctionDeclaration's return type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForReturnType);

               printf ("Putting the name qualification for the type into the return type of SgFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
               setNameQualificationReturnType(memberFunctionDeclaration,amountOfNameQualificationRequiredForReturnType);
#else
               SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(returnType);
               if (declaration != NULL)
                  {
                    int amountOfNameQualificationRequiredForReturnType = nameQualificationDepth(declaration,currentScope,memberFunctionDeclaration);
                    printf ("SgMemberFunctionDeclaration's return type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForReturnType);

                    printf ("Putting the name qualification for the type into the return type of SgMemberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
                    setNameQualificationReturnType(memberFunctionDeclaration,declaration,amountOfNameQualificationRequiredForReturnType);
                  }
                 else
                  {
                    printf ("declaration == NULL: could not put name qualification for the type into the return type of SgMemberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
                  }
#endif
#endif
            // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
               traverseType(returnType,memberFunctionDeclaration,currentScope,memberFunctionDeclaration);
               printf ("Don't forget possible covariant return types for SgMemberFunctionDeclaration IR nodes \n");

            // Only use name qualification where the scopes of the declaration's use (currentScope) is not the same 
            // as the scope of the function declaration.  However, the analysis should work and determin that the 
            // required name qualification length is zero.
               printf ("I would like to not have to have this SgMemberFunctionDeclaration logic, we should get the name qualification correct more directly. \n");
               if (currentScope != memberFunctionDeclaration->get_scope())
                  {
                    int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,memberFunctionDeclaration);
                    printf ("SgMemberFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

                    setNameQualification(memberFunctionDeclaration,amountOfNameQualificationRequired);
                  }
                 else
                  {
                 // Don't know what test code exercises this case (see test2005_73.C).
                    printf ("WARNING: SgMemberFunctionDeclaration -- currentScope is not available through predicate (currentScope != memberFunctionDeclaration->get_scope()), not clear why! \n");
                 // ROSE_ASSERT(false);
                  }
             }
            else
             {
            // Note that test2005_63.C presents an example that triggers this case and so might be a relevant.
            // This is also the reason why test2005_73.C is failing!!!  Fix it tomorrow!!! (SgTemplateInstantiationDirectiveStatement)
               SgDeclarationStatement* currentStatement = isSgDeclarationStatement(memberFunctionDeclaration->get_parent());
               ROSE_ASSERT(currentStatement != NULL);
               SgScopeStatement* currentScope = isSgScopeStatement(currentStatement->get_parent());
               if (currentScope != NULL)
                  {
                    int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,memberFunctionDeclaration);
                    printf ("SgMemberFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

                    setNameQualification(memberFunctionDeclaration,amountOfNameQualificationRequired);
                  }
                 else
                  {
                    printf ("WARNING: SgMemberFunctionDeclaration -- currentScope is not available through parent SgDeclarationStatement, not clear why! \n");
                    ROSE_ASSERT(false);
                  }

               printf ("WARNING: SgMemberFunctionDeclaration -- currentScope is not available, not clear why! \n");
            // ROSE_ASSERT(false);
             }
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
            // int amountOfNameQualificationRequiredForBaseType = nameQualificationDepth(baseTypeDeclaration,currentScope,typedefDeclaration);
               int amountOfNameQualificationRequiredForBaseType = nameQualificationDepth(baseType,currentScope,typedefDeclaration);
               printf ("SgTypedefDeclaration: amountOfNameQualificationRequiredForBaseType = %d \n",amountOfNameQualificationRequiredForBaseType);

               ROSE_ASSERT(baseTypeDeclaration != NULL);
               setNameQualification(typedefDeclaration,baseTypeDeclaration,amountOfNameQualificationRequiredForBaseType);
             }

       // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
          traverseType(baseType,typedefDeclaration,currentScope,typedefDeclaration);
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

#if 1
            // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in 
            // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
            // definition could not live in the SgBasicBlock.
               bool skipNameQualification = skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(functionDeclaration);
               if (skipNameQualification == false)
                    setNameQualification(functionRefExp,functionDeclaration,amountOfNameQualificationRequired);
#else
            // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in 
            // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
            // definition could not live in the SgBasicBlock.
               if (referencedNameSet.find(functionDeclaration->get_firstNondefiningDeclaration()) == referencedNameSet.end())
                  {
                    printf ("$$$$$$$$$$ NOT Found: declaration %p = %s in referencedNameSet \n",functionDeclaration,functionDeclaration->get_name().str());
                    amountOfNameQualificationRequired = 0;
                  }
                 else
                  {
                    printf ("$$$$$$$$$$ FOUND: declaration %p = %s in referencedNameSet \n",functionDeclaration,functionDeclaration->get_name().str());
                  }

               setNameQualification(functionRefExp,functionDeclaration,amountOfNameQualificationRequired);
#endif
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

            // DQ (6/5/2011): test2005_112.C demonstrates a case where this special case applies.
            // If this had to be more qualified, (amountOfNameQualificationRequired > 0), then it should be sufficently qualified and not need special case handling.
               if (amountOfNameQualificationRequired == 0)
                  {
                 // GNU reports that: "ISO C++ forbids taking the address of an unqualified or parenthesized non-static member function to form a pointer to member function.  Say '&A::f1'"
                    ROSE_ASSERT(memberFunctionRefExp->get_parent() != NULL);
                    SgAddressOfOp* addressOfOp = isSgAddressOfOp(memberFunctionRefExp->get_parent());
                    if (addressOfOp != NULL)
                       {
                      // I think that setting this to 1 is enough, but there could be a case where it must be more qualified than just to include it's qualified class name.
                         amountOfNameQualificationRequired = 1;
                       }
                  }

               setNameQualification(memberFunctionRefExp,memberFunctionDeclaration,amountOfNameQualificationRequired);
             }
            else
             {
               printf ("WARNING: memberFunctionDeclaration == NULL in SgMemberFunctionCallExp for name qualification support! \n");
             }
        }

  // DQ (5/31/2011): This is a derived class from SgExpression and SgInitializer...
     SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(n);
     if (constructorInitializer != NULL)
        {
          SgMemberFunctionDeclaration* memberFunctionDeclaration = constructorInitializer->get_declaration();
#if 0
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(constructorInitializer->get_declaration());
          if (templateInstantiationMemberFunctionDeclaration != NULL)
             {
               printf ("Need to process the template arguments for the SgTemplateInstantiationMemberFunctionDecl \n");
               ROSE_ASSERT(false);
             }
#endif
          SgStatement* currentStatement = TransformationSupport::getStatement(constructorInitializer);
          ROSE_ASSERT(currentStatement != NULL);

       // If this could occur in a SgForStatement then this should be fixed up as it is elsewhere...
          SgScopeStatement* currentScope = currentStatement->get_scope();
          ROSE_ASSERT(currentScope != NULL);

          if (memberFunctionDeclaration != NULL)
             {
               int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,currentStatement);
               printf ("SgConstructorInitializer's constructor member function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

               setNameQualification(constructorInitializer,memberFunctionDeclaration,amountOfNameQualificationRequired);
             }
            else
             {
            // DQ (6/1/2011): This happens when there is not constructor that can be used to build a class, in this case the class name must be used to define a default constructor.
            // This is a problem for test2004_130.C (at line 165 col = 14 file = /home/dquinlan/ROSE/ROSE_CompileTree/git-LINUX-64bit-4.2.4-dq-cxx-rc/include-staging/g++_HEADERS/hdrs3/bits/stl_iterator_base_types.h).
            // Need to investigate this later (it is strange that it is not an issue in test2011_63.C, but it is a struct instead of a class and that might be why).
               printf ("WARNING: memberFunctionDeclaration == NULL in SgConstructorInitializer for name qualification support! \n");
            // ROSE_ASSERT(false);

            // DQ (6/4/2011): Added support for this case.
               SgClassDeclaration* classDeclaration = constructorInitializer->get_class_decl();
            // ROSE_ASSERT(classDeclaration != NULL);
               if (classDeclaration != NULL)
                  {
                 // An example of the problem is test2005_42.C, where the class name is used to generate the constructor initializer name.
                    int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,currentStatement);
                    printf ("SgConstructorInitializer's constructor member function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

                    setNameQualification(constructorInitializer,classDeclaration,amountOfNameQualificationRequired);
                  }
                 else
                  {
                 // This is a strange error: see test2004_77.C
                    printf ("WARNING: In SgConstructorInitializer name qualification support: neither memberFunctionDeclaration or classDeclaration are valid pointers. \n");
                  }
             }
        }

     SgVarRefExp* varRefExp = isSgVarRefExp(n);
     if (varRefExp != NULL)
        {
       // We need to store the information about the required name qualification in the SgVarRefExp IR node.

          SgStatement* currentStatement = TransformationSupport::getStatement(varRefExp);
          ROSE_ASSERT(currentStatement != NULL);

       // DQ (5/30/2011): Handle the case of test2011_58.C (index declaration in for loop construct).
       // SgScopeStatement* currentScope = currentStatement->get_scope();
          SgScopeStatement* currentScope = isSgScopeStatement(currentStatement);
          if (currentScope == NULL)
               currentScope = currentStatement->get_scope();

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
                 // Skip these cases ... no name qualification required.
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

                         SgStatement* currentStatement = TransformationSupport::getStatement(initializedName->get_parent());
                         ROSE_ASSERT(currentStatement != NULL);

                         SgScopeStatement* targetScope = initializedName->get_scope();
                         printf ("targetScope = %p = %s \n",targetScope,targetScope->class_name().c_str());

                         printf ("SgVarRefExp case (no associated variableDeclaration): currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());

                         printf ("Exiting as a test! \n");
                      // ROSE_ASSERT(false);

#if 0
                         int amountOfNameQualificationRequired = nameQualificationDepth(targetScope,currentScope,currentStatement);
                         printf ("SgVarRefExp's SgDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

                         setNameQualification(varRefExp,targetScope,amountOfNameQualificationRequired);
#endif
#if 0
                      // DQ (6/4/2011): test2005_68.C is an example that fails this test.
                      // ROSE_ASSERT(variableDeclaration != NULL);
                         if (variableDeclaration != NULL)
                            {
                              printf ("WARNING: varRefExp name qualification case of variableDeclaration == NULL \n");
                            }
#endif
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

  // DQ (6/9/2011): Added support for test2011_79.C (enum values can require name qualification).
     SgEnumVal* enumVal = isSgEnumVal(n);
     if (enumVal != NULL)
        {
          SgStatement* currentStatement = TransformationSupport::getStatement(enumVal);
          ROSE_ASSERT(currentStatement != NULL);

          SgScopeStatement* currentScope = isSgScopeStatement(currentStatement);

       // If the current statement was not a scope, then what scope contains the current statement.
          if (currentScope == NULL)
               currentScope = currentStatement->get_scope();

          ROSE_ASSERT(currentScope != NULL);

          SgEnumDeclaration* enumDeclaration = enumVal->get_declaration();
          ROSE_ASSERT(enumDeclaration != NULL);

          int amountOfNameQualificationRequired = nameQualificationDepth(enumDeclaration,currentScope,currentStatement);
          printf ("SgEnumVal: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);

          setNameQualification(enumVal,enumDeclaration,amountOfNameQualificationRequired);
        }

  // DQ (6/2/2011): Handle the range of expressions that can reference types that might require name qualification...
     SgNewExp*   newExp   = isSgNewExp(n);
     SgSizeOfOp* sizeOfOp = isSgSizeOfOp(n);
     SgCastExp*  castExp  = isSgCastExp(n);
     SgTypeIdOp* typeIdOp = isSgTypeIdOp(n);
     if (newExp != NULL || sizeOfOp != NULL || castExp != NULL || typeIdOp != NULL)
        {
          SgExpression* referenceToType = isSgExpression(n);

          bool skipQualification = false;

          SgType* qualifiedType = NULL;
          switch(n->variantT())
             {
               case V_SgNewExp:   qualifiedType = newExp->get_specified_type(); break;
               case V_SgSizeOfOp: 
                  {
                    qualifiedType = sizeOfOp->get_operand_type(); 
                    if (qualifiedType == NULL)
                       {
                      // This is the case of a value, which need not be qualified. Excpet that it could be a variable, but then it should be a SgVarRefExp
                         ROSE_ASSERT(sizeOfOp->get_operand_expr() != NULL);
                         skipQualification = true;
                       }
                    break;
                  }

               case V_SgCastExp:  qualifiedType = castExp->get_type();          break;
               case V_SgTypeIdOp: qualifiedType = typeIdOp->get_operand_type(); break;

               default:
                  {
                 // Anything else should not make it this far...
                    printf ("Error: default reached in switch... n = %p = %s \n",n,n->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }

          if (qualifiedType == NULL)
             {
            // We see this case for test2006_139.C  (code is: "sizeof("string")" or "sizeof(<SgVarRefExp>)" ).
               printf ("Note: qualifiedType == NULL for n = %p = %s \n",n,n->class_name().c_str());
             }
       // ROSE_ASSERT(qualifiedType != NULL);

          if (skipQualification == false)
             {
               SgDeclarationStatement* associatedTypeDeclaration = associatedDeclaration(qualifiedType);
               if (associatedTypeDeclaration != NULL)
                  {
                    SgStatement* currentStatement = TransformationSupport::getStatement(n);
                    ROSE_ASSERT(currentStatement != NULL);

                    SgScopeStatement* currentScope = currentStatement->get_scope();
                    ROSE_ASSERT(currentScope != NULL);

                    int amountOfNameQualificationRequiredForType = nameQualificationDepth(associatedTypeDeclaration,currentScope,currentStatement);
                    printf ("SgExpression (name = %s) type: amountOfNameQualificationRequiredForType = %d \n",referenceToType->class_name().c_str(),amountOfNameQualificationRequiredForType);

                    setNameQualification(referenceToType,associatedTypeDeclaration,amountOfNameQualificationRequiredForType);

                 // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
                    traverseType(qualifiedType,referenceToType,currentScope,currentStatement);
                  }
                 else
                  {
                    printf ("Note: associatedTypeDeclaration == NULL in SgExpression for name qualification support! referenceToType = %s \n",referenceToType->class_name().c_str());
                  }
             }
        }


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

          SgDeclarationStatement* firstNondefiningDeclaration   = declaration->get_firstNondefiningDeclaration();
       // ROSE_ASSERT(firstNondefiningDeclaration != NULL);
          if (firstNondefiningDeclaration == NULL)
             {
               printf ("WARNING: declaration->get_firstNondefiningDeclaration() == NULL for declaration = %p = %s \n",declaration,declaration->class_name().c_str());
             }

       // Look at each declaration, but as soon as we find an acceptable one put the firstNondefiningDeclaration 
       // into the set so that we can search on a uniform representation of the declaration.
       // SgScopeStatement*       scopeOfNondefiningDeclaration = isSgScopeStatement(firstNondefiningDeclaration->get_parent());
          SgScopeStatement*       scopeOfDeclaration = isSgScopeStatement(declaration->get_parent());

          bool acceptableDeclarationScope = false;

       // I think that some declarations might not appear in a scope properly (e.g pointer to function, etc.)
       // ROSE_ASSERT(scopeOfNondefiningDeclaration != NULL);
          if (scopeOfDeclaration != NULL)
             {
               switch(scopeOfDeclaration->variantT())
                  {
                 // At least this case is not allowed.
                    case V_SgBasicBlock: acceptableDeclarationScope = false;
                         break;

                 // Everything else is OK!
                    default:
                       {
                         printf ("scopeOfNondefiningDeclaration = %p = %s \n",scopeOfDeclaration,scopeOfDeclaration->class_name().c_str());
                         acceptableDeclarationScope = true;
                       }
                  }
             }
            else
             {
            // This appears to fail for something in rose_edg_required_macros_and_functions.h.
               printf ("I hope that we can make this an error (scopeOfDeclaration == NULL \n");
            // ROSE_ASSERT(false);
             }

       // if (referencedNameSet.find(firstNondefiningDeclaration) == referencedNameSet.end())
          if (acceptableDeclarationScope == true && referencedNameSet.find(firstNondefiningDeclaration) == referencedNameSet.end())
             {
            // printf ("Adding declaration to set of visited declarations \n");
               referencedNameSet.insert(firstNondefiningDeclaration);
             }
        }

#if 0
     printf ("Leaving HiddenListTraversal::evaluateInheritedAttribute(): node = %p = %s \n",n,n->class_name().c_str());
#endif

     return HiddenListInheritedAttribute(inheritedAttribute);
   }


HiddenListSynthesizedAttribute
HiddenListTraversal::evaluateSynthesizedAttribute(SgNode* n, HiddenListInheritedAttribute inheritedAttribute, SynthesizedAttributesList synthesizedAttributeList)
   {
  // This is not used now but will likely be used later.
     HiddenListSynthesizedAttribute returnAttribute;

     return returnAttribute;
   }

// ************************************************************************************
//    These overloaded functions, setNameQualification(), support references to IR 
// nodes that require name qualification.  Each function inserts a qualified name 
// (string) into a map stored as a static data member in SgNode. For each IR node 
// that is qualified, the reference to the IR node carries the name qualification 
// (is used as a key in the map of qualified names).  There are two maps, one for 
// the qualification of names and one for qualification of types.  Note that, since 
// types are shared, it is more clear that the type can't carry the qualified name
// because it could be different at each location where the type is referenced; thus
// the reference to the type carries the qualified name (via the map).  The case of 
// why named IR constructs have to have there qualified name in the IR node referencing
// the named construct is similar.
//
// They are only a few IR nodes that reference IR nodes that can be qualified:
//    SgExpression IR nodes:
//       SgVarRefExp
//       SgFunctionRefExp
//       SgMemberFunctionRefExp
//       SgConstructorInitializer
//       SgNewExp
//       SgCastExp
//       SgSizeOfOp
//       SgTypeIdOp
//
//    SgDeclarationStatement IR nodes:
//       SgFunctionDeclaration (for the function name)
//       SgFunctionDeclaration (for the return type)
//       SgUsingDeclarationStatement (for references to a declaration (e.g. namespace or class))
//       SgUsingDeclarationStatement (for references to a SgInitializedName)
//       SgUsingDirectiveStatement
//       SgVariableDeclaration 
//       SgTypedefDeclaration
//       SgClassDeclaration
//
//    SgStatement IR nodes:
//       SgForInitStatement is not a problems since it is a list of SgInitializedName
//
//    SgLocatedNode nodes:
//       SgInitializedName
//
//    SgSupport nodes:
//       SgBaseClass
//       SgTemplateArgument
//
// Other (not yet supported) IR nodes recognized to reference types that could require name 
// qualification support:
//    SgExpression IR nodes:
//
//    And maybe also:
//       SgPseudoDestructorRefExp
//       SgTemplateParameter
//
// ************************************************************************************

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

     string qualifier = setNameQualificationSupport(variableDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     varRefExp->set_global_qualification_required(outputGlobalQualification);
     varRefExp->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     varRefExp->set_type_elaboration_required(outputTypeEvaluation);

     printf ("In HiddenListTraversal::setNameQualification(): varRefExp->get_name_qualification_length()     = %d \n",varRefExp->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): varRefExp->get_type_elaboration_required()     = %s \n",varRefExp->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): varRefExp->get_global_qualification_required() = %s \n",varRefExp->get_global_qualification_required() ? "true" : "false");

     if (qualifiedNameMapForNames.find(varRefExp) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),varRefExp,varRefExp->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(varRefExp,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
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

     string qualifier = setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     functionRefExp->set_global_qualification_required(outputGlobalQualification);
     functionRefExp->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     functionRefExp->set_type_elaboration_required(outputTypeEvaluation);

     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_name_qualification_length()     = %d \n",functionRefExp->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_type_elaboration_required()     = %s \n",functionRefExp->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_global_qualification_required() = %s \n",functionRefExp->get_global_qualification_required() ? "true" : "false");

     if (qualifiedNameMapForNames.find(functionRefExp) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionRefExp,functionRefExp->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(functionRefExp,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
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

     string qualifier = setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     functionRefExp->set_global_qualification_required(outputGlobalQualification);
     functionRefExp->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     functionRefExp->set_type_elaboration_required(outputTypeEvaluation);

     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_name_qualification_length()     = %d \n",functionRefExp->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_type_elaboration_required()     = %s \n",functionRefExp->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): functionRefExp->get_global_qualification_required() = %s \n",functionRefExp->get_global_qualification_required() ? "true" : "false");

     if (qualifiedNameMapForNames.find(functionRefExp) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionRefExp,functionRefExp->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(functionRefExp,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }


// DQ (6/4/2011): This function handles a specific case that is demonstrated by test2005_42.C.
// DQ (6/1/2011): Added support for qualification of the SgConstructorInitializer.
// void HiddenListTraversal::setNameQualification(SgConstructorInitializer* constructorInitializer, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
void
HiddenListTraversal::setNameQualification(SgConstructorInitializer* constructorInitializer, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // DQ (6/4/2011): This handles the case of both the declaration being a SgMemberFunctionDeclaration and a SgClassDeclaration.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     constructorInitializer->set_global_qualification_required(outputGlobalQualification);
     constructorInitializer->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     constructorInitializer->set_type_elaboration_required(outputTypeEvaluation);

     printf ("In HiddenListTraversal::setNameQualification(): constructorInitializer->get_name_qualification_length()     = %d \n",constructorInitializer->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): constructorInitializer->get_type_elaboration_required()     = %s \n",constructorInitializer->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): constructorInitializer->get_global_qualification_required() = %s \n",constructorInitializer->get_global_qualification_required() ? "true" : "false");

     if (qualifiedNameMapForNames.find(constructorInitializer) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),constructorInitializer,constructorInitializer->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(constructorInitializer,qualifier));

        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }

  // DQ (6/4/2011): Added test...
     ROSE_ASSERT(SgNode::get_globalQualifiedNameMapForNames().find(constructorInitializer) != SgNode::get_globalQualifiedNameMapForNames().end());
   }


void
HiddenListTraversal::setNameQualification(SgEnumVal* enumVal, SgEnumDeclaration* enumDeclaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(enumDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     enumVal->set_global_qualification_required(outputGlobalQualification);
     enumVal->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     enumVal->set_type_elaboration_required(outputTypeEvaluation);

     printf ("In HiddenListTraversal::setNameQualification(): enumVal->get_name_qualification_length()     = %d \n",enumVal->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): enumVal->get_type_elaboration_required()     = %s \n",enumVal->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): enumVal->get_global_qualification_required() = %s \n",enumVal->get_global_qualification_required() ? "true" : "false");

     if (qualifiedNameMapForNames.find(enumVal) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),enumVal,enumVal->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(enumVal,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }


void
HiddenListTraversal::setNameQualification ( SgBaseClass* baseClass, SgClassDeclaration* classDeclaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(classDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

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

     if (qualifiedNameMapForNames.find(baseClass) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),baseClass,baseClass->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(baseClass,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }


void
HiddenListTraversal::setNameQualification ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired )
   {
  // This takes only a SgFunctionDeclaration since it is where we locate the name qualification information AND
  // is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

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

     printf ("In HiddenListTraversal::setNameQualification(): functionDeclaration = %p firstNondefiningDeclaration() = %p \n",functionDeclaration,functionDeclaration->get_firstNondefiningDeclaration());
     printf ("In HiddenListTraversal::setNameQualification(): functionDeclaration = %p definingDeclaration()         = %p \n",functionDeclaration,functionDeclaration->get_definingDeclaration());
#endif

     if (qualifiedNameMapForNames.find(functionDeclaration) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(functionDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(functionDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());

       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if 1
               printf ("Error: name in qualifiedNameMapForNames already exists and is different... \n");
               ROSE_ASSERT(false);
#endif
             }
#if 0
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
#endif
        }
   }

// void HiddenListTraversal::setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired )
void
HiddenListTraversal::setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired ) 
   {
  // This takes only a SgFunctionDeclaration since it is where we locate the name qualification information AND
  // is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     functionDeclaration->set_global_qualification_required_for_return_type(outputGlobalQualification);
     functionDeclaration->set_name_qualification_length_for_return_type(outputNameQualificationLength);
     functionDeclaration->set_type_elaboration_required_for_return_type(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): functionDeclaration->get_name_qualification_length_for_return_type()     = %d \n",functionDeclaration->get_name_qualification_length_for_return_type());
     printf ("In HiddenListTraversal::setNameQualification(): functionDeclaration->get_type_elaboration_required_for_return_type()     = %s \n",functionDeclaration->get_type_elaboration_required_for_return_type() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): functionDeclaration->get_global_qualification_required_for_return_type() = %s \n",functionDeclaration->get_global_qualification_required_for_return_type() ? "true" : "false");
#endif

     if (qualifiedNameMapForTypes.find(functionDeclaration) == qualifiedNameMapForTypes.end())
        {
          printf ("Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(functionDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(functionDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());

       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if 1
               printf ("Error: name in qualifiedNameMapForNames already exists and is different... \n");
               ROSE_ASSERT(false);
#endif
             }
#if 0
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
#endif
        }
   }


void
HiddenListTraversal::setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

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

     if (qualifiedNameMapForNames.find(usingDeclaration) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDeclaration,usingDeclaration->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(usingDeclaration,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }


void
HiddenListTraversal::setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgInitializedName* associatedInitializedName, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(associatedInitializedName->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

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

     if (qualifiedNameMapForNames.find(usingDeclaration) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDeclaration,usingDeclaration->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(usingDeclaration,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }

void
HiddenListTraversal::setNameQualification ( SgUsingDirectiveStatement* usingDirective, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

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

     if (qualifiedNameMapForNames.find(usingDirective) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDirective,usingDirective->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(usingDirective,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }


// void HiddenListTraversal::setNameQualification(SgInitializedName* initializedName,SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
void
HiddenListTraversal::setNameQualification(SgInitializedName* initializedName,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // This is used to set the name qualification on the return type referenced by the SgInitializedName, and not on the SgInitializedName IR node itself.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

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

     if (qualifiedNameMapForTypes.find(initializedName) == qualifiedNameMapForTypes.end())
        {
          printf ("Inserting qualifier for type = %s into list at SgInitializedName IR node = %p = %s \n",qualifier.c_str(),initializedName,initializedName->get_name().str());
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(initializedName,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(initializedName);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());

       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if 1
               printf ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
            // ROSE_ASSERT(false);
#endif
             }
#if 0
          printf ("Error: name in qualifiedNameMapForTypes already exists... \n");
          ROSE_ASSERT(false);
#endif
        }
   }

void
HiddenListTraversal::setNameQualification(SgVariableDeclaration* variableDeclaration,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // This is used to set the name qualification on the associated SgInitializedName (there is only one per SgVariableDeclaration at present, but this may be changed (fixed) in the future.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

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

     if (qualifiedNameMapForNames.find(variableDeclaration) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at SgVariableDeclaration IR node = %p = %s \n",qualifier.c_str(),variableDeclaration,variableDeclaration->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(variableDeclaration,qualifier));
        }
       else
        {
#if 1
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
#endif
        }
   }

void
HiddenListTraversal::setNameQualification(SgTypedefDeclaration* typedefDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

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

     if (qualifiedNameMapForTypes.find(typedefDeclaration) == qualifiedNameMapForTypes.end())
        {
          printf ("Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),typedefDeclaration,typedefDeclaration->class_name().c_str());
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(typedefDeclaration,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForTypes already exists... \n");
          ROSE_ASSERT(false);
        }
   }


void
HiddenListTraversal::setNameQualification(SgTemplateArgument* templateArgument, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // This function will generate the qualified name prefix (without the name of the declaration) and add it to 
  // the map of name qualification strings referenced via the IR node that references the SgTemplateArgument.

  // DQ (6/1/2011): Note that the name qualification could be more complex than this function presently supports. 
  // The use of derivation can permit there to be multiple legal qualified names for a single construct.  There 
  // could also be some qualified names using using type names that are private or protected and thus can only 
  // be used in restricted contexts.  This sumbject of multiple qualified names or selecting amongst them for 
  // where each may be used is not handled presently.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
  // These may not be important under the newest version of name qualification that uses the qualified 
  // name string map to IR nodes that reference the construct using the name qualification.
     templateArgument->set_global_qualification_required(outputGlobalQualification);
     templateArgument->set_name_qualification_length(outputNameQualificationLength);
     templateArgument->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): templateArgument                                      = %p \n",templateArgument);
     printf ("In HiddenListTraversal::setNameQualification(): templateArgument->get_name_qualification_length()     = %d \n",templateArgument->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): templateArgument->get_type_elaboration_required()     = %s \n",templateArgument->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): templateArgument->get_global_qualification_required() = %s \n",templateArgument->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForTypes.find(templateArgument) == qualifiedNameMapForTypes.end())
        {
          printf ("Inserting qualifier for name or type = %s into list at IR node = %p = %s \n",qualifier.c_str(),templateArgument,templateArgument->class_name().c_str());
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(templateArgument,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(templateArgument);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());

       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if 1
               printf ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
            // ROSE_ASSERT(false);
#endif

               SgName testNameInMap = templateArgument->get_qualified_name_prefix();
               printf ("testNameInMap = %s \n",testNameInMap.str());
             }
        }
   }

// void HiddenListTraversal::setNameQualification(SgCastExp* castExp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired)
void
HiddenListTraversal::setNameQualification(SgExpression* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // DQ (6/4/2011): This should not be a SgConstructorInitializer since that uses the qualifiedNameMapForNames instead of the qualifiedNameMapForTypes.
     ROSE_ASSERT(isSgConstructorInitializer(exp) == NULL);

     string qualifier = setNameQualificationSupport(typeDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     exp->set_global_qualification_required(outputGlobalQualification);
     exp->set_name_qualification_length(outputNameQualificationLength);

  // DQ (6/2/2011): I think that type elaboration could be required for casts, but I am not certain.
     exp->set_type_elaboration_required(outputTypeEvaluation);

     printf ("In HiddenListTraversal::setNameQualification(): exp->get_name_qualification_length()     = %d \n",exp->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): exp->get_type_elaboration_required()     = %s \n",exp->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): exp->get_global_qualification_required() = %s \n",exp->get_global_qualification_required() ? "true" : "false");

     if (qualifiedNameMapForTypes.find(exp) == qualifiedNameMapForTypes.end())
        {
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),exp,exp->class_name().c_str());
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(exp,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForTypes already exists... \n");
          ROSE_ASSERT(false);
        }
   }


void
HiddenListTraversal::setNameQualification(SgClassDeclaration* classDeclaration, int amountOfNameQualificationRequired)
   {
  // This is used to set the name qualification on the associated SgClassDeclaration.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(classDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

#if 1
     classDeclaration->set_global_qualification_required(outputGlobalQualification);
     classDeclaration->set_name_qualification_length(outputNameQualificationLength);
     classDeclaration->set_type_elaboration_required(outputTypeEvaluation);
#endif

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
#if 1
     printf ("In HiddenListTraversal::setNameQualification(): classDeclaration->get_name_qualification_length()     = %d \n",classDeclaration->get_name_qualification_length());
     printf ("In HiddenListTraversal::setNameQualification(): classDeclaration->get_type_elaboration_required()     = %s \n",classDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In HiddenListTraversal::setNameQualification(): classDeclaration->get_global_qualification_required() = %s \n",classDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(classDeclaration) == qualifiedNameMapForNames.end())
        {
          printf ("Inserting qualifier for name = %s into list at SgClassDeclaration IR node = %p = %s \n",qualifier.c_str(),classDeclaration,classDeclaration->class_name().c_str());
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(classDeclaration,qualifier));
        }
       else
        {
#if 1
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
#endif
        }
   }


string
HiddenListTraversal::setNameQualificationSupport(SgScopeStatement* scope, const int inputNameQualificationLength, int & output_amountOfNameQualificationRequired , bool & outputGlobalQualification, bool & outputTypeEvaluation )
   {
  // This is lower level support for the different overloaded setNameQualification() functions.
  // This function builds up the qualified name as a string and then returns it to be used in 
  // either the map to names or the map to types (two different hash maps).
     string qualifierString;

     output_amountOfNameQualificationRequired = inputNameQualificationLength;
     outputGlobalQualification                = false;
     outputTypeEvaluation                     = false;

     printf ("In HiddenListTraversal::setNameQualificationSupport(): scope = %p = %s = %s inputNameQualificationLength = %d \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str(),inputNameQualificationLength);

     for (int i = 0; i < inputNameQualificationLength; i++)
        {
          printf ("   --- In loop: scope = %p = %s = %s \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());

          string scope_name;

       // This requirement to visit the template arguments occurs for templaed functions and templated member functions as well.
          SgTemplateInstantiationDefn* templateClassDefinition = isSgTemplateInstantiationDefn(scope);
          if (templateClassDefinition != NULL)
             {
            // Need to investigate how to generate a better quality name.
               SgTemplateInstantiationDecl* templateClassDeclaration = isSgTemplateInstantiationDecl(templateClassDefinition->get_declaration());
               ROSE_ASSERT(templateClassDeclaration != NULL);
#if 0
            // This is the normalized name (without name qualification for internal template arguments)
               printf ("templateClassDeclaration->get_name()          = %s \n",templateClassDeclaration->get_name().str());

            // This is the name of the template (without and internal template arguments)
               printf ("templateClassDeclaration->get_templateName() = %s \n",templateClassDeclaration->get_templateName().str());
#endif
#if 1
               SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
               ROSE_ASSERT (unparseInfoPointer != NULL);
               unparseInfoPointer->set_outputCompilerGeneratedStatements();
#endif
#if 0
               string template_name_1 = templateClassDeclaration->unparseToString(unparseInfoPointer);
               printf ("template_name (unparseToString()) = %s \n",template_name_1.c_str());
#endif
#if 0
               string template_name_2 = templateClassDeclaration->get_templateArguments()->unparseToString(unparseInfoPointer);
               printf ("template_name (unparseToString()) = %s \n",template_name_1.c_str());
#endif
#if 0
               string template_name_2 = unparseDeclarationToString(templateClassDeclaration);
               printf ("template_name (unparseDeclarationToString()) = %s \n",template_name_2.c_str());
#endif
               templateClassDeclaration->get_file_info()->display("SgTemplateInstantiationDecl trying to generate the qualified name: debug");

               string template_name = templateClassDeclaration->get_templateName();
               template_name += "< ";
            // printf ("START: template_name = %s \n",template_name.c_str());
               SgTemplateArgumentPtrList & templateArgumentList = templateClassDeclaration->get_templateArguments();
               SgTemplateArgumentPtrList::iterator i = templateArgumentList.begin();
               while (i != templateArgumentList.end())
                  {
                    SgTemplateArgument* templateArgument = *i;
                    ROSE_ASSERT(templateArgument != NULL);
#if 1
                    string template_argument_name = globalUnparseToString(templateArgument,unparseInfoPointer);
                    printf ("template_argument_name (globalUnparseToString()) = %s \n",template_argument_name.c_str());

                    template_name += template_argument_name;
#else
                 // Since templates arguments only have references to types, we search the qualifiedNameMapForTypes Map.
                 // Note that values can also be template arguments, but they don't get qualification (unless it is an 
                 // enum field but lets worry about that later).
                    std::map<SgNode*,std::string>::iterator qualifiedNameMapIterator = qualifiedNameMapForTypes.find(*i);
                    ROSE_ASSERT (qualifiedNameMapIterator != qualifiedNameMapForTypes.end());

                    string template_argument_qualified_name = qualifiedNameMapIterator->second;
                    string template_argument_name = globalUnparseToString(templateArgument,unparseInfoPointer);
                    printf ("aaaaaa --- templateArgument = %p template_argument_qualified_name = %s template_argument_name = %s \n",templateArgument,template_argument_qualified_name.c_str(),template_argument_name.c_str());

                    template_name += template_argument_qualified_name + template_argument_name;
#endif
                    i++;

                    if (i != templateArgumentList.end())
                         template_name += ",";
                  }

               template_name += "> ";

               scope_name = template_name;

               printf ("NAME OF SCOPE: scope name -- template_name = %s \n",template_name.c_str());
             }
            else
             {
            // scope_name = scope->class_name().c_str();
               scope_name = SageInterface::get_name(scope).c_str();
             }

          SgGlobal* globalScope = isSgGlobal(scope);
          if (globalScope != NULL)
             {
            // If we have iterated beyond the number of nested scopes, then set the global 
            // qualification and reduce the name_qualification_length correspondingly by one.

               printf ("!!!!! We have iterated beyond the number of nested scopes: setting outputGlobalQualification == true \n");

               outputGlobalQualification = true;
               output_amountOfNameQualificationRequired = inputNameQualificationLength-1;

               scope_name = "::";
             }

          printf ("scope_name = %s \n",scope_name.c_str());

       // qualifierString = scope_name + "::" + qualifierString;
          if (outputGlobalQualification == true)
             {
            // Avoid out put of "::::" as substrings.
               qualifierString = "::" + qualifierString;
             }
            else
             {
            // qualifierString = scope_name + "::" + qualifierString;
               if (scope_name.length() == 0)
                  {
                 // Nothing to do for this case of an empty string for a scope name (see test2006_121.C).
                  }
                 else
                  {
                    qualifierString = scope_name + "::" + qualifierString;
                  }
             }

       // We have to loop over scopes that are not named scopes!
          scope = scope->get_scope();
        }

     printf ("In HiddenListTraversal::setNameQualificationSupport(): outputGlobalQualification = %s output_amountOfNameQualificationRequired = %d qualifierString = %s \n",outputGlobalQualification ? "true" : "false",output_amountOfNameQualificationRequired,qualifierString.c_str());

  // DQ (6/12/2011): Make sure we have not generated a qualified name with "::::" because of an scope translated to an empty name.
     ROSE_ASSERT(qualifierString.find("::::") == string::npos);

     return qualifierString;
   }


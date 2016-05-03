#include "sage3basic.h"

#include "Diagnostics.h"

#include "nameQualificationSupport.h"

using namespace std;

// DQ (3/24/2016): Adding Robb's message logging mechanism to contrl output debug message from the EDG/ROSE connection code.
using namespace rose::Diagnostics;

// DQ (3/24/2016): Adding Robb's meageage mechanism (data member and function).
Sawyer::Message::Facility NameQualificationTraversal::mlog;


#define DEBUG_NAME_QUALIFICATION_LEVEL 0

// ***********************************************************
// Main calling function to support name qualification support
// ***********************************************************

void
generateNameQualificationSupport( SgNode* node, std::set<SgNode*> & referencedNameSet )
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
  // the NameQualificationTraversal class.

     TimingPerformance timer ("Name qualification support:");

  // DQ (5/28/2011): Initialize the local maps to the static maps in SgNode.  This is requires so the
  // types used in template arguments can call the unparser to support there generation of name qualified 
  // nested types.

#if 0
     printf ("In generateNameQualificationSupport(): MangledNameSupport::visitedTemplateDefinitions.size() = %zu \n",MangledNameSupport::visitedTemplateDefinitions.size());
#endif

  // DQ (9/7/2014): Modified to handle template header map (for template declarations).
  // NameQualificationTraversal t(SgNode::get_globalQualifiedNameMapForNames(),SgNode::get_globalQualifiedNameMapForTypes(),SgNode::get_globalTypeNameMap(),referencedNameSet);
     NameQualificationTraversal t(SgNode::get_globalQualifiedNameMapForNames(),SgNode::get_globalQualifiedNameMapForTypes(),SgNode::get_globalQualifiedNameMapForTemplateHeaders(),SgNode::get_globalTypeNameMap(),referencedNameSet);

     NameQualificationInheritedAttribute ih;

#if 0
     printf ("Calling SageInterface::buildDeclarationSets(node = %p = %s) \n",node,node->class_name().c_str());
#endif

  // DQ (4/3/2014): Added assertion.
     t.declarationSet = SageInterface::buildDeclarationSets(node);
     ROSE_ASSERT(t.declarationSet != NULL);

#if 0
     printf ("DONE: Calling SageInterface::buildDeclarationSets(node = %p = %s) t.declarationSet = %p \n",node,node->class_name().c_str(),t.declarationSet);
#endif

  // Call the traversal.
     t.traverse(node,ih);
   }

void NameQualificationTraversal::initDiagnostics() 
   {
     static bool initialized = false;
     if (!initialized) 
        {
          initialized = true;
          mlog = Sawyer::Message::Facility("NameQualificationTraversal", rose::Diagnostics::destination);
          rose::Diagnostics::mfacilities.insertAndAdjust(mlog);
        }
   }

// DQ (7/23/2011): This function is only used locally.
// void generateNameQualificationSupportWithScope( SgNode* node, const NameQualificationTraversal & parentTraversal, SgScopeStatement* currentScope )
void
NameQualificationTraversal::generateNestedTraversalWithExplicitScope( SgNode* node, SgScopeStatement* input_currentScope )
   {
     ROSE_ASSERT(input_currentScope != NULL);

  // DQ (9/7/2014): Modified to handle template header map (for template declarations).
  // NameQualificationTraversal t(this->qualifiedNameMapForNames,this->qualifiedNameMapForTypes,this->typeNameMap,this->referencedNameSet);
     NameQualificationTraversal t(this->qualifiedNameMapForNames,this->qualifiedNameMapForTypes,this->qualifiedNameMapForTemplateHeaders,this->typeNameMap,this->referencedNameSet);

     t.explictlySpecifiedCurrentScope = input_currentScope;

  // DQ (4/7/2014): Set this explicitly using the one already built.
     ROSE_ASSERT(declarationSet != NULL);
     t.declarationSet = declarationSet;
     ROSE_ASSERT(t.declarationSet != NULL);

     NameQualificationInheritedAttribute ih;

  // DQ (4/3/2014): Added assertion.
     ROSE_ASSERT(declarationSet != NULL);

  // This fails for test2001_02.C.
  // ROSE_ASSERT(declarationSet == NULL);

  // DQ (5/24/2013): Added scope to inherited attribute.
     ih.set_currentScope(input_currentScope);

  // Call the traversal.
     t.traverse(node,ih);
   }


// *******************
// Inherited Attribute
// *******************

NameQualificationInheritedAttribute::NameQualificationInheritedAttribute()
   {
  // Default constructor

  // DQ (5/24/2013): Allow the current scope to be tracked from the traversal of the AST
  // instead of being computed at each IR node which is a problem for template arguments.
  // See test2013_187.C for an example of this.
     currentScope = NULL;
   }

NameQualificationInheritedAttribute::NameQualificationInheritedAttribute ( const NameQualificationInheritedAttribute & X )
   {
  // Copy constructor.

  // DQ (5/24/2013): Allow the current scope to be tracked from the traversal of the AST
  // instead of being computed at each IR node which is a problem for template arguments.
  // See test2013_187.C for an example of this.
     currentScope = X.currentScope;

#if 0
     printf ("In NameQualificationInheritedAttribute(): copy constructor: currentScope = %p = %s \n",currentScope,currentScope != NULL ? currentScope->class_name().c_str() : "NULL");
#endif
   }

SgScopeStatement* NameQualificationInheritedAttribute::get_currentScope()
   {
     return currentScope;
   }

void NameQualificationInheritedAttribute::set_currentScope(SgScopeStatement* scope)
   {
     currentScope = scope;
   }


// *********************
// Synthesized Attribute
// *********************

NameQualificationSynthesizedAttribute::NameQualificationSynthesizedAttribute()
   {
  // Default constructor
   }


NameQualificationSynthesizedAttribute::NameQualificationSynthesizedAttribute ( const NameQualificationSynthesizedAttribute & X )
   {
  // Copy constructor.
   }


// *******************
// NameQualificationTraversal
// *******************

NameQualificationTraversal::NameQualificationTraversal(
     std::map<SgNode*,std::string> & input_qualifiedNameMapForNames, 
     std::map<SgNode*,std::string> & input_qualifiedNameMapForTypes,
     std::map<SgNode*,std::string> & input_qualifiedNameMapForTemplateHeaders,
     std::map<SgNode*,std::string> & input_typeNameMap, 
     std::set<SgNode*> & input_referencedNameSet)
   : referencedNameSet(input_referencedNameSet),
     qualifiedNameMapForNames(input_qualifiedNameMapForNames),
     qualifiedNameMapForTypes(input_qualifiedNameMapForTypes),
     qualifiedNameMapForTemplateHeaders(input_qualifiedNameMapForTemplateHeaders),
     typeNameMap(input_typeNameMap)
   {
  // Nothing to do here.

     explictlySpecifiedCurrentScope = NULL;

     declarationSet = NULL;
   }


// DQ (5/28/2011): Added support to set the static global qualified name map in SgNode.
const std::map<SgNode*,std::string> &
NameQualificationTraversal::get_qualifiedNameMapForNames() const
   {
     return qualifiedNameMapForNames;
   }

// DQ (5/28/2011): Added support to set the static global qualified name map in SgNode.
const std::map<SgNode*,std::string> &
NameQualificationTraversal::get_qualifiedNameMapForTypes() const
   {
     return qualifiedNameMapForTypes;
   }

// DQ (9/7/2014): Added support to set the template headers in template declarations.
const std::map<SgNode*,std::string> &
NameQualificationTraversal::get_qualifiedNameMapForTemplateHeaders() const
   {
     return qualifiedNameMapForTemplateHeaders;
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In numberOfSymbolsWithName(): symbol = %p scope = %p = %s \n",symbol,scope,scope->class_name().c_str());
#endif

     while (symbol != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("     In loop: symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
          count++;
          symbol = scope->next_any_symbol();
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In numberOfSymbolsWithName(): count = %d \n",count);
#endif

     return count;
   }

SgDeclarationStatement*
NameQualificationTraversal::associatedDeclaration(SgScopeStatement* scope)
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

       // DQ (11/20/2011): Added support for template declarations (template class declarations)
          case V_SgTemplateClassDefinition:
             {
               SgTemplateClassDefinition* definition = isSgTemplateClassDefinition(scope);
               ROSE_ASSERT(definition != NULL);

               SgTemplateClassDeclaration* declaration = definition->get_declaration();
               ROSE_ASSERT(declaration != NULL);

               return_declaration = declaration;
               break;
             }

          case V_SgNamespaceDefinitionStatement:
             {
               SgNamespaceDefinitionStatement* definition = isSgNamespaceDefinitionStatement(scope);
               ROSE_ASSERT(definition != NULL);

            // Let the first definition be used to get the associated first declaration so that we 
            // are always refering to a consistant declaration for any chain of namespaces.  If not
            // the first then perhaps the last?
               while (definition->get_previousNamespaceDefinition() != NULL)
                  {
                 // printf ("Iterating through the namespace chain... \n");
                    definition = definition->get_previousNamespaceDefinition();
                  }

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

       // DQ (7/11/2014): Added this case to support test2014_84.C.
          case V_SgFunctionDefinition:
             {
               SgFunctionDefinition* definition = isSgFunctionDefinition(scope);
               ROSE_ASSERT(definition != NULL);

               SgFunctionDeclaration* declaration = isSgFunctionDeclaration(definition->get_declaration());
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
               printf ("Default reached in NameQualificationTraversal::associatedDeclaration() scope = %s \n",scope->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

     return return_declaration;
   }

SgDeclarationStatement*
NameQualificationTraversal::associatedDeclaration(SgType* type)
   {
     SgDeclarationStatement* return_declaration = NULL;

  // DQ (1/26/2013): Added assertion.
     ROSE_ASSERT(type != NULL);

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

       // DQ (11/20/2011): Adding support for template declarations in the AST.
          case V_SgTemplateType:
             {
               SgTemplateType* templateType = isSgTemplateType(strippedType);
               ROSE_ASSERT(templateType != NULL);
#if 0
               SgTemplateDeclaration* declaration = isSgTemplateDeclaration(templateType->get_declaration());
               ROSE_ASSERT(declaration != NULL);

               return_declaration = declaration;
#else
               return_declaration = NULL;
#endif
               break;
             }

          case V_SgFunctionType:
          case V_SgMemberFunctionType:
             {
            // Not clear if I have to resolve declarations associated with function types.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 0)
               printf ("In NameQualificationTraversal::associatedDeclaration(): Case of SgFunctionType not implemented strippedType = %s \n",strippedType->class_name().c_str());
#endif
               return_declaration = NULL;
               break;
             }

       // DQ (6/25/2011): Demonstrated by calling unparseToString on all possible type.
          case V_SgTypeDefault:

       // Some scopes don't have an associated declaration (return NULL in these cases).
       // Also missing some of the Fortran specific scopes.
          case V_SgTypeInt:
          case V_SgTypeUnsignedLong:
          case V_SgTypeUnsignedLongLong:
          case V_SgTypeUnsignedChar:
          case V_SgTypeUnsignedShort:
          case V_SgTypeUnsignedInt:
          case V_SgTypeSignedChar:
          case V_SgTypeSignedShort:
          case V_SgTypeSignedInt:
          case V_SgTypeSignedLong:
          case V_SgTypeSignedLongLong:

       // DQ (11/6/2014): Added support for C++11 rvalue references.
          case V_SgRvalueReferenceType:

       // DQ (3/24/2014): Added support for 128-bit integers.
          case V_SgTypeSigned128bitInteger:
          case V_SgTypeUnsigned128bitInteger:

       // DQ (7/30/2014): Adding C++11 support.
          case V_SgTypeNullptr:

       // DQ (8/12/2014): Adding C++11 support.
          case V_SgDeclType:

       // DQ (3/28/2015): Adding GNU C language extension.
          case V_SgTypeOfType:

       // DQ (4/29/2016): Added support for complex types.
          case V_SgTypeComplex:

          case V_SgTypeShort:
          case V_SgTypeLong:
          case V_SgTypeLongLong:
          case V_SgTypeVoid:
          case V_SgTypeChar:
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
               printf ("Default reached in NameQualificationTraversal::associatedDeclaration() type = %s strippedType = %s \n",type->class_name().c_str(),strippedType->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

     return return_declaration;
   }


void
NameQualificationTraversal::evaluateTemplateInstantiationDeclaration ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // DQ (9/23/2012): Added assertions.
     ROSE_ASSERT(declaration       != NULL);
     ROSE_ASSERT(currentScope      != NULL);
     ROSE_ASSERT(positionStatement != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In evaluateTemplateInstantiationDeclaration(): declaration = %p = %s currentScope = %p = %s positionStatement = %p = %s \n",
          declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),positionStatement,positionStatement->class_name().c_str());
#endif

  // DQ (10/31/2015): This code is designed to eliminate the infinite recursion possible in some rare cases of 
  // template instantiation (see test2015_105.C extracted from ROSE compiling ROSE header files and the boost 
  // usage present there).  Note that this could be restricted to the handling of SgTemplateInstantiationDecl 
  // instead (I think).  But it might be that I have just not yet seen a recursive case using template functions 
  // instantiations, template member function instantiations and template variable instantiations.
     SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
     SgClassDefinition* nonconst_def = templateInstantiationDeclaration != NULL ? isSgClassDefinition(templateInstantiationDeclaration->get_definition()) : NULL;
     if (MangledNameSupport::visitedTemplateDefinitions.find(nonconst_def) != MangledNameSupport::visitedTemplateDefinitions.end())
        {
       // Skip the call that would result in infinte recursion.
#if 0
          printf ("In nameQualificationSupport.C: evaluateTemplateInstantiationDeclaration(): skipping the call to process the template class instantiation definition: def = %p = %s \n",declaration,declaration->class_name().c_str()); 
#endif
        }
       else
        {
       // Only handle the case of a SgTemplateInstantiationDecl.
          SgClassDefinition* templateInstantiationDefinition = isSgTemplateInstantiationDefn(nonconst_def);
          if (templateInstantiationDefinition != NULL)
             {
#if 0
               printf ("Insert templateInstantiationDefinition = %p into visitedTemplateDeclarations (stl set)\n",templateInstantiationDefinition);
#endif
            // Not clear why we need to use an iterator to simply insert a pointer into the set.
            // SgTemplateInstantiationDefn* nonconst_templateInstantiationDefinition = const_cast<SgTemplateInstantiationDefn*>(templateInstantiationDefinition);
               MangledNameSupport::setType::iterator it = MangledNameSupport::visitedTemplateDefinitions.begin();
            // MangledNameSupport::visitedTemplateDeclarations.insert(it,nonconst_templateInstantiationDefinition);
               MangledNameSupport::visitedTemplateDefinitions.insert(it,nonconst_def);
             }

       // DQ (11/1/2015): Indented this code (switch statement) to conform to new block layout.
       // DQ (6/1/2011): Added support for template arguments.
          switch (declaration->variantT())
             {
               case V_SgTemplateInstantiationDecl:
                  {
                    SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
                    ROSE_ASSERT(templateInstantiationDeclaration != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                 // printf ("$$$$$$$$$ --- templateInstantiationDeclaration = %p \n",templateInstantiationDeclaration);
                    printf ("$$$$$$$$$ --- templateInstantiationDeclaration = %p templateInstantiationDeclaration->get_templateArguments().size() = %" PRIuPTR " \n",templateInstantiationDeclaration,templateInstantiationDeclaration->get_templateArguments().size());
#endif
                 // Evaluate all template arguments.
                    evaluateNameQualificationForTemplateArgumentList (templateInstantiationDeclaration->get_templateArguments(),currentScope,positionStatement);
                    break;
                  }

               case V_SgTemplateInstantiationFunctionDecl:
                  {
                    SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(declaration);
                    ROSE_ASSERT(templateInstantiationFunctionDeclaration != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("$$$$$$$$$ --- templateInstantiationFunctionDeclaration = %p \n",templateInstantiationFunctionDeclaration);
#endif
                 // Evaluate all template arguments.
                    evaluateNameQualificationForTemplateArgumentList (templateInstantiationFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
                    break;
                  }

               case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
                    SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(declaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("$$$$$$$$$ --- templateInstantiationMemberFunctionDeclaration = %p \n",templateInstantiationMemberFunctionDeclaration);
#endif
                    ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration != NULL);

                 // Evaluate all template arguments.
                 // evaluateNameQualificationForTemplateArgumentList (templateInstantiationMemberFunctionDeclaration->get_templateArguments(),currentScope,positionStatement);
                    SgTemplateArgumentPtrList & templateArgumentList = templateInstantiationMemberFunctionDeclaration->get_templateArguments();
                    evaluateNameQualificationForTemplateArgumentList(templateArgumentList,currentScope,positionStatement);
                    break;
                  }

               default:
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("This IR node does not contain template arguments to process: declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
                  }
             }

       // DQ (10/31/2015): The rule here is that after processing as a mangled name we remove the 
       // template instantiation from the list so that other non-nested uses of the template 
       // instantiation will force the manged name to be generated.
          if (templateInstantiationDefinition != NULL)
             {
#if 0
               printf ("Erase templateInstantiationDefinition = %p from visitedTemplateDeclarations (stl set)\n",templateInstantiationDefinition);
#endif
               MangledNameSupport::visitedTemplateDefinitions.erase(nonconst_def);
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("Leaving evaluateTemplateInstantiationDeclaration(): declaration = %p = %s currentScope = %p = %s positionStatement = %p = %s \n",
          declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),positionStatement,positionStatement->class_name().c_str());
#endif
   }


int
NameQualificationTraversal::nameQualificationDepthOfParent(SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // Now resolve how much name qualification is required.
     int qualificationDepth = 0;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("***** Inside of NameQualificationTraversal::nameQualificationDepthOfParent() ***** \n");
     printf ("   declaration  = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
     printf ("   currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
#endif

  // qualificationDepth++;
  // SgDeclaration* classOrNamespaceDefinition = classDefinition->get_declaration()->get_scope();
     SgScopeStatement* parentScope = declaration->get_scope();
  // SgName parentName = associatedName(parentScope);

  // qualificationDepth = nameQualificationDepth(parentName,parentScope,positionStatement) + 1;
     SgGlobal* globalScope = isSgGlobal(parentScope);
     if (globalScope != NULL)
        {
       // There is no declaration associated with global scope so we have to process the case of a null pointer...
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 1)
          printf ("parentDeclaration == NULL: parentScope = %p = %s \n",parentScope,parentScope->class_name().c_str());
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("In NameQualificationTraversal::nameQualificationDepthOfParent(): calling nameQualificationDepth(): parentDeclaration = %p = %s = %s \n",
                    parentDeclaration,parentDeclaration->class_name().c_str(),SageInterface::get_name(parentDeclaration).c_str());
               printf ("   --- currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
            // qualificationDepth = nameQualificationDepth(parentDeclaration,parentScope,positionStatement);
               qualificationDepth = nameQualificationDepth(parentDeclaration,currentScope,positionStatement);
             }
        }

     return qualificationDepth;
   }

bool
NameQualificationTraversal::requiresTypeElaboration(SgSymbol* symbol)
   {
  // DQ (5/14/2011): type elaboration only works between non-types and types.  Different types must be distinquished using name qualification.
  // If this is a type then since all types are given equal weight we need more name qualification to distinquish them.
  // However, if this is a non-type then type elaboration is sufficent to distinquish the type (e.g. from a variable name).
     bool typeElaborationRequired = false;

     ROSE_ASSERT(symbol != NULL);
     switch (symbol->variantT())
        {
       // DQ (7/23/2011): Class elaboration can be required....
          case V_SgClassSymbol:

       // DQ (2/25/2012): Added support for SgTemplateClassSymbol.
          case V_SgTemplateClassSymbol:

       // DQ (2/12/2012): Added support for SgTemplateMemberFunctionSymbol.
          case V_SgTemplateMemberFunctionSymbol:

       // DQ (2/12/2013): Added support for SgTemplateFunctionSymbol.
          case V_SgTemplateFunctionSymbol:

       // DQ (6/21/2011): Added case for SgFunctionSymbol (triggers type elaboration).
          case V_SgFunctionSymbol:
          case V_SgMemberFunctionSymbol:
          case V_SgVariableSymbol:
             {
               typeElaborationRequired = true;
               break;
             }

       // DQ (3/31/2013): We need an example of this before I allow it (I don't think type elaboration is required here).
          case V_SgEnumFieldSymbol:

       // DQ (6/22/2011): Added case for SgEnumSymbol (see test2011_95.C)
          case V_SgEnumSymbol: 
          case V_SgNamespaceSymbol: // Note sure about this!!!
          case V_SgTemplateSymbol: // Note sure about this!!!
          case V_SgTypedefSymbol:
             {
               typeElaborationRequired = false;
               break;
             }

       // DQ (9/21/2011): Added support for alias symbol (recursive call).
          case V_SgAliasSymbol:
             {
               SgAliasSymbol* alias = isSgAliasSymbol(symbol);
               ROSE_ASSERT(alias != NULL);

            // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
               ROSE_ASSERT(false);

               SgSymbol* baseSymbol = alias->get_alias();
               ROSE_ASSERT(baseSymbol != NULL);
#if 1
               printf ("NameQualificationTraversal::requiresTypeElaboration(): Detected a SgAliasSymbol: alias = %p baseSymbol = %p = %s \n",alias,baseSymbol,baseSymbol->class_name().c_str());
#endif
               typeElaborationRequired = requiresTypeElaboration(baseSymbol);
               break;
             }

          default:
             {
               printf ("Default reached in NameQualificationTraversal::requiresTypeElaboration(): symbol = %p = %s \n",symbol,symbol->class_name().c_str());
               ROSE_ASSERT(false);
             }           
        }

     return typeElaborationRequired;
   }


void
NameQualificationTraversal::processNameQualificationArrayType(SgArrayType* arrayType, SgScopeStatement* currentScope)
   {
  // Note that we may have to traverse base types in case they include other SgArrayType IR nodes where their index requires name qualification.
  // SgType* strippedArrayType = arrayType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE);
  // ROSE_ASSERT(strippedArrayType != NULL);

     SgExpression* index = arrayType->get_index();
     if (index != NULL)
        {
#if 0
          printf ("~~~~~ Handling case of SgArrayType: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
          printf ("~~~~~ Handling case of SgArrayType: index        = %p = %s \n",index,index->class_name().c_str());
#endif
       // DQ (7/23/2011): This will not work since the current scope is not know and can't be determined from the type (which is shared).
       // printf ("Support name qualification on the array index expression if required (recursive call) \n");
       // generateNameQualificationSupport(index,referencedNameSet);
       // generateNameQualificationSupportWithScope(index,this,currentScope);
          ROSE_ASSERT(currentScope != NULL);
          generateNestedTraversalWithExplicitScope(index,currentScope);
#if 0
          printf ("DONE: Support name qualification on the array index expression if required (recursive call) \n");
#endif
       // DQ (8/21/2014): It appears that test2014_137.C is not being caught by this trap.
          SgVarRefExp* varRefExp = isSgVarRefExp(index);
          if (varRefExp != NULL)
             {
#if 0
               printf ("ERROR: Case of SgArrayType with a SgVarRefExp index (exiting as a test) \n");
               ROSE_ASSERT(false);
#endif
             }
        }
#if 0
     printf ("ERROR: Case of SgArrayType not supported yet! \n");
     ROSE_ASSERT(false);
#endif
   }

void
NameQualificationTraversal::processNameQualificationForPossibleArrayType(SgType* possibleArrayType, SgScopeStatement* currentScope)
   {
  // DQ (7/23/2011): Refactored support for name qualification of the index expressions used in array types.

     SgType* strippedPossibleArrayType = possibleArrayType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE);
     ROSE_ASSERT(strippedPossibleArrayType != NULL);
     SgArrayType* arrayType = isSgArrayType(strippedPossibleArrayType);
     if (arrayType != NULL)
        {
          processNameQualificationArrayType(arrayType,currentScope);

       // Now process the base type, since it might be part of a multi-dimentional array type (in C/C++ these are a chain of array types).
          processNameQualificationForPossibleArrayType(arrayType->get_base_type(),currentScope);
        }
   }


// int NameQualificationTraversal::nameQualificationDepth ( SgScopeStatement* classOrNamespaceDefinition )
int 
NameQualificationTraversal::nameQualificationDepth ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement, bool forceMoreNameQualification )
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

  // DQ (6/22/2011): Assert this as a preliminary step to its removal.
     ROSE_ASSERT(forceMoreNameQualification == false);

  // DQ (4/4/2014): Added assertion.
     ROSE_ASSERT(positionStatement != NULL);

     int  qualificationDepth        = 0;
     bool typeElaborationIsRequired = false;
  // bool globalQualifierIsRequired = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("##### Inside of NameQualificationTraversal::nameQualificationDepth() ##### \n");

  // The use of SageInterface::generateUniqueName() can cause the unparser to be called and triggers the name 
  // qualification recursively but only for template declaration (SgTemplateInstantiationDecl, I think).
  // printf ("declaration  = %p = %s = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str(),SageInterface::generateUniqueName(declaration,true).c_str());
     printf ("   --- declaration  = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
     declaration->get_startOfConstruct()->display("declaration");
     printf ("   --- currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
     currentScope->get_startOfConstruct()->display("currentScope");
     printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
     positionStatement->get_startOfConstruct()->display("positionStatement");
#endif

     SgClassDeclaration*              classDeclaration     = isSgClassDeclaration(declaration);
     SgVariableDeclaration*           variableDeclaration  = isSgVariableDeclaration(declaration);
     SgFunctionDeclaration*           functionDeclaration  = isSgFunctionDeclaration(declaration);
     SgTypedefDeclaration*            typedefDeclaration   = isSgTypedefDeclaration(declaration);
     SgTemplateDeclaration*           templateDeclaration  = isSgTemplateDeclaration(declaration);
     SgEnumDeclaration*               enumDeclaration      = isSgEnumDeclaration(declaration);
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In nameQualificationDepth(SgDeclarationStatement*,...): declaration = %p = %s name = %s \n",declaration,declaration->class_name().c_str(),name.str());
#endif

#if 0
  // ROSE_ASSERT(name.is_null() == false);

  // DQ (8/31/2014): The handling of this as a special case is funtamentally the problem with the test code (test2014_183.C)
  // which demonstrates a namespace nested in a namespace with the same name (when this happens it is likely a bug, however
  // it is legal C++ code and ROSE was having problem with this that sort of language construct and the issue is trased to
  // the handling of un-named constructs in this code below. 
     if (name.is_null() == true)
        {
       // This name is empty so we need to keep going to evaluate the qualified name (see test2006_121.C).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("CASE OF EMPTY NAME: declaration = %p = %s but has no valid name (it has and empty name), thus we have to recurse to the next level \n",declaration,declaration->class_name().c_str());
          printf ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
          printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif

       // DQ (8/20/2014): Add support for un-named namespaces (see test2014_132.C for an example).
       // qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
          if (namespaceDeclaration != NULL)
             {
            // I am clearer on the case that for a namespace declaration we do not want to increment the qualificationDepth.
               qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 0;
             }
            else
             {
               printf ("NEW CASE OF EMPTY NAME: declaration = %p = %s but has no valid name (it has and empty name), thus we add 1 to the qualificationDepth and have to recurse to the next level \n",declaration,declaration->class_name().c_str());

               qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
             }
        }
#else
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In nameQualificationDepth(SgDeclarationStatement*,...): Skipping special handling of un-named constructs (not required): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
#endif

  // DQ (8/16/2013): Build the template parameters and template arguments as appropriate (will be NULL pointers for some types of declarations).
     SgTemplateParameterPtrList* templateParameterList = SageBuilder::getTemplateParameterList(declaration);
     SgTemplateArgumentPtrList*  templateArgumentList  = SageBuilder::getTemplateArgumentList(declaration);

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
          ROSE_ASSERT(currentScope != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Initial lookup: name = %s currentScope = %p = %s \n",name.str(),currentScope,currentScope->class_name().c_str());
#endif

       // DQ (8/16/2013): Added support for moe precise symbol lookup (which requires the template parameters and template arguments).
       // DQ 8/21/2012): this is looking in the parent scopes of the currentScope and thus not including the currentScope.
       // This is a bug for test2011_31.C where there is a variable who's name hides the name in the parent scopes (and it not detected).
       // SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope);
          SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope,templateParameterList,templateArgumentList);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Initial lookup: symbol = %p = %s \n",symbol,(symbol != NULL) ? symbol->class_name().c_str() : "NULL");
#endif

       // This is used to count the number of symbols of the same type in a single scope.
       // size_t numberOfSymbols = 0;
       // bool forceMoreNameQualification = false;

       // DQ (4/12/2014): we need to record that there was another function identified in the parent scopes that we will want to have force name qualification.
          bool foundAnOverloadedFunctionWithSameName = false;
          bool foundAnOverloadedFunctionInSameScope  = false;

          if (symbol != NULL)
             {
            // printf ("Lookup symbol based on name only: symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Lookup symbol based on name only (via parents starting at currentScope = %p = %s: name = %s symbol = %p = %s) \n",currentScope,currentScope->class_name().c_str(),name.str(),symbol,symbol->class_name().c_str());
#endif
               SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);

            // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
               ROSE_ASSERT(aliasSymbol == NULL);

               if (aliasSymbol != NULL)
                  {
                    symbol = aliasSymbol->get_alias();
                    ROSE_ASSERT(symbol != NULL);

                 // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
                    ROSE_ASSERT(false);
#if 1
                    printf ("In NameQualificationTraversal::nameQualificationDepth(): Detected a SgAliasSymbol: alias = %p baseSymbol = %p = %s \n",aliasSymbol,symbol,symbol->class_name().c_str());
#endif
#if 1
                 // DQ (8/21/2012): Commented out the assertion, but warn about how nesting appears to be present.
                 // DQ (8/20/2012): The symbol in side the SgAliasSymbol should not be another alias. This fails for test2004_48.C).
                    if (isSgAliasSymbol(symbol) != NULL)
                       {
                      // DQ (8/24/2012): Allow this to be output a little less often.
                         static int counter = 0;
                         if (counter++ % 100 == 0)
                            {
                              printf ("WARNING: can't assert isSgAliasSymbol(symbol) == NULL after processing SgAliasSymbol (might require loop to strip away nested SgAliasSymbol symbols. \n");
                            }
                       }
                 // ROSE_ASSERT(isSgAliasSymbol(symbol) == NULL);
#endif
                  }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
            // We have to check the kind of declaration against the kind of symbol found. A local variable (for example) 
            // could hide the same name used for the declaration.  This if we find symbol inconsistant with the declaration 
            // then we need some form of qualification (sometimes just type elaboration).
               printf ("### Targeting a declaration = %p = %s \n",declaration,declaration->class_name().c_str());
               printf ("   --- declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
               printf ("   --- declaration->get_definingDeclaration()         = %p \n",declaration->get_definingDeclaration());
#endif
               switch (declaration->variantT())
                  {
                 // DQ (12/26/2011): Added support for template class declarations (part of new design for template declarations).
                    case V_SgTemplateClassDeclaration:
                    case V_SgClassDeclaration:
                       {
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                         ROSE_ASSERT(classDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("classDeclaration name = %s \n",classDeclaration->get_name().str());
#endif
                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                      // ROSE_ASSERT(classSymbol != NULL);
                         if (classSymbol == NULL)
                            {
                           // This is only type elaboration if it is a variable that is the conflict, if it is a typedef then more qualification is required. (see test2011_37.C).
                           // printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
                           // typeElaborationIsRequired = true;
                              if (requiresTypeElaboration(symbol) == true)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                                   typeElaborationIsRequired = true;
                                 }
                                else
                                 {
                                // I think we have to force an extra level of name qualification.
                                   forceMoreNameQualification = true;
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("Forcing an extra level of name qualification forceMoreNameQualification = %s \n",forceMoreNameQualification ? "true" : "false");
#endif
#if 0
                                   printf ("I think we have to force an extra level of name qualification (not implemented) \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                              
                           // DQ (8/16/2013): Modified API for symbol lookup.
                           // Reset the symbol to one that will match the declaration.
                           // symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope);
                              symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope,NULL);
                           // ROSE_ASSERT(symbol != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): classSymbol == NULL \n");
                                 }
#endif
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Symbol matches the class declaration (classDeclaration->get_firstNondefiningDeclaration()) classSymbol->get_declaration() = %p \n",classSymbol->get_declaration());
#endif
                           // DQ (6/9/2011): I would prefer to have this be true and it might work if it is not, but I would like to have this be a warning for now!
                           // ROSE_ASSERT(classDeclaration->get_firstNondefiningDeclaration() == classSymbol->get_declaration());
                              if (classDeclaration->get_firstNondefiningDeclaration() != classSymbol->get_declaration())
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("WARNING: classDeclaration->get_firstNondefiningDeclaration() != classSymbol->get_declaration() \n");
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): functionSymbol == NULL \n");
                                 }
#endif
                            }
                           else
                            {
                           // This is the typical case.
                            }
                         
                         break;
                       }

                 // DQ (12/28/2011): Added support for template functions and template member functions.
                    case V_SgTemplateFunctionDeclaration:
                    case V_SgTemplateMemberFunctionDeclaration:

                 // DQ (6/1/2011): Added case for SgTemplateInstantiationFunctionDecl.
                 // case V_SgTemplateInstantiationFunctionDecl:
                    case V_SgTemplateInstantiationMemberFunctionDecl:
                    case V_SgMemberFunctionDeclaration:
                    case V_SgFunctionDeclaration:
                       {
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
                         ROSE_ASSERT(functionDeclaration != NULL);

                         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("case function declaration: functionSymbol = %p \n",functionSymbol);
#endif
                         SgFunctionType* functionType = functionDeclaration->get_type();
                         ROSE_ASSERT(functionType != NULL);

                      // ROSE_ASSERT(classSymbol != NULL);
                         if (functionSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("WARNING: Present implementation of symbol table will not find alias sysmbols of SgFunctionSymbol \n");
#endif
                           // Reset the symbol to one that will match the declaration.
                           // DQ (4/12/2014): I think we need to use the version of the function that matches the function type.
                           // See test2014_42.C for an example of this.
                           // symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,currentScope);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("NOTE: we are now using the function type in the initial function symbol lookup? \n");
#endif
                              symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,functionType,currentScope);

                           // ROSE_ASSERT(symbol != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): functionSymbol == NULL \n");
                                 }
#endif
                            }
                           else
                            {
                           // DQ (4/12/2014): But is this the correct symbol for a function of the same type.
                           // See test2014_42.C for an example where this is an overloaded function declaration and the WRONG one.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("case function declaration: functionSymbol = %p: but is it associated with the correct type \n",functionSymbol);
#endif
                              SgFunctionType* functionTypeAssociatedWithSymbol = isSgFunctionType(functionSymbol->get_type());
                              ROSE_ASSERT(functionTypeAssociatedWithSymbol != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("case function declaration: functionType = %p \n",functionType);
                              printf ("case function declaration: functionTypeAssociatedWithSymbol = %p \n",functionTypeAssociatedWithSymbol);
#endif
                              if (functionType != functionTypeAssociatedWithSymbol)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("NOTE: we are now using the function type in the initial function symbol lookup? \n");
#endif
                                // DQ (4/12/2014): we need to record that there was another function identified in the parent scopes that we will want to have force name qualification.
                                   foundAnOverloadedFunctionWithSameName = true;

                                // DQ (4/12/2014): Check if the scopes are the same.  If the same then we don't need name qualification.
                                   SgScopeStatement* scopeAssociatedWithSymbol = functionSymbol->get_declaration()->get_scope();
                                   ROSE_ASSERT(scopeAssociatedWithSymbol != NULL);
                                   SgScopeStatement* scopeOfDeclaration = declaration->get_scope();
                                   ROSE_ASSERT(scopeOfDeclaration != NULL);

                                   if (scopeAssociatedWithSymbol == scopeOfDeclaration)
                                      {
                                        foundAnOverloadedFunctionInSameScope = true;
                                      }

                                   symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,functionType,currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("After using the function type: symbol = %p \n",symbol);
#endif
                                 }
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
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


                 // DQ (11/10/2014): Added support for templated typedefs (and their instantiations).
                    case V_SgTemplateTypedefDeclaration:
                    case V_SgTemplateInstantiationTypedefDeclaration:

                    case V_SgTypedefDeclaration:
                       {
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(declaration);
                         ROSE_ASSERT(typedefDeclaration != NULL);

                         SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                         if (typedefSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
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
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): typedefSymbol == NULL \n");
                                 }
#else
                           // DQ (5/15/2011): Added this to support where symbol after moving name qualification 
                           // support to the astPostProcessing phase instead of calling it in the unparser.
                              if (symbol != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
                                 }
                                else
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): typedefSymbol == NULL \n");
#endif
                                // DQ (7/24/2011): I don't understand this code...this appears to be a cut/paste error.
                                // Look for a template symbol
                                // symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);
                                // ROSE_ASSERT(symbol != NULL);
                                 }
#endif
                            }

                         break;
                       }


                 // DQ (8/13/2013): I think that this case should not appear, since SgTemplateDeclaration is a part of pre-EDG4x work.
                    case V_SgTemplateDeclaration:
                       {
                         SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(declaration);
                         ROSE_ASSERT(templateDeclaration != NULL);

#if 0
                      // DQ (8/13/2013): This is older code that we should not have to support how.

                         SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
                         if (templateSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

                           // DQ (8/13/2013): We want to use a SageInterface function that uses: lookup_template_class_symbol()
                           // Reset the symbol to one that will match the declaration.
                           // symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);
                              symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);
                           // ROSE_ASSERT(symbol != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                // DQ (6/22/2011): This is demonstrated by test2004_48.C when using the Thrifty simulator.
                                   printf ("Detected no template symbol in a parent scope (ignoring this case for now) \n");
                                 }
#endif
                            }
#else
                         printf ("In NameQualificationTraversal::nameQualificationDepth(): case V_SgTemplateDeclaration: This case should not be seen in the newer EDG4x work \n");
                         ROSE_ASSERT(false);
#endif

                         break;
                       }

                 // DQ (11/16/2013): I think we do need this case and test2013_273.C demonstrates this.
                 // DQ (6/1/2011): Not clear if we need a special case for the case of SgTemplateInstantiationMemberFunctionDecl.
                 // I think we need to call: evaluateNameQualificationForTemplateArgumentList()
                 // to evaluate template arguments for both SgTemplateInstantiationFunctionDecl and SgTemplateInstantiationMemberFunctionDecl.
                    case V_SgTemplateInstantiationFunctionDecl:
                       {
                         SgTemplateInstantiationFunctionDecl* templateFunction = isSgTemplateInstantiationFunctionDecl(declaration);
                         ROSE_ASSERT(templateFunction != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("In NameQualificationTraversal::nameQualificationDepth(): case V_SgTemplateInstantiationFunctionDecl: templateFunction = %p = %s \n",templateFunction,templateFunction->class_name().c_str());
                         printf ("   --- templateFunction->get_name()         = %s \n",templateFunction->get_name().str());
                         printf ("   --- templateFunction->get_templateName() = %s \n",templateFunction->get_templateName().str());
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                         SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
                         if (templateSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("calling lookupFunctionSymbolInParentScopes(): name = %s currentScope = %p = %s \n",name.str(),currentScope,currentScope->class_name().c_str());
#endif
                           // Reset the symbol to one that will match the declaration.
                           // DQ (4/12/2014): I think we need to use the function type here!
#if (DEBUG_NAME_QUALIFICATION_LEVEL >= 1)
                              printf ("Should we be using the function type in the initial SgTemplateInstantiationFunctionDecl symbol lookup? \n");
#endif
                              symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,currentScope);

                           // DQ (6/23/2013): Fixing test2013_223.C (function hiding template function instantiation).
                              SgName templateFunctionNameWithoutTemplateArguments = templateFunction->get_templateName();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("   --- templateFunctionNameWithoutTemplateArguments = %s \n",templateFunctionNameWithoutTemplateArguments.str());
#endif
                           // DQ (4/4/2014): Modified this to use the SgFunctionSymbol type.
                           // SgSymbol* symbolHiddingTemplateSymbol = SageInterface::lookupFunctionSymbolInParentScopes(templateFunctionNameWithoutTemplateArguments,currentScope);
                              SgFunctionSymbol* symbolHiddingTemplateInstantiationSymbol = SageInterface::lookupFunctionSymbolInParentScopes(templateFunctionNameWithoutTemplateArguments,currentScope);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("symbolHiddingTemplateInstantiationSymbol = %p \n",symbolHiddingTemplateInstantiationSymbol);
#endif
                              if (symbolHiddingTemplateInstantiationSymbol != NULL && symbolHiddingTemplateInstantiationSymbol != symbol)
                                 {
                                // This looks up the scope via the symbol table's parent (not sure that is a great approach).
                                   SgScopeStatement* hiddingSymbolScope = isSgScopeStatement(symbolHiddingTemplateInstantiationSymbol->get_parent()->get_parent());
                                   ROSE_ASSERT(hiddingSymbolScope != NULL);
                                   SgScopeStatement* functionScope = templateFunction->get_scope();
                                   ROSE_ASSERT(functionScope != NULL);

                                // DQ (4/4/2014): Look at the declarations associated with these symbols.
                                   SgDeclarationStatement* declarationHidingCurrentDeclaration = symbolHiddingTemplateInstantiationSymbol->get_declaration();
                                   ROSE_ASSERT(declarationHidingCurrentDeclaration != NULL);

                                   SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                                   ROSE_ASSERT(functionSymbol != NULL);

                                   SgDeclarationStatement* declarationFromSymbol = functionSymbol->get_declaration();
                                   ROSE_ASSERT(declarationFromSymbol != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("declaration from symbol             = %p = %s \n",declarationFromSymbol,declarationFromSymbol->class_name().c_str());
                                   printf ("declaration hidding template symbol = %p = %s \n",declarationHidingCurrentDeclaration,declarationHidingCurrentDeclaration->class_name().c_str());
#endif
                                   bool currentScopeIsNestedWithinScopeOfHiddingDeclaration = false;

                                // SgScopeStatement* temp_symbolScope = symbolScope;
                                   SgScopeStatement* temp_scope = currentScope;
                                   while (isSgGlobal(temp_scope) == NULL && temp_scope != hiddingSymbolScope)
                                      {
                                        temp_scope = temp_scope->get_scope();
                                      }

                                // if (temp_scope == hiddingSymbolScope)
                                   if (hiddingSymbolScope != currentScope && temp_scope == hiddingSymbolScope)
                                      {
                                        currentScopeIsNestedWithinScopeOfHiddingDeclaration = true;
                                      }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("currentScopeIsNestedWithinScopeOfHiddingDeclaration = %s \n",currentScopeIsNestedWithinScopeOfHiddingDeclaration ? "true" : "false");
                                   printf ("currentScope       = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   printf ("hiddingSymbolScope = %p = %s \n",hiddingSymbolScope,hiddingSymbolScope->class_name().c_str());
                                   printf ("temp_scope         = %p = %s \n",temp_scope,temp_scope->class_name().c_str());
#endif
                                // DQ (4/4/2014): We don't want to treat the template instantiation as be hidden by the template declaration
                                // because that does not make sense. So check the declarations.  Also, if the template instatiation is
                                // not output in the generated code, then it can't be name qualified.
                                   bool willBeOutput = (declaration->get_file_info()->isCompilerGenerated() == false ||
                                                         (declaration->get_file_info()->isCompilerGenerated() &&
                                                          declaration->get_file_info()->isOutputInCodeGeneration()) );
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("$$$$$ --- willBeOutput = %s \n",willBeOutput ? "true" : "false");
#endif
                                // DQ (4/4/2014): I think that they don't have to be the same kid of function.
                                // bool isSameKindOfFunction = (declarationHidingCurrentDeclaration->variant() == declarationFromSymbol->variant());

                                // If we visit the scope with the function hidding our template function then we will need some name qualification.
                                // if (temp_scope == symbolScope)
                                // if (willBeOutput && isSameKindOfFunction && temp_scope == symbolScope)
                                // if (willBeOutput == true || (willBeOutput == false && temp_scope == hiddingSymbolScope))
                                   if (willBeOutput == true || (willBeOutput == false && currentScopeIsNestedWithinScopeOfHiddingDeclaration == true))
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        printf ("WARNING: Some qualification is required to get past the non-template function hidding the template function = %p = %s \n",templateFunction,templateFunction->get_name().str());
                                        printf ("   --- Resetting the recorded symbol to NULL \n");
#endif
                                        symbol = NULL;
                                      }
#if 0
                                   printf ("ERROR: declaration from symbolHiddingTemplateInstantiationSymbol = %p hides SgTemplateInstantiationFunctionDecl \n",symbolHiddingTemplateInstantiationSymbol);
                                   ROSE_ASSERT(false);
#endif
                                 }

                           // DQ (7/24/2011): The symbol is NULL for test2011_121.C
                           // ROSE_ASSERT(symbol != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                // DQ (6/22/2011): This is demonstrated by test2011_121.C
                                   printf ("Detected no template function instantiation symbol in a parent scope (ignoring this case for now) \n");
                                 }
#endif
                            }
#if 0
                      // DQ (10/31/2013): I think this should be active code since test2013_273.C demonstrates a case where this is required.
                         printf ("&&&&&&&&&&&& Calling evaluateNameQualificationForTemplateArgumentList() templateFunction = %p = %s \n",templateFunction,templateFunction->class_name().c_str());

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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                              symbol = SageInterface::lookupEnumSymbolInParentScopes(name,currentScope);

                           // ROSE_ASSERT(symbol != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              if (symbol != NULL)
                                 {
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                 }
                                else
                                 {
                                // DQ (6/22/2011): This is demonstrated by test2011_95.C
                                   printf ("Detected no enum symbol in a parent scope (ignoring this case for now) \n");
                                 }
#endif
                            }

                         break;
                       }

                    case V_SgTemplateInstantiationDecl:
                       {
                         SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
                         ROSE_ASSERT(templateInstantiationDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("Found a case of declaration == SgTemplateInstantiationDecl \n");
#endif
                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                         if (classSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

                           // printf ("We might need the template arguments to look up this template class instantiation. \n");

                           // DQ (8/15/2013): This needs to be used in lookupClassSymbolInParentScopes(), but the function does not accept a SgTemplateArgumentPtrList pointer yet.
                              SgTemplateArgumentPtrList* templateArgumentsList = &(templateInstantiationDeclaration->get_templateArguments());

                           // DQ (8/16/2013): Modified API for symbol lookup.
                           // Reset the symbol to one that will match the declaration.
                           // symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope);
                              symbol = SageInterface::lookupClassSymbolInParentScopes(name,currentScope,templateArgumentsList);

                           // DQ (5/15/2011): Added this to support where symbol after moving name qualification 
                           // support to the astPostProcessing phase instead of calling it in the unparser.
                              if (symbol != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
                                 }
                                else
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): classSymbol == NULL \n");
#endif
#if 0
                                // DQ (8/13/2013): This is older code that we should not have to support how.
                                // template symbol lookup must be specific to classes, functions, or member functions to make sense now.

                                // Look for a template symbol
                                   symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);
#else
                                   SgTemplateClassDeclaration* templateClassDeclaration = templateInstantiationDeclaration->get_templateDeclaration();
                                   ROSE_ASSERT(templateClassDeclaration != NULL);

                                   SgTemplateParameterPtrList & templateParameterList = templateClassDeclaration->get_templateParameters();
                                   SgTemplateArgumentPtrList  & templateArgumentList  = templateClassDeclaration->get_templateSpecializationArguments();

                                // DQ (8/13/2013): This needs to be looked up as a SgTemplateClassSymbol.
                                   symbol = SageInterface::lookupTemplateClassSymbolInParentScopes(name,&templateParameterList,&templateArgumentList,currentScope);
#endif
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
                              SgDeclarationStatement* nestedDeclaration = classSymbol->get_declaration();
                              ROSE_ASSERT(nestedDeclaration != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Need to dig deeper into this symbol! \n");
                              printf ("nestedDeclaration = %p = %s \n",nestedDeclaration,nestedDeclaration->class_name().c_str());
#endif
                              SgTemplateInstantiationDecl* nestedTemplateDeclaration = isSgTemplateInstantiationDecl(nestedDeclaration);
                              if (nestedTemplateDeclaration != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("nestedTemplateDeclaration = %p = %s \n",nestedTemplateDeclaration,nestedTemplateDeclaration->get_name().str());
#endif
                                 }
                            }

                         break;
                       }

                 // DQ (3/13/2012): Added support for SgTemplateVariableDeclaration (I think it can just leverage the SgVariableDeclaration case).
                    case V_SgTemplateVariableDeclaration:

                    case V_SgVariableDeclaration:
                       {
                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declaration);
                         ROSE_ASSERT(variableDeclaration != NULL);

                         SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("variableSymbol = %p = %s \n",variableSymbol,symbol->class_name().c_str());
#endif
#if 0
                         printf ("In NameQualificationTraversal::nameQualificationDepth(): variableSymbol = %p = %s \n",variableSymbol,(symbol != NULL) ? symbol->class_name().c_str() : "null");
#endif
#if 0
                      // DQ (8/21/2012): This is better fixed by another method that elimiated a redundant SgBasicBlock that was being generated (in edgRose.C).
                      // DQ (8/20/2012): Check that we have the correct symbol.
                         if (variableSymbol != NULL)
                            {
                           // DQ (8/20/2012): If this is NON-NULL then we still have to check that it is the correct symbol (see test2011_31.C).
                           // SgInitializedName* declarationForVariableAssociatedWithSymbol = variableSymbol->get_declaration();
                              SgInitializedName* initializedNameForVariableAssociatedWithSymbol = variableSymbol->get_declaration();
                              ROSE_ASSERT(initializedNameForVariableAssociatedWithSymbol != NULL);
                              SgInitializedName* currentVariableDeclarationInitializedName = SageInterface::getFirstInitializedName(variableDeclaration); 
                              ROSE_ASSERT(currentVariableDeclarationInitializedName != NULL);

                              printf ("initializedNameForVariableAssociatedWithSymbol = %p \n",initializedNameForVariableAssociatedWithSymbol);
                              initializedNameForVariableAssociatedWithSymbol->get_startOfConstruct()->display("initializedNameForVariableAssociatedWithSymbol");
                              printf ("currentVariableDeclarationInitializedName      = %p \n",currentVariableDeclarationInitializedName);
                              currentVariableDeclarationInitializedName->get_startOfConstruct()->display("currentVariableDeclarationInitializedName");

                              if (initializedNameForVariableAssociatedWithSymbol != currentVariableDeclarationInitializedName)
                                 {
                                // If this is not the correct then rest the variableSymbol to NULL and look for another symbol in an outer scope.
                                   variableSymbol = NULL;
                                 }
                            }
#endif
                         if (variableSymbol == NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Type elaboration is required: declaration = %s symbol = %s \n",declaration->class_name().c_str(),symbol->class_name().c_str());
#endif
                              typeElaborationIsRequired = true;

                           // Reset the symbol to one that will match the declaration.
                           // symbol = SageInterface::lookupVariableSymbolInParentScopes(name,currentScope);
                              variableSymbol = SageInterface::lookupVariableSymbolInParentScopes(name,currentScope);
                           // if (symbol != NULL)
                              if (variableSymbol != NULL)
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                // printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                                   printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",variableSymbol,variableSymbol->class_name().c_str());
#endif
                                   symbol = variableSymbol;
                                 }
                                else
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): variableSymbol == NULL \n");
#endif
#if 0
                                // DQ (8/13/2013): The function lookupTemplateSymbolInParentScopes() has been removed from the API
                                // (it would have required template parameters and template arguments that are not available here).
                                // Look for a template symbol
                                   symbol = SageInterface::lookupTemplateSymbolInParentScopes(name,currentScope);
#else
#if 0
                                // DQ (8/13/2013): Output the source position of this problem.
                                   ROSE_ASSERT(variableDeclaration->get_file_info() != NULL);
                                   variableDeclaration->get_file_info()->display("variableDeclaration: debug");
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("WARNING: In NameQualificationTraversal::nameQualificationDepth(): variableSymbol == NULL: searching for the associated symbol using the declaration \n");
#endif
                                // DQ (8/14/2013): Use an alternative mechanism to get the correct symbol more directly 
                                // (might be more expensive, or perhaps this mechanism should be used more generally in 
                                // this name qualification support).
                                   SgInitializedName* currentVariableDeclarationInitializedName = SageInterface::getFirstInitializedName(variableDeclaration); 
                                   ROSE_ASSERT(currentVariableDeclarationInitializedName != NULL);

#if 0
                                // DQ (4/7/2014): We should not be resolving a symbol if we didn't find the correct symbol (see test2014_39.C).
                                   symbol = currentVariableDeclarationInitializedName->search_for_symbol_from_symbol_table();
                                   ROSE_ASSERT(symbol != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("In NameQualificationTraversal::nameQualificationDepth(): variableSymbol != NULL: variableDeclaration = %p symbol = %p = %s \n",variableDeclaration,symbol,symbol->class_name().c_str());
#endif
#else
                                // DQ (4/7/2014): Reset the symbol to NULL (snce we didn't find an associated SgVariableSymbol).
                                // In the switch statement (below) we will use the declaration to obtain the correct
                                // symbol and then compare if we have found the correct one using the name lookup through
                                // parent scopes.  Then we will beable to know if name qualification is required. If we
                                // use the declaration to find the symbol here, then we will detect that no name
                                // qualification is required (where it might be).  Testcode test2014_39.C demonstrates
                                // this issue.
                                   symbol = NULL;
#endif
#if 0
                                // DQ (8/13/2013): I think this case should not come up if everything else works.  Since there are no templated variables I'm not sure what to do here.
                                   printf ("Error: In NameQualificationTraversal::nameQualificationDepth(): variableSymbol == NULL: I think we should not have to support this case! \n");
                                   ROSE_ASSERT(false);
#endif
#endif
                                // DQ (6/5/2011): This assert fails for test2005_107.C (this is OK, the referenced symbol is not visible from the current scope).
                                // ROSE_ASSERT(symbol != NULL);
                                 }
                           // ROSE_ASSERT(symbol != NULL);
                           // printf ("Lookup symbol based symbol type: reset symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                            }

                         break;
                       }

                    default:
                       {
                      // Handle cases are we work through specific example codes.
                      // printf ("default reached symbol = %s \n",symbol->class_name().c_str());
                         printf ("default reached declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("SageInterface::lookupSymbolInParentScopes(name,currentScope) returned NULL \n");
#endif
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Calling evaluateTemplateInstantiationDeclaration() from nameQualificationDepth() declaration = %p = %s currentScope = %p = %s \n",declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str());
#endif

       // Refactored this code to another member function so that it could also support evaluation of declarations found in types (more generally).
          evaluateTemplateInstantiationDeclaration(declaration,currentScope,positionStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("DONE: Calling evaluateTemplateInstantiationDeclaration() from nameQualificationDepth() declaration = %p = %s currentScope = %p = %s \n",declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str());
#endif

       // At this point if there was any ambiguity in the first matching symbol that was found, then
       // we have resolved this to the correct type of symbol (SgClassSymbol, SgFunctionSymbol, etc.).
       // Now we want to resolve it to the exact symbol that matches the declaration.
          if (symbol != NULL)
             {
            // DQ (5/6/2011): Now we have fixed derived class symbol tables to inject there base classes symbols into the derived class.
               SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);

            // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
               ROSE_ASSERT(aliasSymbol == NULL);
#if 1
            // DQ (7/12/2014): debugging use of SgAliasSymbol.
               if (aliasSymbol != NULL)
                  {
                 // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
                    ROSE_ASSERT(false);

                    ROSE_ASSERT(aliasSymbol->get_alias() != NULL);
                    printf ("In NameQualificationTraversal::nameQualificationDepth(): Detected a SgAliasSymbol: alias = %p baseSymbol = %p = %s \n",aliasSymbol,aliasSymbol->get_alias(),aliasSymbol->get_alias()->class_name().c_str());
                  }
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("forceMoreNameQualification = %s \n",forceMoreNameQualification ? "true" : "false");
#endif

               if (forceMoreNameQualification == true)
                  {
                 // If there is more than one symbol with the same name then name qualification is required to distinguish between them.
                 // The exception to this is overloaded member functions.  But might also be where type evaluation is required.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Found a case of ambiguity (forceMoreNameQualification == true) of declaration = %s in the currentScope = %p = %s = %s (trigger additional name qualifier). \n",
                         declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
#endif
                    switch(declaration->variantT())
                       {
                         case V_SgFunctionDeclaration:
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Error: Skipping forced name qualification for SgFunctionDeclaration (sorry, not implemented) \n");
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("The associated class for the member function does not match its structural location so we require name qualification \n");
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Error: Skipping forced name qualification for enum types (sorry, not implemented) \n");
#endif
                           // We do reach this point in test2004_105.C
                           // ROSE_ASSERT(false);

                              break;
                            }

                         default:
                            {
                           // But we have to keep moving up the chain of scopes to see if the parent might also require qualification.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("We are forcing the name qualification so continue to resolve the name qualification depth... \n");
#endif
                              qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                            }
                       }
                  }
                 else
                  {
                 // The numberOfSymbols can be zero or one, because the symbol might not be the the current scope.  
                 // If it is zero then it just means that the name is visible 
                 // from the current scope by is not located in the current scope.  If it is one, then there is a 
                 // symbol matching the name and we need to check if it is associated with the same declaration or not.

                 // However, since symbol != NULL, the numberOfSymbols should be non-zero.
                 // ROSE_ASSERT(numberOfSymbols > 0);

                 // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
                    ROSE_ASSERT(aliasSymbol == NULL);

                 // Not clear if we want to resolve this to another scope since the alias sysmbols scope 
                 // is want might have to be qualified (not the scope of the aliased declaration).
                    if (aliasSymbol != NULL)
                       {
                      // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
                         ROSE_ASSERT(false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("Resetting the symbol to that stored in the SgAliasSymbol \n");
#endif
                         symbol = aliasSymbol->get_alias();

                      // DQ (7/23/2011): If we can't assert this, then we need to loop through the chain of alias 
                      // symbols to get to the non-alias (original) symbol.
                         ROSE_ASSERT(isSgAliasSymbol(symbol) == NULL);
                       }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("AT SWITCH: symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
                    switch (symbol->variantT())
                       {
                      // DQ (12/27/2011): Added support for template class symbols.
                         case V_SgTemplateClassSymbol:
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                // This class is visible from where it is referenced.
                                   printf ("This class IS visible from where it is referenced \n");

                                // However, if this is a templated class then we have to dig deeper to identify if the template arguments require name qualification.
                                   printf ("symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
#if 0
                                   printf ("I think this is associated with a template, need to stop here! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                                else
                                 {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                // The name does not match, so the associatedClassDeclaration is hidding the base class declaration.
                                   printf ("This class is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   printf ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("associatedNamespaceDeclaration = %p \n",associatedNamespaceDeclaration);
                              printf ("namespaceDeclaration           = %p \n",namespaceDeclaration);
#endif
                              if (associatedNamespaceDeclaration->get_firstNondefiningDeclaration() == namespaceDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This class is visible from where it is referenced.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This namespace IS visible from where it is referenced \n");
#endif
                                 }
                                else
                                 {
                                // The name does not match, so the associatedClassDeclaration is hidding the base class declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This namespace is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   printf ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
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
                                // This variable is visible from where it is referenced.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This variable IS visible from where it is referenced \n");
#endif
                                // DQ (12/23/2015): Need to check if there is an opportunity for an ambigous reference.
                                   size_t numberOfAliasSymbols = currentScope->count_alias_symbol(name);
                                   if (numberOfAliasSymbols >= 2)
                                      {
                                        qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        printf ("   --- qualificationDepth = %d \n",qualificationDepth);
#endif
                                      }
                                 }
                                else
                                 {
                                // The name does not match, so the associatedClassDeclaration is hidding the base class declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This variable is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   printf ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
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

                      // DQ (12/28/2011): Added support for new template handling in the AST.
                         case V_SgTemplateMemberFunctionSymbol:
                         case V_SgTemplateFunctionSymbol:

                         case V_SgMemberFunctionSymbol:
                         case V_SgFunctionSymbol:
                            {
                              SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                              ROSE_ASSERT(functionSymbol != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Case of SgFunctionSymbol or SgMemberFunctionSymbol: functionSymbol = %p = %s \n",functionSymbol,functionSymbol->class_name().c_str());
#endif
                           // This is a function symbol, check if the declaration is the same.
                           // SgFunctionDeclaration* associatedFunctionDeclaration = functionSymbol->get_declaration();
                              SgFunctionDeclaration* associatedFunctionDeclarationFromSymbol = functionSymbol->get_declaration();
                              ROSE_ASSERT(associatedFunctionDeclarationFromSymbol != NULL);

                              ROSE_ASSERT(functionDeclaration != NULL);

                           // DQ (11/19/2013): This is added to support testing cases where we would clearly fail the AST consistancy tests (e.g. LoopProcessing.C).
                           // I hate this work around, but I am hoping it will help identify a root cause of the problem.
                           // ROSE_ASSERT(functionDeclaration->get_firstNondefiningDeclaration() != NULL);
                              if (functionDeclaration->get_firstNondefiningDeclaration() == NULL)
                                 {
                                   printf ("***** ERROR: In NameQualificationTraversal::nameQualificationDepth(): we are supporting this case though it is a violation of the AST consistancy tests! ***** \n");
                                   return 0;
                                 }

                           // DQ (11/18/2013): This is an assertion inside of get_declaration_associated_with_symbol() which we are now failing.
                              ROSE_ASSERT(functionDeclaration->get_firstNondefiningDeclaration() == functionDeclaration->get_firstNondefiningDeclaration()->get_firstNondefiningDeclaration()); 

                              SgDeclarationStatement* declarationFromSymbol = functionDeclaration->get_declaration_associated_with_symbol();
#if 1
                           // DQ (11/18/2013): Try to reset this...
                              if (declarationFromSymbol == NULL)
                                 {
#if 0
                                   printf ("In name qualification support: declarationFromSymbol == NULL: retry using functionDeclaration->get_firstNondefiningDeclaration() \n");
#endif
                                   declarationFromSymbol = functionDeclaration->get_firstNondefiningDeclaration()->get_declaration_associated_with_symbol();
                                 }
#endif
                              ROSE_ASSERT(declarationFromSymbol != NULL);

                              SgFunctionDeclaration* functionDeclarationFromSymbol = isSgFunctionDeclaration(declarationFromSymbol);
                              ROSE_ASSERT(functionDeclarationFromSymbol != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("associatedFunctionDeclarationFromSymbol = %p = %s \n",associatedFunctionDeclarationFromSymbol,associatedFunctionDeclarationFromSymbol->class_name().c_str());
                              printf ("functionDeclarationFromSymbol           = %p = %s \n",functionDeclarationFromSymbol,functionDeclarationFromSymbol->class_name().c_str());

                              printf ("associatedFunctionDeclarationFromSymbol->get_firstNondefiningDeclaration() = %p \n",associatedFunctionDeclarationFromSymbol->get_firstNondefiningDeclaration());
                              printf ("functionDeclarationFromSymbol->get_firstNondefiningDeclaration()           = %p \n",functionDeclarationFromSymbol->get_firstNondefiningDeclaration());
                              if (associatedFunctionDeclarationFromSymbol->get_firstNondefiningDeclaration() == NULL)
                                 {
                                // DQ (6/22/2011): This is the case when a function has only a defining declaration and in this case no nondefining declaration is built for a function.
                                // This is true for both SgFunctionDeclaration and SgMemberFunctionDeclaration handling (but may change to be more uniform with other declarations in the future).
                                   printf ("Found a valid function with get_firstNondefiningDeclaration() == NULL (not a problem, just a special case)\n");
                                 }
#endif
                           // if (associatedFunctionDeclaration->get_firstNondefiningDeclaration() == functionDeclaration->get_firstNondefiningDeclaration())
                              if (associatedFunctionDeclarationFromSymbol == functionDeclarationFromSymbol)
                                 {
                                // DQ (4/12/2014): Now we know that it can be found, but we still need to check if there would 
                                // be another function that could be used and for which we need name qualification to avoid.

                                // DQ (4/12/2014): We need to use the recorded value foundAnOverloadedFunctionWithSameName because we may want to have force name qualification.
                                   if (foundAnOverloadedFunctionWithSameName == true)
                                      {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                        printf ("There was another function identified in the process of resolving that this function could be found. thus we will require some name qualification \n");
#endif
                                        if (foundAnOverloadedFunctionInSameScope == false)
                                           {
#if (DEBUG_NAME_QUALIFICATION_LEVEL >= 1)
                                             printf ("In name qualification support: case V_SgFunctionSymbol: We need to compute the CORRECT name qualification depth: using 1 for now! \n");
#endif
                                             qualificationDepth = 1;
                                           }
#if 0
                                          else
                                           {
                                             qualificationDepth = 0;
                                           }
#endif
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }

                                // DQ (6/20/2011): But we don't check for if there was another declaration that might be a problem (overloaded functions don't count!)...
                                // This function is visible from where it is referenced. 
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This function or member function IS visible from where it is referenced (but there could still be ambiguity if this was just the first of several symbols found in the current scope) \n");
                                   printf ("   --- currentScope        = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   printf ("   --- functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());

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
#endif
#if 0
                                // DQ (6/23/2013): Fixing test2013_223.C (function hiding template function instantiation).
                                   SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(functionDeclaration);
                                   if (templateInstantiationFunctionDeclaration != NULL)
                                      {
                                        SgName templateFunctionNameWithoutTemplateArguments = templateInstantiationFunctionDeclaration->get_templateName();
                                        printf ("   --- templateFunctionNameWithoutTemplateArguments = %s \n",templateFunctionNameWithoutTemplateArguments.str());
                                      }
#endif
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the base class declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This function or member function is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   printf ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
#if 0
                                // DQ (4/12/2014): Since we have searched using the function type, we don't need to compare the function types here.
                                // DQ (4/8/2014): Added test on function types.
                                   SgFunctionType* associatedfunctionTypeFromSymbol = associatedFunctionDeclarationFromSymbol->get_type();
                                   SgFunctionType* functionTypeFromSymbol           = functionDeclarationFromSymbol->get_type();

                                   bool functionInSameScope = (associatedFunctionDeclarationFromSymbol->get_scope() == functionDeclarationFromSymbol->get_scope());
                                   bool functionTypesMatch  = (associatedfunctionTypeFromSymbol == functionTypeFromSymbol);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("   --- functionTypesMatch = %s \n",functionTypesMatch ? "true" : "false");
#endif
                                // int increment = functionTypesMatch ? 1 : 0;
                                   int increment = functionInSameScope && functionTypesMatch ? 1 : 0;
#else
                                   int increment = 1;
#endif
                                // DQ (4/8/2014): If the type match then use an increment of 1, else don't increment the qualificationDepth.
                                // Now resolve how much name qualification is required; what ever is required for the parent plus 1.
                                // qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
                                   qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + increment;
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

                      // DQ (11/10/2014): Adding support for templated typedef declarations.
                         case V_SgTemplateTypedefSymbol:

                         case V_SgTypedefSymbol:
                            {
                              SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                              ROSE_ASSERT(typedefSymbol != NULL);

                           // This is a typdef symbol, check if the declaration is the same.
                              SgTypedefDeclaration* associatedTypedefDeclaration = typedefSymbol->get_declaration();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("associatedTypedefDeclaration = %p = %s \n",associatedTypedefDeclaration,associatedTypedefDeclaration->get_name().str());
#endif
                              ROSE_ASSERT(typedefDeclaration != NULL);
                              ROSE_ASSERT(associatedTypedefDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("associatedTypedefDeclaration->get_firstNondefiningDeclaration() = %p \n",associatedTypedefDeclaration->get_firstNondefiningDeclaration());
                              printf ("typedefDeclaration->get_firstNondefiningDeclaration()           = %p \n",typedefDeclaration->get_firstNondefiningDeclaration());
                              printf ("currentScope                                                    = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                              if (associatedTypedefDeclaration->get_firstNondefiningDeclaration() == typedefDeclaration->get_firstNondefiningDeclaration())
                                 {
                                // This typedef is visible from where it is referenced.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This typedef IS visible from where it is referenced \n");
#endif
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the base class declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This typedef is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   printf ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("WARNING: Support for name qualification for SgTemplateInstantiationFunctionDecl is not implemented yet \n");
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This enum IS visible from where it is referenced \n");
#endif
                                 }
                                else
                                 {
                                // The name does not match, so the associatedFunctionDeclaration is hidding the base class declaration.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("This enum is NOT visible from where it is referenced (declaration with same name does not match) \n");
                                   printf ("   --- currentScope      = %p = %s \n",currentScope,currentScope->class_name().c_str());
                                   printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("This declaration = %p = %s is NOT visible from where it is referenced (no declaration with same name, calling nameQualificationDepthOfParent()) \n",declaration,declaration->class_name().c_str());
               printf ("   --- currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               printf ("   --- positionStatement = %p = %s \n",positionStatement,positionStatement->class_name().c_str());
#endif
               qualificationDepth = nameQualificationDepthOfParent(declaration,currentScope,positionStatement) + 1;
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::nameQualificationDepth(): qualificationDepth = %d Report type elaboration: typeElaborationIsRequired = %s \n",qualificationDepth,(typeElaborationIsRequired == true) ? "true" : "false");
#endif

     return qualificationDepth;
   }



SgDeclarationStatement* 
NameQualificationTraversal::getDeclarationAssociatedWithType( SgType* type )
   {
  // Note that this function could be eliminated since it only wraps another function.

     ROSE_ASSERT(type != NULL);

#if 0
     printf ("In getDeclarationAssociatedWithType(): type = %s \n",type->class_name().c_str());
#endif

     SgDeclarationStatement* declaration = type->getAssociatedDeclaration();

#if 0
     printf ("In getDeclarationAssociatedWithType(): declaration = %p \n",declaration);
#endif

  // Primative types will not have an asociated declaration...
  // ROSE_ASSERT(declaration != NULL);
     if (declaration == NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In getDeclarationAssociatedWithType(): declaration == NULL type = %s \n",type->class_name().c_str());
#endif
#if 0
          SgArrayType* arrayType = isSgArrayType(type);
          if (arrayType != NULL)
             {
#if 0
               printf ("Detected SgArrayType: checking for variable reference in index (might be redundant): arrayType = %p \n",arrayType);
#endif
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#endif
        }
       else
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In getDeclarationAssociatedWithType(): declaration                                    = %p \n",declaration);
          printf ("In getDeclarationAssociatedWithType(): declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
          printf ("In getDeclarationAssociatedWithType(): declaration->get_definingDeclaration()         = %p \n",declaration->get_definingDeclaration());
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          if (declaration != declaration->get_firstNondefiningDeclaration())
             {
            // Output some debug information to learn more about this error
               printf ("In getDeclarationAssociatedWithType(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
               ROSE_ASSERT(declaration->get_file_info() != NULL);
               declaration->get_file_info()->display("declaration");

               ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() != NULL);
               printf ("In getDeclarationAssociatedWithType(): declaration->get_firstNondefiningDeclaration() = %p = %s \n",declaration->get_firstNondefiningDeclaration(),declaration->get_firstNondefiningDeclaration()->class_name().c_str());
               ROSE_ASSERT(declaration->get_firstNondefiningDeclaration()->get_file_info() != NULL);
               declaration->get_firstNondefiningDeclaration()->get_file_info()->display("declaration->get_firstNondefiningDeclaration()");
             }
#endif

       // DQ (11/6/2011): I think it is OK for where this is a SgEnumDeclaration.
       // ROSE_ASSERT(declaration == declaration->get_firstNondefiningDeclaration());
          ROSE_ASSERT(declaration == declaration->get_firstNondefiningDeclaration() || isSgEnumDeclaration(declaration) != NULL);
        }

     return declaration;
   }


// void evaluateNameQualificationForTemplateArgumentList (SgTemplateArgumentPtrList & templateArgumentList, SgScopeStatement* currentScope, SgStatement* positionStatement);
void
NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList (SgTemplateArgumentPtrList & templateArgumentList, SgScopeStatement* currentScope, SgStatement* positionStatement)
   {
  // DQ (6/4/2011): Note that test2005_73.C demonstrate where the Template arguments are shared between template instantiations.

  // DQ (9/24/2012): Track the recursive depth in computing name qualification for template arguments of template instantiations used as template arguments.
     static int recursiveDepth = 0;

  // Used for debugging...
     int counter = 0;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("\n\n*********************************************************************************************************************\n");
     printf ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): templateArgumentList.size() = %" PRIuPTR " recursiveDepth = %d \n",templateArgumentList.size(),recursiveDepth);
     printf ("*********************************************************************************************************************\n");

     printf ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): currentScope = %p = %s positionStatement = %p = %s \n",
          currentScope,currentScope->class_name().c_str(),positionStatement,positionStatement->class_name().c_str());

     ROSE_ASSERT(positionStatement != NULL);
     positionStatement->get_file_info()->display("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList()");
#endif

     SgTemplateArgumentPtrList::iterator i = templateArgumentList.begin();
     while (i != templateArgumentList.end())
        {
          SgTemplateArgument* templateArgument = *i;
          ROSE_ASSERT(templateArgument != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("*** Processing template argument #%d templateArgument = %p \n",counter,templateArgument);
       // SgName testNameInMap = templateArgument->get_qualified_name_prefix();
          printf ("===== Before being finished with evaluation of templateArgument name qualification: testNameInMap = %s \n",templateArgument->unparseToString().c_str());
#endif
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("templateArgument = %p contains type which is namedType = %p = %s \n",templateArgument,namedType,namedType->class_name().c_str());
#endif
                    SgDeclarationStatement* templateArgumentTypeDeclaration = getDeclarationAssociatedWithType(type);
                    if (templateArgumentTypeDeclaration != NULL)
                       {
                      // Check the visability and unambiguity of this declaration.
                      // Note that since the recursion happens before we set the names, all qualified name are set first 
                      // at the nested types and then used in the setting of qualified names at the higher level types 
                      // (less nested types).

                      // DQ (5/15/2011): Added recursive handling of template arguments which can require name qualification.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("xxxxxx --- Making a RECURSIVE call to nameQualificationDepth() on the template argument recursiveDepth = %d \n",recursiveDepth);
                      // DQ (9/24/2012): I think this is the way to make the recursive call to handle name qualification of template arguments in nexted template instantiations.
                         printf ("Need to call evaluateNameQualificationForTemplateArgumentList() on any possible template argument list for type in templateArgument = %p (namely namedType = %p = %s) \n",
                              templateArgument,namedType,namedType->class_name().c_str());
#endif

                         SgClassType* classType = isSgClassType(namedType);
                         if (classType != NULL)
                            {
                           // If this is a class then it should be relative to it's declaration.
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                              ROSE_ASSERT(classDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("namedType is a SgClassType: classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
#endif
                              SgTemplateInstantiationDecl* templateClassInstantiationDeclaration = isSgTemplateInstantiationDecl(classDeclaration);
                              if (templateClassInstantiationDeclaration != NULL)
                                 {
                                   recursiveDepth++;
                                   evaluateNameQualificationForTemplateArgumentList(templateClassInstantiationDeclaration->get_templateArguments(),currentScope,positionStatement);
                                   recursiveDepth--;
                                 }
                            }
                           else
                            {
                           // If not a class then (e.g. typedef) then it is relative to the typedef declaration, but we don't have to recursively evaluate the type.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("This is not a SgClassType, so we don't have to recursively evaluate for template arguments. \n");
#endif
                            }

                         int amountOfNameQualificationRequiredForTemplateArgument = nameQualificationDepth(namedType,currentScope,positionStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("xxxxxx --- amountOfNameQualificationRequiredForTemplateArgument = %d (for type = %p = %s) (counter = %d recursiveDepth = %d) \n",
                              amountOfNameQualificationRequiredForTemplateArgument,namedType,namedType->class_name().c_str(),counter,recursiveDepth);
                         printf ("xxxxxx --- Must call a function to set the name qualification data in the SgTemplateArgument = %p \n",templateArgument);
#endif
                         setNameQualification(templateArgument,templateArgumentTypeDeclaration,amountOfNameQualificationRequiredForTemplateArgument);
                       }
                  }

            // If this was not a SgNamedType (and even if it is, see test2011_117.C), it still might be a type 
            // where name qualification is required (might be a SgArrayType with an index requiring qualification).
               processNameQualificationForPossibleArrayType(type,currentScope);
             }
            else
             {
            // DQ (6/20/2011): This might be a variable reference requiring name qualification (see test2011_87.C and test2011_88.C).
               SgExpression* expression = templateArgument->get_expression();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Template argument was not a type: expression = %p \n",expression);
#endif
               if (expression != NULL)
                  {
                 // Check if this is a variable in which case it might require name qualification.  If we we have to traverse this expression recursively.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Expression found in SgTemplateArgument = %s \n",expression->class_name().c_str());
                 // We need to traverse this expression and evaluate if any name qualification is required on its pieces (e.g. referenced variables)
                    printf ("Recursive call to generateNameQualificationSupport() with expression = %p = %s \n",expression,expression->class_name().c_str());
#endif

#if 0
                 // This is the older version, but debug what I have first before switching.
                    printf ("This is the older version, used generateNestedTraversalWithExplicitScope() instead \n");
                    generateNameQualificationSupport(expression,referencedNameSet);
#else
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): Calling generateNestedTraversalWithExplicitScope(): with expression = %p = %s and currentScope = %p = %s \n",
                         expression,expression->class_name().c_str(),currentScope,currentScope->class_name().c_str());
#endif
                    generateNestedTraversalWithExplicitScope(expression,currentScope);
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("In NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): DONE: Recursive call to generateNameQualificationSupport() with expression = %p = %s \n",expression,expression->class_name().c_str());
#endif
                  }
                 else
                  {
                 // DQ (6/23/2013): Make it an error to detect a SgTemplateArgument that is neither a type or expression (should not exist, I think; or is at least not handled).
#if 0
                 // This fails for test2006_122.C.
                    printf ("This template argument is not an SgType OR an SgExpression: templateArgument = %p \n",templateArgument);
                    ROSE_ASSERT(false);
#else
                 // DQ (6/23/2013): Output the template argument type so that we can evaluate this (might be template template argument that requires name qualificaion).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("WARNING: This template argument is not an SgType OR an SgExpression: templateArgument = %p argumentType = %d \n",templateArgument,(int)templateArgument->get_argumentType());
#endif
#endif
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("===== After finishing with evaluation of templateArgument name qualification: templateArgument = %p testNameInMap = %s \n",templateArgument,templateArgument->unparseToString().c_str());
#endif

          i++;

       // Used for debugging...
          counter++;
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("*****************************************************************************************************************************\n");
     printf ("Leaving NameQualificationTraversal::evaluateNameQualificationForTemplateArgumentList(): templateArgumentList.size() = %" PRIuPTR " recursiveDepth = %d \n",templateArgumentList.size(),recursiveDepth);
     printf ("*****************************************************************************************************************************\n\n");
#endif
   }


int
NameQualificationTraversal::nameQualificationDepth ( SgType* type, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
     int amountOfNameQualificationRequired = 0;

     ROSE_ASSERT(type != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In nameQualificationDepth(SgType*): type = %p = %s \n",type,type->class_name().c_str());
#endif

  // DQ (7/23/2011): Test if we see array types here (looking for way to process all array types).
  // ROSE_ASSERT(isSgArrayType(type) == NULL);

  // DQ (7/23/2011): If this is an array type, then we need special processing for any name qualification of its index expressions.
     processNameQualificationForPossibleArrayType(type,currentScope);

     SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(type);
     if (declaration != NULL)
        {
       // SgScopeStatement* currentScope = initializedName->get_scope();

       // Check the visability and unambiguity of this declaration.
          amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif

          SgTemplateInstantiationDecl* templateClassInstantiationDeclaration = isSgTemplateInstantiationDecl(declaration);
          if (templateClassInstantiationDeclaration != NULL)
             {
            // We need to handle any possible template arguments.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("In nameQualificationDepth(SgType*): declaration = %p = %s Unhandled template arguments \n",declaration,declaration->class_name().c_str());
#endif
            // ROSE_ASSERT(false);
             }
        }
       else
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("ERROR: In nameQualificationDepth(SgType*): declaration NOT found for type = %p = %s\n",type,type->class_name().c_str());
#endif
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("Leaving nameQualificationDepth(SgType*): type = %p = %s amountOfNameQualificationRequired = %d \n",type,type->class_name().c_str(),amountOfNameQualificationRequired);
#endif

     return amountOfNameQualificationRequired;
   }


// int NameQualificationTraversal::nameQualificationDepthForType ( SgInitializedName* initializedName, SgStatement* positionStatement )
int
NameQualificationTraversal::nameQualificationDepthForType ( SgInitializedName* initializedName, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
     ROSE_ASSERT(initializedName   != NULL);
     ROSE_ASSERT(positionStatement != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In nameQualificationDepthForType(): initializedName = %s type = %p = %s currentScope = %p = %s \n",initializedName->get_name().str(),initializedName->get_type(),initializedName->get_type()->class_name().c_str(),currentScope,currentScope->class_name().c_str());
#endif

  // return nameQualificationDepth(initializedName->get_type(),initializedName->get_scope(),positionStatement);
     return nameQualificationDepth(initializedName->get_type(),currentScope,positionStatement);
   }


int
NameQualificationTraversal::nameQualificationDepth ( SgInitializedName* initializedName, SgScopeStatement* currentScope, SgStatement* positionStatement )
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

  // DQ (12/28/2011): Added test...
     ROSE_ASSERT(initializedName->get_scope() != NULL);

  // printf ("In NameQualificationTraversal::nameQualificationDepth(): initializedName->get_scope() = %p = %s \n",initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());

     SgDeclarationStatement* declaration = associatedDeclaration(initializedName->get_scope());
  // ROSE_ASSERT(declaration != NULL);

  // printf ("************** In nameQualificationDepth(): declaration = %p \n",declaration);

     SgVariableSymbol* variableSymbol = NULL;

  // DQ (8/16/2013): Modified to support new API.
  // SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope);
     SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name,currentScope,NULL,NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::nameQualificationDepth(SgInitializedName* = %p): symbol = %p \n",initializedName,symbol);
#endif

     if (symbol != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Lookup symbol based on name only (via parents starting at currentScope = %p = %s: name = %s symbol = %p = %s) \n",currentScope,currentScope->class_name().c_str(),name.str(),symbol,symbol->class_name().c_str());
#endif

#if 0
          printf ("This should be a while loop to resolve the full possible chain of alias symbols! \n");
          SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);
          if (aliasSymbol != NULL) 
             {
               symbol = aliasSymbol->get_alias();

               ROSE_ASSERT(isSgAliasSymbol(symbol) == NULL);
             }
#else
       // Loop over possible chain of alias symbols to find the original sysmbol.
          SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);

       // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
          ROSE_ASSERT(aliasSymbol == NULL);

          while (aliasSymbol != NULL) 
             {
            // DQ (7/12/2014): The newer design of the symbol table handling means that we will never see a SgAliasSymbol at this level.
               ROSE_ASSERT(false);
#if 1
            // DQ (7/12/2014): debugging use of SgAliasSymbol.
               printf ("In NameQualificationTraversal::nameQualificationDepth(): resolving alias symbol in loop: alias = %p baseSymbol = %p = %s \n",aliasSymbol,aliasSymbol->get_alias(),aliasSymbol->get_alias()->class_name().c_str());
#endif
               symbol = aliasSymbol->get_alias();
               aliasSymbol = isSgAliasSymbol(symbol);
             }
          ROSE_ASSERT(isSgAliasSymbol(symbol) == NULL);
#endif
          variableSymbol = isSgVariableSymbol(symbol);
          if (variableSymbol == NULL)
             {
               variableSymbol = SageInterface::lookupVariableSymbolInParentScopes(name,currentScope);

            // ROSE_ASSERT(variableSymbol != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               if (variableSymbol != NULL)
                  {
                    printf ("Lookup symbol based symbol type: variableSymbol = %p = %s \n",variableSymbol,variableSymbol->class_name().c_str());
                  }
                 else
                  {
                    printf ("In NameQualificationTraversal::nameQualificationDepth(SgInitializedName*,SgScopeStatement*,SgStatement*): variableSymbol == NULL \n");
                  }
#endif

            // amountOfNameQualificationRequired = nameQualificationDepth(associatedDeclaration(initializedName->get_scope()),currentScope,positionStatement) + 1;
            // SgDeclarationStatement* declaration = associatedDeclaration(initializedName->get_scope());

            // DQ (6/21/2011): This assertion fails for test2007_55.C.
            // DQ (6/5/2011): This assertion fails for test2005_114.C.
            // ROSE_ASSERT(declaration != NULL);
            // amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
               if (declaration != NULL)
                  {
                    amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
                  }
                 else
                  {
                 // DQ (6/21/2011): This is the case for test2007_55.C.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("WARNING: there is no associated declaration! \n");
#endif
                  }
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("initializedName->get_prev_decl_item() = %p \n",initializedName->get_prev_decl_item());
#endif
            // DQ (6/4/2011): Get the associated symbol so that we can avoid matching on name only; and not the actual SgVariableSymbol symbols.
               SgVariableSymbol* targetInitializedNameSymbol = isSgVariableSymbol(initializedName->search_for_symbol_from_symbol_table());
               ROSE_ASSERT(targetInitializedNameSymbol != NULL);

            // DQ (6/4/2011): Make sure we have the correct symbol, else we have detected a collision which will require name qualification to resolve.
               if (variableSymbol == targetInitializedNameSymbol)
                  {
                 // Found the correct symbol.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Found the correct SgVariableSymbol \n");
#endif
                  }
                 else
                  {
                 // This is not the correct symbol, even though the unqualified names match.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("These symbols only match based on name and is not the targetInitializedNameSymbol. \n");
#endif
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                 // DQ (12/15/2014): Liao's move tool will cause this message to be output, but it is not a problem (I think).
                    if (declaration == NULL)
                       {
                         printf ("variableSymbol = %p \n",variableSymbol);
                         printf ("targetInitializedNameSymbol = %p = %s \n",targetInitializedNameSymbol,targetInitializedNameSymbol->get_name().str());
                         printf ("initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
                         initializedName->get_file_info()->display("NameQualificationTraversal::nameQualificationDepth(): initializedName");
                       }
#endif

                 // DQ (12/28/2011): I think it may be OK to have this be NULL, in which case there is not name qualification (scope has no associated declaration, so it is NULL as is should be).
                 // ROSE_ASSERT(declaration != NULL);
                    if (declaration != NULL)
                       {
                         amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement) + 1;
                       }
                      else
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                      // DQ (12/15/2014): Liao's move tool will cause this message to be output, but it is not a problem (I think).
                         printf ("Warning: In NameQualificationTraversal::nameQualificationDepth() --- It might be that this is an incorrect fix for where declaration == NULL in test2004_97.C \n");
#endif
                       }
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("In NameQualificationTraversal::nameQualificationDepth(SgInitializedName*): declaration == NULL, why is this? initializedName->get_scope() = %p = %s \n",initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());
#endif
            // ROSE_ASSERT(false);
             }
        }

  // amountOfNameQualificationRequired = nameQualificationDepth(declaration,currentScope,positionStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif

     return amountOfNameQualificationRequired;
   }


void
NameQualificationTraversal::addToNameMap ( SgNode* nodeReference, string typeNameString )
   {
  // DQ (6/21/2011): This is refactored code used in traverseType() and traverseTemplatedFunction().
     bool isTemplateName = (typeNameString.find('<') != string::npos) && (typeNameString.find("::") != string::npos);

     if (isTemplateName == true)
        {
          if (typeNameMap.find(nodeReference) == typeNameMap.end())
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("============== Inserting qualifier for name = %s into typeNameMap list at IR node = %p = %s \n",typeNameString.c_str(),nodeReference,nodeReference->class_name().c_str());
#endif
               typeNameMap.insert(std::pair<SgNode*,std::string>(nodeReference,typeNameString));
             }
            else
             {
            // If it already existes then overwrite the existing information.
               std::map<SgNode*,std::string>::iterator i = typeNameMap.find(nodeReference);
               ROSE_ASSERT (i != typeNameMap.end());

               string previousTypeName = i->second.c_str();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
// #if 1
               printf ("WARNING: replacing previousTypeName = %s with new typeNameString = %s for nodeReference = %p = %s \n",previousTypeName.c_str(),typeNameString.c_str(),nodeReference,nodeReference->class_name().c_str());
#endif
               if (i->second != typeNameString)
                  {
                    i->second = typeNameString;

                    printf ("Error: name in qualifiedNameMapForNames already exists and is different... nodeReference = %p = %s \n",nodeReference,nodeReference->class_name().c_str());
                    ROSE_ASSERT(false);
                 }
             }
        }
       else
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
       // DQ (8/19/2013): comment added for debugging.
          printf ("In NameQualificationTraversal::addToNameMap(): isTemplateName == false, typeNameString = %s NOT added to typeNameMap for key = %p = %s \n",typeNameString.c_str(),nodeReference,nodeReference->class_name().c_str());
#endif
        }
   }


void
NameQualificationTraversal::traverseType ( SgType* type, SgNode* nodeReferenceToType, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // The type can contain subtypes (e.g. template arguments) and when the subtypes need to be name qualificed the name of the encompassing type 
  // has a name that depends upon its location in the source code (and could vary depending on the positon in a single basic block, I think).

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("<<<<< Starting traversal of type: type = %p = %s \n",type,type->class_name().c_str());
#endif

     ROSE_ASSERT(nodeReferenceToType != NULL);

  // Some type IR nodes are difficult to save as a string and reuse. So for now we will skip supporting 
  // some type IR nodes with generated name qualification specific to where they are used.
     bool skipThisType = false;
     if (isSgPointerMemberType(type) != NULL)
        {
          skipThisType = true;
        }

#if 0
  // DQ (4/23/2013): Added this case to allow pointers to function to have there argument types unparsed with name qualification.
     if (isSgPointerType(type) != NULL)
        {

#error "DEAD CODE!"

          SgPointerType* pointerType = isSgPointerType(type);
#if 0
          printf ("In NameQualificationTraversal::traverseType(): pointerType->get_base_type() = %p = %s \n",pointerType->get_base_type(),pointerType->get_base_type()->class_name().c_str());
#endif
          skipThisType = true;
        }
#endif

     if (skipThisType == false)
        {
          SgDeclarationStatement* declaration = associatedDeclaration(type);
          if (declaration != NULL)
             {
#if 0
               printf ("In NameQualificationTraversal::traverseType(): Calling evaluateTemplateInstantiationDeclaration(): declaration = %p = %s currentScope = %p = %s positionStatement = %p = %s \n",
                    declaration,declaration->class_name().c_str(),currentScope,currentScope->class_name().c_str(),positionStatement,positionStatement->class_name().c_str());
#endif
               evaluateTemplateInstantiationDeclaration(declaration,currentScope,positionStatement);
             }

          SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
          ROSE_ASSERT (unparseInfoPointer != NULL);
          unparseInfoPointer->set_outputCompilerGeneratedStatements();

       // Avoid unpasing the class definition when unparseing the type.
          unparseInfoPointer->set_SkipClassDefinition();

       // DQ (5/8/2013): Added specification to skip enum definitions also (see test2012_202.C).
          unparseInfoPointer->set_SkipEnumDefinition();

       // Associate the unparsing of this type with the statement or scope where it occures.
       // This is the key to use in the lookup of the qualified name. But this is the correct key....
       // unparseInfoPointer->set_reference_node_for_qualification(positionStatement);
       // unparseInfoPointer->set_reference_node_for_qualification(currentScope);
          unparseInfoPointer->set_reference_node_for_qualification(nodeReferenceToType);

       // DQ (5/7/2013): A problem with this is that it combines the first and second parts of the 
       // type into a single string (e.g. the array type will include two parts "base_type" <array name> "[index]".
       // When this is combined for types that have two parts (most types don't) the result is an error
       // when the type is unparsed.  It is not clear, but a solution might be for this to be built here
       // as just the 1st part, and let the second part be generated when the array type is unparsed.
       // BTW, the reason why it is computed here is that there may be many nested types that require 
       // name qualifications and so it is required that we save the whole string.  However, name 
       // qualification might only apply to the first part of types.  So we need to investigate this.
       // This is a problem demonstrated in test2013_156.C and test2013_158.C.

       // DQ (5/8/2013): Set the SgUnparse_Info so that only the first part will be unparsed.
          unparseInfoPointer->set_isTypeFirstPart();

       // DQ (8/19/2013): Added specification to skip class specifier (fixes problem with test2013_306.C).
          unparseInfoPointer->set_SkipClassSpecifier();

          string typeNameString = globalUnparseToString(type,unparseInfoPointer);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("++++++++++++++++ typeNameString (globalUnparseToString()) = %s \n",typeNameString.c_str());
#endif

       // DQ (7/13/2011): OSX can have types that are about 2487 characters long (see test2004_35.C).
       // This is symptematic of an error which causes the whole class to be included with the class 
       // definition.  This was fixed by calling unparseInfoPointer->set_SkipClassDefinition() above.
          if (typeNameString.length() > 6000)
             {
               if (SgProject::get_verbose() > 0)
                  {
                    printf ("Warning: type names should not be this long...(unless this is boost) typeNameString.length() = %" PRIuPTR " \n",typeNameString.length());
                  }
#if 0
            // DQ (6/30/2013): Allow the output of an example so that we can verify that this make 
            // sense (and that it is not a result of some comment output from the unparsing of types).
               static bool outputOneExample = true;
               if (outputOneExample == true)
                  {
                    printf ("   --- example typeNameString = %s \n",typeNameString.c_str());
                    outputOneExample = false;
                  }
#endif

            // DQ (10/11/2015): Increased max size of typename handled in ROSE (a 42K charater long 
            // typename was generated by test2015_87.C), so we will allow this.
            // DQ (1/30/2013): Increased already too long limit for string lengths for typenames.  This 
            // test fails for ROSE compiling ROSE with a type name that is 17807 characters long.  I have
            // increased the max allowable typename lengtt to over twice that for good measure.
            // DQ (7/22/2011): The a992-thrifty-mips-compiler Hudson test fails because it generates a 
            // typename that is even longer 5149, so we need an even larger upper bound.  This should be 
            // looked into later to see why some of these different platforms are generating such large 
            // typenames. See testcode: tests/CompileTests/PythonExample_tests/test2004_92.C (on thrifty).
            // if (typeNameString.length() > 10000)
            // if (typeNameString.length() > 40000)
               if (typeNameString.length() > 400000)
                  {
                 // If your ever curious, you can output the type name.
                 // printf ("Error: typeNameString = %s \n",typeNameString.c_str());

                    printf ("Error: type names should not be this long... (even in boost, I think) typeNameString.length() = %" PRIuPTR " \n",typeNameString.length());
                    ROSE_ASSERT(false);
                  }
#if 0
            // DQ (2/18/2013): I think that the output if such long strings in a problem for the Jenkins tests, 
            // outside of Jenkins this branch executes fine.  This branch is executed only for ROSE compiling
            // ROSE (so far) and is executed for the typename generated from the enum in Cxx_Grammar.h which 
            // is converted to a unique typename (and the string function for this contatinates all of the names
            // of the 700+ enum fields to generate the typename, hence a long name exceeding 10K characters).
            // ROSE compiling the ROSE Cxx_Grammar.h file is not a problem outside of Jenkins so I suspect that
            // Jenkins is having problems with the processing of long strings generated as part of the tests.

            // DQ (1/30/2013): Print out the long name that previously violated our initial limits.
               if (typeNameString.length() > 10000)
                  {
                    printf ("WARNING: extremely long type name found: typeNameString.length() = %" PRIuPTR " \n",typeNameString.length());
                    printf ("typeNameString = %s \n",typeNameString.c_str());
                  }
#endif
             }

       // DQ (6/21/2011): Refactored this code for use in traverseTemplatedFunction()
          addToNameMap(nodeReferenceToType,typeNameString);

       // DQ (2/18/2013): Fixing generation of too many SgUnparse_Info object.
          delete unparseInfoPointer;
        }
       else
        {
       // Output a message when we cheat on this IR node (even if this is a clue for George).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 0)
          printf ("Skipping precompuation of string for name qualified type = %p = %s \n",type,type->class_name().c_str());
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("<<<<< Ending traversal of type: type = %p = %s \n",type,type->class_name().c_str());
#endif
   }


void
NameQualificationTraversal::traverseTemplatedFunction(SgFunctionRefExp* functionRefExp, SgNode* nodeReference, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // Called using traverseTemplatedFunction(functionRefExp,templateInstantiationFunctionDeclaration,currentScope,currentStatement)

     ROSE_ASSERT(functionRefExp    != NULL);
     ROSE_ASSERT(nodeReference     != NULL);
     ROSE_ASSERT(currentScope      != NULL);
     ROSE_ASSERT(positionStatement != NULL);

  // printf ("Inside of traverseTemplatedFunction functionRefExp = %p currentScope = %p = %s \n",functionRefExp,currentScope,currentScope->class_name().c_str());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("<<<<< Starting traversal of traverseTemplatedFunction functionRefExp = %p currentScope = %p = %s \n",functionRefExp,currentScope,currentScope->class_name().c_str());
#endif
     
     bool skipThisFunction = false;
     if (skipThisFunction == false)
        {
          SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(functionRefExp->getAssociatedFunctionDeclaration());
          if (templateInstantiationFunctionDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Found a SgTemplateInstantiationFunctionDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationFunctionDeclaration->get_name().str());
#endif
               evaluateTemplateInstantiationDeclaration(templateInstantiationFunctionDeclaration,currentScope,positionStatement);
             }

          SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
          ROSE_ASSERT (unparseInfoPointer != NULL);
          unparseInfoPointer->set_outputCompilerGeneratedStatements();

       // Avoid unpasing the class definition when unparseing the type.
          unparseInfoPointer->set_SkipClassDefinition();

       // DQ (1/13/2014): Set the output of the enum defintion to match that of the class definition (consistancy is now inforced).
          unparseInfoPointer->set_SkipEnumDefinition();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("nodeReference = %p = %s \n",nodeReference,nodeReference->class_name().c_str());
#endif
       // Associate the unparsing of this type with the statement or scope where it occures.
       // This is the key to use in the lookup of the qualified name. But this is the correct key....
       // unparseInfoPointer->set_reference_node_for_qualification(positionStatement);
       // unparseInfoPointer->set_reference_node_for_qualification(currentScope);
          unparseInfoPointer->set_reference_node_for_qualification(nodeReference);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Calling globalUnparseToString() \n");
#endif
          string functionNameString = globalUnparseToString(functionRefExp,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("++++++++++++++++ functionNameString (globalUnparseToString()) = %s \n",functionNameString.c_str());
#endif
#if 0
          printf ("++++++++++++++++ functionNameString (globalUnparseToString()) = %s \n",functionNameString.c_str());
#endif

       // DQ (10/31/2015): Increased the maximum allowable size of function names (because test2015_98.C 
       // demonstrates a longer name (length == 5062)).
       // DQ (6/24/2013): Increased upper bound to support ROSE compiling ROSE.
       // This is symptematic of an error which causes the whole class to be included with the class 
       // definition.  This was fixed by calling unparseInfoPointer->set_SkipClassDefinition() above.
       // if (functionNameString.length() > 2000)
       // if (functionNameString.length() > 5000)
          if (functionNameString.length() > 10000)
             {
               printf ("Error: function names should not be this long... functionNameString.length() = %" PRIuPTR " \n",functionNameString.length());
#if 1
               printf ("Error: function names should not be this long... functionNameString          = \n%s \n",functionNameString.c_str());
#endif
               ROSE_ASSERT(false);
             }

       // DQ (6/21/2011): Refactored this code for use in traverseTemplatedFunction()
          addToNameMap(nodeReference,functionNameString);

       // DQ (2/18/2013): Fixing generation of too many SgUnparse_Info object.
          delete unparseInfoPointer;
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("<<<<< Ending traversal of traverseTemplatedFunction functionRefExp = %p currentScope = %p = %s \n",functionRefExp,currentScope,currentScope->class_name().c_str());
#endif
   }


void
NameQualificationTraversal::traverseTemplatedMemberFunction(SgMemberFunctionRefExp* memberFunctionRefExp, SgNode* nodeReference, SgScopeStatement* currentScope, SgStatement* positionStatement )
   {
  // Called using traverseTemplatedFunction(functionRefExp,templateInstantiationFunctionDeclaration,currentScope,currentStatement)

     ROSE_ASSERT(memberFunctionRefExp != NULL);
     ROSE_ASSERT(nodeReference        != NULL);
     ROSE_ASSERT(currentScope         != NULL);
     ROSE_ASSERT(positionStatement    != NULL);

  // printf ("Inside of traverseTemplatedFunction functionRefExp = %p currentScope = %p = %s \n",functionRefExp,currentScope,currentScope->class_name().c_str());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("<<<<< Starting traversal of traverseTemplatedFunction memberFunctionRefExp = %p currentScope = %p = %s \n",memberFunctionRefExp,currentScope,currentScope->class_name().c_str());
#endif
     
     bool skipThisFunction = false;
     if (skipThisFunction == false)
        {
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionRefExp->getAssociatedMemberFunctionDeclaration());
          if (templateInstantiationMemberFunctionDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Found a SgTemplateInstantiationMemberFunctionDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationMemberFunctionDeclaration->get_name().str());
#endif
               evaluateTemplateInstantiationDeclaration(templateInstantiationMemberFunctionDeclaration,currentScope,positionStatement);
             }

          SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
          ROSE_ASSERT (unparseInfoPointer != NULL);
          unparseInfoPointer->set_outputCompilerGeneratedStatements();

       // Avoid unpasing the class definition when unparsing the type.
          unparseInfoPointer->set_SkipClassDefinition();

       // DQ (1/13/2014): Set the output of the enum defintion to match that of the class definition (consistancy is now inforced).
          unparseInfoPointer->set_SkipEnumDefinition();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("nodeReference = %p = %s \n",nodeReference,nodeReference->class_name().c_str());
#endif
       // Associate the unparsing of this type with the statement or scope where it occures.
       // This is the key to use in the lookup of the qualified name. But this is the correct key....
       // unparseInfoPointer->set_reference_node_for_qualification(positionStatement);
       // unparseInfoPointer->set_reference_node_for_qualification(currentScope);
          unparseInfoPointer->set_reference_node_for_qualification(nodeReference);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Calling globalUnparseToString() \n");
#endif
          string memberFunctionNameString = globalUnparseToString(memberFunctionRefExp,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("++++++++++++++++ memberFunctionNameString (globalUnparseToString()) = %s \n",memberFunctionNameString.c_str());
#endif
#if 0
          printf ("++++++++++++++++ memberFunctionNameString (globalUnparseToString()) = %s \n",memberFunctionNameString.c_str());
#endif
       // DQ (12/3/2014): Incremented this for ARES application files.
       // DQ (6/9/2013): I have incremented this value to support mangled names in the protobuf-2.5.0 application.
       // This is symptematic of an error which causes the whole class to be included with the class 
       // definition.  This was fixed by calling unparseInfoPointer->set_SkipClassDefinition() above.
       // if (memberFunctionNameString.length() > 4000)
          if (memberFunctionNameString.length() > 8000)
             {
               printf ("Error: function names should not be this long... memberFunctionNameString.length() = %" PRIuPTR " \n",memberFunctionNameString.length());
#if 1
               printf ("Error: function names should not be this long... memberFunctionNameString = \n%s \n",memberFunctionNameString.c_str());
#endif
               ROSE_ASSERT(false);
             }

       // DQ (6/21/2011): Refactored this code for use in traverseTemplatedFunction()
          addToNameMap(nodeReference,memberFunctionNameString);

       // DQ (2/18/2013): Fixing generation of too many SgUnparse_Info object.
          delete unparseInfoPointer;
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("<<<<< Ending traversal of traverseTemplatedMemberFunction memberFunctionRefExp = %p currentScope = %p = %s \n",memberFunctionRefExp,currentScope,currentScope->class_name().c_str());
#endif
   }



bool
NameQualificationTraversal::skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(SgDeclarationStatement* declaration)
   {
  // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in 
  // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
  // definition could not live in the SgBasicBlock.

     bool skipNameQualification = false;
     SgDeclarationStatement* declarationToSearchForInReferencedNameSet = declaration->get_firstNondefiningDeclaration() != NULL ? declaration->get_firstNondefiningDeclaration() : declaration;
     ROSE_ASSERT(declarationToSearchForInReferencedNameSet != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(): declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
     printf ("   --- declarationToSearchForInReferencedNameSet->get_parent() = %p = %s \n",declarationToSearchForInReferencedNameSet->get_parent(),declarationToSearchForInReferencedNameSet->get_parent()->class_name().c_str());

     printf ("   --- declaration                                             = %p = %s \n",declaration,declaration->class_name().c_str());
     printf ("   --- declaration->get_parent()                               = %p = %s \n",declaration->get_parent(),declaration->get_parent()->class_name().c_str());
     if (declaration->get_firstNondefiningDeclaration() != NULL)
        {
          printf ("   --- declaration ->get_firstNondefiningDeclaration()              = %p = %s \n",declaration->get_firstNondefiningDeclaration(),declaration->get_firstNondefiningDeclaration()->class_name().c_str());
          printf ("   --- declaration->get_firstNondefiningDeclaration()->get_parent() = %p = %s \n",declaration->get_firstNondefiningDeclaration()->get_parent(),declaration->get_firstNondefiningDeclaration()->get_parent()->class_name().c_str());
        }
     if (declaration->get_definingDeclaration() != NULL)
        {
          printf ("   --- declaration ->get_definingDeclaration()                      = %p = %s \n",declaration->get_definingDeclaration(),declaration->get_definingDeclaration()->class_name().c_str());
          printf ("   --- declaration->get_definingDeclaration()->get_parent()         = %p = %s \n",declaration->get_definingDeclaration()->get_parent(),declaration->get_definingDeclaration()->get_parent()->class_name().c_str());
        }
#endif
#if 0
     for (std::set<SgNode*>::iterator i = referencedNameSet.begin(); i != referencedNameSet.end(); i++)
        {
          printf ("   --- *** referencedNameSet member *i = %p = %s \n",*i,(*i)->class_name().c_str());
        }
#endif

  // DQ (8/18/2012): If this is a template instantiation, then we need to look at where the template declaration is and if IT is defined.
  // See test2009_30.C for an example of this.
     SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(declaration);
     if (templateInstantiationFunctionDecl != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(): templateInstantiationFunctionDecl->get_name() = %p = %s \n",templateInstantiationFunctionDecl,templateInstantiationFunctionDecl->get_name().str());
#endif

       // DQ (8/18/2012): Note that test2012_57.C and test2012_59.C have template specalizations that don't appear 
       // to have there associated template declaration set properly, issue a warning for now.
       // declarationToSearchForInReferencedNameSet = templateInstantiationFunctionDecl->get_templateDeclaration();
          if (templateInstantiationFunctionDecl->get_templateDeclaration() == NULL)
             {
               printf ("WARNING: templateInstantiationFunctionDecl->get_templateDeclaration() == NULL for templateInstantiationFunctionDecl = %p = %s \n",templateInstantiationFunctionDecl,templateInstantiationFunctionDecl->get_name().str());
             }
            else
             {
               declarationToSearchForInReferencedNameSet = templateInstantiationFunctionDecl->get_templateDeclaration();
             }
          ROSE_ASSERT(declarationToSearchForInReferencedNameSet != NULL);
        }
       else
        {
       // Also test for member function.
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(declaration);
          if (templateInstantiationMemberFunctionDecl != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("In skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(): templateInstantiationMemberFunctionDecl->get_name() = %p = %s \n",templateInstantiationMemberFunctionDecl,templateInstantiationMemberFunctionDecl->get_name().str());
#endif
               declarationToSearchForInReferencedNameSet = templateInstantiationMemberFunctionDecl->get_templateDeclaration();
               ROSE_ASSERT(declarationToSearchForInReferencedNameSet != NULL);
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("In skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(): This is not a template function instantation (member nor non-member function) \n");
#endif
             }
        }

  // DQ (6/22/2011): This fixes test2011_97.C which only has a defining declaration so that the declaration->get_firstNondefiningDeclaration() was NULL.
  // if (referencedNameSet.find(declaration->get_firstNondefiningDeclaration()) == referencedNameSet.end())
     if (referencedNameSet.find(declarationToSearchForInReferencedNameSet) == referencedNameSet.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("   --- $$$$$$$$$$ NOT Found: declaration %p = %s in referencedNameSet referencedNameSet.size() = %" PRIuPTR " \n",declaration,declaration->class_name().c_str(),referencedNameSet.size());
#endif
          skipNameQualification = true;
        }
       else
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("   --- $$$$$$$$$$ FOUND: declaration %p = %s in referencedNameSet \n",declaration,declaration->class_name().c_str());
#endif
        }

     return skipNameQualification;
   }


NameQualificationInheritedAttribute
NameQualificationTraversal::evaluateInheritedAttribute(SgNode* n, NameQualificationInheritedAttribute inheritedAttribute)
   {
     ROSE_ASSERT(n != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("\n\n****************************************************** \n");
     printf ("****************************************************** \n");
     printf ("Inside of NameQualificationTraversal::evaluateInheritedAttribute(): node = %p = %s = %s \n",n,n->class_name().c_str(),SageInterface::get_name(n).c_str());
     printf ("****************************************************** \n");
#endif

  // DQ (5/24/2013): Allow the current scope to be tracked from the traversal of the AST
  // instead of being computed at each IR node which is a problem for template arguments.
  // See test2013_187.C for an example of this.
     SgScopeStatement* evaluateInheritedAttribute_currentScope = isSgScopeStatement(n);
     if (evaluateInheritedAttribute_currentScope != NULL)
        {
          inheritedAttribute.set_currentScope(evaluateInheritedAttribute_currentScope);
        }

  // DQ (5/24/2013): We can't set the current scope until we at first get past the SgProject and SgSourceFile IR nodes in the AST traversal.
     if (isSgSourceFile(n) == NULL && isSgProject(n) == NULL)
        {
       // DQ (5/25/2013): This only appears to fail for test2013_63.C.
          if (inheritedAttribute.get_currentScope() == NULL)
             {
               printf ("WARNING: In NameQualificationTraversal::evaluateInheritedAttribute(): inheritedAttribute.get_currentScope() == NULL: node = %p = %s = %s \n",n,n->class_name().c_str(),SageInterface::get_name(n).c_str());
             }
       // ROSE_ASSERT(inheritedAttribute.get_currentScope() != NULL);
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
  // Extra information about the location of the current node.
     Sg_File_Info* fileInfo = n->get_file_info();
     if (fileInfo != NULL)
        {
          printf ("NameQualificationTraversal: --- n = %p = %s line %d col = %d file = %s \n",n,n->class_name().c_str(),fileInfo->get_line(),fileInfo->get_col(),fileInfo->get_filenameString().c_str());
        }
#endif

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
  //  19) SgVarRefExp's hidden in array types (SgArrayType) (requires explicitly specified current scope).
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

  // DQ (8/4/2012): It is too complex to add this declaration support here (use the previous code and just
  // handle the specific cases where we need to add a declaration to the reference set spereately.
  // DQ (8/4/2012): Let any procesing define a declaration to be used for the reference set.
  // Ititially it is NULL, but specific cases can set this so that the associated declaration 
  // we be recorded as referenced.  This is important for test2012_164.C, where a variable declaration
  // generates a reference to a type that at first must not use a qualified name (since there is no
  // explicit forward declaration for the type (a class/struct).  This is a facinating case since
  // the scope of the declaration is the outer namespace from where it is first implicitly referenced 
  // via a variable declaration.  This is part of debugging test2005_133.C (of which test2012_16[3-5].C
  // are simpler cases.  
  // SgDeclarationStatement* declarationForReferencedNameSet = NULL;

  // DQ (6/11/2011): This is a new IR nodes, but the use of it causes a few problems (test2004_109.C) 
  // because the source position is not computed correctly (I think).
     SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(n);
     if (templateClassDefinition != NULL)
        {
#if 0
       // DQ (3/5/2012): I think this is OK, since the SgTemplateClassDefinition is derived from the SgClassDefinition, 
       // that case below will work well. This example is demonstrated in test2012_10.C.
       // DQ (11/20/2011): Debugging new use of SgTemplateClassDefinition (only used in new version of EDG 4.x support).
          printf ("Name qualification of SgTemplateClassDefinition = %p not implemented, OK (no special handling is required, we be processed as a SgClassDefinition) (need and example to debug this case) \n",templateClassDefinition);
#endif

       // DQ (11/20/2011): Commented out this assertion.
       // ROSE_ASSERT(false);
        }

     SgClassDefinition* classDefinition = isSgClassDefinition(n);
  // if (classDefinition != NULL && templateClassDefinition == NULL)
     if (classDefinition != NULL)
        {
       // Add all of the named types from this class into the set that have already been seen.
       // Note that this should not include nested classes (I think).

          SgBaseClassPtrList & baseClassList = classDefinition->get_inheritances();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("!!!!! Evaluate the derived classes: are they visible --- baseClassList.size() = %" PRIuPTR " \n",baseClassList.size());
#endif
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Calling nameQualificationDepth() for base class classDeclaration name = %s \n",classDeclaration->get_name().str());
#endif
               int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,classDefinition);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("amountOfNameQualificationRequired (base class) = %d \n",amountOfNameQualificationRequired);
#endif
               setNameQualification(baseClass,classDeclaration,amountOfNameQualificationRequired);

            // DQ (12/23/2015): Also need to add this to the aliasSymbolCausalNodeSet.
               SgSymbolTable::get_aliasSymbolCausalNodeSet().insert(baseClass);

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
                 // DQ (1/21/2013): We should be able to assert this.
                    ROSE_ASSERT(classDeclaration->get_scope() != NULL);
#if 1
                 // DQ (1/21/2013): Added new static function to support testing for equivalent when the scopes are namespaces.
                    bool isSameNamespace = SgScopeStatement::isEquivalentScope(currentScope,classDeclaration->get_scope());
#else
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
                    printf ("classDeclaration->get_scope() = %p = %s \n",classDeclaration->get_scope(),classDeclaration->get_scope()->class_name().c_str());
#endif
                 // DQ (1/21/2013): If these are both namespace scopes then do a more sophisticated test for equivalent namespace.
                 // Note that over qualification of generated code can be an error.
                    SgNamespaceDefinitionStatement* currentNamespaceDefinition = isSgNamespaceDefinitionStatement(currentScope);
                    SgNamespaceDefinitionStatement* classDeclarationNamespaceDefinition = isSgNamespaceDefinitionStatement(classDeclaration->get_scope());
                    bool isSameNamespace = false;
                    if (currentNamespaceDefinition != NULL && classDeclarationNamespaceDefinition != NULL)
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("These are both in namespaces and we need to check if it is the same namespace (instead of just matching scope pointers) \n");
#endif
                         SgNamespaceDeclarationStatement* currentNamespaceDeclaration = isSgNamespaceDeclarationStatement(currentNamespaceDefinition->get_namespaceDeclaration());
                         SgNamespaceDeclarationStatement* classDeclarationNamespaceDeclaration = isSgNamespaceDeclarationStatement(classDeclarationNamespaceDefinition->get_namespaceDeclaration());

                      // Test for equivalent namespaces.
                         if (currentNamespaceDeclaration->get_firstNondefiningDeclaration() == classDeclarationNamespaceDeclaration->get_firstNondefiningDeclaration())
                            {
                              isSameNamespace = true;
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("These namespaces are different \n");
#endif
                            }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("SgClassDeclaration: (detected equivalent namespaces) isSameNamespace = %s \n",isSameNamespace ? "true" : "false");
#endif
                       }
#endif
                 // DQ (1/21/2013): Added code to support when equivalent namespaces are detected.
                    if (isSameNamespace == false)
                       {
                      // DQ (6/11/2013): Added test to make sure that name qualification is ignored for friend function where the class has not yet been seen.
                      // if (classDeclaration->get_declarationModifier().isFriend() == false)
                         SgDeclarationStatement* declarationForReferencedNameSet = classDeclaration->get_firstNondefiningDeclaration();
                         ROSE_ASSERT(declarationForReferencedNameSet != NULL);
                         if (referencedNameSet.find(declarationForReferencedNameSet) != referencedNameSet.end())
                            {
                              int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,classDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("SgClassDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                              setNameQualification(classDeclaration,amountOfNameQualificationRequired);
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("This classDeclaration has not been seen before so skip the name qualification \n");
#endif
                            }
                       }
                  }
                 else
                  {
                 // Don't know what test code exercises this case (see test2011_62.C).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("WARNING: SgClassDeclaration -- currentScope is not available through predicate (currentScope != classDeclaration->get_scope()), not clear why! \n");
#endif
                    ROSE_ASSERT(classDeclaration->get_parent() == classDeclaration->get_scope());
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("name qualification for classDeclaration->get_scope()  = %p = %s \n",classDeclaration->get_scope(),classDeclaration->get_scope()->class_name().c_str());
                    printf ("classDeclaration->get_parent()                        = %p = %s \n",classDeclaration->get_parent(),classDeclaration->get_parent()->class_name().c_str());
#endif
                 // ROSE_ASSERT(false);
                  }
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("WARNING: SgClassDeclaration -- currentScope is not available, not clear why! \n");
#endif
            // ROSE_ASSERT(false);
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
       // DQ (9/23/2012): We need to handle the template arguments associate with this template instantiation.
          SgTemplateInstantiationDecl* templateClassInstantiationDeclaration = isSgTemplateInstantiationDecl(classDeclaration);
          if (templateClassInstantiationDeclaration != NULL)
             {
            // There are template parameters that may require name qualification.
               printf ("WARNING: There are template parameters that may require name qualification. templateClassInstantiationDeclaration = %p \n",templateClassInstantiationDeclaration);

               printf ("  --- templateClassInstantiationDeclaration->get_firstNondefiningDeclaration() = %p \n",templateClassInstantiationDeclaration->get_firstNondefiningDeclaration());
               printf ("  --- templateClassInstantiationDeclaration->get_definingDeclaration()         = %p \n",templateClassInstantiationDeclaration->get_definingDeclaration());

               SgTemplateArgumentPtrList & l = templateClassInstantiationDeclaration->get_templateArguments();
               for (SgTemplateArgumentPtrList::iterator i = l.begin(); i != l.end(); i++)
                  {
                    printf ("  --- template argument = %p = %s \n",*i,(*i)->class_name().c_str());
                  }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#endif
        }

  // Handle the types used in variable declarations...
  // A problem with this implementation is that it relies on there being one SgInitializedName per SgVariableDeclaration.
  // This is currently the case for C++, but we would like to fix this.  It is not clear if the SgInitializedName should carry its
  // own qualification or not (this violates the idea that the IR node that has the reference stored the name qualification data).
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(n);
     if (variableDeclaration != NULL)
        {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
       // DQ (8/24/2012): Allow this to be output a little less often.
       // DQ (8/4/2012): Why is this case procesed if the SgInitializedNames are processed sperately (this appears to be redundant with that).
          static int counter = 0;
          if (counter++ % 100 == 0)
             {
               printf ("QUESTION: Why is this case of name qualification SgVariableDeclaration procesed if the SgInitializedNames are processed sperately (this appears to be redundant with that). \n");
             }
#endif
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);
          ROSE_ASSERT(initializedName != NULL);

       // This is not always the correct current scope (see test2011_70.C for an example).
          SgScopeStatement* currentScope = SageInterface::getScope(variableDeclaration);
       // SgScopeStatement* currentScope = isSgScopeStatement(variableDeclaration->get_parent());
          ROSE_ASSERT(currentScope != NULL);

       // DQ (5/24/2013): This should be the same scope as what is in the inherited attribute, I think.
       // This fails for test2013_186.C, I expect it is because one of these is NULL (allowed for some nested traversals).
          if (currentScope != inheritedAttribute.get_currentScope())
             {
               if (currentScope != NULL && inheritedAttribute.get_currentScope() != NULL)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 1)
                    printf ("WARNING: currentScope != inheritedAttribute.get_currentScope(): currentScope = %p = %s inheritedAttribute.get_currentScope() = %p = %s \n",
                         currentScope,currentScope->class_name().c_str(),inheritedAttribute.get_currentScope(),inheritedAttribute.get_currentScope()->class_name().c_str());
#endif
                  }
             }
       // ROSE_ASSERT(currentScope == inheritedAttribute.get_currentScope());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("================ Calling nameQualificationDepthForType to evaluate the type \n");
#endif
       // Compute the depth of name qualification from the current statement:  variableDeclaration.
       // int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,variableDeclaration);
          int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentScope,variableDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("SgVariableDeclaration's type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForType);
#endif
       // Not all types have an associated declaration, but some do: examples include classes, typedefs, and enums.
          SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());
          if (declaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Putting the name qualification for the type into the SgInitializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif


#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif

#if 1
            // **************************************************
            // DQ (8/4/2012): The type being used might not have to be qualified if it is associated with a SgClassDeclaration that has not been defined yet.
            // **************************************************
               bool skipGlobalNameQualification = skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(declaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Test of Type used in SgVariableDeclaration: skipGlobalNameQualification = %s \n",skipGlobalNameQualification ? "true" : "false");
#endif
#if 1
               setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType,skipGlobalNameQualification);
#else

#error "DEAD CODE!"

               if (skipGlobalNameQualification == false)
                  {
                    setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType);
                  }
#endif
            // **************************************************
#else

#error "DEAD CODE!"

               setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType);
#endif
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("declaration == NULL: could not put name qualification for the type into the SgInitializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("\n++++++++++++++++ Calling nameQualificationDepth to evaluate the name currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
          int amountOfNameQualificationRequiredForName = nameQualificationDepth(initializedName,currentScope,variableDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("SgVariableDeclaration's variable name: amountOfNameQualificationRequiredForName = %d \n",amountOfNameQualificationRequiredForName);
#endif

       // DQ (6/5/2011): Debugging test2011_75.C ...

       // If there is a previous declaration then that is the original declaration (see test2011_30.C for an example of this case).
          SgInitializedName* originalInitializedName = initializedName->get_prev_decl_item(); 
       // SgInitializedName* originalInitializedName = initializedName;
#if 0
          printf ("Case of SgInitializedName: originalInitializedName = %p \n",originalInitializedName);
#endif
          if (originalInitializedName != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("originalInitializedName = %p = %s \n",originalInitializedName,originalInitializedName->get_name().str());
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("WARNING: variableDeclaration case of name qualification originalVariableDeclaration == NULL (building alternativeDecaration) \n");
#endif
                         SgFunctionDeclaration* alternativeDecaration = TransformationSupport::getFunctionDeclaration(originalInitializedName->get_parent());

                      // DQ (7/19/2012): Allow this to be NULL (see test2005_103.C), at least for testing.
                      // ROSE_ASSERT(alternativeDecaration != NULL);
                      // setNameQualification(variableDeclaration,alternativeDecaration,amountOfNameQualificationRequiredForName);
                         if (alternativeDecaration != NULL)
                            {
                              setNameQualification(variableDeclaration,alternativeDecaration,amountOfNameQualificationRequiredForName);
                            }
                           else
                            {
                              mprintf ("Warning: In name qualification support: alternativeDecaration == NULL (see test2005_103.C) \n");
                            }
                       }
                  }
                 else
                  {
                 // This is demonstrated by test2011_72.C (and maybe test2005_103.C).  I can't really fix this in the AST 
                 // until I can get all of the test codes back to passing with the new name qualification support, so this 
                 // will have to wait.  We output an error message for now.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Error: originalInitializedName->get_parent() == NULL (skipping any required name qualification, I think this is a bug in the AST) \n");   
#endif
                  }
             }

       // DQ (7/24/2011): if there is a bit-field width specifier then it could contain variable references that require name qualification.
          SgVariableDefinition* variableDefinition = isSgVariableDefinition(initializedName->get_declptr());
          if (variableDefinition != NULL)
             {
               SgExpression* bitFieldWidthSpecifier = variableDefinition->get_bitfield();
               if (bitFieldWidthSpecifier != NULL)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Traverse the bitFieldWidthSpecifier and add any required name qualification.\n");
#endif
                    generateNestedTraversalWithExplicitScope(bitFieldWidthSpecifier,currentScope);
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("++++++++++++++++ DONE: Calling nameQualificationDepth to evaluate the name \n\n");
#endif
        }


  // DQ (8/23/2014): Adding more uniform support for SgInitializedName objects by supporting the SgFunctionParameterList (similar to the SgVariableDeclaration).
     SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(n);
     if (functionParameterList != NULL)
        {
#if 0
          printf ("Case of SgFunctionParameterList: functionParameterList = %p \n",functionParameterList);
#endif
          SgScopeStatement* currentScope = SageInterface::getScope(functionParameterList);

       // DQ (8/29/2014): This is a result of a transformation in the tutorial (codeCoverage.C) that does not appear to be implemeting a transformation correctly.
          if (currentScope == NULL)
             {
               printf ("Error: currentScope == NULL: functionParameterList = %p \n",functionParameterList);
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionParameterList->get_parent());
               ROSE_ASSERT(functionDeclaration != NULL);
               printf ("Error: currentScope == NULL: functionDeclaration = %p = %s name = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
               ROSE_ASSERT(functionDeclaration->get_file_info() != NULL);
               functionDeclaration->get_file_info()->display("Error: currentScope == NULL: functionParameterList->get_parent(): debug");
               SgScopeStatement* temp_scope = SageInterface::getScope(functionDeclaration);
               ROSE_ASSERT(temp_scope != NULL);

            // DQ (8/29/2014): It appears that we can't ask the SgFunctionParameterList for it's scope, but we can find the SgFunctionDeclaration from the parent and ask it; so this should be fixed.
               currentScope = temp_scope;

               printf ("It appears that in the case of a transforamtion, we can't always ask the SgFunctionParameterList for it's scope, but we can find the SgFunctionDeclaration from the parent and ask it; so this should be fixed. \n");
             }
          ROSE_ASSERT(currentScope != NULL);

       // DQ (8/24/2014): This should be the same scope as what is in the inherited attribute, I think.
       // This fails for test2013_186.C, I expect it is because one of these is NULL (allowed for some nested traversals).
          if (currentScope != inheritedAttribute.get_currentScope())
             {
               if (currentScope != NULL && inheritedAttribute.get_currentScope() != NULL)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 1)
                    printf ("WARNING: currentScope != inheritedAttribute.get_currentScope(): currentScope = %p = %s inheritedAttribute.get_currentScope() = %p = %s \n",
                         currentScope,currentScope->class_name().c_str(),inheritedAttribute.get_currentScope(),inheritedAttribute.get_currentScope()->class_name().c_str());
#endif
                  }
             }
       // ROSE_ASSERT(currentScope == inheritedAttribute.get_currentScope());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("================ Calling nameQualificationDepthForType to evaluate the type \n");
#endif

          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionParameterList->get_parent());
          ROSE_ASSERT(functionDeclaration != NULL);

          SgInitializedNamePtrList::iterator i = functionParameterList->get_args().begin();
          while (i != functionParameterList->get_args().end())
             {
               SgInitializedName* initializedName = *i;
               ROSE_ASSERT(initializedName != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("================ Calling nameQualificationDepthForType to evaluate the type for initializedName = %p \n",initializedName);
#endif
            // Compute the depth of name qualification from the current statement:  variableDeclaration.
               int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentScope,functionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("SgFunctionParameterList's type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForType);
               printf ("   --- initializedName->get_type() = %p = %s \n",initializedName->get_type(),initializedName->get_type()->class_name().c_str());
#endif
            // Not all types have an associated declaration, but some do: examples include classes, typedefs, and enums.
               SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());
               if (declaration != NULL)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Putting the name qualification for the type into the SgInitializedName = %p = %s \n",initializedName,initializedName->get_name().str());
                    printf ("   --- getDeclarationAssociatedWithType(): declaration                 = %p = %s \n",declaration,declaration->class_name().c_str());
#endif

                 // **************************************************
                 // DQ (8/4/2012): The type being used might not have to be qualified if it is associated with a SgClassDeclaration that has not been defined yet.
                 // **************************************************
                    bool skipGlobalNameQualification = false; // skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(declaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Test of Type used in SgVariableDeclaration: skipGlobalNameQualification = %s \n",skipGlobalNameQualification ? "true" : "false");
#endif
                    setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType,skipGlobalNameQualification);
                  }

               i++;
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }


  // Handle SgType name qualification where SgInitializedName's appear outside of SgVariableDeclaration's (e.g. in function parameter declarations).
     SgInitializedName* initializedName = isSgInitializedName(n);
     if (initializedName != NULL)
        {
       // We want to handle types from every where a SgInitializedName might be used.
          SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(initializedName->get_type());
#if 0
          printf ("Case of SgInitializedName: getDeclarationAssociatedWithType(): type = %p = %s declaration = %p \n",initializedName->get_type(),initializedName->get_type()->class_name().c_str(),declaration);
#endif
          if (declaration != NULL)
             {
               SgStatement* currentStatement = TransformationSupport::getStatement(initializedName);
               ROSE_ASSERT(currentStatement != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("case of SgInitializedName: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif
               SgScopeStatement* currentScope = currentStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("case of SgInitializedName: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
            // int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentScope,currentStatement);
            // int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentStatement);
               int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentScope,currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("SgInitializedName's (%s) type: amountOfNameQualificationRequiredForType = %d \n",initializedName->get_name().str(),amountOfNameQualificationRequiredForType);
#endif

#if 1
            // DQ (8/4/2012): This is redundant code with where the SgInitializedName appears in the SgVariableDeclaration.
            // **************************************************
            // DQ (8/4/2012): The type being used might not have to be qualified if it is associated with a SgClassDeclaration 
            // that has not been defined yet.  This fixes test2012_165.C.
            // **************************************************
               bool skipGlobalNameQualification = skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(declaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("case of SgInitializedName: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif

            // DQ (8/4/2012): However, this quasi-pathological case does not apply to template instantiations 
            // (only non-template classes or maybe named types more generally?).  Handle template declarations similarly.
            // OR enum declarations (since they can have a forward declaration (except that this is a common languae extension...).
               if (isSgTemplateInstantiationDecl(declaration) != NULL || isSgEnumDeclaration(declaration) != NULL)
                  {
                 // Do the regularly schedule name qualification for these cases.
                    skipGlobalNameQualification = false;
                  }
                 else
                  {
                 // Look back through the scopes and see if we are in a template instantiation or template scope, 
                 // if so then do the regularly scheduled name qualification.

                    SgScopeStatement* scope = declaration->get_scope();
                 // printf ("case of SgInitializedName: scope = %p = %s \n",scope,scope->class_name().c_str());
                    int distanceBackThroughScopes = amountOfNameQualificationRequiredForType;
                 // printf ("case of SgInitializedName: distanceBackThroughScopes = %d \n",distanceBackThroughScopes);
                    while (distanceBackThroughScopes > 0 && scope != NULL)
                       {
                      // Traverse backwards through the scopes checking for a SgTemplateClassDefinition scope
                      // If we traverse off the end of SgGlobal then the amountOfNameQualificationRequiredForType 
                      // value was trying to trigger global qualification, so this is not a problem.
                      // We at least need isSgTemplateInstantiationDefn, not clear about isSgTemplateClassDefinition.

                         if (isSgTemplateInstantiationDefn(scope) != NULL || isSgTemplateClassDefinition(scope) != NULL)
                            {
                              skipGlobalNameQualification = false;
                            }

                         ROSE_ASSERT(scope != NULL);
                         scope = scope->get_scope();

                         distanceBackThroughScopes--;
                       }
                  }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Test of Type used in SgInitializedName: declaration = %p = %s skipGlobalNameQualification = %s \n",declaration,declaration->class_name().c_str(),skipGlobalNameQualification ? "true" : "false");
#endif

            // DQ (8/4/2012): Added support to permit global qualification be be skipped explicitly (see test2012_164.C and test2012_165.C for examples where this is important).
            // setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType,skipGlobalNameQualification);

            // DQ (12/17/2013): Added support for name qualification of preinitialization list elements (see test codes: test2013_285-288.C).
               if (initializedName->get_initptr() != NULL)
                  {
                    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializedName->get_initptr());
                 // ROSE_ASSERT(constructorInitializer != NULL);
                    if (constructorInitializer != NULL)
                       {
                      // SgType* type = initializedName->get_type();
                         SgType* type = constructorInitializer->get_type();
                         ROSE_ASSERT(type != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("Test for special case of SgInitializedName used in SgCtorInitializerList: type = %p = %s \n",type,type->class_name().c_str());
#endif
                         SgFunctionType* functionType             = isSgFunctionType(type);
                         SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(type);

                         SgCtorInitializerList* ctor = isSgCtorInitializerList(currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("Test for special case of SgInitializedName used in SgCtorInitializerList: ctor = %p functionType = %p memberFunctionType = %p \n",ctor,functionType,memberFunctionType);
#endif
                      // if (ctor != NULL)
                         if (ctor != NULL && (functionType != NULL || memberFunctionType != NULL))
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Calling setNameQualificationOnName() (operating DIRECTLY on the SgInitializedName) \n");
#endif
                           // DQ (1/13/2014): This only get's qualification when the name being used matches the class name, 
                           // else this is a data member and should not be qualified.  See test2014_01.C.
                           // SgName functionName = (functionType != NULL) ? functionType->get_name() : memberFunctionType->get_name();
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(constructorInitializer->get_declaration());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Test for special case of SgInitializedName used in SgCtorInitializerList: functionDeclaration = %p \n",functionDeclaration);
#endif
                              SgName functionName = functionDeclaration->get_name();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Test for special case of SgInitializedName used in SgCtorInitializerList: functionName = %s \n",functionName.str());
                              printf ("Test for special case of SgInitializedName used in SgCtorInitializerList: initializedName->get_name() = %s \n",initializedName->get_name().str());
#endif
                              if (initializedName->get_name() == functionName)
                                 {
                                   setNameQualificationOnName(initializedName,declaration,amountOfNameQualificationRequiredForType,skipGlobalNameQualification);
                                 }
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Calling setNameQualification() (operating on the TYPE of the SgInitializedName) \n");
#endif
                              setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType,skipGlobalNameQualification);
                            }
                       }
                      else
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("Calling setNameQualification(): constructorInitializer == NULL: (operating on the TYPE of the SgInitializedName) \n");
#endif
                         setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType,skipGlobalNameQualification);
                       }
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Calling setNameQualification(): initializedName->get_initptr() == NULL: (operating on the TYPE of the SgInitializedName) \n");
#endif
                    setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType,skipGlobalNameQualification);
                  }

            // **************************************************
#else
            // setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType);
               setNameQualification(initializedName,declaration,amountOfNameQualificationRequiredForType,false);
#endif

#if 1
            // DQ (8/4/2012): Isolate that handling of the referencedNameSet from the use of skipGlobalNameQualification so that we can debug (test2012_96.C).
            // if (skipGlobalNameQualification == true && referencedNameSet.find(declaration) == referencedNameSet.end())
               if (referencedNameSet.find(declaration) == referencedNameSet.end())
                  {
                 // No qualification is required but we do want to count this as a reference to the class.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("No qualification should be used for this type (class) AND insert it into the referencedNameSet \n");
#endif
                    referencedNameSet.insert(declaration);
                  }
#endif
            // This can be inside of the case where (declaration != NULL)
            // Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
#if 0
               printf ("Calling traverseType on SgInitializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif
               traverseType(initializedName->get_type(),initializedName,currentScope,currentStatement);
             }
            else
             {
            // DQ (8/23/2014): This case is deomonstrated by test2014_145.C. where a SgInitializedName is used in a SgArrayType.
            // However, it would provide greater symetry to handle the SgInitializedName objects in the processing of the 
            // SgFunctionParameterList similar to how they are handling in the SgVariableDeclaration.

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Case of SgInitializedName: getDeclarationAssociatedWithType() == NULL (this not associated with a type)  \n");
#endif
#if 0
            // DQ (8/23/2014): Adding this to support SgInitializedName in SgArrayType in function parameter lists.
            // SgDeclarationStatement* associatedDeclaration = NULL;

               SgScopeStatement* currentScope = inheritedAttribute.get_currentScope();
               ROSE_ASSERT(currentScope != NULL);

               SgStatement* associatedStatement = currentScope;

            // int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentScope,associatedDeclaration);
            // int amountOfNameQualificationRequiredForType = nameQualificationDepthForType(initializedName,currentScope,associatedStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
            // printf ("SgInitializedName's type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForType);
#endif
            // traverseType(initializedName->get_type(),initializedName,currentScope,associatedStatement);
#endif
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
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
#if 0
          printf ("Case of (functionDeclaration != NULL && isSgMemberFunctionDeclaration(n) == NULL): currentScope = %p \n",currentScope);
#endif
       // ROSE_ASSERT(currentScope != NULL);
          if (currentScope != NULL)
             {
            // Handle the function return type...
               ROSE_ASSERT(functionDeclaration->get_orig_return_type() != NULL);
               ROSE_ASSERT(functionDeclaration->get_type() != NULL);
               ROSE_ASSERT(functionDeclaration->get_type()->get_return_type() != NULL);
               SgType* returnType = functionDeclaration->get_type()->get_return_type();
               ROSE_ASSERT(returnType != NULL);

               SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(returnType);
               if (declaration != NULL)
                  {
                    int amountOfNameQualificationRequiredForReturnType = nameQualificationDepth(declaration,currentScope,functionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("SgFunctionDeclaration's return type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForReturnType);
                    printf ("Putting the name qualification for the type into the return type of SgFunctionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->get_name().str());
#endif
                 // setNameQualificationReturnType(functionDeclaration,amountOfNameQualificationRequiredForReturnType);
                    setNameQualificationReturnType(functionDeclaration,declaration,amountOfNameQualificationRequiredForReturnType);
                  }
                 else
                  {
                 // This case is common for builtin functions such as: __builtin_powi
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("declaration == NULL: could not put name qualification for the type into the return type of SgFunctionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->get_name().str());
#endif
                  }

#if 0
               printf ("Calling traverseType on SgFunctionDeclaration = %p = %s name = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
#endif
            // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
               traverseType(returnType,functionDeclaration,currentScope,functionDeclaration);

            // Handle the function name...
            // DQ (6/20/2011): Friend function can be qualified...sometimes...
            // if (functionDeclaration->get_declarationModifier().isFriend() == true || functionDeclaration->get_specialFunctionModifier().isOperator() == true)
               if (functionDeclaration->get_specialFunctionModifier().isOperator() == true)
                  {
                 // DQ (6/19/2011): We sometimes have to qualify friends if it is to avoid ambiguity (see test2006_159.C) (but we never qualify an operator, I think).
                 // Maybe a friend declaration should add an SgAliasSymbol to the class definition scope's symbol table.
                 // Then simpler rules (no special case) would cause the name qualification to be generated properly.

                 // Old comment
                 // Never use name qualification for friend functions or operators. I am more sure of the case of friend functions than operators.
                 // Friend functions will have a global scope (though this might change in the future; google "friend global scope injection").
                 // printf ("Detected a friend or operator function, these are not provided with name qualification. \n");
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Detected a operator function, these are not provided with name qualification. \n");
#endif
                  }
                 else
                  {
                 // Only use name qualification where the scopes of the declaration's use (currentScope) is not the same 
                 // as the scope of the function declaration.  However, the analysis should work and determin that the 
                 // required name qualification length is zero.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("I would like to not have to have this SgFunctionDeclaration logic, we should get the name qualification correct more directly. \n");
#endif
                 // DQ (6/20/2011): Friend function can be qualified and a fix to add a SgAliasSymbol to the class definition scope's symbol table 
                 // should allow it to be handled with greater precission.

                 // DQ (6/25/2011): Friend functions can require global qualification as well (see test2011_106.C).
                 // Not clear how to handle this case.
                    if (functionDeclaration->get_declarationModifier().isFriend() == true)
                       {
                      // This is the case of a friend function declaration which requires more name qualification than expected.
                      // Note that this might be compiler dependent but at least GNU g++ required more qualification than expected.
                         SgScopeStatement* scope = functionDeclaration->get_scope();
                         SgGlobal* globalScope = isSgGlobal(scope);
                         if (globalScope != NULL)
                            {
                           // We want to specify global qualification when the friend function is in global scope (see test2011_106.C).
#if 1
                           // DQ (6/25/2011): This will output the name qualification correctly AND cause the output of the outlined function to be supressed.
                              int amountOfNameQualificationRequired = 0;

                           // Check if this function declaration has been seen already...
                           // if (functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration())

                           // This is the same code as below so it could be refactored (special handling for function declarations 
                           // that are defining declaration and don't have an associated nondefining declaration).

                           // DQ (8/4/2012): This is a case using the referencedNameSet that should be refactored out of this location.
                              SgDeclarationStatement* declarationForReferencedNameSet = functionDeclaration->get_firstNondefiningDeclaration();

                              if (declarationForReferencedNameSet == NULL)
                                 {
                                // Note that a function with only a defining declaration will not have a nondefining declaration 
                                // automatically constructed in the AST (unlike classes and some onther sorts of declarations).
                                   declarationForReferencedNameSet = functionDeclaration->get_definingDeclaration();

                                // DQ (6/22/2011): I think this is true.  This assertion fails for test2006_78.C (a template example code).
                                // ROSE_ASSERT(declarationForReferencedNameSet == declaration);

                                // DQ (6/23/2011): This assertion fails for the LoopProcessor on tests/roseTests/loopProcessingTests/mm.C
                                // ROSE_ASSERT(declarationForReferencedNameSet != NULL);
                                   if (declarationForReferencedNameSet == NULL)
                                      {
                                        declarationForReferencedNameSet = functionDeclaration;
                                        ROSE_ASSERT(declarationForReferencedNameSet != NULL);
                                      }
                                   ROSE_ASSERT(declarationForReferencedNameSet != NULL);
                                 }
                              ROSE_ASSERT(declarationForReferencedNameSet != NULL);

                           // DQ (8/4/2012): We would like to refactor this code (I think).
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                              printf ("Name qualification for SgFunctionDeclaration: I think this should be using the defined function xxx so that we isolate references to the referencedNameSet \n");
#endif
                              if (referencedNameSet.find(declarationForReferencedNameSet) == referencedNameSet.end())
                                 {
                                // No global qualification is required.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("No qualification should be used for this friend function. \n");
#endif
                                 }
                                else
                                 {
                                   amountOfNameQualificationRequired = 1;
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("Force global qualification for friend function: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                                 }
#else
                           // DQ (6/25/2011): This will not generate the correct name qualification AND will cause the outlined function to be output.
                              int amountOfNameQualificationRequired = 0;
#endif
                              setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
                            }
                           else
                            {
                           // Not clear what to do with this case, I guess we just want standard qualification rules.
                              int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
                              setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
                            }
                       }
                      else
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("currentScope = %p functionDeclaration->get_scope() = %p \n",currentScope,functionDeclaration->get_scope());
#endif
                      // Case of non-member functions (more logical name qualification rules).
                         if (currentScope != functionDeclaration->get_scope())
                            {
                           // DQ (1/21/2013): Added support for testing the more general equivalence of scopes (where the pointers are not equal, applies only to namespaces, I think).
                              bool isSameNamespace = SgScopeStatement::isEquivalentScope(currentScope,functionDeclaration->get_scope());

                              if (isSameNamespace == false)
                                 {
                                   int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("SgFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                                // DQ (21/2011): test2011_89.C demonstrates a case where name qualification of a functionRef expression is required.
                                // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in 
                                // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
                                // definition could not live in the SgBasicBlock.
                                   bool skipNameQualification = skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(functionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                                   printf ("Test of functionDeclaration: skipNameQualification = %s \n",skipNameQualification ? "true" : "false");
#endif
                                   if (skipNameQualification == false)
                                      {
                                        setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
                                      }
                                 }
                            }
#if 1
                           else
                            {
                           // DQ (10/31/2013): Added to support name qualification on template parameters (see test2013_273.C).
                           // However, this just leads to over qualification (use of global qualification which is not required).

                              SgTemplateInstantiationFunctionDecl* templateFunction = isSgTemplateInstantiationFunctionDecl(functionDeclaration);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("@@@@@@@@@@@@@ Calling nameQualificationDepth(): functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());
                              if (templateFunction != NULL)
                                 {
                                   printf ("In NameQualificationTraversal::evaluateInheritedAttribute(): for case of SgTemplateInstantiationFunctionDecl: templateFunction = %p = %s \n",templateFunction,templateFunction->class_name().c_str());
                                   printf ("   --- templateFunction->get_name()         = %s \n",templateFunction->get_name().str());
                                   printf ("   --- templateFunction->get_templateName() = %s \n",templateFunction->get_templateName().str());
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
#endif

                           // DQ (11/16/2013): The point is that we need to handle the name qualification on any associated template arguments not 
                           // on the function itself. So we just want to call nameQualificationDepth(), to get the anem qualification on the template 
                           // arguments, but we can safely ignore the return result since it need not be used to drive name qualification of the
                           // function.  Either that or we handle the template arguments explicitly.
                           // int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
#if 0
                              printf ("In NameQualificationTraversal::evaluateInheritedAttribute(): non-member declaration: functionDeclaration = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
                              printf ("In NameQualificationTraversal::evaluateInheritedAttribute(): non-member declaration: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                           // DQ (11/18/2013): Restrict this to template instantiations, else failing some astInterface tests (deepcopy.C).
                           // nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
                              if (templateFunction != NULL)
                                 {
                                   nameQualificationDepth(functionDeclaration,currentScope,functionDeclaration);
                                 }
                              
                           // setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
                            }
#endif
                       }
                  }
             }
            else
             {
            // Note that test2005_57.C presents an example that triggers this case and so might be a relevant 
            // test code.  Example: "template<typename T> void foobar (T x){ }".
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("WARNING: SgFunctionDeclaration -- currentScope is not available, not clear why! \n");
#endif
            // ROSE_ASSERT(false);
             }
        }

  // Handle references to SgMemberFunctionDeclaration...
     SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(n);
     if (memberFunctionDeclaration != NULL)
        {
       // Could it be that we only want to do this for the defining declaration? No, since prototypes must also use name qualification!

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
        // DQ (9/7/2014): Added debugging to verify that this case is supporting name qualification of SgTemplateMemberFunctionDeclaration IR node.
           if (isSgTemplateMemberFunctionDeclaration(memberFunctionDeclaration) != NULL)
              {
                printf ("Note: This case supports SgTemplateMemberFunctionDeclaration as well: memberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
             // ROSE_ASSERT(false);
              }
#endif

       // We need the structural location in scope (not the semantic one).
          SgScopeStatement* currentScope = isSgScopeStatement(memberFunctionDeclaration->get_parent());

       // ROSE_ASSERT(currentScope != NULL);
          if (currentScope != NULL)
             {
            // Handle the function return type...
               ROSE_ASSERT(memberFunctionDeclaration->get_orig_return_type() != NULL);
               ROSE_ASSERT(memberFunctionDeclaration->get_type() != NULL);
               ROSE_ASSERT(memberFunctionDeclaration->get_type()->get_return_type() != NULL);
               SgType* returnType = memberFunctionDeclaration->get_type()->get_return_type();
               ROSE_ASSERT(returnType != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("case SgMemberFunctionDeclaration: returnType = %p = %s \n",returnType,returnType->class_name().c_str());
               SgTemplateType* template_returnType = isSgTemplateType(returnType);
               if (template_returnType != NULL)
                  {
                    printf ("template_returnType                                    = %p \n",template_returnType);
                    printf ("template_returnType->get_name()                        = %s \n",template_returnType->get_name().str());
                    printf ("template_returnType->get_template_parameter_position() = %d \n",template_returnType->get_template_parameter_position());

                  }
               printf ("   --- memberFunctionDeclaration->get_firstNondefiningDeclaration() = %p = %s \n",memberFunctionDeclaration->get_firstNondefiningDeclaration(),memberFunctionDeclaration->get_firstNondefiningDeclaration()->class_name().c_str());
               if (memberFunctionDeclaration->get_definingDeclaration() != NULL)
                  {
                    printf ("   --- memberFunctionDeclaration->get_definingDeclaration() = %p = %s \n",memberFunctionDeclaration->get_definingDeclaration(),memberFunctionDeclaration->get_definingDeclaration()->class_name().c_str());
                  }
               printf ("memberFunctionDeclaration->get_type() = %p = %s \n",memberFunctionDeclaration->get_type(),memberFunctionDeclaration->get_type()->class_name().c_str());
#if 1
               if (isSgTemplateMemberFunctionDeclaration(memberFunctionDeclaration) != NULL)
                  {
                    printf ("Exiting as a test: memberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
                 // ROSE_ASSERT(false);
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                  }
#endif
#endif

               SgDeclarationStatement* declaration = getDeclarationAssociatedWithType(returnType);
               if (declaration != NULL)
                  {
                    int amountOfNameQualificationRequiredForReturnType = nameQualificationDepth(declaration,currentScope,memberFunctionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("SgMemberFunctionDeclaration's return type: amountOfNameQualificationRequiredForType = %d \n",amountOfNameQualificationRequiredForReturnType);
                    printf ("Putting the name qualification for the type into the return type of SgMemberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
#endif
                    setNameQualificationReturnType(memberFunctionDeclaration,declaration,amountOfNameQualificationRequiredForReturnType);
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("declaration == NULL: could not put name qualification for the type into the return type of SgMemberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->get_name().str());
#endif
                  }

#if 0
               printf ("Calling traverseType on SgMemberFunctionDeclaration = %p = %s name = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str(),memberFunctionDeclaration->get_name().str());
#endif
            // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
               traverseType(returnType,memberFunctionDeclaration,currentScope,memberFunctionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Don't forget possible covariant return types for SgMemberFunctionDeclaration IR nodes \n");

            // Only use name qualification where the scopes of the declaration's use (currentScope) is not the same 
            // as the scope of the function declaration.  However, the analysis should work and determin that the 
            // required name qualification length is zero.
               printf ("I would like to not have to have this SgMemberFunctionDeclaration logic, we should get the name qualification correct more directly. \n");
               printf ("   --- memberFunctionDeclaration->get_scope() = %p = %s \n",memberFunctionDeclaration->get_scope(),memberFunctionDeclaration->get_scope()->class_name().c_str());
               printf ("   --- currentScope                           = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
               if (currentScope != memberFunctionDeclaration->get_scope())
                  {
                 // DQ (1/21/2013): Note that the concept of equivalent scope is fine here if it only tests the equivalence of the pointers.  
                 // We can't have member functions in namespaces so we don't require that more general test for scope equivalence.

                    int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,memberFunctionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("SgMemberFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                    setNameQualification(memberFunctionDeclaration,amountOfNameQualificationRequired);
                  }
                 else
                  {
                 // DQ (9/7/2014): This branch is taken by the non-defining template member functions defined outside of their 
                 // associated template class declarations. There are also other cases where this branch is taken.

                 // Don't know what test code exercises this case (see test2005_73.C).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("WARNING: SgMemberFunctionDeclaration -- currentScope is not available through predicate (currentScope != memberFunctionDeclaration->get_scope()), not clear why! \n");
                    printf ("   --- memberFunctionDeclaration->get_scope() = %p = %s \n",memberFunctionDeclaration->get_scope(),memberFunctionDeclaration->get_scope()->class_name().c_str());
                    printf ("   --- currentScope                           = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                 // ROSE_ASSERT(false);
                  }
             }
            else
             {
            // Note that test2005_63.C presents an example that triggers this case and so might be a relevant.
            // This is also the reason why test2005_73.C is failing!!!  Fix it tomorrow!!! (SgTemplateInstantiationDirectiveStatement)
               SgDeclarationStatement* currentStatement = isSgDeclarationStatement(memberFunctionDeclaration->get_parent());

            // DQ (9/4/2014): Lambda functions (in SgLambdaExp) are an example where this fails.
            // ROSE_ASSERT(currentStatement != NULL);
               if (currentStatement != NULL)
                  {
                    SgScopeStatement* currentScope = isSgScopeStatement(currentStatement->get_parent());
                    if (currentScope != NULL)
                       {
                         int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,memberFunctionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("SgMemberFunctionDeclaration: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                         setNameQualification(memberFunctionDeclaration,amountOfNameQualificationRequired);
                       }
                      else
                       {
                         printf ("WARNING: SgMemberFunctionDeclaration -- currentScope is not available through parent SgDeclarationStatement, not clear why! \n");
                         ROSE_ASSERT(false);
                       }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("WARNING: SgMemberFunctionDeclaration -- currentScope is not available, not clear why! \n");
#endif
                 // ROSE_ASSERT(false);
                  }
                 else
                  {
                 // This should only be a lambda function defined in a SgLambdaExp.
                    ROSE_ASSERT(isSgLambdaExp(memberFunctionDeclaration->get_parent()) != NULL);
                  }
             }
        }

  // DQ (4/3/2014): Added new case to address no longer traversing this IR node's member.
  // See test2005_73.C.
     SgTemplateInstantiationDirectiveStatement* templateInstantiationDirectiveStatement = isSgTemplateInstantiationDirectiveStatement(n);
     if (templateInstantiationDirectiveStatement != NULL)
        {
       // DQ (4/3/2014): We no longer traverse the declaration referenced in this IR node so we
       // have to handle the name qualification requiredments for the associated declaration directly.
#if 0
          printf ("Processing SgTemplateInstantiationDirectiveStatement \n");
#endif
       // Note that test2005_63.C presents an example that triggers this case and so might be a relevant.
       // This is also the reason why test2005_73.C is failing!!!  Fix it tomorrow!!! (SgTemplateInstantiationDirectiveStatement)
       // SgDeclarationStatement* currentStatement = isSgDeclarationStatement(memberFunctionDeclaration->get_parent());
          SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(templateInstantiationDirectiveStatement->get_declaration());
          ROSE_ASSERT(declarationStatement != NULL);
          SgDeclarationStatement* currentStatement = isSgDeclarationStatement(declarationStatement->get_parent());
          if (currentStatement == NULL)
             {
#if 0
               printf ("In name qualification: case SgTemplateInstantiationDirectiveStatement: Using backup mechanism to generate current statement (because EDG 4.8 shared template instantiations) \n");
#endif
               currentStatement = templateInstantiationDirectiveStatement;
             }
          ROSE_ASSERT(currentStatement != NULL);
#if 0
          printf ("declarationStatement = %p = %s \n",declarationStatement,declarationStatement->class_name().c_str());
          printf ("currentStatement     = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif
          SgScopeStatement* currentScope = isSgScopeStatement(currentStatement->get_parent());
          if (currentScope != NULL)
             {
#if 0
               printf ("currentScope     = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
               int amountOfNameQualificationRequired = nameQualificationDepth(declarationStatement,currentScope,declarationStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("SgTemplateInstantiationDirectiveStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif

            // Since the reference to the class or function declaration is shared, we might want to
            // attached the name qualification to the SgTemplateInstantiationDirectiveStatement instead.
            // setNameQualification(templateInstantiationDirectiveStatement->get_declaration(),amountOfNameQualificationRequired);
               SgClassDeclaration* classDeclaration       = isSgClassDeclaration(declarationStatement);
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declarationStatement);
               if (classDeclaration != NULL)
                  {
                    setNameQualification(classDeclaration,amountOfNameQualificationRequired);
                  }
                 else
                  {
                    ROSE_ASSERT(functionDeclaration != NULL);
                    setNameQualification(functionDeclaration,amountOfNameQualificationRequired);
                  }
             }
            else
             {
               printf ("WARNING: SgTemplateInstantiationDirectiveStatement -- currentScope is not available through parent of SgDeclarationStatement, not clear why! \n");
               ROSE_ASSERT(false);
             }
#if 0
          printf ("In case of name qualification for SgTemplateInstantiationDirectiveStatement \n");
          ROSE_ASSERT(false);
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() = %s \n",typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() ? "true" : "false");
#endif
       // This is NULL if the base type is not associated with a declaration (e.g. not a SgNamedType).
       // ROSE_ASSERT(baseTypeDeclaration != NULL);
       // if (baseTypeDeclaration != NULL)
          if ( (baseTypeDeclaration != NULL) && (typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() == false) )
             {
            // int amountOfNameQualificationRequiredForBaseType = nameQualificationDepth(baseTypeDeclaration,currentScope,typedefDeclaration);
               int amountOfNameQualificationRequiredForBaseType = nameQualificationDepth(baseType,currentScope,typedefDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("SgTypedefDeclaration: amountOfNameQualificationRequiredForBaseType = %d \n",amountOfNameQualificationRequiredForBaseType);
#endif
               ROSE_ASSERT(baseTypeDeclaration != NULL);
               setNameQualification(typedefDeclaration,baseTypeDeclaration,amountOfNameQualificationRequiredForBaseType);
             }

#if 0
          printf ("Calling traverseType on SgTypedefDeclaration = %p name = %s \n",typedefDeclaration,typedefDeclaration->get_name().str());
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("SgUsingDirectiveStatement's SgNamespaceDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
          setNameQualification(usingDirective,namespaceDeclaration,amountOfNameQualificationRequired);
        }

     SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(n);
     if (usingDeclaration != NULL)
        {
          SgDeclarationStatement* associatedDeclaration     = usingDeclaration->get_declaration();
          SgInitializedName*      associatedInitializedName = usingDeclaration->get_initializedName();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In case for SgUsingDeclarationStatement: associatedDeclaration = %p associatedInitializedName = %p \n",associatedDeclaration,associatedInitializedName);
          if (associatedDeclaration != NULL)
             printf ("associatedDeclaration = %p = %s = %s = %s \n",associatedDeclaration,associatedDeclaration->class_name().c_str(),SageInterface::get_name(associatedDeclaration).c_str(),SageInterface::generateUniqueName(associatedDeclaration,true).c_str());
          if (associatedInitializedName != NULL)
             printf ("associatedInitializedName = %p = %s = %s = %s \n",associatedInitializedName,associatedInitializedName->class_name().c_str(),SageInterface::get_name(associatedInitializedName).c_str(),SageInterface::generateUniqueName(associatedInitializedName,true).c_str());
#endif
          SgScopeStatement* currentScope = usingDeclaration->get_scope();
          ROSE_ASSERT(currentScope != NULL);

          int amountOfNameQualificationRequired = 0;
          if (associatedDeclaration != NULL)
             {
#if 0
               printf ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               if (currentScope->get_scope() != NULL)
                    printf ("currentScope->get_scope() = %p = %s \n",currentScope->get_scope(),currentScope->get_scope()->class_name().c_str());
#endif
#if 0
            // DQ (6/22/2011): The declaration in a SgUsingDeclarationStatement must have some qualification else it is not 
            // required and will be an error.  To avoid it being confused with a name in the current scope we have to start 
            // the process assuming the scope of the current scope. This is a special case in the name qualification handling.
            // But we also do not need the forceMoreNameQualification mechanism (I think).
            // DQ (5/14/2011): For the case of test2001_46.C I think we need to force name qualification.
            // In general any reference using a SgUsingDeclarationStatement should likely be qualified (at least one level, via the parent of the associatedDeclaration).
            // amountOfNameQualificationRequired = nameQualificationDepth(associatedDeclaration,currentScope,usingDeclaration);
            // amountOfNameQualificationRequired = nameQualificationDepth(associatedDeclaration,currentScope,usingDeclaration, /* forceMoreNameQualification = */ true);
               amountOfNameQualificationRequired = nameQualificationDepth(associatedDeclaration,currentScope->get_scope(),usingDeclaration, /* forceMoreNameQualification = */ false);
#else
            // DQ (3/31/2014): Compute the required depth for global qualification (required for using declarations).
               amountOfNameQualificationRequired = depthOfGlobalNameQualification(associatedDeclaration);
#endif
            // DQ (6/22/2011): If the amountOfNameQualificationRequired is zero then add one to force at least global qualification.
            // See test2004_80.C for an example.
               if (isSgGlobal(currentScope->get_scope()) != NULL && amountOfNameQualificationRequired == 0)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Handling special case to force at least global qualification. \n");
#endif
                    amountOfNameQualificationRequired += 1;
                  }

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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("SgUsingDeclarationStatement's SgVarRefExp: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
        }


  // DQ (7/8/2014): Adding support for name qualification of the SgNamespaceDeclarationStatement referenced by a SgNamespaceAliasDeclarationStatement.
  // Handle references in SgNamespaceAliasDeclarationStatement...
     SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(n);
     if (namespaceAliasDeclaration != NULL)
        {
          SgNamespaceDeclarationStatement* namespaceDeclaration = namespaceAliasDeclaration->get_namespaceDeclaration();
          ROSE_ASSERT(namespaceDeclaration != NULL);
          SgScopeStatement* currentScope = namespaceAliasDeclaration->get_scope();
          ROSE_ASSERT(currentScope != NULL);

          int amountOfNameQualificationRequired = nameQualificationDepth(namespaceDeclaration,currentScope,namespaceAliasDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("SgNamespaceAliasDeclarationStatement's SgNamespaceDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
          setNameQualification(namespaceAliasDeclaration,namespaceDeclaration,amountOfNameQualificationRequired);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("DONE: SgNamespaceAliasDeclarationStatement's SgNamespaceDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
#if 0
       // We want to debug this case later!
          printf ("Exiting in unimplemented case of name qualification for the SgNamespaceDeclarationStatement in a SgNamespaceAliasDeclarationStatement \n");
          ROSE_ASSERT(false);
#endif
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("case SgFunctionRefExp: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif
            // DQ (9/17/2011); Added escape for where the currentStatement == NULL (fails for STL code when the original expression trees are used to eliminate the constant folded values).
            // ROSE_ASSERT(currentStatement != NULL);
               if (currentStatement != NULL)
                  {
                 // DQ (9/17/2011): this is the original case we want to restore later...
                    SgScopeStatement* currentScope = currentStatement->get_scope();
                 // ROSE_ASSERT(currentScope != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("case SgFunctionRefExp: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                    int amountOfNameQualificationRequired = nameQualificationDepth(functionDeclaration,currentScope,currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("SgFunctionCallExp's function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                 // DQ (6/9/2011): Support for test2011_78.C (we only qualify function call references where the function has been declared in 
                 // a scope where it could be expected to be defined (e.g. not using a forward declaration in a SgBasicBlock, since the function
                 // definition could not live in the SgBasicBlock.
                    bool skipNameQualification = skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(functionDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Test of functionRefExp: skipNameQualification = %s \n",skipNameQualification ? "true" : "false");
#endif
                    if (skipNameQualification == false)
                       {
                         setNameQualification(functionRefExp,functionDeclaration,amountOfNameQualificationRequired);
                       }
                  }
                 else
                  {
                 // DQ (9/17/2011): Added this case, print a warning and fix thiat after debugging the constant folding value elimination..
                    printf ("WARNING: SgFunctionRefExp name qualification not handled for the case of currentStatement == NULL \n");
                  }
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("WARNING: functionDeclaration == NULL in SgFunctionCallExp for name qualification support! \n");
#endif
             }

       // If this is a templated function then we have to save the name becuase its templated name might have template arguments that require name qualification.
          SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(functionRefExp->getAssociatedFunctionDeclaration());
          if (templateInstantiationFunctionDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Found a SgTemplateInstantiationFunctionDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationFunctionDeclaration->get_name().str());
#endif
               SgStatement* currentStatement = TransformationSupport::getStatement(functionRefExp);
               ROSE_ASSERT(currentStatement != NULL);

               SgScopeStatement* currentScope = currentStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);

            // traverseTemplatedFunction(functionRefExp,templateInstantiationFunctionDeclaration,currentScope,currentStatement);
               traverseTemplatedFunction(functionRefExp,functionRefExp,currentScope,currentStatement);
             }
        }

     SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(n);
     if (memberFunctionRefExp != NULL)
        {
          SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionRefExp->getAssociatedMemberFunctionDeclaration();
       // ROSE_ASSERT(functionDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("case of SgMemberFunctionRefExp: memberFunctionDeclaration = %p \n",memberFunctionDeclaration);
#endif
          if (memberFunctionDeclaration != NULL)
             {
               SgStatement* currentStatement = TransformationSupport::getStatement(memberFunctionRefExp);
               if (currentStatement == NULL)
                  {
                 // DQ (8/19/2014): Because we know where this can happen we don't need to always output debugging info.
                 // A better test might be to find the type that embeds the expression and make sure it is a SgArrayType.
                 // DQ (7/11/2014): test2014_83.C demonstrates how this can happen because the SgMemberFunctionRefExp 
                 // appears in an index expression of an array type in a variable declaration.
                    SgType* associatedType = TransformationSupport::getAssociatedType(memberFunctionRefExp);
                    if (associatedType != NULL)
                       {
                         SgArrayType* arrayType = isSgArrayType(associatedType);
                         if (arrayType == NULL)
                            {
                              printf ("Warning: Location of where we can NOT associate the expression to a SgArrayType \n");
                              memberFunctionRefExp->get_file_info()     ->display("Error: currentStatement == NULL: memberFunctionRefExp: debug");
                              memberFunctionDeclaration->get_file_info()->display("Error: currentStatement == NULL: memberFunctionDeclaration: debug");
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Note: Location of where we CAN associate the expression to a statement: confirmed unassociated expression is buried in a type: associatedType = %p = %s \n",associatedType,associatedType->class_name().c_str());
#endif
                            }
                       }
                      else
                       {
                         printf ("Error: Location of where we can NOT associate the expression to a statement \n");
                         memberFunctionRefExp->get_file_info()     ->display("Error: currentStatement == NULL: memberFunctionRefExp: debug");
                         memberFunctionDeclaration->get_file_info()->display("Error: currentStatement == NULL: memberFunctionDeclaration: debug");
                       }

                 // DQ (7/11/2014): Added wupport for when this is a nested call and the scope where the call is made from is essential.
                    if (explictlySpecifiedCurrentScope != NULL)
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("explictlySpecifiedCurrentScope = %p = %s \n",explictlySpecifiedCurrentScope,explictlySpecifiedCurrentScope->class_name().c_str());
#endif
                         currentStatement = explictlySpecifiedCurrentScope;
                       }
                      else
                       {
                         printf ("Error: explictlySpecifiedCurrentScope == NULL \n");

                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
                       }

                  }
               ROSE_ASSERT(currentStatement != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("case of SgMemberFunctionRefExp: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
#endif
               SgScopeStatement* currentScope = currentStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("case of SgMemberFunctionRefExp: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               printf ("***** case of SgMemberFunctionRefExp: Calling nameQualificationDepth() ***** \n");
#endif
               int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("***** case of SgMemberFunctionRefExp: DONE: Calling nameQualificationDepth() ***** \n");
               printf ("SgMemberFunctionCallExp's member function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
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
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("WARNING: memberFunctionDeclaration == NULL in SgMemberFunctionCallExp for name qualification support! \n");
#endif
             }

       // If this is a templated function then we have to save the name because its templated name might have template arguments that require name qualification.
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionRefExp->getAssociatedMemberFunctionDeclaration());
          if (templateInstantiationMemberFunctionDeclaration != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Found a SgTemplateInstantiationMemberFunctionDecl that will have template arguments that might require qualification. name = %s \n",templateInstantiationMemberFunctionDeclaration->get_name().str());
               printf ("Must handle templated SgMemberFunctionRefExp! \n");
#endif

            // DQ (5/24/2013): Added support for member function template argument lists to have similar handling, such as to 
            // SgTemplateInstantiationFunctionDecl IR nodes.  This is required to support test codes such as test2013_188.C.
               SgStatement* currentStatement = TransformationSupport::getStatement(memberFunctionRefExp);
               ROSE_ASSERT(currentStatement != NULL);

               SgScopeStatement* currentScope = currentStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("case of SgMemberFunctionRefExp: currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
               printf ("case of SgMemberFunctionRefExp: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               printf ("***** calling traverseTemplatedMemberFunction() \n");
#endif

            // traverseTemplatedFunction(functionRefExp,templateInstantiationFunctionDeclaration,currentScope,currentStatement);
            // traverseTemplatedFunction(functionRefExp,functionRefExp,currentScope,currentStatement);
               traverseTemplatedMemberFunction(memberFunctionRefExp,memberFunctionRefExp,currentScope,currentStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("***** DONE: calling traverseTemplatedMemberFunction() \n");
#endif
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
        }

  // DQ (5/31/2011): This is a derived class from SgExpression and SgInitializer...
     SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(n);
     if (constructorInitializer != NULL)
        {
          SgMemberFunctionDeclaration* memberFunctionDeclaration = constructorInitializer->get_declaration();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Case of SgConstructorInitializer: memberFunctionDeclaration = %p \n",memberFunctionDeclaration);
#endif
          SgStatement* currentStatement = TransformationSupport::getStatement(constructorInitializer);
          if (currentStatement == NULL)
             {
               printf ("Error in constructorInitializer = %p \n",constructorInitializer);
               ROSE_ASSERT(constructorInitializer->get_parent() != NULL);
             }
          ROSE_ASSERT(currentStatement != NULL);

       // If this could occur in a SgForStatement then this should be fixed up as it is elsewhere...
          SgScopeStatement* currentScope = currentStatement->get_scope();
          ROSE_ASSERT(currentScope != NULL);

          if (memberFunctionDeclaration != NULL)
             {
               int amountOfNameQualificationRequired = nameQualificationDepth(memberFunctionDeclaration,currentScope,currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("SgConstructorInitializer's constructor member function name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
               setNameQualification(constructorInitializer,memberFunctionDeclaration,amountOfNameQualificationRequired);
#if 0
               printf ("In name qualification: Case of SgConstructorInitializer: memberFunctionDeclaration = %p \n",memberFunctionDeclaration);
#endif
#if 0
               printf ("DONE: Calling setNameQualification() on constructorInitializer: Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
            else
             {
            // DQ (6/1/2011): This happens when there is no explicit constructor that can be used to build a class, in this case the class name must be used to define a default constructor.
            // This is a problem for test2004_130.C (at line 165 col = 14 file = /home/dquinlan/ROSE/ROSE_CompileTree/git-LINUX-64bit-4.2.4-dq-cxx-rc/include-staging/g++_HEADERS/hdrs3/bits/stl_iterator_base_types.h).
            // Need to investigate this later (it is strange that it is not an issue in test2011_63.C, but it is a struct instead of a class and that might be why).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("WARNING: memberFunctionDeclaration == NULL in SgConstructorInitializer for name qualification support! \n");
#endif
            // ROSE_ASSERT(false);

            // DQ (6/4/2011): Added support for this case.
               SgClassDeclaration* classDeclaration = constructorInitializer->get_class_decl();
            // ROSE_ASSERT(classDeclaration != NULL);
               if (classDeclaration != NULL)
                  {
                 // An example of the problem is test2005_42.C, where the class name is used to generate the constructor initializer name.
                    int amountOfNameQualificationRequired = nameQualificationDepth(classDeclaration,currentScope,currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("SgConstructorInitializer's constructor (class default constructor) name: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                 // This will attach the new type string to the classDeclaration.
                    setNameQualification(constructorInitializer,classDeclaration,amountOfNameQualificationRequired);
                  }
                 else
                  {
                 // This is a strange error: see test2004_77.C
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("WARNING: In SgConstructorInitializer name qualification support: neither memberFunctionDeclaration or classDeclaration are valid pointers. \n");
#endif
                  }
             }

       // After processing the name qualification for the class declaration, we need to also process the 
       // reference to the type for any name qualification on possible template arguments.
          ROSE_ASSERT(constructorInitializer->get_type() != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Calling traverseType() on constructorInitializer = %p class type = %p = %s \n",
                    constructorInitializer,constructorInitializer->get_type(),constructorInitializer->get_type()->class_name().c_str());
#endif
       // DQ (8/19/2013): Added the call to associate the name qualified class name with the constructorInitializer.
       // DQ (8/19/2013): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
          traverseType(constructorInitializer->get_type(),constructorInitializer,currentScope,currentStatement);
#if 0
          printf ("DONE: Calling traverseType() on constructorInitializer: Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     SgVarRefExp* varRefExp = isSgVarRefExp(n);
     if (varRefExp != NULL)
        {
       // We need to store the information about the required name qualification in the SgVarRefExp IR node.

          SgStatement* currentStatement = TransformationSupport::getStatement(varRefExp);
#if 0
          printf ("Case of SgVarRefExp: varRefExp = %p currentStatement = %p = %s \n",varRefExp,currentStatement,currentStatement != NULL ? currentStatement->class_name().c_str() : "null");
#endif
       // DQ (6/23/2011): This test fails for the new name qualification after a transformation in tests/roseTests/programTransformationTests/test1.C
       // ROSE_ASSERT(currentStatement != NULL);
          if (currentStatement != NULL)
             {
            // DQ (5/30/2011): Handle the case of test2011_58.C (index declaration in for loop construct).
            // SgScopeStatement* currentScope = currentStatement->get_scope();
               SgScopeStatement* currentScope = isSgScopeStatement(currentStatement);
               if (currentScope == NULL)
                  {
                    currentScope = currentStatement->get_scope();
                  }
               ROSE_ASSERT(currentScope != NULL);

            // printf ("Case SgVarRefExp: (could this be in an array type?) currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());

               SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
               ROSE_ASSERT(variableSymbol != NULL);
               SgInitializedName* initializedName = variableSymbol->get_declaration();
               ROSE_ASSERT(initializedName != NULL);

            // DQ (7/18/2012): Added test as part of debugging test2011_75.C.
               ROSE_ASSERT(initializedName->get_parent() != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Case of SgVarRefExp: varRefExp = %p : initializedName name = %s parent = %p = %s \n",
                    varRefExp,initializedName->get_name().str(),initializedName->get_parent(),initializedName->get_parent()->class_name().c_str());
#endif
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initializedName->get_parent());
            // ROSE_ASSERT(variableDeclaration != NULL);
               if (variableDeclaration == NULL)
                  {
                 // This is the special case for the compiler generated variable "__PRETTY_FUNCTION__".
                    if (initializedName->get_name() == "__PRETTY_FUNCTION__" ||  initializedName->get_name() == "__func__")
                       {
                      // Skip these cases ... no name qualification is required.
                       }
                      else
                       {
                      // If this is a SgInitializedName from a function parameter list then it does not need qualification.
                         SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(initializedName->get_parent());
                         if (functionParameterList != NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("Names from function parameter list can not be name qualified (because they are the initial declarations): name = %s \n",initializedName->get_name().str());
#endif
                            }
                           else
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                              printf ("varRefExp's initialized name = %s is not associated with a SgVariableDeclaration \n",initializedName->get_name().str());
                              initializedName->get_file_info()->display("This SgInitializedName is not associated with a SgVariableDeclaration");

                              SgStatement* currentStatement = TransformationSupport::getStatement(initializedName->get_parent());
                              ROSE_ASSERT(currentStatement != NULL);

                              SgScopeStatement* targetScope = initializedName->get_scope();
                              printf ("targetScope = %p = %s \n",targetScope,targetScope->class_name().c_str());

                              printf ("SgVarRefExp case (no associated variableDeclaration): currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());

                              printf ("Exiting as a test! \n");
#endif
                           // DQ (7/18/2012): Uncommented to debug test2011_75.C, not fixed, but test2005_103.C fails and so this shuld be commented again (I think).
                           // ROSE_ASSERT(false);
                            }
                       }
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("In case SgVarRefExp: (currentStatement != NULL) Calling nameQualificationDepth() \n");
#endif
                 // DQ (12/21/2015): When this is a data member of a class/struct then we are consistatnly oberqualifying the SgVarRefExp
                 // because we are not considering the case of a variable of type class that is being used with the SgArrowExp or SgDotExp
                 // which would not require the name qualification.  The only case where we would still need the name qualification is the
                 // relatively rare case of multiple inheritance (which must be detected seperately).
#if 1

#define DEBUG_MEMBER_DATA_QUALIFICATION 0

                    SgScopeStatement* variableDeclarationScope = variableDeclaration->get_scope();
                    ROSE_ASSERT(variableDeclarationScope != NULL);

                 // If this is the same scoep then it is not interesting to debug this case.
                    if (variableDeclarationScope != currentScope)
                       {
                         SgExpression* parentExpression = isSgExpression(varRefExp->get_parent());

                         SgBinaryOp* binaryOperator = isSgBinaryOp(parentExpression);
                         if (binaryOperator != NULL)
                            {
                              ROSE_ASSERT(parentExpression != NULL);
#if DEBUG_MEMBER_DATA_QUALIFICATION
                              printf ("   --- parentExpression of varRefExp = %p varRefExp = %p = %s \n",varRefExp,parentExpression,parentExpression->class_name().c_str());
#endif
                              SgArrowExp* arrowExp = isSgArrowExp(binaryOperator);
                              SgDotExp* dotExp     = isSgDotExp(binaryOperator);

                              SgExpression* lhs  = binaryOperator->get_lhs_operand();
                              ROSE_ASSERT(lhs != NULL);
                              SgCastExp* castExp = isSgCastExp(lhs);
#if DEBUG_MEMBER_DATA_QUALIFICATION
                              printf ("   --- lhs = %p = %s \n",lhs,lhs->class_name().c_str());
#endif
                           // DQ (12/22/2015): Iterate though any possible chain of SgCastExp IR nodes (see test2005_89.C).
                           // if (castExp != NULL && castExp->get_file_info()->isCompilerGenerated() == true)
                              while (castExp != NULL && castExp->get_file_info()->isCompilerGenerated() == true)
                                 {
#if DEBUG_MEMBER_DATA_QUALIFICATION
                                   printf ("   --- detected implicit (compiler generated) cast: castExp = %p \n",castExp);
#endif
                                   lhs = isSgExpression(castExp->get_operand());
                                   ROSE_ASSERT(lhs != NULL);

                                // We have to handle chains of implicit casts.
                                // ROSE_ASSERT(isSgCastExp(lhs) == NULL);

                                   castExp = isSgCastExp(lhs);
#if 0
                                   if (isSgCastExp(lhs) != NULL)
                                      {
                                        printf ("lhs = %p = %s \n",lhs,lhs->class_name().c_str());
                                        lhs->get_file_info()->display("implicit cast");
                                        varRefExp->get_file_info()->display("varRefExp associated with implicit cast");
                                      }
#endif
                                 }

#if DEBUG_MEMBER_DATA_QUALIFICATION
                              printf ("   --- (after filtering implicit casts) lhs = %p = %s \n",lhs,lhs->class_name().c_str());
#endif
                              SgClassType* classType = NULL;

                           // DQ (12/22/2015): If the varRefExp is the lhs then we don't want to change the name qualification. 
                           // See test2015_138.C for example for the case of SgDotExp.
                           // if (arrowExp != NULL)
                              if (arrowExp != NULL && lhs != varRefExp)
                                 {
#if DEBUG_MEMBER_DATA_QUALIFICATION
                                   printf ("   --- Found SgArrowExp: lhs = %p = %s \n",lhs,lhs->class_name().c_str());
#endif
                                   SgType* lhs_type = lhs->get_type();
                                   ROSE_ASSERT(lhs_type != NULL);
#if DEBUG_MEMBER_DATA_QUALIFICATION
                                   printf ("   --- lhs_type = %p = %s \n",lhs_type,lhs_type->class_name().c_str());
#endif
                                // Handle the case of reference to pointer type.
                                   lhs_type = lhs_type->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_TYPEDEF_TYPE);

                                   SgPointerType* pointerType = isSgPointerType(lhs_type);
                                // ROSE_ASSERT(pointerType != NULL);
                                   if (pointerType != NULL)
                                      {
                                        SgType* baseType = pointerType->get_base_type();
                                        ROSE_ASSERT(baseType != NULL);

                                     // Handle the case of pointer to reference type.
                                     // Not clear if we have to handle array types
                                        baseType = baseType->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_TYPEDEF_TYPE);
                                        ROSE_ASSERT(baseType != NULL);
#if DEBUG_MEMBER_DATA_QUALIFICATION
                                        printf ("   --- baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif
                                     // It might be better to resolve this to a SgNamedType instead of SgClassType.
                                        classType = isSgClassType(baseType);
                                        ROSE_ASSERT(classType != NULL);
                                      }
                                     else
                                      {
#if 0
                                     // Debug this case!
                                     // I think this is happening for template classes (might be interesting to look into further).
                                        printf ("   --- SgArrowExp without associated SgPointer \n");
                                        varRefExp->get_file_info()->display("SgArrowExp without associated SgPointer: debug");
#endif
                                      }
                                 }

                           // DQ (12/22/2015): If the varRefExp is the lhs then we don't want to change the name qualification. See test2015_138.C for example.
                           // if (dotExp != NULL)
                              if (dotExp != NULL && lhs != varRefExp)
                                 {
#if DEBUG_MEMBER_DATA_QUALIFICATION
                                   printf ("   --- Found SgDotExp: lhs = %p = %s \n",lhs,lhs->class_name().c_str());
#endif
                                   SgType* lhs_type = lhs->get_type();
                                   ROSE_ASSERT(lhs_type != NULL);
#if DEBUG_MEMBER_DATA_QUALIFICATION
                                   printf ("   --- lhs_type = %p = %s = %s \n",lhs_type,lhs_type->class_name().c_str(),lhs_type->unparseToString().c_str());
#endif
                                // Not clear if we have to handle array types
                                   SgType* baseType = lhs_type->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_TYPEDEF_TYPE);
                                   ROSE_ASSERT(baseType != NULL);
#if DEBUG_MEMBER_DATA_QUALIFICATION
                                   printf ("   --- baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif
                                   classType = isSgClassType(baseType);
                                // ROSE_ASSERT(classType != NULL);
                                 }

                              if (classType != NULL)
                                 {
                                   SgDeclarationStatement* declarationStatement = classType->get_declaration();
                                   ROSE_ASSERT(declarationStatement != NULL);
#if DEBUG_MEMBER_DATA_QUALIFICATION
                                   printf ("   --- declarationStatement = %p = %s = %s \n",declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
#endif
                                   SgDeclarationStatement* definingDeclarationStatement = declarationStatement->get_definingDeclaration();
                                // ROSE_ASSERT(definingDeclarationStatement != NULL);
                                   if (definingDeclarationStatement != NULL)
                                      {
                                        SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(definingDeclarationStatement);
                                        ROSE_ASSERT(definingClassDeclaration != NULL);
                                        SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                        ROSE_ASSERT(classDefinition != NULL);

                                        currentScope = classDefinition;
#if DEBUG_MEMBER_DATA_QUALIFICATION
                                        printf ("   --- Case of SgVarRefExp: recomputed currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                                      }
                                     else
                                      {
#if 0
                                     // Debug this case!
                                     // I think this happends in template member functions.
                                     // I think this is happening for template classes (might be interesting to look into further).
                                        printf ("   --- SgClassType without associated defining declaration \n");
                                        varRefExp->get_file_info()->display("SgClassType without associated defining declaration: debug");
#endif
                                      }
                                 }
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
#endif

                    int amountOfNameQualificationRequired = nameQualificationDepth(variableDeclaration,currentScope,currentStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("SgVarRefExp's SgDeclarationStatement: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
                    setNameQualification(varRefExp,variableDeclaration,amountOfNameQualificationRequired);

                 // DQ (12/23/2015): If there are multiple symbols with the same name then we require the name qualification.
                 // See test2015_140.C for an example.
                    SgName name = initializedName->get_name().str();
                 // size_t numberOfAliasSymbols = currentScope->count_alias_symbol(name);
                    int numberOfAliasSymbols = currentScope->count_alias_symbol(name);
                 // if (numberOfAliasSymbols > 1)
                    if (numberOfAliasSymbols > 1 && amountOfNameQualificationRequired == 0)
                       {
                         printf ("WARNING: name qualification can be required when there are multiple base classes with the same referenced variable via SgAliasSymbol \n");
                       }
                      else
                       {
                      // DQ (12/23/2015): Note that this is not a count of the SgVariableSymbol IR nodes.
                      // size_t numberOfSymbolsWithSameName = currentScope->count_symbol(name);
                         int numberOfSymbolsWithSameName = (int)currentScope->count_symbol(name);
                      // if (numberOfSymbolsWithSameName > 1)
                      // if (numberOfSymbolsWithSameName > 1 && amountOfNameQualificationRequired == 0)
                         if ((numberOfSymbolsWithSameName - numberOfAliasSymbols) > 1 && amountOfNameQualificationRequired == 0)
                            {
                              printf ("WARNING: name qualification can be required when there are multiple base classes with the same referenced variable via SgVariableSymbol \n");
                            }
                      // ROSE_ASSERT(numberOfSymbolsWithSameName < 2);
                      // if (numberOfSymbolsWithSameName >= 2 && amountOfNameQualificationRequired == 0)
                         if ((numberOfSymbolsWithSameName - numberOfAliasSymbols) > 1 && amountOfNameQualificationRequired == 0)
                            {
                              printf ("   --- numberOfSymbolsWithSameName       = %d \n",numberOfSymbolsWithSameName);
                              printf ("   --- amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
                            }
                       // ROSE_ASSERT(numberOfSymbolsWithSameName < 2 || amountOfNameQualificationRequired > 0);

                       // DQ (12/23/2015): This fails for the LULESH-OMP tests, I think I need to relax this (it is a new assection).
                       // It might be that we need to also check that these are all SgVariableSymbol (since there could be different 
                       // kinds of SgSymbol (which would have to be allowed)).
                       // ROSE_ASSERT((numberOfSymbolsWithSameName - numberOfAliasSymbols) <= 1 || amountOfNameQualificationRequired > 0);
                       }
                 // ROSE_ASSERT(numberOfAliasSymbols < 2);
                    ROSE_ASSERT(numberOfAliasSymbols <= 1 || amountOfNameQualificationRequired > 0);
                  }

            // End of new test...
             }
            else
             {
            // DQ (7/23/2011): This case happens when the SgVarRefExp can not be associated with a statement.
            // I think this only happens when a constant variable is used in an array index of an array type.
#if 0
               printf ("Case of TransformationSupport::getStatement(varRefExp) == NULL explictlySpecifiedCurrentScope = %p \n",explictlySpecifiedCurrentScope);
#endif
            // DQ (7/24/2011): This fails for the tests/CompileTests/OpenMP_tests/objectLastprivate.cpp test code.
            // ROSE_ASSERT(explictlySpecifiedCurrentScope != NULL);
               if (explictlySpecifiedCurrentScope != NULL)
                  {
                    SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
                    ROSE_ASSERT(variableSymbol != NULL);
                    SgInitializedName* initializedName = variableSymbol->get_declaration();
                    ROSE_ASSERT(initializedName != NULL);
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initializedName->get_parent());
#if 1
                    currentStatement = explictlySpecifiedCurrentScope;
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("In case SgVarRefExp: (currentStatement == NULL) Calling nameQualificationDepth() variableDeclaration = %p initializedName->get_parent() = %p = %s \n",
                         variableDeclaration,initializedName->get_parent(),initializedName->get_parent()->class_name().c_str());
#endif
                 // ROSE_ASSERT(variableDeclaration != NULL);
                    if (variableDeclaration == NULL)
                       {
                      // DQ (7/11/2014): Test code test2014_84.C demonstrates this problem.
                         ROSE_ASSERT(explictlySpecifiedCurrentScope != NULL);

                         SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(initializedName->get_parent());
                         ROSE_ASSERT(functionParameterList != NULL);

                         int amountOfNameQualificationRequired = nameQualificationDepth(initializedName,explictlySpecifiedCurrentScope,currentStatement);
                         setNameQualification(varRefExp,functionParameterList,amountOfNameQualificationRequired);
                       }
                      else
                       {
                         int amountOfNameQualificationRequired = nameQualificationDepth(variableDeclaration,explictlySpecifiedCurrentScope,currentStatement);
                         setNameQualification(varRefExp,variableDeclaration,amountOfNameQualificationRequired);
                       }
#else
                    ROSE_ASSERT(variableDeclaration != NULL);
                    currentStatement = explictlySpecifiedCurrentScope;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("In case SgVarRefExp: (currentStatement == NULL) Calling nameQualificationDepth() \n");
#endif
                    int amountOfNameQualificationRequired = nameQualificationDepth(variableDeclaration,explictlySpecifiedCurrentScope,currentStatement);

                    setNameQualification(varRefExp,variableDeclaration,amountOfNameQualificationRequired);
#endif
                  }
             }

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // DQ (6/9/2011): Added support for test2011_79.C (enum values can require name qualification).
     SgEnumVal* enumVal = isSgEnumVal(n);
     if (enumVal != NULL)
        {
          SgScopeStatement* currentScope = NULL;

          SgEnumDeclaration* enumDeclaration = enumVal->get_declaration();
          ROSE_ASSERT(enumDeclaration != NULL);

          SgStatement* currentStatement = TransformationSupport::getStatement(enumVal);
       // ROSE_ASSERT(currentStatement != NULL);
#if 0
          printf ("case of SgEnumVal: currentStatement = %p \n",currentStatement);
#endif
          if (currentStatement != NULL)
             {
               currentScope = isSgScopeStatement(currentStatement);
#if 0
               printf ("case of SgEnumVal: currentStatement = %p = %s currentScope = %p = %s \n",
                    currentStatement,currentStatement->class_name().c_str(),currentScope,currentScope != NULL ? currentScope->class_name().c_str() : "NULL");
#endif
            // If the current statement was not a scope, then what scope contains the current statement.
               if (currentScope == NULL)
                  {
                 // DQ (5/24/2013): This is a better way to set the scope (see test2013_187.C).
                 // currentScope = currentStatement->get_scope();
                    ROSE_ASSERT(inheritedAttribute.get_currentScope() != NULL);
                    currentScope = inheritedAttribute.get_currentScope();
                  }
               ROSE_ASSERT(currentScope != NULL);
#if 0
               printf ("case of SgEnumVal (after setting currentScope): currentStatement = %p = %s currentScope = %p = %s \n",
                    currentStatement,currentStatement->class_name().c_str(),currentScope,currentScope != NULL ? currentScope->class_name().c_str() : "NULL");
#endif
               ROSE_ASSERT(inheritedAttribute.get_currentScope() != NULL);
#if 0
               printf ("case of SgEnumVal : inheritedAttribute.get_currentScope() = %p = %s \n",
                       inheritedAttribute.get_currentScope(),inheritedAttribute.get_currentScope()->class_name().c_str());
#endif
             }
            else
             {
            // If the enum value is contained in an index expression then currentStatement will be NULL.
            // But then the current scope should be known explicitly.
               currentScope = explictlySpecifiedCurrentScope;

            // DQ (9/17/2011); Added escape for where the currentScope == NULL (fails for STL code when the original expression trees are used to eliminate the constant folded values).
            // ROSE_ASSERT(currentScope != NULL);

            // Use the currentScope as the currentStatement
               currentStatement = currentScope;
             }

       // DQ (9/17/2011); Added escape for where the currentScope == NULL (fails for STL code when the original expression trees are used to eliminate the constant folded values).
       // ROSE_ASSERT(currentScope != NULL);
          if (currentScope != NULL)
             {
            // DQ (9/17/2011): this is the original case we waant to restore later...
               ROSE_ASSERT(currentScope != NULL);
               int amountOfNameQualificationRequired = nameQualificationDepth(enumDeclaration,currentScope,currentStatement);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("SgEnumVal: amountOfNameQualificationRequired = %d \n",amountOfNameQualificationRequired);
#endif
               setNameQualification(enumVal,enumDeclaration,amountOfNameQualificationRequired);
             }
            else
             {
            // DQ (9/17/2011): Added this case, print a warning and fix thiat after debugging the constant folding value elimination..
               printf ("WARNING: SgEnumVal name qualification not handled for the case of currentScope == NULL \n");
             }

#if 0
          printf ("Exiting as a test: case of SgEnumVal \n");
          ROSE_ASSERT(false);
#endif
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

            // DQ (1/26/2013): typeId operator can take either an expression or a type, get_type() returns the type independent of which is specified.
               case V_SgTypeIdOp: qualifiedType = typeIdOp->get_operand_type(); break;
               //case V_SgTypeIdOp: qualifiedType = typeIdOp->get_type(); break;

               default:
                  {
                 // Anything else should not make it this far...
                    printf ("Error: default reached in switch... n = %p = %s \n",n,n->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          if (qualifiedType == NULL)
             {
            // We see this case for test2006_139.C  (code is: "sizeof("string")" or "sizeof(<SgVarRefExp>)" ).
               printf ("Note: qualifiedType == NULL for n = %p = %s \n",n,n->class_name().c_str());
             }
#endif
       // ROSE_ASSERT(qualifiedType != NULL);

          if (skipQualification == false)
             {
            // DQ (1/26/2013): added assertion.
               ROSE_ASSERT(qualifiedType != NULL);
               SgDeclarationStatement* associatedTypeDeclaration = associatedDeclaration(qualifiedType);
               if (associatedTypeDeclaration != NULL)
                  {
                    SgStatement* currentStatement = TransformationSupport::getStatement(n);

                 // ROSE_ASSERT(currentStatement != NULL);
                    if (currentStatement != NULL)
                       {
                         SgScopeStatement* currentScope = currentStatement->get_scope();
                         ROSE_ASSERT(currentScope != NULL);

                         int amountOfNameQualificationRequiredForType = nameQualificationDepth(associatedTypeDeclaration,currentScope,currentStatement);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("SgExpression (name = %s) type: amountOfNameQualificationRequiredForType = %d \n",referenceToType->class_name().c_str(),amountOfNameQualificationRequiredForType);
#endif
                         setNameQualification(referenceToType,associatedTypeDeclaration,amountOfNameQualificationRequiredForType);

#if 0
                         printf ("Calling traverseType on referenceToType = %p = %s \n",referenceToType,referenceToType->class_name().c_str());
#endif
                      // DQ (6/3/2011): Traverse the type to set any possible template arguments (or other subtypes?) that require name qualification.
                         traverseType(qualifiedType,referenceToType,currentScope,currentStatement);
                       }
                      else
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("WARNING: currentStatement == NULL for case of referenceToType = %p = %s \n",referenceToType,referenceToType->class_name().c_str());
#endif
                       }
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("Note: associatedTypeDeclaration == NULL in SgExpression for name qualification support! referenceToType = %s \n",referenceToType->class_name().c_str());
#endif
                  }
             }
        }

  // DQ (6/21/2011): Added support for name qualification of expressions contained in originalExpressionTree's where they are stored.
     SgExpression* expression = isSgExpression(n);
     if (expression != NULL)
        {
          SgExpression* originalExpressionTree = expression->get_originalExpressionTree();
          if (originalExpressionTree != NULL)
             {
#if 0
            // DQ (7/23/2011): I don't think this code is required or executed (testing this!)
               printf ("I don't think this is executed since original expression tree's are traversed as part of the AST \n");
               ROSE_ASSERT(false);
#endif
            // Note that we have to pass the local copy of the referencedNameSet so that the same set will be used for all recursive calls (see test2011_89.C).
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("@@@@@@@@@@@@@ Recursive call to the originalExpressionTree = %p = %s \n",originalExpressionTree,originalExpressionTree->class_name().c_str());
#endif
            // DQ (6/30/2013): Added to support using generateNestedTraversalWithExplicitScope() instead of generateNameQualificationSupport().
               SgStatement* currentStatement = TransformationSupport::getStatement(n);
#if 1
            // DQ (9/14/2015): Added debugging code.
            // DQ (9/14/2015): This can be an expression in a type, in which case we don't have an associated scope.
               if (currentStatement == NULL)
                  {
                 // This can be an expression in a type, in which case we don't have an associated scope.
                    printf ("Note: This can be an expression in a type, in which case we don't have an associated scope: expression = %p = %s originalExpressionTree = %p = %s \n",
                         expression,expression->class_name().c_str(),originalExpressionTree,originalExpressionTree->class_name().c_str());
                  }
                 else
                  {
                    ROSE_ASSERT(currentStatement != NULL);
                    SgScopeStatement* currentScope = currentStatement->get_scope();
                    ROSE_ASSERT(currentScope != NULL);

                 // DQ (6/30/2013): For the recursive call use generateNestedTraversalWithExplicitScope() instead of generateNameQualificationSupport().
                 // generateNameQualificationSupport(originalExpressionTree,referencedNameSet);
                    generateNestedTraversalWithExplicitScope(originalExpressionTree,currentScope);
                  }
#else
               ROSE_ASSERT(currentStatement != NULL);
               SgScopeStatement* currentScope = currentStatement->get_scope();
               ROSE_ASSERT(currentScope != NULL);

            // DQ (6/30/2013): For the recursive call use generateNestedTraversalWithExplicitScope() instead of generateNameQualificationSupport().
            // generateNameQualificationSupport(originalExpressionTree,referencedNameSet);
               generateNestedTraversalWithExplicitScope(originalExpressionTree,currentScope);
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("@@@@@@@@@@@@@ DONE: Recursive call to the originalExpressionTree = %p = %s \n",originalExpressionTree,originalExpressionTree->class_name().c_str());
#endif
             }
        }

  // DQ (6/25/2011): Added support for use from unparseToString().
  // I don't think that we need this case since the unparser handles the case of using 
  // the fully qualified name directly when called from the unparseToString() function.
     SgType* type = isSgType(n);
     if (type != NULL)
        {
#if 0
          printf ("Found a type in the evaluation of name qualification type = %p = %s \n",type,type->class_name().c_str());
#endif
       // void NameQualificationTraversal::traverseType ( SgType* type, SgNode* nodeReferenceToType, SgScopeStatement* currentScope, SgStatement* positionStatement )
       // SgNode* nodeReferenceToType    = NULL;
       // SgScopeStatement* currentScope = NULL;
       // SgStatement* positionStatement = NULL;
       // traverseType(type,initializedName,currentScope,currentStatement);
        }

  // ******************************************************************************
  // Now that this declaration is pocessed, mark it as being seen (place into set).
  // ******************************************************************************

     SgDeclarationStatement* declaration = isSgDeclarationStatement(n);
     if (declaration != NULL)
        {
#if 0
          printf ("Found a SgDeclarationStatement in the evaluation of name qualification declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
       // If this is a declaration of something that has a name then we need to mark it as having been seen.

       // In some cases of C++ name qualification depending on if the defining declaration (or a forward 
       // declaration is in a scope that would define the declaration to a scope where the declaration could be 
       // present).  This detail is handled by reporting if such a declaration has been seen yet.  Since the 
       // preorder traversal is the same as the traversal used in the unparsing it is sufficient to record
       // the order of the processing here and not complicate the unparser directly.  Note that the use of
       // function declarations follow these rules and so are a problem when the prototype is defined in a
       // function (where it does not communicate the defining declarations location) instead of in a global 
       // scope or namespace scope (where it does appear to communicate its position.

       // SgDeclarationStatement* firstNondefiningDeclaration   = declaration->get_firstNondefiningDeclaration();
          SgDeclarationStatement* declarationForReferencedNameSet = declaration->get_firstNondefiningDeclaration();
       // ROSE_ASSERT(declarationForReferencedNameSet == NULL);
       // declarationForReferencedNameSet = declaration->get_firstNondefiningDeclaration();

          if (declarationForReferencedNameSet == NULL)
             {
            // Note that a function with only a defining declaration will not have a nondefining declaration 
            // automatically constructed in the AST (unlike classes and some other sorts of declarations).
               declarationForReferencedNameSet = declaration->get_definingDeclaration();

            // DQ (6/22/2011): I think this is true.  This assertion fails for test2006_78.C (a template example code).
            // ROSE_ASSERT(declarationForReferencedNameSet == declaration);

            // DQ (6/23/2011): This assertion fails for the LoopProcessor on tests/roseTests/loopProcessingTests/mm.C
            // ROSE_ASSERT(declarationForReferencedNameSet != NULL);
               if (declarationForReferencedNameSet == NULL)
                  {
                    declarationForReferencedNameSet = declaration;
                    ROSE_ASSERT(declarationForReferencedNameSet != NULL);
                  }
               ROSE_ASSERT(declarationForReferencedNameSet != NULL);
             }
          ROSE_ASSERT(declarationForReferencedNameSet != NULL);
#if 0
       // ROSE_ASSERT(firstNondefiningDeclaration != NULL);
          if (firstNondefiningDeclaration == NULL)
             {
               printf ("WARNING: declaration->get_firstNondefiningDeclaration() == NULL for declaration = %p = %s \n",declaration,declaration->class_name().c_str());
             }
#endif
       // Look at each declaration, but as soon as we find an acceptable one put the declarationForReferencedNameSet
       // into the set so that we can search on a uniform representation of the declaration. Note that we want the 
       // scope of where it is located and not it's scope if it were name qualified...
       // SgScopeStatement* scopeOfNondefiningDeclaration = isSgScopeStatement(firstNondefiningDeclaration->get_parent());
          ROSE_ASSERT(declaration->get_parent() != NULL);
          SgScopeStatement* scopeOfDeclaration = isSgScopeStatement(declaration->get_parent());

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
                 // DQ (6/22/2011): Note that a declaration in a typedef is an acceptable scope under some cases (not clear on the limits of this case).
                    default:
                       {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                         printf ("scopeOfNondefiningDeclaration = %p = %s \n",scopeOfDeclaration,scopeOfDeclaration->class_name().c_str());
#endif
                         acceptableDeclarationScope = true;
                       }
                  }
             }
            else
             {
            // This appears to fail for something in rose_edg_required_macros_and_functions.h.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("I hope that we can make this an error (scopeOfDeclaration == NULL) declaration = %p = %s declaration->get_parent() = %p = %s \n",declaration,declaration->class_name().c_str(),declaration->get_parent(),declaration->get_parent()->class_name().c_str());
#endif
            // ROSE_ASSERT(false);
             }

          ROSE_ASSERT(declarationForReferencedNameSet != NULL);
       // if (referencedNameSet.find(firstNondefiningDeclaration) == referencedNameSet.end())
       // if (acceptableDeclarationScope == true && firstNondefiningDeclaration != NULL && referencedNameSet.find(firstNondefiningDeclaration) == referencedNameSet.end())
          if (acceptableDeclarationScope == true && referencedNameSet.find(declarationForReferencedNameSet) == referencedNameSet.end())
             {
            // printf ("Adding firstNondefiningDeclaration = %p = %s to set of visited declarations \n",firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str());
            // referencedNameSet.insert(firstNondefiningDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Adding declarationForReferencedNameSet = %p = %s to set of visited declarations \n",declarationForReferencedNameSet,declarationForReferencedNameSet->class_name().c_str());
#endif
               referencedNameSet.insert(declarationForReferencedNameSet);
             }
            else
             {
            // printf ("firstNondefiningDeclaration = %p NOT added to referencedNameSet \n",firstNondefiningDeclaration);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("declarationForReferencedNameSet = %p NOT added to referencedNameSet \n",declarationForReferencedNameSet);
#endif
             }
        }

  // DQ (7/12/2014): Add any possible nodes that can generate SgAliasSymbols to the SgSymbolTable::p_aliasSymbolCausalNodeSet SgNodeSet.
  // This is used by the symbol table to know when to use or ignore SgAliasSymbols in symbol table lookups.
     if (isSgUsingDirectiveStatement(n) != NULL || isSgUsingDeclarationStatement(n) != NULL || isSgBaseClass(n) != NULL)
        {
       // SgNodeSet & get_aliasSymbolCausalNodeSet()

          SgSymbolTable::get_aliasSymbolCausalNodeSet().insert(n);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In NameQualificationTraversal::evaluateInheritedAttribute(): Added SgAliasSymbols causal node = %p = %s to SgSymbolTable::p_aliasSymbolCausalNodeSet size = %" PRIuPTR " \n",
               n,n->class_name().c_str(),SgSymbolTable::get_aliasSymbolCausalNodeSet().size());
#endif

#if 1
       // DQ (12/23/2015): This does not appear to have any effect (the SgBaseClass is not traversed in the AST).
          if (isSgBaseClass(n) != NULL)
             {
               printf ("NameQualificationTraversal::evaluateInheritedAttribute(): Identified SgBaseClass in traversal \n");
               ROSE_ASSERT(false); 
             }
#endif
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("****************************************************** \n");
     printf ("Leaving NameQualificationTraversal::evaluateInheritedAttribute(): node = %p = %s \n",n,n->class_name().c_str());
     printf ("******************************************************\n\n\n");
     SgLocatedNode* locatedNode = isSgLocatedNode(n);
     if (locatedNode != NULL)
        {
          locatedNode->get_file_info()->display("Leaving NameQualificationTraversal::evaluateInheritedAttribute()");
        }
     printf ("******************************************************\n\n\n");
     printf ("******************************************************\n\n\n");
#endif

     return NameQualificationInheritedAttribute(inheritedAttribute);
   }


NameQualificationSynthesizedAttribute
NameQualificationTraversal::evaluateSynthesizedAttribute(SgNode* n, NameQualificationInheritedAttribute inheritedAttribute, SynthesizedAttributesList synthesizedAttributeList)
   {
  // This is not used now but will likely be used later.
     NameQualificationSynthesizedAttribute returnAttribute;

// #if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
#if 0
     printf ("\n\n****************************************************** \n");
     printf ("****************************************************** \n");
     printf ("Inside of NameQualificationTraversal::evaluateSynthesizedAttribute(): node = %p = %s = %s \n",n,n->class_name().c_str(),SageInterface::get_name(n).c_str());
     printf ("****************************************************** \n");
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
  // DQ (6/23/2013): Output the generated name with required name qualification for debugging.
     switch(n->variantT())
        {
       // DQ (8/19/2013): Added case to support debugging.
          case V_SgConstructorInitializer:

          case V_SgMemberFunctionRefExp:
             {
               printf ("************************************************************************************** \n");
               printf ("In evaluateSynthesizedAttribute(): node = %p = %s node->unparseToString() = %s \n",n,n->class_name().c_str(),n->unparseToString().c_str());              
               printf ("************************************************************************************** \n");
               break;
             }
     
          default:
             {
            // do nothing
             }
        }
#endif

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
// Note also that name qualifiction can be required on expressions that are a part of 
// the originalExpressionTree that represent the expanded representation from constant 
// folding.  Thus we have to make a recursive call on all valid originalExpressionTree
// pointers where they are present:
//     SgBinaryOp
//     SgValueExp
//     SgFunctionRefExp
//     SgValueExp
//     SgCastExp
// ************************************************************************************

void
NameQualificationTraversal::setNameQualification(SgVarRefExp* varRefExp, SgVariableDeclaration* variableDeclaration, int amountOfNameQualificationRequired)
   {
  // This is where we hide the details of translating the intepretation of the amountOfNameQualificationRequired
  // which can be greater than the number of nested scopes to a representation that is bounded by the number of 
  // nested scopes and sets the global qualification to be true. If I decide I don't like this here, then we
  // might find a way to handling this point more directly later. This at least gets it set properly in the AST.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // DQ (7/31/2012): check if this is a SgVarRefExp that is associated with a class that is un-named, if so then 
  // supress the name qualification (which would use the internally generated name).  Note that all constructs 
  // that are un-named have names generated internally for them so that we can support the AST merge process
  // and generally reference multiple un-named constructs that may exist in a single compilation unit.
  // SgClassDeclaration* classDeclaration = isSgClassDeclaration(varRefExp->parent());

     ROSE_ASSERT(varRefExp != NULL);
     SgBinaryOp* dotExp   = isSgDotExp(varRefExp->get_parent());
     SgBinaryOp* arrowExp = isSgArrowExp(varRefExp->get_parent());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("dotExp = %p arrowExp = %p \n",dotExp,arrowExp);
#endif

     SgVarRefExp* possibleClassVarRefExp = NULL;
     if (dotExp != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Note that this code is overly sensitive to the local structure of the AST expressions \n");
#endif
          possibleClassVarRefExp = isSgVarRefExp(dotExp->get_lhs_operand());

          if (possibleClassVarRefExp == NULL)
             {
               SgPntrArrRefExp* possiblePntrArrRefExp = isSgPntrArrRefExp(dotExp->get_lhs_operand());
               if (possiblePntrArrRefExp != NULL)
                  {
                    possibleClassVarRefExp = isSgVarRefExp(possiblePntrArrRefExp->get_lhs_operand());
                  }
             }
        }

     if (arrowExp != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Note that this code is overly sensitive to the local structure of the AST expressions \n");
#endif
          possibleClassVarRefExp = isSgVarRefExp(arrowExp->get_lhs_operand());

          if (possibleClassVarRefExp == NULL)
             {
               SgPntrArrRefExp* possiblePntrArrRefExp = isSgPntrArrRefExp(arrowExp->get_lhs_operand());
               if (possiblePntrArrRefExp != NULL)
                  {
                    possibleClassVarRefExp = isSgVarRefExp(possiblePntrArrRefExp->get_lhs_operand());
                  }
             }
#if 0
          printf ("Case of SgVarRefExp to un-named class not finished yet! \n");
          ROSE_ASSERT(false);
#endif
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("possibleClassVarRefExp = %p \n",possibleClassVarRefExp);
#endif

     SgClassType* classType = NULL;
     if (possibleClassVarRefExp != NULL)
        {
          SgType* varRefExpType = possibleClassVarRefExp->get_type(); 
          ROSE_ASSERT(varRefExpType != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In NameQualificationTraversal::setNameQualification(): varRefExpType = %p = %s \n",varRefExpType,varRefExpType->class_name().c_str());
#endif

       // DQ (8/2/2012): test2007_06.C and test2012_156.C show that we need to strip past the typedefs.
       // Note that we don't want to strip typedefs, since that could take us past public types and into private types.
       // SgType* possibleClassType = varRefExpType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE); // Excluding SgType::STRIP_TYPEDEF_TYPE
          SgType* possibleClassType = varRefExpType->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE|SgType::STRIP_TYPEDEF_TYPE);
          classType = isSgClassType(possibleClassType);
        }


     bool isAnUnamedConstructs = false;
     if (classType != NULL)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In NameQualificationTraversal::setNameQualification(): classType = %p = %s \n",classType,classType->class_name().c_str());
#endif
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
          ROSE_ASSERT(classDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In NameQualificationTraversal::setNameQualification(): classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
#endif

       // DQ (9/4/2012): I don't think that the defining declaration should have to exist.
       // However this was a previously passing test for all of the regression tests.
       // ROSE_ASSERT(classDeclaration->get_definingDeclaration() != NULL);
          if (classDeclaration->get_definingDeclaration() != NULL)
             {
               SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
               if (definingClassDeclaration == NULL)
                  {
                    printf ("ERROR: definingClassDeclaration == NULL: classDeclaration->get_definingDeclaration() = %p = %s \n",classDeclaration->get_definingDeclaration(),classDeclaration->get_definingDeclaration()->class_name().c_str());
                  }
               ROSE_ASSERT(definingClassDeclaration != NULL);

            // This should be true so assert this here.
               ROSE_ASSERT(classDeclaration->get_isUnNamed() == definingClassDeclaration->get_isUnNamed());
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("WARNING: classDeclaration->get_definingDeclaration() == NULL: This was a previously passing test, but not now that we have force SgTemplateTypes to be handled in the local type table. \n");
#endif
             }

          if (classDeclaration->get_isUnNamed() == true)
             {
               isAnUnamedConstructs = true;
#if 0
               printf ("Error: This is an un-named class/struct and so we can't generate name qualification for it's data members. \n");
               ROSE_ASSERT(false);
#endif
             }
        }

  // DQ (7/31/2012): If this is an un-named construct then no qualifiaction can be used since there is no associated name.
     if (isAnUnamedConstructs == false)
        {
          string qualifier = setNameQualificationSupport(variableDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

          varRefExp->set_global_qualification_required(outputGlobalQualification);
          varRefExp->set_name_qualification_length(outputNameQualificationLength);

       // There should be no type evaluation required for a variable reference, as I recall.
          ROSE_ASSERT(outputTypeEvaluation == false);
          varRefExp->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In NameQualificationTraversal::setNameQualification(): varRefExp->get_name_qualification_length()     = %d \n",varRefExp->get_name_qualification_length());
          printf ("In NameQualificationTraversal::setNameQualification(): varRefExp->get_type_elaboration_required()     = %s \n",varRefExp->get_type_elaboration_required() ? "true" : "false");
          printf ("In NameQualificationTraversal::setNameQualification(): varRefExp->get_global_qualification_required() = %s \n",varRefExp->get_global_qualification_required() ? "true" : "false");
#endif

          if (qualifiedNameMapForNames.find(varRefExp) == qualifiedNameMapForNames.end())
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),varRefExp,varRefExp->class_name().c_str());
#endif
               qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(varRefExp,qualifier));
             }
            else
             {
            // DQ (6/20/2011): We see this case in test2011_87.C.
            // If it already existes then overwrite the existing information.
               std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(varRefExp);
               ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               string previousQualifier = i->second.c_str();
               printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
               if (i->second != qualifier)
                  {
                 // DQ (7/23/2011): Multiple uses of the SgVarRefExp expression in SgArrayType will cause
                 // the name qualification to be reset each time.  This is OK since it is used to build
                 // the type name that will be saved.
                    i->second = qualifier;
#if 0
                    printf ("Note: name in qualifiedNameMapForNames already exists and is different... \n");
#endif
                  }
             }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgFunctionRefExp* functionRefExp, SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): functionRefExp->get_name_qualification_length()     = %d \n",functionRefExp->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): functionRefExp->get_type_elaboration_required()     = %s \n",functionRefExp->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): functionRefExp->get_global_qualification_required() = %s \n",functionRefExp->get_global_qualification_required() ? "true" : "false");
#endif

  // DQ (5/2/2012): I don't think that global qualification is allowed for friend functions (so test for this).
  // test2012_59.C is an example of this issue.
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_declarationModifier().isFriend() = %s \n",functionDeclaration->get_declarationModifier().isFriend() ? "true" : "false");
     if (functionDeclaration->get_firstNondefiningDeclaration() != NULL)
          printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_firstNondefiningDeclaration()->get_declarationModifier().isFriend() = %s \n",functionDeclaration->get_firstNondefiningDeclaration()->get_declarationModifier().isFriend() ? "true" : "false");
     if (functionDeclaration->get_definingDeclaration() != NULL)
          printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_definingDeclaration()->get_declarationModifier().isFriend()         = %s \n",functionDeclaration->get_definingDeclaration()->get_declarationModifier().isFriend() ? "true" : "false");
#endif

  // Look for friend declaration on both declaration (defining and non-defining).
     bool isFriend = false;
     if (functionDeclaration->get_firstNondefiningDeclaration() != NULL)
        {
          isFriend = isFriend || functionDeclaration->get_firstNondefiningDeclaration()->get_declarationModifier().isFriend();
        }
     if (functionDeclaration->get_definingDeclaration() != NULL)
        {
          isFriend = isFriend || functionDeclaration->get_definingDeclaration()->get_declarationModifier().isFriend();
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("isFriend                                       = %s \n",isFriend ? "true" : "false");
     printf ("functionDeclaration->get_definingDeclaration() = %p \n",functionDeclaration->get_definingDeclaration());
#endif

  // DQ (4/2/2014): After discusion with Markus, this is a problem that is a significant
  // issue and requires a general solution that would be useful more generally than just 
  // to this specific problem.  We need to build a data stucture and hide it behind a
  // class with an appropriate API.  The data structure should have a container of
  // non-defining declarations for each first-non-defining declaration.  Thus we would
  // have a way to find all of the non-defining declarations associated with any
  // function and thus query if one of them was declard in a scope that defined its
  // scope definatively.  This class should be a part of an AST Information sort of 
  // object that we would use to collect similar analysis information that would be
  // seperate from the AST, but might be used with the AST for certain purposes
  // (e.g. removning all functions including all associated non-defining declarations).

  // DQ (4/6/2014): Adding support for new analysis results. Fails for test2013_242.C.
     ROSE_ASSERT(declarationSet != NULL);
  // ROSE_ASSERT(declarationSet->getDeclarationMap().size() != 0);

  // DQ (4/1/2014): It might be that we need a still better test (such as if it a friend function).
     if (isFriend == true)
        {
          if (functionDeclaration->get_definingDeclaration() == NULL)
             {
            // We need to check if there is a non-defining declaration (beyond the friend declaration
            // in the Class definition) appearing in a named scope (which specifies the scope where 
            // the defining declaration would appear.

               SgDeclarationStatement* firstNondefiningDeclaration = functionDeclaration->get_firstNondefiningDeclaration();
               ROSE_ASSERT(firstNondefiningDeclaration != NULL);
               bool declarationIsLocatedInDefiningScope = declarationSet->isLocatedInDefiningScope(firstNondefiningDeclaration);

            // This is required to allow test2013_115.C to pass.
            // declarationIsLocatedInDefiningScope = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("declarationIsLocatedInDefiningScope = %s \n",declarationIsLocatedInDefiningScope ? "true" : "false");
#endif
               if (declarationIsLocatedInDefiningScope == true)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("NOTE: using global qualification because there is a declaration in a defining scope \n");
#endif
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("WARNING: skipping global qualification because there is no defining declaration \n");
#endif
                    outputGlobalQualification = false;
                    qualifier = "";
                  }
             }
            else
             {
                SgClassDefinition* classDefinition = isSgClassDefinition(functionDeclaration->get_definingDeclaration()->get_parent());
             // if (functionDeclaration->get_definingDeclaration() != NULL && functionDeclaration->get_definingDeclaration()->get_parent() == functionDeclaration->get_scope())
                if (functionDeclaration->get_definingDeclaration() != NULL && classDefinition != NULL)
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("WARNING: skipping global qualification because the defining declaration is defined in the class declaration \n");
#endif
                    outputGlobalQualification = false;
                    qualifier = "";
                  }
                 else
                  {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("WARNING: allow global name qualification if required since function is defined outside of the class \n");
#endif
                 // DQ (11/26/2015): See test2012_59.C for an example of where this is required to be turned off.
                 // In this case it is associated with a case of multiple defining declarations due to EDG template function normalization.
                    outputGlobalQualification = false;
                    qualifier = "";
                  }
             }
        }

#if 0
  // DQ (4/1/2014): This causes test2014_29.C to fail I think it might be that the defining declaration 
  // that nails down the location (scope) has been seen, but it id had not then we would want to 
  // disable name qualification.
     if (outputGlobalQualification == true && isFriend == true)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("WARNING: We can't specify global qualification of friend function (qualifier reset to be empty string) \n");
#endif
       // Note that I think this might only be an issue where outputNameQualificationLength == 0.
          ROSE_ASSERT (outputNameQualificationLength == 0);

       // Reset the values (and the qualifier string).
       // outputNameQualificationLength = 0;
          outputGlobalQualification = false;
          qualifier = "";
        }
#endif

  // printf ("In NameQualificationTraversal::setNameQualification(): qualifier = %s \n",qualifier.c_str());

     if (qualifiedNameMapForNames.find(functionRefExp) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for (SgFunctionRefExp) name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionRefExp,functionRefExp->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(functionRefExp,qualifier));

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Testing name in map: for SgFunctionRefExp = %p qualified name = %s \n",functionRefExp,functionRefExp->get_qualified_name_prefix().str());
          printf ("SgNode::get_globalQualifiedNameMapForNames().size() = %" PRIuPTR " \n",SgNode::get_globalQualifiedNameMapForNames().size());
#endif
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(functionRefExp);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               i->second = qualifier;
#if 1
               printf ("Error: name in qualifiedNameMapForNames already exists and is different... \n");
               ROSE_ASSERT(false);
#endif
             }
        }
   }

void
NameQualificationTraversal::setNameQualification(SgMemberFunctionRefExp* functionRefExp, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): memberFunctionRefExp->get_name_qualification_length()     = %d \n",functionRefExp->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): memberFunctionRefExp->get_type_elaboration_required()     = %s \n",functionRefExp->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): memberFunctionRefExp->get_global_qualification_required() = %s \n",functionRefExp->get_global_qualification_required() ? "true" : "false");
#endif
     if (qualifiedNameMapForNames.find(functionRefExp) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting (memberFunction) qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionRefExp,functionRefExp->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(functionRefExp,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(functionRefExp);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
          if (i->second != qualifier)
             {
               i->second = qualifier;
#if 1
               printf ("Error: name in qualifiedNameMapForNames already exists and is different... \n");
               ROSE_ASSERT(false);
#endif
             }
        }
   }


// DQ (6/4/2011): This function handles a specific case that is demonstrated by test2005_42.C.
// DQ (6/1/2011): Added support for qualification of the SgConstructorInitializer.
// void NameQualificationTraversal::setNameQualification(SgConstructorInitializer* constructorInitializer, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
void
NameQualificationTraversal::setNameQualification(SgConstructorInitializer* constructorInitializer, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // DQ (6/4/2011): This handles the case of both the declaration being a SgMemberFunctionDeclaration and a SgClassDeclaration.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(declaration->get_scope(), amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     constructorInitializer->set_global_qualification_required(outputGlobalQualification);
     constructorInitializer->set_name_qualification_length(outputNameQualificationLength);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);
     constructorInitializer->set_type_elaboration_required(outputTypeEvaluation);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): constructorInitializer->get_name_qualification_length()     = %d \n",constructorInitializer->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): constructorInitializer->get_type_elaboration_required()     = %s \n",constructorInitializer->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): constructorInitializer->get_global_qualification_required() = %s \n",constructorInitializer->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(constructorInitializer) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),constructorInitializer,constructorInitializer->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(constructorInitializer,qualifier));
        }
       else
        {
       // DQ (2/12/2012): Fixing support where the name qualification must be rewritten where it is used in a different context.
       // this appears to be a common requirement.  This case appears to not have been a problem before but is now with the 
       // new EDG 4.3 support.  This has been added because of the requirements of that support.

       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(constructorInitializer);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
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
       // DQ (2/12/2012): commented this code out.
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
#endif
        }

  // DQ (6/4/2011): Added test...
     ROSE_ASSERT(SgNode::get_globalQualifiedNameMapForNames().find(constructorInitializer) != SgNode::get_globalQualifiedNameMapForNames().end());
   }


void
NameQualificationTraversal::setNameQualification(SgEnumVal* enumVal, SgEnumDeclaration* enumDeclaration, int amountOfNameQualificationRequired)
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): enumVal->get_name_qualification_length()     = %d \n",enumVal->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): enumVal->get_type_elaboration_required()     = %s \n",enumVal->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): enumVal->get_global_qualification_required() = %s \n",enumVal->get_global_qualification_required() ? "true" : "false");
#endif
     if (qualifiedNameMapForNames.find(enumVal) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),enumVal,enumVal->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(enumVal,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(enumVal);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
            // DQ (5/3/2013): Note that this happens for test2013_144.C where the enumValue is used as the size 
            // in the array type (and the SgArrayType is shared). See comments in the test code for how this 
            // might be improved (forcing name qualification).
               i->second = qualifier;

#if 0
               enumVal->get_file_info()->display("In NameQualificationTraversal::setNameQualification(): enumVal: Where is this located");
#endif

#if 0
               printf ("Error: name in qualifiedNameMapForNames already exists and is different... \n");
               ROSE_ASSERT(false);
#endif
             }
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgBaseClass* baseClass, SgClassDeclaration* classDeclaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(classDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     baseClass->set_global_qualification_required(outputGlobalQualification);
     baseClass->set_name_qualification_length(outputNameQualificationLength);
     baseClass->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): baseClass->get_name_qualification_length()     = %d \n",baseClass->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): baseClass->get_type_elaboration_required()     = %s \n",baseClass->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): baseClass->get_global_qualification_required() = %s \n",baseClass->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(baseClass) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),baseClass,baseClass->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(baseClass,qualifier));
        }
       else
        {
       // DQ (6/17/2013): I think it is reasonable that this might have been previously set and 
       // we have to overwrite the last value as we handle it again in a different context.

       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(baseClass);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;
#if 1
            // DQ (6/17/2013): Commented out this assertion.
               printf ("Error: name in qualifiedNameMapForNames already exists and is different... \n");
               ROSE_ASSERT(false);
#else
            // DQ (6/17/2013): I think this is OK, but I'm not certain (see test2012_57.C).
               printf ("WARNING: name in qualifiedNameMapForNames already exists and is different... (reset) \n");
#endif
             }
#if 0
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
#endif
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired )
   {
  // This takes only a SgFunctionDeclaration since it is where we locate the name qualification information AND
  // is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // printf ("\n************************************************ \n");

     string qualifier = setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

  // DQ (9/7/2014): Added suppor for where this is a template member or non-member function declaration and we need to genrate the name with the associated template header.
     string template_header;
     SgTemplateFunctionDeclaration*       templateFunctionDeclaration       = isSgTemplateFunctionDeclaration(functionDeclaration);
     SgTemplateMemberFunctionDeclaration* templateMemberFunctionDeclaration = isSgTemplateMemberFunctionDeclaration(functionDeclaration);
     bool buildTemplateHeaderString = (templateFunctionDeclaration != NULL || templateMemberFunctionDeclaration != NULL);
     if (buildTemplateHeaderString == true)
        {
       // DQ (9/7/2014): First idea, but not likely to work...and too complex.
       // Note that another aspect of this implementation might be that we save a set of template class 
       // declarations so that we can match types in the function's parameter list against the template class declaration 
       // set so that we know when to build function parameter types as template types vs. template instantiation types.
       // This would require that we save a more complex data structure than a simple string.  It is also not clear if
       // all references to a template class instantiation could be assumed to be references to it's template declaration?
       // Or maybe the problem is that there is some other function parameter lis that we need to consult.

       // DQ (9/7/2014): Better:
       // A better solution would be to make sure that we generate type in the EDG/ROSE translation using the template 
       // function's paramter list associated with the first non-defining declaration (instead of the one being generated
       // as part of building the defining declaration (which is using the same a_routine_ptr as that used to build the 
       // template instantiation.  As a result we a mixing the types in the defining template declaration with that of the
       // defining template instantiation (which is always wrong).  So the simple solution is to just use the types from
       // the non-defining template member or non-member function declaration.  The same should apply to the function 
       // return type.  This is the simplest solution to date.

       // DQ (9/8/2014): The best solution was to translate the defining non-template function declarations when we saw them
       // as defining declarations, but only put the non-defining declaration into the class template (to match the normalization
       // done by EDG) and then attach the defining template declaration ahead of the first associated template instantiation.
       // This appears to work well and will soon be evaluated for further tests.
#if 0
          printf ("WARNING: technical problem with function paramter types of template functions (should maybe not be template instantiations) \n");
#endif
          template_header = setTemplateHeaderNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired);
#if 0
          printf ("In NameQualificationTraversal::setNameQualification(): qualifier = %s template_header = %s \n",qualifier.c_str(),template_header.c_str());
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_declarationModifier().isFriend() = %s \n",functionDeclaration->get_declarationModifier().isFriend() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): outputNameQualificationLength                             = %d \n",outputNameQualificationLength);
     printf ("In NameQualificationTraversal::setNameQualification(): outputGlobalQualification                                 = %s \n",outputGlobalQualification ? "true" : "false");
#endif

  // DQ (2/16/2013): Note that test2013_67.C is a case where name qualification of the friend function is required.
  // I think it is because it is a non defining declaration instead of a defining declaration.
  // DQ (3/31/2012): I don't think that global qualification is allowed for friend functions (so test for this).
  // test2012_57.C is an example of this issue.
  // if (outputGlobalQualification == true && functionDeclaration->get_declarationModifier().isFriend() == true)
     if ( (outputGlobalQualification == true) && (functionDeclaration->get_declarationModifier().isFriend() == true) && (functionDeclaration == functionDeclaration->get_definingDeclaration()))
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("WARNING: We can't specify global qualification of friend function (qualifier reset to be empty string) \n");
#endif
       // Note that I think this might only be an issue where outputNameQualificationLength == 0.
          ROSE_ASSERT (outputNameQualificationLength == 0);

       // Reset the values (and the qualifier string).
       // outputNameQualificationLength = 0;
          outputGlobalQualification = false;
          qualifier = "";
        }

     functionDeclaration->set_global_qualification_required(outputGlobalQualification);
     functionDeclaration->set_name_qualification_length(outputNameQualificationLength);
     functionDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_name_qualification_length()     = %d \n",functionDeclaration->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_type_elaboration_required()     = %s \n",functionDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_global_qualification_required() = %s \n",functionDeclaration->get_global_qualification_required() ? "true" : "false");

     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration = %p firstNondefiningDeclaration() = %p \n",functionDeclaration,functionDeclaration->get_firstNondefiningDeclaration());
     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration = %p definingDeclaration()         = %p \n",functionDeclaration,functionDeclaration->get_definingDeclaration());
#endif

     if (qualifiedNameMapForNames.find(functionDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(functionDeclaration,qualifier));
        }
       else
        {
       // If it already exists then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(functionDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if 1
            // DQ (3/31/2012): Commented out this assertion.
               printf ("Error: name in qualifiedNameMapForNames already exists and is different... \n");
               ROSE_ASSERT(false);
#else
            // DQ (3/31/2012): I think this is OK, but I'm not certain (see test2012_57.C).
               printf ("WARNING: name in qualifiedNameMapForNames already exists and is different... (reset) \n");
#endif
             }
        }

     if (buildTemplateHeaderString == true)
        {
       // Add the template header string to a new map.
#if 0
          printf ("Add the template header string to a new map: template_header = %s \n",template_header.c_str());
#endif

          if (qualifiedNameMapForTemplateHeaders.find(functionDeclaration) == qualifiedNameMapForTemplateHeaders.end())
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Inserting qualifier for template header = %s into list at IR node = %p = %s \n",template_header.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
#endif
               qualifiedNameMapForTemplateHeaders.insert(std::pair<SgNode*,std::string>(functionDeclaration,template_header));
             }
            else
             {
            // If it already exists then overwrite the existing information.
               std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTemplateHeaders.find(functionDeclaration);
               ROSE_ASSERT (i != qualifiedNameMapForTemplateHeaders.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               string previous_template_header = i->second.c_str();
               printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previous_template_header.c_str(),template_header.c_str());
#endif
               if (i->second != template_header)
                  {
                    i->second = template_header;
#if 1
                 // DQ (9/7/2014): Make this an error.
                    printf ("Error: name in qualifiedNameMapForTemplateHeaders already exists and is different... \n");
                    ROSE_ASSERT(false);
#else
                 // DQ (9/7/2014): Let's not alow this.
                    printf ("WARNING: name in qualifiedNameMapForTemplateHeaders already exists and is different... (reset) \n");
#endif
                  }
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }


  // printf ("****************** DONE ******************** \n\n");
   }

// void NameQualificationTraversal::setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired )
void
NameQualificationTraversal::setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired ) 
   {
  // This takes only a SgFunctionDeclaration since it is where we locate the name qualification information AND
  // is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     functionDeclaration->set_global_qualification_required_for_return_type(outputGlobalQualification);
     functionDeclaration->set_name_qualification_length_for_return_type(outputNameQualificationLength);
     functionDeclaration->set_type_elaboration_required_for_return_type(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_name_qualification_length_for_return_type()     = %d \n",functionDeclaration->get_name_qualification_length_for_return_type());
     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_type_elaboration_required_for_return_type()     = %s \n",functionDeclaration->get_type_elaboration_required_for_return_type() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): functionDeclaration->get_global_qualification_required_for_return_type() = %s \n",functionDeclaration->get_global_qualification_required_for_return_type() ? "true" : "false");
#endif

     if (qualifiedNameMapForTypes.find(functionDeclaration) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(functionDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(functionDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
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
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     usingDeclaration->set_global_qualification_required(outputGlobalQualification);
     usingDeclaration->set_name_qualification_length(outputNameQualificationLength);
     usingDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_name_qualification_length()     = %d \n",usingDeclaration->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_type_elaboration_required()     = %s \n",usingDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_global_qualification_required() = %s \n",usingDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(usingDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDeclaration,usingDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(usingDeclaration,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgInitializedName* associatedInitializedName, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(associatedInitializedName->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     usingDeclaration->set_global_qualification_required(outputGlobalQualification);
     usingDeclaration->set_name_qualification_length(outputNameQualificationLength);
     usingDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_name_qualification_length()     = %d \n",usingDeclaration->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_type_elaboration_required()     = %s \n",usingDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): usingDeclaration->get_global_qualification_required() = %s \n",usingDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(usingDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDeclaration,usingDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(usingDeclaration,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }


void
NameQualificationTraversal::setNameQualification ( SgUsingDirectiveStatement* usingDirective, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     usingDirective->set_global_qualification_required(outputGlobalQualification);
     usingDirective->set_name_qualification_length(outputNameQualificationLength);
     usingDirective->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): usingDirective->get_name_qualification_length()     = %d \n",usingDirective->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): usingDirective->get_type_elaboration_required()     = %s \n",usingDirective->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): usingDirective->get_global_qualification_required() = %s \n",usingDirective->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(usingDirective) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),usingDirective,usingDirective->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(usingDirective,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }


// DQ (7/8/2014): Adding support for name qualification of SgNamespaceDeclarations within a SgNamespaceAliasDeclarationStatement.
void
NameQualificationTraversal::setNameQualification ( SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired )
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     namespaceAliasDeclaration->set_global_qualification_required(outputGlobalQualification);
     namespaceAliasDeclaration->set_name_qualification_length(outputNameQualificationLength);
     namespaceAliasDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): namespaceAliasDeclaration->get_name_qualification_length()     = %d \n",namespaceAliasDeclaration->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): namespaceAliasDeclaration->get_type_elaboration_required()     = %s \n",namespaceAliasDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): namespaceAliasDeclaration->get_global_qualification_required() = %s \n",namespaceAliasDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(namespaceAliasDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),namespaceAliasDeclaration,namespaceAliasDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(namespaceAliasDeclaration,qualifier));
        }
       else
        {
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
        }
   }


// DQ (8/4/2012): Added support to permit global qualification to be skipped explicitly (see test2012_164.C and test2012_165.C for examples where this is important).
// void NameQualificationTraversal::setNameQualification(SgInitializedName* initializedName,SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired)
// void NameQualificationTraversal::setNameQualification(SgInitializedName* initializedName,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
void
NameQualificationTraversal::setNameQualification(SgInitializedName* initializedName,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired, bool skipGlobalQualification)
   {
  // This is used to set the name qualification on the type referenced by the SgInitializedName, and not on the SgInitializedName IR node itself.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

  // DQ (8/4/2012): In rare cases we have to eliminate qualification only if it is going to be global qualification.
  // if (skipGlobalQualification == true && qualifier == "::")
     if (skipGlobalQualification == true)
        {
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In NameQualificationTraversal::setNameQualification(SgInitializedName* initializedName): skipGlobalQualification has caused global qualification to be ignored \n");
#endif
          qualifier = "";

          outputNameQualificationLength = 0;
          outputGlobalQualification     = false;

       // Note clear if this is what we want.
          outputTypeEvaluation          = false;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     initializedName->set_global_qualification_required_for_type(outputGlobalQualification);
     initializedName->set_name_qualification_length_for_type(outputNameQualificationLength);
     initializedName->set_type_elaboration_required_for_type(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): initializedName->get_name_qualification_length_for_type()     = %d \n",initializedName->get_name_qualification_length_for_type());
     printf ("In NameQualificationTraversal::setNameQualification(): initializedName->get_type_elaboration_required_for_type()     = %s \n",initializedName->get_type_elaboration_required_for_type() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): initializedName->get_global_qualification_required_for_type() = %s \n",initializedName->get_global_qualification_required_for_type() ? "true" : "false");
#endif

     if (qualifiedNameMapForTypes.find(initializedName) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for type = %s into list at SgInitializedName IR node = %p = %s \n",qualifier.c_str(),initializedName,initializedName->get_name().str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(initializedName,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(initializedName);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
#endif
            // ROSE_ASSERT(false);
             }
        }
   }


// DQ (12/17/2013): Added support for the name qualification of the SgInitializedName object when used in the context of the preinitialization list.
void
NameQualificationTraversal::setNameQualificationOnName(SgInitializedName* initializedName,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired, bool skipGlobalQualification)
   {
  // This is used to set the name qualification on the SgInitializedName directly, and not on the type referenced by the SgInitializedName IR node.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

  // DQ (8/4/2012): In rare cases we have to eliminate qualification only if it is going to be global qualification.
  // if (skipGlobalQualification == true && qualifier == "::")
     if (skipGlobalQualification == true)
        {
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In NameQualificationTraversal::setNameQualification(SgInitializedName* initializedName): skipGlobalQualification has caused global qualification to be ignored \n");
#endif
          qualifier = "";

          outputNameQualificationLength = 0;
          outputGlobalQualification     = false;

       // Note clear if this is what we want.
          outputTypeEvaluation          = false;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     initializedName->set_global_qualification_required_for_type(outputGlobalQualification);
     initializedName->set_name_qualification_length_for_type(outputNameQualificationLength);
     initializedName->set_type_elaboration_required_for_type(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): initializedName->get_name_qualification_length_for_type()     = %d \n",initializedName->get_name_qualification_length_for_type());
     printf ("In NameQualificationTraversal::setNameQualification(): initializedName->get_type_elaboration_required_for_type()     = %s \n",initializedName->get_type_elaboration_required_for_type() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): initializedName->get_global_qualification_required_for_type() = %s \n",initializedName->get_global_qualification_required_for_type() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(initializedName) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for type = %s into list at SgInitializedName IR node = %p = %s \n",qualifier.c_str(),initializedName,initializedName->get_name().str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(initializedName,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(initializedName);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("WARNING: name in qualifiedNameMapForNames already exists and is different... \n");
#endif
            // ROSE_ASSERT(false);
             }
        }

#if 0
     printf ("Exiting as a test in the support for name qualifiction in the preinitialization list \n");
     ROSE_ASSERT(false);
#endif
   }


void
NameQualificationTraversal::setNameQualification(SgVariableDeclaration* variableDeclaration,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // This is used to set the name qualification on the associated SgInitializedName (there is only one per SgVariableDeclaration at present, but this may be changed (fixed) in the future.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     variableDeclaration->set_global_qualification_required(outputGlobalQualification);
     variableDeclaration->set_name_qualification_length(outputNameQualificationLength);
     variableDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): variableDeclaration->get_name_qualification_length()     = %d \n",variableDeclaration->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): variableDeclaration->get_type_elaboration_required()     = %s \n",variableDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): variableDeclaration->get_global_qualification_required() = %s \n",variableDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     std::map<SgNode*,std::string>::iterator it_qualifiedNameMapForNames = qualifiedNameMapForNames.find(variableDeclaration);
     if (it_qualifiedNameMapForNames == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at SgVariableDeclaration IR node = %p = %s \n",qualifier.c_str(),variableDeclaration,variableDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(variableDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(variableDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("WARNING: name in qualifiedNameMapForNames already exists and is different... \n");
#endif
            // ROSE_ASSERT(false);
             }
#if 0
          printf ("Error: name in qualifiedNameMapForNames already exists... \n");
          ROSE_ASSERT(false);
#endif
        }
   }


void
NameQualificationTraversal::setNameQualification(SgTypedefDeclaration* typedefDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     typedefDeclaration->set_global_qualification_required_for_base_type(outputGlobalQualification);
     typedefDeclaration->set_name_qualification_length_for_base_type(outputNameQualificationLength);
     typedefDeclaration->set_type_elaboration_required_for_base_type(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): typedefDeclaration->get_name_qualification_length_for_base_type()     = %d \n",typedefDeclaration->get_name_qualification_length_for_base_type());
     printf ("In NameQualificationTraversal::setNameQualification(): typedefDeclaration->get_type_elaboration_required_for_base_type()     = %s \n",typedefDeclaration->get_type_elaboration_required_for_base_type() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): typedefDeclaration->get_global_qualification_required_for_base_type() = %s \n",typedefDeclaration->get_global_qualification_required_for_base_type() ? "true" : "false");
#endif

     if (qualifiedNameMapForTypes.find(typedefDeclaration) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for type = %s into list at IR node = %p = %s \n",qualifier.c_str(),typedefDeclaration,typedefDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(typedefDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(typedefDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
            // ROSE_ASSERT(false);

               SgName testNameInMap = typedefDeclaration->get_qualified_name_prefix();
               printf ("testNameInMap = %s \n",testNameInMap.str());
#endif
             }
#if 0
          printf ("Error: name in qualifiedNameMapForTypes already exists... \n");
          ROSE_ASSERT(false);
#endif
        }
   }


void
NameQualificationTraversal::setNameQualification(SgTemplateArgument* templateArgument, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired)
   {
  // This function will generate the qualified name prefix (without the name of the declaration) and add it to 
  // the map of name qualification strings referenced via the IR node that references the SgTemplateArgument.

  // DQ (6/1/2011): Note that the name qualification could be more complex than this function presently supports. 
  // The use of derivation can permit there to be multiple legal qualified names for a single construct.  There 
  // could also be some qualified names using using type names that are private or protected and thus can only 
  // be used in restricted contexts.  This sumbject of multiple qualified names or selecting amongst them for 
  // where each may be used is not handled presently.

  // DQ (9/23/2012): Note that the template arguments of the defining declaration don't appear to be set (only for 
  // the nondefining declaration).  This was a problem for test2012_220.C.  The fix was to make sure that the 
  // unparsing of the SgClassType consistantly uses the nondefining declaration, and it's template arguments.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(declaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

  // These may not be important under the newest version of name qualification that uses the qualified 
  // name string map to IR nodes that reference the construct using the name qualification.
     templateArgument->set_global_qualification_required(outputGlobalQualification);
     templateArgument->set_name_qualification_length(outputNameQualificationLength);
     templateArgument->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): templateArgument                                      = %p \n",templateArgument);
     printf ("In NameQualificationTraversal::setNameQualification(): templateArgument->get_name_qualification_length()     = %d \n",templateArgument->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): templateArgument->get_type_elaboration_required()     = %s \n",templateArgument->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): templateArgument->get_global_qualification_required() = %s \n",templateArgument->get_global_qualification_required() ? "true" : "false");
#endif

  // DQ (9/25/2012): The code below is more complex than I would like because it has to set the name qualification 
  // on both the template arguments of the defining and nondefining declarations.

  // DQ (9/22/2012): This is the bug to fix tomorrow morning...
  // XXX:  Either we should be setting the name_qualification_length on the SgTemplateArgument for the defining declaration (as well as the (first?) nondefining declaration)
  //       or we should be sharing the SgTemplateArgument across both the non-defining and defining declarations.
  //       I think I would like to share the SgTemplateArgument (this this problem would take care of itself).

     ROSE_ASSERT(templateArgument->get_parent() != NULL);
     SgDeclarationStatement* associatedDeclaration = isSgDeclarationStatement(templateArgument->get_parent());
     ROSE_ASSERT(associatedDeclaration != NULL);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("associatedDeclaration = %p = %s \n",associatedDeclaration,associatedDeclaration->class_name().c_str());
#endif
     SgDeclarationStatement* firstNondefining_associatedDeclaration = associatedDeclaration->get_firstNondefiningDeclaration();
     SgDeclarationStatement* defining_associatedDeclaration         = associatedDeclaration->get_definingDeclaration();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("firstNondefining_associatedDeclaration = %p \n",firstNondefining_associatedDeclaration);
#endif
     SgTemplateInstantiationDecl* firstDefining_classTemplateInstantiationDeclaration = isSgTemplateInstantiationDecl(firstNondefining_associatedDeclaration);

  // SgTemplateArgument* nondefining_templateArgument = templateArgument;
     SgTemplateArgument* defining_templateArgument    = NULL;

     int nondefiningDeclaration_templateArgument_position = 0;
     int definingDeclaration_templateArgument_position    = 0;

     bool found = false;
     if (firstDefining_classTemplateInstantiationDeclaration != NULL)
        {
       // Find the index position of the current template argument.
          SgTemplateArgumentPtrList & l = firstDefining_classTemplateInstantiationDeclaration->get_templateArguments();
          for (SgTemplateArgumentPtrList::iterator i = l.begin(); i != l.end(); i++)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("--- template argument = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
               if (found == false)
                  {
                    if (*i == templateArgument)
                         found = true;
                      else
                         nondefiningDeclaration_templateArgument_position++;
                  }
             }
        }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("defining_associatedDeclaration                   = %p \n",defining_associatedDeclaration);
     printf ("nondefiningDeclaration_templateArgument_position = %d \n",nondefiningDeclaration_templateArgument_position);
     printf ("definingDeclaration_templateArgument_position    = %d \n",definingDeclaration_templateArgument_position);
#endif

     SgTemplateInstantiationDecl* defining_classTemplateInstantiationDeclaration      = isSgTemplateInstantiationDecl(defining_associatedDeclaration);
     if (defining_classTemplateInstantiationDeclaration != NULL)
        {
       // Find the associated template argument (matching position) in the template argument list of the defining declaration.
#if 0
       // This is simpler code (but it causes some sort of error in the stack).
          defining_templateArgument = defining_classTemplateInstantiationDeclaration->get_templateArguments()[nondefiningDeclaration_templateArgument_position];
#else
       // This code is better tested and works well.
          SgTemplateArgumentPtrList & l = defining_classTemplateInstantiationDeclaration->get_templateArguments();
          for (SgTemplateArgumentPtrList::iterator i = l.begin(); i != l.end(); i++)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("--- template argument = %p = %s \n",*i,(*i)->class_name().c_str());
               printf ("In loop: nondefiningDeclaration_templateArgument_position = %d \n",nondefiningDeclaration_templateArgument_position);
               printf ("In loop: definingDeclaration_templateArgument_position    = %d \n",definingDeclaration_templateArgument_position);
#endif
               if (definingDeclaration_templateArgument_position == nondefiningDeclaration_templateArgument_position)
                  {
                 // This is the template argument in the coresponding defining declaration.
                    defining_templateArgument = *i;
                  }

               definingDeclaration_templateArgument_position++;
             }
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("defining_templateArgument = %p \n",defining_templateArgument);
#endif

       // This is false when the template arguments are shared (which appears to happen sometimes, see test2004_38.C).
       // ROSE_ASSERT(defining_templateArgument != NULL);
       // if (defining_templateArgument != NULL)
          if (defining_templateArgument != NULL && defining_templateArgument != templateArgument)
             {
            // Mark the associated template argument in the defining declaration so that it can be output with qualification (see test2012_220.C).
               defining_templateArgument->set_global_qualification_required(outputGlobalQualification);
               defining_templateArgument->set_name_qualification_length(outputNameQualificationLength);
               defining_templateArgument->set_type_elaboration_required(outputTypeEvaluation);

            // DQ (9/24/2012): Make sure these are different.
               ROSE_ASSERT(defining_templateArgument != templateArgument);
             }
        }


  // Look for the template argument in the IR node map and either reset it or add it to the map.
  // The support for the template argument from the defining declaration makes this a bit more 
  // complex, but both are set to always be the same (since we will prefer to use that from the 
  // defining declaration in the unparsing).  Note that the preference for the defining declaration
  // use in the unparsing comes from supporting the corner case of type declarations nested in 
  // other declarations; e.g. "struct X { int a; } Y;" where the declaration of the type "X" is
  // nested in the declaration of the variable "Y" (there are several different forms of this).
     if (qualifiedNameMapForTypes.find(templateArgument) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name or type = %s into list at IR node = %p = %s \n",qualifier.c_str(),templateArgument,templateArgument->class_name().c_str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(templateArgument,qualifier));

       // Handle the definig declaration's template argument.
       // if (defining_templateArgument != NULL)
          if (defining_templateArgument != NULL && defining_templateArgument != templateArgument)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Insert qualified name = %s for defining_templateArgument = %p \n",qualifier.c_str(),defining_templateArgument);
#endif
               qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(defining_templateArgument,qualifier));
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                printf ("NOTE: defining_templateArgument != NULL && defining_templateArgument != templateArgument (qualified not inserted into qualifiedNameMapForTypes using defining_templateArgument = %p \n",defining_templateArgument);
#endif
             }
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(templateArgument);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
            // ROSE_ASSERT(false);
#endif

               SgName testNameInMap = templateArgument->get_qualified_name_prefix();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("testNameInMap = %s \n",testNameInMap.str());
#endif

            // DQ (9/24/2012): Check that the defining declaration's template argument is uniformally set.
            // if (defining_templateArgument != NULL)
               if (defining_templateArgument != NULL && defining_templateArgument != templateArgument)
                  {
                    ROSE_ASSERT(qualifiedNameMapForTypes.find(defining_templateArgument) != qualifiedNameMapForTypes.end());
                    std::map<SgNode*,std::string>::iterator j = qualifiedNameMapForTypes.find(defining_templateArgument);
                    ROSE_ASSERT (j != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("For defining_templateArgument = %p j->second = %s qualifier = %s \n",defining_templateArgument,j->second.c_str(),qualifier.c_str());
#endif
                 // ROSE_ASSERT(j->second != qualifier);

                    if (j->second != qualifier)
                       {
                         j->second = qualifier;
                       }

                    SgName defining_testNameInMap = defining_templateArgument->get_qualified_name_prefix();
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("defining_testNameInMap = %s \n",defining_testNameInMap.str());
#endif
                    ROSE_ASSERT(defining_testNameInMap == testNameInMap);
                  }
             }
        }
   }


// void NameQualificationTraversal::setNameQualification(SgCastExp* castExp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired)
void
NameQualificationTraversal::setNameQualification(SgExpression* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired)
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

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): exp->get_name_qualification_length()     = %d \n",exp->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): exp->get_type_elaboration_required()     = %s \n",exp->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): exp->get_global_qualification_required() = %s \n",exp->get_global_qualification_required() ? "true" : "false");
#endif
     if (qualifiedNameMapForTypes.find(exp) == qualifiedNameMapForTypes.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at IR node = %p = %s \n",qualifier.c_str(),exp,exp->class_name().c_str());
#endif
          qualifiedNameMapForTypes.insert(std::pair<SgNode*,std::string>(exp,qualifier));
        }
       else
        {
       // DQ (6/21/2011): Now we are catching this case...

       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForTypes.find(exp);
          ROSE_ASSERT (i != qualifiedNameMapForTypes.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
       // I think I can do this!
       // *i = std::pair<SgNode*,std::string>(templateArgument,qualifier);
          if (i->second != qualifier)
             {
               i->second = qualifier;

#if 1
               printf ("WARNING: name in qualifiedNameMapForTypes already exists and is different... \n");
               ROSE_ASSERT(false);
#endif

               SgName testNameInMap = exp->get_qualified_name_prefix();
               printf ("testNameInMap = %s \n",testNameInMap.str());
             }
        }
   }


void
NameQualificationTraversal::setNameQualification(SgClassDeclaration* classDeclaration, int amountOfNameQualificationRequired)
   {
  // This is used to set the name qualification on the associated SgClassDeclaration.

  // Setup call to refactored code.
     int  outputNameQualificationLength = 0;
     bool outputGlobalQualification     = false;
     bool outputTypeEvaluation          = false;

  // setNameQualificationSupport(functionDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);
     string qualifier = setNameQualificationSupport(classDeclaration->get_scope(),amountOfNameQualificationRequired, outputNameQualificationLength, outputGlobalQualification, outputTypeEvaluation);

     classDeclaration->set_global_qualification_required(outputGlobalQualification);
     classDeclaration->set_name_qualification_length(outputNameQualificationLength);
     classDeclaration->set_type_elaboration_required(outputTypeEvaluation);

  // There should be no type evaluation required for a variable reference, as I recall.
     ROSE_ASSERT(outputTypeEvaluation == false);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualification(): classDeclaration->get_name_qualification_length()     = %d \n",classDeclaration->get_name_qualification_length());
     printf ("In NameQualificationTraversal::setNameQualification(): classDeclaration->get_type_elaboration_required()     = %s \n",classDeclaration->get_type_elaboration_required() ? "true" : "false");
     printf ("In NameQualificationTraversal::setNameQualification(): classDeclaration->get_global_qualification_required() = %s \n",classDeclaration->get_global_qualification_required() ? "true" : "false");
#endif

     if (qualifiedNameMapForNames.find(classDeclaration) == qualifiedNameMapForNames.end())
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("Inserting qualifier for name = %s into list at SgClassDeclaration IR node = %p = %s \n",qualifier.c_str(),classDeclaration,classDeclaration->class_name().c_str());
#endif
          qualifiedNameMapForNames.insert(std::pair<SgNode*,std::string>(classDeclaration,qualifier));
        }
       else
        {
       // If it already existes then overwrite the existing information.
          std::map<SgNode*,std::string>::iterator i = qualifiedNameMapForNames.find(classDeclaration);
          ROSE_ASSERT (i != qualifiedNameMapForNames.end());

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          string previousQualifier = i->second.c_str();
          printf ("WARNING: replacing previousQualifier = %s with new qualifier = %s \n",previousQualifier.c_str(),qualifier.c_str());
#endif
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


string
NameQualificationTraversal::setNameQualificationSupport(SgScopeStatement* scope, const int inputNameQualificationLength, int & output_amountOfNameQualificationRequired , bool & outputGlobalQualification, bool & outputTypeEvaluation )
   {
  // This is lower level support for the different overloaded setNameQualification() functions.
  // This function builds up the qualified name as a string and then returns it to be used in 
  // either the map to names or the map to types (two different hash maps).
     string qualifierString;

     output_amountOfNameQualificationRequired = inputNameQualificationLength;
     outputGlobalQualification                = false;
     outputTypeEvaluation                     = false;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
     printf ("In NameQualificationTraversal::setNameQualificationSupport(): scope = %p = %s = %s inputNameQualificationLength = %d \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str(),inputNameQualificationLength);
#endif

     for (int i = 0; i < inputNameQualificationLength; i++)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("   --- In loop: i = %d scope = %p = %s = %s \n",i,scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
#endif
          string scope_name;

       // DQ (8/19/2014): This is used to control the generation of qualified names for un-named namespaces
       // (and maybe also other un-named language constructs).
          bool skip_over_scope = false;

       // This requirement to visit the template arguments occurs for templaed functions and templated member functions as well.
          SgTemplateInstantiationDefn* templateClassDefinition = isSgTemplateInstantiationDefn(scope);
          if (templateClassDefinition != NULL)
             {
            // Need to investigate how to generate a better quality name.
               SgTemplateInstantiationDecl* templateClassDeclaration = isSgTemplateInstantiationDecl(templateClassDefinition->get_declaration());
               ROSE_ASSERT(templateClassDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
            // This is the normalized name (without name qualification for internal template arguments)
               printf ("templateClassDeclaration->get_name()          = %s \n",templateClassDeclaration->get_name().str());

            // This is the name of the template (without and internal template arguments)
               printf ("templateClassDeclaration->get_templateName() = %s \n",templateClassDeclaration->get_templateName().str());
#endif

               SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
               ROSE_ASSERT (unparseInfoPointer != NULL);
               unparseInfoPointer->set_outputCompilerGeneratedStatements();

            // templateClassDeclaration->get_file_info()->display("SgTemplateInstantiationDecl trying to generate the qualified name: debug");

               string template_name = templateClassDeclaration->get_templateName();
               template_name += "< ";
            // printf ("START: template_name = %s \n",template_name.c_str());
               SgTemplateArgumentPtrList & templateArgumentList = templateClassDeclaration->get_templateArguments();
               SgTemplateArgumentPtrList::iterator i = templateArgumentList.begin();
               while (i != templateArgumentList.end())
                  {
                    SgTemplateArgument* templateArgument = *i;
                    ROSE_ASSERT(templateArgument != NULL);

                    string template_argument_name = globalUnparseToString(templateArgument,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("templateArgument = %p template_argument_name (globalUnparseToString()) = %s \n",templateArgument,template_argument_name.c_str());
#endif
                    template_name += template_argument_name;
                    i++;

                    if (i != templateArgumentList.end())
                         template_name += ",";
                  }

               template_name += "> ";

               scope_name = template_name;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("NAME OF SCOPE: scope name -- template_name = %s \n",template_name.c_str());
#endif

            // DQ (2/18/2013): Fixing generation of too many SgUnparse_Info object.
               delete unparseInfoPointer;
             }
            else
             {
            // scope_name = scope->class_name().c_str();
               scope_name = SageInterface::get_name(scope).c_str();

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Before test for __anonymous_ un-named scopes: scope_name = %s \n",scope_name.c_str());
#endif
            // DQ (4/6/2013): Test this scope name for that of n un-named scope so that we can avoid name qualification 
            // using an internally generated scope name.
            // Note that the pointer is from an EDG object (e.g. a_type_ptr), so we can't reproduce it in ROSE.
            // This might be something to fix if we want to be able to reproduce it.
               if (scope_name.substr(0,14) == "__anonymous_0x")
                  {
                 // DQ (4/6/2013): Added test (this would be better to added to the AST consistancy tests).
                    SgClassDefinition* classDefinition = isSgClassDefinition(scope);
                    if (classDefinition != NULL)
                       {
                         if (classDefinition->get_declaration()->get_isUnNamed() == false)
                            {
                              SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
                              printf ("Error: class should be marked as unnamed: classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
                              printf ("   --- classDeclaration name = %s \n",classDeclaration->get_name().str());
                            }
                         ROSE_ASSERT(classDefinition->get_declaration()->get_isUnNamed() == true);
                       }
#if 0
                    printf ("In NameQualificationTraversal::setNameQualificationSupport(): Detected scope_name of un-named scope: scope_name = %s (reset to empty string for name qualification) \n",scope_name.c_str());
#endif
                    scope_name = "";

                 // DQ (5/3/2013): If this case was detected then we can't use the qualified name.
                 // The test2013_145.C demonstrates this case where one part of the name qualification 
                 // is empty string (unnamed scope, specifically a union in the test code).
                    qualifierString = "";
#if 0
                    printf ("In NameQualificationTraversal::setNameQualificationSupport(): Exiting loop prematurely... \n");
#endif
                    break;
                  }
                 else
                  {
                 // DQ (4/6/2013): Added test (this would be better to add to the AST consistancy tests).
                 // ROSE_ASSERT(scope->get_isUnNamed() == false);

                    SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(scope);
                    if (namespaceDefinition != NULL)
                       {
#if 0
                         printf ("In NameQualificationTraversal::setNameQualificationSupport(): Detected a SgNamespaceDefinition: namespaceDefinition = %p \n",namespaceDefinition);
#endif

                         SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(namespaceDefinition->get_namespaceDeclaration());
                         ROSE_ASSERT(namespaceDeclaration != NULL);
                         if (namespaceDeclaration->get_isUnnamedNamespace() == true)
                            {
#if 0
                              printf ("In NameQualificationTraversal::setNameQualificationSupport(): found un-named namespace: namespaceDefinition = %p \n",namespaceDefinition);
#endif
                              skip_over_scope = true;
                            }
                       }
                      else
                       {
                      // DQ (9/7/2014): Added case for template class definitions (which we were not using and thus 
                      // it was not a problem that we didn't compute them quite right).  These were being computed
                      // as "class-name::class-name", but we need then to be computed to be:
                      // "class-name<template-parameter>::class-name<template-parameter>" instead.
                      // Other logic will have to add the template header where these are used (not clear how to 
                      // do that if we don't do it here).
                         SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(scope);
                         if (templateClassDefinition != NULL)
                            {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                              printf ("In NameQualificationTraversal::setNameQualificationSupport(): Found SgTemplateClassDefinition: templateClassDefinition = %p = %s \n",templateClassDefinition,templateClassDefinition->class_name().c_str());
#endif
                              SgTemplateClassDeclaration* templateClassDeclaration = templateClassDefinition->get_declaration();
                              ROSE_ASSERT(templateClassDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                           // This is the normalized name (without name qualification for internal template arguments)
                              printf ("In NameQualificationTraversal::setNameQualificationSupport(): templateClassDeclaration->get_name()          = %s \n",templateClassDeclaration->get_name().str());

                           // This is the name of the template (without and internal template arguments)
                              printf ("In NameQualificationTraversal::setNameQualificationSupport(): templateClassDeclaration->get_templateName() = %s \n",templateClassDeclaration->get_templateName().str());
#endif

                              SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
                              ROSE_ASSERT (unparseInfoPointer != NULL);
                              unparseInfoPointer->set_outputCompilerGeneratedStatements();

                           // templateClassDeclaration->get_file_info()->display("SgTemplateInstantiationDecl trying to generate the qualified name: debug");

                              string template_name = templateClassDeclaration->get_templateName();

                           // DQ (9/12/2014): If we have template specialization arguments then we wnat to use these instead of the template parameters (I think).
                           // See test2014_222.C for an example.
                              SgTemplateArgumentPtrList & templateSpecializationArgumentList = templateClassDeclaration->get_templateSpecializationArguments();
#if 0
                              printf ("templateSpecializationArgumentList.size() = %zu \n",templateSpecializationArgumentList.size());
                              printf ("specialization = %d \n",templateClassDeclaration->get_specialization());
#endif
                              if (templateSpecializationArgumentList.empty() == false)
                                 {
                                // DQ (9/13/2014): I have build overloaded versions of globalUnparseToString() to handle that case of SgTemplateArgumentPtrList.
                                   string template_specialization_argument_list_string = globalUnparseToString(&templateSpecializationArgumentList,unparseInfoPointer);
#if 0
                                   printf ("template_specialization_argument_list_string = %s \n",template_specialization_argument_list_string.c_str());
#endif
                                   template_name += template_specialization_argument_list_string;
                                 }
                                else
                                 {

                           // ROSE_ASSERT(templateSpecializationArgumentList.size() == 0);
                           // if (templateSpecializationArgumentList.empty() == true)

                           // DQ (9/9/2014): Modified to support empty name template parameter lists as what appear if none are present 
                           // (and this is a non-template function in a template class which we consider to be a template function 
                           // because it can be instantiated).
                              SgTemplateParameterPtrList & templateParameterList = templateClassDeclaration->get_templateParameters();
                              if (templateParameterList.empty() == false)
                                 {
#if 1
                                // DQ (9/13/2014): I have build overloaded versions of globalUnparseToString() to handle that case of SgTemplateParameterPtrList.
                                   string template_parameter_list_string = globalUnparseToString(&templateParameterList,unparseInfoPointer);
                                   template_name += template_parameter_list_string;
#else
                                   template_name += "< ";
                                // printf ("START: template_name = %s \n",template_name.c_str());
                                   SgTemplateParameterPtrList::iterator i = templateParameterList.begin();

#error "DEAD CODE!"

                                   while (i != templateParameterList.end())
                                      {
                                        SgTemplateParameter* templateParameter = *i;
                                        ROSE_ASSERT(templateParameter != NULL);

                                        string template_parameter_name = globalUnparseToString(templateParameter,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 1
                                        printf ("In NameQualificationTraversal::setNameQualificationSupport(): templateParameter = %p template_parameter_name (globalUnparseToString()) = %s \n",templateParameter,template_parameter_name.c_str());
#endif
                                        template_name += "/* Is this a pointer? */";
                                        template_name += template_parameter_name;
                                        i++;

                                        if (i != templateParameterList.end())
                                             template_name += ",";
                                      }

                                   template_name += "> ";
#endif
                                 }
                                 }

                              scope_name = template_name;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                              printf ("setNameQualificationSupport(): case of SgTemplateClassDefinition: scope_name = %s \n",scope_name.c_str());
#endif
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
                  }
             }

          SgGlobal* globalScope = isSgGlobal(scope);
          if (globalScope != NULL)
             {
            // If we have iterated beyond the number of nested scopes, then set the global 
            // qualification and reduce the name_qualification_length correspondingly by one.

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("!!!!! We have iterated beyond the number of nested scopes: setting outputGlobalQualification == true \n");
#endif
               outputGlobalQualification = true;
               output_amountOfNameQualificationRequired = inputNameQualificationLength-1;

               scope_name = "::";
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          printf ("In NameQualificationTraversal::setNameQualificationSupport(): scope_name = %s skip_over_scope = %s \n",scope_name.c_str(),skip_over_scope ? "true" : "false");
#endif

          if (skip_over_scope == false)
             {
#if 0
               printf ("In NameQualificationTraversal::setNameQualificationSupport(): outputGlobalQualification = %s \n",outputGlobalQualification ? "true" : "false");
#endif
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
                      // Nothing to do for this case of an empty string for a scope name (see test2006_121.C, using an un-named namespace).
                       }
                      else
                       {
                         qualifierString = scope_name + "::" + qualifierString;
                       }
                  }
             }
            else
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("In NameQualificationTraversal::setNameQualificationSupport(): Case of skip_over_scope == true! \n");
#endif
             }

       // We have to loop over scopes that are not named scopes!
          scope = scope->get_scope();
        }

#if 0
     printf ("In NameQualificationTraversal::setNameQualificationSupport(): After loop over name qualifiction depth: inputNameQualificationLength = %d \n",inputNameQualificationLength);
#endif

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     printf ("Leaving NameQualificationTraversal::setNameQualificationSupport(): outputGlobalQualification = %s output_amountOfNameQualificationRequired = %d qualifierString = %s \n",
          outputGlobalQualification ? "true" : "false",output_amountOfNameQualificationRequired,qualifierString.c_str());
#endif

  // DQ (6/12/2011): Make sure we have not generated a qualified name with "::::" because of an scope translated to an empty name.
     ROSE_ASSERT(qualifierString.find("::::") == string::npos);

  // DQ (6/23/2011): Never generate a qualified name from a pointer value.
  // This is a bug in the inlining support where the symbol tables are not setup just right.
     if (qualifierString.substr(0,2) == "0x")
        {
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("WARNING: Detected qualified name generated from pointer value 0x..., reset to empty string (inlining does not fixup symbol tables) \n");
#endif
          qualifierString = "";
        }
     ROSE_ASSERT(qualifierString.substr(0,2) != "0x");

     return qualifierString;
   }


string
NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(SgScopeStatement* scope, const int inputNameQualificationLength )
   {
  // DQ (9/7/2014): This function generates a string that is used with name qualification of template declarations.
  // For example:
  //      template < typename T >
  //      template < typename S >
  //      void X<T>::A<S>::foobar (int x) { int a_value; }
  // Requires the template header string: "template < typename T > template < typename S >"
  // in addition to the usual name qualification (which here is in terms of template parameters 
  // instead of template arguments (as in a template instantiation), namely "X<T>::A<S>::").  
  // This new support for template headers also requires a new map of names to template declarations.

  // This is lower level support for the different overloaded setNameQualification() functions.
  // This function builds up the qualified name as a string and then returns it to be used in 
  // either the map to names or the map to types (two different hash maps).
     string accumulated_template_header_name;

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     printf ("In NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(): scope = %p = %s = %s inputNameQualificationLength = %d \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str(),inputNameQualificationLength);
#endif

     for (int i = 0; i < inputNameQualificationLength; i++)
        {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("   --- In loop: i = %d scope = %p = %s = %s \n",i,scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
#endif
          string template_header_name;

       // DQ (9/7/2014): Added case for template class definitions (which we were not using and thus 
       // it was not a problem that we didn't compute them quite right).  These were being computed
       // as "class-name::class-name", but we need then to be computed to be:
       // "class-name<template-parameter>::class-name<template-parameter>" instead.
       // Other logic will have to add the template header where these are used (not clear how to 
       // do that if we don't do it here).
          SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(scope);
          if (templateClassDefinition != NULL)
             {
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
               printf ("Found SgTemplateClassDefinition: templateClassDefinition = %p = %s \n",templateClassDefinition,templateClassDefinition->class_name().c_str());
#endif
               SgTemplateClassDeclaration* templateClassDeclaration = templateClassDefinition->get_declaration();
               ROSE_ASSERT(templateClassDeclaration != NULL);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
            // This is the normalized name (without name qualification for internal template arguments)
               printf ("templateClassDeclaration->get_name()          = %s \n",templateClassDeclaration->get_name().str());

            // This is the name of the template (without and internal template arguments)
               printf ("templateClassDeclaration->get_templateName() = %s \n",templateClassDeclaration->get_templateName().str());
#endif
#if 0
               SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
               ROSE_ASSERT (unparseInfoPointer != NULL);
               unparseInfoPointer->set_outputCompilerGeneratedStatements();
#endif
            // templateClassDeclaration->get_file_info()->display("SgTemplateInstantiationDecl trying to generate the qualified name: debug");

               SgTemplateParameterPtrList & templateParameterList = templateClassDeclaration->get_templateParameters();
               if (templateParameterList.empty() == false)
                  {
            // string template_name = templateClassDeclaration->get_templateName();

#if 1
               string template_name = buildTemplateHeaderString(templateParameterList);
#else
               string template_name = "template < ";
            // printf ("START: template_name = %s \n",template_name.c_str());
               SgTemplateParameterPtrList::iterator i = templateParameterList.begin();
               while (i != templateParameterList.end())
                  {

#error "DEAD CODE!"

#if 0
                    printf ("In NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(): Check for the type of the template parameters (could be non-type, etc.) \n");
#endif
                    SgTemplateParameter* templateParameter = *i;
                    ROSE_ASSERT(templateParameter != NULL);

#error "DEAD CODE!"

                 // DQ (9/10/2014): We only want to output the "typename" when it is required (and exactly when it is required is not clear).
                 // Note that in C++ using "class" or "typename" is equivalent.
                 // template_name += "typename ";
                    switch(templateParameter->get_parameterType())
                       {
                      // Only type parameters should require "typename" (but not if the type was explicit).
                         case SgTemplateParameter::type_parameter:
                            {
                           // DQ (9/10/2014): Added support for case SgTemplateParameter::type_parameter.
                              SgType* type = templateParameter->get_type();
                              ROSE_ASSERT(type != NULL);
#if 0
                              printf ("In NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(): case SgTemplateParameter::type_parameter: type = %p = %s \n",type,type->class_name().c_str());
#endif
                           // If the type was explicit then don't output a redundant "typename".
                              SgTemplateType* templateType = isSgTemplateType(type);
                              if (templateType == NULL)
                                 {
#error "DEAD CODE!"

#if 0
                                // This might tell us when to use "class: instead of "typename" but since they are equivalent we can prefer to output "typename".
                                   SgClassType* classType = isSgClassType(type);
                                   if (classType != NULL)
                                      {
                                        string name = classType->get_name();
                                        curprint(name);
                                      }
                                     else
                                      {
                                        SgUnparse_Info ninfo(info);
                                        unp->u_type->unparseType(type,ninfo);
                                      }
#endif
                                 }
                                else
                                 {
#error "DEAD CODE!"

                                   template_name += "typename ";
#if 0
                                   string name = templateType->get_name();
#if 0
                                   printf ("In NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(): case SgTemplateParameter::type_parameter: type->get_name() = %s \n",name.c_str());
#endif
                                // unp->u_exprStmt->curprint(" typename ");
                                   curprint(name);
#endif
                                 }

                              break;
                            }

#error "DEAD CODE!"

                      // Non-type parameters should not require "typename".
                         case SgTemplateParameter::nontype_parameter:
                            {
                              if (templateParameter->get_expression() != NULL)
                                 {
                                // unp->u_exprStmt->unparseExpression(templateParameter->get_expression(),info);
                                 }
                                else
                                 {
                                   if (templateParameter->get_initializedName() == NULL)
                                      {
                                     // Not clear what this is?
                                      }
                                   ROSE_ASSERT(templateParameter->get_initializedName() != NULL);

#error "DEAD CODE!"
                                   SgType* type = templateParameter->get_initializedName()->get_type();
                                   ROSE_ASSERT(type != NULL);

                                // unp->u_type->outputType<SgInitializedName>(templateParameter->get_initializedName(),type,info);
                                   SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
                                   ROSE_ASSERT (unparseInfoPointer != NULL);
                                   unparseInfoPointer->set_outputCompilerGeneratedStatements();

                                   string template_parameter_name = globalUnparseToString(type,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                                   printf ("templateParameter = %p template_parameter_name (globalUnparseToString()) = %s \n",templateParameter,template_parameter_name.c_str());
#endif
                                // DQ (9/11/2014): Need to add a space.
                                   template_parameter_name += " ";

                                   template_name += template_parameter_name;
                                }
                              break;
                            }

#error "DEAD CODE!"

                         case SgTemplateParameter::template_parameter:
                            {
                              printf ("setTemplateHeaderNameQualificationSupport(): case SgTemplateParameter::template_parameter: Sorry, not implemented (ignored) \n");

                              ROSE_ASSERT(templateParameter->get_templateDeclaration() != NULL);
                              SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(templateParameter->get_templateDeclaration());
                              ROSE_ASSERT(templateDeclaration != NULL);

                              SgTemplateParameterPtrList & templateParameterList = templateDeclaration->get_templateParameters();
                              SgTemplateParameterPtrList::iterator i = templateParameterList.begin();
                              curprint(" template < ");
                              while (i != templateParameterList.end())
                                 {
                                   SgUnparse_Info newInfo(info);
                                   curprint(" typename ");
                                // unparseTemplateParameter(*i,newInfo);
                                // curprint(" SgTemplateDeclaration_name ");

                                   i++;

                                   if (i != templateParameterList.end())
                                        curprint(",");
                                 }

#error "DEAD CODE!"

                              curprint(" > ");
                              curprint(templateDeclaration->get_name());
#if 0
                              ROSE_ASSERT(false);
#endif
                              break;
                            }

                         default:
                            {
                              printf ("Error: setTemplateHeaderNameQualificationSupport(): default reached \n");
                              ROSE_ASSERT(false);
                              break;
                            }
                       }

                 // Maybe the unparser support should optionally insert the "typename" or other parameter kind support.
                    string template_parameter_name = globalUnparseToString(templateParameter,unparseInfoPointer);
#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
                    printf ("templateParameter = %p template_parameter_name (globalUnparseToString()) = %s \n",templateParameter,template_parameter_name.c_str());
#endif
                    template_name += template_parameter_name;
                    i++;

                    if (i != templateParameterList.end())
                         template_name += ",";
                  }

#error "DEAD CODE!"

               template_name += "> ";
#endif
               template_header_name = template_name;
                  }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3)
          printf ("In NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(): template_header_name = %s accumulated_template_header_name = %s \n",template_header_name.c_str(),accumulated_template_header_name.c_str());
#endif
          accumulated_template_header_name = template_header_name + accumulated_template_header_name;

       // We have to loop over scopes that are not named scopes!
          scope = scope->get_scope();
        }

     ROSE_ASSERT(accumulated_template_header_name.substr(0,2) != "0x");

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
     printf ("In NameQualificationTraversal::setTemplateHeaderNameQualificationSupport(): accumulated_template_header_name = %s \n",accumulated_template_header_name.c_str());
#endif
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return accumulated_template_header_name;
   }

 
string
NameQualificationTraversal::buildTemplateHeaderString ( SgTemplateParameterPtrList & templateParameterList )
   {
     SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
     ROSE_ASSERT (unparseInfoPointer != NULL);
     unparseInfoPointer->set_outputCompilerGeneratedStatements();

#if 0
     printf ("In NameQualificationTraversal::buildTemplateHeaderString(): templateParameterList.size() = %zu \n",templateParameterList.size());
#endif

     string template_name = "template < ";
  // printf ("START: template_name = %s \n",template_name.c_str());
     SgTemplateParameterPtrList::iterator i = templateParameterList.begin();
     while (i != templateParameterList.end())
        {
#if 0
          printf ("In NameQualificationTraversal::buildTemplateHeaderString(): Check for the type of the template parameters (could be non-type, etc.) \n");
#endif
          SgTemplateParameter* templateParameter = *i;
          ROSE_ASSERT(templateParameter != NULL);

       // Maybe the unparser support should optionally insert the "typename" or other parameter kind support.
          string template_parameter_name = globalUnparseToString(templateParameter,unparseInfoPointer);

          bool template_parameter_name_has_been_output = false;

       // DQ (9/10/2014): We only want to output the "typename" when it is required (and exactly when it is required is not clear).
       // Note that in C++ using "class" or "typename" is equivalent.
       // template_name += "typename ";
          switch(templateParameter->get_parameterType())
             {
            // Only type parameters should require "typename" (but not if the type was explicit).
               case SgTemplateParameter::type_parameter:
                  {
                 // DQ (9/10/2014): Added support for case SgTemplateParameter::type_parameter.
                    SgType* type = templateParameter->get_type();
                    ROSE_ASSERT(type != NULL);
#if 0
                    printf ("In NameQualificationTraversal::buildTemplateHeaderString(): case SgTemplateParameter::type_parameter: type = %p = %s \n",type,type->class_name().c_str());
#endif
                 // If the type was explicit then don't output a redundant "typename".
                    SgTemplateType* templateType = isSgTemplateType(type);
                    if (templateType == NULL)
                       {
                      // This might tell us when to use "class: instead of "typename" but since they are equivalent we can prefer to output "typename".
                         SgClassType* classType = isSgClassType(type);
                         if (classType != NULL)
                            {
                           // DQ (9/13/2014): See test2014_224.C for where this is required.
                              template_name += "typename ";
                           // string name = classType->get_name();
                           // curprint(name);
                            }
                           else
                            {
                           // SgUnparse_Info ninfo(info);
                           // unp->u_type->unparseType(type,ninfo);
                            }
                       }
                      else
                       {
                         template_name += "typename ";
#if 0
                         string name = templateType->get_name();
#if 0
                         printf ("In NameQualificationTraversal::buildTemplateHeaderString(): case SgTemplateParameter::type_parameter: type->get_name() = %s \n",name.c_str());
#endif
                      // unp->u_exprStmt->curprint(" typename ");
                         curprint(name);
#endif
                       }

                    break;
                  }

            // Non-type parameters should not require "typename".
               case SgTemplateParameter::nontype_parameter:
                  {
                    if (templateParameter->get_expression() != NULL)
                       {
                      // unp->u_exprStmt->unparseExpression(templateParameter->get_expression(),info);
                       }
                      else
                       {
                         if (templateParameter->get_initializedName() == NULL)
                            {
                           // Not clear what this is?
                            }
                         ROSE_ASSERT(templateParameter->get_initializedName() != NULL);

                         SgType* type = templateParameter->get_initializedName()->get_type();
                         ROSE_ASSERT(type != NULL);
#if 0
                         printf ("In NameQualificationTraversal::buildTemplateHeaderString(): case SgTemplateParameter::nontype_parameter: type = %p = %s \n",type,type->class_name().c_str());
#endif
                      // unp->u_type->outputType<SgInitializedName>(templateParameter->get_initializedName(),type,info);
                         SgUnparse_Info* unparseInfoPointer = new SgUnparse_Info();
                         ROSE_ASSERT (unparseInfoPointer != NULL);
                         unparseInfoPointer->set_outputCompilerGeneratedStatements();

                         unparseInfoPointer->set_isTypeFirstPart();

                         string template_parameter_name_1stpart = globalUnparseToString(type,unparseInfoPointer);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                         printf ("case SgTemplateParameter::nontype_parameter: templateParameter = %p template_parameter_name_1stpart (globalUnparseToString()) = %s \n",templateParameter,template_parameter_name_1stpart.c_str());
#endif
                         unparseInfoPointer->unset_isTypeFirstPart();

                      // DQ (9/11/2014): Need to add a space.
                         template_parameter_name_1stpart += " ";

                         template_name += template_parameter_name_1stpart;

                      // Put out the template parameter name.
                         template_name += template_parameter_name;

                         template_parameter_name_has_been_output = true;

                         unparseInfoPointer->set_isTypeSecondPart();

                      // Output the second part of the type.
                         string template_parameter_name_2ndpart = globalUnparseToString(type,unparseInfoPointer);

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
                         printf ("case SgTemplateParameter::nontype_parameter: templateParameter = %p template_parameter_name_2ndpart (globalUnparseToString()) = %s \n",templateParameter,template_parameter_name_2ndpart.c_str());
#endif
                         template_name += template_parameter_name_2ndpart;

                      // This is not really required since the SgUnparse_Info object will not be used further.
                         unparseInfoPointer->unset_isTypeSecondPart();
                       }
                    break;
                  }

               case SgTemplateParameter::template_parameter:
                  {
                    ROSE_ASSERT(templateParameter->get_templateDeclaration() != NULL);
                    SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(templateParameter->get_templateDeclaration());
                    ROSE_ASSERT(templateDeclaration != NULL);

                    SgTemplateParameterPtrList & templateParameterList = templateDeclaration->get_templateParameters();

                    template_name += buildTemplateHeaderString(templateParameterList);

                 // Not clear if this should always be marked as "class".
                    template_name += "class ";
#if 0
                    printf ("buildTemplateHeaderString(): case SgTemplateParameter::template_parameter: Sorry, not implemented (ignored) \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

               default:
                  {
                    printf ("Error: buildTemplateHeaderString(): default reached \n");
                    ROSE_ASSERT(false);
                    break;
                  }
             }

#if (DEBUG_NAME_QUALIFICATION_LEVEL > 3) || 0
          printf ("templateParameter = %p template_parameter_name (globalUnparseToString()) = %s \n",templateParameter,template_parameter_name.c_str());
#endif
       // template_name += template_parameter_name;
          if (template_parameter_name_has_been_output == false)
             {
               template_name += template_parameter_name;
             }
          i++;

          if (i != templateParameterList.end())
               template_name += ",";
        }

     template_name += " > ";

#if 0
     printf ("Leaving buildTemplateHeaderString(): template_name = %s \n",template_name.c_str());
#endif

     return template_name;
   }



// DQ (3/31/2014): Adding support for global qualifiction.
size_t
NameQualificationTraversal::depthOfGlobalNameQualification(SgDeclarationStatement* declaration)
   {
     ROSE_ASSERT(declaration != NULL);
#if 0
     printf ("In depthOfGlobalNameQualification(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
     size_t depthOfNameQualification = 0;

     SgScopeStatement* scope = declaration->get_scope();
     while (isSgGlobal(scope) == NULL)
        {
#if 0
          printf ("   --- scope = %p = %s \n",scope,scope->class_name().c_str());
#endif
          if (scope->isNamedScope() == true)
             {
               depthOfNameQualification++;
#if 0
               printf ("Incrementing depthOfNameQualification = %" PRIuPTR " \n",depthOfNameQualification);
#endif
             }

          scope = scope->get_scope();
#if 0
          printf ("   --- (after scope->get_scope()): scope = %p = %s \n",scope,scope->class_name().c_str());
#endif
        }

#if 0
     printf ("In depthOfGlobalNameQualification(): depthOfNameQualification = %" PRIuPTR " \n",depthOfNameQualification);
#endif

     return depthOfNameQualification;
   }


// See header file for documentation.

#include "sage3basic.h"

#include "fixupTemplateArguments.h"

// We need this so that BACKEND_COMPILERS will be known.
#include <rose_config.h>

using namespace std;

#define DEBUG_PRIVATE_TYPE 0
#define DEBUGGING_USING_RECURSIVE_DEPTH 0

// DQ (2/11/2017): This are tailored to provide less output for debugging larger problems.
#define DEBUG_VISIT_PRIVATE_TYPE 0
#define DEBUG_PRIVATE_TYPE_TRANSFORMATION 0

// bool contains_private_type (SgType* type);
// bool contains_private_type (SgTemplateArgument* templateArgument);

#if DEBUGGING_USING_RECURSIVE_DEPTH
// For debugging, keep track of the recursive depth.
static size_t global_depth = 0;
#endif

bool FixupTemplateArguments::contains_private_type (SgType* type, SgScopeStatement* targetScope)
   {
  // DQ (4/2/2018): Note that this function now addresses requirements of supporting both private and protected types.

#if DEBUGGING_USING_RECURSIVE_DEPTH
  // For debugging, keep track of the recursive depth.
     static size_t depth = 0;

     printf ("In contains_private_type(SgType*): depth = %zu \n",depth);
     ROSE_ASSERT(depth < 500);

     printf ("In contains_private_type(SgType*): global_depth = %zu \n",global_depth);
     ROSE_ASSERT(global_depth < 55);
#endif

  // Note this is the recursive function.
     bool returnValue = false;

#if DEBUG_PRIVATE_TYPE || 0
  // DQ (1/7/2016): It is a problem to do this for some files (failing about 35 files in Cxx_tests).
  // The issues appears to be in the unparsing of the template arguments of the qualified names for the types.
  // printf ("In contains_private_type(SgType*): type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
     printf ("In contains_private_type(SgType*): type = %p = %s \n",type,type->class_name().c_str());
#endif

     SgTypedefType* typedefType = isSgTypedefType(type);
     if (typedefType != NULL)
        {
       // Get the associated declaration.
          SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(typedefType->get_declaration());
          ROSE_ASSERT(typedefDeclaration != NULL);

#if 0
          bool isPrivate = typedefDeclaration->get_declarationModifier().get_accessModifier().isPrivate();
#else
       // DQ (4/2/2018): Fix this to address requirements of both private and protected class members (see Cxx11_tests/test2018_71.C).
          bool isPrivate = typedefDeclaration->get_declarationModifier().get_accessModifier().isPrivate() ||
                           typedefDeclaration->get_declarationModifier().get_accessModifier().isProtected();
#endif
#if DEBUG_PRIVATE_TYPE || 0
          printf ("typedefDeclaration isPrivate = %s \n",isPrivate ? "true" : "false");
#endif

       // First we need to know if this is a visable type.
          bool isVisable = false;
#if 0
          printf ("targetScope                     = %p = %s \n",targetScope,targetScope->class_name().c_str());
       // printf ("typedefDeclaration              = %p = %s \n",typedefDeclaration,typedefDeclaration->class_name().c_str());
          printf ("typedefDeclaration->get_scope() = %p = %s \n",typedefDeclaration->get_scope(),typedefDeclaration->get_scope()->class_name().c_str());
#endif
#if 0
          printf ("SageInterface::whereAmI(targetScope): \n");
          SageInterface::whereAmI(targetScope);
          printf ("SageInterface::whereAmI(typedefDeclaration): \n");
          SageInterface::whereAmI(typedefDeclaration);
#endif
#if 0
          printf ("\ntargetScope symbol table: \n");
          targetScope->get_symbol_table()->print("targetScope");
          printf ("end of symbol table \n");
          printf ("\ntypedefDeclaration->get_scope() symbol table: \n");
          typedefDeclaration->get_scope()->get_symbol_table()->print("typedefDeclaration->get_scope()");
          printf ("end of symbol table \n\n");
#endif
       // Test for the trivial case of matching scope (an even better test (below) is be to make sure that the targetScope is nested in the typedef scope).
          if (typedefDeclaration->get_scope() == targetScope)
             {
#if 0
               printf ("In contains_private_type(SgType*): This is a typedef type from the same scope as the target declaration \n");
#endif
            // ROSE_ASSERT(false);
            // return false;
               isVisable = true;
             }
            else
             {
            // SgTypedefSymbol*   lookupTypedefSymbolInParentScopes  (const SgName & name, SgScopeStatement *currentScope = NULL);
               SgTypedefSymbol* typedefSymbol = SageInterface::lookupTypedefSymbolInParentScopes (typedefDeclaration->get_name(),targetScope);
               if (typedefSymbol != NULL)
                  {
#if 0
                    printf ("In contains_private_type(SgType*): This is not in the current scope but can be reached from the current scope \n");
#endif
                 // ROSE_ASSERT(false);
                 // return false;
                    isVisable = true;
                  }
                 else
                  {
#if 0
                    printf ("Symbol for typedef name = %s not found in parent scopes \n",typedefDeclaration->get_name().str());
#endif
                 // ROSE_ASSERT(false);
                  }
             }
#if 0
       // Testing codes because it seems that "BitSet" shuld be visiable and so we need to debug this first.
          if (typedefDeclaration->get_name() == "BitSet")
             {
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
             }
#endif
       // If this is not private, then we are looking at what would be possbile template arguments used in a possible name qualification.
       // if (isPrivate == false)
       // if (isPrivate == false && isVisable == false)
          if (isVisable == false)
             {
               if (isPrivate == true)
                  {
                    return true;
                  }
                 else
                  {

                 // Get the scope and see if it is a template instantiation.
                    SgScopeStatement* scope = typedefDeclaration->get_scope();
#if DEBUG_PRIVATE_TYPE || 0
                    printf ("++++++++++++++ Looking in parent scope for template arguments: scope = %p = %s \n",scope,scope->class_name().c_str());
#endif
                 // Get the associated declaration.
                    switch (scope->variantT())
                       {
                         case V_SgTemplateInstantiationDefn:
                            {
                              SgTemplateInstantiationDefn* templateInstantiationDefinition = isSgTemplateInstantiationDefn(scope);
                              ROSE_ASSERT(templateInstantiationDefinition != NULL);

                              SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(templateInstantiationDefinition->get_declaration());
                              ROSE_ASSERT(templateInstantiationDeclaration != NULL);

                              SgTemplateArgumentPtrList & templateArgumentPtrList = templateInstantiationDeclaration->get_templateArguments();
                              for (SgTemplateArgumentPtrList::iterator i = templateArgumentPtrList.begin(); i != templateArgumentPtrList.end(); i++)
                                 {
#if DEBUG_PRIVATE_TYPE
                                   printf ("recursive call to contains_private_type(%p): name = %s = %s \n",*i,(*i)->class_name().c_str(),(*i)->unparseToString().c_str());
#endif
#if DEBUGGING_USING_RECURSIVE_DEPTH
                                   global_depth++;
#endif

                                   bool isPrivateType = contains_private_type(*i,targetScope);

#if DEBUGGING_USING_RECURSIVE_DEPTH
                                   global_depth--;
#endif
                                   returnValue |= isPrivateType;
                                 }
                              break;
                            }

                         default:
                            {
#if DEBUG_PRIVATE_TYPE
                              printf ("Ignoring non-SgTemplateInstantiationDefn \n");
#endif
                            }
                       }
                  }
             }
            else
             {
            // If it is visible then it need not be qualified and we don't care about if it was private.
               ROSE_ASSERT(isVisable == true);

            // returnValue = true;
               returnValue = false;
             }
        }
       else
        {
#if DEBUG_PRIVATE_TYPE || 0
          printf ("could be a wrapped type: type = %p = %s (not a template class instantiaton) \n",type,type->class_name().c_str());
          if (isSgModifierType(type) != NULL)
             {
               SgModifierType* modifierType = isSgModifierType(type);
               SgType* base_type = modifierType->get_base_type();
               printf ("--- base_type = %p = %s \n",base_type,base_type->class_name().c_str());
               SgNamedType* namedType = isSgNamedType(base_type);
               if (namedType != NULL)
                  {
                    printf ("--- base_type: name = %s \n",namedType->get_name().str());
                  }
             }
#endif
       // If this is a default SgModifierType then unwrap it.
#if 0
          SgModifierType* modifierType = isSgModifierType(type);
          if (modifierType != NULL)
             {

#error "DEAD CODE!"

            // What kind of modifier is this?
               printf ("What kind of type modifier: %s \n",modifierType->get_typeModifier().displayString().c_str());
               if (modifierType->get_typeModifier().isDefault() == true)
                  {
                 // This is a default mode modifier (acting as a wrapper type).
                    type = modifierType->get_base_type();
                  }
                 else
                  {
                    printf ("Not a default modifierType wrapper (need to handle this case) \n");
                    ROSE_ASSERT(false);
                  }
             }
#else

       // DQ (4/15/2019): With the new support for SgType::STRIP_POINTER_MEMBER_TYPE, we want to use it here.
       // Strip past pointers and other wrapping modifiers (but not the typedef types, since the whole point is to detect private instatances).
       // type = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
          type = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_POINTER_MEMBER_TYPE|SgType::STRIP_ARRAY_TYPE);
#endif

#if 0
          printf ("After stripType(): type = %p = %s \n",type,type->class_name().c_str());
          SgNamedType* namedType = isSgNamedType(type);
          if (namedType != NULL)
             {
               printf ("--- stripType: name = %s \n",namedType->get_name().str());
             }
#endif
          ROSE_ASSERT(type != NULL);


       // Make sure this is not a simple template type (else we will have infinite recursion).
       // if (type != NULL && type->isIntegerType() == false && type->isFloatType() == false)
       // if (type != NULL)
          SgTemplateType*        templateType        = isSgTemplateType(type);
          SgClassType*           classType           = isSgClassType(type);
          SgTypeVoid*            voidType            = isSgTypeVoid(type);
          SgRvalueReferenceType* rvalueReferenceType = isSgRvalueReferenceType(type);
          SgFunctionType*        functionType        = isSgFunctionType(type);
          SgDeclType*            declType            = isSgDeclType(type);

       // DQ (12/7/2016): An enum type needs to be handled since the declaration might be private (but still debugging this for now).
          SgEnumType*            enumType            = isSgEnumType(type);

       // DQ (2/12/2017): Added specific type (causing infinite recursion for CompileTests/RoseExample_tests/testRoseHeaders_03.C.
          SgTypeEllipse*         typeEllipse         = isSgTypeEllipse(type);
          SgTypeUnknown*         typeUnknown         = isSgTypeUnknown(type);
          SgTypeComplex*         typeComplex         = isSgTypeComplex(type);

       // DQ (2/16/2017): This is a case causeing many C codes to fail.
          SgTypeOfType* typeOfType = isSgTypeOfType(type);

       // TV (04/23/2018): deprecated SgTemplateType. Now using the notion of non-real declaration (and associated declaration, symbol, and reference expression)
          SgNonrealType*         typeNonreal         = isSgNonrealType(type);
          SgAutoType*            typeAuto            = isSgAutoType(type);
          SgTypeNullptr *        typeNullptr         = isSgTypeNullptr(type);

          if (type != NULL && typeNonreal  == NULL && classType   == NULL && voidType   == NULL && rvalueReferenceType == NULL && 
                              functionType == NULL && declType    == NULL && enumType   == NULL && typeEllipse         == NULL && 
                              typeUnknown  == NULL && typeComplex == NULL && typeOfType == NULL && typeAuto            == NULL && 
                              templateType == NULL && typeNullptr == NULL)
             {
#if DEBUG_PRIVATE_TYPE || 0
               printf ("found unwrapped type = %p = %s = %s (not a template class instantiaton) \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
            // if (type->isIntegerType() == false && type->isFloatType() == false)
            // if (type->isIntegerType() == false && type->isFloatType() == false)
               if (type->isIntegerType() == false && type->isFloatType() == false)
                  {
#if DEBUG_PRIVATE_TYPE || 0
                    printf ("Making a recursive call to contains_private_type(type): not integer or float type: type = %p = %s  \n",type,type->class_name().c_str());
#endif
#if DEBUGGING_USING_RECURSIVE_DEPTH
                    depth++;
                    global_depth++;
#endif
                    bool isPrivateType = contains_private_type(type,targetScope);

#if DEBUGGING_USING_RECURSIVE_DEPTH
                    depth--;
                    global_depth--;
#endif
                    returnValue = isPrivateType;
                  }
                 else
                  {
                 // This can't be a private type.
#if DEBUG_PRIVATE_TYPE
                    printf ("This is an integer or float type (of some sort): type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
                    returnValue = false;
                  }
             }
            else
             {
            // This is where we need to resolve is any types that are associated with declarations might be private (e.g. SgEnumType).

            // DQ (3/1/2019): I think we need to resolve if an unnamed type is being used here (since they can't be template arguments).

               if (classType != NULL)
                  {
                 // Check if this is associated with a template class instantiation.
#if DEBUG_PRIVATE_TYPE
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                    ROSE_ASSERT(classDeclaration != NULL);
                    printf ("--------- classDeclaration = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
#endif
                    SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(classType->get_declaration());
                    if (templateInstantiationDeclaration != NULL)
                       {
#if DEBUGGING_USING_RECURSIVE_DEPTH
                         global_depth++;
#endif
#if 0
                         printf ("Calling contains_private_type(SgTemplateArgumentPtrList): templateInstantiationDeclaration = %p = %s \n",
                              templateInstantiationDeclaration,templateInstantiationDeclaration->get_name().str());
#endif
                         returnValue = contains_private_type(templateInstantiationDeclaration->get_templateArguments(),targetScope);

#if DEBUGGING_USING_RECURSIVE_DEPTH
                         global_depth--;
#endif
#if 0
                         printf ("DONE: Calling contains_private_type(SgTemplateArgumentPtrList): templateInstantiationDeclaration = %p = %s \n",
                              templateInstantiationDeclaration,templateInstantiationDeclaration->get_name().str());
#endif
                       }
#if 0
                    printf ("DONE: --- classDeclaration = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
#endif
                  }
             }
        }

#if DEBUG_PRIVATE_TYPE || 0
     printf ("Leaving contains_private_type(SgType*): type = %p = %s = %s returnValue = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str(),returnValue ? "true" : "false");
#endif

     return returnValue;
   }


bool FixupTemplateArguments::contains_private_type ( SgTemplateArgumentPtrList & templateArgList, SgScopeStatement* targetScope )
   {
     bool returnValue = false;

  // ROSE_ASSERT(templateArgList.empty() == false);

     static std::set<SgTemplateArgumentPtrList> templateArgumentListSet;

  // DQ (2/15/2017): Make a copy of the vector and use that as a basis for identifing if the list has been previously processed.
  // SgTemplateArgumentPtrList templateArgList = templateArgList;
  // ROSE_ASSERT(templateArgList.empty() == false);

     if (templateArgumentListSet.find(templateArgList) == templateArgumentListSet.end())
        {
          templateArgumentListSet.insert(templateArgList);
        }
       else
        {
// #if DEBUGGING_USING_RECURSIVE_DEPTH
#if 0
          printf ("################# Already been or being processed: templateArgumentListSet.size() = %zu \n",templateArgumentListSet.size());
#endif

#if 0
          printf ("Leaving contains_private_type(SgTemplateArgumentPtrList): templateArgumentListSet.size() = %zu returning FALSE \n",templateArgumentListSet.size());
#endif
       // DQ (2/15/2017): Unclear if this is the correct return value, it might be that we want to record 
       // the associated value from the first time the argument list was processed and use that value.
       // Then again, if the value had already been substituted into the template argument then no further 
       // processing is required.
          return false;
        }

     int counter = 0;
     SgTemplateArgumentPtrList::const_iterator i = templateArgList.begin();
     while (returnValue == false && i != templateArgList.end())
        {
#if 0
#if DEBUGGING_USING_RECURSIVE_DEPTH
          printf ("In contains_private_type(SgTemplateArgumentPtrList): global_depth = %zu Evaluating template argument # %d = %p = %s \n",
               global_depth,counter,*i,(*i)->class_name().c_str());
#else
          printf ("In contains_private_type(SgTemplateArgumentPtrList): Evaluating template argument # %d = %p = %s \n",counter,*i,(*i)->class_name().c_str());
#endif
#endif
          returnValue |= contains_private_type(*i,targetScope);
#if 0
#if DEBUGGING_USING_RECURSIVE_DEPTH
          printf ("In contains_private_type(SgTemplateArgumentPtrList): global_depth = %zu template argument # %d = %p = %s returnValue = %s \n",
               global_depth,counter,*i,(*i)->class_name().c_str(),returnValue ? "true" : "false");
#else
          printf ("In contains_private_type(SgTemplateArgumentPtrList): template argument # %d = %p = %s returnValue = %s \n",
               counter,*i,(*i)->class_name().c_str(),returnValue ? "true" : "false");
#endif
#endif
          i++;
          counter++;
        }

#if DEBUG_PRIVATE_TYPE || 0
     printf ("Leaving contains_private_type(SgTemplateArgumentPtrList): templateArgumentListSet.size() = %zu returnValue = %s \n",templateArgumentListSet.size(),returnValue ? "true" : "false");
#endif

     return returnValue;
   }


bool
FixupTemplateArguments::contains_private_type (SgTemplateArgument* templateArgument, SgScopeStatement* targetScope)
   {
  // Note that within EDG and ROSE the template arguments may be shared so that we can support testing for equivalence.

  // static std::list<SgTemplateArgument*> templateArgumentList;
  // templateArgumentList.push_back(templateArgument);
     static std::set<SgTemplateArgument*> templateArgumentSet;

     if (templateArgumentSet.find(templateArgument) == templateArgumentSet.end())
        {
          templateArgumentSet.insert(templateArgument);
        }
       else
        {
#if DEBUGGING_USING_RECURSIVE_DEPTH
          printf ("@@@@@@@@@@@@@@@@@ Already been or being processed: templateArgument = %p = %s templateArgumentSet.size() = %zu \n",templateArgument,templateArgument->unparseToString().c_str(),templateArgumentSet.size());
#endif

#if 0
          printf ("Leaving contains_private_type(SgTemplateArgument): templateArgument = %p returning FALSE \n",templateArgument);
#endif

       // DQ (2/15/2017): Unclear if this is the correct return value, it might be that we want to record 
       // the associated value from the first time the argument list was processed and use that value.
       // Then again, if the value had already been substituted into the template argument then no further 
       // processing is required.
          return false;
        }

#if DEBUGGING_USING_RECURSIVE_DEPTH
     printf ("--- added templateArgument = %p templateArgumentSet.size() = %zu \n",templateArgument,templateArgumentSet.size());
#endif

#if DEBUGGING_USING_RECURSIVE_DEPTH
  // For debugging, keep track of the recursive depth.
     static size_t depth = 0;

     printf ("In contains_private_type(SgTemplateArgument*): depth = %zu \n",depth);
     ROSE_ASSERT(depth < 500);

     printf ("In contains_private_type(SgTemplateArgument*): global_depth = %zu \n",global_depth);
     if (global_depth >= 50)
        {
       // output the list of SgTemplateArgument in the list
          printf ("Error: too many elements in list: recursuion too deep \n");
          size_t counter = 0;
          for (std::set<SgTemplateArgument*>::iterator i = templateArgumentSet.begin(); i != templateArgumentSet.end(); i++)
             {
               printf ("--- templateArgumentSet[counter] = %p = %s \n",*i,templateArgument->unparseToString().c_str()); 
               counter++;
             }
        }
     ROSE_ASSERT(global_depth < 50);
#endif

  // Note this is the recursive function.
     bool returnValue = false;

#if DEBUG_PRIVATE_TYPE
     printf ("In contains_private_type(SgTemplateArgument*): templateArgument = %p = %s = %s \n",templateArgument,templateArgument->class_name().c_str(),templateArgument->unparseToString().c_str());
#endif

     switch (templateArgument->get_argumentType())
        {
          case SgTemplateArgument::type_argument:
             {
               ROSE_ASSERT (templateArgument->get_type() != NULL);

               SgType* templateArgumentType = templateArgument->get_type();
#if DEBUG_PRIVATE_TYPE
               printf ("templateArgumentType = %p = %s \n",templateArgumentType,templateArgumentType->class_name().c_str());
               if (isSgModifierType(templateArgumentType) != NULL)
                  {
                    SgModifierType* modifierType = isSgModifierType(templateArgumentType);
                    SgType* base_type = modifierType->get_base_type();
                    printf ("--- base_type = %p = %s \n",base_type,base_type->class_name().c_str());
                    SgNamedType* namedType = isSgNamedType(base_type);
                    if (namedType != NULL)
                       {
                         printf ("--- base_type: name = %s \n",namedType->get_name().str());
                       }
                  }
#endif
#if DEBUGGING_USING_RECURSIVE_DEPTH
               depth++;
               global_depth++;
#endif

#if 0
               printf ("In contains_private_type(SgTemplateArgument*): case SgTemplateArgument::type_argument: Calling contains_private_type(templateArgumentType) \n");
#endif
            // DQ (2/14/2017): We might want to generate a list of the private types used so
            // that we can check them against the scope of the declaration where they occur.
            // Note also that this does not address types that might appear in name qualification.
               returnValue = contains_private_type(templateArgumentType,targetScope);


            // DQ (3/2/2019): Detect any class declaration that is un-named since it can't be a tempalte argument either.
               SgNamedType* namedType = isSgNamedType(templateArgumentType);
               if (namedType != NULL)
                  {
                    SgDeclarationStatement* declarationStatement = namedType->get_declaration();
                    ROSE_ASSERT(declarationStatement != NULL);

                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
                    if (classDeclaration != NULL)
                       {
#if DEBUG_PRIVATE_TYPE
                         printf ("classDeclaration->get_isUnNamed() = %s \n",classDeclaration->get_isUnNamed() ? "true" : "false");
#endif
                         if (classDeclaration->get_isUnNamed() == true)
                            {
                              returnValue = true;
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                              SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(declarationStatement->get_definingDeclaration());
                              if (definingClassDeclaration != NULL)
                                 {
#if 0
                                   printf ("--- definingClassDeclaration = %p = %s name = %s \n",definingClassDeclaration,definingClassDeclaration->class_name().c_str(),definingClassDeclaration->get_name().str());
#endif
                                   ROSE_ASSERT(definingClassDeclaration->get_isUnNamed() == true);
                                 }
                            }

                       }
                  }

#if DEBUG_PRIVATE_TYPE
            // DQ (3/1/2019): I think we need to resolve if an unnamed type is being used here (since they can't be template arguments).
               printf ("In contains_private_type(SgTemplateArgument*): templateArgumentType = %p = %s \n",templateArgumentType,templateArgumentType->class_name().c_str());
               if (namedType != NULL)
                  {
                    printf ("--- namedType: name = %s \n",namedType->get_name().str());
                    SgDeclarationStatement* declarationStatement = namedType->get_declaration();
                    ROSE_ASSERT(declarationStatement != NULL);

                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
                    if (classDeclaration != NULL)
                       {
                         printf ("--- classDeclaration = %p = %s name = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
                         SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(declarationStatement->get_definingDeclaration());
                         if (definingClassDeclaration != NULL)
                            {
                              printf ("--- definingClassDeclaration = %p = %s name = %s \n",definingClassDeclaration,definingClassDeclaration->class_name().c_str(),definingClassDeclaration->get_name().str());
                            }
                       }
                  }
#endif

#if DEBUGGING_USING_RECURSIVE_DEPTH
               depth--;
               global_depth--;
#endif

#if DEBUG_PRIVATE_TYPE || 0
               printf ("In contains_private_type(SgTemplateArgument*): case SgTemplateArgument::type_argument: DONE calling contains_private_type(templateArgumentType): returnValue = %s \n",returnValue ? "true" : "false");
#endif
               if (returnValue == true)
                  {
                 // Find an alternative typedef to use instead.

                 // Note that this need not be a SgTypedefType (the lists are available in every SgType).
                    SgTypedefType* typedefType = isSgTypedefType(templateArgumentType);

                    if (typedefType == NULL && isSgModifierType(templateArgumentType) != NULL)
                       {
                         SgModifierType* modifierType = isSgModifierType(templateArgumentType);
                         SgType* base_type = modifierType->get_base_type();
#if 0
                         printf ("Found SgModifierType: --- base_type = %p = %s \n",base_type,base_type->class_name().c_str());
                         SgNamedType* namedType = isSgNamedType(base_type);
                         if (namedType != NULL)
                            {
                              printf ("--- base_type: name = %s \n",namedType->get_name().str());
                            }
#endif
#if DEBUG_PRIVATE_TYPE || 0
                         printf ("******* Reset the typedefType to what was found in the modifier type as a base type = %p = %s \n",base_type,base_type->class_name().c_str());
#endif
                         typedefType = isSgTypedefType(base_type);
                       }

                    SgTypedefSeq* typedef_table = NULL;
                    if (typedefType != NULL)
                       {
                      // Check if this is a type from a typedef that is in the same scope as the target declaration (variable declaration).
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(typedefType->get_declaration());
                         ROSE_ASSERT(typedefDeclaration != NULL);

                      // Test for the matching scope (an even better test would be to make sure that the targetScope is nested in the typedef scope).
                         SgScopeStatement* typedefDeclarationScope = typedefDeclaration->get_scope();
                         ROSE_ASSERT(targetScope != NULL);
                         ROSE_ASSERT(typedefDeclarationScope != NULL);
#if 0
                         printf ("targetScope             = %p = %s \n",targetScope,targetScope->class_name().c_str());
                         printf ("typedefDeclarationScope = %p = %s \n",typedefDeclarationScope,typedefDeclarationScope->class_name().c_str());
                         if (typedefDeclarationScope == targetScope)
                            {
                              printf ("In contains_private_type(SgTemplateArgument*): This is a typedef type from the same scope as the target declaration \n");
                              ROSE_ASSERT(false);
                            }
#endif
#if DEBUG_PRIVATE_TYPE || 0
                         printf ("Looking at typedef typedefType = %p = %s = %s \n",typedefType,typedefType->class_name().c_str(),typedefType->unparseToString().c_str());
#endif

                      // Consult the list of alreanative typedefs.
                         typedef_table = typedefType->get_typedefs();
                       }
                      else
                       {
                      // DQ (3/2/2019): We need to select an alternative type to support a non-un-named and non-private type as a template argument.
                         typedef_table = templateArgumentType->get_typedefs();
                       }

                 // Consult the list of alternative typedefs.
                 // SgTypedefSeq* typedef_table = typedefType->get_typedefs();
                    ROSE_ASSERT(typedef_table != NULL);

                    SgTypePtrList & typedefList = typedef_table->get_typedefs();

                    bool foundNonPrivateTypeAlias = false;
                    SgType* suitableTypeAlias = NULL;

                    int counter = 0;
                    SgTypePtrList::iterator i = typedefList.begin();
                    while (foundNonPrivateTypeAlias == false && i != typedefList.end())
                      {
                        ROSE_ASSERT(*i != NULL);
#if DEBUG_PRIVATE_TYPE || 0
                        printf ("Looking for suitable type alias (#%d): *i = %p = %s = %s \n",counter,*i,(*i)->class_name().c_str(),(*i)->unparseToString().c_str());
#endif
#if DEBUGGING_USING_RECURSIVE_DEPTH
                        global_depth++;
#endif
                        bool isPrivateType = contains_private_type(*i,targetScope);

#if DEBUGGING_USING_RECURSIVE_DEPTH
                        global_depth--;
#endif
                        if (isPrivateType == false)
                           {
                             suitableTypeAlias = *i;
                             foundNonPrivateTypeAlias = true;
                           }

                     // foundNonPrivateTypeAlias = !isPrivateType;

                        i++;
                        counter++;
                      }
#if 0
                    printf ("foundNonPrivateTypeAlias = %s \n",foundNonPrivateTypeAlias ? "true" : "false");
#endif
                    if (foundNonPrivateTypeAlias == true)
                       {
                         ROSE_ASSERT(suitableTypeAlias != NULL);
#if 0
#if DEBUG_PRIVATE_TYPE_TRANSFORMATION || 0
                         printf ("targetScope             = %p = %s \n",targetScope,targetScope->class_name().c_str());
                         printf ("typedefDeclarationScope = %p = %s \n",typedefDeclarationScope,typedefDeclarationScope->class_name().c_str());
                         targetScope->get_file_info()->display("targetScope: debug");
                         typedefDeclarationScope->get_file_info()->display("typedefDeclarationScope: debug");

                         printf ("Found private type to be replaced: typedefType       = %p = %s = %s \n",typedefType,typedefType->class_name().c_str(),typedefType->unparseToString().c_str());
                         printf ("Found suitable type alias:         suitableTypeAlias = %p = %s = %s \n",suitableTypeAlias,suitableTypeAlias->class_name().c_str(),suitableTypeAlias->unparseToString().c_str());
#endif
#endif
#if 0
                         printf ("SageInterface::whereAmI(targetScope): \n");
                         SageInterface::whereAmI(targetScope);
                         printf ("SageInterface::whereAmI(typedefDeclaration): \n");
                         SageInterface::whereAmI(typedefDeclaration);
#endif
#if 0
                         printf ("Selecting alternative type to use for unparsing: \n");
                         printf ("--- were going to use: %s \n",templateArgument->unparseToString().c_str());
                         printf ("--- selecing instead : %s \n",suitableTypeAlias->unparseToString().c_str());
#endif

                      // TV (10/05/2018): (ROSE-1431) Traverse the chain of all associated template arguments (coming from the same EDG template argument)
                         SgTemplateArgument * templateArgument_it = templateArgument;
                         while (templateArgument_it->get_previous_instance() != NULL) 
                            {
                              templateArgument_it = templateArgument_it->get_previous_instance();
                            }
                         ROSE_ASSERT(templateArgument_it != NULL && templateArgument_it->get_previous_instance() == NULL);
                         do {
#if 0
                              printf ("  Update templateArgument = %p\n", templateArgument);
#endif
                              templateArgument_it->set_unparsable_type_alias(suitableTypeAlias);

                           // DQ (1/9/2017): Also set the return result from get_type() so that the name qualification will be handled correctly.
                              templateArgument_it->set_type(suitableTypeAlias);

                              templateArgument_it = templateArgument_it->get_next_instance();
                            }
                         while (templateArgument_it != NULL);

                         ROSE_ASSERT(templateArgument_it == NULL);

// #if DEBUG_PRIVATE_TYPE_TRANSFORMATION
#if 0
                         string typedefType_typeName       = generate_string_name (typedefType,NULL);
                         printf ("typedefType_typeName size = %zu \n",typedefType_typeName.length());
                         printf ("typedefType_typeName       = %s \n",typedefType_typeName.c_str());

                         string suitableTypeAlias_typeName = generate_string_name (suitableTypeAlias,NULL);
                         printf ("suitableTypeAlias_typeName size = %zu \n",suitableTypeAlias_typeName.length());
                         printf ("suitableTypeAlias_typeName size = %s \n",suitableTypeAlias_typeName.c_str());

                         ROSE_ASSERT(suitableTypeAlias_typeName.length() < typedefType_typeName.length() * 100);
                         ROSE_ASSERT(suitableTypeAlias_typeName.length() < 40000);
#endif
                       }

#if 0
                 // DQ (3/1/2019): We now handle the case where the typedef is NULL.
                      else
                       {
#if DEBUG_PRIVATE_TYPE
                         printf ("Alternative types not searched for in nontypedef types (not implemented) \n");
#endif
#if 0
                         printf ("####### Alternative types not searched: templateArgumentType = %p = %s \n",templateArgumentType,templateArgumentType->class_name().c_str());
                         if (isSgModifierType(templateArgumentType) != NULL)
                            {
                              SgModifierType* modifierType = isSgModifierType(templateArgumentType);
                              SgType* base_type = modifierType->get_base_type();
                              printf ("--- base_type = %p = %s \n",base_type,base_type->class_name().c_str());
                              SgNamedType* namedType = isSgNamedType(base_type);
                              if (namedType != NULL)
                                 {
                                   printf ("--- base_type: name = %s \n",namedType->get_name().str());
                                 }
                            }
#endif
                       }
#endif
                  }

               break;
             }

          default:
             {
#if DEBUG_PRIVATE_TYPE
               printf ("Ignoring non-type template arguments \n");
#endif
             }
        }

#if DEBUG_PRIVATE_TYPE
     printf ("Leaving contains_private_type(SgTemplateArgument*): templateArgument = %p = %s = %s \n",templateArgument,templateArgument->class_name().c_str(),templateArgument->unparseToString().c_str());
#endif

  // templateArgumentList.pop_back();
     templateArgumentSet.erase(templateArgument);

#if DEBUGGING_USING_RECURSIVE_DEPTH
     printf ("--- pop templateArgument = %p templateArgumentSet.size() = %zu \n",templateArgument,templateArgumentSet.size());
#endif

#if 0
     printf ("Leaving contains_private_type(SgTemplateArgument): templateArgument = %p returnValue = %s \n",templateArgument,returnValue ? "true" : "false");
#endif

     return returnValue;
   }


#if 0
string
FixupTemplateArguments::generate_string_name (SgType* type, SgNode* nodeReferenceToType)
   {
  // DQ (2/11/2017): This function has been added to support debugging.  It is a current problem that
  // the substitution of private types in template arguments with type aliases not containing private 
  // types, is that the typename can be generated to be extremely large (e.g 803+ million characters long).

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

     return typeNameString;
   }
#endif

// void FixupTemplateArguments::visit ( SgNode* node )
void
FixupTemplateArguments::processTemplateArgument ( SgTemplateArgument* templateArgument, SgScopeStatement* targetScope )
   {
  // ROSE_ASSERT(node != NULL);
     ROSE_ASSERT(templateArgument != NULL);

#if DEBUGGING_USING_RECURSIVE_DEPTH
  // For debugging, keep track of the recursive depth.
     printf ("In FixupTemplateArguments::visit: global_depth = %zu \n",global_depth);
     ROSE_ASSERT(global_depth < 50);
#endif

  // DQ (2/11/2017): Change this traversal to not use memory pools.
  // SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
     ROSE_ASSERT(templateArgument != NULL);

#if DEBUGGING_USING_RECURSIVE_DEPTH
     global_depth++;
#endif

     bool result = contains_private_type(templateArgument,targetScope);

#if DEBUGGING_USING_RECURSIVE_DEPTH
     global_depth--;
#endif

     if (result == true)
        {
       // This type will be a problem to unparse, because it contains parts that are private (or protected).
#if DEBUG_VISIT_PRIVATE_TYPE
          printf ("\n\nWARNING: This template parameter can NOT be unparsed (contains references to private types): templateArgument = %p = %s \n",templateArgument,templateArgument->unparseToString().c_str());
          SgNode* parent = templateArgument->get_parent();
          SgDeclarationStatement* parentDeclaration = isSgDeclarationStatement(parent);
          if (parentDeclaration != NULL)
             {
               if (parentDeclaration->get_file_info() != NULL)
                  {
#if 0
                    parentDeclaration->get_file_info()->display("location of parent non-defining declaration using templateArgument: debug");
                    SgDeclarationStatement* parentDefiningDeclaration = isSgDeclarationStatement(parentDeclaration->get_definingDeclaration());
                    if (parentDefiningDeclaration != NULL)
                       {
                         parentDefiningDeclaration->get_file_info()->display("location of parent defining declaration using templateArgument: debug");
                       }
#endif
                  }
             }
            else
             {
               if (parent->get_file_info() != NULL)
                  {
#if 0
                    parent->get_file_info()->display("location of parent node using templateArgument: debug");
#endif
                  }
             }
#endif
        }
       else
        {
       // This type is fine to unparse
#if DEBUG_PRIVATE_TYPE
          printf ("Template parameter CAN be unparsed (no private types) \n\n");
#endif
        }
   }


void
FixupTemplateArguments::visit ( SgNode* node )
   {
     ROSE_ASSERT(node != NULL);

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
     if (variableDeclaration != NULL)
        {
       // Check the type of the variable declaration, and any template arguments if it is a template type with template arguments.
       // SgType* type = variableDeclaration->get_type();
       // ROSE_ASSERT(type != NULL);
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);
          ROSE_ASSERT(initializedName != NULL);
          SgType* type = initializedName->get_type();
          ROSE_ASSERT(type != NULL);
#if 0
          printf ("\n**************************************************************************** \n");
          printf ("FixupTemplateArguments::visit(): variableDeclaration = %p = %s initializedName = %s \n",variableDeclaration,variableDeclaration->class_name().c_str(),initializedName->get_name().str());
          printf ("   --- type = %p = %s \n",type,type->class_name().c_str());
          string filename = initializedName->get_file_info()->get_filename();
          int linenumber  = initializedName->get_file_info()->get_line();
          printf ("   --- filename = %s line = %d \n",filename.c_str(),linenumber);
#endif
          SgScopeStatement* targetScope = variableDeclaration->get_scope();
          ROSE_ASSERT(targetScope != NULL);
#if 0
          printf ("In FixupTemplateArguments::visit(): targetScope for variableDeclaration = %p = %s \n",targetScope,targetScope->class_name().c_str());
#endif

       // DQ (2/16/2017): Don't process code in template instantiations.
          SgTemplateInstantiationDefn*               templateInstantiationDefn              = isSgTemplateInstantiationDefn(targetScope);
          SgFunctionDeclaration*                     functionDeclaration                    = TransformationSupport::getFunctionDeclaration(targetScope);
          SgTemplateInstantiationFunctionDecl*       templateInstantiationFunctionDec       = isSgTemplateInstantiationFunctionDecl(functionDeclaration);
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDec = isSgTemplateInstantiationMemberFunctionDecl(functionDeclaration);
       // if (templateInstantiationDefn == NULL)
          if (templateInstantiationDefn == NULL && templateInstantiationFunctionDec == NULL && templateInstantiationMemberFunctionDec == NULL)
             {
#if 1
            // DQ (2/15/2017): When this is run, we cause transformations that cause ROSE to have an infinte loop.
            // Since this is a second (redundant) invocaion, we likely should just not run this.  But it is not 
            // clear if this truely fixes the problem that I am seeing.
               bool result = contains_private_type(type,targetScope);

            // DQ (3/25/2017): Added a trivial use to eliminate Clang warning about the return value not being used.
            // But it might be that we should not run the function, however this is a complex subject from last month 
            // that I don't wish to revisit at the moment while being focused om eliminating warnings from Clang.
               ROSE_ASSERT(result == true || result == false);
#endif
#if 0
               if (result == true)
                  {
                    printf ("******** contains private type: variableDeclaration = %p = %s initializedName = %s \n",variableDeclaration,variableDeclaration->class_name().c_str(),initializedName->get_name().str());
                  }
#endif
             }
#if 0
          printf ("DONE: FixupTemplateArguments::visit(): variableDeclaration = %p = %s initializedName = %s \n",variableDeclaration,variableDeclaration->class_name().c_str(),initializedName->get_name().str());
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
   }

// void fixupTemplateArguments()
void fixupTemplateArguments( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance fixupTemplateArguments_timer ("Add reference to non-private template arguments (for unparsing):");

#if 0
     printf ("Inside of fixupTemplateArguments() \n");
#endif

#if 1
  // DQ (2/10/2017): The default should be for this to be on, but it needs to be fixed to allow some of the later test codes to also pass.
  // DQ (2/9/2017): This is causing type names to be too long (over 400K), so it causes tests/CompileTests/RoseExample_tests/testRoseHeaders_03.C
  // to fails (along with several other of the ROSE specific test codes in that directory (tests 3,4,5,and 6)).

  // DQ (1/15/2017): Since this is a fix for GNU 4.9 and greater backend compilers, and Intel and Clang compilers, 
  // we only want to test fixing it there initially. Later we can apply the fix more uniformally.
  // DQ (11/27/2016): We only want to support calling this fixup where I am testing it with the GNU 6.x compilers.
  // #if (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER >= 6)
#if defined(BACKEND_CXX_IS_GNU_COMPILER) && ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER == 9) || BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER >= 5 )
     FixupTemplateArguments t;
  // SgTemplateArgument::traverseMemoryPoolNodes(t);
  // t.traverse(node,preorder);
     t.traverse(node,preorder);
#else
// DQ (1/15/2017): And apply this fix for all versions of Intel and Clang backend compilers as well.
   #if defined(BACKEND_CXX_IS_INTEL_COMPILER) || defined (BACKEND_CXX_IS_CLANG_COMPILER)
     FixupTemplateArguments t;
  // SgTemplateArgument::traverseMemoryPoolNodes(t);
     t.traverse(node,preorder);
   #endif
#endif

#endif

#if 0
// #if DEBUG_PRIVATE_TYPE
     printf ("DONE: Inside of fixupTemplateArguments() \n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

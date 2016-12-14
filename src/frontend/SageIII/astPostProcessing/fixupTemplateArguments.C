// See header file for documentation.

#include "sage3basic.h"

#include "fixupTemplateArguments.h"

// We need this so that BACKEND_COMPILERS will be known.
#include <rose_config.h>

using namespace std;

bool contains_private_type (SgType* type);
bool contains_private_type (SgTemplateArgument* templateArgument);

bool contains_private_type (SgType* type)
   {
  // Note this is the recursive function.
     bool returnValue = false;

#if 1
     printf ("In contains_private_type(): type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif

     SgTypedefType* typedefType = isSgTypedefType(type);
     if (typedefType != NULL)
        {
       // Get the associated declaration.
          SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(typedefType->get_declaration());
          ROSE_ASSERT(typedefDeclaration != NULL);

          bool isPrivate = typedefDeclaration->get_declarationModifier().get_accessModifier().isPrivate();
#if 1
          printf ("typedefDeclaration isPrivate = %s \n",isPrivate ? "true" : "false");
#endif
          if (isPrivate == false)
             {
            // Get the scope and see if it is a template instantiation.
               SgScopeStatement* scope = typedefDeclaration->get_scope();
#if 1
               printf ("scope = %p = %s \n",scope,scope->class_name().c_str());
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

                      // SgTemplateArgumentList* templateArgumentList = templateInstantiationDeclaration->get_templateArguments();
                      // ROSE_ASSERT(templateArgumentList != NULL);
                         SgTemplateArgumentPtrList & templateArgumentPtrList = templateInstantiationDeclaration->get_templateArguments();

                         for (SgTemplateArgumentPtrList::iterator i = templateArgumentPtrList.begin(); i != templateArgumentPtrList.end(); i++)
                            {
#if 1
                              printf ("recursive call to contains_private_type(%p): name = %s = %s \n",*i,(*i)->class_name().c_str(),(*i)->unparseToString().c_str());
#endif
                              bool isPrivateType = contains_private_type(*i);

                              returnValue |= isPrivateType;
                            }

                         break;
                       }

                    default:
                       {
#if 1
                         printf ("Ignoring non-SgTemplateInstantiationDefn \n");
#endif
                       }
                  }
             }
            else
             {
               returnValue = true;
             }
        }
       else
        {
#if 1
          printf ("could be a wrapped type: type = %p = %s (not a template class instantiaton) \n",type,type->class_name().c_str());
#endif
       // If this is a default SgModifierType then unwrap it.
#if 0
          SgModifierType* modifierType = isSgModifierType(type);
          if (modifierType != NULL)
             {
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
       // Strip past pointers and other wrapping modifiers.
       // type = type->strip_type(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE|SgType::STRIP_TYPEDEF_TYPE);
          type = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
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

          if (type != NULL && templateType == NULL && classType == NULL && voidType == NULL && rvalueReferenceType == NULL && functionType == NULL && declType == NULL && enumType == NULL)
             {
#if 1
               printf ("found unwrapped type = %p = %s = %s (not a template class instantiaton) \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
            // if (type->isIntegerType() == false && type->isFloatType() == false)
               if (type->isIntegerType() == false && type->isFloatType() == false)
                  {
#if 1
                    printf ("Making call to contains_private_type(type) \n");
#endif
                    bool isPrivateType = contains_private_type(type);
                    returnValue = isPrivateType;
                  }
                 else
                  {
                 // This can't be a private type.
#if 1
                    printf ("This is an integer or float type (of some sort): type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
                    returnValue = false;
                  }
             }
            else
             {
            // This is where we need to resolve is any types that are associated with declarations might be private (e.g. SgEnumType).
             }
        }

#if 1
     printf ("Leaving contains_private_type(): type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif

     return returnValue;
   }


bool contains_private_type (SgTemplateArgument* templateArgument)
   {
  // Note this is the recursive function.
     bool returnValue = false;

#if 1
     printf ("In contains_private_type(): templateArgument = %p = %s = %s \n",templateArgument,templateArgument->class_name().c_str(),templateArgument->unparseToString().c_str());
#endif

     switch (templateArgument->get_argumentType())
        {
          case SgTemplateArgument::type_argument:
             {
               ROSE_ASSERT (templateArgument->get_type() != NULL);

               SgType* templateArgumentType = templateArgument->get_type();
#if 1
               printf ("templateArgumentType = %p = %s \n",templateArgumentType,templateArgumentType->class_name().c_str());
#endif
               returnValue = contains_private_type(templateArgumentType);

               if (returnValue == true)
                  {
                 // Find an alternative typedef to use instead.

                 // Note that this need not be a SgTypedefType (the lists are available in every SgType).
                    SgTypedefType* typedefType = isSgTypedefType(templateArgumentType);
                    if (typedefType != NULL)
                       {
                      // Consult the list of alreanative typedefs.
                         SgTypedefSeq* typedef_table = typedefType->get_typedefs();
                         ROSE_ASSERT(typedef_table != NULL);

                         SgTypePtrList & typedefList = typedef_table->get_typedefs();

                         bool foundNonPrivateTypeAlias = false;
                         SgType* suitableTypeAlias = NULL;

                         SgTypePtrList::iterator i = typedefList.begin();
                         while (foundNonPrivateTypeAlias == false && i != typedefList.end())
                           {
                             ROSE_ASSERT(*i != NULL);
#if 1
                             printf ("Looking for suitable type alias: *i = %p = %s = %s \n",*i,(*i)->class_name().c_str(),(*i)->unparseToString().c_str());
#endif
                             bool isPrivateType = contains_private_type(*i);

                             if (isPrivateType == false)
                                {
                                  suitableTypeAlias = *i;
                                  foundNonPrivateTypeAlias = true;
                                }

                          // foundNonPrivateTypeAlias = !isPrivateType;

                             i++;
                           }

                         if (foundNonPrivateTypeAlias == true)
                            {
                              ROSE_ASSERT(suitableTypeAlias != NULL);
#if 1
                              printf ("Found suitable type alias: suitableTypeAlias = %p = %s = %s \n",suitableTypeAlias,suitableTypeAlias->class_name().c_str(),suitableTypeAlias->unparseToString().c_str());
#endif
                              templateArgument->set_uparsable_type_alias(suitableTypeAlias);
                            }
                       }
                      else
                       {
                         printf ("Alternative types not searched for in nontypedef types (not implemented) \n");
                       }
                  }

               break;
             }

          default:
             {
#if 1
               printf ("Ignoring non-type template arguments \n");
#endif
             }
        }

#if 1
     printf ("Leaving contains_private_type(): templateArgument = %p = %s = %s \n",templateArgument,templateArgument->class_name().c_str(),templateArgument->unparseToString().c_str());
#endif

     return returnValue;
   }


void
FixupTemplateArguments::visit ( SgNode* node )
   {
     ROSE_ASSERT(node != NULL);

     SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
     ROSE_ASSERT(templateArgument != NULL);

     bool result = contains_private_type(templateArgument);

     if (result == true)
        {
       // This type will be a problem to unparse, because it contains parts that are private (or protected).
          printf ("WARNING: This template parameter can NOT be unparsed (contains references to private types) \n\n");
        }
       else
        {
       // This type is fine to unparse
          printf ("Template parameter CAN be unparsed (no private types) \n\n");
        }
   }


void fixupTemplateArguments()
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance fixupTemplateArguments_timer ("Add reference to non-private template arguments (for unparsing):");

#if 0
  // DQ (11/27/2016): We only want to support calling this fixup where I am testing it with the GNU 6.x compilers.
#if (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER >= 6)
     printf ("Inside of fixupTemplateArguments() \n");

     FixupTemplateArguments t;

     SgTemplateArgument::traverseMemoryPoolNodes(t);

     printf ("DONE: Inside of fixupTemplateArguments() \n");
#endif
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

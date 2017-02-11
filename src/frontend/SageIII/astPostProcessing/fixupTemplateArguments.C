// See header file for documentation.

#include "sage3basic.h"

#include "fixupTemplateArguments.h"

// We need this so that BACKEND_COMPILERS will be known.
#include <rose_config.h>

using namespace std;

#define DEBUG_PRIVATE_TYPE 0
#define DEBUGGING_USING_RECURSIVE_DEPTH 0

bool contains_private_type (SgType* type);
bool contains_private_type (SgTemplateArgument* templateArgument);

#if DEBUGGING_USING_RECURSIVE_DEPTH
// For debugging, keep track of the recursive depth.
static size_t global_depth = 0;
#endif

bool contains_private_type (SgType* type)
   {
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

#if DEBUG_PRIVATE_TYPE
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

          bool isPrivate = typedefDeclaration->get_declarationModifier().get_accessModifier().isPrivate();
#if DEBUG_PRIVATE_TYPE
          printf ("typedefDeclaration isPrivate = %s \n",isPrivate ? "true" : "false");
#endif
          if (isPrivate == false)
             {
            // Get the scope and see if it is a template instantiation.
               SgScopeStatement* scope = typedefDeclaration->get_scope();
#if DEBUG_PRIVATE_TYPE
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
#if DEBUG_PRIVATE_TYPE
                              printf ("recursive call to contains_private_type(%p): name = %s = %s \n",*i,(*i)->class_name().c_str(),(*i)->unparseToString().c_str());
#endif
#if DEBUGGING_USING_RECURSIVE_DEPTH
                              global_depth++;
#endif
                              bool isPrivateType = contains_private_type(*i);

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
            else
             {
               returnValue = true;
             }
        }
       else
        {
#if DEBUG_PRIVATE_TYPE
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
#if DEBUG_PRIVATE_TYPE
               printf ("found unwrapped type = %p = %s = %s (not a template class instantiaton) \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
            // if (type->isIntegerType() == false && type->isFloatType() == false)
               if (type->isIntegerType() == false && type->isFloatType() == false)
                  {
#if DEBUG_PRIVATE_TYPE
                    printf ("Making call to contains_private_type(type) \n");
#endif
#if DEBUGGING_USING_RECURSIVE_DEPTH
                    depth++;
                    global_depth++;
#endif
                    bool isPrivateType = contains_private_type(type);

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
             }
        }

#if DEBUG_PRIVATE_TYPE
     printf ("Leaving contains_private_type(SgType*): type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif

     return returnValue;
   }


bool contains_private_type (SgTemplateArgument* templateArgument)
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
          printf ("Already being processed: templateArgument = %p = %s templateArgumentSet.size() = %zu \n",templateArgument,templateArgument->unparseToString().c_str(),templateArgumentSet.size());
#endif
          return false;
        }

#if DEBUGGING_USING_RECURSIVE_DEPTH
     printf ("--- added templateArgument = %p templateArgumentList.size() = %zu \n",templateArgument,templateArgumentSet.size());
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
#endif
#if DEBUGGING_USING_RECURSIVE_DEPTH
               depth++;
               global_depth++;
#endif

               returnValue = contains_private_type(templateArgumentType);

#if DEBUGGING_USING_RECURSIVE_DEPTH
               depth--;
               global_depth--;
#endif

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
#if DEBUG_PRIVATE_TYPE
                             printf ("Looking for suitable type alias: *i = %p = %s = %s \n",*i,(*i)->class_name().c_str(),(*i)->unparseToString().c_str());
#endif
#if DEBUGGING_USING_RECURSIVE_DEPTH
                             global_depth++;
#endif
                             bool isPrivateType = contains_private_type(*i);

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
                           }

                         if (foundNonPrivateTypeAlias == true)
                            {
                              ROSE_ASSERT(suitableTypeAlias != NULL);
#if DEBUG_PRIVATE_TYPE
                              printf ("Found suitable type alias: suitableTypeAlias = %p = %s = %s \n",suitableTypeAlias,suitableTypeAlias->class_name().c_str(),suitableTypeAlias->unparseToString().c_str());
#endif
                              templateArgument->set_unparsable_type_alias(suitableTypeAlias);

                           // DQ (1/9/2017): Also set the return result from get_type() so that the name qualification will be handled correctly.
                              templateArgument->set_type(suitableTypeAlias);
                            }
                       }
                      else
                       {
#if DEBUG_PRIVATE_TYPE
                         printf ("Alternative types not searched for in nontypedef types (not implemented) \n");
#endif
                       }
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

     return returnValue;
   }


void
FixupTemplateArguments::visit ( SgNode* node )
   {
     ROSE_ASSERT(node != NULL);

#if DEBUGGING_USING_RECURSIVE_DEPTH
  // For debugging, keep track of the recursive depth.
     printf ("In FixupTemplateArguments::visit: global_depth = %zu \n",global_depth);
     ROSE_ASSERT(global_depth < 50);
#endif

     SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
     ROSE_ASSERT(templateArgument != NULL);

#if DEBUGGING_USING_RECURSIVE_DEPTH
     global_depth++;
#endif

     bool result = contains_private_type(templateArgument);

#if DEBUGGING_USING_RECURSIVE_DEPTH
     global_depth--;
#endif

     if (result == true)
        {
       // This type will be a problem to unparse, because it contains parts that are private (or protected).
#if DEBUG_PRIVATE_TYPE
          printf ("\n\nWARNING: This template parameter can NOT be unparsed (contains references to private types): templateArgument = %p = %s \n",templateArgument,templateArgument->unparseToString().c_str());
          SgNode* parent = templateArgument->get_parent();
          SgDeclarationStatement* parentDeclaration = isSgDeclarationStatement(parent);
          if (parentDeclaration != NULL)
             {
               if (parentDeclaration->get_file_info() != NULL)
                  {
                    parentDeclaration->get_file_info()->display("location of parent non-defining declaration using templateArgument: debug");
                    SgDeclarationStatement* parentDefiningDeclaration = isSgDeclarationStatement(parentDeclaration->get_definingDeclaration());
                    if (parentDefiningDeclaration != NULL)
                       {
                         parentDefiningDeclaration->get_file_info()->display("location of parent defining declaration using templateArgument: debug");
                       }
                  }
             }
            else
             {
               if (parent->get_file_info() != NULL)
                  {
                    parent->get_file_info()->display("location of parent node using templateArgument: debug");
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


void fixupTemplateArguments()
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
     SgTemplateArgument::traverseMemoryPoolNodes(t);
#else
// DQ (1/15/2017): And apply this fix for all versions of Intel and Clang backend compilers as well.
   #if defined(BACKEND_CXX_IS_INTEL_COMPILER) || defined (BACKEND_CXX_IS_CLANG_COMPILER)
     FixupTemplateArguments t;
     SgTemplateArgument::traverseMemoryPoolNodes(t);
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

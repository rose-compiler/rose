
#include "rose.h"
// #include "sharedMemoryDSL.h"

#include "sharedMemoryTypeTraversal.h"


TypeTraversalInheritedAttribute::TypeTraversalInheritedAttribute()
   {
  // It appears that I might not need this (at least for the simple cases).
   }

TypeTraversalSynthesizedAttribute::TypeTraversalSynthesizedAttribute()
   {
  // It appears that I might not need this (at least for the simple cases).
   }

#define DEBUG_TYPE_TRAVERSAL 0

void
TypeTraversal::transformType(SgType* type)
   {
  // Since only the base_types of pointers are shared, we can take as input the pointer type and just change it internally.
  // The reference to the type from non-type IR nodes need not be modified.

  // DQ (6/11/2014): This function could be made more recursive to better address more general types.

     ROSE_ASSERT(type != NULL);

#if DEBUG_TYPE_TRAVERSAL
     printf ("Inside of TypeTraversal::transformType(): type = %p = %s \n",type,type->class_name().c_str());
#endif

  // How complex can be expect the type system to be (do we required a nested type traversal).
     SgPointerType* pointerType = isSgPointerType(type);
     if (pointerType != NULL)
        {
       // Check if the base type is marked as shared.
          SgModifierType* mod_type = isSgModifierType(pointerType->get_base_type());
          if (mod_type != NULL)
             {
#if DEBUG_TYPE_TRAVERSAL
               printf ("(mod_type != NULL): mod_type->get_typeModifier().displayString() = %s \n",mod_type->get_typeModifier().displayString().c_str());
#endif
               if (mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
                  {
#if DEBUG_TYPE_TRAVERSAL
                    printf ("TypeTraversal::transformType(): (mod_type != NULL): Detected a shared type! (transform the type) \n");
#endif
                 // Reset the base_type on the pointer to point to the base_type of the modifier.
                 // Note that a less elegant solution would be to call: mod_type->get_typeModifier().get_upcModifier().set_isShared(false).
                    SgType* modifier_base_type = mod_type->get_base_type();
                    ROSE_ASSERT(modifier_base_type != NULL);
#if DEBUG_TYPE_TRAVERSAL
                    printf ("TypeTraversal::transformType(): (mod_type != NULL): Removing shared base_type from pointerType = %p replacing with modifier_base_type = %p = %s \n",pointerType,modifier_base_type,modifier_base_type->class_name().c_str());
#endif
                    pointerType->set_base_type(modifier_base_type);
#if 1
                 // DQ (4/26/2014): Also mark this as not shared, since the cast expressions will refer directly to this SgModifierType type.
                    mod_type->get_typeModifier().get_upcModifier().set_isShared(false);
#else
#error "DEAD CODE!"
                    printf ("In TypeTraversal::transformType(): (mod_type != NULL): Skipping reset of upc modifier in SgModifierType: mod_type = %p \n",mod_type);
#endif
                  }
                 else
                  {
                 // DQ (5/16/2014): in the case of test2014_20.c the function parameter has a type with a short
                 // chain of SgModifierType IR nodes. In this case only the last one is marked as shared.
                 // There might be a more general AST post processing step for this, or it might be that we 
                 // need to build the attributes better to avoid such chains of SgModifierType IR nodes.
                    SgModifierType* nested_mod_type = isSgModifierType(mod_type->get_base_type());
                    if (nested_mod_type != NULL)
                       {
#if DEBUG_TYPE_TRAVERSAL
                         printf ("(mod_type != NULL): (nested_mod_type != NULL): nested_mod_type->get_typeModifier().displayString() = %s \n",nested_mod_type->get_typeModifier().displayString().c_str());
#endif
                         if (nested_mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
                            {
#if DEBUG_TYPE_TRAVERSAL
                              printf ("TypeTraversal::transformType(): (mod_type != NULL): (nested_mod_type != NULL): Detected a nested shared type! (transform the type) \n");
#endif
                           // Reset the base_type on the pointer to point to the base_type of the modifier.
                           // Note that a less elegant solution would be to call: mod_type->get_typeModifier().get_upcModifier().set_isShared(false).
                              SgType* nested_modifier_base_type = nested_mod_type->get_base_type();
                              ROSE_ASSERT(nested_modifier_base_type != NULL);
#if 0
                              printf ("TypeTraversal::transformType(): (mod_type != NULL): (nested_mod_type != NULL): Removing shared base_type from pointerType = %p replacing with modifier_base_type = %p = %s \n",pointerType,modifier_base_type,modifier_base_type->class_name().c_str());
#endif
                              mod_type->set_base_type(nested_modifier_base_type);
#if 1
                           // DQ (4/26/2014): Also mark this as not shared, since the cast expressions will refer directly to this SgModifierType type.
                              nested_mod_type->get_typeModifier().get_upcModifier().set_isShared(false);
#else
#error "DEAD CODE!"
                              printf ("In TypeTraversal::transformType(): (mod_type != NULL): (nested_mod_type != NULL): Skipping reset of upc modifier in SgModifierType: mod_type = %p \n",mod_type);
#endif
                            }
                       }
                  }
             }
        }
       else
        {
          SgModifierType* mod_type = isSgModifierType(type);
          if (mod_type != NULL)
             {
#if DEBUG_TYPE_TRAVERSAL
               printf ("Found a modifier type (not from a pointer type) \n");
#endif
#if DEBUG_TYPE_TRAVERSAL
               printf ("(Found a modifier type (not from a pointer type): mod_type->get_typeModifier().displayString() = %s \n",mod_type->get_typeModifier().displayString().c_str());
#endif
               SgType* base_type = mod_type->get_base_type();
#if DEBUG_TYPE_TRAVERSAL
               printf ("Found a modifier type (not from a pointer type): base_type = %p = %s \n",base_type,base_type->class_name().c_str());
#endif
            // DQ (5/31/2014): Reset the type to eliminate the shared keyword.
               mod_type->get_typeModifier().get_upcModifier().set_isShared(false);
             }
            else
             {
            // DQ (6/11/2014): Added more general support to include SgArrayType (see test2014_48.c).
#if 0
               printf ("In TypeTraversal::transformType(): type = %p = %s \n",type,type->class_name().c_str());
#endif
               SgArrayType* arrayType = isSgArrayType(type);
               if (arrayType != NULL)
                  {
#if 0
                    printf ("In TypeTraversal::transformType(): Found a SgArrayType: arrayType = %p \n",arrayType);
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                    transformType(arrayType->get_base_type());
                  }

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
        }
   }



TypeTraversalInheritedAttribute
TypeTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     TypeTraversalInheritedAttribute inheritedAttribute )
   {
#if DEBUG_TYPE_TRAVERSAL
     printf ("In TypeTraversal::evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

     return inheritedAttribute;
   }


TypeTraversalSynthesizedAttribute
TypeTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     TypeTraversalInheritedAttribute inheritedAttribute,
     SynthesizedAttributesList childAttributes )
   {
#if 0
     printf ("In TypeTraversal::evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
     
     SgType* type = isSgType(astNode);
     if (type != NULL)
        {
          transformType(type);
        }

     TypeTraversalSynthesizedAttribute localResult;

     return localResult;
   }


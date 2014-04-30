
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


void
TypeTraversal::transformType(SgType* type)
   {
  // Since only the base_types of pointers are shared, we can take as input the pointer type and just change it internally.
  // The reference to the type from non-type IR nodes need not be modified.

  // How complex can be expect the type system to be (do we required a nested type traversal).
     SgPointerType* pointerType = isSgPointerType(type);
     if (pointerType != NULL)
        {
       // Check if the base type is marked as shared.
          SgModifierType* mod_type = isSgModifierType(pointerType->get_base_type());
          if (mod_type != NULL && mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
             {
#if 0
               printf ("TypeTraversal::transformType(): Detected a shared type! (transform the type) \n");
#endif
            // Reset the base_type on the pointer to point to the base_type of the modifier.
            // Note that a less elegant solution would be to call: mod_type->get_typeModifier().get_upcModifier().set_isShared(false).
               SgType* modifier_base_type = mod_type->get_base_type();
               ROSE_ASSERT(modifier_base_type != NULL);
#if 0
               printf ("TypeTraversal::transformType(): Removing shared base_type from pointerType = %p replacing with modifier_base_type = %p = %s \n",pointerType,modifier_base_type,modifier_base_type->class_name().c_str());
#endif
               pointerType->set_base_type(modifier_base_type);
#if 1
            // DQ (4/26/2014): Also mark this as not shared, since the cast expressions will refer directly to this SgModifierType type.
               mod_type->get_typeModifier().get_upcModifier().set_isShared(false);
#else
               printf ("In TypeTraversal::transformType(): Skipping reset of upc modifier in SgModifierType: mod_type = %p \n",mod_type);
#endif
             }
        }
   }



TypeTraversalInheritedAttribute
TypeTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     TypeTraversalInheritedAttribute inheritedAttribute )
   {
#if 0
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


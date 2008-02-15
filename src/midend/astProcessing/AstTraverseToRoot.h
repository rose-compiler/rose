// ************************************************************************
//                           Traversal To Root
// ************************************************************************
// This traversal represents a 2nd alternative traversal from any AST node 
// back up the AST to the AST Root node (SgFile or SgProject) along the
// unique chain defined by the parent pointers at each node in the AST.
// This traversal can be helpful for queries (or potentially transformations)
// on variables where they are defined in any of the defining scopes from
// the current scope to the global scope.  It was initially developed to
// support queries to identify containment of subtrees in specific
// language constructs (required for the unparser, which currently does not
// use the AST traversal mechanism defined by ROSE, and so can't communicate
// context information though an inherited attribute).
// ************************************************************************

template< class InheritedAttributeType, class SynthesizedAttributeType >
class TraverseToRoot
   {
     public:
//        ReverseTraversal();

          SynthesizedAttributeType traverse (
               SgNode* astNode,
               InheritedAttributeType inputInheritedAttribute );

          virtual InheritedAttributeType evaluateInheritedAttribute (
               SgNode* astNode,
               InheritedAttributeType inputInheritedAttribute ) = 0;

          virtual SynthesizedAttributeType evaluateSynthesizedAttribute (
               SgNode* astNode,
               InheritedAttributeType inputInheritedAttribute,
               SynthesizedAttributeType inputSynthesizedAttribute ) = 0;
   };


// Implementation of traverse function
template< class InheritedAttributeType, class SynthesizedAttributeType >
SynthesizedAttributeType
TraverseToRoot<InheritedAttributeType,SynthesizedAttributeType>::traverse (
   SgNode* node,
   InheritedAttributeType inputInheritedAttribute)
   {
  // Trace the current node back as far as possible (should be able to reach SgGlobal)
  // printf ("Starting at node->sage_class_name() = %s \n",node->sage_class_name());
#if 1
     printf ("In traverse: at node->sage_class_name() = %s \n",node->sage_class_name());
#endif

     SynthesizedAttributeType returnAttribute;

     if (node->get_parent() != NULL)
        {
          SgNode* parentNode = node->get_parent();
       // printf ("     parentNode->sage_class_name() = %s \n",parentNode->sage_class_name());

          InheritedAttributeType localInheritedAttribute = evaluateInheritedAttribute(parentNode,inputInheritedAttribute);
          SynthesizedAttributeType localSynthesizedAttribute = traverse (parentNode,localInheritedAttribute);

          returnAttribute =
               evaluateSynthesizedAttribute (parentNode,localInheritedAttribute,localSynthesizedAttribute);
        }
#if 1
       else
        {
          printf ("final node in chain of parents is a %s \n",node->sage_class_name());
        }
#endif

     return returnAttribute;
   }




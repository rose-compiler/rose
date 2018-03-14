#include "sage3basic.h"
#include "UntypedFortranTraversal.h"

using namespace Untyped;
using std::cout;
using std::endl;

UntypedFortranTraversal::UntypedFortranTraversal(SgSourceFile* sourceFile, UntypedConverter* converter)
   : UntypedTraversal(sourceFile, converter)
   {
   }

InheritedAttribute
UntypedFortranTraversal::evaluateInheritedAttribute(SgNode* node, InheritedAttribute currentScope)
{
   switch (node->variantT())
   {
    default:
      {
         return UntypedTraversal::evaluateInheritedAttribute(node, currentScope);
      }
   }

   return currentScope;
}


SynthesizedAttribute
UntypedFortranTraversal::evaluateSynthesizedAttribute(SgNode* node, InheritedAttribute currentScope, SynthesizedAttributesList childAttrs)
{
// Synthesized attribute is an expression initialized to NULL for when an expression is a statement.
// Statements are added to the scope and don't need to be returned as an attribute.
   SynthesizedAttribute sg_expr = NULL;

   switch (node->variantT())
   {
  // At the moment this is a special case because of the type kind parameter.
  // May not be needed once a way is found to delete untyped type nodes.
     case V_SgUntypedValueExpression:
       {
         SgUntypedValueExpression* ut_expr = isSgUntypedValueExpression(node);
         sg_expr = pConverter->convertSgUntypedExpression(ut_expr);
         SgUntypedType* type = ut_expr->get_type();
         if (type->get_has_kind())
            {
            // TODO - find a way to delete untyped type nodes (perhaps let them be traversed)
               cout << "Up   traverse: deleted node of type ... " << node->class_name() << ": " << node << endl;
               cout << "                  has kind: " << type->get_has_kind() << endl;
            }
         delete ut_expr;
         break;
       }
    default:
      {
         return UntypedTraversal::evaluateSynthesizedAttribute(node, currentScope, childAttrs);
      }
   }

   return sg_expr;
}

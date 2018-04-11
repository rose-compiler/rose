#include "sage3basic.h"
#include "UntypedFortranTraversal.h"

#define DEBUG_UNTYPED_TRAVERSAL 0

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
#if DEBUG_UNTYPED_TRAVERSAL
   cout << "........  inherited traversing, scope is " << currentScope << endl;
#endif

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
    default:
      {
         return UntypedTraversal::evaluateSynthesizedAttribute(node, currentScope, childAttrs);
      }
   }

   return sg_expr;
}

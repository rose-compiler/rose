#include "sage3basic.h"
#include "UntypedTraversal.h"
#include "UntypedConverter.h"

#define DEBUG_UNTYPED_TRAVERSAL 0

using namespace Fortran::Untyped;


UntypedTraversal::UntypedTraversal(SgSourceFile* sourceFile)
{
   p_source_file = sourceFile;
}


InheritedAttribute
UntypedTraversal::evaluateInheritedAttribute(SgNode* n, InheritedAttribute currentScope)
{
   if (isSgUntypedFile(n) != NULL)
      {
         SgUntypedFile* ut_file = dynamic_cast<SgUntypedFile*>(n);
         SgSourceFile*  sg_file = p_source_file;
         ROSE_ASSERT(sg_file != NULL);

         currentScope = UntypedConverter::initialize_global_scope(sg_file);
      }

   else if (isSgUntypedGlobalScope(n) != NULL)
      {
         SgUntypedGlobalScope* ut_scope = dynamic_cast<SgUntypedGlobalScope*>(n);
         SgGlobal*             sg_scope = UntypedConverter::convertSgUntypedGlobalScope(ut_scope, SageBuilder::getGlobalScopeFromScopeStack());

         currentScope = sg_scope;
      }

   else if (isSgUntypedProgramHeaderDeclaration(n) != NULL)
      {
         SgUntypedProgramHeaderDeclaration* ut_program = dynamic_cast<SgUntypedProgramHeaderDeclaration*>(n);
         SgProgramHeaderStatement*          sg_program = UntypedConverter::convertSgUntypedProgramHeaderDeclaration(ut_program, currentScope);

         currentScope = SageBuilder::topScopeStack();
      }

   else if (isSgUntypedSubroutineDeclaration (n) != NULL)
      {
         SgUntypedSubroutineDeclaration* ut_function = dynamic_cast<SgUntypedSubroutineDeclaration*>(n);
         SgProcedureHeaderStatement*     sg_function = UntypedConverter::convertSgUntypedSubroutineDeclaration(ut_function, currentScope);

         currentScope = SageBuilder::topScopeStack();
      }

   else if (isSgUntypedFunctionDeclaration (n) != NULL)
      {
         SgUntypedFunctionDeclaration* ut_function = dynamic_cast<SgUntypedFunctionDeclaration*>(n);
         SgProcedureHeaderStatement*   sg_function = UntypedConverter::convertSgUntypedFunctionDeclaration(ut_function, currentScope);

         currentScope = SageBuilder::topScopeStack();
      }

   else if (isSgUntypedFunctionDeclarationList(n) != NULL)
      {
         SgUntypedFunctionDeclarationList* ut_list = dynamic_cast<SgUntypedFunctionDeclarationList*>(n);

      // The list is not converted (note that nothing is returned) but the current scope may be modified
         UntypedConverter::convertSgUntypedFunctionDeclarationList(ut_list, currentScope);
      }

   else if (isSgUntypedVariableDeclaration(n) != NULL)
      {
         SgUntypedVariableDeclaration* ut_decl = dynamic_cast<SgUntypedVariableDeclaration*>(n);
         UntypedConverter::convertSgUntypedVariableDeclaration(ut_decl, currentScope);
      }

   else if (isSgUntypedImplicitDeclaration(n) != NULL)
      {
         SgUntypedImplicitDeclaration* ut_decl = dynamic_cast<SgUntypedImplicitDeclaration*>(n);
         UntypedConverter::convertSgUntypedImplicitDeclaration(ut_decl, currentScope);
      }

   else
      {
#if DEBUG_UNTYPED_TRAVERSAL
         printf ("Down traverse: found a node of type ... %s\n", n->class_name().c_str());
#endif
      }
   return currentScope;
}


SynthesizedAttribute
UntypedTraversal::evaluateSynthesizedAttribute(SgNode* n, InheritedAttribute currentScope, SynthesizedAttributesList childAttrs)
{
// Synthesized attribute is temporarily an expression, initialize to NULL for when an expression doesn't make sense.
// Probaby should change the SynthesizedAttribute to an expression as statements will be added to the scope aren't
// returned as an attribute.
   SynthesizedAttribute sg_expr = NULL;   

   if ( isSgUntypedExpression(n) != NULL)
      {
         SgUntypedExpression* ut_expr = isSgUntypedExpression(n);
         SgExpressionPtrList children(childAttrs);

         sg_expr = UntypedConverter::convertSgUntypedExpression(ut_expr, children, currentScope);
      }
   else if ( isSgUntypedAssignmentStatement(n) != NULL )
      {
         SgUntypedAssignmentStatement* ut_stmt = dynamic_cast<SgUntypedAssignmentStatement*>(n);
         SgExpressionPtrList children(childAttrs);

         UntypedConverter::convertSgUntypedAssignmentStatement(ut_stmt, children, currentScope);
      }
   else
      {
#if DEBUG_UNTYPED_TRAVERSAL
         printf ("Up   traverse: found a node of type ... %s\n", n->class_name().c_str());
#endif
      }
   
   return sg_expr;
}

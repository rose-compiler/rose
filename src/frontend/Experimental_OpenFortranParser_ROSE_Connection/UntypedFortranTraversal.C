#include "sage3basic.h"
#include "UntypedFortranTraversal.h"

#define DEBUG_UNTYPED_TRAVERSAL 0

using namespace Fortran::Untyped;


UntypedFortranTraversal::UntypedFortranTraversal(SgSourceFile* sourceFile)
{
   p_source_file = sourceFile;
   pConverter = new UntypedFortranConverter(this);
}

UntypedFortranTraversal::~UntypedFortranTraversal()
{
   if (pConverter) delete pConverter;
}

InheritedAttribute
UntypedFortranTraversal::evaluateInheritedAttribute(SgNode* n, InheritedAttribute currentScope)
{
   if (isSgUntypedFile(n) != NULL)
      {
      // SgUntypedFile* ut_file = dynamic_cast<SgUntypedFile*>(n);
         SgSourceFile*  sg_file = p_source_file;
         ROSE_ASSERT(sg_file != NULL);

         currentScope = pConverter->initialize_global_scope(sg_file);
      }

   else if (isSgUntypedGlobalScope(n) != NULL)
      {
         SgUntypedGlobalScope* ut_scope = dynamic_cast<SgUntypedGlobalScope*>(n);
         SgGlobal*             sg_scope = pConverter->convertSgUntypedGlobalScope(ut_scope, SageBuilder::getGlobalScopeFromScopeStack());

         currentScope = sg_scope;
      }

   else if (isSgUntypedModuleDeclaration(n) != NULL)
      {
         SgUntypedModuleDeclaration* ut_module = dynamic_cast<SgUntypedModuleDeclaration*>(n);
         pConverter->convertSgUntypedModuleDeclaration(ut_module,currentScope);

         currentScope = SageBuilder::topScopeStack();
      }

   else if (isSgUntypedProgramHeaderDeclaration(n) != NULL)
      {
         SgUntypedProgramHeaderDeclaration* ut_program = dynamic_cast<SgUntypedProgramHeaderDeclaration*>(n);
         pConverter->convertSgUntypedProgramHeaderDeclaration(ut_program,currentScope);

      // TODO - think about using SageBuild scope stack (currently used for programs)
         currentScope = SageBuilder::topScopeStack();
      }

   else if (isSgUntypedSubroutineDeclaration (n) != NULL)
      {
         SgUntypedSubroutineDeclaration* ut_function = dynamic_cast<SgUntypedSubroutineDeclaration*>(n);
         SgProcedureHeaderStatement* sg_function = pConverter->convertSgUntypedSubroutineDeclaration(ut_function, currentScope);

         currentScope = sg_function->get_definition()->get_body();
      }

   else if (isSgUntypedInterfaceDeclaration (n) != NULL)
      {
         std::cout << "--- TODO: convert SgUntypedInterfaceDeclaration\n";
      }

   else if (isSgUntypedBlockDataDeclaration (n) != NULL)
      {
         SgUntypedBlockDataDeclaration* ut_block_data = dynamic_cast<SgUntypedBlockDataDeclaration*>(n);
         SgProcedureHeaderStatement* sg_function = pConverter->convertSgUntypedBlockDataDeclaration(ut_block_data, currentScope);

         currentScope = sg_function->get_definition()->get_body();
      }

   else if (isSgUntypedFunctionDeclaration (n) != NULL)
      {
         SgUntypedFunctionDeclaration* ut_function = dynamic_cast<SgUntypedFunctionDeclaration*>(n);
         SgProcedureHeaderStatement* sg_function = pConverter->convertSgUntypedFunctionDeclaration(ut_function, currentScope);

         currentScope = sg_function->get_definition()->get_body();
      }

   else if (isSgUntypedFunctionDeclarationList(n) != NULL)
      {
         SgUntypedFunctionDeclarationList* ut_list = dynamic_cast<SgUntypedFunctionDeclarationList*>(n);

      // The list is not converted (note that nothing is returned) but the current scope may be modified
         pConverter->convertSgUntypedFunctionDeclarationList(ut_list, currentScope);
      }

   else if (isSgUntypedVariableDeclaration(n) != NULL)
      {
         SgUntypedVariableDeclaration* ut_decl = dynamic_cast<SgUntypedVariableDeclaration*>(n);
         pConverter->convertSgUntypedVariableDeclaration(ut_decl, currentScope);
      }

   else if (isSgUntypedImplicitDeclaration(n) != NULL)
      {
         SgUntypedImplicitDeclaration* ut_decl = dynamic_cast<SgUntypedImplicitDeclaration*>(n);
         pConverter->convertSgUntypedImplicitDeclaration(ut_decl, currentScope);
      }

   else if (isSgUntypedNameListDeclaration(n) != NULL)
      {
         SgUntypedNameListDeclaration* ut_decl = dynamic_cast<SgUntypedNameListDeclaration*>(n);
         std::cout << "NEED to convert name list decl\n";
         pConverter->convertSgUntypedNameListDeclaration(ut_decl, currentScope);
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
UntypedFortranTraversal::evaluateSynthesizedAttribute(SgNode* n, InheritedAttribute currentScope, SynthesizedAttributesList childAttrs)
{
// Synthesized attribute is temporarily an expression, initialize to NULL for when an expression doesn't make sense.
// Probaby should change the SynthesizedAttribute to an expression as statements will be added to the scope aren't
// returned as an attribute.
   SynthesizedAttribute sg_expr = NULL;   

   if ( isSgUntypedExpression(n) != NULL)
      {
         SgUntypedExpression* ut_expr = isSgUntypedExpression(n);
         SgExpressionPtrList children(childAttrs);

         sg_expr = pConverter->convertSgUntypedExpression(ut_expr, children, currentScope);
      }
   else if ( isSgUntypedAssignmentStatement(n) != NULL )
      {
         SgUntypedAssignmentStatement* ut_stmt = dynamic_cast<SgUntypedAssignmentStatement*>(n);
         SgExpressionPtrList children(childAttrs);

         pConverter->convertSgUntypedAssignmentStatement(ut_stmt, children, currentScope);
      }
   else if ( isSgUntypedExpressionStatement(n) != NULL )
      {
         SgUntypedExpressionStatement* ut_stmt = dynamic_cast<SgUntypedExpressionStatement*>(n);
         SgExpressionPtrList children(childAttrs);

         pConverter->convertSgUntypedExpressionStatement(ut_stmt, children, currentScope);
      }
   else if ( isSgUntypedOtherStatement(n) != NULL )
      {
         SgUntypedOtherStatement* ut_stmt = dynamic_cast<SgUntypedOtherStatement*>(n);
         pConverter->convertSgUntypedOtherStatement(ut_stmt, currentScope);
      }
   else
      {
#if DEBUG_UNTYPED_TRAVERSAL
         printf ("Up   traverse: found a node of type ... %s\n", n->class_name().c_str());
#endif
      }

   return sg_expr;
}

#include "sage3basic.h"
#include "UntypedTraversal.h"

#define DEBUG_UNTYPED_TRAVERSAL 0

using namespace Untyped;
using std::cout;
using std::endl;

UntypedTraversal::UntypedTraversal(SgSourceFile* sourceFile, UntypedConverter* converter)
{
   p_source_file = sourceFile;
   pConverter = converter;
}

InheritedAttribute
UntypedTraversal::evaluateInheritedAttribute(SgNode* node, InheritedAttribute currentScope)
{
#if DEBUG_UNTYPED_TRAVERSAL > 0
        cout << "Down traverse: found a node of type ... " << node->class_name() << ": " << node << endl;
#endif

// The currentScope is being changed somewhere unexpectedly
   if (currentScope != SageBuilder::topScopeStack()) {
      cout << "Down traverse: found a node of type ... " << node->class_name() << ": " << node << endl;
      cout << "WARNING --- currentScope different from SageBuilder::topScopeStack() using SageBuilder::topScopeStack "
           << currentScope << ":" << SageBuilder::topScopeStack()<< endl;
      cout << endl;
      currentScope = SageBuilder::topScopeStack();
   }

   switch (node->variantT())
   {
     case V_SgUntypedFile:
       {
          SgSourceFile* sg_file = p_source_file;
          ROSE_ASSERT(sg_file != NULL);
          currentScope = pConverter->initialize_global_scope(sg_file);
          break;
       }
     case V_SgUntypedGlobalScope:
       {
          SgUntypedGlobalScope* ut_scope = dynamic_cast<SgUntypedGlobalScope*>(node);
          SgGlobal* sg_scope = pConverter->convertUntypedGlobalScope(ut_scope, SageBuilder::getGlobalScopeFromScopeStack());
          currentScope = sg_scope;
          break;
      }
    case V_SgUntypedDirectiveDeclaration:
      {
         SgUntypedDirectiveDeclaration* ut_decl = dynamic_cast<SgUntypedDirectiveDeclaration*>(node);
         pConverter->convertUntypedDirectiveDeclaration(ut_decl, currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedEnumDeclaration:
      {
         SgUntypedEnumDeclaration* ut_decl = dynamic_cast<SgUntypedEnumDeclaration*>(node);
         pConverter->convertUntypedEnumDeclaration(ut_decl, currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedStructureDeclaration:
      {
         SgUntypedStructureDeclaration* ut_struct = dynamic_cast<SgUntypedStructureDeclaration*>(node);
         pConverter->convertUntypedStructureDeclaration(ut_struct,currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedStructureDefinition:
      {
      // This node is converted by the containing SgUntypedStructureDeclaration
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedTypedefDeclaration:
      {
         SgUntypedTypedefDeclaration* ut_decl = dynamic_cast<SgUntypedTypedefDeclaration*>(node);
         pConverter->convertUntypedTypedefDeclaration(ut_decl, currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedModuleDeclaration:
      {
         SgUntypedModuleDeclaration* ut_module = dynamic_cast<SgUntypedModuleDeclaration*>(node);
         pConverter->convertUntypedModuleDeclaration(ut_module,currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedProgramHeaderDeclaration:
      {
         SgUntypedProgramHeaderDeclaration* ut_program = dynamic_cast<SgUntypedProgramHeaderDeclaration*>(node);
         pConverter->convertUntypedProgramHeaderDeclaration(ut_program,currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedSubroutineDeclaration:
      {
         SgUntypedSubroutineDeclaration* ut_function = dynamic_cast<SgUntypedSubroutineDeclaration*>(node);
         pConverter->convertUntypedSubroutineDeclaration(ut_function, currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedFunctionDeclaration:
      {
         SgUntypedFunctionDeclaration* ut_function = dynamic_cast<SgUntypedFunctionDeclaration*>(node);
         pConverter->convertUntypedFunctionDeclaration(ut_function, currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedFunctionDeclarationList:
      {
         SgUntypedFunctionDeclarationList* ut_list = dynamic_cast<SgUntypedFunctionDeclarationList*>(node);

      // The list is not converted (needed to add a contains statement) but the current scope may be modified
         pConverter->convertUntypedFunctionDeclarationList(ut_list, currentScope);
         break;
      }
    case V_SgUntypedInterfaceDeclaration:
      {
         cout << "--- TODO: convert SgUntypedInterfaceDeclaration\n";
         break;
      }
    case V_SgUntypedBlockDataDeclaration:
      {
         SgUntypedBlockDataDeclaration* ut_block_data = dynamic_cast<SgUntypedBlockDataDeclaration*>(node);
         pConverter->convertUntypedBlockDataDeclaration(ut_block_data, currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedUseStatement:
      {
         cout << "-x- convert SgUntypedUseStatement node " << node << endl;
         SgUntypedUseStatement* ut_use_stmt = dynamic_cast<SgUntypedUseStatement*>(node);
         pConverter->convertUntypedUseStatement(ut_use_stmt, currentScope);
         break;
      }
    case V_SgUntypedVariableDeclaration:
      {
         SgUntypedVariableDeclaration* ut_decl = dynamic_cast<SgUntypedVariableDeclaration*>(node);
         pConverter->convertSgUntypedVariableDeclaration(ut_decl, currentScope);
         break;
      }
    case V_SgUntypedImplicitDeclaration:
      {
         SgUntypedImplicitDeclaration* ut_decl = dynamic_cast<SgUntypedImplicitDeclaration*>(node);
         pConverter->convertSgUntypedImplicitDeclaration(ut_decl, currentScope);
         break;
      }
    case V_SgUntypedNameListDeclaration:
      {
         SgUntypedNameListDeclaration* ut_decl = dynamic_cast<SgUntypedNameListDeclaration*>(node);
         pConverter->convertUntypedNameListDeclaration(ut_decl, currentScope);
         break;
      }
    case V_SgUntypedInitializedNameListDeclaration:
      {
         SgUntypedInitializedNameListDeclaration* ut_decl = dynamic_cast<SgUntypedInitializedNameListDeclaration*>(node);
         pConverter->convertSgUntypedInitializedNameListDeclaration(ut_decl, currentScope);
         break;
      }
    case V_SgUntypedBlockStatement:
      {
         SgUntypedBlockStatement* ut_block_stmt = dynamic_cast<SgUntypedBlockStatement*>(node);
#if 0
         cout << "-x- SgUntypedBlockStatement: " << ut_block_stmt << std::endl;
         cout << "-x- scope is : " << ut_block_stmt->get_scope() << std::endl;
#endif

         SgBasicBlock* sg_basic_block = pConverter->convertSgUntypedBlockStatement(ut_block_stmt, currentScope);

#if 0
         cout << "---     top scope is : " << SageBuilder::topScopeStack() << " "
              << SageBuilder::topScopeStack()->class_name() << endl;
         cout << "--- current scope is : " << currentScope << " " << currentScope->class_name() << endl;
         cout << "---     new scope is : " << sg_basic_block << " " << sg_basic_block->class_name() << endl;
#endif

         currentScope = SageBuilder::topScopeStack();

         break;
      }
    case V_SgUntypedForAllStatement:
      {
         SgUntypedForAllStatement* ut_stmt = dynamic_cast<SgUntypedForAllStatement*>(node);
         pConverter->convertSgUntypedForAllStatement(ut_stmt, currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedImageControlStatement:
      {
         SgUntypedImageControlStatement* ut_stmt = dynamic_cast<SgUntypedImageControlStatement*>(node);
         pConverter->convertSgUntypedImageControlStatement(ut_stmt, currentScope);
         break;
      }
    case V_SgUntypedLabelStatement:
      {
         SgUntypedLabelStatement* ut_decl = dynamic_cast<SgUntypedLabelStatement*>(node);
         pConverter->convertSgUntypedLabelStatement_decl(ut_decl, currentScope);
         break;
      }
    case V_SgUntypedNamedStatement:
      {
         SgUntypedNamedStatement* ut_decl = dynamic_cast<SgUntypedNamedStatement*>(node);
         pConverter->convertUntypedNamedStatement(ut_decl, currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedNullStatement:
      {
         SgUntypedNullStatement* ut_decl = dynamic_cast<SgUntypedNullStatement*>(node);
         pConverter->convertSgUntypedNullStatement(ut_decl, currentScope);
         break;
      }
    default:
      {
#if DEBUG_UNTYPED_TRAVERSAL > 1
        cout << "Down traverse: found a node of type ... " << node->class_name() << ": " << node << endl;
#endif
      }
   }

#if DEBUG_UNTYPED_TRAVERSAL > 1
        cout << "     checking scope stack " << (currentScope == SageBuilder::topScopeStack())
             << " " << currentScope << ":" << SageBuilder::topScopeStack() << " isa ==> " << currentScope->class_name() << endl;
#endif

// The currentScope is being changed somewhere unexpectedly
   ROSE_ASSERT(currentScope == SageBuilder::topScopeStack());

   return currentScope;
}


SynthesizedAttribute
UntypedTraversal::evaluateSynthesizedAttribute(SgNode* node, InheritedAttribute currentScope, SynthesizedAttributesList childAttrs)
{
// Synthesized attribute is an expression initialized to NULL for when an expression is a statement.
// Statements are added to the scope and don't need to be returned as an attribute.
   SynthesizedAttribute sg_node = NULL;

#if DEBUG_UNTYPED_TRAVERSAL > 0
         cout << "Up   traverse: found a node of type ... " << node->class_name() << ": " << node << endl;
#endif

   switch (node->variantT())
   {
    case V_SgUntypedUnaryOperator:
      {
         SgUntypedExpression* ut_expr = isSgUntypedUnaryOperator(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedExpression(ut_expr, children);
         break;
      }

    case V_SgUntypedBinaryOperator:
      {
         SgUntypedExpression* ut_expr = isSgUntypedBinaryOperator(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedExpression(ut_expr, children);
         break;
      }

    case V_SgUntypedNullExpression:
    case V_SgUntypedReferenceExpression:
    case V_SgUntypedValueExpression:
      {
         SgUntypedExpression* ut_expr = isSgUntypedExpression(node);
         sg_node = pConverter->convertSgUntypedExpression(ut_expr);
         break;
      }
    case V_SgUntypedArrayReferenceExpression:
      {
         SgUntypedArrayReferenceExpression* ut_expr = isSgUntypedArrayReferenceExpression(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertUntypedArrayReferenceExpression(ut_expr, children);
         break;
      }
    case V_SgUntypedExprListExpression:
      {
         SgUntypedExprListExpression* ut_expr = isSgUntypedExprListExpression(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedExprListExpression(ut_expr, children);
         break;
      }
    case V_SgUntypedSubscriptExpression:
      {
         SgUntypedSubscriptExpression* ut_expr = isSgUntypedSubscriptExpression(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedSubscriptExpression(ut_expr, children);
         break;
      }
    case V_SgUntypedAbortStatement:
      {
         SgUntypedAbortStatement* ut_stmt = dynamic_cast<SgUntypedAbortStatement*>(node);
         sg_node = pConverter->convertSgUntypedAbortStatement(ut_stmt, currentScope);
         break;
      }
    case V_SgUntypedAssignmentStatement:
      {
         SgUntypedAssignmentStatement* ut_stmt = dynamic_cast<SgUntypedAssignmentStatement*>(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedAssignmentStatement(ut_stmt, children, currentScope);
         break;
      }
    case V_SgUntypedCaseStatement:
      {
         SgUntypedCaseStatement* ut_stmt = dynamic_cast<SgUntypedCaseStatement*>(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertUntypedCaseStatement(ut_stmt, children, currentScope);
         break;
      }
    case V_SgUntypedExpressionStatement:
      {
         SgUntypedExpressionStatement* ut_stmt = dynamic_cast<SgUntypedExpressionStatement*>(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedExpressionStatement(ut_stmt, children, currentScope);
         break;
      }
    case V_SgUntypedExitStatement:
      {
         SgUntypedExitStatement* ut_stmt = dynamic_cast<SgUntypedExitStatement*>(node);
         sg_node = pConverter->convertSgUntypedExitStatement(ut_stmt, currentScope);
         break;
      }
    case V_SgUntypedForStatement:
      {
         SgUntypedForStatement* ut_stmt = dynamic_cast<SgUntypedForStatement*>(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertUntypedForStatement(ut_stmt, children, currentScope);
         break;
      }
    case V_SgUntypedFunctionCallStatement:
      {
         SgUntypedFunctionCallStatement* ut_stmt = dynamic_cast<SgUntypedFunctionCallStatement*>(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedFunctionCallStatement(ut_stmt, children, currentScope);
         break;
      }
    case V_SgUntypedGotoStatement:
      {
         SgUntypedGotoStatement* ut_stmt = dynamic_cast<SgUntypedGotoStatement*>(node);
         sg_node = pConverter->convertSgUntypedGotoStatement(ut_stmt, currentScope);
         break;
      }
    case V_SgUntypedIfStatement:
      {
         SgUntypedIfStatement* ut_stmt = dynamic_cast<SgUntypedIfStatement*>(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedIfStatement(ut_stmt, children, currentScope);
         break;
      }
    case V_SgUntypedImageControlStatement:
      {
         SgUntypedImageControlStatement* ut_stmt = dynamic_cast<SgUntypedImageControlStatement*>(node);
         SgNodePtrList children(childAttrs);
         pConverter->convertSgUntypedImageControlStatement(ut_stmt, children, currentScope);
         break;
      }
#if 0
    case V_SgUntypedScope:
      {
         SgUntypedScope* ut_stmt = dynamic_cast<SgUntypedScope*>(node);
         SgNodePtrList children(childAttrs);
         cout << "--- SgUntypedScope: " << ut_stmt << " # children is " << children.size() << endl;
         break;
      }
#endif
    case V_SgUntypedOtherStatement:
      {
         SgUntypedOtherStatement* ut_stmt = dynamic_cast<SgUntypedOtherStatement*>(node);
         sg_node = pConverter->convertSgUntypedOtherStatement(ut_stmt, currentScope);
         break;
      }
    case V_SgUntypedReturnStatement:
      {
         SgUntypedReturnStatement* ut_stmt = dynamic_cast<SgUntypedReturnStatement*>(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedReturnStatement(ut_stmt, children, currentScope);
         break;
      }
    case V_SgUntypedStopStatement:
      {
         SgUntypedStopStatement* ut_stmt = dynamic_cast<SgUntypedStopStatement*>(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertSgUntypedStopStatement(ut_stmt, children, currentScope);
         break;
      }
    case V_SgUntypedWhileStatement:
      {
         SgUntypedWhileStatement* ut_stmt = dynamic_cast<SgUntypedWhileStatement*>(node);
         SgNodePtrList children(childAttrs);
         sg_node = pConverter->convertUntypedWhileStatement(ut_stmt, children, currentScope);
         break;
      }

 // Declarations that require cleaning up the SageBuilder scope stack
    case V_SgUntypedBlockDataDeclaration:
      {
         SageBuilder::popScopeStack();  // block data body
         SageBuilder::popScopeStack();  // block data definition
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedBlockStatement:
      {
#if 0
         SgNodePtrList children(childAttrs);
         cout << "--- SgUntypedBlockStatement: # children is " << children.size() << endl;
         cout << "---     top scope is : " << SageBuilder::topScopeStack() << " "
              << SageBuilder::topScopeStack()->class_name() << endl;
         cout << "--- current scope is : " << currentScope << " " << currentScope->class_name() << endl;
#endif

         sg_node = SageBuilder::topScopeStack();
         SageBuilder::popScopeStack();

         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedStructureDeclaration:
      {
         SageBuilder::popScopeStack();  // structure scope
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedProgramHeaderDeclaration:
      {
         SageBuilder::popScopeStack();  // program body
         SageBuilder::popScopeStack();  // program definition
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedSubroutineDeclaration:
      {
         SageBuilder::popScopeStack();  // procedure body
         SageBuilder::popScopeStack();  // procedure definition
         currentScope = SageBuilder::topScopeStack();
         break;
      }
    case V_SgUntypedFunctionDeclaration:
      {
         SgUntypedFunctionDeclaration* ut_function = dynamic_cast<SgUntypedFunctionDeclaration*>(node);
         SgNodePtrList children(childAttrs);

      // Convert the types in the parameter list to what has by now been seen in variable declarations
         sg_node = pConverter->convertUntypedFunctionDeclaration(ut_function, children, currentScope);
         currentScope = SageBuilder::topScopeStack();
         break;
      }

    default:
      {
#if DEBUG_UNTYPED_TRAVERSAL > 1
         cout << "Up   traverse: found a node of type ... " << node->class_name() << ": " << node << endl;
         cout << "     checking scope stack " << (currentScope == SageBuilder::topScopeStack())
              << " " << currentScope << ":" << SageBuilder::topScopeStack() << endl;
#endif
      }
   }

// Make sure that the untyped node has been deleted as it is no longer needed
// Note that SgUntypedFile nodes are constructed and deleted by the ATerm traversal class
   if (node != NULL && !isSgUntypedFile(node))
      {
#if DEBUG_UNTYPED_TRAVERSAL > 0
         cout << "-----traverse: deletng node of type ... " << node->class_name() << ": " << node << endl;
         cout << "     checking scope stack " << (currentScope == SageBuilder::topScopeStack())
              << " " << currentScope << ":" << SageBuilder::topScopeStack() << " isa ==> " << currentScope->class_name() << endl;
#endif
         delete node;
      }
   
#if 0
   SgBasicBlock* block = isSgBasicBlock(sg_node);
   if (block)
      {
         cout << "-x- isSgBasicBlock " << block << endl;
         cout << "-x-         parent " << block->get_parent();
         if (block->get_parent()) cout << " " << block->get_parent()->class_name();
         cout << endl;
      }
#endif

// The currentScope is being changed somewhere unexpectedly
   ROSE_ASSERT(currentScope == SageBuilder::topScopeStack());

   return sg_node;
}

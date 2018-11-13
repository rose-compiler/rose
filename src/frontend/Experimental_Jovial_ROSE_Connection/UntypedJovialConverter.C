#include "sage3basic.h"
#include "UntypedJovialConverter.h"
#include "Jovial_to_ROSE_translation.h"
#include "general_language_translation.h"

#define DEBUG_UNTYPED_CONVERTER 0

using namespace Untyped;
using std::cout;
using std::endl;


bool
UntypedJovialConverter::convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                                     SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope)
{
   return UntypedJovialConverter::convertLabel(ut_stmt, sg_stmt, label_scope);
}

bool
UntypedJovialConverter::convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt, SgScopeStatement* scope)
{
   SgNode* parent;
   SgUntypedLabelStatement* ut_label_stmt;
   bool hasLabel = false;

   ROSE_ASSERT(scope != NULL);

   parent = ut_stmt->get_parent();
   if (parent != NULL)
     {
        ut_label_stmt = isSgUntypedLabelStatement(parent);
        if (ut_label_stmt != NULL)
          {
             SgLabelSymbol* label_symbol = NULL;
             SgLabelStatement* label_stmt = NULL;

          // This statement has a label get the SgLabelStatement from the scope and insert it
             SgName label_name(ut_label_stmt->get_label_string());

             SgFunctionDefinition * label_scope = SageInterface::getEnclosingFunctionDefinition(scope, true);
             ROSE_ASSERT (label_scope);

             label_symbol = label_scope->lookup_label_symbol(label_name);
             ROSE_ASSERT(label_symbol != NULL);

             label_stmt = label_symbol->get_declaration();
             ROSE_ASSERT(label_stmt != NULL);

             label_stmt->set_statement(sg_stmt);
             sg_stmt->set_parent(label_stmt);

             hasLabel = true;
          }
     }

   return hasLabel;
}

SgDeclarationStatement*
UntypedJovialConverter::convertUntypedJovialCompoolStatement(SgUntypedNameListDeclaration* ut_decl, SgScopeStatement* scope)
{
   ROSE_ASSERT(ut_decl);
   ROSE_ASSERT(ut_decl->get_statement_enum() == General_Language_Translation::e_jovial_compool_stmt);

   SgUntypedNamePtrList ut_names = ut_decl->get_names()->get_name_list();
   ROSE_ASSERT(ut_names.size() == 1);

   SgUntypedName* name = ut_names[0];
   SgJovialCompoolStatement* compool_decl = new SgJovialCompoolStatement(name->get_name());
   setSourcePositionFrom(compool_decl, ut_decl);

   compool_decl->set_definingDeclaration(compool_decl);
   compool_decl->set_firstNondefiningDeclaration(compool_decl);

   SageInterface::appendStatement(compool_decl, scope);

   return compool_decl;
}

SgStatement*
UntypedJovialConverter::convertUntypedCaseStatement (SgUntypedCaseStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
  {
     SgStatement* sg_stmt = UntypedConverter::convertUntypedCaseStatement(ut_stmt, children, scope);

  // If a Jovial CaseAlternative rule doesn't have a FALLTHRU, then create a
  // compiler-generated break statement so that program analysis will be the same as for C.
  //
     if (ut_stmt->get_has_fall_through() == false && isSgSwitchStatement(sg_stmt) == false)
       {
          SgBreakStmt* sg_break_stmt = SageBuilder::buildBreakStmt();
          ROSE_ASSERT(sg_break_stmt);

          sg_break_stmt->setCompilerGenerated();
          SageInterface::appendStatement(sg_break_stmt, scope);
       }

     return sg_stmt;
  }

SgStatement*
UntypedJovialConverter::convertUntypedForStatement (SgUntypedForStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      ROSE_ASSERT(children.size() == 4);

      SgStatement* sg_stmt = NULL;
      int stmt_enum = ut_stmt->get_statement_enum();

      SgAssignOp* init_expr = isSgAssignOp(children[0]);
      ROSE_ASSERT(init_expr);

      SgExpression* test_expr = isSgExpression(children[1]);
      ROSE_ASSERT(test_expr);

      SgExpression* incr_expr = isSgExpression(children[2]);
      ROSE_ASSERT(incr_expr);

      SgStatement* loop_body = isSgStatement(children[3]);
      ROSE_ASSERT(loop_body);

#if 0
      cout << "-x- convert ForStatement # children is " << children.size() << endl;
      cout << "-x- convert for: initialization is " << init_expr << ": " << init_expr->class_name() << endl;
      cout << "-x- convert for: increment      is " << incr_expr << ": " << incr_expr->class_name() << endl;
      cout << "-x- convert for: test           is " << test_expr << ": " << test_expr->class_name() << endl;
      cout << "-x- convert for: body           is " << loop_body << ": " << loop_body->class_name() << endl;
#endif

   // The loop body (at least for a single statement) will have been inserted in the scope
      SageInterface::removeStatement(loop_body, scope);

      switch (stmt_enum)
      {
        case Jovial_ROSE_Translation::e_for_by_while_stmt:
        case Jovial_ROSE_Translation::e_for_while_by_stmt:
          {
             SgExprStatement* init_stmt = SageBuilder::buildExprStatement(init_expr);
             SgExprStatement* test_stmt = SageBuilder::buildExprStatement(test_expr);
                                sg_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, incr_expr, loop_body, NULL);
             break;
          }
        case Jovial_ROSE_Translation::e_for_then_while_stmt:
        case Jovial_ROSE_Translation::e_for_while_then_stmt:
          {
             SgBasicBlock* block_body = isSgBasicBlock(loop_body);
             if (block_body == NULL) {
                block_body = SageBuilder::buildBasicBlock(loop_body);
             }
             ROSE_ASSERT (block_body);

          // TODO: create a SageBuilder function for this
             sg_stmt = new SgJovialForThenStatement(init_expr, incr_expr, test_expr, block_body);

             SageInterface::setOneSourcePositionForTransformation(sg_stmt);
             init_expr->set_parent(sg_stmt);
             incr_expr->set_parent(sg_stmt);
             test_expr->set_parent(sg_stmt);
             block_body->set_parent(sg_stmt);

             break;
          }
        default:
          {
             ROSE_ASSERT(0);
          }
      }

      ROSE_ASSERT(sg_stmt != NULL);
      setSourcePositionFrom(sg_stmt, ut_stmt);

      SageInterface::appendStatement(sg_stmt, scope);

      return sg_stmt;
   }

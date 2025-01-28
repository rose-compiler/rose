
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG_unparseNewOp 0

void
Unparse_ExprStmt::unparseNewOp(SgExpression* expr, SgUnparse_Info& info)
   {
#if DEBUG_unparseNewOp
     printf ("Enter Unparse_ExprStmt::unparseNewOp()\n");
#endif

     SgNewExp* new_op = isSgNewExp(expr);
     ASSERT_not_null(new_op);

     if (new_op->get_need_global_specifier()) curprint ( "::");

     curprint ("new ");

     SgUnparse_Info newinfo(info);
     newinfo.unset_inVarDecl();
     if (new_op->get_placement_args() != NULL)
        {
          curprint ( "(");
          unparseExpression(new_op->get_placement_args(), newinfo);
          curprint ( ") ");
        }

     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart();
     newinfo.unset_isTypeSecondPart();
     newinfo.set_SkipClassSpecifier();
     newinfo.unset_SkipBaseType();
     newinfo.set_reference_node_for_qualification(new_op);
     ASSERT_not_null(newinfo.get_reference_node_for_qualification());

     bool add_parenthesis_around_type = false;
     if (new_op->get_constructor_args() != NULL)
        {
          SgType* newOperatorSpecifiedType = new_op->get_specified_type();
          ASSERT_not_null(newOperatorSpecifiedType);
          SgArrayType* newOperatorArrayType = isSgArrayType(newOperatorSpecifiedType);
          if (newOperatorArrayType == NULL)
             {
               add_parenthesis_around_type = true;
             }
        }

#if DEBUG_unparseNewOp
     printf ("  add_parenthesis_around_type = %s\n", add_parenthesis_around_type ? "true" : "false");
#endif

     if (add_parenthesis_around_type) curprint ("( ");

     newinfo.set_reference_node_for_qualification(new_op);
     ASSERT_not_null(newinfo.get_reference_node_for_qualification());
     unp->u_type->unparseType(new_op->get_specified_type(), newinfo);

     if (add_parenthesis_around_type) curprint (") ");

     if (new_op->get_constructor_args() != NULL)
        {
          unparseExpression(new_op->get_constructor_args(), newinfo);
        }

     if (new_op->get_builtin_args() != NULL)
        {
          unparseExpression(new_op->get_builtin_args(), newinfo);
        }

#if DEBUG_unparseNewOp
     printf ("Leaving Unparse_ExprStmt::unparseNewOp()\n");
#endif
   }

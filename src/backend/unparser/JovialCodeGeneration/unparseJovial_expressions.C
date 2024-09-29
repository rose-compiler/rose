/*
 * unparseJovial_expressions.C
 */
#include "sage3basic.h"
#include "unparser.h"

#ifdef _MSC_VER
#include "Cxx_Grammar.h"
#endif


void UnparseJovial::unparseNullptrVal(SgExpression*, SgUnparse_Info&)
  {
     curprint("NULL");
  }

void UnparseJovial::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info)
  {
    ASSERT_not_null(expr);

     // unparse comments preceding the expression
     unparseCommentsBefore(expr, info);

    switch (expr->variantT())
       {
       // function, intrinsic calls
          case V_SgFunctionCallExp:     unparseFuncCall(expr, info);             break;

       // expressions
          case V_SgSubscriptExpression: unparseSubscriptExpr(expr, info);        break;
          case V_SgAsteriskShapeExp:    unparseAsteriskShapeExpr(expr, info);    break;
          case V_SgJovialBitVal:        unparseJovialBitVal(expr, info);         break;

       // symbol references
          case V_SgFunctionRefExp:      unparseFuncRef    (expr, info);          break;
          case V_SgLabelRefExp:         unparseLabelRef   (expr, info);          break;
          case V_SgVarRefExp:           unparseVarRef     (expr, info);          break;
          case V_SgPointerDerefExp:     unparsePtrDeref   (expr, info);          break;
          case V_SgTypeExpression:      unparseTypeExpr   (expr, info);          break;

       // operators
          case V_SgUnaryOp:             unparseUnaryExpr  (expr, info);          break;
          case V_SgBinaryOp:            unparseBinaryExpr (expr, info);          break;
          case V_SgCastExp:             unparseCastExp    (expr, info);          break;

          case V_SgAssignOp:            unparseAssignOp   (expr, info);          break;

          case V_SgAddOp:               unparseBinaryOperator(expr, "+", info);  break;
          case V_SgAtOp:                unparseBinaryOperator(expr, "@", info);  break;
          case V_SgSubtractOp:          unparseBinaryOperator(expr, "-", info);  break;
          case V_SgMultiplyOp:          unparseBinaryOperator(expr, "*", info);  break;
          case V_SgDivideOp:            unparseBinaryOperator(expr, "/", info);  break;
          case V_SgModOp:               unparseBinaryOperator(expr,"MOD",info);  break;
          case V_SgEqualityOp:          unparseBinaryOperator(expr, "=", info);  break;
          case V_SgExponentiationOp:    unparseBinaryOperator(expr,"**", info);  break;
          case V_SgLessThanOp:          unparseBinaryOperator(expr, "<", info);  break;
          case V_SgLessOrEqualOp:       unparseBinaryOperator(expr,"<=", info);  break;
          case V_SgGreaterThanOp:       unparseBinaryOperator(expr, ">", info);  break;
          case V_SgGreaterOrEqualOp:    unparseBinaryOperator(expr,">=", info);  break;
          case V_SgNotEqualOp:          unparseBinaryOperator(expr,"<>", info);  break;
          case V_SgBitAndOp:            unparseBinaryOperator(expr,"AND", info); break;
          case V_SgBitEqvOp:            unparseBinaryOperator(expr,"EQV", info); break;
          case V_SgBitOrOp:             unparseBinaryOperator(expr,"OR", info);  break;
          case V_SgBitXorOp:            unparseBinaryOperator(expr,"XOR", info); break;

          case V_SgUnaryAddOp:          unparseUnaryOperator(expr, "+", info);   break;
          case V_SgMinusOp:             unparseUnaryOperator(expr, "-", info);   break;
          case V_SgNotOp:               unparseUnaryOperator(expr, "NOT ", info);break;

          case V_SgPntrArrRefExp:       unparseArrayOp(expr, info);              break;

       // initializers
          case V_SgAssignInitializer:       unparseAssnInit     (expr, info);    break;
          case V_SgJovialTablePresetExp:    unparseTablePreset  (expr, info);    break;
          case V_SgJovialPresetPositionExp: unparsePresetPos    (expr, info);    break;
          case V_SgReplicationOp:           unparseReplicationOp(expr, info);    break;

          case V_SgNullExpression:                                               break;

          default:
             std::cout << "error: unparseExpression() is unimplemented for " << expr->class_name() << std::endl;
             ROSE_ABORT();
       }

    // unparse comments after the expression
    unparseCommentsAfter(expr, info);
  }

void
UnparseJovial::unparseJovialBitVal(SgExpression* expr, SgUnparse_Info &)
  {
     SgJovialBitVal* bitval = isSgJovialBitVal(expr);
     ASSERT_not_null(bitval);
     curprint(bitval->get_valueString());
  }

void
UnparseJovial::unparseStringVal(SgExpression* expr, SgUnparse_Info &)
  {
     SgStringVal* string_val = isSgStringVal(expr);
     ASSERT_not_null(string_val);

  // Add quotes back to the string (removed from string coming from parser)
     std::string quoted_string = "'" + string_val->get_value() + "'";
     curprint(quoted_string);
  }

void
UnparseJovial::unparseAssignOp(SgExpression* expr, SgUnparse_Info &info)
  {
     SgAssignOp* op = isSgAssignOp(expr);
     ASSERT_not_null(op);

     SgExpression* lhs = op->get_lhs_operand();
     SgExpression* rhs = op->get_rhs_operand();

     unparseExpression(lhs, info);

  // An assignment statement may have multiple variables
     while (SgAssignOp* assign_op = isSgAssignOp(rhs))
        {
           curprint(",");
           unparseExpression(assign_op->get_lhs_operand(), info);
           rhs = assign_op->get_rhs_operand();
        }

     curprint(" = ");
     unparseExpression(rhs, info);
     curprint(";");
  }

void
UnparseJovial::unparseBinaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);
     ninfo.set_operator_name(op);
     unparseBinaryExpr(expr, ninfo);
   }

void
UnparseJovial::unparseUnaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);
     ninfo.set_operator_name(op);

     bool need_parens = expr->get_need_paren();

     if (need_parens) curprint("(");

     unparseUnaryExpr(expr, ninfo);

     if (need_parens) curprint(")");
   }

//----------------------------------------------------------------------------
//  cast expr
//----------------------------------------------------------------------------
void
UnparseJovial::unparseCastExp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgCastExp* cast_expr = isSgCastExp(expr);
     ASSERT_not_null(cast_expr);

     SgType* type = cast_expr->get_type();
     ASSERT_not_null(type);

     SgExpression* size = type->get_type_kind();

  // If there is a size it won't be SgModifierType, SgTypedefType or SgJovialBitType
     if (size) {
        curprint("(* ");
     }

     switch(type->variantT()) {
        // Note fall through
        case V_SgTypeInt:
        case V_SgTypeUnsignedInt:
        case V_SgTypeChar:
        case V_SgTypeFloat:
           unparseType(type, info);
           break;

        case V_SgPointerType: {
           SgPointerType* pointer = isSgPointerType(type);
           if (SgTypedefType* typedef_type = isSgTypedefType(pointer->get_base_type())) {
             curprint("(* P ");
             unparseType(typedef_type, info);
             curprint(" *)");
           }
           else {
             unparseType(type, info);
           }
           break;
        }

        case V_SgJovialBitType:
        case V_SgModifierType:
        case V_SgTypeFixed:
        case V_SgTypeString:
           curprint("(* ");
           unparseType(type, info);
           curprint(" *)");
           break;

        case V_SgEnumType:
           curprint("(* ");
           unparseJovialType(isSgEnumType(type), info);
           curprint(" *)");
           break;
        case V_SgJovialTableType: {
           auto tableType = isSgJovialTableType(type);
           curprint("(* ");
           curprint(tableType->get_name());
           curprint(" *)");
           break;
        }
        case V_SgTypedefType:
           curprint("(* ");
           unparseJovialType(isSgTypedefType(type), info);
           curprint(" *)");
           break;

        default:
           std::cout << "error: unparseCastExp() is unimplemented for " << type->class_name() << std::endl;
           ROSE_ABORT();
     }

     if (size) {
        curprint(" *)");
     }

     SgExpression* operand_i = cast_expr->get_operand_i();
     ASSERT_not_null(operand_i);
     curprint("(");
     unparseExpression(operand_i, info);
     curprint(")");
   }

//----------------------------------------------------------------------------
//  Table/array subscripts
//----------------------------------------------------------------------------

void
UnparseJovial::unparseSubscriptExpr(SgExpression* expr, SgUnparse_Info& info)
   {
     SgSubscriptExpression* sub_expr = isSgSubscriptExpression(expr);
     ASSERT_not_null(sub_expr);

     ASSERT_not_null(sub_expr->get_lowerBound());
     ASSERT_not_null(sub_expr->get_upperBound());
     ASSERT_not_null(sub_expr->get_stride());

     if (isSgNullExpression(sub_expr->get_lowerBound()) == NULL)
        {
       // Since the lower bound is not an SgNullExpression, there is a lower bound
          unparseExpression(sub_expr->get_lowerBound(), info);
          curprint(":");
        }

     if (isSgNullExpression(sub_expr->get_upperBound()) == NULL)
        {
          unparseExpression(sub_expr->get_upperBound(), info);
        }

     SgExpression* strideExpression = sub_expr->get_stride();
     ASSERT_not_null(strideExpression);

     SgIntVal* integerValue = isSgIntVal(strideExpression);
     ASSERT_not_null(integerValue);

  // See if this is the default value for the stride (unit stride) and skip the output in this case.
     bool defaultValue = (integerValue->get_value() == 1) ? true : false;
     if (defaultValue == false)
        {
          curprint(":");
          unparseExpression(sub_expr->get_stride(), info);
        }
   }

void
UnparseJovial::unparseArrayOp(SgExpression* expr, SgUnparse_Info &info)
   {
  // Sage node corresponds to array indicing
     SgPntrArrRefExp* arrayRefExp = isSgPntrArrRefExp(expr);

     unparseExpression(arrayRefExp->get_lhs_operand(),info);

     SgUnparse_Info ninfo(info);
     ninfo.set_SkipParen();

     curprint("(");
     unparseExpression(arrayRefExp->get_rhs_operand(),ninfo);
     curprint(")");
   }

void
UnparseJovial::unparseAsteriskShapeExpr(SgExpression* expr, SgUnparse_Info &)
   {
     ASSERT_not_null( isSgAsteriskShapeExp(expr));
     curprint("*");
   }

void
UnparseJovial::unparseFuncCall(SgExpression* expr, SgUnparse_Info &info)
   {
      SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
      ASSERT_not_null(func_call);

      SgFunctionRefExp* func_ref = isSgFunctionRefExp(func_call->get_function());
      ASSERT_not_null(func_ref);

      SgFunctionSymbol* func_symbol = isSgFunctionSymbol(func_ref->get_symbol());
      ASSERT_not_null(func_symbol);

      SgFunctionDeclaration* func_decl = func_symbol->get_declaration();
      SgInitializedNamePtrList& formal_params = func_decl->get_parameterList()->get_args();
      SgExpressionPtrList& actual_params = func_call->get_args()->get_expressions();

      ROSE_ASSERT(formal_params.size() == actual_params.size());

   // function name
      unparseExpression(func_call->get_function(), info);

   // unparse arguments individually to separate the input and output parameters
   //
      bool firstOutParam = false;
      bool foundOutParam = false;

      curprint("(");

      int i = 0;
      for (SgInitializedName* arg : formal_params)
        {
           if (arg->get_storageModifier().isMutable() && foundOutParam == false)
              {
                 firstOutParam = true;
                 foundOutParam = true;
                 curprint(":");
              }

           // Don't output comma if this is the first out parameter
           if (i > 0 && firstOutParam == false) curprint(",");
           firstOutParam = false;

        // curprint(arg->get_name());
           unparseExpression(actual_params[i++], info);
        }
      curprint(")");
   }

void
UnparseJovial::unparseOverlayExpr(SgExprListExp* overlay, SgUnparse_Info& info)
  {
     ASSERT_not_null(overlay);

  // The OverlayExpression consists of a list of OverlayString expressions.
  // An OverlayString consists of a list of OverlayElement expressions.
  // Presently, the following convention is used for an OverlayElement:
  //   1. Spacer is a SgExpression;
  //   2. DataName is a SgVarRefExp
  //   3. OverlayExpression is a SgExprListExp

     SgExpression* spacer;
     SgVarRefExp* data_name;
     SgExprListExp* overlay_expr;

     int n = overlay->get_expressions().size();
     for (SgExpression* expr : overlay->get_expressions())
       {
          SgExprListExp* overlay_string_list = isSgExprListExp(expr);
          ASSERT_not_null(overlay_string_list);

          int ns = overlay_string_list->get_expressions().size();
          for (SgExpression* overlay_string : overlay_string_list->get_expressions())
             {
               if ( (overlay_expr = isSgExprListExp(overlay_string)) != NULL)
                  {
                     curprint("(");
                     unparseOverlayExpr(overlay_expr, info);
                     curprint(")");
                  }
               else if ( (data_name = isSgVarRefExp(overlay_string)) != NULL)
                  {
                     unparseExpression(data_name, info);
                  }
               else
                  {
                     spacer = overlay_string;
                     curprint("W ");
                     unparseExpression(spacer, info);
                  }
               if (--ns > 0) curprint(", ");
             }
          if (--n > 0) curprint(": ");
       }
  }


//----------------------------------------------------------------------------
//  ::<symbol references>
//----------------------------------------------------------------------------

void
UnparseJovial::unparseFuncRef(SgExpression* expr, SgUnparse_Info &)
   {
      SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
      ASSERT_not_null(func_ref);

      std::string func_name = func_ref->get_symbol()->get_name().str();
      curprint(func_name);
   }

void
UnparseJovial::unparseLabelRef(SgExpression* expr, SgUnparse_Info &)
   {
     SgLabelRefExp* labelRef = isSgLabelRefExp(expr);

     ASSERT_not_null(labelRef);
     ASSERT_not_null(labelRef->get_symbol());

     curprint(labelRef->get_symbol()->get_name().str());
   }

void
UnparseJovial::unparseVarRef(SgExpression* expr, SgUnparse_Info &)
   {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);

     ASSERT_not_null(var_ref);
     ASSERT_not_null(var_ref->get_symbol());

     curprint(var_ref->get_symbol()->get_name().str());
   }

void
UnparseJovial::unparsePtrDeref(SgExpression* expr, SgUnparse_Info &info)
   {
     SgPointerDerefExp* deref = isSgPointerDerefExp(expr);
     ASSERT_not_null(deref);

     SgExpression* operand = deref->get_operand();
     ASSERT_not_null(operand);

     switch (operand->variantT())
        {
        case V_SgAtOp:
           curprint("@");
           unparseExpression(operand, info);
           break;
        case V_SgVarRefExp:
           curprint("@");
           unparseVarRef(operand, info);
           break;
        case V_SgPntrArrRefExp: {
           // Unparse SgPntrArrRefExp, explicitly handling parens locally
           auto ptr = isSgPntrArrRefExp(operand);

           // Unparse dereference of pointer name
           curprint("@(");
           unparseExpression(ptr->get_lhs_operand(), info);
           curprint(")");

           // Unparse table subscript
           curprint("(");
           unparseExpression(ptr->get_rhs_operand(), info);
           curprint(")");
           break;
        }
        case V_SgCastExp:
           curprint("@ (");
           unparseCastExp(operand, info);
           curprint(")");
           break;
        default:
           std::cout << "error: unparsePtrDeref() is unimplemented for " << operand->class_name() << "\n";
           ROSE_ABORT();
        }
   }

void
UnparseJovial::unparseTypeExpr(SgExpression* expr, SgUnparse_Info &)
   {
     SgTypeExpression* type_expr = isSgTypeExpression(expr);
     ASSERT_not_null(type_expr);

     SgType* type = type_expr->get_type();
     ASSERT_not_null(type);

     SgName name;

     switch (type->variantT())
        {
        case V_SgJovialTableType:
           {
              name = isSgJovialTableType(type)->get_name();
              break;
           }
        case V_SgEnumType:
           {
              name = isSgEnumType(type)->get_name();
              break;
           }
        case V_SgTypedefType:
           {
              name = isSgTypedefType(type)->get_name();
              break;
           }
        default:
           std::cout << "error: unparseTypeExpr() is unimplemented for " << type->class_name() << "\n";
           ROSE_ABORT();
        }

     curprint(name);
   }

//----------------------------------------------------------------------------
//  initializers
//----------------------------------------------------------------------------

void
UnparseJovial::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
{
  SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
  ASSERT_not_null(assn_init);

  unparseExpression(assn_init->get_operand(), info);

  // unparse comments after the expression
  const AttachedPreprocessingInfoType* preprocInfo = assn_init->get_attachedPreprocessingInfoPtr();
  if (preprocInfo) {
    for (PreprocessingInfo* info : *preprocInfo) {
      if (info->getRelativePosition() == PreprocessingInfo::after) {
        curprint(info->getString());
      }
    }
  }
}

void
UnparseJovial::unparseTablePreset(SgExpression* expr, SgUnparse_Info& info)
  {
     SgJovialTablePresetExp* table_preset = isSgJovialTablePresetExp(expr);
     ASSERT_not_null(table_preset);

     SgExprListExp* presets = table_preset->get_preset_list();
     ASSERT_not_null(presets);

     if (table_preset->get_need_paren()) curprint("(");

  // Unparse the preset list individually (to get parentheses/precedence proper)
     bool first = true;
     for (SgExpression* preset : presets->get_expressions())
       {
         if (first) first = false;
         else curprint(",");
         if (preset->variantT() != V_SgNullExpression) // otherwise "()" is printed
           // Unparsing comments will probably be moved to C++ eventually
           unparseCommentsBefore(preset, info);
           unparseExpression(preset, info);
           unparseCommentsAfter(preset, info);
       }

     if (table_preset->get_need_paren()) curprint(")");
  }

void
UnparseJovial::unparsePresetPos(SgExpression* expr, SgUnparse_Info& info)
  {
     SgJovialPresetPositionExp* pos_preset = isSgJovialPresetPositionExp(expr);
     ASSERT_not_null(pos_preset);

     SgExprListExp* indices = pos_preset->get_indices();
     SgExpression* value = pos_preset->get_value();

     ASSERT_not_null(indices);
     ASSERT_not_null(value);

  // Unparse the position
     curprint("POS(");
     unparseExpression(indices, info);
     curprint("):");

  // Unparse the value
     unparseExpression(value, info);
  }

void
UnparseJovial::unparseReplicationOp(SgExpression* expr, SgUnparse_Info& info)
  {
     SgReplicationOp* rep_op = isSgReplicationOp(expr);
     ASSERT_not_null(rep_op);

     unparseExpression(rep_op->get_lhs_operand(), info);
     curprint("(");
     unparseExpression(rep_op->get_rhs_operand(), info);
     curprint(")");
  }

//----------------------------------------------------------------------------
//  Table dimension list
//----------------------------------------------------------------------------

void
UnparseJovial::unparseDimInfo(SgExprListExp* dim_info, SgUnparse_Info& info)
   {
      ASSERT_not_null(dim_info);

   // If this is a scalar don't print the parens
      if (dim_info->get_expressions().size() < 1) return;

      bool first = true;
      curprint("(");

      for (SgExpression* expr : dim_info->get_expressions())
         {
            if (first) first = false;
            else       curprint(",");

            if (isSgSubscriptExpression(expr))
               {
                  unparseSubscriptExpr(expr, info);
               }
            else if (isSgAsteriskShapeExp(expr))
               {
                  unparseAsteriskShapeExpr(expr, info);
               }
            else
               {
                  unparseExpression(expr, info);
               }
         }

      curprint(") ");
   }

void
UnparseJovial::unparseCommentsBefore(SgExpression* expr, SgUnparse_Info &)
{
  // unparse comments before the expression
  const AttachedPreprocessingInfoType* preprocInfo = expr->get_attachedPreprocessingInfoPtr();
  if (preprocInfo) {
    for (PreprocessingInfo* info : *preprocInfo) {
      if (info->getRelativePosition() == PreprocessingInfo::before) {
        curprint(" ");
        curprint(info->getString());
      }
    }
  }
}

void
UnparseJovial::unparseCommentsAfter(SgExpression* expr, SgUnparse_Info &)
{
  // unparse comments after the expression
  const AttachedPreprocessingInfoType* preprocInfo = expr->get_attachedPreprocessingInfoPtr();
  if (preprocInfo) {
    for (PreprocessingInfo* info : *preprocInfo) {
      auto pos = info->getRelativePosition();
      if (pos == PreprocessingInfo::end_of || pos == PreprocessingInfo::after) {
        curprint(" ");
        curprint(info->getString());
      }
    }
  }
}

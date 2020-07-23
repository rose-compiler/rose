/*
 * unparseJovial_expressions.C
 */
#include "sage3basic.h"
#include "unparser.h"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#ifdef _MSC_VER
#include "Cxx_Grammar.h"
#endif


void Unparse_Jovial::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info) 
   {
      ASSERT_not_null(expr);

    // Check if this expression requires parentheses.  If so, process the opening parentheses now.
    //
    AstIntAttribute *parenthesis_attribute = (AstIntAttribute *) expr->getAttribute("x10-parentheses-count");
    if (parenthesis_attribute) { // Output the left paren
        for (int i = 0; i < parenthesis_attribute -> getValue(); i++) {
            curprint("(");
        }
    }

    switch (expr->variantT())
       {
       // function, intrinsic calls
          case V_SgFunctionCallExp:     unparseFuncCall(expr, info);             break;

       // expressions
          case V_SgSubscriptExpression: unparseSubscriptExpr(expr, info);        break;
          case V_SgAsteriskShapeExp:    unparseAsteriskShapeExpr(expr, info);    break;

       // symbol references
          case V_SgFunctionRefExp:      unparseFuncRef    (expr, info);          break;
          case V_SgVarRefExp:           unparseVarRef     (expr, info);          break;
          case V_SgPointerDerefExp:     unparsePtrDeref   (expr, info);          break;
          case V_SgTypeExpression:      unparseTypeExpr   (expr, info);          break;

       // operators
          case V_SgUnaryOp:             unparseUnaryExpr  (expr, info);          break;
          case V_SgBinaryOp:            unparseBinaryExpr (expr, info);          break;
          case V_SgCastExp:             unparseCastExp    (expr, info);          break;

          case V_SgAssignOp:            unparseAssignOp   (expr, info);          break;

          case V_SgAddOp:               unparseBinaryOperator(expr, "+", info);  break;
          case V_SgSubtractOp:          unparseBinaryOperator(expr, "-", info);  break;
          case V_SgMultiplyOp:          unparseBinaryOperator(expr, "*", info);  break;
          case V_SgDivideOp:            unparseBinaryOperator(expr, "/", info);  break;
          case V_SgModOp:               unparseBinaryOperator(expr,"MOD",info);  break;
          case V_SgExponentiationOp:    unparseBinaryOperator(expr,"**", info);  break;
          case V_SgLessThanOp:          unparseBinaryOperator(expr, "<", info);  break;
          case V_SgLessOrEqualOp:       unparseBinaryOperator(expr,"<=", info);  break;
          case V_SgGreaterThanOp:       unparseBinaryOperator(expr, ">", info);  break;
          case V_SgGreaterOrEqualOp:    unparseBinaryOperator(expr,">=", info);  break;
          case V_SgEqualityOp:          unparseBinaryOperator(expr, "=", info);  break;
          case V_SgNotEqualOp:          unparseBinaryOperator(expr,"<>", info);  break;
          case V_SgBitAndOp:            unparseBinaryOperator(expr,"AND", info); break;
          case V_SgBitOrOp:             unparseBinaryOperator(expr,"OR", info);  break;
          case V_SgBitXorOp:            unparseBinaryOperator(expr,"XOR", info); break;

          case V_SgUnaryAddOp:          unparseUnaryOperator(expr, "+", info);   break;
          case V_SgMinusOp:             unparseUnaryOperator(expr, "-", info);   break;
          case V_SgNotOp:               unparseUnaryOperator(expr, "NOT ", info);break;


          case V_SgPntrArrRefExp:       unparseArrayOp(expr, info);              break;

       // initializers
          case V_SgAssignInitializer:    unparseAssnInit    (expr, info);        break;
          case V_SgJovialTablePresetExp: unparseTablePreset (expr, info);        break;

          case V_SgNullExpression:                                               break;

          default:
             std::cout << "error: unparseExpression() is unimplemented for " << expr->class_name() << std::endl;
             ROSE_ASSERT(false);
             break;
       }

    // If this expression requires closing parentheses, emit them now.
    //
    if (parenthesis_attribute)
       {
       // Output the right parentheses
          for (int i = 0; i < parenthesis_attribute -> getValue(); i++) {
             curprint(")");
          }
       }
   }

void
Unparse_Jovial::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
  {
     SgStringVal* string_val = isSgStringVal(expr);
     ASSERT_not_null(string_val);
     curprint(string_val->get_value());
  }

void
Unparse_Jovial::unparseAssignOp(SgExpression* expr, SgUnparse_Info& info) 
  {
     SgBinaryOp* op = isSgBinaryOp(expr);
     ASSERT_not_null(op);

     unparseExpression(op->get_lhs_operand(), info);
     curprint(" = ");
     unparseExpression(op->get_rhs_operand(), info);
     curprint(";");
  }

void
Unparse_Jovial::unparseBinaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);
     ninfo.set_operator_name(op);
     unparseBinaryExpr(expr, ninfo);
   }

void
Unparse_Jovial::unparseUnaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);
     ninfo.set_operator_name(op);
     unparseUnaryExpr(expr, ninfo);
   }

//----------------------------------------------------------------------------
//  cast expr
//----------------------------------------------------------------------------
void
Unparse_Jovial::unparseCastExp(SgExpression* expr, SgUnparse_Info& info)
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

     switch(type->variantT())
        {
     // Note fall through
        case V_SgTypeInt:
        case V_SgTypeUnsignedInt:
        case V_SgTypeChar:
        case V_SgTypeFloat:
        case V_SgPointerType:
           unparseType(type, info);
           break;

        case V_SgJovialBitType:
        case V_SgModifierType:
           curprint("(* ");
           unparseType(type, info);
           curprint(" *)");
           break;
        case V_SgTypedefType:
           curprint("(* ");
           unparseJovialType(isSgTypedefType(type), info);
           curprint(" *)");
           break;
        default:
           std::cout << "error: unparseCastExp() is unimplemented for " << type->class_name() << std::endl;
           ROSE_ASSERT(false);
           break;
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
Unparse_Jovial::unparseSubscriptExpr(SgExpression* expr, SgUnparse_Info& info) 
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
Unparse_Jovial::unparseArrayOp(SgExpression* expr, SgUnparse_Info& info)
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
Unparse_Jovial::unparseAsteriskShapeExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
     ASSERT_not_null( isSgAsteriskShapeExp(expr));

     curprint("*");
   }

void
Unparse_Jovial::unparseFuncCall(SgExpression* expr, SgUnparse_Info& info)
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

   // argument list
      SgUnparse_Info ninfo(info);
      curprint("(");
      if (func_call->get_args()) {
         SgInitializedNamePtrList::iterator formal = formal_params.begin();
         SgExpressionPtrList::iterator actual = actual_params.begin();

         bool firstOutParam = false;
         bool foundOutParam = false;

         while (actual != actual_params.end()) {

         // TODO - Change temporary hack of using storage modifier isMutable to represent an out parameter
            if ((*formal)->get_storageModifier().isMutable() && foundOutParam == false)
               {
                  firstOutParam = true;
                  foundOutParam = true;
                  curprint(":");
               }
            formal++;

            unparseExpression((*actual), ninfo);
            actual++;
            if (actual != actual_params.end() && firstOutParam == false) {
               curprint(",");
            }
         }
      }
      curprint(")");
   }

void
Unparse_Jovial::unparseOverlayExpr(SgExprListExp* overlay, SgUnparse_Info& info)
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
     foreach(SgExpression* expr, overlay->get_expressions())
       {
          SgExprListExp* overlay_string_list = isSgExprListExp(expr);
          ASSERT_not_null(overlay_string_list);

          int ns = overlay_string_list->get_expressions().size();
          foreach(SgExpression* overlay_string, overlay_string_list->get_expressions())
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
Unparse_Jovial::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
      SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
      ASSERT_not_null(func_ref);

      std::string func_name = func_ref->get_symbol()->get_name().str();
      curprint(func_name);
   }

void
Unparse_Jovial::unparseVarRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);

     ASSERT_not_null(var_ref);
     ASSERT_not_null(var_ref->get_symbol());

     curprint(var_ref->get_symbol()->get_name().str());
   }

void
Unparse_Jovial::unparsePtrDeref(SgExpression* expr, SgUnparse_Info& info)
   {
     SgPointerDerefExp* deref = isSgPointerDerefExp(expr);
     ASSERT_not_null(deref);

     SgExpression* operand = deref->get_operand();
     ASSERT_not_null(operand);

     switch (operand->variantT())
        {
        case V_SgVarRefExp:
           curprint("@");
           unparseVarRef(operand, info);
           break;
        default:
           std::cout << "error: unparsePtrDeref() is unimplemented for " << operand->class_name() << "\n";
           ROSE_ASSERT(false);
           break;
        }
   }

void
Unparse_Jovial::unparseTypeExpr(SgExpression* expr, SgUnparse_Info& info)
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
           ROSE_ASSERT(false);
           break;
        }

     curprint(name);
   }

//----------------------------------------------------------------------------
//  initializers
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
  {
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ASSERT_not_null(assn_init);

     unparseExpression(assn_init->get_operand(), info);
  }

void
Unparse_Jovial::unparseTablePreset(SgExpression* expr, SgUnparse_Info& info)
  {
     SgJovialTablePresetExp* table_preset = isSgJovialTablePresetExp(expr);
     ASSERT_not_null(table_preset);

     SgExprListExp* default_sublist = table_preset->get_default_sublist();
     SgExprListExp* specified_sublist = table_preset->get_specified_sublist();

     ASSERT_not_null(default_sublist);
     ASSERT_not_null(specified_sublist);

     bool has_specified_sublist = (specified_sublist->get_expressions().size() == 2);

  // Unparse the optional DefaultPresetSublist
     if (default_sublist->get_expressions().size() > 0)
        {
           unparseExpression(default_sublist, info);
           if (has_specified_sublist) curprint(", ");
        }

     if (has_specified_sublist)
        {
        // Unparse the PresetIndexSpecifier
           curprint("POS(");
           unparseExpression(specified_sublist->get_expressions()[0], info);
           curprint("): ");

        // Unparse the PresetValuesOption
           unparseExpression(specified_sublist->get_expressions()[1], info);
        }
  }

//----------------------------------------------------------------------------
//  Table dimension list
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseDimInfo(SgExprListExp* dim_info, SgUnparse_Info& info)
   {
      ASSERT_not_null(dim_info);

   // If this is a scalar don't print the parens
      if (dim_info->get_expressions().size() < 1) return;

      bool first = true;
      curprint("(");

      BOOST_FOREACH(SgExpression* expr, dim_info->get_expressions())
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

## Do the following after running build_classes Fortran.sig.finished
#
#  In junk_traverse.hpp do:
#
1. Change typename to type_name in setTypeName (3 locations)
2. Remove class Expr from generated class header file.
3. Remove class IntrinsicOperator from generated class header file.

## SgUntypedNode usage
#
1. ofp_traverse_Icon, setPayload(value)
      char * Icon_val;
      SgUntypedValueExpression* value = NULL;
      value = new SgUntypedValueExpression(Icon_val);
      printf("ROSE Icon: .......................... %s\n", value->get_value().c_str());

2. ofp_traverse_DigitString: inheritPayload(DigitString->getIcon())
      SgUntypedValueExpression* value = dynamic_cast<SgUntypedValueExpression*>(DigitString->getPayload());
      printf("ROSE DigitString: ................... %s\n", value->get_value().c_str());

3. ofp_traverse_KindParam: inheritPayload(KindParam->getDigitString())
      SgUntypedValueExpression* kind = dynamic_cast<SgUntypedValueExpression*>(KindParam->getPayload());
      printf("ROSE KindParam: ..................... %s\n", kind->get_value().c_str());

4. ofp_traverse_IntLiteralConstant: inheritPayload(IntLiteralConstant->getDigitString())
      SgUntypedValueExpression* value = dynamic_cast<SgUntypedValueExpression*>(IntLiteralConstant->getPayload());
      printf("ROSE IntLiteralConstant: ............ %s\n", value->get_value().c_str());

      ## if kind is present
      OFP::KindParam*         kindParam  = IntLiteralConstant->getKindParam();
      SgUntypedValueExpression* kindExpr = dynamic_cast<SgUntypedValueExpression*>(kindParam->getPayload());
      printf("ROSE IntLiteralConstant(kind): ...... %s\n", kindExpr->get_value().c_str());



# changes after running build_classes Fortran.sig.finished

1. typename -> type_name

...

N. copy #ifdef NEEDS_LIST classes from OFPNodes.h to junk_traverse.hpp

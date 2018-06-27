#include <rosetollvm/ConstantCodeEvaluator.h>


/**
 *
 */
ConstantValue ConstantCodeEvaluator::evaluateVariableReference(SgVarRefExp *ref) {
    ConstantValue value;

    SgVariableSymbol *sym = ref -> get_symbol();
    ROSE2LLVM_ASSERT(sym);
    SgInitializedName *decl = sym -> get_declaration();
    ROSE2LLVM_ASSERT(decl);

    SgModifierType *ref_type = isSgModifierType(ref -> get_type());
    if (ref_type && ref_type -> get_typeModifier().get_constVolatileModifier().isConst()) { // check if this variable was assigned to a constant.
        // We know that the var value is const, so get the initialized name and evaluate it
        SgInitializer *initializer = decl -> get_initializer();
                                                                                 
        if (initializer && isSgAssignInitializer(initializer)) {
            SgExpression *rhs = isSgAssignInitializer(initializer) -> get_operand();
            ConstantExpressionEvaluator variableEval(attributes);
            value = variableEval.traverse(rhs);
        }
    }
    else {
        ROSE2LLVM_ASSERT(decl -> getAttribute(Control::LLVM_NAME));
        string var_name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_NAME)) -> getValue();
        ROSE2LLVM_ASSERT(decl -> get_type());
        SgType *var_type = decl -> get_type() -> stripTypedefsAndModifiers();
        if (isSgAddressOfOp(ref -> get_parent())) { // are we going to take the address of this variable?
            value.setOtherExpression(ref, var_name);
        }
        else if (isSgArrayType(var_type)) {
            // If the declaration itself was tagged with a type (because of its initializer), use that type.
            // Otherwise, use the the specified type.
            string type_name = (decl -> attributeExists(Control::LLVM_TYPE) 
                                      ? ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                                      : ((StringAstAttribute *) var_type -> getAttribute(Control::LLVM_TYPE)) -> getValue());

            // For an array, take the address of the array's first element.
            value.setOtherExpression(ref, "getelementptr inbounds (" + type_name + "," + type_name + "* " + var_name + ", i32 0, i32 0)");
        }
        else if (isSgClassType(var_type)) {
            // This case is necessary in case the address of a
            // member of this struct or union is computed by
            // ancestor nodes.
            value.setOtherExpression(ref, var_name);
        }
        else {
            // A variable of a primitive, enum, or pointer type
            // cannot appear in a global initializer unless its
            // address is being computed because loads are not
            // allowed.
            // ROSE2LLVM_ASSERT(! "yet know how to deal with this kind of variable");
        }
    }

    return value;
}

/**
 *
 */
ConstantValue ConstantCodeEvaluator::evaluateSynthesizedAttribute(SgNode *some_node, SynthesizedAttributesList synList) {
    SgExpression *node = isSgExpression(some_node);
    ROSE2LLVM_ASSERT(node);

    //
    // The value that will be returned.
    //
    ConstantValue value; // by default, a ConstantValue has no value

    if (isSgValueExp(node)) {
        value = this -> getValueExpressionValue(isSgValueExp(node));
    }
    else if (isSgNullExpression(node)) {
        value.setIntValue(0);
    }
    else if (isSgVarRefExp(node)) {
        value = evaluateVariableReference(isSgVarRefExp(node));
    }
    else if (isSgSizeOfOp(node)) {
        SgSizeOfOp *sizeof_exp = (SgSizeOfOp *) node;
        IntAstAttribute *size_attribute = (IntAstAttribute *) sizeof_exp -> getAttribute(Control::LLVM_SIZE);
        ROSE2LLVM_ASSERT(size_attribute);
        value.setIntValue(size_attribute -> getValue());
    }
    else if (isSgUnaryOp(node)) {
        assert(synList.size() == 1);

        if (isSgCastExp(node)) {
            SgCastExp *cast_expression = isSgCastExp(node);
            // C99 explicitly states that "a cast does not yield an
            // lvalue" and that "the operand of the unary & operator
            // shall be either a function designator, the result of a []
            // or unary * operator, or an lvalue that designates an
            // object that is not a bit-field and is not declared with
            // the register storage-class specifier".  Hopefully ROSE
            // never inserts an implicit SgCastExp as the child of an
            // SgAddressOfOp.
            ROSE2LLVM_ASSERT(! isSgAddressOfOp(cast_expression -> get_parent()));
            SgType *result_type =  ((SgTypeAstAttribute *) cast_expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
            SgExpression *operand = cast_expression -> get_operand();
            SgType *operand_type = ((SgTypeAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
            if (isSgPointerType(result_type)) {
                // FIXME: So far, the only trees I've found where the
                // operand is not an SgAddressOfOp are corrupt trees
                // from ROSE.  For example,
                // https://mailman.nersc.gov/pipermail/rose-public/2010-December/000568.html.
                // Does this properly handle operands other than
                // SgAddressOfOp?

                //
                // TODO: remove the first case below that checks for LLVM_NULL_VALUE.
                //       This is now be calculated properly in this method. The assertion below
                //       in effect demonstrates that this code is unnecessary.
                //
                if (node -> attributeExists(Control::LLVM_NULL_VALUE)) {
                    ROSE2LLVM_ASSERT(synList.at(0).hasIntValue() && synList.at(0).int_value == 0);
                    value.setOtherExpression(node, "null");
                }
                else if (isSgPointerType(operand_type)) {
                    string operand_llvm_type = ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                    string operand_name = synList.at(0).getCode();
                    string result_llvm_type = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                    value.setOtherExpression(node, "bitcast (" + operand_llvm_type + " " + operand_name + " to " + result_llvm_type + ")");
                }
                else if (isSgFunctionType(operand_type)) {
                    string operand_llvm_type = ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue() + "*";
                    string operand_name = synList.at(0).getCode(); // ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                    string result_llvm_type = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

                    if (operand_llvm_type.compare(result_llvm_type) == 0) {
                        value.setOtherExpression(node, operand_name);
                    }
                    else {
                        value.setOtherExpression(node, "bitcast (" + operand_llvm_type + " " + operand_name + " to " + result_llvm_type + ")");
                    }
                }
                else if (synList.at(0).hasIntValue()) {
                    ROSE2LLVM_ASSERT(attributes -> isIntegerType(operand_type));

                    if (synList.at(0).int_value == 0) {
                        value.setOtherExpression(node, "null");
                    }
                    else  {
                        value.setOtherExpression(node, "inttoptr ("
                                                     + ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                                                     + " "
                                                     + Control::IntToString(synList.at(0).int_value) // ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue()
                                                     + " to "
                                                     + ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                                                     + ")");
                    }
                }
                else if (isSgTypeString(operand_type)) {
                    // Keep the original Control::LLVM_EXPRESSION_RESULT_NAME.
                    // TODO: Can this happen?  How do I set up the value for this?
                    value.setOtherExpression(node, ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue());
                }
                else if (isSgArrayType(operand_type)) {
                    value.setOtherExpression(node, "getelementptr inbounds (i8, i8* "
                                                   + synList.at(0).getCode() // ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue()
                                                   + ")");
                }
                else {
                    ROSE2LLVM_ASSERT(! "This should not happen");
                }
            }
            else if (isSgPointerType(operand_type) && attributes -> isIntegerType(result_type)) {
                // Even though C99 appears to say that, after folding, a
                // cast from pointer to integer is not allowed in a
                // global init, gcc and ROSE accept it, so handle it.
                string operand_llvm_type = ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                string operand_name = synList.at(0).getCode();
                string result_llvm_type = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                value.setOtherExpression(node, "ptrtoint (" + operand_llvm_type + " " + operand_name + " to " + result_llvm_type + ")");
            }
            else if (isSgFunctionType(operand_type) && attributes -> isIntegerType(result_type)) {
                // Even though C99 appears to say that, after folding, a
                // cast from pointer to integer is not allowed in a
                // global init, gcc and ROSE accept it, so handle it.
                string operand_llvm_type = ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue() + "*";
                string operand_name = synList.at(0).getCode(); // ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                string result_llvm_type = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                value.setOtherExpression(node, "ptrtoint (" + operand_llvm_type + " " + operand_name + " to " + result_llvm_type + ")");
            }
            else if (isSgTypeFloat(result_type)) {
                if (synList.at(0).hasIntValue()) {
                    value.setFloatValue((float) synList.at(0).int_value);
                }
                else if (synList.at(0).hasFloatValue()) {
                    value = synList.at(0);
                }
                else if (synList.at(0).hasDoubleValue()) {
                    value.setFloatValue((float) synList.at(0).double_value);
                }
                else if (synList.at(0).hasLongDoubleValue()) {
                    value.setFloatValue((float) synList.at(0).long_double_value);
                }
            }
            else if (isSgTypeDouble(result_type)) {
                if (synList.at(0).hasIntValue()) {
                    value.setDoubleValue((double) synList.at(0).int_value);
                }
                else if (synList.at(0).hasFloatValue()) {
                    value.setDoubleValue((double) synList.at(0).float_value);
                }
                else if (synList.at(0).hasDoubleValue()) {
                    value = synList.at(0);
                }
                else if (synList.at(0).hasLongDoubleValue()) {
                    value.setDoubleValue((double) synList.at(0).long_double_value);
                }
            }
            else if (isSgTypeLongDouble(result_type)) {
                if (synList.at(0).hasIntValue()) {
                    value.setLongDoubleValue((long double) synList.at(0).int_value);
                }
                else if (synList.at(0).hasFloatValue()) {
                    value.setLongDoubleValue((long double) synList.at(0).float_value);
                }
                else if (synList.at(0).hasDoubleValue()) {
                    value.setLongDoubleValue((long double) synList.at(0).double_value);
                }
                else if (synList.at(0).hasLongDoubleValue()) {
                    value = synList.at(0);
                }
            }
            else if (attributes -> isIntegerType(result_type)) { // isIntegerType includes isBooleanType.
                if (synList.at(0).hasFloatValue()) {
                    value.setIntValue((long long) synList.at(0).float_value);
                }
                else if (synList.at(0).hasDoubleValue()) {
                    value.setIntValue((long long) synList.at(0).double_value);
                }
                else if (synList.at(0).hasLongDoubleValue()) {
                    value.setIntValue((long long) synList.at(0).long_double_value);
                }
                else if (synList.at(0).hasIntValue()) {
                    value = synList.at(0);
                }
                else if (! synList.at(0).hasArithmeticValue()) {
                    //
                    // If this is a conversion to the Boolean type and the operand is not an arithmetic value...
                    //
                    if (attributes -> isBooleanType(result_type)) {
                        if (isSgPointerType(operand_type) || isSgTypeString(operand_type)) {
                            value.setIntValue(1); // Except for the constant NULL, all pointer values are positive.
                        }
                        else {
                            cout << ";*** Don't know yet how to cast node " << node -> class_name()
                                 << " of type " << result_type -> class_name()
                                 << endl;
                            cout.flush();
                            ROSE2LLVM_ASSERT(false);
                        }
                    }
                }
            }
            else {
                cerr << "Don't know how to cast from type "
                     << attributes -> getExpressionType(cast_expression -> get_operand()) -> class_name()
                     << " to type " 
                     << result_type -> class_name()
                     << "; The source type is "
                     << node -> get_type() -> class_name()
                     << endl;
                cerr.flush();
                ROSE2LLVM_ASSERT(false);
            }
        }
        else if (isSgUnaryAddOp(node)) {
            value = synList.at(0);
        }
        else if (isSgMinusOp(node)) {
            if (synList.at(0).hasIntValue()) {
                value.setIntValue(- synList.at(0).int_value);
            }
            else if (synList.at(0).hasFloatValue()) {
                value.setFloatValue(- synList.at(0).float_value);
            }
            else if (synList.at(0).hasDoubleValue()) {
                value.setDoubleValue(- synList.at(0).double_value);
            }
            else if (synList.at(0).hasLongDoubleValue()) {
                value.setLongDoubleValue(- synList.at(0).long_double_value);
            }
        }
        else if (isSgBitComplementOp(node)) {
            if (synList.at(0).hasIntValue()) {
                value.setIntValue(~ synList.at(0).int_value);
            }
        }
        else if (isSgNotOp(node)) {
            if (synList.at(0).hasIntValue()) {
                value.setIntValue(! synList.at(0).int_value);
            }
        }
        else if (isSgAddressOfOp(node)) {
            //
            // TODO: Generate address dereference code here!!!
            //
            SgExpression *operand = isSgAddressOfOp(node) -> get_operand();
            // Every node type's handler is responsible for checking if
            // its meaning is modified by a parent SgAddressOfOp, so we
            // just copy that result here.  This approach seems to be
            // encouraged by C99, which describes cases in which &
            // operating on the result of a * or [] simply prevents part
            // of the effect of the * or [].  However, C99 does not
            // allow applying & to a & result, so assert that does not
            // happen.
            value.setOtherExpression(node, synList.at(0).getCode());
        }
        else if (isSgPointerDerefExp(node)) {
            SgPointerDerefExp *n = isSgPointerDerefExp(node); 
            SgExpression *operand = n -> get_operand();
            string indexes_string = ", i32 0, i32 0";
            while (isSgPointerDerefExp(operand)) {
                operand = isSgPointerDerefExp(operand) -> get_operand();
                indexes_string += ", i32 0, i32 0";
            } 
            SgVarRefExp *var_ref = isSgVarRefExp(operand);
            ROSE2LLVM_ASSERT(var_ref);
            ROSE2LLVM_ASSERT(var_ref -> get_symbol());
            SgInitializedName *decl = var_ref -> get_symbol() -> get_declaration();
            ROSE2LLVM_ASSERT(decl);
            string name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_GLOBAL_CONSTANT_NAME)) -> getValue();
            string type_name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_TYPE)) -> getValue(); 

/**/        value.setOtherExpression(node, "getelementptr inbounds (" + type_name + ", " + type_name + "* " + name + indexes_string + ")");
        }
        else {
            std::cerr << "We don't yet evaluate integer binary Expression " << node -> class_name() << std::endl;
        }
    }
    else if (isSgBinaryOp(node)) {
        assert(synList.size() == 2);

        if (synList.at(0).hasArithmeticValue() && synList.at(1).hasArithmeticValue()) {
            SgType *result_type =  ((SgTypeAstAttribute *) node -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
            if (isSgTypeFloat(result_type)) {
                float left = (synList.at(0).hasIntValue()
                                     ? (float) synList.at(0).int_value
                                     : synList.at(0).hasFloatValue()
                                              ? (float) synList.at(0).float_value
                                              : synList.at(0).hasDoubleValue()
                                                       ? (float) synList.at(0).double_value
                                                       : (float) synList.at(0).long_double_value);
                float right = (synList.at(1).hasIntValue()
                                     ? (float) synList.at(1).int_value
                                     : synList.at(1).hasFloatValue()
                                              ? (float) synList.at(1).float_value
                                              : synList.at(1).hasDoubleValue()
                                                       ? (float) synList.at(1).double_value
                                                       : (float) synList.at(1).long_double_value);
                if (isSgAddOp(node)) {
                    value.setFloatValue(left + right);
                }
                else if (isSgSubtractOp(node)) {
                    value.setFloatValue(left - right);
                }
                else if (isSgMultiplyOp(node)) {
                    value.setFloatValue(left * right);
                }
                else if (isSgDivideOp(node)) {
                    value.setFloatValue(left / right);
                }
                else if (isSgEqualityOp(node)) {
                    value.setIntValue(left == right);
                }
                else if (isSgNotEqualOp(node)) {
                    value.setIntValue(left != right);
                }
                else if (isSgLessThanOp(node)) {
                    value.setIntValue(left < right);
                }
                else if (isSgLessOrEqualOp(node)) {
                    value.setIntValue(left <= right);
                }
                else if (isSgGreaterThanOp(node)) {
                    value.setIntValue(left > right);
                }
                else if (isSgGreaterOrEqualOp(node)) {
                    value.setIntValue(left >= right);
                }
                else {
                    std::cerr << "We don't yet evaluate floating-point binary Expression " << node -> class_name() << std::endl;
                }
            }
            else if (isSgTypeDouble(result_type)) {
                double left = (synList.at(0).hasIntValue()
                                     ? (double) synList.at(0).int_value
                                     : synList.at(0).hasFloatValue()
                                              ? (double) synList.at(0).float_value
                                              : synList.at(0).hasDoubleValue()
                                                       ? (double) synList.at(0).double_value
                                                       : (double) synList.at(0).long_double_value);
                double right = (synList.at(1).hasIntValue()
                                     ? (double) synList.at(1).int_value
                                     : synList.at(1).hasFloatValue()
                                              ? (double) synList.at(1).float_value
                                              : synList.at(1).hasDoubleValue()
                                                       ? (double) synList.at(1).double_value
                                                       : (double) synList.at(1).long_double_value);
                if (isSgAddOp(node)) {
                    value.setDoubleValue(left + right);
                }
                else if (isSgSubtractOp(node)) {
                    value.setDoubleValue(left - right);
                }
                else if (isSgMultiplyOp(node)) {
                    value.setDoubleValue(left * right);
                }
                else if (isSgDivideOp(node)) {
                    value.setDoubleValue(left / right);
                }
                else if (isSgEqualityOp(node)) {
                    value.setIntValue(left == right);
                }
                else if (isSgNotEqualOp(node)) {
                    value.setIntValue(left != right);
                }
                else if (isSgLessThanOp(node)) {
                    value.setIntValue(left < right);
                }
                else if (isSgLessOrEqualOp(node)) {
                    value.setIntValue(left <= right);
                }
                else if (isSgGreaterThanOp(node)) {
                    value.setIntValue(left > right);
                }
                else if (isSgGreaterOrEqualOp(node)) {
                    value.setIntValue(left >= right);
                }
                else {
                    std::cerr << "We don't yet evaluate floating-point binary Expression " << node -> class_name() << std::endl;
                }
            }
            else if (isSgTypeLongDouble(result_type)) {
                long double left = (synList.at(0).hasIntValue()
                                     ? (long double) synList.at(0).int_value
                                     : synList.at(0).hasFloatValue()
                                              ? (long double) synList.at(0).float_value
                                              : synList.at(0).hasDoubleValue()
                                                       ? (long double) synList.at(0).double_value
                                                       : (long double) synList.at(0).long_double_value);
                long double right = (synList.at(1).hasIntValue()
                                     ? (long double) synList.at(1).int_value
                                     : synList.at(1).hasFloatValue()
                                              ? (long double) synList.at(1).float_value
                                              : synList.at(1).hasDoubleValue()
                                                       ? (long double) synList.at(1).double_value
                                                       : (long double) synList.at(1).long_double_value);
                if (isSgAddOp(node)) {
                    value.setLongDoubleValue(left + right);
                }
                else if (isSgSubtractOp(node)) {
                    value.setLongDoubleValue(left - right);
                }
                else if (isSgMultiplyOp(node)) {
                    value.setLongDoubleValue(left * right);
                }
                else if (isSgDivideOp(node)) {
                    value.setLongDoubleValue(left / right);
                }
                else if (isSgEqualityOp(node)) {
                    value.setIntValue(left == right);
                }
                else if (isSgNotEqualOp(node)) {
                    value.setIntValue(left != right);
                }
                else if (isSgLessThanOp(node)) {
                    value.setIntValue(left < right);
                }
                else if (isSgLessOrEqualOp(node)) {
                    value.setIntValue(left <= right);
                }
                else if (isSgGreaterThanOp(node)) {
                    value.setIntValue(left > right);
                }
                else if (isSgGreaterOrEqualOp(node)) {
                    value.setIntValue(left >= right);
                }
                else {
                    std::cerr << "We don't yet evaluate floating-point binary Expression " << node -> class_name() << std::endl;
                }
            }
            else {
                long long left = (synList.at(0).hasIntValue()
                                     ? (long long) synList.at(0).int_value
                                     : synList.at(0).hasFloatValue()
                                              ? (long long) synList.at(0).float_value
                                              : synList.at(0).hasDoubleValue()
                                                       ? (long long) synList.at(0).double_value
                                                       : (long long) synList.at(0).long_double_value);
                long long right = (synList.at(1).hasIntValue()
                                     ? (long long) synList.at(1).int_value
                                     : synList.at(1).hasFloatValue()
                                              ? (long long) synList.at(1).float_value
                                              : synList.at(1).hasDoubleValue()
                                                       ? (long long) synList.at(1).double_value
                                                       : (long long) synList.at(1).long_double_value);
                if (isSgModOp(node)) {
                    value.setIntValue(left % right);
                }
                else if (isSgAddOp(node)) {
                    value.setIntValue(left + right);
                }
                else if (isSgSubtractOp(node)) {
                    value.setIntValue(left - right);
                }
                else if (isSgMultiplyOp(node)) {
                    value.setIntValue(left * right);
                }
                else if (isSgDivideOp(node)) {
                    value.setIntValue(left / right);
                }
                else if (isSgEqualityOp(node)) {
                    value.setIntValue(left == right);
                }
                else if (isSgNotEqualOp(node)) {
                    value.setIntValue(left != right);
                }
                else if (isSgLessThanOp(node)) {
                    value.setIntValue(left < right);
                }
                else if (isSgLessOrEqualOp(node)) {
                    value.setIntValue(left <= right);
                }
                else if (isSgGreaterThanOp(node)) {
                    value.setIntValue(left > right);
                }
                else if (isSgGreaterOrEqualOp(node)) {
                    value.setIntValue(left >= right);
                }
                else if (isSgOrOp(node)) {
                    value.setIntValue(left || right);
                }
                else if (isSgAndOp(node)) {
                    value.setIntValue(left && right);
                }
                else if (isSgBitOrOp(node)) {
                    value.setIntValue(left | right);
                }
                else if (isSgBitXorOp(node)) {
                    value.setIntValue(left ^ right);
                }
                else if (isSgBitAndOp(node)) {
                    value.setIntValue(left & right);
                }
                else if (isSgLshiftOp(node)) {
                    value.setIntValue(left << right);
                }
                else if (isSgRshiftOp(node)) {
                    value.setIntValue(left >> right);
                }
                else {
                    std::cerr << "We don't yet evaluate integer binary Expression " << node -> class_name() << std::endl;
                }
            }
        }
        else if (isSgPntrArrRefExp(node)) {
            // In a folded constant expression, we assume the LHS of an
            // SgPntrArrRefExp is always a reference to an array (or
            // equivalent pointer), and we assume the RHS is always a
            // constant integer.  FIXME: Does ROSE guarantee all that?
            SgPntrArrRefExp *n = isSgPntrArrRefExp(node);
            SgExpression *array_pointer,
                         *array_index;
            string array_name,
                   index_name;
            if (attributes -> getExpressionType(n -> get_rhs_operand()) -> isIntegerType()) {
                array_pointer = n -> get_lhs_operand();
                array_index = n -> get_rhs_operand();
                array_name = synList.at(0).getCode();
                ROSE2LLVM_ASSERT(synList.at(1).hasIntValue());
                index_name = Control::IntToString(synList.at(1).int_value);
            }
            else  {
                array_pointer = n -> get_rhs_operand();
                array_index = n -> get_lhs_operand();
                array_name = synList.at(1).getCode();
                ROSE2LLVM_ASSERT(synList.at(0).hasIntValue());
                index_name = Control::IntToString(synList.at(0).int_value);
            }
            ROSE2LLVM_ASSERT(isSgVarRefExp(array_pointer) || isSgPntrArrRefExp(array_pointer));
            ROSE2LLVM_ASSERT(isSgValueExp(array_index));
            SgType *array_type = attributes -> getExpressionType(array_pointer);
            SgType *index_type = attributes -> getExpressionType(array_index);
            string array_type_name;
            if (isSgArrayType(array_type)) { // Most likely this will not occur as the CodeAttributeVisitor would have already transformed array types into pointer types.
                array_type_name = ((StringAstAttribute*) isSgArrayType(array_type) -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue() + "*";
            }
            else {
                ROSE2LLVM_ASSERT(isSgPointerType(array_type));
                array_type_name = ((StringAstAttribute *) array_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
            }
            ROSE2LLVM_ASSERT(attributes -> isIntegerType(index_type));
            string index_type_name = ((StringAstAttribute*) index_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

            ROSE2LLVM_ASSERT(array_type_name.length() > 1 && array_type_name[array_type_name.length() - 1] == '*');
// TODO: Remove this !!!
/*
cout << "; The array_type_name is: " << array_type_name.substr(0, array_type_name.length() - 1)
     << "; the pointer is: " << array_type_name
     << endl;
cout.flush();
*/
            value.setOtherExpression(node, "getelementptr inbounds (" + array_type_name.substr(0, array_type_name.length() - 1) + "," + array_type_name + " " + array_name + ", " + index_type_name + " " + index_name + (! isSgAddressOfOp(n -> get_parent()) ? ", i32 0)" : ")"));

        }
        else if (isSgAddOp(node)) {
            //
            // 08/10/2016
            // Make sure that we are only dealing with array dereferences here
            //
            if (isSgPointerType(attributes -> getExpressionType(node))) {
                // The only arithmetic that should appear in a folded global
                // initializer is on an address.  Rose always makes sure
                // that the pointer type is the left operand. It transforms
                // the original source if that was not the case.  Thus,
                // consider the following example:
                //
                //   int a[] = { 0, 1, 2, 3, 4};
                //   int *q;
                //
                //   q = &a[3];
                //   q = a + 3;
                //   q = 3 + a;
                //
                // In all 3 cases above, the AST node generated will
                // correspond to: q = a + 3.
                SgAddOp *n = isSgAddOp(node);
                // C99 does not permit taking the address of a temporary
                // arithmetic result.  Unfortunately, ROSE sometimes
                // generates such trees
                // (https://mailman.nersc.gov/pipermail/rose-public/2010-December/000568.html),
                // but we will wait for that to be fixed rather than
                // attempting to make sense of it.
                ROSE2LLVM_ASSERT(!isSgAddressOfOp(n->get_parent()));
                SgExpression *lhs_operand = n -> get_lhs_operand(),
                             *rhs_operand = n -> get_rhs_operand();
                string lhs_name = synList.at(0).getCode(); // ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                ROSE2LLVM_ASSERT(synList.at(1).hasIntValue());
                string rhs_name = Control::IntToString(synList.at(1).int_value);
                SgType *lhs_type = attributes -> getExpressionType(lhs_operand),
                       *rhs_type = attributes -> getExpressionType(rhs_operand);
                string lhs_type_name;
                if (isSgArrayType(lhs_type)) {
                    lhs_type_name = ((StringAstAttribute*)isSgArrayType(lhs_type)->get_base_type()->getAttribute(Control::LLVM_TYPE))->getValue() + "*";
                }
                else {
                    ROSE2LLVM_ASSERT(isSgPointerType(lhs_type));
                    lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                }
                ROSE2LLVM_ASSERT(attributes -> isIntegerType(rhs_type));
                string rhs_type_name = ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

                ROSE2LLVM_ASSERT(lhs_type_name.length() > 1 && lhs_type_name[lhs_type_name.length() - 1] == '*');
// TODO: Remove this !!!
/*
cout << "; The lhs_type_name is: " << lhs_type_name.substr(0, lhs_type_name.length() - 1)
     << "; the pointer is: " << lhs_type_name
     << endl;
cout.flush();
*/
                value.setOtherExpression(node, "getelementptr inbounds (" + lhs_type_name.substr(0, lhs_type_name.length() - 1) + ", " + lhs_type_name + " " + lhs_name + ", " + rhs_type_name + " " + rhs_name + ")");
            }
        }
        else if (isSgSubtractOp(node)) {
            //
            // 08/10/2016
            // Make sure that we are only dealing with array dereferences here
            //
            if (isSgPointerType(attributes -> getExpressionType(node))) {
                // The only arithmetic that should appear in a folded global
                // initializer is on an address. For example:
                //
                //   int a[] = { 0, 1, 2, 3, 4};
                //   int *q;
                //   q = a - 3;
                //
                SgSubtractOp *n = isSgSubtractOp(node);
                // C99 does not permit taking the address of a temporary
                // arithmetic result.  Unfortunately, ROSE sometimes
                // generates such trees
                // (https://mailman.nersc.gov/pipermail/rose-public/2010-December/000568.html),
                // but we will wait for that to be fixed rather than
                // attempting to make sense of it.
                ROSE2LLVM_ASSERT(! isSgAddressOfOp(n -> get_parent()));
                SgExpression *lhs_operand = n -> get_lhs_operand(),
                             *rhs_operand = n -> get_rhs_operand();
                string lhs_name = synList.at(0).getCode(); // ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();

                ROSE2LLVM_ASSERT(synList.at(1).hasIntValue());
                string rhs_name = Control::IntToString(- synList.at(1).int_value); // Since we are going to add (instead of subtracting) this value to the pointer, reverse its sign.
                SgType *lhs_type = attributes -> getExpressionType(lhs_operand),
                       *rhs_type = attributes -> getExpressionType(rhs_operand);
                string lhs_type_name;
                if (isSgArrayType(lhs_type)) {
                    lhs_type_name = ((StringAstAttribute*) isSgArrayType(lhs_type) -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue() + "*";
                }
                else {
                    ROSE2LLVM_ASSERT(isSgPointerType(lhs_type));
                    lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                }
                ROSE2LLVM_ASSERT(attributes -> isIntegerType(rhs_type));
                string rhs_type_name = ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

                ROSE2LLVM_ASSERT(lhs_type_name.length() > 1 && lhs_type_name[lhs_type_name.length() - 1] == '*');
// TODO: Remove this !!!
/*
cout << "; The lhs_type_name is: " << lhs_type_name.substr(0, lhs_type_name.length() - 1)
     << "; the pointer is: " << lhs_type_name
     << endl;
cout.flush();
*/
                value.setOtherExpression(node, "getelementptr inbounds (" + lhs_type_name.substr(0, lhs_type_name.length() - 1) + ", " + lhs_type_name + " " + lhs_name + ", " + rhs_type_name + " " + rhs_name + ")");
            }
        }
        else if (isSgArrowExp(node) || isSgDotExp(node)) {
            // So far, the only way I've found to get ROSE to produce an
            // SgArrowExp in a tree is when a load must be performed to
            // obtain the pointer value.  Otherwise, it converts it to
            // an SgDotExp.  For example, in "p->a", p's value must be
            // loaded, so ROSE produces an SgArrowExp.  However, in
            // "(&s)->a", only the address of s is needed, and ROSE
            // converts this to "s.a".  Thus, because a constant
            // expression can't load from memory, it appears there's no
            // way for the SgArrowExp case below to be exercised.
            // However, ROSE might evolve, so I include the SgArrowExp
            // case anyway.
            SgBinaryOp *n = isSgBinaryOp(node);
            SgExpression *lhs_operand = n -> get_lhs_operand(),
                         *rhs_operand = n -> get_rhs_operand();
            string lhs_name = synList.at(0).getCode(); // ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
            SgType *lhs_type = attributes -> getExpressionType(lhs_operand);
            SgType *result_type = attributes -> getExpressionType(n);
            string lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
            string result_type_name = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
            SgType *base_type = lhs_type;
            if (isSgArrowExp(n)) {
                base_type = isSgPointerType(lhs_type) -> get_base_type();
            }
            SgClassType *class_type = isSgClassType(base_type);
            SgClassDeclaration *decl= isSgClassDeclaration(class_type -> get_declaration());
            if (isSgDotExp(n)) {
                lhs_type_name += "*";
            }

            //
            // This code is here for documentation and sanity check. See else segment below.
            //
            if (isSgAddressOfOp(n -> get_parent())) {
                // FIXME: I have so far found no test case for which the
                // field type (i.e., result_type) is an array and the
                // parent is an SgAddressOfOp.  Instead, ROSE transforms
                // the tree so it does not have the SgArrowExp or
                // SgDotExp, and sometimes it also corrupts the tree
                // (similar to
                // https://mailman.nersc.gov/pipermail/rose-public/2010-December/000568.html).
            }
            else if (isSgArrayType(result_type)) {
                // Take the address of the array's first element.
            }
            else if (isSgClassType(result_type)) {
                // This case is necessary in case the address of a
                // member of this struct or union is computed by
                // ancestor nodes.
            }
            else {
                // A variable of a primitive, enum, or pointer type
                // cannot appear in a global initializer unless its
                // address is being computed because loads are not
                // allowed.
                ROSE2LLVM_ASSERT(! "yet know how to deal with this kind of field in global initializer");
            }

            //
            //
            //
            string out;
            if (decl -> get_class_type() == SgClassDeclaration::e_union) {
                out = "bitcast (" + lhs_type_name + " " + lhs_name + " to " + result_type_name + "*)";
            }
            else if (isSgCastExp(n -> get_parent()) && isSgArrayType(result_type)) {
                int index = ((IntAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_CLASS_MEMBER)) -> getValue();
                SgType *cast_result_type =  ((SgTypeAstAttribute *) isSgCastExp(n -> get_parent()) -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
                string cast_result_type_name = ((StringAstAttribute *) cast_result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                out = "bitcast (" + lhs_type_name + " " + lhs_name + " to " + cast_result_type_name + "), i64 " + Control::IntToString(index); // TODO: NEED the offset here and not the index !!!
            }
            else {
                int index = ((IntAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_CLASS_MEMBER)) -> getValue();

                ROSE2LLVM_ASSERT(lhs_type_name.length() > 1 && lhs_type_name[lhs_type_name.length() - 1] == '*');
// TODO: Remove this !!!
/*
cout << "; The lhs_type_name is: " << lhs_type_name.substr(0, lhs_type_name.length() - 1)
     << "; the pointer is: " << lhs_type_name
     << endl;
cout.flush();
*/
                out = "getelementptr inbounds (" + lhs_type_name.substr(0, lhs_type_name.length() - 1) + ", " + lhs_type_name + " " + lhs_name + ", i32 0, i32 " + Control::IntToString(index) + ")";
            }

            if ((! isSgAddressOfOp(n -> get_parent())) && (! isSgCastExp(n -> get_parent())) && isSgArrayType(result_type)) {
                out = "getelementptr inbounds (" + result_type_name + ", " + result_type_name + "* " + out + ", i32 0, i32 0)";
            }

            value.setOtherExpression(node, out);
        }
    }
    else if (isSgConditionalExp(node)) {
        assert(synList.size() == 3);
        ROSE2LLVM_ASSERT(synList.at(0).hasIntValue());
        value = (synList.at(0).int_value ? synList.at(1) : synList.at(2));
    }
    else if (isSgFunctionRefExp(node)) {
        // Keep the original Control::LLVM_EXPRESSION_RESULT_NAME.
        // C99 says that a function designator converts to a
        // function pointer implicitly except in two cases:
        // (1) when it appears as the operand of a sizeof, which is an
        // error that should be caught by ROSE, and (2) when it appears
        // as the operand of an &, which just performs the same
        // conversion explicitly.  Thus, there's no case where we need a
        // special handler for a parent SgAddressOfOp.

        SgFunctionRefExp *function_reference = isSgFunctionRefExp(node);
        string function_name = ((StringAstAttribute *) function_reference -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
        value.setFunctionReference(function_reference, function_name);
    }
    else {
        // TODO: Remove this !
        cout << ";*** We don't yet evaluate Expression " << node -> class_name()
             << " of type " << node -> get_type() -> class_name()
             << endl;
        cout.flush();

        value.setOtherExpression(node, "");
    } 

    //
    // Nodes with the LLVM_BOOLEAN_CAST attribute are nodes on which a boolean operation will be
    // performed that had not been explicitly cast into a boolean value by Rose.
    //
    if (node -> attributeExists(Control::LLVM_BOOLEAN_CAST)) {
        ROSE2LLVM_ASSERT(node -> attributeExists(Control::LLVM_EXPRESSION_RESULT_TYPE));
        SgType *type = ((SgTypeAstAttribute *) node -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
        if (isSgPointerType(type) || isSgTypeString(type)) { // a pointer address is always positive.
            value.setIntValue(1);
        }
        else if (value.hasFloatValue()) {
            value.setIntValue((long long) value.float_value);
        }
        else if (value.hasDoubleValue()) {
            value.setIntValue((long long ) value.double_value);
        }
        else if (value.hasLongDoubleValue()) {
            value.setIntValue((long long) value.long_double_value);
        }
        else if (! value.hasIntValue()) {
            cout << ";*** Don't know yet how to cast node " << node -> class_name()
                 << " of type " << node -> get_type() -> class_name()
                 << endl;
            cout.flush();
            ROSE2LLVM_ASSERT(false);
        }
    }

    return value;
}


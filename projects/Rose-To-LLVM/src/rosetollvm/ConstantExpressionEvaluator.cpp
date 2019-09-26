#include <rosetollvm/ConstantExpressionEvaluator.h>

/**
 *
 */
ConstantValue ConstantExpressionEvaluator::getValueExpressionValue(SgValueExp *valExp) {
    ConstantValue subtreeVal;
    if (isSgCharVal(valExp)) {
        subtreeVal.setIntValue(isSgCharVal(valExp) -> get_value());
    }
    else if (isSgIntVal(valExp)) {
        subtreeVal.setIntValue(isSgIntVal(valExp) -> get_value());
    }
    else if (isSgEnumVal(valExp)) {
        subtreeVal.setIntValue(isSgEnumVal(valExp) -> get_value());
    }
    else if (isSgLongIntVal(valExp)) {
        subtreeVal.setIntValue(isSgLongIntVal(valExp) -> get_value());
    }
    else if (isSgLongLongIntVal(valExp)) {
        subtreeVal.setIntValue(isSgLongLongIntVal(valExp) -> get_value());
    }
    else if (isSgShortVal(valExp)) {
       subtreeVal.setIntValue(isSgShortVal(valExp) -> get_value());
    }
    else if (isSgUnsignedIntVal(valExp)) {
        subtreeVal.setIntValue(isSgUnsignedIntVal(valExp) -> get_value());
    }
    else if (isSgUnsignedLongVal(valExp)) {
        subtreeVal.setIntValue(isSgUnsignedLongVal(valExp) -> get_value());
    } 
    else if (isSgUnsignedLongLongIntVal(valExp)) {
        subtreeVal.setIntValue(isSgUnsignedLongLongIntVal(valExp) -> get_value());
    } 
    else if (isSgUnsignedShortVal(valExp)) {
        subtreeVal.setIntValue(isSgUnsignedShortVal(valExp) -> get_value());
    }
    else if (isSgFloatVal(valExp)) {
        subtreeVal.setFloatValue(isSgFloatVal(valExp) -> get_value());
    }
    else if (isSgDoubleVal(valExp)) {
        subtreeVal.setDoubleValue(isSgDoubleVal(valExp) -> get_value());
    }
    else if (isSgLongDoubleVal(valExp)) {
        subtreeVal.setLongDoubleValue(isSgLongDoubleVal(valExp) -> get_value());
    }
    else if (isSgComplexVal(valExp)) {
        SgComplexVal *complex_val = isSgComplexVal(valExp);
        if (isSgTypeFloat(complex_val -> get_precisionType())) {
            ROSE2LLVM_ASSERT(isSgFloatVal(complex_val -> get_real_value()) && isSgFloatVal(complex_val -> get_imaginary_value()));
            float real_value = isSgFloatVal(complex_val -> get_real_value()) -> get_value(),
                  imaginary_value = isSgFloatVal(complex_val -> get_imaginary_value()) -> get_value();
            std::complex<float> value(real_value, imaginary_value);
            subtreeVal.setFloatComplexValue(value);
        }
        else if (isSgTypeDouble(complex_val -> get_precisionType())) {
            ROSE2LLVM_ASSERT(isSgDoubleVal(complex_val -> get_real_value()) && isSgDoubleVal(complex_val -> get_imaginary_value()));
            double real_value = isSgDoubleVal(complex_val -> get_real_value()) -> get_value(),
                   imaginary_value = isSgDoubleVal(complex_val -> get_imaginary_value()) -> get_value();
            std::complex<double> value(real_value, imaginary_value);
            subtreeVal.setDoubleComplexValue(value);
        }
        else if (isSgTypeLongDouble(complex_val -> get_precisionType())) {
            ROSE2LLVM_ASSERT(isSgLongDoubleVal(complex_val -> get_real_value()) && isSgLongDoubleVal(complex_val -> get_imaginary_value()));
            long double real_value = isSgLongDoubleVal(complex_val -> get_real_value()) -> get_value(),
                        imaginary_value = isSgLongDoubleVal(complex_val -> get_imaginary_value()) -> get_value();
            std::complex<long double> value(real_value, imaginary_value);
            subtreeVal.setLongDoubleComplexValue(value);
        }
        else {
            cout << "*** Not yet able to process Complex constant value with base type " << valExp -> class_name() << endl;
            cout.flush();
            ROSE2LLVM_ASSERT(0);
        }
    }
    else if (isSgStringVal(valExp)) {
        subtreeVal.setStringValue(isSgStringVal(valExp));
    }
    else if (isSgBoolValExp(valExp)) {
        subtreeVal.setIntValue(isSgBoolValExp(valExp) -> get_value() != 0 ? 1 : 0);
    }
    else {
        cout << "*** Not yet able to process constant value of type " << valExp -> class_name() << endl;
        cout.flush();
        ROSE2LLVM_ASSERT(0);
    }

    return subtreeVal;
}


/**
 *
 */
ConstantValue ConstantExpressionEvaluator::evaluateVariableReference(SgVarRefExp *ref) {
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
     
    return value;
}


/**
 *
 */
ConstantValue ConstantExpressionEvaluator::evaluateSynthesizedAttribute(SgNode *some_node, SynthesizedAttributesList synList) {
    SgExpression *node = isSgExpression(some_node);
    ROSE2LLVM_ASSERT(node);


    /**
     * Print debugging information, if requested.
     */
    if (attributes -> getOption().isDebugPostTraversal()) {
        cerr << "    Constant Visitor: "
             << ((unsigned long) node) << " " << ((unsigned long) node -> get_parent()) << " " 
             << node -> class_name() << endl;  // Used for Debugging
         if (isSgValueExp(node)) {
             SgValueExp *n = isSgValueExp(node);
             cerr << "    Constant is ===> " << n -> get_constant_folded_value_as_string()
             << endl; 
         }
         cerr.flush();
    }

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
            // C99 explicitly states that "a cast does not yield an
            // lvalue" and that "the operand of the unary & operator
            // shall be either a function designator, the result of a []
            // or unary * operator, or an lvalue that designates an
            // object that is not a bit-field and is not declared with
            // the register storage-class specifier".  Hopefully ROSE
            // never inserts an implicit SgCastExp as the child of an
            // SgAddressOfOp.
            SgType *result_type = ((SgTypeAstAttribute *) node -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
/*
SgType *operand_type = ((SgTypeAstAttribute *) isSgCastExp(node) -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
cout << "Casting constant expression from type  "
     << ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
     << " to type "
     << ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
     << endl;
cout.flush();
*/
            if (isSgTypeFloat(result_type)) {
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
            else if (isSgTypeComplex(result_type)) {
                SgType *base_type = isSgTypeComplex(result_type) -> get_base_type();
                if (isSgTypeFloat(base_type)) {
                    if (synList.at(0).hasIntValue()) {
                        value.setFloatComplexValue((std::complex<float>) synList.at(0).int_value);
                    }
                    else if (synList.at(0).hasFloatValue()) {
                        value.setFloatComplexValue((std::complex<float>) synList.at(0).float_value);
                    }
                    else if (synList.at(0).hasDoubleValue()) {
                        value.setFloatComplexValue((std::complex<float>)  synList.at(0).double_value);
                    }
                    else if (synList.at(0).hasLongDoubleValue()) {
                        value.setFloatComplexValue((std::complex<float>)  synList.at(0).long_double_value);
                    }
                    else if (synList.at(0).hasFloatComplexValue()) {
                        value = synList.at(0);
                    }
                    else if (synList.at(0).hasDoubleComplexValue()) {
                        value.setFloatComplexValue((std::complex<float>)  synList.at(0).double_complex_value);
                    }
                    else if (synList.at(0).hasLongDoubleComplexValue()) {
                        value.setFloatComplexValue((std::complex<float>)  synList.at(0).long_double_complex_value);
                    }
                }
                else if (isSgTypeDouble(base_type)) {
                    if (synList.at(0).hasIntValue()) {
                        value.setDoubleComplexValue((std::complex<double>) synList.at(0).int_value);
                    }
                    else if (synList.at(0).hasFloatValue()) {
                        value.setDoubleComplexValue((std::complex<double>) synList.at(0).float_value);
                    }
                    else if (synList.at(0).hasDoubleValue()) {
                        value.setDoubleComplexValue((std::complex<double>)  synList.at(0).double_value);
                    }
                    else if (synList.at(0).hasLongDoubleValue()) {
                        value.setDoubleComplexValue((std::complex<double>)  synList.at(0).long_double_value);
                    }
                    else if (synList.at(0).hasFloatComplexValue()) {
                        value.setDoubleComplexValue((std::complex<double>)  synList.at(0).float_complex_value);
                    }
                    else if (synList.at(0).hasDoubleComplexValue()) {
                        value = synList.at(0);
                    }
                    else if (synList.at(0).hasLongDoubleComplexValue()) {
                        value.setDoubleComplexValue((std::complex<double>)  synList.at(0).long_double_complex_value);
                    }
                }
                else if (isSgTypeLongDouble(base_type)) {
                    if (synList.at(0).hasIntValue()) {
                        value.setLongDoubleComplexValue((std::complex<long double>) synList.at(0).int_value);
                    }
                    else if (synList.at(0).hasFloatValue()) {
                        value.setLongDoubleComplexValue((std::complex<long double>) synList.at(0).float_value);
                    }
                    else if (synList.at(0).hasDoubleValue()) {
                        value.setLongDoubleComplexValue((std::complex<long double>)  synList.at(0).double_value);
                    }
                    else if (synList.at(0).hasLongDoubleValue()) {
                        value.setLongDoubleComplexValue((std::complex<long double>)  synList.at(0).long_double_value);
                    }
                    else if (synList.at(0).hasFloatComplexValue()) {
                        value.setLongDoubleComplexValue((std::complex<long double>)  synList.at(0).float_complex_value);
                    }
                    else if (synList.at(0).hasDoubleComplexValue()) {
                        value.setLongDoubleComplexValue((std::complex<long double>)  synList.at(0).double_complex_value);
                    }
                    else if (synList.at(0).hasLongDoubleComplexValue()) {
                        value = synList.at(0);
                    }
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
                        SgExpression *operand = isSgCastExp(node) -> get_operand();
                        SgType *operand_type = ((SgTypeAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
                        if (isSgPointerType(operand_type) || isSgTypeString(operand_type)) {
                            value.setIntValue(1); // Except for the constant NULL, all pointer values are positive.
                        }
                    }
                }
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
            else if (isSgTypeComplex(result_type)) {
                SgType *base_type = isSgTypeComplex(result_type) -> get_base_type();
                if (isSgTypeFloat(base_type)) {
                    ROSE2LLVM_ASSERT(synList.at(0).hasFloatComplexValue() && synList.at(1).hasFloatComplexValue());
                    std::complex<float> left = synList.at(0).float_complex_value,
                                        right = synList.at(1).float_complex_value;
                    if (isSgAddOp(node)) {
                        value.setFloatComplexValue(left + right);
                    }
                    else if (isSgSubtractOp(node)) {
                        value.setFloatComplexValue(left - right);
                    }
                    else if (isSgMultiplyOp(node)) {
                        value.setFloatComplexValue(left * right);
                    }
                    else if (isSgDivideOp(node)) {
                        value.setFloatComplexValue(left / right);
                    }
                    else if (isSgEqualityOp(node)) {
                        value.setIntValue(left == right);
                    }
                    else if (isSgNotEqualOp(node)) {
                        value.setIntValue(left != right);
                    }
                    else {
                        std::cerr << "We don't yet evaluate floating-point binary Expression " << node -> class_name() << std::endl;
                    }
                }
                else if (isSgTypeDouble(base_type)) {
                    ROSE2LLVM_ASSERT(synList.at(0).hasDoubleComplexValue() && synList.at(1).hasDoubleComplexValue());
                    std::complex<double> left = synList.at(0).double_complex_value,
                                         right = synList.at(1).double_complex_value;
                    if (isSgAddOp(node)) {
                        value.setDoubleComplexValue(left + right);
                    }
                    else if (isSgSubtractOp(node)) {
                        value.setDoubleComplexValue(left - right);
                    }
                    else if (isSgMultiplyOp(node)) {
                        value.setDoubleComplexValue(left * right);
                    }
                    else if (isSgDivideOp(node)) {
                        value.setDoubleComplexValue(left / right);
                    }
                    else if (isSgEqualityOp(node)) {
                        value.setIntValue(left == right);
                    }
                    else if (isSgNotEqualOp(node)) {
                        value.setIntValue(left != right);
                    }
                    else {
                        std::cerr << "We don't yet evaluate floating-point binary Expression " << node -> class_name() << std::endl;
                    }
                }
                else if (isSgTypeLongDouble(base_type)) {
                    ROSE2LLVM_ASSERT(synList.at(0).hasLongDoubleComplexValue() && synList.at(1).hasLongDoubleComplexValue());
                    std::complex<long double> left = synList.at(0).long_double_complex_value,
                                        right = synList.at(1).long_double_complex_value;
                    if (isSgAddOp(node)) {
                        value.setLongDoubleComplexValue(left + right);
                    }
                    else if (isSgSubtractOp(node)) {
                        value.setLongDoubleComplexValue(left - right);
                    }
                    else if (isSgMultiplyOp(node)) {
                        value.setLongDoubleComplexValue(left * right);
                    }
                    else if (isSgDivideOp(node)) {
                        value.setLongDoubleComplexValue(left / right);
                    }
                    else if (isSgEqualityOp(node)) {
                        value.setIntValue(left == right);
                    }
                    else if (isSgNotEqualOp(node)) {
                        value.setIntValue(left != right);
                    }
                    else {
                        std::cerr << "We don't yet evaluate floating-point binary Expression " << node -> class_name() << std::endl;
                    }
                }
            }
            else {
                ROSE2LLVM_ASSERT(attributes -> isIntegerType(result_type));
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
    }
    else if (isSgConditionalExp(node)) {
        assert(synList.size() == 3);
        if (synList.at(0).hasIntValue()) {
            value = (synList.at(0).int_value ? synList.at(1) : synList.at(2));
        }
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
        else if (! value.hasIntValue()) { // Should not happen !!!
            cout << ";*** Don't know yet how to cast node " << node -> class_name()
                 << " of type " << node -> get_type() -> class_name()
                 << endl;
            cout.flush();
            ROSE2LLVM_ASSERT(false);
        }
    }

    return value;
}

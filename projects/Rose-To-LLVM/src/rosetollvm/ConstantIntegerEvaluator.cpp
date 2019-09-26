#include <rosetollvm/ConstantIntegerEvaluator.h>

/**
 *
 */
ConstantValue ConstantIntegerEvaluator::getValueExpressionValue(SgValueExp *valExp) {
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
        subtreeVal.setIntValue(isSgFloatVal(valExp) -> get_value());
    }
    else if (isSgDoubleVal(valExp)) {
        subtreeVal.setIntValue(isSgDoubleVal(valExp) -> get_value());
    }
    else if (isSgLongDoubleVal(valExp)) {
        subtreeVal.setIntValue(isSgLongDoubleVal(valExp) -> get_value());
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
ConstantValue ConstantIntegerEvaluator::evaluateVariableReference(SgVarRefExp *ref) {
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
            ConstantIntegerEvaluator variableEval(attributes);
            value = variableEval.traverse(rhs);
        }
    }

    return value;
}

/**
 *
 */
ConstantValue ConstantIntegerEvaluator::evaluateSynthesizedAttribute(SgNode *some_node, SynthesizedAttributesList synList) {
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
        if (size_attribute && size_attribute -> getValue() > 0) {
            value.setIntValue(size_attribute -> getValue());
        }
    }
    else if (isSgUnaryOp(node)) {
        assert(synList.size() == 1);

        if (synList.at(0).hasIntValue()) {
            if (isSgCastExp(node)) {
                value = synList.at(0);
            }
            else if (isSgUnaryAddOp(node)) {
                value = synList.at(0);
            }
            else if (isSgMinusOp(node)) {
                value.setIntValue(- synList.at(0).int_value);
            }
            else if (isSgBitComplementOp(node)) {
                value.setIntValue(~ synList.at(0).int_value);
            }
            else if (isSgNotOp(node)) {
                value.setIntValue(! synList.at(0).int_value);
            }
        }
    }
    else if (isSgBinaryOp(node)) {
        assert(synList.size() == 2);

        if (synList.at(0).hasIntValue() && synList.at(1).hasIntValue()) {
            long long left = synList.at(0).int_value,
                      right = synList.at(1).int_value;
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
    else if (isSgConditionalExp(node)) {
        assert(synList.size() == 3);
        ROSE2LLVM_ASSERT(synList.at(0).hasIntValue());
        value = (synList.at(0).int_value ? synList.at(1) : synList.at(2));
    }

    return value;
}

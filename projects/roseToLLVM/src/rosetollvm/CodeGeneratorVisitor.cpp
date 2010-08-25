#include <rosetollvm/CodeGeneratorVisitor.h>
#include <iostream>

using namespace std;

/**
 * A pointer type initialized with NULL (int value of 0) ?
 */
void CodeGeneratorVisitor::processRemainingFunctions() {
    for (int k = 0; k < revisitAttributes.size(); k++) {
        setAttributes(revisitAttributes[k]); // Reset the correct environment

        for (int i = 0; i < attributes -> numAdditionalFunctions(); i++) {
            SgFunctionDeclaration *function_declaration = attributes -> getAdditionalFunction(i);
            if (function_declaration -> attributeExists(Control::LLVM_FUNCTION_NEEDS_REVISIT)) {
                if (function_declaration -> attributeExists(Control::LLVM_IGNORE)) {  // During the revisit don't ignore any function
                    control.RemoveAttribute(function_declaration, Control::LLVM_IGNORE);
                }
                control.RemoveAttribute(function_declaration, Control::LLVM_FUNCTION_NEEDS_REVISIT);  // each function should only be processed once.
                this -> traverse(function_declaration);
            }
        }
    }
}


/**
 * Rose has a function type -> isUnsignedType() that is supposed to yield the same result
 * as this function. However, it has a bug and does not include the type: unsigned long.
 */
bool CodeGeneratorVisitor::isUnsignedType(SgType *type) {
    return type -> isUnsignedType() || isSgTypeUnsignedLong(type);
}


/**
 * The type might be encapsulated in an SgModifierType.
 */
bool CodeGeneratorVisitor::isFloatType(SgType *type) {
    return attributes -> getSourceType(type) -> isFloatType();
}


/**
 * As initializations to global variables must be constant, they may require some conversion preprocessing.
 * 
 * TODO: COMPLETE THIS FUNCTION
 */
void CodeGeneratorVisitor::preprocessGlobalInitializer(SgExpression *initializer) {
    if (dynamic_cast<SgAggregateInitializer *>(initializer)) {
        SgAggregateInitializer *aggregate_init = isSgAggregateInitializer(initializer);
        vector<SgExpression *> elements = aggregate_init -> get_initializers() -> get_expressions();
        for (int i = 0; i < elements.size(); i++) {
            preprocessGlobalInitializer(elements[i]);
        }
    }
    else if (dynamic_cast<SgAssignInitializer *>(initializer)) {
        preprocessGlobalInitializer(isSgAssignInitializer(initializer) -> get_operand());
    }
    else {
        stringstream out;
        if (isSgCastExp(initializer)) {
            SgCastExp *cast_expression = isSgCastExp(initializer);
            SgType *result_type =  ((SgTypeAstAttribute *) cast_expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
            SgExpression *operand = cast_expression -> get_operand();
            SgType *operand_type = ((SgTypeAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
            if (isSgPointerType(result_type)) {
                if (initializer -> attributeExists(Control::LLVM_NULL_VALUE)) {
                    out << "null";
                }
                else if (isSgPointerType(operand_type)) {
                    out << "bitcast ("
                        << ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                        << " "
                        << ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue()
                        << " to "
                        << ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                        << ")";
                }
                else if (isSgTypeInt(operand_type) || isSgEnumType(operand_type)) {
                    if ((isSgIntVal(operand) && isSgIntVal(operand) -> get_value() == 0) ||
                        (isSgEnumVal(operand) && isSgEnumVal(operand) -> get_value() == 0)) {
                        out << "null";
                    }
                    else  {
                        out << "inttoptr (i64 "
                            << ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue()
                            << " to "
                            << ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                            << ")";
                    }
                }
            }
            else {
                // anything else?
            }
        }
        else if (dynamic_cast<SgFunctionRefExp *>(initializer)) {
            SgFunctionRefExp *function = isSgFunctionRefExp(initializer);
            // Nothing to do!
        }
        else if (isSgAddressOfOp(initializer)) {
            SgAddressOfOp *address_expression = isSgAddressOfOp(initializer);
            SgExpression *operand = address_expression -> get_operand();
            if (! isSgVarRefExp(operand)) {
                preprocessGlobalInitializer(operand);
            }
            out << ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
        }
        else if (isSgAddOp(initializer)) {
            SgAddOp *n = isSgAddOp(initializer);
            SgExpression *lhs_operand = n -> get_lhs_operand(),
                         *rhs_operand = n -> get_rhs_operand();
            preprocessGlobalInitializer(lhs_operand);
            preprocessGlobalInitializer(rhs_operand);
            SgType *type = attributes -> getSourceType(attributes -> getExpressionType(n));
            SgType *base_type = (isSgPointerType(type) ? isSgPointerType(type) -> get_base_type()
                                                       : isSgArrayType(type) ? isSgArrayType(type) -> get_base_type() 
                                                                             : NULL);
            if (base_type) {
                SgType *lhs_type = attributes -> getExpressionType(lhs_operand),
                       *rhs_type = attributes -> getExpressionType(rhs_operand);
                assert(rhs_type -> isIntegerType() || isSgEnumType(rhs_type));
                /**
                 * Note that Rose always makes sure that the pointer type is the left-operand. It transforms the original source if
                 * that was not the case.  Thus, consider the following example:
                 *
                 *    int a[] = { 0, 1, 2, 3, 4};
                 *    int *q;
                 *
                 *    q = &a[3];
                 *    q = a + 3;
                 *    q = 3 + a;
                 *
                 *   In all 3 cases above, the AST node generated will correspond to: q = a + 3.
                 */
                string cast_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue(),
                       rhs_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                       rhs_type_name = ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                       base_type_name = ((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                long element_size = ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                out << "bitcast (i8* getelementptr (i8* " << cast_name << ", " << rhs_type_name << " " << (atoi(rhs_name.c_str()) * element_size) << ") to " << base_type_name << "*)";
            }
            else {
                assert(! "yet know how to deal with addition");
            }
        }
        else if (isSgSubtractOp(initializer)) {
            assert(! "yet know how to deal with subtraction");
        }
        else if (isSgVarRefExp(initializer)) {
            SgVarRefExp *var = isSgVarRefExp(initializer);
            string result;
            SgVariableSymbol *sym = var -> get_symbol();
            assert(sym);
            SgInitializedName *decl = sym -> get_declaration();
            assert(decl);
            string var_name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                   type_name =  ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_TYPE)) -> getValue();
            /**
             * If we are dealing with an array aggregate, we need to load its address.
             */
            if (decl -> attributeExists(Control::LLVM_AGGREGATE) && isSgArrayType(attributes -> getExpressionType(var))) {
                out << "getelementptr (" << type_name << "* " << var_name << ", i32 0, i32 0)";
                stringstream cast;
                cast << "bitcast (" << type_name << "* " << var_name << " to i8*)";
                control.SetAttribute(initializer, Control::LLVM_BIT_CAST, new StringAstAttribute(cast.str()));
            }
            else assert(! "yet know how to deal with this kind of variable");
        }
        else if (isSgPntrArrRefExp(initializer)) {
            assert(! "yet know how to deal with array reference");
        }
        else if (isSgValueExp(initializer)) {
            out << ((StringAstAttribute *) initializer -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
        }
        else {
            cerr << "Don't know yet how to process initializer expression of type: " << initializer -> class_name() << endl;
            cerr.flush();
            assert(! "this should not happen!");
        }

        /**
         *
         */
        if (out.str().size()) { // a new name was computed
            StringAstAttribute *attribute = (StringAstAttribute *) initializer -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
            attribute -> resetValue(out.str());
        }
    }
}


/**
 * Generate global declarations.
 */
void CodeGeneratorVisitor::generateGlobals() {
    /**
     * Generate global declarations for string constants
     */
    for (int i = 0; i < attributes -> numStrings() ; i++) {
        const char *str = attributes -> getString(i);
        (*codeOut) << attributes -> getGlobalStringConstantName(i)
                    << " = internal constant [" << attributes -> getStringLength(i) << " x i8] c\"" << str << "\"" << endl;
    }

    /**
     * Process global variable declarations. It appears to be legal in LLVM to place the declaration of
     * the global variables after the funtion(s) that use them.
     */
    for (int i = 0; i < attributes -> numGlobalDeclarations(); i++) {
        if (dynamic_cast<SgClassDeclaration *>(attributes -> getGlobalDeclaration(i))) {
            SgClassDeclaration *class_decl = isSgClassDeclaration(attributes -> getGlobalDeclaration(i));
            DeclarationsAstAttribute *attribute = (DeclarationsAstAttribute *) class_decl -> getAttribute(Control::LLVM_LOCAL_DECLARATIONS);
            string class_type_name = ((StringAstAttribute *) attribute -> getClassType() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
            if (! class_decl -> get_definingDeclaration()) {
                (*codeOut) << class_type_name << " = type opaque" << endl;
            }
            else {
                (*codeOut) << class_type_name << " = type <{ ";
                if (class_decl -> get_class_type() == SgClassDeclaration::e_union) {
                    int size = ((IntAstAttribute *) attribute -> getClassType() -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                    (*codeOut) << "[" << size << " x i8]";
                }
                else {
                    DeclarationsAstAttribute *attribute = (DeclarationsAstAttribute *) class_decl -> getAttribute(Control::LLVM_LOCAL_DECLARATIONS);
                    for (int k = 0; k < attribute -> numSgInitializedNames(); k++) {
                        SgInitializedName *field_decl = attribute -> getSgInitializedName(k);
                        string field_type_name = ((StringAstAttribute *) field_decl -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                        if (field_decl -> attributeExists(Control::LLVM_STRUCTURE_PADDING)) {
                            int pad_size = ((IntAstAttribute *) field_decl -> getAttribute(Control::LLVM_STRUCTURE_PADDING)) -> getValue();
                            for (int j = 0; j < pad_size; j++) {
                                (*codeOut) << "i8, ";
                            }
                        }
                        (*codeOut) << field_type_name;
                        if (k + 1 < attribute -> numSgInitializedNames())
                            (*codeOut) << ", ";
                    }

                    /**
                     *
                     */
                    if (attribute -> getClassType() -> attributeExists(Control::LLVM_STRUCTURE_PADDING)) {
                        int pad_size = ((IntAstAttribute *) attribute -> getClassType() -> getAttribute(Control::LLVM_STRUCTURE_PADDING)) -> getValue();
			//                        if (pad_size < 3) {
                        for (int k = 0; k < pad_size; k++) {
                            (*codeOut) << ", i8";
                        }
			    //                        }
			    //                        else {
			    //                        (*codeOut) << ", [" << pad_size << " x i8]";
			    //			    }
                    }
                }
                (*codeOut) << " }>" << endl;
            }
        }
        else if (dynamic_cast<SgInitializedName *>(attributes -> getGlobalDeclaration(i))) {
            SgInitializedName *decl = isSgInitializedName(attributes -> getGlobalDeclaration(i));

            /**
             * Not a chosen global declaration? Ignore it...  Recall that both a definition and a declaration may be specified
             */
            string name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_NAME)) -> getValue();
            vector<SgInitializedName *> decls = attributes -> global_variable_declaration_map[name];
            if (decls.size() > 0 && decls[0] != decl) {
                continue;
            }

            SgType *var_type = decl -> get_type();
            string var_type_name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_TYPE)) -> getValue();

            SgInitializer *init = decl -> get_initializer();

            //
            // Note: Do not use decl -> get_storageModifier(); See SgStorageModifier documentation for detail.
            //
            SgStorageModifier &sm = decl -> get_declaration() -> get_declarationModifier().get_storageModifier();

            (*codeOut) << name << " = " << (sm.isStatic() ? "internal "
                                                          : sm.isExtern() ? "external "
                                                                          : init ? " "
                                                                                 : "common ") << "global " << var_type_name;

            /**
             * if this declaration is a local declaration and it has an initializer, then 
             * generate the initialization code here.
             */
            if (init) {

                preprocessGlobalInitializer(init);

                if (dynamic_cast<SgAssignInitializer *>(init)) {
                    SgAssignInitializer *assign_init = isSgAssignInitializer(init);
                    SgValueExp *literal = isValueInitializer(assign_init);
                    if (literal) {
                        SgStringVal *string_literal = isSgStringVal(assign_init -> get_operand());
                        if (string_literal) {
                            if (decl -> attributeExists(Control::LLVM_STRING_SIZE)) { 
                                int string_size = ((IntAstAttribute *) decl -> getAttribute(Control::LLVM_STRING_SIZE)) -> getValue(); 
                                (*codeOut) << " c\"" << attributes -> filter(string_literal -> get_value(), string_size) << "\"";
                            }
                            else {
                                (*codeOut) << " " << ((StringAstAttribute *) literal -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                            }
                        }
                        else {
                            string init_expr_name =  ((StringAstAttribute *) assign_init -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                            (*codeOut) << " " << init_expr_name;
                        }
                    }
                    else if (dynamic_cast<SgFunctionRefExp *>(assign_init -> get_operand())) {
                        SgFunctionRefExp *function = isSgFunctionRefExp(assign_init -> get_operand());
                        string function_name = ((StringAstAttribute *) function -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                               function_type_name = ((StringAstAttribute *) attributes -> getExpressionType(function) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                        (*codeOut) << " bitcast (" << function_type_name << "* " << function_name << " to " << var_type_name << ")";
                    }
                    else if (isSgPointerType(attributes -> getSourceType(var_type))) { // Initialization with a location. E.g., char str[20] = "abc", *str2 = str;
                        (*codeOut) << " " << ((StringAstAttribute *) assign_init -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                    }
                    else {
                        cerr << "Don't know yet how to process initializer expression of type: " << assign_init -> get_operand() -> class_name() << endl;
                        cerr.flush();
                        assert(0);
                    }
                }
                else if (dynamic_cast<SgAggregateInitializer *>(init)) {
                    SgAggregateInitializer *aggregate_init = isSgAggregateInitializer(init);
                    (*codeOut) << " ";
                    genGlobalAggregateInitialization(aggregate_init);
                }
            }
            else {
                if (! sm.isExtern()) { // non-extern vars must be initialized
                    (*codeOut) << " zeroinitializer";
                }
            }

            if (var_type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
                int alignment = ((IntAstAttribute *) var_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                (*codeOut) << ", align " << alignment;
            }

            (*codeOut) << endl;
        }
    }

    /**
     * Generate global declarations for functions
     */
    if (attributes -> needsMemcopy()) {
        (*codeOut) << "declare void @llvm.memcpy.i32(i8* nocapture, i8* nocapture, i32, i32) nounwind" << endl;
    }
    for (int i = 0; i < attributes -> numFunctions() ; i++) {
        if (! attributes -> isDefinedFunction(attributes -> getFunction(i))) {
            (*codeOut) << "declare " << attributes -> getFunction(i) << endl;
        }
    }

    return;
}


void CodeGeneratorVisitor::genGlobalAggregateInitialization(SgAggregateInitializer *aggregate) {
    AggregateAstAttribute *attribute = (AggregateAstAttribute *) aggregate -> getAttribute(Control::LLVM_AGGREGATE);
    SgArrayType *array_type = attribute -> getArrayType();
    SgClassType *class_type = attribute -> getClassType();
    assert(array_type || class_type);

    (*codeOut) << (array_type ? " [" : " <{ ");
    vector<SgExpression *> exprs = aggregate -> get_initializers() -> get_expressions();
    DeclarationsAstAttribute *class_attribute = (class_type ? attributes -> class_map[class_type -> get_qualified_name().getString()] : NULL);
    int type_limit = (array_type ? (isSgUnsignedLongVal(array_type -> get_index()) ? isSgUnsignedLongVal(array_type -> get_index()) -> get_value() : exprs.size())
                                 : class_attribute -> numSgInitializedNames());
    for (int i = 0; i < type_limit; i++) {
        SgType *element_type = attributes -> getSourceType(array_type ? array_type -> get_base_type() : class_attribute -> getSgInitializedName(i) -> get_type());
        string element_type_name = ((StringAstAttribute *) element_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

        /**
         * If we are dealing with a field declaration that requires padding, process the padding here.
         */
        if (class_attribute) {
            SgInitializedName *field_decl = class_attribute -> getSgInitializedName(i);
            if (field_decl -> attributeExists(Control::LLVM_STRUCTURE_PADDING)) {
                int pad_size = ((IntAstAttribute *) field_decl -> getAttribute(Control::LLVM_STRUCTURE_PADDING)) -> getValue();
                for (int k = 0; k < pad_size; k++) {
                    (*codeOut) << "i8 0, ";
                }
            }
        }

        /**
         *
         */
        if (i >= exprs.size()) { // Not enough initializers were specified?
            if (isSgClassType(element_type) || element_type -> attributeExists(Control::LLVM_AGGREGATE)) {
                (*codeOut) << element_type_name << " zeroinitializer";
            }
            else {
                string element_type_default_value = ((StringAstAttribute *) element_type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue();
                (*codeOut) << element_type_name << " " << element_type_default_value;
            }
        }
        else { // Emit code for this initializer
            SgAggregateInitializer *sub_aggregate = isSgAggregateInitializer(exprs[i]);
            if (sub_aggregate) {
                (*codeOut) << element_type_name;
                genGlobalAggregateInitialization(sub_aggregate);
            }
            else {
                SgAssignInitializer *assign_init = isSgAssignInitializer(exprs[i]);
                assert(assign_init);
                SgArrayType *sub_array_type = isSgArrayType(element_type);
                if (sub_array_type) {
                    SgStringVal *init_string = isSgStringVal(assign_init -> get_operand());
                    string value = init_string -> get_value();
                    string sub_aggregate_type_name = ((StringAstAttribute *) element_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                    int string_size = ((IntAstAttribute *) init_string -> getAttribute(Control::LLVM_STRING_SIZE)) -> getValue(); 
                    (*codeOut) << sub_aggregate_type_name << " c\"" << attributes -> filter(init_string -> get_value(), string_size) << "\"";
                }
                else {
                    if (isValueInitializer(assign_init)) {
                        string expr_name = ((StringAstAttribute *) assign_init -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                        (*codeOut) << element_type_name << " " << expr_name;
                    }
                    else {
		        // TODO: MOVE this processing to processGlobalInitializer in CodeAttributeVisitor?
                        SgExpression *operand = assign_init -> get_operand();
                        SgExpression *function = (dynamic_cast<SgFunctionRefExp *>(operand)
                                                          ? isSgFunctionRefExp(operand)
                                                          : dynamic_cast<SgCastExp *>(operand) && dynamic_cast<SgFunctionRefExp *>(isSgCastExp(operand) -> get_operand())
                                                                ? isSgFunctionRefExp(isSgCastExp(operand) -> get_operand())
                                                                : NULL);
                        if (function) {
                            string function_name = ((StringAstAttribute *) function -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                                   function_type_name = ((StringAstAttribute *) attributes -> getExpressionType(function) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                            (*codeOut) << element_type_name << " bitcast (" << function_type_name << "* " << function_name << " to " << element_type_name << ")";
                        }
                        else {
                            cerr << "Don't know yet how to process initializer expression of type: " << operand -> class_name() << endl;
                            cerr.flush();
                            assert(0);
                        }
                    }
                }
            }
        }

        if (i + 1 < type_limit) {
            (*codeOut) << ", ";
        }
    }

    /**
     * Note that to make sure we access the correct entity that represent the class type in question, we use the class_attribute
     * instead of the variable class_type.   This is necessary to bypass a ROSE bug.
     */
    if (class_attribute && class_attribute -> getClassType() -> attributeExists(Control::LLVM_STRUCTURE_PADDING)) {
        int pad_size = ((IntAstAttribute *)  class_attribute -> getClassType() -> getAttribute(Control::LLVM_STRUCTURE_PADDING)) -> getValue();
        (*codeOut) << ", ";
        for (int i = 0; i < pad_size; i++) {
            (*codeOut) << "i8 0";
            if (i + 1 < pad_size) {
                (*codeOut) << ", ";
            }
        }
    }

    (*codeOut) << (array_type ? "]" : " }>");

    return;
}


void CodeGeneratorVisitor::genLocalAggregateInitialization(string var_name, SgAggregateInitializer *aggregate) {
     AggregateAstAttribute *attribute = (AggregateAstAttribute *) aggregate -> getAttribute(Control::LLVM_AGGREGATE);
     SgArrayType *array_type = attribute -> getArrayType();
     SgClassType *class_type = attribute -> getClassType();
     assert(array_type || class_type);

     /**
      * Say something here !
      */
     string aggregate_type_name = (array_type && isSgUnsignedLongVal(array_type -> get_index())
                                        ? (StringAstAttribute *) array_type -> getAttribute(Control::LLVM_TYPE)
                                        : (StringAstAttribute *) aggregate -> getAttribute(Control::LLVM_TYPE)) -> getValue();
     vector<SgExpression *> exprs = aggregate -> get_initializers() -> get_expressions();
     int type_limit = (array_type ? (isSgUnsignedLongVal(array_type -> get_index()) ? isSgUnsignedLongVal(array_type -> get_index()) -> get_value() : exprs.size())
                                  : attributes -> class_map[class_type -> get_qualified_name().getString()] -> numSgInitializedNames()),
         limit = (type_limit < exprs.size() ? type_limit : exprs.size());
     for (int i = 0; i < limit; i++) {
         string temp_name = attributes -> getTemp(array_type ? LLVMAstAttributes::TEMP_ARRAY : LLVMAstAttributes::TEMP_GENERIC);
         (*codeOut) << CodeEmitter::indent() << temp_name << " = getelementptr " << aggregate_type_name << "* " << var_name << ", i32 0, i32 " << i << endl;
         SgAggregateInitializer *sub_aggregate = isSgAggregateInitializer(exprs[i]);
         if (sub_aggregate) {
             genLocalAggregateInitialization(temp_name, sub_aggregate);
         }
         else {
             SgAssignInitializer *assign_init = isSgAssignInitializer(exprs[i]);
             assert(assign_init);
             SgType *element_type = attributes -> getSourceType(array_type ? array_type -> get_base_type()
                                                                        : attributes -> class_map[class_type -> get_qualified_name().getString()] -> getSgInitializedName(i) -> get_type());
             string element_type_name = ((StringAstAttribute *) element_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    expr_name = ((StringAstAttribute *) assign_init -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             SgArrayType *sub_array_type = isSgArrayType(element_type);
             if (sub_array_type) {
                 string array_type_name = ((StringAstAttribute *) assign_init -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 SgStringVal *str = isSgStringVal(assign_init -> get_operand()); // the string being copied may be shorter than the target.
                 string temp_ptr = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);
                 int size = ((IntAstAttribute *) sub_array_type -> getAttribute(Control::LLVM_SIZE)) -> getValue(); // TODO: review this ... is explicit size guaranteed to be there?
                 (*codeOut) << CodeEmitter::indent() << temp_ptr << " = bitcast [" << size << " x i8]* " << temp_name << " to " << array_type_name <<  endl;
                 (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.i32(i8* " << temp_ptr << ", " << array_type_name << " " << expr_name
                            << ", i32 " << (str ? str -> get_value().size() + 1 : size) << ", i32 1)" << endl;
             }
             else {
                 SgExpression *operand = assign_init -> get_operand();
                 SgExpression *function = (dynamic_cast<SgFunctionRefExp *>(operand)
                                                   ? isSgFunctionRefExp(operand)
                                                   : dynamic_cast<SgCastExp *>(operand) && dynamic_cast<SgFunctionRefExp *>(isSgCastExp(operand) -> get_operand())
                                                         ? isSgFunctionRefExp(isSgCastExp(operand) -> get_operand())
                                                         : NULL);
                 if (function) { // a pointer to function initialization?
                     string function_name = ((StringAstAttribute *) function -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                            function_type_name = ((StringAstAttribute *) attributes -> getExpressionType(function) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     (*codeOut) << CodeEmitter::indent()
                                << "store " << element_type_name << " bitcast (" << function_type_name << "* " << function_name << " to " << element_type_name << ")"
                                << ", " << element_type_name << "* " << temp_name << endl;
                 }
                 else {
                     (*codeOut) << CodeEmitter::indent() << "store " << element_type_name << " " << expr_name << ", " << element_type_name << "* " << temp_name << endl;
                 }
             }
         }
     }
}


void CodeGeneratorVisitor::genBinaryCompareOperation(SgBinaryOp *node, string condition_code) {
     SgExpression *lhs_operand = node -> get_lhs_operand(),
                  *rhs_operand = node -> get_rhs_operand();
     SgType *lhs_type = attributes -> getSourceType(attributes -> getExpressionType(lhs_operand)),
            *rhs_type = attributes -> getSourceType(attributes -> getExpressionType(rhs_operand));
     string code;
     if (isFloatType(lhs_type))
         code = "u"; // unordered
     else if (condition_code.compare("eq") != 0 && condition_code.compare("ne") != 0) { // integer? 
         code = (isUnsignedType(lhs_type)  &&  isUnsignedType(rhs_type)
                       ? "u"   // unsigned
                       : "s"); // signed
     }
     else code = "";
     code += condition_code;

     string result_name   = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            lhs_name      = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            lhs_type_name = ((StringAstAttribute *)  (lhs_operand -> attributeExists(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                         ? lhs_operand -> getAttribute(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                         : lhs_type -> getAttribute(Control::LLVM_TYPE))) -> getValue(),
            rhs_name      = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
     (*codeOut) << CodeEmitter::indent() << result_name << " = " << (isFloatType(lhs_type) ? "f" : "i")
                << "cmp " << code << " " << lhs_type_name << " " << lhs_name << ", " << rhs_name << endl;
}


void CodeGeneratorVisitor::genAddOrSubtractOperation(SgBinaryOp *node, string opcode) {
     SgExpression *lhs_operand = node -> get_lhs_operand(),
                  *rhs_operand = node -> get_rhs_operand();
     SgType *lhs_type = attributes -> getExpressionType(lhs_operand),
            *rhs_type = attributes -> getExpressionType(rhs_operand);
     string result_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            lhs_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            rhs_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
     /**
      * Note that Rose always makes sure that the pointer type is the left-operand. It transforms the original source if
      * that was not the case.  Thus, consider the following example:
      *
      *    int a[] = { 0, 1, 2, 3, 4};
      *    int *q;
      *
      *    q = &a[3];
      *    q = a + 3;
      *    q = 3 + a;
      *
      *   In all 3 cases above, the AST node generated will correspond to: q = a + 3.
      */
     SgArrayType *lhs_array_type = isSgArrayType(lhs_type);
     SgPointerType *lhs_pointer_type = isSgPointerType(attributes -> getSourceType(lhs_type));
     if (lhs_pointer_type || lhs_array_type) {
         string lhs_type_name = ((StringAstAttribute *)  (lhs_operand -> attributeExists(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                             ? lhs_operand -> getAttribute(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                             : lhs_type -> getAttribute(Control::LLVM_TYPE))) -> getValue(),
                rhs_type_name = ((StringAstAttribute *) (rhs_operand -> attributeExists(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                           ? rhs_operand -> getAttribute(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                           : rhs_type -> getAttribute(Control::LLVM_TYPE))) -> getValue();

         if (lhs_operand -> attributeExists(Control::LLVM_POINTER_TO_INT_CONVERSION)) {
             assert((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_POINTER_TO_INT_CONVERSION));
             string lhs_cast_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_POINTER_TO_INT_CONVERSION)) -> getValue(),
                    rhs_cast_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_POINTER_TO_INT_CONVERSION)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << lhs_cast_name << " = ptrtoint " << lhs_type_name << " " <<  lhs_name << " to " << attributes -> getIntegerPointerTarget() << endl;
             (*codeOut) << CodeEmitter::indent() << rhs_cast_name << " = ptrtoint " << rhs_type_name << " " <<  rhs_name << " to " << attributes -> getIntegerPointerTarget() << endl;
             lhs_name = lhs_cast_name;
             rhs_name = rhs_cast_name;
             (*codeOut) << CodeEmitter::indent() << result_name << " = sub " << attributes -> getIntegerPointerTarget() << " " <<  lhs_cast_name << ", " << rhs_cast_name << endl;
             if (node -> attributeExists(Control::LLVM_POINTER_DIFFERENCE_DIVIDER)) {
                 SgType *base_type = (lhs_array_type ? lhs_array_type -> get_base_type() : lhs_pointer_type -> get_base_type());
                 int size = ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                 if (size > 1) { // element size greater than 1?
                     string divide_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_POINTER_DIFFERENCE_DIVIDER)) -> getValue();
                     int shift_size = 0;
                     for (int k = size >> 1; k > 0; k >>= 1) {
                         shift_size++;
                     }
                     if (size == 1 << shift_size) {
                         (*codeOut) << CodeEmitter::indent() << divide_name << " = ashr " << attributes -> getIntegerPointerTarget() << " " <<  result_name << ", " << shift_size << endl;
                     }
                     else {
                         (*codeOut) << CodeEmitter::indent() << divide_name << " = sdiv " << attributes -> getIntegerPointerTarget() << " " <<  result_name << ", " << size << endl;
                     }
                 }
             }
         }
         else {
             if (isSgSubtractOp(node)) {
                 assert((StringAstAttribute *) node -> getAttribute(Control::LLVM_NEGATION_NAME));
                 string negation_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NEGATION_NAME)) -> getValue();
                 if (! (isSgIntVal(rhs_operand) || isSgEnumVal(rhs_operand))) { // not a constant value?
                     (*codeOut) << CodeEmitter::indent() << negation_name << " = sub " << rhs_type_name << " 0, " << rhs_name << endl;
                 }
                 rhs_name = negation_name;
             }
             (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr " << lhs_type_name << " " <<  lhs_name << ", " << rhs_type_name << " " << rhs_name << endl;
         }
     }
     else {
         string type_name = ((StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << result_name << " = " << (isFloatType(node -> get_type()) ? "f" : "") << opcode
                    << " " <<  type_name << " " << lhs_name << ", " << rhs_name << endl;
     }
}


void CodeGeneratorVisitor::genAddOrSubtractOperationAndAssign(SgBinaryOp *node, string opcode) {
     SgExpression *lhs_operand = node -> get_lhs_operand(),
                  *rhs_operand = node -> get_rhs_operand();
     SgType *lhs_type = attributes -> getExpressionType(lhs_operand),
            *rhs_type = attributes -> getExpressionType(rhs_operand);
     string result_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            lhs_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            rhs_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            ref_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue(),
            lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            rhs_type_name = ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            result_type_name = ((StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE)) -> getValue();

    if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) {
        string promote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) -> getValue();
        (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (isUnsignedType(lhs_type) ? "zext " : "sext ")
                   << lhs_type_name << " " <<  lhs_name << " to " << rhs_type_name << endl;
        lhs_name = promote_name;
        result_type_name = rhs_type_name;
    }

    if (isSgPointerType(attributes -> getSourceType(lhs_type)) || isSgArrayType(lhs_type)) {
         if (isSgMinusAssignOp(node)) {
             string negation_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NEGATION_NAME)) -> getValue();
             if (! (isSgIntVal(rhs_operand) || isSgEnumVal(rhs_operand))) { // not a constant value?
                 (*codeOut) << CodeEmitter::indent() << negation_name << " = sub " << rhs_type_name << " 0, " << rhs_name << endl;
             }
             rhs_name = negation_name;
         }
         (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr " << lhs_type_name << " " <<  lhs_name << ", " << rhs_type_name << " " << rhs_name << endl;
     }
     else if (isSgPointerType(attributes -> getSourceType(rhs_type)) || isSgArrayType(rhs_type)) {
         assert(! "This is not supposed to happen !!!");
     }
     else {
         (*codeOut) << CodeEmitter::indent() << result_name << " = " << (isFloatType(attributes -> getExpressionType(node)) ? "f" : "") << opcode
                    << " " << result_type_name << " " << lhs_name << ", " << rhs_name << endl;
     }

     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION)) {
         string  demote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION)) -> getValue();
         StringAstAttribute *result_attribute = (StringAstAttribute *) node -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME),
                            *result_type_attribute = (StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE);
         (*codeOut) << CodeEmitter::indent() << demote_name << " = trunc " << result_type_name << " " <<  result_name << " to " << result_type_attribute -> getValue() << endl;
         result_name = demote_name;
         result_type_name = result_type_attribute -> getValue();
     }

     (*codeOut) << CodeEmitter::indent() << "store " << result_type_name << " " << result_name << ", " << result_type_name << "* " << ref_name << endl;
}


void CodeGeneratorVisitor::genBasicBinaryOperation(SgBinaryOp *node, string opcode) {
     string result_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            type_name = ((StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            lhs_name = ((StringAstAttribute *) node -> get_lhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            rhs_name = ((StringAstAttribute *) node -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
     (*codeOut) << CodeEmitter::indent() << result_name << " = " << (isFloatType(attributes -> getExpressionType(node)) ? "f" : "") << opcode
                << " " <<  type_name << " " << lhs_name << ", " << rhs_name << endl;
}


void CodeGeneratorVisitor::genBasicBinaryOperationAndAssign(SgBinaryOp *node, string opcode) {
     SgExpression *lhs_operand = node -> get_lhs_operand(),
                  *rhs_operand = node -> get_rhs_operand();
     SgType *lhs_type = attributes -> getExpressionType(lhs_operand),
            *rhs_type = attributes -> getExpressionType(rhs_operand);
     string result_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            lhs_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            rhs_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            ref_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue(),
            lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            rhs_type_name = ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            result_type_name = ((StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE)) -> getValue();

    if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) {
        string promote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) -> getValue();
        (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (isUnsignedType(lhs_type) ? "zext " : "sext ")
                   << lhs_type_name << " " <<  lhs_name << " to " << rhs_type_name << endl;
        lhs_name = promote_name;
        result_type_name = rhs_type_name;
    }

     (*codeOut) << CodeEmitter::indent() << result_name << " = " << (isFloatType(attributes -> getExpressionType(node)) ? "f" : "") << opcode
                << " " << result_type_name << " " << lhs_name << ", " << rhs_name << endl;

     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION)) {
         string  demote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << demote_name << " = trunc " << result_type_name << " " <<  result_name << " to " << result_type_name << endl;
         result_name = demote_name;
     }

     (*codeOut) << CodeEmitter::indent() << "store " << result_type_name << " " << result_name << ", " << result_type_name << "* " << ref_name << endl;
}


void CodeGeneratorVisitor::genDivideBinaryOperation(SgBinaryOp *node, string opcode) {
     string result_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            lhs_type_name = ((StringAstAttribute *) attributes -> getExpressionType(node -> get_lhs_operand()) -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            lhs_name = ((StringAstAttribute *) node -> get_lhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            rhs_name = ((StringAstAttribute *) node -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
     (*codeOut) << CodeEmitter::indent() << result_name << " = "
                << (isFloatType(attributes -> getExpressionType(node)) ? "f" : isUnsignedType(attributes -> getExpressionType(node)) ? "u" : "s")
                << opcode << " " <<  lhs_type_name << " " << lhs_name << ", " << rhs_name << endl;
}


void CodeGeneratorVisitor::genDivideBinaryOperationAndAssign(SgBinaryOp *node, string opcode) {
     SgExpression *lhs_operand = node -> get_lhs_operand(),
                  *rhs_operand = node -> get_rhs_operand();
     SgType *lhs_type = attributes -> getExpressionType(lhs_operand),
            *rhs_type = attributes -> getExpressionType(rhs_operand);
     string result_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            lhs_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            rhs_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            ref_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue(),
            lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            rhs_type_name = ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            result_type_name = ((StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE)) -> getValue();

     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) {
         string promote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (isUnsignedType(lhs_type) ? "zext " : "sext ")
                    << lhs_type_name << " " <<  lhs_name << " to " << rhs_type_name << endl;
         lhs_name = promote_name;
         result_type_name = rhs_type_name;
     }

     (*codeOut) << CodeEmitter::indent() << result_name << " = "
                << (isFloatType(attributes -> getExpressionType(node)) ? "f" : isUnsignedType(attributes -> getExpressionType(node)) ? "u" : "s")
                << opcode << " " <<  result_type_name << " " << lhs_name << ", " << rhs_name << endl;

     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION)) {
         string  demote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << demote_name << " = trunc " << result_type_name << " " <<  result_name << " to " << result_type_name << endl;
         result_name = demote_name;
     }

     (*codeOut) << CodeEmitter::indent() << "store " << result_type_name << " " << result_name << ", " << result_type_name << "* " << ref_name << endl;
}


void CodeGeneratorVisitor::preOrderVisit(SgNode *node) {
     if (option.isDebugPreTraversal()) {
         cerr << "CodeGenerator Visitor Pre-processing: "
              <<  (isSgFunctionDeclaration(node) ? " (*** Function " : "")
              <<  (isSgFunctionDeclaration(node) ? isSgFunctionSymbol(isSgFunctionDeclaration(node) -> search_for_symbol_from_symbol_table()) -> get_name().getString() : "")
              <<  (isSgFunctionDeclaration(node) ? ") " : "")
              << node -> class_name() << endl;  // Used for Debugging
         cerr.flush();
     }

     if (visit_suspended_by_node) { // If visiting was suspended, ignore this node
         if (option.isDebugPreTraversal()) {
       //         cerr << "Skipping node "
       //              << node -> class_name()
       //              << endl;
       //          cerr.flush();
         }
         return;
     }

     /**
      * Special case for for_increment
      */
     if (node -> attributeExists(Control::LLVM_BUFFERED_OUTPUT)) {
         codeOut -> startOutputToBuffer();
     }

     /**
      * Special case for if blocks.
      */
     if (dynamic_cast<SgStatement *>(node)) {
         SgStatement *n = isSgStatement(node);
         if (n -> attributeExists(Control::LLVM_IF_COMPONENT_LABELS)) {
             IfComponentAstAttribute *attribute = (IfComponentAstAttribute *) n -> getAttribute(Control::LLVM_IF_COMPONENT_LABELS);
             codeOut -> emitLabel(current_function_decls, attribute -> getLabel());
         }
     }

     /**
      * Special case for conditional true and false expressions
      */
     if (dynamic_cast<SgExpression *>(node)) {
         SgExpression *n = isSgExpression(node);
         if (n -> attributeExists(Control::LLVM_CONDITIONAL_COMPONENT_LABELS)) {
             ConditionalComponentAstAttribute *attribute = (ConditionalComponentAstAttribute *) n -> getAttribute(Control::LLVM_CONDITIONAL_COMPONENT_LABELS);
             codeOut -> emitLabel(current_function_decls, attribute -> getLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_AND_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_AND_RHS);
             codeOut -> emitLabel(current_function_decls, attribute -> getRhsLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_OR_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_OR_RHS);
             codeOut -> emitLabel(current_function_decls, attribute -> getRhsLabel());
         }
     }

     /**
      * The main switch:
      */
     // SgNode:
     if (false) { // If visiting was suspended, ignore this node
          ;
     }
     //     SgSupport:
     //         SgModifier:
     //             SgModifierNodes
     //             SgConstVolatileModifier
     //             SgStorageModifier
     //             SgAccessModifier
     //             SgFunctionModifier
     //             SgUPC_AccessModifier
     //             SgSpecialFunctionModifier
     //             SgElaboratedTypeModifier
     //             SgLinkageModifier
     //             SgBaseClassModifier
     //             SgTypeModifier
     //             SgDeclarationModifier
     //         SgName
     //         SgSymbolTable
     //         SgInitializedName
     else if (dynamic_cast<SgInitializedName *>(node)) {
         SgInitializedName *n = isSgInitializedName(node);

         /**
          * Suspend traversal of global declarations. If the global declaration of a pointer is
          * initialized with a cast expression we don't want to traverse that expression and emit
          * code for it.
          */
         if (n -> attributeExists(Control::LLVM_GLOBAL_DECLARATION)) {
             visit_suspended_by_node = node;
         }
     }
     //         SgAttribute:
     //             SgPragma
     //             SgBitAttribute:
     //                 SgFuncDecl_attr
     //                 SgClassDecl_attr
     //         Sg_File_Info
     //         SgFile:
     //             SgSourceFile
     else if (dynamic_cast<SgSourceFile *> (node)) {
         SgSourceFile *n = isSgSourceFile(node);
         setAttributes((LLVMAstAttributes *) n -> getAttribute(Control::LLVM_AST_ATTRIBUTES));
         generateGlobals();           // generate globals declarations
     }
     //             SgBinaryFile
     //             SgUnknownFile
     //         SgProject
     //         SgOptions
     //         SgUnparse_Info
     //         SgBaseClass
     //         SgTypedefSeq
     //         SgTemplateParameter
     //         SgTemplateArgument
     //         SgDirectory
     //         SgFileList
     //         SgDirectoryList
     //         SgFunctionParameterTypeList
     //         SgQualifiedName
     //         SgTemplateArgumentList
     //         SgTemplateParameterList
     //         SgGraph:
     //             SgIncidenceDirectedGraph:
     //                 SgBidirectionalGraph:
     //                     SgStringKeyedBidirectionalGraph
     //                     SgIntKeyedBidirectionalGraph
     //             SgIncidenceUndirectedGraph
     //         SgGraphNode
     //         SgGraphEdge:
     //             SgDirectedGraphEdge
     //             SgUndirectedGraphEdge
     //         SgGraphNodeList
     //         SgGraphEdgeList
     //         SgNameGroup
     //         SgCommonBlockObject
     //         SgDimensionObject
     //         SgFormatItem
     //         SgFormatItemList
     //         SgDataStatementGroup
     //         SgDataStatementObject
     //         SgDataStatementValue
     //     SgType:
     //         SgTypeUnknown
     //         SgTypeChar
     //         SgTypeSignedChar
     //         SgTypeUnsignedChar
     //         SgTypeShort
     //         SgTypeSignedShort
     //         SgTypeUnsignedShort
     //         SgTypeInt
     //         SgTypeSignedInt
     //         SgTypeUnsignedInt
     //         SgTypeLong
     //         SgTypeSignedLong
     //         SgTypeUnsignedLong
     //         SgTypeVoid
     //         SgTypeGlobalVoid
     //         SgTypeWchar
     //         SgTypeFloat
     //         SgTypeDouble
     //         SgTypeLongLong
     //         SgTypeSignedLongLong
     //         SgTypeUnsignedLongLong
     //         SgTypeLongDouble
     //         SgTypeString
     //         SgTypeBool
     //         SgPointerType:
     //             SgPointerMemberType
     //         SgReferenceType
     //         SgNamedType:
     //             SgClassType
     //             SgEnumType
     //             SgTypedefType
     //         SgModifierType
     //         SgFunctionType:
     //             SgMemberFunctionType:
     //                 SgPartialFunctionType:
     //                     SgPartialFunctionModifierType
     //         SgArrayType
     //         SgTypeEllipse
     //         SgTemplateType
     //         SgQualifiedNameType
     //         SgTypeComplex
     //         SgTypeImaginary
     //         SgTypeDefault
     //     SgLocatedNode:
     //         SgStatement:
     //             SgScopeStatement:
     //                 SgGlobal
     //                 SgBasicBlock
     //                 SgIfStmt
     else if (dynamic_cast<SgIfStmt *> (node)) {
         SgIfStmt *n = isSgIfStmt(node);
     }
     //                 SgForStatement
     else if (dynamic_cast<SgForStatement *> (node)) {
         SgForStatement *n = isSgForStatement(node);

         scopeStack.push(n);
     }
     //                 SgFunctionDefinition
     //                 SgClassDefinition:
     //                     SgTemplateInstantiationDefn
     //                 SgWhileStmt
     else if (dynamic_cast<SgWhileStmt *> (node)) {
         SgWhileStmt *n = isSgWhileStmt(node);

         scopeStack.push(n);

         WhileAstAttribute *attribute = ( WhileAstAttribute *) n -> getAttribute(Control::LLVM_WHILE_LABELS);
         codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel());
         codeOut -> emitLabel(current_function_decls, attribute -> getConditionLabel());
     }
     //                 SgDoWhileStmt
     else if (dynamic_cast<SgDoWhileStmt *> (node)) {
         SgDoWhileStmt *n = isSgDoWhileStmt(node);

         scopeStack.push(n);

         DoAstAttribute *attribute = (DoAstAttribute *) n -> getAttribute(Control::LLVM_DO_LABELS);
         codeOut -> emitUnconditionalBranch(attribute -> getBodyLabel());
         codeOut -> emitLabel(current_function_decls, attribute -> getBodyLabel());
     }
     //                 SgSwitchStatement
     else if (dynamic_cast<SgSwitchStatement *>(node)) {
         SgSwitchStatement *n = isSgSwitchStatement(node);

         scopeStack.push(n);

         switchStack.push(n);
     }
     //                 SgCatchOptionStmt
     //                 SgNamespaceDefinitionStatement
     //                 SgBlockDataStatement
     //                 SgAssociateStatement
     //                 SgFortranDo:
     //                     SgFortranNonblockedDo
     //                 SgForAllStatement
     //                 SgUpcForAllStatement
     //             SgFunctionTypeTable
     //             SgDeclarationStatement:
     //                 SgFunctionParameterList
     //                 SgVariableDeclaration
     //                 SgVariableDefinition
     //                 SgClinkageDeclarationStatement:
     //                     SgClinkageStartStatement
     //                     SgClinkageEndStatement
     //             SgEnumDeclaration
     //             SgAsmStmt
     //             SgAttributeSpecificationStatement
     //             SgFormatStatement
     //             SgTemplateDeclaration
     //             SgTemplateInstantiationDirectiveStatement
     //             SgUseStatement
     //             SgParameterStatement
     //             SgNamespaceDeclarationStatement
     //             SgEquivalenceStatement
     //             SgInterfaceStatement
     //             SgNamespaceAliasDeclarationStatement
     //             SgCommonBlock
     //             SgTypedefDeclaration
     //             SgStatementFunctionStatement
     //             SgCtorInitializerList
     //             SgPragmaDeclaration
     //             SgUsingDirectiveStatement
     //             SgClassDeclaration:
     //                 SgTemplateInstantiationDecl
     //                 SgDerivedTypeStatement
     //                 SgModuleStatement
     //             SgImplicitStatement
     //             SgUsingDeclarationStatement
     //             SgNamelistStatement
     //             SgImportStatement
     //             SgFunctionDeclaration:
     else if (dynamic_cast<SgFunctionDeclaration *>(node)) {
         SgFunctionDeclaration *n = isSgFunctionDeclaration(node);

         if ((! n -> get_definition()) || // A function header without definition
             (option.isQuery() && (! n -> attributeExists(Control::LLVM_TRANSLATE))) || // a query translation that is not applicable to this function
             n -> attributeExists(Control::LLVM_IGNORE)) { // A function that requires full traversal - Ignore it here on the regular pass.
             visit_suspended_by_node = node;
         }
         else {
             attributes -> resetIntCount();

             current_function_decls = (FunctionAstAttribute *) n -> getAttribute(Control::LLVM_LOCAL_DECLARATIONS);
             SgType *return_type = n -> get_type() -> get_return_type();
             SgClassType *class_type = isSgClassType(attributes -> getSourceType(return_type));
             int integral_class_return_type = attributes -> integralStructureType(attributes -> getSourceType(return_type));
             stringstream out;
             out << "i" << (integral_class_return_type * 8);

             string original_return_type_name = ((StringAstAttribute *) return_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    return_type_name = (integral_class_return_type
                                                ? out.str()
                                                : class_type
                                                       ? "void"
                                                       : original_return_type_name);

             SgStorageModifier &sm = n -> get_declarationModifier().get_storageModifier();
             (*codeOut) << "define " << (sm.isStatic() ? "internal " : "") << return_type_name << " @" << n -> get_name().getString() << "(";

             /**
              * First, declare a reference parameter for the return type if it is a structure whose size is > 64 bits
              * Next declare the remaining parameters.
              */
             vector<SgInitializedName *> parms = n -> get_args();
             if (class_type && integral_class_return_type == 0) {
                 (*codeOut) << original_return_type_name << "* noalias sret %agg.result" << (parms.size() > 0 ? ", " : "");
             }
             for (int i = 0; i < parms.size(); i++) {
                 SgInitializedName *parm = parms[i];
                 SgType *type = attributes -> getSourceType(parm -> get_type()); // original type
                 string type_name = ((StringAstAttribute *) parm -> getAttribute(Control::LLVM_TYPE)) -> getValue(); 
                 (*codeOut) << type_name;
                 if (! isSgTypeEllipse(type)) {
                     if (isSgClassType(type)) {
                         (*codeOut) << "* byval";
                     }
                     (*codeOut) << " %" << parm -> get_name().getString();
                 }
                 if (i + 1 < parms.size()) {
                     (*codeOut) << ", ";
                 }
             }
             (*codeOut) << ") nounwind {" << endl;

             codeOut -> emitLabel(current_function_decls, current_function_decls -> getEntryLabel());

             /**
              * Declare variable for returning value, if needed
              */
             if (! (isSgTypeVoid(attributes -> getSourceType(return_type)) || (class_type && integral_class_return_type == 0))) {
                  (*codeOut) << CodeEmitter::indent() << "%.retval = alloca " << original_return_type_name;
                  if (return_type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
                       int alignment = ((IntAstAttribute *) return_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                       (*codeOut) << ", align " << alignment;
                  }
                  (*codeOut) << endl;
             }

             /**
              * Declare local variables
              */
             for (int i = 0; i < current_function_decls -> numSgInitializedNames(); i++) {
                 SgInitializedName *decl = current_function_decls -> getSgInitializedName(i);
                 SgType *type = attributes -> getSourceType(decl -> get_type()); // original type
                 if (! (isSgTypeEllipse(type) || (decl -> attributeExists(Control::LLVM_PARAMETER) && isSgClassType(type)))) {
                     string name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_NAME)) -> getValue();
                     string type_name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_TYPE)) -> getValue();

                     (*codeOut) << CodeEmitter::indent() << name << " = alloca " << type_name;

                     IntAstAttribute *alignment_attribute = (IntAstAttribute *) type -> getAttribute(Control::LLVM_ALIGN_TYPE);
                     if (alignment_attribute) {
                         (*codeOut) << ", align " << alignment_attribute -> getValue();
                     }
                     (*codeOut) << endl;
                 }
             }

             /**
              * Declare temporary names used for coersion.
              */
             for (int i = 0; i < current_function_decls -> numCoerces(); i++) {
                 SgType *coerce_type = current_function_decls -> getCoerceType(i);
                 string coerce_name = current_function_decls -> getCoerceName(i),
                        coerce_type_name = ((StringAstAttribute *) coerce_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

                 (*codeOut) << CodeEmitter::indent() << coerce_name << " = alloca " << coerce_type_name;

                 IntAstAttribute *alignment_attribute = (IntAstAttribute *) coerce_type -> getAttribute(Control::LLVM_ALIGN_TYPE);
                 if (alignment_attribute) {
                     (*codeOut) << ", align " << alignment_attribute -> getValue();
                 }
                 (*codeOut) << endl;
             }

             /**
              * Store primitive parameters
              */
             for (int i = 0; i < parms.size(); i++) {
                 SgInitializedName *parm = parms[i];
                 SgType *type = attributes -> getSourceType(parm -> get_type()); // original type
                 if (! (isSgTypeEllipse(type) || isSgClassType(type))) {
                     string parm_name = ((StringAstAttribute *) parm -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                            parm_type = ((StringAstAttribute *) parm -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << "store " << parm_type << " %" << parm -> get_name().getString()
                                << ", " << parm_type << "* " << parm_name << endl;
                 }
             }
         }
     }
     //                 SgMemberFunctionDeclaration:
     //                     SgTemplateInstantiationMemberFunctionDecl
     //                 SgTemplateInstantiationFunctionDecl
     //                 SgProgramHeaderStatement
     //                 SgProcedureHeaderStatement
     //                 SgEntryStatement
     //             SgContainsStatement
     //             SgC_PreprocessorDirectiveStatement:
     //                 SgIncludeDirectiveStatement
     //                 SgDefineDirectiveStatement
     //                 SgUndefDirectiveStatement
     //                 SgIfdefDirectiveStatement
     //                 SgIfndefDirectiveStatement
     //                 SgIfDirectiveStatement
     //                 SgDeadIfDirectiveStatement
     //                 SgElseDirectiveStatement
     //                 SgElseifDirectiveStatement
     //                 SgEndifDirectiveStatement
     //                 SgLineDirectiveStatement
     //                 SgWarningDirectiveStatement
     //                 SgErrorDirectiveStatement
     //                 SgEmptyDirectiveStatement
     //                 SgIncludeNextDirectiveStatement
     //                 SgIdentDirectiveStatement
     //                 SgLinemarkerDirectiveStatement
     //             SgOmpThreadprivateStatement
     //             SgFortranIncludeLine
     //             SgExprStatement
     else if (dynamic_cast<SgExprStatement *>(node)) {
         SgExprStatement *n = isSgExprStatement(node);

         if (n -> attributeExists(Control::LLVM_DO_LABELS)) {
             DoAstAttribute *attribute = (DoAstAttribute *) n -> getAttribute(Control::LLVM_DO_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel());
             codeOut -> emitLabel(current_function_decls, attribute -> getConditionLabel());
         }
     }
     //             SgLabelStatement
     //             SgCaseOptionStmt
     else if (dynamic_cast<SgCaseOptionStmt *>(node)) {
         SgCaseOptionStmt *n = isSgCaseOptionStmt(node);
         CaseAstAttribute *attribute = (CaseAstAttribute *) n -> getAttribute(Control::LLVM_CASE_INFO);
         assert(attribute);
         if (! attribute -> reusedLabel()) {
             codeOut -> emitUnconditionalBranch(attribute -> getCaseLabel());
             codeOut -> emitLabel(current_function_decls, attribute -> getCaseLabel());
         }
     }
     //             SgTryStmt
     //             SgDefaultOptionStmt
     else if (dynamic_cast<SgDefaultOptionStmt *>(node)) {
         SgDefaultOptionStmt *n = isSgDefaultOptionStmt(node);
         string default_label = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_DEFAULT_LABEL)) -> getValue();
         codeOut -> emitUnconditionalBranch(default_label);
         codeOut -> emitLabel(current_function_decls, default_label);
     }
     //             SgBreakStmt
     //             SgContinueStmt
     //             SgReturnStmt
     //             SgGotoStatement
     //             SgSpawnStmt
     //             SgNullStatement
     //             SgVariantStatement
     //             SgForInitStatement
     //             SgCatchStatementSeq
     //             SgStopOrPauseStatement
     //             SgIOStatement:
     //                 SgPrintStatement
     //                 SgReadStatement
     //                 SgWriteStatement
     //                 SgOpenStatement
     //                 SgCloseStatement
     //                 SgInquireStatement
     //                 SgFlushStatement
     //                 SgBackspaceStatement
     //                 SgRewindStatement
     //                 SgEndfileStatement
     //                 SgWaitStatement
     //             SgWhereStatement
     //             SgElseWhereStatement
     //             SgNullifyStatement
     //             SgArithmeticIfStatement
     //             SgAssignStatement
     //             SgComputedGotoStatement
     //             SgAssignedGotoStatement
     //             SgAllocateStatement
     //             SgDeallocateStatement
     //             SgUpcNotifyStatement
     //             SgUpcWaitStatement
     //             SgUpcBarrierStatement
     //             SgUpcFenceStatement
     //             SgOmpBarrierStatement
     //             SgOmpTaskwaitStatement
     //             SgOmpFlushStatement
     //             SgOmpBodyStatement:
     //                 SgOmpAtomicStatement
     //                 SgOmpMasterStatement
     //                 SgOmpOrderedStatement
     //                 SgOmpCriticalStatement
     //                 SgOmpSectionStatement
     //                 SgOmpWorkshareStatement
     //                 SgOmpClauseBodyStatement:
     //                     SgOmpParallelStatement
     //                     SgOmpSingleStatement
     //                     SgOmpTaskStatement
     //                     SgOmpForStatement
     //                     SgOmpDoStatement
     //                     SgOmpSectionsStatement
     //             SgSequenceStatement
     //         SgExpression:
     //             SgUnaryOp:
     //                 SgExpressionRoot
     //                 SgMinusOp
     //                 SgUnaryAddOp
     //                 SgNotOp
     //                 SgPointerDerefExp
     //                 SgAddressOfOp
     //                 SgMinusMinusOp
     //                 SgPlusPlusOp
     //                 SgBitComplementOp
     //                 SgCastExp
     else if (dynamic_cast<SgCastExp *>(node)) {
         SgCastExp *n = isSgCastExp(node);

         /**
          * The following casts have already been taken care of and no code need to be generated for them.
          */
         if (isSgCharVal(n -> get_operand()) ||
             isSgUnsignedCharVal(n -> get_operand()) ||
             n -> attributeExists(Control::LLVM_NULL_VALUE) ||
             n -> attributeExists(Control::LLVM_IGNORE)) {
             visit_suspended_by_node = node; // Nothing to do for these two cases.
         }
     }
     //                 SgThrowOp
     //                 SgRealPartOp
     //                 SgImagPartOp
     //                 SgConjugateOp
     //                 SgUserDefinedUnaryOp
     //             SgBinaryOp:
     //                 SgArrowExp
     else if (dynamic_cast<SgArrowExp *>(node)) {
         SgArrowExp *n = isSgArrowExp(node);
     }
     //                 SgDotExp
     else if (dynamic_cast<SgDotExp *>(node)) {
         SgDotExp *n = isSgDotExp(node);
     }
     //                 SgDotStarOp
     //                 SgArrowStarOp
     //                 SgEqualityOp
     //                 SgLessThanOp
     //                 SgGreaterThanOp
     //                 SgNotEqualOp
     //                 SgLessOrEqualOp
     //                 SgGreaterOrEqualOp
     //                 SgAddOp
     //                 SgSubtractOp
     //                 SgMultiplyOp
     //                 SgDivideOp
     //                 SgIntegerDivideOp
     //                 SgModOp
     //                 SgAndOp
     //                 SgOrOp
     //                 SgBitXorOp
     //                 SgBitAndOp
     //                 SgBitOrOp
     //                 SgCommaOpExp
     //                 SgLshiftOp
     //                 SgRshiftOp
     //                 SgPntrArrefExp
     //                 SgScopeOp
     //                 SgAssignOp
     //                 SgPlusAssignOp
     //                 SgMinusAssignOp
     //                 SgAndAssignOp
     //                 SgIorAssignOp
     //                 SgMultAssignOp
     //                 SgDivAssignOp
     //                 SgModAssignOp
     //                 SgXorAssignOp
     //                 SgLshiftAssignOp
     //                 SgRshiftAssignOp
     //                 SgExponentiationOp
     //                 SgConcatenationOp
     //                 SgPointerAssignOp
     //                 SgUserDefinedBinaryOp
     //             SgExprListExp
     //             SgVarRefExp
     //             SgClassNameRefExp
     //             SgFunctionRefExp
     //             SgMemberFunctionRefExp
     //             SgValueExp:
     //                 SgBoolValExp
     //                 SgStringVal
     //                 SgShortVal
     //                 SgCharVal
     //                 SgUnsignedCharVal
     //                 SgWcharVal
     //                 SgUnsignedShortVal
     //                 SgIntVal
     //                 SgEnumVal
     //                 SgUnsignedIntVal
     //                 SgLongIntVal
     //                 SgLongLongIntVal
     //                 SgUnsignedLongLongIntVal
     //                 SgUnsignedLongVal
     //                 SgFloatVal
     //                 SgDoubleVal
     //                 SgLongDoubleVal
     //                 SgComplexVal
     //                 SgUpcThreads
     //                 SgUpcMythread
     //                 SgFunctionCallExp
     //                 SgSizeOfOp
     //                 SgUpcLocalsizeof
     //                 SgUpcBlocksizeof
     //                 SgUpcElemsizeof
         /**
          * Warning!!!
          *
          * SgValueExp is a superclass of other AST nodes, to add a test case for any of its
          * subclasses the test case must be nested inside this basic block.
                    SgValueExp:
                        SgBoolValExp
                        SgStringVal
                        SgShortVal
                        SgCharVal
                        SgUnsignedCharVal
                        SgWcharVal
                        SgUnsignedShortVal
                        SgIntVal
                        SgEnumVal
                        SgUnsignedIntVal
                        SgLongIntVal
                        SgLongLongIntVal
                        SgUnsignedLongLongIntVal
                        SgUnsignedLongVal
                        SgFloatVal
                        SgDoubleVal
                        SgLongDoubleVal
                        SgComplexVal
                        SgUpcThreads
                        SgUpcMythread
                        SgFunctionCallExp
                        SgSizeOfOp
                        SgUpcLocalsizeof
                        SgUpcBlocksizeof
                        SgUpcElemsizeof
          */
     else if (dynamic_cast<SgValueExp *>(node)) {
         SgValueExp *n = isSgValueExp(node);
         visit_suspended_by_node = node;
     }
     //             SgTypeIdOp
     //             SgConditionalExp
     else if (dynamic_cast<SgConditionalExp *>(node)) {
         SgConditionalExp *n = isSgConditionalExp(node);
     }
     //             SgNewExp
     //             SgDeleteExp
     //             SgThisExp
     //             SgRefExp
     //             SgInitializer:
     //                 SgAggregateInitializer
     //                 SgConstructorInitializer
     //                 SgAssignInitializer
     //                 SgDesignatedInitializer
     //             SgVarArgStartOp
     //             SgVarArgOp
     //             SgVarArgEndOp
     //             SgVarArgCopyOp
     //             SgVarArgStartOneOperandOp
     //             SgNullExpression
     //             SgVariantExpression
     //             SgSubscriptExpression
     //             SgColonShapeExp
     //             SgAsteriskShapeExp
     //             SgImpliedDo
     //             SgIOItemExpression
     //             SgStatementExpression
     //             SgAsmOp
     //             SgLabelRefExp
     //             SgActualArgumentExpression
     //             SgUnknownArrayOrFunctionReference
     //         SgLocatedNodeSupport:
     //             SgInterfaceBody
     //             SgRenamePair
     //             SgOmpClause:
     //                 SgOmpOrderedClause
     //                 SgOmpNowaitClause
     //                 SgOmpUntiedClause
     //                 SgOmpDefaultClause
     //                 SgOmpExpressionClause:
     //                     SgOmpCollapseClause
     //                     SgOmpIfClause
     //                     SgOmpNumThreadsClause
     //                 SgOmpVariablesClause:
     //                     SgOmpCopyprivateClause
     //                     SgOmpPrivateClause
     //                     SgOmpFirstprivateClause
     //                     SgOmpSharedClause
     //                     SgOmpCopyinClause
     //                     SgOmpLastprivateClause
     //                     SgOmpReductionClause
     //                 SgOmpScheduleClause
     //         SgToken
     //     SgSymbol:
     //         SgVariableSymbol
     //         SgFunctionSymbol:
     //             SgMemberFunctionSymbol
     //             SgRenameSymbol
     //         SgFunctionTypeSymbol
     //         SgClassSymbol
     //         SgTemplateSymbol
     //         SgEnumSymbol
     //         SgEnumFieldSymbol
     //         SgTypedefSymbol
     //         SgLabelSymbol
     //         SgDefaultSymbol
     //         SgNamespaceSymbol
     //         SgIntrinsicSymbol
     //         SgModuleSymbol
     //         SgInterfaceSymbol
     //         SgCommonSymbol
     //         SgAliasSymbol
     //     SgAsmNode:
     //         SgAsmStatement:
     //             SgAsmDeclaration:
     //                 SgAsmDataStructureDeclaration
     //                 SgAsmFunctionDeclaration
     //                 SgAsmFieldDeclaration
     //             SgAsmBlock
     //             SgAsmInstruction:
     //                 SgAsmx86Instruction
     //                 SgAsmArmInstruction
     //                 SgAsmPowerpcInstruction
     //         SgAsmExpression:
     //             SgAsmValueExpression:
     //                 SgAsmByteValueExpression
     //                 SgAsmWordValueExpression
     //                 SgAsmDoubleWordValueExpression
     //                 SgAsmQuadWordValueExpression
     //                 SgAsmSingleFloatValueExpression
     //                 SgAsmDoubleFloatValueExpression
     //                 SgAsmVectorValueExpression
     //             SgAsmBinaryExpression:
     //                 SgAsmBinaryAdd
     //                 SgAsmBinarySubtract
     //                 SgAsmBinaryMultiply
     //                 SgAsmBinaryDivide
     //                 SgAsmBinaryMod
     //                 SgAsmBinaryAddPreupdate
     //                 SgAsmBinarySubtractPreupdate
     //                 SgAsmBinaryAddPostupdate
     //                 SgAsmBinarySubtractPostupdate
     //                 SgAsmBinaryLsl
     //                 SgAsmBinaryLsr
     //                 SgAsmBinaryAsr
     //                 SgAsmBinaryRor
     //             SgAsmUnaryExpression:
     //                 SgAsmUnaryPlus
     //                 SgAsmUnaryMinus
     //                 SgAsmUnaryRrx
     //                 SgAsmUnaryArmSpecialRegisterList
     //             SgAsmMemoryReferenceExpression
     //             SgAsmRegisterReferenceExpression:
     //                 SgAsmx86RegisterReferenceExpression
     //                 SgAsmArmRegisterReferenceExpression
     //                 SgAsmPowerpcRegisterReferenceExpression
     //             SgAsmControlFlagsExpression
     //             SgAsmCommonSubExpression
     //             SgAsmExprListExp
     //             SgAsmFile
     //             SgAsmInterpretation
     //             SgAsmOperandList
     //             SgAsmType
     //             SgAsmTypeByte
     //             SgAsmTypeWord
     //             SgAsmTypeDoubleWord
     //             SgAsmTypeQuadWord
     //             SgAsmTypeDoubleQuadWord
     //             SgAsmType80bitFloat
     //             SgAsmType128bitFloat
     //             SgAsmTypeSingleFloat
     //             SgAsmTypeDoubleFloat
     //             SgAsmTypeVector
     //             SgAsmExecutableFileFormat
     //             SgAsmGenericDLL
     //             SgAsmGenericFormat
     //             SgAsmGenericDLLList
     //             SgAsmElfEHFrameEntryFD
     //             SgAsmGenericFile
     //             SgAsmGenericSection
     //             SgAsmGenericHeader
     //             SgAsmPEFileHeader
     //             SgAsmLEFileHeader
     //             SgAsmNEFileHeader
     //             SgAsmDOSFileHeader
     //             SgAsmElfFileHeader
     //             SgAsmElfSection
     //             SgAsmElfSymbolSection
     //             SgAsmElfRelocSection
     //             SgAsmElfDynamicSection
     //             SgAsmElfStringSection
     //             SgAsmElfNoteSection
     //             SgAsmElfEHFrameSection
     //             SgAsmElfSectionTable
     //             SgAsmElfSegmentTable
     //             SgAsmPESection
     //             SgAsmPEImportSection
     //             SgAsmPEExportSection
     //             SgAsmPEStringSection
     //             SgAsmPESectionTable
     //             SgAsmDOSExtendedHeader
     //             SgAsmCoffSymbolTable
     //             SgAsmNESection
     //             SgAsmNESectionTable
     //             SgAsmNENameTable
     //             SgAsmNEModuleTable
     //             SgAsmNEStringTable
     //             SgAsmNEEntryTable
     //             SgAsmNERelocTable
     //             SgAsmLESection
     //             SgAsmLESectionTable
     //             SgAsmLENameTable
     //             SgAsmLEPageTable
     //             SgAsmLEEntryTable
     //             SgAsmLERelocTable
     //             SgAsmGenericSymbol
     //             SgAsmCoffSymbol
     //             SgAsmElfSymbol
     //             SgAsmGenericStrtab
     //             SgAsmElfStrtab
     //             SgAsmCoffStrtab
     //             SgAsmGenericSymbolList
     //             SgAsmGenericSectionList
     //             SgAsmGenericHeaderList
     //             SgAsmGenericString
     //             SgAsmBasicString
     //             SgAsmStoredString
     //             SgAsmElfSectionTableEntry
     //             SgAsmElfSegmentTableEntry
     //             SgAsmElfSymbolList
     //             SgAsmPEImportILTEntry
     //             SgAsmElfRelocEntry
     //             SgAsmElfRelocEntryList
     //             SgAsmPEExportEntry
     //             SgAsmPEExportEntryList
     //             SgAsmElfDynamicEntry
     //             SgAsmElfDynamicEntryList
     //             SgAsmElfSegmentTableEntryList
     //             SgAsmStringStorage
     //             SgAsmElfNoteEntry
     //             SgAsmElfNoteEntryList
     //             SgAsmPEImportDirectory
     //             SgAsmPEImportHNTEntry
     //             SgAsmPESectionTableEntry
     //             SgAsmPEExportDirectory
     //             SgAsmPERVASizePair
     //             SgAsmCoffSymbolList
     //             SgAsmPERVASizePairList
     //             SgAsmElfEHFrameEntryCI
     //             SgAsmPEImportHNTEntryList
     //             SgAsmPEImportILTEntryList
     //             SgAsmPEImportLookupTable
     //             SgAsmPEImportDirectoryList
     //             SgAsmNEEntryPoint
     //             SgAsmNERelocEntry
     //             SgAsmNESectionTableEntry
     //             SgAsmElfEHFrameEntryCIList
     //             SgAsmLEPageTableEntry
     //             SgAsmLEEntryPoint
     //             SgAsmLESectionTableEntry
     //             SgAsmElfEHFrameEntryFDList
     //             SgAsmDwarfInformation
     //             SgAsmDwarfMacro
     //             SgAsmDwarfMacroList
     //             SgAsmDwarfLine
     //             SgAsmDwarfLineList
     //             SgAsmDwarfCompilationUnitList
     //             SgAsmDwarfConstruct
     //             SgAsmDwarfArrayType
     //             SgAsmDwarfClassType
     //             SgAsmDwarfEntryPoint
     //             SgAsmDwarfEnumerationType
     //             SgAsmDwarfFormalParameter
     //             SgAsmDwarfImportedDeclaration
     //             SgAsmDwarfLabel
     //             SgAsmDwarfLexicalBlock
     //             SgAsmDwarfMember
     //             SgAsmDwarfPointerType
     //             SgAsmDwarfReferenceType
     //             SgAsmDwarfCompilationUnit
     //             SgAsmDwarfStringType
     //             SgAsmDwarfStructureType
     //             SgAsmDwarfSubroutineType
     //             SgAsmDwarfTypedef
     //             SgAsmDwarfUnionType
     //             SgAsmDwarfUnspecifiedParameters
     //             SgAsmDwarfVariant
     //             SgAsmDwarfCommonBlock
     //             SgAsmDwarfCommonInclusion
     //             SgAsmDwarfInheritance
     //             SgAsmDwarfInlinedSubroutine
     //             SgAsmDwarfModule
     //             SgAsmDwarfPtrToMemberType
     //             SgAsmDwarfSetType
     //             SgAsmDwarfSubrangeType
     //             SgAsmDwarfWithStmt
     //             SgAsmDwarfAccessDeclaration
     //             SgAsmDwarfBaseType
     //             SgAsmDwarfCatchBlock
     //             SgAsmDwarfConstType
     //             SgAsmDwarfConstant
     //             SgAsmDwarfEnumerator
     //             SgAsmDwarfFileType
     //             SgAsmDwarfFriend
     //             SgAsmDwarfNamelist
     //             SgAsmDwarfNamelistItem
     //             SgAsmDwarfPackedType
     //             SgAsmDwarfSubprogram
     //             SgAsmDwarfTemplateTypeParameter
     //             SgAsmDwarfTemplateValueParameter
     //             SgAsmDwarfThrownType
     //             SgAsmDwarfTryBlock
     //             SgAsmDwarfVariantPart
     //             SgAsmDwarfVariable
     //             SgAsmDwarfVolatileType
     //             SgAsmDwarfDwarfProcedure
     //             SgAsmDwarfRestrictType
     //             SgAsmDwarfInterfaceType
     //             SgAsmDwarfNamespace
     //             SgAsmDwarfImportedModule
     //             SgAsmDwarfUnspecifiedType
     //             SgAsmDwarfPartialUnit
     //             SgAsmDwarfImportedUnit
     //             SgAsmDwarfMutableType
     //             SgAsmDwarfCondition
     //             SgAsmDwarfSharedType
     //             SgAsmDwarfFormatLabel
     //             SgAsmDwarfFunctionTemplate
     //             SgAsmDwarfClassTemplate
     //             SgAsmDwarfUpcSharedType
     //             SgAsmDwarfUpcStrictType
     //             SgAsmDwarfUpcRelaxedType
     //             SgAsmDwarfUnknownConstruct
     //             SgAsmDwarfConstructList

     return;
}


void CodeGeneratorVisitor::postOrderVisit(SgNode *node) {
     if (option.isDebugPostTraversal()) {
         cerr << "CodeGenerator Visitor Post-processing: " << node -> class_name() << endl;  // Used for Debugging
         cerr.flush();
     }

     /**
      * Check for suspension of visit and take apropriate action.
      */
     if (visit_suspended_by_node) { 
         if (visit_suspended_by_node == node) { // If visiting was suspended by this node, resume visiting.
             visit_suspended_by_node = NULL;
         }
         else return;
     }

     /**
      * If the node in question is an expression node that contains an operand that needs to be promoted,
      * we do so here.  This is needed because of the pointer difference bug in Rose.
      * TODO: Remove this code when the bug is fixed.
      */
     if (node -> attributeExists(Control::LLVM_LEFT_OPERAND_INTEGRAL_PROMOTION)) {
         SgBinaryOp *n = isSgBinaryOp(node);
         SgExpression *lhs_operand = n -> get_lhs_operand();
         SgType *type = attributes -> getExpressionType(n),
                *lhs_type = attributes -> getExpressionType(lhs_operand);
         string promote_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_LEFT_OPERAND_INTEGRAL_PROMOTION)) -> getValue(),
                type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                lhs_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (isUnsignedType(type) ? "zext " : "sext ")
                    << lhs_type_name << " " << lhs_name << " to " << type_name << endl;
         ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> resetValue(promote_name);
     }
     else if (node -> attributeExists(Control::LLVM_RIGHT_OPERAND_INTEGRAL_PROMOTION)) {
         SgBinaryOp *n = isSgBinaryOp(node);
         SgExpression *rhs_operand = n -> get_rhs_operand();
         SgType *type = attributes -> getExpressionType(n),
                *rhs_type = attributes -> getExpressionType(rhs_operand);
         string promote_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_RIGHT_OPERAND_INTEGRAL_PROMOTION)) -> getValue(),
                type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                rhs_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                rhs_type_name = ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (isUnsignedType(type) ? "zext " : "sext ")
                    << rhs_type_name << " " << rhs_name << " to " << type_name << endl;
         ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> resetValue(promote_name);
     }


     /**
      * The main switch:
      */
     // SgNode:
     if (false)
         ;
     //     SgSupport:
     //         SgModifier:
     //             SgModifierNodes
     //             SgConstVolatileModifier
     //             SgStorageModifier
     //             SgAccessModifier
     //             SgFunctionModifier
     //             SgUPC_AccessModifier
     //             SgSpecialFunctionModifier
     //             SgElaboratedTypeModifier
     //             SgLinkageModifier
     //             SgBaseClassModifier
     //             SgTypeModifier
     //             SgDeclarationModifier
     //         SgName
     //         SgSymbolTable
     //         SgInitializedName
     else if (dynamic_cast<SgInitializedName *>(node)) {
         SgInitializedName *n = isSgInitializedName(node);
         SgInitializer *init = n -> get_initializer();
         if (init && (! n -> attributeExists(Control::LLVM_GLOBAL_DECLARATION))) {
             SgType *var_type = n -> get_type();
             string type_name = ((StringAstAttribute *) var_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    var_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();

             /**
              * if this declaration is a local declaration and it has an initializer, then 
              * generate the initialization code here.
              */
             if (dynamic_cast<SgAssignInitializer *>(init)) {
                 SgAssignInitializer *assign_init = isSgAssignInitializer(init);
                 string rhs_name = ((StringAstAttribute *) assign_init -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();

                 SgArrayType *array_type = isSgArrayType(attributes -> getSourceType(var_type));
                 if (array_type) {
                     SgStringVal *str = isSgStringVal(assign_init -> get_operand()); // the string being copied may be shorter than the target.
                     string target_bit_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue();
                     string aggregate_type_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     string subtype_name = ((StringAstAttribute *) array_type -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     int string_size = ((IntAstAttribute *) n -> getAttribute(Control::LLVM_STRING_SIZE)) -> getValue(); 
                     (*codeOut) << CodeEmitter::indent() << target_bit_name << " = bitcast " << aggregate_type_name << "* " << var_name << " to " << subtype_name << "*" <<  endl;
                     (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.i32(i8* " << target_bit_name << ", " << subtype_name << "* " << rhs_name
                                << ", i32 " << string_size << ", i32 1)" << endl;
                 }
                 else if (isSgClassType(attributes -> getSourceType(var_type))) {
                     string target_bit_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue();
                     int size = ((IntAstAttribute *) var_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << target_bit_name << " = bitcast " << type_name << "* " << var_name << " to i8*" << endl;
                     (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.i32(i8* " << target_bit_name << ", i8* " << rhs_name << ", i32 " << size << ", i32 4)" << endl;
                 }
                 else {
                     if (dynamic_cast<SgFunctionRefExp *>(assign_init -> get_operand())) {
                         SgFunctionRefExp *function = isSgFunctionRefExp(assign_init -> get_operand());
                         string function_name = ((StringAstAttribute *) function -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                                function_type_name = ((StringAstAttribute *) attributes -> getExpressionType(function) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                         (*codeOut) << CodeEmitter::indent() << "store " << type_name
                                    << " bitcast (" << function_type_name << "* " << function_name << " to " << type_name << "), "
                                    << type_name << "* " << var_name << endl;
                     }
                     else {
                         (*codeOut) << CodeEmitter::indent() << "store " << type_name << " " << rhs_name << ", " << type_name << "* " << var_name << endl;
                     }
                 }
             }
             else if (dynamic_cast<SgAggregateInitializer *>(init)) {
                 SgAggregateInitializer *aggregate = isSgAggregateInitializer(init);
                 genLocalAggregateInitialization(var_name, aggregate);
             }
             else if (init != NULL){
                 cerr << "Don't know yet how to process initializer of type: " << init -> class_name() << endl;
                 cerr.flush();
                 assert(0);
             }
         }
     }
     //         SgAttribute:
     //             SgPragma
     //             SgBitAttribute:
     //                 SgFuncDecl_attr
     //                 SgClassDecl_attr
     //         Sg_File_Info
     //         SgFile:
     //             SgSourceFile
     else if (dynamic_cast<SgSourceFile *> (node)) {
         SgSourceFile *n = isSgSourceFile(node);

         if (attributes -> numAdditionalFunctions() > 0) {
             revisitAttributes.push_back(attributes);
         }
     }
     //             SgBinaryFile
     //             SgUnknownFile
     //         SgProject
     //         SgOptions
     //         SgUnparse_Info
     //         SgBaseClass
     //         SgTypedefSeq
     //         SgTemplateParameter
     //         SgTemplateArgument
     //         SgDirectory
     //         SgFileList
     //         SgDirectoryList
     //         SgFunctionParameterTypeList
     //         SgQualifiedName
     //         SgTemplateArgumentList
     //         SgTemplateParameterList
     //         SgGraph:
     //             SgIncidenceDirectedGraph:
     //                 SgBidirectionalGraph:
     //                     SgStringKeyedBidirectionalGraph
     //                     SgIntKeyedBidirectionalGraph
     //             SgIncidenceUndirectedGraph
     //         SgGraphNode
     //         SgGraphEdge:
     //             SgDirectedGraphEdge
     //             SgUndirectedGraphEdge
     //         SgGraphNodeList
     //         SgGraphEdgeList
     //         SgNameGroup
     //         SgCommonBlockObject
     //         SgDimensionObject
     //         SgFormatItem
     //         SgFormatItemList
     //         SgDataStatementGroup
     //         SgDataStatementObject
     //         SgDataStatementValue
     //     SgType:
     //         SgTypeUnknown
     //         SgTypeChar
     //         SgTypeSignedChar
     //         SgTypeUnsignedChar
     //         SgTypeShort
     //         SgTypeSignedShort
     //         SgTypeUnsignedShort
     //         SgTypeInt
     //         SgTypeSignedInt
     //         SgTypeUnsignedInt
     //         SgTypeLong
     //         SgTypeSignedLong
     //         SgTypeUnsignedLong
     //         SgTypeVoid
     //         SgTypeGlobalVoid
     //         SgTypeWchar
     //         SgTypeFloat
     //         SgTypeDouble
     //         SgTypeLongLong
     //         SgTypeSignedLongLong
     //         SgTypeUnsignedLongLong
     //         SgTypeLongDouble
     //         SgTypeString
     //         SgTypeBool
     //         SgPointerType:
     //             SgPointerMemberType
     //         SgReferenceType
     //         SgNamedType:
     //             SgClassType
     //             SgEnumType
     //             SgTypedefType
     //         SgModifierType
     //         SgFunctionType:
     //             SgMemberFunctionType:
     //                 SgPartialFunctionType:
     //                     SgPartialFunctionModifierType
     //         SgArrayType
     //         SgTypeEllipse
     //         SgTemplateType
     //         SgQualifiedNameType
     //         SgTypeComplex
     //         SgTypeImaginary
     //         SgTypeDefault
     //     SgLocatedNode:
     //         SgStatement:
     //             SgScopeStatement:
     //                 SgGlobal
     else if (dynamic_cast<SgGlobal *>(node)) {
         SgGlobal *n = isSgGlobal(node);
     }
     //                 SgBasicBlock
     else if (dynamic_cast<SgBasicBlock *>(node)) {
         SgBasicBlock *n = isSgBasicBlock(node);
     }
     //                 SgIfStmt
     else if (dynamic_cast<SgIfStmt *> (node)) {
         SgIfStmt *n = isSgIfStmt(node);
         IfAstAttribute *attribute = (IfAstAttribute *) n -> getAttribute(Control::LLVM_IF_LABELS);
         codeOut -> emitLabel(current_function_decls, attribute -> getEndLabel());
     }
     //                 SgForStatement
     else if (dynamic_cast<SgForStatement *> (node)) {
         SgForStatement *n = isSgForStatement(node);

         assert(scopeStack.top() == n);
         scopeStack.pop();

         ForAstAttribute *attribute = (ForAstAttribute *) n -> getAttribute(Control::LLVM_FOR_LABELS);

         /**
          * If this for-statement had increment statements generate code that had been buffered for them here.
          */
         if (! isSgNullExpression(n -> get_increment())) {
             codeOut -> emitUnconditionalBranch(attribute -> getIncrementLabel());
             codeOut -> emitLabel(current_function_decls, attribute -> getIncrementLabel());
             codeOut -> flushTopBuffer();
         }

         codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel());
         codeOut -> emitLabel(current_function_decls, attribute -> getEndLabel());
     }
     //                 SgFunctionDefinition
     else if (dynamic_cast<SgFunctionDefinition *>(node)) {
         SgFunctionDefinition *n = isSgFunctionDefinition(node);
     }
     //                 SgClassDefinition:
     else if (dynamic_cast<SgClassDefinition *>(node)) {
         SgClassDefinition *n = isSgClassDefinition(node);
     }
     //                     SgTemplateInstantiationDefn
     //                 SgWhileStmt
     else if (dynamic_cast<SgWhileStmt *> (node)) {
         SgWhileStmt *n = isSgWhileStmt(node);

         assert(scopeStack.top() == n);
         scopeStack.pop();

         WhileAstAttribute *attribute = (WhileAstAttribute *) n -> getAttribute(Control::LLVM_WHILE_LABELS);
         codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel());
         codeOut -> emitLabel(current_function_decls, attribute -> getEndLabel());
     }
     //                 SgDoWhileStmt
     else if (dynamic_cast<SgDoWhileStmt *> (node)) {
         SgDoWhileStmt *n = isSgDoWhileStmt(node);

         assert(scopeStack.top() == n);
         scopeStack.pop();
     }
     //                 SgSwitchStatement
     else if (dynamic_cast<SgSwitchStatement *>(node)) {
         SgSwitchStatement *n = isSgSwitchStatement(node);

         assert(scopeStack.top() == n);
         scopeStack.pop();

         assert(switchStack.top() == n);
         switchStack.pop();

         SwitchAstAttribute *switch_attribute = (SwitchAstAttribute *) n -> getAttribute(Control::LLVM_SWITCH_INFO);
         codeOut -> emitUnconditionalBranch(switch_attribute -> getEndLabel());
         codeOut -> emitLabel(current_function_decls, switch_attribute -> getEndLabel());
     }
     //                 SgCatchOptionStmt
     //                 SgNamespaceDefinitionStatement
     //                 SgBlockDataStatement
     //                 SgAssociateStatement
     //                 SgFortranDo:
     //                     SgFortranNonblockedDo
     //                 SgForAllStatement
     //                 SgUpcForAllStatement
     //             SgFunctionTypeTable
     //             SgDeclarationStatement:
     //                 SgFunctionParameterList
     else if (dynamic_cast<SgFunctionParameterList *>(node)) {
         SgFunctionParameterList *n = isSgFunctionParameterList(node);
     }
     //                 SgVariableDeclaration
     else if (dynamic_cast<SgVariableDeclaration *>(node)) {
         SgVariableDeclaration *n = isSgVariableDeclaration(node);
     }
     //                 SgVariableDefinition
     //                 SgClinkageDeclarationStatement:
     //                     SgClinkageStartStatement
     //                     SgClinkageEndStatement
     //             SgEnumDeclaration
     else if (dynamic_cast<SgEnumDeclaration *>(node)) {
         SgEnumDeclaration *n = isSgEnumDeclaration(node);
     }
     //             SgAsmStmt
     //             SgAttributeSpecificationStatement
     //             SgFormatStatement
     //             SgTemplateDeclaration
     //             SgTemplateInstantiationDirectiveStatement
     //             SgUseStatement
     //             SgParameterStatement
     //             SgNamespaceDeclarationStatement
     //             SgEquivalenceStatement
     //             SgInterfaceStatement
     //             SgNamespaceAliasDeclarationStatement
     //             SgCommonBlock
     //             SgTypedefDeclaration
     else if (dynamic_cast<SgTypedefDeclaration *>(node)) {
         SgTypedefDeclaration *n = isSgTypedefDeclaration(node);
     }
     //             SgStatementFunctionStatement
     //             SgCtorInitializerList
     //             SgPragmaDeclaration
     //             SgUsingDirectiveStatement
     //             SgClassDeclaration:
     else if (dynamic_cast<SgClassDeclaration *>(node)) {
         SgClassDeclaration *n = isSgClassDeclaration(node);
     }
     //                 SgTemplateInstantiationDecl
     //                 SgDerivedTypeStatement
     //                 SgModuleStatement
     //             SgImplicitStatement
     //             SgUsingDeclarationStatement
     //             SgNamelistStatement
     //             SgImportStatement
     //             SgFunctionDeclaration:
     else if (dynamic_cast<SgFunctionDeclaration *>(node)) {
         SgFunctionDeclaration *n = isSgFunctionDeclaration(node);

         if (n -> get_definition() &&  // A function header with a definition that should not be ignored because ...
             ((! option.isQuery()) || n -> attributeExists(Control::LLVM_TRANSLATE)) &&
             (! n -> attributeExists(Control::LLVM_IGNORE))) {
             /**
              * If the function only has more than one return statement at the end of its body, then we
              * don't need to create a separate "return" basic block here.
              */
             string return_label = current_function_decls -> getReturnLabel();
             if (current_function_decls -> numLabelPredecessors(return_label) > 0) {
                 codeOut -> emitUnconditionalBranch(return_label);
                 codeOut -> emitLabel(current_function_decls, return_label);
             }

             SgType *return_type = attributes -> getSourceType(n -> get_type() -> get_return_type());
             if (isSgTypeVoid(return_type)) {
                 (*codeOut) << CodeEmitter::indent() << "ret void" << endl;
             }
             else {
                 string type_name = ((StringAstAttribute *) n -> get_type() -> get_return_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                        return_name = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);

                 if (isSgClassType(return_type)) {
                     int size = attributes -> integralStructureType(return_type);
                     if (size) {
                         stringstream out;
                         out << "i" << (size * 8);
                         string cast_name = attributes -> getTemp(LLVMAstAttributes::TEMP_INT),
                                cast_return_name = attributes -> getTemp(LLVMAstAttributes::TEMP_INT),
                                cast_type_name = out.str();
                         (*codeOut) << CodeEmitter::indent() << cast_name << " = bitcast " << type_name << "* " << "%.retval to " << cast_type_name << "*" << endl;
                         (*codeOut) << CodeEmitter::indent() << cast_return_name << " = load " << cast_type_name << "* " << cast_name << ", align 1" << endl;
                         type_name = cast_type_name;
                         return_name = cast_return_name;
                     }
                     else {
                         type_name = "void";
                         return_name = "";
                     }
                 }
                 else {
                     (*codeOut) << CodeEmitter::indent() << return_name << " = load " << type_name << "* " << "%.retval" << endl;
                 }
                 (*codeOut) << CodeEmitter::indent() << "ret " << type_name << " " << return_name << endl;
             }
             (*codeOut) << "}" << endl;

             current_function_decls = NULL; // done with this function
         }
     }
     //                 SgMemberFunctionDeclaration:
     //                     SgTemplateInstantiationMemberFunctionDecl
     //                 SgTemplateInstantiationFunctionDecl
     //                 SgProgramHeaderStatement
     //                 SgProcedureHeaderStatement
     //                 SgEntryStatement
     //             SgContainsStatement
     //             SgC_PreprocessorDirectiveStatement:
     //                 SgIncludeDirectiveStatement
     //                 SgDefineDirectiveStatement
     //                 SgUndefDirectiveStatement
     //                 SgIfdefDirectiveStatement
     //                 SgIfndefDirectiveStatement
     //                 SgIfDirectiveStatement
     //                 SgDeadIfDirectiveStatement
     //                 SgElseDirectiveStatement
     //                 SgElseifDirectiveStatement
     //                 SgEndifDirectiveStatement
     //                 SgLineDirectiveStatement
     //                 SgWarningDirectiveStatement
     //                 SgErrorDirectiveStatement
     //                 SgEmptyDirectiveStatement
     //                 SgIncludeNextDirectiveStatement
     //                 SgIdentDirectiveStatement
     //                 SgLinemarkerDirectiveStatement
     //             SgOmpThreadprivateStatement
     //             SgFortranIncludeLine
     //             SgExprStatement
     else if (dynamic_cast<SgExprStatement *>(node)) {
         SgExprStatement *n = isSgExprStatement(node);

         /**
          * The test expression in an IfStmt, WhileStmt, DoStatement and ForStatement,...and the item_selector in a SwitchStatement is wrapped in an SgExprStatement
          */
         if (n -> attributeExists(Control::LLVM_IF_LABELS)) {
             IfAstAttribute *attribute = (IfAstAttribute *) n -> getAttribute(Control::LLVM_IF_LABELS);
             string name = ((StringAstAttribute *) n -> get_expression() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getTrueLabel() << ", label %" << attribute -> getFalseLabel() << endl;
         }
         else if (n -> attributeExists(Control::LLVM_WHILE_LABELS)) {
             WhileAstAttribute *attribute = (WhileAstAttribute *) n -> getAttribute(Control::LLVM_WHILE_LABELS);
             string name = ((StringAstAttribute *) n -> get_expression() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getBodyLabel() << ", label %" << attribute -> getEndLabel() << endl;
             codeOut -> emitLabel(current_function_decls, attribute -> getBodyLabel());
         }
         else if (n -> attributeExists(Control::LLVM_DO_LABELS)) {
             DoAstAttribute *attribute = (DoAstAttribute *) n -> getAttribute(Control::LLVM_DO_LABELS);
             string name = ((StringAstAttribute *) n -> get_expression() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getBodyLabel() << ", label %" << attribute -> getEndLabel() << endl;
             codeOut -> emitLabel(current_function_decls, attribute -> getEndLabel());
         }
         else if (n -> attributeExists(Control::LLVM_FOR_LABELS)) {
             ForAstAttribute *attribute = (ForAstAttribute *) n -> getAttribute(Control::LLVM_FOR_LABELS);
             if (! isSgNullExpression(n -> get_expression())) { // if a conditional expression was present
                 string name = ((StringAstAttribute *) n -> get_expression() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getBodyLabel() << ", label %" << attribute -> getEndLabel() << endl;
             }
             else {
                 codeOut -> emitUnconditionalBranch(attribute -> getBodyLabel());
             }
             codeOut -> emitLabel(current_function_decls, attribute -> getBodyLabel());
         }
         else if (n -> attributeExists(Control::LLVM_SWITCH_EXPRESSION)) {
             SwitchAstAttribute *switch_attribute = (SwitchAstAttribute *) switchStack.top() -> getAttribute(Control::LLVM_SWITCH_INFO);
             SgDefaultOptionStmt *default_stmt = (SgDefaultOptionStmt *) switch_attribute -> getDefaultStmt();
             string expr_name = ((StringAstAttribute *) n -> get_expression() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                    expr_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_expression()) -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    end_label = (default_stmt
                                        ?  ((StringAstAttribute *) default_stmt -> getAttribute(Control::LLVM_DEFAULT_LABEL)) -> getValue()
                                        : switch_attribute -> getEndLabel());
             (*codeOut) << CodeEmitter::indent() << "switch " << expr_type_name << " " << expr_name << ", label %" << end_label << " [" << endl;
             for (int i = 0; i < switch_attribute -> numCaseAttributes(); i++) {
                 CaseAstAttribute *case_attribute = switch_attribute -> getCaseAttribute(i);
                 (*codeOut) << CodeEmitter::indent() << "       " << expr_type_name << " "  << case_attribute -> getKey() << ", label %" << case_attribute -> getCaseLabel() << endl;
             }
             (*codeOut) << CodeEmitter::indent() << "]" << endl;
         }
     }
     //             SgLabelStatement
     else if (dynamic_cast<SgLabelStatement *>(node)) {
         SgLabelStatement *n = isSgLabelStatement(node);
         codeOut -> emitUnconditionalBranch(n -> get_label().getString());
         codeOut -> emitLabel(current_function_decls, n -> get_label().getString());
     }
     //             SgCaseOptionStmt
     else if (dynamic_cast<SgCaseOptionStmt *>(node)) {
         SgCaseOptionStmt *n = isSgCaseOptionStmt(node);
     }
     //             SgTryStmt
     //             SgDefaultOptionStmt
     else if (dynamic_cast<SgDefaultOptionStmt *>(node)) {
         SgDefaultOptionStmt *n = isSgDefaultOptionStmt(node);
     }
     //             SgBreakStmt
     else if (dynamic_cast<SgBreakStmt *>(node)) {
         SgBreakStmt *n = isSgBreakStmt(node);
         SgScopeStatement *scope = scopeStack.top();
         if (dynamic_cast<SgForStatement *>(scope)) {
             SgForStatement *for_stmt = isSgForStatement(scope);
             ForAstAttribute *attribute = (ForAstAttribute *) for_stmt -> getAttribute(Control::LLVM_FOR_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel());
         }
         else if (dynamic_cast<SgWhileStmt *>(scope)) {
             SgWhileStmt *while_stmt = isSgWhileStmt(scope);
             WhileAstAttribute *attribute = (WhileAstAttribute *) while_stmt -> getAttribute(Control::LLVM_WHILE_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel());
         }
         else if (dynamic_cast<SgDoWhileStmt *>(scope)) {
             SgDoWhileStmt *do_stmt = isSgDoWhileStmt(scope);
             DoAstAttribute *attribute = (DoAstAttribute *) do_stmt -> getAttribute(Control::LLVM_DO_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel());
         }
         else if (dynamic_cast<SgSwitchStatement *>(scope)) {
             SgSwitchStatement *switch_stmt = isSgSwitchStatement(scope);
             SwitchAstAttribute *attribute = (SwitchAstAttribute *) switch_stmt -> getAttribute(Control::LLVM_SWITCH_INFO);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel());
         }
     }
     //             SgContinueStmt
     else if (dynamic_cast<SgContinueStmt *>(node)) {
         SgContinueStmt *n = isSgContinueStmt(node);
         SgScopeStatement *scope = scopeStack.top();
         if (dynamic_cast<SgForStatement *>(scope)) {
             SgForStatement *for_stmt = isSgForStatement(scope);
             ForAstAttribute *attribute = (ForAstAttribute *) for_stmt -> getAttribute(Control::LLVM_FOR_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getIncrementLabel());
         }
         else if (dynamic_cast<SgWhileStmt *>(scope)) {
             SgWhileStmt *while_stmt = isSgWhileStmt(scope);
             WhileAstAttribute *attribute = (WhileAstAttribute *) while_stmt -> getAttribute(Control::LLVM_WHILE_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel());
         }
         else if (dynamic_cast<SgDoWhileStmt *>(scope)) {
             SgDoWhileStmt *do_stmt = isSgDoWhileStmt(scope);
             DoAstAttribute *attribute = (DoAstAttribute *) do_stmt -> getAttribute(Control::LLVM_DO_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel());
         }
     }
     //             SgReturnStmt
     else if (dynamic_cast<SgReturnStmt *>(node)) {
         SgReturnStmt *n = isSgReturnStmt(node);
         SgExpression *expression = n -> get_expression();
         if (expression) {
             SgType *return_type = current_function_decls -> getFunctionType() -> get_return_type();
             string return_type_name = ((StringAstAttribute *) return_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    return_name = ((StringAstAttribute *) expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();

             /**
              * For a function that returns a stucture, ...
              */
             if (isSgClassType(attributes -> getSourceType(return_type))) {
                 string return_variable_name = (attributes -> integralStructureType(return_type) ? "%.retval" : "%agg.result"),
                        return_bit_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue();
                 int size = ((IntAstAttribute *) return_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << return_bit_name << " = bitcast " << return_type_name << "* " << return_variable_name <<" to i8*" << endl;
                 (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.i32(i8* " << return_bit_name << ", i8* " << return_name << ", i32 " << size << ", i32 4)" << endl;
             }
             else {
                 (*codeOut) << CodeEmitter::indent() << "store " << return_type_name << " " << return_name << ", " << return_type_name << "* %.retval" << endl;
             }
         }

         /**
          * If the function only has one return statement and after executing the body of the function, control
          * falls directly into the return statement then there is no need to emit a branch.
          */
         string return_label = current_function_decls -> getReturnLabel();
         if (current_function_decls -> numLabelPredecessors(return_label) > 0) {
             codeOut -> emitUnconditionalBranch(return_label);
         }
     }
     //             SgGotoStatement
     else if (dynamic_cast<SgGotoStatement *> (node)) {
         SgGotoStatement *n = isSgGotoStatement(node);
         /**
          * The casts were added below to avoid confusion. Note that an SgGotoStatement contains a get_label()
          * metho that returns an SgLabelStatement. An SgLabelStatement also contains a get_label() method that
          * returns an SgName (name of the actual label).
          */
         codeOut -> emitUnconditionalBranch(((SgName) ((SgLabelStatement *) n -> get_label()) -> get_label()).getString());
     }
     //             SgSpawnStmt
     //             SgNullStatement
     //             SgVariantStatement
     //             SgForInitStatement
     else if (dynamic_cast<SgForInitStatement *>(node)) {
         SgForInitStatement *n = isSgForInitStatement(node);
         ForAstAttribute *attribute = (ForAstAttribute *) n -> getAttribute(Control::LLVM_FOR_LABELS);
         codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel());
         codeOut -> emitLabel(current_function_decls, attribute -> getConditionLabel());
     }
     //             SgCatchStatementSeq
     //             SgStopOrPauseStatement
     //             SgIOStatement:
     //                 SgPrintStatement
     //                 SgReadStatement
     //                 SgWriteStatement
     //                 SgOpenStatement
     //                 SgCloseStatement
     //                 SgInquireStatement
     //                 SgFlushStatement
     //                 SgBackspaceStatement
     //                 SgRewindStatement
     //                 SgEndfileStatement
     //                 SgWaitStatement
     //             SgWhereStatement
     //             SgElseWhereStatement
     //             SgNullifyStatement
     //             SgArithmeticIfStatement
     //             SgAssignStatement
     //             SgComputedGotoStatement
     //             SgAssignedGotoStatement
     //             SgAllocateStatement
     //             SgDeallocateStatement
     //             SgUpcNotifyStatement
     //             SgUpcWaitStatement
     //             SgUpcBarrierStatement
     //             SgUpcFenceStatement
     //             SgOmpBarrierStatement
     //             SgOmpTaskwaitStatement
     //             SgOmpFlushStatement
     //             SgOmpBodyStatement:
     //                 SgOmpAtomicStatement
     //                 SgOmpMasterStatement
     //                 SgOmpOrderedStatement
     //                 SgOmpCriticalStatement
     //                 SgOmpSectionStatement
     //                 SgOmpWorkshareStatement
     //                 SgOmpClauseBodyStatement:
     //                     SgOmpParallelStatement
     //                     SgOmpSingleStatement
     //                     SgOmpTaskStatement
     //                     SgOmpForStatement
     //                     SgOmpDoStatement
     //                     SgOmpSectionsStatement
     //             SgSequenceStatement
     //         SgExpression:
     //             SgUnaryOp:
     //                 SgExpressionRoot
     //                 SgMinusOp
     else if (dynamic_cast<SgMinusOp *> (node)) {
         SgMinusOp *n = isSgMinusOp(node);
         SgType *type = n -> get_type();
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                default_value = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << result_name << " = " << (isFloatType(n -> get_type()) ? "f" : "") << "sub" << " "
                    <<  type_name << " " << default_value << ", " << operand_name << endl;
     }
     //                 SgUnaryAddOp
     else if (dynamic_cast<SgUnaryAddOp *>(node)) {
         SgUnaryAddOp *n = isSgUnaryAddOp(node);
         // No need to do anything here.
     }
     //                 SgNotOp
     else if (dynamic_cast<SgNotOp *>(node)) {
         SgNotOp *n = isSgNotOp(node);
         /**
          * Since Rose transforms the NotOp operation into a NotEqual operation.  We simply need to 
          * flip the bit result and zero-extend the operand into the size of the resulting integral type.
          */
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << result_name << " = xor i1 " << operand_name << ", true" << endl;
     }
     //                 SgPointerDerefExp
     else if (dynamic_cast<SgPointerDerefExp *>(node)) {
         SgPointerDerefExp *n = isSgPointerDerefExp(node);
         if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
             string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                    operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                    result_type_name = ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             if (dynamic_cast<SgClassType *> (attributes -> getSourceType(n -> get_type()))) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << result_type_name << "* " << operand_name << " to i8*" << endl;
             }
             else {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = load " << result_type_name << "* " << operand_name << endl;
             }
         }
     }
     //                 SgAddressOfOp
     else if (dynamic_cast<SgAddressOfOp *>(node)) {
         SgAddressOfOp *n = isSgAddressOfOp(node);
         // No need to do anything here.
     }
     //                 SgMinusMinusOp
     else if (dynamic_cast<SgMinusMinusOp *>(node)) {
         SgMinusMinusOp *n = isSgMinusMinusOp(node);
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                type_name = ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                ref_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue();
         if (isSgPointerType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_operand()))) ||
             isSgArrayType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_operand())))) {
             (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr " << type_name << " " <<  name << ", i32 -1" << endl;
         }
         else {
             (*codeOut) << CodeEmitter::indent() << result_name << " = " << (isFloatType(n -> get_type()) ? "fsub " : "sub ") << type_name << " " << name << ", 1" << endl;
         }
         (*codeOut) << CodeEmitter::indent() << "store " << type_name << " " << result_name << ", " << type_name << "* " << ref_name << endl;
     }
     //                 SgPlusPlusOp
     else if (dynamic_cast<SgPlusPlusOp *>(node)) {
         SgPlusPlusOp *n = isSgPlusPlusOp(node);
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                type_name = ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                ref_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue();
         if (isSgPointerType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_operand()))) ||
             isSgArrayType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_operand())))) {
             (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr " << type_name << " " <<  name << ", i32 1" << endl;
         }
         else {
             (*codeOut) << CodeEmitter::indent() << result_name << " = " << (isFloatType(n -> get_type()) ? "fadd " : "add ") << type_name << " " << name << ", 1" << endl;
         }
         (*codeOut) << CodeEmitter::indent() << "store " << type_name << " " << result_name << ", " << type_name << "* " << ref_name << endl;
     }
     //                 SgBitComplementOp
     else if (dynamic_cast<SgBitComplementOp *>(node)) {
         SgBitComplementOp *n = isSgBitComplementOp(node);
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                operand_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_operand()) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << result_name << " = xor " << operand_type_name << " " << operand_name << ", -1" << endl;
     }
     //                 SgCastExp
     else if (dynamic_cast<SgCastExp *>(node)) {
         SgCastExp *n = isSgCastExp(node);

         //
         // TODO: Factor this code into a function... if it starts getting too big!
         //

         /**
          * The following casts have already been taken care of and no code need to be generated for them.
          */
         if (isSgCharVal(n -> get_operand()) ||
             isSgUnsignedCharVal(n -> get_operand()) ||
             n -> attributeExists(Control::LLVM_NULL_VALUE) ||
             n -> attributeExists(Control::LLVM_IGNORE)) {
             // Nothing to do!
         }
         /**
          * Trivial casts are processed during the Attribute visit... They are ignored here.
          */
         else if (! n -> attributeExists(Control::LLVM_TRIVIAL_CAST)) { // process only non-trivial casts
             SgExpression *operand = n -> get_operand();
             SgType *result_type = attributes -> getSourceType(n -> get_type()),
                    *operand_type = attributes -> getSourceType(attributes -> getExpressionType(operand));

             string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                    result_type_name = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    operand_name = ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                    operand_type_name = ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

             if (result_type -> isIntegerType() || isSgEnumType(result_type)) {
                 if (dynamic_cast<SgPointerType *> (operand_type)) {
                     (*codeOut) << CodeEmitter::indent() << result_name << " = ptrtoint " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                 }
                 else if (isUnsignedType(result_type)) {
                     if (operand_type -> isIntegerType() || isSgEnumType(operand_type)) {
                         if (((IntAstAttribute *) operand_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() > 
                             ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()) {
                              (*codeOut) << CodeEmitter::indent() << result_name << " = trunc " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                         }
                         else {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = zext " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                         }
                     }
                     else if (isFloatType(operand_type)) {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = fptoui " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                     }
                     else {
                          cerr << "can't convert yet from " << operand_type -> class_name() << " to " << result_type -> class_name() << endl;
                          cerr.flush();
                          assert(0);
                     }
                 }
                 else {
                     if (operand_type -> isIntegerType() || isSgEnumType(operand_type)) {
                         if (((IntAstAttribute *) operand_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() > 
                             ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()) {
                              (*codeOut) << CodeEmitter::indent() << result_name << " = trunc " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                         }
                         else if (isUnsignedType(operand_type)) {
                              (*codeOut) << CodeEmitter::indent() << result_name << " = zext " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                         }
                         else {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = sext " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                         }
                     }
                     else if (isFloatType(operand_type)) {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = fptosi " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                     }
                     else {
                          cerr << "can't convert yet from " << operand_type -> class_name() << " to " << result_type -> class_name() << endl;
                          cerr.flush();
                          assert(0);
                     }
                 }
             }
             else if (isFloatType(result_type)) {
                 if (operand_type -> isIntegerType() || isSgEnumType(operand_type)) {
                     if (isUnsignedType(operand_type)) {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = uitofp " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                     }
                     else {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = sitofp " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                     }
                 }
                 else if (isFloatType(operand_type)) {
                      if (((IntAstAttribute *) operand_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() > 
                          ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()) {
                           (*codeOut) << CodeEmitter::indent() << result_name << " = fptrunc " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                      }
                      else {
                          (*codeOut) << CodeEmitter::indent() << result_name << " = fpext " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                      }
                 }
                 else {
                      cerr << "can't convert yet from " << operand_type -> class_name() << " to " << result_type -> class_name() << endl;
                      cerr.flush();
                      assert(0);
                 }
             }
             else if (dynamic_cast<SgPointerType *> (result_type)) {
                 if (dynamic_cast<SgTypeString *> (operand_type))
                      ;  // already taken care of
                 else if (operand_type -> isIntegerType() || isSgEnumType(operand_type)) {
                      (*codeOut) << CodeEmitter::indent() << result_name << " = inttoptr " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                 }
                 else if (dynamic_cast<SgPointerType *> (operand_type)) {
                      (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << operand_type_name << " " << operand_name << " to " << result_type_name << endl;
                 }
                 else {
                      cerr << "can't convert yet from " << operand_type -> class_name() << " to " << result_type -> class_name() << endl;
                      cerr.flush();
                      assert(0);
                 }
             }
             else {
                 cerr << "Funny conversion from type " << operand_type -> class_name() << " to type " << result_type -> class_name() << endl;
                 cerr.flush();
                 assert(0);
             }
         }
     }
     //                 SgThrowOp
     //                 SgRealPartOp
     //                 SgImagPartOp
     //                 SgConjugateOp
     //                 SgUserDefinedUnaryOp
     //             SgBinaryOp:
     //                 SgArrowExp
     else if (dynamic_cast<SgArrowExp *>(node)) {
         SgArrowExp *n = isSgArrowExp(node);

         SgType *lhs_type = attributes -> getExpressionType(n -> get_lhs_operand());
         SgClassType *class_type = isSgClassType(attributes -> getSourceType(isSgPointerType(attributes -> getSourceType(lhs_type)) -> get_base_type()));
         assert(class_type);
         string reference_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue(),
                lhs_name = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                result_type_name = ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         SgClassDeclaration *decl= isSgClassDeclaration(class_type -> get_declaration());
         if (decl -> get_class_type() == SgClassDeclaration::e_union) {
             string union_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_UNION_NAME)) -> getValue();
             int size = ((IntAstAttribute *) class_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << union_name << " = getelementptr " << lhs_type_name << " " << lhs_name << ", i32 0, i32 0" << endl;
             (*codeOut) << CodeEmitter::indent() << reference_name << " = bitcast [" << size << " x i8]* " << union_name << " to " << result_type_name << "*" <<  endl;
         }
         else {
             int index = ((IntAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_CLASS_MEMBER)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << reference_name << " = getelementptr " << lhs_type_name << " " << lhs_name << ", i32 0, i32 " << index << endl;
         }

         if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
             string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             if (dynamic_cast<SgClassType *> (attributes -> getSourceType(n -> get_type()))) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << result_type_name << "* " << reference_name << " to i8*" << endl;
             }
             else if (n -> attributeExists(Control::LLVM_AGGREGATE)) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr " << result_type_name << "* " << reference_name << ", i32 0, i32 0" << endl;
             }
             else {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = load " << result_type_name << "* " << reference_name << endl;
             }
         }
     }
     //                 SgDotExp
     else if (dynamic_cast<SgDotExp *>(node)) {
         SgDotExp *n = isSgDotExp(node);
         string reference_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue(),
                lhs_name = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                lhs_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_lhs_operand()) -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                result_type_name = ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         SgType *lhs_type = attributes -> getExpressionType(n -> get_lhs_operand());
         SgClassType *class_type = isSgClassType(attributes -> getSourceType(lhs_type));
         SgClassDeclaration *decl= isSgClassDeclaration(class_type -> get_declaration());
         if (decl -> get_class_type() == SgClassDeclaration::e_union) {
             string union_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_UNION_NAME)) -> getValue();
             int size = ((IntAstAttribute *) class_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << union_name << " = getelementptr " << lhs_type_name << "* " << lhs_name << ", i32 0, i32 0" << endl;
             (*codeOut) << CodeEmitter::indent() << reference_name << " = bitcast [" << size << " x i8]* " << union_name << " to " << result_type_name << "*" <<  endl;
         }
         else {
             int index = ((IntAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_CLASS_MEMBER)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << reference_name << " = getelementptr " << lhs_type_name << "* " << lhs_name << ", i32 0, i32 " << index << endl;
         }

         if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
             string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             if (dynamic_cast<SgClassType *>(attributes -> getSourceType(n -> get_type()))) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << result_type_name << "* " << reference_name << " to i8*" << endl;
             }
             else if (n -> attributeExists(Control::LLVM_AGGREGATE)) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr " << result_type_name << "* " << reference_name << ", i32 0, i32 0" << endl;
             }
             else {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = load " << result_type_name << "* " << reference_name << endl;
             }
         }
     }
     //                 SgDotStarOp
     //                 SgArrowStarOp
     //                 SgEqualityOp
     else if (dynamic_cast<SgEqualityOp *> (node)) {
         SgEqualityOp *n = isSgEqualityOp(node);
         genBinaryCompareOperation(n, "eq");
     }
     //                 SgLessThanOp
     else if (dynamic_cast<SgLessThanOp *>(node)) {
         SgLessThanOp *n = isSgLessThanOp(node);
         genBinaryCompareOperation(n, "lt");
     }
     //                 SgGreaterThanOp
     else if (dynamic_cast<SgGreaterThanOp *>(node)) {
         SgGreaterThanOp *n = isSgGreaterThanOp(node);
         genBinaryCompareOperation(n, "gt");
     }
     //                 SgNotEqualOp
     else if (dynamic_cast<SgNotEqualOp *> (node)) {
         SgNotEqualOp *n = isSgNotEqualOp(node);
         genBinaryCompareOperation(n, "ne");
     }
     //                 SgLessOrEqualOp
     else if (dynamic_cast<SgLessOrEqualOp *> (node)) {
         SgLessOrEqualOp *n = isSgLessOrEqualOp(node);
         genBinaryCompareOperation(n, "le");
     }
     //                 SgGreaterOrEqualOp
     else if (dynamic_cast<SgGreaterOrEqualOp *> (node)) {
         SgGreaterOrEqualOp *n = isSgGreaterOrEqualOp(node);
         genBinaryCompareOperation(n, "ge");
     }
     //                 SgAddOp
     else if (dynamic_cast<SgAddOp *>(node)) {
         SgAddOp *n = isSgAddOp(node);
         genAddOrSubtractOperation(n, "add");
     }
     //                 SgSubtractOp
     else if (dynamic_cast<SgSubtractOp *>(node)) {
         SgSubtractOp *n = isSgSubtractOp(node);
         genAddOrSubtractOperation(n, "sub");
     }
     //                 SgMultiplyOp
     else if (dynamic_cast<SgMultiplyOp *>(node)) {
         SgMultiplyOp *n = isSgMultiplyOp(node);
         genBasicBinaryOperation(n, "mul");
     }
     //                 SgDivideOp
     else if (dynamic_cast<SgDivideOp *>(node)) {
         SgDivideOp *n = isSgDivideOp(node);
         genDivideBinaryOperation(n, "div");
     }
     //                 SgIntegerDivideOp
     //                 SgModOp
     else if (dynamic_cast<SgModOp *>(node)) {
         SgModOp *n = isSgModOp(node);
         genDivideBinaryOperation(n, "rem");
     }
     //                 SgAndOp
     else if (dynamic_cast<SgAndOp *>(node)) {
         SgAndOp *n = isSgAndOp(node);
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                rhs_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         LogicalAstAttribute *lhs_attribute = (LogicalAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_LOGICAL_AND_LHS),
                             *rhs_attribute = (LogicalAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_LOGICAL_AND_RHS);
         codeOut -> emitLabel(current_function_decls, lhs_attribute -> getEndLabel());
         (*codeOut) << CodeEmitter::indent() << result_name << " = phi i1 [false, %" << lhs_attribute -> getLastLhsLabel() << "], "
                    << "[" << rhs_name << ", %" << rhs_attribute -> getLastRhsLabel() << "]" << endl;
     }
     //                 SgOrOp
     else if (dynamic_cast<SgOrOp *>(node)) {
         SgOrOp *n = isSgOrOp(node);
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                rhs_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         LogicalAstAttribute *lhs_attribute = (LogicalAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_LOGICAL_OR_LHS),
                             *rhs_attribute = (LogicalAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_LOGICAL_OR_RHS);
         codeOut -> emitLabel(current_function_decls, lhs_attribute -> getEndLabel());
         (*codeOut) << CodeEmitter::indent() << result_name << " = phi i1 [true, %" << lhs_attribute -> getLastLhsLabel() << "], "
                    << "[" << rhs_name << ", %" << rhs_attribute -> getLastRhsLabel() << "]" << endl;
     }
     //                 SgBitXorOp
     else if (dynamic_cast<SgBitXorOp *>(node)) {
         SgBitXorOp *n = isSgBitXorOp(node);
         genBasicBinaryOperation(n, "xor");
     }
     //                 SgBitAndOp
     else if (dynamic_cast<SgBitAndOp *>(node)) {
         SgBitAndOp *n = isSgBitAndOp(node);
         genBasicBinaryOperation(n, "and");
     }
     //                 SgBitOrOp
     else if (dynamic_cast<SgBitOrOp *>(node)) {
         SgBitOrOp *n = isSgBitOrOp(node);
         genBasicBinaryOperation(n, "or");
     }
     //                 SgCommaOpExp
     else if (dynamic_cast<SgCommaOpExp *>(node)) {
         SgCommaOpExp *n = isSgCommaOpExp(node);
     }
     //                 SgLshiftOp
     else if (dynamic_cast<SgLshiftOp *>(node)) {
         SgLshiftOp *n = isSgLshiftOp(node);
         genBasicBinaryOperation(n, "shl");
     }
     //                 SgRshiftOp
     else if (dynamic_cast<SgRshiftOp *>(node)) {
         SgRshiftOp *n = isSgRshiftOp(node);
         genBasicBinaryOperation(n, (isUnsignedType(attributes -> getExpressionType(n -> get_lhs_operand())) ? "lshr" : "ashr"));
     }
     //                 SgPntrArrRefExp
     else if (dynamic_cast<SgPntrArrRefExp *> (node)) {
         SgPntrArrRefExp *n = isSgPntrArrRefExp(node);

         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                lhs_name = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                rhs_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_rhs_operand()) -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                rhs_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                reference_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue(),
                result_type_name = ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         /**
          * TODO: Remove this code when bug is fixed.
          *
          * Pointer difference operation does not properly generate an integer result.
          */
         if (n -> get_rhs_operand() -> attributeExists(Control::LLVM_DO_NOT_CAST)) {
             rhs_type_name = attributes -> getIntegerPointerTarget();
         }

         /**
          * Say something!
          */
         AggregateAstAttribute *aggregate_attribute = (AggregateAstAttribute *) n -> getAttribute(Control::LLVM_AGGREGATE);
         string aggregate_type_name = (aggregate_attribute
                                                ? (aggregate_attribute -> getAggregate()
                                                            ? ((StringAstAttribute *) aggregate_attribute -> getAggregate() -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                                                            : (aggregate_attribute -> getArrayType()
                                                                  ? ((StringAstAttribute *) aggregate_attribute -> getArrayType() -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                                                                  : ((StringAstAttribute *) aggregate_attribute -> getClassType() -> getAttribute(Control::LLVM_TYPE)) -> getValue()))
                                                : result_type_name);

         /**
          * Say something !
          */
         (*codeOut) << CodeEmitter::indent() << reference_name << " = getelementptr " << aggregate_type_name << "* " << lhs_name << ", "
                    << rhs_type_name << " " << rhs_name << endl;

         /**
          * Say something !
          */
         if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
             if (n -> attributeExists(Control::LLVM_AGGREGATE) && isSgArrayType(n -> get_type())) { // TODO: just added array test!  Not too sure of myself here!
                 (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr " << aggregate_type_name << "* " << reference_name << ", i32 0, i32 0" << endl;
             }
             else {
                 if (dynamic_cast<SgClassType *> (attributes -> getSourceType(n -> get_type()))) {
                     (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << result_type_name << "* " << reference_name << " to i8*" << endl;
                 }
                 else {
                     (*codeOut) << CodeEmitter::indent() << result_name << " = load " << result_type_name << "* " << reference_name << endl;
                 }
             }
         }
     }
     //                 SgScopeOp
     //                 SgAssignOp
     else if (dynamic_cast<SgAssignOp *>(node)) {
         SgAssignOp *n = isSgAssignOp(node);
         string result_type_name = ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                rhs_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         if (isSgClassType(attributes -> getSourceType(n -> get_type()))) {
             string lhs_name = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             int size = ((IntAstAttribute *) attributes -> getSourceType(n -> get_type()) -> getAttribute(Control::LLVM_SIZE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.i32(i8* " << lhs_name << ", i8* " << rhs_name << ", i32 " << size << ", i32 4)" << endl;
         }
         else {
             string ref_name = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue();

             if (dynamic_cast<SgFunctionRefExp *>(n -> get_rhs_operand())) {
                 SgFunctionRefExp *function = isSgFunctionRefExp(n -> get_rhs_operand());
                 string function_name = ((StringAstAttribute *) function -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                        function_type_name = ((StringAstAttribute *) attributes -> getExpressionType(function) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << "store " << result_type_name
                            << " bitcast (" << function_type_name << "* " << function_name << " to " << result_type_name << "), "
                            << result_type_name << "* " << ref_name << endl;
             }
             else {
                 (*codeOut) << CodeEmitter::indent() << "store " << result_type_name << " " << rhs_name << ", " << result_type_name << "* " << ref_name << endl;
             }
         }
     }
     //                 SgPlusAssignOp
     else if (dynamic_cast<SgPlusAssignOp *>(node)) {
         SgPlusAssignOp *n = isSgPlusAssignOp(node);
         genAddOrSubtractOperationAndAssign(n, "add");
     }
     //                 SgMinusAssignOp
     else if (dynamic_cast<SgMinusAssignOp *>(node)) {
         SgMinusAssignOp *n = isSgMinusAssignOp(node);
         genAddOrSubtractOperationAndAssign(n, "sub");
     }
     //                 SgAndAssignOp
     else if (dynamic_cast<SgAndAssignOp *>(node)) {
         SgAndAssignOp *n = isSgAndAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "and");
     }
     //                 SgIorAssignOp
     else if (dynamic_cast<SgIorAssignOp *>(node)) {
         SgIorAssignOp *n = isSgIorAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "or");
     }
     //                 SgMultAssignOp
     else if (dynamic_cast<SgMultAssignOp *>(node)) {
         SgMultAssignOp *n = isSgMultAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "mul");
     }
     //                 SgDivAssignOp
     else if (dynamic_cast<SgDivAssignOp *>(node)) {
         SgDivAssignOp *n = isSgDivAssignOp(node);
         genDivideBinaryOperationAndAssign(n, "div");
     }
     //                 SgModAssignOp
     else if (dynamic_cast<SgModAssignOp *>(node)) {
         SgModAssignOp *n = isSgModAssignOp(node);
         genDivideBinaryOperationAndAssign(n, "rem");
     }
     //                 SgXorAssignOp
     else if (dynamic_cast<SgXorAssignOp *>(node)) {
         SgXorAssignOp *n = isSgXorAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "xor");
     }
     //                 SgLshiftAssignOp
     else if (dynamic_cast<SgLshiftAssignOp *>(node)) {
         SgLshiftAssignOp *n = isSgLshiftAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "shl");
     }
     //                 SgRshiftAssignOp
     else if (dynamic_cast<SgRshiftAssignOp *>(node)) {
         SgRshiftAssignOp *n = isSgRshiftAssignOp(node);
         genBasicBinaryOperationAndAssign(n, (isUnsignedType(attributes -> getExpressionType(n -> get_lhs_operand())) ? "lshr" : "ashr"));
     }
     //                 SgExponentiationOp
     //                 SgConcatenationOp
     //                 SgPointerAssignOp
     //                 SgUserDefinedBinaryOp
     //             SgExprListExp
     else if (dynamic_cast<SgExprListExp *>(node)) {
         SgExprListExp *n = isSgExprListExp(node);
     }
     //             SgVarRefExp
     else if (dynamic_cast<SgVarRefExp *>(node)) {
         SgVarRefExp *n = isSgVarRefExp(node);
         if (! n -> attributeExists(Control::LLVM_CLASS_MEMBER)) { // class members are processed at DotExp or ArraowExp level
             SgVariableSymbol *sym = n -> get_symbol();
             assert(sym);
             SgInitializedName *decl = sym -> get_declaration();
             assert(decl);
             string var_name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                    type_name =  ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             /**
              * If we are dealing with an array aggregate, we need to load its address.
              */
             if (decl -> attributeExists(Control::LLVM_AGGREGATE) && isSgArrayType(n -> get_type())) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr " << type_name << "* " << var_name << ", i32 0, i32 0" << endl;
             }
             else if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
                 if (dynamic_cast<SgClassType *> (attributes -> getSourceType(n -> get_type()))) {
                     (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << type_name << "* " << var_name << " to i8*" << endl;
                 }
                 else {
                     (*codeOut) << CodeEmitter::indent() << result_name << " = load " << type_name << "* " << var_name << endl;
                 }
             }
         }
     }
     //             SgClassNameRefExp
     //             SgFunctionRefExp
     else if (dynamic_cast<SgFunctionRefExp *>(node)) {
         SgFunctionRefExp *n = isSgFunctionRefExp(node);
     }
     //             SgMemberFunctionRefExp
     //             SgValueExp:
     //                 SgBoolValExp
     else if (dynamic_cast<SgBoolValExp *>(node)) {
         SgBoolValExp *b = isSgBoolValExp(node);
     }
     //                 SgStringVal
     else if (dynamic_cast<SgStringVal*>(node)) {
         SgStringVal *sval = isSgStringVal(node);
     }
     //                 SgShortVal
     else if (dynamic_cast<SgShortVal *>(node)) {
         SgShortVal *n = isSgShortVal(node);
     }
     //                 SgCharVal
     else if (dynamic_cast<SgCharVal*>(node)) {
         SgCharVal *cval = isSgCharVal(node);
     }
     //                 SgUnsignedCharVal
     else if (dynamic_cast<SgUnsignedCharVal *>(node)) {
         SgUnsignedCharVal *n = isSgUnsignedCharVal(node);
     }
     //                 SgWcharVal
     //                 SgUnsignedShortVal
     else if (dynamic_cast<SgUnsignedShortVal *>(node)) {
         SgUnsignedShortVal *n = isSgUnsignedShortVal(node);
     }
     //                 SgIntVal
     else if (dynamic_cast<SgIntVal*>(node)) {
         SgIntVal *ival = isSgIntVal(node);
     }
     //                 SgEnumVal
     else if (dynamic_cast<SgEnumVal*>(node)) {
         SgEnumVal *ival = isSgEnumVal(node);
     }
     //                 SgUnsignedIntVal
     else if (dynamic_cast<SgUnsignedIntVal *>(node)) {
         SgUnsignedIntVal *n = isSgUnsignedIntVal(node);
     }
     //                 SgLongIntVal
     else if (dynamic_cast<SgLongIntVal *>(node)) {
         SgLongIntVal *n = isSgLongIntVal(node);
     }
     //                 SgLongLongIntVal
     else if (dynamic_cast<SgLongLongIntVal *>(node)) {
         SgLongLongIntVal *n = isSgLongLongIntVal(node);
     }
     //                 SgUnsignedLongLongIntVal 
     else if (dynamic_cast<SgUnsignedLongLongIntVal *>(node)) {
         SgUnsignedLongLongIntVal *n = isSgUnsignedLongLongIntVal(node);
     }
     //                 SgUnsignedLongVal
     else if (dynamic_cast<SgUnsignedLongVal *>(node)) {
         SgUnsignedLongVal *n = isSgUnsignedLongVal(node);
     }
     //                 SgFloatVal
     else if (dynamic_cast<SgFloatVal*>(node)) {
         SgFloatVal *fval = isSgFloatVal(node);
     }
     //                 SgDoubleVal
     else if (dynamic_cast<SgDoubleVal*>(node)) {
         SgDoubleVal *dval = isSgDoubleVal(node);
     }
     //                 SgLongDoubleVal
     else if (dynamic_cast<SgLongDoubleVal *>(node)) {
         SgLongDoubleVal *n = isSgLongDoubleVal(node);
     }
     //                 SgComplexVal
     //                 SgUpcThreads
     //                 SgUpcMythread
     //                 SgFunctionCallExp
     else if (dynamic_cast<SgFunctionCallExp *>(node)) {
         SgFunctionCallExp *n = isSgFunctionCallExp(node);

         /**
          * TODO: Say Something
          */
         string function_name,
                function_type_name;
         vector<SgType *> function_parm_types;
         if (dynamic_cast<SgFunctionRefExp *>(n -> get_function())) {
             SgFunctionRefExp *function = isSgFunctionRefExp(n -> get_function());
             function_name = ((StringAstAttribute *) function -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             function_type_name = ((StringAstAttribute *) attributes -> getExpressionType(function) -> getAttribute(Control::LLVM_TYPE)) -> getValue();

             /**
              * TODO: Remove this when bug is fixed.
              * 
              * Under normal circumstances, we should be able to obtain the type of an argument from the argument
              * expression as the front-end should have insured that any expression that is passed as an argument
              * to a function be either of the same type as the type of the corresponding parameter or be casted 
              * to that type. Rose does not always ensure that that is the case...
              */
             SgFunctionDeclaration *function_declaration = n -> getAssociatedFunctionDeclaration();
             assert(function_declaration);
             vector<SgInitializedName *> parms = function_declaration -> get_args();
             for (int i = 0; i < parms.size(); i++) {
                 function_parm_types.push_back(parms[i] -> get_type());
             }
         }
         else if (dynamic_cast<SgPointerDerefExp *>(n -> get_function())) {
             SgExpression *function_operand = isSgPointerDerefExp(n -> get_function()) -> get_operand();
             SgPointerType *function_operand_type = isSgPointerType(attributes -> getExpressionType(function_operand));
             assert(function_operand_type);
             SgFunctionType *function_type = isSgFunctionType(function_operand_type -> get_base_type());
             assert(function_type);
             function_name = ((StringAstAttribute *) function_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             function_type_name = ((StringAstAttribute *) function_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             function_parm_types = function_type -> get_arguments();
         }
         else {
             assert(0);
         }

         /**
          * TODO: Say Something
          */
         SgType *return_type = n -> get_type();
         string original_return_type_name = ((StringAstAttribute *) return_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                return_type_name = original_return_type_name;

         int integral_class_return_type = attributes -> integralStructureType(return_type);
         vector<SgExpression *> args = n -> get_args() -> get_expressions();
         for (int i = 0; i < args.size(); i++) {
             SgExpression *arg = args[i];
             SgType *arg_type = attributes -> getExpressionType(arg);

             if (isSgClassType(attributes -> getSourceType(arg_type))) {
                 string arg_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                        coerce_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_ARGUMENT_COERCE)) -> getValue(),
                        bit_cast_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_ARGUMENT_BIT_CAST)) -> getValue(),
                        arg_type_name = ((StringAstAttribute *) arg_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 int size = ((IntAstAttribute *) arg_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << bit_cast_name << " = bitcast " << arg_type_name << "* " << coerce_name << " to i8*" << endl;
                 (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.i32(i8* "<< bit_cast_name << ", i8* " << arg_name << ", i32 " << size << ", i32 4)" << endl;
             }
             else if (i < function_parm_types.size()) {
                 if (arg -> attributeExists(Control::LLVM_ARGUMENT_INTEGRAL_PROMOTION)) {
                     string arg_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_ARGUMENT_INTEGRAL_PROMOTION)) -> getValue(),
                            arg_type_name = ((StringAstAttribute *) arg_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                            parm_type_name = ((StringAstAttribute *) function_parm_types[i] -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                            promote_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_ARGUMENT_INTEGRAL_PROMOTION)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (isUnsignedType(function_parm_types[i]) ? "zext " : "sext ")
                                << arg_type_name << " " << arg_name << " to " << parm_type_name << endl;
                 }
                 else if (arg -> attributeExists(Control::LLVM_ARGUMENT_INTEGRAL_DEMOTION)) {
                     string arg_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_ARGUMENT_INTEGRAL_PROMOTION)) -> getValue(),
                            arg_type_name = ((StringAstAttribute *) arg_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                            parm_type_name = ((StringAstAttribute *) function_parm_types[i] -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                            demote_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_ARGUMENT_INTEGRAL_PROMOTION)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << demote_name << " = trunc " << arg_type_name << " " << arg_name << " to " << parm_type_name << endl;
                 }
             }
         }


         string result_name;
         if (isSgTypeVoid(attributes -> getSourceType(return_type))) {
             (*codeOut) << CodeEmitter::indent() << "call void " << function_name << "(";
         }
         else {
             result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent();
             if (isSgClassType(attributes -> getSourceType(return_type))) {
                 if (integral_class_return_type) {
                     stringstream out;
                     out << "i" << (integral_class_return_type * 8);
                     return_type_name = out.str();
                     (*codeOut) << result_name << " = ";
                 }
                 else {
                     return_type_name = "void";
                 }

                 (*codeOut) << "call ";
             }
             else {
                 (*codeOut) << result_name << " = call ";
             }

             /**
              * When invoking a function with variable arguments or a function that returns a pointer to a function,
              * the full signature of the function must be used. Otherwise, we only need to specify the return type.
              */
             SgFunctionSymbol *function_symbol = n -> getAssociatedFunctionSymbol();
             if (function_symbol) {
                 SgFunctionType *function_type = isSgFunctionType(function_symbol -> get_type());
                 assert(function_type);
                 SgPointerType *pointer_return_type = isSgPointerType(attributes -> getSourceType(return_type));
                 /**
                  * It would appear that all compiler generated functions have the get_has_ellipses() flag turned on.
                  * However, we added the test here anyway for completeness.
                  */
                 if (function_type -> get_has_ellipses() || function_type -> attributeExists(Control::LLVM_COMPILER_GENERATED) ||
                     (pointer_return_type && isSgFunctionType(attributes -> getSourceType(pointer_return_type -> get_base_type())))) {   // function returns a pointer to a function?
                       (*codeOut) << function_type_name << "* ";
                 }
                 else {
                     (*codeOut) << return_type_name;
                 }
             }
             else {
                 (*codeOut) << return_type_name;
             }
             (*codeOut) << " " << function_name << "(";
         }

         /**
          * The function returns a structure type that cannot be stored in an integral unit.
          */
         if (isSgClassType(attributes -> getSourceType(return_type)) && integral_class_return_type == 0) {
             (*codeOut) << original_return_type_name << "* noalias sret " << result_name << (args.size() > 0 ? ", " : "");
         }
         for (int i = 0; i < args.size(); i++) {
             SgType *type = (i < function_parm_types.size() && (! isSgTypeEllipse(function_parm_types[i]))
                                               ? function_parm_types[i]
                                               : attributes -> getExpressionType(args[i]));
             SgArrayType *array_type = isSgArrayType(type);
             string arg_name = ((array_type || isSgPointerType(type)) &&
                                ((isSgIntVal(args[i]) && isSgIntVal(args[i]) -> get_value() == 0) || (isSgEnumVal(args[i]) && isSgEnumVal(args[i]) -> get_value() == 0))
                                    ? "null"
                                    : ((StringAstAttribute *) args[i] -> getAttribute(Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME)) -> getValue()),
                    parm_type_name;
             if (array_type) {
                 parm_type_name = ((StringAstAttribute *) array_type -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 parm_type_name += "*";
             }
             else {
                 parm_type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 if (isSgClassType(attributes -> getSourceType(type))) {
                     parm_type_name += "* byval";
                 }
             }

             (*codeOut) << parm_type_name << " " << arg_name;
             if (i + 1 < args.size())
                  (*codeOut) << ", ";
         }
         (*codeOut) << ")" << endl;

         if (isSgClassType(attributes -> getSourceType(return_type))) {
             if (integral_class_return_type) {
                 string cast_name = attributes -> getTemp(LLVMAstAttributes::TEMP_INT),
                        coerce_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_COERCE)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << cast_name << " = bitcast " << original_return_type_name << "* " << coerce_name << " to " << return_type_name << "*" << endl;
                 (*codeOut) << CodeEmitter::indent() << "store " << return_type_name << " " << result_name << ", " << return_type_name << "* " << cast_name << ", align 1" << endl;

                 result_name = coerce_name;
             }

             if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
                 string bit_cast_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_RETURNED_STRUCTURE_BIT_CAST)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << bit_cast_name << " = bitcast " << original_return_type_name << "* " << result_name << " to i8*" << endl;
             }
         }
     }
     //                 SgSizeOfOp
     else if (dynamic_cast<SgSizeOfOp *>(node)) {
         SgSizeOfOp *n = isSgSizeOfOp(node);
     }
     //                 SgUpcLocalsizeof
     //                 SgUpcBlocksizeof
     //                 SgUpcElemsizeof
     //             SgTypeIdOp
     //             SgConditionalExp
     else if (dynamic_cast<SgConditionalExp *>(node)) {
         SgConditionalExp *n = isSgConditionalExp(node);

         if (isSgTypeVoid(n -> get_type())) {
             assert(! "Know yet what to do with conditionals that yield void ... like a call to assert");
         }

         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                true_name = ((StringAstAttribute *) n -> get_true_exp() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                false_name = ((StringAstAttribute *) n -> get_false_exp() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         if (n -> attributeExists(Control::LLVM_SELECT_CONDITIONAL)) {
             string cond_name = ((StringAstAttribute *) n -> get_conditional_exp() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                    true_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_true_exp()) -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    false_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_false_exp()) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << result_name << " = select i1 " << cond_name << ", "
                        << true_type_name << " " << true_name << ", " << false_type_name << " " << false_name << endl;
         }
         else {
             string result_type_name = (n -> attributeExists(Control::LLVM_IS_BOOLEAN)
                                           ? "i1"
                                           : ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue());
             ConditionalComponentAstAttribute *true_attribute = (ConditionalComponentAstAttribute *) n -> get_true_exp() -> getAttribute(Control::LLVM_CONDITIONAL_COMPONENT_LABELS),
                                              *false_attribute = (ConditionalComponentAstAttribute *) n -> get_false_exp() -> getAttribute(Control::LLVM_CONDITIONAL_COMPONENT_LABELS);
             codeOut -> emitLabel(current_function_decls, true_attribute -> getEndLabel());
             (*codeOut) << CodeEmitter::indent() << result_name << " = phi " << result_type_name << " [" << true_name << ", %" << true_attribute -> getLastLabel() << "], "
                        << "[" << false_name << ", %" << false_attribute -> getLastLabel() << "]" << endl;
         }
     }
     //             SgNewExp
     //             SgDeleteExp
     //             SgThisExp
     //             SgRefExp
     //             SgInitializer:
     //                 SgAggregateInitializer
     else if (dynamic_cast<SgAggregateInitializer *>(node)) {
         SgAggregateInitializer *n = isSgAggregateInitializer(node);
     }
     //                 SgConstructorInitializer
     //                 SgAssignInitializer
     else if (dynamic_cast<SgAssignInitializer *>(node)) {
         SgAssignInitializer *n = isSgAssignInitializer(node);
     }
     //                 SgDesignatedInitializer
     //             SgVarArgStartOp
     //             SgVarArgOp
     //             SgVarArgEndOp
     //             SgVarArgCopyOp
     //             SgVarArgStartOneOperandOp
     //             SgNullExpression
     else if (dynamic_cast<SgNullExpression *>(node)) {
         SgNullExpression *n = isSgNullExpression(node);
     }
     //             SgVariantExpression
     //             SgSubscriptExpression
     //             SgColonShapeExp
     //             SgAsteriskShapeExp
     //             SgImpliedDo
     //             SgIOItemExpression
     //             SgStatementExpression
     //             SgAsmOp
     //             SgLabelRefExp
     //             SgActualArgumentExpression
     //             SgUnknownArrayOrFunctionReference
     //         SgLocatedNodeSupport:
     //             SgInterfaceBody
     //             SgRenamePair
     //             SgOmpClause:
     //                 SgOmpOrderedClause
     //                 SgOmpNowaitClause
     //                 SgOmpUntiedClause
     //                 SgOmpDefaultClause
     //                 SgOmpExpressionClause:
     //                     SgOmpCollapseClause
     //                     SgOmpIfClause
     //                     SgOmpNumThreadsClause
     //                 SgOmpVariablesClause:
     //                     SgOmpCopyprivateClause
     //                     SgOmpPrivateClause
     //                     SgOmpFirstprivateClause
     //                     SgOmpSharedClause
     //                     SgOmpCopyinClause
     //                     SgOmpLastprivateClause
     //                     SgOmpReductionClause
     //                 SgOmpScheduleClause
     //         SgToken
     //     SgSymbol:
     //         SgVariableSymbol
     //         SgFunctionSymbol:
     //             SgMemberFunctionSymbol
     //             SgRenameSymbol
     //         SgFunctionTypeSymbol
     //         SgClassSymbol
     //         SgTemplateSymbol
     //         SgEnumSymbol
     //         SgEnumFieldSymbol
     //         SgTypedefSymbol
     //         SgLabelSymbol
     //         SgDefaultSymbol
     //         SgNamespaceSymbol
     //         SgIntrinsicSymbol
     //         SgModuleSymbol
     //         SgInterfaceSymbol
     //         SgCommonSymbol
     //         SgAliasSymbol
     //     SgAsmNode:
     //         SgAsmStatement:
     //             SgAsmDeclaration:
     //                 SgAsmDataStructureDeclaration
     //                 SgAsmFunctionDeclaration
     //                 SgAsmFieldDeclaration
     //             SgAsmBlock
     //             SgAsmInstruction:
     //                 SgAsmx86Instruction
     //                 SgAsmArmInstruction
     //                 SgAsmPowerpcInstruction
     //         SgAsmExpression:
     //             SgAsmValueExpression:
     //                 SgAsmByteValueExpression
     //                 SgAsmWordValueExpression
     //                 SgAsmDoubleWordValueExpression
     //                 SgAsmQuadWordValueExpression
     //                 SgAsmSingleFloatValueExpression
     //                 SgAsmDoubleFloatValueExpression
     //                 SgAsmVectorValueExpression
     //             SgAsmBinaryExpression:
     //                 SgAsmBinaryAdd
     //                 SgAsmBinarySubtract
     //                 SgAsmBinaryMultiply
     //                 SgAsmBinaryDivide
     //                 SgAsmBinaryMod
     //                 SgAsmBinaryAddPreupdate
     //                 SgAsmBinarySubtractPreupdate
     //                 SgAsmBinaryAddPostupdate
     //                 SgAsmBinarySubtractPostupdate
     //                 SgAsmBinaryLsl
     //                 SgAsmBinaryLsr
     //                 SgAsmBinaryAsr
     //                 SgAsmBinaryRor
     //             SgAsmUnaryExpression:
     //                 SgAsmUnaryPlus
     //                 SgAsmUnaryMinus
     //                 SgAsmUnaryRrx
     //                 SgAsmUnaryArmSpecialRegisterList
     //             SgAsmMemoryReferenceExpression
     //             SgAsmRegisterReferenceExpression:
     //                 SgAsmx86RegisterReferenceExpression
     //                 SgAsmArmRegisterReferenceExpression
     //                 SgAsmPowerpcRegisterReferenceExpression
     //             SgAsmControlFlagsExpression
     //             SgAsmCommonSubExpression
     //             SgAsmExprListExp
     //             SgAsmFile
     //             SgAsmInterpretation
     //             SgAsmOperandList
     //             SgAsmType
     //             SgAsmTypeByte
     //             SgAsmTypeWord
     //             SgAsmTypeDoubleWord
     //             SgAsmTypeQuadWord
     //             SgAsmTypeDoubleQuadWord
     //             SgAsmType80bitFloat
     //             SgAsmType128bitFloat
     //             SgAsmTypeSingleFloat
     //             SgAsmTypeDoubleFloat
     //             SgAsmTypeVector
     //             SgAsmExecutableFileFormat
     //             SgAsmGenericDLL
     //             SgAsmGenericFormat
     //             SgAsmGenericDLLList
     //             SgAsmElfEHFrameEntryFD
     //             SgAsmGenericFile
     //             SgAsmGenericSection
     //             SgAsmGenericHeader
     //             SgAsmPEFileHeader
     //             SgAsmLEFileHeader
     //             SgAsmNEFileHeader
     //             SgAsmDOSFileHeader
     //             SgAsmElfFileHeader
     //             SgAsmElfSection
     //             SgAsmElfSymbolSection
     //             SgAsmElfRelocSection
     //             SgAsmElfDynamicSection
     //             SgAsmElfStringSection
     //             SgAsmElfNoteSection
     //             SgAsmElfEHFrameSection
     //             SgAsmElfSectionTable
     //             SgAsmElfSegmentTable
     //             SgAsmPESection
     //             SgAsmPEImportSection
     //             SgAsmPEExportSection
     //             SgAsmPEStringSection
     //             SgAsmPESectionTable
     //             SgAsmDOSExtendedHeader
     //             SgAsmCoffSymbolTable
     //             SgAsmNESection
     //             SgAsmNESectionTable
     //             SgAsmNENameTable
     //             SgAsmNEModuleTable
     //             SgAsmNEStringTable
     //             SgAsmNEEntryTable
     //             SgAsmNERelocTable
     //             SgAsmLESection
     //             SgAsmLESectionTable
     //             SgAsmLENameTable
     //             SgAsmLEPageTable
     //             SgAsmLEEntryTable
     //             SgAsmLERelocTable
     //             SgAsmGenericSymbol
     //             SgAsmCoffSymbol
     //             SgAsmElfSymbol
     //             SgAsmGenericStrtab
     //             SgAsmElfStrtab
     //             SgAsmCoffStrtab
     //             SgAsmGenericSymbolList
     //             SgAsmGenericSectionList
     //             SgAsmGenericHeaderList
     //             SgAsmGenericString
     //             SgAsmBasicString
     //             SgAsmStoredString
     //             SgAsmElfSectionTableEntry
     //             SgAsmElfSegmentTableEntry
     //             SgAsmElfSymbolList
     //             SgAsmPEImportILTEntry
     //             SgAsmElfRelocEntry
     //             SgAsmElfRelocEntryList
     //             SgAsmPEExportEntry
     //             SgAsmPEExportEntryList
     //             SgAsmElfDynamicEntry
     //             SgAsmElfDynamicEntryList
     //             SgAsmElfSegmentTableEntryList
     //             SgAsmStringStorage
     //             SgAsmElfNoteEntry
     //             SgAsmElfNoteEntryList
     //             SgAsmPEImportDirectory
     //             SgAsmPEImportHNTEntry
     //             SgAsmPESectionTableEntry
     //             SgAsmPEExportDirectory
     //             SgAsmPERVASizePair
     //             SgAsmCoffSymbolList
     //             SgAsmPERVASizePairList
     //             SgAsmElfEHFrameEntryCI
     //             SgAsmPEImportHNTEntryList
     //             SgAsmPEImportILTEntryList
     //             SgAsmPEImportLookupTable
     //             SgAsmPEImportDirectoryList
     //             SgAsmNEEntryPoint
     //             SgAsmNERelocEntry
     //             SgAsmNESectionTableEntry
     //             SgAsmElfEHFrameEntryCIList
     //             SgAsmLEPageTableEntry
     //             SgAsmLEEntryPoint
     //             SgAsmLESectionTableEntry
     //             SgAsmElfEHFrameEntryFDList
     //             SgAsmDwarfInformation
     //             SgAsmDwarfMacro
     //             SgAsmDwarfMacroList
     //             SgAsmDwarfLine
     //             SgAsmDwarfLineList
     //             SgAsmDwarfCompilationUnitList
     //             SgAsmDwarfConstruct
     //             SgAsmDwarfArrayType
     //             SgAsmDwarfClassType
     //             SgAsmDwarfEntryPoint
     //             SgAsmDwarfEnumerationType
     //             SgAsmDwarfFormalParameter
     //             SgAsmDwarfImportedDeclaration
     //             SgAsmDwarfLabel
     //             SgAsmDwarfLexicalBlock
     //             SgAsmDwarfMember
     //             SgAsmDwarfPointerType
     //             SgAsmDwarfReferenceType
     //             SgAsmDwarfCompilationUnit
     //             SgAsmDwarfStringType
     //             SgAsmDwarfStructureType
     //             SgAsmDwarfSubroutineType
     //             SgAsmDwarfTypedef
     //             SgAsmDwarfUnionType
     //             SgAsmDwarfUnspecifiedParameters
     //             SgAsmDwarfVariant
     //             SgAsmDwarfCommonBlock
     //             SgAsmDwarfCommonInclusion
     //             SgAsmDwarfInheritance
     //             SgAsmDwarfInlinedSubroutine
     //             SgAsmDwarfModule
     //             SgAsmDwarfPtrToMemberType
     //             SgAsmDwarfSetType
     //             SgAsmDwarfSubrangeType
     //             SgAsmDwarfWithStmt
     //             SgAsmDwarfAccessDeclaration
     //             SgAsmDwarfBaseType
     //             SgAsmDwarfCatchBlock
     //             SgAsmDwarfConstType
     //             SgAsmDwarfConstant
     //             SgAsmDwarfEnumerator
     //             SgAsmDwarfFileType
     //             SgAsmDwarfFriend
     //             SgAsmDwarfNamelist
     //             SgAsmDwarfNamelistItem
     //             SgAsmDwarfPackedType
     //             SgAsmDwarfSubprogram
     //             SgAsmDwarfTemplateTypeParameter
     //             SgAsmDwarfTemplateValueParameter
     //             SgAsmDwarfThrownType
     //             SgAsmDwarfTryBlock
     //             SgAsmDwarfVariantPart
     //             SgAsmDwarfVariable
     //             SgAsmDwarfVolatileType
     //             SgAsmDwarfDwarfProcedure
     //             SgAsmDwarfRestrictType
     //             SgAsmDwarfInterfaceType
     //             SgAsmDwarfNamespace
     //             SgAsmDwarfImportedModule
     //             SgAsmDwarfUnspecifiedType
     //             SgAsmDwarfPartialUnit
     //             SgAsmDwarfImportedUnit
     //             SgAsmDwarfMutableType
     //             SgAsmDwarfCondition
     //             SgAsmDwarfSharedType
     //             SgAsmDwarfFormatLabel
     //             SgAsmDwarfFunctionTemplate
     //             SgAsmDwarfClassTemplate
     //             SgAsmDwarfUpcSharedType
     //             SgAsmDwarfUpcStrictType
     //             SgAsmDwarfUpcRelaxedType
     //             SgAsmDwarfUnknownConstruct
     //             SgAsmDwarfConstructList

     else {
         cerr << "Missing case for " << node -> class_name() << endl;  // Used for Debugging
         cerr.flush();
         assert(0);
     }

     /**
      * Special case for for_increment
      */
     if (node -> attributeExists(Control::LLVM_BUFFERED_OUTPUT)) {
         codeOut -> endOutputToBuffer();
     }

     /**
      * Special case for if blocks.
      */
     if (dynamic_cast<SgStatement *>(node)) {
         SgStatement *n = isSgStatement(node);
         if (n -> attributeExists(Control::LLVM_IF_COMPONENT_LABELS)) {
             IfComponentAstAttribute *attribute = (IfComponentAstAttribute *) n -> getAttribute(Control::LLVM_IF_COMPONENT_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel());
         }
     }

     /**
      * Special case for expressions.
      */
     if (dynamic_cast<SgExpression *>(node)) {
         SgExpression *n = isSgExpression(node);

         /**
          * Special case for boolean expressions.
          *
          * The C language does not contain a boolean primitive type.  However, in LLVM, the result of a
          * boolean operation is a bit (i1) that cannot be subsequently used for arithmetic operations. Thus,
          * in order to perform such an operation in LLVM, the i1 must be explicitly converted to an integer
          * type.
          *
          * Whether or not such a conversion is required for an expression node has already been computed in
          * the CodeAttributeVisitor. So, when we encounter such a node, we emit the LLVM conversion code and
          * replace the original "result" name of the operation by the "extension" name... All subsequent operation
          * that use this node will thereafter only see the extension name.
          */
         if (n -> attributeExists(Control::LLVM_EXTEND_BOOLEAN)) {
             StringAstAttribute *name_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME);
             string extension_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXTEND_BOOLEAN)) -> getValue(),
                    result_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << extension_name << " = zext i1 " << " " << name_attribute -> getValue() << " to " << result_type_name << endl;
         }

         /**
          *
          */
         else if (n -> attributeExists(Control::LLVM_INTEGRAL_PROMOTION)) {
             SgType *type = n -> get_type(),
                    *result_type = attributes -> getExpressionType(n);
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                    promote_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_INTEGRAL_PROMOTION)) -> getValue(),
                    type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    result_type_name = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (isUnsignedType(type) ? "zext " : "sext ")
                        << type_name << " " <<  name << " to " << result_type_name << endl;
         }
         else if (n -> attributeExists(Control::LLVM_INTEGRAL_DEMOTION)) {
             SgType *type = n -> get_type(),
                    *result_type = attributes -> getExpressionType(n);
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                    demote_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_INTEGRAL_DEMOTION)) -> getValue(),
                    type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    result_type_name = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << demote_name << " = " << "trunc " // (isUnsignedType(type) ? "zext " : "sext ")
                        << type_name << " " <<  name << " to " << result_type_name << endl;
        }

         /**
          * These are special cases for the subexpressions in a conditional expression.
          */
         if (n -> attributeExists(Control::LLVM_CONDITIONAL_TEST)) {
             ConditionalAstAttribute *attribute = (ConditionalAstAttribute *) n -> getAttribute(Control::LLVM_CONDITIONAL_TEST);
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getTrueLabel() << ", label %" << attribute -> getFalseLabel() << endl;
         }
         else if (n -> attributeExists(Control::LLVM_CONDITIONAL_COMPONENT_LABELS)) {
             ConditionalComponentAstAttribute *attribute = (ConditionalComponentAstAttribute *) n -> getAttribute(Control::LLVM_CONDITIONAL_COMPONENT_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_AND_LHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_AND_LHS);
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getRhsLabel() << ", label %" << attribute -> getEndLabel() << endl;
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_AND_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_AND_RHS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_OR_LHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_OR_LHS);
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getEndLabel() << ", label %" << attribute -> getRhsLabel() << endl;
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_OR_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_OR_RHS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel());
         }
    }
}

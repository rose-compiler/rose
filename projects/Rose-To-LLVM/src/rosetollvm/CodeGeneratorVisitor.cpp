#include <rosetollvm/CodeGeneratorVisitor.h>
#include <rosetollvm/ConstantCodeEvaluator.h>
#include <iostream>

using namespace std;

/**
 * A pointer type initialized with NULL (int value of 0) ?
 */
void CodeGeneratorVisitor::processDimensionExpressions() {
     /**
      *
      */
// TODO: Remove this !

if (control.numDimensionAttributes() > 0) {
cout
  << "*** Generating dimension Expressions code for "
  << control.numDimensionAttributes()
  << " dimension attributes"
  << endl;
cout.flush();
}

    for (int i = 0; i < control.numDimensionAttributes(); i++) {
        DimensionAstAttribute *dimension_attribute = control.getDimensionAttribute(i);
        current_function_decls = dimension_attribute -> getFunctionAttribute();
        ROSE2LLVM_ASSERT(current_function_decls);
//        ROSE2LLVM_ASSERT(current_function_decls -> getFunctionDeclaration());
// TODO: Remove this !

cout
  << "***     Visiting function "
  << current_function_decls -> getFunctionDeclaration() -> get_name().getString()
  << endl;
cout.flush();


// TODO: Remove this !

cout
  << "***     Processing "
  << dimension_attribute -> numDimensionExpressions()
  << " dimension expressions in declaration of "
  << dimension_attribute -> getVariableDeclaration() -> get_name().getString()
  << endl;
cout.flush();

        setAttributes(dimension_attribute -> getAstAttributes());
        codeOut -> startOutputToBuffer(dimension_attribute -> getBuffer());
        for (int k = 0; k < dimension_attribute -> numDimensionExpressions(); k++) {
// TODO: Remove this !

cout
  << "    Generating dimension Expression code for a "
  << dimension_attribute -> getDimensionExpression(k) -> class_name()
  << endl;
cout.flush();

            this -> traverse(dimension_attribute -> getDimensionExpression(k));
        }
        codeOut -> endOutputToBuffer();
    }
// TODO: Remove this !

if (control.numDimensionAttributes() > 0) {
cout
  << "*** Done Generating dimension Expressions code"
  << endl;
cout.flush();
}

}

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
        attributes->generateMetadataNodes();
    }
}

/**
 * Generate global declarations.
 */
void CodeGeneratorVisitor::generateGlobals() {
    /* Add minimal target data layout. This is required for alias analysis to work
     * properly 
     */
    //
    // TODO: Removed because it's causing problem in LLVM-4.1 ...  To be reviewed
    //
  //    (*codeOut) << "target datalayout = \"e\"" << endl;

    /**
     * Generate global declarations for string constants
     */
    for (int i = 0; i < attributes -> numStrings() ; i++) {
        const char *str = attributes -> getString(i);
        (*codeOut) << attributes -> getGlobalStringConstantName(i)
                   << " = private unnamed_addr constant [" << attributes -> getStringLength(i) << " x i8] c\"" << str << "\"" << endl;
    }

    /**
     * Process global variable declarations. It appears to be legal in LLVM to place the declaration of
     * the global variables after the function(s) that use them.
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
                (*codeOut) << class_type_name << " = type { ";

                if (class_decl -> get_class_type() == SgClassDeclaration::e_union) {
                    // roseToLLVM does not yet support designated initializers.  Within that constraint, C99 says only one
                    // initializer member may be provided to a union, and it initializes the first union member.  So that we can
                    // easily initialize variables of a union type (especially global variables because they require constant
                    // expressions as initializers), we translate each union into an LLVM structure whose first field is the first
                    // field of the union and whose second field is padding for the remaining size.  We never have to worry about
                    // skipping initializers for remaining fields because Rose filters them out.  This will become significantly
                    // harder if we add support for designated initializers.  For example, see what clang does for initializing a
                    // struct that has a union as a member.
                    ROSE2LLVM_ASSERT(attribute -> numSgInitializedNames());
                    SgInitializedName *field_decl = attribute -> getSgInitializedName(0);
                    (*codeOut) << ((StringAstAttribute *) field_decl -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                    if (attribute -> getClassType() -> attributeExists(Control::LLVM_STRUCTURE_PADDING)) {
                        int pad_size = ((IntAstAttribute *) attribute -> getClassType() -> getAttribute(Control::LLVM_STRUCTURE_PADDING)) -> getValue();
                        for (int i = 0; i < pad_size; i++) {
                            (*codeOut) << ", i8";
                        }
                    }
                }
                else {
                    DeclarationsAstAttribute *class_attribute = (DeclarationsAstAttribute *) class_decl -> getAttribute(Control::LLVM_LOCAL_DECLARATIONS);
                    for (int k = 0; k < class_attribute -> numSgInitializedNames(); k++) {
                        SgInitializedName *field_decl = class_attribute -> getSgInitializedName(k);
                        SgType *field_type = (field_decl -> attributeExists(Control::LLVM_DECLARATION_TYPE)
                                                   ? ((SgTypeAstAttribute *) field_decl -> getAttribute(Control::LLVM_DECLARATION_TYPE)) -> getType()
                                                   :  attributes -> getSourceType(field_decl -> get_type()));
                        string field_type_name = ((StringAstAttribute *) field_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                        if (field_decl -> attributeExists(Control::LLVM_STRUCTURE_PADDING)) {
                            int pad_size = ((IntAstAttribute *) field_decl -> getAttribute(Control::LLVM_STRUCTURE_PADDING)) -> getValue();
                            for (int j = 0; j < pad_size; j++) {
                                (*codeOut) << "i8, ";
                            }
                        }
                        (*codeOut) << field_type_name;
                        if (k + 1 < class_attribute -> numSgInitializedNames())
                            (*codeOut) << ", ";
                    }

                    /**
                     *
                     */
                    if (class_attribute -> getClassType() -> attributeExists(Control::LLVM_STRUCTURE_PADDING)) {
                        int pad_size = ((IntAstAttribute *) class_attribute -> getClassType() -> getAttribute(Control::LLVM_STRUCTURE_PADDING)) -> getValue();
//                        if (pad_size < 3) {
                        for (int k = 0; k < pad_size; k++) {
                            (*codeOut) << ", i8";
                        }
                    }
                }
                (*codeOut) << " }" << endl;
            }
        }
        else if (dynamic_cast<SgInitializedName *>(attributes -> getGlobalDeclaration(i))) {
            SgInitializedName *decl = isSgInitializedName(attributes -> getGlobalDeclaration(i));
            /**
             * Not a chosen global declaration? Ignore it...  Recall that both a definition and a declaration may be specified
             */
            string name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_GLOBAL_CONSTANT_NAME)) -> getValue();
            vector<SgInitializedName *> decls = attributes -> global_variable_declaration_map[name];
            if (decls.size() > 0 && decls[0] != decl) {
                continue;
            }

            SgType *var_type = decl -> get_type();
            SgInitializer *initializer = decl -> get_initializer();

            string var_type_name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_TYPE)) -> getValue();

            //
            // WARNING!!! 12/15/2018  Do not use the function "get_storageModifier()" directly off a SgInitializedName
            //                        because it produces incorrect results. For now, it appears that going through
            //                        the "get_declaration()" function first works.
            //
            SgStorageModifier &sm = decl -> /* get_storageModifier(); */ get_declaration() -> get_declarationModifier().get_storageModifier();

            (*codeOut) << name << " = ";

            if (sm.isStatic()) {
                (*codeOut) << "internal global ";
            }
            else if (sm.isExtern()) {
                if (initializer) {
                    cerr << "warning: Attempt to initialize extern variable "
                         << name
                         << endl;
                    (*codeOut) << "global ";
                }
                else {
                    (*codeOut) << (decl -> isGnuAttributeWeak() ? "extern_weak global " : "external global ");
                }
            }
            else if (decl -> isGnuAttributeWeak()) {
                (*codeOut) << "weak global ";
            }
            else if (initializer)  {
                (*codeOut) << (isSgGlobal(decl -> get_scope()) ? "global " : "private unnamed_addr constant ");
            }
            else {
                (*codeOut) << "common global ";
            }

            // TODO: Visibility attributes should be translated here, but it is
            // not clear where that is found in ROSE. Multiple relevant nodes
            // seem to have visibility attributes: SgDeclarationModifier,
            // SgDeclarationStatement, and SgInitializedName, with no
            // documentation for any of them. Leave this out until we can
            // figure out which (if any) is actually used.

            /**
             * if this declaration is a local declaration and it has an initializer, then 
             * generate the initialization code here.
             */
            if (initializer) {
// TODO: Remove this !!!
/*      
cout << "*** Looking at initializer: " << initializer -> class_name() << endl;
cout.flush();
*/
                if (dynamic_cast<SgAssignInitializer *>(initializer)) {
                    SgAssignInitializer *assign_initializer = isSgAssignInitializer(initializer);
                    genGlobalExpressionInitialization(decl, var_type_name, assign_initializer -> get_operand());
                }
                else if (dynamic_cast<SgAggregateInitializer *>(initializer)) {
                    SgAggregateInitializer *aggregate_initializer = isSgAggregateInitializer(initializer);
                    (*codeOut) << var_type_name << " ";
                    genGlobalAggregateInitialization(decl, aggregate_initializer);
                }
                else if (dynamic_cast<SgStringVal *>(initializer)) {
                    SgStringVal *string_literal = isSgStringVal(initializer);
// TODO: Remove this !!!
/*      
cout << "*** (4.0) Looking at String value: \"" << string_literal -> get_value() << "\"" << endl;
cout.flush();
*/
                    (*codeOut) << var_type_name;
                    if (decl -> attributeExists(Control::LLVM_STRING_INDEX)) { 
                        int string_index = ((IntAstAttribute *) decl -> getAttribute(Control::LLVM_STRING_INDEX)) -> getValue(); 
                        (*codeOut) << " c\"" << attributes -> getString(string_index) << "\"";
//                        (*codeOut) << " c\"" << attributes -> filter(string_literal -> get_value(), string_size) << "\"";
// TODO: Remove this !!!
/*      
cout << "*** (4.5) Looking at filtered String value: \"" << attributes -> getString(string_index)  << "\"" << endl;
cout.flush();
*/
                    }
                    else {
                        (*codeOut) << " " << ((StringAstAttribute *) string_literal -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
// TODO: Remove this !!!
/*      
cout << "*** (4.75) Looking at RESULT String value: \"" << ((StringAstAttribute *) string_literal -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue() << "\"" << endl;
cout.flush();
*/
                    }
                }
                else {
                    cout << "Don't know how to process Initializer element of type " << initializer -> class_name() << std::endl;
                    cout.flush();
                    ROSE2LLVM_ASSERT(! "This should not happen");
                }
            }
            else {
                (*codeOut) << var_type_name;
                if (! sm.isExtern()) { // non-extern vars must be initialized
                    (*codeOut) << " zeroinitializer";
                }
            }

            if (decl -> get_gnu_attribute_section_name() != "")
                (*codeOut) << ", section \"" << decl -> get_gnu_attribute_section_name() << "\"";

            SgArrayType *arr_type = dynamic_cast<SgArrayType *>(var_type);
            if (attributes -> isValignType(var_type) || (arr_type && attributes -> isValignType(arr_type->get_base_type()))) {
                (*codeOut) << attributes -> getVectorAlignmentStr();
            }
            else if (decl -> attributeExists(Control::LLVM_ALIGN_VAR)) {
                int alignment = ((IntAstAttribute *) decl -> getAttribute(Control::LLVM_ALIGN_VAR)) -> getValue();
                (*codeOut) << attributes -> getAlignmentStr(alignment);
            }
            else if (var_type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
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
        (*codeOut) << "declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1" << endl;
    }
    if (attributes -> needsStacksave()) {
        (*codeOut) << "declare i8* @llvm.stacksave()" << endl;
        (*codeOut) << "declare void @llvm.stackrestore(i8* %ptr)" << endl;
    }
    for (int i = 0; i < attributes -> numFunctions() ; i++) {
        if (! attributes -> isDefinedFunction(attributes -> getFunction(i))) {
            (*codeOut) << "declare " << attributes -> getFunction(i) << endl;
        }
    }

    return;
}


void CodeGeneratorVisitor::genGlobalExpressionInitialization(SgInitializedName *decl, string element_type_name, SgExpression *expression) {
    ConstantCodeEvaluator evaluator(attributes);
    ConstantValue x = evaluator.traverse(expression);

    if (x.hasArithmeticValue()) {
        SgType *expression_type = attributes -> getSourceType(expression -> get_type());
        if (isSgTypeFloat(expression_type)) {
            ROSE2LLVM_ASSERT(x.hasFloatValue());
            (*codeOut) << element_type_name << " " << Control::FloatToString((float) x.double_value);
        }
        else if (isSgTypeDouble(expression_type)) {
            ROSE2LLVM_ASSERT(x.hasDoubleValue());
            (*codeOut) << element_type_name << " " << Control::DoubleToString(x.double_value);
        }
        else if (isSgTypeLongDouble(expression_type)) {
            ROSE2LLVM_ASSERT(x.hasLongDoubleValue());
            (*codeOut) << element_type_name << " " << Control::LongDoubleToString(x.long_double_value);
        }
        else if (attributes -> isIntegerType(expression_type)) {
            ROSE2LLVM_ASSERT(x.hasIntValue());
            // string expr_name = ((StringAstAttribute *) expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
            (*codeOut) << element_type_name << " " << x.int_value;
        }
        else if (isSgTypeBool(expression_type)) {
            ROSE2LLVM_ASSERT(x.hasIntValue());
            // string expr_name = ((StringAstAttribute *) expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
            (*codeOut) << element_type_name << " " << (x.int_value ? "true" : "false");
        }
        else if (isSgPointerType(expression_type)) {
            ROSE2LLVM_ASSERT(x.hasIntValue());
            // string expr_name = ((StringAstAttribute *) expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
            (*codeOut) << element_type_name << " ";
            if (x.int_value == 0)
                 (*codeOut) << "null";
            else (*codeOut) << "inttoptr (i64 " << x.int_value  << " to i32*)";
        }
        else {
            cerr << "Don't know yet how to process initializer expression \"" << expression -> class_name() << "\" of type: " << expression_type -> class_name() << endl;
            cerr.flush();
            ROSE2LLVM_ASSERT(0);
        }
    }
    else if (x.string_literal) {
        if (decl -> attributeExists(Control::LLVM_STRING_INDEX)) { 
            int string_index = ((IntAstAttribute *) decl -> getAttribute(Control::LLVM_STRING_INDEX)) -> getValue(); 
            (*codeOut) << element_type_name << " c\"" << attributes -> getString(string_index) << "\"";
// TODO: Remove this !!!
/*      
//            (*codeOut) << element_type_name << " c\"" << attributes -> filter(x.string_literal -> get_value(), string_size) << "\"";
cout << "*** (5.0) Looking at String value: \"" << attributes -> getString(string_index) << "\" with size " << attributes -> getStringLength(string_index) 
     << " with ORIGINAL value \"" << x.string_literal -> get_value() << "\"" << endl;
cout.flush();
*/
        }
        else {
            (*codeOut) << "i8* " << ((StringAstAttribute *) x.string_literal -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
// TODO: Remove this !!!
/*      
cout << "*** (5.5) Looking at PLAIN String value: \"" << ((StringAstAttribute *) x.string_literal -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue() << "\"" << endl;
cout.flush();
*/
        }
    }
    else if (x.function_reference) {
        string function_name = ((StringAstAttribute *) x.function_reference -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
               function_type_name = ((StringAstAttribute *) attributes -> getExpressionType(x.function_reference) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
        (*codeOut) << " " << element_type_name << " bitcast (" << function_type_name << "* " << function_name << " to " << element_type_name << ")";
    }
    else {
// TODO: Remove this !!!
/*
cout << "*** (5.6) Dumping code: \"" << x.getCode() << "\"" << endl;
cout.flush();
*/
        (*codeOut) << element_type_name << " " << x.getCode();// << ((StringAstAttribute *) expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
    }
}

void CodeGeneratorVisitor::genGlobalAggregateInitialization(SgInitializedName *decl, SgAggregateInitializer *aggregate) {
    AggregateAstAttribute *attribute = (AggregateAstAttribute *) aggregate -> getAttribute(Control::LLVM_AGGREGATE);
    SgArrayType *array_type = attribute -> getArrayType();
    SgClassType *class_type = attribute -> getClassType();
    ROSE2LLVM_ASSERT(array_type || class_type);

    (*codeOut) << (array_type ? " [" : " { ");
    vector<SgExpression *> exprs = aggregate -> get_initializers() -> get_expressions();
    DeclarationsAstAttribute *class_attribute = (class_type ? attributes -> class_map[class_type -> get_qualified_name().getString()] : NULL);
    int type_limit = (array_type ? (array_type -> get_index() -> attributeExists(Control::LLVM_CONSTANT_VALUE)
                                                ? ((IntAstAttribute *) array_type -> get_index() -> getAttribute(Control::LLVM_CONSTANT_VALUE)) -> getValue()
                                                : exprs.size())
                                 : class_attribute -> numSgInitializedNames());
    if (class_type && isSgClassDeclaration(class_type->get_declaration()) -> get_class_type() == SgClassDeclaration::e_union) {
        type_limit = 1;
    }
    for (int i = 0; i < type_limit; i++) {
        SgType *element_type = attributes -> getSourceType(array_type
                                                                ? array_type -> get_base_type()
                                                                : class_attribute -> getSgInitializedName(i) -> attributeExists(Control::LLVM_DECLARATION_TYPE)
                                                                       ? ((SgTypeAstAttribute *) class_attribute -> getSgInitializedName(i) -> getAttribute(Control::LLVM_DECLARATION_TYPE)) -> getType()
                                                                       :  class_attribute -> getSgInitializedName(i) -> get_type());
        ROSE2LLVM_ASSERT((StringAstAttribute *) element_type -> attributeExists(Control::LLVM_TYPE));
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
                genGlobalAggregateInitialization(decl, sub_aggregate);
            }
            else {
                SgAssignInitializer *assign_init = isSgAssignInitializer(exprs[i]);

            //
            // O2/25/2015: ROSE Issue... A regular expression may now appear directly in an aggregate initializer.
            //
            //    ROSE2LLVM_ASSERT(assign_init);

                SgExpression *operand = (assign_init ? assign_init -> get_operand() : exprs[i]);

                SgArrayType *sub_array_type = isSgArrayType(element_type);
                if (sub_array_type) {
                    SgStringVal *init_string = isSgStringVal(operand);
                    ROSE2LLVM_ASSERT(init_string);
                    string value = init_string -> get_value();
                    string sub_aggregate_type_name = ((StringAstAttribute *) element_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                    int string_index = ((IntAstAttribute *) init_string -> getAttribute(Control::LLVM_STRING_INDEX)) -> getValue(); 
                    (*codeOut) << sub_aggregate_type_name << " c\"" << attributes -> getString(string_index) << "\"";
                }
                else {
                    genGlobalExpressionInitialization(decl, element_type_name, operand);
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

    (*codeOut) << (array_type ? "]" : " }");

    return;
}


void CodeGeneratorVisitor::genBinaryCompareOperation(SgBinaryOp *node, string condition_code, string const &debug_md) {
     SgExpression *lhs_operand = node -> get_lhs_operand(),
                  *rhs_operand = node -> get_rhs_operand();
     SgType *lhs_type = attributes -> getSourceType(attributes -> getExpressionType(lhs_operand)),
            *rhs_type = attributes -> getSourceType(attributes -> getExpressionType(rhs_operand));
     string code;
     if (attributes -> isFloatType(lhs_type))
         code = "u"; // unordered
     else if (condition_code.compare("eq") != 0 && condition_code.compare("ne") != 0) { // integer? 
         code = (attributes -> isUnsignedType(lhs_type)  &&  attributes -> isUnsignedType(rhs_type)
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
// TODO: Remove this !!!
/*
cout << "The left operand type in the compare is: "
     <<  lhs_type -> class_name()
     << "; its type name is " << lhs_type_name
     << ". The right operand type in the compare is: "
     <<  rhs_type -> class_name()
     << endl;
cout.flush();
*/
     (*codeOut) << CodeEmitter::indent() << result_name << " = " << (attributes -> isFloatType(lhs_type) ? "f" : "i")
                << "cmp " << code << " " << lhs_type_name << (isSgFunctionType(lhs_type) ? "* " : " ") << lhs_name << ", " << rhs_name << debug_md << endl;
}


void CodeGeneratorVisitor::genZeroCompareOperation(SgExpression *node, string const &debug_md) {
    SgType *lhs_type = attributes -> getSourceType(attributes -> getExpressionType(node));
    bool is_float = attributes -> isFloatType(lhs_type);
    string code = (is_float ? "u" /* unordered */ : "");
    code += "ne";

    StringAstAttribute *result_attribute = (StringAstAttribute *) node -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);

    string result_name   = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_BOOLEAN_CAST)) -> getValue(),
           lhs_name      = (((isSgPlusPlusOp(node)   && isSgPlusPlusOp(node) -> get_mode() == SgUnaryOp::postfix) ||
                             (isSgMinusMinusOp(node) && isSgMinusMinusOp(node) -> get_mode() == SgUnaryOp::postfix))
                             ? result_attribute
                             : ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME))) -> getValue(),
           lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
           rhs_name      = ((! is_float) ? (isSgPointerType(lhs_type) ? "null" : "0")
                                         : isSgTypeLongDouble(lhs_type) ? Control::LongDoubleToString(0.0)
                                                                        : isSgTypeDouble(lhs_type) ? Control::DoubleToString(0.0)
                                                                                                   : Control::FloatToString(0.0));
    (*codeOut) << CodeEmitter::indent() << result_name << " = " << (is_float ? "f" : "i")
               << "cmp " << code << " " << lhs_type_name << " " << lhs_name << ", " << rhs_name << debug_md << endl;

    result_attribute -> resetValue(result_name);

    result_attribute = (StringAstAttribute *) node -> getAttribute(Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME);
    if (result_attribute) {
        result_attribute -> resetValue(result_name);
    }
}


void CodeGeneratorVisitor::genAddOrSubtractOperation(SgBinaryOp *node, string opcode, string const &debug_md) {
     SgExpression *lhs_operand = node -> get_lhs_operand(),
                  *rhs_operand = node -> get_rhs_operand();
     SgType *lhs_type = attributes -> getSourceType(attributes -> getExpressionType(lhs_operand)),
            *rhs_type = attributes -> getSourceType(attributes -> getExpressionType(rhs_operand));
     string result_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            lhs_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            rhs_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();

     //
     // O3/07/2015: ROSE Issue... This is no longer the case. Either the left or the right operand may be a pointer.  
     //
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
     SgArrayType *lhs_array_type = isSgArrayType(lhs_type),
                 *rhs_array_type = isSgArrayType(rhs_type);
     SgPointerType *lhs_pointer_type = isSgPointerType(attributes -> getSourceType(lhs_type)),
                   *rhs_pointer_type = isSgPointerType(attributes -> getSourceType(rhs_type));
     if (lhs_pointer_type || lhs_array_type) {
         string lhs_type_name = ((StringAstAttribute *) (lhs_operand -> attributeExists(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                             ? lhs_operand -> getAttribute(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                             : lhs_type -> getAttribute(Control::LLVM_TYPE))) -> getValue(),
                rhs_type_name = ((StringAstAttribute *) (rhs_operand -> attributeExists(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                           ? rhs_operand -> getAttribute(Control::LLVM_ARRAY_TO_POINTER_CONVERSION)
                                                           : rhs_type -> getAttribute(Control::LLVM_TYPE))) -> getValue();

         if (lhs_operand -> attributeExists(Control::LLVM_POINTER_TO_INT_CONVERSION)) {
             ROSE2LLVM_ASSERT((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_POINTER_TO_INT_CONVERSION));
             string lhs_cast_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_POINTER_TO_INT_CONVERSION)) -> getValue(),
                    rhs_cast_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_POINTER_TO_INT_CONVERSION)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << lhs_cast_name << " = ptrtoint " << lhs_type_name << " " <<  lhs_name << " to " << attributes -> getIntegerPointerTarget() << debug_md << endl;
             (*codeOut) << CodeEmitter::indent() << rhs_cast_name << " = ptrtoint " << rhs_type_name << " " <<  rhs_name << " to " << attributes -> getIntegerPointerTarget() << debug_md << endl;
             lhs_name = lhs_cast_name;
             rhs_name = rhs_cast_name;
             (*codeOut) << CodeEmitter::indent() << result_name << " = sub " << attributes -> getIntegerPointerTarget() << " " <<  lhs_cast_name << ", " << rhs_cast_name << debug_md << endl;
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
                         (*codeOut) << CodeEmitter::indent() << divide_name << " = ashr " << attributes -> getIntegerPointerTarget() << " " <<  result_name << ", " << shift_size << debug_md << endl;
                     }
                     else {
                         (*codeOut) << CodeEmitter::indent() << divide_name << " = sdiv " << attributes -> getIntegerPointerTarget() << " " <<  result_name << ", " << size << debug_md << endl;
                     }
                 }
             }
         }
         else {
             if (isSgSubtractOp(node)) {
                 ROSE2LLVM_ASSERT((StringAstAttribute *) node -> getAttribute(Control::LLVM_NEGATION_NAME));
                 string negation_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NEGATION_NAME)) -> getValue();
                 if (! (isSgIntVal(rhs_operand) || isSgEnumVal(rhs_operand))) { // not a constant value?
                     (*codeOut) << CodeEmitter::indent() << negation_name << " = sub " << rhs_type_name << " 0, " << rhs_name << debug_md << endl;
                 }
                 rhs_name = negation_name;
             }

// TODO: Remove this !!!
/*
cout
  << "At node "
  << node -> class_name()
  << " with left operand "
  << lhs_operand -> class_name()
  << " and right operand "
  << rhs_operand -> class_name()
  << "; The "
  << (lhs_array_type ? "array " : "")
  << "lhs_type_name is: " << lhs_type_name.substr(0, lhs_type_name.length() - 1)
  << "; the pointer is: " << lhs_type_name
  << "; the rhs_name is: " << rhs_name
  << endl;
cout.flush();
*/
             ROSE2LLVM_ASSERT(lhs_array_type || (lhs_type_name.length() > 1 && lhs_type_name[lhs_type_name.length() - 1] == '*'));
             string element_type_name = (lhs_array_type
                                            ? ((StringAstAttribute *) (lhs_array_type -> get_base_type()  -> getAttribute(Control::LLVM_TYPE))) -> getValue()
                                            : lhs_type_name.substr(0, lhs_type_name.length() - 1));
             (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds " << element_type_name << ", " << element_type_name << "* " <<  lhs_name << ", " << rhs_type_name << " " << rhs_name << debug_md << endl;
         }
     }
     /**
      *   If we are dealing with q = 3 + a.
      */
     else if (rhs_array_type || rhs_pointer_type) {
         ROSE2LLVM_ASSERT(! rhs_operand -> attributeExists(Control::LLVM_ARRAY_TO_POINTER_CONVERSION));
         string lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                rhs_type_name = ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

         SgType *element_type = (rhs_array_type
                                     ? rhs_array_type -> get_base_type()
                                     : rhs_pointer_type -> get_base_type());
         string element_type_name = ((StringAstAttribute *) element_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
// TODO: Remove this !!!
/*
cout
  << "The rhs_type_name is: " << rhs_type_name
  << "; The element_type_name is: " << element_type_name
  << endl;
cout.flush();
*/
         (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds " << element_type_name << ", "
                                             << rhs_type_name << " " << rhs_name << ", " << lhs_type_name << " " <<  lhs_name << debug_md << endl;
     }
     else {
         string type_name = ((StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << result_name << " = " << (attributes -> isFloatType(node -> get_type()) ? "f" : "") << opcode
                    << " " <<  type_name << " " << lhs_name << ", " << rhs_name << debug_md << endl;
     }
}


void CodeGeneratorVisitor::genAddOrSubtractOperationAndAssign(SgBinaryOp *node, string opcode, string const &debug_md) {
     genBasicBinaryOperationAndAssign(node, opcode, debug_md);
}


void CodeGeneratorVisitor::genBasicBinaryOperation(SgBinaryOp *node, string opcode, string const &debug_md, bool type_from_lhs) {
     string result_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            lhs_name = ((StringAstAttribute *) node -> get_lhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            rhs_name = ((StringAstAttribute *) node -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
     SgExpression *node_for_type = type_from_lhs ? node -> get_lhs_operand() : node;
     string type_name = ((StringAstAttribute *) attributes -> getExpressionType(node_for_type) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
     (*codeOut) << CodeEmitter::indent() << result_name << " = " << (attributes -> isFloatType(attributes -> getExpressionType(node)) ? "f" : "") << opcode
                << " " <<  type_name << " " << lhs_name << ", " << rhs_name << debug_md << endl;
}


void CodeGeneratorVisitor::genBasicBinaryOperationAndAssign(SgBinaryOp *node, string opcode, string const &debug_md, bool op_signedness) {
     SgExpression *lhs_operand = node -> get_lhs_operand(),
                  *rhs_operand = node -> get_rhs_operand();
     SgType *lhs_type = attributes -> getSourceType(attributes -> getExpressionType(lhs_operand)),
            *rhs_type = attributes -> getSourceType(attributes -> getExpressionType(rhs_operand));
     string result_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NAME)) -> getValue(),
            lhs_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            rhs_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
            ref_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue(),
            lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            rhs_type_name = ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
            result_type_name = ((StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
     SgExpression *node_for_result_type = node;

     /**
      * The left-hand side operand requires promotion
      */
     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) {
         string promote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (attributes -> isUnsignedType(lhs_type) ? "zext " : "sext ")
                    << lhs_type_name << " " <<  lhs_name << " to " << rhs_type_name << debug_md << endl;
         lhs_name = promote_name;
         result_type_name = rhs_type_name;
         node_for_result_type = rhs_operand;
     }
     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_PROMOTION)) {
         string promote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_PROMOTION)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (attributes -> isUnsignedType(lhs_type) ? "uitofp " : "sitofp ")
                    << lhs_type_name << " " <<  lhs_name << " to " << rhs_type_name << debug_md << endl;
         lhs_name = promote_name;
         result_type_name = rhs_type_name;
         node_for_result_type = rhs_operand;
     }
     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_FP_PROMOTION)) {
         string promote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_FP_PROMOTION)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << promote_name << " = fpext "
                    << lhs_type_name << " " <<  lhs_name << " to " << rhs_type_name << debug_md << endl;
         lhs_name = promote_name;
         result_type_name = rhs_type_name;
         node_for_result_type = rhs_operand;
     }

     /**
      * The right-hand side operand requires promotion
      */
     if (rhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) {
         string promote_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (attributes -> isUnsignedType(rhs_type) ? "zext " : "sext ")
                    << rhs_type_name << " " <<  rhs_name << " to " << lhs_type_name << debug_md << endl;
     }

     /**
      * Handle pointer arithmetic.
      */
     if (isSgPointerType(lhs_type) || isSgArrayType(lhs_type)) {
         if (isSgMinusAssignOp(node)) {
             string negation_name = ((StringAstAttribute *) node -> getAttribute(Control::LLVM_NEGATION_NAME)) -> getValue();
             if (! (isSgIntVal(rhs_operand) || isSgEnumVal(rhs_operand))) { // not a constant value?
                 (*codeOut) << CodeEmitter::indent() << negation_name << " = sub " << rhs_type_name << " 0, " << rhs_name << debug_md << endl;
             }
             rhs_name = negation_name;
         }

         ROSE2LLVM_ASSERT(lhs_type_name.length() > 1 && lhs_type_name[lhs_type_name.length() - 1] == '*');
// TODO: Remove this !!!
/*
cout << "; The lhs_type_name is: " << lhs_type_name.substr(0, lhs_type_name.length() - 1)
     << "; the pointer is: " << lhs_type_name
     << endl;
cout.flush();
*/
         (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds " << lhs_type_name.substr(0, lhs_type_name.length() - 1) << ", " << lhs_type_name << " " <<  lhs_name << ", " << rhs_type_name << " " << rhs_name << debug_md << endl;
     }
     else if (isSgPointerType(rhs_type) || isSgArrayType(rhs_type)) {
         ROSE2LLVM_ASSERT(! "This is not supposed to happen !!!");
     }
     else {
         string opcode_qualified = opcode;
         if (attributes -> isFloatType(attributes -> getExpressionType(node_for_result_type))) {
             opcode_qualified = "f" + opcode;
         }
         else if (op_signedness) {
             opcode_qualified = (attributes -> isUnsignedType(attributes -> getExpressionType(node_for_result_type)) ? "u" : "s") + opcode;
         }
         (*codeOut) << CodeEmitter::indent() << result_name << " = " << opcode_qualified
                    << " " <<  result_type_name << " " << lhs_name << ", " << rhs_name << debug_md << endl;
     }

     /**
      * The left-hand side operand requires demotion.
      */
     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION)) {
         string  demote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION)) -> getValue();
         StringAstAttribute *result_attribute = (StringAstAttribute *) node -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME),
                            *result_type_attribute = (StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE);
         (*codeOut) << CodeEmitter::indent() << demote_name << " = trunc " << result_type_name << " " <<  result_name << " to " << result_type_attribute -> getValue() << debug_md << endl;
         result_name = demote_name;
         result_type_name = result_type_attribute -> getValue();
     }
     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_DEMOTION)) {
         string  demote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_DEMOTION)) -> getValue();
         StringAstAttribute *result_attribute = (StringAstAttribute *) node -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME),
                            *result_type_attribute = (StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE);
         (*codeOut) << CodeEmitter::indent() << demote_name << " = " << (attributes -> isUnsignedType(lhs_type) ? "fptoui " : "fptosi ") << result_type_name << " " <<  result_name << " to " << result_type_attribute -> getValue() << debug_md << endl;
         result_name = demote_name;
         result_type_name = result_type_attribute -> getValue();
     }
     if (lhs_operand -> attributeExists(Control::LLVM_OP_AND_ASSIGN_FP_DEMOTION)) {
         string  demote_name = ((StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_OP_AND_ASSIGN_FP_DEMOTION)) -> getValue();
         StringAstAttribute *result_attribute = (StringAstAttribute *) node -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME),
                            *result_type_attribute = (StringAstAttribute *) attributes -> getExpressionType(node) -> getAttribute(Control::LLVM_TYPE);
         (*codeOut) << CodeEmitter::indent() << demote_name << " = fptrunc " << result_type_name << " " <<  result_name << " to " << result_type_attribute -> getValue() << debug_md << endl;
         result_name = demote_name;
         result_type_name = result_type_attribute -> getValue();
     }
     string alignment = attributes->addVectorAlignment(lhs_operand);
     string bundle_md = attributes->addBundleMetadata(lhs_operand);

     (*codeOut) << CodeEmitter::indent() << "store " << result_type_name << " " << result_name << ", " << result_type_name << "* " << ref_name << alignment << debug_md << bundle_md << endl;
}


void CodeGeneratorVisitor::genDivideBinaryOperation(SgBinaryOp *node, string opcode, string const &debug_md) {
    if (! attributes -> isFloatType(attributes -> getExpressionType(node))) {
        opcode = (attributes -> isUnsignedType(attributes -> getExpressionType(node)) ? "u" : "s") + opcode;
    }
    genBasicBinaryOperation(node, opcode, debug_md, true);
}

void CodeGeneratorVisitor::genDivideBinaryOperationAndAssign(SgBinaryOp *node, string opcode, string const &debug_md) {
    genBasicBinaryOperationAndAssign(node, opcode, debug_md, true);
}

/**
 *
 */
SgFunctionType *CodeGeneratorVisitor::getFunctionType(SgType *type) {
    SgFunctionType *function_type = isSgFunctionType(type);
    if (function_type) {
        return function_type;
    }

    SgPointerType *pointer_type = isSgPointerType(type);
    if (pointer_type) {
        return getFunctionType(pointer_type -> get_base_type());
    }

    SgTypedefType *typedef_type = isSgTypedefType(type);
    if (typedef_type) {
        return getFunctionType(typedef_type -> get_base_type());
    }

    cout << "Don't know how to process Function Type " << type -> class_name() << std::endl;
    cout.flush();
    ROSE2LLVM_ASSERT(! "This should not happen");
    return NULL;
}

/**
 * TODO: Obsolete!!!
 */
/*
void CodeGeneratorVisitor::generateArraySizeCode(const std::string &result_name, SgType *result_type, SgArrayType *array_type) {
    int alignment = ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
    string result_type_name = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
    SgType *base_type = array_type -> get_base_type();
    int size = ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
    if (size > 0) {
        ROSE2LLVM_ASSERT(! array_type -> get_index() -> attributeExists(Control::LLVM_CONSTANT_VALUE));
        ROSE2LLVM_ASSERT(array_type -> get_index() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
        string index_name = ((StringAstAttribute *) array_type -> get_index() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
        (*codeOut) << CodeEmitter::indent() << result_name << " = mul " << result_type_name << " " << index_name << ", " << size
                   << endl;
    }
    else {
        string temp = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);
        SgArrayType *subarray_type = isSgArrayType(base_type);
        ROSE2LLVM_ASSERT(subarray_type);
        generateArraySizeCode(temp, result_type, subarray_type);

        IntAstAttribute *attribute =  (IntAstAttribute *) array_type -> get_index() -> getAttribute(Control::LLVM_CONSTANT_VALUE);
        if (attribute) {
            (*codeOut) << CodeEmitter::indent() << result_name << " = mul " << result_type_name << " " << attribute -> getValue() << ", " << temp
                       << endl;
        }
        else {
            string index_name = ((StringAstAttribute *) array_type -> get_index() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
            (*codeOut) << CodeEmitter::indent() << result_name << " = mul " << result_type_name << " " << index_name  << ", " << temp
                       << endl;
        }
    }
}
*/

/**
 * Check whether or not this node should be visited.  If so, perform any required preprocessing.
 */
bool CodeGeneratorVisitor::preVisitEnter(SgNode *node) {
     /**
      *
      */
     if (option.isSyntheticTranslation()) {
         if (! option.isTranslating()) {
            /**
             * For the unrolled loop, and local declarations, this attribute is set.
             */
             if (node -> attributeExists(Control::LLVM_COST_ANALYSIS)) {
                 option.setTranslating();
             }
             else return false;
         }
     }

     /**
      *
      */
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
         return false;
     }

     /**
      * Special case for cost analysis
      */
     if (node -> attributeExists(Control::LLVM_COST_ANALYSIS)) {
         // TODO: emit metadata flag
         //???
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
      * Special case for expressions
      */
     if (dynamic_cast<SgExpression *>(node)) {
         SgExpression *n = isSgExpression(node);

         /**
          * Special case for conditional true and false expressions
          */
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

         /**
          * Ignore constant expressions that have already been evaluated.
          */
         if (n -> attributeExists(Control::LLVM_IGNORE) ||
             n -> attributeExists(Control::LLVM_CONSTANT_VALUE) ||
             n -> attributeExists(Control::LLVM_NULL_VALUE)) {
// TODO: Remove this !!!
/*
cout
  << "Suspending traversal of node "
  << node -> class_name()
  << endl;
cout.flush(); 
*/
             visit_suspended_by_node = node;
         }
         /**
          * The following casts have also already been taken care of and no code need to be generated for them.
          */
         else if (isSgCastExp(n)) {
             SgCastExp *cast_expression = isSgCastExp(n);
             if (isSgCharVal(cast_expression -> get_operand()) ||
                 isSgUnsignedCharVal(cast_expression -> get_operand())) {
                 visit_suspended_by_node = node; // Nothing to do for these cases.
             }
         }
     }

     return true;
}


/**
 * Pre-visit this node.
 */
void CodeGeneratorVisitor::preVisit(SgNode *node) {
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
     else if (dynamic_cast<SgInitializedName *>(node)) { // preVisit
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
     else if (dynamic_cast<SgSourceFile *>(node)) { // preVisit
         SgSourceFile *n = isSgSourceFile(node);

         if (option.isQuery() && (! node -> attributeExists(Control::LLVM_TRANSLATE))) {
             visit_suspended_by_node = node; // ignore this file
         }
         else {
             setAttributes((LLVMAstAttributes *) n -> getAttribute(Control::LLVM_AST_ATTRIBUTES));
             generateGlobals(); // generate globals declarations
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
     //                 SgBasicBlock
     //                 SgIfStmt
     else if (dynamic_cast<SgIfStmt *>(node)) { // preVisit
         SgIfStmt *n = isSgIfStmt(node);
     }
     //                 SgForStatement
     else if (dynamic_cast<SgForStatement *>(node)) { // preVisit
         SgForStatement *n = isSgForStatement(node);

         scopeStack.push(n);
     }
     //                 SgFunctionDefinition
     //                 SgClassDefinition:
     //                     SgTemplateInstantiationDefn
     //                 SgWhileStmt
     else if (dynamic_cast<SgWhileStmt *>(node)) { // preVisit
         SgWhileStmt *n = isSgWhileStmt(node);

         scopeStack.push(n);

         WhileAstAttribute *attribute = ( WhileAstAttribute *) n -> getAttribute(Control::LLVM_WHILE_LABELS);
         codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel(), attributes->addDebugMetadata(node, current_function_decls));
         codeOut -> emitLabel(current_function_decls, attribute -> getConditionLabel());
     }
     //                 SgDoWhileStmt
     else if (dynamic_cast<SgDoWhileStmt *>(node)) { // preVisit
         SgDoWhileStmt *n = isSgDoWhileStmt(node);

         scopeStack.push(n);

         DoAstAttribute *attribute = (DoAstAttribute *) n -> getAttribute(Control::LLVM_DO_LABELS);
         codeOut -> emitUnconditionalBranch(attribute -> getBodyLabel(), attributes->addDebugMetadata(node, current_function_decls));
         codeOut -> emitLabel(current_function_decls, attribute -> getBodyLabel());
     }
     //                 SgSwitchStatement
     else if (dynamic_cast<SgSwitchStatement *>(node)) { // preVisit
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
     else if (dynamic_cast<SgFunctionDeclaration *>(node)) { // preVisit
         SgFunctionDeclaration *n = isSgFunctionDeclaration(node);
         if ((! n -> get_definition()) || // A function header without definition
             (option.isQuery() && (! n -> attributeExists(Control::LLVM_TRANSLATE))) || // a query translation that is not applicable to this function
             n -> attributeExists(Control::LLVM_IGNORE)) { // A function that requires full traversal - Ignore it here on the regular pass.
             visit_suspended_by_node = node;
         }
         else {
             SgFunctionModifier &functionModifier = n -> get_functionModifier();
             SgDeclarationModifier &functionDeclarationModifier = n -> get_declarationModifier();
             SgTypeModifier &functionTypeModifier = functionDeclarationModifier.get_typeModifier();

             if (!precedingPragmas.empty()) {
                 // Record into metadata all consecutive pragmas
                 // preceding each function definition.  For now, ignore
                 // pragmas preceding only a function declaration
                 // because that's more effort to implement and we don't
                 // appear to need it yet.  Thus, ignore pragmas for
                 // functions whose definitions are not translated.
                 attributes->addFunctionPragmaMetadata(n->get_name().getString(), precedingPragmas);
             }
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
             (*codeOut) << "define ";
             if (functionModifier.isGnuAttributeWeak())
                 (*codeOut) << "weak ";
             if (functionTypeModifier.isGnuAttributeStdcall())
                 (*codeOut) << "x86_stdcallcc ";

             (*codeOut) << (sm.isStatic() ? "internal " : "") << return_type_name << " @" << n -> get_name().getString() << "(";

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
                 // SgDeclarationStatement *decl = parm -> get_declaration();
                 SgType *orig_type = parm -> get_type();
                 SgType *type = attributes -> getSourceType(orig_type); // original type
                 string type_name = ((StringAstAttribute *) parm -> getAttribute(Control::LLVM_TYPE)) -> getValue();
//
// TODO: Remove this !
//
/*  
cout
    << " ***Processing parameter "
    << i
    << ", " << parm -> get_name().getString() << ", "
    << " of function "
  << n -> get_name().getString()
  << " with type "
  << type_name
  << endl;
cout.flush();
*/
               (*codeOut) << type_name;
                 if (SageInterface::isRestrictType(orig_type)) {
                     (*codeOut) << " noalias"; 
                 }

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
             (*codeOut) << ") nounwind";

             if (functionModifier.isGnuAttributePure())
                 (*codeOut) << " readonly";
             if (functionModifier.isInline())
                 (*codeOut) << " inlinehint";
             if (functionModifier.isGnuAttributeNoInline())
                 (*codeOut) << " noinline";
             if (functionModifier.isGnuAttributeAlwaysInline())
                 (*codeOut) << " alwaysinline";
             if (functionModifier.isGnuAttributeNaked())
                 (*codeOut) << " naked";

             if (functionTypeModifier.isGnuAttributeConst())
                 (*codeOut) << " readnone";
             if (functionTypeModifier.isGnuAttributeNoReturn())
                 (*codeOut) << " noreturn";

             string section = functionDeclarationModifier.get_gnu_attribute_section_name();
             if (section != "")
                 *(codeOut) << " section \"" << section << "\"";

             int alignment = functionTypeModifier.get_gnu_attribute_alignment();
//
// TODO: This is a patch!!! This issue needs to be addressed properly
//
//cout << "; *** The ROSE Type Modifier alignment is " << alignment << std::endl;
if (alignment < 1) {
alignment = 8;
//alignment = ((IntAstAttribute *) type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
//cout << "; *** The new alignment is " << alignment << std::endl;
}
             if (alignment != 0) {
                 *(codeOut) << " align " << alignment;
             }

             (*codeOut) << " {" << endl;

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
                  (*codeOut) << attributes->addDebugMetadata(node, current_function_decls) << endl;
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

                     if (decl -> attributeExists(Control::LLVM_ALIGN_VAR)) {
                         int alignment = ((IntAstAttribute *) decl -> getAttribute(Control::LLVM_ALIGN_VAR)) -> getValue();
                         (*codeOut) << attributes->getAlignmentStr(alignment);
                     }
                     else {
                         IntAstAttribute *alignment_attribute = (IntAstAttribute *) type -> getAttribute(Control::LLVM_ALIGN_TYPE);
                         if (alignment_attribute) {
                             (*codeOut) << ", align " << alignment_attribute -> getValue();
                         }
                     }
                     (*codeOut) << attributes->addDebugMetadata(decl, current_function_decls) << endl;
                 }
             }

             /**
              * Declare temporary names used for coercion.
              */
             if (n -> attributeExists(Control::LLVM_NEEDS_STACK)) {
                 string temp = attributes -> getTemp(LLVMAstAttributes::TEMP_INT);
                 (*codeOut) << CodeEmitter::indent() << "%.stacksave = alloca i8*, align 8" << endl;
                 (*codeOut) << CodeEmitter::indent() << temp << " = call i8* @llvm.stacksave()" << endl;
                 (*codeOut) << CodeEmitter::indent() << "store i8* " << temp << ", i8** %.stacksave, align 8" << endl;
             }

             /**
              * Declare temporary names used for coercion.
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
                 (*codeOut) << attributes->addDebugMetadata(node, current_function_decls) << endl;
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
                                << ", " << parm_type << "* " << parm_name << attributes->addDebugMetadata(node, current_function_decls) << endl;
                 }
             }

             /**
              *
              */
             if (option.isQuery()) {
                 option.setSyntheticTranslation();
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
     else if (dynamic_cast<SgExprStatement *>(node)) { // preVisit
         SgExprStatement *n = isSgExprStatement(node);

         if (n -> attributeExists(Control::LLVM_DO_LABELS)) {
             DoAstAttribute *attribute = (DoAstAttribute *) n -> getAttribute(Control::LLVM_DO_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel(), attributes->addDebugMetadata(node, current_function_decls));
             codeOut -> emitLabel(current_function_decls, attribute -> getConditionLabel());
         }
     }
     //             SgLabelStatement
     else if (dynamic_cast<SgLabelStatement *>(node)) { // preVisit
         SgLabelStatement *n = isSgLabelStatement(node);
         codeOut -> emitUnconditionalBranch(attributes -> findLabel(n), attributes->addDebugMetadata(node, current_function_decls));
         codeOut -> emitLabel(current_function_decls, attributes -> findLabel(n));
     }
     //             SgCaseOptionStmt
     else if (dynamic_cast<SgCaseOptionStmt *>(node)) { // preVisit
         SgCaseOptionStmt *n = isSgCaseOptionStmt(node);
         CaseAstAttribute *attribute = (CaseAstAttribute *) n -> getAttribute(Control::LLVM_CASE_INFO);
         ROSE2LLVM_ASSERT(attribute);
         if (! attribute -> reusedLabel()) {
             codeOut -> emitUnconditionalBranch(attribute -> getCaseLabel(), attributes->addDebugMetadata(node, current_function_decls));
             codeOut -> emitLabel(current_function_decls, attribute -> getCaseLabel());
         }
     }
     //             SgTryStmt
     //             SgDefaultOptionStmt
     else if (dynamic_cast<SgDefaultOptionStmt *>(node)) { // preVisit
         SgDefaultOptionStmt *n = isSgDefaultOptionStmt(node);
         string default_label = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_DEFAULT_LABEL)) -> getValue();
         codeOut -> emitUnconditionalBranch(default_label, attributes->addDebugMetadata(node, current_function_decls));
         codeOut -> emitLabel(current_function_decls, default_label);
     }
     //             SgBreakStmt
     //             SgContinueStmt
     //             SgReturnStmt
     //             SgGotoStatement
     //             SgSpawnStmt
     //             SgNullStatement
     else if (dynamic_cast<SgNullStatement *>(node)) { // preVisit
         SgNullStatement *n = isSgNullStatement(node);
         // TODO: Do nothing for now!!!
     }
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
     //             SgCompoundLiteralExp:
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
     else if (dynamic_cast<SgCastExp *>(node)) { // preVisit
         SgCastExp *n = isSgCastExp(node);
     }
     //                 SgThrowOp
     //                 SgRealPartOp
     //                 SgImagPartOp
     //                 SgConjugateOp
     //                 SgUserDefinedUnaryOp
     //             SgBinaryOp:
     //                 SgArrowExp
     else if (dynamic_cast<SgArrowExp *>(node)) { // preVisit
         SgArrowExp *n = isSgArrowExp(node);
     }
     //                 SgDotExp
     else if (dynamic_cast<SgDotExp *>(node)) { // preVisit
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
     //                 SgPntrArrRefExp
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
     else if (dynamic_cast<SgSizeOfOp *>(node)) { // preVisit
         SgSizeOfOp *n = isSgSizeOfOp(node);
         visit_suspended_by_node = node;
     }
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
                        // SgSizeOfOp  03/06/2015 SgSizeOfOp is no longer an SgValueExp but an SgExpression. 
                        SgUpcLocalsizeof
                        SgUpcBlocksizeof
                        SgUpcElemsizeof
          */
     else if (dynamic_cast<SgValueExp *>(node)) { // preVisit
         SgValueExp *n = isSgValueExp(node);
         visit_suspended_by_node = node;
     }
     //             SgTypeIdOp
     //             SgConditionalExp
     else if (dynamic_cast<SgConditionalExp *>(node)) { // preVisit
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

     /**
      * Update pragma list.
      */
     if (isSgPragma(node)) {
         precedingPragmas.push_back(isSgPragma(node));
         ROSE2LLVM_ASSERT(isSgPragmaDeclaration(precedingPragmas.back()->get_parent()));
     }
     else if (!isSgPragmaDeclaration(node) && !precedingPragmas.empty()) {
         // This is a non-pragma sibling following pragmas, so clear
         // list of preceding pragmas.
         precedingPragmas.clear();
     }

     return;
}


/**
 * Nothing to do on pre-visit exit.
 */
void CodeGeneratorVisitor::preVisitExit(SgNode *node) {}


/**
 * Check whether or not this node required a post-visit. If so, perform any required preprocessing.
 */
bool CodeGeneratorVisitor::postVisitEnter(SgNode *node) {
      /**
       *
       */
     if (option.isSyntheticTranslation() && (! option.isTranslating()) && (! isSgFunctionDeclaration(node))) {
         return false;
     }

      /**
       *
       */
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
         else return false;
     }

     return true;
}


/**
 * Post-visit this node.
 */
void CodeGeneratorVisitor::postVisit(SgNode *node) {
     /**
      * Handle case where pragmas have no following sibling.
      */
     if (!precedingPragmas.empty() && !isSgPragma(node) && !isSgPragmaDeclaration(node)) {
         ROSE2LLVM_ASSERT(node == precedingPragmas.front()->get_parent()->get_parent());
         ROSE2LLVM_ASSERT(node == precedingPragmas.back()->get_parent()->get_parent());
         precedingPragmas.clear();
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
     else if (dynamic_cast<SgInitializedName *>(node)) { // postVisit
         SgInitializedName *n = isSgInitializedName(node);
         SgType *var_type = (n -> attributeExists(Control::LLVM_DECLARATION_TYPE)
                                ? ((SgTypeAstAttribute *) n -> getAttribute(Control::LLVM_DECLARATION_TYPE)) -> getType()
                                : attributes -> getSourceType(n -> get_type()));
         ROSE2LLVM_ASSERT(var_type);
    
	 /**
          *
          */
         if (n -> attributeExists(Control::LLVM_NEEDS_STACK)) {
             SgArrayType *array_type = isSgArrayType(attributes -> getSourceType(n -> get_type()));
             ROSE2LLVM_ASSERT(array_type);
             control.getAdHocGeneratorVisitor(attributes) -> traverse(array_type -> get_index());
         }

	 /**
          *
          */
         SgInitializer *init = n -> get_initializer();
// TODO: Remove this !!!
/*
cout
  << "I am Here with variable "
  << n -> get_name().getString()
  << " of type "
  << var_type -> class_name()
  << (init ? " with an initializer" : "")
  << (n -> attributeExists(Control::LLVM_GLOBAL_DECLARATION) ? " (global)" : "")
  << " in scope "
  << n -> get_scope() -> class_name()
  << endl;
cout.flush();
*/
         if (init && (! isSgGlobal(n -> get_scope())) && (! n -> /* get_storageModifier() */ get_declaration() -> get_declarationModifier().get_storageModifier().isStatic())) {
             // (! n -> attributeExists(Control::LLVM_GLOBAL_DECLARATION))) {
             ROSE2LLVM_ASSERT((StringAstAttribute *) var_type -> getAttribute(Control::LLVM_TYPE));
             ROSE2LLVM_ASSERT((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME));
             string type_name = ((StringAstAttribute *) var_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    var_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             string debug_md = attributes->addDebugMetadata(node, current_function_decls);

             /**
              * if this declaration is a local declaration and it has an initializer, then 
              * generate the initialization code here.
              */
             if (dynamic_cast<SgAssignInitializer *>(init)) {
                 SgAssignInitializer *assign_init = isSgAssignInitializer(init);
                 ROSE2LLVM_ASSERT(assign_init -> get_operand() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
                 string rhs_name = ((StringAstAttribute *) assign_init -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 SgArrayType *array_type = isSgArrayType(var_type);
                 if (array_type) {
                     SgType *base_type = array_type -> get_base_type();
                     SgStringVal *str = isSgStringVal(assign_init -> get_operand()); // the string being copied may be shorter than the target.
                     ROSE2LLVM_ASSERT(str);
                     string target_bit_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue();
                     string aggregate_type_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     string subtype_name = ((StringAstAttribute *) array_type -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     int string_index = ((IntAstAttribute *) n -> getAttribute(Control::LLVM_STRING_INDEX)) -> getValue(); 
                     (*codeOut) << CodeEmitter::indent() << target_bit_name << " = bitcast " << aggregate_type_name << "* " << var_name << " to " << subtype_name << "*" <<  debug_md << endl;
                     (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.p0i8.p0i8.i64(i8* " << target_bit_name << ", " << subtype_name << "* " << rhs_name
                                << ", i64 " << attributes -> getStringLength(string_index) << ", i32 1, i1 false)" << debug_md << endl;
                 }
                 else if (isSgClassType(var_type)) {
                     ROSE2LLVM_ASSERT(n -> getAttribute(Control::LLVM_BIT_CAST));
                     string target_bit_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue();
                     ROSE2LLVM_ASSERT(var_type -> getAttribute(Control::LLVM_SIZE));
                     int size = ((IntAstAttribute *) var_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << target_bit_name << " = bitcast " << type_name << "* " << var_name << " to i8*" << debug_md << endl;
                     (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.p0i8.p0i8.i64(i8* " << target_bit_name << ", i8* " << rhs_name << ", i64 " << size << ", i32 4, i1 false)" << debug_md << endl;
                 }
                 else {
                     if (dynamic_cast<SgFunctionRefExp *>(assign_init -> get_operand())) {
                         SgFunctionRefExp *function = isSgFunctionRefExp(assign_init -> get_operand());
                         string function_name = ((StringAstAttribute *) function -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                                function_type_name = ((StringAstAttribute *) attributes -> getExpressionType(function) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                         (*codeOut) << CodeEmitter::indent() << "store " << type_name
                                    << " bitcast (" << function_type_name << "* " << function_name << " to " << type_name << "), "
                                    << type_name << "* " << var_name << debug_md << endl;
                     }
                     else if (isSgTypeComplex(var_type)) {
                         ROSE2LLVM_ASSERT(assign_init -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME));
                         ROSE2LLVM_ASSERT(assign_init -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
                         ROSE2LLVM_ASSERT(assign_init -> get_operand() -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME));

                         SgType *base_type = attributes -> getSourceType(isSgTypeComplex(var_type) -> get_base_type());
                         int alignment = ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                         string base_type_name = ((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                                rhs_real_name = ((StringAstAttribute *) assign_init -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                                rhs_imaginary_name = ((StringAstAttribute *) assign_init -> get_operand() -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME)) -> getValue(),
                                lhs_real = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC),
                                lhs_imaginary = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);
 
                         (*codeOut) << CodeEmitter::indent() << lhs_real << " = getelementptr inbounds " << type_name << ", " << type_name << "* " << var_name << ", i32 0, i32 0" << endl;
                         (*codeOut) << CodeEmitter::indent() << lhs_imaginary << " = getelementptr inbounds " << type_name << ", " << type_name << "* " << var_name << ", i32 0, i32 1" << endl;
     
                         (*codeOut) << CodeEmitter::indent() << "store " << base_type_name << " " << rhs_real_name << ", " << base_type_name << "* " << lhs_real << ", align " << alignment << endl;
                         (*codeOut) << CodeEmitter::indent() << "store " << base_type_name << " " << rhs_imaginary_name << ", " << base_type_name << "* " << lhs_imaginary << ", align " << alignment << endl;
                     }
                     else {
                         (*codeOut) << CodeEmitter::indent() << "store " << type_name << " " << rhs_name << ", " << type_name << "* " << var_name << debug_md << endl;
                     }
                 }
             }
             else if (dynamic_cast<SgAggregateInitializer *>(init)) {
                 SgAggregateInitializer *aggregate = isSgAggregateInitializer(init);
                 AggregateAstAttribute *attribute = (AggregateAstAttribute *) aggregate -> getAttribute(Control::LLVM_AGGREGATE);
                 SgArrayType *array_type = attribute -> getArrayType();
                 SgClassType *class_type = attribute -> getClassType();
                 ROSE2LLVM_ASSERT(array_type || class_type);

                 //
                 // Say something here !
                 //
                 ROSE2LLVM_ASSERT(aggregate -> getAttribute(Control::LLVM_BIT_CAST));
                 string target_bit_name = ((StringAstAttribute *) aggregate -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue();
                 string aggregate_type_name = (array_type && (! array_type -> get_is_variable_length_array())
                                                           ? (StringAstAttribute *) array_type -> getAttribute(Control::LLVM_TYPE)
                                                           : (StringAstAttribute *) aggregate -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 string global_constant_name =  ((StringAstAttribute *) n -> getAttribute(Control::LLVM_GLOBAL_CONSTANT_NAME)) -> getValue();

                 if (array_type) {
                     SgType *sub_type;
                     do {
                         sub_type = array_type -> get_base_type();
                     } while (array_type = isSgArrayType(sub_type));

                     ROSE2LLVM_ASSERT(sub_type);
                     ROSE2LLVM_ASSERT(sub_type -> getAttribute(Control::LLVM_TYPE));
                     string subtype_name = ((StringAstAttribute *) sub_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     int alignment = ((IntAstAttribute *) sub_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                     int size = ((IntAstAttribute *) var_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
// TODO: Remove this !
/*  
cout
  << "*** The Array "
  << n -> get_name().getString()
  << " has size "
  << size
  << endl;
cout.flush();
*/
                     (*codeOut) << CodeEmitter::indent() << target_bit_name << " = bitcast " << aggregate_type_name << "* " << var_name << " to " << /* subtype_name <<*/ "i8*" <<  debug_md << endl;
                     (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.p0i8.p0i8.i64(i8* " << target_bit_name << ", i8* bitcast ("
                                << aggregate_type_name << "* " << global_constant_name << " to i8*), i64 " << size << ", i32 " << alignment << ", i1 false)" << debug_md << endl;
                 }
                 else if (isSgClassType(var_type)) {
                     int size = ((IntAstAttribute *) var_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << target_bit_name << " = bitcast " << type_name << "* " << var_name << " to i8*" << debug_md << endl;
/**/                     (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.p0i8.p0i8.i64(i8* " << target_bit_name <<  ", i8* bitcast ("
                                    << type_name << "* " << global_constant_name <<  " to i8*), i64 " << size << ", i32 4, i1 false)" << debug_md << endl;
     
                 }
             }
             else if (init != NULL){
                 cerr << "Don't know yet how to process initializer of type: " << init -> class_name() << endl;
                 cerr.flush();
                 ROSE2LLVM_ASSERT(0);
             }
         }

         /**
          * If this declaration has non-constant dimension expressions, output the code emitted for them here.
          */
         DimensionAstAttribute *dimension_attribute = (DimensionAstAttribute *) n -> getAttribute(Control::LLVM_DIMENSIONS);
         if (dimension_attribute) {
// TODO: Remove this !!!

cout
  << "*** Flushing dimension Expressions buffer for variable "
  << dimension_attribute -> getVariableDeclaration() -> get_name().getString()
  << endl;
cout.flush();

             codeOut -> flushBuffer(dimension_attribute -> getBuffer());
         }
     }
     //         SgAttribute:
     //             SgPragma
     else if (dynamic_cast<SgPragma *>(node)) { // postVisit
     }
     //             SgBitAttribute:
     //                 SgFuncDecl_attr
     //                 SgClassDecl_attr
     //         Sg_File_Info
     //         SgFile:
     //             SgSourceFile
     else if (dynamic_cast<SgSourceFile *>(node)) { // postVisit
         SgSourceFile *n = isSgSourceFile(node);

         if ((! option.isQuery()) || node -> attributeExists(Control::LLVM_TRANSLATE)) {
             if (attributes -> numAdditionalFunctions() > 0) {
                 revisitAttributes.push_back(attributes);
             }
             else {
                 attributes->generateMetadataNodes();
             }
         }

         /**
          * Declare temporary names used for coercion.
          */
         if (attributes -> needsStacksave()) {
             cout << "*** We don't yet fully support automatic array variables!"
                  << endl;
             cout.flush(); 
             ROSE2LLVM_ASSERT(false);
         }
     }
     //             SgBinaryFile
     //             SgUnknownFile
     //         SgProject
     else if (dynamic_cast<SgProject*>(node)) { // postVisit
       // This is encountered when translateExternal_ is set in
       // RoseToLLVM because it causes the entire AST to be traversed.
     }
     //         SgOptions
     //         SgUnparse_Info
     //         SgBaseClass
     //         SgTypedefSeq
     //         SgTemplateParameter
     //         SgTemplateArgument
     //         SgDirectory
     //         SgFileList
     else if (dynamic_cast<SgFileList*>(node)) { // postVisit
       // This is encountered when translateExternal_ is set in
       // RoseToLLVM because it causes the entire AST to be traversed.
     }
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
     else if (dynamic_cast<SgGlobal *>(node)) { // postVisit
         SgGlobal *n = isSgGlobal(node);
     }
     //                 SgBasicBlock
     else if (dynamic_cast<SgBasicBlock *>(node)) { // postVisit
         SgBasicBlock *n = isSgBasicBlock(node);
     }
     //                 SgIfStmt
     else if (dynamic_cast<SgIfStmt *>(node)) { // postVisit
         SgIfStmt *n = isSgIfStmt(node);
         IfAstAttribute *attribute = (IfAstAttribute *) n -> getAttribute(Control::LLVM_IF_LABELS);
         codeOut -> emitLabel(current_function_decls, attribute -> getEndLabel());
     }
     //                 SgForStatement
     else if (dynamic_cast<SgForStatement *>(node)) { // postVisit
         SgForStatement *n = isSgForStatement(node);
         string debug_md = attributes->addDebugMetadata(node, current_function_decls);

         ROSE2LLVM_ASSERT(scopeStack.top() == n);
         scopeStack.pop();

         ForAstAttribute *attribute = (ForAstAttribute *) n -> getAttribute(Control::LLVM_FOR_LABELS);

         /**
          * If this for-statement had increment statements generate code that had been buffered for them here.
          */
         if (! isSgNullExpression(n -> get_increment())) {
             codeOut -> emitUnconditionalBranch(attribute -> getIncrementLabel(), debug_md);
             codeOut -> emitLabel(current_function_decls, attribute -> getIncrementLabel());
             codeOut -> flushTopBuffer();
         }
         string is_parallel_md = attributes->addIsParallelMetadata(n);
         codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel(), debug_md, is_parallel_md);
         codeOut -> emitLabel(current_function_decls, attribute -> getEndLabel());
     }
     //                 SgFunctionDefinition
     else if (dynamic_cast<SgFunctionDefinition *>(node)) { // postVisit
         SgFunctionDefinition *n = isSgFunctionDefinition(node);
     }
     //                 SgClassDefinition:
     else if (dynamic_cast<SgClassDefinition *>(node)) { // postVisit
         SgClassDefinition *n = isSgClassDefinition(node);
     }
     //                     SgTemplateInstantiationDefn
     //                 SgWhileStmt
     else if (dynamic_cast<SgWhileStmt *>(node)) { // postVisit
         SgWhileStmt *n = isSgWhileStmt(node);

         ROSE2LLVM_ASSERT(scopeStack.top() == n);
         scopeStack.pop();

         WhileAstAttribute *attribute = (WhileAstAttribute *) n -> getAttribute(Control::LLVM_WHILE_LABELS);
         codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel(), attributes->addDebugMetadata(node, current_function_decls));
         codeOut -> emitLabel(current_function_decls, attribute -> getEndLabel());
     }
     //                 SgDoWhileStmt
     else if (dynamic_cast<SgDoWhileStmt *>(node)) { // postVisit
         SgDoWhileStmt *n = isSgDoWhileStmt(node);

         ROSE2LLVM_ASSERT(scopeStack.top() == n);
         scopeStack.pop();
     }
     //                 SgSwitchStatement
     else if (dynamic_cast<SgSwitchStatement *>(node)) { // postVisit
         SgSwitchStatement *n = isSgSwitchStatement(node);

         ROSE2LLVM_ASSERT(scopeStack.top() == n);
         scopeStack.pop();

         ROSE2LLVM_ASSERT(switchStack.top() == n);
         switchStack.pop();

         SwitchAstAttribute *switch_attribute = (SwitchAstAttribute *) n -> getAttribute(Control::LLVM_SWITCH_INFO);
         codeOut -> emitUnconditionalBranch(switch_attribute -> getEndLabel(), attributes->addDebugMetadata(node, current_function_decls));
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
     else if (dynamic_cast<SgFunctionParameterList *>(node)) { // postVisit
         SgFunctionParameterList *n = isSgFunctionParameterList(node);
     }
     //                 SgVariableDeclaration
     else if (dynamic_cast<SgVariableDeclaration *>(node)) { // postVisit
         SgVariableDeclaration *n = isSgVariableDeclaration(node);
     }
     //                 SgVariableDefinition
     //                 SgClinkageDeclarationStatement:
     //                     SgClinkageStartStatement
     //                     SgClinkageEndStatement
     //             SgEnumDeclaration
     else if (dynamic_cast<SgEnumDeclaration *>(node)) { // postVisit
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
     else if (dynamic_cast<SgTypedefDeclaration *>(node)) { // postVisit
         SgTypedefDeclaration *n = isSgTypedefDeclaration(node);
     }
     //             SgStatementFunctionStatement
     //             SgCtorInitializerList
     //             SgPragmaDeclaration
     else if (dynamic_cast<SgPragmaDeclaration *>(node)) { // postVisit
     }
     //             SgUsingDirectiveStatement
     //             SgClassDeclaration:
     else if (dynamic_cast<SgClassDeclaration *>(node)) { // postVisit
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
     else if (dynamic_cast<SgFunctionDeclaration *>(node)) { // postVisit
         SgFunctionDeclaration *n = isSgFunctionDeclaration(node);

         if (n -> get_definition() &&  // A function header with a definition that should not be ignored because ...
             ((! option.isQuery()) || n -> attributeExists(Control::LLVM_TRANSLATE)) &&
             (! n -> attributeExists(Control::LLVM_IGNORE))) {
             /**
              * If the function has more than one return statement at the end of its body, then we
              * don't need to create a separate "return" basic block here.
              */
             string return_label = current_function_decls -> getReturnLabel();
             string debug_md = attributes->addDebugMetadata(node, current_function_decls);
             if (current_function_decls -> numLabelPredecessors(return_label) > 0) {
                 codeOut -> emitUnconditionalBranch(return_label, debug_md);
                 codeOut -> emitLabel(current_function_decls, return_label);
             }

             /**
              *
              */
             if (n -> attributeExists(Control::LLVM_NEEDS_STACK)) {
                 string temp = attributes -> getTemp(LLVMAstAttributes::TEMP_INT);
                 (*codeOut) << CodeEmitter::indent() << temp << " = load i8*, i8** %.stacksave, align 8" << endl;
                 (*codeOut) << CodeEmitter::indent() << "call void @llvm.stackrestore(i8* " << temp << ")" << endl;
             }

             SgType *return_type = attributes -> getSourceType(n -> get_type() -> get_return_type());
             if (isSgTypeVoid(return_type)) {
                 (*codeOut) << CodeEmitter::indent() << "ret void" << debug_md << endl;
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
                         (*codeOut) << CodeEmitter::indent() << cast_name << " = bitcast " << type_name << "* " << "%.retval to " << cast_type_name << "*" << debug_md << endl;
                         (*codeOut) << CodeEmitter::indent() << cast_return_name << " = load " << cast_type_name << ", " << cast_type_name << "* " << cast_name << ", align 1" << debug_md << endl;
                         type_name = cast_type_name;
                         return_name = cast_return_name;
                     }
                     else {
                         type_name = "void";
                         return_name = "";
                     }
                 }
                 else {
                     (*codeOut) << CodeEmitter::indent() << return_name << " = load " << type_name << ", " << type_name << "* " << "%.retval" << debug_md << endl;
                 }
                 (*codeOut) << CodeEmitter::indent() << "ret " << type_name << " " << return_name << debug_md << endl;
             }
             (*codeOut) << "}" << endl;

             current_function_decls = NULL; // done with this function

             if (option.isSyntheticTranslation()) { // If we were processing a synthetic function, indicate that we're done with it.
                 option.resetSyntheticTranslation();
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
     else if (dynamic_cast<SgExprStatement *>(node)) { // postVisit
         SgExprStatement *n = isSgExprStatement(node);

         /**
          * The test expression in an IfStmt, WhileStmt, DoStatement and ForStatement,...and the item_selector in a SwitchStatement is wrapped in an SgExprStatement
          */
         if (n -> attributeExists(Control::LLVM_IF_LABELS)) {
             IfAstAttribute *attribute = (IfAstAttribute *) n -> getAttribute(Control::LLVM_IF_LABELS);
             string name = ((StringAstAttribute *) n -> get_expression() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getTrueLabel() << ", label %" << attribute -> getFalseLabel() << attributes->addDebugMetadata(node, current_function_decls) << endl;
         }
         else if (n -> attributeExists(Control::LLVM_WHILE_LABELS)) {
             WhileAstAttribute *attribute = (WhileAstAttribute *) n -> getAttribute(Control::LLVM_WHILE_LABELS);
             string name = ((StringAstAttribute *) n -> get_expression() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getBodyLabel() << ", label %" << attribute -> getEndLabel() << attributes->addDebugMetadata(node, current_function_decls) << endl;
             codeOut -> emitLabel(current_function_decls, attribute -> getBodyLabel());
         }
         else if (n -> attributeExists(Control::LLVM_DO_LABELS)) {
             DoAstAttribute *attribute = (DoAstAttribute *) n -> getAttribute(Control::LLVM_DO_LABELS);
             string name = ((StringAstAttribute *) n -> get_expression() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getBodyLabel() << ", label %" << attribute -> getEndLabel() << attributes->addDebugMetadata(node, current_function_decls) << endl;
             codeOut -> emitLabel(current_function_decls, attribute -> getEndLabel());
         }
         else if (n -> attributeExists(Control::LLVM_FOR_LABELS)) {
             ForAstAttribute *attribute = (ForAstAttribute *) n -> getAttribute(Control::LLVM_FOR_LABELS);
             if (! isSgNullExpression(n -> get_expression())) { // if a conditional expression was present
                 string name = ((StringAstAttribute *) n -> get_expression() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getBodyLabel() << ", label %" << attribute -> getEndLabel() << attributes->addDebugMetadata(node, current_function_decls) << endl;
             }
             else {
                 codeOut -> emitUnconditionalBranch(attribute -> getBodyLabel(), attributes->addDebugMetadata(node, current_function_decls));
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
             (*codeOut) << CodeEmitter::indent() << "]" << attributes->addDebugMetadata(node, current_function_decls) << endl;
         }
     }
     //             SgLabelStatement
     else if (dynamic_cast<SgLabelStatement *>(node)) { // postVisit
         SgLabelStatement *n = isSgLabelStatement(node);
     }
     //             SgCaseOptionStmt
     else if (dynamic_cast<SgCaseOptionStmt *>(node)) { // postVisit
         SgCaseOptionStmt *n = isSgCaseOptionStmt(node);
     }
     //             SgTryStmt
     //             SgDefaultOptionStmt
     else if (dynamic_cast<SgDefaultOptionStmt *>(node)) { // postVisit
         SgDefaultOptionStmt *n = isSgDefaultOptionStmt(node);
     }
     //             SgBreakStmt
     else if (dynamic_cast<SgBreakStmt *>(node)) { // postVisit
         SgBreakStmt *n = isSgBreakStmt(node);
         SgScopeStatement *scope = scopeStack.top();
         if (dynamic_cast<SgForStatement *>(scope)) {
             SgForStatement *for_stmt = isSgForStatement(scope);
             ForAstAttribute *attribute = (ForAstAttribute *) for_stmt -> getAttribute(Control::LLVM_FOR_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
         else if (dynamic_cast<SgWhileStmt *>(scope)) {
             SgWhileStmt *while_stmt = isSgWhileStmt(scope);
             WhileAstAttribute *attribute = (WhileAstAttribute *) while_stmt -> getAttribute(Control::LLVM_WHILE_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
         else if (dynamic_cast<SgDoWhileStmt *>(scope)) {
             SgDoWhileStmt *do_stmt = isSgDoWhileStmt(scope);
             DoAstAttribute *attribute = (DoAstAttribute *) do_stmt -> getAttribute(Control::LLVM_DO_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
         else if (dynamic_cast<SgSwitchStatement *>(scope)) {
             SgSwitchStatement *switch_stmt = isSgSwitchStatement(scope);
             SwitchAstAttribute *attribute = (SwitchAstAttribute *) switch_stmt -> getAttribute(Control::LLVM_SWITCH_INFO);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
     }
     //             SgContinueStmt
     else if (dynamic_cast<SgContinueStmt *>(node)) { // postVisit
         SgContinueStmt *n = isSgContinueStmt(node);
         SgScopeStatement *scope = scopeStack.top();
         if (dynamic_cast<SgForStatement *>(scope)) {
             SgForStatement *for_stmt = isSgForStatement(scope);
             ForAstAttribute *attribute = (ForAstAttribute *) for_stmt -> getAttribute(Control::LLVM_FOR_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getIncrementLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
         else if (dynamic_cast<SgWhileStmt *>(scope)) {
             SgWhileStmt *while_stmt = isSgWhileStmt(scope);
             WhileAstAttribute *attribute = (WhileAstAttribute *) while_stmt -> getAttribute(Control::LLVM_WHILE_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
         else if (dynamic_cast<SgDoWhileStmt *>(scope)) {
             SgDoWhileStmt *do_stmt = isSgDoWhileStmt(scope);
             DoAstAttribute *attribute = (DoAstAttribute *) do_stmt -> getAttribute(Control::LLVM_DO_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
     }
     //             SgReturnStmt
     else if (dynamic_cast<SgReturnStmt *>(node)) { // postVisit
         SgReturnStmt *n = isSgReturnStmt(node);
         SgExpression *expression = n -> get_expression();
         //
         // 08/20/2016 - For a return statement specified without an expression, Rose may still add a NULL return expression
         // of type SgTypeDefault. We now have to check for this condition also in order to avoid a crash.
         //
         if (expression && (! isSgTypeDefault(expression -> get_type()))) {
             SgType *return_type = current_function_decls -> getFunctionType() -> get_return_type();
             string return_type_name = ((StringAstAttribute *) return_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    return_name = ((StringAstAttribute *) expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();

             /**
              * For a function that returns a stucture, ...
              */
             string debug_md = attributes -> addDebugMetadata(node, current_function_decls);
             if (isSgClassType(attributes -> getSourceType(return_type))) {
                 string return_variable_name = (attributes -> integralStructureType(return_type) ? "%.retval" : "%agg.result"),
                        return_bit_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue();
                 int size = ((IntAstAttribute *) return_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << return_bit_name << " = bitcast " << return_type_name << "* " << return_variable_name <<" to i8*" << debug_md << endl;
//                 (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.i32(i8* " << return_bit_name << ", i8* " << return_name << ", i32 " << size << ", i32 4)" << debug_md << endl;
                 (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.p0i8.p0i8.i64(i8* " << return_bit_name << ", i8* " << return_name << ", i64 " << size << ", i32 4, i1 false)" << debug_md << endl;
             }
             else {
                 (*codeOut) << CodeEmitter::indent() << "store " << return_type_name << " " << return_name << ", " << return_type_name << "* %.retval" << debug_md << endl;
             }
         }

         /**
          * If the function only has one return statement and after executing the body of the function, control
          * falls directly into the return statement then there is no need to emit a branch.
          */
         string return_label = current_function_decls -> getReturnLabel();
         if (current_function_decls -> numLabelPredecessors(return_label) > 0) {
             codeOut -> emitUnconditionalBranch(return_label, attributes->addDebugMetadata(node, current_function_decls));
         }
     }
     //             SgGotoStatement
     else if (dynamic_cast<SgGotoStatement *>(node)) { // postVisit
         SgGotoStatement *n = isSgGotoStatement(node);
         SgLabelStatement *label_statement = (SgLabelStatement *) n -> get_label();

         /**
          * The casts were added below to avoid confusion. Note that an SgGotoStatement contains a get_label()
          * metho that returns an SgLabelStatement. An SgLabelStatement also contains a get_label() method that
          * returns an SgName (name of the actual label).
          */
         codeOut -> emitUnconditionalBranch(attributes -> findLabel(label_statement), attributes->addDebugMetadata(node, current_function_decls));
     }
     //             SgSpawnStmt
     //             SgNullStatement
     else if (dynamic_cast<SgNullStatement *>(node)) { // postVisit
         SgNullStatement *n = isSgNullStatement(node);
         // TODO: Do nothing for now!!!
     }
     //             SgVariantStatement
     //             SgForInitStatement
     else if (dynamic_cast<SgForInitStatement *>(node)) { // postVisit
         SgForInitStatement *n = isSgForInitStatement(node);
         ForAstAttribute *attribute = (ForAstAttribute *) n -> getAttribute(Control::LLVM_FOR_LABELS);
         codeOut -> emitUnconditionalBranch(attribute -> getConditionLabel(), attributes->addDebugMetadata(node, current_function_decls));
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
     //             SgCompoundLiteralExp:
     //             SgUnaryOp:
     //                 SgExpressionRoot
     //                 SgMinusOp
     else if (dynamic_cast<SgMinusOp *>(node)) { // postVisit
         SgMinusOp *n = isSgMinusOp(node);
         SgType *type = n -> get_type();
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                default_value = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << result_name << " = " << (attributes -> isFloatType(type) ? "f" : "") << "sub" << " "
                    <<  type_name << " " << default_value << ", " << operand_name << attributes->addDebugMetadata(node, current_function_decls) << endl;
     }
     //                 SgUnaryAddOp
     else if (dynamic_cast<SgUnaryAddOp *>(node)) { // postVisit
         SgUnaryAddOp *n = isSgUnaryAddOp(node);
         // No need to do anything here.
     }
     //                 SgNotOp
     else if (dynamic_cast<SgNotOp *>(node)) { // postVisit
         SgNotOp *n = isSgNotOp(node);
         /**
          * Since Rose transforms the NotOp operation into a NotEqual operation.  We simply need to 
          * flip the bit result and zero-extend the operand into the size of the resulting integral type.
          */
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << result_name << " = xor i1 " << operand_name << ", true" << attributes->addDebugMetadata(node, current_function_decls) << endl;
     }
     //                 SgPointerDerefExp
     else if (dynamic_cast<SgPointerDerefExp *>(node)) { // postVisit
         SgPointerDerefExp *n = isSgPointerDerefExp(node);
         if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
             string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                    operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                    result_type_name = ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             string debug_md = attributes->addDebugMetadata(node, current_function_decls);

             if (dynamic_cast<SgClassType *> (attributes -> getSourceType(n -> get_type()))) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << result_type_name << "* " << operand_name << " to i8*" << debug_md << endl;
             }
             else {
               string alignment = attributes->addVectorAlignment(node);
               string bundle_md = attributes->addBundleMetadata(node);
               if (isSgArrayType(n -> get_type())) {
                   (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds " << result_type_name << ", " << result_type_name << "* " << operand_name << ", i32 0, i32 0" << debug_md << endl;
               }
               else {
                   (*codeOut) << CodeEmitter::indent() << result_name << " = load " << result_type_name << ", " << result_type_name << "* " << operand_name << alignment << debug_md << bundle_md << endl;
               }
             }
         }
     }
     //                 SgAddressOfOp
     else if (dynamic_cast<SgAddressOfOp *>(node)) { // postVisit
         SgAddressOfOp *n = isSgAddressOfOp(node);
         // No need to do anything here.
     }
     //                 SgMinusMinusOp
     else if (dynamic_cast<SgMinusMinusOp *>(node)) { // postVisit
         SgMinusMinusOp *n = isSgMinusMinusOp(node);
         SgType *type = attributes -> getSourceType(n -> get_type());
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                ref_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue();
         string debug_md = attributes->addDebugMetadata(node, current_function_decls);
         if (isSgPointerType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_operand()))) ||
             isSgArrayType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_operand())))) {

             ROSE2LLVM_ASSERT(type_name.length() > 1 && type_name[type_name.length() - 1] == '*');
// TODO: Remove this !!!
/*
cout << "; The type_name is: " << type_name.substr(0, type_name.length() - 1)
     << "; the pointer is: " << type_name
     << endl;
cout.flush();
*/
             (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds " << type_name.substr(0, type_name.length() - 1) << ", " << type_name << " " <<  name << ", i32 -1" << debug_md << endl;
         }
         else {
             (*codeOut) << CodeEmitter::indent() << result_name << " = " << (attributes -> isFloatType(type) ? "fsub " : "sub ") << type_name << " " << name << ", "
                        << (isSgTypeLongDouble(type) ? Control::LongDoubleToString(1.0)
                                                     : isSgTypeDouble(type) ? Control::DoubleToString(1.0)
                                                                            : isSgTypeFloat(type) ? Control::FloatToString(1.0) : "1")
                        << debug_md << endl;
         }
         (*codeOut) << CodeEmitter::indent() << "store " << type_name << " " << result_name << ", " << type_name << "* " << ref_name << debug_md << endl;
     }
     //                 SgPlusPlusOp
     else if (dynamic_cast<SgPlusPlusOp *>(node)) { // postVisit
         SgPlusPlusOp *n = isSgPlusPlusOp(node);
         SgType *type = attributes -> getExpressionType(n);
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                ref_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue();
         string debug_md = attributes->addDebugMetadata(node, current_function_decls);
         if (isSgPointerType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_operand()))) ||
             isSgArrayType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_operand())))) {

             ROSE2LLVM_ASSERT(type_name.length() > 1 && type_name[type_name.length() - 1] == '*');
// TODO: Remove this !!!
/*
cout << "; The type_name is: " << type_name.substr(0, type_name.length() - 1)
     << "; the pointer is: " << type_name
     << endl;
cout.flush();
*/
             (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds " << type_name.substr(0, type_name.length() - 1) << ", " << type_name << " " <<  name << ", i32 1" << debug_md << endl;
         }
         else {
             (*codeOut) << CodeEmitter::indent() << result_name << " = " << (attributes -> isFloatType(type) ? "fadd " : "add ") << type_name << " " << name << ", "
                        << (isSgTypeLongDouble(type) ? Control::LongDoubleToString(1.0)
                                                     : isSgTypeDouble(type) ? Control::DoubleToString(1.0)
                                                                            : isSgTypeFloat(type) ? Control::FloatToString(1.0) : "1")
                        << debug_md << endl;
         }
         (*codeOut) << CodeEmitter::indent() << "store " << type_name << " " << result_name << ", " << type_name << "* " << ref_name << debug_md << endl;
     }
     //                 SgBitComplementOp
     else if (dynamic_cast<SgBitComplementOp *>(node)) { // postVisit
         SgBitComplementOp *n = isSgBitComplementOp(node);
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                operand_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_operand()) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         (*codeOut) << CodeEmitter::indent() << result_name << " = xor " << operand_type_name << " " << operand_name << ", -1" << attributes->addDebugMetadata(node, current_function_decls) << endl;
     }
     //                 SgCastExp
     else if (dynamic_cast<SgCastExp *>(node)) { // postVisit
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
          * C99 requires that a non-zero value be converted to 1 before
          * being converted to bool.  Thus, the bool will compare
          * correctly with true, which is 1.  The result is an i1, which
          * will be promoted if necessary in postVisitExit.
          */
         else if (isSgTypeBool(attributes -> getSourceType(n -> get_type()))) {
             if (! n -> get_operand() -> attributeExists(Control::LLVM_IS_BOOLEAN)) {
                 string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
                 string operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 SgType *operand_type = attributes -> getExpressionType(n -> get_operand());
                 string operand_type_name = ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 string zero_value = (operand_type -> isFloatType()
                                             ? (isSgTypeFloat(operand_type)
                                                    ? control.FloatToString(0)
                                                    : isSgTypeDouble(operand_type)
                                                          ? control.DoubleToString(0)
                                                          : control.LongDoubleToString(0))
                                             : isSgPointerType(operand_type)
                                                   ? "null"
                                                   : "0");
                 (*codeOut) << CodeEmitter::indent() << name << " = "
                            << (operand_type -> isFloatType() ? "fcmp o" : "icmp ") << "ne "
                            << operand_type_name << " " << operand_name << ", "
                            << zero_value
                            << attributes->addDebugMetadata(node, current_function_decls) << endl;
             }
         }
         /**
          * Trivial casts are processed during the Attribute visit... They are ignored here.
          */
         else if (! n -> attributeExists(Control::LLVM_TRIVIAL_CAST)) { // process only non-trivial casts
             SgExpression *operand = n -> get_operand();
             SgType *result_type = attributes -> getExpressionType(n);
             ROSE2LLVM_ASSERT(result_type);
             if (isSgTypeVoid(result_type)) {
                 // TODO: Do nothing for now!!!
             }
             else {
                 SgType *operand_type = attributes -> getExpressionType(operand);
// TODO: Remove this !!!
/*
cout << "Casting operand "
  << operand -> class_name()
  << " from type "
  << operand_type -> class_name()
  << " to type "
  << result_type -> class_name()
  << endl;
cout.flush();
*/
                 ROSE2LLVM_ASSERT(operand_type);
                 ROSE2LLVM_ASSERT(n -> getAttribute(Control::LLVM_NAME));
                 ROSE2LLVM_ASSERT(result_type -> getAttribute(Control::LLVM_TYPE));
                 ROSE2LLVM_ASSERT(operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
                 ROSE2LLVM_ASSERT(operand_type -> getAttribute(Control::LLVM_TYPE));

                 string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                        result_type_name = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                        operand_name = ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                        operand_type_name = ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

                 if (attributes -> isIntegerType(result_type)) {
                     string debug_md = attributes->addDebugMetadata(node, current_function_decls);
                     if (dynamic_cast<SgPointerType *> (operand_type)) {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = ptrtoint " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                     }
                     else if (dynamic_cast<SgFunctionType *> (operand_type)) {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = ptrtoint " << operand_type_name << "* " << operand_name << " to " << result_type_name << debug_md << endl;
                     }
                     else if (attributes -> isUnsignedType(result_type)) {
                         if (attributes -> isIntegerType(operand_type)) {
                             if (((IntAstAttribute *) operand_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() > 
                                 ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()) {
                                  (*codeOut) << CodeEmitter::indent() << result_name << " = trunc " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                             }
                             else {
                                 (*codeOut) << CodeEmitter::indent() << result_name << " = zext " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                             }
                         }
                         else if (attributes -> isFloatType(operand_type)) {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = fptoui " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                         }
                         else {
                              cerr << "can't convert yet from " << operand_type -> class_name() << " to " << result_type -> class_name() << endl;
                              cerr.flush();
                              ROSE2LLVM_ASSERT(0);
                         }
                     }
                     else {
                         if (attributes -> isIntegerType(operand_type)) {
                             if (((IntAstAttribute *) operand_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() > 
                                 ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()) {
                                  (*codeOut) << CodeEmitter::indent() << result_name << " = trunc " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                             }
                             else if (attributes -> isUnsignedType(operand_type)) {
                                  (*codeOut) << CodeEmitter::indent() << result_name << " = zext " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                             }
                             else {
                                 (*codeOut) << CodeEmitter::indent() << result_name << " = sext " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                             }
                         }
                         else if (attributes -> isFloatType(operand_type)) {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = fptosi " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                         }
                         else if (isSgTypeComplex(operand_type)) {
                             SgTypeComplex *complex_type = isSgTypeComplex(operand_type);
                             SgType *component_type = attributes -> getSourceType(complex_type -> get_base_type());
                             string component_type_name = ((StringAstAttribute *) component_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

                             (*codeOut) << CodeEmitter::indent() << result_name << " = fptosi " << component_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                         }
                         else {
                              cerr << "can't convert yet from " << operand_type -> class_name() << " to " << result_type -> class_name() << endl;
                              cerr.flush();
                              ROSE2LLVM_ASSERT(0);
                         }
                     }
                 }
                 else if (attributes -> isFloatType(result_type)) {
                     string debug_md = attributes->addDebugMetadata(node, current_function_decls);
                     if (attributes -> isIntegerType(operand_type)) {
                         if (attributes -> isUnsignedType(operand_type)) {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = uitofp " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                         }
                         else {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = sitofp " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                         }
                     }
                     else if (attributes -> isFloatType(operand_type)) {
                          if (((IntAstAttribute *) operand_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() > 
                              ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()) {
                               (*codeOut) << CodeEmitter::indent() << result_name << " = fptrunc " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                          }
                          else {
                              (*codeOut) << CodeEmitter::indent() << result_name << " = fpext " << operand_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                          }
                     }
                     else if (isSgTypeComplex(operand_type)) {
                         SgTypeComplex *complex_type = isSgTypeComplex(operand_type);
                         SgType *component_type = attributes -> getSourceType(complex_type -> get_base_type());
                         string component_type_name = ((StringAstAttribute *) component_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

                         if (attributes -> isTrivialCast(component_type, result_type)) {
                             StringAstAttribute *result_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                             result_attribute -> resetValue(operand_name);

                             result_attribute = (StringAstAttribute *) node -> getAttribute(Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME);
                             if (result_attribute) {
                                 result_attribute -> resetValue(operand_name);
                             }
                         }
                         else if (((IntAstAttribute *) component_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() > 
                                  ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()) {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = fptrunc " << component_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                         }
                         else {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = fpext " << component_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                         }
//                         (*codeOut) << CodeEmitter::indent() << result_name << " = fptosi " << component_type_name << " " << operand_name << " to " << result_type_name << debug_md << endl;
                     }
                     else {
                          cerr << "can't convert yet from " << operand_type -> class_name() << " to " << result_type -> class_name() << endl;
                          cerr.flush();
                          ROSE2LLVM_ASSERT(0);
                     }
                 }
                 else if (isSgTypeComplex(result_type)) {
//cout << "result_name = " << result_name
//     << "; result_type_name = " << result_type_name
//     << "; operand_name = " << operand_name
//     << "; operand_type_name = " << operand_type_name
//     << "; operand class = " << operand -> class_name()
//     << endl;
//cout.flush();
//  %3 = getelementptr inbounds { double, double }, { double, double }* %z, i32 0, i32 0
//   %4 = load double, double* %3, align 8
//  %5 = fptosi double %4 to i32
                     ROSE2LLVM_ASSERT(n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
                     ROSE2LLVM_ASSERT(n -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME));
                     SgTypeComplex *complex_type = isSgTypeComplex(result_type);
                     SgType *base_type = attributes -> getSourceType(complex_type -> get_base_type());
                     int alignment = ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                     StringAstAttribute *imaginary_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME);
                     string base_type_name = ((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                            real_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                            imaginary_name = imaginary_attribute -> getValue(),
                            null_imaginary_value = (isSgTypeFloat(base_type)
                                                        ? control.FloatToString((float) 0.0)
                                                        : isSgTypeDouble(base_type) ? control.DoubleToString((double) 0.0)
                                                                                    : control.LongDoubleToString((long double) 0.0));

                     //
                     // Assign the value of the operand to the real component.
                     //
                     if (attributes -> isIntegerType(operand_type)) {
                         if (attributes -> isUnsignedType(operand_type)) {
                             (*codeOut) << CodeEmitter::indent() << real_name << " = uitofp " << operand_type_name << " " << operand_name << " to " << base_type_name << endl;
                         }
                         else {
                             (*codeOut) << CodeEmitter::indent() << real_name << " = sitofp " << operand_type_name << " " << operand_name << " to " << base_type_name << endl;
                         }
                         imaginary_attribute -> resetValue(null_imaginary_value); // Reset the imaginary component to 0.0
                     }
                     else if (attributes -> isFloatType(operand_type)) {
                         if (attributes -> isTrivialCast(operand_type, base_type)) {
                             StringAstAttribute *result_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                             result_attribute -> resetValue(operand_name);

                             result_attribute = (StringAstAttribute *) node -> getAttribute(Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME);
                             if (result_attribute) {
                                 result_attribute -> resetValue(operand_name);
                             }
                         }
                         else if (((IntAstAttribute *) operand_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() > 
                                  ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()) {
                              (*codeOut) << CodeEmitter::indent() << real_name << " = fptrunc " << operand_type_name << " " << operand_name << " to " << base_type_name << endl;
                         }
                         else {
                              (*codeOut) << CodeEmitter::indent() << real_name << " = fpext " << operand_type_name << " " << operand_name << " to " << base_type_name << endl;
                         }
                         imaginary_attribute -> resetValue(null_imaginary_value); // Reset the imaginary component to 0.0
                     }
                     else if (isSgTypeComplex(operand_type)) {
/*
cout << "Casting a complex type with base type "
     << ((StringAstAttribute *) attributes -> getSourceType(isSgTypeComplex(operand_type) -> get_base_type()) -> getAttribute(Control::LLVM_TYPE)) -> getValue()
     << " to a complex type with base type "
     << base_type_name
     << endl
     << "Old operand_type_name is "
     << operand_type_name
     << endl;
cout.flush();
*/
                         SgType *operand_base_type = attributes -> getSourceType(isSgTypeComplex(operand_type) -> get_base_type());
                         operand_type_name = ((StringAstAttribute *) operand_base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
/*
cout << "New operand_type_name is "
     << operand_type_name
     << endl;
cout.flush();
*/
                         string operand_imaginary_name = ((StringAstAttribute *) operand -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME)) -> getValue();

                         if (((IntAstAttribute *) operand_base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() > 
                             ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()) {
                             (*codeOut) << CodeEmitter::indent() << real_name << " = fptrunc " << operand_type_name << " " << operand_name << " to " << base_type_name << endl;
                             (*codeOut) << CodeEmitter::indent() << imaginary_name << " = fptrunc " << operand_type_name << " " << operand_imaginary_name << " to " << base_type_name << endl;
                         }
                         else {
                             ROSE2LLVM_ASSERT(((IntAstAttribute *) operand_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() <
                                              ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue());
                             (*codeOut) << CodeEmitter::indent() << real_name << " = fpext " << operand_type_name << " " << operand_name << " to " << base_type_name << endl;
                             (*codeOut) << CodeEmitter::indent() << imaginary_name << " = fpext " << operand_type_name << " " << operand_imaginary_name << " to " << base_type_name << endl;
                         }
                     }
                     else ROSE2LLVM_ASSERT(! "yet know how to convert to a complex type with this precision");
                 }
                 else if (dynamic_cast<SgPointerType *> (result_type)) {
                     if (dynamic_cast<SgTypeString *> (operand_type))
                          ;  // already taken care of
                     else if (attributes -> isIntegerType(operand_type)) {
                          (*codeOut) << CodeEmitter::indent() << result_name << " = inttoptr " << operand_type_name << " " << operand_name << " to " << result_type_name << attributes->addDebugMetadata(node, current_function_decls) << endl;
                     }
                     else if (dynamic_cast<SgPointerType *> (operand_type)) {
                          (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << operand_type_name << " " << operand_name << " to " << result_type_name << attributes->addDebugMetadata(node, current_function_decls) << endl;
                     }
                     else if (dynamic_cast<SgArrayType *> (operand_type)) {
                          SgArrayType *array_type = isSgArrayType(operand_type);
                          //
                          // We need to replace the original array type by a pointer to its base type.
                          //
                          SgType *array_base_type = attributes -> getSourceType(array_type -> get_base_type());
                          operand_type_name = ((StringAstAttribute *) array_base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue() + "*";

                          (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << operand_type_name << " " << operand_name << " to " << result_type_name << attributes->addDebugMetadata(node, current_function_decls) << endl;
                     }
                     else if (dynamic_cast<SgFunctionType *> (operand_type)) {
                          (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << operand_type_name << "* " << operand_name << " to " << result_type_name << attributes->addDebugMetadata(node, current_function_decls) << endl;
                     }
                     else {
                          cerr << "can't convert yet from " << operand_type -> class_name() << " to " << result_type -> class_name() << endl;
                          cerr.flush();
                          ROSE2LLVM_ASSERT(0);
                     }
                 }
                 else {
                     cerr << "Funny conversion from type " << operand_type -> class_name() << " to type " << result_type -> class_name() << endl;
                     cerr.flush();
                     ROSE2LLVM_ASSERT(0);
                 }
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
     //                 SgDotExp
     else if (dynamic_cast<SgArrowExp *>(node) || dynamic_cast<SgDotExp *>(node)) { // postVisit
         SgBinaryOp *n = isSgBinaryOp(node);

         SgType *lhs_type = attributes -> getExpressionType(n -> get_lhs_operand());
         SgType *base_type = lhs_type;
         if (isSgArrowExp(n)) {
             base_type = isSgPointerType(attributes -> getSourceType(lhs_type)) -> get_base_type();
         }
         SgClassType *class_type = isSgClassType(attributes -> getSourceType(base_type));
         ROSE2LLVM_ASSERT(class_type);
         string reference_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue(),
                lhs_name = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                lhs_type_name = ((StringAstAttribute *) lhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                result_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
         if (isSgDotExp(n)) {
             lhs_type_name += "*";
         }
         SgClassDeclaration *decl= isSgClassDeclaration(class_type -> get_declaration());
         string debug_md = attributes->addDebugMetadata(node, current_function_decls);

         if (decl -> get_class_type() == SgClassDeclaration::e_union) {
             (*codeOut) << CodeEmitter::indent() << reference_name << " = bitcast " << lhs_type_name << " " << lhs_name << " to " << result_type_name << "*" <<  debug_md << endl;
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
             (*codeOut) << CodeEmitter::indent() << reference_name << " = getelementptr inbounds" << lhs_type_name.substr(0, lhs_type_name.length() - 1) << ", " << lhs_type_name << " " << lhs_name << ", i32 0, i32 " << index << debug_md << endl;
         }

         if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
             string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             if (dynamic_cast<SgClassType *> (attributes -> getSourceType(n -> get_type()))) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << result_type_name << "* " << reference_name << " to i8*" << debug_md << endl;
             }
             else if (n -> attributeExists(Control::LLVM_AGGREGATE)) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds " << result_type_name << ", " << result_type_name << "* " << reference_name << ", i32 0, i32 0" << debug_md << endl;
             }
             else {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = load " << result_type_name << ", " << result_type_name << "* " << reference_name << debug_md << endl;
             }
         }
     }
     //                 SgDotStarOp
     //                 SgArrowStarOp
     //                 SgEqualityOp
     else if (dynamic_cast<SgEqualityOp *>(node)) { // postVisit
         SgEqualityOp *n = isSgEqualityOp(node);
         genBinaryCompareOperation(n, "eq", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgLessThanOp
     else if (dynamic_cast<SgLessThanOp *>(node)) { // postVisit
         SgLessThanOp *n = isSgLessThanOp(node);
         genBinaryCompareOperation(n, "lt", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgGreaterThanOp
     else if (dynamic_cast<SgGreaterThanOp *>(node)) { // postVisit
         SgGreaterThanOp *n = isSgGreaterThanOp(node);
         genBinaryCompareOperation(n, "gt", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgNotEqualOp
     else if (dynamic_cast<SgNotEqualOp *>(node)) { // postVisit
         SgNotEqualOp *n = isSgNotEqualOp(node);
         genBinaryCompareOperation(n, "ne", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgLessOrEqualOp
     else if (dynamic_cast<SgLessOrEqualOp *>(node)) { // postVisit
         SgLessOrEqualOp *n = isSgLessOrEqualOp(node);
         genBinaryCompareOperation(n, "le", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgGreaterOrEqualOp
     else if (dynamic_cast<SgGreaterOrEqualOp *>(node)) { // postVisit
         SgGreaterOrEqualOp *n = isSgGreaterOrEqualOp(node);
         genBinaryCompareOperation(n, "ge", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgAddOp
     else if (dynamic_cast<SgAddOp *>(node)) { // postVisit
         SgAddOp *n = isSgAddOp(node);
         genAddOrSubtractOperation(n, "add", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgSubtractOp
     else if (dynamic_cast<SgSubtractOp *>(node)) { // postVisit
         SgSubtractOp *n = isSgSubtractOp(node);
         genAddOrSubtractOperation(n, "sub", attributes -> addDebugMetadata(node, current_function_decls));
     }
     //                 SgMultiplyOp
     else if (dynamic_cast<SgMultiplyOp *>(node)) { // postVisit
         SgMultiplyOp *n = isSgMultiplyOp(node);
         genBasicBinaryOperation(n, "mul", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgDivideOp
     else if (dynamic_cast<SgDivideOp *>(node)) { // postVisit
         SgDivideOp *n = isSgDivideOp(node);
         genDivideBinaryOperation(n, "div", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgIntegerDivideOp
     //                 SgModOp
     else if (dynamic_cast<SgModOp *>(node)) { // postVisit
         SgModOp *n = isSgModOp(node);
         genDivideBinaryOperation(n, "rem", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgAndOp
     else if (dynamic_cast<SgAndOp *>(node)) { // postVisit
         SgAndOp *n = isSgAndOp(node);
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                lhs_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                rhs_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         LogicalAstAttribute *lhs_attribute = (LogicalAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_LOGICAL_AND_LHS),
                             *rhs_attribute = (LogicalAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_LOGICAL_AND_RHS);
         codeOut -> emitLabel(current_function_decls, lhs_attribute -> getEndLabel());
         (*codeOut) << CodeEmitter::indent() << result_name << " = phi i1 [false, %" << lhs_attribute -> getLastLhsLabel() << "], "
                    << "[" << rhs_name << ", %" << rhs_attribute -> getLastRhsLabel() << "]" << attributes->addDebugMetadata(node, current_function_decls) << endl;
     }
     //                 SgOrOp
     else if (dynamic_cast<SgOrOp *>(node)) { // postVisit
         SgOrOp *n = isSgOrOp(node);
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                rhs_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         LogicalAstAttribute *lhs_attribute = (LogicalAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_LOGICAL_OR_LHS),
                             *rhs_attribute = (LogicalAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_LOGICAL_OR_RHS);
         codeOut -> emitLabel(current_function_decls, lhs_attribute -> getEndLabel());
         (*codeOut) << CodeEmitter::indent() << result_name << " = phi i1 [true, %" << lhs_attribute -> getLastLhsLabel() << "], "
                    << "[" << rhs_name << ", %" << rhs_attribute -> getLastRhsLabel() << "]" << attributes->addDebugMetadata(node, current_function_decls) << endl;
     }
     //                 SgBitXorOp
     else if (dynamic_cast<SgBitXorOp *>(node)) { // postVisit
         SgBitXorOp *n = isSgBitXorOp(node);
         genBasicBinaryOperation(n, "xor", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgBitAndOp
     else if (dynamic_cast<SgBitAndOp *>(node)) { // postVisit
         SgBitAndOp *n = isSgBitAndOp(node);
         genBasicBinaryOperation(n, "and", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgBitOrOp
     else if (dynamic_cast<SgBitOrOp *>(node)) { // postVisit
         SgBitOrOp *n = isSgBitOrOp(node);
         genBasicBinaryOperation(n, "or", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgCommaOpExp
     else if (dynamic_cast<SgCommaOpExp *>(node)) { // postVisit
         SgCommaOpExp *n = isSgCommaOpExp(node);
     }
     //                 SgLshiftOp
     else if (dynamic_cast<SgLshiftOp *>(node)) { // postVisit
         SgLshiftOp *n = isSgLshiftOp(node);
         genBasicBinaryOperation(n, "shl", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgRshiftOp
     else if (dynamic_cast<SgRshiftOp *>(node)) { // postVisit
         SgRshiftOp *n = isSgRshiftOp(node);
         genBasicBinaryOperation(n, (attributes -> isUnsignedType(attributes -> getExpressionType(n -> get_lhs_operand())) ? "lshr" : "ashr"), attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgPntrArrRefExp
     else if (dynamic_cast<SgPntrArrRefExp *>(node)) { // postVisit
         SgPntrArrRefExp *n = isSgPntrArrRefExp(node);
         string bundle_md = attributes->addBundleMetadata(node);
         string alignment = attributes->addVectorAlignment(node);

         SgExpression *array_pointer,
                      *array_index;
         if (attributes -> getExpressionType(n -> get_rhs_operand()) -> isIntegerType()) {
             array_pointer = n -> get_lhs_operand();
             array_index = n -> get_rhs_operand();
         }
         else {
             array_pointer = n -> get_rhs_operand();
             array_index = n -> get_lhs_operand();
         }

// TODO: Remove this !!!
/*
cout
  << "The array pointer is a "
  << array_pointer -> class_name()
  << " with type "
  << attributes -> getExpressionType(array_pointer) -> class_name()
  << ";  the array index is a "
  << array_index -> class_name()
  << " with type "
  << attributes -> getExpressionType(array_index) -> class_name()
  << endl;
cout.flush();
*/
         ROSE2LLVM_ASSERT(array_pointer -> getAttribute(Control::LLVM_NAME));
         ROSE2LLVM_ASSERT(array_pointer -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
         ROSE2LLVM_ASSERT(array_index -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
         ROSE2LLVM_ASSERT(attributes -> getExpressionType(array_index));
 
         string result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                array_name = ((StringAstAttribute *) array_pointer -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                index_name = ((StringAstAttribute *) array_index -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                reference_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue(),
                index_type_name = ((StringAstAttribute *) attributes -> getExpressionType(array_index) -> getAttribute(Control::LLVM_TYPE)) -> getValue();

         /**
          *
          */
         if (array_pointer -> attributeExists(Control::LLVM_ARRAY_NAME_REFERENCE)) {
             ROSE2LLVM_ASSERT(isSgVarRefExp(array_pointer) || isSgStringVal(array_pointer));
             ROSE2LLVM_ASSERT(attributes -> getExpressionType(array_pointer));

             SgType *source_type;
             /**
              * TODO: Rose does not convert a parameter array specified with [] into a pointer.
              */
             if (isSgVarRefExp(array_pointer)) {
                 SgVariableSymbol *var_sym = isSgVarRefExp(array_pointer) -> get_symbol();
                 ROSE2LLVM_ASSERT(var_sym);
                 SgInitializedName *var = isSgInitializedName(var_sym -> get_declaration());
                 ROSE2LLVM_ASSERT(var);
                 source_type = ((SgTypeAstAttribute *) var -> getAttribute(Control::LLVM_DECLARATION_TYPE)) -> getType();
                 ROSE2LLVM_ASSERT(source_type);
             }
             else {
                 source_type = attributes -> getExpressionType(array_pointer);
             }

             SgPointerType *pointer_type = isSgPointerType(source_type);
             SgArrayType *array_type = isSgArrayType(source_type);
             ROSE2LLVM_ASSERT(array_type || pointer_type);

             string debug_md = attributes -> addDebugMetadata(node, current_function_decls);
     
             if (pointer_type) { // var -> attributeExists(Control::LLVM_PARAMETER)) {
                 string pointer_type_name = ((StringAstAttribute *) pointer_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 SgType *element_type = pointer_type -> get_base_type();
                 string element_type_name = ((StringAstAttribute *) element_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 string parameter_name = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);
                 int alignment = ((IntAstAttribute *) pointer_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();

                 (*codeOut) << CodeEmitter::indent() << parameter_name << " = load " << pointer_type_name << ", " << pointer_type_name << "* " << array_name << ", align " << alignment << endl;
                 (*codeOut) << CodeEmitter::indent() << reference_name << " = getelementptr inbounds " << element_type_name << ", "
                                                     << pointer_type_name << " " << parameter_name << ", i32 " << index_name << endl;
                 if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
                     if (dynamic_cast<SgClassType *> (element_type)) {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << element_type_name << "* " << reference_name << " to i8*" << debug_md << endl;
                     }
                     else {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = load " << element_type_name << ", " << pointer_type_name << " " << reference_name << ", align "
                                                             << ((IntAstAttribute *) pointer_type -> get_base_type() -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()
                                                             << endl;
                     }
                 }
             }
             else {
/**     
                 AggregateAstAttribute *aggregate_attribute = (AggregateAstAttribute *) var -> getAttribute(Control::LLVM_AGGREGATE);
                 SgArrayType *array_type = isSgArrayType(attributes -> getSourceType(array_pointer-> get_type())); // Get the original array type specified by the user.
                 ROSE2LLVM_ASSERT(array_type);
                 string array_type_name = (aggregate_attribute -> getAggregate()
                                                    ? ((StringAstAttribute *) aggregate_attribute -> getAggregate() -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                                                    : ((StringAstAttribute *) array_type -> getAttribute(Control::LLVM_TYPE)) -> getValue());
**/
                 string array_type_name = ((StringAstAttribute *) array_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 SgType *element_type = array_type -> get_base_type();
                 string element_type_name = ((StringAstAttribute *) element_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

// TODO: Remove this !
/*  
cout
  << "*** Processing array dereference with original type name "
  << ((StringAstAttribute *) array_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
  << " and aggregate_type_name "
  << array_type_name
  << "; The AST resulting type is "
  << ((StringAstAttribute *) attributes -> getExpressionType(n) -> getAttribute(Control::LLVM_TYPE)) -> getValue()
  << "; The element_type name is "
  << element_type_name
  << endl;
cout.flush();
*/
                 (*codeOut) << CodeEmitter::indent() << reference_name << " = getelementptr inbounds " << array_type_name << ", " << array_type_name << "* " << array_name
                            << ", " << index_type_name << " 0, " << index_type_name << " " << index_name << endl;
                 if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
                     if (isSgArrayType(element_type)) {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds " << element_type_name << ", " << element_type_name << "* " << reference_name
                                                             << ", i32 0, i32 0" << endl;
                     }
                     else {
                         if (dynamic_cast<SgClassType *> (element_type)) {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << element_type_name << "* " << reference_name << " to i8*" << debug_md << endl;
                         }
                         else {
                             (*codeOut) << CodeEmitter::indent() << result_name << " = load " << element_type_name << ", " << element_type_name << "* " << reference_name << ", align "
                                                                 << ((IntAstAttribute *) array_type -> get_base_type() -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()
                                                                 << endl;
                         }
                     }
                 }
             }
         }
         else {
             /**
              * Say something!
              */
/*   
             AggregateAstAttribute *aggregate_attribute = (AggregateAstAttribute *) n -> getAttribute(Control::LLVM_AGGREGATE);
cout
  << (aggregate_attribute ? "There is an aggregate attribute here ==> " : "No aggregate attribute!")
  << (aggregate_attribute && aggregate_attribute -> getAggregate() ? ((StringAstAttribute *) aggregate_attribute -> getAggregate() -> getAttribute(Control::LLVM_TYPE)) -> getValue() : "")
  << endl;
cout.flush();
*/
             string result_type_name = ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
/*
             string aggregate_type_name = (aggregate_attribute
                                                    ? (aggregate_attribute -> getAggregate()
                                                                ? ((StringAstAttribute *) aggregate_attribute -> getAggregate() -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                                                                : (aggregate_attribute -> getArrayType()
                                                                      ? ((StringAstAttribute *) aggregate_attribute -> getArrayType() -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                                                                      : ((StringAstAttribute *) aggregate_attribute -> getClassType() -> getAttribute(Control::LLVM_TYPE)) -> getValue()))
                                                    : result_type_name);
*/
            SgType *type = attributes -> getExpressionType(array_pointer);
            SgPointerType *pointer_type = isSgPointerType(type);
            SgArrayType *array_type = isSgArrayType(type);
            ROSE2LLVM_ASSERT(array_type || pointer_type);
            string aggregate_type_name = (pointer_type
                                                 ? ((StringAstAttribute *) pointer_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
                                                 : ((StringAstAttribute *) array_type -> getAttribute(Control::LLVM_TYPE)) -> getValue());
     
// TODO: Remove this !
/*  
cout
  << "The resulting type is "
  << result_type_name
  << "; The aggregate type is "
  << aggregate_type_name
  << "; The AST resulting type is "
  << ((StringAstAttribute *) attributes -> getExpressionType(n) -> getAttribute(Control::LLVM_TYPE)) -> getValue()
  << "; The reference name is "
  << reference_name
  << endl;
cout.flush();
*/
             /**
              * Say something !
              */
             string debug_md = attributes->addDebugMetadata(node, current_function_decls);
             if (pointer_type) {
                 string element_type_name = ((StringAstAttribute *) pointer_type -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 string parameter_name = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);
                 int alignment = ((IntAstAttribute *) pointer_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << parameter_name << " = load " << aggregate_type_name << ", " << aggregate_type_name << "* " << array_name << ", align " << alignment << endl;
                 (*codeOut) << CodeEmitter::indent() << reference_name << " = getelementptr inbounds " << element_type_name << ", " << aggregate_type_name << " " << parameter_name << ", "
                                                     << index_type_name << " " << index_name << endl;
             }
             else if (isSgStringVal(array_pointer)) {
                 IntAstAttribute *string_index_attribute = (IntAstAttribute *) array_pointer -> getAttribute(Control::LLVM_STRING_INDEX);
                 (*codeOut) << CodeEmitter::indent() << reference_name << " = " << attributes -> getGlobalStringReference(string_index_attribute -> getValue(), index_name);
             }
             else {
                 (*codeOut) << CodeEmitter::indent() << reference_name << " = getelementptr inbounds " << aggregate_type_name << ", " << aggregate_type_name << "* " << array_name << ", "
                                                     << index_type_name << " 0, " << index_type_name << " " << index_name << debug_md << endl;
             }

             /**
              * Say something !
              */
             if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
// TODO: Remove this !!!
/*
cout
  << "The type of this "
  << n -> class_name()
  << " is "
  << n -> get_type() -> class_name()
  << "; the expression type of n is "
  << attributes -> getExpressionType(n) -> class_name()
  << "; the aggregate type name is "
  << aggregate_type_name
  << "; the array_name is "
  << array_name
  << "; the reference_name is "
  << reference_name
  << endl;
cout.flush();
*/
                 SgType *result_type = attributes -> getExpressionType(n);
                 if ( /* n -> attributeExists(Control::LLVM_AGGREGATE) && */ isSgArrayType(result_type)) { // TODO: just added array test!  Not too sure of myself here!
                     (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds "
                                                         << aggregate_type_name << ", " << aggregate_type_name << "* " << reference_name << ", i32 0, i32 0" << debug_md << endl;
                 }
                 else {
                     if (dynamic_cast<SgClassType *> (result_type)) {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << result_type_name << "* " << reference_name << " to i8*" << debug_md << endl;
                     }
                     else {
                         (*codeOut) << CodeEmitter::indent() << result_name << " = load " << result_type_name << ", "
                                                             << result_type_name << "* " << reference_name << alignment << debug_md << bundle_md << endl;
                     }
                 }
             }
         }
     }
     //                 SgScopeOp
     //                 SgAssignOp
     else if (dynamic_cast<SgAssignOp *>(node)) { // postVisit
         SgAssignOp *n = isSgAssignOp(node);
         SgType *result_type = attributes -> getExpressionType(n);
         /**
          * ROSE BUG: 12/10/2018 - Rose does not always resolve the ressulting type of an assignment properly.
          *           This is particularly the case for an assignment to an array element specified with the index
          *           to the left of the array operator. E.g.  2[str] = 'a';
          */
         SgType *lhs_type = attributes -> getExpressionType(n -> get_lhs_operand()),
                *rhs_type = attributes -> getExpressionType(n -> get_rhs_operand());
         if (attributes -> isTrivialCast(lhs_type, rhs_type) && (! attributes -> isTrivialCast(result_type, rhs_type))) {
// TODO: Remove this !
/*  
cout
  << "*** ASSIGNMENT conflict found between rhs type "
  << ((StringAstAttribute *) rhs_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
  <<  " and result type "
  << ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
  << endl;
cout.flush();
*/
             SgTypeAstAttribute *result_type_attribute = (SgTypeAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
             ROSE2LLVM_ASSERT(result_type_attribute);
             result_type_attribute -> resetType(lhs_type);
             result_type = lhs_type;
        }

         string result_type_name = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                rhs_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         string debug_md = attributes->addDebugMetadata(node, current_function_decls);

 
         if (isSgClassType(result_type)) {
             string lhs_name = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             int size = ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.p0i8.p0i8.i64(i8* " << lhs_name << ", i8* " << rhs_name << ", i64 " << size << ", i32 4, i1 false)" << debug_md << endl;
         }
         else {
             string ref_name = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue();

             if (dynamic_cast<SgFunctionRefExp *>(n -> get_rhs_operand())) {
                 SgFunctionRefExp *function = isSgFunctionRefExp(n -> get_rhs_operand());
                 string function_name = ((StringAstAttribute *) function -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                        function_type_name = ((StringAstAttribute *) attributes -> getExpressionType(function) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 (*codeOut) << CodeEmitter::indent() << "store " << result_type_name
                            << " bitcast (" << function_type_name << "* " << function_name << " to " << result_type_name << "), "
                            << result_type_name << "* " << ref_name << debug_md << endl;
             }
             else {
                 string alignment = attributes->addVectorAlignment(n->get_lhs_operand());
                 string bundle_md = attributes->addBundleMetadata(n->get_lhs_operand());
                 if (isSgTypeComplex(result_type)) {
                     ROSE2LLVM_ASSERT(n -> get_rhs_operand() -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME));
                     SgType *base_type = attributes -> getSourceType(isSgTypeComplex(result_type) -> get_base_type());
                     int alignment = ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                     string base_type_name = ((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                            rhs_real_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                            rhs_imaginary_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME)) -> getValue(),
                            lhs_real = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_REAL)) -> getValue(),
                            lhs_imaginary = ((StringAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_IMAGINARY)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << "store " << base_type_name << " " << rhs_real_name << ", " << base_type_name << "* " << lhs_real << ", align " << alignment << endl;
                     (*codeOut) << CodeEmitter::indent() << "store " << base_type_name << " " << rhs_imaginary_name << ", " << base_type_name << "* " << lhs_imaginary << ", align " << alignment << endl;
                 }
                 else {
                     (*codeOut) << CodeEmitter::indent() << "store " << result_type_name << " " << rhs_name << ", " << result_type_name << "* " << ref_name << alignment << debug_md << bundle_md << endl;
                 }
             }
         }
     }
     //                 SgPlusAssignOp
     else if (dynamic_cast<SgPlusAssignOp *>(node)) { // postVisit
         SgPlusAssignOp *n = isSgPlusAssignOp(node);
         genAddOrSubtractOperationAndAssign(n, "add", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgMinusAssignOp
     else if (dynamic_cast<SgMinusAssignOp *>(node)) { // postVisit
         SgMinusAssignOp *n = isSgMinusAssignOp(node);
         genAddOrSubtractOperationAndAssign(n, "sub", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgAndAssignOp
     else if (dynamic_cast<SgAndAssignOp *>(node)) { // postVisit
         SgAndAssignOp *n = isSgAndAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "and", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgIorAssignOp
     else if (dynamic_cast<SgIorAssignOp *>(node)) { // postVisit
         SgIorAssignOp *n = isSgIorAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "or", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgMultAssignOp
     else if (dynamic_cast<SgMultAssignOp *>(node)) { // postVisit
         SgMultAssignOp *n = isSgMultAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "mul", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgDivAssignOp
     else if (dynamic_cast<SgDivAssignOp *>(node)) { // postVisit
         SgDivAssignOp *n = isSgDivAssignOp(node);
         genDivideBinaryOperationAndAssign(n, "div", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgModAssignOp
     else if (dynamic_cast<SgModAssignOp *>(node)) { // postVisit
         SgModAssignOp *n = isSgModAssignOp(node);
         genDivideBinaryOperationAndAssign(n, "rem", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgXorAssignOp
     else if (dynamic_cast<SgXorAssignOp *>(node)) { // postVisit
         SgXorAssignOp *n = isSgXorAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "xor", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgLshiftAssignOp
     else if (dynamic_cast<SgLshiftAssignOp *>(node)) { // postVisit
         SgLshiftAssignOp *n = isSgLshiftAssignOp(node);
         genBasicBinaryOperationAndAssign(n, "shl", attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgRshiftAssignOp
     else if (dynamic_cast<SgRshiftAssignOp *>(node)) { // postVisit
         SgRshiftAssignOp *n = isSgRshiftAssignOp(node);
         genBasicBinaryOperationAndAssign(n, (attributes -> isUnsignedType(attributes -> getExpressionType(n -> get_lhs_operand())) ? "lshr" : "ashr"), attributes->addDebugMetadata(node, current_function_decls));
     }
     //                 SgExponentiationOp
     //                 SgConcatenationOp
     //                 SgPointerAssignOp
     //                 SgUserDefinedBinaryOp
     //             SgExprListExp
     else if (dynamic_cast<SgExprListExp *>(node)) { // postVisit
         SgExprListExp *n = isSgExprListExp(node);
     }
     //             SgVarRefExp
     else if (dynamic_cast<SgVarRefExp *>(node)) { // postVisit
         SgVarRefExp *n = isSgVarRefExp(node);
         if (n -> attributeExists(Control::LLVM_CLASS_MEMBER) || n -> attributeExists(Control::LLVM_ARRAY_NAME_REFERENCE)) {
             /**
              * Ignore class member variable references as they will be processed at SgDotExp or SgArrowExp level.
              * Also, ignore variables that are used in array element dereferences. They will be processed at the SgPntrArrExp level.
              */
         }
         else {
             SgVariableSymbol *sym = n -> get_symbol();
             ROSE2LLVM_ASSERT(sym);
             SgInitializedName *decl = sym -> get_declaration();
             ROSE2LLVM_ASSERT(decl);
             SgType *decl_type = attributes -> getSourceType(decl -> get_type());
             string var_name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_NAME)) -> getValue(),
                    type_name =  ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             string debug_md = attributes->addDebugMetadata(node, current_function_decls);
             string alignment = attributes->addVectorAlignment(node);
             SgType *result_type = attributes -> getExpressionType(n);

// TODO: Remove this !
/*  
cout
  << " Processing variable "
  << var_name
  << " with variable type "
  << type_name
  << " and result type "
  << ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue()
  << endl;
cout.flush();
*/
             /**
              * If we are dealing with an array aggregate, we need to load its address.
              */
             if (decl -> attributeExists(Control::LLVM_AGGREGATE) && isSgArrayType(decl_type)) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = getelementptr inbounds " << type_name << ", " << type_name << "* " << var_name << ", i32 0";
                 if (! isSgAddressOfOp(n -> get_parent())) {
                     (*codeOut) << ", i32 0";
                 }
                 (*codeOut) << debug_md << endl;
             }
             else if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
                 if (dynamic_cast<SgClassType *> (result_type)) {
                     (*codeOut) << CodeEmitter::indent() << result_name << " = bitcast " << type_name << "* " << var_name << " to i8*" << debug_md << endl;
                 }
                 else if (isSgTypeComplex(result_type)) {
                     SgType *base_type = attributes -> getSourceType(isSgTypeComplex(result_type) -> get_base_type());
                     int alignment = ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                     string real_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_REAL)) -> getValue(),
                            imaginary_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_IMAGINARY)) -> getValue(),
                            imaginary_result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME)) -> getValue(),
                            base_type_name = ((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
/*
string reference_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue();
cout << "Encountered value of complex " << reference_name
     << " with name "
     << var_name
     << ", Real name "
     << real_name
     << " and Imaginary name "
     << imaginary_name
     << " with type " << type_name
     << " and real result_name " << result_name
     << " and imaginary result_name " << imaginary_result_name
  << endl;
*/
                     (*codeOut) << CodeEmitter::indent() << real_name << " = getelementptr inbounds " << type_name << ", " << type_name << "* " << var_name << ", i32 0, i32 0" << endl;
                     (*codeOut) << CodeEmitter::indent() << result_name << " = load " << base_type_name << ", " << base_type_name << "* " << real_name << ", align " << alignment << endl;
                     (*codeOut) << CodeEmitter::indent() << imaginary_name << " = getelementptr inbounds " << type_name << ", " << type_name << "* " << var_name << ", i32 0, i32 1" << endl;
                     (*codeOut) << CodeEmitter::indent() << imaginary_result_name << " = load " << base_type_name << ", " << base_type_name << "* " << imaginary_name <<  ", align " << alignment << endl;
                 }
                 else {
                     (*codeOut) << CodeEmitter::indent() << result_name << " = load " << type_name << ", " << type_name << "* " << var_name << alignment << debug_md << endl;
                 }
             }
             else if (isSgTypeComplex(result_type)) {
       // SgType *base_type = attributes -> getSourceType(isSgTypeComplex(result_type) -> get_base_type());
       // int alignment = ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                     string real_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_REAL)) -> getValue(),
                            imaginary_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_IMAGINARY)) -> getValue(),
                            imaginary_result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME)) -> getValue();
      //  string base_type_name = ((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
/*
string reference_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_REFERENCE_NAME)) -> getValue();
cout << "Encountered value of complex " << reference_name
     << " with name "
     << var_name
     << ", Real name "
     << real_name
     << " and Imaginary name "
     << imaginary_name
     << " with type " << type_name
     << " and real result_name " << result_name
     << " and imaginary result_name " << imaginary_result_name
  << endl;
*/
                     (*codeOut) << CodeEmitter::indent() << real_name << " = getelementptr inbounds " << type_name << ", " << type_name << "* " << var_name << ", i32 0, i32 0" << endl;
//                     (*codeOut) << CodeEmitter::indent() << result_name << " = load " << base_type_name << ", " << base_type_name << "* " << real_name << ", align " << alignment << endl;
                     (*codeOut) << CodeEmitter::indent() << imaginary_name << " = getelementptr inbounds " << type_name << ", " << type_name << "* " << var_name << ", i32 0, i32 1" << endl;
//                     (*codeOut) << CodeEmitter::indent() << imaginary_result_name << " = load " << base_type_name << ", " << base_type_name << "* " << imaginary_name <<  ", align " << alignment << endl;
             }
         }
     }
     //             SgClassNameRefExp
     //             SgFunctionRefExp
     else if (dynamic_cast<SgFunctionRefExp *>(node)) { // postVisit
         SgFunctionRefExp *n = isSgFunctionRefExp(node);
     }
     //             SgMemberFunctionRefExp
     //             SgValueExp:
     //                 SgBoolValExp
     else if (dynamic_cast<SgBoolValExp *>(node)) { // postVisit
         SgBoolValExp *b = isSgBoolValExp(node);
     }
     //                 SgStringVal
     else if (dynamic_cast<SgStringVal*>(node)) { // postVisit
         SgStringVal *sval = isSgStringVal(node);
     }
     //                 SgShortVal
     else if (dynamic_cast<SgShortVal *>(node)) { // postVisit
         SgShortVal *n = isSgShortVal(node);
     }
     //                 SgCharVal
     else if (dynamic_cast<SgCharVal*>(node)) { // postVisit
         SgCharVal *cval = isSgCharVal(node);
     }
     //                 SgUnsignedCharVal
     else if (dynamic_cast<SgUnsignedCharVal *>(node)) { // postVisit
         SgUnsignedCharVal *n = isSgUnsignedCharVal(node);
     }
     //                 SgWcharVal
     //                 SgUnsignedShortVal
     else if (dynamic_cast<SgUnsignedShortVal *>(node)) { // postVisit
         SgUnsignedShortVal *n = isSgUnsignedShortVal(node);
     }
     //                 SgIntVal
     else if (dynamic_cast<SgIntVal*>(node)) { // postVisit
         SgIntVal *ival = isSgIntVal(node);
     }
     //                 SgEnumVal
     else if (dynamic_cast<SgEnumVal*>(node)) { // postVisit
         SgEnumVal *ival = isSgEnumVal(node);
     }
     //                 SgUnsignedIntVal
     else if (dynamic_cast<SgUnsignedIntVal *>(node)) { // postVisit
         SgUnsignedIntVal *n = isSgUnsignedIntVal(node);
     }
     //                 SgLongIntVal
     else if (dynamic_cast<SgLongIntVal *>(node)) { // postVisit
         SgLongIntVal *n = isSgLongIntVal(node);
     }
     //                 SgLongLongIntVal
     else if (dynamic_cast<SgLongLongIntVal *>(node)) { // postVisit
         SgLongLongIntVal *n = isSgLongLongIntVal(node);
     }
     //                 SgUnsignedLongLongIntVal 
     else if (dynamic_cast<SgUnsignedLongLongIntVal *>(node)) { // postVisit
         SgUnsignedLongLongIntVal *n = isSgUnsignedLongLongIntVal(node);
     }
     //                 SgUnsignedLongVal
     else if (dynamic_cast<SgUnsignedLongVal *>(node)) { // postVisit
         SgUnsignedLongVal *n = isSgUnsignedLongVal(node);
     }
     //                 SgFloatVal
     else if (dynamic_cast<SgFloatVal*>(node)) { // postVisit
         SgFloatVal *n = isSgFloatVal(node);
     }
     //                 SgDoubleVal
     else if (dynamic_cast<SgDoubleVal*>(node)) { // postVisit
         SgDoubleVal *n = isSgDoubleVal(node);
     }
     //                 SgLongDoubleVal
     else if (dynamic_cast<SgLongDoubleVal *>(node)) { // postVisit
         SgLongDoubleVal *n = isSgLongDoubleVal(node);
     }
     //                 SgComplexVal
     else if (dynamic_cast<SgComplexVal*>(node)) { // postVisit
         SgComplexVal *n = isSgComplexVal(node);
     }
     //                 SgUpcThreads
     //                 SgUpcMythread
     //                 SgFunctionCallExp
     else if (dynamic_cast<SgFunctionCallExp *>(node)) { // postVisit
         SgFunctionCallExp *n = isSgFunctionCallExp(node);
         string debug_md = attributes->addDebugMetadata(node, current_function_decls);

         SgExpression *expr;
         if (expr = Control::isComplexMacro(n, "creal")) {
/*
cout << "Found a creal call!  Expression is a "
     << expr -> class_name()
     << " with type "
     << attributes -> getExpressionType(expr) -> class_name()
     << "; the resulting type is "
     << attributes -> getExpressionType(n) -> class_name()
     << endl;
cout.flush();
*/ 
             ROSE2LLVM_ASSERT(expr -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
             ROSE2LLVM_ASSERT(expr -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME));

             string  real_result_name = ((StringAstAttribute *) expr -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                     imaginary_result_name = ((StringAstAttribute *) expr -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME)) -> getValue();
             StringAstAttribute *name_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME);
             StringAstAttribute *result_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
             StringAstAttribute *argument_result_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME);

/*
cout << "real_name = "
     << real_name
     << "; imaginary_name = "
     << imaginary_name
     << "; real_result_name = "
     << real_result_name
     << "; imaginary_name = "
     << imaginary_result_name
     << "; name = "
     << name_attribute -> getValue()
     << "; result = "
     << result_attribute -> getValue()
     << (argument_result_attribute ? "; argument_result = " : "")
     << (argument_result_attribute ? argument_result_attribute -> getValue() : "")
     << endl;
cout.flush();
*/
             name_attribute -> resetValue(real_result_name);
             result_attribute -> resetValue(real_result_name);
             if (argument_result_attribute) {
                 argument_result_attribute -> resetValue(real_result_name);
             }
         }
         else if (expr = Control::isComplexMacro(n, "cimag")) {
/*
cout << "Found a cimag call!  Expression is a "
     << expr -> class_name()
     << " with type "
     << attributes -> getExpressionType(expr) -> class_name()
     << "; the resulting type is "
     << attributes -> getExpressionType(n) -> class_name()
     << endl;
cout.flush();
*/
             ROSE2LLVM_ASSERT(expr -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
             ROSE2LLVM_ASSERT(expr -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME));

             string real_result_name = ((StringAstAttribute *) expr -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                    imaginary_result_name = ((StringAstAttribute *) expr -> getAttribute(Control::LLVM_IMAGINARY_RESULT_NAME)) -> getValue();
             StringAstAttribute *name_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME);
             StringAstAttribute *result_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
             StringAstAttribute *argument_result_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME);
/*
cout << "real_name = "
     << real_name
     << "; imaginary_name = "
     << imaginary_name
     << "; real_result_name = "
     << real_result_name
     << "; imaginary_name = "
     << imaginary_result_name
     << "; name = "
     << name_attribute -> getValue()
     << "; result = "
     << result_attribute -> getValue()
     << (argument_result_attribute ? "; argument_result = " : "")
     << (argument_result_attribute ? argument_result_attribute -> getValue() : "")
     << endl;
cout.flush();
*/
             name_attribute -> resetValue(imaginary_result_name);
             result_attribute -> resetValue(imaginary_result_name);
             if (argument_result_attribute) {
                 argument_result_attribute -> resetValue(imaginary_result_name);
             }
         }
         else {
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
                 ROSE2LLVM_ASSERT(function_declaration);
                 vector<SgInitializedName *> parms = function_declaration -> get_args();
                 for (int i = 0; i < parms.size(); i++) {
                     function_parm_types.push_back(parms[i] -> get_type());
                 }
             }
             else if (dynamic_cast<SgPointerDerefExp *>(n -> get_function())) {
                 SgExpression *function_operand = isSgPointerDerefExp(n -> get_function()) -> get_operand();
                 SgFunctionType *function_type = getFunctionType(attributes -> getSourceType(attributes -> getExpressionType(function_operand)));
                 ROSE2LLVM_ASSERT(function_type);
                 function_name = ((StringAstAttribute *) function_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 function_type_name = ((StringAstAttribute *) function_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 function_parm_types = function_type -> get_arguments();
             }
             else if (dynamic_cast<SgVarRefExp *>(n -> get_function())) {
                 SgVarRefExp *var_ref = isSgVarRefExp(n -> get_function());
                 SgFunctionType *function_type = getFunctionType(attributes -> getSourceType(attributes -> getExpressionType(var_ref)));
                 ROSE2LLVM_ASSERT(function_type);
                 function_name = ((StringAstAttribute *) var_ref -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 function_type_name = ((StringAstAttribute *) function_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 function_parm_types = function_type -> get_arguments();
             }
             else if (dynamic_cast<SgDotExp *>(n -> get_function())) {
                 SgDotExp *dot_exp = isSgDotExp(n -> get_function());
                 SgFunctionType *function_type = getFunctionType(attributes -> getSourceType(attributes -> getExpressionType(dot_exp)));
                 ROSE2LLVM_ASSERT(function_type);
                 ROSE2LLVM_ASSERT(dot_exp -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
                 function_name = ((StringAstAttribute *) dot_exp -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 function_type_name = ((StringAstAttribute *) function_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 function_parm_types = function_type -> get_arguments();
             }
             else if (dynamic_cast<SgPntrArrRefExp *>(n -> get_function())) {
                 SgPntrArrRefExp *pntr_arr_ref = isSgPntrArrRefExp(n -> get_function());
                 SgFunctionType *function_type = getFunctionType(attributes -> getSourceType(attributes -> getExpressionType(pntr_arr_ref)));
                 ROSE2LLVM_ASSERT(function_type);
                 ROSE2LLVM_ASSERT(pntr_arr_ref -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
                 function_name = ((StringAstAttribute *) pntr_arr_ref -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 function_type_name = ((StringAstAttribute *) function_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 function_parm_types = function_type -> get_arguments();
             }
             else if (dynamic_cast<SgFunctionCallExp *>(n -> get_function())) {
                 SgFunctionCallExp *function_call = isSgFunctionCallExp(n -> get_function());
                 SgFunctionType *function_type = getFunctionType(attributes -> getExpressionType(function_call));
                 ROSE2LLVM_ASSERT(function_type);
                 ROSE2LLVM_ASSERT(function_call -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
                 function_name = ((StringAstAttribute *) function_call -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 function_type_name = ((StringAstAttribute *) function_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 function_parm_types = function_type -> get_arguments();
             }
             else if (dynamic_cast<SgCastExp *>(n -> get_function())) {
                 SgCastExp *cast_exp = isSgCastExp(n -> get_function());
                 SgFunctionType *function_type = getFunctionType(attributes -> getSourceType(attributes -> getExpressionType(cast_exp)));
                 ROSE2LLVM_ASSERT(function_type);
                 ROSE2LLVM_ASSERT(cast_exp -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
                 function_name = ((StringAstAttribute *) cast_exp -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 function_type_name = ((StringAstAttribute *) function_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 function_parm_types = function_type -> get_arguments();
             }
             else {
                 cout << "Don't know how to process Initializer element of type " << n -> get_function() -> class_name() << std::endl;
                 cout.flush();
                 ROSE2LLVM_ASSERT(! "This should not happen");
             }

             /**
              * TODO: Say Something
              */
             SgType *return_type = attributes -> getExpressionType(n); // n -> get_type();
             string original_return_type_name = ((StringAstAttribute *) return_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    return_type_name = original_return_type_name;

             int integral_class_return_type = attributes -> integralStructureType(return_type);
             vector<SgExpression *> args = n -> get_args() -> get_expressions();
             for (int i = 0; i < args.size(); i++) {
                 SgExpression *arg = args[i];
                 SgType *arg_type = attributes -> getSourceType(arg -> get_type());

                 if (isSgClassType(attributes -> getSourceType(arg_type))) {
                     string arg_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                            coerce_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_CLASS_COERCE)) -> getValue(),
                            bit_cast_name = ((StringAstAttribute *) arg -> getAttribute(Control::LLVM_CLASS_BIT_CAST)) -> getValue(),
                            arg_type_name = ((StringAstAttribute *) arg_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     int size = ((IntAstAttribute *) arg_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << bit_cast_name << " = bitcast " << arg_type_name << "* " << coerce_name << " to i8*" << debug_md << endl;
                     (*codeOut) << CodeEmitter::indent() << "call void @llvm.memcpy.p0i8.p0i8.i64(i8* "<< bit_cast_name << ", i8* " << arg_name << ", i64 " << size << ", i32 4, i1 false)" << debug_md << endl;
                 }
             }

             string result_name;
             if (isSgTypeVoid(attributes -> getSourceType(return_type))) {
                 (*codeOut) << CodeEmitter::indent() << "call ";
                 return_type_name = "void";
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
             }

             /**
              * When invoking a function with variable arguments or a function that returns a pointer to a function,
              * the full signature of the function must be used. Otherwise, we only need to specify the return type.
              */
             SgFunctionSymbol *function_symbol = n -> getAssociatedFunctionSymbol();
             if (function_symbol) {
                 SgFunctionType *function_type = isSgFunctionType(function_symbol -> get_type());
                 ROSE2LLVM_ASSERT(function_type);
                 SgPointerType *pointer_return_type = isSgPointerType(attributes -> getSourceType(return_type));
                 /**
                  * ERROR TODO: ROSE does not set the get_has_ellipses() flag properly. So, we have to recompute it here!
                  *
                  * We will replace the following line with the lines below enclosed between the two lines with "//*** BUG FIX"
                  *
                  *     if (function_type -> get_has_ellipses() || function_type -> attributeExists(Control::LLVM_COMPILER_GENERATED) ||
                  */
                  //*** BUG FIX
                 SgFunctionDeclaration *function_declaration = n -> getAssociatedFunctionDeclaration();
                 vector <SgInitializedName *> &args = function_declaration -> get_args();
                 bool has_ellipses = (args.size() > 0 && isSgTypeEllipse(args[args.size() - 1] -> get_type()));
                 if (has_ellipses || function_type -> attributeExists(Control::LLVM_COMPILER_GENERATED) ||
                  //*** BUG FIX
                     (pointer_return_type && isSgFunctionType(attributes -> getSourceType(pointer_return_type -> get_base_type())))) {   // function returns a pointer to a function?
                       (*codeOut) << function_type_name
                                  // << "* " // Not needed starting with LLVM 4.0
                                  ;
                 }
                 else {
                     (*codeOut) << return_type_name;
                 }
             }
             else {
                 (*codeOut) << return_type_name;
             }

             (*codeOut) << " " << function_name << "(";

             /**
              * The function returns a structure type that cannot be stored in an integral unit.
              */
             if (isSgClassType(attributes -> getSourceType(return_type)) && integral_class_return_type == 0) {
                 (*codeOut) << original_return_type_name << "* noalias sret " << result_name << (args.size() > 0 ? ", " : "");
             }
// TODO: Remove this !!!
/*
cout
  << "*** I am here with function "
  << (function_symbol ? function_symbol -> get_name().getString() : function_name +"???")
  << ":"
 << endl;
cout.flush();
*/
             for (int i = 0; i < args.size(); i++) {
                 SgType *type = (i < function_parm_types.size() && (! isSgTypeEllipse(function_parm_types[i]) && (! args[i] -> attributeExists(Control::LLVM_ARRAY_BIT_CAST)))
                                                   ? attributes -> getSourceType(function_parm_types[i])
                                                   : attributes -> getExpressionType(args[i]));
                 SgArrayType *array_type = isSgArrayType(type);
                 string arg_name = ((array_type || isSgPointerType(type)) &&
                                    ((isSgIntVal(args[i]) && isSgIntVal(args[i]) -> get_value() == 0) || (isSgEnumVal(args[i]) && isSgEnumVal(args[i]) -> get_value() == 0))
                                        ? "null"
                                        : ((StringAstAttribute *) args[i] -> getAttribute(Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME)) -> getValue()),
                        parm_type_name;
// TODO: Remove this !!!
/*
ROSE2LLVM_ASSERT(type);
cout
  << "    argument "
  << i
  << (i < function_parm_types.size() ? " (from parms)" : " (from args)")
  << " has type "
  << type -> class_name()
 << endl;
cout.flush();
*/
                 if (array_type) {
                     parm_type_name = ((StringAstAttribute *) array_type -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     parm_type_name += "*";
                 }
                 else {
                     parm_type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     if (isSgClassType(type)) {
                         parm_type_name += "* byval";
                     }
                     else if (isSgFunctionType(type)) {
                         parm_type_name += "*";
                     }
                 }

                 (*codeOut) << parm_type_name << " " << arg_name;
                 if (i + 1 < args.size())
                      (*codeOut) << ", ";
             }
             (*codeOut) << ")" << debug_md << endl;

             if (isSgClassType(attributes -> getSourceType(return_type))) {
                 if (integral_class_return_type) {
                     string cast_name = attributes -> getTemp(LLVMAstAttributes::TEMP_INT),
                            coerce_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_COERCE)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << cast_name << " = bitcast " << original_return_type_name << "* " << coerce_name << " to " << return_type_name << "*" << debug_md << endl;
                     (*codeOut) << CodeEmitter::indent() << "store " << return_type_name << " " << result_name << ", " << return_type_name << "* " << cast_name << ", align 1" << debug_md << endl;

                     result_name = coerce_name;
                 }

                 if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
                     string bit_cast_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_RETURNED_STRUCTURE_BIT_CAST)) -> getValue();
                     (*codeOut) << CodeEmitter::indent() << bit_cast_name << " = bitcast " << original_return_type_name << "* " << result_name << " to i8*" << debug_md << endl;
                 }
             }
         }
     }
     //                 SgSizeOfOp
     else if (dynamic_cast<SgSizeOfOp *>(node)) { // postVisit
         SgSizeOfOp *n = isSgSizeOfOp(node);

         /**
          * TODO: Obsolete !
          *
         SgExpression *expr_operand = n -> get_operand_expr();
         ROSE2LLVM_ASSERT(n -> get_operand_type() || expr_operand);
         SgType *operand_type = attributes -> getSourceType(n -> get_operand_type() ? n -> get_operand_type() : expr_operand -> get_type());

         stringstream out;
          */

         /**
          * TODO: Obsolete !
          *
          * If the SgSizeOfOp expression is not a constant expression then it has not yet being computed and processed.
          * We need to generate code for computing its size here.
         if (! n -> attributeExists(Control::LLVM_SIZE)) {
             SgArrayType *array_type = isSgArrayType(operand_type);
             ROSE2LLVM_ASSERT(array_type);
             StringAstAttribute *attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
             ROSE2LLVM_ASSERT(attribute);
             ROSE2LLVM_ASSERT(array_type -> get_index());
             generateArraySizeCode(attribute -> getValue(), attributes -> getSourceType(array_type -> get_index() -> get_type()), array_type);
         }
         */
     }
     //                 SgUpcLocalsizeof
     //                 SgUpcBlocksizeof
     //                 SgUpcElemsizeof
     //             SgTypeIdOp
     //             SgConditionalExp
     else if (dynamic_cast<SgConditionalExp *>(node)) { // postVisit
         SgConditionalExp *n = isSgConditionalExp(node);

         string result_name, true_name, false_name;

         if (n -> attributeExists(Control::LLVM_SELECT_CONDITIONAL) || !isSgTypeVoid(n -> get_type())) {
             result_name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             true_name = ((StringAstAttribute *) n -> get_true_exp() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             false_name = ((StringAstAttribute *) n -> get_false_exp() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         }

         string debug_md = attributes->addDebugMetadata(node, current_function_decls);
         if (n -> attributeExists(Control::LLVM_SELECT_CONDITIONAL)) {
             string cond_name = ((StringAstAttribute *) n -> get_conditional_exp() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue(),
                    result_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n) -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    true_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_true_exp()) -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                    false_type_name = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_false_exp()) -> getAttribute(Control::LLVM_TYPE)) -> getValue();
// TODO: Remove this !
/*
cout
  << "result_type_name = "
  << result_type_name
  << ";  true_type_name = "
  << true_type_name
  << ";  false_type_name = "
  << false_type_name
  << endl;
cout.flush();
*/
             if (isSgFunctionType(attributes -> getExpressionType(n))) { // Function types must be converted to pointer. TODO: Should we always BitCast to this type?
                 result_type_name += "*"; 
             }
             if (isSgFunctionType(attributes -> getExpressionType(n -> get_true_exp()))) { // Function types must be converted to pointer.
                 true_type_name += "*"; 
             }
             if (isSgFunctionType(attributes -> getExpressionType(n -> get_false_exp()))) { // Function types must be converted to pointer.
                 false_type_name += "*";
             }
     
             (*codeOut) << CodeEmitter::indent() << result_name << " = select i1 " << cond_name << ", "
                        << true_type_name << " " << true_name << ", " << false_type_name << " " << false_name << debug_md << endl;
         }
         else {
             string result_type_name = (n -> attributeExists(Control::LLVM_IS_BOOLEAN)
                                           ? "i1"
// TODO: Remove this !
//                                           : ((StringAstAttribute *) n -> get_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue());
                                           : ((StringAstAttribute *) attributes -> getExpressionType(n) -> getAttribute(Control::LLVM_TYPE)) -> getValue());
             ConditionalComponentAstAttribute *true_attribute = (ConditionalComponentAstAttribute *) n -> get_true_exp() -> getAttribute(Control::LLVM_CONDITIONAL_COMPONENT_LABELS),
                                              *false_attribute = (ConditionalComponentAstAttribute *) n -> get_false_exp() -> getAttribute(Control::LLVM_CONDITIONAL_COMPONENT_LABELS);
             codeOut -> emitLabel(current_function_decls, true_attribute -> getEndLabel());

             // If the conditional has void type, we don't need to save
             // the result, so no phi node is needed
             if (! isSgTypeVoid(n -> get_type())) {
                 (*codeOut) << CodeEmitter::indent() << result_name << " = phi " << result_type_name << " [" << true_name << ", %" << true_attribute -> getLastLabel() << "], "
                            << "[" << false_name << ", %" << false_attribute -> getLastLabel() << "]" << debug_md << endl;
             }
         }
     }
     //             SgNewExp
     //             SgDeleteExp
     //             SgThisExp
     //             SgRefExp
     //             SgInitializer:
     //                 SgAggregateInitializer
     else if (dynamic_cast<SgAggregateInitializer *>(node)) { // postVisit
         SgAggregateInitializer *n = isSgAggregateInitializer(node);
     }
     //                 SgConstructorInitializer
     //                 SgAssignInitializer
     else if (dynamic_cast<SgAssignInitializer *>(node)) { // postVisit
         SgAssignInitializer *n = isSgAssignInitializer(node);
     }
     //                 SgDesignatedInitializer
     //             SgVarArgStartOp
     //             SgVarArgOp
     //             SgVarArgEndOp
     //             SgVarArgCopyOp
     //             SgVarArgStartOneOperandOp
     //             SgNullExpression
     else if (dynamic_cast<SgNullExpression *>(node)) { // postVisit
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
         ROSE2LLVM_ASSERT(0);
     }
}


/**
 * Perform any required exit action for this node after completion of its post-visit.
 */
void CodeGeneratorVisitor::postVisitExit(SgNode *node) {
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
         if (option.isDebugPreTraversal() && option.isDebugPostTraversal() && option.isDebugOutput()) {
             Sg_File_Info *file_info = n -> get_endOfConstruct();
             cout << "*** Done with statement ended at line "
                  << file_info -> get_line() 
                  << endl;
             cout.flush();
         }

         if (n -> attributeExists(Control::LLVM_IF_COMPONENT_LABELS)) {
             IfComponentAstAttribute *attribute = (IfComponentAstAttribute *) n -> getAttribute(Control::LLVM_IF_COMPONENT_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel(), attributes->addDebugMetadata(node, current_function_decls));
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
             (*codeOut) << CodeEmitter::indent() << extension_name << " = zext i1 " << " " << name_attribute -> getValue() << " to " << result_type_name << attributes->addDebugMetadata(node, current_function_decls) << endl;
         }

         /**
          * Special case for arithmetic expressions.
          *
          * The C language allows an arithmetic value to be used where a boolean value is expected.
          * In such a case we have to "cast" the arithmetic value into a boolean by comparing it to 0.
          */
         if (n -> attributeExists(Control::LLVM_BOOLEAN_CAST)) {
             genZeroCompareOperation(n, attributes->addDebugMetadata(n, current_function_decls));
         }

         /**
          *
          */
         else if (n -> attributeExists(Control::LLVM_INTEGRAL_PROMOTION)) {
             SgCastAstAttribute *promote_type_attribute = (SgCastAstAttribute *) n -> getAttribute(Control::LLVM_INTEGRAL_PROMOTION);
             ROSE2LLVM_ASSERT(promote_type_attribute);
             SgType *result_type = promote_type_attribute -> getType(),
                    *type = attributes -> getExpressionType(n);
             string promote_name = promote_type_attribute -> getName();
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             string type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             string result_type_name = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << promote_name << " = " << (attributes -> isUnsignedType(type) ? "zext " : "sext ")
                        << type_name << " " <<  name << " to " << result_type_name << attributes->addDebugMetadata(node, current_function_decls) << endl;

             StringAstAttribute *result_name_attribute = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
             ROSE2LLVM_ASSERT(result_name_attribute);
             result_name_attribute -> resetValue(promote_name);

             result_name_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME);
             if (result_name_attribute) {
                 result_name_attribute -> resetValue(promote_name);
             }

             SgTypeAstAttribute *result_type_attribute = (SgTypeAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
             ROSE2LLVM_ASSERT(result_type_attribute);
             result_type_attribute -> resetType(result_type);
         }
         else if (n -> attributeExists(Control::LLVM_INTEGRAL_DEMOTION)) {
             SgCastAstAttribute *demote_type_attribute = (SgCastAstAttribute *) n -> getAttribute(Control::LLVM_INTEGRAL_DEMOTION);
             ROSE2LLVM_ASSERT(demote_type_attribute);
             SgType *result_type = demote_type_attribute -> getType(),
                    *type = attributes -> getExpressionType(n);
             string demote_name = demote_type_attribute -> getName();
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
             string type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             string result_type_name = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << demote_name << " = " << "trunc " // (isUnsignedType(type) ? "zext " : "sext ")
                        << type_name << " " <<  name << " to " << result_type_name << attributes->addDebugMetadata(node, current_function_decls) << endl;

             StringAstAttribute *result_name_attribute = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
             ROSE2LLVM_ASSERT(result_name_attribute);
             result_name_attribute -> resetValue(demote_name);

             result_name_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME);
             if (result_name_attribute) {
                 result_name_attribute -> resetValue(demote_name);
             }

             SgTypeAstAttribute *result_type_attribute = (SgTypeAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
             ROSE2LLVM_ASSERT(result_type_attribute);
             result_type_attribute -> resetType(result_type);
         }

         /**
          * These are special cases for the subexpressions in a conditional expression.
          */
         if (n -> attributeExists(Control::LLVM_CONDITIONAL_TEST)) {
             ConditionalAstAttribute *attribute = (ConditionalAstAttribute *) n -> getAttribute(Control::LLVM_CONDITIONAL_TEST);
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getTrueLabel() << ", label %" << attribute -> getFalseLabel() << attributes->addDebugMetadata(node, current_function_decls) << endl;
         }
         else if (n -> attributeExists(Control::LLVM_CONDITIONAL_COMPONENT_LABELS)) {
             ConditionalComponentAstAttribute *attribute = (ConditionalComponentAstAttribute *) n -> getAttribute(Control::LLVM_CONDITIONAL_COMPONENT_LABELS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_AND_LHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_AND_LHS);
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getRhsLabel() << ", label %" << attribute -> getEndLabel() << attributes->addDebugMetadata(node, current_function_decls) << endl;
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_AND_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_AND_RHS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_OR_LHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_OR_LHS);
             string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             (*codeOut) << CodeEmitter::indent() << "br i1 " << name << ", label %" << attribute -> getEndLabel() << ", label %" << attribute -> getRhsLabel() << attributes->addDebugMetadata(node, current_function_decls) << endl;
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_OR_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_OR_RHS);
             codeOut -> emitUnconditionalBranch(attribute -> getEndLabel(), attributes->addDebugMetadata(node, current_function_decls));
         }
    }

    /**
     * If we are processing a synthetic function and we are done processing a declaration or an "elected"
     * loop then stop the translation to prevent code from being emitted for other executable statements.
     */
    if (option.isTranslating() && node -> attributeExists(Control::LLVM_COST_ANALYSIS)) {
        option.resetTranslating();
    }
}

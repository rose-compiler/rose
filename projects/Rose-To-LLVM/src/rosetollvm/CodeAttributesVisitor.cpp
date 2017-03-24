#include "rosetollvm/CodeAttributesVisitor.h"
#include "rosetollvm/AttributesAstAttribute.h"
#include "rosetollvm/Control.h"
#include "FileHelper.h"
#include <iostream>

using namespace std;
using namespace NodeQuery;
using namespace llvm;

void CodeAttributesVisitor::processRemainingComponents() {
    /**
     * Note that NO MORE attributes will be added to revisitAttributes as we only revisit function declarations here.
     */
    for (int k = 0; k < revisitAttributes.size(); k++) {
        int num_additional_functions_start = 0,
            num_additional_function_attributes_start = 0;

        do {
            setAttributes(revisitAttributes[k]); // Reset the correct environment
            //
            // Note that MORE function declarations may be added to additional functions as we are going through this loop.
            //
            for (int i = num_additional_functions_start; i < attributes -> numAdditionalFunctions(); i++) {
                SgFunctionDeclaration *function_declaration = attributes -> getAdditionalFunction(i);

                /**
                 * This condition occurs when an invocation to a function is encountered in a file prior to the 
                 * definition in the same file.
                 */
                if (function_declaration -> attributeExists(Control::LLVM_IGNORE)) {
                    control.SetAttribute(function_declaration, Control::LLVM_FUNCTION_NEEDS_REVISIT, new AttributesAstAttribute(attributes)); 
                }
                else if (! function_declaration -> attributeExists(Control::LLVM_LOCAL_DECLARATIONS)) {
                    control.SetAttribute(function_declaration, Control::LLVM_FUNCTION_NEEDS_REVISIT, new AttributesAstAttribute(attributes)); 
                    this -> traverse(function_declaration);
                }
            }
            num_additional_functions_start = attributes -> numAdditionalFunctions(); // save next index in case more additional functions are added

            /**
             * Process declarations that were imported from a header file. Note
             * that this loop must be processed after the previous one as more
             * declarations may be imported as the remaining functions are being
             * processed.
             */
            for (int j = num_additional_function_attributes_start; j < attributes -> numAdditionalFunctionAttributes(); j++) {
                current_function_attribute.push(attributes -> getAdditionalFunctionAttribute(j));
                for (int i = 0; i < current_function_attribute.top() -> numRemoteVariableDeclarations(); i++) {
                    SgInitializedName *decl = current_function_attribute.top() -> getRemoteVariableDeclaration(i);
                    this -> traverse(decl);
                }
                current_function_attribute.pop();
            }
            num_additional_function_attributes_start = attributes -> numAdditionalFunctionAttributes();

            /**
             * Process global declarations that were imported from another global declaration.
             */
            for (int i = 0; i < attributes -> numRemoteGlobalDeclarations(); i++) {
                SgInitializedName *decl = attributes -> getRemoteGlobalDeclaration(i);
                this -> traverse(decl);
            }
        } while (num_additional_functions_start < attributes -> numAdditionalFunctions() ||
                num_additional_function_attributes_start < attributes -> numAdditionalFunctionAttributes());
    }

    /**
     * Traverse each file; look for global variables with several declarations; make their attributes consistent. 
     */
    for (int k = 0; k < sourceFiles.size(); k++) {
        setAttributes((LLVMAstAttributes *) sourceFiles[k] -> getAttribute(Control::LLVM_AST_ATTRIBUTES));

        for (int j = 0; j < attributes -> numGlobalDeclarations(); j++) {
            if (dynamic_cast<SgInitializedName *>(attributes -> getGlobalDeclaration(j))) {
                SgInitializedName *n = isSgInitializedName(attributes -> getGlobalDeclaration(j));
                if (n -> get_initializer()) {
                    /**
                     * If this global declaration is an initialized declaration then check whether or not other uninitialized 
                     * declaration of this variable were encountered. If so, update the attributes of these other declarations
                     * with the correct attributes.  For example:
                     *
                     *   extern int a[];
                     *   . . .  
                     *   int a[] = { 0, 1, 2, 3, 4, 5 }; // size of this array is 6
                     */
                    string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
                    vector<SgInitializedName *> &decls = attributes -> global_variable_declaration_map[name];
                    if (decls.size() > 1) {
                        for (int i = 0; i < decls.size(); i++) {
                            SgInitializedName *decl = decls[i];
                            if (i != 0 && decl == n) {
                                decls[i] = decls[0];
                                decls[0] = n;
                            }
                            else {
                                if (decl -> attributeExists(Control::LLVM_TYPE)) {
                                    control.UpdateAttribute(decl, Control::LLVM_TYPE, new StringAstAttribute(((StringAstAttribute *) n -> getAttribute(Control::LLVM_TYPE)) -> getValue()));
                                }
                                else {
                                    control.SetAttribute(decl, Control::LLVM_TYPE, new StringAstAttribute(((StringAstAttribute *) n -> getAttribute(Control::LLVM_TYPE)) -> getValue()));
                                }
                                if (n -> attributeExists(Control::LLVM_AGGREGATE)) {
                                    if (decl -> attributeExists(Control::LLVM_AGGREGATE)) {
                                        control.UpdateAttribute(decl, Control::LLVM_AGGREGATE, new AggregateAstAttribute((AggregateAstAttribute *) n -> getAttribute(Control::LLVM_AGGREGATE)));
                                    }
                                    else {
                                        control.SetAttribute(decl, Control::LLVM_AGGREGATE, new AggregateAstAttribute((AggregateAstAttribute *) n -> getAttribute(Control::LLVM_AGGREGATE)));
                                    }
                                }
                                if (n -> attributeExists(Control::LLVM_BIT_CAST)) {
                                    if (decl -> attributeExists(Control::LLVM_BIT_CAST)) {
                                        control.UpdateAttribute(decl, Control::LLVM_BIT_CAST, new StringAstAttribute(((StringAstAttribute *) n -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue()));
                                    }
                                    else {
                                        control.SetAttribute(decl, Control::LLVM_BIT_CAST, new StringAstAttribute(((StringAstAttribute *) n -> getAttribute(Control::LLVM_BIT_CAST)) -> getValue()));
                                    }
                                }
                                if (n -> attributeExists(Control::LLVM_STRING_SIZE)) {
                                    if (decl -> attributeExists(Control::LLVM_STRING_SIZE)) {
                                        control.UpdateAttribute(decl, Control::LLVM_STRING_SIZE, new IntAstAttribute(((IntAstAttribute *) n -> getAttribute(Control::LLVM_STRING_SIZE)) -> getValue()));
                                    }
                                    else {
                                        control.SetAttribute(decl, Control::LLVM_STRING_SIZE, new IntAstAttribute(((IntAstAttribute *) n -> getAttribute(Control::LLVM_STRING_SIZE)) -> getValue()));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}



void CodeAttributesVisitor::processVariableDeclaration(SgInitializedName *n) {
    /**
     * Field declarations are processed the first time a use of their type in encountered.
     * Other variables are processed the first time they are used in an expression.
     */
    if (isSgClassDefinition(n -> get_scope())) {
        return;
    }

    ROSE2LLVM_ASSERT(! n -> attributeExists(Control::LLVM_NAME));
    SgType *type = attributes -> getSourceType(n -> get_type());
    attributes -> setLLVMTypeName(type);

    SgArrayType *array_type = isSgArrayType(type); // Record whether or not this type is an array type
    SgInitializer *initializer = n -> get_initializer();
    /**
     * If we are dealing with a variable with an array type without specified bounds that is initialized
     * with an aggregate then we need to compute the  size of the array from the aggregate.
     */
    if (array_type && initializer && (! isSgIntVal(array_type -> get_index()))) {
        size_t size;
        if (isSgAggregateInitializer(initializer)) {
            size = isSgAggregateInitializer(initializer) -> get_initializers() -> get_expressions().size();
        }
        else { // a string is an array of characters.
            SgAssignInitializer *assign_initializer = isSgAssignInitializer(initializer);
            ROSE2LLVM_ASSERT(assign_initializer);
            SgStringVal *str = isSgStringVal(assign_initializer -> get_operand());
            ROSE2LLVM_ASSERT(str);
            size = str -> get_value().size() + 1;
        }
        // To be sure to get new nodes that we can
        // later free, don't use SageBuilder for types.
        SgIntVal *expr = isSgIntVal(control.ownNode(SageBuilder::buildIntVal(size)));
        ROSE2LLVM_ASSERT(expr);
        array_type = control.ownNode(new SgArrayType(array_type -> get_base_type(), expr)); // create new array type with bound
        type = array_type;
        attributes -> setLLVMTypeName(type);
    }

    string type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
    control.SetAttribute(n, Control::LLVM_TYPE, new StringAstAttribute(type_name)); // Tag this declaration with its bounded type name.
    control.SetAttribute(n, Control::LLVM_DECLARATION_TYPE, new SgTypeAstAttribute(type)); // TODO: We tag declaration with the sized type here. Currently, this attribute is not used anywhere else... Do we need it?

    // If the alignment has been specified through GCC attributes, store this
    // with the declaration so we can use it later
    if (n -> get_gnu_attribute_alignment() != 0) {
        int alignment = n -> get_gnu_attribute_alignment();

        //
        // TODO: Remove this code when this Rose bug is fixed. Rose needs to set alignment properly for variables.
        //
        if (alignment < 1) {
            if (! type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
                attributes -> setLLVMTypeName(type);
            }
            ROSE2LLVM_ASSERT((IntAstAttribute *) type -> getAttribute(Control::LLVM_ALIGN_TYPE));
            alignment = ((IntAstAttribute *) type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
        }
        control.SetAttribute(n, Control::LLVM_ALIGN_VAR, new IntAstAttribute(alignment));
    }


    //
    // WARNING: Do not use n -> get_storageModifier(); See SgStorageModifier documentation for detail.
    //
    SgStorageModifier &sm = n -> get_declaration() -> get_declarationModifier().get_storageModifier();
    string name;

    /**
     * For some reason, Rose places parameter declarations in a function header in the global scope.
     * Hence the explicit test to exclude these parameters here.
     */
    if (isSgGlobal(n -> get_scope()) && (! n -> attributeExists(Control::LLVM_PARAMETER))) {
        name = "@";
        name += n -> get_name().getString();
        attributes -> insertGlobalDeclaration(n);
        control.SetAttribute(n, Control::LLVM_GLOBAL_DECLARATION);
        attributes -> global_variable_declaration_map[name].push_back(n);
        control.SetAttribute(n, Control::LLVM_GLOBAL_CONSTANT_NAME, new StringAstAttribute(name));
    }
    else if (sm.isStatic() || sm.isExtern()) {
        if (!current_function_attribute.empty()) { // static local variable (declaration is nested inside a function)
            ROSE2LLVM_ASSERT(getFunctionDefinition(n -> get_scope()));
            SgFunctionDeclaration *current_function = current_function_attribute.top() -> getFunctionDeclaration();
            if (sm.isStatic()) {
                name = "@" + current_function_attribute.top() -> getFunctionDeclaration() -> get_name() + "." + n -> get_name().getString();
            }
            else {
                name = "@";
                name += n -> get_name().getString();
            }
        }
        else {
            // TODO:
            cerr << "Don't yet know how to process this nested static or extern declarations inside a "
                 << current_function_attribute.top() -> getFunctionDeclaration() -> class_name() <<  endl;
            cerr.flush();
            ROSE2LLVM_ASSERT(0);
        }
        control.SetAttribute(n, Control::LLVM_GLOBAL_DECLARATION);
        attributes -> insertGlobalDeclaration(n);
        control.SetAttribute(n, Control::LLVM_GLOBAL_CONSTANT_NAME, new StringAstAttribute(name));
    }
    else { // local variable
        current_function_attribute.top() -> addSgInitializedName(n);
        name = "%";
        name += n -> get_name().getString();

        int index = (current_function_attribute.top() -> containsName(name) ? current_function_attribute.top() -> numSgInitializedNames() : 0); // may be needed for disambiguation 

        if (n -> attributeExists(Control::LLVM_PARAMETER) && (! isSgClassType(attributes -> getSourceType(type)))) {
            name += ".addr";
        }
        else if (index > 0) {
            name += Control::IntToString(index); // add index in local_decls array to disambiguate
        }
        else {
            current_function_attribute.top() -> insertName(name);
        }

        if (dynamic_cast<SgAggregateInitializer *>(initializer)) { // if the local variable has an aggregate initializer
            control.SetAttribute(n, Control::LLVM_GLOBAL_DECLARATION);
            attributes -> insertGlobalDeclaration(n);
            string aggregate_init_name = "@" + current_function_attribute.top() -> getFunctionDeclaration() -> get_name() + "." + n -> get_name().getString()
                                             + (index > 0 ? Control::IntToString(index) : ""); // add index in local_decls array to disambiguate, if needed
            control.SetAttribute(n, Control::LLVM_GLOBAL_CONSTANT_NAME, new StringAstAttribute(aggregate_init_name));
        }
    }
    control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(name));

    /**
     * Identify the real type associated with this aggregate. Rose simply associates a SgTypeDefault with an SgAggregateInitializer.
     */
    if (dynamic_cast<SgAggregateInitializer *>(initializer)) {
        SgAggregateInitializer *aggregate = isSgAggregateInitializer(initializer);
        tagAggregate(aggregate, type, n -> attributeExists(Control::LLVM_GLOBAL_DECLARATION));

        /**
         * Set the alignment and the type for this aggregate initializer
         */
        SgClassType *class_type = isSgClassType(type);
        ROSE2LLVM_ASSERT(array_type || class_type);
        int alignment = 0;
        if (array_type) {
            if (array_type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
                alignment = ((IntAstAttribute *) array_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
            }
        }
        else if (class_type) {
            if (class_type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
                alignment = ((IntAstAttribute *) class_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
            }
        }
        if (alignment > 0) {
            control.SetAttribute(aggregate, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(alignment));
        }
    }
    else if (dynamic_cast<SgAssignInitializer *>(initializer)) {
        SgAssignInitializer *assign_initializer = isSgAssignInitializer(initializer);
        if (array_type) {
            SgStringVal *string_init = isSgStringVal(assign_initializer -> get_operand());
            ROSE2LLVM_ASSERT(string_init);
            if (string_init) {
                /**
                 * If a size was specified in the type specification, then tag the string with that size.
                 * Otherwise, the size will be inferred from the length of the string
                 */
                SgIntVal *array_size = isSgIntVal(array_type -> get_index());
                int string_size = (array_size ? array_size -> get_value() : (string_init -> get_value().size() + 1)); 
                control.SetAttribute(string_init, Control::LLVM_STRING_SIZE, new IntAstAttribute(string_size));
                if (n -> attributeExists(Control::LLVM_GLOBAL_DECLARATION)) {
                    control.SetAttribute(string_init, Control::LLVM_STRING_INITIALIZATION);
                }
                else attributes -> setNeedsMemcopy();
            }
        }
        else if (isSgClassType(attributes -> getSourceType(type))) {
            attributes -> setNeedsMemcopy();
        }
        else {
            SgCastExp *cast_expression = isSgCastExp(assign_initializer -> get_operand());
            SgValueExp *value = (cast_expression ? isSgValueExp(cast_expression -> get_operand()) : NULL);
            if (value && (! isSgStringVal(value))) {
                control.SetAttribute(cast_expression, Control::LLVM_IGNORE); // Ignore this subtree from now on.
                SgType *target_type = cast_expression -> get_type(),
                       *source_type = value -> get_type();
                string value_string = Control::primitiveCast(value, target_type);
                control.SetAttribute(cast_expression, Control::LLVM_NAME, new StringAstAttribute(value_string));
                control.SetAttribute(cast_expression, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(value_string));
                control.SetAttribute(cast_expression, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(target_type));
            }
        }

        /**
         * Set the alignment and the type for this assign initializer.
         */
        if (type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
            ROSE2LLVM_ASSERT((IntAstAttribute *) type -> getAttribute(Control::LLVM_ALIGN_TYPE));
            int alignment = ((IntAstAttribute *) type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
            control.SetAttribute(assign_initializer, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(alignment));
        }
        ROSE2LLVM_ASSERT((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE));
        string type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
        control.SetAttribute(assign_initializer, Control::LLVM_TYPE, new StringAstAttribute(type_name));
    }

    /**
     * if this variable declaration does not have an initializer assign its type here.
     */
    if (! initializer) {
        string type_name;
        if (array_type) {
            if (n -> attributeExists(Control::LLVM_PARAMETER)) { // a formal parameter with an array type? Change it to a pointer type.
                SgType *base_type = array_type -> get_base_type();
                type_name = ((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                type_name += "*";
            }
            else { // Identify this array type as an "aggregate" so that it gets loaded with the getelementptr instruction
                if (n -> attributeExists(Control::LLVM_GLOBAL_DECLARATION) && (! isSgIntVal(array_type -> get_index()))) {
                    // TODO: Should not the base type be used here?
                    type_name = "[0 x i8]";
                }
                else type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(array_type));
            }
        }
        else type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

        control.SetAttribute(n, Control::LLVM_TYPE, new StringAstAttribute(type_name));
    }

    return;
}

/**
 * We sometimes traverse the AST using the traverseInputFiles interface. In this traversal mode only nodes that
 * correspond to a construct in a given file are visited. Thus, if a declaration was imported via an
 * #include directive, we have to "manually" traverse that AST. This function does so for variables.
 */
void CodeAttributesVisitor::checkVariableDeclaration(SgVarRefExp *var_ref) {
    SgVariableSymbol *var_sym = var_ref -> get_symbol();
    ROSE2LLVM_ASSERT(var_sym);
    SgInitializedName *n = isSgInitializedName(var_sym -> get_declaration());
    ROSE2LLVM_ASSERT(n);

    /**
     * If this declaration has not yet been processed, take care of it here.
     */
    if (! n -> attributeExists(Control::LLVM_NAME)) {
        processVariableDeclaration(n);

        if (n -> get_initializer()) {
            if (current_function_attribute.empty()) {
                attributes -> insertRemoteGlobalDeclaration(n);
            }
            else {
                current_function_attribute.top() -> insertRemoteVariableDeclaration(n);
                if (! current_function_attribute.top() -> getFunctionDeclaration() -> attributeExists(Control::LLVM_IGNORE)) {
                    control.SetAttribute(current_function_attribute.top() -> getFunctionDeclaration(), Control::LLVM_IGNORE);
                    attributes -> insertAdditionalFunction(current_function_attribute.top() -> getFunctionDeclaration());
                }
            }
        }
    }

    return;
}


/**
 * Same as CheckVariableDeclaration() for functions.
 */
void CodeAttributesVisitor::checkFunctionDeclaration(SgFunctionRefExp *function_ref) {
    SgFunctionSymbol *function_sym = function_ref -> get_symbol();
    ROSE2LLVM_ASSERT(function_sym);
    SgFunctionDeclaration *n = isSgFunctionDeclaration(function_sym -> get_declaration() -> get_definingDeclaration());
    if (n) { // is there a defining declaration?
        if ((! n -> attributeExists(Control::LLVM_LOCAL_DECLARATIONS)) &&
            (! n -> attributeExists(Control::LLVM_FUNCTION_VISITED))) {
            SgStorageModifier &sm = n -> get_declarationModifier().get_storageModifier();
            if (sm.isStatic() || sm.isUnspecified() || sm.isDefault()) {
                control.SetAttribute(n, Control::LLVM_FUNCTION_VISITED);
                attributes -> insertAdditionalFunction(n);
            }
        }
    }

    return;
}


void CodeAttributesVisitor::tagAggregate(SgAggregateInitializer *aggregate_init, SgType *type, bool is_global_initialization) {
    SgArrayType *array_type = isSgArrayType(attributes -> getSourceType(type));
    SgClassType *class_type = isSgClassType(attributes -> getSourceType(type));
    ROSE2LLVM_ASSERT(array_type || class_type);
 
    /**
     * It's important that we tag aggregates here (recursively) so that all the sub-aggregates are also processed.
     */
    string type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
    control.SetAttribute(aggregate_init, Control::LLVM_TYPE, new StringAstAttribute(type_name));

    if (array_type) {
        control.SetAttribute(aggregate_init, Control::LLVM_AGGREGATE, new AggregateAstAttribute(array_type));

        SgType *sub_type = attributes -> getSourceType(array_type -> get_base_type());
        if (isSgArrayType(sub_type) || isSgClassType(sub_type)) { // an array of aggregates?
            vector<SgExpression *> exprs = aggregate_init -> get_initializers() -> get_expressions();
            for (int i = 0; i < exprs.size(); i++) {
                SgExpression *expr = exprs[i];
                if (dynamic_cast<SgAggregateInitializer *> (expr)) {
                    SgAggregateInitializer *sub_aggregate_init = isSgAggregateInitializer(expr);
                    tagAggregate(sub_aggregate_init, sub_type, is_global_initialization);
                }
                else if (dynamic_cast<SgAssignInitializer *>(expr)) {
                    SgAssignInitializer *assign_initializer = isSgAssignInitializer(expr);

                    ROSE2LLVM_ASSERT(sub_type -> getAttribute(Control::LLVM_TYPE));
                    string init_type_name = ((StringAstAttribute *) sub_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                    control.SetAttribute(assign_initializer, Control::LLVM_TYPE, new StringAstAttribute(init_type_name));

                    SgStringVal *string_init = isSgStringVal(assign_initializer -> get_operand());
                    if (isSgArrayType(sub_type) && string_init) {
                        /**
                         * If a size was specified in the type specification, then tag the string with that size.
                         * Otherwise, the size will be inferred from the length of the string
                         */
                        SgIntVal *array_size = isSgIntVal(isSgArrayType(sub_type) -> get_index());
                        ROSE2LLVM_ASSERT(array_size); // assert that the size was specified. The size can only be unspecified for a top-level declaration!
                        int string_size = array_size -> get_value();
                        control.SetAttribute(string_init, Control::LLVM_STRING_SIZE, new IntAstAttribute(string_size));
                        if (is_global_initialization) {
                            control.SetAttribute(string_init, Control::LLVM_STRING_INITIALIZATION);
                        }
                        else attributes -> setNeedsMemcopy();
                    }
                }
                else {
                    cout << "Don't know how to process element of type " << expr -> class_name() << " in aggregate initializer" << std::endl;
                    cout.flush();
                    ROSE2LLVM_ASSERT(! "This should not happen");
                }
            }
        }
        else { // This is an array of scalars.  Check whether or not the operand at Casts of a constant 
            vector<SgExpression *> exprs = aggregate_init -> get_initializers() -> get_expressions();
            for (int i = 0; i < exprs.size(); i++) {
                SgAssignInitializer *assign_initializer = isSgAssignInitializer(exprs[i]);
                ROSE2LLVM_ASSERT(assign_initializer);
                SgCastExp *cast_expression = isSgCastExp(assign_initializer -> get_operand());
                SgValueExp *value = (cast_expression ? isSgValueExp(cast_expression -> get_operand()) : NULL);
                if (value  && (! isSgStringVal(value))) {
                    control.SetAttribute(cast_expression, Control::LLVM_IGNORE); // Ignore this subtree from now on.
                    SgType *target_type = cast_expression -> get_type(),
                           *source_type = value -> get_type();
                    string value_string = Control::primitiveCast(value, target_type);
                    control.SetAttribute(cast_expression, Control::LLVM_NAME, new StringAstAttribute(value_string));
                    control.SetAttribute(cast_expression, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(value_string));
                    control.SetAttribute(cast_expression, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(target_type));
                }
            }
        }
    }
    else if (class_type) {
        control.SetAttribute(aggregate_init, Control::LLVM_AGGREGATE, new AggregateAstAttribute(class_type));

        DeclarationsAstAttribute *attribute = attributes -> class_map[class_type -> get_qualified_name().getString()];

        /**
         * Note that an aggregate initializer may have too many or too few elements.  Thus, we iterate over
         * the elements of the aggregate until we reach the upper bound element of the aggregate or the upper
         * bound of the fields in the structure.
         */
        vector<SgExpression *> exprs = aggregate_init -> get_initializers() -> get_expressions();
        for (int i = 0; i < exprs.size() && i < attribute -> numSgInitializedNames(); i++) {
            SgExpression *expr = exprs[i];
            SgInitializedName *decl = attribute -> getSgInitializedName(i);

            if (dynamic_cast<SgAggregateInitializer *> (expr)) {
                SgAggregateInitializer *sub_aggregate_init = isSgAggregateInitializer(expr);
                tagAggregate(sub_aggregate_init, decl -> get_type(), is_global_initialization);
            }
            else if (dynamic_cast<SgAssignInitializer *>(expr)) {
                SgAssignInitializer *assign_initializer = isSgAssignInitializer(expr);
                SgType *init_type = assign_initializer -> get_operand() -> get_type();
                attributes -> setLLVMTypeName(init_type);
                ROSE2LLVM_ASSERT(init_type -> getAttribute(Control::LLVM_TYPE));
                string init_type_name = ((StringAstAttribute *) init_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                control.SetAttribute(assign_initializer, Control::LLVM_TYPE, new StringAstAttribute(init_type_name));

                SgStringVal *string_value = isSgStringVal(assign_initializer -> get_operand());
                if (string_value) {
                    /**
                     * If a size was specified in the type specification, then tag the string with that size.
                     * Otherwise, the size will be inferred from the length of the string
                     */
                    SgArrayType *array_field_type = isSgArrayType(decl -> get_type());
                    if (array_field_type) {
                        SgIntVal *array_size = isSgIntVal(array_field_type -> get_index());
                        ROSE2LLVM_ASSERT(array_size); // assert that the size was specified. The size can only be unspecified for a top-level declaration!
                        int string_size = (array_size ? array_size -> get_value() : (string_value -> get_value().size() + 1)); 
                        control.SetAttribute(string_value, Control::LLVM_STRING_SIZE, new IntAstAttribute(string_size));
                        if (is_global_initialization) {
                            control.SetAttribute(string_value, Control::LLVM_STRING_INITIALIZATION);
                        }
                        else attributes -> setNeedsMemcopy();
                    }
                }
                else {
                    SgCastExp *cast_expression = isSgCastExp(assign_initializer -> get_operand());
                    SgValueExp *value = (cast_expression ? isSgValueExp(cast_expression -> get_operand()) : NULL);
                    if (value && (! isSgStringVal(value))) {
                        control.SetAttribute(cast_expression, Control::LLVM_IGNORE); // Ignore this subtree from now on.
                        SgType *target_type = cast_expression -> get_type(),
                               *source_type = value -> get_type();
                        string value_string = Control::primitiveCast(value, target_type);
                        control.SetAttribute(cast_expression, Control::LLVM_NAME, new StringAstAttribute(value_string));
                        control.SetAttribute(cast_expression, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(value_string));
                        control.SetAttribute(cast_expression, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(target_type));
                    }
                }
            }
            else if (dynamic_cast<SgStringVal *>(expr)) {
                SgStringVal *string_value = isSgStringVal(expr);
                if (string_value) {
                    /**
                     * If a size was specified in the type specification, then tag the string with that size.
                     * Otherwise, the size will be inferred from the length of the string
                     */
                    SgArrayType *array_field_type = isSgArrayType(decl -> get_type());
                    if (array_field_type) {
                        SgIntVal *array_size = isSgIntVal(array_field_type -> get_index());
                        ROSE2LLVM_ASSERT(array_size); // assert that the size was specified. The size can only be unspecified for a top-level declaration!
                        int string_size = (array_size ? array_size -> get_value() : (string_value -> get_value().size() + 1)); 
                        control.SetAttribute(string_value, Control::LLVM_STRING_SIZE, new IntAstAttribute(string_size));
                        if (is_global_initialization) {
                            control.SetAttribute(string_value, Control::LLVM_STRING_INITIALIZATION);
                        }
                        else attributes -> setNeedsMemcopy();
                    }
                }
            }
            else if (dynamic_cast<SgCastExp *>(expr)) {
                SgCastExp *cast_expression = isSgCastExp(expr);
                SgValueExp *value = isSgValueExp(cast_expression -> get_operand());
                if (value && (! isSgStringVal(value))) {
                    control.SetAttribute(cast_expression, Control::LLVM_IGNORE); // Ignore this subtree from now on.
                    SgType *target_type = cast_expression -> get_type(),
                           *source_type = value -> get_type();
                    string value_string = Control::primitiveCast(value, target_type);
                    control.SetAttribute(cast_expression, Control::LLVM_NAME, new StringAstAttribute(value_string));
                    control.SetAttribute(cast_expression, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(value_string));
                    control.SetAttribute(cast_expression, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(target_type));
                }
            }
            else if (dynamic_cast<SgFunctionRefExp *>(expr)) {
                SgFunctionRefExp *function_ref = isSgFunctionRefExp(expr);
                //  TODO: No need to do anything here ... I don't think.
            }
            else {
                cout << "Don't know how to process element of type " << expr -> class_name() << " in aggregate initializer" << std::endl;
                cout.flush();
                ROSE2LLVM_ASSERT(! "This should not happen");
            }
        }
    }

    return;
}


/**
  * Check whether a subtree rooted at n contains a node of type V_node and if it does, return it. Note that if n
  * is of type V_node then this function will return n. If no V_node is found then the value NULL is returned.
  */
SgNode *CodeAttributesVisitor::subtreeContainsNode(SgNode *n, VariantT V_node) {
    Rose_STL_Container<SgNode *> ops = NodeQuery::querySubTree(n, V_node);
    Rose_STL_Container<SgNode *>::iterator i = ops.begin();
    return (i != ops.end() ? isSgNode(*i) : NULL);
}


/**
  * Check whether a subtree rooted at n contains a node of type V_node and if it does, return it. Note that if n
  * is of type V_node then this function will return n. If no V_node is found then the value NULL is returned.
  */
bool CodeAttributesVisitor::hasChildren(SgNode *n) {
    Rose_STL_Container<SgNode *> nodes = NodeQuery::querySubTree(n, V_SgNode);
    Rose_STL_Container<SgNode *>::iterator i = nodes.begin();
    return (++i /* ++ to skip the root: n */ != nodes.end());
}


/**
 * Check whether or not this node should be visited.  If so, perform any required preprocessing.
 */
bool CodeAttributesVisitor::preVisitEnter(SgNode *node) {
     /**
      *
      */
     if (option.isSyntheticTranslation()) {
         if (! option.isTranslating()) {
             /**
              * For now, process all declarations.  TODO: tighten this so that we only process
              * the declarations on which the loop has a dependence.
              */
             if (isSgInitializedName(node)) {
                 control.SetAttribute(node, Control::LLVM_COST_ANALYSIS);
             }

             /**
              * For the unrolled loop, the cost analysis attribute has already been set.
              */
             if (node -> attributeExists(Control::LLVM_COST_ANALYSIS)) {
                 option.setTranslating();
             }
             else return false;
         }
     }

     /**
      * If traversal has been suspended, return false so that the SgNode is not visited.
      */
     if (visit_suspended_by_node) { // If visiting was suspended, ignore this node
         if (option.isDebugPreTraversal()) {
       //         cerr << "Pre-Skipping node "
       //              << node -> class_name()
       //              << endl;
       //         cerr.flush();
         }

        //
        // O2/25/2015: ROSE Issue... This is no longer the case. TODO: Add special processing for sizeof!!!!
        //
        // 
         /**
          * There is some really bizarre and inconsistent processing of SizeOF in ROSE.
          * I gave up on trying to deal with it and, instead, use this attribute to bypasss
          * these problems!
          *
          * Rose resolves the SgSizeOF expression into an SgValueExp. However, it keeps the original SizeOF
          * expression as a descendant of that SgValueExp. When an SgValue is encountered, the traversal of
          * its subtree is short-circuited.  Thus, if such a node has a SgSizeOfOp as a descendant, we can 
          * only catch it here.  In such a case, we associate the SgSizeOfOp node with the SgValueExp node
          * using the LLVM_SIZEOF_NODE attribute.  After post traversal of an expression node, we check to see
          * if (it is an SgValue and ) it has an LLVM_SIZEOF_NODE attribute.  If so, we complete the processing
          * of the SgValue at that point.
          */

         return false;
     }

    /**
     * Print debugging information, if requested.
     */
     if (option.isDebugPreTraversal()) {
        cerr << "Attribute Visitor Pre-processing: "
             << ((unsigned long) node) << " " << ((unsigned long) node -> get_parent()) << " " 
             << (isSgStatement(node) ? " *Statement* " : "")
             <<  (isSgFunctionDeclaration(node) ? " (***Processing Function " : "")
             <<  (isSgFunctionDeclaration(node) ? isSgFunctionSymbol(isSgFunctionDeclaration(node) -> search_for_symbol_from_symbol_table()) -> get_name().getString() : "")
             <<  (isSgFunctionDeclaration(node) ? ") " : "")
             << node -> class_name() << endl;  // Used for Debugging
        cerr.flush();
     }

     /**
      * Special case for for_increment
      */
     if (node -> attributeExists(Control::LLVM_BUFFERED_OUTPUT)) {
         if (dynamic_cast<ForAstAttribute *>(node -> getAttribute(Control::LLVM_BUFFERED_OUTPUT))) {
             ForAstAttribute *attribute = (ForAstAttribute *) node -> getAttribute(Control::LLVM_BUFFERED_OUTPUT);
             attribute -> saveEndOfBodyLabel(current_function_attribute.top() -> getCurrentLabel());
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getIncrementLabel());
         }
         else ROSE2LLVM_ASSERT(! "This assertion should not have executed.  Only for-increment clauses need be buffered");
     }

     /**
      * Special case for if blocks.
      */
     if (dynamic_cast<SgStatement *>(node)) {
         SgStatement *n = isSgStatement(node);
         if (n -> attributeExists(Control::LLVM_IF_COMPONENT_LABELS)) {
             IfComponentAstAttribute *attribute = (IfComponentAstAttribute *) n -> getAttribute(Control::LLVM_IF_COMPONENT_LABELS);
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getLabel());
         }
     }

     /**
      * Special case for conditional true and false expressions
      */
     if (dynamic_cast<SgExpression *>(node)) {
         SgExpression *n = isSgExpression(node);
         if (n -> attributeExists(Control::LLVM_CONDITIONAL_COMPONENT_LABELS)) {
             ConditionalComponentAstAttribute *attribute = (ConditionalComponentAstAttribute *) n -> getAttribute(Control::LLVM_CONDITIONAL_COMPONENT_LABELS);
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_AND_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_AND_RHS);
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getRhsLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_OR_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_OR_RHS);
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getRhsLabel());
         }
         else if (dynamic_cast<SgFunctionRefExp *>(node)) {
             SgFunctionRefExp *function_ref = isSgFunctionRefExp(node);
             SgFunctionSymbol *function_sym = function_ref -> get_symbol();
             ROSE2LLVM_ASSERT(function_sym);

             //
             // If the function in question was implicitly declared by the compiler, make sure that it "has ellipses"
             // so that LLVM will allow it to be legally invoked. See how function setLLVMType processed SgFunctionType
             // for more detail.
             //
             if (! isSgGlobal(function_sym -> get_scope())) { // this function was declared inside a block?
                 SgFunctionType *function_type = isSgFunctionType(function_sym -> get_type());
                 ROSE2LLVM_ASSERT(function_type);
                 control.SetAttribute(function_type, Control::LLVM_COMPILER_GENERATED);
             }
         }
     }

     return true;
}


/**
 * Pre-visit a node.
 */
void CodeAttributesVisitor::preVisit(SgNode *node) {
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
         SgInitializedName *n = (SgInitializedName *) isSgInitializedName(node);

         if (! n -> attributeExists(Control::LLVM_NAME)) { // not yet processed?
             processVariableDeclaration(n);
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

         if (option.isQuery() && (! node -> attributeExists(Control::LLVM_TRANSLATE))) {
             visit_suspended_by_node = node; // ignore this file
         }
         else {
             sourceFiles.push_back(n); // save list of SgSourceFile(s) associated with this SgProject.
             LLVMAstAttributes *attributes = new LLVMAstAttributes(option, control, FileHelper::getFileName(n->getFileName()));
             setAttributes(attributes);
             control.SetAttribute(n, Control::LLVM_AST_ATTRIBUTES, attributes); // The attributes that will be used for this translation
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
         current_function_attribute.push(new FunctionAstAttribute(NULL)); // Allocate a global environment for the FILE.
     }
     //                 SgBasicBlock
     //                 SgIfStmt
     else if (dynamic_cast<SgIfStmt *> (node)) {
         SgIfStmt *n = isSgIfStmt(node);
         string true_label = attributes -> getTemp(LLVMAstAttributes::TEMP_IF_TRUE),
                false_label = (n -> get_false_body() == NULL
                                                      ? attributes -> getTemp(LLVMAstAttributes::TEMP_IF_END)
                                                      : attributes -> getTemp(LLVMAstAttributes::TEMP_IF_FALSE)),
                end_label = (n -> get_false_body() == NULL ? false_label : attributes -> getTemp(LLVMAstAttributes::TEMP_IF_END));
         IfAstAttribute *attribute = new IfAstAttribute(true_label,
                                                        false_label,
                                                        end_label);
         control.SetAttribute(n, Control::LLVM_IF_LABELS, attribute);
         ROSE2LLVM_ASSERT(isSgExprStatement(n -> get_conditional())); // I am relying on this assumption.
         control.SetAttribute(n -> get_conditional(), Control::LLVM_IF_LABELS, new IfAstAttribute(attribute));
         control.SetAttribute(n -> get_true_body(), Control::LLVM_IF_COMPONENT_LABELS, new IfComponentAstAttribute(true_label, end_label));
         if (n -> get_false_body() != NULL) {
             control.SetAttribute(n -> get_false_body(), Control::LLVM_IF_COMPONENT_LABELS, new IfComponentAstAttribute(false_label, end_label));
         }
     }
     //                 SgForStatement
     else if (dynamic_cast<SgForStatement *> (node)) {
         SgForStatement *n = isSgForStatement(node);

         scopeStack.push(n);
         string condition_label = attributes -> getTemp(LLVMAstAttributes::TEMP_FOR_CONDITION),
                body_label      = attributes -> getTemp(LLVMAstAttributes::TEMP_FOR_BODY),
                increment_label = (isSgNullExpression(n -> get_increment())
                                       ? condition_label
                                       : attributes -> getTemp(LLVMAstAttributes::TEMP_FOR_INCREMENT)),
                end_label       = attributes -> getTemp(LLVMAstAttributes::TEMP_FOR_END);

         ForAstAttribute *attribute = new ForAstAttribute(condition_label, body_label, increment_label, end_label);
         control.SetAttribute(n, Control::LLVM_FOR_LABELS, attribute);
         control.SetAttribute(n -> get_for_init_stmt(), Control::LLVM_FOR_LABELS, new ForAstAttribute(attribute));
         ROSE2LLVM_ASSERT(isSgExprStatement(n -> get_test())); // I am relying on this assumption.
         control.SetAttribute(n -> get_test(), Control::LLVM_FOR_LABELS, new ForAstAttribute(attribute));
         /**
          * If there are increment expressions, buffer their output 
          */
         if (! isSgNullExpression(n -> get_increment())) {
             control.SetAttribute(n -> get_increment(), Control::LLVM_BUFFERED_OUTPUT, new ForAstAttribute(attribute));
         }
     }
     //                 SgFunctionDefinition
     //                 SgClassDefinition:
     //                     SgTemplateInstantiationDefn
     //                 SgWhileStmt
     else if (dynamic_cast<SgWhileStmt *> (node)) {
         SgWhileStmt *n = isSgWhileStmt(node);

         scopeStack.push(n);

         WhileAstAttribute *attribute = new WhileAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_WHILE_CONDITION),
                                                              attributes -> getTemp(LLVMAstAttributes::TEMP_WHILE_BODY),
                                                              attributes -> getTemp(LLVMAstAttributes::TEMP_WHILE_END));
         control.SetAttribute(n, Control::LLVM_WHILE_LABELS, attribute);
         ROSE2LLVM_ASSERT(isSgExprStatement(n -> get_condition())); // I am relying on this assumption.
         control.SetAttribute(n -> get_condition(), Control::LLVM_WHILE_LABELS, new WhileAstAttribute(attribute));
         current_function_attribute.top() -> addLabelPredecessor(attribute -> getConditionLabel());
         current_function_attribute.top() -> resetCurrentLabel(attribute -> getConditionLabel());
     }
     //                 SgDoWhileStmt
     else if (dynamic_cast<SgDoWhileStmt *> (node)) {
         SgDoWhileStmt *n = isSgDoWhileStmt(node);

         scopeStack.push(n);

         DoAstAttribute *attribute = new DoAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_DO_CONDITION),
                                                        attributes -> getTemp(LLVMAstAttributes::TEMP_DO_BODY),
                                                        attributes -> getTemp(LLVMAstAttributes::TEMP_DO_END));
         control.SetAttribute(n, Control::LLVM_DO_LABELS, attribute);
         ROSE2LLVM_ASSERT(isSgExprStatement(n -> get_condition())); // I am relying on this assumption.
         control.SetAttribute(n -> get_condition(), Control::LLVM_DO_LABELS, new DoAstAttribute(attribute));
         current_function_attribute.top() -> addLabelPredecessor(attribute -> getBodyLabel());
         current_function_attribute.top() -> resetCurrentLabel(attribute -> getBodyLabel());
     }
     //                 SgSwitchStatement
     else if (dynamic_cast<SgSwitchStatement *>(node)) {
         SgSwitchStatement *n = isSgSwitchStatement(node);

         scopeStack.push(n);

         switchStack.push(n);

         SgExprStatement *stmt = isSgExprStatement(n -> get_item_selector());
         ROSE2LLVM_ASSERT(stmt); // I am relying on this assumption
         control.SetAttribute(stmt, Control::LLVM_SWITCH_EXPRESSION);
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
     else if (dynamic_cast<SgClassDeclaration *>(node)) {
         SgClassDeclaration *n = isSgClassDeclaration(node);

         switch (n -> get_class_type()) {
             case SgClassDeclaration::e_struct:
             case SgClassDeclaration::e_union:
                 break;
             case SgClassDeclaration::e_class:
                 ROSE2LLVM_ASSERT(! "support for class in C.  Use struct.");
                 break;
             default:
                 ROSE2LLVM_ASSERT(! "know how to process unknown kind of class declaration in C.");
                 break;
         }
         visit_suspended_by_node = node; // nothing more to do
     }
     //                 SgTemplateInstantiationDecl
     //                 SgDerivedTypeStatement
     //                 SgModuleStatement
     //             SgImplicitStatement
     //             SgUsingDeclarationStatement
     //             SgNamelistStatement
     //             SgImportStatement
     //             SgFunctionDeclaration:
     /**
      * Warning!!!
      *
      * SgFunctionDeclaration is a superclass of other AST nodes, to add a test case for any of its
      * subclasses the test case must be nested inside this basic block.
                    SgFunctionDeclaration:
                        SgMemberFunctionDeclaration:
                            SgTemplateInstantiationMemberFunctionDecl
                        SgTemplateInstantiationFunctionDecl
                        SgProgramHeaderStatement
                        SgProcedureHeaderStatement
                        SgEntryStatement
      */
     else if (dynamic_cast<SgFunctionDeclaration *>(node)) {
         SgFunctionDeclaration *n = isSgFunctionDeclaration(node);

         current_function_attribute.push(new FunctionAstAttribute(n));
         control.SetAttribute(n, Control::LLVM_LOCAL_DECLARATIONS, current_function_attribute.top());
         attributes -> setLLVMTypeName(n -> get_type());

         vector<SgInitializedName *> parms = n -> get_args();
         for (int i = 0; i < parms.size(); i++) { // mark all parameter declarations for special processing later
             control.SetAttribute(parms[i], Control::LLVM_PARAMETER);
         }

         SgFunctionSymbol *fsym = isSgFunctionSymbol(n -> search_for_symbol_from_symbol_table());
         ROSE2LLVM_ASSERT(fsym);
         if (option.isQuery() && (! n -> attributeExists(Control::LLVM_TRANSLATE))) {
             visit_suspended_by_node = node; // treat this function as if it was a header
         }
         else if (n -> get_definition()) {
             attributes -> insertDefinedFunction(attributes -> getFunctionSignature(fsym));
             if (option.isQuery()) {
                 option.setSyntheticTranslation();
             }
         }
     }
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
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getConditionLabel());
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getConditionLabel());
         }
     }
     //             SgLabelStatement
     //             SgCaseOptionStmt
     else if (dynamic_cast<SgCaseOptionStmt *>(node)) {
         SgCaseOptionStmt *n = isSgCaseOptionStmt(node);
         SgExpression *key = n -> get_key();
         while (isSgCastExp(key)) {
            key = isSgCastExp(key) -> get_operand();
         }
         ROSE2LLVM_ASSERT(isSgValueExp(key));
         stringstream case_value;
         if (isSgIntVal(key)) {
             case_value << isSgIntVal(key) -> get_value();
         }
         else if (isSgUnsignedIntVal(key)) {
             case_value << isSgUnsignedIntVal(key) -> get_value();
         }
         else if (isSgEnumVal(key)) {
             case_value << isSgEnumVal(key) -> get_value();
         }
         else if (isSgLongIntVal(key)) {
             case_value << isSgLongIntVal(key) -> get_value();
         }
         else if (isSgUnsignedLongVal(key)) {
             case_value << isSgUnsignedLongVal(key) -> get_value();
         }
         else if (isSgLongLongIntVal(key)) {
             case_value << isSgLongLongIntVal(key) -> get_value();
         }
         else if (isSgUnsignedLongLongIntVal(key)) {
             case_value << isSgUnsignedLongLongIntVal(key) -> get_value();
         }
         else ROSE2LLVM_ASSERT(! "know how to process a value of this type in a switch case statement");

         SwitchAstAttribute *switch_attribute = (SwitchAstAttribute *) switchStack.top() -> getAttribute(Control::LLVM_SWITCH_INFO);
         ROSE2LLVM_ASSERT(switch_attribute);
         int last_index = switch_attribute -> numCaseAttributes() - 1;
         bool reused_label = (last_index >= 0 && switch_attribute -> getCaseAttribute(last_index) -> emptyBody());
         string label = (reused_label
                              ? switch_attribute -> getCaseAttribute(last_index) -> getCaseLabel()
                              : attributes -> getTemp(LLVMAstAttributes::TEMP_CASE_LABEL));
         bool empty_body = ((! isSgBasicBlock(n -> get_body())) || isSgBasicBlock(n -> get_body()) -> get_statements().size() == 0);
         CaseAstAttribute *case_attribute = new CaseAstAttribute(case_value.str(), label, empty_body, reused_label);
         switch_attribute -> addCaseAttribute(case_attribute);
         control.SetAttribute(n, Control::LLVM_CASE_INFO, case_attribute);
         if (! reused_label) {
             current_function_attribute.top() -> addLabelPredecessor(case_attribute -> getCaseLabel(), switch_attribute -> getEntryLabel());
             if (switch_attribute -> numCaseAttributes() > 1) { // not the first case
                 current_function_attribute.top() -> addLabelPredecessor(case_attribute -> getCaseLabel());
             }
         }
         current_function_attribute.top() -> resetCurrentLabel(case_attribute -> getCaseLabel());
     }
     //             SgTryStmt
     //             SgDefaultOptionStmt
     else if (dynamic_cast<SgDefaultOptionStmt *>(node)) {
         SgDefaultOptionStmt *n = isSgDefaultOptionStmt(node);
         SwitchAstAttribute *switch_attribute = (SwitchAstAttribute *) switchStack.top() -> getAttribute(Control::LLVM_SWITCH_INFO);
         switch_attribute -> setDefaultStmt(n);
         string default_label = attributes -> getTemp(LLVMAstAttributes::TEMP_DEFAULT_LABEL);
         control.SetAttribute(n, Control::LLVM_DEFAULT_LABEL, new StringAstAttribute(default_label));
         current_function_attribute.top() -> addLabelPredecessor(default_label, switch_attribute -> getEntryLabel());
         if (switch_attribute -> numCaseAttributes() > 0) { // not the first case
             current_function_attribute.top() -> addLabelPredecessor(default_label);
         }
         current_function_attribute.top() -> resetCurrentLabel(default_label);
     }
     //             SgBreakStmt
     //             SgContinueStmt
     //             SgReturnStmt
     else if (dynamic_cast<SgReturnStmt *>(node)) {
         SgReturnStmt *n = isSgReturnStmt(node);

         if (n -> get_expression()) {
             SgType *return_type = current_function_attribute.top() -> getFunctionType() -> get_return_type();
             SgPointerType *pointer_type = isSgPointerType(attributes -> getSourceType(return_type));
             if (pointer_type &&
                 ((isSgIntVal(n -> get_expression()) &&
                   isSgIntVal(n -> get_expression()) -> get_value() == 0) ||
                  (isSgEnumVal(n -> get_expression()) &&
                   isSgEnumVal(n -> get_expression()) -> get_value() == 0))) {
                 control.SetAttribute(n -> get_expression(), Control::LLVM_TYPE, new StringAstAttribute("void *"));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_NAME, new StringAstAttribute("null"));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute("null"));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(return_type));
                 visit_suspended_by_node = node; // nothing more to do
             }
         }
     }
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
     /**
      * Warning!!!
      *
      * SgExpression is a superclass of other AST nodes, to add a test case for any of its
      * subclasses the test case must be nested inside this basic block.
      */
     //         SgExpression:
     else if (dynamic_cast<SgExpression *>(node)) {
         SgExpression *n = isSgExpression(node);
         attributes -> setLLVMTypeName(n -> get_type());
         control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(n -> get_type()));

         if (false)
            ;
     //*            SgUnaryOp:
     //*                SgExpressionRoot
     //*                SgMinusOp
     //*                SgUnaryAddOp
     //*                SgNotOp
     //*                SgPointerDerefExp
     //*                SgAddressOfOp
         else if (dynamic_cast<SgAddressOfOp *>(node)) {
             SgAddressOfOp *n = isSgAddressOfOp(node);
             control.SetAttribute(n -> get_operand(), Control::LLVM_REFERENCE_ONLY);
             // In the case that the operand is a reference to an array
             // variable whose first dimension is taken from its
             // initializer, roseToLLVM previously left the dimension as
             // 1, so we now need to recompute it based on the
             // initializer.
             SgVarRefExp *var_operand = isSgVarRefExp(n->get_operand());
             if (var_operand) {
                 SgArrayType *array_type = isSgArrayType(attributes->getSourceType(n->get_operand()->get_type()));
                 if (array_type) {
                     SgIntVal *specified_size = isSgIntVal(array_type -> get_index());
                     if (!specified_size) {
                         SgExpression *init = var_operand->get_symbol()->get_declaration()->get_initializer();
                         size_t size;
                         if (isSgAggregateInitializer(init)) {
                             size = isSgAggregateInitializer(init)->get_initializers()->get_expressions().size();
                         }
                         else {
                             SgAssignInitializer *assign_init = isSgAssignInitializer(init);
                             ROSE2LLVM_ASSERT(assign_init);
                             SgStringVal *str = isSgStringVal(assign_init->get_operand());
                             ROSE2LLVM_ASSERT(str);
                             size = str->get_value().size() + 1;
                         }
                         // To be sure to get new nodes that we can
                         // later free, don't use SageBuilder for types.
                         SgExpression *expr = control.ownNode(SageBuilder::buildIntVal(size));
                         SgArrayType *base_type = control.ownNode(new SgArrayType(array_type->get_base_type(), expr));
                         SgType *type = control.ownNode(new SgPointerType(base_type));
                         control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(type));
                         attributes->setLLVMTypeName(type);
                     }
                 }
             }
         }
     //*                SgMinusMinusOp
     //*                SgPlusPlusOp
     //*                SgBitComplementOp
     //*                SgCastExp
         else if (dynamic_cast<SgCastExp *>(node)) {
             SgCastExp *n = isSgCastExp(node);
             SgType *type = n -> get_type();
             SgExpression *operand = n -> get_operand();

             //
             // For character types, compute the integer value of the character in question.
             //
             if (isSgCharVal(operand)) {
                 stringstream out;
                 out << ((int) isSgCharVal(operand) -> get_value());
                 control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(out.str()));
                 control.SetAttribute(operand, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(out.str()));
                 control.SetAttribute(operand, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(type));
                 control.SetAttribute(operand, Control::LLVM_NAME, new StringAstAttribute(out.str()));
                 visit_suspended_by_node = node;
             }
             else if (isSgUnsignedCharVal(operand)) {
                 stringstream out;
                 out << ((int) isSgUnsignedCharVal(operand) -> get_value());
                 control.SetAttribute(operand, Control::LLVM_NAME, new StringAstAttribute(out.str()));
                 control.SetAttribute(operand, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(out.str()));
                 control.SetAttribute(operand, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(type));
                 control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(out.str()));
                 visit_suspended_by_node = node;
             }
             //
             // Check if NULL Value 
             //
             else if ((isSgTypeVoid(attributes -> getSourceType(type)) || isSgPointerType(attributes -> getSourceType(type)) || isSgArrayType(type)) &&
                      ((isSgIntVal(operand) && isSgIntVal(operand) -> get_value() == 0)) ||
                       (isSgEnumVal(operand) && isSgEnumVal(operand) -> get_value() == 0)) {
                 control.SetAttribute(operand, Control::LLVM_NAME, new StringAstAttribute("null"));
                 control.SetAttribute(operand, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute("null"));
                 control.SetAttribute(operand, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(type));
                 control.SetAttribute(n, Control::LLVM_NULL_VALUE);
                 control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute("null"));
                 visit_suspended_by_node = node;
             }
             /**
              * There is a "feature" in ROSE whereby it sometimes generates two operand subtrees for a cast expression.
              * In such a case, we mark the compiler-generated operand subtree so that it will be ignored by the visitor.
              */ 
             else if (isSgCastExp(n -> get_parent()) && (n != isSgCastExp(n -> get_parent()) -> get_operand())) {
                 control.SetAttribute(n, Control::LLVM_IGNORE); // Ignore the original tree so that duplicate code is not generated from it.
                 visit_suspended_by_node = node;
             }
         }
     //*                SgThrowOp
     //*                SgRealPartOp
     //*                SgImagPartOp
     //*                SgConjugateOp
     //*                SgUserDefinedUnaryOp
     //*            SgBinaryOp:
     //*                SgArrowExp
         else if (dynamic_cast<SgArrowExp *>(node)) {
             SgArrowExp *n = isSgArrowExp(node);
             if (dynamic_cast<SgVarRefExp *>(n -> get_rhs_operand())) {
                 SgVarRefExp *var_ref = isSgVarRefExp(n -> get_rhs_operand());
                 checkVariableDeclaration(var_ref);
                 SgType *lhs_type = n -> get_lhs_operand() -> get_type();
                 SgClassType *class_type = isSgClassType(attributes -> getSourceType(isSgPointerType(attributes -> getSourceType(lhs_type)) -> get_base_type()));
                 ROSE2LLVM_ASSERT(class_type);
                 SgClassDeclaration *decl= isSgClassDeclaration(class_type -> get_declaration());
                 attributes -> setLLVMTypeName(lhs_type); // process class type of lhs operand
                 DeclarationsAstAttribute *class_attr = attributes -> class_map[class_type -> get_qualified_name().getString()]; // needed because of Rose bug.
                 ROSE2LLVM_ASSERT(class_attr);
                 SgVariableSymbol *sym = var_ref -> get_symbol();
                 string var_name = ((StringAstAttribute *) sym -> get_declaration() -> getAttribute(Control::LLVM_NAME)) -> getValue();
                 int index = class_attr -> nameIndex(var_name);
                 control.SetAttribute(var_ref, Control::LLVM_CLASS_MEMBER, new IntAstAttribute(index));
             }
             else {
                 cerr << n -> get_rhs_operand() -> class_name() << " is not allowed in an SgArrowExp in C" << endl;
                 cerr.flush();
                 ROSE2LLVM_ASSERT(0);
             }
         }
     //*                SgDotExp
         else if (dynamic_cast<SgDotExp *>(node)) {
             SgDotExp *n = isSgDotExp(node);
             control.SetAttribute(n -> get_lhs_operand(), Control::LLVM_REFERENCE_ONLY);
             if (dynamic_cast<SgVarRefExp *>(n -> get_rhs_operand())) {
                 SgVarRefExp *var_ref = isSgVarRefExp(n -> get_rhs_operand());
                 // FIXME: Why is checkVariableDeclaration needed?
                 // Doesn't the setLLVMTypeName below always set
                 // LLVM_NAME on all fields of the class/union?
                 checkVariableDeclaration(var_ref);
                 SgType *lhs_type = n -> get_lhs_operand() -> get_type();
                 SgClassType *class_type = isSgClassType(attributes -> getSourceType(lhs_type));
                 ROSE2LLVM_ASSERT(class_type);
                 SgClassDeclaration *decl= isSgClassDeclaration(class_type -> get_declaration());
                 attributes -> setLLVMTypeName(lhs_type); // process class type of lhs operand
                 DeclarationsAstAttribute *class_attr = attributes -> class_map[class_type -> get_qualified_name().getString()]; // needed because of Rose bug.
                 ROSE2LLVM_ASSERT(class_attr);
                 SgVariableSymbol *sym = var_ref -> get_symbol();
                 string var_name = ((StringAstAttribute *) sym -> get_declaration() -> getAttribute(Control::LLVM_NAME)) -> getValue();
                 int index = class_attr -> nameIndex(var_name);
                 control.SetAttribute(var_ref, Control::LLVM_CLASS_MEMBER, new IntAstAttribute(index));
             }
             else { // not supposed to happen in C.
                 cerr << n -> get_rhs_operand() -> class_name() << " is not allowed in an SgDotExp in C" << endl;
                 cerr.flush();
                 ROSE2LLVM_ASSERT(0);
             }
         }
     //*                SgDotStarOp
     //*                SgArrowStarOp
     //*                SgEqualityOp
     //*                SgLessThanOp
     //*                SgGreaterThanOp
     //*                SgNotEqualOp
     //*                SgLessOrEqualOp
     //*                SgGreaterOrEqualOp
     //*                SgAddOp
     //*                SgSubtractOp
     //*                SgMultiplyOp
     //*                SgDivideOp
     //*                SgIntegerDivideOp
     //*                SgModOp
     //*                SgAndOp
         else if (dynamic_cast<SgAndOp *>(node)) {
             SgAndOp *n = isSgAndOp(node);
             string rhs_label = attributes -> getTemp(LLVMAstAttributes::TEMP_LOGICAL_AND_RHS),
                    end_label = attributes -> getTemp(LLVMAstAttributes::TEMP_LOGICAL_AND_END);
             control.SetAttribute(n -> get_lhs_operand(), Control::LLVM_LOGICAL_AND_LHS, new LogicalAstAttribute(rhs_label, end_label));
             control.SetAttribute(n -> get_rhs_operand(), Control::LLVM_LOGICAL_AND_RHS, new LogicalAstAttribute(rhs_label, end_label));
         }
     //*                SgOrOp
         else if (dynamic_cast<SgOrOp *>(node)) {
             SgOrOp *n = isSgOrOp(node);
             string rhs_label = attributes -> getTemp(LLVMAstAttributes::TEMP_LOGICAL_OR_RHS),
                    end_label = attributes -> getTemp(LLVMAstAttributes::TEMP_LOGICAL_OR_END);
             control.SetAttribute(n -> get_lhs_operand(), Control::LLVM_LOGICAL_OR_LHS, new LogicalAstAttribute(rhs_label, end_label));
             control.SetAttribute(n -> get_rhs_operand(), Control::LLVM_LOGICAL_OR_RHS, new LogicalAstAttribute(rhs_label, end_label));
         }
     //*                SgBitXorOp
     //*                SgBitAndOp
     //*                SgBitOrOp
     //*                SgCommaOpExp
     //*                SgLshiftOp
     //*                SgRshiftOp
     //*                SgPntrArrRefExp
     //*                SgScopeOp
     //*                SgAssignOp
         else if (dynamic_cast<SgAssignOp *>(node)) {
             SgAssignOp *n = isSgAssignOp(node);
             /**
              * When assigning a source structure to a target structure, the assignment is performed
              * with a call to the memcopy function. Thus, we need to load the address of the left-hand
              * side of the assignment as well as the address of the right-hand side so that we can pass
              * them as source and target arguments to the copy function.
              *
              * All other left-hand side as marked as reference only to prevent their value from being
              * loaded.
              */
             if (isSgClassType(attributes -> getSourceType(n -> get_type()))) {
                 attributes -> setNeedsMemcopy();
             }
             else {
                 control.SetAttribute(n -> get_lhs_operand(), Control::LLVM_REFERENCE_ONLY);
             }
         }
     //*                SgPlusAssignOp
     //*                SgMinusAssignOp
     //*                SgAndAssignOp
     //*                SgIorAssignOp
     //*                SgMultAssignOp
     //*                SgDivAssignOp
     //*                SgModAssignOp
     //*                SgXorAssignOp
     //*                SgLshiftAssignOp
     //*                SgRshiftAssignOp
     //*                SgExponentiationOp
     //*                SgConcatenationOp
     //*                SgPointerAssignOp
     //*                SgUserDefinedBinaryOp
     //*            SgExprListExp
         else if (dynamic_cast<SgExprListExp *>(node)) {
             SgExprListExp *n = isSgExprListExp(node);
         }
     //*            SgVarRefExp
         else if (dynamic_cast<SgVarRefExp *>(node)) {
             SgVarRefExp *n = isSgVarRefExp(node);
             checkVariableDeclaration(n);
             if (isSgArrayType(n->get_type())) {
                 // To be sure to get new nodes that we can later free,
                 // don't use SageBuilder for types.
                 SgType *array_ref_type = control.ownNode(new SgPointerType(isSgArrayType(n->get_type())->get_base_type()));
                 control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_TYPE, new SgTypeAstAttribute(array_ref_type));
                 attributes->setLLVMTypeName(array_ref_type);
             }
         }
     //*            SgClassNameRefExp
     //*            SgFunctionRefExp
         else if (dynamic_cast<SgFunctionRefExp *>(node)) {
             SgFunctionRefExp *n = isSgFunctionRefExp(node);
             checkFunctionDeclaration(n);
         }
     //*            SgMemberFunctionRefExp
     /**
      * Warning!!!
      *
      * SgValueExp is a superclass of other AST nodes, to add a test case for any of its
      * subclasses the test case must be nested inside this basic block.
      */
     //*            SgValueExp:
         else if (dynamic_cast<SgValueExp *> (node)) {
             SgValueExp *n = isSgValueExp(node);
             stringstream out;
             if (false)
                 ;
     //*                SgBoolValExp
             else if (dynamic_cast<SgBoolValExp *>(n)) {
                 out << isSgBoolValExp(n) -> get_value();
             }
     //*                SgStringVal
             /**
              * If a string constant is used for initialization, do not create a global constant declaration for it.
              */
             else if (dynamic_cast<SgStringVal *>(n)) {
                 if (n -> getAttribute(Control::LLVM_STRING_INITIALIZATION)) {
                     out << isSgStringVal(n) -> get_value(); // TODO: Set to null string to save space ?
                 }
                 else {
                     IntAstAttribute *string_size_attribute = (IntAstAttribute *) n -> getAttribute(Control::LLVM_STRING_SIZE);
                     int index = (string_size_attribute
                                        ? attributes -> insertString(isSgStringVal(n) -> get_value(), string_size_attribute -> getValue())
                                        : attributes -> insertString(isSgStringVal(n) -> get_value()));
                     out << attributes -> getGlobalStringReference(index);
                 }
             }

     //*                SgShortVal
             else if (dynamic_cast<SgShortVal *>(n)) {
                 out << isSgShortVal(n) -> get_value();
             }
     //*                SgCharVal
             else if (dynamic_cast<SgCharVal *>(n)) {
                 out << isSgCharVal(n) -> get_value();
             }
     //*                SgUnsignedCharVal
             else if (dynamic_cast<SgUnsignedCharVal *>(n)) {
                 out << isSgUnsignedCharVal(n) -> get_value();
             }
     //*                SgWcharVal
     //*                SgUnsignedShortVal
             else if (dynamic_cast<SgUnsignedShortVal *>(n)) {
                 out << isSgUnsignedShortVal(n) -> get_value();
             }
     //*                SgIntVal
             else if (dynamic_cast<SgIntVal *>(n)) {
                 out << isSgIntVal(n) -> get_value();
             }
     //*                SgEnumVal
             else if (dynamic_cast<SgEnumVal *>(n)) {
                 out << isSgEnumVal(n) -> get_value();
             }
     //*                SgUnsignedIntVal
             else if (dynamic_cast<SgUnsignedIntVal *>(n)) {
                 out << isSgUnsignedIntVal(n) -> get_value();
             }
     //*                SgLongIntVal
             else if (dynamic_cast<SgLongIntVal *>(n)) {
                 out << isSgLongIntVal(n) -> get_value();
             }
     //*                SgLongLongIntVal
             else if (dynamic_cast<SgLongLongIntVal *>(n)) {
                 out << isSgLongLongIntVal(n) -> get_value();
             }
     //*                SgUnsignedLongLongIntVal
             else if (dynamic_cast<SgUnsignedLongLongIntVal *>(n)) {
                 out << isSgUnsignedLongLongIntVal(n) -> get_value();
             }
     //*                SgUnsignedLongVal
             else if (dynamic_cast<SgUnsignedLongVal *>(n)) {
                  out << isSgUnsignedLongVal(n) -> get_value();
             }
     //*                SgFloatVal
             else if (dynamic_cast<SgFloatVal *>(n)) {
                 out << Control::FloatToString(isSgFloatVal(n) -> get_value());
             }
     //*                SgDoubleVal
             else if (dynamic_cast<SgDoubleVal *>(n)) {
                 out << Control::FloatToString(isSgDoubleVal(n) -> get_value());
             }
     //*                SgLongDoubleVal
             else if (dynamic_cast<SgLongDoubleVal *>(n)) {
                 out << scientific << isSgLongDoubleVal(n) -> get_value();
                 ROSE2LLVM_ASSERT(! "yet know how to process long double constants");
             }
     //*                SgComplexVal
     //*                SgUpcThreads
     //*                SgUpcMythread

             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(out.str()));

             visit_suspended_by_node = node; // Do not bother visiting children of constant values
         } //* else if (dynamic_cast<SgValueExp *> (n)) { ...

     //*                SgFunctionCallExp
         else if (dynamic_cast<SgFunctionCallExp *>(node)) {
             SgFunctionCallExp *n = isSgFunctionCallExp(node);

             SgPointerDerefExp *function_pointer_deref = isSgPointerDerefExp(n -> get_function());
             if (function_pointer_deref) {
                 control.SetAttribute(function_pointer_deref, Control::LLVM_REFERENCE_ONLY);
             }
         }
     //*                SgSizeOfOp
         else if (dynamic_cast<SgSizeOfOp *>(node)) {
             SgSizeOfOp *n = isSgSizeOfOp(node);
             SgType *type_operand = n -> get_operand_type();
             SgExpression *expr_operand = n -> get_operand_expr();
             if (expr_operand) {
                 ROSE2LLVM_ASSERT(! type_operand);
                 SgVarRefExp *var_ref = isSgVarRefExp(expr_operand);
                 if (var_ref) { // in case we are dealing with an variable that gets its size from an aggregate initializer
                     SgVariableSymbol *sym = var_ref -> get_symbol();
                     ROSE2LLVM_ASSERT(sym);
                     SgInitializedName *decl = sym -> get_declaration();
                     ROSE2LLVM_ASSERT(decl);
                     SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) decl -> getAttribute(Control::LLVM_DECLARATION_TYPE);
                     ROSE2LLVM_ASSERT(type_attribute);
                     type_operand = type_attribute -> getType();
                 }
                 else {
                     type_operand = expr_operand -> get_type();
                 }
             }
             else ROSE2LLVM_ASSERT(type_operand);

             int size;
             if (isSgStringVal(expr_operand)) {
                 size = isSgStringVal(expr_operand) -> get_value().size();
             }
             else {
                 attributes -> setLLVMTypeName(type_operand); // Make sure this type has been preprocessed.
                 IntAstAttribute *size_attribute = (IntAstAttribute *) type_operand -> getAttribute(Control::LLVM_SIZE);
                 size = size_attribute -> getValue();
             }
             control.SetAttribute(n, Control::LLVM_SIZE, new IntAstAttribute(size));
             stringstream out;
             out << size;
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(out.str()));
             visit_suspended_by_node = node; // Do not bother visiting children of SizeOfOp node - constant.
         }
     //*                SgUpcLocalsizeof
     //*                SgUpcBlocksizeof
     //*                SgUpcElemsizeof
     //*            SgTypeIdOp
     //*            SgConditionalExp
         else if (dynamic_cast<SgConditionalExp *>(node)) {
             SgConditionalExp *n = isSgConditionalExp(node);
             /**
              * Check whether or not we can use a select instruction for this conditional expression or we
              * need a Phi function. 
              */
             if ((isSgVarRefExp(n -> get_true_exp())  || Control::isPrimitiveValue(n -> get_true_exp())) &&
                 (isSgVarRefExp(n -> get_false_exp()) || Control::isPrimitiveValue(n -> get_false_exp()))) {
                 control.SetAttribute(n, Control::LLVM_SELECT_CONDITIONAL);
             }
             else {
                 ConditionalAstAttribute *attribute = new ConditionalAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_COND_TRUE),
                                                                                  attributes -> getTemp(LLVMAstAttributes::TEMP_COND_FALSE),
                                                                                  attributes -> getTemp(LLVMAstAttributes::TEMP_COND_END));
                 control.SetAttribute(n, Control::LLVM_CONDITIONAL_LABELS, attribute);
                 control.SetAttribute(n -> get_conditional_exp(), Control::LLVM_CONDITIONAL_TEST, new ConditionalAstAttribute(attribute));
                 control.SetAttribute(n -> get_true_exp(),
                                      Control::LLVM_CONDITIONAL_COMPONENT_LABELS, new ConditionalComponentAstAttribute(attribute -> getTrueLabel(), attribute -> getEndLabel()));
                 control.SetAttribute(n -> get_false_exp(),
                                      Control::LLVM_CONDITIONAL_COMPONENT_LABELS, new ConditionalComponentAstAttribute(attribute -> getFalseLabel(), attribute -> getEndLabel()));
             }
         }
     //*            SgNewExp
     //*            SgDeleteExp
     //*            SgThisExp
     //*            SgRefExp
     //*            SgInitializer:
     //*                SgAggregateInitializer
     //*                SgConstructorInitializer
     //*                SgAssignInitializer
     //*                SgDesignatedInitializer
     //*            SgVarArgStartOp
     //*            SgVarArgOp
     //*            SgVarArgEndOp
     //*            SgVarArgCopyOp
     //*            SgVarArgStartOneOperandOp
     //*            SgNullExpression
     //*            SgVariantExpression
     //*            SgSubscriptExpression
     //*            SgColonShapeExp
     //*            SgAsteriskShapeExp
     //*            SgImpliedDo
     //*            SgIOItemExpression
     //*            SgStatementExpression
     //*            SgAsmOp
     //*            SgLabelRefExp
     //*            SgActualArgumentExpression
     //*            SgUnknownArrayOrFunctionReference
     } // else if (dynamic_cast<SgExpression *>(node)) { ...
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


/**
 * Nothing to do on exit of a pre-visit
 */
void CodeAttributesVisitor::preVisitExit(SgNode *node) {}


/**
 * When a boolean expression is used where an arithmetic expression is expected, we need a temporary
 * in order to convert the boolean (extend 1 bit) into an integer (into 32 bits).
 */
void CodeAttributesVisitor::addBooleanExtensionAttributeIfNeeded(SgExpression *n) {
    if (n -> attributeExists(Control::LLVM_IS_BOOLEAN)) {
        string extend_name = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);
        control.SetAttribute(n, Control::LLVM_EXTEND_BOOLEAN, new StringAstAttribute(extend_name));
        StringAstAttribute *result_attribute = (StringAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
        ROSE2LLVM_ASSERT(result_attribute);
        result_attribute -> resetValue(extend_name);
    }
}

/**
 * These attributes warn the code generator when the operands of an
 * "op=" are of incompatible types.  In such a case we need to convert
 * the arguments to the proper type in order to perform the operation
 * and demote (truncate) the result afterward before storing it.
 */
void CodeAttributesVisitor::addConversionAttributeIfNeeded(SgBinaryOp *n) {
    SgExpression *lhs_operand = n -> get_lhs_operand(),
                 *rhs_operand = n -> get_rhs_operand();
    SgType *lhs_type = attributes -> getSourceType(attributes -> getExpressionType(lhs_operand)),
           *rhs_type = attributes -> getSourceType(attributes -> getExpressionType(rhs_operand));

    if ((lhs_type -> isIntegerType() || isSgEnumType(lhs_type)) && (rhs_type -> isIntegerType() || isSgEnumType(rhs_type))) {
        /**
         * The left-hand side operand needs to be promoted?
         */
        if (((IntAstAttribute *) lhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue() <
            ((IntAstAttribute *) rhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()) {
            control.SetAttribute(lhs_operand, Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT)));
            string demote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
            control.SetAttribute(lhs_operand, Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION, new StringAstAttribute(demote_name));
            control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(demote_name));
        }
        /**
         * The right-hand side operand needs to be promoted?
         */
        else if (((IntAstAttribute *) lhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue() >
                 ((IntAstAttribute *) rhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()) {
            string promote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
            control.SetAttribute(rhs_operand, Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION, new StringAstAttribute(promote_name));
            control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(promote_name));
        }
    }

    /**
     * The left-hand side operand needs to be promoted to a floating-point type?
     */
    if ((lhs_type -> isIntegerType() || isSgEnumType(lhs_type)) && rhs_type -> isFloatType()) {
        control.SetAttribute(lhs_operand, Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_PROMOTION, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT)));
        string demote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
        control.SetAttribute(lhs_operand, Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_DEMOTION, new StringAstAttribute(demote_name));
        control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(demote_name));
    }
    /**
     * The left-hand side operand needs to be promoted to a larger floating-point type?
     */
    if (lhs_type -> isFloatType() && rhs_type -> isFloatType()
        && (((IntAstAttribute *) lhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue() <
            ((IntAstAttribute *) rhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue())) {
        control.SetAttribute(lhs_operand, Control::LLVM_OP_AND_ASSIGN_FP_PROMOTION, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT)));
        string demote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
        control.SetAttribute(lhs_operand, Control::LLVM_OP_AND_ASSIGN_FP_DEMOTION, new StringAstAttribute(demote_name));
        control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(demote_name));
    }
}

/**
 * TODO: Remove this function when these bugs are fixed. 
 */
void CodeAttributesVisitor::checkIntegralOperation(SgBinaryOp *n) {
    SgExpression *lhs_operand = n -> get_lhs_operand(),
                 *rhs_operand = n -> get_rhs_operand();
    SgType *result_type = attributes -> getSourceType(attributes -> getExpressionType(n)),
           *lhs_type = attributes -> getSourceType(attributes -> getExpressionType(lhs_operand)),
           *rhs_type = attributes -> getSourceType(attributes -> getExpressionType(rhs_operand));

    /**
     *
     */
    if ((lhs_type -> isIntegerType() || isSgEnumType(lhs_type)) &&
        (rhs_type -> isIntegerType() || isSgEnumType(rhs_type))) {
        /**
         * If the left-hand side is a pointer difference operation then its result is an integer.
         * If the right-hand side of the operation is also an integer then the result should be an integer.
         */
        if (isSgPointerType(result_type)) {
            ROSE2LLVM_ASSERT(((IntAstAttribute *) lhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue() ==
                             ((IntAstAttribute *) rhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue());
            SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
            type_attribute -> resetType(lhs_type);
        }
        else {
            int result_size = ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_SIZE)) -> getValue(),
                left_size = ((IntAstAttribute *) lhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue(),
                right_size = ((IntAstAttribute *) rhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();

            if (isSgLshiftOp(n) || isSgRshiftOp(n)) {
                ROSE2LLVM_ASSERT(left_size == result_size);

                if (right_size < result_size) {
                    string promote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                    control.SetAttribute(rhs_operand, Control::LLVM_INTEGRAL_PROMOTION, new StringAstAttribute(promote_name));
                    StringAstAttribute *rhs_result_attribute = (StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                    ROSE2LLVM_ASSERT(rhs_result_attribute);
                    rhs_result_attribute -> resetValue(promote_name);
                    SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
                    type_attribute -> resetType(lhs_type);
                }
                else if (right_size > result_size) {
                    string demote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                    control.SetAttribute(rhs_operand, Control::LLVM_INTEGRAL_DEMOTION, new StringAstAttribute(demote_name));
                    StringAstAttribute *rhs_result_attribute = (StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                    ROSE2LLVM_ASSERT(rhs_result_attribute);
                    rhs_result_attribute -> resetValue(demote_name);
                    SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
                    type_attribute -> resetType(lhs_type);
                }
            }
            else if (isSgAddOp(n) ||
                     isSgBitAndOp(n) ||
                     isSgBitOrOp(n) ||
                     isSgBitXorOp(n) ||
                     isSgDivideOp(n) ||
                     isSgModOp(n) ||
                     isSgMultiplyOp(n) ||
                     isSgSubtractOp(n)) {
                if (left_size != result_size || right_size != result_size) {
                    //
                    // TODO: Review this at some later date. 09/12/2016
                    //
                    // This patch is needed because of a bug we found in Rose. For the code:
                    // 
                    //            if (sizeof(void *) + 8)
                    // 
                    // Rose tags the left subexpression "sizeof(void *)" as well as the expression "(sizeof(void *) + 8)"
                    // as having the type SgTypeUnsignedInt.  However, it nonetheless adds a cast operator for the 
                    // right subexpression consisting of the integer constant 8 into the type SgTypeLong.
                    //
                    if (left_size < result_size) {
                        string promote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                        control.SetAttribute(lhs_operand, Control::LLVM_INTEGRAL_PROMOTION, new StringAstAttribute(promote_name));
                        StringAstAttribute *lhs_result_attribute = (StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                        ROSE2LLVM_ASSERT(lhs_result_attribute);
                        lhs_result_attribute -> resetValue(promote_name);
                        SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
                        type_attribute -> resetType(result_type);
                    }
                    else if (result_size < left_size) {
                        string demote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                        control.SetAttribute(lhs_operand, Control::LLVM_INTEGRAL_DEMOTION, new StringAstAttribute(demote_name));
                        StringAstAttribute *lhs_result_attribute = (StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                        ROSE2LLVM_ASSERT(lhs_result_attribute);
                        lhs_result_attribute -> resetValue(demote_name);
                        SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
                        type_attribute -> resetType(result_type);
                    }

                    if (right_size < result_size) {
                        string promote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                        control.SetAttribute(rhs_operand, Control::LLVM_INTEGRAL_PROMOTION, new StringAstAttribute(promote_name));
                        StringAstAttribute *rhs_result_attribute = (StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                        ROSE2LLVM_ASSERT(rhs_result_attribute);
                        rhs_result_attribute -> resetValue(promote_name);
                        SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
                        type_attribute -> resetType(result_type);
                    }
                    else if (result_size < right_size) {
                        string demote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                        control.SetAttribute(rhs_operand, Control::LLVM_INTEGRAL_DEMOTION, new StringAstAttribute(demote_name));
                        StringAstAttribute *rhs_result_attribute = (StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                        ROSE2LLVM_ASSERT(rhs_result_attribute);
                        rhs_result_attribute -> resetValue(demote_name);
                        SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
                        type_attribute -> resetType(result_type);
                    }
                }
            }
            else if (isSgEqualityOp(n) ||
                     isSgNotEqualOp(n) ||
                     isSgGreaterOrEqualOp(n) ||
                     isSgGreaterThanOp(n) ||
                     isSgLessOrEqualOp(n) ||
                     isSgLessThanOp(n)) {
                if (left_size != right_size) {
                    //
                    // TODO: Review this at some later date. 09/12/2016
                    //
                    // This patch is needed because of a bug we found in Rose. For the code:
                    // 
                    //            if (sizeof(void *) == 8)
                    // 
                    // Rose tags the left subexpression "sizeof(void *)" as well as the expression "(sizeof(void *) + 8)"
                    // as having the type SgTypeUnsignedInt.  However, it nonetheless adds a cast operator for the 
                    // right subexpression consisting of the integer constant 8 into the type SgTypeLong.
                    //
                    if (left_size < right_size) {
                        string promote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                        control.SetAttribute(lhs_operand, Control::LLVM_INTEGRAL_PROMOTION, new StringAstAttribute(promote_name));
                        StringAstAttribute *lhs_result_attribute = (StringAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                        ROSE2LLVM_ASSERT(lhs_result_attribute);
                        lhs_result_attribute -> resetValue(promote_name);
                        SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) lhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
                        type_attribute -> resetType(rhs_type);
                    }
                    else { // (right_size < left_size)
                        string promote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                        control.SetAttribute(rhs_operand, Control::LLVM_INTEGRAL_PROMOTION, new StringAstAttribute(promote_name));
                        StringAstAttribute *rhs_result_attribute = (StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                        ROSE2LLVM_ASSERT(rhs_result_attribute);
                        rhs_result_attribute -> resetValue(promote_name);
                        SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
                        type_attribute -> resetType(lhs_type);
                    }
                }
            }
        }
    }
    else {
    // TODO: what to do for -(p -q) which gets converted to (null - (p - q))
    }
}


/**
 * Check whether or not this node is to be post-visited.
 */
bool CodeAttributesVisitor::postVisitEnter(SgNode *node) {
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
         cerr << "Attribute Visitor Post-processing: "
              << ((unsigned long) node) << " " << ((unsigned long) node -> get_parent()) << " " 
              << (isSgStatement(node) ? " *Statement* " : "")
              << node -> class_name() << endl;  // Used for Debugging
         cerr.flush();
     }

     /**
      * Check for suspension of visit and take apropriate action - If the node in question had requested
      * the suspension then we resume traversal as this point.  Otherwise, we return false to indicate to
      * the visitor that visitation is still suspended.
      */
     if (visit_suspended_by_node) { 
         if (visit_suspended_by_node == node) // If visiting was suspended by this node, resume visiting.
             visit_suspended_by_node = NULL;
         else {
   //             cerr << "Post-Skipping node "
   //                  << node -> class_name()
   //                  << endl;
   //             cerr.flush();
             return false;
         }
     }

     return true;
}


/**
 * Post-visit this node.
 */
void CodeAttributesVisitor::postVisit(SgNode *node) {
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

         /**
          * If this declaration had an initializer and it was not already processed, update it with relevant information.
          */
         if (n -> get_initializer()) {
             SgType *type = ((SgTypeAstAttribute *) n -> getAttribute(Control::LLVM_DECLARATION_TYPE)) -> getType();
             string type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();

             /**
              * Rose does not always compute aggregate types correctly.  In particular, if the definition of a type
              * depends on an aggregate declaration then it it not processed correctly. For example, 
              *
              *     char name[] = "Joe";
              */
             SgArrayType *array_type = isSgArrayType(attributes -> getSourceType(type));

             if (dynamic_cast<SgAggregateInitializer *>(n -> get_initializer())) {
                 SgAggregateInitializer *aggregate_init = isSgAggregateInitializer(n -> get_initializer());
                 control.SetAttribute(aggregate_init, Control::LLVM_BIT_CAST, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
                 control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(aggregate_init));
             }
             else 
            /**
             * Case 1:An aggregate initialization of an aggregate to variable that is of array type without a specified size.
             *
             * Case 2: An assign initialization of a string to a variable that is of array type
             *
             * Case 3: Everything else.
             */
            if (dynamic_cast<SgAssignInitializer *>(n -> get_initializer())) {
                 SgAssignInitializer *assign_initializer = isSgAssignInitializer(n -> get_initializer());
                 if (isSgStringVal(assign_initializer -> get_operand()) && array_type) {
                     SgExpression *init_expr = assign_initializer -> get_operand();
                     ROSE2LLVM_ASSERT(init_expr);
                     int string_size = ((IntAstAttribute *) init_expr -> getAttribute(Control::LLVM_STRING_SIZE)) -> getValue();
                     stringstream out;
                     out << "[" << string_size << " x i8]";
                     if (array_type) { // an array type with specified size?
                         control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(array_type));
                     }
                     control.SetAttribute(n, Control::LLVM_BIT_CAST, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
                     control.SetAttribute(n, Control::LLVM_STRING_SIZE, new IntAstAttribute(string_size));
                     control.SetAttribute(n, Control::LLVM_TYPE, new StringAstAttribute(out.str()));
                 }
                 else if (isSgClassType(attributes -> getSourceType(type))) {
                     control.SetAttribute(n, Control::LLVM_BIT_CAST, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
                     control.SetAttribute(n, Control::LLVM_TYPE, new StringAstAttribute(type_name));
                 }
                 else {
                     /**
                      * Identify this array type as an "aggregate" so that it gets loaded with the getelementptr instruction
                      */
                     if (array_type && n -> attributeExists(Control::LLVM_GLOBAL_DECLARATION) && (! isSgIntVal(array_type -> get_index()))) {
                         // TODO: Should not the base type be used here?
                         type_name = "[0 x i8]";
                         control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(array_type));
                     }
                     control.SetAttribute(n, Control::LLVM_TYPE, new StringAstAttribute(type_name));
                 }
             }
         }
     }
     //         SgAttribute:
     //             SgPragma
     else if (dynamic_cast<SgPragma *> (node)) {
     }
     //             SgBitAttribute:
     //                 SgFuncDecl_attr
     //                 SgClassDecl_attr
     //         Sg_File_Info
     //         SgFile:
     //             SgSourceFile
     else if (dynamic_cast<SgSourceFile *>(node)) {
         SgSourceFile *n = isSgSourceFile(node);
         if ((! option.isQuery()) || node -> attributeExists(Control::LLVM_TRANSLATE)) {
             if (attributes -> numAdditionalFunctions() > 0 ||
                 attributes -> numRemoteGlobalDeclarations() > 0 ||
                 attributes -> numAdditionalFunctionAttributes() > 0) {
                 revisitAttributes.push_back(attributes);
             }
         }
     }
     //             SgBinaryFile
     //             SgUnknownFile
     //         SgProject
     else if (dynamic_cast<SgProject*>(node)) {
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
     else if (dynamic_cast<SgFileList*>(node)) {
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
     else if (dynamic_cast<SgClassType *>(node)) {
         SgClassType *n = isSgClassType(node);
     }
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
         ROSE2LLVM_ASSERT(current_function_attribute.top() -> getFunctionDeclaration() == NULL);
         current_function_attribute.pop(); // done with this global scope
     }
     //                 SgBasicBlock
     else if (dynamic_cast<SgBasicBlock *>(node)) {
         SgBasicBlock *n = isSgBasicBlock(node);
     }
     //                 SgIfStmt
     else if (dynamic_cast<SgIfStmt *> (node)) {
         SgIfStmt *n = isSgIfStmt(node);
         IfAstAttribute *attribute = (IfAstAttribute *) n -> getAttribute(Control::LLVM_IF_LABELS);
         current_function_attribute.top() -> resetCurrentLabel(attribute -> getEndLabel());
     }
     //                 SgForStatement
     else if (dynamic_cast<SgForStatement *> (node)) {
         SgForStatement *n = isSgForStatement(node);

         ROSE2LLVM_ASSERT(scopeStack.top() == n);
         scopeStack.pop();

         ForAstAttribute *attribute = (ForAstAttribute *) n -> getAttribute(Control::LLVM_FOR_LABELS);

         /**
          * If this for-statement had increment statements generate code that had been buffered for them here.
          */
         if (! isSgNullExpression(n -> get_increment())) {
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getIncrementLabel());
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getIncrementLabel());
         }

         current_function_attribute.top() -> addLabelPredecessor(attribute -> getConditionLabel());
         current_function_attribute.top() -> resetCurrentLabel(attribute -> getEndLabel());
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

         ROSE2LLVM_ASSERT(scopeStack.top() == n);
         scopeStack.pop();

         WhileAstAttribute *attribute = (WhileAstAttribute *) n -> getAttribute(Control::LLVM_WHILE_LABELS);
         current_function_attribute.top() -> addLabelPredecessor(attribute -> getConditionLabel());
         current_function_attribute.top() -> resetCurrentLabel(attribute -> getEndLabel());
     }
     //                 SgDoWhileStmt
     else if (dynamic_cast<SgDoWhileStmt *> (node)) {
         SgDoWhileStmt *n = isSgDoWhileStmt(node);

         ROSE2LLVM_ASSERT(scopeStack.top() == n);
         scopeStack.pop();
     }
     //                 SgSwitchStatement
     else if (dynamic_cast<SgSwitchStatement *>(node)) {
         SgSwitchStatement *n = isSgSwitchStatement(node);

         ROSE2LLVM_ASSERT(scopeStack.top() == n);
         scopeStack.pop();

         ROSE2LLVM_ASSERT(switchStack.top() == n);
         switchStack.pop();

         SwitchAstAttribute *switch_attribute = (SwitchAstAttribute *) n -> getAttribute(Control::LLVM_SWITCH_INFO);
         current_function_attribute.top() -> resetCurrentLabel(switch_attribute -> getEndLabel());

         SgExprStatement *stmt = isSgExprStatement(n -> get_item_selector());
         addBooleanExtensionAttributeIfNeeded(stmt -> get_expression());
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
     else if (dynamic_cast<SgPragmaDeclaration *> (node)) {
     }
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
      /**
           At the end of a function declaration, save all the declarations encountered in an attribute for
           that function and reset the local_decls container for the next function declaration.

                    SgFunctionDeclaration:
                        SgMemberFunctionDeclaration:
                            SgTemplateInstantiationMemberFunctionDecl
                        SgTemplateInstantiationFunctionDecl
                        SgProgramHeaderStatement
                        SgProcedureHeaderStatement
                        SgEntryStatement
       */
     else if (dynamic_cast<SgFunctionDeclaration *>(node)) {
         SgFunctionDeclaration *n = isSgFunctionDeclaration(node);

         /**
          * If the function only has one return statement at the end of its body then indicate
          * that there is no need to create a "return" basic block.
          */
         string return_label = current_function_attribute.top() -> getReturnLabel();
         if (current_function_attribute.top() -> numLabelPredecessors(return_label) == 1 &&
             current_function_attribute.top() -> getLabelPredecessor(return_label, 0).compare(current_function_attribute.top() -> getCurrentLabel()) == 0) {
             current_function_attribute.top() -> removeLastLabelPredecessor(return_label);
         }

         /**
          * If this function requires imported declaration, save it for later processing.
          */
         if (current_function_attribute.top() -> numRemoteVariableDeclarations() > 0) {
             attributes -> insertAdditionalFunctionAttribute(current_function_attribute.top());
         }

         ROSE2LLVM_ASSERT(current_function_attribute.top() -> getFunctionDeclaration() == n);
         current_function_attribute.pop(); // done with this function

         if (option.isSyntheticTranslation()) { // If we were processing a synthetic function, indicate that we're done with it.
             option.resetSyntheticTranslation();
         }
     }
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
          * The test expression in an IfStmt, WhileStmt, ForStatement,... is wrapped in an SgExprStatement
          */
         if (n -> attributeExists(Control::LLVM_IF_LABELS)) {
             if (! n -> get_expression() -> attributeExists(Control::LLVM_IS_BOOLEAN)) { // If not boolean expression, convert to a boolean.
                 ROSE2LLVM_ASSERT(n -> get_expression() -> attributeExists(Control::LLVM_NAME));
                 // ROSE2LLVM_ASSERT(! n -> get_expression() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
             }
             IfAstAttribute *attribute = (IfAstAttribute *) n -> getAttribute(Control::LLVM_IF_LABELS);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getTrueLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getFalseLabel());
         }
         else if (n -> attributeExists(Control::LLVM_WHILE_LABELS)) {
             if (! n -> get_expression() -> attributeExists(Control::LLVM_IS_BOOLEAN)) { // If not boolean expression, convert to a boolean.
                 ROSE2LLVM_ASSERT(n -> get_expression() -> attributeExists(Control::LLVM_NAME));
                 // ROSE2LLVM_ASSERT(! n -> get_expression() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
             }
             WhileAstAttribute *attribute = (WhileAstAttribute *) n -> getAttribute(Control::LLVM_WHILE_LABELS);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getBodyLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getBodyLabel());
         }
         else if (n -> attributeExists(Control::LLVM_DO_LABELS)) {
             if (! n -> get_expression() -> attributeExists(Control::LLVM_IS_BOOLEAN)) { // If not boolean expression, convert to a boolean.
                 ROSE2LLVM_ASSERT(n -> get_expression() -> attributeExists(Control::LLVM_NAME));
                 // ROSE2LLVM_ASSERT(! n -> get_expression() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
             }
             DoAstAttribute *attribute = (DoAstAttribute *) n -> getAttribute(Control::LLVM_DO_LABELS);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getBodyLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getEndLabel());
         }
         else if (n -> attributeExists(Control::LLVM_FOR_LABELS)) {
             if ((! isSgNullExpression(n -> get_expression())) && (! n -> get_expression() -> attributeExists(Control::LLVM_IS_BOOLEAN))) { // If not boolean expression, convert to a boolean.
                 ROSE2LLVM_ASSERT(n -> get_expression() -> attributeExists(Control::LLVM_NAME));
                 // ROSE2LLVM_ASSERT(! n -> get_expression() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
                 control.SetAttribute(n -> get_expression(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
             }
             ForAstAttribute *attribute = (ForAstAttribute *) n -> getAttribute(Control::LLVM_FOR_LABELS);
             if (! isSgNullExpression(n -> get_expression())) { // if a conditional expression was present
                 current_function_attribute.top() -> addLabelPredecessor(attribute -> getBodyLabel());
                 current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
             }
             else {
                 current_function_attribute.top() -> addLabelPredecessor(attribute -> getBodyLabel());
             }
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getBodyLabel());
         }
         /**
          * Create the switch attribute after the expression has been processed as the expression may have generated 
          * some new labels, thus changing the current label that precedes the branching of the switch.
          */
         else if (n -> attributeExists(Control::LLVM_SWITCH_EXPRESSION)) {
             SwitchAstAttribute *attribute = new SwitchAstAttribute(current_function_attribute.top() -> getCurrentLabel(), attributes -> getTemp(LLVMAstAttributes::TEMP_SWITCH_END_LABEL));
             control.SetAttribute(switchStack.top(), Control::LLVM_SWITCH_INFO, attribute);
         }
     }
     //             SgLabelStatement
     else if (dynamic_cast<SgLabelStatement *>(node)) {
         SgLabelStatement *n = isSgLabelStatement(node);
         current_function_attribute.top() -> addLabelPredecessor(n -> get_label().getString());
         current_function_attribute.top() -> resetCurrentLabel(n -> get_label().getString());
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
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
         else if (dynamic_cast<SgWhileStmt *>(scope)) {
             SgWhileStmt *while_stmt = isSgWhileStmt(scope);
             WhileAstAttribute *attribute = (WhileAstAttribute *) while_stmt -> getAttribute(Control::LLVM_WHILE_LABELS);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
         else if (dynamic_cast<SgDoWhileStmt *>(scope)) {
             SgDoWhileStmt *do_stmt = isSgDoWhileStmt(scope);
             DoAstAttribute *attribute = (DoAstAttribute *) do_stmt -> getAttribute(Control::LLVM_DO_LABELS);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
         else if (dynamic_cast<SgSwitchStatement *>(scope)) {
             SgSwitchStatement *switch_stmt = isSgSwitchStatement(scope);
             SwitchAstAttribute *attribute = (SwitchAstAttribute *) switch_stmt -> getAttribute(Control::LLVM_SWITCH_INFO);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
     }
     //             SgContinueStmt
     else if (dynamic_cast<SgContinueStmt *>(node)) {
         SgContinueStmt *n = isSgContinueStmt(node);
         SgScopeStatement *scope = scopeStack.top();
         if (dynamic_cast<SgForStatement *>(scope)) {
             SgForStatement *for_stmt = isSgForStatement(scope);
             ForAstAttribute *attribute = (ForAstAttribute *) for_stmt -> getAttribute(Control::LLVM_FOR_LABELS);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getIncrementLabel());
         }
         else if (dynamic_cast<SgWhileStmt *>(scope)) {
             SgWhileStmt *while_stmt = isSgWhileStmt(scope);
             WhileAstAttribute *attribute = (WhileAstAttribute *) while_stmt -> getAttribute(Control::LLVM_WHILE_LABELS);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getConditionLabel());
         }
         else if (dynamic_cast<SgDoWhileStmt *>(scope)) {
             SgDoWhileStmt *do_stmt = isSgDoWhileStmt(scope);
             DoAstAttribute *attribute = (DoAstAttribute *) do_stmt -> getAttribute(Control::LLVM_DO_LABELS);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getConditionLabel());
         }
     }
     //             SgReturnStmt
     else if (dynamic_cast<SgReturnStmt *>(node)) {
         SgReturnStmt *n = isSgReturnStmt(node);
         if (n -> get_expression()) {
             addBooleanExtensionAttributeIfNeeded(n -> get_expression());

             SgType *return_type = attributes -> getExpressionType(n -> get_expression());
             if (isSgClassType(attributes -> getSourceType(return_type))) {
                 control.SetAttribute(n, Control::LLVM_BIT_CAST, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
                 // Needed by SgReturnStmt case in CodeGeneratorVisitor::postVisit.
                 attributes -> setNeedsMemcopy();
             }
         }
         current_function_attribute.top() -> addLabelPredecessor(current_function_attribute.top() -> getReturnLabel());
     }
     //             SgGotoStatement
     else if (dynamic_cast<SgGotoStatement *> (node)) {
         SgGotoStatement *n = isSgGotoStatement(node);
         current_function_attribute.top() -> addLabelPredecessor(((SgName) ((SgLabelStatement *) n -> get_label()) -> get_label()).getString());
     }
     //             SgSpawnStmt
     //             SgNullStatement
     //             SgVariantStatement
     //             SgForInitStatement
     else if (dynamic_cast<SgForInitStatement *>(node)) {
         SgForInitStatement *n = isSgForInitStatement(node);
         ForAstAttribute *attribute = (ForAstAttribute *) n -> getAttribute(Control::LLVM_FOR_LABELS);
         current_function_attribute.top() -> addLabelPredecessor(attribute -> getConditionLabel());
         current_function_attribute.top() -> resetCurrentLabel(attribute -> getConditionLabel());
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

         addBooleanExtensionAttributeIfNeeded(n -> get_operand());

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_NEGATION)));
     }
     //                 SgUnaryAddOp
     else if (dynamic_cast<SgUnaryAddOp *>(node)) {
         SgUnaryAddOp *n = isSgUnaryAddOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_operand());

         string name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(name));
     }
     //                 SgNotOp
     else if (dynamic_cast<SgNotOp *>(node)) {
         SgNotOp *n = isSgNotOp(node);
         control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);

         if (! n -> get_operand() -> attributeExists(Control::LLVM_IS_BOOLEAN)) { // If the operand is not boolean expression, convert it to a boolean.
             ROSE2LLVM_ASSERT(n -> get_operand() -> attributeExists(Control::LLVM_NAME));
             ROSE2LLVM_ASSERT(((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_NAME)) -> getValue().compare(((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue()) == 0);
             control.SetAttribute(n -> get_operand(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
             control.SetAttribute(n -> get_operand(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
         }

         /**
          * Rose transforms the operand to the NotOp operation into a NotEqual to 0 operation.  Thus, we need to create
          * two new names for the NotOP node here so that we can 1) flip the resulting bit from the compare operation and
          * 2) extend the result for the proper integral type.
          */
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_NOT)));
     }
     //                 SgPointerDerefExp
     else if (dynamic_cast<SgPointerDerefExp *>(node)) {
         SgPointerDerefExp *n = isSgPointerDerefExp(node);

         string operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         control.SetAttribute(n, Control::LLVM_REFERENCE_NAME, new StringAstAttribute(operand_name));
         if (n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(operand_name));
         }
         else {
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
         }
     }
     //                 SgAddressOfOp
     else if (dynamic_cast<SgAddressOfOp *>(node)) {
         SgAddressOfOp *n = isSgAddressOfOp(node);
         string name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(name));
     }
     //                 SgMinusMinusOp
     else if (dynamic_cast<SgMinusMinusOp *>(node)) {
         SgMinusMinusOp *n = isSgMinusMinusOp(node);
         string result_name = attributes -> getTemp(LLVMAstAttributes::TEMP_DECREMENT);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(result_name));
         if (n -> get_mode() == SgUnaryOp::postfix) {
             string operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(operand_name));
         }
         else {
             control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(result_name));
         }
     }
     //                 SgPlusPlusOp
     else if (dynamic_cast<SgPlusPlusOp *>(node)) {
         SgPlusPlusOp *n = isSgPlusPlusOp(node);
         string result_name = attributes -> getTemp(LLVMAstAttributes::TEMP_INCREMENT);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(result_name));
         if (n -> get_mode() == SgUnaryOp::postfix) {
             string operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
             control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(operand_name));
         }
         else {
             control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(result_name));
         }
     }
     //                 SgBitComplementOp
     else if (dynamic_cast<SgBitComplementOp *>(node)) {
         SgBitComplementOp *n = isSgBitComplementOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_operand());

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_NEGATION)));
     }
     //                 SgCastExp
     else if (dynamic_cast<SgCastExp *>(node)) {
         SgCastExp *n = isSgCastExp(node);

         if (isSgCharVal(n -> get_operand()) ||
             isSgUnsignedCharVal(n -> get_operand()) ||
             n -> attributeExists(Control::LLVM_NULL_VALUE) ||
             n -> attributeExists(Control::LLVM_IGNORE)) {
             if (n -> get_operand() -> attributeExists(Control::LLVM_NAME)) {
                 string operand_name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_NAME)) -> getValue();
                 control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(operand_name));
             }
         }
         else if (isSgTypeBool(attributes -> getSourceType(n -> get_type()))) {
             control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
             if (n->get_operand()->attributeExists(Control::LLVM_IS_BOOLEAN)) {
                 control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue()));
             }
             else {
                 control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT)));
             }
         }
         else {
             SgType *result_type = attributes -> getSourceType(n -> get_type()),
                    *operand_type = attributes -> getSourceType(attributes -> getExpressionType(n -> get_operand()));
             addBooleanExtensionAttributeIfNeeded(n -> get_operand());

             /**
              * We copy the operand name involved in a trivial cast and will ignore this operation thereafter.
              * A type is trivial if either:
              *
              *    . both the operand and the result type are exactly the same type
              *    . both the operand and the result type are integer types and they have the same alignment.
              */
             if (result_type == operand_type ||
                      ((result_type -> isIntegerType() || isSgEnumType(result_type)) && (operand_type -> isIntegerType() || isSgEnumType(operand_type)) &&
                       ((IntAstAttribute *) operand_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() ==
                       ((IntAstAttribute *) result_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue())) {
                 StringAstAttribute *attribute = (StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                 control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attribute -> getValue()));
                 control.SetAttribute(n, Control::LLVM_TRIVIAL_CAST);
             }
             else if (dynamic_cast<SgPointerType *> (result_type)) {
                 if (dynamic_cast<SgTypeString *> (operand_type)) {
                     string reference = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                     control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(reference));
                 }
                 else if (operand_type -> isIntegerType() || isSgEnumType(operand_type)) {
                     control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT)));
                 }
                 else if (dynamic_cast<SgArrayType *> (operand_type)) {
                     StringAstAttribute *attribute = (StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                     //
                     // TODO: This appears to be wrong!  Consider the case where we have a arrow derefence of an array p -> array.
                     //       Such a deference should be treated the same way as a reference to an ordinary array variable.
                     //
                     //  control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attribute -> getValue()));
                     //  control.SetAttribute(n, Control::LLVM_TRIVIAL_CAST);
                     control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT)));
                 }
                 else if (dynamic_cast<SgPointerType *> (operand_type)) {
                      /**
                       * A C void* type is represented as i8* in LLVM just like a char* type in C.
                       *
                       */
                     if (isSgTypeChar(isSgPointerType(result_type) -> get_base_type()) && isSgTypeVoid(isSgPointerType(operand_type) -> get_base_type())) {
                         StringAstAttribute *attribute = (StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attribute -> getValue()));
                         control.SetAttribute(n, Control::LLVM_TRIVIAL_CAST);
                     }
                     else{
                         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT)));
                     }
                 }
                 else if (dynamic_cast<SgFunctionType *> (operand_type)) {
                     control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT)));
                 }
                 else {
                      string source = ((StringAstAttribute *) operand_type -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                             target = ((StringAstAttribute *) result_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                      cerr << "CodeAttributesVisitor.cpp Can't convert yet from "
                           << operand_type -> class_name() << ": " << source << " to " << result_type -> class_name() << ":" << target << endl;
                      cerr.flush();
                      ROSE2LLVM_ASSERT(0);
                 }
             }
             else {
                 control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT)));
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
         string temp = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);
         control.SetAttribute(n, Control::LLVM_REFERENCE_NAME, new StringAstAttribute(temp));
         if (n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
              control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(temp));
         }
         else {
             SgArrayType *array_type = isSgArrayType(n -> get_type());
             string temp_name = attributes -> getTemp(array_type ? LLVMAstAttributes::TEMP_ARRAY : LLVMAstAttributes::TEMP_GENERIC);
             if (array_type) {
                 control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(array_type));
             }
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(temp_name));
         }
     }
     //                 SgDotExp
     else if (dynamic_cast<SgDotExp *>(node)) {
         SgDotExp *n = isSgDotExp(node);
         string temp = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);
         control.SetAttribute(n, Control::LLVM_REFERENCE_NAME, new StringAstAttribute(temp));
         if (n -> attributeExists(Control::LLVM_REFERENCE_ONLY))
              control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(temp));
         else {
             SgArrayType *array_type = isSgArrayType(n -> get_type());
             string temp_name = attributes -> getTemp(array_type ? LLVMAstAttributes::TEMP_ARRAY : LLVMAstAttributes::TEMP_GENERIC);
             if (array_type) {
                 control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(array_type));
             }
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(temp_name));
         }
     }
     //                 SgDotStarOp
     //                 SgArrowStarOp
     //                 SgEqualityOp
     else if (dynamic_cast<SgEqualityOp *> (node)) {
         SgEqualityOp *n = isSgEqualityOp(node);
         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_COMPARE)));
     }
     //                 SgLessThanOp
     else if (dynamic_cast<SgLessThanOp *>(node)) {
         SgLessThanOp *n = isSgLessThanOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_COMPARE)));
     }
     //                 SgGreaterThanOp
     else if (dynamic_cast<SgGreaterThanOp *>(node)) {
         SgGreaterThanOp *n = isSgGreaterThanOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_COMPARE)));
     }
     //                 SgNotEqualOp
     else if (dynamic_cast<SgNotEqualOp *> (node)) {
         SgNotEqualOp *n = isSgNotEqualOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_COMPARE)));
     }
     //                 SgLessOrEqualOp
     else if (dynamic_cast<SgLessOrEqualOp *> (node)) {
         SgLessOrEqualOp *n = isSgLessOrEqualOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_COMPARE)));
     }
     //                 SgGreaterOrEqualOp
     else if (dynamic_cast<SgGreaterOrEqualOp *> (node)) {
         SgGreaterOrEqualOp *n = isSgGreaterOrEqualOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_COMPARE)));
     }
     //                 SgAddOp
     else if (dynamic_cast<SgAddOp *>(node)) {
         SgAddOp *n = isSgAddOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         string temp_name = attributes -> getTemp(LLVMAstAttributes::TEMP_ADD);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(temp_name));
     }
     //                 SgSubtractOp
     else if (dynamic_cast<SgSubtractOp *>(node)) {
         SgSubtractOp *n = isSgSubtractOp(node);
         SgExpression *lhs_operand = n -> get_lhs_operand(),
                      *rhs_operand = n -> get_rhs_operand();

         addBooleanExtensionAttributeIfNeeded(lhs_operand);
         addBooleanExtensionAttributeIfNeeded(rhs_operand);
         checkIntegralOperation(n);

         string temp_name;

         //
         // O3/07/2015: ROSE Issue... This is no longer the case. Either the left or the right operand may be a pointer
         //             in the case of an addition.  In any case, a subtraction with an integer as the left-operand and a 
         //             pointer or array as the right operand is illegal.
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
          * In all 3 cases above, the AST node generated will correspond to the C statement: q = a + 3.
          */
         SgType *lhs_type = attributes -> getExpressionType(lhs_operand),
                *rhs_type = attributes -> getExpressionType(rhs_operand);
         SgArrayType *lhs_array_type = isSgArrayType(lhs_type);
         if (lhs_array_type  || isSgPointerType(attributes -> getSourceType(lhs_type))) {
             /**
              * Say Something!
              *
              *
              *
              */
             if (lhs_array_type) {
                 string lhs_type_name = ((StringAstAttribute *) lhs_array_type -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 lhs_type_name += "*";
                 control.SetAttribute(lhs_operand, Control::LLVM_ARRAY_TO_POINTER_CONVERSION, new StringAstAttribute(lhs_type_name));
             }

             SgArrayType *rhs_array_type = isSgArrayType(rhs_type);
             if (rhs_array_type) {
                 string rhs_type_name = ((StringAstAttribute *) rhs_array_type -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                 rhs_type_name += "*";
                 control.SetAttribute(rhs_operand, Control::LLVM_ARRAY_TO_POINTER_CONVERSION, new StringAstAttribute(rhs_type_name));
             }

             /**
              * This is the case of a pointer subtraction.
              *
              * Say Something!
              *
              */
             if ((lhs_array_type || isSgPointerType(attributes -> getSourceType(lhs_type))) && (rhs_array_type || isSgPointerType(attributes -> getSourceType(rhs_type)))) {
                 control.SetAttribute(lhs_operand, Control::LLVM_POINTER_TO_INT_CONVERSION, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_LHS_POINTER_TO_INT)));
                 control.SetAttribute(rhs_operand, Control::LLVM_POINTER_TO_INT_CONVERSION, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_RHS_POINTER_TO_INT)));
                 temp_name = attributes -> getTemp(LLVMAstAttributes::TEMP_SUBTRACT_PTR);
                 SgType *base_type = (lhs_array_type ? lhs_array_type -> get_base_type() : isSgPointerType(attributes -> getSourceType(lhs_type)) -> get_base_type());
                 int size = ((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
                 if (size > 1) { // element size greater than 1?
                     int shift_size = 0;
                     for (int k = size >> 1; k > 0; k >>= 1) {
                         shift_size++;
                     }
                     string divide_name = (size == 1 << shift_size ? attributes -> getTemp(LLVMAstAttributes::TEMP_POINTER_DIFFERENCE_ARITHMETIC_SHIFT_RIGHT)
                                                                   : attributes -> getTemp(LLVMAstAttributes::TEMP_POINTER_DIFFERENCE_DIVISION));
                     control.SetAttribute(n, Control::LLVM_POINTER_DIFFERENCE_DIVIDER, new StringAstAttribute(divide_name));
                     control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(divide_name));
                 }
                 SgTypeAstAttribute *type_attribute = (SgTypeAstAttribute *) n -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
                 type_attribute -> resetType(attributes -> getPointerSizeIntegerType());
             }
             else {
                 string negation_name;
                 if (isSgIntVal(rhs_operand) || isSgEnumVal(rhs_operand)) {
                     string value = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                     if (value[0] == '-') {
                         negation_name = value.substr(1);
                     }
                     else {
                         negation_name = "-";
                         negation_name += value;
                     }
                 }
                 else negation_name = attributes -> getTemp(LLVMAstAttributes::TEMP_NEGATION);
                 control.SetAttribute(n, Control::LLVM_NEGATION_NAME, new StringAstAttribute(negation_name));
                 temp_name = attributes -> getTemp(LLVMAstAttributes::TEMP_SUBTRACT_PTR);
             }
         }
         else  temp_name = attributes -> getTemp(LLVMAstAttributes::TEMP_SUBTRACT);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(temp_name));

     }
     //                 SgMultiplyOp
     else if (dynamic_cast<SgMultiplyOp *>(node)) {
         SgMultiplyOp *n = isSgMultiplyOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_MULTIPLY)));
     }
     //                 SgDivideOp
     else if (dynamic_cast<SgDivideOp *>(node)) {
         SgDivideOp *n = isSgDivideOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_DIVIDE)));
     }
     //                 SgIntegerDivideOp
     //                 SgModOp
     else if (dynamic_cast<SgModOp *>(node)) {
         SgModOp *n = isSgModOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_REMAINDER)));
     }
     //                 SgAndOp
     else if (dynamic_cast<SgAndOp *>(node)) {
         SgAndOp *n = isSgAndOp(node);

         string end_label = ((LogicalAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_LOGICAL_AND_LHS)) -> getEndLabel();
         current_function_attribute.top() -> resetCurrentLabel(end_label);

         control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         if (! n -> get_lhs_operand() -> attributeExists(Control::LLVM_IS_BOOLEAN)) { // If not a boolean expression, convert it to a boolean.
             ROSE2LLVM_ASSERT(n -> get_lhs_operand() -> attributeExists(Control::LLVM_NAME));
             // ROSE2LLVM_ASSERT(! n -> get_lhs_operand() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
             control.SetAttribute(n -> get_lhs_operand(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
             control.SetAttribute(n -> get_lhs_operand(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
         }
         if (! n -> get_rhs_operand() -> attributeExists(Control::LLVM_IS_BOOLEAN)) { // If not a boolean expression, convert it to a boolean.
             ROSE2LLVM_ASSERT(n -> get_rhs_operand() -> attributeExists(Control::LLVM_NAME));
             // ROSE2LLVM_ASSERT(! n -> get_rhs_operand() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
             control.SetAttribute(n -> get_rhs_operand(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
             control.SetAttribute(n -> get_rhs_operand(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
         }
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_AND)));
     }
     //                 SgOrOp
     else if (dynamic_cast<SgOrOp *>(node)) {
         SgOrOp *n = isSgOrOp(node);

         string end_label = ((LogicalAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_LOGICAL_OR_LHS)) -> getEndLabel();
         current_function_attribute.top() -> resetCurrentLabel(end_label);

         control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         if (! n -> get_lhs_operand() -> attributeExists(Control::LLVM_IS_BOOLEAN)) { // If not a boolean expression, convert it to a boolean.
             ROSE2LLVM_ASSERT(n -> get_lhs_operand() -> attributeExists(Control::LLVM_NAME));
             // ROSE2LLVM_ASSERT(! n -> get_lhs_operand() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
             control.SetAttribute(n -> get_lhs_operand(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
             control.SetAttribute(n -> get_lhs_operand(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
         }
         if (! n -> get_rhs_operand() -> attributeExists(Control::LLVM_IS_BOOLEAN)) { // If not a boolean expression, convert it to a boolean.
             ROSE2LLVM_ASSERT(n -> get_rhs_operand() -> attributeExists(Control::LLVM_NAME));
             // ROSE2LLVM_ASSERT(! n -> get_rhs_operand() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
             control.SetAttribute(n -> get_rhs_operand(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
             control.SetAttribute(n -> get_rhs_operand(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
         }
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_OR)));
     }
     //                 SgBitXorOp
     else if (dynamic_cast<SgBitXorOp *>(node)) {
         SgBitXorOp *n = isSgBitXorOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_XOR)));
     }
     //                 SgBitAndOp
     else if (dynamic_cast<SgBitAndOp *>(node)) {
         SgBitAndOp *n = isSgBitAndOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_AND)));
     }
     //                 SgBitOrOp
     else if (dynamic_cast<SgBitOrOp *>(node)) {
         SgBitOrOp *n = isSgBitOrOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_OR)));
     }
     //                 SgCommaOpExp
     else if (dynamic_cast<SgCommaOpExp *>(node)) {
         SgCommaOpExp *n = isSgCommaOpExp(node);
         if (n -> get_rhs_operand() -> attributeExists(Control::LLVM_IS_BOOLEAN))
             control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         StringAstAttribute *attribute = (StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attribute -> getValue()));
     }
     //                 SgLshiftOp
     else if (dynamic_cast<SgLshiftOp *>(node)) {
         SgLshiftOp *n = isSgLshiftOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_SHIFT_LEFT)));
     }
     //                 SgRshiftOp
     else if (dynamic_cast<SgRshiftOp *>(node)) {
         SgRshiftOp *n = isSgRshiftOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_lhs_operand());
         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         checkIntegralOperation(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_SHIFT_RIGHT)));
     }
     //                 SgPntrArrRefExp
     else if (dynamic_cast<SgPntrArrRefExp *> (node)) {
         SgPntrArrRefExp *n = isSgPntrArrRefExp(node);

         string rhs_type = ((StringAstAttribute *) attributes -> getExpressionType(n -> get_rhs_operand()) -> getAttribute(Control::LLVM_TYPE)) -> getValue(),
                rhs_name = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());

         /**
          * If this operation is dereferencing a multi-dimensional array aggragate and the element 
          * being dereferenced is an aggregate then set the LLVM_AGGREGATE attribute for this node. 
          */
         if (n -> get_lhs_operand() -> attributeExists(Control::LLVM_AGGREGATE)) {
             SgAggregateInitializer *aggregate = ((AggregateAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_AGGREGATE)) -> getAggregate();
             if (aggregate) {
                 vector<SgExpression *> exprs = aggregate -> get_initializers() -> get_expressions();
                 ROSE2LLVM_ASSERT(exprs.size() > 0);
                 SgAggregateInitializer *sub_aggregate = isSgAggregateInitializer(exprs[0]);
                 if (sub_aggregate) {
                     control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(sub_aggregate));
                 }
             }
             else {
                 SgArrayType *array_type = ((AggregateAstAttribute *) n -> get_lhs_operand() -> getAttribute(Control::LLVM_AGGREGATE)) -> getArrayType();
                 ROSE2LLVM_ASSERT(array_type);
                 SgArrayType *array_base_type = isSgArrayType(array_type -> get_base_type());
                 if (array_base_type && array_base_type -> attributeExists(Control::LLVM_AGGREGATE)) {
                     control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(array_base_type));
                 }
             }
         }

         //
         // TODO: Review this... I think either the reference or the name should be needed but not both.
         //
         string reference_name = attributes -> getTemp(LLVMAstAttributes::TEMP_ARRAY_INDEX);
         control.SetAttribute(n, Control::LLVM_REFERENCE_NAME, new StringAstAttribute(reference_name));
         if (n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(reference_name));
         }
         else {
             string temp_name = attributes -> getTemp(isSgArrayType(n -> get_type()) ? LLVMAstAttributes::TEMP_ARRAY : LLVMAstAttributes::TEMP_GENERIC);
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(temp_name));
         }
     }
     //                 SgScopeOp
     //                 SgAssignOp
     else if (dynamic_cast<SgAssignOp *>(node)) {
         SgAssignOp *n = isSgAssignOp(node);

         SgExpression *lhs_operand = n -> get_lhs_operand(),
                      *rhs_operand = n -> get_rhs_operand();
         SgType *lhs_type = attributes -> getExpressionType(lhs_operand),
                *rhs_type = attributes -> getExpressionType(rhs_operand);

         addBooleanExtensionAttributeIfNeeded(rhs_operand);

         if ((lhs_type -> isIntegerType() || isSgEnumType(lhs_type)) && (rhs_type -> isIntegerType() || isSgEnumType(rhs_type)) &&
             (((IntAstAttribute *) lhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue() !=
              ((IntAstAttribute *) rhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue())) {
             if (((IntAstAttribute *) lhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue() <
                 ((IntAstAttribute *) rhs_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()) {
                 string demote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                 control.SetAttribute(rhs_operand, Control::LLVM_INTEGRAL_DEMOTION, new StringAstAttribute(demote_name));
                 StringAstAttribute *rhs_result_attribute = (StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                 ROSE2LLVM_ASSERT(rhs_result_attribute);
                 rhs_result_attribute -> resetValue(demote_name);
             }
             else {
                 string promote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                 control.SetAttribute(rhs_operand, Control::LLVM_INTEGRAL_PROMOTION, new StringAstAttribute(promote_name));
                 StringAstAttribute *rhs_result_attribute = (StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                 ROSE2LLVM_ASSERT(rhs_result_attribute);
                 rhs_result_attribute -> resetValue(promote_name);
             }
         }

         string rhs_expression_name = ((StringAstAttribute *) rhs_operand -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(rhs_expression_name));
     }
     //                 SgPlusAssignOp
     else if (dynamic_cast<SgPlusAssignOp *>(node)) {
         SgPlusAssignOp *n = isSgPlusAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_ADD)));
     }
     //                 SgMinusAssignOp
     else if (dynamic_cast<SgMinusAssignOp *>(node)) {
         SgMinusAssignOp *n = isSgMinusAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

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
          *   In all 3 cases above, the AST node generated will correspond to the C statement: q = a + 3.
          */
         if (isSgPointerType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_lhs_operand()))) ||
             isSgArrayType(attributes -> getSourceType(attributes -> getExpressionType(n -> get_lhs_operand())))) {
             string negation_name;
             if (isSgIntVal(n -> get_rhs_operand()) || isSgEnumVal(n -> get_rhs_operand())) {
                 string value = ((StringAstAttribute *) n -> get_rhs_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
                 if (value[0] == '-') {
                     negation_name = value.substr(1);
                 }
                 else {
                     negation_name = "-";
                     negation_name += value;
                 }
             }
             else negation_name = attributes -> getTemp(LLVMAstAttributes::TEMP_NEGATION);

             control.SetAttribute(n, Control::LLVM_NEGATION_NAME, new StringAstAttribute(negation_name));
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_ADD)));
         }
         else {
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_SUBTRACT)));
         }
     }
     //                 SgAndAssignOp
     else if (dynamic_cast<SgAndAssignOp *>(node)) {
         SgAndAssignOp *n = isSgAndAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_AND)));
     }
     //                 SgIorAssignOp
     else if (dynamic_cast<SgIorAssignOp *>(node)) {
         SgIorAssignOp *n = isSgIorAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_OR)));
     }
     //                 SgMultAssignOp
     else if (dynamic_cast<SgMultAssignOp *>(node)) {
         SgMultAssignOp *n = isSgMultAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_MULTIPLY)));
     }
     //                 SgDivAssignOp
     else if (dynamic_cast<SgDivAssignOp *>(node)) {
         SgDivAssignOp *n = isSgDivAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_DIVIDE)));
     }
     //                 SgModAssignOp
     else if (dynamic_cast<SgModAssignOp *>(node)) {
         SgModAssignOp *n = isSgModAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_REMAINDER)));
     }
     //                 SgXorAssignOp
     else if (dynamic_cast<SgXorAssignOp *>(node)) {
         SgXorAssignOp *n = isSgXorAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_XOR)));
     }
     //                 SgLshiftAssignOp
     else if (dynamic_cast<SgLshiftAssignOp *>(node)) {
         SgLshiftAssignOp *n = isSgLshiftAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_SHIFT_LEFT)));
     }
     //                 SgRshiftAssignOp
     else if (dynamic_cast<SgRshiftAssignOp *>(node)) {
         SgRshiftAssignOp *n = isSgRshiftAssignOp(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_rhs_operand());
         addConversionAttributeIfNeeded(n);

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_SHIFT_RIGHT)));
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
         if (! n -> attributeExists(Control::LLVM_CLASS_MEMBER)) { // class members are processed at DotExp or ArrowExp level
             SgVariableSymbol *sym = n -> get_symbol();
             ROSE2LLVM_ASSERT(sym);
             SgInitializedName *decl = sym -> get_declaration();
             ROSE2LLVM_ASSERT(decl);
             string var_name = ((StringAstAttribute *) decl -> getAttribute(Control::LLVM_NAME)) -> getValue();

             // Set the alignment on the SgVarRefExp so it can easily be
             // retrieved during code generation.
             if (decl -> attributeExists(Control::LLVM_ALIGN_VAR)) {
                 int alignment = ((IntAstAttribute *) decl -> getAttribute(Control::LLVM_ALIGN_VAR)) -> getValue();
                 ROSE2LLVM_ASSERT(alignment > 0);
                 control.SetAttribute(n, Control::LLVM_ALIGN_VAR, new IntAstAttribute(alignment));
             }

             if (decl -> attributeExists(Control::LLVM_AGGREGATE)) {
                 AggregateAstAttribute *aggregate_attribute = (AggregateAstAttribute *) decl -> getAttribute(Control::LLVM_AGGREGATE);
                 SgArrayType *array_type = isSgArrayType(attributes -> getSourceType(n -> get_type()));
                 SgClassType *class_type = isSgClassType(attributes -> getSourceType(n -> get_type()));
                 ROSE2LLVM_ASSERT(array_type || class_type);

                 if (array_type) {
                     string temp = attributes -> getTemp(LLVMAstAttributes::TEMP_ARRAY);
                     control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(aggregate_attribute));
                     control.SetAttribute(n, Control::LLVM_REFERENCE_NAME, new StringAstAttribute(temp));
                     control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(temp));
                 }
                 else if (class_type) {
                     control.SetAttribute(n, Control::LLVM_AGGREGATE, new AggregateAstAttribute(aggregate_attribute));
                     control.SetAttribute(n, Control::LLVM_REFERENCE_NAME, new StringAstAttribute(var_name));
                     string name = (n -> attributeExists(Control::LLVM_REFERENCE_ONLY)
                                       ? var_name
                                       : attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC));
                     control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(name));
                 }
             }
             else {
                 ROSE2LLVM_ASSERT((StringAstAttribute *) decl -> getAttribute(Control::LLVM_NAME));
                 control.SetAttribute(n, Control::LLVM_REFERENCE_NAME, new StringAstAttribute(var_name));
                 string name = (n -> attributeExists(Control::LLVM_REFERENCE_ONLY)
                                   ? var_name
                                   : attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC));
                 control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(name));
             }
         }
     }
     //             SgClassNameRefExp
     //             SgFunctionRefExp
     else if (dynamic_cast<SgFunctionRefExp *>(node)) {
         SgFunctionRefExp *n = isSgFunctionRefExp(node);
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getFunctionName(n -> get_symbol())));
         attributes -> insertFunction(attributes -> getFunctionSignature(n -> get_symbol()).c_str());
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
         vector<SgExpression *> args = n -> get_args() -> get_expressions();

         /**
          * TODO: Remove this when bug is fixed.
          * For now, we process the types of parameters because Rose does not always cast the result of an expression
          * to be passed to a function properly... For example, if the expression is the subtraction of two pointers
          * Rose does properly type the result as an integer.
          */
         SgFunctionDeclaration *function_declaration = n -> getAssociatedFunctionDeclaration();
         if (function_declaration) { // TODO: a pointer to function does not have associated declaration!!!
             vector<SgInitializedName *> parms = function_declaration -> get_args();
             for (int i = 0; i < parms.size(); i++) {
                 attributes -> setLLVMTypeName(parms[i] -> get_type());
             }

             /**
              *
              */
             for (int i = 0; i < args.size() && i < parms.size(); i++) {
                 SgExpression *arg = args[i];
                 SgType *arg_type = attributes -> getExpressionType(arg),
                        *parm_type = parms[i] -> get_type();
                 if ((arg_type -> isIntegerType() || isSgEnumType(arg_type)) &&
                     (parm_type -> isIntegerType() || isSgEnumType(parm_type)) &&
                     (((IntAstAttribute *) arg_type -> getAttribute(Control::LLVM_SIZE)) -> getValue() !=
                      ((IntAstAttribute *) parm_type -> getAttribute(Control::LLVM_SIZE)) -> getValue())) {
                     if (((IntAstAttribute *) arg_type -> getAttribute(Control::LLVM_SIZE)) -> getValue() <
                         ((IntAstAttribute *) parm_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()) {
                         string promote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                         control.SetAttribute(arg, Control::LLVM_ARGUMENT_INTEGRAL_PROMOTION, new StringAstAttribute(promote_name));
                     }
                     else {
                         string demote_name = attributes -> getTemp(LLVMAstAttributes::TEMP_CONVERT);
                         control.SetAttribute(n, Control::LLVM_ARGUMENT_INTEGRAL_DEMOTION, new StringAstAttribute(demote_name));
                     }
                 }
             }
         }

         for (int i = 0; i < args.size(); i++) {
             SgExpression *arg = args[i];
             addBooleanExtensionAttributeIfNeeded(arg);
             SgType *arg_type = attributes -> getExpressionType(arg);
             if (isSgClassType(attributes -> getSourceType(arg_type))) {
                 string temp_name = attributes -> getTemp(LLVMAstAttributes::TEMP_AGGREGATE);
                 current_function_attribute.top() -> addCoerce(temp_name, arg_type);
                 control.SetAttribute(arg, Control::LLVM_ARGUMENT_COERCE, new StringAstAttribute(temp_name));
                 control.SetAttribute(arg, Control::LLVM_ARGUMENT_BIT_CAST, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
                 control.SetAttribute(arg, Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME, new StringAstAttribute(temp_name));
                 attributes -> setNeedsMemcopy();
             }
             else {
                 StringAstAttribute *arg_result_attribute = (StringAstAttribute *) arg -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME);
                 ROSE2LLVM_ASSERT(arg_result_attribute);
                 control.SetAttribute(arg, Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME, new StringAstAttribute(arg_result_attribute -> getValue()));
             }
         }

         SgType *return_type = n -> get_type();
         if (! isSgTypeVoid(attributes -> getSourceType(return_type))) {
             string result_name = attributes -> getTemp(isSgClassType(attributes -> getSourceType(return_type))
                                                            ? ((! attributes -> integralStructureType(return_type)) ? LLVMAstAttributes::TEMP_GENERIC : LLVMAstAttributes::TEMP_CALL)
                                                            : LLVMAstAttributes::TEMP_CALL);
             control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(result_name));

             if (isSgClassType(attributes -> getSourceType(return_type))) {
                 if (attributes -> integralStructureType(return_type)) {
                     string coerce_name = attributes -> getTemp(LLVMAstAttributes::TEMP_COERCE);
                     current_function_attribute.top() -> addCoerce(coerce_name, return_type);
                     control.SetAttribute(n, Control::LLVM_COERCE, new StringAstAttribute(coerce_name));

                     if (n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
                         control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(coerce_name));
                     }
                 }
                 else {
                     current_function_attribute.top() -> addCoerce(result_name, return_type);
                 }

                 if (! n -> attributeExists(Control::LLVM_REFERENCE_ONLY)) {
                     string cast_name = attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC);
                     control.SetAttribute(n, Control::LLVM_RETURNED_STRUCTURE_BIT_CAST, new StringAstAttribute(cast_name));
                     control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(cast_name));
                 }
             }
         }
     }
     //                 SgSizeOfOp
     else if (dynamic_cast<SgSizeOfOp *>(node)) {
         SgSizeOfOp *n = isSgSizeOfOp(node);
         /**
          * SgSizeOfOp is processed by the front-end and resolved to a SgUnsignedIntVal.  We Do Nothing!
          *
          * I don't understand why in that case, the visitor visits the original cast expression, if any,
          * and sizeof AST nodes anyway since they've been resolved to the proper constant.
          *
          * ADDITIONAL NOTES - 10/5/09: We confirmed that Rose/EDG does not always properly compute the sizeof 
          * operation.  Dan Quinlan has agreed to fix this.  We will continue to use the constant value provided
          * while waiting for the fix.  We attempted to bypass the precomputed value and recompute it but
          * the AST proved to be too complicated in some cases.
          */
     }
     //                 SgUpcLocalsizeof
     //                 SgUpcBlocksizeof
     //                 SgUpcElemsizeof
     //             SgTypeIdOp
     //             SgConditionalExp
     else if (dynamic_cast<SgConditionalExp *>(node)) {
         SgConditionalExp *n = isSgConditionalExp(node);

         /**
          * Make sure that the conditional expression is of type boolean. If not, we need to cast it into a boolean.
          */
         if (! n -> get_conditional_exp() -> attributeExists(Control::LLVM_IS_BOOLEAN)) { // If not boolean expression, convert to a boolean.
             ROSE2LLVM_ASSERT(n -> get_conditional_exp() -> attributeExists(Control::LLVM_NAME));
             // ROSE2LLVM_ASSERT(! n -> get_conditional_exp() -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME));
             control.SetAttribute(n -> get_conditional_exp(), Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_GENERIC)));
             control.SetAttribute(n -> get_conditional_exp(), Control::LLVM_BOOLEAN_CAST, new StringAstAttribute(""));
         }

         /**
          * If both expressions yield a boolean result then the final result is also of type boolean.
          * Otherwise, if an expression yields a boolean result then that result needs to be converted
          * (extended) into an integral type.
          */
         if (n -> get_true_exp() -> attributeExists(Control::LLVM_IS_BOOLEAN) &&
             n -> get_false_exp() -> attributeExists(Control::LLVM_IS_BOOLEAN)) {
             control.SetAttribute(n, Control::LLVM_IS_BOOLEAN);
         }
         else if (n -> get_true_exp() -> attributeExists(Control::LLVM_IS_BOOLEAN)) {
             addBooleanExtensionAttributeIfNeeded(n -> get_true_exp());
         }
         else if (n -> get_false_exp() -> attributeExists(Control::LLVM_IS_BOOLEAN)) {
             addBooleanExtensionAttributeIfNeeded(n -> get_false_exp());
         }

         /**
          * If a Phi function is required for this conditional expression, update the current label.
          */
         if (n -> attributeExists(Control::LLVM_CONDITIONAL_LABELS)) {
             ConditionalAstAttribute *attribute = (ConditionalAstAttribute *) n -> getAttribute(Control::LLVM_CONDITIONAL_LABELS);
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getEndLabel());
         }

         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(attributes -> getTemp(LLVMAstAttributes::TEMP_COND)));
     }
     //             SgNewExp
     //             SgDeleteExp
     //             SgThisExp
     //             SgRefExp
     //             SgInitializer:
     //                 SgAggregateInitializer
     else if (dynamic_cast<SgAggregateInitializer *>(node)) {
     }
     //                 SgConstructorInitializer
     //                 SgAssignInitializer
     else if (dynamic_cast<SgAssignInitializer *>(node)) {
         SgAssignInitializer *n = isSgAssignInitializer(node);

         addBooleanExtensionAttributeIfNeeded(n -> get_operand());

         /**
           * Pass up the name, type and alignment attributes from the expression to the Initializer.
           */
         ROSE2LLVM_ASSERT((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME));
         string name = ((StringAstAttribute *) n -> get_operand() -> getAttribute(Control::LLVM_EXPRESSION_RESULT_NAME)) -> getValue();
         control.SetAttribute(n, Control::LLVM_NAME, new StringAstAttribute(name));
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
         cerr << "CodeAttributesVisitor misses case for " << node -> class_name() << endl;  // Used for Debugging
         cerr.flush();
         ROSE2LLVM_ASSERT(0);
     }
}


/**
 * Process exit actions after a post-visit of the node.
 */
void CodeAttributesVisitor::postVisitExit(SgNode *node) {
     /**
      * Special case for for_increment
      */
     if (node -> attributeExists(Control::LLVM_BUFFERED_OUTPUT)) {
         if (dynamic_cast<ForAstAttribute *>(node -> getAttribute(Control::LLVM_BUFFERED_OUTPUT))) {
             ForAstAttribute *attribute = (ForAstAttribute *) node -> getAttribute(Control::LLVM_BUFFERED_OUTPUT);
             current_function_attribute.top() -> resetCurrentLabel(attribute -> getEndOfBodyLabel());
         }
         else ROSE2LLVM_ASSERT(! "This assertion should not have executed.  Only for-increment clauses need be buffered");
     }

     /**
      * Special case for if blocks.
      */
     if (dynamic_cast<SgStatement *>(node)) {
         SgStatement *n = isSgStatement(node);
         if (n -> attributeExists(Control::LLVM_IF_COMPONENT_LABELS)) {
             IfComponentAstAttribute *attribute = (IfComponentAstAttribute *) n -> getAttribute(Control::LLVM_IF_COMPONENT_LABELS);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
     }

     /**
      * Special case for subexpressions in a conditional expression.
      */
     if (dynamic_cast<SgExpression *>(node)) {
         SgExpression *n = isSgExpression(node);

         if (! n -> attributeExists(Control::LLVM_EXPRESSION_RESULT_NAME)) {
             if (n -> attributeExists(Control::LLVM_NAME)) {
                 string name = ((StringAstAttribute *) n -> getAttribute(Control::LLVM_NAME)) -> getValue();
                 control.SetAttribute(n, Control::LLVM_EXPRESSION_RESULT_NAME, new StringAstAttribute(name));
             }
         }

         /**
          * This is a special case for the subexpressins in a conditional expression.
          */
         if (n -> attributeExists(Control::LLVM_CONDITIONAL_TEST)) {
             ConditionalAstAttribute *attribute = (ConditionalAstAttribute *) n -> getAttribute(Control::LLVM_CONDITIONAL_TEST);
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getTrueLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getFalseLabel());
         }
         else if (n -> attributeExists(Control::LLVM_CONDITIONAL_COMPONENT_LABELS)) {
             ConditionalComponentAstAttribute *attribute = (ConditionalComponentAstAttribute *) n -> getAttribute(Control::LLVM_CONDITIONAL_COMPONENT_LABELS);
             attribute -> setLastLabel(current_function_attribute.top() -> getCurrentLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_AND_LHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_AND_LHS);
             attribute -> setLastLhsLabel(current_function_attribute.top() -> getCurrentLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getRhsLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_AND_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_AND_RHS);
             attribute -> setLastRhsLabel(current_function_attribute.top() -> getCurrentLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_OR_LHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_OR_LHS);
             attribute -> setLastLhsLabel(current_function_attribute.top() -> getCurrentLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getRhsLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
         else if (n -> attributeExists(Control::LLVM_LOGICAL_OR_RHS)) {
             LogicalAstAttribute *attribute = (LogicalAstAttribute *) n -> getAttribute(Control::LLVM_LOGICAL_OR_RHS);
             attribute -> setLastRhsLabel(current_function_attribute.top() -> getCurrentLabel());
             current_function_attribute.top() -> addLabelPredecessor(attribute -> getEndLabel());
         }
     }

     /**
      * If we are processing a synthetic function and we are done processing a declaration or an "elected"
      * loop then stop the translation to prevent code from being emitted for other executable statements.
      */
     if (option.isTranslating() && node -> attributeExists(Control::LLVM_COST_ANALYSIS)) {
         option.resetTranslating();
     }

     return;
}

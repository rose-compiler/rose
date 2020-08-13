// This is the file containing the C++ versions of the JNI functions written to interface with ECJ.
// These functions are called from the JAVA code (within the Java based traversal over the ECJ AST) 
// and construct the ROSE AST.

// Support for the internal ROSE IR classes (Precompiled header rules require it to go first).
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// **********************************************************************************
// WARNING: If this header file is not present, the source file will compile but ROSE
//          will generate an empty AST (no global declarations).
// This is generated from "$(JAVAH) -jni -classpath ${srcdir}/ecjROSE.jar:. JavaParser" 
// called within the src/3rdPartyLibraries/java-parser/Makefile.am and generated only 
// in the build tree (not the source tree).  $(JAVAH) evaluates to "javah" which 
// automatically generates header files (the implimentations of these functions are 
// defined in this file below.
// **********************************************************************************
#include "JavaParser.h"

// Support functions so that this file can be restricted to be just parser (AST traversal) rules.
#include "jni_JavaSourceCodePosition.h"
#include "token.h"
#include "java_support.h"
#include "jni_utils.h"
#include "VisitorContext.h"
#include "jni_token.h"

// This is needed so that we can call the FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope() function.
#include "fixupCxxSymbolTablesToSupportAliasingSymbols.h"

using namespace std;
using namespace Rose::Frontend::Java;

/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionParenthesizedExpression(JNIEnv *env, jclass, jint java_parentheses_count) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionParenthesizedExpression\n");

    int parentheses_count = java_parentheses_count;

    SgExpression *expression = isSgExpression(astJavaComponentStack.top());
    ROSE_ASSERT(expression);

    expression -> setAttribute("java-parentheses-count", new AstIntAttribute(parentheses_count));

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionParenthesizedExpression\n");
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionSetupSourceFilename(JNIEnv *env, jclass xxx, jstring java_full_file_name) {
    string full_file_name = convertJavaStringToCxxString(env, java_full_file_name);
    ::currentSourceFile = isSgSourceFile((*::project)[full_file_name]);
// TODO: Remove this!
//cout << "*+* Setting up source file " << full_file_name << endl;
//cout.flush();
    ROSE_ASSERT(::currentSourceFile);
    ROSE_ASSERT(::currentSourceFile -> get_file_info());
}

/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionClearSourceFilename(JNIEnv *env, jclass xxx) {
// TODO: Remove this!
//if (::currentSourceFile != NULL){
//cout << "*+* Leaving source file " << ::currentSourceFile -> getFileName() << endl;
//cout.flush();
//}
    ::currentSourceFile = NULL;
}

/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionInsertClassStart(JNIEnv *env, jclass xxx, jstring java_string, jboolean is_interface, jboolean is_enum, jboolean is_anonymous, jobject jToken) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInsertClassStart(): = %s \n", name.str());

    SgScopeStatement *outer_scope = astJavaScopeStack.top();
    ROSE_ASSERT(outer_scope != NULL);

    SgClassDeclaration *class_declaration = NULL;
    SgClassDefinition *class_definition = NULL;
    SgClassSymbol *class_symbol = lookupClassSymbolInScope(outer_scope, name);
    if (class_symbol) { // class already entered in scope... Update location, if needed
        class_declaration = isSgClassDeclaration(class_symbol -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(class_declaration);
        class_definition = class_declaration -> get_definition();
        ROSE_ASSERT(class_definition);
    }
    else {
        class_declaration = SageBuilder::buildJavaDefiningClassDeclaration(outer_scope, name,
                                                                           (is_interface
                                                                                ? SgClassDeclaration::e_java_interface
                                                                                : is_enum
                                                                                      ? SgClassDeclaration::e_java_enum
                                                                                      : SgClassDeclaration::e_class));

        setJavaSourcePosition(class_declaration, env, jToken);
        class_definition = class_declaration -> get_definition();
// TODO: Remove this!
/*
cout << "Adding class " << class_definition -> get_qualified_name().getString() << " (" << ((unsigned long) class_definition) << ")" << endl; cout.flush();
cout << "...in the stack: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); i != astJavaScopeStack.end(); i++) {
cout << "    "
<< (isSgClassDefinition(*i) ? isSgClassDefinition(*i) -> get_qualified_name().getString()
                            : isSgFunctionDefinition(*i) ? (isSgFunctionDefinition(*i) -> get_declaration() -> get_name().getString() + "(...)")
                                                         : (*i) -> class_name())
<< " ("
<< ((unsigned long) (*i))
<< ")"
<< endl;
cout.flush();
}
*/
        ROSE_ASSERT(class_definition);
        setJavaSourcePosition(class_definition, env, jToken);
    }

    SgScopeStatement *type_space = SageBuilder::buildScopeStatement(class_definition);
    setJavaSourcePosition(type_space, env, jToken);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) class_declaration -> getAttribute("type_space");
    ROSE_ASSERT(attribute);
    attribute -> setNode(type_space);

    astJavaScopeStack.push(class_definition); // to contain the class members...
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionInsertClassEnd(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInsertClassEnd: %s \n", name.str());

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgClassDefinition *class_definition = astJavaScopeStack.popClassDefinition();

 // DQ (3/25/2017): Eliminate Clang warning for unused variable (define a valid usage).
    ROSE_ASSERT(class_definition != NULL);
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildClassSupportStart(JNIEnv *env, jclass xxx, jstring java_name, jstring java_external_name, jboolean java_user_defined_class, jboolean java_is_interface, jboolean java_is_enum, jboolean java_is_anonymous, jobject jToken) {
    SgName name = convertJavaStringToCxxString(env, java_name);
//    SgName external_name = convertJavaStringToCxxString(env, java_external_name);

 // DQ (3/25/2017): Eliminate Clang warning for unused variable.
 // bool user_defined_class = java_user_defined_class;

    bool is_interface = java_is_interface;
    bool is_enum = java_is_enum;
    bool is_anonymous = java_is_anonymous;

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildClassSupportStart(): %s %s \n", (is_interface ? "interface" : "class"), name.str());

    SgScopeStatement *type_space = astJavaScopeStack.pop(); // The type parameters scope
    ROSE_ASSERT(type_space);

    //
    // Note that in the case of a local type, the scope here is not a SgClassDefinition.
    //
    SgScopeStatement *outerScope = astJavaScopeStack.top(); // The enclosing scope of this type
    ROSE_ASSERT(outerScope);

    astJavaScopeStack.push(type_space); // replace the type space onto the stack

    SgClassSymbol *class_symbol = lookupClassSymbolInScope(outerScope, name);
// TODO: Remove this!
/*
if (!class_symbol) {
cout << "Could not find class " 
     << name.getString()
     << " in scope "
     << (isSgClassDefinition(outerScope) ? isSgClassDefinition(outerScope) -> get_qualified_name().getString() : outerScope -> class_name())
<< endl;
cout.flush();
}
*/
    ROSE_ASSERT(class_symbol);
    SgClassDeclaration *class_declaration = (SgClassDeclaration *) class_symbol -> get_declaration() -> get_definingDeclaration();
    ROSE_ASSERT(class_declaration);
    setJavaSourcePosition(class_declaration, env, jToken);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition);
    setJavaSourcePosition(class_definition, env, jToken);
    astJavaScopeStack.push(class_definition); // to contain the class members...

    class_declaration -> set_explicit_interface(is_interface); // Identify whether or not this is an interface.
    class_declaration -> set_explicit_enum(is_enum);           // Identify whether or not this is an enum.
    class_declaration -> set_explicit_anonymous(is_anonymous); // Identify whether or not this is an anonymous class.
// TODO: Remove this!
/*
    if (is_anonymous) {
        class_declaration -> setAttribute("anonymous", new AstRegExAttribute(""));
    }
*/

// TODO: Remove this!
/*
    SgClassType *class_type = class_declaration -> get_type();
    if (external_name.getString().size() > 0) {
        ROSE_ASSERT(class_type);
        class_type -> setAttribute("name", new AstRegExAttribute(external_name.getString()));
    }
*/

    //
    // If this type is a user-defined class, we may need to keep track of some of its class members.
    //
    // Each method in a class definition is mapped into a unique method index.  This is done via 
    // the attribute
    class_definition -> setAttribute("method-members-map", new AstSgNodeListAttribute());
// TODO: Remove this! 12/09/13
//    class_definition -> setAttribute("method-type-parameter-scopes", new AstSgNodeListAttribute());

// TODO: Remove this! 12/09/13
//    astJavaComponentStack.push(class_definition); // To mark the end of the list of components in this type.

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionBuildClassSupportStart(): %s %s \n", (is_interface ? "interface" : "class"), name.str());
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionUpdateClassSupportStart(JNIEnv *env, jclass xxx, jstring java_name, jobject jToken) {
    SgName name = convertJavaStringToCxxString(env, java_name);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionUpdateClassSupportStart(): %s \n", name.str());

    SgScopeStatement *type_space = astJavaScopeStack.pop(); // The type parameters scope
    ROSE_ASSERT(type_space);

    //
    // Note that in the case of a local type, the scope here is not a SgClassDefinition.
    //
    SgScopeStatement *outerScope = astJavaScopeStack.top(); // The enclosing scope of this type
    ROSE_ASSERT(outerScope != NULL);

    astJavaScopeStack.push(type_space); // replace the type space onto the stack

    SgClassSymbol *class_symbol = lookupClassSymbolInScope(outerScope, name);
    ROSE_ASSERT(class_symbol);
    SgClassDeclaration *class_declaration = (SgClassDeclaration *) class_symbol -> get_declaration() -> get_definingDeclaration();
    ROSE_ASSERT(class_declaration);
    ROSE_ASSERT(class_declaration -> get_file_info() -> get_line() == 0);
    ROSE_ASSERT(class_declaration -> get_file_info() -> get_col() == 0);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition);
    ROSE_ASSERT(class_definition -> get_file_info() -> get_line() == 0);
    ROSE_ASSERT(class_definition -> get_file_info() -> get_col() == 0);

    setJavaSourcePosition(class_declaration, env, jToken);
    setJavaSourcePosition(class_definition, env, jToken);

    astJavaScopeStack.push(class_definition); // scope containing the class members...

// TODO: Remove this! 12/09/13
/*
    //
    // This is a user-defined class, we may need to keep track of some of its class members.
    //
    //    class_definition -> setAttribute("method-members-map", new AstSgNodeListAttribute());
    //    class_definition -> setAttribute("method-type-parameter-scopes", new AstSgNodeListAttribute());
*/

    ROSE_ASSERT(class_definition -> attributeExists("method-members-map"));

// TODO: Remove this! 12/09/13
//    ROSE_ASSERT(class_definition -> attributeExists("method-type-parameter-scopes"));

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionUpdateClassSupportStart(): %s \n", name.str());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInsertTypeParameter(JNIEnv *env, jclass, jstring java_name, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Build an Type Parameter \n");

    SgName name = convertJavaStringToCxxString(env, java_name);
    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgScopeStatement *outer_scope = astJavaScopeStack.top();
    ROSE_ASSERT(outer_scope != NULL);
// TODO: Remove this!

// TODO: Remove this
/*
cout << "Adding type parameter " << name.getString() << " to scope " << (isSgClassDefinition(outer_scope -> get_parent()) ? isSgClassDefinition(outer_scope -> get_parent()) -> get_qualified_name().getString() : outer_scope -> class_name()) << " (" << ((unsigned long) outer_scope) << ")" << endl;
cout.flush();

//SgClassSymbol *class_symbol = lookupClassSymbolInScope(outer_scope, name);
//if (class_symbol){
//cout << "The class found is " << ((SgClassDeclaration *) class_symbol -> get_declaration() -> get_definingDeclaration()) -> get_qualified_name().getString() << endl; cout.flush();
//}
//ROSE_ASSERT(class_symbol == NULL);
*/
    SgClassDeclaration *parameter_declaration = SageBuilder::buildJavaDefiningClassDeclaration(outer_scope, name, SgClassDeclaration::e_java_parameter);
    ROSE_ASSERT(parameter_declaration -> get_parent() == outer_scope);
    SgClassDefinition *parameter_definition = parameter_declaration -> get_definition();
    ROSE_ASSERT(parameter_definition);
    setJavaSourcePosition(parameter_declaration, env, jToken);
    setJavaSourcePosition(parameter_definition, env, jToken);

// TODO: Remove this
//    SgJavaParameterType *parameter_type = isSgJavaParameterType(parameter_declaration -> get_type());
//    parameter_type -> setAttribute("is_parameter_type", new AstRegExAttribute(""));
//
    if (SgProject::get_verbose() > 2)
        printf ("Done Building a Type Parameter \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBuildTypeParameterSupport(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jint method_index, jstring java_name, jint num_bounds, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Build an Type Parameter \n");

    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name),
           type_parameter_name = convertJavaStringToCxxString(env, java_name);

// TODO: Remove this
/*
cout << "Here 8 for parameter type " << type_parameter_name.getString()  << endl;
cout.flush();
*/
    SgType *enclosing_type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
    ROSE_ASSERT(enclosing_type);
    SgClassDeclaration *class_declaration = isSgClassDeclaration(enclosing_type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition);
 
// TODO: Remove this
/*
    AstSgNodeAttribute *type_space_attribute = NULL;
    if (method_index >= 0) { // The type parameter is enclosed in a method in the enclosing type?
// TODO: Remove this! 12/09/13
//        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-type-parameter-scopes");
//        ROSE_ASSERT(method_index < attribute -> size());
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
        ROSE_ASSERT(attribute);
        SgFunctionDefinition *method_definition = isSgFunctionDefinition(attribute -> getNode(method_index));
        ROSE_ASSERT(method_definition);
        SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
        ROSE_ASSERT(method_declaration);
        type_space_attribute = (AstSgNodeAttribute *) method_declaration -> getAttribute("type_space");
    }
    else {
        type_space_attribute = (AstSgNodeAttribute *) class_declaration -> getAttribute("type_space");
    }

    ROSE_ASSERT(type_space_attribute);
    SgScopeStatement *type_space = isSgScopeStatement(type_space_attribute -> getNode());
    ROSE_ASSERT(type_space);
*/
    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.top());
    ROSE_ASSERT(type_space && (! isSgClassDefinition(type_space)) && (! isSgFunctionDefinition(type_space)) && (! isSgBasicBlock(type_space)));
    SgClassSymbol *class_symbol = lookupClassSymbolInScope(type_space, type_parameter_name);

    SgClassDeclaration *parameter_declaration = isSgClassDeclaration(class_symbol -> get_declaration() -> get_definingDeclaration());
    ROSE_ASSERT(parameter_declaration);
    SgClassDefinition *parameter_definition = parameter_declaration -> get_definition();
    ROSE_ASSERT(parameter_definition);
    SgJavaParameterType *parameter_type = isSgJavaParameterType(parameter_declaration -> get_type());

    //
    // Take care of the super types, if any.
    //
    string type_parameter_bounds_name = "";
    for (int i = 0; i < num_bounds; i++) {
        SgNamedType *bound_type = isSgNamedType(astJavaComponentStack.popType());
        ROSE_ASSERT(bound_type);

        type_parameter_bounds_name = (i + 1 == num_bounds ? " extends " : " & ") + getTypeName(bound_type) + type_parameter_bounds_name;

        if (SgProject::get_verbose() > 0) {
            cout << "   Type " << getTypeName(bound_type)
                 << endl;
            cout.flush();
        }

        SgClassDeclaration *bound_declaration = isSgClassDeclaration(bound_type -> getAssociatedDeclaration() -> get_definingDeclaration());
        ROSE_ASSERT(bound_declaration != NULL);
        SgBaseClass *base = new SgBaseClass(bound_declaration); // TODO: Why can't one associate attributes with an SgBaseClass?
        base -> set_parent(parameter_definition);
        parameter_definition -> prepend_inheritance(base);
    }

    ROSE_ASSERT(parameter_type != NULL);
    parameter_type -> setAttribute("type", new AstRegExAttribute(parameter_type -> get_name().getString() + type_parameter_bounds_name));

// TODO: Remove this!!!
/*
cout << "The mangled name for "
     << getTypeName(parameter_type)
     << " is "
     << parameter_type -> get_mangled()
<< endl;
cout.flush();
*/

    astJavaComponentStack.push(parameter_declaration);

    if (SgProject::get_verbose() > 2)
        printf ("Done Building a Type Parameter \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionUpdatePushMethodParameterScope(JNIEnv *env, jclass, int method_index, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Push a Type Parameter scope\n");

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
    ROSE_ASSERT(attribute);
    SgFunctionDefinition *method_definition = isSgFunctionDefinition(attribute -> getNode(method_index));
    ROSE_ASSERT(method_definition);
    SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
    ROSE_ASSERT(method_declaration);
    AstSgNodeAttribute *type_space_attribute = (AstSgNodeAttribute *) method_declaration -> getAttribute("type_space");
    SgScopeStatement *type_space = isSgScopeStatement(type_space_attribute -> getNode());

    astJavaScopeStack.push(type_space);

    if (SgProject::get_verbose() > 2)
        printf ("Done Popping a Type Parameter \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionUpdateTypeParameterSupport(JNIEnv *env, jclass, jstring java_name, int method_index, jint num_bounds, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Update an Type Parameter \n");

    SgName name = convertJavaStringToCxxString(env, java_name);
    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgScopeStatement *outer_scope = astJavaScopeStack.top();
    SgFunctionDefinition *method_definition = isSgFunctionDefinition(outer_scope);
    if (method_definition) {
        AstSgNodeAttribute *type_space_attribute = (AstSgNodeAttribute *) method_definition -> get_declaration() -> getAttribute("type_space");
        outer_scope = isSgScopeStatement(type_space_attribute -> getNode());
    }
    ROSE_ASSERT(outer_scope);

    SgClassSymbol *class_symbol = lookupClassSymbolInScope(outer_scope, name);
// TODO: Remove this!
/*
if (!class_symbol) {
cout << "Could not locate type "
     << name.getString()
     << " in scope "
     << outer_scope -> class_name()
     << "; method_index = "
     << method_index
     << endl;
 cout.flush();
}
*/
    ROSE_ASSERT(class_symbol);
    SgClassDeclaration *parameter_declaration = (SgClassDeclaration *) class_symbol -> get_declaration() -> get_definingDeclaration();
    ROSE_ASSERT(parameter_declaration);
    ROSE_ASSERT(parameter_declaration -> get_file_info() -> get_line() == 0);
    ROSE_ASSERT(parameter_declaration -> get_file_info() -> get_col() == 0);
    setJavaSourcePosition(parameter_declaration, env, jToken);
    SgClassDefinition *parameter_definition = parameter_declaration -> get_definition();
    ROSE_ASSERT(parameter_definition);
    ROSE_ASSERT(parameter_definition -> get_file_info() -> get_line() == 0);
    ROSE_ASSERT(parameter_definition -> get_file_info() -> get_col() == 0);

    setJavaSourcePosition(parameter_declaration, env, jToken);
    setJavaSourcePosition(parameter_definition, env, jToken);

    //
    // Take care of the super types, if any.
    //
    SgBaseClassPtrList& bases = parameter_definition -> get_inheritances();

 // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
 // ROSE_ASSERT(bases.size() == num_bounds);
    ROSE_ASSERT(bases.size() == (size_t)num_bounds);

    string type_parameter_bounds_name = "";
    for (int i = 0, k = num_bounds - 1; i < num_bounds; i++, k--) {
        SgNamedType *bound_type = isSgNamedType(astJavaComponentStack.popType());
        ROSE_ASSERT(bound_type);

        type_parameter_bounds_name = ((k == 0) ? " extends " : " & ") + getTypeName(bound_type) + type_parameter_bounds_name;

        SgClassDeclaration *bound_declaration = isSgClassDeclaration(bound_type -> getAssociatedDeclaration() -> get_definingDeclaration());
        ROSE_ASSERT(bound_declaration);
        if (bound_declaration != bases[k] -> get_base_class()) {
            bases[k] -> set_base_class(bound_declaration);
        }
    }

    SgJavaParameterType *parameter_type = isSgJavaParameterType(parameter_declaration -> get_type());
    ROSE_ASSERT(parameter_type);
    AstRegExAttribute *attribute = (AstRegExAttribute *) parameter_type -> getAttribute("type");
    ROSE_ASSERT(attribute);
    attribute -> expression = parameter_type -> get_name().getString() + type_parameter_bounds_name;

// TODO: Remove this!
/*
cout << "The updated mangled name for "
     << getTypeName(parameter_type)
     << " is "
     << parameter_type -> get_mangled()
<< endl;
cout.flush();
*/
    if (SgProject::get_verbose() > 2)
        printf ("Done Updating a Type Parameter \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionUpdatePopMethodParameterScope(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Pop a Type Parameter scope\n");

    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());
    ROSE_ASSERT(type_space != NULL);

    if (SgProject::get_verbose() > 2)
        printf ("Done Popping a Type Parameter \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBuildClassExtendsAndImplementsSupport(JNIEnv *env, jclass xxx, jint java_num_type_parameters, jboolean java_has_super_class, jint java_num_interfaces, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildClassExtendsAndImplementsSupport()\n");

    int number_of_type_parameters = java_num_type_parameters;
    bool has_super_class = java_has_super_class;
    int number_of_interfaces = java_num_interfaces;

    //
    // Temporarily pop this type definition off the stack to that we can process its super class and interfaces.
    // We will push it back when we are done processing this type header.
    //
    ROSE_ASSERT( ! astJavaScopeStack.empty());
    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.pop());

    if (SgProject::get_verbose() > 0) {
        ROSE_ASSERT(class_definition != NULL);
        cout << "Type " << class_definition -> get_qualified_name()
             << " has "
             << (has_super_class ? "a super class" : "no super class")
             << " and "
             << number_of_interfaces
             << " interface(s)."
             << endl;
        cout.flush();
    }

    //
    // TODO: Need to do this right !!!
    //
    // As an SgJavaParameterized type is not associated with a unique declaration but with the declaration of its 
    // base raw type, we need to create an attribute that contains the list of types that extends or must be 
    // implemented by a given base type.
    //
    std::list<SgNode *> extension_list;

    //
    // Process the interfaces for this type, if any.
    //
    for (int i = 0; i < number_of_interfaces; i++) {
        SgNamedType *type = (SgNamedType *) astJavaComponentStack.popType();
// TODO: Remove this!!!
/*
        SgClassType *interface_type = isSgClassType(type);
        SgJavaParameterizedType *param_interface_type = isSgJavaParameterizedType(type);
        SgJavaQualifiedType *qualified_interface_type = isSgJavaQualifiedType(type);
        ROSE_ASSERT(interface_type || param_interface_type || qualified_interface_type);
*/
         extension_list.push_front(type); // (interface_type ? (SgNode *) interface_type : (SgNode *) param_interface_type);

// TODO: Remove this!!!
/*
         SgDeclarationStatement *declaration = (interface_type 
                                                     ? interface_type -> get_declaration() 
                                                     : param_interface_type
                                                            ? param_interface_type -> get_declaration()
                                                            : qualified_interface_type -> get_declaration());
*/
         SgClassDeclaration *interface_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
         ROSE_ASSERT(interface_declaration);
         SgBaseClass *base = new SgBaseClass(interface_declaration);
         base -> set_parent(class_definition);
         class_definition -> prepend_inheritance(base);
    }

    //
    // Add Super class to the current Class definition.
    //
    if (has_super_class) {
        SgNamedType *type = (SgNamedType *) astJavaComponentStack.popType();
// TODO: Remove this!!!
/*
        SgClassType *class_type = isSgClassType(type);
        SgJavaParameterizedType *param_class_type = isSgJavaParameterizedType(type);
        SgJavaQualifiedType *qualified_class_type = isSgJavaQualifiedType(type);
        ROSE_ASSERT(class_type || param_class_type || qualified_class_type);
*/
        extension_list.push_front(type); // (class_type ? (SgNode *) class_type : (SgNode *) param_class_type);

// TODO: Remove this!!!
/*
        SgDeclarationStatement *declaration = (class_type ? class_type -> get_declaration() : param_class_type ? param_class_type -> get_declaration() : qualified_class_type -> get_declaration());
*/
        SgClassDeclaration *class_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
        ROSE_ASSERT(! class_declaration -> get_explicit_interface()); // must be a class
        SgBaseClass *base = new SgBaseClass(class_declaration);
        base -> set_parent(class_definition);
        class_definition -> prepend_inheritance(base);
    }

    if (number_of_type_parameters > 0) {
        list<SgTemplateParameter *> parameter_list;
        for (int i = 0; i < number_of_type_parameters; i++) { // Reverse the content of the stack.
            SgClassDeclaration *parameter_decl = isSgClassDeclaration(astJavaComponentStack.pop());
            ROSE_ASSERT(parameter_decl != NULL);
            SgTemplateParameter *parameter = new SgTemplateParameter(parameter_decl -> get_type(), NULL);
            parameter_list.push_front(parameter);
        }

        SgTemplateParameterPtrList final_list;
        while (! parameter_list.empty()) { // Now that we have the parameters in the right order, create the final list.
            SgTemplateParameter *parameter = parameter_list.front();
            parameter_list.pop_front();
            final_list.push_back(parameter);
        }

        SgTemplateParameterList *template_parameter_list = new SgTemplateParameterList();
        template_parameter_list -> set_args(final_list);
        ROSE_ASSERT(class_definition->get_declaration() != NULL);
        class_definition -> get_declaration() -> setAttribute("type_parameters", new AstSgNodeAttribute(template_parameter_list));
    }

    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("extensions");
    for (list<SgNode *>::iterator extension = extension_list.begin(); extension != extension_list.end(); extension++) {
        SgType *type = isSgType(*extension);
        ROSE_ASSERT(type != NULL);
        attribute -> addNode(type);
// getTypeName(type); // TODO: this is here temporarily to check whether or not this file needs to be fully qualified in this file.
    }

// TODO: Remove this!!!
/*
    class_definition -> setAttribute("extensions", attribute); // TODO: Since declarations are not mapped one-to-one with parameterized types, we need this attribute.
    SgClassDeclaration *class_declaration = class_definition -> get_declaration();
    ROSE_ASSERT(class_declaration);
    string extension_names = getExtensionNames(extension_list, class_declaration, has_super_class);

    class_definition -> setAttribute("extension_type_names", new AstRegExAttribute(extension_names)); // TODO: Since declarations are not mapped one-to-one with parameterized types, we need this attribute.
*/
    astJavaScopeStack.push(class_definition);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionBuildClassExtendsAndImplementsSupport()\n");
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildClassSupportEnd(JNIEnv *env, jclass xxx, jstring java_string, jint num_class_members, jobject jToken) {
    SgName class_name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildClassSupportEnd: %s \n", class_name.str());

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgClassDefinition *class_definition = astJavaScopeStack.popClassDefinition();
    ROSE_ASSERT(class_definition);

// TODO: Remove this!!!
/*
    for (SgStatement *statement = astJavaComponentStack.popStatement();
        statement != class_definition;
        statement = astJavaComponentStack.popStatement()) {
        if (SgProject::get_verbose() > 2) {
            cerr << "(1) Adding statement "
                 << statement -> class_name()
                 << " to an implicit Type Declaration"
                 << endl;
            cerr.flush();
        }
        ROSE_ASSERT(statement != NULL);

        class_definition -> prepend_statement(statement);
    }
*/

    for (int i = 0; i < num_class_members; i++) {
        SgDeclarationStatement *declaration = isSgDeclarationStatement(astJavaComponentStack.pop());
        ROSE_ASSERT(declaration != NULL);
        if (SgProject::get_verbose() > 2) {
            cerr << "(1) Adding declaration "
                 << declaration -> class_name()
                 << " to class "
                 << class_name.getString()
                 << endl;
            cerr.flush();
        }

        class_definition -> prepend_member(declaration);
    }

// TODO: Remove this! 12/09/13
//    ROSE_ASSERT(astJavaComponentStack.top() == class_definition);
//    astJavaComponentStack.pop();

    ROSE_ASSERT(! astJavaScopeStack.empty());
    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop()); // Remove the type parameter scope temporarily
    ROSE_ASSERT(type_space);

    SgScopeStatement *outerScope = astJavaScopeStack.top();
    astJavaScopeStack.push(type_space); // replate the type parameter scope on the stack.

    SgClassDeclaration *class_declaration = class_definition -> get_declaration();
    ROSE_ASSERT(class_declaration);

    class_declaration -> setAttribute("complete", new AstRegExAttribute(""));

    //
    // TODO:  Review this because of the package issue and the inability to build a global AST.
    //
    ROSE_ASSERT(outerScope != NULL);
    if ((isSgClassDefinition(outerScope) != NULL) && (isSgJavaPackageDeclaration(isSgClassDefinition(outerScope)->get_declaration()) != NULL)) { // a type in a package?
        isSgClassDefinition(outerScope)->append_member(class_declaration);
    }
    else if ((isSgClassDefinition(outerScope) != NULL) && (isSgJavaPackageDeclaration(isSgClassDefinition(outerScope)->get_declaration()) == NULL)) { // an inner type?
        ; // Ignore an inner type here as it will be proceessed later when the class member declarations are visited.
          // See Java_JavaParser_cactionBuildInnerTypeSupport(...).
    }
    else if (isSgBasicBlock(outerScope) != NULL) { // a local type declaration?
        astJavaComponentStack.push(class_declaration);
    }
    else { // What is this?
        cout << "Found class declaration in scope "
             << outerScope -> class_name()
             << endl;
        ROSE_ASSERT(false);
    }

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionBuildClassSupportEnd: %s \n", class_name.str());
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionUpdateClassSupportEnd(JNIEnv *env, jclass xxx, jstring java_name, jboolean has_super_class, jint num_interfaces, jint num_class_members, jobject jToken) {
    SgName class_name = convertJavaStringToCxxString(env, java_name);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionUpdateClassSupportEnd(): %s \n", class_name.str());

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgClassDefinition *class_definition = astJavaScopeStack.popClassDefinition();
    ROSE_ASSERT(class_definition);
    int num_super_types = (has_super_class ? num_interfaces + 1 : num_interfaces);
// TODO: Remove this!
/*
cout << "Updating Class "
<< class_definition -> get_qualified_name().getString()
<< " containing "
<< num_class_members
<< " members and "
<< num_super_types
<< " super types."
<< endl;
cout.flush();
*/
    SgDeclarationStatementPtrList &declaration_list = class_definition -> get_members();
    declaration_list.resize(0); // Remove all the previous elements in order to rebuild the list
    for (int i = 0; i < num_class_members; i++) {
        SgDeclarationStatement *declaration = isSgDeclarationStatement(astJavaComponentStack.pop());
        ROSE_ASSERT(declaration != NULL);
        if (SgProject::get_verbose() > 2) {
            cerr << "(2) Adding declaration "
                 << declaration -> class_name()
                 << " to class "
                 << class_name.getString()
                 << endl;
            cerr.flush();
        }
        class_definition -> prepend_member(declaration);
    }

    SgBaseClassPtrList &super_type_list = class_definition -> get_inheritances();

 // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
 // if (super_type_list.size() != num_super_types)
    if (super_type_list.size() != (size_t)num_super_types)
       {
         cout << "Completing processing of class " << class_definition -> get_qualified_name().getString()
              << "; super_type_list.size() = "
              << super_type_list.size()
              << "; num_super_types = "
              << num_super_types
              << endl;

      // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
      // for (int i = 0; i < super_type_list.size(); i++)
         for (size_t i = 0; i < super_type_list.size(); i++)
              cout << "    -> " << super_type_list[i] -> get_base_class() -> get_qualified_name() << endl;
         cout.flush();
       }

 // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
 // ROSE_ASSERT(super_type_list.size() == num_super_types);
    ROSE_ASSERT(super_type_list.size() == (size_t)num_super_types);

    std::list<SgNode *> extension_list;
    for (int i = 0; i < num_super_types; i++) {
         SgNamedType *type = (SgNamedType *) astJavaComponentStack.popType();
         extension_list.push_front(type);
// TODO: Remove this!
/*
        SgClassType *class_type = isSgClassType(type);
        SgJavaParameterizedType *parameterized_type = isSgJavaParameterizedType(type);
        SgJavaQualifiedType *qualified_type = isSgJavaQualifiedType(type);
        ROSE_ASSERT(class_type || parameterized_type || qualified_type);

        SgClassDeclaration *super_declaration = isSgClassDeclaration((class_type
                                                                           ? class_type -> get_declaration()
                                                                           : parameterized_type
                                                                                          ? parameterized_type -> get_declaration()
                                                                                          : qualified_type -> get_declaration()) -> get_definingDeclaration());
*/
        SgClassDeclaration *super_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
 
        ROSE_ASSERT(super_declaration);
        SgBaseClass *base_class =  super_type_list[i];
        base_class -> set_base_class(super_declaration);
        if (SgProject::get_verbose() > 2) {
            cerr << "(2) Updating super type "
                 << getTypeName(type)
                 << " ["
                 << i
                 << "] to class "
                 << class_name.getString()
                 << endl;
            cerr.flush();
        }
    }

    //
    //
    //
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("extensions");
    ROSE_ASSERT(attribute);
 // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
 // TODO: Remove this!
 // if (attribute -> size() != extension_list.size()) 
    if ((size_t)(attribute -> size()) != extension_list.size()) 
       {
         cout << "attribute -> size() = "
              << attribute -> size()
              << "; extension_list.size() = "
              << extension_list.size()
              << endl;
         cout.flush();
       }

 // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
 // ROSE_ASSERT(attribute -> size() == extension_list.size());
    ROSE_ASSERT((size_t)(attribute -> size()) == extension_list.size());

    int k = 0;
    for (list<SgNode *>::iterator extension = extension_list.begin(); extension != extension_list.end(); extension++, k++) {
        SgType *type = isSgType(*extension);
        ROSE_ASSERT(type);
        attribute -> setNode(type, k);
    }

// TODO: Remove this!
/*
    SgClassDeclaration *class_declaration = class_definition -> get_declaration();
    ROSE_ASSERT(class_declaration);
    AstRegExAttribute *extension_type_names_attribute = (AstRegExAttribute *) class_definition -> getAttribute("extension_type_names");
    extension_type_names_attribute -> expression = getExtensionNames(extension_list, class_declaration, has_super_class);
*/

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionUpdateClassSupportEnd(): %s \n", class_name.str());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBuildInnerTypeSupport(JNIEnv *env, jclass,
                                                                    jstring java_package_name,
                                                                    jstring java_type_name,
                                                                    jobject jToken)
{
    if (SgProject::get_verbose() > 0)
        printf ("cactionBuildInnerTypeSupport(...)\n");

    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

// TODO: Remove this
//cout << "Here 1" << endl;
//cout.flush();
    SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);

    SgClassDeclaration *class_declaration = (SgClassDeclaration *) type -> getAssociatedDeclaration() -> get_definingDeclaration();
    ROSE_ASSERT(class_declaration);
    astJavaComponentStack.push(class_declaration);

    if (SgProject::get_verbose() > 0)
        printf ("Leaving cactionBuildInnerTypeSupport(...)\n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionUpdateInnerTypeSupport(JNIEnv *env, jclass,
                                                                     jstring java_package_name,
                                                                     jstring java_type_name,
                                                                     jobject jToken)
{
    if (SgProject::get_verbose() > 0)
        printf ("cactionUpdateInnerTypeSupport(...)\n");

    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

// TODO: Remove this
//cout << "Here 2" << endl;
//cout.flush();
    SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);

    SgClassDeclaration *class_declaration = (SgClassDeclaration *) type -> getAssociatedDeclaration() -> get_definingDeclaration();
    ROSE_ASSERT(class_declaration);
    astJavaComponentStack.push(class_declaration);

    if (SgProject::get_verbose() > 0)
        printf ("Leaving cactionUpdateInnerTypeSupport(...)\n");
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildMethodSupportStart(JNIEnv *env, jclass,
                                                                      jstring java_name,
                                                                      jint method_index,
                                                                      jobject method_location) {
    SgName method_name = convertJavaStringToCxxString(env, java_name);
    if (SgProject::get_verbose() > 1)
    printf ("Inside of BuildMethodSupportStart for method = %s with index = %d\n", method_name.str(), method_index);

// TODO: Remove this
//cout << "Processing method " << method_name.getString() << endl;
//cout.flush();

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    //
    // This scope will be used to store Type Parameters, if there are any.
    //
    SgScopeStatement *type_space = SageBuilder::buildScopeStatement(class_definition);
    type_space -> setAttribute("name", new AstRegExAttribute(method_name.getString())); // TODO: temporary patch used for mangling!
    setJavaSourcePosition(type_space, env, method_location);

// TODO: Remove this
/*
    if (method_index >= 0) {
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-type-parameter-scopes");
        ROSE_ASSERT(attribute);
        attribute -> setNode(type_space, method_index);
    }
*/

// TODO: Remove this
/*
if (method_name.getString().compare("parse") == 0) {
cout << "The TypeSpace address for method " << method_name.getString() << " is: " 
     << ((unsigned long) type_space)
     << endl;
cout.flush();
}
*/
    astJavaScopeStack.push(type_space);

    if (SgProject::get_verbose() > 1)
        printf ("Exiting BuildMethodSupportStart for method = %s with index = %d\n", method_name.str(), method_index);
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionUpdateMethodSupportStart(JNIEnv *env, jclass,
                                                                       jstring java_name,
                                                                       jint method_index,
                                                                       jint num_formal_parameters,
                                                                       jobject method_location) {
    SgName name = convertJavaStringToCxxString(env, java_name);

    if (SgProject::get_verbose() > 1)
          printf ("Inside of UpdateMethodSupportStart for method = %s \n", name.str());

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
    ROSE_ASSERT(attribute);
    SgFunctionDefinition *method_definition = isSgFunctionDefinition(attribute -> getNode(method_index));
    ROSE_ASSERT(method_definition);
    SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
    ROSE_ASSERT(method_declaration);

    SgType *return_type = astJavaComponentStack.popType();

    if (method_declaration -> get_type() -> get_return_type() != return_type) {
// TODO: Remove this!
/*
cout << "Remapping the return type of method " 
<< name.getString()
<< " in class "
<< class_definition -> get_qualified_name().getString()
<< " from "
<< getTypeName(method_declaration -> get_type() -> get_return_type())
<< " to "
<< getTypeName(return_type)
<< endl;
cout.flush();
*/
        //
        // TODO: We are trying to replace the whole function type instead of just the return type...
        // Currently, this does not work.  Since the return type is not used in defining the function type,
        // two function types that differ only in their return types are indistinguishable.
        //
        /*
        SgFunctionParameterTypeList *parameter_type_list = SageBuilder::buildFunctionParameterTypeList();
        ROSE_ASSERT(parameter_type_list != NULL);
        for (list<SgType *>::iterator t = function_parameter_types.begin(); t != function_parameter_types.end(); t++) {
            parameter_type_list -> append_argument(*t);
        }
        unsigned int mfunc_specifier = 0;
        SgMemberFunctionType *member_function_type = SageBuilder::buildMemberFunctionType(return_type,
                                                                                          parameter_type_list,
                                                                                          class_definition,
                                                                                          mfunc_specifier);
        ROSE_ASSERT(member_function_type != NULL);
        ROSE_ASSERT(member_function_type -> get_return_type() == return_type);
        parameter_type_list -> set_parent(member_function_type);
        member_function_type -> set_parent(method_declaration);
        method_declaration -> set_type(member_function_type);
        */
        method_declaration -> setAttribute("alternate-return-type", new AstRegExAttribute(getTypeName(return_type))); // TODO: This is a temporary patch to replace the commented code above until the bug is fixed!
    }

// TODO: Remove this!
/*
   if (method_index >= 0) { // Save the type_space in the class. Make it indexable by the method index.
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-type-parameter-scopes");
        ROSE_ASSERT(attribute);
        attribute -> setNode(type_space, method_index);
    }
*/

// TODO: Remove this!
/*
cout << "Introducing type space " << ((long unsigned) type_space) << " for method "
     << method_declaration -> get_name().getString()
     << endl;
cout.flush();
*/

    astJavaScopeStack.push(method_definition);

    if (SgProject::get_verbose() > 1)
        printf ("Exiting UpdateMethodSupportStart for method = %s \n", name.str());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBuildArgumentSupport(JNIEnv *env, jclass, jstring java_argument_name, jstring java_argument_type_name, jboolean java_is_var_args, jboolean java_is_final, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Build argument support\n");

// TODO: Remove this !!!
//    SgFunctionDefinition *method_definition = isSgFunctionDefinition(astJavaScopeStack.top());
//    ROSE_ASSERT(method_definition);

    SgName argument_name = convertJavaStringToCxxString(env, java_argument_name),
           argument_type_name = convertJavaStringToCxxString(env, java_argument_type_name);
    bool is_final = java_is_final;
    bool is_var_args = java_is_var_args;

    if (SgProject::get_verbose() > 0)
        printf ("argument argument_name = %s \n", argument_name.str());

    SgType *argument_type = astJavaComponentStack.popType();
    ROSE_ASSERT(argument_type);


// TODO: Remove this !!
/*
    SgScopeStatement *type_space = astJavaScopeStack.top();
    SgVariableDeclaration *parameter_declaration = SageBuilder::buildVariableDeclaration(argument_name, argument_type, NULL, type_space);
    ROSE_ASSERT(parameter_declaration);
    parameter_declaration -> set_parent(type_space);
    ROSE_ASSERT(parameter_declaration -> get_scope() != NULL);
    SgInitializedName *initialized_name = parameter_declaration -> get_decl_item(argument_name);
    ROSE_ASSERT(initialized_name);
    ROSE_ASSERT(initialized_name -> get_scope() != NULL);

    setJavaSourcePosition(initialized_name, env, jToken);
    setJavaSourcePosition(parameter_declaration, env, jToken);

// TODO: Remove this !!!
if (argument_name.getString().compare("cBuilder") == 0 || argument_name.getString().compare("builder") == 0) {
cout << "TypeSpace address for "
     << argument_name.getString()
     << " is: " 
     << ((unsigned long) type_space)
     << endl;
cout.flush();
}

    ROSE_ASSERT(type_space -> lookup_variable_symbol(argument_name));
*/


    // Until we attached this to the AST, this will generate an error in the AST consistancy tests.
    SgArrayType *array_type = isSgArrayType(argument_type);
    ROSE_ASSERT((! is_var_args) || array_type);
    SgType *element_type = (array_type ? array_type -> get_base_type() : NULL);
    ROSE_ASSERT((! is_var_args) || element_type);
    SgInitializedName *initialized_name = SageBuilder::buildJavaFormalParameter((is_var_args ? element_type : argument_type), argument_name, is_var_args, is_final); // SageBuilder::buildInitializedName(argument_name, argument_type, NULL);
    setJavaSourcePosition(initialized_name, env, jToken);
    ROSE_ASSERT(initialized_name != NULL);

    //
    // TODO: This is a patch.  Currently, the final attribute can only be associated with a
    //       variable declaration. However, a parameter declaration is an SgInitializedName
    //       in the Sage III representation and not an SgVariableDeclaration.
    //
    // The correct code should look something like this:
    //
    //    if (is_final) {
    //        initialized_name -> get_declarationModifier().setFinal();
    //    }
    //
    // if (is_final) {
    //     initialized_name -> setAttribute("final", new AstRegExAttribute(""));
    // }

    //
    // Identify Arguments with var arguments.
    //
    initialized_name -> setAttribute("type", new AstRegExAttribute(argument_type_name)); // getTypeName(element_type) + "..."));
 
    astJavaComponentStack.push(initialized_name);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Build argument support\n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionUpdateArgumentSupport(JNIEnv *env, jclass, jint argument_index, jstring java_argument_name, jstring java_argument_type_name, jboolean is_var_args, jboolean is_final, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Update argument support\n");

    SgName argument_name = convertJavaStringToCxxString(env, java_argument_name),
           argument_type_name = convertJavaStringToCxxString(env, java_argument_type_name);

    SgFunctionDefinition *method_definition = isSgFunctionDefinition(astJavaScopeStack.top());
// TODO: Remove this!
/*
if (!method_definition){
cout << "I expected a method definition but I found a "
     << astJavaScopeStack.top() -> class_name()
 << endl;
cout.flush();
}
*/
    ROSE_ASSERT(method_definition);
    SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
    ROSE_ASSERT(method_declaration);

    SgInitializedNamePtrList &args = method_declaration -> get_args();
    SgInitializedName *initialized_name = args[argument_index];
    ROSE_ASSERT(initialized_name != NULL);

// TODO: Remove this!
/*
cout << "Argument "
     << argument_index
     << " of method "
     << method_definition -> get_declaration() -> get_name().getString()
     << ": "
     << argument_name.getString()
     << " replacing "
     << initialized_name -> get_name().getString()
     << " has location: "  
     << initialized_name -> get_file_info() -> get_line()
     << ","
     << initialized_name -> get_file_info() -> get_col()
     << endl;
cout.flush();
*/
    ROSE_ASSERT(initialized_name -> get_file_info() -> get_line() == 0);
    ROSE_ASSERT(initialized_name -> get_file_info() -> get_col() == 0);
    setJavaSourcePosition(initialized_name, env, jToken);

// TODO: Remove this !!!
/*
ostringstream parm_name;     // stream used for the conversion
parm_name << argument_index
          << method_declaration -> get_name().getString(); // insert the textual representation of num_dimensions in the characters in the stream
ROSE_ASSERT(initialized_name -> get_name().getString() == parm_name.str());
SgVariableSymbol *variable_symbol = isSgVariableSymbol(initialized_name -> search_for_symbol_from_symbol_table());
ROSE_ASSERT(variable_symbol);
ROSE_ASSERT(variable_symbol -> get_name() == initialized_name -> get_name());
ROSE_ASSERT(variable_symbol == method_definition -> lookup_variable_symbol(initialized_name -> get_name()));
*/
  
    SgType *argument_type = astJavaComponentStack.popType();

// TODO: Remove this!
/*
cout << "Updating  argument type to "
     << getTypeName(argument_type)
     << "; its mangled name is " 
     << argument_type -> get_mangled().getString()
     << endl;
cout.flush();
*/
    SgVariableDeclaration *alias_declaration = SageBuilder::buildVariableDeclaration(argument_name, argument_type, NULL, method_definition);
    ROSE_ASSERT(alias_declaration);
    alias_declaration -> set_parent(method_definition);
    ROSE_ASSERT(alias_declaration -> get_scope() != NULL);
    SgInitializedName *alias_name = alias_declaration -> get_decl_item(argument_name);
    ROSE_ASSERT(alias_name);
    ROSE_ASSERT(alias_name -> get_scope() != NULL);

    setJavaSourcePosition(alias_name, env, jToken);
    setJavaSourcePosition(alias_declaration, env, jToken);

    //
    // TODO: This is a patch.  Currently, the final attribute can only be associated with a
    //       variable declaration. However, a parameter declaration is an SgInitializedName
    //       in the Sage III representation and not an SgVariableDeclaration.
    //
    // The correct code should look something like this:
    //
    //    if (is_final) {
    //        initialized_name -> get_declarationModifier().setFinal();
    //    }
    //
    if (is_final) {
        alias_name -> setAttribute("final", new AstRegExAttribute(""));
    }

    //
    // Identify Arguments with var arguments.
    //
    if (is_var_args) {
        ROSE_ASSERT(initialized_name -> attributeExists("var_args"));
        SgArrayType *array_type = isSgArrayType(argument_type);
// TODO: Remove this
//if (!array_type)
//cout << "The type is a " << (isSgClassType(argument_type) ? isSgClassType(argument_type) -> get_qualified_name().getString() : argument_type -> class_name()) << endl;
        ROSE_ASSERT(array_type);

     // DQ (3/25/2017): Eliminate Clang warning for unused variable.
     // SgType *element_type = array_type -> get_base_type();

        alias_name -> setAttribute("var_args", new AstRegExAttribute(""));
        alias_name -> setAttribute("type", new AstRegExAttribute(argument_type_name)); // getTypeName(element_type) + "..."));
    }
    else {
        ROSE_ASSERT(! initialized_name -> attributeExists("var_args"));
        alias_name -> setAttribute("type", new AstRegExAttribute(argument_type_name)); // getTypeName(argument_type)));
    }

// TODO: Remove this old code... Replaced by the code above.
/* 
    AstSgNodeAttribute *var_args_attribute = (AstSgNodeAttribute *) initialized_name -> getAttribute("var_args");
    if (var_args_attribute) {
        alias_name -> setAttribute("var_args", new AstSgNodeAttribute(var_args_attribute -> getNode()));
    }
    //
    // The initialized name must have had a "type" attribute.
    //
    alias_name -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));
*/

    SgAliasSymbol *aliasSymbol = new SgAliasSymbol(initialized_name -> search_for_symbol_from_symbol_table());
    method_definition -> insert_symbol(argument_name, aliasSymbol);
    initialized_name -> setAttribute("real_name", new AstSgNodeAttribute(alias_name));

    astJavaComponentStack.push(alias_name);

// TODO: Remove this !!!
/*
if (method_definition -> get_declaration() -> get_name().getString().compare("analyzeMethod") == 0) {
cout << "Remapping the type of argument " << argument_index <<  " of method analyzeMethod from " << getTypeName(initialized_name -> get_type()) << " to " << getTypeName(alias_name -> get_type()) << endl;
cout.flush();
}
variable_symbol = lookupVariableByName(argument_name);
ROSE_ASSERT(variable_symbol);
cout << "Found variable " << variable_symbol -> get_name().getString() << " by looking up " << argument_name << endl;
cout.flush();
*/

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Update argument support\n");
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildMethodSupportEnd(JNIEnv *env, jclass xxx,
                                                                    jstring java_string,
                                                                    jint method_index,
                                                                    jboolean java_is_constructor,
                                                                    jboolean java_is_abstract,
                                                                    jboolean java_is_native,
                                                                    jint java_number_of_type_parameters,
                                                                    jint java_number_of_arguments,
                                                                    jboolean java_is_compiler_generated,
                                                                    jobject args_location,
                                                                    jobject method_location) {
    SgName method_name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 1)
        printf ("Entering BuildMethodSupport for name = %s in file %s\n", method_name.str(), ::currentSourceFile -> getFileName().c_str());

    int number_of_type_parameters = java_number_of_type_parameters;
    int number_of_arguments = java_number_of_arguments;
    bool is_constructor = java_is_constructor,
         is_abstract = java_is_abstract,
         is_native = java_is_native,
         is_compiler_generated = java_is_compiler_generated;

    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());
    ROSE_ASSERT(type_space);

// TODO: Remove this !!!
//    SgFunctionDefinition *method_definition = isSgFunctionDefinition(((AstSgNodeAttribute *) type_space -> getAttribute("method")) -> getNode());
//    ROSE_ASSERT(method_definition);

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    //
    // There is no reason to distinguish between defining and non-defining declarations in Java...
    //
    SgMemberFunctionDeclaration *method_declaration = buildDefiningMemberFunction(is_constructor ? "<init>" : method_name, class_definition, number_of_arguments, env, method_location, args_location);
    method_declaration -> setAttribute("type_space", new AstSgNodeAttribute(type_space));
    setJavaSourcePosition(method_declaration, env, method_location);
    ROSE_ASSERT(method_declaration != NULL);
    if (is_compiler_generated) {
        method_declaration -> setAttribute("compiler-generated", new AstRegExAttribute("")); // TODO: This is needed because the ROSE flag below cannot be tested.
        method_declaration -> setCompilerGenerated();
    }

    SgFunctionDefinition *method_definition = method_declaration -> get_definition();
    ROSE_ASSERT(method_definition);

    if (method_index >= 0) {
// TODO: Remove this !!!
//        method_definition -> setAttribute("type_space", new AstSgNodeAttribute(type_space));
//        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
//        ROSE_ASSERT(attribute);
//        attribute -> setNode(method_definition, method_index);
//
//        method_definition -> setAttribute("index", new AstIntAttribute(method_index));
//
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
        ROSE_ASSERT(attribute);
        attribute -> setNode(method_definition, method_index);
    }

    if (is_constructor) {
        method_declaration -> get_specialFunctionModifier().setConstructor();
    }
    if (is_abstract) {
        method_declaration -> get_declarationModifier().setJavaAbstract();
    }
    if (is_native) {
        method_declaration -> get_functionModifier().setJavaNative();
    }

    if (number_of_type_parameters > 0) {
        list<SgTemplateParameter *> parameter_list;
        for (int i = 0; i < number_of_type_parameters; i++) { // Reverse the content of the stack.
            SgClassDeclaration *parameter_decl = isSgClassDeclaration(astJavaComponentStack.pop());
            ROSE_ASSERT(parameter_decl);
            SgTemplateParameter *parameter = new SgTemplateParameter(parameter_decl -> get_type(), NULL);
            parameter_list.push_front(parameter);
        }

        SgTemplateParameterPtrList final_list;
        while (! parameter_list.empty()) { // Now that we have the parameters in the right order, create the final list.
            SgTemplateParameter *parameter = parameter_list.front();
            parameter_list.pop_front();
            final_list.push_back(parameter);
        }

        SgTemplateParameterList *template_parameter_list = new SgTemplateParameterList();
        template_parameter_list -> set_args(final_list);
        method_declaration -> setAttribute("type_parameters", new AstSgNodeAttribute(template_parameter_list));
    }

    // TODO: We need the next 3 lines for EDG4 [DONE]
    SgMemberFunctionDeclaration *nondefining_method_declaration = isSgMemberFunctionDeclaration(method_declaration -> get_firstNondefiningDeclaration());
    ROSE_ASSERT(nondefining_method_declaration);
    nondefining_method_declaration -> get_declarationModifier().get_accessModifier().set_modifier(method_declaration -> get_declarationModifier().get_accessModifier().get_modifier());

    astJavaComponentStack.push(method_declaration);
// TODO: Remove this !!!
/*
if (method_name.getString().compare("analyzeMethod") == 0){
cout << "In class " 
<< class_definition -> get_qualified_name().getString()
<< ", inserted analyzeMethod(";
vector<SgInitializedName *> args = method_declaration -> get_args();
for (int k = 0; k < args.size(); k++) {
SgType *type = args[k] -> get_type();
if (k > 0) cout << ", ";
cout << getTypeName(type);
}
cout << ")" << endl;
}
*/
    if (SgProject::get_verbose() > 1)
        printf ("Exiting BuildMethodSupport for name = %s \n", method_name.str());
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionUpdateMethodSupportEnd(JNIEnv *env, jclass xxx,
                                                                     jstring java_string,
                                                                     jint method_index,
                                                                     jboolean is_compiler_generated,
                                                                     jint number_of_parameters,
                                                                     jobject args_location,
                                                                     jobject method_location) {
    SgName method_name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 1)
        printf ("Updating UpdateMethodSupportEnd for name = %s \n", method_name.str());

    SgFunctionDefinition *method_definition = astJavaScopeStack.popFunctionDefinition();
    ROSE_ASSERT(method_definition);

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    //
    // There is no reason to distinguish between defining and non-defining declarations in Java...
    //
    SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
    ROSE_ASSERT(method_declaration != NULL);
    ROSE_ASSERT(method_declaration -> get_file_info() -> get_line() == 0);
    ROSE_ASSERT(method_declaration -> get_file_info() -> get_col() == 0);

    setJavaSourcePosition(method_declaration, env, method_location);

    if (is_compiler_generated && (! method_declaration -> attributeExists("compiler-generated"))) {
        method_declaration -> setAttribute("compiler-generated", new AstRegExAttribute("")); // TODO: This is needed because the ROSE flag below cannot be tested.
        method_declaration -> setCompilerGenerated();
    }

    //
    //
    //
    if (number_of_parameters > 0) {
        AstSgNodeListAttribute *attribute = new AstSgNodeListAttribute();
        for (int i = 0, k = number_of_parameters - 1; i < number_of_parameters; i++, k--) { // reverse the order
            SgInitializedName *parm = isSgInitializedName(astJavaComponentStack.pop());
            attribute -> setNode(parm -> get_type(), k);
        }
        method_declaration -> setAttribute("updated-parameter-types", attribute);
    }

    if (method_index >= 0) {
// TODO: Remove this !!!
//        method_definition -> setAttribute("type_space", new AstSgNodeAttribute(type_space));
//        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
//        ROSE_ASSERT(attribute);
//        attribute -> setNode(method_definition, method_index);

        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
        ROSE_ASSERT(attribute);
// TODO: Remove this !!!
/*
if (attribute -> size() <= method_index){
cout << "While processing method "
     << method_name.getString()
     << "; the list size is "
<< attribute -> size()
<< " while the method index is " 
<< method_index
<< endl;
cout.flush();
}
*/
        attribute -> setNode(method_definition, method_index);
    }

    astJavaComponentStack.push(method_declaration);
// TODO: Remove this !!!
/*
if (method_name.getString().compare("analyzeMethod") == 0){
cout << "In class " 
<< class_definition -> get_qualified_name().getString()
<< ", updated analyzeMethod(";
vector<SgInitializedName *> args = method_declaration -> get_args();
for (int k = 0; k < args.size(); k++) {
SgType *type = args[k] -> get_type();
if (k > 0) cout << ", ";
cout << getTypeName(type);
}
cout << ")" << endl;
}
*/
    if (SgProject::get_verbose() > 1)
        printf ("Exit Updating UpdateMethodSupportEnd for name = %s \n", method_name.str());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBuildInitializerSupport(JNIEnv *env, jclass, jboolean java_is_static, jstring java_string, jint initializer_index, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildInitializerSupport \n");

    bool isStatic = java_is_static;
    SgName name = convertJavaStringToCxxString(env, java_string);

    //
    // The initializer block has no return type as it is not a real method. So, we
    // start by pushing a VOID return type to make it look like a method.
    //
    astJavaComponentStack.push(SgTypeVoid::createType()); 

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition != NULL);

    SgMemberFunctionDeclaration *initializer_declaration = buildDefiningMemberFunction(name, class_definition, 0 /* no arguments */, env, jToken, jToken);
    ROSE_ASSERT(initializer_declaration != NULL);

    setJavaSourcePosition(initializer_declaration, env, jToken);
    // Set the specific modifier, this modifier is common to C/C++.
    initializer_declaration -> get_functionModifier().setJavaInitializer();
    initializer_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);
    if (isStatic) {
        initializer_declaration -> get_declarationModifier().get_storageModifier().setStatic();
    }

// TODO: Remove this !!!
//    SgBasicBlock *type_space = new SgBasicBlock();
//    type_space -> set_parent(class_definition);
//    setJavaSourcePosition(type_space, env, jToken);
// cout << "Processed type space " << std::hex << ((unsigned long) type_space) << '\n';

    SgFunctionDefinition *initializer_definition = initializer_declaration -> get_definition();
    ROSE_ASSERT(initializer_definition);
    if (initializer_index >= 0) {
// TODO: Remove this !!!
//        initializer_definition -> setAttribute("type_space", new AstSgNodeAttribute(type_space));
//        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
//        ROSE_ASSERT(attribute);
//        attribute -> setNode(initializer_definition, initializer_index);

        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
        ROSE_ASSERT(attribute);
        attribute -> setNode(initializer_definition, initializer_index);

// TODO: Remove this !!!
//        attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-type-parameter-scopes");
//        ROSE_ASSERT(attribute);
//        attribute -> setNode(type_space, initializer_index);
    }

    // TODO: We need the next 3 lines for EDG4 [DONE]
    SgMemberFunctionDeclaration *nondefining_initializer_declaration = isSgMemberFunctionDeclaration(initializer_declaration -> get_firstNondefiningDeclaration());
    ROSE_ASSERT(nondefining_initializer_declaration);
    nondefining_initializer_declaration -> get_declarationModifier().get_accessModifier().set_modifier(initializer_declaration -> get_declarationModifier().get_accessModifier().get_modifier());

    astJavaComponentStack.push(initializer_declaration);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionBuildInitializerSupport \n");
}


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildFieldSupport(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildFieldSupport (variable declaration for field) \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    SgType *type = astJavaComponentStack.popType();

// TODO: Remove this !!!
/*
cout << "building field "
     << name.getString()
     << " of type "
     << getTypeName(type)
     << endl;
cout.flush();
*/

    SgVariableDeclaration *variable_declaration = SageBuilder::buildVariableDeclaration (name, type, NULL, astJavaScopeStack.top());
    ROSE_ASSERT(variable_declaration != NULL);
    variable_declaration -> set_parent(astJavaScopeStack.top());
    setJavaSourcePosition(variable_declaration, env, jToken);
    SgInitializedName *initialized_name = *(variable_declaration -> get_variables().begin());
    setJavaSourcePosition(initialized_name, env, jToken);

// TODO: Remove this !!!
/*
SgScopeStatement *field_definition_scope = variable_declaration -> get_scope();
cout << "Found field "
     << vars[0] -> get_name().getString()
     << " with type "
     << getTypeName(vars[0] -> get_type())
     << " in scope "
     << (isSgClassDefinition(field_definition_scope) ? isSgClassDefinition(field_definition_scope) -> get_qualified_name().getString() : field_definition_scope -> class_name())
     << endl;
cout.flush();
*/

    astJavaComponentStack.push(variable_declaration);

    if (SgProject::get_verbose() > 0)
        variable_declaration -> get_file_info() -> display("source position in Java_JavaParser_cactionBuildFieldSupport(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionBuildFieldSupport (variable declaration for field) \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionUpdateFieldSupport(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionUpdateFieldSupport (variable declaration for field) \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    SgType *type = astJavaComponentStack.popType();

    SgScopeStatement *outer_scope = astJavaScopeStack.top();
    ROSE_ASSERT(outer_scope);
    SgVariableSymbol *symbol = outer_scope -> lookup_variable_symbol(name);
    SgVariableDeclaration *variable_declaration = NULL;
    if (! symbol) { // this field is not in the Symbol table?  Enter it.
        variable_declaration = SageBuilder::buildVariableDeclaration (name, type, NULL, outer_scope);
        ROSE_ASSERT(variable_declaration != NULL);
        variable_declaration -> set_parent(outer_scope);
    }
    else {
        SgInitializedName *initialized_name = symbol -> get_declaration();
        ROSE_ASSERT(initialized_name);
        initialized_name -> set_type(type);
        variable_declaration = isSgVariableDeclaration(initialized_name -> get_declaration());
        ROSE_ASSERT(variable_declaration);
        ROSE_ASSERT(variable_declaration -> get_file_info() -> get_line() == 0);
        ROSE_ASSERT(variable_declaration -> get_file_info() -> get_col() == 0);
    }

    setJavaSourcePosition(variable_declaration, env, jToken);
    SgInitializedName *initialized_name = *(variable_declaration -> get_variables().begin());
    ROSE_ASSERT(initialized_name -> get_file_info() -> get_line() == 0);
    ROSE_ASSERT(initialized_name -> get_file_info() -> get_col() == 0);
    setJavaSourcePosition(initialized_name, env, jToken);

    astJavaComponentStack.push(variable_declaration);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionUpdateFieldSupport (variable declaration for field) \n");
}


// ***************************************************************************
// ***************************************************************************
// Implementation of functions generated from JNI specifications and matching
// prototype declarations found in the auto-generated file JavaParser.h in 
// the build tree at: src/3rdPartyLibraries/java-parser.
// ***************************************************************************
// ***************************************************************************


JNIEXPORT jboolean JNICALL Java_JavaParser_cactionIsSpecifiedSourceFile(JNIEnv *env, jclass, jstring java_full_file_name) {
    string full_file_name = convertJavaStringToCxxString(env, java_full_file_name);
    return ((*::project)[full_file_name] != NULL);
}

JNIEXPORT void JNICALL Java_JavaParser_cactionTest(JNIEnv *env, jclass) {
    cout << "Ok, The JNI connection was made !!!" << endl;
    cout.flush();
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInsertImportedPackageOnDemand(JNIEnv *env, jclass, jstring java_package_name, jobject jToken) {
    SgName package_name = convertJavaStringToCxxString(env, java_package_name);
    SgClassDefinition *package = findOrInsertPackage(package_name, env, jToken);
    ROSE_ASSERT(package);

    ROSE_ASSERT(::currentSourceFile != NULL);
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("import_on_demand_packages");
    if (attribute == NULL) { // if not yet allocated then allocate it!
        attribute = new AstSgNodeListAttribute();
        ::currentSourceFile -> setAttribute("import_on_demand_packages", attribute);
    }
    attribute -> addNode(package);
// TODO: Remove this!
//cout << "Importing on-demandpackage " << package -> get_qualified_name().getString() << " to file " << ::currentSourceFile -> getFileName()
//<< endl;
//cout.flush();
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInsertImportedTypeOnDemand(JNIEnv *env, jclass, jstring java_package_name,  jstring java_type_name, jobject jToken) {
    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

// TODO: Remove this
//cout << "Here 3" << endl;
//cout.flush();
    SgNamedType *type = (SgNamedType *) lookupTypeByName(package_name, type_name, 0);
    ROSE_ASSERT(type);
    SgClassDeclaration *class_declaration = (SgClassDeclaration *) type -> getAssociatedDeclaration() -> get_definingDeclaration();
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition);

    ROSE_ASSERT(::currentSourceFile != NULL);
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("import_on_demand_types");
    if (attribute == NULL) { // if not yet allocated then allocate it!
        attribute = new AstSgNodeListAttribute();
        ::currentSourceFile -> setAttribute("import_on_demand_types", attribute);
    }
    attribute -> addNode(class_definition);
// TODO: Remove this!
//cout << "Importing on-demand type " << type -> get_qualified_name().getString() << " to file " << ::currentSourceFile -> getFileName()
//<< endl;
//cout.flush();
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInsertImportedType(JNIEnv *env, jclass, jstring java_package_name,  jstring java_type_name, jobject jToken) {
    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

// TODO: Remove this
//cout << "Here 4" << endl;
//cout.flush();
    SgNamedType *type = (SgNamedType *) lookupTypeByName(package_name, type_name, 0);
    ROSE_ASSERT(type);

    ROSE_ASSERT(::currentSourceFile != NULL);
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("imported_types");
    if (attribute == NULL) { // if not yet allocated then allocate it!
        attribute = new AstSgNodeListAttribute();
        ::currentSourceFile -> setAttribute("imported_types", attribute);
    }
    attribute -> addNode(type);
// TODO: Remove this!
//cout << "Importing type " << type -> get_qualified_name().getString() << " to file " << ::currentSourceFile -> getFileName()
//<< endl;
//cout.flush();
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInsertImportedStaticField(JNIEnv *env, jclass, jstring java_variable_name, jobject jToken) {
    SgName variable_name = convertJavaStringToCxxString(env, java_variable_name);

    SgNamedType *type = isSgNamedType(astJavaComponentStack.popType());
    ROSE_ASSERT(type);

    SgClassDeclaration *class_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    ROSE_ASSERT(class_declaration -> get_definition());
    SgVariableSymbol *variable_symbol = lookupSimpleNameVariableInClass(variable_name, class_declaration -> get_definition());
// TODO: Remove this !

if (! variable_symbol) {
  cout << "Could not find variable \"" << variable_name.getString()
       << "\" in type: " << class_declaration -> get_qualified_name().getString()
       << " in file " << ::currentSourceFile -> getFileName()
       << endl;
  cout.flush();
}

    ROSE_ASSERT(variable_symbol);

    ROSE_ASSERT(::currentSourceFile != NULL);
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("imported_fields");
    if (attribute == NULL) { // if not yet allocated then allocate it!
        attribute = new AstSgNodeListAttribute();
        ::currentSourceFile -> setAttribute("imported_fields", attribute);
    }
    attribute -> addNode(variable_symbol);
// TODO: Remove this!
//cout << "Importing field " << variable_name.getString() << " to file " << ::currentSourceFile -> getFileName()
//<< endl;
//cout.flush();
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPushPackage(JNIEnv *env, jclass, jstring java_package_name, jobject jToken) {
    SgName package_name = convertJavaStringToCxxString(env, java_package_name);
    SgClassDefinition *package_definition = findOrInsertPackage(package_name, env, jToken);
    ROSE_ASSERT(package_definition);
    astJavaScopeStack.push(::globalScope);  // Push the global scope onto the stack.
    astJavaScopeStack.push(package_definition);        // Push the package onto the scopestack.
}


JNIEXPORT void JNICALL Java_JavaParser_cactionUpdatePushPackage(JNIEnv *env, jclass, jstring java_package_name, jobject jToken) {
    SgName package_name = convertJavaStringToCxxString(env, java_package_name);
    SgClassDeclaration *package_declaration = findPackageDeclaration(package_name);
    ROSE_ASSERT(package_declaration);
    SgClassDefinition *package_definition = package_declaration -> get_definition();
    ROSE_ASSERT(package_definition);

    if (package_declaration -> get_file_info() -> get_line() == 0) { // if location is missing on this package declaration, add it
        setJavaSourcePosition(package_declaration, env, jToken);
        setJavaSourcePosition(package_definition, env, jToken);
    }

    astJavaScopeStack.push(::globalScope);      // Push the global scope onto the stack.
    astJavaScopeStack.push(package_definition); // Push the package onto the scopestack.
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPopPackage(JNIEnv *env, jclass) {
    ROSE_ASSERT(isSgClassDefinition(astJavaScopeStack.top()) && isSgJavaPackageDeclaration(isSgClassDefinition(astJavaScopeStack.top()) -> get_declaration()));

    astJavaScopeStack.popClassDefinition();
    astJavaScopeStack.popGlobal(); // Pop the global scope onto the stack.
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPushTypeScope(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jobject jToken) {
    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);
    SgClassDefinition *definition = findOrInsertPackage(package_name, env, jToken);
    ROSE_ASSERT(definition);
    astJavaScopeStack.push(::globalScope);  // Push the global scope onto the stack.
    astJavaScopeStack.push(definition);     // Push the package onto the scopestack.

    list<SgName> qualifiedTypeName = generateQualifierList(type_name);
    ROSE_ASSERT(qualifiedTypeName.size());
    for (list<SgName>::iterator name = qualifiedTypeName.begin(); name != qualifiedTypeName.end(); name++) {
        SgClassSymbol *class_symbol = lookupClassSymbolInScope(definition, *name);
        ROSE_ASSERT(class_symbol);
        SgClassDeclaration *declaration = isSgClassDeclaration(class_symbol -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(declaration);
        definition = declaration -> get_definition();
        astJavaScopeStack.push(definition);
    }
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPopTypeScope(JNIEnv *env, jclass) {
    SgScopeStatement *scope;
    do {
        scope = astJavaScopeStack.pop(); // Pop the enclosing type
    } while(scope != ::globalScope);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPushTypeParameterScope(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jobject jToken) {
    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

// TODO: Remove this
//cout << "Here 5" << endl;
//cout.flush();
    SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
    SgClassDeclaration *class_declaration = (SgClassDeclaration *) type -> getAssociatedDeclaration() -> get_definingDeclaration();
    ROSE_ASSERT(class_declaration);

    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) class_declaration -> getAttribute("type_space");
    ROSE_ASSERT(attribute);
    SgScopeStatement *type_space = (SgScopeStatement *) attribute -> getNode();
    astJavaScopeStack.push(type_space);  // Push the type parameters scope for this type onto the stack.
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPopTypeParameterScope(JNIEnv *env, jclass, jobject jToken) {
    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());  // Pop a type parameters scope from the stack.
    ROSE_ASSERT(type_space);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitList(JNIEnv *env, jclass) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionCompilationUnitList \n");

    // TODO: We need the next line for EDG4 [DONE]
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);

    // This is already setup by ROSE as part of basic file initialization before calling ECJ.
    if (SgProject::get_verbose() > 0) {
        printf(
            "Rose::Frontend::Java::Ecj::Ecj_globalProjectPointer = %s \n",
            Rose::Frontend::Java::Ecj::Ecj_globalProjectPointer -> class_name().c_str());

    }
    ::project = Rose::Frontend::Java::Ecj::Ecj_globalProjectPointer;
    ROSE_ASSERT(::project != NULL);

    // Get the pointer to the global scope and push it onto the astJavaScopeStack.
    ::globalScope = ::project -> get_globalScopeAcrossFiles(); // */ sourcefile -> get_globalScope(); // TODO: Do this right!!!
    if (::globalScope == NULL) { // TODO: Ask Dan about this!!!?
        ::globalScope = new SgGlobal(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
        //SageInterface::setSourcePosition(::globalScope);
        ::globalScope -> set_parent(::project);
        ::project -> set_globalScopeAcrossFiles(::globalScope);
    }
    ROSE_ASSERT(::globalScope != NULL);

    //
    // At this point, the scope stack should be empty. Push the global scope into it.
    //
    ROSE_ASSERT(astJavaScopeStack.empty());
    astJavaScopeStack.push(::globalScope); // Push the global scope onto the stack.

    // Verify that the parent is set, these AST nodes are already setup by ROSE before calling this function.
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);


    //
    // At this point, the component stack should be empty.
    //
    ROSE_ASSERT(astJavaComponentStack.empty());

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionCompilationUnitList \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitListEnd(JNIEnv *env, jclass) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionCompilationUnitListEnd() \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgGlobal *global = astJavaScopeStack.popGlobal(); // remove the global scope
    ROSE_ASSERT(global == ::globalScope);
    ROSE_ASSERT(astJavaScopeStack.empty());

    // TODO: We need the next line for EDG4 [DONE]
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionTransformation);
    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionCompilationUnitListEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionSetupBasicTypes(JNIEnv *env, jclass) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionSetupObject\n");

    ROSE_ASSERT(! ::ObjectClassType); // Object not yet processed.

    SgName java_lang = "java.lang";
    SgJavaPackageDeclaration *java_lang_package_declaration = findPackageDeclaration(java_lang);
    ROSE_ASSERT(java_lang_package_declaration);
    ::javaLangPackageDefinition = java_lang_package_declaration -> get_definition();
    ROSE_ASSERT(::javaLangPackageDefinition);

    //
    // Create the Object type
    //
    SgName object_name = "Object";
    SgClassSymbol *Object_class_symbol = lookupClassSymbolInScope(::javaLangPackageDefinition, object_name);
    ::ObjectClassType = isSgClassType(Object_class_symbol -> get_type());
    ROSE_ASSERT(::ObjectClassType);
    SgClassDeclaration *object_declaration = isSgClassDeclaration(::ObjectClassType -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(object_declaration);
    ::ObjectClassDefinition = object_declaration -> get_definition();
    ROSE_ASSERT(::ObjectClassDefinition);
    ROSE_ASSERT(::ObjectClassDefinition -> get_qualified_name().getString().compare("java.lang.Object") == 0);

    //
    // Now, create an artificial "length" field in Object to be used for arrays.
    //
    SgVariableDeclaration *variable_declaration = SageBuilder::buildVariableDeclaration ("length", SgTypeInt::createType(), NULL, ::ObjectClassDefinition);
    ROSE_ASSERT(variable_declaration != NULL);
    variable_declaration -> set_parent(::ObjectClassDefinition);
    SageInterface::setSourcePosition(variable_declaration); // setJavaSourcePositionUnavailableInFrontend(variable_declaration);
    SgInitializedName *initialized_name = *(variable_declaration -> get_variables().begin());
    ROSE_ASSERT(initialized_name);
    SageInterface::setSourcePosition(initialized_name); // setJavaSourcePositionUnavailableInFrontend(initialized_name);

    ::lengthSymbol = ::ObjectClassDefinition -> lookup_variable_symbol("length");
    ROSE_ASSERT(::lengthSymbol);

    //
    // Create the String type
    //
    SgName string_name = "String";
    SgClassSymbol *String_class_symbol = lookupClassSymbolInScope(::javaLangPackageDefinition, string_name);
    ::StringClassType = isSgClassType(String_class_symbol -> get_type());
    ROSE_ASSERT(::StringClassType);
    ROSE_ASSERT(::StringClassType -> get_qualified_name().getString().compare("java.lang.String") == 0);

    //
    // Create the Class type.
    //
    SgName class_name = "Class";
    SgClassSymbol *Class_class_symbol = lookupClassSymbolInScope(::javaLangPackageDefinition, class_name);
    ::ClassClassType = isSgClassType(Class_class_symbol -> get_type());
    ROSE_ASSERT(::ClassClassType);
    ROSE_ASSERT(::ClassClassType -> get_qualified_name().getString().compare("java.lang.Class") == 0);

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionSetupObject\n");
}

JNIEXPORT void JNICALL Java_JavaParser_cactionPackageAnnotations(JNIEnv *env, jclass, int num_annotations, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionPackageAnnotations(): %d annotations\n", num_annotations);

    if (num_annotations > 0) {
        ROSE_ASSERT(::currentSourceFile);
        SgJavaPackageStatement *package_statement = ::currentSourceFile -> get_package();
        ROSE_ASSERT(package_statement);
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        package_statement -> setAttribute("annotations", annotations_attribute);
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astJavaComponentStack.popExpression();
            annotation -> set_parent(package_statement);
            annotations_attribute -> setNode(annotation, i);
        }
    }

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionPackageAnnotetions(): %d annotations\n", num_annotations);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclaration(JNIEnv *env, jclass, jstring java_full_file_name, jstring java_package_name, jstring java_filename, jobject jToken) {
     if (SgProject::get_verbose() > 0)
         printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration() \n");

    string full_file_name = convertJavaStringToCxxString(env, java_full_file_name);

    ROSE_ASSERT(::currentSourceFile == isSgSourceFile((*::project)[full_file_name]));
// TODO: Remove this !
/*
    if (! sourcefile) {
cout << "File " << full_file_name << " was not found.  I am inserting it!" << endl;
cout.flush();
        sourcefile = new SgSourceFile();
        // newFile -> set_C_only(false);
        Sg_File_Info *fileInfo = new Sg_File_Info(full_file_name, 0, 0);
        sourcefile -> set_startOfConstruct(fileInfo);
        fileInfo -> set_parent(sourcefile);
        sourcefile -> set_sourceFileNameWithPath(full_file_name);
        sourcefile -> set_globalScope(::globalScope);

        //::project -> get_sourceFileNameList().push_back(full_file_name);
        ::project -> get_fileList_ptr() -> get_listOfFiles().push_back(sourcefile);
        ROSE_ASSERT(sourcefile == isSgSourceFile((*::project)[full_file_name]));
    }
    ROSE_ASSERT(sourcefile);
*/
//cout << "*** Processing file " << ::currentSourceFile -> getFileName() /* full_file_name */<< endl;
//cout.flush();

    SgName package_name = convertJavaStringToCxxString(env, java_package_name);
    ROSE_ASSERT(astJavaScopeStack.top() == ::globalScope); // There must be a scope element in the scope stack.
    SgJavaPackageDeclaration *package_declaration = findPackageDeclaration(package_name);
    ROSE_ASSERT(package_declaration);
    SgClassDefinition *package_definition = package_declaration -> get_definition();
    ROSE_ASSERT(package_definition);

    ROSE_ASSERT(! ::currentSourceFile -> get_package());
    SgJavaPackageStatement *package_statement = SageBuilder::buildJavaPackageStatement(convertJavaStringToCxxString(env, java_package_name));
    package_statement -> set_parent(package_definition);
    setJavaSourcePosition(package_statement, env, jToken);
    ::currentSourceFile -> set_package(package_statement);

    ROSE_ASSERT(! ::currentSourceFile -> get_import_list());
    //
    // DQ (11/20/2013): Fixed to include source position information (and parent, etc.)
    // Actually, there are derived from SgSupport instead of SgLocatedNode, so they don't have a source position, but they do have a parent.
    //
    SgJavaImportStatementList* import_statement_list = new SgJavaImportStatementList();
    import_statement_list -> set_parent(::currentSourceFile);
    // setJavaSourcePosition(import_statement_list, env, jToken);
    ::currentSourceFile -> set_import_list(import_statement_list);


    ROSE_ASSERT(! ::currentSourceFile -> get_class_list());
    //
    // DQ (11/20/2013): Fixed to include source position information (and parent, etc.)
    // Actually, there are derived from SgSupport instead of SgLocatedNode, so they don't have a source position, but they do have a parent.
    //
    SgJavaClassDeclarationList* class_declaration_list = new SgJavaClassDeclarationList();
    class_declaration_list -> set_parent(package_definition);
    // setJavaSourcePosition(class_declaration_list, env, jToken);
    ::currentSourceFile -> set_class_list(class_declaration_list);

    //
    // Tag the package so that the unparser can process its containing user-defined types.
    //
    AstRegExAttribute *attribute =  new AstRegExAttribute(package_name);
    package_definition -> setAttribute("translated_package", attribute);
    astJavaScopeStack.push(package_definition); // Push the package onto the scopestack.

    // Example of how to get the string...but we don't really use the absolutePathFilename in this function.
    const char *absolutePathFilename = env -> GetStringUTFChars(java_filename, NULL);
    ROSE_ASSERT(absolutePathFilename != NULL);
    // printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration absolutePathFilename = %s \n", absolutePathFilename);
    env -> ReleaseStringUTFChars(java_filename, absolutePathFilename);

    // This is already setup by ROSE as part of basic file initialization before calling ECJ.
    ROSE_ASSERT(Rose::Frontend::Java::Ecj::Ecj_globalProjectPointer != NULL);

// TODO: Remove this! 12/09/13
//    astJavaComponentStack.push(astJavaScopeStack.top()); // To mark the end of the list of components in this Compilation unit.
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclarationEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclarationEnd() \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());
    SgClassDefinition *package = astJavaScopeStack.popPackage();

 // DQ (3/25/2017): Eliminate Clang warning for unused variable (define a valid usage).
    ROSE_ASSERT(package != NULL);

// TODO: Remove this!
/*
    //
    //
    //
    if (::currentSourceFile -> attributeExists("contains_wide_characters")) {
        AstRegExAttribute *attribute = (AstRegExAttribute *) ::currentSourceFile -> getAttribute("contains_wide_characters");
        string error_message = "Java-ROSE error: At least one non-ASCII character with value " + attribute -> expression + " encountered in a string literal.";
        cout << endl << error_message << " in file "
             << ::currentSourceFile -> getFileName()
             << endl << endl;
        cout.flush();
        if (! ::currentSourceFile -> attributeExists("error")) {
            ::currentSourceFile -> setAttribute("error", new AstRegExAttribute(error_message));
        }
    }

    //
    // Finally, we make sure that the import_list and the class_list associated with this file have a location
    //
    SgJavaImportStatementList *import_list = ::currentSourceFile -> get_import_list();
    if (import_list -> get_file_info() == NULL) { // no location yet?
        setJavaSourcePosition(import_list, env, jToken);
    }
    SgJavaClassDeclarationList *class_list = ::currentSourceFile -> get_class_list();
    if (class_list -> get_file_info() == NULL) { // no location yet?
        setJavaSourcePosition(class_list, env, jToken);
    }
*/

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionCompilationUnitDeclarationEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionEcjFatalCompilationErrors(JNIEnv *env, jclass, jstring java_full_file_name) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionSkipErroneousCompilationUnitDeclarationEnd() \n");

    //
    // The scope stack should contain only the global scope, the component stack should be empty
    // and the project should have been set.
    //
    ROSE_ASSERT(astJavaScopeStack.size() == 1);
    ROSE_ASSERT(astJavaComponentStack.size() == 0);
    ROSE_ASSERT(::project);

    string full_file_name = convertJavaStringToCxxString(env, java_full_file_name);
    SgSourceFile *source_file = isSgSourceFile((*::project)[full_file_name]);
    ROSE_ASSERT(source_file);
    ROSE_ASSERT(source_file -> get_file_info());
    source_file -> set_ecjErrorCode(1);
    source_file -> setAttribute("error", new AstRegExAttribute("Ecj fatal compilation errors detected"));

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionCompilationUnitDeclarationEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclarationError(JNIEnv *env, jclass, jstring java_error_message, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclarationEnd() \n");

    string error_message = convertJavaStringToCxxString(env, java_error_message);
    ROSE_ASSERT(! astJavaScopeStack.empty());

    //
    // Clear out the scope stack down to its bottom element: the global scope.
    //
    while (astJavaScopeStack.size() > 1) {
        SgNode *node = astJavaScopeStack.pop();
        if (node -> get_parent() == NULL) { // If this node is an orphan, assign an arbitrary safe parent it.
// TODO: Remove this !
/*
cout << "(1) Orphan scope node "
     << node -> class_name().c_str()
     << endl;
cout.flush();
*/
            node -> set_parent(::globalScope);
        }
    }

    ROSE_ASSERT(astJavaScopeStack.size() == 1);
    ROSE_ASSERT(isSgGlobal(astJavaScopeStack.top()) == ::globalScope);


    //
    // Empty out the component stack.
    //
    while (! astJavaComponentStack.empty()) {
        SgNode *node = astJavaComponentStack.pop();
        if (node -> get_parent() == NULL) { // If this node is an orphan, assign an arbitrary safe parent it.
// TODO: Remove this !
/*
cout << "(2) Orphan Component node "
     << node -> class_name().c_str()
     << endl;
cout.flush();
*/
            node -> set_parent(::globalScope);
        }
    }

    //
    // If this file was not already tagged as an erroneous file, do so now.
    //
    if (! ::currentSourceFile -> attributeExists("error")) {
        ::currentSourceFile -> setAttribute("error", new AstRegExAttribute(error_message));
    }

// TODO: Remove this !
/*
    //
    // make sure the source file has a package.
    //
    if (::currentSourceFile -> get_package() == NULL) {
cout << "Adding a package to source file "
     << ::currentSourceFile -> getFileName()
     << endl;
cout.flush();

        SgJavaPackageStatement *package_statement = new SgJavaPackageStatement("");
        package_statement -> set_parent(::globalScope);
        package_statement -> set_firstNondefiningDeclaration(package_statement);
        setJavaSourcePosition(package_statement, env, jToken);
        ::currentSourceFile -> set_package(package_statement);
    }
else{
cout << "Source file "
     << ::currentSourceFile -> getFileName()
     << " already has a package statement"
     << endl;
cout.flush();
}
*/

    cout << "ERROR encountered while translating "
         << ::currentSourceFile -> getFileName()
         << endl;
    cout.flush();

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionCompilationUnitDeclarationEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclaration(JNIEnv *env, jclass,
                                                              jstring java_package_name,
                                                              jstring java_type_name,
                                                              jint num_annotations,
                                                              jboolean has_super_class,
                                                              jboolean java_is_annotation_interface,
                                                              jboolean java_is_interface,
                                                              jboolean java_is_enum,
                                                              jboolean java_is_abstract,
                                                              jboolean java_is_final,
                                                              jboolean java_is_private,
                                                              jboolean java_is_public,
                                                              jboolean java_is_protected,
                                                              jboolean java_is_static,
                                                              jboolean java_is_strictfp,
                                                              jobject jToken)
{
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgClassDeclaration \n");

    // We could provide a constructor for "SgName" that takes a "jstring".  This might help support a simpler interface.
    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

// TODO: Remove this
/*
cout << "Here 6 with "
     << "; package_name = " << package_name
     << "; type_name = " << type_name
 << endl;
cout.flush();
*/
    SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);

    bool is_annotation_interface = java_is_annotation_interface;
    bool is_interface = java_is_interface;
    bool is_enum = java_is_enum;

    bool is_abstract = java_is_abstract;
    bool is_final = java_is_final;
    bool is_private = java_is_private;
    bool is_public = java_is_public;
    bool is_protected = java_is_protected;
    bool is_static = java_is_static;
    bool is_strictfp = java_is_strictfp;

    if (SgProject::get_verbose() > 0)
        printf ("Build class type: name = %s \n", type_name.str());

    ROSE_ASSERT(astJavaScopeStack.top() != NULL);

    SgClassDeclaration *class_declaration = (SgClassDeclaration *) type -> getAssociatedDeclaration() -> get_definingDeclaration();
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition);

    if (num_annotations > 0) {
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astJavaComponentStack.popExpression();
            annotation -> set_parent(class_declaration);
            annotations_attribute -> setNode(annotation, i);
        }
        class_declaration -> setAttribute("annotations", annotations_attribute);
    }

    class_declaration -> setAttribute("sourcefile", new AstSgNodeAttribute(::currentSourceFile));
    class_declaration -> set_explicit_annotation_interface(is_annotation_interface);      // Identify whether or not this is an annotation interface.
    class_declaration -> set_explicit_interface(is_annotation_interface || is_interface); // Identify whether or not this is an interface.
    class_declaration -> set_explicit_enum(is_enum);                                      // Identify whether or not this is an enum.

    if (is_abstract && (! is_annotation_interface) && (! is_interface) && (! is_enum)) // Enum should not be marked as abstract; Interfaces need not be marked as abstract
         class_declaration -> get_declarationModifier().setJavaAbstract();
    else class_declaration -> get_declarationModifier().unsetJavaAbstract();
    if (is_final && (! is_enum)) // Enum should not be marked as final
         class_declaration -> get_declarationModifier().setFinal();
    else class_declaration -> get_declarationModifier().unsetFinal();
    if (is_strictfp)
      {
        ; // charles4 - TODO: there is currently no place to hang this information.
      }

    class_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);
    if (is_private) {
        class_declaration -> get_declarationModifier().get_accessModifier().setPrivate();
    }
    if (is_public) {
        class_declaration -> get_declarationModifier().get_accessModifier().setPublic();
    }
    if (is_protected) {
        class_declaration -> get_declarationModifier().get_accessModifier().setProtected();
    }

    class_declaration -> get_declarationModifier().get_storageModifier().set_modifier(SgStorageModifier::e_unknown);
    if (is_static && (! is_annotation_interface) && (! is_interface)) { // Interfaces need not be marked as static
        class_declaration -> get_declarationModifier().get_storageModifier().setStatic();
    }

    // TODO: We need the next 4 lines for EDG4 [DONE]
    SgClassDeclaration *nondefining_class_declaration = isSgClassDeclaration(class_declaration -> get_firstNondefiningDeclaration());
    ROSE_ASSERT(nondefining_class_declaration);
    nondefining_class_declaration -> get_declarationModifier().get_accessModifier().set_modifier(class_declaration -> get_declarationModifier().get_accessModifier().get_modifier());
    ROSE_ASSERT(nondefining_class_declaration -> get_declarationModifier().get_accessModifier().get_modifier() == class_declaration -> get_declarationModifier().get_accessModifier().get_modifier());

    //
    //
    //
    AstSgNodeListAttribute *extension_names_attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("extensions");
    ROSE_ASSERT(extension_names_attribute);
    string extension_names = getExtensionNames(extension_names_attribute -> getNodeList(), class_declaration, has_super_class);
    AstRegExAttribute *extension_type_names_attribute = (AstRegExAttribute *) class_definition -> getAttribute("extension_type_names");
    ROSE_ASSERT(extension_type_names_attribute);
    extension_type_names_attribute -> expression = extension_names;

    AstSgNodeAttribute *type_space_attribute = (AstSgNodeAttribute *) class_declaration -> getAttribute("type_space");
    ROSE_ASSERT(type_space_attribute);
    SgScopeStatement *type_space = (SgScopeStatement *) type_space_attribute -> getNode();
    astJavaScopeStack.push(type_space);           // Push the type parameters scope for this type onto the stack.
    astJavaScopeStack.push(class_definition);     // Open new scope for this type.
    astJavaComponentStack.push(class_definition); // To mark the end of the list of components in this type.

    if (SgProject::get_verbose() > 0)
        astJavaScopeStack.top() -> get_file_info() -> display("source position in Java_JavaParser_cactionTypeDeclaration(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionTypeDeclaration() \n");
}


/*
JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclarationHeader(JNIEnv *env, jclass,
                                                                    jboolean java_has_super_class,
                                                                    jint num_interfaces,
                                                                    jint num_parameters,
                                                                    jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgClassDeclaration (cactionTypeDeclarationHeader) \n");

    bool has_super_class = java_has_super_class;

    ROSE_ASSERT(astJavaScopeStack.top() != NULL);
    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    if (SgProject::get_verbose() > 0)
        printf ("Build class type: name = %s \n", class_definition -> get_qualified_name().str());

    //
    // Process the interfaces for this type, if any.
    //
    for (int i = 0; i < num_interfaces; i++) {
         SgType *type = astJavaComponentStack.popType();
    }

    //
    // Add Super class to the current Class definition.
    //
    if (has_super_class) {
        SgType *type = astJavaComponentStack.popType();
    }

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionTypeDeclarationHeader() (cactionTypeDeclarationHeader) \n");
}
*/


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclarationEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgClassDeclaration (cactionTypeDeclarationEnd) \n");

    ROSE_ASSERT(astJavaScopeStack.top() != NULL);
    SgClassDefinition *class_definition = astJavaScopeStack.popClassDefinition(); // pop the class definition
    ROSE_ASSERT(class_definition);

    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());  // Pop the type parameters scope from the stack.
    ROSE_ASSERT(type_space);

    SgClassDeclaration *class_declaration = isSgClassDeclaration(class_definition -> get_declaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration != NULL);

    ROSE_ASSERT(class_definition == astJavaComponentStack.top());
    astJavaComponentStack.pop(); // remove the class definition from the stack

    //
    // Now that we are processing declarations in two passes, type declarations are always entered in their
    // respective scope during the first pass. Thus, in general, we don't need to process a type declaration
    // here unless it is an Anonymous type that needs to be associated with an Allocation expression.
    //
// TODO: Remove this!
/*
    if (class_declaration -> attributeExists("anonymous")) {
        astJavaComponentStack.push(class_declaration);
    }
*/
    if (class_declaration -> get_explicit_anonymous()) {
        astJavaComponentStack.push(class_declaration);
    }
    else { // Check if this is a type-level type. If so, add it to its sourcefile list.
        SgClassDefinition *package_definition = isSgClassDefinition(astJavaScopeStack.top());
// TODO: Remove this!
/*
if (! package_definition) {
cout << "A package definition was expected, but we found a " << astJavaScopeStack.top() -> class_name().c_str() << endl;
cout.flush();
}
*/
        if (package_definition) { // if the type is a local type, its scope is an SgBasicBlock
            SgJavaPackageDeclaration *package_declaration = isSgJavaPackageDeclaration(package_definition -> get_parent());
            if (package_declaration) {
// TODO: Remove this!
/*
                SgJavaImportStatementList *import_list = ::currentSourceFile -> get_import_list();
                if (import_list -> get_file_info() == NULL) { // If the import list is empty
                    setJavaSourcePosition(import_list, env, jToken);
                }
*/
                SgJavaClassDeclarationList *class_list = ::currentSourceFile -> get_class_list();
// TODO: Remove this!
/*
                if (class_list -> get_file_info() == NULL) { // The first
                    setJavaSourcePosition(class_list, env, jToken);
                }
*/
                class_list -> get_java_class_list().push_back(class_declaration);

// TODO: Remove this!
/*
cout << "The type " << class_declaration -> get_qualified_name() << " is a top level type declaration of " << ::currentSourceFile -> getFileName() << endl
     << "It is associated with file name " << class_declaration -> get_file_info() -> get_filenameString() << endl
     << "It contains " << class_definition -> get_members().size() << " statements" << endl;
for (int i = 0; i < class_definition -> get_members().size(); i++) {
  SgNode *statement = class_definition -> get_members()[i];
  cout << "    " << statement -> class_name()  << " statements" << endl;
}
cout.flush();
*/
            }
        }
// TODO: Remove this!
/*
else {
cout << "NO, the type " << class_declaration -> get_qualified_name() << " is NOT a top level type declaration" << endl;
cout.flush();
}
*/
    }

    ROSE_ASSERT(astJavaScopeStack.top() != NULL);
    if (SgProject::get_verbose() > 0)
        astJavaScopeStack.top() -> get_file_info() -> display("source position in Java_JavaParser_cactionTypeDeclarationEnd(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionTypeDeclarationEnd() (cactionTypeDeclarationEnd) \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclaration(JNIEnv *env, jclass, jstring java_string, jint constructor_index, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgMemberFunctionDeclaration (constructor) \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
    ROSE_ASSERT(attribute);
    SgFunctionDefinition *constructor_definition = isSgFunctionDefinition(attribute -> getNode(constructor_index));
    ROSE_ASSERT(constructor_definition);
    SgMemberFunctionDeclaration *constructor_declaration = isSgMemberFunctionDeclaration(constructor_definition -> get_declaration());
    ROSE_ASSERT(constructor_declaration);
    AstSgNodeAttribute *type_space_attribute = (AstSgNodeAttribute *) constructor_declaration -> getAttribute("type_space");
    ROSE_ASSERT(type_space_attribute);
    SgScopeStatement *type_space = (SgScopeStatement *) type_space_attribute -> getNode();
    ROSE_ASSERT(type_space);
    astJavaScopeStack.push(type_space);
    astJavaScopeStack.push(constructor_definition);
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    //TODO: REMOVE this!!!
    //
    // We start by pushing a VOID return type to make the constructor look like a method.
    //
    //    astJavaComponentStack.push(SgTypeVoid::createType()); 
}


JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclarationHeader(JNIEnv *env, jclass,
                                                                           jstring java_string, 
                                                                           jboolean java_is_public,
                                                                           jboolean java_is_protected,
                                                                           jboolean java_is_private,
                                                                           jint java_numberOfTypeParameters,
                                                                           jint java_numberOfArguments,
                                                                           jint java_numberOfThrownExceptions,
                                                                           jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("End of SgMemberFunctionDeclaration (constructor) \n");

    SgName name   = convertJavaStringToCxxString(env, java_string);
    bool is_public = java_is_public;
    bool is_protected = java_is_protected;
    bool is_private = java_is_private;

    // DQ (7/31/2011): Add more precise handling of the statement stack.
    // This does not count (include) explicit constructor calls...

 // DQ (3/25/2017): Eliminate Clang warning for unused variable.
 // int number_of_type_parameters = java_numberOfTypeParameters;
 // int numberOfArguments = java_numberOfArguments;

    int numberOfThrownExceptions = java_numberOfThrownExceptions;

    //
    // Pop the exception types; reverse the list of exception types; create an output string of the names of the tyoes.
    //
    SgTypePtrList exception_list;
    for (int i = 0; i < numberOfThrownExceptions; i++) { // Pop the Thrown types
        SgType *type = astJavaComponentStack.popType();
        exception_list.push_back(type);

    }
    for (int i = 0, k = numberOfThrownExceptions - 1; i < k; i++, k--) { // reverse the list to restore the original order
        SgType *temp = exception_list[i];
        exception_list[i] = exception_list[k];
        exception_list[k] = temp;
    }
    string exceptions = "";
    for (int i = 0;  i < numberOfThrownExceptions; i++) {
        if (i > 0)
            exceptions += ", ";
        exceptions += getTypeName(exception_list[i]);
    }

    //
    //
    //
    SgFunctionDefinition *constructor_definition = isSgFunctionDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(constructor_definition -> get_body() != NULL);
    astJavaScopeStack.push(constructor_definition -> get_body());
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    setJavaSourcePosition(constructor_definition -> get_body(), env, jToken);

    SgFunctionDeclaration *constructor_declaration = constructor_definition -> get_declaration();
    constructor_declaration -> set_exceptionSpecification(exception_list);
    if (exceptions.size()) {
        constructor_declaration -> setAttribute("exception", new AstRegExAttribute(exceptions));
    }

    // Since this is a constructor, set it explicitly as such.
    constructor_declaration -> get_specialFunctionModifier().setConstructor();

    // By default, the access modifier is set to unknown
    constructor_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    // Set the access modifiers (shared between C++ and Java).
    if (is_public) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Public \n");
        constructor_declaration -> get_declarationModifier().get_accessModifier().setPublic();
    }
    else if (is_protected) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Protected \n");
        constructor_declaration -> get_declarationModifier().get_accessModifier().setProtected();
    }
    else if (is_private) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Private \n");
        constructor_declaration -> get_declarationModifier().get_accessModifier().setPrivate();
    }

    // TODO: We need the next 3 lines for EDG4 [DONE]
    SgMemberFunctionDeclaration *nondefining_constructor_declaration = isSgMemberFunctionDeclaration(constructor_declaration -> get_firstNondefiningDeclaration());
    ROSE_ASSERT(nondefining_constructor_declaration);
    nondefining_constructor_declaration -> get_declarationModifier().get_accessModifier().set_modifier(constructor_declaration -> get_declarationModifier().get_accessModifier().get_modifier());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclarationEnd(JNIEnv *env, jclass, jint num_annotations, jint num_statements, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("End of SgMemberFunctionDeclaration (constructor) \n");

    if (SgProject::get_verbose() > 0)
        printf ("cactionConstructorDeclarationEnd(): number of statements = %d\n", num_statements);

    // Pop the constructor body...
    ROSE_ASSERT(! astJavaScopeStack.empty());
    SgBasicBlock *constructor_body = astJavaScopeStack.popBasicBlock();
    for (int i = 0; i  < num_statements; i++) {
        SgStatement *statement = astJavaComponentStack.popStatement();
        if (SgProject::get_verbose() > 2) {
            cerr << "(3) Adding statement "
                 << statement -> class_name()
                 << " to a constructor declaration block"
                 << endl;
            cerr.flush();
        }
        constructor_body -> prepend_statement(statement);
    }

    SgFunctionDefinition *constructor_definition = astJavaScopeStack.popFunctionDefinition();
    SgMemberFunctionDeclaration *constructor_declaration = isSgMemberFunctionDeclaration(constructor_definition -> get_declaration());

    if (num_annotations > 0) {
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astJavaComponentStack.popExpression();
            annotation -> set_parent(constructor_declaration);
            annotations_attribute -> setNode(annotation, i);
        }
        constructor_declaration -> setAttribute("annotations", annotations_attribute);
    }

    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());
    ROSE_ASSERT(type_space);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionExplicitConstructorCall(JNIEnv *env, jclass, jobject jToken) {
    // Build a member function call...
    if (SgProject::get_verbose() > 0)
        printf ("Build a explicit constructor function call \n");

    // Do Nothing on the way down!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionExplicitConstructorCallEnd(JNIEnv *env, jclass,
                                                                         jboolean java_is_implicit_super,
                                                                         jboolean java_is_super,
                                                                         jboolean java_has_qualification,
                                                                         jstring java_package_name,
                                                                         jstring java_type_name,
                                                                         jint constructor_index,
                                                                         jint java_number_of_type_arguments,
                                                                         jint java_number_of_arguments,
                                                                         jobject jToken) {
    // Build a member function call...
    if (SgProject::get_verbose() > 0)
        printf ("Build a explicit constructor function call END \n");

 // DQ (3/25/2017): Eliminate Clang warning for unused variable.
 // bool is_implicit_super = java_is_implicit_super;

    bool is_super = java_is_super;
    bool has_qualification = java_has_qualification;
    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);
// TODO: Remove this!!!
//    int number_of_parameters = java_number_of_parameters;
    int number_of_type_arguments = java_number_of_type_arguments;
    int number_of_arguments = java_number_of_arguments;

// TODO: Remove this!!!
//    ROSE_ASSERT(number_of_type_arguments == 0 && "! yet support type arguments");

/*
    //
    // The astJavaComponentStack has all of the types of the parameters of the function being called. Note that
    // it is necessary to use the original types of the formal parameters of the function in order to find the
    // perfect match for the function.
    //
    list<SgType *> parameter_types;
    for (int i = 0; i < number_of_parameters; i++) { // reverse the arguments' order
        SgType *type = astJavaComponentStack.popType();
        parameter_types.push_front(type);
    }
*/

    // The astJavaComponentStack has all of the arguments to the function call.
    SgExprListExp *arguments = SageBuilder::buildExprListExp();
    for (int i = 0; i < number_of_arguments; i++) { // reverse the arguments' order
        SgExpression *expr = astJavaComponentStack.popExpression();
        arguments -> prepend_expression(expr);
    }
    setJavaSourcePosition(arguments, env, jToken);

    //
    // Are there any Type arguments?
    //
    string parm_names;
    AstSgNodeListAttribute *parameter_types_attribute = NULL;
    if (number_of_type_arguments > 0) {
        parm_names = "";
        parameter_types_attribute = new AstSgNodeListAttribute();
        for (int i = number_of_type_arguments - 1; i >= 0; i--) { // Note that we are reversing the type parameters here!
            SgType *type_argument = astJavaComponentStack.popType();
            ROSE_ASSERT(type_argument);
            parameter_types_attribute -> setNode(type_argument, i);

            string name = getTypeName(type_argument);
            parm_names = (parm_names.size() == 0 ? name : (name + ", " + parm_names));
        }
        parm_names = "<" + parm_names + ">";
    }

    SgExpression *qualification = (has_qualification ? astJavaComponentStack.popExpression() : NULL);

    /*
    //
    //
    //
    SgClassDefinition *class_definition = getCurrentTypeDefinition();
    ROSE_ASSERT(class_definition);

    if (qualification != NULL) {
        SgClassType *class_type = isSgClassType(qualification -> get_type());
        ROSE_ASSERT(class_type);

        SgDeclarationStatement *declaration = class_type -> get_declaration() -> get_definingDeclaration();
        SgClassDeclaration *class_declaration = (SgClassDeclaration *) declaration;
        class_definition = isSgClassDefinition(class_declaration -> get_definition());
        ROSE_ASSERT(class_definition);
    }

    if (is_super) {
        vector<SgBaseClass *> &inheritances = class_definition -> get_inheritances();
        if (inheritances.size() == 0 || inheritances[0] -> get_base_class() -> get_explicit_interface()) { // no super class specified?
            class_definition = ::ObjectClassDefinition;
        }
        else {
            class_definition = inheritances[0] -> get_base_class() -> get_definition(); // get the super class definition
        }
    }
    */

    //
    //
    //
// TODO: Remove this
//cout << "Here 7" << endl;
//cout.flush();
    SgType *containing_type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
    ROSE_ASSERT(containing_type);
    SgClassDefinition *containing_class_definition = isSgClassDeclaration(containing_type -> getAssociatedDeclaration() -> get_definingDeclaration()) -> get_definition();
    ROSE_ASSERT(containing_class_definition);
/*
if (containing_class_definition != class_definition){
cout << "containing_class_definition is: "
     << containing_class_definition -> get_qualified_name().getString()
     << endl
     << "class_definition is: "
     << class_definition -> get_qualified_name().getString()
     << endl;
cout.flush();
}
    ROSE_ASSERT(containing_class_definition == class_definition);
*/
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) containing_class_definition -> getAttribute("method-members-map");
    ROSE_ASSERT(attribute);
    SgFunctionDefinition *constructor_definition = isSgFunctionDefinition(attribute -> getNode(constructor_index));
    ROSE_ASSERT(constructor_definition);
    SgMemberFunctionDeclaration *constructor_declaration = isSgMemberFunctionDeclaration(constructor_definition -> get_declaration());
    ROSE_ASSERT(constructor_declaration);
    SgMemberFunctionType *constructor_type = isSgMemberFunctionType(constructor_declaration -> get_type());
    ROSE_ASSERT(constructor_type);
    SgMemberFunctionSymbol *constructor_symbol = isSgMemberFunctionSymbol(constructor_declaration -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(constructor_symbol);

 /*
    SgClassSymbol *class_symbol = isSgClassSymbol(class_definition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(class_symbol != NULL);
    SgName name = class_definition -> get_declaration() -> get_name();

    SgMemberFunctionSymbol *function_symbol = findFunctionSymbolInClass(class_definition, name, parameter_types);
    ROSE_ASSERT(function_symbol);
    SgFunctionCallExp *function_call_exp = SageBuilder::buildFunctionCallExp(function_symbol, arguments);
*/

    SgFunctionCallExp *function_call_exp = SageBuilder::buildFunctionCallExp(constructor_symbol, arguments);
    setJavaSourcePosition(function_call_exp, env, jToken);
    function_call_exp -> setAttribute("<init>", new AstRegExAttribute(is_super ? "super" : "this"));
    if (parameter_types_attribute != NULL) {
        function_call_exp -> setAttribute("invocation_parameter_types", parameter_types_attribute);
        function_call_exp -> setAttribute("function_parameter_types", new AstRegExAttribute(parm_names));
    }

    SgExpression *expr_for_function = function_call_exp;
    if (qualification != NULL) {
        expr_for_function = SageBuilder::buildBinaryExpression<SgDotExp>(qualification, expr_for_function);
        setJavaSourcePosition(expr_for_function, env, jToken);
    }

    SgExprStatement *expression_statement = SageBuilder::buildExprStatement(expr_for_function);
    ROSE_ASSERT(expression_statement != NULL);
    setJavaSourcePosition(expression_statement, env, jToken);
    ROSE_ASSERT(expression_statement != NULL);

    // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
    // printf ("Previously calling appendStatement in cactionExplicitConstructorCallEnd() \n");
    // appendStatement(expressionStatement);
    astJavaComponentStack.push(expression_statement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclaration(JNIEnv *env, jclass, jstring java_string, jint method_index, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgMemberFunctionDeclaration for method with index %d\n", method_index);

    SgName name = convertJavaStringToCxxString(env, java_string);

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
    ROSE_ASSERT(attribute);
    SgFunctionDefinition *method_definition = isSgFunctionDefinition(attribute -> getNode(method_index));
    ROSE_ASSERT(method_definition);
    SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
    ROSE_ASSERT(method_declaration);
    AstSgNodeAttribute *type_space_attribute = (AstSgNodeAttribute *) method_declaration -> getAttribute("type_space");
    ROSE_ASSERT(type_space_attribute);
    SgScopeStatement *type_space = (SgScopeStatement *) type_space_attribute -> getNode();
    ROSE_ASSERT(type_space);
    ROSE_ASSERT(method_declaration -> get_type());
    ROSE_ASSERT(method_declaration -> get_type() -> get_return_type());
    method_declaration -> setAttribute("type", new AstRegExAttribute(method_declaration -> attributeExists("alternate-return-type") // TODO: This is here to bypass a bug!!!  See Java_JavaParser_cactionUpdateMethodSupportStart.
                                                                           ? ((AstRegExAttribute *) method_declaration -> getAttribute("alternate-return-type")) -> expression
                                                                           : getTypeName(method_declaration -> get_type() -> get_return_type())));
/*
cout << "The return type for method "
     << name.getString()
     << " is ("
     << method_declaration -> get_type() -> get_return_type() -> class_name()
     << ") "
     << getTypeName(method_declaration -> get_type() -> get_return_type())
<< endl;
cout.flush();
*/
    ROSE_ASSERT(method_declaration -> attributeExists("type"));
    astJavaScopeStack.push(type_space);
    astJavaScopeStack.push(method_definition);
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Build a SgMemberFunctionDeclaration for method with index %d\n", method_index);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclarationHeader(JNIEnv *env, jclass,
                                                                      jstring java_string,
                                                                      jboolean java_is_abstract,
                                                                      jboolean java_is_native,
                                                                      jboolean java_is_static,
                                                                      jboolean java_is_final,
                                                                      jboolean java_is_synchronized,
                                                                      jboolean java_is_public,
                                                                      jboolean java_is_protected,
                                                                      jboolean java_is_private,
                                                                      jboolean java_is_strictfp,
                                                                      jint java_numberOfTypeParameters,
                                                                      jint java_numberOfArguments,
                                                                      jint java_numberOfThrownExceptions,
                                                                      jobject jToken) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Build Method Header for method  %s\n", name.getString().c_str());

    bool isAbstract     = java_is_abstract;
    bool isNative       = java_is_native;
    bool isStatic       = java_is_static;
    bool isFinal        = java_is_final;
    bool isSynchronized = java_is_synchronized;
    bool isPublic       = java_is_public;
    bool isProtected    = java_is_protected;
    bool isPrivate      = java_is_private;
    bool isStrictfp     = java_is_strictfp;

 // DQ (3/25/2017): Eliminate Clang warning for unused variable.
 // int number_of_type_parameters = java_numberOfTypeParameters;
 // int numberOfArguments         = java_numberOfArguments;

    int numberOfThrownExceptions  = java_numberOfThrownExceptions;

    //
    // Pop the exception types; reverse the list of exception types; create an output string of the names of the tyoes.
    //
    SgTypePtrList exception_list;
    for (int i = 0; i < numberOfThrownExceptions; i++) { // Pop the Thrown types
        SgType *type = astJavaComponentStack.popType();
        exception_list.push_back(type);

    }
    for (int i = 0, k = numberOfThrownExceptions - 1; i < k; i++, k--) { // reverse the list to restore the original order
        SgType *temp = exception_list[i];
        exception_list[i] = exception_list[k];
        exception_list[k] = temp;
    }
    string exceptions = "";
    for (int i = 0;  i < numberOfThrownExceptions; i++) {
// TODO: Remove this
/*
        SgClassType *c_type = isSgClassType(exception_list[i]);
        SgJavaParameterizedType *p_type = isSgJavaParameterizedType(exception_list[i]);
        SgJavaQualifiedType *q_type = isSgJavaQualifiedType(exception_list[i]);
        string throw_name = (c_type ? getTypeName(c_type)
                                   : p_type ? getTypeName(p_type)
                                            : getTypeName(q_type));
        ROSE_ASSERT(c_type || p_type || q_type);

        exceptions += throw_name;
*/

        if (i > 0)
            exceptions += ", ";
        exceptions += getTypeName(exception_list[i]);
    }

// TODO: Remove this
/*
    for (int i = 0; i < numberOfArguments; i++) { // Pop the argument types
        SgType *type = astJavaComponentStack.popType();
cout << "Popping argument of type "
     << getTypeName(type)
     << " for method "
     << name.getString()
<< endl;
cout.flush();
    }
*/

    //
    //
    //
    SgFunctionDefinition *method_definition = isSgFunctionDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(method_definition -> get_body() != NULL);
    astJavaScopeStack.push(method_definition -> get_body());
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    setJavaSourcePosition(method_definition -> get_body(), env, jToken);

    SgFunctionDeclaration *method_declaration = method_definition -> get_declaration();
    method_declaration -> set_exceptionSpecification(exception_list);
    if (exceptions.size()) {
        method_declaration -> setAttribute("exception", new AstRegExAttribute(exceptions));
    }

    // By default, the access modifier is set to unknown
    method_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    // Set the Java specific modifiers
    if (isAbstract) {
        method_declaration -> get_declarationModifier().setJavaAbstract();
    }

    // Set the Java specific modifiers
    if (isNative) {
        method_declaration -> get_functionModifier().setJavaNative();
    }

    // Set the specific modifier, this modifier is common to C/C++.
    if (isStatic) {
        method_declaration -> get_declarationModifier().get_storageModifier().setStatic();
    }

    // Set the modifier (shared between PHP and Java).
    if (isFinal) {
        method_declaration -> get_declarationModifier().setFinal();
    }

    // DQ (8/13/2011): Added more modifiers.
    if (isSynchronized) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Synchronized \n");
        method_declaration -> get_functionModifier().setJavaSynchronized();
    }

    // Set the access modifiers (shared between C++ and Java).
    if (isPublic) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Public \n");
        method_declaration -> get_declarationModifier().get_accessModifier().setPublic();
    }

    if (isProtected) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Protected \n");
        method_declaration -> get_declarationModifier().get_accessModifier().setProtected();
    }

    if (isPrivate) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Private \n");
        method_declaration -> get_declarationModifier().get_accessModifier().setPrivate();
    }

    // Set the Java specific modifier for strict floating point (defined for functions).
    if (isStrictfp) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as strictfp \n");
        method_declaration -> get_functionModifier().setJavaStrictfp();
    }
    // TODO: We need the next 3 lines for EDG4 [DONE]
    SgMemberFunctionDeclaration *nondefining_method_declaration = isSgMemberFunctionDeclaration(method_declaration -> get_firstNondefiningDeclaration());
    ROSE_ASSERT(nondefining_method_declaration);
    nondefining_method_declaration -> get_declarationModifier().get_accessModifier().set_modifier(method_declaration -> get_declarationModifier().get_accessModifier().get_modifier());

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Build Method header for method %s\n", name.getString().c_str());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclarationEnd(JNIEnv *env, jclass, jsize num_annotations, jsize num_statements, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Entering  cactionMethodDeclarationEnd (method) for %d statements\n", num_statements);

    // Pop the constructor body...
    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgBasicBlock *method_body = astJavaScopeStack.popBasicBlock(); // pop the body block
    for (int i = 0; i < num_statements; i++) {
         SgStatement *statement = astJavaComponentStack.popStatement();
         if (SgProject::get_verbose() > 2) {
             cerr << "(4) Adding statement "
                  << statement -> class_name()
                  << " to a method declaration block"
                  << endl;
             cerr.flush();
        }
        method_body -> prepend_statement(statement);
    }

    SgFunctionDefinition *method_definition = astJavaScopeStack.popFunctionDefinition();
    SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());

    if (num_annotations > 0) {
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astJavaComponentStack.popExpression();
            annotation -> set_parent(method_declaration);
            annotations_attribute -> setNode(annotation, i);
        }
        ROSE_ASSERT(method_declaration != NULL);
        method_declaration -> setAttribute("annotations", annotations_attribute);
    }

    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());
    ROSE_ASSERT(type_space != NULL);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting  cactionMethodDeclarationEnd (method) \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeParameterReference(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jint method_index, jstring java_type_parameter_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside cactionTypeParameterReference\n");

    SgName type_parameter_name = convertJavaStringToCxxString(env, java_type_parameter_name);

    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);
// TODO: Remove this
//cout << "Here 8" << endl;
//cout.flush();
    SgType *enclosing_type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
    ROSE_ASSERT(enclosing_type);
    SgClassDeclaration *class_declaration = isSgClassDeclaration(enclosing_type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition);
 
    //
    // Look in the type parameter scopes for the type in question.
    //
    SgClassSymbol *class_symbol = NULL;
    if (method_index >= 0) { // The type parameter is enclosed in a method in the enclosing type?
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
        ROSE_ASSERT(attribute);
        SgFunctionDefinition *method_definition = isSgFunctionDefinition(attribute -> getNode(method_index));
        if (method_definition) { // The method header is available!
            SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
            ROSE_ASSERT(method_declaration);
            AstSgNodeAttribute *type_space_attribute = (AstSgNodeAttribute *) method_declaration -> getAttribute("type_space");
            ROSE_ASSERT(type_space_attribute);
            SgScopeStatement *type_space = (SgScopeStatement *) type_space_attribute -> getNode();
            ROSE_ASSERT(type_space);
            class_symbol = lookupClassSymbolInScope(type_space, type_parameter_name);
        }
        else { // The method header not yet available, look for type in the Scope Stack!
            for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); class_symbol == NULL && i != astJavaScopeStack.end(); i++) {
                class_symbol = lookupClassSymbolInScope(*i, type_parameter_name);
            }
        }
    }
    else {
        AstSgNodeAttribute *type_space_attribute = (AstSgNodeAttribute *) class_declaration -> getAttribute("type_space");
        SgScopeStatement *type_space = isSgScopeStatement(type_space_attribute -> getNode());
        ROSE_ASSERT(type_space);
        class_symbol = lookupClassSymbolInScope(type_space, type_parameter_name);
    }

// TODO:  Remove this!
//    SgClassSymbol *class_symbol = lookupParameterTypeByName(type_parameter_name); // lookupSimpleNameTypeInClass(type_parameter_name, class_definition);

// TODO: Remove this !!!
/*
if (! class_symbol){
cout << "Could not find type parameter " << type_parameter_name.getString() << " enclosed in scope " << (isSgClassDefinition(astJavaScopeStack.top()) ? isSgClassDefinition(astJavaScopeStack.top()) -> get_qualified_name().getString() : astJavaScopeStack.top() -> class_name())
<< "; method_index = " << method_index 
  //<< "; Maybe it is in the containing class " << class_definition -> get_qualified_name().getString()
<< endl;
cout.flush();

cout << "...in the stack: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); i != astJavaScopeStack.end(); i++) {
cout << "    "
<< (isSgClassDefinition(*i) ? isSgClassDefinition(*i) -> get_qualified_name().getString()
                            : isSgFunctionDefinition(*i) ? (isSgFunctionDefinition(*i) -> get_declaration() -> get_name().getString() + "(...)")
                                                         : (*i) -> class_name())
<< " ("
<< ((unsigned long) (*i))
<< ")"
<< endl;
cout.flush();
}
}
*/
if (! class_symbol){
cout << "Could not find parameter " 
     << type_parameter_name.getString()
     << " in type "
     << class_declaration -> get_qualified_name().getString()
     << " in file "
     << ::currentSourceFile -> getFileName()
<< endl;
cout.flush();
}
    ROSE_ASSERT(class_symbol);

    SgJavaParameterType *parameter_type = isSgJavaParameterType(class_symbol -> get_type());
    ROSE_ASSERT(parameter_type);

    astJavaComponentStack.push(parameter_type);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting cactionTypeParameterReference\n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeReference(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside cactionTypeReference\n");

    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

// TODO: Remove this
//cout << "Here 9"
//     << "; package_name = " << package_name
//     << "; type_name = " << type_name
// << endl;
//cout.flush();
    SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
    ROSE_ASSERT(type != NULL);

// TODO: Remove this
//if (isSgClassType(type)) {
//SgClassType *class_type = isSgClassType(type);
//cout << "Came across type " << getTypeName(class_type) << endl;
//cout.flush();
//}

    astJavaComponentStack.push(type);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting cactionTypeReference\n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedTypeReference(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside cactionQualifiedTypeReference\n");

    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

    SgNamedType *parent_type = (SgNamedType *) astJavaComponentStack.popType();
    ROSE_ASSERT(parent_type -> getAssociatedDeclaration());

// TODO: Remove this
/*
    SgClassDeclaration *parent_class_declaration = isSgClassDeclaration(parent_type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(parent_class_declaration);
    SgClassDefinition *parent_class_definition = parent_class_declaration -> get_definition();
    ROSE_ASSERT(parent_class_definition);

    SgClassSymbol *class_symbol = lookupUniqueSimpleNameTypeInClass(type_name, parent_class_definition);
    ROSE_ASSERT(class_symbol);
    SgClassType *type = isSgClassType(class_symbol -> get_type()); // the type being dereferenced.
    ROSE_ASSERT(type);
*/

// TODO: Remove this
//cout << "Here 10" << endl;
//cout.flush();
    SgNamedType *type = isSgNamedType(lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */));
    ROSE_ASSERT(type != NULL);

    SgClassDeclaration *class_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);

    SgJavaQualifiedType *qualified_type = SageBuilder::getUniqueJavaQualifiedType(class_declaration, parent_type, type);

    astJavaComponentStack.push(qualified_type);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting cactionQualifiedTypeReference\n");
}



JNIEXPORT void JNICALL Java_JavaParser_cactionCatchArgument(JNIEnv *env, jclass, jstring java_argument_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a function argument \n");

    SgName argument_name = convertJavaStringToCxxString(env, java_argument_name);

    SgCatchOptionStmt *catch_option_stmt = SageBuilder::buildCatchOptionStmt();
    ROSE_ASSERT(catch_option_stmt != NULL);
    setJavaSourcePosition(catch_option_stmt, env, jToken);
    catch_option_stmt -> set_parent(astJavaScopeStack.top());
    astJavaScopeStack.push(catch_option_stmt);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArgument(JNIEnv *env, jclass, jstring java_argument_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a function argument \n");

    // TODO: Do Nothing ... At some point, this function should be removed!!!

    if (SgProject::get_verbose() > 0)
        printf ("Done Building a function argument \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCatchArgumentEnd(JNIEnv *env, jclass, jint num_annotations, jstring java_argument_name, jint num_types, jboolean java_is_final, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a function argument \n");

    SgName argument_name = convertJavaStringToCxxString(env, java_argument_name);

    bool is_final = java_is_final;

    if (SgProject::get_verbose() > 0)
        printf ("argument argument_name = %s \n", argument_name.str());

    ROSE_ASSERT(num_types > 0);
    SgType *argument_type = (num_types == 1 ? astJavaComponentStack.popType() : new SgJavaUnionType());
    if (num_types > 1) {
        SgJavaUnionType *union_type = isSgJavaUnionType(argument_type);
        SgTypePtrList type_list;
        type_list.resize(num_types);

        //
        // Pop the component types and place them in the list in the proper order.
        //
        for (int i = num_types - 1; i >= 0; i--) {
            type_list[i] = astJavaComponentStack.popType();
        }
        union_type -> set_type_list(type_list);
    }
    ROSE_ASSERT(argument_type);

    //
    // Create a variable declaration for the Catch Block parameter ... Make sure that it is inserted
    // in the proper scope.
    //
    SgCatchOptionStmt *catch_option_stmt = isSgCatchOptionStmt(astJavaScopeStack.top());
    ROSE_ASSERT(catch_option_stmt);
    SgVariableDeclaration *variable_declaration = SageBuilder::buildVariableDeclaration(argument_name, argument_type, NULL, catch_option_stmt);
    variable_declaration -> set_parent(catch_option_stmt);
    setJavaSourcePosition(variable_declaration, env, jToken);
    variable_declaration -> get_declarationModifier().get_accessModifier().setUnknown();  // make sure that all the access flags are off!!!
    variable_declaration -> get_declarationModifier().get_storageModifier().setUnknown(); // make sure that all the storage flags are off!!!
    variable_declaration -> get_declarationModifier().get_typeModifier().setUnknown();    // make sure that all the type flags are off!!!
    if (is_final) {
        variable_declaration -> get_declarationModifier().setFinal();
    }

    ROSE_ASSERT(variable_declaration -> get_variables().size() == 1);
    SgInitializedName *init_name = variable_declaration -> get_variables()[0];
// TODO: Remove this!
//cout << "Catch argument type " << getTypeName(argument_type)
//<< endl;
//cout.flush();
    init_name -> setAttribute("type", new AstRegExAttribute(getTypeName(argument_type)));


    if (num_annotations > 0) {
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astJavaComponentStack.popExpression();
            annotation -> set_parent(variable_declaration);
            annotations_attribute -> setNode(annotation, i);
        }
        variable_declaration -> setAttribute("annotations", annotations_attribute);
    }

    catch_option_stmt -> set_condition(variable_declaration);
    variable_declaration -> set_parent(catch_option_stmt);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArgumentEnd(JNIEnv *env, jclass, jint num_annotations, jstring java_argument_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a function argument \n");

    SgName argument_name = convertJavaStringToCxxString(env, java_argument_name);
    SgVariableSymbol *variable_symbol = lookupVariableByName(argument_name);
    ROSE_ASSERT(variable_symbol);
    SgInitializedName *initialized_name = variable_symbol -> get_declaration();
    ROSE_ASSERT(initialized_name);
    if (num_annotations > 0) {
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astJavaComponentStack.popExpression();
            annotation -> set_parent(initialized_name);
            annotations_attribute -> setNode(annotation, i);
        }
        initialized_name -> setAttribute("annotations", annotations_attribute);
    }

    if (SgProject::get_verbose() > 0)
        printf ("Build a function argument \n");
}


/*
JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReference(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jint java_number_of_dimensions, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a array type \n");

    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);
    int number_of_dimensions = java_number_of_dimensions;

    SgType *array_type = lookupTypeByName(package_name, type_name, number_of_dimensions);
    ROSE_ASSERT(array_type);

    astJavaComponentStack.push(array_type);
}
*/


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReference(JNIEnv *env, jclass, jint java_num_dimensions, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a array type \n");

    int num_dimensions = java_num_dimensions;
    if (num_dimensions > 0) { // No dimensions?  Then leave the base type on the stack.
        SgType *base_type = astJavaComponentStack.popType();
        ROSE_ASSERT(base_type);
        SgType *array_type = SageBuilder::getUniqueJavaArrayType(base_type, num_dimensions);
        ROSE_ASSERT(array_type);

        astJavaComponentStack.push(array_type);
    }
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReferenceEnd(JNIEnv *env, jclass, jstring java_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a array type \n");

    // Nothing to do !!!
}

JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSend(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jstring java_function_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a function call (Message Send) \n");

    // Do Nothing on the way down !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSendEnd(JNIEnv *env, jclass, 
                                                             jboolean java_is_static,
                                                             jboolean java_has_receiver,
                                                             jstring java_package_name,
                                                             jstring java_type_name,
                                                             jstring java_method_name,
                                                             jint method_index,
                                                             jint numTypeArguments,
                                                             jint numArguments,
                                                             jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionMessageSendEnd() \n");

    bool is_static = java_is_static,
         has_receiver = java_has_receiver;

    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name),
           method_name = convertJavaStringToCxxString(env, java_method_name);

    //
    // TODO: Since array types are not properly represented as class types but as (C++) pointer types,
    // when an array type is used to call a function, we have to substitute the Object class for the
    // array type in question as the Object type is its (only) super type and contains all the method
    // that is invokable from an array.
    //
    SgType *target_type = astJavaComponentStack.popType();
    if (isSgArrayType(target_type)) { // is this type an array type?
        target_type = ::ObjectClassType;
    }
    else if (isSgJavaWildcardType(target_type)) { // is this type a wildcard type?
        SgJavaWildcardType *wildcard_type = isSgJavaWildcardType(target_type);
        /*
        if (wildcard_type -> get_is_unbound()) {
            target_type = ::ObjectClassType;
        }
        else if (wildcard_type -> get_has_extends()) {
            target_type = wildcard_type -> get_bound_type();
        }
        else {
            ROSE_ASSERT(false && "! yet support wildcard with super bound");
        }
        */
        target_type = wildcard_type -> get_bound_type();
        if (target_type == NULL) {
            target_type = ::ObjectClassType;
        }
    }
// TODO: Remove this !!!
/*
if (has_receiver){
SgExpression *expr_receiver = isSgExpression(astJavaComponentStack.top());
if(expr_receiver){
if (expr_receiver -> get_type() != target_type) {
cout << "For method "
     << method_name.getString()
     << " invoked by receiver "
     << expr_receiver -> class_name()
     << ", the type of the receiver is "
     << getTypeName(expr_receiver -> get_type())
     << " while the target type is "
     << getTypeName(target_type)
     << endl;
cout.flush();
}
else {
cout << "For method "
     << method_name.getString()
     << " invoked by receiver "
     << expr_receiver -> class_name()
     << ", the type of the receiver is "
     << getTypeName(expr_receiver -> get_type())
     << " while the target type is "
     << getTypeName(target_type)
     << endl;
cout.flush();
}
}
else {
SgNode *node = astJavaComponentStack.top();
SgType *type = isSgType(astJavaComponentStack.top());
cout << "*** Method "
     << method_name.getString()
     << " invoked by receiver "
     << (type ? "type " : "node ")
     << (type ? getTypeName(type) : node -> class_name())
     << " with target type "
     << getTypeName(target_type)
     << endl;
cout.flush();
}
}
else {
cout << "*** Method "
     << method_name.getString()
     << " has no receiver and its target type is "
     << getTypeName(target_type)
     << endl;
cout.flush();
}
*/

/*
    SgClassDeclaration *class_declaration = isSgClassDeclaration(target_type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *targetClassScope = class_declaration -> get_definition();
    ROSE_ASSERT(targetClassScope);
*/

// TODO: Remove this !!!
/*
    SgType *return_type = astJavaComponentStack.popType(); // The return type of the function
    ROSE_ASSERT(return_type);
*/

// TODO: Remove this !!!
/*
cout << "Looking for function " 
<< method_name
<< " in type "
<< targetClassScope -> get_qualified_name()
<< " with parameter types: (";
std::list<SgType*>::iterator i = method_parameter_types.begin();
if (i != method_parameter_types.end()) {
cout << getTypeName(*i);
for (i++; i != method_parameter_types.end(); i++) {
cout << ", " << getTypeName(*i);
}
}
cout << ")"
<< endl;
cout.flush();
*/
/*
    SgMemberFunctionSymbol *method_symbol = findFunctionSymbolInClass(targetClassScope, method_name, method_parameter_types);
    ROSE_ASSERT(method_symbol);
    SgMemberFunctionType *method_type = isSgMemberFunctionType(method_symbol -> get_type());
    ROSE_ASSERT(method_type);
*/

// TODO: Remove this
//cout << "Here 11" << endl;
//cout.flush();
    SgType *containing_type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
    ROSE_ASSERT(containing_type);
    SgClassDefinition *containing_class_definition = isSgClassDeclaration(containing_type -> getAssociatedDeclaration() -> get_definingDeclaration()) -> get_definition();
    ROSE_ASSERT(containing_class_definition);
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) containing_class_definition -> getAttribute("method-members-map");
    ROSE_ASSERT(attribute);
    SgFunctionDefinition *method_definition = isSgFunctionDefinition(attribute -> getNode(method_index));
    ROSE_ASSERT(method_definition);
    SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
    ROSE_ASSERT(method_declaration);
    SgMemberFunctionType *method_type = isSgMemberFunctionType(method_declaration -> get_type());
    ROSE_ASSERT(method_type);
    SgMemberFunctionSymbol *method_symbol = isSgMemberFunctionSymbol(method_declaration -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(method_symbol);
    ROSE_ASSERT(method_symbol -> get_name().getString().compare(method_name.getString()) == 0);

// TODO: Remove this !!!
/*
    //
    // If we have an accurate return type for this function, set it !!!
    // This occurs when the method being invoked belongs to a parameterized type.
    // We need this temporary "zapping" so that the result of this expression will have
    // the correct type in case it is used for further dereferencing.
    //
    if (method_type -> get_return_type() != return_type) {
        method_type -> set_return_type(return_type);
    }
*/

    // The astJavaComponentStack has all of the arguments to the function call.
    SgExprListExp *arguments = SageBuilder::buildExprListExp();
    for (int i = 0; i < numArguments; i++) { // reverse the arguments' order
        SgExpression *expr = astJavaComponentStack.popExpression();
        arguments -> prepend_expression(expr);
    }
    setJavaSourcePosition(arguments, env, jToken);

    SgFunctionCallExp *method_call_exp = SageBuilder::buildFunctionCallExp(method_symbol, arguments);
    setJavaSourcePosition(method_call_exp, env, jToken);
    if (numTypeArguments > 0) {
        string parm_names = "";
        AstSgNodeListAttribute *attribute = new AstSgNodeListAttribute();
        for (int i = numTypeArguments - 1; i >= 0; i--) { // Note that we are reversing the type parameters here!
            SgType *type_argument = astJavaComponentStack.popType();
            ROSE_ASSERT(type_argument);
            attribute -> setNode(type_argument, i);

            string name = getTypeName(type_argument);
            parm_names = (parm_names.size() == 0 ? name : (name + ", " + parm_names));
        }
        parm_names = "<" + parm_names + ">";
        method_call_exp -> setAttribute("invocation_parameter_types", attribute);
        method_call_exp -> setAttribute("function_parameter_types", new AstRegExAttribute(parm_names));
    }

    SgExpression *exprForFunction = method_call_exp;

    //
    // This receiver, if present, is an expression or type that indicates the enclosing type of
    // the function being invoked. 
    //
    SgNode *receiver = (has_receiver ? astJavaComponentStack.pop() : NULL);

    //
    // If this function call has a receiver, finalize its invocation by adding the "receiver" prefix.  Note
    // that it is illegal to add a "this." prefix in front of a static method call - Hence the guard statement
    // below. (ECJ always adds a "this." prefix in front of every function whose receiver was not specified by
    // the user.)
    //
    if (receiver != NULL) {
        if (isSgNamedType(receiver)) { // Note that if this is true then the function must be static... See unparseJava_expression.C: unparseFucnCall
// TODO: Remove this !
/*
            if (isSgClassType(receiver)) { // Note that if this is true then the function must be static... See unparseJava_expression.C: unparseFucnCall
                SgClassType *type = isSgClassType(receiver);
                string type_name = isSgNamedType(receiver) -> get_name();
                string full_name = getFullyQualifiedTypeName(type);

                string class_name = (full_name.size() == type_name.size() ? type_name : full_name.substr(0, full_name.size() - type_name.size()) + type_name);

            SgClassType *c_type = isSgClassType(receiver);
            SgJavaParameterizedType *p_type = isSgJavaParameterizedType(receiver);
            SgJavaQualifiedType *q_type = isSgJavaQualifiedType(receiver);
            string type_name = (c_type ? getTypeName(c_type)
                                       : p_type ? getTypeName(p_type)
                                                : getTypeName(q_type));
            exprForFunction -> setAttribute("prefix", new AstRegExAttribute(type_name));
*/
//        exprForFunction -> setAttribute("prefix", new AstRegExAttribute(getTypeName(isSgNamedType(receiver))));

/*
cout << "(1) * * * Processed a function with a type qualifier"
     << endl;
cout.flush();
*/
        SgNamedType *qualification_type = isSgNamedType(receiver);
        SgJavaTypeExpression *type_expression = SageBuilder::buildJavaTypeExpression(qualification_type);
        setJavaSourcePosition(type_expression, env, jToken);
        type_expression -> setAttribute("type", new AstRegExAttribute(getTypeName(qualification_type)));
        exprForFunction = SageBuilder::buildBinaryExpression<SgDotExp>(type_expression, exprForFunction);


// TODO: Remove this !
/*
            }
            else { // this can't happen!?
                // TODO: What if the class is a parameterized type?
                ROSE_ASSERT(false); 
            }
*/
        }
        else if (is_static && isSgThisExp(receiver)) { // A sgThisExp receiver in front of a static function?
            delete receiver; // Ignore the receiver!
        }
        else {
            SgClassDefinition *current_class_definition = getCurrentTypeDefinition();
            SgType *enclosing_type = current_class_definition -> get_declaration() -> get_type();
            if (isSgThisExp(receiver) && (! isCompatibleTypes(target_type, enclosing_type))) {
// TODO: Remove this !
/*
                SgClassType *c_type = isSgClassType(target_type);
                SgJavaParameterizedType *p_type = isSgJavaParameterizedType(target_type);
                SgJavaQualifiedType *q_type = isSgJavaQualifiedType(target_type);
                string prefix_name = (c_type ? getTypeName(c_type) // getFullyQualifiedTypeName(c_type)
                                             : p_type ? getTypeName(p_type) // getFullyQualifiedTypeName(p_type)
                                                      : q_type ? getTypeName(q_type) // getFullyQualifiedTypeName(q_type)
                                                               : "");
                ROSE_ASSERT(prefix_name.size() != 0);
                receiver -> setAttribute("prefix", new AstRegExAttribute(prefix_name));
*/
/*
                receiver -> setAttribute("prefix", new AstRegExAttribute(getTypeName(target_type)));
*/

/*
cout << "(2) * * * Processed a function call with a qualified receiver"
     << endl;
cout.flush();
*/
                SgJavaTypeExpression *type_expression = new SgJavaTypeExpression(target_type);
                setJavaSourcePosition(type_expression, env, jToken);
                type_expression -> setAttribute("type", new AstRegExAttribute(getTypeName(target_type)));
                receiver = SageBuilder::buildBinaryExpression<SgDotExp>(type_expression, (SgExpression *) receiver);
            }

            exprForFunction = SageBuilder::buildBinaryExpression<SgDotExp>((SgExpression *) receiver, exprForFunction);
            setJavaSourcePosition(exprForFunction, env, jToken);
        }
    }

    astJavaComponentStack.push(exprForFunction);

    if (SgProject::get_verbose() > 2)
        printf ("Leaving Java_JavaParser_cactionMessageSendEnd(): %s\n", method_name.getString().c_str());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteral(JNIEnv *env, jclass, jstring java_string, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgStringVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    // string stringLiteral = "stringLiteral_abc";
    SgName stringLiteral = javaStringToUtf8(java_string); // convertJavaStringValToUtf8(env, java_string); // convertJavaStringToCxxString(env, java_string);

    // printf ("Building a string value expression = %s \n", stringLiteral.str());

    SgStringVal *stringValue = SageBuilder::buildStringVal(stringLiteral); // new SgStringVal(stringLiteral); 
    setJavaSourcePosition(stringValue, env, jToken);
    ROSE_ASSERT(stringValue != NULL);

    // Set the source code position (default values for now).
    // setJavaSourcePosition(stringValue);

    astJavaComponentStack.push(stringValue);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionAllocationExpression(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionAllocationExpression() \n");

    // Nothing to do !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionAllocationExpressionEnd(JNIEnv *env, jclass, jboolean has_type, jint java_num_arguments, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionAllocationExpressionEnd() \n");

    int num_arguments = java_num_arguments;

    list<SgExpression*> argument_list;
    for (int i = 0; i < num_arguments; i++) { // pop the arguments in reverse order
        SgExpression *arg = astJavaComponentStack.popExpression();
        argument_list.push_front(arg);
    }
    vector<SgExpression*> arguments;
    while (! argument_list.empty()) { // place the arguments in the vector proper order
        arguments.push_back(argument_list.front());
        argument_list.pop_front();
    }

    //
    // A null type indicates that we are dealing with parameters for an Enum constant.
    // In such a case, we use the Object type...
    //
    SgType *type = (has_type ? astJavaComponentStack.popType() : ::ObjectClassType);
    SgConstructorInitializer *constInit = SageBuilder::buildConstructorInitializer(NULL,
                                                                                   SageBuilder::buildExprListExp(arguments),
                                                                                   type,
                                                                                   false,
                                                                                   false,
                                                                                   false,
                                                                                   ! (isSgNamedType(type))); // ! (isSgClassType(type)));

    // TODO: An SgJavaParameterizedType and an SgJavaQualifiedType should be a SgClassType?  Currrently, they are not!

    // For the simple case, we only need the type as input to build SgNewExp.
    SgExprListExp *exprListExp = NULL;
    SgExpression *expr                  = NULL;
    short int val                       = 0;
    SgFunctionDeclaration *funcDecl     = NULL;

    SgNewExp *newExpression = SageBuilder::buildNewExp(type, exprListExp, constInit, expr, val, funcDecl);
    setJavaSourcePosition(newExpression, env, jToken);
    ROSE_ASSERT(newExpression != NULL);
    setJavaSourcePosition(newExpression, env, jToken);
    constInit -> set_parent(newExpression);

    string type_name = getTypeName(type);
// TODO: Remove this !!!
//cout << "(1) Allocating a method of type " << type_name << endl;
//cout.flush();
    newExpression -> setAttribute("type", new AstRegExAttribute(type_name));

    astJavaComponentStack.push(newExpression);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionANDANDExpression(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionANDANDExpression() \n");

    // Nothing to do !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionANDANDExpressionEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionANDANDExpressionEnd() \n");

    binaryExpressionSupport<SgAndOp>();

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionAnnotationMethodDeclaration(JNIEnv *env, jclass, jstring java_string, jint method_index, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Entering Java_JavaParser_cactionAnnotationMethodDeclaration() \n");

    // Nothing to do !!!

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionAnnotationMethodDeclaration() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionAnnotationMethodDeclarationEnd(JNIEnv *env, jclass, jstring java_string, jint method_index, jint num_annotations, jboolean has_default, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Entering Java_JavaParser_cactionAnnotationMethodDeclarationEnd() \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
    ROSE_ASSERT(attribute);
    SgFunctionDefinition *method_definition = isSgFunctionDefinition(attribute -> getNode(method_index));
    ROSE_ASSERT(method_definition);
    SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
    ROSE_ASSERT(method_declaration);
    ROSE_ASSERT(method_declaration -> get_type());
    ROSE_ASSERT(method_declaration -> get_type() -> get_return_type());
    method_declaration -> setAttribute("type", new AstRegExAttribute(getTypeName(method_declaration -> get_type() -> get_return_type())));

    if (has_default) {
        SgExpression *default_expression = astJavaComponentStack.popExpression();
        method_declaration -> setAttribute("default", new AstSgNodeAttribute(default_expression));
    }

    if (num_annotations > 0) {
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astJavaComponentStack.popExpression();
            annotation -> set_parent(method_declaration);
            annotations_attribute -> setNode(annotation, i);
        }
        method_declaration -> setAttribute("annotations", annotations_attribute);
    }

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionAnnotationMethodDeclarationEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayAllocationExpression(JNIEnv *env, jclass, jobject jToken) {
    // NOTHING TO DO !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayAllocationExpressionEnd(JNIEnv *env, jclass, jint java_num_dimensions, jboolean java_has_initializers, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionArrayAllocationExpressionEnd() \n");
  
    int num_dimensions = java_num_dimensions;
    bool has_initializers = java_has_initializers;

    SgAggregateInitializer *initializer = NULL;
    if (has_initializers) {
        initializer = isSgAggregateInitializer(astJavaComponentStack.pop());
        ROSE_ASSERT(initializer);
    }

    list<SgExpression*> argument_list;
    for (int i = 0; i < num_dimensions; i++) { // pop the arguments in reverse order
        SgExpression *arg = astJavaComponentStack.popExpression();
        argument_list.push_front(arg);
    }
    vector<SgExpression*> arguments;
    while (! argument_list.empty()) { // place the arguments in the vector in proper order
        arguments.push_back(argument_list.front());
        argument_list.pop_front();
    }

    SgType *type = astJavaComponentStack.popType();
    ROSE_ASSERT(type);

    SgArrayType *array_type = SageBuilder::getUniqueJavaArrayType(type, num_dimensions);
    SgConstructorInitializer *constInit = SageBuilder::buildConstructorInitializer(NULL,
                                                                                   SageBuilder::buildExprListExp(arguments),
                                                                                   type,
                                                                                   false,
                                                                                   false,
                                                                                   false,
                                                                                   ! (isSgNamedType(type))); // ! (isSgClassType(type) || isSgJavaParameterizedType(type) || isSgJavaQualifiedType(type)));
    // TODO: I think a SgJavaParameterizedType should be a SgClassType.  Currrently, it is not!

    // For the simple case, we only need the type as input to build SgNewExp.
    SgExprListExp *exprListExp = NULL;
    SgExpression *expr                  = NULL;
    short int val                       = 0;
    SgFunctionDeclaration *funcDecl     = NULL;

    // SgNewExp *newExpression = SageBuilder::buildNewExp(array_type, exprListExp, constInit, expr, val, funcDecl);
    SgNewExp *newExpression = SageBuilder::buildNewExp(array_type, exprListExp, constInit, expr, val, funcDecl);
    setJavaSourcePosition(newExpression, env, jToken);
    ROSE_ASSERT(newExpression != NULL);
    setJavaSourcePosition(newExpression, env, jToken);
    constInit -> set_parent(newExpression);

    //
    // TODO: Temporary patch until the SgNewExp can handle aggregate initializers.
    //
    if (has_initializers) {
        newExpression -> setAttribute("initializer", new AstSgNodeAttribute(initializer));
    }

    SgType *base_type = array_type -> get_base_type();
    while(isSgArrayType(base_type)) { // find the base type...
        base_type = ((SgArrayType *) base_type) -> get_base_type();
    }
    string type_name = getTypeName(type);
// TODO: Remove this !!!
//cout << "(2) Allocating a method of type " << type_name << endl;
//cout.flush();
    newExpression -> setAttribute("type", new AstRegExAttribute(type_name));

    astJavaComponentStack.push(newExpression);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayInitializer(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do
}

JNIEXPORT void JNICALL Java_JavaParser_cactionArrayInitializerEnd(JNIEnv *env, jclass, jint java_num_expressions, jobject jToken) {
    if (SgProject::get_verbose() > 2)
         printf ("Inside of Java_JavaParser_cactionArrayInitializerEnd() \n");

    int num_expressions = java_num_expressions;

    list<SgExpression *> init_list;
    for (int i = 0; i < num_expressions; i++) {
        SgExpression *init = astJavaComponentStack.popExpression();
        if (! isSgAggregateInitializer(init)) { // wrap simple expression in SgAssignInitializer
            init = SageBuilder::buildAssignInitializer(init, init -> get_type());
            // TODO: copy the location of the expression in question to the init node.
        }
        init_list.push_front(init);
    }

    vector<SgExpression *> init_vector;
    while (! init_list.empty()) { // place the arguments in the vector proper order
        init_vector.push_back(init_list.front());
        init_list.pop_front();
    }

    SgAggregateInitializer *initializers = SageBuilder::buildAggregateInitializer(SageBuilder::buildExprListExp(init_vector));
    setJavaSourcePosition(initializers, env, jToken);

    // Pushing 'initializers' on the statement stack
    astJavaComponentStack.push(initializers);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayReference(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionArrayReference() \n");

    // Nothing to do !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayReferenceEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionArrayReferenceEnd() \n");

    binaryExpressionSupport<SgPntrArrRefExp>();
}


JNIEXPORT void JNICALL Java_JavaParser_cactionAssertStatement(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
         printf ("Inside of Java_JavaParser_cactionAssertStatement() \n");
    // Nothing to do.
}

JNIEXPORT void JNICALL Java_JavaParser_cactionAssertStatementEnd(JNIEnv *env, jclass, jboolean hasExceptionArgument, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionAssertStatementEnd() \n");

    // Build the Assert Statement
    SgExpression *exceptionArgument = (hasExceptionArgument ? astJavaComponentStack.popExpression() : NULL);
    SgExpression *expression = astJavaComponentStack.popExpression();

    SgAssertStmt *assertStatement = SageBuilder::buildAssertStmt(expression, exceptionArgument);
    setJavaSourcePosition(assertStatement, env, jToken);

    // Pushing 'assert' on the statement stack
    astJavaComponentStack.push(assertStatement);

    if (SgProject::get_verbose() > 2)
        printf ("Leaving Java_JavaParser_cactionAssertStatementEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionAssignment(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Build an assignement statement (expression?) \n");

    // Nothing to do !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionAssignmentEnd(JNIEnv *env, jclass, jobject jToken) {
    // This function builds an assignement statement (not an expression).
    if (SgProject::get_verbose() > 2)
        printf ("Build an assignement statement (expression?) \n");

    binaryExpressionSupport<SgAssignOp>();

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBinaryExpression(JNIEnv *env, jclass, jobject jToken) {
    // I don't think we need this function.
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBinaryExpressionEnd(JNIEnv *env, jclass, jint java_operator_kind, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Build an Binary Expression End \n");

    // These are the operator code values directly from ECJ.
    enum ops {
        ERROR_OPERATOR       = 0, // This is not a ECJ value 
        AND                  = 2,
        DIVIDE               = 9, 
        GREATER              = 6,
        GREATER_EQUAL        = 7, 
        LEFT_SHIFT           = 10, 
        LESS                 = 4, 
        LESS_EQUAL           = 5, 
        MINUS                = 13, 
        MULTIPLY             = 15, 
        OR                   = 3,
        PLUS                 = 14,  
        REMAINDER            = 16, 
        RIGHT_SHIFT          = 17, 
        UNSIGNED_RIGHT_SHIFT = 19, 
        XOR                  = 8,
        OR_OR                = 100, // Handled by separate function 
        AND_AND              = 101, // Handled by separate function 
        LAST_OPERATOR
    };

    // printf ("PLUS = %d \n", PLUS);

    int operator_kind = java_operator_kind;
    // printf ("operator_kind = %d \n", operator_kind);

    switch(operator_kind) {
        // Operator codes used by the BinaryExpression in ECJ.
        case LESS:                 binaryExpressionSupport<SgLessThanOp>();       break;
        case LESS_EQUAL:           binaryExpressionSupport<SgLessOrEqualOp>();    break;
        case GREATER:              binaryExpressionSupport<SgGreaterThanOp>();    break;
        case GREATER_EQUAL:        binaryExpressionSupport<SgGreaterOrEqualOp>(); break;
        case AND:                  binaryExpressionSupport<SgBitAndOp>();         break;
        case OR:                   binaryExpressionSupport<SgBitOrOp>();          break;
        case XOR:                  binaryExpressionSupport<SgBitXorOp>();         break;
        case DIVIDE:               binaryExpressionSupport<SgDivideOp>();         break;
        case MINUS:                binaryExpressionSupport<SgSubtractOp>();       break;
        case PLUS:                 binaryExpressionSupport<SgAddOp>();            break;
        case MULTIPLY:             binaryExpressionSupport<SgMultiplyOp>();       break;
        case RIGHT_SHIFT:          binaryExpressionSupport<SgRshiftOp>();         break;
        case LEFT_SHIFT:           binaryExpressionSupport<SgLshiftOp>();         break;
        case REMAINDER:            binaryExpressionSupport<SgModOp>();            break;

        // This may have to handled special in ROSE. ROSE does not represent the semantics,
        // and so this support my require a special operator to support Java in ROSE. For
        // now we will use the more common SgRshiftOp.
        case UNSIGNED_RIGHT_SHIFT: binaryExpressionSupport<SgJavaUnsignedRshiftOp>();         break;

        // These are handled through separate functions (not a BinaryExpression in ECJ).
        // case OR_OR:   ROSE_ASSERT(false); break;
        // case AND_AND: ROSE_ASSERT(false); break;

        default:
            //
            // If this file was not already tagged as an erroneous file, do so now.
            //
            if (! ::currentSourceFile -> attributeExists("error")) {
                ::currentSourceFile -> setAttribute("error", new AstRegExAttribute("Illegal use of a binary operator"));
            }

            cout << "Error: default reached in cactionBinaryExpressionEnd() operator_kind = " <<  operator_kind << endl;
            cout.flush();
            binaryExpressionSupport<SgAddOp>(); // Any operator so that we can "keep going" !!! EROSE_ASSERT(false);
    }

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBlock(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Build an SgBasicBlock scope \n");

// TODO: Remove this !!!
/*
    // There could be a conditional from an IF statement on the stack.
    // ROSE_ASSERT(astJavaExpressionStack.empty());

    // If there is an expression on the expression stack and an SgIfStmt on the scope stack then 
    // this might be a good time to associate the conditional with the SgIfStmt and have a more
    // enforceble rules going forward.  But then there might not be a SgBasicBlock, so don't do this.

    // Since we build the true body when we build the ifStmt, we need to detect and reuse this 
    // SgBasicBlock instead of building a new one.
    // SgBasicBlock *block = SageBuilder::buildBasicBlock();
    SgBasicBlock *block = NULL;

    if (isSgIfStmt(astJavaScopeStack.top())) {
        SgIfStmt *ifStatement = (SgIfStmt*) astJavaScopeStack.top();
        SgNullStatement *nullStatement = isSgNullStatement(ifStatement -> get_true_body());
        if (nullStatement != NULL) {
cout << "Adding true if block"
     << endl;
cout.flush();
            // block = ifStatement -> get_true_body();
            block = SageBuilder::buildBasicBlock();
            ROSE_ASSERT(block != NULL);
            ifStatement -> set_true_body(block);

            delete nullStatement;
        }
        else {
cout << "Adding false if block"
     << endl;
cout.flush();
            // Set the false body
            block = SageBuilder::buildBasicBlock();
            ROSE_ASSERT(block != NULL);
            ifStatement -> set_false_body(block);
        }
    }
    else if (isSgForStatement(astJavaScopeStack.top())) {
        // DQ (7/30/2011): Handle the case of a block after a SgForStatement
        // Because we build the SgForStatement on the stack and then the cactionBlock 
        // function is called, we have to detect and fixup the SgForStatement.
        SgForStatement *forStatement = (SgForStatement*) astJavaScopeStack.top();
        SgNullStatement *nullStatement = isSgNullStatement(forStatement -> get_loop_body());
        if (nullStatement != NULL) {
            block = SageBuilder::buildBasicBlock();
            ROSE_ASSERT(block != NULL);
            forStatement -> set_loop_body(block);
            delete nullStatement;
        }
    }
    else {
        block = SageBuilder::buildBasicBlock();
    }
    ROSE_ASSERT(block != NULL);
*/

    SgBasicBlock *block = SageBuilder::buildBasicBlock();
    block -> set_parent(astJavaScopeStack.top());
    setJavaSourcePosition(block, env, jToken);

    astJavaScopeStack.push(block);

    if (SgProject::get_verbose() > 2)
        printf ("Done Building an SgBasicBlock scope \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBlockEnd(JNIEnv *env, jclass, jint java_numberOfStatements, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Pop the current SgBasicBlock scope off the scope stack...\n");

    int numberOfStatements = java_numberOfStatements;

    if (SgProject::get_verbose() > 2)
        printf ("In cactionBlockEnd(): numberOfStatements = %d \n", numberOfStatements);

    ROSE_ASSERT(! astJavaScopeStack.empty());

    // DQ (7/30/2011): Take the block off of the scope stack and put it onto the statement stack so that we can 
    // process either blocks of other statements uniformally.
    SgBasicBlock *body = astJavaScopeStack.popBasicBlock();
    for (int i = 0; i  < numberOfStatements; i++) {
        SgStatement *statement = astJavaComponentStack.popStatement();
        if (SgProject::get_verbose() > 2) {
            cerr << "(5) Adding statement "
                 << statement -> class_name()
                 << " to a block"
                 << endl;
            cerr.flush();
        }
        body -> prepend_statement(statement);
    }
    
    astJavaComponentStack.push(body);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBreakStatement(JNIEnv *env, jclass, jstring java_string, jobject jToken) {
    SgBreakStmt *stmt = SageBuilder::buildBreakStmt();
    ROSE_ASSERT(stmt != NULL);

    string label_name = convertJavaStringToCxxString(env, java_string);
    if (label_name.length() > 0) {
        assert(lookupLabelByName(label_name) != NULL);
        stmt -> set_do_string_label(label_name);
    }

    setJavaSourcePosition(stmt, env, jToken);
    astJavaComponentStack.push(stmt);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCaseStatement(JNIEnv *env, jclass, jboolean hasCaseExpression, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionCaseStatement() \n");

    // 
    // We build on the way down because the scope information and symbol table information is contained
    // in the Ast node.  This AST node is a subclass of SgScopeStatement
    //
    SgStatement *caseStatement = NULL;
    if (hasCaseExpression) {
        caseStatement = SageBuilder::buildCaseOptionStmt(); // the body will be added later
    } else {
        caseStatement = SageBuilder::buildDefaultOptionStmt(); // the body will be added later
    }
    ROSE_ASSERT(caseStatement != NULL);

    setJavaSourcePosition(caseStatement, env, jToken);

    // DQ (7/30/2011): For the build interface to work we have to initialize the parent pointer to the SgForStatement.
    // Charles4 (8/23/2011): When and why parent pointers should be set needs to be clarified. Perhaps the SageBuilder
    // functions should be revisited?
    caseStatement -> set_parent(astJavaScopeStack.top());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCaseStatementEnd(JNIEnv *env, jclass, jboolean hasCaseExpression, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionCaseStatementEnd() \n");

    SgStatement *case_statement = NULL;
    // update the Case Statement
    if (hasCaseExpression) {
        SgExpression *case_expression = astJavaComponentStack.popExpression();
        case_statement = SageBuilder::buildCaseOptionStmt(case_expression, NULL); // the body will be added later
    }
    else {
        case_statement = SageBuilder::buildDefaultOptionStmt(NULL); // the body will be added later
    }

    // Pushing 'case' on the statement stack
    astJavaComponentStack.push(case_statement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCastExpression(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCastExpressionEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionCastExpressionEnd() \n");

    SgExpression *expression = astJavaComponentStack.popExpression();
    SgType *castType = astJavaComponentStack.popType();
    ROSE_ASSERT(castType);

    SgCastExp *castExp = SageBuilder::buildCastExp(expression, castType);
    ROSE_ASSERT(castExp != NULL);
    castExp -> setAttribute("type", new AstRegExAttribute(getTypeName(castType)));

    astJavaComponentStack.push(castExp);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCharLiteral(JNIEnv *env, jclass, jchar java_char_value, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a CharVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    wchar_t value = java_char_value;

    SgWcharVal *charValue = SageBuilder::buildWcharVal(value);
    ROSE_ASSERT(charValue != NULL);

    setJavaSourcePosition(charValue, env, jToken);

    astJavaComponentStack.push(charValue);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionClassLiteralAccess(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do
}


JNIEXPORT void JNICALL Java_JavaParser_cactionClassLiteralAccessEnd(JNIEnv *env, jclass, jobject jToken) {
    SgType *type = astJavaComponentStack.popType();
    ROSE_ASSERT(type);

    SgClassDeclaration *class_declaration = isSgClassDeclaration(::ClassClassType -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassSymbol *class_symbol = isSgClassSymbol(class_declaration -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(class_symbol);

    SgExpression *class_expression = SageBuilder::buildClassExp(class_symbol);
    ROSE_ASSERT(class_expression != NULL);

/*
cout << "(3) * * * Processed a class literal with a type qualifier"
     << endl;
cout.flush();
*/
    SgJavaTypeExpression *type_expression = new SgJavaTypeExpression(type);
    setJavaSourcePosition(type_expression, env, jToken);
    type_expression -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));
    class_expression = SageBuilder::buildBinaryExpression<SgDotExp>(type_expression, class_expression);

    astJavaComponentStack.push(class_expression);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionClinit(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do
}


JNIEXPORT void JNICALL Java_JavaParser_cactionConditionalExpression(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do
}


JNIEXPORT void JNICALL Java_JavaParser_cactionConditionalExpressionEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionConditionalExpressionEnd() \n");

    SgExpression *false_exp = astJavaComponentStack.popExpression();

    SgExpression *true_exp = astJavaComponentStack.popExpression();

    SgExpression *test_exp = astJavaComponentStack.popExpression();

    // Build the assignment operator and push it onto the stack.
    SgConditionalExp *conditional = SageBuilder::buildConditionalExp(test_exp, true_exp, false_exp);
    ROSE_ASSERT(conditional != NULL);

    astJavaComponentStack.push(conditional);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionContinueStatement(JNIEnv *env, jclass, jstring java_string, jobject jToken) {
    SgContinueStmt *stmt = SageBuilder::buildContinueStmt();
    ROSE_ASSERT(stmt != NULL);

    string label_name = convertJavaStringToCxxString(env, java_string);
    if (label_name.length() > 0) {
        assert(lookupLabelByName(label_name) != NULL);
        stmt -> set_do_string_label(label_name);
    }

    setJavaSourcePosition(stmt, env, jToken);
    astJavaComponentStack.push(stmt);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompoundAssignment(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompoundAssignmentEnd(JNIEnv *env, jclass, jint java_operator_kind, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionCompoundAssignmentEnd() \n");

    // These are the operator code values directly from ECJ.
    enum ops // NO_STRINGIFY
    {
        ERROR_OPERATOR       = 0, // This is not a ECJ value 
        AND                  = 2,
        DIVIDE               = 9, 
        LEFT_SHIFT           = 10, 
        MINUS                = 13, 
        MULTIPLY             = 15, 
        OR                   = 3,
        PLUS                 = 14,  
        REMAINDER            = 16, 
        RIGHT_SHIFT          = 17, 
        UNSIGNED_RIGHT_SHIFT = 19, 
        XOR                  = 8,
        LAST_OPERATOR
    };

    int operator_kind = java_operator_kind;
    // printf ("operator_kind = %d \n", operator_kind);

    switch(operator_kind) {
        // Operator codes used by the CompoundAssignment in ECJ.

        case PLUS:        binaryExpressionSupport<SgPlusAssignOp>();   break;
        case MINUS:       binaryExpressionSupport<SgMinusAssignOp>();  break;
        case DIVIDE:      binaryExpressionSupport<SgDivAssignOp>();    break;
        case MULTIPLY:    binaryExpressionSupport<SgMultAssignOp>();   break;
        case OR:          binaryExpressionSupport<SgIorAssignOp>();    break;
        case AND:         binaryExpressionSupport<SgAndAssignOp>();    break;
        case XOR:         binaryExpressionSupport<SgXorAssignOp>();    break;
        case REMAINDER:   binaryExpressionSupport<SgModAssignOp>();    break;
        case RIGHT_SHIFT: binaryExpressionSupport<SgRshiftAssignOp>(); break;
        case LEFT_SHIFT:  binaryExpressionSupport<SgLshiftAssignOp>(); break;

        // This may have to handled special in ROSE. ROSE does not represent the semantics,
        // and so this support my require a special operator to support Java in ROSE. For
        // now we will use the more common SgRshiftOp.
        case UNSIGNED_RIGHT_SHIFT: binaryExpressionSupport<SgJavaUnsignedRshiftAssignOp>(); break;

        default:
            //
            // If this file was not already tagged as an erroneous file, do so now.
            //
            if (! ::currentSourceFile -> attributeExists("error")) {
                ::currentSourceFile -> setAttribute("error", new AstRegExAttribute("Illegal use of a compound operator"));
            }

            cout << "Error: default reached in cactionCompoundAssignmentEnd() operator_kind = " <<  operator_kind << endl;
            cout.flush();
            binaryExpressionSupport<SgPlusAssignOp>(); // Any operator so that we can "keep going" !!! EROSE_ASSERT(false);
    }

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);

    // Also set the source position of the nested expression in the expression statement.
    SgExprStatement *exprStatement = isSgExprStatement(astJavaComponentStack.top());
    if (exprStatement != NULL) {
        setJavaSourcePosition(exprStatement, env, jToken);
    }
}


JNIEXPORT void JNICALL Java_JavaParser_cactionDoStatement(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionDoStatement() \n");

    //
    // NOTE: The function SageBuilder::buildDoWhileStmt(...) is not invoked here because it does not
    // take NULL arguments. However, we need to construct the while statement here because we need
    // its scope and the arguments are not yet available.
    //
    SgDoWhileStmt *do_while_statement = new SgDoWhileStmt((SgStatement *) NULL, (SgStatement *) NULL);
    ROSE_ASSERT(do_while_statement != NULL);
    do_while_statement -> set_parent(astJavaScopeStack.top());
    setJavaSourcePosition(do_while_statement, env, jToken);

    astJavaScopeStack.push(do_while_statement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionDoStatementEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionDoStatementEnd() \n");

    SgExpression *test_expression = astJavaComponentStack.popExpression();
    SgExprStatement *test_statement = SageBuilder::buildExprStatement(test_expression);
    SgStatement *body = astJavaComponentStack.popStatement();

    SgDoWhileStmt *do_while_statement = astJavaScopeStack.popDoWhileStmt();

    do_while_statement -> set_body(body);
    body -> set_parent(do_while_statement);

    do_while_statement -> set_condition(test_statement);
    test_statement -> set_parent(do_while_statement);


    astJavaComponentStack.push(do_while_statement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionDoubleLiteral(JNIEnv *env, jclass, jdouble java_value, jstring java_source, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a DoubleVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    double value = java_value;
    SgName source = convertJavaStringToCxxString(env, java_source);

    SgDoubleVal *doubleValue = SageBuilder::buildDoubleVal_nfi(value, source);
    ROSE_ASSERT(doubleValue != NULL);

    setJavaSourcePosition(doubleValue, env, jToken);

    astJavaComponentStack.push(doubleValue);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionEmptyStatement(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do;
}


JNIEXPORT void JNICALL Java_JavaParser_cactionEmptyStatementEnd(JNIEnv *env, jclass, jobject jToken) {
    SgNullStatement *stmt = SageBuilder::buildNullStatement();
    ROSE_ASSERT(stmt != NULL);
    setJavaSourcePosition(stmt, env, jToken);
    astJavaComponentStack.push(stmt);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionEqualExpression(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionEqualExpression() \n");
    // Nothing to do !
}


JNIEXPORT void JNICALL Java_JavaParser_cactionEqualExpressionEnd(JNIEnv *env, jclass, jint java_operator_kind, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionEqualExpressionEnd() \n");

    // These are the operator code values directly from ECJ.
    enum ops // NO_STRINGIFY
    {
        ERROR_OPERATOR = 0, // This is not a ECJ value 
        EQUAL_EQUAL    = 18,
        NOT_EQUAL      = 29,
        LAST_OPERATOR
    };

    int operator_kind = java_operator_kind;
    // printf ("operator_kind = %d \n", operator_kind);

    switch(operator_kind) {
        // Operator codes used by the UnaryExpression in ECJ.
        case EQUAL_EQUAL: binaryExpressionSupport<SgEqualityOp>(); break;
        case NOT_EQUAL:   binaryExpressionSupport<SgNotEqualOp>(); break;

        default:
            //
            // If this file was not already tagged as an erroneous file, do so now.
            //
            if (! ::currentSourceFile -> attributeExists("error")) {
                ::currentSourceFile -> setAttribute("error", new AstRegExAttribute("Illegal use of a logical operator"));
            }

            cout << "Error: default reached in cactionEqualExpressionEnd() operator_kind = " <<  operator_kind << endl;
            cout.flush();
            binaryExpressionSupport<SgEqualityOp>(); // Any operator so that we can "keep going" !!! EROSE_ASSERT(false);
    }

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionExtendedStringLiteral(JNIEnv *env, jclass, jstring java_string, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build an ExtendedStringVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgName stringLiteral = javaStringToUtf8(java_string); // convertJavaStringValToUtf8(env, java_string); // convertJavaStringToCxxString(env, java_string);

    // printf ("Building a string value expression = %s \n", stringLiteral.str());

    SgStringVal *stringValue = SageBuilder::buildStringVal(stringLiteral); // new SgStringVal(stringLiteral);
    ROSE_ASSERT(stringValue != NULL);

    setJavaSourcePosition(stringValue, env, jToken);

    astJavaComponentStack.push(stringValue);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionFalseLiteral(JNIEnv *env, jclass, jobject jToken) {
    SgExpression *expression = SageBuilder::buildBoolValExp(false);
    astJavaComponentStack.push(expression);
}


// DQ (9/5/2011): This was changed to be processed bottom up (so there is no Java_JavaParser_cactionFieldDeclaration() function now.
JNIEXPORT void JNICALL Java_JavaParser_cactionFieldDeclarationEnd(JNIEnv *env, jclass,
                                                                  jstring variable_name,
                                                                  jint num_annotations,
                                                                  jboolean is_enum_constant,
                                                                  jboolean has_initializer,
                                                                  jboolean is_final,
                                                                  jboolean is_private,
                                                                  jboolean is_protected,
                                                                  jboolean is_public, 
                                                                  jboolean is_volatile,
                                                                  jboolean is_synthetic,
                                                                  jboolean is_static,
                                                                  jboolean is_transient,
                                                                  jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldDeclarationEnd() \n");

    SgName name = convertJavaStringToCxxString(env, variable_name);

    if (SgProject::get_verbose() > 2)
        printf ("Building a Field declaration for name = %s \n", name.str());

    SgExpression *initializer_expression = (has_initializer ? astJavaComponentStack.popExpression() : NULL);

    SgScopeStatement *outer_scope = astJavaScopeStack.top();
    ROSE_ASSERT(outer_scope);
    SgVariableSymbol *symbol = outer_scope -> lookup_variable_symbol(name);
    ROSE_ASSERT(symbol);
    SgInitializedName *initialized_name = symbol -> get_declaration();
    ROSE_ASSERT(initialized_name);
    SgVariableDeclaration *variable_declaration = isSgVariableDeclaration(initialized_name -> get_declaration());
    ROSE_ASSERT(variable_declaration);

    // By default, the access modifier is set to unknown
    variable_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    if (is_enum_constant) { // identify ENUM fields
        variable_declaration -> setAttribute("enum-constant", new AstRegExAttribute(""));
    }
    else { // if this is not an ENUM field then it has a type on the stack.
        SgType *type = astJavaComponentStack.popType();
        ROSE_ASSERT(type);
        initialized_name -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));
    }

    if (num_annotations > 0) {
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astJavaComponentStack.popExpression();
            annotation -> set_parent(variable_declaration);
            annotations_attribute -> setNode(annotation, i);
        }
        variable_declaration -> setAttribute("annotations", annotations_attribute);
    }

    // Set the modifiers (shared between PHP and Java)
    if (is_final) {
        variable_declaration -> get_declarationModifier().setFinal();
    }

    if (is_private) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Private \n");
        variable_declaration -> get_declarationModifier().get_accessModifier().setPrivate();
    }

    if (is_protected) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Protected \n");
        variable_declaration -> get_declarationModifier().get_accessModifier().setProtected();
    }

    if (is_public) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Public \n");
        variable_declaration -> get_declarationModifier().get_accessModifier().setPublic();
    }

    if (is_volatile) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Volatile \n");
        variable_declaration -> get_declarationModifier().get_typeModifier().get_constVolatileModifier().setVolatile();
    }

    if (is_synthetic) {
        // Synthetic is not a keyword, not clear if we want to record this explicitly.
        printf ("Specification of isSynthetic is not supported in the IR (should it be?) \n");
    }

    if (is_static) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Static \n");
        variable_declaration -> get_declarationModifier().get_storageModifier().setStatic();
    }

    if (is_transient) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Transient \n");
        variable_declaration -> get_declarationModifier().get_typeModifier().get_constVolatileModifier().setJavaTransient();
    }

    if (SgProject::get_verbose() > 0)
        variable_declaration -> get_file_info() -> display("source position in Java_JavaParser_cactionFieldDeclarationEnd(): debug");

    if (initializer_expression) { // There is an initialization expression
        SgInitializer *initializer = SageBuilder::buildAssignInitializer(initializer_expression);
        ROSE_ASSERT(initializer != NULL);

        setJavaSourcePosition(initializer_expression, env, jToken);
        setJavaSourcePosition(initializer, env, jToken);

        initializer_expression -> set_parent(initializer);

        initializer -> get_file_info() -> display("cactionFieldDeclarationEnd()");

        initialized_name -> set_initptr(initializer);
        initializer -> set_parent(initialized_name);
    }
}


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReference(JNIEnv *env, jclass, jstring java_field, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldReference() \n");

    // Nothing to do !!!

    if (SgProject::get_verbose() > 2)
        printf ("Leaving of Java_JavaParser_cactionFieldReference() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReferenceEnd(JNIEnv *env, jclass, jboolean explicit_type, jstring java_field, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldReferenceEnd() \n");

    SgName field_name = convertJavaStringToCxxString(env, java_field);

    if (SgProject::get_verbose() > 0)
        printf ("Building a Field reference for name = %s \n", field_name.str());

    SgType *receiver_type = (explicit_type ? astJavaComponentStack.popType() : NULL);
    SgNode *prefix = astJavaComponentStack.pop();
    SgExpression *receiver = isSgExpression(prefix);
    ROSE_ASSERT(receiver || isSgType(prefix));
    if (! explicit_type) {
        receiver_type = (receiver ? receiver -> get_type() : isSgType(prefix));
    }
// TODO: Remove this !!!
/*
cout <<  "The receiver type is "
     << getTypeName(receiver_type)
     << endl
     << " The prefix type is "
     << prefix -> class_name()
     << endl
     << " The field name "
     << field_name
     << endl;
cout.flush();
*/
    SgType *type = isSgType(prefix);
    ROSE_ASSERT(receiver || type);
    if (type == NULL) {
        type = receiver -> get_type();
        if (isSgMemberFunctionType(type)) {
            type = ((SgMemberFunctionType *) type) -> get_return_type();
        }
    }
    ROSE_ASSERT(type);

ROSE_ASSERT(! isSgMemberFunctionType(receiver_type));


    SgExpression *result;

// TODO: Remove this !!!
/*
    if (isSgThisExp(receiver) || isSgSuperExp(receiver)) { // First, take care of these special pointer types:  "this" and "super"
        SgClassType *class_type = isSgClassType(isSgPointerType(receiver_type) -> get_base_type());
        ROSE_ASSERT(class_type);
        SgClassDeclaration *declaration = isSgClassDeclaration(class_type -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(declaration);
        ROSE_ASSERT(declaration -> get_definition());
        SgVariableSymbol *variable_symbol = lookupSimpleNameVariableInClass(field_name, declaration -> get_definition());
        ROSE_ASSERT(variable_symbol);
        SgVarRefExp *field = SageBuilder::buildVarRefExp(variable_symbol);
        ROSE_ASSERT(field != NULL);
        setJavaSourcePosition(field, env, jToken);
        result = SageBuilder::buildBinaryExpression<SgDotExp>(receiver, field);
    }
    else 
*/

    //
    // TODO: Note that the use of attributes is not a valid substitute for supporting these features!
    // In particular, this approach is not robust enough to fully support parenthesized expressions
    // as we have no way of indicating whether or not an expression involving a suffix was
    // parenthesized - In other words, we can't add a "java-parenthesis-info" attribute to a "length"
    // attribute.
    //
    if (isSgArrayType(receiver_type) && field_name.getString().compare("length") == 0) { // In fact, this is a Java array which is a type !!!
        SgVarRefExp *field = SageBuilder::buildJavaArrayLengthVarRefExp();
        setJavaSourcePosition(field, env, jToken);
        result = SageBuilder::buildBinaryExpression<SgDotExp>(receiver, field);
    }
    else {
        if (isSgArrayType(receiver_type)) {
            receiver_type = isSgArrayType(receiver_type) -> get_base_type();
        }

// TODO: Remove this !
/*
        else {
            SgJavaWildcardType *wildcard_type = isSgJavaWildcardType(receiver_type);
            if (wildcard_type) {
                receiver_type = (wildcard_type -> is_unbound() ? ::ObjectClassType : wildcard_type -> get_bound_type());
            }
        }
*/

// TODO: Remove this !
/*
        //
        // TODO: should Parameterized types be class types? ... Need to review this!
        //
        SgClassType *class_type = isSgClassType(receiver_type);
        SgJavaParameterizedType *param_type = isSgJavaParameterizedType(receiver_type);
        SgJavaQualifiedType *qualified_type = isSgJavaQualifiedType(receiver_type);
        ROSE_ASSERT(class_type || param_type || qualified_type);
        SgClassDeclaration *declaration = isSgClassDeclaration((class_type
                                                                     ? class_type -> get_declaration()
                                                                     : param_type 
                                                                            ? param_type -> get_declaration()
                                                                            : qualified_type -> get_declaration()) -> get_definingDeclaration());
*/

        SgClassDeclaration *declaration = isSgClassDeclaration(isSgNamedType(receiver_type) -> getAssociatedDeclaration() -> get_definingDeclaration());
        ROSE_ASSERT(declaration);
        ROSE_ASSERT(declaration -> get_definition());
        SgVariableSymbol *variable_symbol = lookupSimpleNameVariableInClass(field_name, declaration -> get_definition());
// TODO: Remove this !
/*
if (! variable_symbol) {
    SgClassType *c_type = isSgClassType(receiver_type);
    SgJavaParameterizedType *p_type = isSgJavaParameterizedType(receiver_type);
    SgJavaQualifiedType *q_type = isSgJavaQualifiedType(receiver_type);
    ROSE_ASSERT(c_type || p_type || q_type);
    string type_name = (c_type ? getFullyQualifiedTypeName(c_type)
                               : p_type ? getFullyQualifiedTypeName(p_type)
                                        : getFullyQualifiedTypeName(q_type));
  cout << "Could not find variable \"" << field_name.getString()
       << "\" in type: " << type_name
       << " in file " << ::currentSourceFile -> getFileName()
       << endl;
  cout.flush();
}
*/
        ROSE_ASSERT(variable_symbol);
        SgVarRefExp *field = SageBuilder::buildVarRefExp(variable_symbol);
        ROSE_ASSERT(field != NULL);
        setJavaSourcePosition(field, env, jToken);

        if (receiver) {
// TODO: Remove this !
/*
  cout << "Emitting a SgDotExp with receiver of type "
       << getTypeName(receiver -> get_type())
       << endl;
  cout.flush();
*/
            result = SageBuilder::buildBinaryExpression<SgDotExp>(receiver, field);
        }
        else {
// "Good code"
/*
            string class_name = getTypeName(receiver_type); // getFullyQualifiedTypeName(class_type);
            field -> setAttribute("prefix", new AstRegExAttribute(class_name));
            result = field;
*/
// "Let's try this code instead"
/*
cout << "* * * Processed a receiver with a type qualifier: "
     << getTypeName(receiver_type)
     << endl;
cout.flush();
*/
            SgJavaTypeExpression *type_expression = new SgJavaTypeExpression(receiver_type);
            setJavaSourcePosition(type_expression, env, jToken);
            type_expression -> setAttribute("type", new AstRegExAttribute(getTypeName(receiver_type)));
            result = SageBuilder::buildBinaryExpression<SgDotExp>(type_expression, field);

// TODO: Remove this !
/*
  cout << "Decorating a field name"
       << endl;
  cout.flush();
*/
        }
    }

    astJavaComponentStack.push(result);

    if (SgProject::get_verbose() > 2)
        printf ("Leaving of Java_JavaParser_cactionFieldReferenceEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionFloatLiteral(JNIEnv *env, jclass, jfloat java_value, jstring java_source, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a FloatVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    float value = java_value;
    SgName source = convertJavaStringToCxxString(env, java_source);

    SgFloatVal *floatValue = SageBuilder::buildFloatVal_nfi(value, source);
    ROSE_ASSERT(floatValue != NULL);

    setJavaSourcePosition(floatValue, env, jToken);

    astJavaComponentStack.push(floatValue);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionForeachStatement(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionForeachStatement() \n");

    // 
    // We build on the way down because the scope information and symbol table information is contained
    // in the Ast node.  This AST node is a subclass of SgScopeStatement
    //
    SgJavaForEachStatement *foreachStatement = SageBuilder::buildJavaForEachStatement();
    ROSE_ASSERT(foreachStatement != NULL);

    setJavaSourcePosition(foreachStatement, env, jToken);

    // DQ (7/30/2011): For the build interface to work we have to initialize the parent pointer to the SgForStatement.
    // Charles4 (8/23/2011): When and why parent pointers should be set needs to be clarified. Perhaps the SageBuilder
    // functions should be revisited?
    foreachStatement -> set_parent(astJavaScopeStack.top());

    astJavaScopeStack.push(foreachStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionForeachStatementEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionForEachStatementEnd() \n");

    SgStatement *action = astJavaComponentStack.popStatement();                                                  // Get the action statement
    SgExpression *collection = astJavaComponentStack.popExpression();                                            // Get the collection expr
    SgVariableDeclaration *variable_declaration = isSgVariableDeclaration(astJavaComponentStack.popStatement()); // Get the declaration statement

    // Build the final Foreach Statement
    SgJavaForEachStatement *foreach_statement = astJavaScopeStack.popJavaForEachStatement();

    // DQ (9/3/2011): Change API as suggested by Philippe.
    foreach_statement -> set_element(variable_declaration);
    variable_declaration -> set_parent(foreach_statement);

    foreach_statement -> set_collection(collection);
    collection -> set_parent(foreach_statement);
    foreach_statement -> set_loop_body(action);
    action -> set_parent(foreach_statement);

    // Pushing 'foreach' on the statement stack
    astJavaComponentStack.push(foreach_statement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionForStatement(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionForStatement() \n");

    SgStatementPtrList statements;
    SgForInitStatement *forInitStatement = SageBuilder::buildForInitStatement_nfi(statements);
    ROSE_ASSERT(forInitStatement != NULL);
    SageInterface::setOneSourcePositionForTransformation(forInitStatement); // We need to set the source code position information
    ROSE_ASSERT(forInitStatement -> get_startOfConstruct() != NULL);

    SgForStatement *forStatement = SageBuilder::buildForStatement(forInitStatement, (SgStatement *) NULL, (SgExpression *) NULL, (SgStatement *) NULL);
    ROSE_ASSERT(forStatement != NULL);
    ROSE_ASSERT(forInitStatement -> get_startOfConstruct() != NULL);

    forStatement -> set_parent(astJavaScopeStack.top());
    ROSE_ASSERT(forStatement -> get_parent());

    astJavaScopeStack.push(forStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionForStatementEnd(JNIEnv *env, jclass, jint num_initializations, jboolean has_condition, jint num_increments, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionForStatementEnd() \n");

    SgStatement *loop_body = astJavaComponentStack.popStatement();

    //
    // There may be more than 1 increment expressions. In such a case, we merge them into a
    // single expression: a hierarchy of SgCommaOpExp.
    //
    for (int i = 1; i < num_increments; i++) {
        binaryExpressionSupport<SgCommaOpExp>();
    }
    SgExpression *increment_expression = (num_increments > 0 ? astJavaComponentStack.popExpression() : SageBuilder::buildNullExpression());
    SgExpression *test_expression = (has_condition ? astJavaComponentStack.popExpression() : SageBuilder::buildNullExpression());
    SgStatement *test_statement = SageBuilder::buildExprStatement(test_expression); // The ROSE IR is based on C which allows a statment for the test...
    test_expression -> set_parent(test_statement);

    SgForStatement *for_statement = astJavaScopeStack.popForStatement(); // SageBuilder::buildForStatement(forInitStatement, testStatement, incrementExpression, bodyStatement);
    SgForInitStatement *for_init_statement = for_statement -> get_for_init_stmt();
    ROSE_ASSERT(for_init_statement);
    SgStatementPtrList &init_statements = for_init_statement -> get_init_stmt();
    ROSE_ASSERT(init_statements.size() == 0);
    for (int i = 0; i < num_initializations; i++) {
        for_init_statement -> prepend_init_stmt(astJavaComponentStack.popStatement());
    }
 // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
 // ROSE_ASSERT(init_statements.size() == num_initializations);
    ROSE_ASSERT(init_statements.size() == (size_t)num_initializations);

    for_statement -> set_test(test_statement);
    test_statement -> set_parent(for_statement);
    for_statement -> set_increment(increment_expression);
    increment_expression -> set_parent(for_statement);
    for_statement -> set_loop_body(loop_body);
    loop_body -> set_parent(for_statement);

    astJavaComponentStack.push(for_statement);

// TODO: Remove this !
/*
    ROSE_ASSERT(test_expression -> get_parent() != NULL);
    ROSE_ASSERT(test_statement -> get_parent() != NULL);

    ROSE_ASSERT(increment_expression -> get_parent() != NULL);
    ROSE_ASSERT(increment_expression -> get_startOfConstruct() != NULL);

    // printf ("for_statement -> get_for_init_stmt() = %p \n", forStatement -> get_for_init_stmt());
    // printf ("for_statement -> get_for_init_stmt() = %p \n", forStatement -> get_for_init_stmt());
    ROSE_ASSERT(for_statement -> get_for_init_stmt() != NULL);
    ROSE_ASSERT(for_statement -> get_for_init_stmt() -> get_parent() != NULL);
    ROSE_ASSERT(for_statement -> get_for_init_stmt() -> get_startOfConstruct() != NULL);
    // printf ("for_statement -> get_for_init_stmt() = %p \n", for_statement -> get_for_init_stmt());

    ROSE_ASSERT(for_statement -> get_startOfConstruct() != NULL);

    ROSE_ASSERT(for_statement -> get_parent() != NULL);

    ROSE_ASSERT(for_init_statement != NULL);
    ROSE_ASSERT(for_init_statement -> get_startOfConstruct() != NULL);
    ROSE_ASSERT(for_init_statement -> get_endOfConstruct() != NULL);
*/
}


JNIEXPORT void JNICALL Java_JavaParser_cactionIfStatement(JNIEnv *env, jclass, jboolean has_false_body, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionIfStatement() \n");

    //
    // NOTE: The function SageBuilder::buildIfStmt(...) is not invoked here because it does not
    // take NULL arguments. However, we need to construct the If statement here because we need
    // its scope and the arguments are not yet available.
    //
    SgIfStmt *ifStatement = new SgIfStmt((SgStatement *) NULL, (SgStatement *) NULL, (SgStatement *) NULL);
    ROSE_ASSERT(ifStatement != NULL);
    ifStatement -> set_parent(astJavaScopeStack.top());
    setJavaSourcePosition(ifStatement, env, jToken);
    astJavaScopeStack.push(ifStatement); // Push the SgIfStmt onto the stack.
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionIfStatementEnd(JNIEnv *env, jclass, jboolean has_false_body, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionIfStatementEnd() \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgIfStmt *if_statement = astJavaScopeStack.popIfStmt();
    ROSE_ASSERT(if_statement != NULL);
    ROSE_ASSERT(if_statement -> get_parent() != NULL);

    // If there are two required then the first is for the false branch.
    SgStatement *false_body = (has_false_body ? astJavaComponentStack.popStatement() : NULL);
    if (false_body) {
        if_statement -> set_false_body(false_body);
        false_body -> set_parent(if_statement);
    }

    SgStatement *true_body = astJavaComponentStack.popStatement();
    if_statement -> set_true_body(true_body);
    true_body -> set_parent(if_statement);

    SgExpression *condititonal_expression = astJavaComponentStack.popExpression();
    SgExprStatement *expression_statement = SageBuilder::buildExprStatement(condititonal_expression);
    setJavaSourcePosition(expression_statement, env, jToken); // TODO: copy location from conditional expression instead of jToken!
    if_statement -> set_conditional(expression_statement);
    expression_statement -> set_parent(if_statement);

    astJavaComponentStack.push(if_statement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionImportReference(JNIEnv *env, jclass,
                                                              jboolean java_is_static,
                                                              jstring java_qualified_name,
                                                              jboolean java_contains_wildcard,
                                                              jobject jToken) {
    // This is the import statement.  The semantics is to include the named file and add its 
    // declarations to the global scope so that they can be referenced by the current file.
    // The import directive tells the compiler where to look for the class definitions 
    // when it comes upon a class that it cannot find in the default java.lang package.

    if (SgProject::get_verbose() > 1)
        printf ("Inside of Java_JavaParser_cactionImportReference() \n");

    bool is_static = java_is_static;
    SgName qualified_name = convertJavaStringToCxxString(env, java_qualified_name);
    bool contains_wildcard = java_contains_wildcard;

    // I could not debug passing a Java "Boolean" variable, but "int" works fine.
    // containsWildcard = convertJavaBooleanToCxxBoolean(env, input_containsWildcard);
    // containsWildcard = (bool) (env -> CallBooleanMethod(xxx, input_containsWildcard) == 1);
    // containsWildcard = (bool) input_containsWildcard;
    // containsWildcard = (bool) (env -> CallStaticBooleanMethod(xxx, java_containsWildcard) == 1);
    // containsWildcard = (java_containsWildcard == 1);

    // printf ("import qualifiedName = %s containsWildcard = %s \n", qualifiedName.str(), containsWildcard ? "true" : "false");

    SgJavaImportStatement *importStatement = SageBuilder::buildJavaImportStatement(qualified_name, contains_wildcard);
    ROSE_ASSERT(importStatement != NULL);
    importStatement -> set_parent(astJavaScopeStack.top()); // We also have to set the parent so that the stack debugging output will work.
    setJavaSourcePosition(importStatement, env, jToken);

    if (is_static) {
        importStatement -> get_declarationModifier().get_storageModifier().setStatic();
    }

    ROSE_ASSERT(! astJavaScopeStack.empty());

// TODO: Remove this! 12/09/13
//    astJavaComponentStack.push(importStatement);

    SgJavaImportStatementList *import_list = ::currentSourceFile -> get_import_list();
    import_list -> get_java_import_list().push_back(importStatement);
// TODO: Remove this!
/*
    if (import_list -> get_file_info() == NULL) { // The first
        setJavaSourcePosition(import_list, env, jToken);
    }

    if (type_name.getString().size() > 0) { // we are importing a type?
        SgClassType *class_type =  isSgClassType(lookupTypeByName(package_name, type_name, 0));
        ROSE_ASSERT(class_type);

        // DQ (8/22/2011): The Java generics support is tied to the handling of the import statement so that we can find
        // the parameterized class from the name when it appears without name qualification.
        // The import statement should act like the using namespace directive in C++ to bring in a class or set of classes
        // so that they will be visible in the current scope.  On the Java side the classes have all been read.  Now we
        // just have to build the SgAliasSymbol in the current scope (do this tomorrow morning).
        // printf ("Now build the SgAliasSymbol in the current scope \n");

        // DQ (8/23/2011): This is part of the AST post-processing, but it has to be done as we process the Java import 
        // statements (top down) so that the symbol tables will be correct and variable, function, and type references 
        // will be resolved correctly.

        // This is most likely global scope (where import statements are typically used).
        SgScopeStatement *currentScope = astJavaScopeStack.top();
        ROSE_ASSERT(currentScope != NULL);

        // SgSymbol *importClassSymbol = lookupSymbolInParentScopesUsingQualifiedName(qualifiedName, currentScope);
        SgSymbol *importClassSymbol = class_type -> getAssociatedDeclaration() -> search_for_symbol_from_symbol_table();

        ROSE_ASSERT(importClassSymbol != NULL);

        if (contains_wildcard) {
            // This processing requires that we inject alias symbols from the reference class for all of its data members and member functions. 

            // Note that the enum values for e_default and e_public are equal.
            // SgAccessModifier::access_modifier_enum accessLevel = SgAccessModifier::e_default;
            SgAccessModifier::access_modifier_enum accessLevel = SgAccessModifier::e_public;

            SgScopeStatement *referencedScope = get_scope_from_symbol(importClassSymbol);
            ROSE_ASSERT(referencedScope != NULL);

            FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope(referencedScope, currentScope, accessLevel);
        }
        else {
            // Find the referenced class and insert its symbol as an alias into the current scope.

            // printf ("Find the referenced class and insert its symbol as an alias into the current scope. \n");

            SgAliasSymbol *aliasSymbol = new SgAliasSymbol(importClassSymbol);

            // Use the current name and the alias to the symbol
            list<SgName> qualifiedNameList = generateQualifierList(qualifiedName);
            SgName unqualifiedName = *(qualifiedNameList.rbegin());

            // printf ("Building an alias (SgAliasSymbol) for unqualifiedName = %s in qualifiedName = %s \n", unqualifiedName.str(), qualifiedName.str());

            currentScope -> insert_symbol(unqualifiedName, aliasSymbol);
        }
    }
*/

    if (SgProject::get_verbose() > 1)
        printf ("Leaving Java_JavaParser_cactionImportReference() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInitializer(JNIEnv *env, jclass, jboolean java_is_static, jstring java_string, jint initializer_index, jobject jToken) {
    SgName name = convertJavaStringToCxxString(env, java_string);

 // DQ (3/25/2017): Eliminate Clang warning for unused variable.
 // bool isStatic = java_is_static;

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition);

    //TODO: REMOVE this!!!
    //
    // The initializer block has no return type as it is not a real method. So, we
    // start by pushing a VOID return type to make it look like a method.
    //
    //    astJavaComponentStack.push(SgTypeVoid::createType()); 

#if 0
 // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=comment.
    SgMemberFunctionDeclaration *method_declaration = lookupMemberFunctionDeclarationInClassScope(class_definition, name, 0 /* no arguments */);
    ROSE_ASSERT(method_declaration != NULL);

    // This is not a defining function declaration so we can't identify the SgFunctionDefinition and push it's body onto the astJavaScopeStack.
    SgFunctionDefinition *method_definition = method_declaration -> get_definition();
    ROSE_ASSERT(method_definition != NULL);
#endif

    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("method-members-map");
    ROSE_ASSERT(attribute);
    SgFunctionDefinition *method_definition = isSgFunctionDefinition(attribute -> getNode(initializer_index));
    ROSE_ASSERT(method_definition != NULL);

//TODO: REMOVE this!!!
/*
    SgBasicBlock *type_space = isSgBasicBlock(((AstSgNodeAttribute *) method_definition -> getAttribute("type_space")) -> getNode());
    ROSE_ASSERT(type_space);
    astJavaScopeStack.push(type_space);
*/

    astJavaScopeStack.push(method_definition);
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInitializerEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("End of SgMemberFunctionDeclaration (method) \n");

    ROSE_ASSERT(isSgBasicBlock(astJavaComponentStack.top()));
    SgBasicBlock *initializer_body = (SgBasicBlock *) astJavaComponentStack.popStatement();

    SgFunctionDefinition *memberFunctionDefinition = astJavaScopeStack.popFunctionDefinition();
    memberFunctionDefinition -> set_body(initializer_body);

//TODO: REMOVE this!!!
/*
    SgBasicBlock *type_space = isSgBasicBlock(astJavaScopeStack.pop());
    ROSE_ASSERT(type_space);
*/
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInstanceOfExpression(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInstanceOfExpression() \n");

    // Do Nothing on the way down.
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInstanceOfExpressionEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInstanceOfExpressionEnd() \n");

    SgType *type = astJavaComponentStack.popType();
    ROSE_ASSERT(type);

    SgExpression *exp = astJavaComponentStack.popExpression();
    SgExpression *result = SageBuilder::buildJavaInstanceOfOp(exp, type);
    result -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));

    astJavaComponentStack.push(result);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionIntLiteral(JNIEnv *env, jclass, jint java_value, jstring java_source, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build IntVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    int value = java_value;
    SgName source = convertJavaStringToCxxString(env, java_source);

    if (SgProject::get_verbose() > 1)
        printf ("Building an integer value expression = %d with string representaion %s\n", value, source.getString().c_str());

    SgIntVal *integer_value = SageBuilder::buildIntVal_nfi(value, source);

    setJavaSourcePosition(integer_value, env, jToken);

    astJavaComponentStack.push(integer_value);
}

//
// Java Doc is not supported!
//
/*
JNIEXPORT void JNICALL Java_JavaParser_cactionJavadoc(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocAllocationExpression(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocAllocationExpressionClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArgumentExpression(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArgumentExpressionClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArrayQualifiedTypeReference(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArraySingleTypeReference(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArraySingleTypeReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocFieldReference(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocFieldReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocImplicitTypeReference(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocImplicitTypeReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocMessageSend(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocMessageSendClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocQualifiedTypeReference(JNIEnv *env, jclass, jobject jToken)  {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocQualifiedTypeReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocReturnStatement(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocReturnStatementClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleNameReference(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleNameReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleTypeReference(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleTypeReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    // Ignore Javadoc structures !!!
}
*/

JNIEXPORT void JNICALL Java_JavaParser_cactionLabeledStatement(JNIEnv *env, jclass, jstring labelName, jobject jToken) {
    SgName label_name = convertJavaStringToCxxString(env, labelName);
    SgJavaLabelStatement *labelStatement = SageBuilder::buildJavaLabelStatement(label_name);
    ROSE_ASSERT(labelStatement != NULL);
    setJavaSourcePosition(labelStatement, env, jToken);
    labelStatement -> set_parent(astJavaScopeStack.top());
    astJavaScopeStack.push(labelStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionLabeledStatementEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionLabelStatementEnd() \n");

    SgJavaLabelStatement *labelStatement = astJavaScopeStack.popJavaLabelStatement();

    SgStatement *statement = astJavaComponentStack.popStatement();
    labelStatement -> set_statement(statement);

    // Pushing 'label' on the statement stack
    astJavaComponentStack.push(labelStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionLocalDeclaration(JNIEnv *env, jclass, jint num_annotations, jstring java_variable_name, jboolean is_final, jobject jToken) {
    SgName variable_name = convertJavaStringToCxxString(env, java_variable_name);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionLocalDeclaration() for %s\n", variable_name.getString().c_str());

    SgType *type = astJavaComponentStack.popType();
    ROSE_ASSERT(type);

    // Note that the type should have already been built and should be on the astJavaComponentStack.
    SgVariableDeclaration *variable_declaration = SageBuilder::buildVariableDeclaration(variable_name, type, NULL, astJavaScopeStack.top());
    ROSE_ASSERT(variable_declaration != NULL);
    variable_declaration -> set_parent(astJavaScopeStack.top());
    setJavaSourcePosition(variable_declaration, env, jToken);

    if (is_final) {
        variable_declaration -> get_declarationModifier().setFinal();
    }

    //
    //
    //
    if (num_annotations > 0) {
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astJavaComponentStack.popExpression();
            annotation -> set_parent(variable_declaration);
            annotations_attribute -> setNode(annotation, i);
        }
        variable_declaration -> setAttribute("annotations", annotations_attribute);
    }

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionLocalDeclaration() for %s \n", variable_name.getString().c_str());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionLocalDeclarationEnd(JNIEnv *env, jclass, jstring variable_name, jboolean hasInitializer, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionLocalDeclarationEnd() \n");

    SgName name = convertJavaStringToCxxString(env, variable_name);

    if (SgProject::get_verbose() > 2)
        printf ("Building a local variable declaration for name = %s \n", name.str());

    SgVariableSymbol *variable_symbol = astJavaScopeStack.top() -> lookup_variable_symbol(name);
    SgInitializedName *initialized_name = variable_symbol -> get_declaration();
    ROSE_ASSERT(initialized_name);
    ROSE_ASSERT(initialized_name -> get_scope() != NULL);
    initialized_name -> setAttribute("type", new AstRegExAttribute(getTypeName(initialized_name -> get_type())));
    setJavaSourcePosition(initialized_name, env, jToken);

    //
    //
    //
    if (hasInitializer) {
        SgExpression *initializer_expression = (hasInitializer ? astJavaComponentStack.popExpression() : NULL);
// TODO: Remove this !
/*
cout << "The expression is a "
<< initializer_expression -> class_name()
<< endl
<< "; The top of the stack is a "
     << (isSgClassDefinition(astJavaComponentStack.top()) ? isSgClassDefinition(astJavaComponentStack.top()) -> get_qualified_name().getString() : astJavaComponentStack.top() -> class_name())
<< endl;
cout.flush();
*/
        SgAssignInitializer *initializer = SageBuilder::buildAssignInitializer(initializer_expression, initialized_name -> get_type());
        ROSE_ASSERT(initializer != NULL);

        setJavaSourcePosition(initializer, env, jToken);
        setJavaSourcePosition(initializer_expression, env, jToken);

        initializer_expression -> set_parent(initializer);
        initialized_name -> set_initptr(initializer);
        initializer -> set_parent(initialized_name);

        ROSE_ASSERT(initializer -> get_parent() != NULL);
        ROSE_ASSERT(initializer -> get_parent() == initialized_name);
    }

    SgVariableDeclaration *variable_declaration = isSgVariableDeclaration(initialized_name -> get_declaration());
    ROSE_ASSERT(variable_declaration != NULL);
    // Local declarations have no definition for this (default is NOT public so we need to explicity mark this as unknown).
    variable_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);
    ROSE_ASSERT(! variable_declaration -> get_declarationModifier().get_accessModifier().isPublic());

    // Save it on the stack so that we can add SgInitializedNames to it.
    astJavaComponentStack.push(variable_declaration);

    if (SgProject::get_verbose() > 0)
        variable_declaration -> get_file_info() -> display("source position in Java_JavaParser_cactionLocalDeclarationEnd(): debug");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionLongLiteral(JNIEnv *env, jclass, jlong java_value, jstring java_source, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build LongVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    long value = java_value;
    SgName source = convertJavaStringToCxxString(env, java_source);

    // printf ("Building an integer value expression = %d = %s \n", value, valueString.c_str());

    SgLongIntVal *longValue = SageBuilder::buildLongIntVal_nfi(value, source);
    ROSE_ASSERT(longValue != NULL);

    setJavaSourcePosition(longValue, env, jToken);

    astJavaComponentStack.push(longValue);

    if (SgProject::get_verbose() > 0)
        printf ("Done Building LongVal \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMarkerAnnotationEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build MarkerAnnotationEnd() \n");

    SgType *type = astJavaComponentStack.popType();

    SgJavaMarkerAnnotation *marker_annotation = SageBuilder::buildJavaMarkerAnnotation(type);
    marker_annotation -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));
    setJavaSourcePosition(marker_annotation, env, jToken);

    astJavaComponentStack.push(marker_annotation);

    if (SgProject::get_verbose() > 0)
        printf ("Done building MarkerAnnotationEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMemberValuePairEnd(JNIEnv *env, jclass, jstring java_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build MemberValuePairEnd() \n");

    SgName name = convertJavaStringToCxxString(env, java_name);
    SgExpression *value = astJavaComponentStack.popExpression();

    SgJavaMemberValuePair *member_value_pair = SageBuilder::buildJavaMemberValuePair(name, value);
    setJavaSourcePosition(member_value_pair, env, jToken);

    astJavaComponentStack.push(member_value_pair);

    if (SgProject::get_verbose() > 0)
        printf ("Done building MemberValuePairEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteralConcatenation(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet support string concatenation operation");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionNormalAnnotationEnd(JNIEnv *env, jclass, jint num_member_value_pairs, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build NormalAnnotationEnd() \n");

    list<SgJavaMemberValuePair *> pair_list;
    for (int i = 0; i < num_member_value_pairs; i++) {
        SgJavaMemberValuePair *member_value_pair = isSgJavaMemberValuePair(astJavaComponentStack.pop());
        ROSE_ASSERT(member_value_pair);
        pair_list.push_front(member_value_pair);
    }

    SgType *type = astJavaComponentStack.popType();
    SgJavaNormalAnnotation *normal_annotation = SageBuilder::buildJavaNormalAnnotation(type, pair_list);
    normal_annotation -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));
    setJavaSourcePosition(normal_annotation, env, jToken);

    astJavaComponentStack.push(normal_annotation);

    if (SgProject::get_verbose() > 0)
        printf ("Done Building NormalAnnotationEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionNullLiteral(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build support for null literal \n");

    SgNullExpression *null_expression = SageBuilder::buildNullExpression();
    setJavaSourcePosition(null_expression, env, jToken);

    astJavaComponentStack.push(null_expression);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionORORExpression(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionORORExpression() \n");

    // Nothing to do !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionORORExpressionEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionORORExpressionEnd() \n");

    binaryExpressionSupport<SgOrOp>();

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedTypeReference(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionParameterizedTypeReference() \n");

    // Nothing to do here !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedTypeReferenceEnd(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jboolean has_type_arguments, int java_num_type_arguments, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Entering Java_JavaParser_cactionParameterizedTypeReferenceEnd() \n");

    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

    int num_type_arguments = java_num_type_arguments;

// TODO: Remove this
//cout << "Here 12 with package " << package_name.getString() << " and type " << type_name.getString() 
//     << " with " << num_type_arguments << " arguments"
//<< endl;
//cout.flush();
    SgNamedType *raw_type = isSgNamedType(lookupTypeByName(package_name, type_name, 0));
    ROSE_ASSERT(raw_type);
    list<SgTemplateParameter *> type_list;
    for (int i = 0; i < num_type_arguments; i++) {
        SgType *type_argument = astJavaComponentStack.popType();
        ROSE_ASSERT(type_argument);
        SgTemplateParameter *templateParameter = new SgTemplateParameter(type_argument, NULL);
        type_list.push_front(templateParameter); // place the arguments in the list in reverse to restore their proper order
    }
    SgTemplateParameterPtrList ordered_type_list;
    while(! type_list.empty()) {
        ordered_type_list.push_back(type_list.front());
        type_list.pop_front();
    }
    ROSE_ASSERT(has_type_arguments);
    SgJavaParameterizedType *parameterized_type = SageBuilder::getUniqueJavaParameterizedType(raw_type, &ordered_type_list);

    astJavaComponentStack.push(parameterized_type);

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionParameterizedTypeReferenceEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedQualifiedTypeReferenceEnd(JNIEnv *env, jclass, jstring java_type_name, jboolean has_type_arguments, int java_num_type_arguments, jobject jToken) {
    SgName type_name = convertJavaStringToCxxString(env, java_type_name);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionParameterizedQualifiedTypeReferenceEnd() for %s \n", type_name.getString().c_str());

    int num_type_arguments = java_num_type_arguments;

    list<SgTemplateParameter *> type_list;
    for (int i = 0; i < num_type_arguments; i++) {
        SgType *type_argument = astJavaComponentStack.popType();
        SgTemplateParameter *templateParameter = new SgTemplateParameter(type_argument, NULL);
        type_list.push_front(templateParameter); // place the arguments in the list in reverse to restore their proper order
    }
    SgTemplateParameterPtrList ordered_type_list;
    while(! type_list.empty()) {
        ordered_type_list.push_back(type_list.front());
        type_list.pop_front();
    }
    ROSE_ASSERT(has_type_arguments || num_type_arguments == 0);

    SgNamedType *base_type = isSgNamedType(astJavaComponentStack.popType());
// TODO: Remove this !!!
/*
    SgClassType *base_class_type = isSgClassType(base_type);
    SgJavaParameterizedType *base_parameterized_type = isSgJavaParameterizedType(base_type);
    SgJavaQualifiedType *base_qualified_type = isSgJavaQualifiedType(base_type);
    ROSE_ASSERT(base_class_type || base_parameterized_type || base_qualified_type);
    SgClassDeclaration *class_declaration = isSgClassDeclaration((base_class_type ? base_class_type -> get_declaration() : base_parameterized_type ? base_parameterized_type -> get_declaration() : base_qualified_type -> get_declaration())-> get_definingDeclaration());
*/
    SgClassDeclaration *class_declaration = isSgClassDeclaration(base_type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);

    SgClassSymbol *class_symbol = lookupUniqueSimpleNameTypeInClass(type_name, class_declaration -> get_definition());
if (! class_symbol){
cout << "Could not find type " << type_name.getString() << " in " << class_declaration -> get_qualified_name().getString() << endl;
cout.flush();
}
    ROSE_ASSERT(class_symbol);
    SgNamedType *raw_type = isSgNamedType(class_symbol -> get_type());
    ROSE_ASSERT(raw_type);

    //
    // 
    //
    SgClassDeclaration *raw_class_declaration = isSgClassDeclaration(raw_type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(raw_class_declaration != NULL);
    SgNamedType *type = isSgNamedType(has_type_arguments ? (SgType *) SageBuilder::getUniqueJavaParameterizedType(raw_type, &ordered_type_list) : (SgType *) raw_type);
    ROSE_ASSERT(type);

    SgJavaQualifiedType *qualified_type = SageBuilder::getUniqueJavaQualifiedType(raw_class_declaration, base_type, type);

    astJavaComponentStack.push(qualified_type);

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionParameterizedQualifiedTypeReferenceEnd() for %s \n", type_name.getString().c_str());
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPostfixExpression(JNIEnv *env, jclass, jobject jToken) {
    // Nothing To Do !!!
}

JNIEXPORT void JNICALL Java_JavaParser_cactionPostfixExpressionEnd(JNIEnv *env, jclass, jint java_operator_kind, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionPostfixExpressionEnd() \n");

    // These are the operator code values directly from ECJ.
    enum ops // NO_STRINGIFY
    {
        ERROR_OPERATOR = 0, // This is not a ECJ value 
        MINUS          = 13,
        PLUS           = 14,
        LAST_OPERATOR 
    };

    int operator_kind = java_operator_kind;
    // printf ("operator_kind = %d \n", operator_kind);

    switch(operator_kind) {
        // Operator codes used by the UnaryExpression in ECJ.
        case PLUS:  unaryExpressionSupport<SgPlusPlusOp>(); break;
        case MINUS: unaryExpressionSupport<SgMinusMinusOp>(); break;

        default:
            //
            // If this file was not already tagged as an erroneous file, do so now.
            //
            if (! ::currentSourceFile -> attributeExists("error")) {
                ::currentSourceFile -> setAttribute("error", new AstRegExAttribute("Illegal use of a postfix operator"));
            }

            cout << "Error: default reached in cactionPostfixExpressionEnd() operator_kind = " <<  operator_kind << endl;
            cout.flush();
            unaryExpressionSupport<SgPlusPlusOp>(); // Any operator so that we can "keep going" !!! EROSE_ASSERT(false);
    }

    // Mark this a a postfix operator
    SgUnaryOp *unaryOp = isSgUnaryOp(astJavaComponentStack.top());
    ROSE_ASSERT(unaryOp != NULL);
    unaryOp -> set_mode(SgUnaryOp::postfix);

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPrefixExpression(JNIEnv *env, jclass, jobject jToken) {
     // Nothing to do !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPrefixExpressionEnd(JNIEnv *env, jclass, jint java_operator_kind, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionPrefixExpressionEnd() \n");

    // These are the operator code values directly from ECJ.
    enum ops // NO_STRINGIFY
    {
        ERROR_OPERATOR = 0, // This is not a ECJ value 
        MINUS          = 13,
        PLUS           = 14,
        LAST_OPERATOR
    };

    int operator_kind = java_operator_kind;
    // printf ("operator_kind = %d \n", operator_kind);

    switch(operator_kind) {
        // Operator codes used by the UnaryExpression in ECJ.
        case PLUS:  unaryExpressionSupport<SgPlusPlusOp>(); break;
        case MINUS: unaryExpressionSupport<SgMinusMinusOp>(); break;

        default:
            //
            // If this file was not already tagged as an erroneous file, do so now.
            //
            if (! ::currentSourceFile -> attributeExists("error")) {
                ::currentSourceFile -> setAttribute("error", new AstRegExAttribute("Illegal use of a prefix operator"));
            }

            cout << "Error: default reached in cactionPrefixExpressionEnd() operator_kind = " <<  operator_kind << endl;
            cout.flush();
            unaryExpressionSupport<SgPlusPlusOp>(); // Any operator so that we can "keep going" !!! EROSE_ASSERT(false);
    }

    // Mark this a a prefix operator
    SgUnaryOp *unaryOp = isSgUnaryOp(astJavaComponentStack.top());
    ROSE_ASSERT(unaryOp != NULL);
    unaryOp -> set_mode(SgUnaryOp::prefix);

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedAllocationExpression(JNIEnv *env, jclass, jobject jToken) {
    // Nothing To Do !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedAllocationExpressionEnd(JNIEnv *env, jclass, jboolean has_type, jboolean java_contains_enclosing_instance, jint java_num_arguments, jboolean java_is_anonymous, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionQualifiedAllocationExpressionEnd() \n");

    bool contains_enclosing_instance = java_contains_enclosing_instance;
    int num_arguments = java_num_arguments;
    bool is_anonymous = java_is_anonymous;

    SgClassDeclaration *declaration = isSgClassDeclaration(is_anonymous ? astJavaComponentStack.pop() : NULL);
    list<SgExpression*> argument_list;
    for (int i = 0; i < num_arguments; i++) { // pop the arguments in reverse order
        SgExpression *arg = astJavaComponentStack.popExpression();
        argument_list.push_front(arg);
    }
    vector<SgExpression*> arguments;
    while (! argument_list.empty()) { // place the arguments in the vector proper order
        arguments.push_back(argument_list.front());
        argument_list.pop_front();
    }

    //
    // A null type indicates that we are dealing with parameters for an Enum constant.
    // In such a case, we use the Object type...
    //
    SgType *type = (has_type ? astJavaComponentStack.popType() : ::ObjectClassType);
    ROSE_ASSERT(type);
    SgExpression *expression_prefix = (contains_enclosing_instance ? astJavaComponentStack.popExpression() :  NULL);

    SgConstructorInitializer *constInit = SageBuilder::buildConstructorInitializer(NULL,
                                                                                   SageBuilder::buildExprListExp(arguments),
                                                                                   type,
                                                                                   false,
                                                                                   false,
                                                                                   false,
                                                                                   ! (isSgNamedType(type))); // ! (isSgClassType(type)));
    setJavaSourcePosition(constInit, env, jToken);

    // TODO: I think a SgJavaParameterizedType should be a SgClassType.  Currrently, it is not!

    // For the simple case, we only need the type as input to build SgNewExp.
    SgExprListExp *exprListExp      = NULL;
    SgExpression *expr              = NULL;
    short int val                   = 0;
    SgFunctionDeclaration *funcDecl = NULL;

    SgNewExp *newExpression = SageBuilder::buildNewExp(type, exprListExp, constInit, expr, val, funcDecl);
    ROSE_ASSERT(newExpression != NULL);
    constInit -> set_parent(newExpression);

    //
    // TODO: Temporary patch until the SgNewExp can handle anonymous types.
    //
    if (is_anonymous) {
        newExpression -> setAttribute("body", new AstSgNodeAttribute(declaration));
    }

    setJavaSourcePosition(newExpression, env, jToken);

    SgExpression *result = newExpression;
    if (expression_prefix) {
        newExpression -> setAttribute("type", new AstRegExAttribute(getUnqualifiedTypeName(type)));
        result = SageBuilder::buildBinaryExpression<SgDotExp>(expression_prefix, newExpression);
        setJavaSourcePosition(result, env, jToken);
    }
    else {
        newExpression -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));
    }
    astJavaComponentStack.push(result);
}


// TODO: Remove this !!!
/*
JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReference(JNIEnv *env, jclass, jobject jToken) {
    // Build a member function call...
    if (SgProject::get_verbose() > 0)
        printf ("Build a Qualified Super Reference\n");

    // Do Nothing on the way down!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReferenceEnd(JNIEnv *env, jclass, jobject jToken) {
    SgClassType *type = isSgClassType(astJavaComponentStack.popType());
    ROSE_ASSERT(type);
    SgClassDeclaration *class_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *class_definition = getCurrentTypeDefinition();
    ROSE_ASSERT(class_definition -> get_declaration());

    vector<SgBaseClass *> &inheritances = class_definition -> get_inheritances();
    ROSE_ASSERT(inheritances.size() > 0);
    SgClassDeclaration *super_declaration = inheritances[0] -> get_base_class();
    ROSE_ASSERT(! super_declaration -> get_explicit_interface()); // this class must have a super class

    class_definition = super_declaration -> get_definition(); // get the super class definition

    // SgClassSymbol *classSymbol = class_definition -> get_declaration() -> get_symbol();
    SgClassSymbol *classSymbol = isSgClassSymbol(class_definition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(classSymbol != NULL);

    SgSuperExp *superExp = SageBuilder::buildSuperExp(classSymbol);
    ROSE_ASSERT(superExp != NULL);

    superExp -> setAttribute("prefix", new AstRegExAttribute(getFullyQualifiedTypeName(super_declaration -> get_type()))); // TODO: Figure out how to extend the Sage representation to process this feature better.

    astJavaComponentStack.push(superExp);
}
*/

JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReference(JNIEnv *env, jclass, jobject jToken) {
    // Build a member function call...
    if (SgProject::get_verbose() > 0)
        printf ("Build a Qualified Super Reference\n");

    // Do Nothing on the way down!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReferenceEnd(JNIEnv *env, jclass, jobject jToken) {
    SgNamedType *type = isSgNamedType(astJavaComponentStack.popType());
    ROSE_ASSERT(type);
    SgClassDeclaration *class_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition && class_definition -> get_declaration());

    SgClassSymbol *super_class_symbol = NULL;
    vector<SgBaseClass *> &inheritances = class_definition -> get_inheritances();
    if (inheritances.size() == 0 || inheritances[0] -> get_base_class() -> get_explicit_interface()) { // no super class specified?
        super_class_symbol = isSgClassSymbol(::ObjectClassDefinition -> get_declaration() -> search_for_symbol_from_symbol_table());
    }
    else {
        SgClassDeclaration *super_declaration = inheritances[0] -> get_base_class();
        ROSE_ASSERT(super_declaration && (! super_declaration -> get_explicit_interface())); // this class must have a super class
        super_class_symbol = isSgClassSymbol(super_declaration -> search_for_symbol_from_symbol_table());
    }
    ROSE_ASSERT(super_class_symbol);

    SgSuperExp *super_expression = SageBuilder::buildSuperExp(super_class_symbol);
    ROSE_ASSERT(super_expression);

/*
cout << "(5) * * * Processed a super expression with a type qualifier"
     << endl;
cout.flush();
*/
    SgJavaTypeExpression *type_expression = new SgJavaTypeExpression(type);
    setJavaSourcePosition(type_expression, env, jToken);
    type_expression -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));
    SgExpression *result = SageBuilder::buildBinaryExpression<SgDotExp>(type_expression, super_expression);

    astJavaComponentStack.push(result);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedThisReference(JNIEnv *env, jclass, jobject jToken) {
    // Build a member function call...
    if (SgProject::get_verbose() > 0)
        printf ("Build a Qualified This Reference\n");

    // Do Nothing on the way down!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedThisReferenceEnd(JNIEnv *env, jclass, jobject jToken) {
    SgNamedType *type = isSgNamedType(astJavaComponentStack.popType());
    ROSE_ASSERT(type);
    SgClassDeclaration *class_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition);
    SgClassSymbol *class_symbol = isSgClassSymbol(class_declaration -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(class_symbol != NULL);

    SgThisExp *this_expression = SageBuilder::buildThisExp(class_symbol);
    ROSE_ASSERT(this_expression);

    //    thisExp -> setAttribute("prefix", new AstRegExAttribute(getTypeName /*getFullyQualifiedTypeName*/(type))); // TODO: Figure out how to extend the Sage representation to process this feature better.
/*
cout << "(6) * * * Processed a This expression with a type qualifier"
     << endl;
cout.flush();
*/
    SgJavaTypeExpression *type_expression = new SgJavaTypeExpression(type);
    setJavaSourcePosition(type_expression, env, jToken);
    type_expression -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));
    SgExpression *result = SageBuilder::buildBinaryExpression<SgDotExp>(type_expression, this_expression);

    astJavaComponentStack.push(result);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionReturnStatement(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionReturnStatementEnd(JNIEnv *env, jclass, jboolean has_expression, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionReturnStatementEnd() \n");

    // Build the Return Statement
    SgExpression *expression = (has_expression ? astJavaComponentStack.popExpression() : NULL);
    SgReturnStmt *returnStatement = SageBuilder::buildReturnStmt_nfi(expression);
    ROSE_ASSERT(has_expression || returnStatement -> get_expression() == NULL); // TODO: there is an issue with the implementation of buildReturnStmt()...
    setJavaSourcePosition(returnStatement, env, jToken);

    // Pushing 'return' on the statement stack
    astJavaComponentStack.push(returnStatement);
}

JNIEXPORT void JNICALL Java_JavaParser_cactionSingleMemberAnnotationEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionSingleMemberAnnotationEnd() \n");

    SgExpression *value = astJavaComponentStack.popExpression();
    SgType *type = astJavaComponentStack.popType();

    SgJavaSingleMemberAnnotation *single_member_annotation = SageBuilder::buildJavaSingleMemberAnnotation(type, value);
    single_member_annotation -> setAttribute("type", new AstRegExAttribute(getTypeName(type)));
    setJavaSourcePosition(single_member_annotation, env, jToken);

    astJavaComponentStack.push(single_member_annotation);

    if (SgProject::get_verbose() > 2)
        printf ("Exiting Java_JavaParser_cactionSingleMemberAnnotationEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleNameReference(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jstring java_name, jobject jToken) {
    SgName package_name = convertJavaStringToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);
    SgName name = convertJavaStringToCxxString(env, java_name);

    SgVariableSymbol *variable_symbol = NULL;
    if (! type_name.getString().empty()) { // an instance variable?
        if (SgProject::get_verbose() > 0)
            printf ("Building a Single Name reference for name = %s%s%s \n", (package_name.getString().empty() ? "" : (package_name.getString() + ".")).c_str(), (type_name.getString() + ".").c_str(), name.str());

// TODO: Remove this
/*
cout << "Here 13" << endl;
cout.flush();
*/
        SgNamedType *type = isSgNamedType(lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */));
        ROSE_ASSERT(type);
        SgClassDeclaration *declaration = isSgClassDeclaration(type -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(declaration);
        ROSE_ASSERT(declaration -> get_definition());
        variable_symbol = lookupSimpleNameVariableInClass(name, declaration -> get_definition());
    }
    else { // a local variable!
        if (SgProject::get_verbose() > 0)
            printf ("Building a Single Name reference for name = %s \n", name.str());
        variable_symbol = lookupVariableByName(name);

// TODO: Remove this !!!

if (!variable_symbol){
 cout << "How come I could not find variable " << name.getString()
 << "; Here is the stack: "
 << endl;
for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); i != astJavaScopeStack.end(); i++) {
cout << "    "
<< (isSgClassDefinition(*i) ? isSgClassDefinition(*i) -> get_qualified_name().getString()
                            : isSgFunctionDefinition(*i) ? (isSgFunctionDefinition(*i) -> get_declaration() -> get_name().getString() + "(...)")
                                                         : (*i) -> class_name())
<< " ("
<< ((unsigned long) (*i))
<< ")"
<< endl;
cout.flush();
}
cout.flush();
}

        ROSE_ASSERT(variable_symbol);

        //
        // Check to see if this variable has an alias that should be substituted for it.
        //
        SgInitializedName *var_decl = variable_symbol -> get_declaration();
// TODO: Remove this !!!
/*
SgScopeStatement *field_definition_scope = var_decl -> get_scope();
cout << "Found field "
     << var_decl -> get_name().getString()
     << " with type "
     << getTypeName(var_decl -> get_type())
     << " in scope "
     << (isSgClassDefinition(field_definition_scope) ? isSgClassDefinition(field_definition_scope) -> get_qualified_name().getString() : field_definition_scope -> class_name())
     << endl;
cout.flush();
*/
        ROSE_ASSERT(var_decl);
        AstSgNodeAttribute *alias_attribute = (AstSgNodeAttribute *) var_decl -> getAttribute("real_name");
        if (alias_attribute) {
            var_decl = isSgInitializedName(alias_attribute -> getNode());
            ROSE_ASSERT(var_decl);
            variable_symbol = isSgVariableSymbol(var_decl -> search_for_symbol_from_symbol_table());

// TODO: Remove this !!!

if (! variable_symbol) {
cout << "How come I could not find variable " << var_decl -> get_name().getString()
     << " in scope " << (isSgFunctionDefinition(var_decl -> get_scope()) ? isSgFunctionDefinition(var_decl -> get_scope()) -> get_declaration() -> get_name().getString() : var_decl -> get_scope() -> class_name())
<< endl;
cout.flush();
}
            ROSE_ASSERT(variable_symbol);
/*
cout << "Substituting Alias variable " 
     << variable_symbol -> get_name().getString()
     << endl;
cout.flush();
*/
        }
    }

// TODO: Remove this !

if (!variable_symbol)
cout << "*** Could not find name "
 << (! type_name.getString().empty() ? type_name.getString() : name.getString())
     << " while processing file "
     << ::currentSourceFile -> getFileName()
 << endl;
cout.flush();

    ROSE_ASSERT(variable_symbol);

    SgVarRefExp *varRefExp = SageBuilder::buildVarRefExp(variable_symbol);
    ROSE_ASSERT(varRefExp != NULL);

    if (SgProject::get_verbose() > 0)
        printf ("In cactionSingleNameReference(): varRefExp = %p type = %p = %s \n", varRefExp, varRefExp -> get_type(), varRefExp -> get_type() -> class_name().c_str());

    setJavaSourcePosition(varRefExp, env, jToken);

    ROSE_ASSERT(! varRefExp -> get_file_info() -> isTransformation());
    ROSE_ASSERT(! varRefExp -> get_file_info() -> isCompilerGenerated());

    astJavaComponentStack.push(varRefExp);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionSuperReference(JNIEnv *env, jclass, jobject jToken) {
    SgClassDefinition *class_definition = getCurrentTypeDefinition();
    ROSE_ASSERT(class_definition && class_definition -> get_declaration());

    vector<SgBaseClass *> &inheritances = class_definition -> get_inheritances();
    if (inheritances.size() == 0 || inheritances[0] -> get_base_class() -> get_explicit_interface()) { // no super class specified?
        class_definition = ::ObjectClassDefinition; // ... then Object is the super class.
    }
    else {
        SgClassDeclaration *super_declaration = inheritances[0] -> get_base_class();
        ROSE_ASSERT(super_declaration && (! super_declaration -> get_explicit_interface())); // this class must have a super class
        class_definition = super_declaration -> get_definition(); // get the super class definition
    }

    SgClassSymbol *class_symbol = isSgClassSymbol(class_definition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(class_symbol != NULL);

    SgSuperExp *superExp = SageBuilder::buildSuperExp(class_symbol);
    ROSE_ASSERT(superExp != NULL);

    astJavaComponentStack.push(superExp);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionSwitchStatement(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionSwitchStatement() \n");

    // 
    // We build on the way down because the scope information and symbol table information is contained
    // in the Ast node.  This AST node is a subclass of SgScopeStatement
    //
    SgSwitchStatement *switchStatement = SageBuilder::buildSwitchStatement();
    ROSE_ASSERT(switchStatement != NULL);

    setJavaSourcePosition(switchStatement, env, jToken);

    // DQ (7/30/2011): For the build interface to work we have to initialize the parent pointer to the SgForStatement.
    // Charles4 (8/23/2011): When and why parent pointers should be set needs to be clarified. Perhaps the SageBuilder
    // functions should be revisited?
    switchStatement -> set_parent(astJavaScopeStack.top());

    astJavaScopeStack.push(switchStatement);
}

JNIEXPORT void JNICALL Java_JavaParser_cactionSwitchStatementEnd(JNIEnv *env, jclass, jint numCases, jboolean hasDefaultCase, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionSwitchStatementEnd() \n");

    SgBasicBlock *switch_block = SageBuilder::buildBasicBlock();
    switch_block -> set_parent(astJavaScopeStack.top());
    ROSE_ASSERT(isSgSwitchStatement(astJavaScopeStack.top()));

    // read 'nb_stmt' elements from the stmt stack
    // they should be every direct statement children the block has
    SgDefaultOptionStmt *default_stmt = NULL;
    for (int i = (hasDefaultCase ? numCases + 1 : numCases); i > 0; i--) {
        SgBasicBlock *case_block = SageBuilder::buildBasicBlock();

        SgStatement *sg_stmt = astJavaComponentStack.popStatement();
        while (! (isSgCaseOptionStmt(sg_stmt) || isSgDefaultOptionStmt(sg_stmt))) {
            case_block -> prepend_statement(sg_stmt);
            sg_stmt = astJavaComponentStack.popStatement();
        }
        case_block -> set_parent(sg_stmt);

        if  (isSgCaseOptionStmt(sg_stmt)) {
            SgCaseOptionStmt *case_stmt = (SgCaseOptionStmt *) sg_stmt;
            case_stmt -> set_body(case_block);
            case_stmt -> set_parent(switch_block);
            switch_block -> prepend_statement(case_stmt);
        }
        else {
            ROSE_ASSERT(default_stmt == NULL); // only onde default section is expected!
            default_stmt = (SgDefaultOptionStmt *) sg_stmt;
            default_stmt -> set_body(case_block);
            default_stmt -> set_parent(switch_block);
            switch_block -> prepend_statement(default_stmt);
        }
    }

    // Build the final Switch Statement
    SgSwitchStatement *switch_statement = astJavaScopeStack.popSwitchStatement();

    // Get the selectorl expression
    SgExpression *expr_selector = astJavaComponentStack.popExpression();

    SgExprStatement *item_selector = SageBuilder::buildExprStatement(expr_selector);
    item_selector -> set_parent(switch_statement);
    switch_statement -> set_item_selector(item_selector);
    switch_statement -> set_body(switch_block);

    // Pushing 'switch' on the statement stack
    astJavaComponentStack.push(switch_statement);
}

JNIEXPORT void JNICALL Java_JavaParser_cactionSynchronizedStatement(JNIEnv *env, jclass, jobject jToken) {
    // Nothing To Do !!!
}

JNIEXPORT void JNICALL Java_JavaParser_cactionSynchronizedStatementEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionSynchronizedStatementEnd() \n");

    SgBasicBlock *body = (SgBasicBlock *) astJavaComponentStack.popStatement();

    // Build the Synchronized Statement
    SgExpression *expression = astJavaComponentStack.popExpression();

    SgJavaSynchronizedStatement *synchronizedStatement = SageBuilder::buildJavaSynchronizedStatement(expression, body);
    setJavaSourcePosition(synchronizedStatement, env, jToken);

    // Pushing 'synchronized' on the statement stack
    astJavaComponentStack.push(synchronizedStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionThisReference(JNIEnv *env, jclass, jobject jToken) {
    SgClassDefinition *class_definition = getCurrentTypeDefinition();
    ROSE_ASSERT(class_definition);

    string className = class_definition -> get_declaration() -> get_name();
    // printf ("Current class for ThisReference is: %s \n", className.c_str());

    SgClassSymbol *class_symbol = isSgClassSymbol(class_definition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(class_symbol != NULL);

    SgThisExp *thisExp = SageBuilder::buildThisExp(class_symbol);
    ROSE_ASSERT(thisExp != NULL);

    astJavaComponentStack.push(thisExp);
}


// TODO: Same as function above... Remove it !
/*
JNIEXPORT void JNICALL Java_JavaParser_cactionThisReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    SgClassDefinition *class_definition = getCurrentTypeDefinition();

    string className = class_definition -> get_declaration() -> get_name();
    // printf ("Current class for ThisReference is: %s \n", className.c_str());

    // SgClassSymbol *classSymbol = class_definition -> get_declaration() -> get_symbol();
    SgClassSymbol *classSymbol = isSgClassSymbol(class_definition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(classSymbol != NULL);

    SgThisExp *thisExp = SageBuilder::buildThisExp(classSymbol);
    ROSE_ASSERT(thisExp != NULL);

    astJavaComponentStack.push(thisExp);
}
*/


JNIEXPORT void JNICALL Java_JavaParser_cactionThrowStatement(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do !!!
}

JNIEXPORT void JNICALL Java_JavaParser_cactionThrowStatementEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionThrowStatementEnd() \n");

    // Build the Throw Statement
    SgExpression *expression = astJavaComponentStack.popExpression();

    SgThrowOp *throw_op = SageBuilder::buildThrowOp(expression, SgThrowOp::throw_expression);

    SgJavaThrowStatement *throwStatement = SageBuilder::buildJavaThrowStatement(throw_op);
    setJavaSourcePosition(throwStatement, env, jToken);

    // Pushing 'throw' on the statement stack
    astJavaComponentStack.push(throwStatement);
}

JNIEXPORT void JNICALL Java_JavaParser_cactionTrueLiteral(JNIEnv *env, jclass, jobject jToken) {
    SgExpression *expression = SageBuilder::buildBoolValExp(true);
    astJavaComponentStack.push(expression);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCatchBlockEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionCatchBlockEnd() \n");

    SgBasicBlock *catch_body = (SgBasicBlock *) astJavaComponentStack.popStatement();
    ROSE_ASSERT(isSgBasicBlock(catch_body));

    SgCatchOptionStmt *catch_option_stmt = astJavaScopeStack.popCatchOptionStmt();
    catch_option_stmt -> set_body(catch_body);

    // Pushing 'Catch' on the statement stack
    astJavaComponentStack.push(catch_option_stmt);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionTryStatement(JNIEnv *env, jclass, jint numCatchBlocks, jboolean hasFinallyBlock, jobject jToken) {
    // Do Nothing!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionTryStatementEnd(JNIEnv *env, jclass, jint num_resources, jint num_catch_blocks, jboolean has_finally_block, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionTryStatement() \n");

    SgBasicBlock *finally_body = (SgBasicBlock *) (has_finally_block ? astJavaComponentStack.popStatement() : NULL);
    ROSE_ASSERT(finally_body == NULL || isSgBasicBlock(finally_body));

    list<SgCatchOptionStmt *> catches;
    for (int i = 0; i < num_catch_blocks; i++) {
        SgCatchOptionStmt *catch_option_stmt = (SgCatchOptionStmt *) astJavaComponentStack.popStatement();
        ROSE_ASSERT(isSgCatchOptionStmt(catch_option_stmt));
        catches.push_front(catch_option_stmt);
    }

    SgBasicBlock *try_body = (SgBasicBlock *) astJavaComponentStack.popStatement();
    ROSE_ASSERT(isSgBasicBlock(try_body));

    SgTryStmt *try_statement = SageBuilder::buildTryStmt(try_body, finally_body);
    setJavaSourcePosition(try_statement, env, jToken);
    try_statement -> set_parent(astJavaScopeStack.top());

    if (num_resources > 0) {
        AstSgNodeListAttribute *attribute = new AstSgNodeListAttribute();
        for (int i = num_resources - 1; i >= 0; i--) { // pop declarations from the stack and place them on the list in proper order.
            SgVariableDeclaration *local_declaration = isSgVariableDeclaration(astJavaComponentStack.popStatement());
            ROSE_ASSERT(local_declaration);
            attribute -> setNode(local_declaration, i);
        }
        try_statement -> setAttribute("resources", attribute);
    }

    //
    // charles4 09/23/2011 - Wwhen an SgTryStmt is allocated, its constructor
    // preallocates a SgCatchStementSeq (See comment in SageBuilder::buildTryStmt(...))
    // for the field p_catch_statement_sequence_root. The position of that field cannot
    // be set during the allocation as it was not yet set for the SgTryStmt being allocated.
    // Thus, we have to set the location here!
    //
    SgCatchStatementSeq *catch_statement_sequence = try_statement -> get_catch_statement_seq_root();
    setJavaSourcePosition(catch_statement_sequence, env, jToken);

    ROSE_ASSERT(catches.size() == (unsigned) num_catch_blocks);
    for (int i = 0; i < num_catch_blocks; i++) {
        SgCatchOptionStmt *catch_option_stmt = catches.front();
        catches.pop_front();
        ROSE_ASSERT(catch_option_stmt);
        try_statement -> append_catch_statement(catch_option_stmt);
    }

    // Pushing 'try' on the statement stack
    astJavaComponentStack.push(try_statement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionUnaryExpression(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Build an Unary Expression \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionUnaryExpressionEnd(JNIEnv *env, jclass, jint java_operator_kind, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Build an Unary Expression End \n");

    // These are the operator code values directly from ECJ.
    enum ops // NO_STRINGIFY
    {
        ERROR_OPERATOR = 0, // This is not a ECJ value 
        NOT            = 11,
        TWIDDLE        = 12,
        MINUS          = 13,
        PLUS           = 14,
        LAST_OPERATOR
    };

    int operator_kind = java_operator_kind;
    // printf ("operator_kind = %d \n", operator_kind);

    switch(operator_kind) {
        // Operator codes used by the UnaryExpression in ECJ.
        case NOT:     unaryExpressionSupport<SgNotOp>();           break;
        case TWIDDLE: unaryExpressionSupport<SgBitComplementOp>(); break;
        case MINUS:   unaryExpressionSupport<SgMinusOp>();         break;
        case PLUS:    unaryExpressionSupport<SgUnaryAddOp>();      break;

        default:
            //
            // If this file was not already tagged as an erroneous file, do so now.
            //
            if (! ::currentSourceFile -> attributeExists("error")) {
                ::currentSourceFile -> setAttribute("error", new AstRegExAttribute("Illegal use of a unary operator"));
            }

            cout << "Error: default reached in cactionUnaryExpressionEnd() operator_kind = " <<  operator_kind << endl;
            cout.flush();
            unaryExpressionSupport<SgUnaryAddOp>(); // Any operator so that we can "keep going" !!! EROSE_ASSERT(false);
    }

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionWhileStatement(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionWhileStatement() \n");

    //
    // NOTE: The function SageBuilder::buildWhileStmt(...) is not invoked here because it does not
    // take NULL arguments. However, we need to construct the while statement here because we need
    // its scope and the arguments are not yet available.
    //
    SgWhileStmt *while_statement = new SgWhileStmt((SgStatement *) NULL, (SgStatement *) NULL);
    ROSE_ASSERT(while_statement != NULL);
    while_statement -> set_parent(astJavaScopeStack.top());
    setJavaSourcePosition(while_statement, env, jToken);

    astJavaScopeStack.push(while_statement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionWhileStatementEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionWhileStatementEnd() \n");

    SgStatement *body = astJavaComponentStack.popStatement();
    SgExpression *test_expression = astJavaComponentStack.popExpression();
    SgExprStatement *test_statement = SageBuilder::buildExprStatement(test_expression);

    SgWhileStmt *while_statement = astJavaScopeStack.popWhileStmt();
    ROSE_ASSERT(while_statement != NULL);

    while_statement -> set_body(body);
    body -> set_parent(while_statement);

    while_statement -> set_condition(test_statement);
    test_statement -> set_parent(while_statement);

    astJavaComponentStack.push(while_statement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionWildcard(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside cactionWildcard \n");

    // Do Nothing!

    if (SgProject::get_verbose() > 2)
        printf ("Exiting cactionWildcard \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionWildcardEnd(JNIEnv *env, jclass, jboolean is_unbound, jboolean has_extends_bound, jboolean has_super_bound, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside cactionWildcardEnd \n");

    SgType *bound_type = (is_unbound ? NULL : astJavaComponentStack.popType());
// TODO: Temporary Patch
//if (! is_unbound) getTypeName(type);
    SgJavaWildcardType *wildcard = (is_unbound ? SageBuilder::getUniqueJavaWildcardUnbound()
                                               : (has_extends_bound ? SageBuilder::getUniqueJavaWildcardExtends(bound_type)
                                                                    : SageBuilder::getUniqueJavaWildcardSuper(bound_type)));
    ROSE_ASSERT(wildcard);

    astJavaComponentStack.push(wildcard);

    if (SgProject::get_verbose() > 2)
        printf ("Exiting cactionWildcardEnd \n");
}


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
#include "x10Parser.h"

#include "x10.h"

// Support functions so that this file can be restricted to be just parser (AST traversal) rules.
#include "x10_support.h"
//#include "java_support.h"
#include "jni_x10_utils.h"
#include "jni_x10SourceCodePosition.h"


// #include "VisitorContext.h"

// This is needed so that we can call the FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope() function.
#include "fixupCxxSymbolTablesToSupportAliasingSymbols.h"

using namespace std;
using namespace Rose::Frontend::Java;

SgClassDeclaration *buildDefiningClassDeclaration(SgName class_name, SgScopeStatement *scope) {
     SgClassDeclaration* nonDefiningDecl              = NULL;
     bool buildTemplateInstantiation                  = false;
     SgTemplateArgumentPtrList* templateArgumentsList = NULL;

     SgClassDeclaration* declaration = SageBuilder::buildClassDeclaration_nfi(class_name, SgClassDeclaration::e_class, scope, nonDefiningDecl , buildTemplateInstantiation, templateArgumentsList);
     ROSE_ASSERT(declaration != NULL);
     declaration -> set_parent(scope);
     declaration -> set_scope(scope);

     return declaration;
}


JNIEXPORT void JNICALL
Java_x10rose_visit_JNI_cactionInsertClassStart2(JNIEnv *env, jclass, jstring java_string, jobject jToken) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInsertClassStart2(): = %s \n", name.str());

    SgScopeStatement *outerScope = astJavaScopeStack.top();
    ROSE_ASSERT(outerScope != NULL);
    SgClassDeclaration *class_declaration = buildDefiningClassDeclaration(name, outerScope);
//MH-20140314
#if 1
    SgClassType *unknown = SgClassType::createType(class_declaration, NULL);
    astJavaComponentStack.push(unknown);
#else
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));
    setJavaSourcePosition(class_definition, env, jToken);
    astJavaScopeStack.push(class_definition); // to contain the class members...
#endif
}


JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionParenthesizedExpression(JNIEnv *env, jclass clz, jint java_parentheses_count) 
{ 
	Java_JavaParser_cactionParenthesizedExpression(env, clz, java_parentheses_count);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSetupSourceFilename(JNIEnv *env, jclass xxx, jstring java_full_file_name) 
{ 
#if 1
	Java_JavaParser_cactionSetupSourceFilename(env, xxx, java_full_file_name);
#else
    string full_file_name = convertJavaStringToCxxString(env, java_full_file_name);
    ::currentSourceFile = isSgSourceFile((*::project)[full_file_name]);
// TODO: Remove this!
//cout << "*+* Setting up source file " << full_file_name << endl;
//cout.flush();
    ROSE_ASSERT(::currentSourceFile);
    ROSE_ASSERT(::currentSourceFile -> get_file_info());
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionClearSourceFilename(JNIEnv *env, jclass xxx) 
{ 
	Java_JavaParser_cactionClearSourceFilename(env, xxx);
}

#if 1
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertClassStart(JNIEnv *env, jclass xxx, jstring java_string, jboolean is_interface, jboolean is_enum, jboolean is_anonymous, jobject jToken) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertClassStart(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken) 
#endif
{ 
#if 1
	Java_JavaParser_cactionInsertClassStart(env, xxx, java_string, is_interface, is_enum, is_anonymous, jToken);
#else
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInsertClassStart(): = %s \n", name.str());
    SgScopeStatement *outerScope = astJavaScopeStack.top();
    ROSE_ASSERT(outerScope != NULL);

    SgClassDeclaration *class_declaration = buildDefiningClassDeclaration(name, outerScope);
    setJavaSourcePosition(class_declaration, env, jToken);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));
    setJavaSourcePosition(class_definition, env, jToken);
//MH-20140501
	SgScopeStatement *type_space = SageBuilder::buildScopeStatement(class_definition);
    setJavaSourcePosition(type_space, env, jToken);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) class_declaration -> getAttribute("type_space");
    ROSE_ASSERT(attribute);
    attribute -> setNode(type_space);

    astJavaScopeStack.push(class_definition); // to contain the class members...
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertClassEnd(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionInsertClassEnd(env, xxx, java_string, jToken);
#else
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInsertClassEnd: %s \n", name.str());
    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgClassDefinition *class_definition = astJavaScopeStack.popClassDefinition();
    ROSE_ASSERT(! class_definition -> attributeExists("namespace"));
#endif
}


/**
 * Java_x10rose_visit_JNI_cactionBuildClassSupportStart(JNIEnv *env, jclass xxx, jstring java_name, jstring java_external_name, jboolean java_user_defined_class, jboolean java_has_conflicts, jboolean java_is_interface, jboolean java_is_enum, jboolean java_is_anonymous, jobject jToken) {
 *
 */
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildClassSupportStart(JNIEnv *env, jclass xxx, jstring java_name, jstring java_external_name, jboolean java_user_defined_class, jboolean java_has_conflicts, jboolean java_is_interface, jboolean java_is_enum, jboolean java_is_anonymous, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionBuildClassSupportStart(env, xxx, java_name, java_external_name, java_user_defined_class, java_is_interface, java_is_enum, java_is_anonymous, jToken);
#else
    SgName name = convertJavaStringToCxxString(env, java_name);
    SgName external_name = convertJavaStringToCxxString(env, java_external_name);
    bool user_defined_class = java_user_defined_class;
    bool has_conflicts = java_has_conflicts;
    bool is_interface = java_is_interface;
    bool is_enum = java_is_enum;
    bool is_anonymous = java_is_anonymous;

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildClassSupportStart(): %s %s \n", (is_interface ? "interface" : "class"), name.str());

    SgScopeStatement *outerScope = astJavaScopeStack.top();
    ROSE_ASSERT(outerScope != NULL);

    SgClassSymbol *class_symbol = outerScope -> lookup_class_symbol(name);
    ROSE_ASSERT(class_symbol);
    SgClassDeclaration *declaration = (SgClassDeclaration *) class_symbol -> get_declaration() -> get_definingDeclaration();
    ROSE_ASSERT(declaration);
    SgClassDefinition *class_definition = declaration -> get_definition();
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));
    astJavaScopeStack.push(class_definition); // to contain the class members...

    declaration -> set_explicit_interface(is_interface); // Identify whether or not this is an interface.
    declaration -> set_explicit_enum(is_enum);           // Identify whether or not this is an enum.

    SgClassType *class_type = declaration -> get_type();
    if (external_name.getString().size() > 0) {
        ROSE_ASSERT(class_type);
        class_type -> setAttribute("name", new AstRegExAttribute(external_name.getString()));
        if (is_anonymous) {
            class_type -> setAttribute("anonymous", new AstRegExAttribute(""));
            declaration -> setAttribute("anonymous", new AstRegExAttribute(""));
        }
    }
    //
    // Identify classes that are conflicting that must be fully-qualified when used.
    //
    if (has_conflicts) {
        class_type -> setAttribute("has_conflicts", new AstRegExAttribute(""));
    }

    //
    // If this is a user-defined class, we may need to keep track of some of its class members.
    //
    if (user_defined_class) {
        class_definition -> setAttribute("class_members", new AstSgNodeListAttribute());
        class_definition -> setAttribute("type_parameter_space", new AstSgNodeListAttribute());
    }

    astJavaComponentStack.push(class_definition); // To mark the end of the list of components in this type.

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionBuildClassSupportStart(): %s %s \n", (is_interface ? "interface" : "class"), name.str());

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateClassSupportStart(JNIEnv *env, jclass xxx, jstring java_name, jobject jToken) 
{ 
	Java_JavaParser_cactionUpdateClassSupportStart(env, xxx, java_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertTypeParameter(JNIEnv *env, jclass clz, jstring java_name, jobject jToken) 
{ 
	Java_JavaParser_cactionInsertTypeParameter(env, clz, java_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildTypeParameterSupport(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jint method_index, jstring java_name, jint num_bounds, jobject jToken) 
{ 
	Java_JavaParser_cactionBuildTypeParameterSupport(env, clz, java_package_name, java_type_name, method_index, java_name, num_bounds, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdatePushMethodParameterScope(JNIEnv *env, jclass clz, jstring str1, jstring str2, jobject jToken) 
{ 
	Java_JavaParser_cactionUpdatePushMethodParameterScope(env, clz, str1, str2, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateTypeParameterSupport(JNIEnv *env, jclass clz, jstring java_name, int method_index, jint num_bounds, jobject jToken) 
{ 
	Java_JavaParser_cactionUpdateTypeParameterSupport(env, clz, java_name, method_index, num_bounds, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdatePopMethodParameterScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionUpdatePopMethodParameterScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildClassExtendsAndImplementsSupport(JNIEnv *env, jclass xxx, jint java_num_type_parameters, jboolean java_has_super_class, jint java_num_interfaces, jobject jToken) 
{ 
	Java_JavaParser_cactionBuildClassExtendsAndImplementsSupport(env, xxx, java_num_type_parameters, java_has_super_class, java_num_interfaces, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildClassSupportEnd(JNIEnv *env, jclass xxx, jstring java_string, jint num_class_members, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionBuildClassSupportEnd(env, xxx, java_string, num_class_members, jToken);
#else
   SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildClassSupportEnd: %s \n", name.str());

    ROSE_ASSERT(! astJavaScopeStack.empty());
    SgClassDefinition *class_definition = astJavaScopeStack.popClassDefinition();
    ROSE_ASSERT(! class_definition -> attributeExists("namespace"));
    for (SgStatement *statement = astJavaComponentStack.popStatement();
#if 1
        statement != class_definition;
#else
        statement != NULL;
#endif
        statement = astJavaComponentStack.popStatement()) {

#if 1   
        if (SgProject::get_verbose() > 2) {
            cerr << "(1) Adding statement "
                 << statement -> class_name()
                 << " to an implicit Type Declaration"
                 << endl;
            cerr.flush();
        }
        ROSE_ASSERT(statement != NULL);

        class_definition -> prepend_statement(statement);
#endif
    }

    ROSE_ASSERT(! astJavaScopeStack.empty());
    SgScopeStatement *outerScope = astJavaScopeStack.top();

    SgClassDeclaration *class_declaration = class_definition -> get_declaration();
    ROSE_ASSERT(class_declaration);


    //
    // TODO:  Review this because of the package issue and the inability to build a global AST.
    //
    ROSE_ASSERT(outerScope != NULL);
    if (isSgClassDefinition(outerScope) && isSgClassDefinition(outerScope) -> attributeExists("namespace")) { // a type in a package?
        isSgClassDefinition(outerScope) -> append_statement(class_declaration);
    }
    else if (isSgClassDefinition(outerScope) && (! isSgClassDefinition(outerScope) -> attributeExists("namespace"))) { // an inner type?
        astJavaComponentStack.push(class_declaration);
    }
    else if (isSgBasicBlock(outerScope)) { // a local type declaration?
        astJavaComponentStack.push(class_declaration);
    }
    else if (outerScope == ::globalScope) { // a user-defined type?
        ::globalScope -> append_statement(class_declaration);
    }
    else { // What is this?
#if 1 //MH-20140313 so far push global scope
        ::globalScope -> append_statement(class_declaration);
#else
        ROSE_ASSERT(false);
#endif
    }

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionBuildClassSupportEnd: %s \n", name.str());

    astJavaScopeStack.push(::globalScope);
    astJavaScopeStack.push(class_definition);

// MH-20140326
#if 0
for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); i != astJavaScopeStack.end(); i++) {
cout << "Resolved type : "
<< (isSgClassDefinition(*i) ? isSgClassDefinition(*i) -> get_qualified_name().getString()
                            : isSgFunctionDefinition(*i) ? isSgFunctionDefinition(*i) -> get_qualified_name().getString()
                                                         : (*i) -> class_name())
<< " ("
<< (isSgClassDefinition(*i) ? isSgClassDefinition(*i) 
                            : isSgFunctionDefinition(*i) ? isSgFunctionDefinition(*i) 
                                                         : (*i)) 
<< ") "
<< endl;
}
#endif

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateClassSupportEnd(JNIEnv *env, jclass xxx, jstring java_name, jboolean has_super_class, jint num_interfaces, jint num_class_members, jobject jToken) 
{ 
	Java_JavaParser_cactionUpdateClassSupportEnd(env, xxx, java_name, has_super_class, num_interfaces, num_class_members, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildInnerTypeSupport(JNIEnv *env, jclass clz,                                                                     jstring java_package_name,
                                                                    jstring java_type_name,
                                                                    jobject jToken)

{

	Java_JavaParser_cactionBuildInnerTypeSupport(env, clz, java_package_name, java_type_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateInnerTypeSupport(JNIEnv *env, jclass clz,                                                                      jstring java_package_name,
                                                                     jstring java_type_name,
                                                                     jobject jToken)

{

	Java_JavaParser_cactionUpdateInnerTypeSupport(env, clz, java_package_name, java_type_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildMethodSupportStart(JNIEnv *env, jclass clz,                                                                       jstring java_name,
                                                                      jint method_index,
                                                                      jobject method_location) 
{
#if 0
	Java_JavaParser_cactionBuildMethodSupportStart(env, clz, java_name, method_index, method_location);
#else
   SgName name = convertJavaStringToCxxString(env, java_name);
    if (SgProject::get_verbose() > 1)
          printf ("Inside of BuildMethodSupportStart for method = %s \n", name.str());
    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));

//MH-20140403
#if 1
    //
    // This scope will be used to store Type Parameters, if there are any.
    //
    SgScopeStatement *type_space = new SgScopeStatement();
    type_space -> set_parent(class_definition);
    setJavaSourcePosition(type_space, env, method_location);
    if (method_index >= 0) {
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("type_parameter_space");
        ROSE_ASSERT(attribute);
        attribute -> setNode(type_space, method_index);
    }
#endif
//MH-20140315
//    astJavaScopeStack.push(class_definition);
//MH-20140403
    astJavaScopeStack.push(type_space);

    if (SgProject::get_verbose() > 1)
        printf ("Exiting BuildMethodSupportStart for method = %s \n", name.str());

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateMethodSupportStart(JNIEnv *env, jclass clz,                                                                        jstring java_name,
                                                                       jint method_index,
                                                                       jint num_formal_parameters,
                                                                       jobject method_location) 
{

	Java_JavaParser_cactionUpdateMethodSupportStart(env, clz, java_name, method_index, num_formal_parameters, method_location);
}

#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildArgumentSupport(JNIEnv *env, jclass clz, jstring java_argument_name, jstring java_argument_type_name, jboolean java_is_var_args, jboolean java_is_final, jobject jToken) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildArgumentSupport(JNIEnv *env, jclass, jstring java_argument_name, jboolean java_is_var_args, jboolean java_is_final, jobject jToken) 
#endif
{ 
#if 0
	Java_JavaParser_cactionBuildArgumentSupport(env, clz, java_argument_name, java_argument_type_name, java_is_var_args, java_is_final, jToken);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Build argument support\n");

// TODO: Remove this !!!
//    SgFunctionDefinition *method_definition = isSgFunctionDefinition(astJavaScopeStack.top());
//    ROSE_ASSERT(method_definition);

    SgName argument_name = convertJavaStringToCxxString(env, java_argument_name);
    bool is_final = java_is_final;
    bool is_var_args = java_is_var_args;

    if (SgProject::get_verbose() > 0)
        printf ("argument argument_name = %s \n", argument_name.str());

    SgType *argument_type = astJavaComponentStack.popType();
//cout << "ArgType="<< argument_type << endl;
    ROSE_ASSERT(argument_type);

    // Until we attached this to the AST, this will generate an error in the AST consistancy tests.
    SgInitializedName *initialized_name = SageBuilder::buildInitializedName(argument_name, argument_type, NULL);

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
    if (is_final) {
        initialized_name -> setAttribute("final", new AstRegExAttribute(""));
    }

    //
    // Identify Arguments with var arguments.
    //
    if (is_var_args) {
        SgPointerType *array_type = isSgPointerType(argument_type);
        ROSE_ASSERT(array_type);
        SgType *element_type = array_type -> get_base_type();

        initialized_name -> setAttribute("var_args", new AstSgNodeAttribute(element_type));
        initialized_name -> setAttribute("type", new AstRegExAttribute(getTypeName(element_type) + "..."));
    }
    else initialized_name -> setAttribute("type", new AstRegExAttribute(getTypeName(argument_type)));

// TODO: Remove this !!!
//    initialized_name -> set_scope(method_definition);
//    method_definition -> insert_symbol(argument_name, new SgVariableSymbol(initialized_name));

    astJavaComponentStack.push(initialized_name);

// TODO: Remove this!
//cout << "Pushed " << initialized_name->get_name() << ", " << initialized_name -> class_name() << endl; cout.flush();

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Build argument support\n");

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateArgumentSupport(JNIEnv *env, jclass clz, jint argument_index, jstring java_argument_name, jstring java_argument_type_name, jboolean is_var_args, jboolean is_final, jobject jToken) 
{ 
	Java_JavaParser_cactionUpdateArgumentSupport(env, clz, argument_index, java_argument_name, java_argument_type_name, is_var_args, is_final, jToken);
}

/*
 * JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildMethodSupportEnd(JNIEnv *env, jclass xxx,
                                                                    jstring java_string,
                                                                    jint method_index,
                                                                    jboolean java_is_constructor,
                                                                    jboolean java_is_abstract,
                                                                    jboolean java_is_native,
                                                                    jint java_number_of_type_parameters,
                                                                    jint java_number_of_arguments,
                                                                    jboolean java_is_user_defined,
                                                                    jobject method_location,
                                                                    jobject args_location) {

 **/
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildMethodSupportEnd(JNIEnv *env, jclass xxx,
               													   jstring java_string,
                                                                    jint method_index,
                                                                    jboolean java_is_constructor,
                                                                    jboolean java_is_abstract,
                                                                    jboolean java_is_native,
                                                                    jint java_number_of_type_parameters,
                                                                    jint java_number_of_arguments,
                                                                    jboolean java_is_user_defined,
                                                                    jobject method_location,
                                                                    jobject args_location) 
{
#if 0
	Java_JavaParser_cactionBuildMethodSupportEnd(env, xxx, java_string, method_index, java_is_constructor, java_is_abstract, java_is_native, java_number_of_type_parameters, java_number_of_arguments, java_is_compiler_generated, args_location, method_location);
#else
    SgName name = convertJavaStringToCxxString(env, java_string);
    int number_of_type_parameters = java_number_of_type_parameters;
    int number_of_arguments = java_number_of_arguments;
    bool is_constructor = java_is_constructor,
         is_abstract = java_is_abstract,
         is_native = java_is_native,
         is_user_defined = java_is_user_defined;

    if (SgProject::get_verbose() > 1)
        printf ("Build support for implicit class member function (method) name = %s \n", name.str());

//MH-20140403
    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());
    ROSE_ASSERT(type_space);

// TODO: Remove this !!!
//    SgFunctionDefinition *method_definition = isSgFunctionDefinition(((AstSgNodeAttribute *) type_space -> getAttribute("method")) -> getNode());
//    ROSE_ASSERT(method_definition);

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition != NULL && (! class_definition -> attributeExists("namespace")));

    //
    // There is no reason to distinguish between defining and non-defining declarations in Java...
    //
    SgMemberFunctionDeclaration *method_declaration = buildDefiningMemberFunction(name, class_definition, number_of_arguments, env, method_location, args_location);
    setJavaSourcePosition(method_declaration, env, method_location);
    ROSE_ASSERT(method_declaration != NULL);

    SgFunctionDefinition *method_definition = method_declaration -> get_definition();
    ROSE_ASSERT(method_definition);
    if (method_index >= 0) {
// TODO: Remove this !!!
//        method_definition -> setAttribute("type_space", new AstSgNodeAttribute(type_space));
//        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("class_members");
//        ROSE_ASSERT(attribute);
//        attribute -> setNode(method_definition, method_index);

        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("class_members");
        ROSE_ASSERT(attribute);
        attribute -> setNode(method_definition, method_index);

//MH-20140320
#if 0
        class_definition -> setAttribute("class_members", attribute);
#endif
    }

//MH-20140320
#if 0
vector<SgDeclarationStatement *> declarations = class_definition -> get_members();
printf("*Class definition:%p, member size=%d\n", class_definition, declarations.size());
#endif

    if (is_constructor) {
        method_declaration -> get_specialFunctionModifier().setConstructor();
    }
    if (is_abstract) {
        method_declaration -> get_declarationModifier().setJavaAbstract();
        method_declaration -> setForward(); // indicate that this function does not contain a body.
    }
    if (is_native) {
        method_declaration -> get_functionModifier().setJavaNative();
        method_declaration -> setForward(); // indicate that this function does not contain a body.
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

    // TODO: We need the next 3 lines for EDG4 
    // SgMemberFunctionDeclaration *nondefining_method_declaration = isSgMemberFunctionDeclaration(method_declaration -> get_firstNondefiningDeclaration());
    // ROSE_ASSERT(nondefining_method_declaration);
    // nondefining_method_declaration -> get_declarationModifier().get_accessModifier().set_modifier(method_declaration -> get_declarationModifier().get_accessModifier().get_modifier());

#if 0 
//MH-20140403 just to check emthod_declaration has sg_file_info 
    printf("method_declaration->get_startOfConstruct()=%p\n", method_declaration->get_startOfConstruct());
#endif

    astJavaComponentStack.push(method_declaration);

//MH-20140312
// Tentatively push class definition onto stack top.
// So far, this is necessary for passing an assertion in Java_x10rose_visit_JNI_cactionBuildMethodSupportStart 
// for checking if stack top is class definition or not.
#if 1
//    astJavaScopeStack.push(method_definition);
    astJavaScopeStack.push(class_definition);
#endif


#if 0
    SgSymbolTable *tab = class_definition->get_symbol_table();
    int size = tab->size();
    printf("Symbol table size=%d\n", size);
    tab->print();
#endif

    if (SgProject::get_verbose() > 1)
        printf ("Exiting build support for implicit class member function (method) name = %s \n", name.str());

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateMethodSupportEnd(JNIEnv *env, jclass xxx,                                                                      jstring java_string,
                                                                     jint method_index,
                                                                     jboolean is_compiler_generated,
                                                                     jint number_of_parameters,
                                                                     jobject args_location,
                                                                     jobject method_location) 
{

	Java_JavaParser_cactionUpdateMethodSupportEnd(env, xxx, java_string, method_index, is_compiler_generated, number_of_parameters, args_location, method_location);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildInitializerSupport(JNIEnv *env, jclass clz, jboolean java_is_static, jstring java_string, jint initializer_index, jobject jToken) 
{ 
	Java_JavaParser_cactionBuildInitializerSupport(env, clz, java_is_static, java_string, initializer_index, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildFieldSupport(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionBuildFieldSupport(env, xxx, java_string, jToken);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildFieldSupport (variable declaration for field) \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    SgType *type = astJavaComponentStack.popType();
    SgVariableDeclaration *variableDeclaration = SageBuilder::buildVariableDeclaration (name, type, NULL, astJavaScopeStack.top());
    ROSE_ASSERT(variableDeclaration != NULL);
    variableDeclaration -> set_parent(astJavaScopeStack.top());
//    setJavaSourcePosition(variableDeclaration, env, jToken);
    vector<SgInitializedName *> vars = variableDeclaration -> get_variables();
/*
    for (vector<SgInitializedName *>::iterator name_it = vars.begin(); name_it != vars.end(); name_it++) {
        setJavaSourcePosition(*name_it, env, jToken);
    }
*/

    astJavaComponentStack.push(variableDeclaration);

    if (SgProject::get_verbose() > 0)
        variableDeclaration -> get_file_info() -> display("source position in Java_JavaParser_cactionBuildFieldSupport(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Java_JavaParser_cactionBuildFieldSupport (variable declaration for field) \n");

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateFieldSupport(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken) 
{ 
	Java_JavaParser_cactionUpdateFieldSupport(env, xxx, java_string, jToken);
}

JNIEXPORT jboolean JNICALL Java_x10rose_visit_JNI_cactionIsSpecifiedSourceFile(JNIEnv *env, jclass clz, jstring java_full_file_name) 
{ 
	return Java_JavaParser_cactionIsSpecifiedSourceFile(env, clz, java_full_file_name);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTest(JNIEnv *env, jclass clz) 
{ 
	Java_JavaParser_cactionTest(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertImportedPackageOnDemand(JNIEnv *env, jclass clz, jstring java_package_name, jobject jToken) 
{ 
	Java_JavaParser_cactionInsertImportedPackageOnDemand(env, clz, java_package_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertImportedTypeOnDemand(JNIEnv *env, jclass clz, jstring java_package_name,  jstring java_type_name, jobject jToken) 
{ 
	Java_JavaParser_cactionInsertImportedTypeOnDemand(env, clz, java_package_name, java_type_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertImportedType(JNIEnv *env, jclass clz, jstring java_package_name,  jstring java_type_name, jobject jToken) 
{ 
	Java_JavaParser_cactionInsertImportedType(env, clz, java_package_name, java_type_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertImportedStaticField(JNIEnv *env, jclass clz, jstring java_variable_name, jobject jToken) 
{ 
	Java_JavaParser_cactionInsertImportedStaticField(env, clz, java_variable_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPushPackage(JNIEnv *env, jclass clz, jstring x10_package_name, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionPushPackage(env, clz, package_name, jToken);
#else
    SgName package_name = convertJavaStringToCxxString(env, x10_package_name);
    SgClassDefinition *package_definition = findOrInsertPackage(package_name, env, jToken);
    ROSE_ASSERT(package_definition);
// MH-20140502 comment out following two lines because cactionCompilationUnitDeclaration is immediately invoked after the invocation of this function
//    astJavaScopeStack.push(::globalScope);  // Push the global scope onto the stack.
//    astJavaScopeStack.push(package);        // Push the package onto the scopestack.
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdatePushPackage(JNIEnv *env, jclass clz, jstring java_package_name, jobject jToken) 
{ 
	Java_JavaParser_cactionUpdatePushPackage(env, clz, java_package_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPopPackage(JNIEnv *env, jclass clz) 
{ 
	Java_JavaParser_cactionPopPackage(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPushTypeScope(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jobject jToken) 
{ 
	Java_JavaParser_cactionPushTypeScope(env, clz, java_package_name, java_type_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPopTypeScope(JNIEnv *env, jclass clz) 
{ 
	Java_JavaParser_cactionPopTypeScope(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPushTypeParameterScope(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jobject jToken) 
{ 
	Java_JavaParser_cactionPushTypeParameterScope(env, clz, java_package_name, java_type_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPopTypeParameterScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionPopTypeParameterScope(env, clz, jToken);
}

//JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitList(JNIEnv *env, jclass clz, jint argc, jobjectArray argv) 
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitList(JNIEnv *env, jclass clz, jint, jobjectArray) 
{ 
#if 0
	Java_JavaParser_cactionCompilationUnitList(env, clz);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_x10rose_visit_JNI_cactionCompilationUnitList \n");

    // This is already setup by ROSE as part of basic file initialization before calling ECJ.
//MH-20140401
#if 0
    ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
#else
    ROSE_ASSERT(Rose::Frontend::Java::Ecj::Ecj_globalFilePointer != NULL);
#endif
    if (SgProject::get_verbose() > 0)
        printf ("Rose::Frontend::Java::Ecj::Ecj_globalFilePointer = %s \n", Rose::Frontend::Java::Ecj::Ecj_globalFilePointer -> class_name().c_str());

    // TODO: We need the next line for EDG4 
    // SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
    SgSourceFile *sourceFile = isSgSourceFile(Rose::Frontend::Java::Ecj::Ecj_globalFilePointer);

    ROSE_ASSERT(sourceFile != NULL);

    if (SgProject::get_verbose() > 0)
        printf ("sourceFile -> getFileName() = %s \n", sourceFile -> getFileName().c_str());

    // We don't use the SgProject but since it should have already been built, we can verify that it is present.
    ::project = sourceFile -> get_project();
    ROSE_ASSERT(::project);
	
    // Get the pointer to the global scope and push it onto the astJavaScopeStack.
    ::globalScope = sourceFile -> get_globalScope();
    ROSE_ASSERT(::globalScope != NULL);
	
    //
    // At this point, the scope stack should be empty. Push the global scope into it.
    //
//    ROSE_ASSERT(astJavaScopeStack.empty());
    astJavaScopeStack.push(::globalScope); // Push the global scope onto the stack.


#if 0 //MH-20140326
for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); i != astJavaScopeStack.end(); i++) {
//cout << "java_support.C lookupTypeSymbol 2" << *i << endl;
// TODO: Remove this!
cout << "Stored type "
<< (isSgClassDefinition(*i) ? isSgClassDefinition(*i) -> get_qualified_name().getString()
                            : isSgFunctionDefinition(*i) ? isSgFunctionDefinition(*i) -> get_qualified_name().getString()
                                                         : (*i) -> class_name())
<< " ("
<< (isSgClassDefinition(*i) ? isSgClassDefinition(*i)
                            : isSgFunctionDefinition(*i) ? isSgFunctionDefinition(*i)
                                                         : (*i))
<< ") "
<< endl;
}
#endif

    // Verify that the parent is set, these AST nodes are already setup by ROSE before calling this function.
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

     if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_x10rose_visit_JNI_cactionCompilationUnitList \n");
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitListEnd(JNIEnv *env, jclass clz) 
{ 
	Java_JavaParser_cactionCompilationUnitListEnd(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSetupBasicTypes(JNIEnv *env, jclass clz) 
{ 
	Java_JavaParser_cactionSetupBasicTypes(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPackageAnnotations(JNIEnv *env, jclass clz, int num_annotations, jobject jToken) 
{ 
	Java_JavaParser_cactionPackageAnnotations(env, clz, num_annotations, jToken);
}

#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitDeclaration(JNIEnv *env, jclass clz, jstring java_full_file_name, jstring java_package_name, jstring java_filename, jobject jToken) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitDeclaration(JNIEnv *env, jclass clz, jstring java_full_file_name, jstring java_package_name, jstring java_filename, jobject jToken) 
#endif
{ 
#if 0
	Java_JavaParser_cactionCompilationUnitDeclaration(env, clz, java_full_file_name, java_package_name, java_filename, jToken);
#else
    if (SgProject::get_verbose() > 0)
         printf ("Inside of Java_x10rose_visit_JNI_cactionCompilationUnitDeclaration() \n");

	string full_file_name = convertJavaStringToCxxString(env, java_full_file_name);
	ROSE_ASSERT(::currentSourceFile == isSgSourceFile((*::project)[full_file_name]));

    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name);
    ROSE_ASSERT(astJavaScopeStack.top() == ::globalScope); // There must be a scope element in the scope stack.

//MH-20140502
//////////////////////////////////////
    SgJavaPackageDeclaration *package_declaration = findPackageDeclaration(package_name);
    ROSE_ASSERT(package_declaration);
    SgClassDefinition *package_definition = package_declaration -> get_definition();
    ROSE_ASSERT(package_definition);
    ROSE_ASSERT(! ::currentSourceFile -> get_package());
    SgJavaPackageStatement *package_statement = SageBuilder::buildJavaPackageStatement(package_name);
	package_statement -> set_parent(package_definition);	
    ::currentSourceFile -> set_package(package_statement);
#if 0
    SgClassSymbol *namespace_symbol = ::globalScope -> lookup_class_symbol(package_name);
printf("namespace_symbol=%p\n", namespace_symbol);    
    ROSE_ASSERT(namespace_symbol);
    SgClassDeclaration *declaration = (SgClassDeclaration *) namespace_symbol -> get_declaration() -> get_definingDeclaration();
    SgClassDefinition *package = declaration -> get_definition();
    ROSE_ASSERT(package);

    //
    // Tag the package so that the unparser can process its containing user-defined types.
    //
    AstRegExAttribute *attribute =  new AstRegExAttribute(convertJavaStringToCxxString(env, java_package_name));
    package -> setAttribute("translated_package", attribute);
//    astJavaScopeStack.push(package); // Push the package onto the scopestack.
#endif
//////////////////////////////////////

    // Example of how to get the string...but we don't really use the absolutePathFilename in this function.
    const char *absolutePathFilename = env -> GetStringUTFChars(java_filename, NULL);
    ROSE_ASSERT(absolutePathFilename != NULL);
    // printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration absolutePathFilename = %s \n", absolutePathFilename);
    env -> ReleaseStringUTFChars(java_filename, absolutePathFilename);

    // This is already setup by ROSE as part of basic file initialization before calling ECJ.
#if 0
    ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
#else
    ROSE_ASSERT(Rose::Frontend::Java::Ecj::Ecj_globalFilePointer != NULL);
#endif

//////////// MH-20140404
//    SgSourceFile *sourceFile = isSgSourceFile(Rose::Frontend::Java::Ecj::Ecj_globalFilePointer);
    SgJavaClassDeclarationList* class_declaration_list = new SgJavaClassDeclarationList();
//MH-20140416
//    setJavaSourcePosition(class_declaration_list, env, jToken);
//MH-20140418
    class_declaration_list -> set_parent(currentSourceFile);
    currentSourceFile -> set_class_list(class_declaration_list);
//////////// 

    astJavaComponentStack.push(astJavaScopeStack.top()); // To mark the end of the list of components in this Compilation unit.
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitDeclarationEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionCompilationUnitDeclarationEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEcjFatalCompilationErrors(JNIEnv *env, jclass clz, jstring java_full_file_name) 
{ 
	Java_JavaParser_cactionEcjFatalCompilationErrors(env, clz, java_full_file_name);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitDeclarationError(JNIEnv *env, jclass clz, jstring java_error_message, jobject jToken) 
{ 
	Java_JavaParser_cactionCompilationUnitDeclarationError(env, clz, java_error_message, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeDeclaration(JNIEnv *env, jclass clz,                                                               jstring java_package_name,
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

#if 0
	Java_JavaParser_cactionTypeDeclaration(env, clz, java_package_name, java_type_name, num_annotations, has_super_class, java_is_annotation_interface, java_is_interface, java_is_enum, java_is_abstract, java_is_final, java_is_private, java_is_public, java_is_protected, java_is_static, java_is_strictfp, jToken);
#else
//          astJavaScopeStack.push(::globalScope);

       // We could provide a constructor for "SgName" that takes a "jstring".  This might help support a simpler interface.
        SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
               type_name = convertJavaStringToCxxString(env, java_type_name);

// push package
//  Java_x10rose_visit_JNI_cactionPushPackage(env, NULL, java_package_name, NULL);

//MH-20140418 Added
	SgScopeStatement *outerScope = astJavaScopeStack.top();

//MH-20140418 Changed <tt>::globalScope</tt> to <tt>outerScope</tt>
    SgClassDeclaration *class_declaration = buildDefiningClassDeclaration(type_name, /*::globalScope*/outerScope);

   ROSE_ASSERT(class_declaration);

    SgClassDefinition *definition = class_declaration->get_definition();
    ROSE_ASSERT(definition);
//  astJavaScopeStack.push(class_declaration->get_definition()); 

    // MH-20140311
    definition -> setAttribute("class_members", new AstSgNodeListAttribute());
    definition -> setAttribute("type_parameter_space", new AstSgNodeListAttribute());

    astJavaScopeStack.push(definition);
    astJavaComponentStack.push(definition);

    SgSymbol *sym = globalScope->lookup_symbol(type_name);

//  insert_symbol(type_name, class_declaration->get_symbol_from_symbol_table());
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeDeclarationHeader(JNIEnv *env, jclass clz,                                                                     jboolean java_has_super_class,
                                                                    jint num_interfaces,
                                                                    jint num_parameters,
                                                                    jobject jToken) 
{

	Java_JavaParser_cactionTypeDeclarationHeader(env, clz, java_has_super_class, num_interfaces, num_parameters, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeDeclarationEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionTypeDeclarationEnd(env, clz, jToken);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgClassDeclaration (cactionTypeDeclarationEnd) \n");

//MH-20140403
    SgSourceFile *sourceFile = isSgSourceFile(Rose::Frontend::Java::Ecj::Ecj_globalFilePointer);

    ROSE_ASSERT(astJavaScopeStack.top() != NULL);
    SgClassDefinition *class_definition = astJavaScopeStack.popClassDefinition(); // pop the class definition
    ROSE_ASSERT(class_definition);

    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());  // Pop the type parameters scope from the stack.
    ROSE_ASSERT(type_space);

    SgClassDeclaration *class_declaration = isSgClassDeclaration(class_definition -> get_declaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration != NULL);

/*
    ROSE_ASSERT(class_definition == astJavaComponentStack.top());
    astJavaComponentStack.pop(); // remove the class definition from the stack
*/

#if 1 //MH-20140404
	SgJavaClassDeclarationList *class_list = sourceFile -> get_class_list();
#if 0
    if (class_list == NULL) {
    	printf("class_list NULL\n");
    }
    else if (class_list -> get_file_info() == NULL) { // The first
//      setJavaSourcePosition(class_list, env, jToken);
    	printf("file info NULL\n");
    }
#endif
    class_list -> get_java_class_list().push_back(class_declaration);

    vector<SgClassDeclaration *> &type_list = class_list -> get_java_class_list();
    for (int i = 0; i < type_list.size(); i++) {
    	SgClassDeclaration *type_declaration = type_list[i];
		type_declaration->set_parent(class_list);
	}

#endif

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
                SgJavaClassDeclarationList *class_list = sourceFile -> get_class_list();
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
        astJavaScopeStack.top() -> get_file_info() -> display("source position in Java_x10rose_visit_JNI_cactionTypeDeclarationEnd(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_x10rose_visit_JNI_cactionTypeDeclarationEnd() (cactionTypeDeclarationEnd) \n");
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConstructorDeclaration(JNIEnv *env, jclass clz, jstring java_string, jint constructor_index, jobject jToken) 
{ 
	Java_JavaParser_cactionConstructorDeclaration(env, clz, java_string, constructor_index, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConstructorDeclarationHeader(JNIEnv *env, jclass clz,                                                                            jstring java_string, 
                                                                           jboolean java_is_public,
                                                                           jboolean java_is_protected,
                                                                           jboolean java_is_private,
                                                                           jint java_numberOfTypeParameters,
                                                                           jint java_numberOfArguments,
                                                                           jint java_numberOfThrownExceptions,
                                                                           jobject jToken) 
{

	Java_JavaParser_cactionConstructorDeclarationHeader(env, clz, java_string, java_is_public, java_is_protected, java_is_private, java_numberOfTypeParameters, java_numberOfArguments, java_numberOfThrownExceptions, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConstructorDeclarationEnd(JNIEnv *env, jclass clz, jint num_annotations, jint num_statements, jobject jToken) 
{ 
	Java_JavaParser_cactionConstructorDeclarationEnd(env, clz, num_annotations, num_statements, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionExplicitConstructorCall(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionExplicitConstructorCall(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionExplicitConstructorCallEnd(JNIEnv *env, jclass clz,                                                                          jboolean java_is_implicit_super,
                                                                         jboolean java_is_super,
                                                                         jboolean java_has_qualification,
                                                                         jstring java_package_name,
                                                                         jstring java_type_name,
                                                                         jint constructor_index,
                                                                         jint java_number_of_type_arguments,
                                                                         jint java_number_of_arguments,
                                                                         jobject jToken) 
{

	Java_JavaParser_cactionExplicitConstructorCallEnd(env, clz, java_is_implicit_super, java_is_super, java_has_qualification, java_package_name, java_type_name, constructor_index, java_number_of_type_arguments, java_number_of_arguments, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMethodDeclaration(JNIEnv *env, jclass clz, jstring java_string, jint method_index, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionMethodDeclaration(env, clz, java_string, method_index, jToken);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgMemberFunctionDeclaration \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
//MH-20140403
    ROSE_ASSERT(class_definition != NULL);
    ROSE_ASSERT(! class_definition -> attributeExists("namespace"));

    ROSE_ASSERT(class_definition != NULL  && (! class_definition -> attributeExists("namespace")));
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("type_parameter_space");
    ROSE_ASSERT(attribute);
    SgScopeStatement *type_space = isSgScopeStatement(attribute -> getNode(method_index));
    ROSE_ASSERT(type_space);
    attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("class_members");
    ROSE_ASSERT(attribute);
    SgFunctionDefinition *method_definition = isSgFunctionDefinition(attribute -> getNode(method_index));
    ROSE_ASSERT(method_definition);
    SgMemberFunctionDeclaration *method_declaration = isSgMemberFunctionDeclaration(method_definition -> get_declaration());
    ROSE_ASSERT(method_declaration);
    ROSE_ASSERT(method_declaration -> get_type());
    ROSE_ASSERT(method_declaration -> get_type() -> get_return_type());
    method_declaration -> setAttribute("type", new AstRegExAttribute(getTypeName(method_declaration -> get_type() -> get_return_type())));

//MH-20140403
    astJavaScopeStack.push(type_space);
    astJavaScopeStack.push(method_definition);
//MH-20140312
//temtatively push class_definition into astJavaScopeStack to pass assertion for checking if 
//stack top is class definition or not
#if 0 // MH-20140320 comment out again
    astJavaScopeStack.push(class_definition);
#endif

    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMethodDeclarationHeader(JNIEnv *env, jclass clz,                                                                       jstring java_string,
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
                                                                      jobject jToken) 
{

	Java_JavaParser_cactionMethodDeclarationHeader(env, clz, java_string, java_is_abstract, java_is_native, java_is_static, java_is_final, java_is_synchronized, java_is_public, java_is_protected, java_is_private, java_is_strictfp, java_numberOfTypeParameters, java_numberOfArguments, java_numberOfThrownExceptions, jToken);
}


/*
 */
#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMethodDeclarationEnd(JNIEnv *env, jclass clz, int num_annotations, int num_statements, jobject jToken) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMethodDeclarationEnd(JNIEnv *env, jclass, jint java_numberOfStatements, jobject jToken) 
#endif
{ 
#if 0
	Java_JavaParser_cactionMethodDeclarationEnd(env, clz, num_annotations, num_statements, jToken);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Entering  cactionMethodDeclarationEnd (method) \n");

    // Pop the constructor body...
    ROSE_ASSERT(! astJavaScopeStack.empty());


    int numberOfStatements = java_numberOfStatements;

    if (SgProject::get_verbose() > 0)
        printf ("In cactionMethodDeclarationEnd(): numberOfStatements = %d\n", numberOfStatements);


#if 0
    SgBasicBlock *method_body = astJavaScopeStack.popBasicBlock(); // pop the body block

//    for (int i = 0; i < numberOfStatements; i++) {
    for (int i = 0; i < 0; i++) {
         SgStatement *statement = astJavaComponentStack.popStatement();
         if (SgProject::get_verbose() > 2) {
             cerr << "(5) Adding statement "
                  << statement -> class_name()
                  << " to a method declaration block"
                  << endl;
             cerr.flush();
        }
        method_body -> prepend_statement(statement);
    }

    /* SgFunctionDefinition *memberFunctionDefinition = */
    astJavaScopeStack.popFunctionDefinition();

    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());
    ROSE_ASSERT(type_space);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting  cactionMethodDeclarationEnd (method) \n");
#else
//MH-20140317
// pop unnecessary node to have classdefinition as the top of astJavaScopeStack 
//    astJavaScopeStack.pop();
#if 0
#endif

//    astJavaScopeStack.pop();

/*
    for (int i = 0; i < numberOfStatements-1; i++) {
        astJavaScopeStack.pop();
    }
*/

/*
    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(class_definition != NULL  && (! class_definition -> attributeExists("namespace")));
*/

//    SgFunctionDefinition *memberFunctionDefinition = astJavaScopeStack.popFunctionDefinition();

    SgStatement *method_body = astJavaComponentStack.popStatement();
/*
    SgClassDefinition *class_definition = isSgClassDefinition(astJavaScopeStack.top());
    printf("class_definition=%p\n", class_definition);
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));
*/
//  SgStatement *function_declaration = astJavaComponentStack.popStatement();
    SgFunctionDefinition *memberFunctionDefinition = astJavaScopeStack.popFunctionDefinition();
//    SgFunctionDeclaration *memberFunctionDeclaration = astJavaComponentStack.popStatement();
//  SgFunctionDefinition *memberFunctionDefinition = memberFunctionDeclaration -> get_definition();
//  SgFunctionDefinition *memberFunctionDefinition = ((SgMemberFunctionDeclaration *)function_declaration) -> get_definition();
    memberFunctionDefinition -> set_body((SgBasicBlock *)method_body);
//  astJavaComponentStack.push(function_declaration);
//  astJavaScopeStack.push(((SgMemberFunctionDeclaration *)function_declaration)->get_class_scope());

    SgScopeStatement *type_space = isSgScopeStatement(astJavaScopeStack.pop());
    ROSE_ASSERT(type_space);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting  cactionMethodDeclarationEnd (method) \n");
#endif

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeParameterReference(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jint method_index, jstring java_type_parameter_name, jobject jToken) 
{ 
	Java_JavaParser_cactionTypeParameterReference(env, clz, java_package_name, java_type_name, method_index, java_type_parameter_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeReference(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionTypeReference(env, clz, java_package_name, java_type_name, jToken);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Inside cactionTypeReference\n");

    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

    SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);

// MH-20140226
// If type is null, a new type should be defined
#if 1
    if (type == NULL) {
//        cout << "PACKAGE_NAME=" << package_name << ", TYPE_NAME=" << type_name << endl;
        // cactionInsertClassStart2() does not have a parameter for specifying a package name, although
        // this is fine for x10 because x10 classes have been fully qualified with package  name
        Java_x10rose_visit_JNI_cactionInsertClassStart2(env, NULL, java_type_name, jToken);
    }
    else
#else
    ROSE_ASSERT(type != NULL);
#endif

// TODO: Remove this
//if (isSgClassType(type)) {
//SgClassType *class_type = isSgClassType(type);
//cout << "Came across type " << getTypeName(class_type) << endl;
//cout.flush();
//}

    astJavaComponentStack.push(type);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting cactionTypeReference\n");
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedTypeReference(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jobject jToken) 
{ 
	Java_JavaParser_cactionQualifiedTypeReference(env, clz, java_package_name, java_type_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCatchArgument(JNIEnv *env, jclass clz, jstring java_argument_name, jobject jToken) 
{ 
	Java_JavaParser_cactionCatchArgument(env, clz, java_argument_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArgument(JNIEnv *env, jclass clz, jstring java_argument_name, jobject jToken) 
{ 
	Java_JavaParser_cactionArgument(env, clz, java_argument_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCatchArgumentEnd(JNIEnv *env, jclass clz, jint num_annotations, jstring java_argument_name, jint num_types, jboolean java_is_final, jobject jToken) 
{ 
	Java_JavaParser_cactionCatchArgumentEnd(env, clz, num_annotations, java_argument_name, num_types, java_is_final, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArgumentEnd(JNIEnv *env, jclass clz, jint num_annotations, jstring java_argument_name, jobject jToken) 
{ 
	Java_JavaParser_cactionArgumentEnd(env, clz, num_annotations, java_argument_name, jToken);
}

/*
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayTypeReference(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jint java_number_of_dimensions, jobject jToken) 
{ 
	Java_JavaParser_cactionArrayTypeReference(env, clz, java_package_name, java_type_name, java_number_of_dimensions, jToken);
}
*/

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayTypeReference(JNIEnv *env, jclass clz, jint java_num_dimensions, jobject jToken) 
{ 
	Java_JavaParser_cactionArrayTypeReference(env, clz, java_num_dimensions, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayTypeReferenceEnd(JNIEnv *env, jclass clz, jstring java_name, int int1, jobject jToken) 
{ 
	Java_JavaParser_cactionArrayTypeReferenceEnd(env, clz, java_name, int1, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMessageSend(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jstring java_function_name, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionMessageSend(env, clz, java_package_name, java_type_name, java_function_name, jToken);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build a function call (Message Send) \n");

    // Do Nothing on the way down !!!
#endif
}


/*
 * Java_x10rose_visit_JNI_cactionMessageSendEnd(JNIEnv *env, jclass,
                                                             jboolean java_is_static,
                                                             jboolean java_has_receiver,
                                                             jstring java_function_name,
                                                             jint java_number_of_parameters,
                                                             jint numTypeArguments,
                                                             jint numArguments,
                                                             jobject jToken) {
 *
 */
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMessageSendEnd(JNIEnv *env, jclass clz,
                                                             jboolean java_is_static,
                                                             jboolean java_has_receiver,
                                                             jstring java_function_name,
                                                             jint java_number_of_parameters,
                                                             jint numTypeArguments,
                                                             jint numArguments,
                                                             jobject jToken) 
{
#if 0
	Java_JavaParser_cactionMessageSendEnd(env, clz, java_is_static, java_has_receiver, java_package_name, java_type_name, java_method_name, method_index, numTypeArguments, numArguments, jToken);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionMessageSendEnd() \n");

    bool is_static = java_is_static,
         has_receiver = java_has_receiver;

    SgName function_name  = convertJavaStringToCxxString(env, java_function_name);
    int num_parameters = java_number_of_parameters;

    //
    // TODO: Since array types are not properly represented as class types but as (C++) pointer types,
    // when an array type is used to call a function, we have to substitute the Object class for the
    // array type in question as the Object type is its (only) super type and contains all the method
    // that is invokable from an array.
    //
    SgType *containing_type = astJavaComponentStack.popType();
    if (isSgPointerType(containing_type)) { // is this type an array type?
        containing_type = ::ObjectClassType;
    }
    else if (isSgJavaWildcardType(containing_type)) { // is this type a wildcard type?
        SgJavaWildcardType *wildcard_type = isSgJavaWildcardType(containing_type);
        if (wildcard_type -> get_is_unbound()) {
            containing_type = ::ObjectClassType;
        }
        else if (wildcard_type -> get_has_extends()) {
            containing_type = wildcard_type -> get_bound_type();
        }
        else {
            ROSE_ASSERT(false && "! yet support wildcard with super bound");
        }
    }
    SgClassDeclaration *class_declaration = isSgClassDeclaration(containing_type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *targetClassScope = class_declaration -> get_definition();
    ROSE_ASSERT(targetClassScope != NULL && (! targetClassScope -> attributeExists("namespace")));
// TODO: Remove this !!!
/*
    SgType *return_type = astJavaComponentStack.popType(); // The return type of the function
    ROSE_ASSERT(return_type);
*/

    //
    // The astJavaComponentStack has all of the types of the parameters of the function being called. Note that
    // it is necessary to use the original types of the formal parameters of the function in order to find the
    // perfect match for the function.
    //
    list<SgType *> function_parameter_types;
    for (int i = 0; i < num_parameters; i++) { // reverse the arguments' order
        SgType *type = astJavaComponentStack.popType();
        function_parameter_types.push_front(type);
    }

// TODO: Remove this !!!
/*
cout << "Looking for function "
<< function_name
<< " in type "
<< targetClassScope -> get_qualified_name()
<< "("
<< targetClassScope
<< ")"
<< " with parameter types: (";
std::list<SgType*>::iterator i = function_parameter_types.begin();
if (i != function_parameter_types.end()) {
cout << getTypeName(*i);
for (i++; i != function_parameter_types.end(); i++) {
cout << ", " << getTypeName(*i);
}
}
cout << ")"
<< endl;
cout.flush();
*/

    SgMemberFunctionSymbol *function_symbol = findFunctionSymbolInClass(targetClassScope, function_name, function_parameter_types);
    ROSE_ASSERT(function_symbol);
    SgMemberFunctionType *function_type = isSgMemberFunctionType(function_symbol -> get_type());
    ROSE_ASSERT(function_type);

// TODO: Remove this !!!
/*
    //
    // If we have an accurate return type for this function, set it !!!
    // This occurs when the method being invoked belongs to a parameterized type.
    // We need this temporary "zapping" so that the result of this expression will have
    // the correct type in case it is used for further dereferencing.
    //
    if (function_type -> get_return_type() != return_type) {
        function_type -> set_return_type(return_type);
    }
*/
    // The astJavaComponentStack has all of the arguments to the function call.
    SgExprListExp *arguments = new SgExprListExp();
    for (int i = 0; i < numArguments; i++) { // reverse the arguments' order
        SgExpression *expr = astJavaComponentStack.popExpression();
        arguments -> prepend_expression(expr);
    }
    setJavaSourcePosition(arguments, env, jToken);

    SgFunctionCallExp *function_call_exp = SageBuilder::buildFunctionCallExp(function_symbol, arguments);
    setJavaSourcePosition(function_call_exp, env, jToken);
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
        function_call_exp -> setAttribute("invocation_parameter_types", attribute);
        function_call_exp -> setAttribute("function_parameter_types", new AstRegExAttribute(parm_names));
    }

    SgExpression *exprForFunction = function_call_exp;

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
            if (isSgClassType(receiver)) { // Note that if this is true then the function must be static... See unparseJava_expression.C: unparseFucnCall
                SgClassType *type = isSgClassType(receiver);
// TODO: Remove this !
/*
                string type_name = isSgNamedType(receiver) -> get_name();
                string full_name = getFullyQualifiedTypeName(type);

                string class_name = (full_name.size() == type_name.size() ? type_name : full_name.substr(0, full_name.size() - type_name.size()) + type_name);
*/
                string class_name = getTypeName(type);
                exprForFunction -> setAttribute("prefix", new AstRegExAttribute(class_name));
            }
            else { // this can't happen!?
                // TODO: What if the class is a parameterized type?
                ROSE_ASSERT(false);
            }
        }
        else if (is_static && isSgThisExp(receiver) && (! receiver -> attributeExists("class"))) { // A sgThisExp receiver in front of a static function?
            delete receiver; // Ignore the receiver!
        }
        else {
            exprForFunction = SageBuilder::buildBinaryExpression<SgDotExp>((SgExpression *) receiver, exprForFunction);
            setJavaSourcePosition(exprForFunction, env, jToken);

            SgClassDefinition *current_class_definition = getCurrentTypeDefinition();
            SgType *enclosing_type = current_class_definition -> get_declaration() -> get_type();
            if (isSgThisExp(receiver) && (! receiver -> attributeExists("class")) && (! receiver -> attributeExists("prefix")) && (! isCompatibleTypes(containing_type, enclosing_type))) {
                SgClassType *c_type = isSgClassType(containing_type);
                SgJavaParameterizedType *p_type = isSgJavaParameterizedType(containing_type);
                string prefix_name = (c_type ? getTypeName(c_type) // getFullyQualifiedTypeName(c_type)
                                             : p_type ? getTypeName(p_type) // getFullyQualifiedTypeName(p_type)
                                                      : "");
                ROSE_ASSERT(prefix_name.size() != 0);
                receiver -> setAttribute("prefix", new AstRegExAttribute(prefix_name));
            }
        }
    }

    astJavaComponentStack.push(exprForFunction);

    if (SgProject::get_verbose() > 2)
        printf ("Leaving Java_JavaParser_cactionMessageSendEnd(): %s\n", function_name.getString().c_str());
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionStringLiteral(JNIEnv *env, jclass clz, jstring java_string, jobject jToken) 
{ 
	Java_JavaParser_cactionStringLiteral(env, clz, java_string, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAllocationExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionAllocationExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAllocationExpressionEnd(JNIEnv *env, jclass clz, jboolean has_type, jint java_num_arguments, jobject jToken) 
{ 
	Java_JavaParser_cactionAllocationExpressionEnd(env, clz, has_type, java_num_arguments, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionANDANDExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionANDANDExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionANDANDExpressionEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionANDANDExpressionEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAnnotationMethodDeclaration(JNIEnv *env, jclass clz, jstring java_string, jint method_index, jobject jToken) 
{ 
	Java_JavaParser_cactionAnnotationMethodDeclaration(env, clz, java_string, method_index, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAnnotationMethodDeclarationEnd(JNIEnv *env, jclass clz, jstring java_string, jint method_index, jint num_annotations, jboolean has_default, jobject jToken) 
{ 
	Java_JavaParser_cactionAnnotationMethodDeclarationEnd(env, clz, java_string, method_index, num_annotations, has_default, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayAllocationExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionArrayAllocationExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayAllocationExpressionEnd(JNIEnv *env, jclass clz, jint java_num_dimensions, jboolean java_has_initializers, jobject jToken) 
{ 
	Java_JavaParser_cactionArrayAllocationExpressionEnd(env, clz, java_num_dimensions, java_has_initializers, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayInitializer(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionArrayInitializer(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayInitializerEnd(JNIEnv *env, jclass clz, jint java_num_expressions, jobject jToken) 
{ 
	Java_JavaParser_cactionArrayInitializerEnd(env, clz, java_num_expressions, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionArrayReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayReferenceEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionArrayReferenceEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAssertStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionAssertStatement(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAssertStatementEnd(JNIEnv *env, jclass clz, jboolean hasExceptionArgument, jobject jToken) 
{ 
	Java_JavaParser_cactionAssertStatementEnd(env, clz, hasExceptionArgument, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAssignment(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionAssignment(env, clz, jToken);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Build an assignement statement (expression?) \n");

    // Nothing to do !!!
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAssignmentEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionAssignmentEnd(env, clz, jToken);
#else
    // This function builds an assignement statement (not an expression).
    if (SgProject::get_verbose() > 2)
        printf ("Build an assignement statement (expression?) \n");

    binaryExpressionSupport<SgAssignOp>();

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBinaryExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionBinaryExpression(env, clz, jToken);
#else
	    // I don't think we need this function.
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBinaryExpressionEnd(JNIEnv *env, jclass clz, jint java_operator_kind, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionBinaryExpressionEnd(env, clz, java_operator_kind, jToken);
#else
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
        case OR_OR:   ROSE_ASSERT(false); break;
        case AND_AND: ROSE_ASSERT(false); break;

        default:
            printf ("Error: default reached in cactionBinaryExpressionEnd() operator_kind = %d \n", operator_kind);
            ROSE_ASSERT(false);
    }

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBlock(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionBlock(env, clz, jToken);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Build an SgBasicBlock scope \n");

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
            // block = ifStatement -> get_true_body();
            block = SageBuilder::buildBasicBlock();
            ROSE_ASSERT(block != NULL);
            ifStatement -> set_true_body(block);

            delete nullStatement;
        }
        else {
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

    setJavaSourcePosition(block, env, jToken);

    block -> set_parent(astJavaScopeStack.top());
    ROSE_ASSERT(block -> get_parent() != NULL);

    astJavaScopeStack.push(block);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBlockEnd(JNIEnv *env, jclass clz, jint java_numberOfStatements, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionBlockEnd(env, clz, java_numberOfStatements, jToken);
#else
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
            cerr << "(7) Adding statement "
                 << statement -> class_name()
                 << " to a block"
                 << endl;
            cerr.flush();
        }
        body -> prepend_statement(statement);
    }


    astJavaComponentStack.push(body);
//MH-20140317 added 
//MH-20140320 comment out again
//    astJavaScopeStack.push(body);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBreakStatement(JNIEnv *env, jclass clz, jstring java_string, jobject jToken) 
{ 
	Java_JavaParser_cactionBreakStatement(env, clz, java_string, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCaseStatement(JNIEnv *env, jclass clz, jboolean hasCaseExpression, jobject jToken) 
{ 
	Java_JavaParser_cactionCaseStatement(env, clz, hasCaseExpression, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCaseStatementEnd(JNIEnv *env, jclass clz, jboolean hasCaseExpression, jobject jToken) 
{ 
	Java_JavaParser_cactionCaseStatementEnd(env, clz, hasCaseExpression, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCastExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionCastExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCastExpressionEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionCastExpressionEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCharLiteral(JNIEnv *env, jclass clz, jchar java_char_value, jobject jToken) 
{ 
	Java_JavaParser_cactionCharLiteral(env, clz, java_char_value, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionClassLiteralAccess(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionClassLiteralAccess(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionClassLiteralAccessEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionClassLiteralAccessEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionClinit(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionClinit(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConditionalExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionConditionalExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConditionalExpressionEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionConditionalExpressionEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionContinueStatement(JNIEnv *env, jclass clz, jstring java_string, jobject jToken) 
{ 
	Java_JavaParser_cactionContinueStatement(env, clz, java_string, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompoundAssignment(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionCompoundAssignment(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompoundAssignmentEnd(JNIEnv *env, jclass clz, jint java_operator_kind, jobject jToken) 
{ 
	Java_JavaParser_cactionCompoundAssignmentEnd(env, clz, java_operator_kind, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionDoStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionDoStatement(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionDoStatementEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionDoStatementEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionDoubleLiteral(JNIEnv *env, jclass clz, jdouble java_value, jstring java_source, jobject jToken) 
{ 
	Java_JavaParser_cactionDoubleLiteral(env, clz, java_value, java_source, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEmptyStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionEmptyStatement(env, clz, jToken);
#else
    // Nothing to do;
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEmptyStatementEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionEmptyStatementEnd(env, clz, jToken);
#else
    SgNullStatement *stmt = SageBuilder::buildNullStatement();
    ROSE_ASSERT(stmt != NULL);
    setJavaSourcePosition(stmt, env, jToken);
    astJavaComponentStack.push(stmt);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEqualExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionEqualExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEqualExpressionEnd(JNIEnv *env, jclass clz, jint java_operator_kind, jobject jToken) 
{ 
	Java_JavaParser_cactionEqualExpressionEnd(env, clz, java_operator_kind, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionExtendedStringLiteral(JNIEnv *env, jclass clz, jstring java_string, jobject jToken) 
{ 
	Java_JavaParser_cactionExtendedStringLiteral(env, clz, java_string, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFalseLiteral(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionFalseLiteral(env, clz, jToken);
#else
    SgExpression *expression = SageBuilder::buildBoolValExp(false);
    astJavaComponentStack.push(expression);
#endif
}

#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFieldDeclarationEnd(JNIEnv *env, jclass clz,                                                                   jstring variable_name,
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
                                                                  jobject jToken) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFieldDeclarationEnd(JNIEnv *env, jclass,
                                                                  jstring variableName,
                                                                  jboolean is_enum_field,
                                                                  jboolean has_initializer,
                                                                  jboolean java_is_final,
                                                                  jboolean java_is_private,
                                                                  jboolean java_is_protected,
                                                                  jboolean java_is_public,
                                                                  jboolean java_is_volatile,
                                                                  jboolean java_is_synthetic,
                                                                  jboolean java_is_static,
                                                                  jboolean java_is_transient,
                                                                  jobject jToken) 
#endif
{
#if 0
	Java_JavaParser_cactionFieldDeclarationEnd(env, clz, variable_name, num_annotations, is_enum_constant, has_initializer, is_final, is_private, is_protected, is_public, is_volatile, is_synthetic, is_static, is_transient, jToken);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldDeclarationEnd() \n");

    SgName name = convertJavaStringToCxxString(env, variableName);

    // if (SgProject::get_verbose() > 2)
    //      printf ("hasInitializer = %s (but not used except in bottom up processing) \n", hasInitializer ? "true" : "false");

    bool isFinal     = java_is_final;
    bool isPrivate   = java_is_private;
    bool isProtected = java_is_protected;
    bool isPublic    = java_is_public;
    bool isVolatile  = java_is_volatile;
    bool isSynthetic = java_is_synthetic;
    bool isStatic    = java_is_static;
    bool isTransient = java_is_transient;

    if (SgProject::get_verbose() > 2)
        printf ("Building a Field declaration for name = %s \n", name.str());

    SgExpression *initializer_expression = (((! is_enum_field) && has_initializer) ? astJavaComponentStack.popExpression() : NULL);

    SgScopeStatement *outer_scope = astJavaScopeStack.top();
    ROSE_ASSERT(outer_scope);
    SgVariableSymbol *symbol = outer_scope -> lookup_variable_symbol(name);
    ROSE_ASSERT(symbol);
    SgInitializedName *initializedName = symbol -> get_declaration();
    ROSE_ASSERT(initializedName);
    SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(initializedName -> get_declaration());
    ROSE_ASSERT(variableDeclaration);

    // By default, the access modifier is set to unknown
    variableDeclaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    if (! is_enum_field) { // if this is not an ENUM field then it has a type on the stack.
#if 0
        SgType *type = astJavaComponentStack.popType();
        ROSE_ASSERT(type);
#endif
        initializedName -> setAttribute("type",
#if 0
                                new AstRegExAttribute(getTypeName(type))
#else
                                new AstRegExAttribute(getTypeName(initializedName -> get_type()))
#endif
                            );
    }
    // Set the modifiers (shared between PHP and Java)
    if (isFinal) {
        variableDeclaration -> get_declarationModifier().setFinal();
    }

    // DQ (8/13/2011): Added modifier support.
    if (isPrivate) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Private \n");
        variableDeclaration -> get_declarationModifier().get_accessModifier().setPrivate();
    }

    if (isProtected) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Protected \n");
        variableDeclaration -> get_declarationModifier().get_accessModifier().setProtected();
    }

    if (isPublic) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Public \n");
        variableDeclaration -> get_declarationModifier().get_accessModifier().setPublic();
    }

    if (isVolatile) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Volatile \n");
        variableDeclaration -> get_declarationModifier().get_typeModifier().get_constVolatileModifier().setVolatile();
    }
    if (isSynthetic) {
        // Synthetic is not a keyword, not clear if we want to record this explicitly.
        printf ("Specification of isSynthetic is not supported in the IR (should it be?) \n");
    }

    if (isStatic) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Static \n");
        variableDeclaration -> get_declarationModifier().get_storageModifier().setStatic();
    }

    if (isTransient) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Transient \n");
        variableDeclaration -> get_declarationModifier().get_typeModifier().get_constVolatileModifier().setJavaTransient();
    }

    if (SgProject::get_verbose() > 0)
        variableDeclaration -> get_file_info() -> display("source position in Java_JavaParser_cactionFieldDeclarationEnd(): debug");

    // DQ (9/5/2011): Added from previous Java_JavaParser_cactionFieldDeclarationEnd() function.
    if (has_initializer) {
        SgInitializer *initializer = SageBuilder::buildAssignInitializer(initializer_expression);
        ROSE_ASSERT(initializer != NULL);

//        setJavaSourcePosition(initializer_expression, env, jToken);
//        setJavaSourcePosition(initializer, env, jToken);

        initializer_expression -> set_parent(initializer);

        // printf ("In cactionFieldDeclarationEnd(): initializer = %p = %s \n", initializer, initializer -> class_name().c_str());
        initializer -> get_file_info() -> display("cactionFieldDeclarationEnd()");

        initializedName -> set_initptr(initializer);
        initializer -> set_parent(initializedName);
    }

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFieldReference(JNIEnv *env, jclass clz, jstring java_field, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionFieldReference(env, clz, java_field, jToken);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldReference() \n");
    // Nothing to do !!!
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFieldReferenceEnd(JNIEnv *env, jclass clz, jboolean explicit_type, jstring java_field, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionFieldReferenceEnd(env, clz, explicit_type, java_field, jToken);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldReference() \n");

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
/*
*/

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
    if (isSgPointerType(receiver_type) && field_name.getString().compare("length") == 0) { // In fact, this is a Java array which is a type !!!
        receiver -> setAttribute("suffix", new AstRegExAttribute("length")); // TODO: The field "length" does not exist since we don't have a real type!
        result = receiver;
    }
    else {
        //
        // TODO: (PC) This is very sloppy and imprecise because we do not yet have an array type !!!
        //
        if (isSgPointerType(receiver_type)) { // In fact, this is a Java array which is a type!!!
            SgType *base_type = isSgPointerType(receiver_type) -> get_base_type();
            if (! isSgClassType(base_type)) {
                receiver_type = ::ObjectClassType;
            }
            else receiver_type = base_type;
        }

        //
        // TODO: Parameterized types should be class types... Need to review this!
        //
        if (isSgJavaParameterizedType(receiver_type)) {
            receiver_type = isSgJavaParameterizedType(receiver_type) -> get_raw_type();
        }

        SgClassType *class_type = isSgClassType(receiver_type);
        ROSE_ASSERT(class_type);
        SgClassDeclaration *declaration = isSgClassDeclaration(class_type -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(declaration);
        ROSE_ASSERT(declaration -> get_definition());
        SgVariableSymbol *variable_symbol = lookupSimpleNameVariableInClass(field_name, declaration -> get_definition());
// TODO: Remove this !
/*
if (! variable_symbol) {
  cout << "Could not find variable " << field_name.getString()
       << " in type " << getTypeName(class_type)
       << endl;
  cout.flush();
}
else {
  cout << "Found variable " << field_name.getString()
       << " in type " << getTypeName(class_type)
       << endl;
  cout.flush();
}
*/
        ROSE_ASSERT(variable_symbol);
        SgVarRefExp *field = SageBuilder::buildVarRefExp(variable_symbol);
        ROSE_ASSERT(field != NULL);
//        setJavaSourcePosition(field, env, jToken);

        if (receiver) {
            result = SageBuilder::buildBinaryExpression<SgDotExp>(receiver, field);
// TODO: Remove this !
/*
  cout << "Emitted a SgDotExp"
       << endl;
  cout.flush();
*/
        }
        else {
            string class_name = getTypeName(class_type); // getFullyQualifiedTypeName(class_type);

            field -> setAttribute("prefix", new AstRegExAttribute(class_name));
            result = field;
// TODO: Remove this !
/*
  cout << "Decorating a field name"
       << endl;
  cout.flush();
*/
        }
    }

    astJavaComponentStack.push(result);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFloatLiteral(JNIEnv *env, jclass clz, jfloat java_value, jstring java_source, jobject jToken) 
{ 
	Java_JavaParser_cactionFloatLiteral(env, clz, java_value, java_source, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionForeachStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionForeachStatement(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionForeachStatementEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionForeachStatementEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionForStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionForStatement(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionForStatementEnd(JNIEnv *env, jclass clz, jint num_initializations, jboolean has_condition, jint num_increments, jobject jToken) 
{ 
	Java_JavaParser_cactionForStatementEnd(env, clz, num_initializations, has_condition, num_increments, jToken);
}

#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionIfStatement(JNIEnv *env, jclass clz, jboolean has_false_body, jobject jToken) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionIfStatement(JNIEnv *env, jclass, jobject jToken) 
#endif
{ 
#if 0
	Java_JavaParser_cactionIfStatement(env, clz, has_false_body, jToken);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionIfStatement() \n");

    // Build a SgIfStatement and push it onto the stack with a true block.

    // We need a predicate to use to call the SageBuilder::buildIfStmt() function.  So build a SgNullExpression for now. 
    SgNullStatement *temp_conditional = SageBuilder::buildNullStatement();
    SgNullStatement *true_block = SageBuilder::buildNullStatement();
    ROSE_ASSERT(true_block != NULL);

    SgIfStmt *ifStatement = SageBuilder::buildIfStmt(temp_conditional, true_block, NULL);
    ROSE_ASSERT(ifStatement != NULL);

    ifStatement -> set_parent(astJavaScopeStack.top());

    setJavaSourcePosition(ifStatement, env, jToken);

    // Push the SgIfStmt onto the stack, but not the true block.
    astJavaScopeStack.push(ifStatement);
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionIfStatementEnd(JNIEnv *env, jclass clz, jboolean has_false_body, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionIfStatementEnd(env, clz, has_false_body, jToken);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionIfStatementEnd() \n");

    // There should be a predicate on the stack for us to use as a final step in construction of the SgIfStmt.

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgIfStmt *ifStatement = astJavaScopeStack.popIfStmt();
    ROSE_ASSERT(ifStatement -> get_parent() != NULL);

    setJavaSourcePosition(ifStatement, env, jToken);

    // If there are two required then the first is for the false branch.
    if (has_false_body) {
        SgStatement *false_body = astJavaComponentStack.popStatement();
        ifStatement -> set_false_body(false_body);
        false_body -> set_parent(ifStatement);
        ROSE_ASSERT(false_body -> get_parent() != NULL);
    }

    SgStatement *true_body = astJavaComponentStack.popStatement();
    ifStatement -> set_true_body(true_body);
    true_body -> set_parent(ifStatement);
    ROSE_ASSERT(true_body -> get_parent() != NULL);

    SgExpression *condititonalExpr = astJavaComponentStack.popExpression();

    SgExprStatement *exprStatement = SageBuilder::buildExprStatement(condititonalExpr);

    setJavaSourcePosition(exprStatement, env, jToken);

    ROSE_ASSERT(exprStatement != NULL);
    ROSE_ASSERT(condititonalExpr -> get_parent() != NULL);

    ifStatement -> set_conditional(exprStatement);

    ROSE_ASSERT(exprStatement -> get_parent() == NULL);
    exprStatement -> set_parent(ifStatement);
    ROSE_ASSERT(exprStatement -> get_parent() != NULL);

    // DQ (7/30/2011): Take the block off of the scope stack and put it onto the statement stack so that we can 
    // process either blocks of other statements uniformally.
    astJavaComponentStack.push(ifStatement);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionImportReference(JNIEnv *env, jclass clz,                                                               jboolean java_is_static,
                                                              jstring java_qualified_name,
                                                              jboolean java_contains_wildcard,
                                                              jobject jToken) 
{
#if 1
	Java_JavaParser_cactionImportReference(env, clz, java_is_static, java_qualified_name, java_contains_wildcard, jToken);
#else
    // This is the import statement.  The semantics is to include the named file and add its
    // declarations to the global scope so that they can be referenced by the current file.
    // The import directive tells the compiler where to look for the class definitions
    // when it comes upon a class that it cannot find in the default java.lang package.

    if (SgProject::get_verbose() > 1)
        printf ("Inside of Java_x10rose_visit_JNI_cactionImportReference() \n");

    bool is_static = java_is_static;

    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name),
           name_suffix = convertJavaStringToCxxString(env, java_name_suffix),
           qualifiedName = convertJavaStringToCxxString(env, java_package_name) +
                           string(package_name.getString().size() && type_name.getString().size() ? "." : "") +
                           type_name.getString() +
                           (name_suffix.getString().size() ? ("." + name_suffix.getString()) : "");

    bool contains_wildcard = java_contains_wildcard;

    // I could not debug passing a Java "Boolean" variable, but "int" works fine.
    // containsWildcard = convertJavaBooleanToCxxBoolean(env, input_containsWildcard);
    // containsWildcard = (bool) (env -> CallBooleanMethod(xxx, input_containsWildcard) == 1);
    // containsWildcard = (bool) input_containsWildcard;
    // containsWildcard = (bool) (env -> CallStaticBooleanMethod(xxx, java_containsWildcard) == 1);
    // containsWildcard = (java_containsWildcard == 1);

    // printf ("import qualifiedName = %s containsWildcard = %s \n", qualifiedName.str(), containsWildcard ? "true" : "false");

    SgJavaImportStatement *importStatement = new SgJavaImportStatement(qualifiedName, contains_wildcard);
    ROSE_ASSERT(importStatement != NULL);
    importStatement -> set_firstNondefiningDeclaration(importStatement);
    importStatement -> set_definingDeclaration(importStatement);
    ROSE_ASSERT(importStatement == importStatement ->  get_firstNondefiningDeclaration());
    ROSE_ASSERT(importStatement == importStatement ->  get_definingDeclaration());
    importStatement -> set_parent(astJavaScopeStack.top()); // We also have to set the parent so that the stack debugging output will work.
    setJavaSourcePosition(importStatement, env, jToken);

    if (is_static) {
        importStatement -> get_declarationModifier().get_storageModifier().setStatic();
    }

    ROSE_ASSERT(! astJavaScopeStack.empty());

    // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
    // printf ("Previously calling appendStatement in cactionImportReference() \n");
    // appendStatement(importStatement);
    astJavaComponentStack.push(importStatement);
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

    if (SgProject::get_verbose() > 1)
        printf ("Leaving Java_x10rose_visit_JNI_cactionImportReference() \n");

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInitializer(JNIEnv *env, jclass clz, jboolean java_is_static, jstring java_string, jint initializer_index, jobject jToken) 
{ 
	Java_JavaParser_cactionInitializer(env, clz, java_is_static, java_string, initializer_index, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInitializerEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionInitializerEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInstanceOfExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionInstanceOfExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInstanceOfExpressionEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionInstanceOfExpressionEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionIntLiteral(JNIEnv *env, jclass clz, jint java_value, jstring java_source, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionIntLiteral(env, clz, java_value, java_source, jToken);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build IntVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    int value = java_value;
    SgName source = convertJavaStringToCxxString(env, java_source);

    // printf ("Building an integer value expression = %d = %s \n", value, valueString.c_str());

    SgIntVal *integerValue = new SgIntVal(value, source);
    ROSE_ASSERT(integerValue != NULL);

    setJavaSourcePosition(integerValue, env, jToken);

    astJavaComponentStack.push(integerValue);
#endif
}

/*
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadoc(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadoc(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocAllocationExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocAllocationExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocAllocationExpressionClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocAllocationExpressionClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArgumentExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocArgumentExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArgumentExpressionClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocArgumentExpressionClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArrayQualifiedTypeReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocArrayQualifiedTypeReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocArrayQualifiedTypeReferenceClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArraySingleTypeReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocArraySingleTypeReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArraySingleTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocArraySingleTypeReferenceClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocFieldReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocFieldReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocFieldReferenceClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocFieldReferenceClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocImplicitTypeReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocImplicitTypeReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocImplicitTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocImplicitTypeReferenceClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocMessageSend(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocMessageSend(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocMessageSendClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocMessageSendClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocQualifiedTypeReference(JNIEnv *env, jclass clz, jobject jToken)  
{ 
	Java_JavaParser_cactionJavadocQualifiedTypeReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocQualifiedTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocQualifiedTypeReferenceClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocReturnStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocReturnStatement(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocReturnStatementClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocReturnStatementClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocSingleNameReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocSingleNameReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocSingleNameReferenceClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocSingleNameReferenceClassScope(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocSingleTypeReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocSingleTypeReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocSingleTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionJavadocSingleTypeReferenceClassScope(env, clz, jToken);
}
*/

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLabeledStatement(JNIEnv *env, jclass clz, jstring labelName, jobject jToken) 
{ 
	Java_JavaParser_cactionLabeledStatement(env, clz, labelName, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLabeledStatementEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionLabeledStatementEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLocalDeclaration(JNIEnv *env, jclass clz, jint num_annotations, jstring java_variable_name, jboolean is_final, jobject jToken) 
{ 
#if 1
	Java_JavaParser_cactionLocalDeclaration(env, clz, num_annotations, java_variable_name, is_final, jToken);
#else
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
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLocalDeclarationEnd(JNIEnv *env, jclass clz, jstring variable_name, jboolean hasInitializer, jobject jToken) 
{ 
#if 1
	Java_JavaParser_cactionLocalDeclarationEnd(env, clz, variable_name, hasInitializer, jToken);
#else

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
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLongLiteral(JNIEnv *env, jclass clz, jlong java_value, jstring java_source, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionLongLiteral(env, clz, java_value, java_source, jToken);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build LongVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    long value = java_value;
    SgName source = convertJavaStringToCxxString(env, java_source);

//   printf ("Building an integer value expression = %d = %s \n", value, valueString.c_str());

    SgLongIntVal *longValue = new SgLongIntVal(value, source);
    ROSE_ASSERT(longValue != NULL);

    setJavaSourcePosition(longValue, env, jToken);
// MH-20140407 confirmed that value is properly set to longValue, although DOT file does not have information about value...
//printf("longValue.get_value()=%d \n", longValue->get_value());
    astJavaComponentStack.push(longValue);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMarkerAnnotationEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionMarkerAnnotationEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMemberValuePairEnd(JNIEnv *env, jclass clz, jstring java_name, jobject jToken) 
{ 
	Java_JavaParser_cactionMemberValuePairEnd(env, clz, java_name, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionStringLiteralConcatenation(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionStringLiteralConcatenation(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionNormalAnnotationEnd(JNIEnv *env, jclass clz, jint num_member_value_pairs, jobject jToken) 
{ 
	Java_JavaParser_cactionNormalAnnotationEnd(env, clz, num_member_value_pairs, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionNullLiteral(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionNullLiteral(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionORORExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionORORExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionORORExpressionEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionORORExpressionEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionParameterizedTypeReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionParameterizedTypeReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionParameterizedTypeReferenceEnd(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jboolean has_type_arguments, int java_num_type_arguments, jobject jToken) 
{ 
	Java_JavaParser_cactionParameterizedTypeReferenceEnd(env, clz, java_package_name, java_type_name, has_type_arguments, java_num_type_arguments, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionParameterizedQualifiedTypeReferenceEnd(JNIEnv *env, jclass clz, jstring java_type_name, jboolean has_type_arguments, int java_num_type_arguments, jobject jToken) 
{ 
	Java_JavaParser_cactionParameterizedQualifiedTypeReferenceEnd(env, clz, java_type_name, has_type_arguments, java_num_type_arguments, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPostfixExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionPostfixExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPostfixExpressionEnd(JNIEnv *env, jclass clz, jint java_operator_kind, jobject jToken) 
{ 
	Java_JavaParser_cactionPostfixExpressionEnd(env, clz, java_operator_kind, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPrefixExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionPrefixExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPrefixExpressionEnd(JNIEnv *env, jclass clz, jint java_operator_kind, jobject jToken) 
{ 
	Java_JavaParser_cactionPrefixExpressionEnd(env, clz, java_operator_kind, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedAllocationExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionQualifiedAllocationExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedAllocationExpressionEnd(JNIEnv *env, jclass clz, jboolean has_type, jboolean java_contains_enclosing_instance, jint java_num_arguments, jboolean java_is_anonymous, jobject jToken) 
{ 
	Java_JavaParser_cactionQualifiedAllocationExpressionEnd(env, clz, has_type, java_contains_enclosing_instance, java_num_arguments, java_is_anonymous, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedSuperReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionQualifiedSuperReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedSuperReferenceEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionQualifiedSuperReferenceEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedThisReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionQualifiedThisReference(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedThisReferenceEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionQualifiedThisReferenceEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionReturnStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionReturnStatement(env, clz, jToken);
#else
    // Nothing to do !!!
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionReturnStatementEnd(JNIEnv *env, jclass clz, jboolean has_expression, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionReturnStatementEnd(env, clz, has_expression, jToken);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionReturnStatementEnd() \n");

    // Build the Return Statement
    SgExpression *expression = (has_expression ? astJavaComponentStack.popExpression() : NULL);
    SgReturnStmt *returnStatement = SageBuilder::buildReturnStmt_nfi(expression);
    ROSE_ASSERT(has_expression || returnStatement -> get_expression() == NULL); // TODO: there is an issue with the implementation of buildReturnStmt()...
    setJavaSourcePosition(returnStatement, env, jToken);

    // Pushing 'return' on the statement stack
    astJavaComponentStack.push(returnStatement);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSingleMemberAnnotationEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionSingleMemberAnnotationEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSingleNameReference(JNIEnv *env, jclass clz, jstring java_package_name, jstring java_type_name, jstring java_name, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionSingleNameReference(env, clz, java_package_name, java_type_name, java_name, jToken);
#else
    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name),
           name = convertJavaStringToCxxString(env, java_name);
    SgVariableSymbol *variable_symbol = NULL;
    if (! type_name.getString().empty()) { // an instance variable?
        if (SgProject::get_verbose() > 0)
            printf ("Building a Single Name reference for name = %s%s%s \n", (package_name.getString().empty() ? "" : (package_name.getString() + ".")).c_str(), (type_name.getString() + ".").c_str(), name.str());

        SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
        ROSE_ASSERT(type);
        SgClassType *class_type = isSgClassType(type);
        ROSE_ASSERT(class_type);
        SgClassDeclaration *declaration = isSgClassDeclaration(class_type -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(declaration);
        ROSE_ASSERT(declaration -> get_definition());
        variable_symbol = lookupSimpleNameVariableInClass(name, declaration -> get_definition());
    }
    else { // a local variable!
        if (SgProject::get_verbose() > 0)
            printf ("Building a Single Name reference for name = %s \n", name.str());
        variable_symbol = lookupVariableByName(name);
    }

    ROSE_ASSERT(variable_symbol);
    SgVarRefExp *varRefExp = SageBuilder::buildVarRefExp(variable_symbol);
    ROSE_ASSERT(varRefExp != NULL);
    if (SgProject::get_verbose() > 0)
        printf ("In cactionSingleNameReference(): varRefExp = %p type = %p = %s \n", varRefExp, varRefExp -> get_type(), varRefExp -> get_type() -> class_name().c_str());

    setJavaSourcePosition(varRefExp, env, jToken);

#if 0
    ROSE_ASSERT(! varRefExp -> get_file_info() -> isTransformation());
    ROSE_ASSERT(! varRefExp -> get_file_info() -> isCompilerGenerated());
#endif
    astJavaComponentStack.push(varRefExp);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSuperReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionSuperReference(env, clz, jToken);
#else
	    SgClassDefinition *class_definition = getCurrentTypeDefinition();
    ROSE_ASSERT(class_definition -> get_declaration() && (! class_definition -> attributeExists("namespace")));

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
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSwitchStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionSwitchStatement(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSwitchStatementEnd(JNIEnv *env, jclass clz, jint numCases, jboolean hasDefaultCase, jobject jToken) 
{ 
	Java_JavaParser_cactionSwitchStatementEnd(env, clz, numCases, hasDefaultCase, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSynchronizedStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionSynchronizedStatement(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSynchronizedStatementEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionSynchronizedStatementEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionThisReference(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionThisReference(env, clz, jToken);
#else
    SgClassDefinition *class_definition = getCurrentTypeDefinition();
    ROSE_ASSERT(! class_definition -> attributeExists("namespace"));

    string className = class_definition -> get_declaration() -> get_name();
    // printf ("Current class for ThisReference is: %s \n", className.c_str());

    SgClassSymbol *class_symbol = isSgClassSymbol(class_definition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(class_symbol != NULL);

    SgThisExp *thisExp = SageBuilder::buildThisExp(class_symbol);
    ROSE_ASSERT(thisExp != NULL);

    astJavaComponentStack.push(thisExp);
#endif
}

/*
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionThisReferenceClassScope(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionThisReferenceClassScope(env, clz, jToken);
}
*/

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionThrowStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionThrowStatement(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionThrowStatementEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionThrowStatementEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTrueLiteral(JNIEnv *env, jclass clz, jobject jToken) 
{ 
#if 0
	Java_JavaParser_cactionTrueLiteral(env, clz, jToken);
#else
    SgExpression *expression = SageBuilder::buildBoolValExp(true);
    astJavaComponentStack.push(expression);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCatchBlockEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionCatchBlockEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTryStatement(JNIEnv *env, jclass clz, jint numCatchBlocks, jboolean hasFinallyBlock, jobject jToken) 
{ 
	Java_JavaParser_cactionTryStatement(env, clz, numCatchBlocks, hasFinallyBlock, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTryStatementEnd(JNIEnv *env, jclass clz, jint num_resources, jint num_catch_blocks, jboolean has_finally_block, jobject jToken) 
{ 
	Java_JavaParser_cactionTryStatementEnd(env, clz, num_resources, num_catch_blocks, has_finally_block, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUnaryExpression(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionUnaryExpression(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUnaryExpressionEnd(JNIEnv *env, jclass clz, jint java_operator_kind, jobject jToken) 
{ 
	Java_JavaParser_cactionUnaryExpressionEnd(env, clz, java_operator_kind, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionWhileStatement(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionWhileStatement(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionWhileStatementEnd(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionWhileStatementEnd(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionWildcard(JNIEnv *env, jclass clz, jobject jToken) 
{ 
	Java_JavaParser_cactionWildcard(env, clz, jToken);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionWildcardEnd(JNIEnv *env, jclass clz, jboolean is_unbound, jboolean has_extends_bound, jboolean has_super_bound, jobject jToken) 
{ 
	Java_JavaParser_cactionWildcardEnd(env, clz, is_unbound, has_extends_bound, has_super_bound, jToken);
}


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
#include "java_support.h"
#include "jni_utils.h"
#include "VisitorContext.h"

// This is needed so that we can call the FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope() function.
#include "fixupCxxSymbolTablesToSupportAliasingSymbols.h"

using namespace std;

/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionInsertClassStart(JNIEnv *env, jclass xxx, jstring java_string) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInsertClassStart(): = %s \n", name.str());

    SgScopeStatement *outerScope = astJavaScopeStack.top();
    ROSE_ASSERT(outerScope != NULL);
// TODO: REMOVE THIS !
/*
cout << "Inserting class " << name << " to scope "
     << (isSgClassDefinition(outerScope) && isSgClassDefinition(outerScope) -> attributeExists("namespace")
             ? isSgClassDefinition(outerScope) -> get_qualified_name().getString()
             : isSgClassDefinition(outerScope) && (! isSgClassDefinition(outerScope) -> attributeExists("namespace"))
                   ? isSgClassDefinition(outerScope) -> get_qualified_name().getString()
                   : outerScope -> class_name())
     << endl;
cout.flush();
*/
    SgClassDeclaration *declaration = buildJavaClass(name, outerScope);
    declaration -> set_parent(outerScope);

    // Make sure that the new class has been added to the correct synbol table.
    ROSE_ASSERT (outerScope -> lookup_class_symbol(declaration -> get_name()) != NULL);

    // If this is NOT an implicit class then use the class definition as the new current scope.
    SgClassDefinition *classDefinition = declaration -> get_definition();
    ROSE_ASSERT(classDefinition && (! classDefinition -> attributeExists("namespace")));
    astJavaScopeStack.push(classDefinition); // to contain the class members...

// TODO: REMOVE THIS !
//cout << "Done inserting up " << name << " in scope " << outerScope -> class_name() << endl;
//cout.flush();
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionInsertClassEnd(JNIEnv *env, jclass xxx, jstring java_string) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInsertClassEnd: %s \n", name.str());

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgClassDefinition *classDefinition = astJavaScopeStack.popClassDefinition();
    ROSE_ASSERT(! classDefinition -> attributeExists("namespace"));

// TODO: REMOVE THIS !
//cout << "Removing definition of " << classDefinition -> get_qualified_name() << " for " << name << endl;
//cout.flush();
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildClassSupportStart(JNIEnv *env, jclass xxx, jstring java_string, jboolean java_is_interface) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildClassSupportStart(): %s \n", name.str());

    bool is_interface = java_is_interface;

    SgScopeStatement *outerScope = astJavaScopeStack.top();
    ROSE_ASSERT(outerScope != NULL);
// TODO: REMOVE THIS !
/*
cout << "Building class " << name << " in scope "
     << (isSgClassDefinition(outerScope) && isSgClassDefinition(outerScope) -> attributeExists("namespace")
             ? isSgClassDefinition(outerScope) -> get_qualified_name().getString()
             : isSgClassDefinition(outerScope) && (! isSgClassDefinition(outerScope) -> attributeExists("namespace"))
                   ? isSgClassDefinition(outerScope) -> get_qualified_name().getString()
                   : outerScope -> class_name())
     << endl;
cout.flush();
*/

    SgClassSymbol *class_symbol = outerScope -> lookup_class_symbol(name);
    ROSE_ASSERT(class_symbol);
    SgClassDeclaration *declaration = (SgClassDeclaration *) class_symbol -> get_declaration() -> get_definingDeclaration();
    ROSE_ASSERT(declaration);
    SgClassDefinition *classDefinition = declaration -> get_definition();
    ROSE_ASSERT(classDefinition && (! classDefinition -> attributeExists("namespace")));
    astJavaScopeStack.push(classDefinition); // to contain the class members...

    declaration -> set_explicit_interface(is_interface); // Identify whether or not this is an interface

    astJavaComponentStack.push(classDefinition); // To mark the end of the list of components in this type.

// TODO: REMOVE THIS !
//cout << "Done setting up " << name << " in scope " << outerScope -> class_name() << endl;
//cout.flush();
    /*
    SgClassSymbol *classSymbol = ::globalScope -> lookup_class_symbol(classNameWithoutQualification);
    if (classSymbol != NULL) {
        // Nothing to do.
        if (SgProject::get_verbose() > 0)
            printf ("NOTHING TO DO: class = %s is already in global scope (qualified name = %s) \n", classNameWithoutQualification.str(), classNameWithQualification.str());
    }
    else {
        if (SgProject::get_verbose() > 0)
            printf ("class = %s must be placed into global scope (qualified name = %s) \n", classNameWithoutQualification.str(), classNameWithQualification.str());

        classSymbol = lookupSymbolFromQualifiedName(classNameWithQualification);

        ROSE_ASSERT(classSymbol != NULL);

        ROSE_ASSERT(! ::globalScope -> symbol_exists(classNameWithoutQualification));
        ROSE_ASSERT(! ::globalScope -> symbol_exists(classSymbol));

        SgAliasSymbol *aliasSymbol = new SgAliasSymbol(classSymbol,/* isRenamed */ /* false);

        if (SgProject::get_verbose() > 0)
            printf ("Adding SgAliasSymbol for classNameWithoutQualification = %s \n", classNameWithoutQualification.str());

        ::globalScope -> insert_symbol(classNameWithoutQualification, aliasSymbol);
    }
    */
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildClassSupportEnd(JNIEnv *env, jclass xxx, jstring java_string) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildClassSupportEnd: %s \n", name.str());

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgClassDefinition *classDefinition = astJavaScopeStack.popClassDefinition();
    ROSE_ASSERT(! classDefinition -> attributeExists("namespace"));

    for (SgStatement *statement = astJavaComponentStack.popStatement();
        statement != classDefinition;
        statement = astJavaComponentStack.popStatement()) {
        if (SgProject::get_verbose() > 2) {
            cerr << "(1) Adding statement "
                 << statement -> class_name()
                 << " to an implicit Type Declaration"
                 << endl;
            cerr.flush();
        }
        ROSE_ASSERT(statement != NULL);

        classDefinition -> prepend_statement(statement);
    }

    ROSE_ASSERT(! astJavaScopeStack.empty());
    SgScopeStatement *outerScope = astJavaScopeStack.top();

    //
    // TODO:  Review this because of the package issue and the inability to build a global AST.
    //
    ROSE_ASSERT(outerScope != NULL);
    if (isSgClassDefinition(outerScope) && isSgClassDefinition(outerScope) -> attributeExists("namespace")) { // a type in a package?
        isSgClassDefinition(outerScope) -> append_statement(classDefinition -> get_declaration());
    }
    else if (isSgClassDefinition(outerScope) && (! isSgClassDefinition(outerScope) -> attributeExists("namespace"))) { // an inner type?
        astJavaComponentStack.push(classDefinition -> get_declaration());
    }
    else if (outerScope == ::globalScope) { // a user-defined type?
        ::globalScope -> append_statement(classDefinition -> get_declaration());
    }
    else { // What is this?
        ROSE_ASSERT(false);
    }
}


/**
 *
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildMethodSupport(JNIEnv *env, jclass xxx,
                                                                 jstring java_string,
                                                                 jboolean java_is_constructor,
                                                                 jboolean java_is_abstract,
                                                                 jboolean java_is_native,
                                                                 jint java_number_of_arguments) {
    SgName name = convertJavaStringToCxxString(env, java_string);
    int number_of_arguments = java_number_of_arguments;
    bool is_constructor = java_is_constructor,
         is_abstract = java_is_abstract,
         is_native = java_is_native;

    if (SgProject::get_verbose() > 1)
        printf ("Build support for implicit class member function (method) name = %s \n", name.str());

    SgClassDefinition *classDefinition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(classDefinition != NULL && (! classDefinition -> attributeExists("namespace")));
// TODO: REMOVE THIS !
//cout << "Processing method " << name << " with " << number_of_arguments << " arguments in class " << classDefinition -> get_qualified_name() << endl;
//cout.flush();

    //
    // There is no reason to distinguish between defining and non-defining declarations in Java...
    //
    SgMemberFunctionDeclaration *functionDeclaration = buildDefiningMemberFunction(name, classDefinition, number_of_arguments);
    ROSE_ASSERT(functionDeclaration != NULL);

    if (is_constructor) {
        functionDeclaration -> get_specialFunctionModifier().setConstructor();
    }
    if (is_abstract) {
        functionDeclaration -> get_declarationModifier().setJavaAbstract();
        functionDeclaration -> setForward(); // indicate that this function does not contain a body.
    }
    if (is_native) {
        functionDeclaration -> get_functionModifier().setJavaNative();
        functionDeclaration -> setForward(); // indicate that this function does not contain a body.
    }

    astJavaComponentStack.push(functionDeclaration);
    setJavaFrontendSpecific(functionDeclaration);
}


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildFieldSupport(JNIEnv *env, jclass xxx, jstring java_string) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildFieldSupport (variable declaration for field) \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    SgType *type = astJavaComponentStack.popType();

    SgVariableDeclaration *variableDeclaration = SageBuilder::buildVariableDeclaration (name, type, NULL, astJavaScopeStack.top());
    ROSE_ASSERT(variableDeclaration != NULL);

    setJavaFrontendSpecific(variableDeclaration);

    astJavaComponentStack.push(variableDeclaration);

    if (SgProject::get_verbose() > 0)
        variableDeclaration -> get_file_info() -> display("source position in Java_JavaParser_cactionBuildFieldSupport(): debug");
}











// ***************************************************************************
// ***************************************************************************
// Implementation of functions generated from JNI specifications and matching
// prototype declarations found in the auto-generated file JavaParser.h in 
// the build tree at: src/3rdPartyLibraries/java-parser.
// ***************************************************************************
// ***************************************************************************


JNIEXPORT void JNICALL Java_JavaParser_cactionTest(JNIEnv *env, jclass) {
    cout << "Ok, The JNI connection was made !!!" << endl;
    cout.flush();
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPushPackage(JNIEnv *env, jclass, jstring java_package_name) {
    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name);

// TODO: REMOVE THIS !
//cout << "Pushing package " << package_name
//     << " (" << astJavaScopeStack.size() << ")"
//     << endl;
//cout.flush();

    astJavaScopeStack.push(::globalScope);

// TODO: Since we cannot use a NamespaceDefinition for a package, see the alternative code below.
/*
    SgNamespaceSymbol *namespace_symbol = ::globalScope -> lookup_namespace_symbol(package_name);
    SgNamespaceDeclarationStatement *declaration;
    if (namespace_symbol == NULL) {
        declaration = SageBuilder::buildNamespaceDeclaration(package_name, ::globalScope);
        ::globalScope -> append_declaration(declaration);
        declaration -> set_parent(::globalScope);
    }
    else {
        declaration = (SgNamespaceDeclarationStatement *) namespace_symbol -> get_declaration() -> get_definingDeclaration();
    }
    SgNamespaceDefinitionStatement *package = declaration -> get_definition();
*/
    SgClassSymbol *namespace_symbol = ::globalScope -> lookup_class_symbol(package_name);
    SgClassDeclaration *declaration;
    if (namespace_symbol == NULL) {
// TODO: REMOVE THIS!!!
//cout << "Pushing package " << package_name
//     << " (" << astJavaScopeStack.size() << ")"
//     << endl;
//cout.flush();
        declaration = SageBuilder::buildDefiningClassDeclaration(package_name, ::globalScope);
        declaration -> setAttribute("namespace", new AstRegExAttribute(package_name));
        SgClassDefinition *definition = declaration -> get_definition();
        ROSE_ASSERT(definition);
        definition -> setAttribute("namespace", new AstRegExAttribute(package_name));
        ::globalScope -> append_declaration(declaration);
        declaration -> set_parent(::globalScope);
    }
    else {
        declaration = (SgClassDeclaration *) namespace_symbol -> get_declaration() -> get_definingDeclaration();
    }
    SgClassDefinition *package = declaration -> get_definition();

    ROSE_ASSERT(package);

    astJavaScopeStack.push(package); // Push the package onto the scopestack.
}


JNIEXPORT void JNICALL Java_JavaParser_cactionPopPackage(JNIEnv *env, jclass) {
    ROSE_ASSERT(isSgClassDefinition(astJavaScopeStack.top()) && isSgClassDefinition(astJavaScopeStack.top()) -> attributeExists("namespace"));

    string package_name = isSgClassDefinition(astJavaScopeStack.top()) -> get_qualified_name().getString();

    astJavaScopeStack.popClassDefinition();
    astJavaScopeStack.popGlobal(); // Push the global scope onto the stack.

// TODO: REMOVE THIS !
//    cout << "Popping package " << package_name
//         << " (" << astJavaScopeStack.size() << ")"
//         << endl;
//    cout.flush();
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitList(JNIEnv *env, jclass, jint, jobjectArray) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionCompilationUnitList \n");

    // This is already setup by ROSE as part of basic file initialization before calling ECJ.
    ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
    if (SgProject::get_verbose() > 0)
        printf ("OpenFortranParser_globalFilePointer = %s \n", OpenFortranParser_globalFilePointer -> class_name().c_str());

    SgSourceFile *sourceFile = isSgSourceFile(OpenFortranParser_globalFilePointer);
    ROSE_ASSERT(sourceFile != NULL);

    if (SgProject::get_verbose() > 0)
        printf ("sourceFile -> getFileName() = %s \n", sourceFile -> getFileName().c_str());

    // We don't use the SgProject but since it should have already been built, we can verify that it is present.
    SgProject *project = sourceFile -> get_project();
    ROSE_ASSERT(project != NULL);
// TODO: REMOVE THIS !
/*
cout << "*** The files in this compilation unit list are:"
     << endl;
vector<string> files = project -> get_sourceFileNameList();
for (int i = 0; i < files.size(); i++) {
  cout << "    " << files[i] << endl;
}
cout << endl;
cout.flush();
*/

    // Get the pointer to the global scope and push it onto the astJavaScopeStack.
    ::globalScope = sourceFile -> get_globalScope();
    ROSE_ASSERT(::globalScope != NULL);

    //
    // At this point, the scope stack should be empty. Push the global scope into it.
    //
    ROSE_ASSERT(astJavaScopeStack.empty());
    astJavaScopeStack.push(::globalScope); // Push the global scope onto the stack.

    // Verify that the parent is set, these AST nodes are already setup by ROSE before calling this function.
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionCompilationUnitList \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitListEnd(JNIEnv *env, jclass) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionCompilationUnitListEnd() \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    astJavaScopeStack.popGlobal(); // remove the global scope

    ROSE_ASSERT(astJavaScopeStack.empty());

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionCompilationUnitListEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionSetupObject(JNIEnv *env, jclass) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionSetupObject\n");

// TODO: REMOVE THIS !
//cout << "About to set up Object" << endl;
//cout.flush();
    // TODO: Remove this!!!
    /*
    ::ObjectClassType = isSgClassType(astJavaComponentStack.popType());
    ROSE_ASSERT(::ObjectClassType);

    SgClassDeclaration *class_declaration = isSgClassDeclaration(::ObjectClassType -> get_declaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    ::ObjectClassDefinition = class_declaration -> get_definition();
    ROSE_ASSERT(::ObjectClassDefinition);
    */

    ROSE_ASSERT(astJavaScopeStack.size());
    ::ObjectClassDefinition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(::ObjectClassDefinition  && (! ::ObjectClassDefinition -> attributeExists("namespace")));
    ROSE_ASSERT(::ObjectClassDefinition -> get_qualified_name().getString().compare("java_lang.Object") == 0);

    SgName main_package_name = "java_lang",
           object_name       = "Object";
    ::ObjectClassType = isSgClassType(lookupTypeByName(main_package_name, object_name, 0));
    ROSE_ASSERT(::ObjectClassType);

// TODO: REMOVE THIS !
//cout << "Done setting up Object " << ::ObjectClassDefinition -> get_qualified_name().getString()  << endl;
//cout.flush();

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionSetupObject\n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionSetupString(JNIEnv *env, jclass) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionSetupObject\n");

    SgName main_package_name = "java_lang",
           string_name       = "String";

    ::StringClassType = isSgClassType(lookupTypeByName(main_package_name, string_name, 0));
    ROSE_ASSERT(::StringClassType);

// TODO: REMOVE THIS !
//cout << "Done setting up String " << ::StringClassType -> get_qualified_name().getString()  << endl;
//cout.flush();

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionSetupObject\n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclaration(JNIEnv *env, jclass, jstring java_package_name, jstring java_filename, jobject jToken) {
     if (SgProject::get_verbose() > 0)
         printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration() \n");
 
    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name);
    ROSE_ASSERT(astJavaScopeStack.top() == ::globalScope); // There must be a scope element in the scope stack.
    SgClassSymbol *namespace_symbol = ::globalScope -> lookup_class_symbol(package_name);

// TODO: REMOVE THIS !
//if (namespace_symbol == NULL) {
//cout << "Looking for but did not find package name: " << package_name
//     << endl;
//cout.flush();
//}

    ROSE_ASSERT(namespace_symbol);
    SgClassDeclaration *declaration = (SgClassDeclaration *) namespace_symbol -> get_declaration() -> get_definingDeclaration();
    SgClassDefinition *package = declaration -> get_definition();
    ROSE_ASSERT(package);

    //
    // Tag the package so that the unparser can process its containing user-defined types.
    //
    package -> setAttribute("package", new AstRegExAttribute(convertJavaStringToCxxString(env, java_package_name)));
    astJavaScopeStack.push(package); // Push the package onto the scopestack.

// TODO: REMOVE THIS !
//cout << "The package name is " << package_name
//     << endl;
//cout.flush();
/*
    ROSE_ASSERT(astJavaScopeStack.top() == ::globalScope);
    if (package_name.getString().size() > 0) { // There is a package?
        SgGlobal *global = (SgGlobal *) astJavaScopeStack.top();
        global -> setAttribute("package", new AstRegExAttribute(convertJavaStringToCxxString(env, java_package_name)));
    }
*/
//
// TODO: REMOVE THIS!!!
//
//    if (0 && package_name.getString().size() > 0) { // There is a package?
//        SgNamespaceDeclarationStatement *declaration = SageBuilder::buildNamespaceDeclaration(package_name, ::globalScope);
//        SgNamespaceDefinitionStatement *package = declaration -> get_definition();
//        astJavaScopeStack.push(package); // Push the package onto the scope stack.
//    }

    // Example of how to get the string...but we don't really use the absolutePathFilename in this function.
    const char *absolutePathFilename = env -> GetStringUTFChars(java_filename, NULL);
    ROSE_ASSERT(absolutePathFilename != NULL);
    // printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration absolutePathFilename = %s \n", absolutePathFilename);
    env -> ReleaseStringUTFChars(java_filename, absolutePathFilename);

    // This is already setup by ROSE as part of basic file initialization before calling ECJ.
    ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);

    astJavaComponentStack.push(astJavaScopeStack.top()); // To mark the end of the list of components in this Compilation unit.
}


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclarationEnd(JNIEnv *env, jclass, jint java_numberOfStatements, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclarationEnd() \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    // DQ (8/13/2011): Add more precise handling of the statement stack.
    int numberOfStatements = java_numberOfStatements;

    if (SgProject::get_verbose() > 0)
        printf ("cactionCompilationUnitDeclarationEnd(): numberOfStatements = %d \n", numberOfStatements);

//    if (isSgGlobal(astJavaScopeStack.top())) {
    SgScopeStatement *package = (SgScopeStatement *) astJavaScopeStack.top();
    for (SgStatement *statement = astJavaComponentStack.popStatement();
        statement != package;
        statement = astJavaComponentStack.popStatement()) {
        if (SgProject::get_verbose() > 2) {
            cerr << "(2) Adding statement "
                 << statement -> class_name()
                 << " to a Global Declaration"
                 << endl;
            cerr.flush();
        }
        ROSE_ASSERT(statement != NULL);
        package -> prepend_statement(statement);
    }
//    }
    astJavaScopeStack.popPackage(); // remove the containing package

// TODO: REMOVE THIS !
//    if (SgProject::get_verbose() > 0)
//        printf ("Now we pop off the global scope! \n");
//
//SgGlobal *global = (SgGlobal *) astJavaScopeStack.top();
//cout << "The name of the global node is " << global -> associatedScopeName() << endl;
//cout.flush();
//    astJavaScopeStack.popGlobal(); // remove the global scope
//    ROSE_ASSERT(astJavaScopeStack.empty());

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionCompilationUnitDeclarationEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclaration(JNIEnv *env, jclass,
                                                              jstring java_package_name,
                                                              jstring java_type_name,
                                                              jboolean java_is_interface,
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
    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

    SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);

    bool is_interface = java_is_interface;

    bool is_abstract = java_is_abstract;
    bool is_final = java_is_final;
    bool is_private = java_is_private;
    bool is_public = java_is_public;
    bool is_protected = java_is_protected;
    bool is_static = java_is_static;
    bool is_strictfp = java_is_strictfp;

    if (SgProject::get_verbose() > 0)
        printf ("Build class type: name = %s \n", type_name.str());

    Token_t *token = create_token(env, jToken);
    ROSE_ASSERT(token != NULL);

    // TODO: REMOVE THIS!!!
    // This builds the associated class in the curren scope as defined by the astJavaScopeStack.
    /*
    buildClass(name, token);
    */

    ROSE_ASSERT(astJavaScopeStack.top() != NULL);
    //
    // Types are always contained in a package. identify this user-defined type as such so that
    // it can be processed by the "unparser".
    //
// TODO: REMOVE THIS!!!
//SgClassDefinition *containing_package = isSgClassDefinition(astJavaScopeStack.top());
//if (containing_package){
//cout << "The containing package for type " << type_name << " is " << containing_package -> class_name()
//<< endl;
//cout.flush();
//}
//else{
//cout << "I expected a containing package for type " << type_name << " but found a " << astJavaScopeStack.top() -> class_name()
//<< endl;
//cout.flush();
//}
    ROSE_ASSERT(isSgClassDefinition(astJavaScopeStack.top()));

// TODO: Remove this!!! ... to be replace by newer code below.
    /*
    SgScopeStatement *outerScope = astJavaScopeStack.top();
    SgClassSymbol *class_symbol = outerScope -> lookup_class_symbol(name);
    ROSE_ASSERT(class_symbol);
    SgClassDeclaration *classDeclaration = (SgClassDeclaration *) class_symbol -> get_declaration() -> get_definingDeclaration();
    ROSE_ASSERT(classDeclaration);
    SgClassDefinition *classDefinition = classDeclaration -> get_definition();
    ROSE_ASSERT(classDefinition && (! classDefinition -> attributeExists("namespace")));
    setJavaSourcePosition(classDefinition, env, jToken);
    */

    SgClassDeclaration *classDeclaration = (SgClassDeclaration *) type -> getAssociatedDeclaration() -> get_definingDeclaration();
    ROSE_ASSERT(classDeclaration);
    SgClassDefinition *classDefinition = classDeclaration -> get_definition();
    ROSE_ASSERT(classDefinition && (! classDefinition -> attributeExists("namespace")));
    setJavaSourcePosition(classDefinition, env, jToken);

    classDeclaration -> setAttribute("user-defined-type", new AstRegExAttribute(type_name));
    classDeclaration -> set_explicit_interface(is_interface); // Identify whether or not this is an interface

    if (is_abstract)
         classDeclaration -> get_declarationModifier().setJavaAbstract();
    else classDeclaration -> get_declarationModifier().unsetJavaAbstract();
    if (is_final)
         classDeclaration -> get_declarationModifier().setFinal();
    else classDeclaration -> get_declarationModifier().unsetFinal();
    if (is_strictfp)
        ; // charles4 - TODO: there is currently no place to hang this information.

    classDeclaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);
    if (is_private) {
        classDeclaration -> get_declarationModifier().get_accessModifier().setPrivate();
    }
    if (is_public) {
        classDeclaration -> get_declarationModifier().get_accessModifier().setPublic();
    }
    if (is_protected) {
        classDeclaration -> get_declarationModifier().get_accessModifier().setProtected();
    }

    classDeclaration -> get_declarationModifier().get_storageModifier().set_modifier(SgStorageModifier::e_unknown);
    if (is_static) {
        classDeclaration -> get_declarationModifier().get_storageModifier().setStatic();
    }

    setJavaSourcePosition(classDefinition -> get_declaration(), env, jToken);

    astJavaScopeStack.push(classDefinition);     // Open new scope for this type.
    astJavaComponentStack.push(classDefinition); // To mark the end of the list of components in this type.

    if (SgProject::get_verbose() > 0)
        astJavaScopeStack.top() -> get_file_info() -> display("source position in Java_JavaParser_cactionTypeDeclaration(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionTypeDeclaration() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclarationHeader(JNIEnv *env, jclass,
                                                                    jboolean java_has_super_class,
                                                                    jint java_numberOfInterfaces,
                                                                    jint java_numberOfParameters,
                                                                    jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgClassDeclaration (cactionTypeDeclarationHeader) \n");

    bool has_super_class = java_has_super_class;
    int number_of_interfaces = java_numberOfInterfaces;
    int number_of_parameters = java_numberOfParameters;

    ROSE_ASSERT(astJavaScopeStack.top() != NULL);
    SgClassDefinition *classDefinition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(classDefinition != NULL  && (! classDefinition -> attributeExists("namespace")));

    if (SgProject::get_verbose() > 0)
        printf ("Build class type: name = %s \n", classDefinition -> get_qualified_name().str());

    ROSE_ASSERT(number_of_parameters == 0 && " We do not yet support Parameterized types");

    //
    // Process the interfaces for this type, if any.
    //
    for (int i = 0; i < number_of_interfaces; i++) {
         SgType *type = astJavaComponentStack.popType();
         SgClassType *interface_type = isSgClassType(type);
         SgDeclarationStatement *declaration = interface_type -> get_declaration();
         SgClassDeclaration *interface_declaration = isSgClassDeclaration(declaration -> get_definingDeclaration());
         ROSE_ASSERT(interface_declaration -> get_explicit_interface()); // must be an interface
         SgBaseClass *base = new SgBaseClass(interface_declaration);
         base -> set_parent(classDefinition);
         classDefinition -> prepend_inheritance(base);
    }

    //
    // Add Super class to the current Class definition.
    //
    if (has_super_class) {
        SgType *type = astJavaComponentStack.popType();
        SgClassType *class_type = isSgClassType(type);
        SgDeclarationStatement *declaration = class_type -> get_declaration();
        SgClassDeclaration *class_declaration = isSgClassDeclaration(declaration -> get_definingDeclaration());
        ROSE_ASSERT(! class_declaration -> get_explicit_interface()); // must be a class
        SgBaseClass *base = new SgBaseClass(class_declaration);
        base -> set_parent(classDefinition);
        classDefinition -> prepend_inheritance(base);
    }

    ROSE_ASSERT(classDefinition == astJavaComponentStack.top());

    if (SgProject::get_verbose() > 0)
        astJavaScopeStack.top() -> get_file_info() -> display("source position in Java_JavaParser_cactionTypeDeclarationHeader(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionTypeDeclarationHeader() (cactionTypeDeclarationHeader) \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclarationEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgClassDeclaration (cactionTypeDeclarationEnd) \n");

    ROSE_ASSERT(astJavaScopeStack.top() != NULL);
    SgClassDefinition *classDefinition = astJavaScopeStack.popClassDefinition();
    ROSE_ASSERT(! classDefinition -> attributeExists("namespace"));

    if (SgProject::get_verbose() > 0)
        printf ("Build class type: name = %s \n", classDefinition -> get_qualified_name().str());

    for (SgNode *node = astJavaComponentStack.top();
         node != classDefinition;
// TODO: REMOVE THIS !
// && (! isSgClassType(node)); // PC: Why is this test for SgClassType needed?
         node = astJavaComponentStack.top()) {
         astJavaComponentStack.pop(); // Remove that node from the stack
         SgStatement *statement = isSgStatement(node);
         ROSE_ASSERT(statement != NULL);
         if (SgProject::get_verbose() > 2) {
             cerr << "(3) Adding statement "
                  << statement -> class_name()
                  << " to a Type Declaration"
                  << endl;
             cerr.flush();
        }

        classDefinition -> prepend_statement(statement);
    }

// TODO: Remove this !!!
/*
SgClassType *t =  isSgClassType(astJavaComponentStack.top());
if (t) {
cout << "Stopped loop because of a class type: "
     << t -> get_qualified_name().str()
     << endl;
cout.flush();
}
*/
    ROSE_ASSERT(classDefinition == astJavaComponentStack.top());
    astJavaComponentStack.pop(); // remove the class definition from the stack

    if (SgProject::get_verbose() > 0)
        printf ("We might have to be popping off the existing scope for class type: name = %s \n", classDefinition -> get_qualified_name().str());

    SgClassDeclaration *classDeclaration = classDefinition -> get_declaration();
    ROSE_ASSERT(classDeclaration != NULL);

// TODO: REMOVE THIS !!! -- Now that we are processing things in two passes, this declaration has already been entered in its package or enclosing class.
//
    // Push the class declaration onto the statement stack.
    //    astJavaComponentStack.push(classDeclaration);

    ROSE_ASSERT(astJavaScopeStack.top() != NULL);
    if (SgProject::get_verbose() > 0)
        astJavaScopeStack.top() -> get_file_info() -> display("source position in Java_JavaParser_cactionTypeDeclarationEnd(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_JavaParser_cactionTypeDeclarationEnd() (cactionTypeDeclarationEnd) \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclaration(JNIEnv *env, jclass, jstring java_string, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgMemberFunctionDeclaration (constructor) \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    //
    // We start by pushing a VOID return type to make the constructor look like a method.
    //
    astJavaComponentStack.push(SgTypeVoid::createType()); 
}


JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclarationHeader(JNIEnv *env, jclass,
                                                                           jstring java_string, 
                                                                           jboolean java_is_native,
                                                                           jboolean java_is_private,
                                                                           jint java_numberOfTypeParameters,
                                                                           jint java_numberOfArguments,
                                                                           jint java_numberOfThrownExceptions,
                                                                           jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("End of SgMemberFunctionDeclaration (constructor) \n");

    SgName name   = convertJavaStringToCxxString(env, java_string);
    bool is_native = java_is_native;
    bool is_private = java_is_private;

    // DQ (7/31/2011): Add more precise handling of the statement stack.
    // This does not count (include) explicit constructor calls...
    int numberOfTypeParameters = java_numberOfTypeParameters;
    int numberOfArguments = java_numberOfArguments;
    int numberOfThrownExceptions = java_numberOfThrownExceptions;

    //
    // TODO:
    //
    for (int i = 0; i  < numberOfThrownExceptions; i++) { // Pop the Thrown types
        SgType *type = astJavaComponentStack.popType();
        // TODO: Do something with this Thrown type !!!
        ROSE_ASSERT(isSgClassType(type));
    }

    SgClassDefinition *classDefinition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(classDefinition != NULL  && (! classDefinition -> attributeExists("namespace"))); // we must be inside a class scope

    //
    // NOTE that we left the arguments and the return type on the Component stack.  They will be processed
    // by this function call. 
    //
    SgMemberFunctionDeclaration *function_declaration = lookupMemberFunctionInClassScope(classDefinition, name, numberOfArguments); // buildDefiningMemberFunction(name, classDefinition, numberOfArguments);
    ROSE_ASSERT(function_declaration);

    ROSE_ASSERT(numberOfTypeParameters == 0); // TODO: pop and process the Type Parameters.

    setJavaSourcePosition(function_declaration, env, jToken);

    SgFunctionDefinition *function_definition = function_declaration -> get_definition();
    ROSE_ASSERT(function_definition != NULL);

    setJavaSourcePosition(function_definition, env, jToken);

    astJavaScopeStack.push(function_definition);
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    ROSE_ASSERT(function_definition -> get_body() != NULL);
    astJavaScopeStack.push(function_definition -> get_body());
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    setJavaSourcePosition(function_definition -> get_body(), env, jToken);

    // Since this is a constructor, set it explicitly as such.
    function_declaration -> get_specialFunctionModifier().setConstructor();

    // Set the Java specific modifiers
    if (is_native) {
        function_declaration -> get_functionModifier().setJavaNative();
    }
}


JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclarationEnd(JNIEnv *env, jclass, jint java_numberOfStatements, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("End of SgMemberFunctionDeclaration (constructor) \n");

    // DQ (7/31/2011): Add more precise handling of the statement stack.
    // This does not count (include) explicit constructor calls...
    int numberOfStatements = java_numberOfStatements;

    if (SgProject::get_verbose() > 0)
        printf ("cactionConstructorDeclarationEnd(): numberOfStatements = %d\n", numberOfStatements);

    // Pop the constructor body...
    ROSE_ASSERT(! astJavaScopeStack.empty());
    SgBasicBlock *constructor_body = astJavaScopeStack.popBasicBlock();
    for (int i = 0; i  < numberOfStatements; i++) {
        SgStatement *statement = astJavaComponentStack.popStatement();
        if (SgProject::get_verbose() > 2) {
            cerr << "(4) Adding statement "
                 << statement -> class_name()
                 << " to a constructor declaration block"
                 << endl;
            cerr.flush();
        }
        constructor_body -> prepend_statement(statement);
    }

    SgFunctionDefinition *memberFunctionDefinition = astJavaScopeStack.popFunctionDefinition();

// TODO: REMOVE THIS !!! -- Now that we are processing things in two passes, this declaration has already been entered in its package or enclosing class.
//    ROSE_ASSERT(memberFunctionDefinition -> get_declaration() != NULL);
//    SgMemberFunctionDeclaration *memberFunctionDeclaration = isSgMemberFunctionDeclaration(memberFunctionDefinition -> get_declaration());
//    ROSE_ASSERT(memberFunctionDeclaration != NULL);
//
//    astJavaComponentStack.push(memberFunctionDeclaration);
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
                                                                         jint java_number_of_type_arguments,
                                                                         jint java_number_of_arguments,
                                                                         jobject jToken) {
    // Build a member function call...
    if (SgProject::get_verbose() > 0)
        printf ("Build a explicit constructor function call END \n");

    bool is_implicit_super = java_is_implicit_super;
    bool is_super = java_is_super;
    bool has_qualification = java_has_qualification;
    int  number_of_type_arguments = java_number_of_type_arguments;
    int  number_of_arguments = java_number_of_arguments;

    ROSE_ASSERT(number_of_type_arguments == 0 && "! yet support type arguments");

    SgExpression *qualification = (has_qualification ? astJavaComponentStack.popExpression() : NULL);

    //
    //
    //
    SgClassDefinition *class_definition = getCurrentClassDefinition();
    ROSE_ASSERT(! class_definition -> attributeExists("namespace"));

    if (qualification != NULL) {
        SgClassType *class_type = isSgClassType(qualification -> get_type());
        ROSE_ASSERT(class_type);

        SgDeclarationStatement *declaration = class_type -> get_declaration() -> get_definingDeclaration();
        SgClassDeclaration *class_declaration = (SgClassDeclaration *) declaration;
        class_definition = isSgClassDefinition(class_declaration -> get_definition());
        ROSE_ASSERT(class_definition  && (! class_definition -> attributeExists("namespace")));
    }

    if (is_super) {
        vector<SgBaseClass *> &inheritances = class_definition -> get_inheritances();
        if (inheritances.size() > 0 && (! inheritances[0] -> get_base_class() -> get_explicit_interface())) {
            class_definition = inheritances[0] -> get_base_class() -> get_definition(); // get the super class definition
        }
        else {
           class_definition = ::ObjectClassDefinition;
        }
    }

    SgClassSymbol *class_symbol = isSgClassSymbol(class_definition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(class_symbol != NULL);
    SgName name = class_definition -> get_declaration() -> get_name();

    //
    // The astJavaComponentStack has all of the arguments to the function call. Note that it is necessary to
    // obtain the original types associated with the formal parameters of the function in order to find the
    // perfect match.
    //
    list<SgType *> parameter_types;
    for (int i = 0; i < number_of_arguments; i++) { // reverse the arguments' order
        SgType *type = astJavaComponentStack.popType();
        parameter_types.push_front(type);
    }

    SgMemberFunctionSymbol *function_symbol = lookupFunctionSymbolInClassScope(class_definition, name, parameter_types);
    ROSE_ASSERT(function_symbol);

    // The astJavaComponentStack has all of the arguments to the function call.
    SgExprListExp *arguments = new SgExprListExp();
    for (int i = 0; i < number_of_arguments; i++) { // reverse the arguments' order
        SgExpression *expr = astJavaComponentStack.popExpression();
        arguments -> prepend_expression(expr);
    }
    setJavaSourcePosition(arguments, env, jToken);
    SgFunctionCallExp *function_call_exp = SageBuilder::buildFunctionCallExp(function_symbol, arguments);
    function_call_exp -> setAttribute("<init>", new AstRegExAttribute(is_super ? "super" : "this"));

    setJavaSourcePosition(function_call_exp, env, jToken);

    SgExpression *expr_for_function = function_call_exp;
    if (qualification != NULL) {
        expr_for_function = SageBuilder::buildBinaryExpression<SgDotExp>(qualification, expr_for_function);
    }

    SgExprStatement *expression_statement = SageBuilder::buildExprStatement(expr_for_function);
    ROSE_ASSERT(expression_statement != NULL);

    setJavaSourcePosition(expr_for_function, env, jToken);
    setJavaSourcePosition(expression_statement, env, jToken);

    // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
    // printf ("Previously calling appendStatement in cactionExplicitConstructorCallEnd() \n");
    // appendStatement(expressionStatement);
    astJavaComponentStack.push(expression_statement);
/*
    }
    else { // an implicit super call:  super();
cout << "An implicit constructor call - "
     << (is_implicit_super ? "is_implicit_super = true; " : "is_implicit_super = false; ")
     << (is_super ? "is_super = true" : "is_super = false")
<< endl;
cout.flush();
        ROSE_ASSERT(! has_qualification);
        ROSE_ASSERT(number_of_arguments == 0);
    }
*/
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclaration(JNIEnv *env, jclass, jstring java_string, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgMemberFunctionDeclaration \n");

    // Do nothing for now... See Java_JavaParser_cactionMethodDeclarationHeader(...)
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

    bool isAbstract     = java_is_abstract;
    bool isNative       = java_is_native;
    bool isStatic       = java_is_static;
    bool isFinal        = java_is_final;
    bool isSynchronized = java_is_synchronized;
    bool isPublic       = java_is_public;
    bool isProtected    = java_is_protected;
    bool isPrivate      = java_is_private;
    bool isStrictfp     = java_is_strictfp;

    int numberOfTypeParameters   = java_numberOfTypeParameters;
    int numberOfArguments        = java_numberOfArguments;
    int numberOfThrownExceptions = java_numberOfThrownExceptions;

    ROSE_ASSERT(numberOfTypeParameters == 0);

    //
    // TODO:
    //
    for (int i = 0; i < numberOfThrownExceptions; i++) { // Pop the Thrown types
        SgType *type = astJavaComponentStack.popType();
        // TODO: Do something with this Thrown type !!!
        ROSE_ASSERT(isSgClassType(type));
    }

    SgClassDefinition *classDefinition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(classDefinition != NULL  && (! classDefinition -> attributeExists("namespace")));

    //
    // NOTE that we left the arguments and the return type on the Component stack.  They will be processed
    // by this function call. 
    //
// TODO: REMOVE THIS !
//cout << "Got this far" << endl;
//cout.flush();
    SgMemberFunctionDeclaration *functionDeclaration = lookupMemberFunctionInClassScope(classDefinition, name, numberOfArguments); // buildDefiningMemberFunction(name, classDefinition, numberOfArguments);
    ROSE_ASSERT(functionDeclaration != NULL);
// TODO: REMOVE THIS !
//cout << "Got back from this far" << endl;
//cout.flush();

    setJavaSourcePosition(functionDeclaration, env, jToken);

    // This is not a defining function declaration so we can't identify the SgFunctionDefinition and push it's body onto the astJavaScopeStack.
    SgFunctionDefinition *functionDefinition = functionDeclaration -> get_definition();
    ROSE_ASSERT(functionDefinition != NULL);

    setJavaSourcePosition(functionDefinition, env, jToken);

    astJavaScopeStack.push(functionDefinition);
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    ROSE_ASSERT(functionDefinition -> get_body() != NULL);
    astJavaScopeStack.push(functionDefinition -> get_body());
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    setJavaSourcePosition(functionDefinition -> get_body(), env, jToken);

    // By default, the access modifier is set to unknown
    functionDeclaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    // Set the Java specific modifiers
    if (isAbstract) {
        functionDeclaration -> get_declarationModifier().setJavaAbstract();
        functionDeclaration -> setForward(); // indicate that this function does not contain a body.
    }

    // Set the Java specific modifiers
    if (isNative) {
        functionDeclaration -> get_functionModifier().setJavaNative();
        functionDeclaration -> setForward(); // indicate that this function does not contain a body.
    }

    // Set the specific modifier, this modifier is common to C/C++.
    if (isStatic) {
        functionDeclaration -> get_declarationModifier().get_storageModifier().setStatic();
    }

    // Set the modifier (shared between PHP and Java).
    if (isFinal) {
        functionDeclaration -> get_declarationModifier().setFinal();
    }

    // DQ (8/13/2011): Added more modifiers.
    if (isSynchronized) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Synchronized \n");
        functionDeclaration -> get_functionModifier().setJavaSynchronized();
    }

    // Set the access modifiers (shared between C++ and Java).
    if (isPublic) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Public \n");
        functionDeclaration -> get_declarationModifier().get_accessModifier().setPublic();
    }

    if (isProtected) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Protected \n");
        functionDeclaration -> get_declarationModifier().get_accessModifier().setProtected();
    }

    if (isPrivate) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as Private \n");
        functionDeclaration -> get_declarationModifier().get_accessModifier().setPrivate();
    }

    // Set the Java specific modifier for strict floating point (defined for functions).
    if (isStrictfp) {
        if (SgProject::get_verbose() > 2)
            printf ("Setting modifier as strictfp \n");
        functionDeclaration -> get_functionModifier().setJavaStrictfp();
    }

// TODO: REMOVE THIS! WE NO LONGER NEED TO PUSH THIS METHOD AS THE PRE-PASS ALREADY TOOK CARE OF THAT.
//    astJavaComponentStack.push(functionDeclaration);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclarationEnd(JNIEnv *env, jclass, int java_numberOfStatements, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("End of SgMemberFunctionDeclaration (method) \n");

    // Pop the constructor body...
    ROSE_ASSERT(! astJavaScopeStack.empty());

    int numberOfStatements = java_numberOfStatements;

    if (SgProject::get_verbose() > 0)
        printf ("In cactionMethodDeclarationEnd(): numberOfStatements = %d\n", numberOfStatements);

    SgBasicBlock *method_body = astJavaScopeStack.popBasicBlock(); // pop the body block
// TODO: REMOVE THIS !
//cout << "Attempting to pop "
//<< numberOfStatements
//<< " statements" << endl;
//cout.flush();
    for (int i = 0; i < numberOfStatements; i++) {
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

    SgFunctionDefinition *memberFunctionDefinition = astJavaScopeStack.popFunctionDefinition();

// TODO: REMOVE THIS! WE NO LONGER NEED TO PUSH THIS METHOD AS THE PRE-PASS ALREADY TOOK CARE OF THAT.
//    ROSE_ASSERT(isSgMemberFunctionDeclaration(astJavaComponentStack.top()));
}



JNIEXPORT void JNICALL Java_JavaParser_cactionTypeReference(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a type \n");

    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name);

    SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);

// REMOVE THIS !!!
/*
if (type == NULL) {
cout << "*Could not locate type " << type_name
     << endl;
cout.flush();
type = ::ObjectClassType;
}
*/
    ROSE_ASSERT(type != NULL);

    astJavaComponentStack.push(type);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArgument(JNIEnv *env, jclass, jstring java_argument_name, jboolean java_is_catch_argument, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a function argument \n");

    SgName argument_name = convertJavaStringToCxxString(env, java_argument_name);
    bool is_catch_argument = java_is_catch_argument;

    if (is_catch_argument) {
        SgCatchOptionStmt *catch_option_stmt = SageBuilder::buildCatchOptionStmt();
        ROSE_ASSERT(catch_option_stmt != NULL);
        setJavaSourcePosition(catch_option_stmt, env, jToken);
        catch_option_stmt -> set_parent(astJavaScopeStack.top());
        astJavaScopeStack.push(catch_option_stmt);
    }
}

JNIEXPORT void JNICALL Java_JavaParser_cactionArgumentEnd(JNIEnv *env, jclass, jstring java_argument_name, jboolean java_is_catch_argument, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a function argument \n");

    SgName argument_name = convertJavaStringToCxxString(env, java_argument_name);
// TODO: REMOVE THIS !
/*
cout << "Processing argument "
     << argument_name
     << endl;
cout.flush();
*/
    bool is_catch_argument = java_is_catch_argument;

    if (SgProject::get_verbose() > 0)
        printf ("argument argument_name = %s \n", argument_name.str());

    SgType *argument_type = astJavaComponentStack.popType();
    ROSE_ASSERT(argument_type);

    //
    // Create a variable declaration for the Catch Block parameter ... Make sure that it is inserted
    // in the proper scope.
    //
    if (is_catch_argument) {
        SgCatchOptionStmt *catch_option_stmt = isSgCatchOptionStmt(astJavaScopeStack.top());
        ROSE_ASSERT(catch_option_stmt);
        SgVariableDeclaration *variable_declaration = SageBuilder::buildVariableDeclaration(argument_name, argument_type, NULL, catch_option_stmt);
        setJavaSourcePosition(variable_declaration, env, jToken);
    
        catch_option_stmt -> set_condition(variable_declaration);
        variable_declaration -> set_parent(catch_option_stmt);
    }
    else {
        // Until we attached this to the AST, this will generate an error in the AST consistancy tests.
        SgInitializedName *initializedName = SageBuilder::buildInitializedName(argument_name, argument_type, NULL);
        ROSE_ASSERT(initializedName != NULL);

        setJavaSourcePosition(initializedName, env, jToken);

        //
        // DQ (4/6/2011): Instead of assuming there is a function declaration available, we 
        // want to put each SgInitializedName onto the stack so that they can be assembled
        // into the parameter list as part of building the function declaration afterwards.
        // This also allows for the function type to be computed from the function
        // parameter list (the types match the types on the function parameters) and
        // then the SgFunctionSymbol can be computed and inserted into the function 
        // declaration's scope (current scope) as part of building the function declaration.
        //
        astJavaComponentStack.push(initializedName);
    }
}


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

JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReferenceEnd(JNIEnv *env, jclass, jstring java_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a array type \n");

    // Nothing to do !!!
}

JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSend(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jstring java_function_name, jobject jToken) {
// TODO: REMOVE THIS!!!
  /*
    if (SgProject::get_verbose() > 1)
        printf ("Build a member function call (message send) \n");

    // Should this be a SgBasicBlock or just a SgScopeStatement?
    SgName package_name  = convertJavaStringToCxxString(env, java_package_name),
           type_name  = convertJavaStringToCxxString(env, java_type_name),
           function_name  = convertJavaStringToCxxString(env, java_function_name);

    if (SgProject::get_verbose() > 1)
        printf ("building function call: name = %s from class name = %s \n", name.str(), className.str());

    // Refactored this code to "lookupSymbolFromQualifiedName()" so it could be used to generate class types.
cout << "Sounding the alarm on class " << className << " and function " << name << endl;
cout.flush();
//    SgClassSymbol *targetClassSymbol = lookupSymbolFromQualifiedName(className);
    SgType *type = lookupTypeByName(package_name, type_name);
    SgClassDeclaration *class_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *targetClassScope = class_declaration -> get_definition();
    ROSE_ASSERT(targetClassScope != NULL && (! targetClassScope -> attributeExists("namespace")));
  */

// TODO: REMOVE THIS!!!
/*
    if (targetClassSymbol == NULL) {
        // This case can happen when in debugging mode where we artifically limit the number of implicit 
        // classes built and so some classes are not available.  We want to have defaults for this mode 
        // as a way of limiting the complexity of problems as they are debugged.

        // printf ("Build a SgTypeInt in the stack since the class was not built (debugging mode): name = %s\n", name.str());

        // This is not correct, but it allows me to move on and debug other work for now.
        // What this means is that functions from classes that we don't have available will not be resolved as function call expressions.
        printf ("ERROR: we can't find the targetClassScope for className = %s (skipping building this function call -- building int value expression for the stack instead) \n", className.str());
        SgValueExp *dummyValueExpression = SageBuilder::buildIntVal(9999999);
        ROSE_ASSERT(dummyValueExpression != NULL);
        astJavaComponentStack.push(dummyValueExpression);

        return;
    }
    else {
        // The associated type was found (after being explicitly built, after not being found the first time)) and is pushed onto the stack.
        // printf ("On the second search for the class = %s (after building it explicitly) it was found! \n", name.str());

        SgClassDeclaration *classDeclaration = isSgClassDeclaration(targetClassSymbol -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(classDeclaration != NULL);

        // SgScopeStatement *targetClassScope = classDeclaration -> get_definition();
        targetClassScope = classDeclaration -> get_definition();
        ROSE_ASSERT(targetClassScope != NULL);
    }
*/
/*
    if (SgProject::get_verbose() > 1)
        printf ("Looking for the function = %s in class parent scope = %p = %s \n", name.str(), targetClassScope, targetClassScope -> class_name().c_str());

    SgFunctionSymbol *functionSymbol = targetClassScope -> lookup_function_symbol(name);
    // ROSE_ASSERT(functionSymbol != NULL);
    if (functionSymbol == NULL) {
        astJavaComponentStack.push(SgTypeVoid::createType()); // push a dummy void return type for the function below.
        SgMemberFunctionDeclaration *functionDeclaration = buildDefiningMemberFunction(name, targetClassScope, 0); // TODO: Assume no ARGUMENT!!!  Temporary patch!!! Should go away !!!
        ROSE_ASSERT(functionDeclaration != NULL);
        functionSymbol = targetClassScope -> lookup_function_symbol(name);
        ROSE_ASSERT(functionSymbol != NULL);
    }

    // printf ("FOUND function symbol = %p \n", functionSymbol);

    // This is OK for now, but might not be good enough for a non-statement function call expression (not clear yet in ECJ AST).
    SgExprListExp *parameters = NULL;

    SgFunctionCallExp *func_call_expr = SageBuilder::buildFunctionCallExp(functionSymbol, parameters);

    setJavaSourcePosition(func_call_expr, env, jToken);

    // Push the expression onto the stack so that the cactionMessageSendEnd can find it 
    // and any function arguments and add the arguments to the function call expression.
    astJavaComponentStack.push(func_call_expr);
*/
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSendEnd(JNIEnv *env, jclass, 
                                                             jboolean java_is_static,
                                                             jstring java_package_name,
                                                             jstring java_type_name,
                                                             jstring java_function_name,
                                                             jint numTypeArguments,
                                                             jint numArguments,
                                                             jobject jToken) {
  bool is_static = java_is_static;

  SgName package_name  = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name  = convertJavaStringToCxxString(env, java_type_name),
           function_name  = convertJavaStringToCxxString(env, java_function_name);

    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionMessageSendEnd() \n");

    SgType *type = lookupTypeByName(package_name, type_name, 0);
    SgClassDeclaration *class_declaration = isSgClassDeclaration(type -> getAssociatedDeclaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *targetClassScope = class_declaration -> get_definition();
    ROSE_ASSERT(targetClassScope != NULL && (! targetClassScope -> attributeExists("namespace")));

    //
    // The astJavaComponentStack has all of the arguments to the function call. Note that it is necessary to
    // obtain the original types associated with the formal parameters of the function in order to find the
    // perfect match.
    //
    list<SgType *> function_parameter_types;
    for (int i = 0; i < numArguments; i++) { // reverse the arguments' order
        SgType *type = astJavaComponentStack.popType();
        function_parameter_types.push_front(type);
    }

    SgMemberFunctionSymbol *function_symbol = lookupFunctionSymbolInClassScope(targetClassScope, function_name, function_parameter_types);

    // The astJavaComponentStack has all of the arguments to the function call.
    SgExprListExp *arguments = new SgExprListExp();
    for (int i = 0; i < numArguments; i++) { // reverse the arguments' order
        SgExpression *expr = astJavaComponentStack.popExpression();
        arguments -> prepend_expression(expr);
    }
    setJavaSourcePosition(arguments, env, jToken);

    //
    // charles4: 11/08/2011 - Note that ECJ always adds a "receiver" as the first argument
    // in a function call.  This receiver is an expression (or type ?) that indicates the instance that
    // is associated with the function to be invoked. 
    //
    SgNode *receiver = astJavaComponentStack.pop();

    SgFunctionCallExp *functionCallExp = SageBuilder::buildFunctionCallExp(function_symbol, arguments);

    setJavaSourcePosition(functionCallExp, env, jToken);
    /*
    SgMemberFunctionRefExp *functionRefExp = isSgMemberFunctionRefExp(functionCallExp -> get_function());
    ROSE_ASSERT(functionRefExp != NULL);

    setJavaSourcePosition(functionRefExp, env, jToken);
    */
    SgMemberFunctionDeclaration *functionDeclaration = isSgMemberFunctionDeclaration(function_symbol -> get_declaration() -> get_definingDeclaration());
    ROSE_ASSERT(functionDeclaration != NULL);

    //
    // Finalize the function by adding its "receiver" prefix.  Note that it is illegal to add a "this."
    // prefix in front of a static method call - Hence the guard statement below. (ECJ always adds a "this."
    // prefix in front of every function whose receiver was not specified by the user.)
    //
    SgExpression *exprForFunction = functionCallExp;
    if (isSgNamedType(receiver)) { // Note that if this is true then the function must be static... See unparseJava_expression.C: unparseFucnCall
// TODO: REMOVE THIS !
/*
cout << "Dealing with type prefix in MessageSend"
     << endl;
cout.flush();
*/
        if (isSgClassType(receiver)) { // Note that if this is true then the function must be static... See unparseJava_expression.C: unparseFucnCall
            SgClassType *type = isSgClassType(receiver);
            string full_name = isSgNamedType(receiver) -> get_qualified_name(),
                   type_name = isSgNamedType(receiver) -> get_name();
            //
            // TODO: Do this cleanly by tagging every package with its original name and reconstituting the name of the type...
            //
            replace(full_name.begin(), full_name.end(), '_', '.'); // restore the package 
            string class_name = (full_name.size() == type_name.size() ? type_name : full_name.substr(0, full_name.size() - type_name.size()) + type_name);
            exprForFunction -> setAttribute("prefix", new AstRegExAttribute(class_name));
        }
        else { // this can't happen!
            ROSE_ASSERT(false); 
        }
    }
    else if (! is_static) { // (isSgThisExp(receiver) && functionDeclaration -> get_declarationModifier().get_storageModifier().isStatic())) {
// TODO: REMOVE THIS !
/*
cout << "Dealing with MessageSend for static function "
     << functionDeclaration -> get_name().getString()
     << endl;
cout.flush();
// delete receiver;
*/
        exprForFunction = SageBuilder::buildBinaryExpression<SgDotExp>((SgExpression *) receiver, exprForFunction);
    }

    astJavaComponentStack.push(exprForFunction);

    if (SgProject::get_verbose() > 2)
        printf ("Leaving Java_JavaParser_cactionMessageSendEnd() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedNameReference(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_prefix, jstring java_type_name, jstring java_name, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a qualified name reference \n");

    string package_name = convertJavaPackageNameToCxxString(env, java_package_name);
    string type_prefix = convertJavaStringToCxxString(env, java_type_prefix);
    string type_name = convertJavaStringToCxxString(env, java_type_name);
    string qualifiedName = convertJavaStringToCxxString(env, java_name);

    // list<SgName> qualifiedNameList = generateQualifierList(qualifiedName);
    SgClassType *class_type = NULL;
    if (type_prefix.length() > 0) {
        string full_name = (package_name.length() > 0 ? (package_name + ".") : "") + type_name;
        class_type = lookupTypeFromQualifiedName(full_name);
        ROSE_ASSERT(class_type);
    }

    if (qualifiedName.length() == 0) { // The qualified name is a Type
        ROSE_ASSERT(type_name.length() > 0);
        astJavaComponentStack.push(class_type);
    }
    else {
        list<SgName> nameList = generateQualifierList(qualifiedName);
        list<SgName>::iterator i = nameList.begin();

        //
        // Find the first variable and create a variable reference for it.
        //
        SgVariableSymbol *variable_symbol;
        if (class_type != NULL) {
             //
             // Look for the class scope.
             //
            SgDeclarationStatement *declaration = class_type -> get_declaration() -> get_definingDeclaration();
            SgClassDeclaration *class_declaration = (SgClassDeclaration *) declaration;
            SgClassDefinition *definition = isSgClassDefinition(class_declaration -> get_definition());
            ROSE_ASSERT(definition  && (! definition -> attributeExists("namespace")));
            variable_symbol = lookupSimpleNameVariableInClassScope(*i, definition);
        }
        else {
            variable_symbol = lookupVariableByName(*i);
        }
        ROSE_ASSERT(variable_symbol);
        SgExpression *result = SageBuilder::buildVarRefExp(variable_symbol);
        ROSE_ASSERT(result != NULL);

        setJavaSourcePosition(result, env, jToken);

        //
        // Now, process the remaining name items as the right-hand side of a DOT expression.
        //
        for (i++; i != nameList.end(); i++) {
            SgType *type = result -> get_type();
            ROSE_ASSERT(! isSgJavaParameterizedType(type));
            if (isSgArrayType(type)) {
                ROSE_ASSERT((*i).getString().compare("length") == 0);
                result -> setAttribute("suffix", new AstRegExAttribute("length"));
            }
            else {
                SgClassType *class_type = isSgClassType(type);
                SgClassDeclaration *class_declaration = (SgClassDeclaration *) class_type -> get_declaration() -> get_definingDeclaration();
                SgClassDefinition *definition = isSgClassDefinition(class_declaration -> get_definition());
                ROSE_ASSERT(definition  && (! definition -> attributeExists("namespace")));
                variable_symbol = lookupSimpleNameVariableInClassScope(*i, definition);
                ROSE_ASSERT(variable_symbol);
                SgVarRefExp *rhs = SageBuilder::buildVarRefExp(variable_symbol);
                ROSE_ASSERT(rhs != NULL);
                setJavaSourcePosition(rhs, env, jToken);
                result = SageBuilder::buildBinaryExpression<SgDotExp>(result, rhs); // The Java "." operator is equivalent to the C++ Arrow " -> ".
            }
        }

        //
        // If there is a type prefix, add it here to the resulting expression
        //
        if (type_prefix.length() > 0) {
            result -> setAttribute("prefix", new AstRegExAttribute(type_prefix));
        }

        astJavaComponentStack.push(result);
    }
}


JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteral(JNIEnv *env, jclass, jstring java_string, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgStringVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    // string stringLiteral = "stringLiteral_abc";
    SgName stringLiteral = convertJavaStringToCxxString(env, java_string);

    // printf ("Building a string value expression = %s \n", stringLiteral.str());

    SgStringVal *stringValue = SageBuilder::buildStringVal(stringLiteral); // new SgStringVal(stringLiteral);
    ROSE_ASSERT(stringValue != NULL);

    // Set the source code position (default values for now).
    // setJavaSourcePosition(stringValue);
    setJavaSourcePosition(stringValue, env, jToken);

    astJavaComponentStack.push(stringValue);
}


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
// JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportStart(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken)
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportStart(JNIEnv *env, jclass xxx, jstring java_string) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildImplicitClassSupportStart(): build support for implicit class (start) name = %s \n", name.str());

    // printf ("In Java_JavaParser_cactionBuildImplicitClassSupportStart(): Exiting as a test! \n");
    // ROSE_ASSERT(false);

    // This builds a class to represent the implicit classes that are available by default within Java.
    // Each is built on an as needed basis (driven by references to the class).
    buildImplicitClass(name);
    SgClassDefinition *classDefinition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(classDefinition != NULL  && (! classDefinition -> attributeExists("namespace")));
    astJavaComponentStack.push(classDefinition); // To mark the end of the list of components in this type.

    // DQ (4/15/2011): This code should be refactored ...
    SgName classNameWithQualification = name;
    if (SgProject::get_verbose() > 0)
        printf ("implicit class = %s \n", classNameWithQualification.str());

    ROSE_ASSERT(::globalScope != NULL);

    SgName classNameWithoutQualification = stripQualifiers(classNameWithQualification);
    SgClassSymbol *classSymbol = ::globalScope -> lookup_class_symbol(classNameWithoutQualification);
    if (classSymbol != NULL) {
        // Nothing to do.
        if (SgProject::get_verbose() > 0)
            printf ("NOTHING TO DO: class = %s is already in global scope (qualified name = %s) \n", classNameWithoutQualification.str(), classNameWithQualification.str());
    }
    else {
        if (SgProject::get_verbose() > 0)
            printf ("class = %s must be placed into global scope (qualified name = %s) \n", classNameWithoutQualification.str(), classNameWithQualification.str());

        classSymbol = lookupSymbolFromQualifiedName(classNameWithQualification);

        ROSE_ASSERT(classSymbol != NULL);

        ROSE_ASSERT(! ::globalScope -> symbol_exists(classNameWithoutQualification));
        ROSE_ASSERT(! ::globalScope -> symbol_exists(classSymbol));

        SgAliasSymbol *aliasSymbol = new SgAliasSymbol(classSymbol,/* isRenamed */ false);

        if (SgProject::get_verbose() > 0)
            printf ("Adding SgAliasSymbol for classNameWithoutQualification = %s \n", classNameWithoutQualification.str());

        ::globalScope -> insert_symbol(classNameWithoutQualification, aliasSymbol);
    }
}

// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
// JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportEnd(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken)
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportEnd(JNIEnv *env, jclass xxx, jint java_numberOfStatements, jstring java_string) {
    SgName name = convertJavaStringToCxxString(env, java_string);

    if (SgProject::get_verbose() > 0)
        printf ("Build support for implicit class (end) for class = %s \n", name.str());

    ROSE_ASSERT(! astJavaScopeStack.empty());

    // DQ (8/20/2011): This is the class that we just built implicitly
    ROSE_ASSERT(astJavaScopeStack.top() != NULL);
    SgClassDefinition *classDefinition = astJavaScopeStack.popClassDefinition();
    ROSE_ASSERT(! classDefinition -> attributeExists("namespace"));

    // DQ (7/31/2011): Collection up all of the statements and append to the current scope.
    // Later I would like to do this more precisely, but for now collect all statements.
    int numberOfStatements = java_numberOfStatements;

    if (SgProject::get_verbose() > 2)
        printf ("Appending %d statments\n", numberOfStatements);

    for (SgStatement *statement = astJavaComponentStack.popStatement();
        statement != classDefinition;
        statement = astJavaComponentStack.popStatement()) {
        if (SgProject::get_verbose() > 2) {
            cerr << "(6) Adding statement "
                 << statement -> class_name()
                 << " to an implicit Type Declaration"
                 << endl;
            cerr.flush();
        }
        ROSE_ASSERT(statement != NULL);
        classDefinition -> prepend_statement(statement);
    }

    // Output the list of implicit classes seen so far...
    if (SgProject::get_verbose() > 0) {
        printf ("astJavaImplicitClassList: \n");
        for (list<SgName>::iterator i = astJavaImplicitClassList.begin(); i != astJavaImplicitClassList.end(); i++) {
            printf ("   --- implicit class = %s \n",(*i).str());
        }
    }
}


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
// JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitMethodSupport(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken)
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitMethodSupport(JNIEnv *env, jclass xxx, jstring java_string, jint java_number_of_arguments) {
    SgName name = convertJavaStringToCxxString(env, java_string);
    int number_of_arguments = java_number_of_arguments;

    if (SgProject::get_verbose() > 1)
        printf ("Build support for implicit class member function (method) name = %s \n", name.str());

    // Not sure if we want anything specific to implicit class handling to touch the astJavaScopeStack!
    SgClassDefinition *classDefinition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(classDefinition != NULL  && (! classDefinition -> attributeExists("namespace")));

    // DQ (3/25/2011): Changed this to a non-defining declaration.
    // SgMemberFunctionDeclaration *functionDeclaration = buildSimpleMemberFunction(name, classDefinition);
    SgMemberFunctionDeclaration *functionDeclaration = buildNonDefiningMemberFunction(name, classDefinition, number_of_arguments);
    ROSE_ASSERT(functionDeclaration != NULL);

    // setJavaCompilerGenerated(functionDeclaration);
    setJavaFrontendSpecific(functionDeclaration);
}


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitFieldSupport(JNIEnv *env, jclass xxx, jstring java_string) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionBuildImplicitFieldSupport (variable declaration for field) \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    SgType *type = astJavaComponentStack.popType();

    SgVariableDeclaration *variableDeclaration = SageBuilder::buildVariableDeclaration(name, type, NULL, astJavaScopeStack.top());
    ROSE_ASSERT(variableDeclaration != NULL);

    // setJavaCompilerGenerated(variableDeclaration);
    setJavaFrontendSpecific(variableDeclaration);

    ROSE_ASSERT(! astJavaScopeStack.empty());

    // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
    // printf ("Previously calling appendStatement in cactionBuildImplicitFieldSupport() \n");
    // appendStatement(variableDeclaration);

    astJavaComponentStack.push(variableDeclaration);

    if (SgProject::get_verbose() > 0)
        variableDeclaration -> get_file_info() -> display("source position in Java_JavaParser_cactionBuildImplicitFieldSupport(): debug");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateType(JNIEnv *env, jclass xxx, jstring java_package_name, jstring java_type_name, jint java_num_dimensions) {
    // This is general support for building types.  Using JNI one to many calls from Java are made to C++ 
    // with enough information to build specific types or parts of types (for aggragate types such as classes).
    // All type information is constructed onto the stack, at the end of any recursion a single type is on
    // the astJavaComponentStack.

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionGenerateType() \n");

    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name),
           name = package_name.getString() + string(package_name.getString().size() ? "." : "") + type_name.getString();
// TODO: REMOVE THIS !
//cout << "cactionGenerating type " 
//     << name
//     << endl;
//cout.flush();

    int number_of_dimensions = java_num_dimensions;
    // printf ("Java_JavaParser_cactionGenerateType(): name = %s \n", name.str());

    // Type details (from http://java.sun.com/docs/books/jls/second_edition/html/jTOC.doc.html):
    // boolean
    //    1-bit. May take on the values true and false only. true and false are defined constants of the language 
    //    and are not the same as True and False, TRUE and FALSE, zero and nonzero, 1 and 0 or any other numeric 
    //    value. Booleans may not be cast into any other type of variable nor may any other variable be cast into 
    //    a boolean.
    // byte
    //    1 signed byte (two's complement). Covers values from -128 to 127.
    // short
    //    2 bytes, signed (two's complement), -32,768 to 32,767
    // int
    //    4 bytes, signed (two's complement). -2,147,483,648 to 2,147,483,647. Like all numeric types ints may be 
    //    cast into other numeric types (byte, short, long, float, double). When lossy casts are done (e.g. int 
    //    to byte) the conversion is done modulo the length of the smaller type.
    // long
    //    8 bytes signed (two's complement). Ranges from -9,223,372,036,854,775,808 to +9,223,372,036,854,775,807.
    // float
    //    4 bytes, IEEE 754. Covers a range from 1.40129846432481707e-45 to 3.40282346638528860e+38 (positive or negative).
    //    Like all numeric types floats may be cast into other numeric types (byte, short, long, int, double). When 
    //    lossy casts to integer types are done (e.g. float to short) the fractional part is truncated and the conversion is done modulo the length of the smaller type.
    // double
    //    8 bytes IEEE 754. Covers a range from 4.94065645841246544e-324d to 1.79769313486231570e+308d (positive or negative). 
    // char
    //    2 bytes, unsigned, Unicode, 0 to 65,535 Chars are not the same as bytes, ints, shorts or Strings.

    SgType *type = NULL;
    if (name == "boolean") {
        // This is a logical true/false value (it's bit widths in implementation dependent.
        type = SgTypeBool::createType();
    }
    else if (name == "byte") {
        // DQ (4/3/2011): In Java the type "byte" is signed!
        // Reference: http://www.javamex.com/java_equivalents/unsigned.shtml
        type = SgTypeSignedChar::createType();
    }
    else if (name == "char") {
        // In Java, all integers are signed, except for "char". However a "char" is 2 byte unicode so it might be better for it to be SgTypeWchar.
        type = SgTypeWchar::createType();
    }
    else if (name == "int") {
        // This should be a 32-bit type, but ROSE does not specify the bit length explictly (we could us 32-bit field widths, I suppose).
        type = SgTypeInt::createType();
    }
    else if (name == "short") {
        // This is a 2 byte signed type.
        type = SgTypeShort::createType();
    }
    else if (name == "float") {
        // This is a 4 byte floating point type.
        type = SgTypeFloat::createType();
    }
    else if (name == "long") {
        // This is a 8 byte signed type.
        type = SgTypeLong::createType();
    }
    else if (name == "double") {
        // This is an 8 byte floating point type.
        type = SgTypeDouble::createType();
    }
    else if (name == "void") {
        // DQ (4/5/2011): Added support for "void" type (also used as return type fo constructors).
        // This is the same sort of void type as in C/C++.
        type = SgTypeVoid::createType();
    }
    else if (name == "null") {
        // There is also a special null type, the type of the expression null, which has no name. Because the null type has 
        // no name, it is impossible to declare a variable of the null type or to cast to the null type. The null reference 
        // is the only possible value of an expression of null type. The null reference can always be cast to any reference 
        // type. In practice, the programmer can ignore the null type and just pretend that null is merely a special literal 
        // that can be of any reference type.

        // Within ROSE it is not yet clear if I should define a new SgType (SgTypeNull) to represent a null type.  For now it
        // is an error to try to build such a type.

        printf ("Error: SgTypeNull (Java null type) support not implemented (name = %s) \n", name.str());
        ROSE_ASSERT(false);
    }
    else {
      /*
        // DQ (8/17/2011): This should be any implicit type defined in Java (so test for the prefix "java.").
        // else if (name == "String")
        // else if (name == "java.lang.String")

        // Strip off the suffix "#RAW" on the class name that is an artifact of Java versions 1.5 and greater???
        name = processNameOfRawType(name);
        string nameString = name;

        // Check if the typename starts with "java."
        if (nameString.find("java.",0) == 0) {
            // DQ (4/9/2011): Added support for "String" type.
            // This should maybe be a SgClassType specific to the String class, instead of using the SgTypeString for Java.

            SgClassType *classType = lookupTypeFromQualifiedName(name);

            // printf ("In Java_JavaParser_cactionGenerateType(): classType = %p \n", classType);
            if (classType == NULL) {
                // If the "String" class was not found then it is likely because we are in a debug mode which limits the number of implecit classes.
                // printf ("Build a class for java.<class name>.<type name>: name = %s\n", name.str());

                // We build the class but we need the declarations in the class.
                buildImplicitClass(name);
                // printf ("DONE: Build a class for java.<class name>.<type name>: name = %s\n", name.str());

                // DQ (8/20/2011): The new class is on the stack, we want to get it's data members into place so that they will be available to be called.
                // Can we call Java to force the members to be traversed?  Not clear how to do this!

                //
                // First, remove the Class Definition that was pushed into the scope stack by the
                // function buildImplicitClass() (via the function buildClassSupport()).
                //
                astJavaScopeStack.popClassDefinition();

                // We need to leave a SgType on the astJavaComponentStack, we need to build the class to build 
                // the SgClassType, but we don't want to leave a SgClassDefinition on the astJavaScopeStack.
                // printf ("When we just build a type we don't want the new class definition on the stack. \n");

                SgClassType *classType = lookupTypeFromQualifiedName(name);
                if (classType == NULL) {
                    printf ("Build a SgTypeInt in the stack since the class was not built (debugging mode): name = %s\n", name.str());
                    type = SgTypeInt::createType();
                }
                else {
                    // The associated type was found (after being explicitly built, after not being found the first time)) and is pushed onto the stack.
                    // printf ("On the second search for the class = %s (after building it explicitly) it was found! \n", name.str());
                    type = classType;
                }
            }
            else {
                // The associated type was found and is pushed onto the stack.
                type = classType;
            }
        }
        else {
            // This is the last chance to resolve the type.
            if (SgProject::get_verbose() > 0)
                printf ("I think this is a user defined class (so we have to look it up) (name = %s) \n", name.str());

            type = lookupTypeFromQualifiedName(name);
//            ROSE_ASSERT(type != NULL);
// REMOVE THIS !!!
if (type == NULL) {
cout << "Could not locate type " << name
     << endl;
cout.flush();
type = ::ObjectClassType;
}
        }
*/
        type = lookupTypeByName(package_name, type_name, number_of_dimensions);
        ROSE_ASSERT(type);
    }
}


// JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateClassType(JNIEnv *env, jclass xxx, jstring java_string, jobject jToken)
JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateClassType(JNIEnv *env, jclass xxx, jstring java_string) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionGenerateClassType() \n");

    SgName name = convertJavaStringToCxxString(env, java_string);

    // printf ("In Java_JavaParser_cactionGenerateClassType(): Calling lookupSymbolFromQualifiedName(name = %s) \n", name.str());
    SgClassType *classType = lookupTypeFromQualifiedName(name);

    // ROSE_ASSERT(classType != NULL);
    if (classType != NULL) {
        astJavaComponentStack.push(classType);
    }
    else {
        printf ("WARNING: SgClassSymbol NOT FOUND in Java_JavaParser_cactionGenerateClassType(): name = %s (build an integer type and keep going...) \n", name.str());
        astJavaComponentStack.push(SgTypeInt::createType());
    }
}


JNIEXPORT void JNICALL Java_JavaParser_cactionAllocationExpression(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionAllocationExpression() \n");

    // Nothing to do !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionAllocationExpressionEnd(JNIEnv *env, jclass, jstring java_string, jint java_num_arguments, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionAllocationExpressionEnd() \n");

    SgName name = convertJavaStringToCxxString(env, java_string);
    int num_arguments = java_num_arguments;

// REMOVE THIS !!!
//    printf ("Build a new operator for type name = %s \n", name.str());

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

    SgType *type = astJavaComponentStack.popType();
    SgConstructorInitializer *constInit = SageBuilder::buildConstructorInitializer(NULL,
                                                                                   SageBuilder::buildExprListExp(arguments),
                                                                                   type,
                                                                                   false,
                                                                                   false,
                                                                                   false,
                                                                                   ! (isSgClassType(type)));

    // TODO: I think a SgJavaParameterizedType should be a SgClassType.  Currrently, it is not!

    // For the simple case, we only need the type as input to build SgNewExp.
    SgExprListExp *exprListExp = NULL;
    SgExpression *expr                  = NULL;
    short int val                       = 0;
    SgFunctionDeclaration *funcDecl     = NULL;

    SgNewExp *newExpression = SageBuilder::buildNewExp(type, exprListExp, constInit, expr, val, funcDecl);
    ROSE_ASSERT(newExpression != NULL);
    constInit -> set_parent(newExpression);

    setJavaSourcePosition(newExpression, env, jToken);

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


JNIEXPORT void JNICALL Java_JavaParser_cactionAnnotationMethodDeclaration(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT( ! "yet implemented");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArgumentClassScope(JNIEnv *, jclass, jstring, jobject jToken) {
    ROSE_ASSERT( ! "yet implemented");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayAllocationExpression(JNIEnv *env, jclass, jobject jToken) {
    // NOTHING TO DO !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayAllocationExpressionEnd(JNIEnv *env, jclass, jstring java_string, jint java_num_dimensions, jboolean java_has_initializers, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionArrayAllocationExpressionEnd() \n");
  
    SgName name = convertJavaStringToCxxString(env, java_string);
    int num_dimensions = java_num_dimensions;
    bool has_initializers = java_has_initializers;
    if (has_initializers) {
        ROSE_ASSERT(! "Yet Implemented");
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

    // SgArrayType *array_type = getUniqueArrayType(type, num_dimensions);
    SgPointerType *pointer_type = getUniquePointerType(type, num_dimensions);
    SgConstructorInitializer *constInit = SageBuilder::buildConstructorInitializer(NULL,
                                                                                   SageBuilder::buildExprListExp(arguments),
                                                                                   type,
                                                                                   false,
                                                                                   false,
                                                                                   false,
                                                                                   ! (isSgClassType(type) || isSgJavaParameterizedType(type)));
    // TODO: I think a SgJavaParameterizedType should be a SgClassType.  Currrently, it is not!

    // For the simple case, we only need the type as input to build SgNewExp.
    SgExprListExp *exprListExp = NULL;
    SgExpression *expr                  = NULL;
    short int val                       = 0;
    SgFunctionDeclaration *funcDecl     = NULL;

    // SgNewExp *newExpression = SageBuilder::buildNewExp(array_type, exprListExp, constInit, expr, val, funcDecl);
    SgNewExp *newExpression = SageBuilder::buildNewExp(pointer_type, exprListExp, constInit, expr, val, funcDecl);
    ROSE_ASSERT(newExpression != NULL);
    constInit -> set_parent(newExpression);

    setJavaSourcePosition(newExpression, env, jToken);

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


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayQualifiedTypeReference(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT( ! "yet implemented");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT( ! "yet implemented");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayReference(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
         printf ("Inside of Java_JavaParser_cactionArrayReference() \n");

    // Nothins to do !!!

}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayReferenceEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionArrayReferenceEnd() \n");

// TODO: Remove this
/*
SgExpression *rhs = astJavaComponentStack.popExpression(),
             *lhs = astJavaComponentStack.popExpression();
if (isSgVarRefExp(lhs)) {
SgVarRefExp *var = isSgVarRefExp(lhs);
SgArrayType *array_type = isSgArrayType(var -> get_type());
ROSE_ASSERT(array_type);
 SgType *type = array_type -> get_base_type();
  int rank = array_type -> get_rank();
 if (rank > 1) { // Turn Java array into a pointer to make it look like C++ in order to please the consistency check in Cxx_Grammar.C
     SgPointerType *pointer_type = SageBuilder::buildPointerType(type);
     for (int i = 1; i < rank; i++) {
        pointer_type = SageBuilder::buildPointerType(pointer_type);
     }
 }
 var -> set_type(pointer_type);
}
astJavaComponentStack.push(lhs);
astJavaComponentStack.push(rhs);
*/
    binaryExpressionSupport<SgPntrArrRefExp>();
}


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReferenceClassScope(JNIEnv *, jclass, jstring, jobject jToken) {
    ROSE_ASSERT( ! "yet implemented");
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
        case OR_OR:   ROSE_ASSERT(false); break;
        case AND_AND: ROSE_ASSERT(false); break;

        default:
            printf ("Error: default reached in cactionBinaryExpressionEnd() operator_kind = %d \n", operator_kind);
            ROSE_ASSERT(false);
    }

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionBlock(JNIEnv *env, jclass, jobject jToken) {
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
            cerr << "(7) Adding statement "
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

    SgCastExp *castExp = SageBuilder::buildCastExp(expression, castType);
    ROSE_ASSERT(castExp != NULL);

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
    ROSE_ASSERT( ! "yet implemented");
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
            printf ("Error: default reached in cactionCompoundAssignmentEnd() operator_kind = %d \n", operator_kind);
            ROSE_ASSERT(false);
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

    SgNullStatement *testStatement = SageBuilder::buildNullStatement();
    SgNullStatement *bodyStatement = SageBuilder::buildNullStatement();

    // It might be that we should build this on the way down so that we can have it on the stack 
    // before the body would be pushed onto the scope stack if a block is used.
    // SgForStatement *forStatement = SageBuilder::buildForStatement(assignmentStatement, testStatement, incrementExpression, bodyStatement);
    SgDoWhileStmt *doWhileStatement = SageBuilder::buildDoWhileStmt(bodyStatement, testStatement);
    ROSE_ASSERT(doWhileStatement != NULL);

    // DQ (7/30/2011): For the build interface to work we have to initialize the parent pointer to the SgForStatement.
    doWhileStatement -> set_parent(astJavaScopeStack.top());

    astJavaScopeStack.push(doWhileStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionDoStatementEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionDoStatementEnd() \n");

    SgExpression *testExpression = astJavaComponentStack.popExpression();

    SgExprStatement *testStatement = SageBuilder::buildExprStatement(testExpression);

    setJavaSourcePosition(testExpression, env, jToken);
    setJavaSourcePosition(testStatement, env, jToken);

    // If we DO put all body's onto the statement stack then we process it this way.
    SgStatement *bodyStatement = astJavaComponentStack.popStatement();

    SgDoWhileStmt *originalDoWhileStatement = astJavaScopeStack.popDoWhileStmt();

    originalDoWhileStatement -> set_body(bodyStatement);
    bodyStatement -> set_parent(originalDoWhileStatement);
    ROSE_ASSERT(bodyStatement -> get_parent() == originalDoWhileStatement);

    originalDoWhileStatement -> set_condition(testStatement);
    testStatement -> set_parent(originalDoWhileStatement);
    ROSE_ASSERT(testStatement -> get_parent() == originalDoWhileStatement);

    setJavaSourcePosition(originalDoWhileStatement, env, jToken);

    astJavaComponentStack.push(originalDoWhileStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionDoubleLiteral(JNIEnv *env, jclass, jdouble java_value, jstring java_source, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a DoubleVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    double value = java_value;
    SgName source = convertJavaStringToCxxString(env, java_source);

    SgDoubleVal *doubleValue = new SgDoubleVal(value, source);
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
            printf ("Error: default reached in cactionEqualExpressionEnd() operator_kind = %d \n", operator_kind);
            ROSE_ASSERT(false);
    }

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionExtendedStringLiteral(JNIEnv *env, jclass, jstring java_string, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build an ExtendedStringVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgName stringLiteral = convertJavaStringToCxxString(env, java_string);

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
                                                                  jstring variableName,
                                                                  jboolean hasInitializer,
                                                                  jboolean java_is_final,
                                                                  jboolean java_is_private,
                                                                  jboolean java_is_protected,
                                                                  jboolean java_is_public, 
                                                                  jboolean java_is_volatile,
                                                                  jboolean java_is_synthetic,
                                                                  jboolean java_is_static,
                                                                  jboolean java_is_transient,
                                                                  jobject jToken) {
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
        printf ("Building a variable declaration for name = %s \n", name.str());

    SgExpression *initializer_expression = (hasInitializer ? astJavaComponentStack.popExpression() : NULL);

    SgType *type = astJavaComponentStack.popType();

// TODO: Remove this
    // Note that the type should have already been built and should be on the astJavaComponentStack.
    /*
    SgVariableDeclaration *variableDeclaration = buildSimpleVariableDeclaration(name, type);

    // DQ (8/13/2011): Added test for scope
    ROSE_ASSERT(variableDeclaration -> get_scope() != NULL);

    // DQ (8/13/2011): This is a test to debug failing test in resetParentPointers.C:1733
    ROSE_ASSERT(! SageInterface::is_Fortran_language());
    SgInitializedName *initializedName = variableDeclaration -> get_decl_item(name);
    ROSE_ASSERT(initializedName != NULL);
    ROSE_ASSERT(initializedName -> get_scope() != NULL);
    */

    SgScopeStatement *outer_scope = astJavaScopeStack.top();
    ROSE_ASSERT(outer_scope);
    SgVariableSymbol *symbol = outer_scope -> lookup_variable_symbol(name);
    ROSE_ASSERT(symbol);
    SgInitializedName *initializedName = symbol -> get_declaration();
    ROSE_ASSERT(initializedName);
    SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(initializedName -> get_declaration());
    ROSE_ASSERT(variableDeclaration);

    setJavaSourcePosition(initializedName, env, jToken);
    setJavaSourcePosition(variableDeclaration, env, jToken);

    // By default, the access modifier is set to unknown
    variableDeclaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    // Set the modifiers (shared between PHP and Java)

    if (isFinal)
        variableDeclaration -> get_declarationModifier().setFinal();

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
    if (hasInitializer) {
        SgInitializer *initializer = SageBuilder::buildAssignInitializer(initializer_expression);
        ROSE_ASSERT(initializer != NULL);

        setJavaSourcePosition(initializer_expression, env, jToken);
        setJavaSourcePosition(initializer, env, jToken);

        initializer_expression -> set_parent(initializer);

        // printf ("In cactionFieldDeclarationEnd(): initializer = %p = %s \n", initializer, initializer -> class_name().c_str());
        initializer -> get_file_info() -> display("cactionFieldDeclarationEnd()");

        initializedName -> set_initptr(initializer);
        initializer -> set_parent(initializedName);
    }

// TODO: REMOVE THIS! WE NO LONGER NEED TO PUSH  THIS VARIABLE AS THE PRE-PASS ALREADY TOOK CARE OF THAT.
    // Save it on the stack so that we can add SgInitializedNames to it.
//    astJavaComponentStack.push(variableDeclaration);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReference(JNIEnv *env, jclass, jstring java_field, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldReference() \n");

    // Nothing to do !!!

}


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReferenceEnd(JNIEnv *env, jclass, jstring java_field, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldReference() \n");

    SgName field_name = convertJavaStringToCxxString(env, java_field);

    if (SgProject::get_verbose() > 0)
        printf ("Building a variable reference for name = %s \n", field_name.str());

    SgExpression *receiver = astJavaComponentStack.popExpression();
    SgExpression *result;
    SgType *type = receiver -> get_type();
    if (isSgPointerType(type)) { // PC: This is NOT C++ !!!
        SgClassType *class_type = isSgClassType(isSgPointerType(type) -> get_base_type());
        ROSE_ASSERT(class_type);
        SgClassDeclaration *declaration = isSgClassDeclaration(class_type -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(declaration);
        ROSE_ASSERT(declaration -> get_definition());
        SgVariableSymbol *variable_symbol = lookupSimpleNameVariableInClassScope(field_name, declaration -> get_definition());
        ROSE_ASSERT(variable_symbol);
        SgVarRefExp *field = SageBuilder::buildVarRefExp(variable_symbol);
        ROSE_ASSERT(field != NULL);
        setJavaSourcePosition(field, env, jToken);
        result = SageBuilder::buildBinaryExpression<SgDotExp>(receiver, field);
    }
    else {
        SgClassType *class_type = isSgClassType(receiver -> get_type());
        ROSE_ASSERT(class_type);
        SgClassDeclaration *declaration = isSgClassDeclaration(class_type -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(declaration);
        ROSE_ASSERT(declaration -> get_definition());
        SgVariableSymbol *variable_symbol = lookupSimpleNameVariableInClassScope(field_name, declaration -> get_definition());
        ROSE_ASSERT(variable_symbol);
        SgVarRefExp *field = SageBuilder::buildVarRefExp(variable_symbol);
        ROSE_ASSERT(field != NULL);
        setJavaSourcePosition(field, env, jToken);
        result = SageBuilder::buildBinaryExpression<SgDotExp>(receiver, field);
    }

    astJavaComponentStack.push(result);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReferenceClassScope(JNIEnv *env, jclass, jstring java_field, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldReferenceClassScope() \n");

    // Nothing to do !!!

}


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReferenceClassScopeEnd(JNIEnv *env, jclass, jstring java_field, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionFieldReferenceClassScope() \n");
 
    SgName field_name = convertJavaStringToCxxString(env, java_field);

    ROSE_ASSERT( ! "yet implemented");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionFloatLiteral(JNIEnv *env, jclass, jfloat java_value, jstring java_source, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build a FloatVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    float value = java_value;
    SgName source = convertJavaStringToCxxString(env, java_source);

    SgFloatVal *floatValue = new SgFloatVal(value, source);
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

    // Get the action statement
    SgStatement *action = astJavaComponentStack.popStatement();

    // Get the collection expr
    SgExpression *collection = astJavaComponentStack.popExpression();

    // Get the declaration statement
    SgVariableDeclaration *variable_declaration = (SgVariableDeclaration *) astJavaComponentStack.popStatement();

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

    SgNullStatement *assignmentStatement  = SageBuilder::buildNullStatement();
    SgNullStatement *testStatement        = SageBuilder::buildNullStatement();
    SgNullExpression *incrementExpression = SageBuilder::buildNullExpression();
    SgNullStatement *bodyStatement        = SageBuilder::buildNullStatement();

    // The SageBuilder::buildForStatement() function works better if we provide a proper SgForInitStatement
    // Else the original SgForInitStatement built by the SgForStatement constructor will be left dangling...
    // and this causes an error in the AST post processing and testing.
    SgStatementPtrList statements;
    statements.push_back(assignmentStatement);
    SgForInitStatement *forInitStatement = SageBuilder::buildForInitStatement_nfi(statements);
    ROSE_ASSERT(forInitStatement != NULL);

    // We need to set the source code position information
    SageInterface::setOneSourcePositionForTransformation(forInitStatement);
    ROSE_ASSERT(forInitStatement -> get_startOfConstruct() != NULL);
    // printf ("forInitStatement = %p \n", forInitStatement);

    // It might be that we should build this on the way down so that we can have it on the stack 
    // before the body would be pushed onto the scope stack if a block is used.
    // SgForStatement *forStatement = SageBuilder::buildForStatement(assignmentStatement, testStatement, incrementExpression, bodyStatement);
    SgForStatement *forStatement = SageBuilder::buildForStatement(forInitStatement, testStatement, incrementExpression, bodyStatement);
    ROSE_ASSERT(forStatement != NULL);

    ROSE_ASSERT(forInitStatement -> get_startOfConstruct() != NULL);

    // printf ("forStatement -> get_for_init_stmt() = %p \n", forStatement -> get_for_init_stmt());

    // DQ (7/30/2011): For the build interface to wrk we have to initialize the parent pointer to the SgForStatement.
    forStatement -> set_parent(astJavaScopeStack.top());

    astJavaScopeStack.push(forStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionForStatementEnd(JNIEnv *env, jclass, jint num_initializations, jboolean has_condition, jint num_increments, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionForStatementEnd() \n");

    // If we DO put all body's onto the statement stack then we process it this way.
    SgStatement *bodyStatement = astJavaComponentStack.popStatement();

    // charles4 10/14/2011: If there are more than 1 increment statements, merge them into a
    // single expression; a hierarchy of SgCommaOpExp.
    for (int i = 1; i < num_increments; i++) {
        binaryExpressionSupport<SgCommaOpExp>();
    }
    SgExpression *incrementExpression = (num_increments > 0 ? astJavaComponentStack.popExpression() : SageBuilder::buildNullExpression());

    SgExpression *testExpression = (has_condition ? astJavaComponentStack.popExpression() : SageBuilder::buildNullExpression());

    // The ROSE IR is based on C which allows a statment for the test. Not clear if Java is the same.
    SgStatement *testStatement = SageBuilder::buildExprStatement(testExpression);
    testExpression -> set_parent(testStatement);

    // It might be that we should build this on the way down so that we can have it on the stack 
    // before the body would be pushed onto the scope stack if a block is used.
    SgForStatement *originalForStatement = astJavaScopeStack.popForStatement();

    ROSE_ASSERT(originalForStatement != NULL);
    // printf ("originalForStatement -> get_for_init_stmt() = %p \n", originalForStatement -> get_for_init_stmt());
    delete originalForStatement -> get_for_init_stmt();
    // originalForStatement -> set_for_init_stmt(NULL);

    delete originalForStatement;

    // The SageBuilder::buildForStatement() function works better if we provide a proper SgForInitStatement
    // Else the original SgForInitStatement built by the SgForStatement constructor will be left dangling...
    // and this causes an error in the AST post processing and testing.

    SgForInitStatement *forInitStatement = SageBuilder::buildForInitStatement();
    ROSE_ASSERT(forInitStatement != NULL);
    // charles4 10/14/2011: A For statement may contain a list of 0 or more initializations.
    for (int i = 0; i < num_initializations; i++) {
        forInitStatement -> prepend_init_stmt(astJavaComponentStack.popStatement());
    }

    // We need to set the source code position information
    SageInterface::setOneSourcePositionForTransformation(forInitStatement);
    ROSE_ASSERT(forInitStatement -> get_startOfConstruct() != NULL);
    // printf ("forInitStatement = %p \n", forInitStatement);

    // SgForStatement *forStatement = SageBuilder::buildForStatement(assignmentStatement, testStatement, incrementExpression, bodyStatement);
    SgForStatement *forStatement = SageBuilder::buildForStatement(forInitStatement, testStatement, incrementExpression, bodyStatement);
    // delete forStatement -> get_for_init_stmt();
    // forStatement -> set_for_init_stmt();
    // printf ("forStatement -> get_for_init_stmt() = %p \n", forStatement -> get_for_init_stmt());

    ROSE_ASSERT(forStatement != NULL);
    astJavaComponentStack.push(forStatement);

    ROSE_ASSERT(testExpression -> get_parent() != NULL);
    ROSE_ASSERT(testStatement -> get_parent() != NULL);

    ROSE_ASSERT(incrementExpression -> get_parent() != NULL);
    ROSE_ASSERT(incrementExpression -> get_startOfConstruct() != NULL);

    // printf ("forStatement -> get_for_init_stmt() = %p \n", forStatement -> get_for_init_stmt());
    // printf ("forStatement -> get_for_init_stmt() = %p \n", forStatement -> get_for_init_stmt());
    ROSE_ASSERT(forStatement -> get_for_init_stmt() != NULL);
    ROSE_ASSERT(forStatement -> get_for_init_stmt() -> get_parent() != NULL);
    ROSE_ASSERT(forStatement -> get_for_init_stmt() -> get_startOfConstruct() != NULL);
    // printf ("forStatement -> get_for_init_stmt() = %p \n", forStatement -> get_for_init_stmt());

    ROSE_ASSERT(forStatement -> get_startOfConstruct() != NULL);

    ROSE_ASSERT(forStatement -> get_parent() == NULL);

    // SgForInitStatement *forInitStatement = forStatement -> get_for_init_stmt();
    ROSE_ASSERT(forInitStatement != NULL);
    ROSE_ASSERT(forInitStatement -> get_startOfConstruct() != NULL);
    ROSE_ASSERT(forInitStatement -> get_endOfConstruct() != NULL);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionIfStatement(JNIEnv *env, jclass, jobject jToken) {
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
}

JNIEXPORT void JNICALL Java_JavaParser_cactionIfStatementEnd(JNIEnv *env, jclass, jboolean has_false_body, jobject jToken) {
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
}


JNIEXPORT void JNICALL Java_JavaParser_cactionImportReference(JNIEnv *env, jclass, jboolean java_is_static, jstring java_package_name, jstring java_type_name, jboolean java_contains_wildcard, jobject jToken) {
    // This is the import statement.  The semantics is to include the named file and add its 
    // declarations to the global scope so that they can be referenced by the current file.
    // The import directive tells the compiler where to look for the class definitions 
    // when it comes upon a class that it cannot find in the default java.lang package.

    if (SgProject::get_verbose() > 1)
        printf ("Inside of Java_JavaParser_cactionImportReference() \n");

    bool is_static = java_is_static;

    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name),
           qualifiedName = convertJavaStringToCxxString(env, java_package_name) + string(package_name.getString().size() && type_name.getString().size() ? "." : "") + type_name.getString();

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
    if (is_static) {
        importStatement -> get_declarationModifier().get_storageModifier().setStatic();
    }


    ROSE_ASSERT(! astJavaScopeStack.empty());

    // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
    // printf ("Previously calling appendStatement in cactionImportReference() \n");
    // appendStatement(importStatement);
    astJavaComponentStack.push(importStatement);

    // We also have to set the parent so that the stack debugging output will work.
    importStatement -> set_parent(astJavaScopeStack.top());

    setJavaSourcePosition(importStatement, env, jToken);

    if (type_name.getString().size() > 0) { // only a package was specified?
        SgType *type =  lookupTypeByName(package_name, type_name, 0);

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
        SgSymbol *importClassSymbol = type -> getAssociatedDeclaration() -> search_for_symbol_from_symbol_table();

        ROSE_ASSERT(importClassSymbol != NULL);

        if (contains_wildcard) {
            // This processing requires that we inject alias symbols from the reference class for all of its data members and member functions. 

// TODO: PC: why is this necessary?
//            printf ("WARNING: The use of wildcards in import statements requires additional symbol table support so that all of the specified members of a class can be incerted into the current scope. \n");

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
        printf ("Leaving Java_JavaParser_cactionImportReference() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInitializer(JNIEnv *env, jclass, jboolean java_is_static, jobject jToken) {
    bool isStatic = java_is_static;

    //
    // The initializer block has no return type as it is not a real method. So, we
    // start by pushing a VOID return type to make it look like a method.
    //
    astJavaComponentStack.push(SgTypeVoid::createType()); 

    SgClassDefinition *classDefinition = isSgClassDefinition(astJavaScopeStack.top());
    ROSE_ASSERT(classDefinition != NULL  && (! classDefinition -> attributeExists("namespace")));

    //
    // NOTE that we left the arguments and the return type on the Component stack.  They will be processed
    // by this function call. 
    //
    ostringstream convert;   // stream used for the conversion
    convert << (initializerCount++);

    SgName name = convert.str() + "block";
    SgMemberFunctionDeclaration *functionDeclaration = buildDefiningMemberFunction(name, classDefinition, 0 /* no arguments */ );
    ROSE_ASSERT(functionDeclaration != NULL);

    setJavaSourcePosition(functionDeclaration, env, jToken);

    // This is not a defining function declaration so we can't identify the SgFunctionDefinition and push it's body onto the astJavaScopeStack.
    SgFunctionDefinition *functionDefinition = functionDeclaration -> get_definition();
    ROSE_ASSERT(functionDefinition != NULL);

    setJavaSourcePosition(functionDefinition, env, jToken);

    astJavaScopeStack.push(functionDefinition);
    ROSE_ASSERT(astJavaScopeStack.top() -> get_parent() != NULL);

    // Set the specific modifier, this modifier is common to C/C++.
    functionDeclaration -> get_functionModifier().setJavaInitializer();
    functionDeclaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);
    if (isStatic) {
        functionDeclaration -> get_declarationModifier().get_storageModifier().setStatic();
    }

    astJavaComponentStack.push(functionDeclaration);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInitializerEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("End of SgMemberFunctionDeclaration (method) \n");

    ROSE_ASSERT(isSgBasicBlock(astJavaComponentStack.top()));
    SgBasicBlock *initializer_body = (SgBasicBlock *) astJavaComponentStack.popStatement();

    SgFunctionDefinition *memberFunctionDefinition = astJavaScopeStack.popFunctionDefinition();
    memberFunctionDefinition -> set_body(initializer_body);

    ROSE_ASSERT(isSgMemberFunctionDeclaration(astJavaComponentStack.top()));
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInstanceOfExpression(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInstanceOfExpression() \n");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionInstanceOfExpressionEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionInstanceOfExpressionEnd() \n");

    // The generation of this type is not yet supported.
    SgType *type = astJavaComponentStack.popType();

    SgExpression *exp = astJavaComponentStack.popExpression();

    SgExpression *result = SageBuilder::buildJavaInstanceOfOp(exp, type);

    astJavaComponentStack.push(result);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionIntLiteral(JNIEnv *env, jclass, jint java_value, jstring java_source, jobject jToken) {
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
}


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


JNIEXPORT void JNICALL Java_JavaParser_cactionLocalDeclaration(JNIEnv *env, jclass, jstring variableName, jboolean java_is_final, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionLocalDeclaration() \n");

     // Nothing to do !!!

}


JNIEXPORT void JNICALL Java_JavaParser_cactionLocalDeclarationEnd(JNIEnv *env, jclass, jstring variableName, jboolean hasInitializer, jboolean java_is_final, jobject jToken) {
    // DQ (9/5/2011): This function is added as part of a move to process local declarations bottom up.

    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionLocalDeclarationEnd() \n");

    SgName name = convertJavaStringToCxxString(env, variableName);
    bool isFinal = java_is_final;

    if (SgProject::get_verbose() > 2)
        printf ("Building a variable declaration for name = %s \n", name.str());

    SgExpression *initializer_expression = (hasInitializer ? astJavaComponentStack.popExpression() : NULL);

    SgType *type = astJavaComponentStack.popType();

    // Note that the type should have already been built and should be on the astJavaComponentStack.
    SgVariableDeclaration *variableDeclaration = buildSimpleVariableDeclaration(name, type);
    ROSE_ASSERT(variableDeclaration != NULL);

    setJavaSourcePosition(variableDeclaration, env, jToken);

    // DQ (7/16/2011): Added test for scope
    ROSE_ASSERT(variableDeclaration -> get_scope() != NULL);

    // DQ (7/16/2011): This is a test to debug failing test in resetParentPointers.C:1733
    ROSE_ASSERT(! SageInterface::is_Fortran_language());
    SgInitializedName *initializedName = variableDeclaration -> get_decl_item (name);
    ROSE_ASSERT(initializedName != NULL);
    ROSE_ASSERT(initializedName -> get_scope() != NULL);

    setJavaSourcePosition(initializedName, env, jToken);
    setJavaSourcePosition(variableDeclaration, env, jToken);

    if (hasInitializer) {
        SgAssignInitializer *initializer = SageBuilder::buildAssignInitializer(initializer_expression, initializedName -> get_type());
        ROSE_ASSERT(initializer != NULL);

        setJavaSourcePosition(initializer, env, jToken);
        setJavaSourcePosition(initializer_expression, env, jToken);

        initializer_expression -> set_parent(initializer);
        initializedName -> set_initptr(initializer);
        initializer -> set_parent(initializedName);

        ROSE_ASSERT(initializer -> get_parent() != NULL);
        ROSE_ASSERT(initializer -> get_parent() == initializedName);
    }

    // We don't want to add the statement to the current scope until it is finished being built.
    // Set the modifiers (shared between PHP and Java)
    if (isFinal) {
        variableDeclaration -> get_declarationModifier().setFinal();
    }

    // Local declarations have no definition for this (default is public so we need to explicity mark this as unknown).
    variableDeclaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    // DQ (8/21/2011): Debugging declarations in local function should (should not be marked as public).
    ROSE_ASSERT(! variableDeclaration -> get_declarationModifier().get_accessModifier().isPublic());

    // Save it on the stack so that we can add SgInitializedNames to it.
    astJavaComponentStack.push(variableDeclaration);

    if (SgProject::get_verbose() > 0)
        variableDeclaration -> get_file_info() -> display("source position in Java_JavaParser_cactionLocalDeclarationEnd(): debug");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionLongLiteral(JNIEnv *env, jclass, jlong java_value, jstring java_source, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Build LongVal \n");

    ROSE_ASSERT(! astJavaScopeStack.empty());

    long value = java_value;
    SgName source = convertJavaStringToCxxString(env, java_source);

    // printf ("Building an integer value expression = %d = %s \n", value, valueString.c_str());

    SgLongIntVal *longValue = new SgLongIntVal(value, source);
    ROSE_ASSERT(longValue != NULL);

    setJavaSourcePosition(longValue, env, jToken);

    astJavaComponentStack.push(longValue);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMarkerAnnotation(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionMemberValuePair(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteralConcatenation(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet support string concatenation operation");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionNormalAnnotation(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
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


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedQualifiedTypeReference(JNIEnv *env, jclass, jobject jToken) {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionParameterizedQualifiedTypeReference() \n");

    ROSE_ASSERT(! "YET IMPLEMENTED");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedQualifiedTypeReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionParameterizedQualifiedTypeReferenceClassScope() \n");

    ROSE_ASSERT(! "YET IMPLEMENTED");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedSingleTypeReference(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionParameterizedSingleTypeReference() \n");

    // Nothing to do here !!!
}


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedSingleTypeReferenceEnd(JNIEnv *env, jclass, jstring java_name, int java_numberOfTypeArguments, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionParameterizedSingleTypeReferenceEnd() \n");

    SgName name = convertJavaStringToCxxString(env, java_name);

    int numberOfTypeArguments = java_numberOfTypeArguments;

    // printf ("Inside of Java_JavaParser_cactionParameterizedSingleTypeReferenceEnd(): numberOfTypeArguments   = %d  \n", numberOfTypeArguments);

    // Need to find the type with the associated name.
    printf ("Looking for parameterized type name = %s with %d type arguments\n", name.str(), numberOfTypeArguments);

    SgClassSymbol *parameterizedClassSymbol = lookupSymbolFromQualifiedName(name);
    ROSE_ASSERT(parameterizedClassSymbol != NULL);

    // This will be the Java raw type, the 
    SgType *rawParameterizedType = parameterizedClassSymbol -> get_type();
    ROSE_ASSERT(rawParameterizedType != NULL);

    // DQ (9/3/2011): This should be constructed into a SageBuilder function.
    // TODO: Add SageBuilder function for this support.

    SgClassDeclaration *classDeclaration = isSgClassDeclaration(parameterizedClassSymbol -> get_declaration());
    ROSE_ASSERT(classDeclaration != NULL);

    SgTemplateParameterList *typeParameterList = new SgTemplateParameterList();
    ROSE_ASSERT(typeParameterList != NULL);

    // SgType *parameterizedType = SgJavaParameterizedType::createType();
    SgJavaParameterizedType *parameterizedType = new SgJavaParameterizedType(classDeclaration, rawParameterizedType, typeParameterList);

    ROSE_ASSERT(parameterizedType != NULL);
    ROSE_ASSERT(parameterizedType -> get_raw_type() != NULL);
    ROSE_ASSERT(parameterizedType -> get_type_list() != NULL);

    SgTemplateParameterPtrList typeList;
    for (int i = 0; i < numberOfTypeArguments; i++) {
        SgType *typeArgument = astJavaComponentStack.popType();

        // Ignore the default type for now (if it exists in Java)
        SgTemplateParameter *templateParameter = new SgTemplateParameter(typeArgument, NULL);
        ROSE_ASSERT(templateParameter != NULL);

        typeList.push_back(templateParameter);
    }

    // DQ (9/3/2011): This is pass by value so we have to set the typeList before using it to initialize the list in typeParameterList.
    // I think this should be fixed in ROSE to be pass by reference so that this is less error prone.
    // TODO: Fix IR node for ROSE to support references (or more intuative semantics).
    typeParameterList -> set_args(typeList);
    ROSE_ASSERT(! typeParameterList -> get_args().empty());

    // Push this onto the type stack.
    astJavaComponentStack.push(parameterizedType);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedSingleTypeReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_JavaParser_cactionParameterizedSingleTypeReferenceClassScope() \n");

    ROSE_ASSERT(! "YET IMPLEMENTED");
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
            printf ("Error: default reached in cactionPostfixExpressionEnd() operator_kind = %d \n", operator_kind);
            ROSE_ASSERT(false);
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
            printf ("Error: default reached in cactionPrefixExpressionEnd() operator_kind = %d \n", operator_kind);
            ROSE_ASSERT(false);
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


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReference(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedThisReference(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedThisReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedTypeReference(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedTypeReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionReturnStatement(JNIEnv *env, jclass, jobject jToken) {
    // Nothing to do !!!
}

JNIEXPORT void JNICALL Java_JavaParser_cactionReturnStatementEnd(JNIEnv *env, jclass, jboolean hasExpression, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionReturnStatementEnd() \n");

    // Build the Return Statement
    SgExpression *expression = (hasExpression ? astJavaComponentStack.popExpression() : NULL);
    SgReturnStmt *returnStatement = SageBuilder::buildReturnStmt(expression);
    setJavaSourcePosition(returnStatement, env, jToken);

    // Pushing 'return' on the statement stack
    astJavaComponentStack.push(returnStatement);
}

JNIEXPORT void JNICALL Java_JavaParser_cactionSingleMemberAnnotation(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleNameReference(JNIEnv *env, jclass, jstring java_package_name, jstring java_type_name, jstring java_name, jobject jToken) {
    SgName package_name = convertJavaPackageNameToCxxString(env, java_package_name),
           type_name = convertJavaStringToCxxString(env, java_type_name),
           name = convertJavaStringToCxxString(env, java_name);

    SgVariableSymbol *variable_symbol = NULL;
    if (! type_name.getString().empty()) { // an instance variable?
        if (SgProject::get_verbose() > 0)
            printf ("Building a variable reference for name = %s%s%s \n", (package_name.getString().empty() ? "" : (package_name.getString() + ".")).c_str(), (type_name.getString() + ".").c_str(), name.str());

        SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
        ROSE_ASSERT(type);
        SgClassType *class_type = isSgClassType(type);
        ROSE_ASSERT(class_type);
        SgClassDeclaration *declaration = isSgClassDeclaration(class_type -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(declaration);
        ROSE_ASSERT(declaration -> get_definition());
        variable_symbol = lookupSimpleNameVariableInClassScope(name, declaration -> get_definition());
    }
    else { // a local variable!
        if (SgProject::get_verbose() > 0)
            printf ("Building a variable reference for name = %s \n", name.str());

        variable_symbol = lookupVariableByName(name);
    }

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


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleNameReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionSuperReference(JNIEnv *env, jclass, jobject jToken) {
    SgClassDefinition *classDefinition = getCurrentClassDefinition();
    ROSE_ASSERT(classDefinition -> get_declaration() && (! classDefinition -> attributeExists("namespace")));

    vector<SgBaseClass *> &inheritances = classDefinition -> get_inheritances();
    ROSE_ASSERT(inheritances.size() > 0);
    SgClassDeclaration *super_declaration = inheritances[0] -> get_base_class();
    ROSE_ASSERT(! super_declaration -> get_explicit_interface()); // this class must have a super class
    string className = super_declaration -> get_name();

    classDefinition = super_declaration -> get_definition(); // get the super class definition

    // SgClassSymbol *classSymbol = classDefinition -> get_declaration() -> get_symbol();
    SgClassSymbol *classSymbol = isSgClassSymbol(classDefinition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(classSymbol != NULL);

    SgSuperExp *superExp = SageBuilder::buildSuperExp(classSymbol);
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
        case_block -> set_parent(switch_block);

        SgStatement *sg_stmt = astJavaComponentStack.popStatement();
        while (! (isSgCaseOptionStmt(sg_stmt) || isSgDefaultOptionStmt(sg_stmt))) {
            case_block -> prepend_statement(sg_stmt);
            sg_stmt = astJavaComponentStack.popStatement();
        }

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
    SgClassDefinition *classDefinition = getCurrentClassDefinition();
    ROSE_ASSERT(! classDefinition -> attributeExists("namespace"));

    string className = classDefinition -> get_declaration() -> get_name();
    // printf ("Current class for ThisReference is: %s \n", className.c_str());

    // SgClassSymbol *classSymbol = classDefinition -> get_declaration() -> get_symbol();
    SgClassSymbol *classSymbol = isSgClassSymbol(classDefinition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(classSymbol != NULL);

    SgThisExp *thisExp = SageBuilder::buildThisExp(classSymbol);
    ROSE_ASSERT(thisExp != NULL);

    astJavaComponentStack.push(thisExp);
}


// TODO: Same as function above... Merge it !
JNIEXPORT void JNICALL Java_JavaParser_cactionThisReferenceClassScope(JNIEnv *env, jclass, jobject jToken) {
    SgClassDefinition *classDefinition = getCurrentClassDefinition();
    ROSE_ASSERT(! classDefinition -> attributeExists("namespace"));

    string className = classDefinition -> get_declaration() -> get_name();
    // printf ("Current class for ThisReference is: %s \n", className.c_str());

    // SgClassSymbol *classSymbol = classDefinition -> get_declaration() -> get_symbol();
    SgClassSymbol *classSymbol = isSgClassSymbol(classDefinition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(classSymbol != NULL);

    SgThisExp *thisExp = SageBuilder::buildThisExp(classSymbol);
    ROSE_ASSERT(thisExp != NULL);

    astJavaComponentStack.push(thisExp);
}


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


JNIEXPORT void JNICALL Java_JavaParser_cactionTryStatementEnd(JNIEnv *env, jclass, jint numCatchBlocks, jboolean hasFinallyBlock, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionTryStatement() \n");
     
    SgBasicBlock *finally_body = (SgBasicBlock *) (hasFinallyBlock ? astJavaComponentStack.popStatement() : NULL);
    ROSE_ASSERT(finally_body == NULL || isSgBasicBlock(finally_body));

    list<SgCatchOptionStmt *> catches;
    for (int i = 0; i < numCatchBlocks; i++) {
        SgCatchOptionStmt *catch_option_stmt = (SgCatchOptionStmt *) astJavaComponentStack.popStatement();
        ROSE_ASSERT(isSgCatchOptionStmt(catch_option_stmt));
        catches.push_front(catch_option_stmt);
    }

    SgBasicBlock *try_body = (SgBasicBlock *) astJavaComponentStack.popStatement();
    ROSE_ASSERT(isSgBasicBlock(try_body));

    SgTryStmt *try_statement = SageBuilder::buildTryStmt(try_body, finally_body);
    setJavaSourcePosition(try_statement, env, jToken);
    try_statement -> set_parent(astJavaScopeStack.top());

    //
    // charles4 09/23/2011 - Wwhen an SgTryStmt is allocated, its constructor
    // preallocates a SgCatchStementSeq (See comment in SageBuilder::buildTryStmt(...))
    // for the field p_catch_statement_sequence_root. The position of that field cannot
    // be set during the allocation as it was not yet set for the SgTryStmt being allocated.
    // Thus, we have to set the location here!
    //
    SgCatchStatementSeq *catch_statement_sequence = try_statement -> get_catch_statement_seq_root();
    setJavaSourcePosition(catch_statement_sequence, env, jToken);

    ROSE_ASSERT(catches.size() == (unsigned) numCatchBlocks);
    for (int i = 0; i < numCatchBlocks; i++) {
        SgCatchOptionStmt *catch_option_stmt = catches.front();
        catches.pop_front();
        ROSE_ASSERT(catch_option_stmt);
        try_statement -> append_catch_statement(catch_option_stmt);
    }

    // Pushing 'try' on the statement stack
    astJavaComponentStack.push(try_statement);
}



JNIEXPORT void JNICALL Java_JavaParser_cactionTypeParameter(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeParameterClassScope(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented Type Parameters");
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
            printf ("Error: default reached in cactionUnaryExpressionEnd() operator_kind = %d \n", operator_kind);
            ROSE_ASSERT(false);
    }

    setJavaSourcePosition((SgLocatedNode *) astJavaComponentStack.top(), env, jToken);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionWhileStatement(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionWhileStatement() \n");

    SgNullStatement *testStatement = SageBuilder::buildNullStatement();
    SgNullStatement *bodyStatement = SageBuilder::buildNullStatement();

    // It might be that we should build this on the way down so that we can have it on the stack 
    // before the body would be pushed onto the scope stack if a block is used.
    // SgForStatement *forStatement = SageBuilder::buildForStatement(assignmentStatement, testStatement, incrementExpression, bodyStatement);
    SgWhileStmt *whileStatement = SageBuilder::buildWhileStmt(testStatement, bodyStatement, NULL);
    ROSE_ASSERT(whileStatement != NULL);

    // DQ (7/30/2011): For the build interface to work we have to initialize the parent pointer to the SgForStatement.
    whileStatement -> set_parent(astJavaScopeStack.top());

    astJavaScopeStack.push(whileStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionWhileStatementEnd(JNIEnv *env, jclass, jobject jToken) {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionWhileStatementEnd() \n");

    // If we DO put all body's onto the statement stack then we process it this way.
    SgStatement *bodyStatement = astJavaComponentStack.popStatement();

    SgExpression *testExpression = astJavaComponentStack.popExpression();

    SgExprStatement *testStatement = SageBuilder::buildExprStatement(testExpression);

    SgWhileStmt *originalWhileStatement = astJavaScopeStack.popWhileStmt();
    ROSE_ASSERT(originalWhileStatement != NULL);

    originalWhileStatement -> set_body(bodyStatement);
    bodyStatement -> set_parent(originalWhileStatement);
    ROSE_ASSERT(bodyStatement -> get_parent() == originalWhileStatement);

    originalWhileStatement -> set_condition(testStatement);
    testStatement -> set_parent(originalWhileStatement);
    ROSE_ASSERT(testStatement -> get_parent() == originalWhileStatement);

    setJavaSourcePosition(originalWhileStatement, env, jToken);

    astJavaComponentStack.push(originalWhileStatement);
}


JNIEXPORT void JNICALL Java_JavaParser_cactionWildcard(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented");
}


JNIEXPORT void JNICALL Java_JavaParser_cactionWildcardClassScope(JNIEnv *env, jclass, jobject jToken) {
    ROSE_ASSERT(! "yet implemented");
}

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
#include "Parser.h"
#include "x10Parser.h"

#include "x10.h"

// Support functions so that this file can be restricted to be just parser (AST traversal) rules.
#include "x10_support.h"
#include "jni_x10_utils.h"
#include "jni_x10SourceCodePosition.h"

// #include "VisitorContext.h"

// This is needed so that we can call the FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope() function.
#include "fixupCxxSymbolTablesToSupportAliasingSymbols.h"

using namespace std;
using namespace Rose::Frontend::X10;


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


SgType *
createDummyClass(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jobject x10Token) {
        SgName type_name = convertJavaStringToCxxString(env, x10_type_name),
                        package_name = convertJavaStringToCxxString(env, x10_package_name);
    if (SgProject::get_verbose() > 0)
        printf ("Inside of createDummyClass(): = %s \n", type_name.str());

//    SgScopeStatement *outerScope = astX10ScopeStack.top();
//    ROSE_ASSERT(outerScope != NULL);

//    SgClassDeclaration *class_declaration = buildDefiningClassDeclaration(type_name, outerScope);
        // dummy class is created with ::globalScope because we cannot figure out which package this class is defined
    SgClassDeclaration *class_declaration = buildDefiningClassDeclaration(type_name, ::globalScope);
//MH-20140314
#if 1
// MH-20140822
    SgClassDefinition *class_definition = class_declaration -> get_definition();
//    astX10ScopeStack.push(class_definition); // to contain the class members...

    SgClassType *unknown = SgClassType::createType(class_declaration, NULL);
//    astX10ComponentStack.push(unknown);
        return unknown;
#else
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));
    setX10SourcePosition(class_definition, env, x10Token);
    astX10ScopeStack.push(class_definition); // to contain the class members...
#endif
}


JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionParenthesizedExpression(JNIEnv *env, jclass clz, jint x10_parentheses_count) 
{ 
        cactionParenthesizedExpression(env, clz, x10_parentheses_count);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSetupSourceFilename(JNIEnv *env, jclass xxx, jstring x10_full_file_name) 
{ 
#if 0
        cactionSetupSourceFilename(env, xxx, x10_full_file_name);
#else
    string full_file_name = convertJavaStringToCxxString(env, x10_full_file_name);
    ::currentSourceFile = isSgSourceFile((*::project)[full_file_name]);
    Rose::Frontend::X10::X10c::X10c_globalFilePointer = ::currentSourceFile;
        if ( ! ::currentSourceFile) {
                // MH (6/23/2014): 
                // Currently all the necessary user-defined files have to be specified on the command line, because classpath support is not available now
                // TODO: Remove this after enabling classpath support for X10
                cout << "Source file "<< full_file_name << " is not specified on the command line, although it is used" << endl;
                cout.flush();
                exit(1);
        }
// TODO: Remove this!
//cout << "*+* Setting up source file " << full_file_name << endl;
//cout.flush();
    ROSE_ASSERT(::currentSourceFile);
    ROSE_ASSERT(::currentSourceFile -> get_file_info());

    SgJavaImportStatementList* import_statement_list = new SgJavaImportStatementList();
    import_statement_list -> set_parent(::currentSourceFile);
    // setX10SourcePosition(import_statement_list, env, x10Token);
    ::currentSourceFile -> set_import_list(import_statement_list);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionClearSourceFilename(JNIEnv *env, jclass xxx) 
{ 
        cactionClearSourceFilename(env, xxx);
}

#if 1
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertClassStart(JNIEnv *env, jclass xxx, jstring x10_string, jboolean is_interface, jboolean is_enum, jboolean is_anonymous, jobject x10Token) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertClassStart(JNIEnv *env, jclass xxx, jstring x10_string, jobject x10Token) 
#endif
{ 
#if 1
        cactionInsertClassStart(env, xxx, x10_string, is_interface, is_enum, is_anonymous, x10Token);
#else
    SgName name = convertJavaStringToCxxString(env, x10_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of cactionInsertClassStart(): = %s \n", name.str());
    SgScopeStatement *outerScope = astX10ScopeStack.top();
    ROSE_ASSERT(outerScope != NULL);

    SgClassDeclaration *class_declaration = buildDefiningClassDeclaration(name, outerScope);
    setX10SourcePosition(class_declaration, env, x10Token);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));
    setX10SourcePosition(class_definition, env, x10Token);
//MH-20140501
        SgScopeStatement *type_space = SageBuilder::buildScopeStatement(class_definition);
    setX10SourcePosition(type_space, env, x10Token);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) class_declaration -> getAttribute("type_space");
    ROSE_ASSERT(attribute);
    attribute -> setNode(type_space);

    astX10ScopeStack.push(class_definition); // to contain the class members...
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertClassEnd(JNIEnv *env, jclass xxx, jstring x10_string, jobject x10Token) 
{ 
#if 0
        cactionInsertClassEnd(env, xxx, x10_string, x10Token);
#else
    SgName name = convertJavaStringToCxxString(env, x10_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of cactionInsertClassEnd: %s \n", name.str());
    ROSE_ASSERT(! astX10ScopeStack.empty());

    SgClassDefinition *class_definition = astX10ScopeStack.popClassDefinition();
    ROSE_ASSERT(! class_definition -> attributeExists("namespace"));

        // MH-20140703
    cout << "get_file_info() 1 =" << class_definition -> get_file_info() -> get_filenameString() << endl;
    cout << "get_file_info() 2 =" << class_definition -> get_file_info() -> get_physical_filename().c_str() << endl;

//MH-20140620
cout << "...in the stack: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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
#endif
}


/**
 * Java_x10rose_visit_JNI_cactionBuildClassSupportStart(JNIEnv *env, jclass xxx, jstring x10_name, jstring x10_external_name, jboolean x10_user_defined_class, jboolean x10_has_conflicts, jboolean x10_is_interface, jboolean x10_is_enum, jboolean x10_is_anonymous, jobject x10Token) {
 *
 */
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildClassSupportStart(JNIEnv *env, jclass xxx, jstring x10_name, jstring x10_external_name, jboolean x10_user_defined_class, jboolean x10_has_conflicts, jboolean x10_is_interface, jboolean x10_is_enum, jboolean x10_is_anonymous, jobject x10Token) 
{ 
#if 0
        cactionBuildClassSupportStart(env, xxx, x10_name, x10_external_name, x10_user_defined_class, x10_is_interface, x10_is_enum, x10_is_anonymous, x10Token);
#else
    SgName name = convertJavaStringToCxxString(env, x10_name);
    SgName external_name = convertJavaStringToCxxString(env, x10_external_name);
    bool user_defined_class = x10_user_defined_class;
    bool has_conflicts = x10_has_conflicts;
    bool is_interface = x10_is_interface;
    bool is_enum = x10_is_enum;
    bool is_anonymous = x10_is_anonymous;

    if (SgProject::get_verbose() > 0)
        printf ("Inside of cactionBuildClassSupportStart(): %s %s \n", (is_interface ? "interface" : "class"), name.str());

    SgScopeStatement *outerScope = astX10ScopeStack.top();
    ROSE_ASSERT(outerScope != NULL);

//MH-20140620
cout << "...in the stack: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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

    SgClassSymbol *class_symbol = outerScope -> lookup_class_symbol(name);
cout << "name=" << name << ", class_symbol=" << class_symbol << ", outerscope=" << outerScope << endl;
    ROSE_ASSERT(class_symbol);
    SgClassDeclaration *declaration = (SgClassDeclaration *) class_symbol -> get_declaration() -> get_definingDeclaration();
    ROSE_ASSERT(declaration);
    SgClassDefinition *class_definition = declaration -> get_definition();
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));
    astX10ScopeStack.push(class_definition); // to contain the class members...

//MH-20140620
cout << "...in the stack: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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
// MH-20140624
cout << "Set attribute for class_definition=" << class_definition << endl;
    }

    astX10ComponentStack.push(class_definition); // To mark the end of the list of components in this type.

    if (SgProject::get_verbose() > 0)
        printf ("Exiting cactionBuildClassSupportStart(): %s %s \n", (is_interface ? "interface" : "class"), name.str());

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateClassSupportStart(JNIEnv *env, jclass xxx, jstring x10_name, jobject x10Token) 
{ 
        cactionUpdateClassSupportStart(env, xxx, x10_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertTypeParameter(JNIEnv *env, jclass clz, jstring x10_name, jobject x10Token) 
{ 
        cactionInsertTypeParameter(env, clz, x10_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildTypeParameterSupport(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jint method_index, jstring x10_name, jint num_bounds, jobject x10Token) 
{ 
        cactionBuildTypeParameterSupport(env, clz, x10_package_name, x10_type_name, method_index, x10_name, num_bounds, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdatePushMethodParameterScope(JNIEnv *env, jclass clz, jstring str1, jint method_index, jobject x10Token) 
{ 
        cactionUpdatePushMethodParameterScope(env, clz, method_index, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateTypeParameterSupport(JNIEnv *env, jclass clz, jstring x10_name, int method_index, jint num_bounds, jobject x10Token) 
{ 
        cactionUpdateTypeParameterSupport(env, clz, x10_name, method_index, num_bounds, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdatePopMethodParameterScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionUpdatePopMethodParameterScope(env, clz, x10Token);
}

#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildClassExtendsAndImplementsSupport(JNIEnv *env, jclass xxx, jint x10_num_type_parameters, jboolean x10_has_super_class, jint x10_num_interfaces, jobject x10Token) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildClassExtendsAndImplementsSupport(JNIEnv *env, jclass xxx, jint x10_num_type_parameters, jobjectArray array_type_parameters, jboolean x10_has_super_class, jstring super_class_name, jint x10_num_interfaces, jobjectArray array_interfaces, jobject x10Token)
#endif
{ 
        cactionBuildClassExtendsAndImplementsSupport(env, xxx, x10_num_type_parameters, array_type_parameters, x10_has_super_class, super_class_name, x10_num_interfaces, array_interfaces, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildClassSupportEnd(JNIEnv *env, jclass xxx, jstring x10_string, jint num_class_members, jobject x10Token) 
{ 
#if 0
        cactionBuildClassSupportEnd(env, xxx, x10_string, num_class_members, x10Token);
#else
   SgName name = convertJavaStringToCxxString(env, x10_string);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of cactionBuildClassSupportEnd: %s \n", name.str());

    ROSE_ASSERT(! astX10ScopeStack.empty());
    SgClassDefinition *class_definition = astX10ScopeStack.popClassDefinition();
    ROSE_ASSERT(! class_definition -> attributeExists("namespace"));
        // skipping handling operator, so it can happen that astX10ComponentStack.size() == 0 such as x10.lang.Arithmetic.x10
        if (astX10ComponentStack.size() == 0) {
                return;
        }
cout << "astX10ComponentStack.size()=" << astX10ComponentStack.size() << endl;
//    for (SgStatement *statement = astX10ComponentStack.popStatement();
    for (SgNode *statement = astX10ComponentStack.pop();
#if 0
        statement != class_definition && statement != ::globalScope;
#else // MH (6/23/2014) : instead of the condition above, if-statement is defined inside this for statement !
      //  statement != NULL;
                astX10ComponentStack.size() >= 0;
#endif
//        statement = astX10ComponentStack.popStatement()) {
        statement = astX10ComponentStack.pop()) {
#if 1   
#if 1 // Remove this
                // MH (6/23/2014) : This condition is needed because elements of scopeStack are inserted into 
                // componentStack in the cactionCompilationUnitDeclaration
                string str = statement->class_name();
                if (str == "SgClassDefinition" || str == "SgGlobal" || str == "SgClassType") {
cout << "statement2=" << statement << endl;
                        break;
                }
#endif
                
        if (SgProject::get_verbose() > 2) {
            cerr << "(1) Adding statement "
                 << statement -> class_name()
                 << " to an implicit Type Declaration"
                 << endl;
            cerr.flush();
        }
        ROSE_ASSERT(statement != NULL);

//        class_definition -> prepend_statement(statement);
        class_definition -> prepend_statement((SgStatement *)statement);

                if (astX10ComponentStack.size() == 0)
                        break;
#endif
    }

    ROSE_ASSERT(! astX10ScopeStack.empty());
    SgScopeStatement *outerScope = astX10ScopeStack.top();

    SgClassDeclaration *class_declaration = class_definition -> get_declaration();
    ROSE_ASSERT(class_declaration);

//MH-20140701
cout << "...in the stack at cactionBuildClassSupportEnd 1: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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


    //
    // TODO:  Review this because of the package issue and the inability to build a global AST.
    //
    ROSE_ASSERT(outerScope != NULL);
    if (isSgClassDefinition(outerScope) && isSgClassDefinition(outerScope) -> attributeExists("namespace")) { // a type in a package?
        isSgClassDefinition(outerScope) -> append_statement(class_declaration);
    }
    else if (isSgClassDefinition(outerScope) && (! isSgClassDefinition(outerScope) -> attributeExists("namespace"))) { // an inner type?
        ;//    astX10ComponentStack.push(class_declaration);
    }
    else if (isSgBasicBlock(outerScope)) { // a local type declaration?
        astX10ComponentStack.push(class_declaration);
    }
    else if (outerScope == ::globalScope) { // a user-defined type?
        ::globalScope -> append_statement(class_declaration);
    }
    else { // What is this?
#if 0 //MH-20140313 so far push global scope
        ::globalScope -> append_statement(class_declaration);
#else
        cout << "Found class declaration in scope "
             << outerScope -> class_name()
             << endl;
        ::globalScope -> append_statement(class_declaration);
//        ROSE_ASSERT(false);
#endif
    }

//MH-20140701
cout << "...in the stack at cactionBuildClassSupportEnd 2: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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

    if (SgProject::get_verbose() > 0)
        printf ("Leaving cactionBuildClassSupportEnd: %s \n", name.str());

//    astX10ScopeStack.push(::globalScope);
//    astX10ScopeStack.push(class_definition);

#if 0 // MH-20140602
        SgName filepath = convertJavaStringToCxxString(env, currentFilePath);
        scopeMap[filepath] = astX10ScopeStack;
        componentMap[filepath] = astX10ComponentStack;
#else
        scopeMap[currentTypeName] = astX10ScopeStack;
        componentMap[currentTypeName] = astX10ComponentStack;
        cout << "BuildClassSupportEnd() for "<< name <<" : astX10ScopeStack's size=" << astX10ScopeStack.size() << ", name=" << currentTypeName  <<  endl;
        cout << "BuildClassSupportEnd() for "<< name <<" : astX10ComponentStack's size=" << astX10ComponentStack.size() << ", name=" << currentTypeName  <<endl;
#endif

// MH-20140326
#if 0
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateClassSupportEnd(JNIEnv *env, jclass xxx, jstring x10_name, jboolean has_super_class, jint num_interfaces, jint num_class_members, jobject x10Token) 
{ 
        cactionUpdateClassSupportEnd(env, xxx, x10_name, has_super_class, num_interfaces, num_class_members, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildInnerTypeSupport(JNIEnv *env, jclass clz,                                                                     jstring x10_package_name,
                                                                    jstring x10_type_name,
                                                                    jobject x10Token)

{

        cactionBuildInnerTypeSupport(env, clz, x10_package_name, x10_type_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateInnerTypeSupport(JNIEnv *env, jclass clz,                                                                      jstring x10_package_name,
                                                                     jstring x10_type_name,
                                                                     jobject x10Token)

{

        cactionUpdateInnerTypeSupport(env, clz, x10_package_name, x10_type_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildMethodSupportStart(JNIEnv *env, jclass clz,                                                                       jstring x10_name,
                                                                      jint method_index,
                                                                      jobject method_location) 
{
#if 0
        cactionBuildMethodSupportStart(env, clz, x10_name, method_index, method_location);
#else
   SgName name = convertJavaStringToCxxString(env, x10_name);
    if (SgProject::get_verbose() > 1)
          printf ("Inside of BuildMethodSupportStart for method = %s \n", name.str());
    SgClassDefinition *class_definition = isSgClassDefinition(astX10ScopeStack.top());
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));

//MH-20140403
#if 1
    //
    // This scope will be used to store Type Parameters, if there are any.
    //
    SgScopeStatement *type_space = new SgScopeStatement();
    type_space -> set_parent(class_definition);
    setX10SourcePosition(type_space, env, method_location);
    if (method_index >= 0) {
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("type_parameter_space");
// MH-20140624
cout << "class_definition=" << class_definition << ", attribute=" << attribute << endl;
        ROSE_ASSERT(attribute);
        attribute -> setNode(type_space, method_index);
    }
#endif
//MH-20140315
//    astX10ScopeStack.push(class_definition);
//MH-20140403
    astX10ScopeStack.push(type_space);

    if (SgProject::get_verbose() > 1)
        printf ("Exiting BuildMethodSupportStart for method = %s \n", name.str());

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateMethodSupportStart(JNIEnv *env, jclass clz,                                                                        jstring x10_name,
                                                                       jint method_index,
                                                                       jint num_formal_parameters,
                                                                       jobject method_location) 
{

        cactionUpdateMethodSupportStart(env, clz, x10_name, method_index, num_formal_parameters, method_location);
}

#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildArgumentSupport(JNIEnv *env, jclass clz, jstring x10_argument_name, jstring x10_argument_type_name, jboolean x10_is_var_args, jboolean x10_is_final, jobject x10Token) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildArgumentSupport(JNIEnv *env, jclass clz, jstring x10_argument_name, jboolean x10_is_var_args, jboolean x10_is_final, jobject x10Token) 
#endif
{ 
#if 0
        cactionBuildArgumentSupport(env, clz, x10_argument_name, x10_argument_type_name, x10_is_var_args, x10_is_final, x10Token);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Build argument support\n");

// TODO: Remove this !!!
//    SgFunctionDefinition *method_definition = isSgFunctionDefinition(astX10ScopeStack.top());
//    ROSE_ASSERT(method_definition);

    SgName argument_name = convertJavaStringToCxxString(env, x10_argument_name);
    bool is_final = x10_is_final;
    bool is_var_args = x10_is_var_args;

    if (SgProject::get_verbose() > 0)
        printf ("argument argument_name = %s \n", argument_name.str());

    SgType *argument_type = astX10ComponentStack.popType();
//cout << "ArgType="<< argument_type << endl;
    ROSE_ASSERT(argument_type);

    // Until we attached this to the AST, this will generate an error in the AST consistancy tests.
    SgInitializedName *initialized_name = SageBuilder::buildInitializedName(argument_name, argument_type, NULL);

    setX10SourcePosition(initialized_name, env, x10Token);

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

    astX10ComponentStack.push(initialized_name);

// TODO: Remove this!
cout << "Pushed " << initialized_name->get_name() << ", " << initialized_name -> class_name() << endl; cout.flush();

    if (SgProject::get_verbose() > 0)
        printf ("Exiting Build argument support\n");

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateArgumentSupport(JNIEnv *env, jclass clz, jint argument_index, jstring x10_argument_name, jstring x10_argument_type_name, jboolean is_var_args, jboolean is_final, jobject x10Token) 
{ 
        cactionUpdateArgumentSupport(env, clz, argument_index, x10_argument_name, x10_argument_type_name, is_var_args, is_final, x10Token);
}

/*
 * JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildMethodSupportEnd(JNIEnv *env, jclass xxx,
                                                                    jstring x10_string,
                                                                    jint method_index,
                                                                    jboolean x10_is_constructor,
                                                                    jboolean x10_is_abstract,
                                                                    jboolean x10_is_native,
                                                                    jint x10_number_of_type_parameters,
                                                                    jint x10_number_of_arguments,
                                                                    jboolean x10_is_user_defined,
                                                                    jobject method_location,
                                                                    jobject args_location) {

 **/
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildMethodSupportEnd(JNIEnv *env, jclass xxx,
                                                                                                                   jstring x10_string,
                                                                    jint method_index,
                                                                    jboolean x10_is_constructor,
                                                                    jboolean x10_is_abstract,
                                                                    jboolean x10_is_native,
                                                                    jint x10_number_of_type_parameters,
                                                                    jint x10_number_of_arguments,
                                                                    jboolean x10_is_user_defined,
                                                                    jobject method_location,
                                                                    jobject args_location) 
{
#if 0
        cactionBuildMethodSupportEnd(env, xxx, x10_string, method_index, x10_is_constructor, x10_is_abstract, x10_is_native, x10_number_of_type_parameters, x10_number_of_arguments, x10_is_compiler_generated, args_location, method_location);
#else
    SgName name = convertJavaStringToCxxString(env, x10_string);
    int number_of_type_parameters = x10_number_of_type_parameters;
    int number_of_arguments = x10_number_of_arguments;
    bool is_constructor = x10_is_constructor,
         is_abstract = x10_is_abstract,
         is_native = x10_is_native,
         is_user_defined = x10_is_user_defined;

    if (SgProject::get_verbose() > 1)
        printf ("Build support for implicit class member function (method) name = %s \n", name.str());

        cout << "BuildMethodSupportEnd() : astX10ScopeStack's size=" << astX10ScopeStack.size() << ", name=" << currentTypeName <<  endl;
        cout << "BuildMethodSupportEnd() : astX10ComponentStack's size=" << astX10ComponentStack.size() << ", name=" << currentTypeName  <<endl;

//MH-20140403
    SgScopeStatement *type_space = isSgScopeStatement(astX10ScopeStack.pop());
    ROSE_ASSERT(type_space);

// TODO: Remove this !!!
//    SgFunctionDefinition *method_definition = isSgFunctionDefinition(((AstSgNodeAttribute *) type_space -> getAttribute("method")) -> getNode());
//    ROSE_ASSERT(method_definition);

    SgClassDefinition *class_definition = isSgClassDefinition(astX10ScopeStack.top());
    ROSE_ASSERT(class_definition != NULL && (! class_definition -> attributeExists("namespace")));

    //
    // There is no reason to distinguish between defining and non-defining declarations in Java...
    //
    SgMemberFunctionDeclaration *method_declaration = buildDefiningMemberFunction(name, class_definition, number_of_arguments, env, method_location, args_location);
    setX10SourcePosition(method_declaration, env, method_location);
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
            SgClassDeclaration *parameter_decl = isSgClassDeclaration(astX10ComponentStack.pop());
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

    astX10ComponentStack.push(method_declaration);

//MH-20140312
// Tentatively push class definition onto stack top.
// So far, this is necessary for passing an assertion in Java_x10rose_visit_JNI_cactionBuildMethodSupportStart 
// for checking if stack top is class definition or not.
// MH-20140620
#if 0
//    astX10ScopeStack.push(method_definition);
    astX10ScopeStack.push(class_definition);
#endif


#if 0
    SgSymbolTable *tab = class_definition->get_symbol_table();
    int size = tab->size();
    printf("Symbol table size=%d\n", size);
    tab->print();
#endif

    if (SgProject::get_verbose() > 1)
        printf ("Exiting build support for implicit class member function (method) name = %s (%p) \n", name.str(), method_declaration);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateMethodSupportEnd(JNIEnv *env, jclass xxx,                                                                      jstring x10_string,
                                                                     jint method_index,
                                                                     jboolean is_compiler_generated,
                                                                     jint number_of_parameters,
                                                                     jobject args_location,
                                                                     jobject method_location) 
{

        cactionUpdateMethodSupportEnd(env, xxx, x10_string, method_index, is_compiler_generated, number_of_parameters, args_location, method_location);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildInitializerSupport(JNIEnv *env, jclass clz, jboolean x10_is_static, jstring x10_string, jint initializer_index, jobject x10Token) 
{ 
        cactionBuildInitializerSupport(env, clz, x10_is_static, x10_string, initializer_index, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBuildFieldSupport(JNIEnv *env, jclass xxx, jstring x10_string, jobject x10Token) 
{ 
#if 0
        cactionBuildFieldSupport(env, xxx, x10_string, x10Token);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Inside of cactionBuildFieldSupport (variable declaration for field) \n");

    SgName name = convertJavaStringToCxxString(env, x10_string);

    SgType *type = astX10ComponentStack.popType();
    SgVariableDeclaration *variableDeclaration = SageBuilder::buildVariableDeclaration (name, type, NULL, astX10ScopeStack.top());
    ROSE_ASSERT(variableDeclaration != NULL);
    variableDeclaration -> set_parent(astX10ScopeStack.top());
//    setX10SourcePosition(variableDeclaration, env, x10Token);
    vector<SgInitializedName *> vars = variableDeclaration -> get_variables();
/*
    for (vector<SgInitializedName *>::iterator name_it = vars.begin(); name_it != vars.end(); name_it++) {
        setX10SourcePosition(*name_it, env, x10Token);
    }
*/

    astX10ComponentStack.push(variableDeclaration);

    if (SgProject::get_verbose() > 0)
        variableDeclaration -> get_file_info() -> display("source position in cactionBuildFieldSupport(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Exiting cactionBuildFieldSupport (variable declaration for field) \n");

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdateFieldSupport(JNIEnv *env, jclass xxx, jstring x10_string, jobject x10Token) 
{ 
        cactionUpdateFieldSupport(env, xxx, x10_string, x10Token);
}

JNIEXPORT jboolean JNICALL Java_x10rose_visit_JNI_cactionIsSpecifiedSourceFile(JNIEnv *env, jclass clz, jstring x10_full_file_name) 
{ 
        return cactionIsSpecifiedSourceFile(env, clz, x10_full_file_name);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTest(JNIEnv *env, jclass clz) 
{ 
        cactionTest(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertImportedPackageOnDemand(JNIEnv *env, jclass clz, jstring x10_package_name, jobject x10Token) 
{ 
        cactionInsertImportedPackageOnDemand(env, clz, x10_package_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertImportedTypeOnDemand(JNIEnv *env, jclass clz, jstring x10_package_name,  jstring x10_type_name, jobject x10Token) 
{ 
        cactionInsertImportedTypeOnDemand(env, clz, x10_package_name, x10_type_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertImportedType(JNIEnv *env, jclass clz, jstring x10_package_name,  jstring x10_type_name, jobject x10Token) 
{ 
        cactionInsertImportedType(env, clz, x10_package_name, x10_type_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInsertImportedStaticField(JNIEnv *env, jclass clz, jstring x10_variable_name, jobject x10Token) 
{ 
        cactionInsertImportedStaticField(env, clz, x10_variable_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPushPackage(JNIEnv *env, jclass clz, jstring x10_package_name, jobject x10Token) 
{ 
#if 1
        cactionPushPackage(env, clz, x10_package_name, x10Token);
#else
    SgName package_name = convertJavaStringToCxxString(env, x10_package_name);
    SgClassDefinition *package_definition = findOrInsertPackage(package_name, env, x10Token);
//    ROSE_ASSERT(package_definition);
// MH-20140502 comment out following two lines because cactionCompilationUnitDeclaration is immediately invoked after the invocation of this function
    astX10ScopeStack.push(package_definition);        // Push the package onto the scopestack.
    astX10ScopeStack.push(::globalScope);  // Push the global scope onto the stack.
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUpdatePushPackage(JNIEnv *env, jclass clz, jstring x10_package_name, jobject x10Token) 
{ 
        cactionUpdatePushPackage(env, clz, x10_package_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPopPackage(JNIEnv *env, jclass clz) 
{ 
        cactionPopPackage(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPushTypeScope(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jobject x10Token) 
{ 
        cactionPushTypeScope(env, clz, x10_package_name, x10_type_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPopTypeScope(JNIEnv *env, jclass clz) 
{ 
        cactionPopTypeScope(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPushTypeParameterScope(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jobject x10Token) 
{ 
        cactionPushTypeParameterScope(env, clz, x10_package_name, x10_type_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPopTypeParameterScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionPopTypeParameterScope(env, clz, x10Token);
}

//JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitList(JNIEnv *env, jclass clz, jint argc, jobjectArray argv) 
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitList(JNIEnv *env, jclass clz, jint size, jobjectArray array) 
{ 
#if 0
        cactionCompilationUnitList(env, clz);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Inside of Java_x10rose_visit_JNI_cactionCompilationUnitList \n");

// MH-20140515 remove this, just trial code
        // suppose that the array size is always 1
//      jobject path = env->GetObjectArrayElement(array, 0);
#if 0
        currentFilePath = (jstring)env->NewGlobalRef(env->GetObjectArrayElement(array, 0));
        SgName filepath = convertJavaStringToCxxString(env, currentFilePath);
#endif

        astX10ScopeStack = *(new ScopeStack);
        scopeMap[currentTypeName] = astX10ScopeStack;
        astX10ComponentStack = *(new ComponentStack);
        componentMap[currentTypeName] = astX10ComponentStack;

    // This is already setup by ROSE as part of basic file initialization before calling ECJ.
//MH-20140401
#if 0
    ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
#else
    ROSE_ASSERT(Rose::Frontend::X10::X10c::X10c_globalFilePointer != NULL);
#endif
    if (SgProject::get_verbose() > 0)
        printf ("Rose::Frontend::X10::X10c::X10c_globalFilePointer = %s \n", Rose::Frontend::X10::X10c::X10c_globalFilePointer -> class_name().c_str());

    // TODO: We need the next line for EDG4 
    // SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
    SgSourceFile *sourceFile = isSgSourceFile(Rose::Frontend::X10::X10c::X10c_globalFilePointer);

    ROSE_ASSERT(sourceFile != NULL);

    if (SgProject::get_verbose() > 0)
        printf ("sourceFile -> getFileName() = %s \n", sourceFile -> getFileName().c_str());

    // We don't use the SgProject but since it should have already been built, we can verify that it is present.
    ::project = sourceFile -> get_project();
    ROSE_ASSERT(::project);
        
#if 0
    // Get the pointer to the global scope and push it onto the astX10ScopeStack.
    ::globalScope = sourceFile -> get_globalScope();
#else
        // MH (6/24/2014) : use the same globalScope across files
    if (!::globalScope) 
                ::globalScope = sourceFile -> get_globalScope();
#endif
        
    ROSE_ASSERT(::globalScope != NULL);
    //
    // At this point, the scope stack should be empty. Push the global scope into it.
    //
//    ROSE_ASSERT(astX10ScopeStack.empty());
    astX10ScopeStack.push(::globalScope); // Push the global scope onto the stack.

        Rose::Frontend::X10::X10c::currentEnvironment = env;
#if 0 //MH-20140326
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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
    ROSE_ASSERT(astX10ScopeStack.top() -> get_parent() != NULL);

     if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_x10rose_visit_JNI_cactionCompilationUnitList \n");
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitListEnd(JNIEnv *env, jclass clz) 
{ 
        cactionCompilationUnitListEnd(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSetupBasicTypes(JNIEnv *env, jclass clz) 
{ 
        cactionSetupBasicTypes(env, clz);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPackageAnnotations(JNIEnv *env, jclass clz, int num_annotations, jobject x10Token) 
{ 
        cactionPackageAnnotations(env, clz, num_annotations, x10Token);
}

#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitDeclaration(JNIEnv *env, jclass clz, jstring x10_full_file_name, jstring x10_package_name, jstring x10_filename, jobject x10Token) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitDeclaration(JNIEnv *env, jclass clz, jstring x10_full_file_name, jstring x10_package_name, jstring x10_filename, jobject x10Token) 
#endif
{ 
#if 0
        cactionCompilationUnitDeclaration(env, clz, x10_full_file_name, x10_package_name, x10_filename, x10Token);
#else
    if (SgProject::get_verbose() > 0)
         printf ("Inside of Java_x10rose_visit_JNI_cactionCompilationUnitDeclaration() \n");

//MH-20140620
cout << "...in the stack: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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
        string full_file_name = convertJavaStringToCxxString(env, x10_full_file_name);
        ROSE_ASSERT(::currentSourceFile == isSgSourceFile((*::project)[full_file_name]));

    SgName package_name = convertX10PackageNameToCxxString(env, x10_package_name);
    ROSE_ASSERT(astX10ScopeStack.top() == ::globalScope); // There must be a scope element in the scope stack.

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
    AstRegExAttribute *attribute =  new AstRegExAttribute(convertJavaStringToCxxString(env, x10_package_name));
    package -> setAttribute("translated_package", attribute);
//    astX10ScopeStack.push(package); // Push the package onto the scopestack.
#endif
//////////////////////////////////////

    // Example of how to get the string...but we don't really use the absolutePathFilename in this function.
    const char *absolutePathFilename = env -> GetStringUTFChars(x10_filename, NULL);
    ROSE_ASSERT(absolutePathFilename != NULL);
    // printf ("Inside of cactionCompilationUnitDeclaration absolutePathFilename = %s \n", absolutePathFilename);
    env -> ReleaseStringUTFChars(x10_filename, absolutePathFilename);

    // This is already setup by ROSE as part of basic file initialization before calling ECJ.
#if 0
    ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
#else
    ROSE_ASSERT(Rose::Frontend::X10::X10c::X10c_globalFilePointer != NULL);
#endif

//////////// MH-20140404
//    SgSourceFile *sourceFile = isSgSourceFile(Rose::Frontend::X10::X10c::X10c_globalFilePointer);
    SgJavaClassDeclarationList* class_declaration_list = new SgJavaClassDeclarationList();
//MH-20140416
//    setX10SourcePosition(class_declaration_list, env, x10Token);
//MH-20140418
    class_declaration_list -> set_parent(currentSourceFile);
    currentSourceFile -> set_class_list(class_declaration_list);
//////////// 

    astX10ComponentStack.push(astX10ScopeStack.top()); // To mark the end of the list of components in this Compilation unit.
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitDeclarationEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionCompilationUnitDeclarationEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEcjFatalCompilationErrors(JNIEnv *env, jclass clz, jstring x10_full_file_name) 
{ 
        cactionEcjFatalCompilationErrors(env, clz, x10_full_file_name);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompilationUnitDeclarationError(JNIEnv *env, jclass clz, jstring x10_error_message, jobject x10Token) 
{ 
        cactionCompilationUnitDeclarationError(env, clz, x10_error_message, x10Token);
}

#if 1
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeDeclaration(JNIEnv *env, jclass clz,
                                                              jstring x10_package_name,
                                                              jstring x10_type_name,
                                                              jint num_annotations,
                                                              jboolean has_super_class,
                                                              jboolean x10_is_annotation_interface,
                                                              jboolean x10_is_interface,
                                                              jboolean x10_is_enum,
                                                              jboolean x10_is_abstract,
                                                              jboolean x10_is_final,
                                                              jboolean x10_is_private,
                                                              jboolean x10_is_public,
                                                              jboolean x10_is_protected,
                                                              jboolean x10_is_static,
                                                              jboolean x10_is_strictfp,
                                                              jobject x10Token)
{
#if 1
        cactionTypeDeclaration(env, clz, x10_package_name, x10_type_name, num_annotations, has_super_class, x10_is_annotation_interface, x10_is_interface, x10_is_enum, x10_is_abstract, x10_is_final, x10_is_private, x10_is_public, x10_is_protected, x10_is_static, x10_is_strictfp, x10Token);
#else
//          astX10ScopeStack.push(::globalScope);

       // We could provide a constructor for "SgName" that takes a "jstring".  This might help support a simpler interface.
        SgName package_name = convertX10PackageNameToCxxString(env, x10_package_name),
               type_name = convertJavaStringToCxxString(env, x10_type_name);

// push package
//  Java_x10rose_visit_JNI_cactionPushPackage(env, NULL, x10_package_name, NULL);

//MH-20140418 Added
        SgScopeStatement *outerScope = astX10ScopeStack.top();

//MH-20140418 Changed <tt>::globalScope</tt> to <tt>outerScope</tt>
    SgClassDeclaration *class_declaration = buildDefiningClassDeclaration(type_name, /*::globalScope*/outerScope);

   ROSE_ASSERT(class_declaration);

    SgClassDefinition *definition = class_declaration->get_definition();
    ROSE_ASSERT(definition);
//  astX10ScopeStack.push(class_declaration->get_definition()); 

    // MH-20140311
    definition -> setAttribute("class_members", new AstSgNodeListAttribute());
    definition -> setAttribute("type_parameter_space", new AstSgNodeListAttribute());
    astX10ScopeStack.push(definition);
    astX10ComponentStack.push(definition);

    SgSymbol *sym = globalScope->lookup_symbol(type_name);

//  insert_symbol(type_name, class_declaration->get_symbol_from_symbol_table());
#endif
}
#endif

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeDeclarationHeader(JNIEnv *env, jclass clz,                                                                     jboolean x10_has_super_class,
                                                                    jint num_interfaces,
                                                                    jint num_parameters,
                                                                    jobject x10Token) 
{

        cactionTypeDeclarationHeader(env, clz, x10_has_super_class, num_interfaces, num_parameters, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeDeclarationEnd(JNIEnv *env, jclass clz, jboolean is_user_defined, jobject x10Token) 
{ 
#if 0
        cactionTypeDeclarationEnd(env, clz, x10Token);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgClassDeclaration (cactionTypeDeclarationEnd) \n");

//      string full_file_name = convertJavaStringToCxxString(env, currentFilePath);
//    ::currentSourceFile = isSgSourceFile((*::project)[full_file_name]);
    Rose::Frontend::X10::X10c::X10c_globalFilePointer = ::currentSourceFile;
    SgSourceFile *sourceFile = isSgSourceFile(Rose::Frontend::X10::X10c::X10c_globalFilePointer);

        scopeMap[currentTypeName] = astX10ScopeStack;
        componentMap[currentTypeName] = astX10ComponentStack;

        map<std::string, ScopeStack>::iterator it = scopeMap.find(currentTypeName);
        if (it != scopeMap.end()) {
                astX10ScopeStack = it->second;
cout << "FOUND existing scopeMap for " << currentTypeName << ", astX10ScopeStack=" << &astX10ScopeStack << endl;
        }
        else {
cout << "*NOT FOUND existing scopeMap for " << currentTypeName << ", astX10ScopeStack=" << &astX10ScopeStack << endl;
        }
        map<std::string, ComponentStack>::iterator it2 = componentMap.find(currentTypeName);
        if (it2 != componentMap.end()) {
                astX10ComponentStack = it2->second;
cout << "FOUND existing componentMap for " << currentTypeName << ", astX10ComponentStack=" << &astX10ComponentStack << endl;
        }
        else {
cout << "*NOT FOUND existing componentMap for " << currentTypeName << ", astX10ComponentStack=" << &astX10ComponentStack << endl;
        }

//MH-20140701
/*
cout << "...in the stack at cactionTypeDeclarationEnd1 : " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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
        
    ROSE_ASSERT(astX10ScopeStack.top() != NULL);
    SgClassDefinition *class_definition = astX10ScopeStack.popClassDefinition(); // pop the class definition
    ROSE_ASSERT(class_definition);

    SgScopeStatement *type_space = isSgScopeStatement(astX10ScopeStack.pop());  // Pop the type parameters scope from the stack.
    ROSE_ASSERT(type_space);

    SgClassDeclaration *class_declaration = isSgClassDeclaration(class_definition -> get_declaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration != NULL);

//MH-20140701
cout << "...in the stack at cactionTypeDeclarationEnd2 : " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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
/*
    ROSE_ASSERT(class_definition == astX10ComponentStack.top());
    astX10ComponentStack.pop(); // remove the class definition from the stack
*/

#if 1 //MH-20140404
        SgJavaClassDeclarationList *class_list = sourceFile -> get_class_list();
#if 0
    if (class_list == NULL) {
        printf("class_list NULL\n");
    }
    else if (class_list -> get_file_info() == NULL) { // The first
//      setX10SourcePosition(class_list, env, x10Token);
        printf("file info NULL\n");
    }
#endif
    class_list -> get_java_class_list().push_back(class_declaration);

    vector<SgClassDeclaration *> &type_list = class_list -> get_java_class_list();
//      cout << "type_list.size()=" << type_list.size() << endl;
    for (int i = 0; i < type_list.size(); i++) {
        SgClassDeclaration *type_declaration = type_list[i];
//              cout << "SET PARENT of " << type_declaration << endl;
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
        astX10ComponentStack.push(class_declaration);
    }
*/
    if (class_declaration -> get_explicit_anonymous()) {
        astX10ComponentStack.push(class_declaration);
    }
    else { // Check if this is a type-level type. If so, add it to its sourcefile list.
        SgClassDefinition *package_definition = isSgClassDefinition(astX10ScopeStack.top());
// TODO: Remove this!
/*
if (! package_definition) {
cout << "A package definition was expected, but we found a " << astX10ScopeStack.top() -> class_name().c_str() << endl;
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
                    setX10SourcePosition(import_list, env, x10Token);
                }
*/
                SgJavaClassDeclarationList *class_list = sourceFile -> get_class_list();
// TODO: Remove this!
/*
                if (class_list -> get_file_info() == NULL) { // The first
                    setX10SourcePosition(class_list, env, x10Token);
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

//MH-20140620
cout << "...in the stack: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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

        if (is_user_defined == JNI_TRUE) {
                string replaced = class_declaration -> get_qualified_name();
                replaceString(replaced, ".", "::");
                Rose::Frontend::X10::X10c::classNames.push_back(replaced);
        }

    ROSE_ASSERT(astX10ScopeStack.top() != NULL);
    if (SgProject::get_verbose() > 0)
        astX10ScopeStack.top() -> get_file_info() -> display("source position in Java_x10rose_visit_JNI_cactionTypeDeclarationEnd(): debug");

    if (SgProject::get_verbose() > 0)
        printf ("Leaving Java_x10rose_visit_JNI_cactionTypeDeclarationEnd() (cactionTypeDeclarationEnd) \n");
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConstructorDeclaration(JNIEnv *env, jclass clz, jstring x10_string, jint constructor_index, jobject x10Token) 
{ 
        cactionConstructorDeclaration(env, clz, x10_string, constructor_index, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConstructorDeclarationHeader(JNIEnv *env, jclass clz,                                                                            jstring x10_string, 
                                                                           jboolean x10_is_public,
                                                                           jboolean x10_is_protected,
                                                                           jboolean x10_is_private,
                                                                           jint x10_numberOfTypeParameters,
                                                                           jint x10_numberOfArguments,
                                                                           jint x10_numberOfThrownExceptions,
                                                                           jobject x10Token) 
{

        cactionConstructorDeclarationHeader(env, clz, x10_string, x10_is_public, x10_is_protected, x10_is_private, x10_numberOfTypeParameters, x10_numberOfArguments, x10_numberOfThrownExceptions, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConstructorDeclarationEnd(JNIEnv *env, jclass clz, jint num_annotations, jint num_statements, jobject x10Token) 
{ 
        cactionConstructorDeclarationEnd(env, clz, num_annotations, num_statements, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionExplicitConstructorCall(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionExplicitConstructorCall(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionExplicitConstructorCallEnd(JNIEnv *env, jclass clz,                                                                          jboolean x10_is_implicit_super,
                                                                         jboolean x10_is_super,
                                                                         jboolean x10_has_qualification,
                                                                         jstring x10_package_name,
                                                                         jstring x10_type_name,
                                                                         jint constructor_index,
                                                                         jint x10_number_of_type_arguments,
                                                                         jint x10_number_of_arguments,
                                                                         jobject x10Token) 
{

        cactionExplicitConstructorCallEnd(env, clz, x10_is_implicit_super, x10_is_super, x10_has_qualification, x10_package_name, x10_type_name, constructor_index, x10_number_of_type_arguments, x10_number_of_arguments, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMethodDeclaration(JNIEnv *env, jclass clz, jstring x10_string, jint method_index, jobject x10Token) 
{ 
#if 0
        cactionMethodDeclaration(env, clz, x10_string, method_index, x10Token);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build a SgMemberFunctionDeclaration \n");

    SgName name = convertJavaStringToCxxString(env, x10_string);

    SgClassDefinition *class_definition = isSgClassDefinition(astX10ScopeStack.top());
//MH-20140403
    ROSE_ASSERT(class_definition != NULL);
    ROSE_ASSERT(! class_definition -> attributeExists("namespace"));

    ROSE_ASSERT(class_definition != NULL  && (! class_definition -> attributeExists("namespace")));
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) class_definition -> getAttribute("type_parameter_space");
// MH-20140624
cout << "class_definition=" << class_definition << ", attribute=" << attribute << endl;
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
//MH-20140704
//cout << "MODIFIER=" << method_declaration->get_declarationModifier().get_accessModifier().isPublic() << endl;
//MH-20140403
    astX10ScopeStack.push(type_space);
    astX10ScopeStack.push(method_definition);
//MH-20140312
//temtatively push class_definition into astX10ScopeStack to pass assertion for checking if 
//stack top is class definition or not
#if 0 // MH-20140320 comment out again
    astX10ScopeStack.push(class_definition);
#endif

    ROSE_ASSERT(astX10ScopeStack.top() -> get_parent() != NULL);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMethodDeclarationHeader(JNIEnv *env, jclass clz,                                                                       jstring x10_string,
                                                                      jboolean x10_is_abstract,
                                                                      jboolean x10_is_native,
                                                                      jboolean x10_is_static,
                                                                      jboolean x10_is_final,
                                                                      jboolean x10_is_synchronized,
                                                                      jboolean x10_is_public,
                                                                      jboolean x10_is_protected,
                                                                      jboolean x10_is_private,
                                                                      jboolean x10_is_strictfp,
                                                                      jint x10_numberOfTypeParameters,
                                                                      jint x10_numberOfArguments,
                                                                      jint x10_numberOfThrownExceptions,
                                                                      jobject x10Token) 
{

        cactionMethodDeclarationHeader(env, clz, x10_string, x10_is_abstract, x10_is_native, x10_is_static, x10_is_final, x10_is_synchronized, x10_is_public, x10_is_protected, x10_is_private, x10_is_strictfp, x10_numberOfTypeParameters, x10_numberOfArguments, x10_numberOfThrownExceptions, x10Token);
}


/*
 */
#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMethodDeclarationEnd(JNIEnv *env, jclass clz, int num_annotations, int num_statements, jobject x10Token) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMethodDeclarationEnd(JNIEnv *env, jclass clz, jint x10_numberOfStatements, jobject x10Token) 
#endif
{ 
#if 1
        cactionMethodDeclarationEnd(env, clz, 0, x10_numberOfStatements, x10Token);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Entering  cactionMethodDeclarationEnd (method) \n");

    // Pop the constructor body...
    ROSE_ASSERT(! astX10ScopeStack.empty());


    int numberOfStatements = x10_numberOfStatements;

    if (SgProject::get_verbose() > 0)
        printf ("In cactionMethodDeclarationEnd(): numberOfStatements = %d\n", numberOfStatements);


#if 0
    SgBasicBlock *method_body = astX10ScopeStack.popBasicBlock(); // pop the body block

//    for (int i = 0; i < numberOfStatements; i++) {
    for (int i = 0; i < 0; i++) {
         SgStatement *statement = astX10ComponentStack.popStatement();
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
    astX10ScopeStack.popFunctionDefinition();

    SgScopeStatement *type_space = isSgScopeStatement(astX10ScopeStack.pop());
    ROSE_ASSERT(type_space);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting  cactionMethodDeclarationEnd (method) \n");
#else
//MH-20140317
// pop unnecessary node to have classdefinition as the top of astX10ScopeStack 
//    astX10ScopeStack.pop();
#if 0
#endif

//    astX10ScopeStack.pop();

/*
    for (int i = 0; i < numberOfStatements-1; i++) {
        astX10ScopeStack.pop();
    }
*/

/*
    SgClassDefinition *class_definition = isSgClassDefinition(astX10ScopeStack.top());
    ROSE_ASSERT(class_definition != NULL  && (! class_definition -> attributeExists("namespace")));
*/

//    SgFunctionDefinition *memberFunctionDefinition = astX10ScopeStack.popFunctionDefinition();

    SgStatement *method_body = astX10ComponentStack.popStatement();
/*
    SgClassDefinition *class_definition = isSgClassDefinition(astX10ScopeStack.top());
    printf("class_definition=%p\n", class_definition);
    ROSE_ASSERT(class_definition && (! class_definition -> attributeExists("namespace")));
*/
//  SgStatement *function_declaration = astX10ComponentStack.popStatement();
    SgFunctionDefinition *memberFunctionDefinition = astX10ScopeStack.popFunctionDefinition();
//    SgFunctionDeclaration *memberFunctionDeclaration = astX10ComponentStack.popStatement();
//  SgFunctionDefinition *memberFunctionDefinition = memberFunctionDeclaration -> get_definition();
//  SgFunctionDefinition *memberFunctionDefinition = ((SgMemberFunctionDeclaration *)function_declaration) -> get_definition();
    memberFunctionDefinition -> set_body((SgBasicBlock *)method_body);
//  astX10ComponentStack.push(function_declaration);
//  astX10ScopeStack.push(((SgMemberFunctionDeclaration *)function_declaration)->get_class_scope());

    SgScopeStatement *type_space = isSgScopeStatement(astX10ScopeStack.pop());
    ROSE_ASSERT(type_space);

cout << "...in the stack at cactionMethodDeclarationEnd: " << endl;
for (std::list<SgScopeStatement*>::iterator i = astX10ScopeStack.begin(); i != astX10ScopeStack.end(); i++) {
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

    if (SgProject::get_verbose() > 0)
        printf ("Exiting  cactionMethodDeclarationEnd (method) \n");
#endif

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeParameterReference(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jint method_index, jstring x10_type_parameter_name, jobject x10Token) 
{ 
        cactionTypeParameterReference(env, clz, x10_package_name, x10_type_name, method_index, x10_type_parameter_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTypeReference(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jobject x10Visitor, jobject x10Token) 
{ 
    if (SgProject::get_verbose() > 0)
        printf ("Inside cactionTypeReference\n");

//    SgName package_name = convertX10PackageNameToCxxString(env, x10_package_name),
    SgName package_name = convertJavaStringToCxxString(env, x10_package_name),
           type_name = convertJavaStringToCxxString(env, x10_type_name);
        cout << "pkg=" << package_name << ", type=" << type_name << endl;
    SgType *type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);

        // looks up a user-defined type 
    if (type == NULL) {
                cout << "Looks up a user-defined type named " << type_name.str() << endl;
                jclass cls = get_class(env, x10Visitor);
                scopeMap[currentTypeName] = astX10ScopeStack;
                componentMap[currentTypeName] = astX10ComponentStack;
                jmethodID method_id = get_method(env, cls, "addFileIndex", "()Z");
                jboolean isOtherFilesExist = env->CallIntMethod(x10Visitor, method_id);

                if (isOtherFilesExist == JNI_TRUE) {
                        method_id = get_method(env, cls, "visitDeclarations", "()V");
                        env->CallIntMethod(x10Visitor, method_id);
#if 0
                        method_id = get_method(env, cls, "subFileIndex", "()V");
                        env->CallIntMethod(x10Visitor, method_id);
#endif
                        map<std::string, ScopeStack>::iterator it = scopeMap.find(currentTypeName);
                        if (it != scopeMap.end()) {
                                astX10ScopeStack = it->second;
                                cout << "FOUND existing scopeMap for " << currentTypeName << ", astX10ScopeStack=" << &astX10ScopeStack << endl;
                        }
                        else {
                                cout << "*NOT FOUND existing scopeMap for " << currentTypeName << ", astX10ScopeStack=" << &astX10ScopeStack << endl;
                        }
                        map<std::string, ComponentStack>::iterator it2 = componentMap.find(currentTypeName);
                        if (it2 != componentMap.end()) {
                                astX10ComponentStack = it2->second;
                                cout << "FOUND existing componentMap for " << currentTypeName << ", astX10ComponentStack=" << &astX10ComponentStack << endl;
                        }
                        else {
                                cout << "*NOT FOUND existing componentMap for " << currentTypeName << ", astX10ComponentStack=" << &astX10ComponentStack << endl;
                        }

                        type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
                }
                else {
                        cout << "User-defined type " <<  type_name << " was not found" << endl;
                        method_id = get_method(env, cls, "subFileIndex", "()V");
                        env->CallIntMethod(x10Visitor, method_id);
                }
                // If type is still NULL, it then looks up a library type
                if (type == NULL) {
                        cout << "Looks up a library type of " << type_name << endl;

/*
                        astX10ScopeStack = *(new ScopeStack);
                        astX10ScopeStack.push(::globalScope);
                        astX10ComponentStack = *(new ComponentStack);
*/
/*
                        string name = package_name.str();
                        name += name.size() == 0 ? "" : ".";
                        name += type_name.str();
                        currentTypeName = name;
*/
                        scopeMap[currentTypeName] = astX10ScopeStack;
                        componentMap[currentTypeName] = astX10ComponentStack;
                        cout << "TypeReference() for "<< type_name <<" : astX10ScopeStack's size=" << astX10ScopeStack.size() << ", name=" << currentTypeName  <<  endl;
                        cout << "TypeReference() for "<<type_name<<" : astX10ComponentStack's size=" << astX10ComponentStack.size() << ", name=" << currentTypeName  <<endl;
                        std::string callerName = currentTypeName;       

//              type = createDummyClass(env, NULL, x10_type_name, x10Token);
                        jmethodID method_id = get_method(env, cls, "searchFileList", "(Ljava/lang/String;Ljava/lang/String;)V");
                        env->CallIntMethod(x10Visitor, method_id, x10_package_name, x10_type_name);

                        currentTypeName = callerName;
                        map<std::string, ScopeStack>::iterator it = scopeMap.find(currentTypeName);
                        if (it != scopeMap.end()) {
                                astX10ScopeStack = it->second;
                        }
                        map<std::string, ComponentStack>::iterator it2 = componentMap.find(currentTypeName);
                        if (it2 != componentMap.end()) {
                                astX10ComponentStack = it2->second;
                        }
                        cout << "TypeReference2() for "<< type_name <<": astX10ScopeStack's size=" << astX10ScopeStack.size() << ", name=" << currentTypeName  <<  endl;
                        cout << "TypeReference2() for "<< type_name <<": astX10ComponentStack's size=" << astX10ComponentStack.size() << ", name=" << currentTypeName  <<endl;
                        
                        cout << "package=" << package_name << ", type=" << type_name << endl;
                        
                type = lookupTypeByName(package_name, type_name, 0 /* not an array - number of dimensions is 0 */);
                                
                        if (type != NULL)
                                cout << "Library type " <<  type_name << " was found" << endl;
                        else
                                cout << "Library type null" << endl;
                }
    }
#if 0
    ROSE_ASSERT(type != NULL);
#else
        if (type == NULL) {
                type = createDummyClass(env, clz, x10_package_name, x10_type_name, x10Token);
        }
#endif

// TODO: Remove this
//if (isSgClassType(type)) {
//SgClassType *class_type = isSgClassType(type);
//cout << "Came across type " << getTypeName(class_type) << endl;
//cout.flush();
//}

    astX10ComponentStack.push(type);

    if (SgProject::get_verbose() > 0)
        printf ("Exiting cactionTypeReference\n");
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedTypeReference(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jobject x10Token) 
{ 
        cactionQualifiedTypeReference(env, clz, x10_package_name, x10_type_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCatchArgument(JNIEnv *env, jclass clz, jstring x10_argument_name, jobject x10Token) 
{ 
        cactionCatchArgument(env, clz, x10_argument_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArgument(JNIEnv *env, jclass clz, jstring x10_argument_name, jobject x10Token) 
{ 
        cactionArgument(env, clz, x10_argument_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCatchArgumentEnd(JNIEnv *env, jclass clz, jint num_annotations, jstring x10_argument_name, jint num_types, jboolean x10_is_final, jobject x10Token) 
{ 
        cactionCatchArgumentEnd(env, clz, num_annotations, x10_argument_name, num_types, x10_is_final, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArgumentEnd(JNIEnv *env, jclass clz, jint num_annotations, jstring x10_argument_name, jobject x10Token) 
{ 
        cactionArgumentEnd(env, clz, num_annotations, x10_argument_name, x10Token);
}

/*
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayTypeReference(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jint x10_number_of_dimensions, jobject x10Token) 
{ 
        cactionArrayTypeReference(env, clz, x10_package_name, x10_type_name, x10_number_of_dimensions, x10Token);
}
*/

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayTypeReference(JNIEnv *env, jclass clz, jint x10_num_dimensions, jobject x10Token) 
{ 
        cactionArrayTypeReference(env, clz, x10_num_dimensions, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayTypeReferenceEnd(JNIEnv *env, jclass clz, jstring x10_name, int int1, jobject x10Token) 
{ 
        cactionArrayTypeReferenceEnd(env, clz, x10_name, int1, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMessageSend(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jstring x10_function_name, jobject x10Token) 
{ 
#if 0
        cactionMessageSend(env, clz, x10_package_name, x10_type_name, x10_function_name, x10Token);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build a function call (Message Send) \n");

    // Do Nothing on the way down !!!
#endif
}


/*
 * Java_x10rose_visit_JNI_cactionMessageSendEnd(JNIEnv *env, jclass,
                                                             jboolean x10_is_static,
                                                             jboolean x10_has_receiver,
                                                             jstring x10_function_name,
                                                             jint x10_number_of_parameters,
                                                             jint numTypeArguments,
                                                             jint numArguments,
                                                             jobject x10Token) {
 *
 */
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMessageSendEnd(JNIEnv *env, jclass clz,
                                                             jboolean x10_is_static,
                                                             jboolean x10_has_receiver,
                                                             jstring x10_function_name,
                                                             jint x10_number_of_parameters,
                                                             jint numTypeArguments,
                                                             jint numArguments,
//                                                           jobject x10Visitor, 
                                                             jobject x10Token) 
{
#if 0
        cactionMessageSendEnd(env, clz, x10_is_static, x10_has_receiver, x10_package_name, x10_type_name, java_method_name, method_index, numTypeArguments, numArguments, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of cactionMessageSendEnd() \n");

    bool is_static = x10_is_static,
         has_receiver = x10_has_receiver;

    SgName function_name  = convertJavaStringToCxxString(env, x10_function_name);
    int num_parameters = x10_number_of_parameters;

    //
    // TODO: Since array types are not properly represented as class types but as (C++) pointer types,
    // when an array type is used to call a function, we have to substitute the Object class for the
    // array type in question as the Object type is its (only) super type and contains all the method
    // that is invokable from an array.
    //
    SgType *containing_type = astX10ComponentStack.popType();
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
    SgType *return_type = astX10ComponentStack.popType(); // The return type of the function
    ROSE_ASSERT(return_type);
*/

    //
    // The astX10ComponentStack has all of the types of the parameters of the function being called. Note that
    // it is necessary to use the original types of the formal parameters of the function in order to find the
    // perfect match for the function.
    //
    list<SgType *> function_parameter_types;
    for (int i = 0; i < num_parameters; i++) { // reverse the arguments' order
        SgType *type = astX10ComponentStack.popType();
        function_parameter_types.push_front(type);
    }

// TODO: Remove this !!!
/*
*/
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

#if 1
    SgMemberFunctionSymbol *function_symbol = findFunctionSymbolInClass(targetClassScope, function_name, function_parameter_types, env);
#else
    // MH : For lazy lookup, but not finish the mechanism for going up to lookup the exact function defined along the class hierarchy
    SgMemberFunctionSymbol *function_symbol = findFunctionSymbolInClass(targetClassScope, function_name, function_parameter_types, env, x10Visitor);
#endif
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
    // The astX10ComponentStack has all of the arguments to the function call.
    SgExprListExp *arguments = new SgExprListExp();
    for (int i = 0; i < numArguments; i++) { // reverse the arguments' order
        SgExpression *expr = astX10ComponentStack.popExpression();
cout << "expr=" << expr << endl;
        arguments -> prepend_expression(expr);
    }
    setX10SourcePosition(arguments, env, x10Token);

    SgFunctionCallExp *function_call_exp = SageBuilder::buildFunctionCallExp(function_symbol, arguments);
    setX10SourcePosition(function_call_exp, env, x10Token);
    if (numTypeArguments > 0) {
        string parm_names = "";
        AstSgNodeListAttribute *attribute = new AstSgNodeListAttribute();
        for (int i = numTypeArguments - 1; i >= 0; i--) { // Note that we are reversing the type parameters here!
            SgType *type_argument = astX10ComponentStack.popType();
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
    SgNode *receiver = (has_receiver ? astX10ComponentStack.pop() : NULL);

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
            setX10SourcePosition(exprForFunction, env, x10Token);

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

    astX10ComponentStack.push(exprForFunction);

    if (SgProject::get_verbose() > 2)
        printf ("Leaving cactionMessageSendEnd(): %s\n", function_name.getString().c_str());
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionStringLiteral(JNIEnv *env, jclass clz, jstring x10_string, jobject x10Token) 
{ 
        cactionStringLiteral(env, clz, x10_string, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAllocationExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionAllocationExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAllocationExpressionEnd(JNIEnv *env, jclass clz, jboolean has_type, jint x10_num_arguments, jobject x10Token) 
{ 
        cactionAllocationExpressionEnd(env, clz, has_type, x10_num_arguments, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionANDANDExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionANDANDExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionANDANDExpressionEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionANDANDExpressionEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAnnotationMethodDeclaration(JNIEnv *env, jclass clz, jstring x10_string, jint method_index, jobject x10Token) 
{ 
        cactionAnnotationMethodDeclaration(env, clz, x10_string, method_index, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAnnotationMethodDeclarationEnd(JNIEnv *env, jclass clz, jstring x10_string, jint method_index, jint num_annotations, jboolean has_default, jobject x10Token) 
{ 
        cactionAnnotationMethodDeclarationEnd(env, clz, x10_string, method_index, num_annotations, has_default, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayAllocationExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionArrayAllocationExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayAllocationExpressionEnd(JNIEnv *env, jclass clz, jint x10_num_dimensions, jboolean x10_has_initializers, jobject x10Token) 
{ 
        cactionArrayAllocationExpressionEnd(env, clz, x10_num_dimensions, x10_has_initializers, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayInitializer(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionArrayInitializer(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayInitializerEnd(JNIEnv *env, jclass clz, jint x10_num_expressions, jobject x10Token) 
{ 
        cactionArrayInitializerEnd(env, clz, x10_num_expressions, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionArrayReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionArrayReferenceEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionArrayReferenceEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAssertStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionAssertStatement(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAssertStatementEnd(JNIEnv *env, jclass clz, jboolean hasExceptionArgument, jobject x10Token) 
{ 
        cactionAssertStatementEnd(env, clz, hasExceptionArgument, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAssignment(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 1
        cactionAssignment(env, clz, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Build an assignement statement (expression?) \n");

    // Nothing to do !!!
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAssignmentEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 1
        cactionAssignmentEnd(env, clz, x10Token);
#else
    // This function builds an assignement statement (not an expression).
    if (SgProject::get_verbose() > 2)
        printf ("Build an assignement statement (expression?) \n");

    binaryExpressionSupport<SgAssignOp>();

    setX10SourcePosition((SgLocatedNode *) astX10ComponentStack.top(), env, x10Token);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBinaryExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 0
        cactionBinaryExpression(env, clz, x10Token);
#else
            // I don't think we need this function.
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBinaryExpressionEnd(JNIEnv *env, jclass clz, jint x10_operator_kind, jobject x10Token) 
{ 
#if 0
        cactionBinaryExpressionEnd(env, clz, x10_operator_kind, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Build an Binary Expression End \n");

    // These are the operator code values directly from ECJ.
    enum ops {                                          // NO_STRINGIFY
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
                DOT_DOT                          = 20,
                EQ                                       = 21,
        OR_OR                = 100, // Handled by separate function 
        AND_AND              = 101, // Handled by separate function 
        LAST_OPERATOR
    };

    // printf ("PLUS = %d \n", PLUS);

    int operator_kind = x10_operator_kind;

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
//        case DOT_DOT:                    binaryExpressionSupport<SgDotDotExp>();        break;
        case EQ:                   binaryExpressionSupport<SgEqualityOp>();       break; 
        case OR_OR:                        binaryExpressionSupport<SgOrOp>();             break; 
        case AND_AND:                      binaryExpressionSupport<SgAndOp>();            break;

        // This may have to handled special in ROSE. ROSE does not represent the semantics,
        // and so this support my require a special operator to support Java in ROSE. For
        // now we will use the more common SgRshiftOp.
        case UNSIGNED_RIGHT_SHIFT: binaryExpressionSupport<SgJavaUnsignedRshiftOp>();         break;

        // These are handled through separate functions (not a BinaryExpression in ECJ).
//        case OR_OR:   ROSE_ASSERT(false); break;
//        case AND_AND: ROSE_ASSERT(false); break;

        default:
            printf ("Error: default reached in cactionBinaryExpressionEnd() operator_kind = %d \n", operator_kind);
            ROSE_ASSERT(false);
    }

    setX10SourcePosition((SgLocatedNode *) astX10ComponentStack.top(), env, x10Token);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBlock(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 0
        cactionBlock(env, clz, x10Token);
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
    if (isSgIfStmt(astX10ScopeStack.top())) {
        SgIfStmt *ifStatement = (SgIfStmt*) astX10ScopeStack.top();
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
    else if (isSgForStatement(astX10ScopeStack.top())) {
        // DQ (7/30/2011): Handle the case of a block after a SgForStatement
        // Because we build the SgForStatement on the stack and then the cactionBlock 
        // function is called, we have to detect and fixup the SgForStatement.
        SgForStatement *forStatement = (SgForStatement*) astX10ScopeStack.top();
        SgNullStatement *nullStatement = isSgNullStatement(forStatement -> get_loop_body());
        if (nullStatement != NULL) {
            block = SageBuilder::buildBasicBlock();
            ROSE_ASSERT(block != NULL);
            forStatement -> set_loop_body(block);
            delete nullStatement;
        }
                else {
            block = SageBuilder::buildBasicBlock();
            ROSE_ASSERT(block != NULL);
            forStatement -> set_loop_body(block);
                }
    }
    else {
        block = SageBuilder::buildBasicBlock();
    }
    ROSE_ASSERT(block != NULL);

    setX10SourcePosition(block, env, x10Token);

    block -> set_parent(astX10ScopeStack.top());
    ROSE_ASSERT(block -> get_parent() != NULL);

    astX10ScopeStack.push(block);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBlockEnd(JNIEnv *env, jclass clz, jint x10_numberOfStatements, jobject x10Token) 
{ 
#if 0
        cactionBlockEnd(env, clz, x10_numberOfStatements, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Pop the current SgBasicBlock scope off the scope stack...\n");

    int numberOfStatements = x10_numberOfStatements;

    if (SgProject::get_verbose() > 2)
        printf ("In cactionBlockEnd(): numberOfStatements = %d \n", numberOfStatements);

    ROSE_ASSERT(! astX10ScopeStack.empty());

    // DQ (7/30/2011): Take the block off of the scope stack and put it onto the statement stack so that we can 
    // process either blocks of other statements uniformally.
    SgBasicBlock *body = astX10ScopeStack.popBasicBlock();
    for (int i = 0; i  < numberOfStatements; i++) {
                // MH (6/24/2014) : Added condition because the size of astX10ComponentStack seems to 
                // be less than the numberOfStatements... should be fixed this
                if (astX10ComponentStack.size() == 0) {
                        printf("Size of astX10ComponentStack is zero\n");
                        break;
                }
        SgStatement *statement = astX10ComponentStack.popStatement();
        if (SgProject::get_verbose() > 2) {
            cerr << "(7) Adding statement "
                 << statement -> class_name()
                 << " to a block"
                 << endl;
            cerr.flush();
        }
        body -> prepend_statement(statement);
    }


    astX10ComponentStack.push(body);

        // MH (7/7/2014) : Added
    if (SgProject::get_verbose() > 2)
        printf ("Leaving cactionBlockEnd()\n");
//MH-20140317 added 
//MH-20140320 comment out again
//    astX10ScopeStack.push(body);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionBreakStatement(JNIEnv *env, jclass clz, jstring x10_string, jobject x10Token) 
{ 
        cactionBreakStatement(env, clz, x10_string, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCaseStatement(JNIEnv *env, jclass clz, jboolean hasCaseExpression, jobject x10Token) 
{ 
        cactionCaseStatement(env, clz, hasCaseExpression, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCaseStatementEnd(JNIEnv *env, jclass clz, jboolean hasCaseExpression, jobject x10Token) 
{ 
        cactionCaseStatementEnd(env, clz, hasCaseExpression, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCastExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionCastExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCastExpressionEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionCastExpressionEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCharLiteral(JNIEnv *env, jclass clz, jchar x10_char_value, jobject x10Token) 
{ 
        cactionCharLiteral(env, clz, x10_char_value, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionClassLiteralAccess(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionClassLiteralAccess(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionClassLiteralAccessEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionClassLiteralAccessEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionClinit(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionClinit(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConditionalExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionConditionalExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionConditionalExpressionEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionConditionalExpressionEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionContinueStatement(JNIEnv *env, jclass clz, jstring x10_string, jobject x10Token) 
{ 
        cactionContinueStatement(env, clz, x10_string, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompoundAssignment(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionCompoundAssignment(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCompoundAssignmentEnd(JNIEnv *env, jclass clz, jint x10_operator_kind, jobject x10Token) 
{ 
        cactionCompoundAssignmentEnd(env, clz, x10_operator_kind, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionDoStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionDoStatement(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionDoStatementEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionDoStatementEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionDoubleLiteral(JNIEnv *env, jclass clz, jdouble x10_value, jstring x10_source, jobject x10Token) 
{ 
        cactionDoubleLiteral(env, clz, x10_value, x10_source, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEmptyStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 0
        cactionEmptyStatement(env, clz, x10Token);
#else
    // Nothing to do;
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEmptyStatementEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 0
        cactionEmptyStatementEnd(env, clz, x10Token);
#else
    SgNullStatement *stmt = SageBuilder::buildNullStatement();
    ROSE_ASSERT(stmt != NULL);
    setX10SourcePosition(stmt, env, x10Token);
    astX10ComponentStack.push(stmt);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEqualExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionEqualExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionEqualExpressionEnd(JNIEnv *env, jclass clz, jint x10_operator_kind, jobject x10Token) 
{ 
        cactionEqualExpressionEnd(env, clz, x10_operator_kind, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionExtendedStringLiteral(JNIEnv *env, jclass clz, jstring x10_string, jobject x10Token) 
{ 
        cactionExtendedStringLiteral(env, clz, x10_string, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFalseLiteral(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 0
        cactionFalseLiteral(env, clz, x10Token);
#else
    SgExpression *expression = SageBuilder::buildBoolValExp(false);
    astX10ComponentStack.push(expression);
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
                                                                  jobject x10Token) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFieldDeclarationEnd(JNIEnv *env, jclass,
                                                                  jstring variableName,
                                                                  jboolean is_enum_field,
                                                                  jboolean has_initializer,
                                                                  jboolean x10_is_final,
                                                                  jboolean x10_is_private,
                                                                  jboolean x10_is_protected,
                                                                  jboolean x10_is_public,
                                                                  jboolean x10_is_volatile,
                                                                  jboolean x10_is_synthetic,
                                                                  jboolean x10_is_static,
                                                                  jboolean x10_is_transient,
                                                                  jobject x10Token) 
#endif
{
#if 0
        cactionFieldDeclarationEnd(env, clz, variable_name, num_annotations, is_enum_constant, has_initializer, is_final, is_private, is_protected, is_public, is_volatile, is_synthetic, is_static, is_transient, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of cactionFieldDeclarationEnd() \n");

    SgName name = convertJavaStringToCxxString(env, variableName);

    // if (SgProject::get_verbose() > 2)
    //      printf ("hasInitializer = %s (but not used except in bottom up processing) \n", hasInitializer ? "true" : "false");

    bool isFinal     = x10_is_final;
    bool isPrivate   = x10_is_private;
    bool isProtected = x10_is_protected;
    bool isPublic    = x10_is_public;
    bool isVolatile  = x10_is_volatile;
    bool isSynthetic = x10_is_synthetic;
    bool isStatic    = x10_is_static;
    bool isTransient = x10_is_transient;

    if (SgProject::get_verbose() > 2)
        printf ("Building a Field declaration for name = %s \n", name.str());

    SgExpression *initializer_expression = (((! is_enum_field) && has_initializer) ? astX10ComponentStack.popExpression() : NULL);

    SgScopeStatement *outer_scope = astX10ScopeStack.top();
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
        SgType *type = astX10ComponentStack.popType();
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
        variableDeclaration -> get_file_info() -> display("source position in cactionFieldDeclarationEnd(): debug");

    // DQ (9/5/2011): Added from previous cactionFieldDeclarationEnd() function.
    if (has_initializer) {
        SgInitializer *initializer = SageBuilder::buildAssignInitializer(initializer_expression);
        ROSE_ASSERT(initializer != NULL);

        setX10SourcePosition(initializer_expression, env, x10Token);
        setX10SourcePosition(initializer, env, x10Token);

        initializer -> set_parent(initializedName);
   
        initializer -> get_file_info() -> display("cactionFieldDeclarationEnd()");

        initializedName -> set_initptr(initializer);
        initializer -> set_parent(initializedName);
     }

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFieldReference(JNIEnv *env, jclass clz, jstring x10_field, jobject x10Token) 
{ 
#if 0
        cactionFieldReference(env, clz, x10_field, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of cactionFieldReference() \n");
    // Nothing to do !!!
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFieldReferenceEnd(JNIEnv *env, jclass clz, jboolean explicit_type, jstring x10_field, jobject x10Token) 
{ 
#if 1
        cactionFieldReferenceEnd(env, clz, explicit_type, x10_field, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of cactionFieldReferenceEnd() \n");

    SgName field_name = convertJavaStringToCxxString(env, x10_field);

    if (SgProject::get_verbose() > 0)
        printf ("Building a Field reference for name = %s \n", field_name.str());

    SgType *receiver_type = (explicit_type ? astX10ComponentStack.popType() : NULL);
    SgNode *prefix = astX10ComponentStack.pop();
cout << "receiver_type=" << getTypeName(receiver_type) << ", prefix=" << prefix << endl;
    SgExpression *receiver = isSgExpression(prefix);
/*
    ROSE_ASSERT(receiver);
    ROSE_ASSERT(isSgType(prefix));
    ROSE_ASSERT(receiver || isSgType(prefix));
*/
    if (! explicit_type) {
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
//    ROSE_ASSERT(receiver || type);
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
        setX10SourcePosition(field, env, x10Token);
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
/*
*/
        ROSE_ASSERT(variable_symbol);
        SgVarRefExp *field = SageBuilder::buildVarRefExp(variable_symbol);
        ROSE_ASSERT(field != NULL);
        setX10SourcePosition(field, env, x10Token);

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

    astX10ComponentStack.push(result);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFloatLiteral(JNIEnv *env, jclass clz, jfloat x10_value, jstring x10_source, jobject x10Token) 
{ 
        cactionFloatLiteral(env, clz, x10_value, x10_source, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionForeachStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionForeachStatement(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionForeachStatementEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
    if (SgProject::get_verbose() > 2)
        printf ("Inside of cactionForEachStatementEnd() \n");

    SgStatement *action = astX10ComponentStack.popStatement();       // Get the action statement

    SgExpression *collection = astX10ComponentStack.popExpression(); // Get the collection expr

    SgVariableDeclaration *variable_declaration = isSgVariableDeclaration(astX10ComponentStack.popStatement()); // Get the declaration statement

    // Build the final Foreach Statement
    SgJavaForEachStatement *foreach_statement = astX10ScopeStack.popJavaForEachStatement();
   
    // DQ (9/3/2011): Change API as suggested by Philippe.
    foreach_statement -> set_element(variable_declaration);
    variable_declaration -> set_parent(foreach_statement);
   
    foreach_statement -> set_collection(collection);
    collection -> set_parent(foreach_statement);
    foreach_statement -> set_loop_body(action);
    action -> set_parent(foreach_statement);

    // Pushing 'foreach' on the statement stack
    astX10ComponentStack.push(foreach_statement);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionForStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionForStatement(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionForStatementEnd(JNIEnv *env, jclass clz, jint num_initializations, jboolean has_condition, jint num_increments, jobject x10Token) 
{ 
        cactionForStatementEnd(env, clz, num_initializations, has_condition, num_increments, x10Token);
}

#if 0
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionIfStatement(JNIEnv *env, jclass clz, jboolean has_false_body, jobject x10Token) 
#else
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionIfStatement(JNIEnv *env, jclass, jobject x10Token) 
#endif
{ 
#if 0
        cactionIfStatement(env, clz, has_false_body, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of cactionIfStatement() \n");

    // Build a SgIfStatement and push it onto the stack with a true block.

    // We need a predicate to use to call the SageBuilder::buildIfStmt() function.  So build a SgNullExpression for now. 
    SgNullStatement *temp_conditional = SageBuilder::buildNullStatement();
    SgNullStatement *true_block = SageBuilder::buildNullStatement();
    ROSE_ASSERT(true_block != NULL);

    SgIfStmt *ifStatement = SageBuilder::buildIfStmt(temp_conditional, true_block, NULL);
    ROSE_ASSERT(ifStatement != NULL);

    ifStatement -> set_parent(astX10ScopeStack.top());

    setX10SourcePosition(ifStatement, env, x10Token);

    // Push the SgIfStmt onto the stack, but not the true block.
    astX10ScopeStack.push(ifStatement);
    ROSE_ASSERT(astX10ScopeStack.top() -> get_parent() != NULL);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionIfStatementEnd(JNIEnv *env, jclass clz, jboolean has_false_body, jobject x10Token) 
{ 
#if 0
        cactionIfStatementEnd(env, clz, has_false_body, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of cactionIfStatementEnd() \n");

    // There should be a predicate on the stack for us to use as a final step in construction of the SgIfStmt.

    ROSE_ASSERT(! astX10ScopeStack.empty());

    SgIfStmt *ifStatement = astX10ScopeStack.popIfStmt();
    ROSE_ASSERT(ifStatement -> get_parent() != NULL);

    setX10SourcePosition(ifStatement, env, x10Token);

    // If there are two required then the first is for the false branch.
    if (has_false_body) {
        SgStatement *false_body = astX10ComponentStack.popStatement();
        ifStatement -> set_false_body(false_body);
        false_body -> set_parent(ifStatement);
        ROSE_ASSERT(false_body -> get_parent() != NULL);
    }

    SgStatement *true_body = astX10ComponentStack.popStatement();
    ifStatement -> set_true_body(true_body);
    true_body -> set_parent(ifStatement);
    ROSE_ASSERT(true_body -> get_parent() != NULL);

    SgExpression *condititonalExpr = astX10ComponentStack.popExpression();

    SgExprStatement *exprStatement = SageBuilder::buildExprStatement(condititonalExpr);

    setX10SourcePosition(exprStatement, env, x10Token);

    ROSE_ASSERT(exprStatement != NULL);
    ROSE_ASSERT(condititonalExpr -> get_parent() != NULL);

    ifStatement -> set_conditional(exprStatement);

    ROSE_ASSERT(exprStatement -> get_parent() == NULL);
    exprStatement -> set_parent(ifStatement);
    ROSE_ASSERT(exprStatement -> get_parent() != NULL);

    // DQ (7/30/2011): Take the block off of the scope stack and put it onto the statement stack so that we can 
    // process either blocks of other statements uniformally.
    astX10ComponentStack.push(ifStatement);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionImportReference(JNIEnv *env, jclass clz,                                                               jboolean x10_is_static,
                                                              jstring x10_qualified_name,
                                                              jboolean x10_contains_wildcard,
                                                              jobject x10Token) 
{
#if 1
        cactionImportReference(env, clz, x10_is_static, x10_qualified_name, x10_contains_wildcard, x10Token);
#else
    // This is the import statement.  The semantics is to include the named file and add its
    // declarations to the global scope so that they can be referenced by the current file.
    // The import directive tells the compiler where to look for the class definitions
    // when it comes upon a class that it cannot find in the default java.lang package.

    if (SgProject::get_verbose() > 1)
        printf ("Inside of Java_x10rose_visit_JNI_cactionImportReference() \n");

    bool is_static = x10_is_static;

    SgName package_name = convertX10PackageNameToCxxString(env, x10_package_name),
           type_name = convertJavaStringToCxxString(env, x10_type_name),
           name_suffix = convertJavaStringToCxxString(env, x10_name_suffix),
           qualifiedName = convertJavaStringToCxxString(env, x10_package_name) +
                           string(package_name.getString().size() && type_name.getString().size() ? "." : "") +
                           type_name.getString() +
                           (name_suffix.getString().size() ? ("." + name_suffix.getString()) : "");

    bool contains_wildcard = x10_contains_wildcard;

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
    importStatement -> set_parent(astX10ScopeStack.top()); // We also have to set the parent so that the stack debugging output will work.
    setX10SourcePosition(importStatement, env, x10Token);

    if (is_static) {
        importStatement -> get_declarationModifier().get_storageModifier().setStatic();
    }

    ROSE_ASSERT(! astX10ScopeStack.empty());

    // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
    // printf ("Previously calling appendStatement in cactionImportReference() \n");
    // appendStatement(importStatement);
    astX10ComponentStack.push(importStatement);
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
        SgScopeStatement *currentScope = astX10ScopeStack.top();
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

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInitializer(JNIEnv *env, jclass clz, jboolean x10_is_static, jstring x10_string, jint initializer_index, jobject x10Token) 
{ 
        cactionInitializer(env, clz, x10_is_static, x10_string, initializer_index, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInitializerEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionInitializerEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInstanceOfExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionInstanceOfExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionInstanceOfExpressionEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionInstanceOfExpressionEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionIntLiteral(JNIEnv *env, jclass clz, jint x10_value, jstring x10_source, jobject x10Token) 
{ 
#if 0
        cactionIntLiteral(env, clz, x10_value, x10_source, x10Token);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build IntVal \n");

    ROSE_ASSERT(! astX10ScopeStack.empty());

    int value = x10_value;
    SgName source = convertJavaStringToCxxString(env, x10_source);

    // printf ("Building an integer value expression = %d = %s \n", value, valueString.c_str());

    SgIntVal *integerValue = new SgIntVal(value, source);
    ROSE_ASSERT(integerValue != NULL);

    setX10SourcePosition(integerValue, env, x10Token);

    astX10ComponentStack.push(integerValue);
#endif
}

/*
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadoc(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadoc(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocAllocationExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocAllocationExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocAllocationExpressionClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocAllocationExpressionClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArgumentExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocArgumentExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArgumentExpressionClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocArgumentExpressionClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArrayQualifiedTypeReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocArrayQualifiedTypeReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocArrayQualifiedTypeReferenceClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArraySingleTypeReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocArraySingleTypeReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocArraySingleTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocArraySingleTypeReferenceClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocFieldReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocFieldReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocFieldReferenceClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocFieldReferenceClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocImplicitTypeReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocImplicitTypeReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocImplicitTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocImplicitTypeReferenceClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocMessageSend(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocMessageSend(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocMessageSendClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocMessageSendClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocQualifiedTypeReference(JNIEnv *env, jclass clz, jobject x10Token)  
{ 
        cactionJavadocQualifiedTypeReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocQualifiedTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocQualifiedTypeReferenceClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocReturnStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocReturnStatement(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocReturnStatementClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocReturnStatementClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocSingleNameReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocSingleNameReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocSingleNameReferenceClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocSingleNameReferenceClassScope(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocSingleTypeReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocSingleTypeReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionJavadocSingleTypeReferenceClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionJavadocSingleTypeReferenceClassScope(env, clz, x10Token);
}
*/

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLabeledStatement(JNIEnv *env, jclass clz, jstring labelName, jobject x10Token) 
{ 
        cactionLabeledStatement(env, clz, labelName, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLabeledStatementEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionLabeledStatementEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLocalDeclaration(JNIEnv *env, jclass clz, jint num_annotations, jstring x10_variable_name, jboolean is_final, jobject x10Token) 
{ 
#if 1
        cactionLocalDeclaration(env, clz, num_annotations, x10_variable_name, is_final, x10Token);
#else
    SgName variable_name = convertJavaStringToCxxString(env, x10_variable_name);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of cactionLocalDeclaration() for %s\n", variable_name.getString().c_str());

    SgType *type = astX10ComponentStack.popType();
    ROSE_ASSERT(type);

    // Note that the type should have already been built and should be on the astX10ComponentStack.
    SgVariableDeclaration *variable_declaration = SageBuilder::buildVariableDeclaration(variable_name, type, NULL, astX10ScopeStack.top());
    ROSE_ASSERT(variable_declaration != NULL);
    variable_declaration -> set_parent(astX10ScopeStack.top());
    setX10SourcePosition(variable_declaration, env, x10Token);

    if (is_final) {
        variable_declaration -> get_declarationModifier().setFinal();
    }

    //
    //
    //
    if (num_annotations > 0) {
        AstSgNodeListAttribute *annotations_attribute = new AstSgNodeListAttribute();
        for (int i = num_annotations - 1; i >= 0; i--) {
            SgExpression *annotation = astX10ComponentStack.popExpression();
            annotation -> set_parent(variable_declaration);
            annotations_attribute -> setNode(annotation, i);
        }
        variable_declaration -> setAttribute("annotations", annotations_attribute);
    }

    if (SgProject::get_verbose() > 0)
        printf ("Leaving cactionLocalDeclaration() for %s \n", variable_name.getString().c_str());
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLocalDeclarationEnd(JNIEnv *env, jclass clz, jstring variable_name, jboolean hasInitializer, jobject x10Token) 
{ 
#if 1
        cactionLocalDeclarationEnd(env, clz, variable_name, hasInitializer, x10Token);
#else

    if (SgProject::get_verbose() > 0)
        printf ("Inside of cactionLocalDeclarationEnd() \n");

    SgName name = convertJavaStringToCxxString(env, variable_name);

    if (SgProject::get_verbose() > 2)
        printf ("Building a local variable declaration for name = %s \n", name.str());

    SgVariableSymbol *variable_symbol = astX10ScopeStack.top() -> lookup_variable_symbol(name);
    SgInitializedName *initialized_name = variable_symbol -> get_declaration();
    ROSE_ASSERT(initialized_name);
    ROSE_ASSERT(initialized_name -> get_scope() != NULL);
    initialized_name -> setAttribute("type", new AstRegExAttribute(getTypeName(initialized_name -> get_type())));
    setX10SourcePosition(initialized_name, env, x10Token);

    //
    //
    //
    if (hasInitializer) {
        SgExpression *initializer_expression = (hasInitializer ? astX10ComponentStack.popExpression() : NULL);
// TODO: Remove this !
/*
cout << "The expression is a "
<< initializer_expression -> class_name()
<< endl
<< "; The top of the stack is a "
     << (isSgClassDefinition(astX10ComponentStack.top()) ? isSgClassDefinition(astX10ComponentStack.top()) -> get_qualified_name().getString() : astX10ComponentStack.top() -> class_name())
<< endl;
cout.flush();
*/
        SgAssignInitializer *initializer = SageBuilder::buildAssignInitializer(initializer_expression, initialized_name -> get_type());
        ROSE_ASSERT(initializer != NULL);

        setX10SourcePosition(initializer, env, x10Token);
        setX10SourcePosition(initializer_expression, env, x10Token);

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
    astX10ComponentStack.push(variable_declaration);

    if (SgProject::get_verbose() > 0)
        variable_declaration -> get_file_info() -> display("source position in cactionLocalDeclarationEnd(): debug");
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionLongLiteral(JNIEnv *env, jclass clz, jlong x10_value, jstring x10_source, jobject x10Token) 
{ 
#if 0
        cactionLongLiteral(env, clz, x10_value, x10_source, x10Token);
#else
    if (SgProject::get_verbose() > 0)
        printf ("Build LongVal \n");

    ROSE_ASSERT(! astX10ScopeStack.empty());

    long value = x10_value;
    SgName source = convertJavaStringToCxxString(env, x10_source);

//   printf ("Building an integer value expression = %d = %s \n", value, valueString.c_str());

    SgLongIntVal *longValue = new SgLongIntVal(value, source);
    ROSE_ASSERT(longValue != NULL);

    setX10SourcePosition(longValue, env, x10Token);
// MH-20140407 confirmed that value is properly set to longValue, although DOT file does not have information about value...
//printf("longValue.get_value()=%d \n", longValue->get_value());
    astX10ComponentStack.push(longValue);

#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMarkerAnnotationEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionMarkerAnnotationEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionMemberValuePairEnd(JNIEnv *env, jclass clz, jstring x10_name, jobject x10Token) 
{ 
        cactionMemberValuePairEnd(env, clz, x10_name, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionStringLiteralConcatenation(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionStringLiteralConcatenation(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionNormalAnnotationEnd(JNIEnv *env, jclass clz, jint num_member_value_pairs, jobject x10Token) 
{ 
        cactionNormalAnnotationEnd(env, clz, num_member_value_pairs, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionNullLiteral(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionNullLiteral(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionORORExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionORORExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionORORExpressionEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionORORExpressionEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionParameterizedTypeReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionParameterizedTypeReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionParameterizedTypeReferenceEnd(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jboolean has_type_arguments, int x10_num_type_arguments, jobject x10Token) 
{ 
        cactionParameterizedTypeReferenceEnd(env, clz, x10_package_name, x10_type_name, has_type_arguments, x10_num_type_arguments, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionParameterizedQualifiedTypeReferenceEnd(JNIEnv *env, jclass clz, jstring x10_type_name, jboolean has_type_arguments, int x10_num_type_arguments, jobject x10Token) 
{ 
        cactionParameterizedQualifiedTypeReferenceEnd(env, clz, x10_type_name, has_type_arguments, x10_num_type_arguments, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPostfixExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionPostfixExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPostfixExpressionEnd(JNIEnv *env, jclass clz, jint x10_operator_kind, jobject x10Token) 
{ 
        cactionPostfixExpressionEnd(env, clz, x10_operator_kind, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPrefixExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionPrefixExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionPrefixExpressionEnd(JNIEnv *env, jclass clz, jint x10_operator_kind, jobject x10Token) 
{ 
        cactionPrefixExpressionEnd(env, clz, x10_operator_kind, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedAllocationExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionQualifiedAllocationExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedAllocationExpressionEnd(JNIEnv *env, jclass clz, jboolean has_type, jboolean x10_contains_enclosing_instance, jint x10_num_arguments, jboolean x10_is_anonymous, jobject x10Token) 
{ 
        cactionQualifiedAllocationExpressionEnd(env, clz, has_type, x10_contains_enclosing_instance, x10_num_arguments, x10_is_anonymous, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedSuperReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionQualifiedSuperReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedSuperReferenceEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionQualifiedSuperReferenceEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedThisReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionQualifiedThisReference(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionQualifiedThisReferenceEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionQualifiedThisReferenceEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionReturnStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 0
        cactionReturnStatement(env, clz, x10Token);
#else
    // Nothing to do !!!
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionReturnStatementEnd(JNIEnv *env, jclass clz, jboolean has_expression, jobject x10Token) 
{ 
#if 0
        cactionReturnStatementEnd(env, clz, has_expression, x10Token);
#else
    if (SgProject::get_verbose() > 2)
        printf ("Inside of cactionReturnStatementEnd() \n");

    // Build the Return Statement
    SgExpression *expression = (has_expression ? astX10ComponentStack.popExpression() : NULL);
    SgReturnStmt *returnStatement = SageBuilder::buildReturnStmt_nfi(expression);
    ROSE_ASSERT(has_expression || returnStatement -> get_expression() == NULL); // TODO: there is an issue with the implementation of buildReturnStmt()...
    setX10SourcePosition(returnStatement, env, x10Token);

    // Pushing 'return' on the statement stack
    astX10ComponentStack.push(returnStatement);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSingleMemberAnnotationEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionSingleMemberAnnotationEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSingleNameReference(JNIEnv *env, jclass clz, jstring x10_package_name, jstring x10_type_name, jstring x10_name, jobject x10Token) 
{ 
#if 0
        cactionSingleNameReference(env, clz, x10_package_name, x10_type_name, x10_name, x10Token);
#else
    SgName package_name = convertX10PackageNameToCxxString(env, x10_package_name),
           type_name = convertJavaStringToCxxString(env, x10_type_name),
           name = convertJavaStringToCxxString(env, x10_name);
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
        variable_symbol = lookupVariableByName(env, name);
    }

    ROSE_ASSERT(variable_symbol);
    SgVarRefExp *varRefExp = SageBuilder::buildVarRefExp(variable_symbol);
    ROSE_ASSERT(varRefExp != NULL);
    if (SgProject::get_verbose() > 0)
        printf ("In cactionSingleNameReference(): varRefExp = %p type = %p = %s \n", varRefExp, varRefExp -> get_type(), varRefExp -> get_type() -> class_name().c_str());

    setX10SourcePosition(varRefExp, env, x10Token);

#if 0
    ROSE_ASSERT(! varRefExp -> get_file_info() -> isTransformation());
    ROSE_ASSERT(! varRefExp -> get_file_info() -> isCompilerGenerated());
#endif
    astX10ComponentStack.push(varRefExp);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSuperReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 0
        cactionSuperReference(env, clz, x10Token);
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

    astX10ComponentStack.push(superExp);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSwitchStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionSwitchStatement(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSwitchStatementEnd(JNIEnv *env, jclass clz, jint numCases, jboolean hasDefaultCase, jobject x10Token) 
{ 
        cactionSwitchStatementEnd(env, clz, numCases, hasDefaultCase, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSynchronizedStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionSynchronizedStatement(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSynchronizedStatementEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionSynchronizedStatementEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionThisReference(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 0
        cactionThisReference(env, clz, x10Token);
#else
    SgClassDefinition *class_definition = getCurrentTypeDefinition();
    ROSE_ASSERT(! class_definition -> attributeExists("namespace"));

    string className = class_definition -> get_declaration() -> get_name();
    // printf ("Current class for ThisReference is: %s \n", className.c_str());

    SgClassSymbol *class_symbol = isSgClassSymbol(class_definition -> get_declaration() -> search_for_symbol_from_symbol_table());
    ROSE_ASSERT(class_symbol != NULL);

    SgThisExp *thisExp = SageBuilder::buildThisExp(class_symbol);
    ROSE_ASSERT(thisExp != NULL);

    astX10ComponentStack.push(thisExp);
#endif
}

/*
JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionThisReferenceClassScope(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionThisReferenceClassScope(env, clz, x10Token);
}
*/

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionThrowStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionThrowStatement(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionThrowStatementEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionThrowStatementEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTrueLiteral(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
#if 0
        cactionTrueLiteral(env, clz, x10Token);
#else
    SgExpression *expression = SageBuilder::buildBoolValExp(true);
    astX10ComponentStack.push(expression);
#endif
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionCatchBlockEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionCatchBlockEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTryStatement(JNIEnv *env, jclass clz, jint numCatchBlocks, jboolean hasFinallyBlock, jobject x10Token) 
{ 
        cactionTryStatement(env, clz, numCatchBlocks, hasFinallyBlock, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionTryStatementEnd(JNIEnv *env, jclass clz, jint num_resources, jint num_catch_blocks, jboolean has_finally_block, jobject x10Token) 
{ 
        cactionTryStatementEnd(env, clz, num_resources, num_catch_blocks, has_finally_block, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUnaryExpression(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionUnaryExpression(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionUnaryExpressionEnd(JNIEnv *env, jclass clz, jint x10_operator_kind, jobject x10Token) 
{ 
        cactionUnaryExpressionEnd(env, clz, x10_operator_kind, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionWhileStatement(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionWhileStatement(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionWhileStatementEnd(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionWhileStatementEnd(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionWildcard(JNIEnv *env, jclass clz, jobject x10Token) 
{ 
        cactionWildcard(env, clz, x10Token);
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionWildcardEnd(JNIEnv *env, jclass clz, jboolean is_unbound, jboolean has_extends_bound, jboolean has_super_bound, jobject x10Token) 
{ 
        cactionWildcardEnd(env, clz, is_unbound, has_extends_bound, has_super_bound, x10Token);
}

JNIEXPORT jstring JNICALL Java_x10rose_visit_JNI_cactionGetCurrentClassName (JNIEnv *env, jclass clz)
{
        int len = currentTypeName.length();
        char *typeName = new char[len + 1];
        memcpy(typeName, currentTypeName.c_str(), len + 1);
        return env->NewStringUTF(typeName);     
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSetCurrentClassName (JNIEnv *env, jclass clz, jstring type_name)
{

        scopeMap[currentTypeName] = astX10ScopeStack;
        componentMap[currentTypeName] = astX10ComponentStack;

        SgName type = convertJavaStringToCxxString(env, (jstring)env->NewGlobalRef(type_name));
        currentTypeName = type.str();
        map<std::string, ScopeStack>::iterator it = scopeMap.find(currentTypeName);
        if (it != scopeMap.end()) {
                astX10ScopeStack = it->second;
cout << "FOUND existing scopeMap for " << currentTypeName << ", astX10ScopeStack=" << &astX10ScopeStack << endl;
        }
        else {
                astX10ScopeStack = *(new ScopeStack);
                astX10ScopeStack.push(::globalScope);
                scopeMap[currentTypeName] = astX10ScopeStack;
cout << "NOT FOUND existing scopeMap for " << currentTypeName << ", astX10ScopeStack=" << &astX10ScopeStack << endl;
        }

        map<std::string, ComponentStack>::iterator it2 = componentMap.find(currentTypeName);
        if (it2 != componentMap.end()) {
                astX10ComponentStack = it2->second;
cout << "FOUND existing componentMap for " << currentTypeName << ", astX10ComponentStack=" << &astX10ComponentStack << endl;
        }       
        else {
                astX10ComponentStack = *(new ComponentStack);
                componentMap[currentTypeName] = astX10ComponentStack;
cout << "NOT FOUND existing componentMap for " << currentTypeName << ", astX10ComponentStack=" << &astX10ComponentStack << endl;
        }
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionSetCurrentFilePath (JNIEnv *env, jclass clz, jstring file)
{
        currentFilePath = (jstring)env->NewGlobalRef(file);
        string full_file_name = convertJavaStringToCxxString(env, currentFilePath);
    ::currentSourceFile = isSgSourceFile((*::project)[full_file_name]);
}


JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAsync (JNIEnv *env, jclass clz, jobject x10Token)
{
        // do nothing
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAsyncEnd (JNIEnv *env, jclass clz, jobject x10Token) 
{
        if (SgProject::get_verbose() > 0)
                printf("Inside of cactionAsyncEnd\n");

    SgBasicBlock *async_body = (SgBasicBlock *)astX10ComponentStack.popStatement();
        SgAsyncStmt *async_stmt = SageBuilder::buildAsyncStmt(async_body);
    setX10SourcePosition(async_stmt, env, x10Token);
    async_stmt -> set_parent(astX10ScopeStack.top());
        
    astX10ComponentStack.push(async_stmt); 
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFinish (JNIEnv *env, jclass clz, jobject x10Token)
{
        // do nothing
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionFinishEnd (JNIEnv *env, jclass clz, jobject x10Token) 
{
        if (SgProject::get_verbose() > 0)
                printf("Inside of cactionFinishEnd");
        
    SgBasicBlock *finish_body = (SgBasicBlock *)astX10ComponentStack.popStatement();
        SgFinishStmt *finish_stmt = SageBuilder::buildFinishStmt(finish_body);
    setX10SourcePosition(finish_stmt, env, x10Token);
        finish_stmt -> set_parent(astX10ScopeStack.top());
        
    astX10ComponentStack.push(finish_stmt); 
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAt (JNIEnv *env, jclass clz, jobject x10Token)
{
        // do nothing
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionAtEnd (JNIEnv *env, jclass clz, jobject x10Token) 
{
        if (SgProject::get_verbose() > 0)
                printf("Inside of cactionAtEnd\n");
        
    SgBasicBlock *at_body = (SgBasicBlock *)astX10ComponentStack.popStatement();
        SgExpression *at_expr = (SgExpression *)astX10ComponentStack.popExpression();
        
        SgAtStmt *at_stmt = SageBuilder::buildAtStmt(at_expr, at_body);
    setX10SourcePosition(at_stmt, env, x10Token);
        
    astX10ComponentStack.push(at_stmt); 
}

JNIEXPORT void JNICALL Java_x10rose_visit_JNI_cactionHere (JNIEnv *env, jclass clz, jobject x10Token) 
{
        SgExpression *hereExp = SageBuilder::buildHereExpression();
        astX10ComponentStack.push(hereExp);
    setX10SourcePosition(hereExp, env, x10Token);
}

#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// Java support for calling C/C++.
#include <jni.h>

// Support functions declaration of function defined in this file.
#include "jni_JavaSourceCodePosition.h"
#include "token.h"
#include "java_support.h"
#include "jni_token.h"
#include "Utf8.h"

#include <inttypes.h> // for PRIuPTR

using namespace Rose::Frontend::Java;
using namespace Rose::Frontend::Java::Ecj;

SgProject *project = NULL;
SgGlobal *globalScope = NULL;
SgSourceFile *currentSourceFile = NULL;
SgClassDefinition *ObjectClassDefinition = NULL;

//
// This function converts a Java string into its Utf8 representation.
//
string javaStringToUtf8(const jstring &java_string) {
    //
    // TODO: Replace code below by this call...
    //
    // string value = ::currentEnvironment -> GetStringUTFChars(java_string, NULL);
    //
    string value = "";
    const jchar *raw = ::currentEnvironment -> GetStringChars(java_string, NULL);
    if (raw != NULL) {
        jsize len = ::currentEnvironment -> GetStringLength(java_string);
        for (const jchar *temp = raw; len > 0; len--,temp++) {
            value += Utf8::getUtf8String(*temp);
        }
        ::currentEnvironment -> ReleaseStringChars(java_string, raw);
    }

    return value;
}


//
// This function converts a Java string into its Utf8 representation.
//
/*
string convertJavaStringValToUtf8(JNIEnv *env, const jstring &java_string) {
    string value = "";

    const jchar *raw = env -> GetStringChars(java_string, NULL);
    if (raw != NULL) {
        jsize len = env -> GetStringLength(java_string);
        for (const jchar *temp = raw; len > 0; len--,temp++) {
            value += Utf8::getUtf8String(*temp);
        }
        env -> ReleaseStringChars(java_string, raw);
    }

    return value;
}
*/


//
// This function is needed in order to bypass a serious bug in the Rose function SgScopeStatement::lookup_class_symbol(...).
//
SgClassSymbol *lookupClassSymbolInScope(SgScopeStatement *scope, const SgName &type_name) {
    SgClassSymbol *class_symbol = scope -> lookup_class_symbol(type_name);
//
// TODO: Remove this when ROSE bug is fixed !!!
//
/*
if (class_symbol != NULL) {
SgScopeStatement *found_scope = class_symbol -> get_declaration() -> get_scope();
if (found_scope != scope) {
cout << "ROSE bug: The type named "
     << isSgClassType(class_symbol -> get_type()) -> get_name().getString()
     << " belonging to scope "
     << (isSgClassDefinition(found_scope) ? isSgClassDefinition(found_scope) -> get_qualified_name().getString() : found_scope -> class_name())
     << " was found in a call to lookup_class_symbol() with scope "
     << (isSgClassDefinition(scope) ? isSgClassDefinition(scope) -> get_qualified_name().getString() : scope -> class_name())
<< endl;
cout.flush();
class_symbol = NULL; // Ignore this error !!!?
}
}
*/
    ROSE_ASSERT(class_symbol == NULL || class_symbol -> get_declaration() -> get_scope() == scope);

    return class_symbol;
}


bool isImportedTypeOnDemand(AstSgNodeListAttribute *attribute, SgClassDefinition *containing_definition, SgClassType *class_type) {
    if (attribute) {
        for (int i = 0; i < attribute -> size(); i++) {
            SgClassDefinition *on_demand_definition = isSgClassDefinition(attribute -> getNode(i));
            ROSE_ASSERT(on_demand_definition);
            if (containing_definition == on_demand_definition) {
                SgClassSymbol *class_symbol = lookupClassSymbolInScope(on_demand_definition, class_type -> get_name());
                if (class_symbol) { // an on-demand imported type?
// TODO: Remove this !!!
//cout << "Type " << class_type -> get_qualified_name().getString() << " is imported on demand in file " << ::currentSourceFile -> getFileName() << endl;
//ROSE_ASSERT(isSgClassType(class_symbol -> get_type()));
//cout.flush();
                    ROSE_ASSERT(class_symbol -> get_type() == class_type);
                    return true;
                }
            }
        }
    }

    return false;
}

bool isImportedType(SgClassType *class_type) {
    if (::currentSourceFile != NULL) { // We are processing a source file
        SgClassDeclaration *class_declaration = isSgClassDeclaration(class_type -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(class_declaration);
        SgClassDefinition *containing_definition = isSgClassDefinition(class_declaration -> get_parent());
        if (containing_definition) { // The type in question is enclosed in a package or a type (class member type)
            AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("imported_types");
            if (attribute) {
                for (int i = 0; i < attribute -> size(); i++) {
                    SgNamedType *imported_type = isSgNamedType(attribute -> getNode(i));
                    ROSE_ASSERT(imported_type);
                    if (imported_type == class_type) { // definitely an imported type
// TODO: Remove this !!!
//cout << "Type " << class_type -> get_qualified_name().getString() << " is directly imported" << endl;
//cout << "The type found is " << imported_type -> get_qualified_name().getString() << endl;
//cout.flush();
                        return true;
                    }
                }
            }

            if (isImportedTypeOnDemand((AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("import_on_demand_packages"), containing_definition, class_type)) {
                return true;
            }

            return isImportedTypeOnDemand((AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("import_on_demand_types"), containing_definition, class_type);
        }
    }

    return false;
}


/*
SgArrayType *getUniqueArrayType(SgType *base_type, int num_dimensions) {
    ROSE_ASSERT(num_dimensions > 0);
    if (num_dimensions > 1) {
        base_type = getUniqueArrayType(base_type, num_dimensions - 1);
    }

    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) base_type -> getAttribute("array");
    if (attribute == NULL) {
        SgArrayType *array_type = SageBuilder::buildArrayType(base_type);
        array_type -> set_rank(num_dimensions);
        attribute = new AstSgNodeAttribute(array_type);
        base_type -> setAttribute("array", attribute);
    }

    return isSgArrayType(attribute -> getNode());
}


SgJavaParameterizedType *getUniqueParameterizedType(SgNamedType *generic_type, SgTemplateParameterPtrList *new_args) {
    AstParameterizedTypeAttribute *attribute = (AstParameterizedTypeAttribute *) generic_type -> getAttribute("parameterized types");
    if (! attribute) {
        attribute = new AstParameterizedTypeAttribute(generic_type);
        generic_type -> setAttribute("parameterized types", attribute);
    }
    ROSE_ASSERT(attribute);

    return attribute -> findOrInsertParameterizedType(new_args);
}


SgJavaQualifiedType *getUniqueQualifiedType(SgClassDeclaration *class_declaration, SgType *parent_type, SgType *type) {
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) type -> getAttribute("qualified types");
    if (! attribute) {
        attribute = new AstSgNodeListAttribute();
        type -> setAttribute("qualified types", attribute);
    }
    ROSE_ASSERT(attribute);

    for (int i = 0; i < attribute -> size(); i++) {
        SgJavaQualifiedType *qualified_type = isSgJavaQualifiedType(attribute -> getNode(i));
        ROSE_ASSERT(qualified_type);
        if (qualified_type -> get_parent_type() == parent_type &&  qualified_type -> get_type() == type) {
            return qualified_type;
        }
    }

    SgJavaQualifiedType *qualified_type = new SgJavaQualifiedType(class_declaration);
    qualified_type -> set_parent_type(parent_type);
    qualified_type -> set_type(type);

    attribute -> addNode(qualified_type);

    return qualified_type;
}
*/

//
// Generate the unbound wildcard if it does not yet exist and return it.  Once the unbound Wildcard
// is generated, it is attached to the Object type so that it can be retrieved later. 
//
/*
SgJavaWildcardType *getUniqueWildcardUnbound() {
    ROSE_ASSERT(::ObjectClassType);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) ::ObjectClassType -> getAttribute("unbound");
    if (! attribute) {
        SgJavaWildcardType *wildcard = new SgJavaWildcardType(::ObjectClassDefinition -> get_declaration());
        attribute = new AstSgNodeAttribute(wildcard);
        ::ObjectClassType -> setAttribute("unbound", attribute);
    }

    return isSgJavaWildcardType(attribute -> getNode());
}
*/


//
// If it does not exist yet, generate wildcard type that extends this type.  Return the wildcard in question. 
//
/*
SgJavaWildcardType *getUniqueWildcardExtends(SgType *type) {
    ROSE_ASSERT(type);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) type -> getAttribute("extends");
    if (! attribute) {
        SgNamedType *named_type = (isSgArrayType(type) ? ::ObjectClassType : isSgNamedType(type));
if (! named_type){
cout << "The type " << getTypeName(type)
     << " is of type "
     << type -> class_name()
     << " in file "
     << ::currentSourceFile -> getFileName()
     << endl;
cout.flush();
}

        ROSE_ASSERT(named_type);
        SgClassDeclaration *class_declaration = isSgClassDeclaration(named_type -> getAssociatedDeclaration());
        ROSE_ASSERT(class_declaration);
        SgJavaWildcardType *wildcard = new SgJavaWildcardType(class_declaration);
        wildcard -> set_extends_type(type);

        attribute = new AstSgNodeAttribute(wildcard);
        type -> setAttribute("extends", attribute);
    }

    return isSgJavaWildcardType(attribute -> getNode());
}
*/

//
// If it does not exist yet, generate a super wildcard for this type.  Return the wildcard in question. 
//
/*
SgJavaWildcardType *getUniqueWildcardSuper(SgType *type) {
    ROSE_ASSERT(type);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) type -> getAttribute("super");
    if (! attribute) {
        SgNamedType *named_type = isSgNamedType(type);
        ROSE_ASSERT(named_type);
        SgClassDeclaration *class_declaration = isSgClassDeclaration(named_type -> getAssociatedDeclaration());
        ROSE_ASSERT(class_declaration);
        SgJavaWildcardType *wildcard = new SgJavaWildcardType(class_declaration);
        wildcard -> set_super_type(type);

        attribute = new AstSgNodeAttribute(wildcard);
        type -> setAttribute("super", attribute);
    }

    return isSgJavaWildcardType(attribute -> getNode());
}
*/

//
//
//
string getExtensionNames( std::vector<SgNode *> &extension_list, SgClassDeclaration *class_declaration, bool has_super_class) {
    string extension_names = "";
    for ( std::vector<SgNode *>::iterator i = extension_list.begin(); i != extension_list.end(); ) {
        SgType *type = isSgType(*i);
        ROSE_ASSERT(type);

        if (i == extension_list.begin()) { // the first element
            i++;
            if (class_declaration -> get_explicit_interface()) {
                extension_names += " extends "; // We are processing an interface.
                extension_names += getTypeName(type);
                if (i != extension_list.end())
                    extension_names += ", ";
            }
            else if (! has_super_class) {
                extension_names += " implements ";
                extension_names += getTypeName(type);
                if (i != extension_list.end())
                    extension_names += ", ";
            }
            else {
                if (type != ::ObjectClassType && (! class_declaration -> get_explicit_enum())) { // Don't process super class for enumeration.
                    extension_names += " extends ";
                    extension_names += getTypeName(type);
                }
                if (i != extension_list.end())
                    extension_names += " implements ";
            }
        }
        else {
            i++;
            extension_names += getTypeName(type);
            if (i != extension_list.end())
                extension_names += ", ";
        }
    }

    return extension_names;
}


//
// Always map the Rose SgTypeString into java.lang.String before making comparison. This is required
// because Rose assigns the the type SgTypeString by default to a string constant (an SgStringVal).
//
bool isCompatibleTypes(SgType *source_type, SgType *target_type) {
    if (isSgJavaQualifiedType(source_type))
        source_type = isSgJavaQualifiedType(source_type) -> get_type();
    if (isSgJavaQualifiedType(source_type))
        target_type = isSgJavaQualifiedType(target_type) -> get_type();
    if (isSgJavaParameterizedType(source_type))
        source_type = isSgJavaParameterizedType(source_type) -> get_raw_type();
    if (isSgJavaParameterizedType(target_type))
        target_type = isSgJavaParameterizedType(target_type) -> get_raw_type();

    if (isSgTypeString(source_type))
        source_type = ::StringClassType;
    if (isSgTypeString(target_type))
        target_type = ::StringClassType;

    return source_type == target_type;
}

//
//
//
string getPrimitiveTypeName(SgType *type) {
    string type_name;

    if (isSgTypeBool(type)) {
        type_name = "boolean";
    }
    else if (isSgTypeSignedChar(type)) {
        type_name = "byte";
    }
    else if (isSgTypeWchar(type)) {
        type_name = "char";
    }
    else if (isSgTypeInt(type)) {
        type_name = "int";
    }
    else if (isSgTypeShort(type)) {
        type_name = "short";
    }
    else if (isSgTypeFloat(type)) {
        type_name = "float";
    }
    else if (isSgTypeLong(type)) {
        type_name = "long";
    }
    else if (isSgTypeDouble(type)) {
        type_name = "double";
    }
    else if (isSgTypeVoid(type)) {
        type_name = "void";
    }
    else if (isSgTypeString(type)) {
        type_name = getTypeName(::StringClassType);
    }
    else {
        ROSE_ASSERT(type);
        string error_message = "*** Unrecognized type " + type -> class_name();
        if (! ::currentSourceFile -> attributeExists("error")) {
            ::currentSourceFile -> setAttribute("error", new AstRegExAttribute(error_message));
        }
        cout << error_message << endl;
        type_name = "int"; // Pick any default type in order to "keep going" // ROSE_ASSERT(false);
    }

    return type_name;
}


//
//
//
string getWildcardTypeName(SgJavaWildcardType *wild_type) {
    string name = "?";

    if (wild_type -> get_has_extends()) {
        name += " extends ";
    }
    else if (wild_type -> get_has_super()) {
        name += " super ";
    }

    SgType *bound_type = wild_type -> get_bound_type();
    if (bound_type != NULL) {
        name += getTypeName(bound_type);
    }

    return name;
}


//
//
//
string getUnionTypeName(SgJavaUnionType *union_type) {
    string name = "";
    SgTypePtrList type_list = union_type -> get_type_list();

 // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
 // for (int i = 0; i < type_list.size(); i++) {
    for (size_t i = 0; i < type_list.size(); i++) {
        if (i > 0) {
            name += " | ";
        }
        name += getTypeName(type_list[i]);
    }

    return name;
}


//
// Does the type associated with this class declaration require qualification?
//
bool hasConflicts(SgClassDeclaration *class_declaration) {
    SgScopeStatement *scope = class_declaration -> get_scope();
    ROSE_ASSERT(scope);
    string type_name = class_declaration -> get_name().getString();
    while (isSgClassDefinition(scope) && (! isSgJavaPackageDeclaration(scope -> get_parent()))) {
        class_declaration = isSgClassDeclaration(scope -> get_parent());
        ROSE_ASSERT(class_declaration);
        type_name = class_declaration -> get_name().getString() + "." + type_name;
        scope = class_declaration -> get_scope();
    }
    if (isSgClassDefinition(scope)) {
        string package_name = scope -> get_qualified_name().getString();

        //
        // Check whether or not there is a class with this name that is visible and belongs to 2 or more separate packages.
        // This test is calculated by calling the Java function JavaTraversal with two arguments: the source
        // filename and the class name.  If false is returned then we can use the type name without qualification.
        //
        ROSE_ASSERT(::currentSourceFile);
        ROSE_ASSERT(::currentEnvironment);
        ROSE_ASSERT(::currentJavaTraversalClass);
        ROSE_ASSERT(::hasConflictsMethod);
        return (::currentEnvironment -> CallBooleanMethod(::currentJavaTraversalClass,
                                                          ::hasConflictsMethod,
                                                          jserver_getJavaString(::currentSourceFile -> getFileName().c_str()),
                                                          jserver_getJavaString(package_name.c_str()),
                                                          jserver_getJavaString(type_name.c_str())));
    }

    return false;
}


// TODO: Remove this !
/*
bool isVisibleSimpleTypeName(SgNamedType *named_type) {
    if (isSgJavaParameterType(named_type)) { // -> attributeExists("is_parameter_type")) { // a parameter type?
        return true;
    }
    SgJavaQualifiedType *q_type = isSgJavaQualifiedType(named_type);
    if (q_type) {
        named_type = isSgNamedType(q_type -> get_type());
        ROSE_ASSERT(named_type);
    }
    list<SgClassSymbol *> locally_accessible_class_symbol;
    SgClassType *class_type = isSgClassType(named_type);
    ROSE_ASSERT(class_type);
    SgName class_name = class_type -> get_name(); // do a local lookup of the type.
    lookupLocalTypeSymbols(locally_accessible_class_symbol, class_name); // do a local lookup of the type.
    bool imported_type = isImportedType(class_type);
    return ((imported_type && locally_accessible_class_symbol.size() == 0) ||  // an imported type that does not conflict with a local type?
            ((! imported_type) && locally_accessible_class_symbol.size() == 1 && locally_accessible_class_symbol.front() -> get_type() == class_type));  // a locally visible type?
}
*/


bool mustBeFullyQualified(SgClassType *class_type) {
    if (::currentSourceFile != NULL) {
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("must_be_qualified");
        if (attribute != NULL) {
            for (int i = 0; i < attribute -> size(); i++) {
                if (isSgClassType(attribute -> getNode(i)) == class_type) {
                    return true;
                }
            }
        }
    }
    return (::currentSourceFile == NULL);
}

string markAndGetQualifiedTypeName(SgClassType *class_type) {
    if (::currentSourceFile != NULL) {
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("must_be_qualified");
        if (attribute == NULL) { // if not yet allocated then allocate it!
            attribute = new AstSgNodeListAttribute();
            ::currentSourceFile -> setAttribute("must_be_qualified", attribute);
        }

        int i;
        for (i = 0; i < attribute -> size(); i++) {
            if (isSgClassType(attribute -> getNode(i)) == class_type) {
                break;
            }
        }
        if (i == attribute -> size()) {
            attribute -> addNode(class_type);
        }
    }
    return getFullyQualifiedTypeName(class_type);
}


//
// Compute a usable minimally-qualified name for this type in this context.
//
string getTypeName(SgClassType *class_type) {
/*
cout << "***Getting name for type "
     << class_type -> get_qualified_name().getString()
     << endl;
cout.flush();
*/
    //
    // If we are not currently processing a compilation unit or this type was already tagged as must-be fully qualified in this source file, ...
    // TODO: I think this is necessary because of a javac bug... Check on that.
    //
    if (mustBeFullyQualified(class_type)) {
        return getFullyQualifiedTypeName(class_type);
    }

    //
    // If this type conflictc within the compilation unit then it requires qualification.
    //
    SgClassDeclaration *class_declaration = isSgClassDeclaration(class_type -> get_declaration());
    ROSE_ASSERT(class_declaration);
    if (hasConflicts(class_declaration)) {
        return markAndGetQualifiedTypeName(class_type);
    }

    //
    //
    //
    SgName class_name = class_type -> get_name(), // do a local lookup of the type.
           class_simple_name = class_type -> get_name().getString();

    if (isSgJavaParameterType(class_type)) { // -> attributeExists("is_parameter_type")) { // a parameter type?
// TODO: Remove this !!!
/*
cout << "The class "
     << class_type -> get_qualified_name().getString()
     << " is a parameter"
<< endl;
cout.flush();
*/
        return class_simple_name;
    }

    //
    // Check whether or not the class is visible locally.
    //
    list<SgClassSymbol *> locally_accessible_class_symbol; 
    lookupLocalTypeSymbols(locally_accessible_class_symbol, class_name); // do a local lookup of the type.
    if (isImportedType(class_type) && locally_accessible_class_symbol.size() == 0) { // an imported type that does not conflict with a local type?
// TODO: Remove this !!!
/*
cout << "The class "
     << class_type -> get_qualified_name().getString()
     << " is imported"
<< endl;
cout.flush();
*/
        return class_simple_name;
    }
    else if (locally_accessible_class_symbol.size() > 0) { // 1 or more local types with this class_name is visible
// TODO: Remove this !!!
/*
cout << "The class "
     << class_type -> get_qualified_name().getString()
     << " was found locally"
<< endl;
cout.flush();
*/
        if (locally_accessible_class_symbol.size() == 1) { // the type is visible
            SgClassSymbol *class_symbol = locally_accessible_class_symbol.front();
            if (class_symbol -> get_type() == class_type) {
                return class_simple_name;
            }
/*
cout << "Locally found class "
     << class_symbol -> get_declaration() -> get_qualified_name().getString()
     << " does not match "
     << class_type -> get_qualified_name().getString()
     << endl;
cout.flush();
*/
        }
        else {
/*
cout << "Found " << locally_accessible_class_symbol.size()
     << " local classes with the name "
     << class_type -> get_name().getString()
     << endl;
cout.flush();
*/
/*
for (list<SgClassSymbol *>::iterator i = locally_accessible_class_symbol.begin(); i != locally_accessible_class_symbol.end(); i++) {
SgClassSymbol *class_symbol = (*i);
SgJavaParameterizedType *p_type = isSgJavaParameterizedType(class_symbol -> get_type());
SgJavaQualifiedType *q_type = isSgJavaQualifiedType(class_symbol -> get_type());
SgClassType *c_type = isSgClassType(class_symbol -> get_type());
ROSE_ASSERT(p_type || c_type || q_type);
 cout << "    "
      << (p_type ? getFullyQualifiedTypeName(p_type)
                 : q_type ? getFullyQualifiedTypeName(q_type)
                          : getFullyQualifiedTypeName(c_type))
      << endl;
 cout.flush();
}
*/

            for (list<SgClassSymbol *>::iterator i = locally_accessible_class_symbol.begin(); i != locally_accessible_class_symbol.end(); i++) {
                SgClassSymbol *class_symbol = (*i);
                if (class_symbol -> get_type() == class_type) {
/*
cout << "Locally found class "
     << class_symbol -> get_declaration() -> get_qualified_name().getString()
     << "  *matches* "
     << class_type -> get_qualified_name().getString()
     << endl;
cout.flush();
*/
                    SgClassDefinition *containing_definition = isSgClassDefinition(class_symbol -> get_scope());
                    ROSE_ASSERT(containing_definition);
//                    SgClassType *parent_type  = containing_definition -> get_declaration() -> get_type();
//                    ROSE_ASSERT(parent_type);
//                    return (mustBeFullyQualified(parent_type) ? getFullyQualifiedTypeName(parent_type) : containing_definition -> get_declaration() -> get_name().getString()) + "." + class_simple_name;
                    return containing_definition -> get_qualified_name().getString() + "." + class_simple_name;
                }
/*
cout << "Locally found class "
     << class_symbol -> get_declaration() -> get_qualified_name().getString()
     << " does not match "
     << class_type -> get_qualified_name().getString()
     << endl;
cout.flush();
*/
            }

//cout << "No match found for type  "
//     << class_type -> get_qualified_name().getString()
//     << " with some local types of the same name"
//     << endl;
//cout.flush();

        }
    }

    string result = class_simple_name;
//cout << "Starting with simple name " << class_simple_name << endl;
//cout.flush();
    for (SgScopeStatement *scope = class_declaration -> get_scope(); scope != ::globalScope; scope = class_declaration -> get_scope()) {
        SgClassDefinition *class_definition = isSgClassDefinition(scope);
        if ((! class_definition) || class_definition == astJavaScopeStack.top()) {
// TODO: Remove this !!!
//cout << "Type " << class_simple_name << " found on the stack" << endl;
//cout.flush();
            break;
        }

        class_declaration = isSgClassDeclaration(class_definition -> get_parent());
        ROSE_ASSERT(class_declaration);
        SgJavaPackageDeclaration *package_declaration = isSgJavaPackageDeclaration(class_declaration);
        if (package_declaration || hasConflicts(class_declaration)) {
            result = markAndGetQualifiedTypeName(class_type); // this type requires full qualification in this file
// TODO: Remove this !!!
/*
if (package_declaration){
cout << "Bumped into package " << package_declaration -> get_qualified_name().getString() << " ... returning " << result << endl;
cout.flush();
}
*/
            break;
        }
        else {
            SgClassType *parent_type  = class_declaration -> get_type();
            ROSE_ASSERT(parent_type);
            if (mustBeFullyQualified(parent_type)) {
                result = getFullyQualifiedTypeName(parent_type) + "." + result;
                break;
            }
            else result = class_declaration -> get_name().getString() + "." + result;
        }
        
        if (isImportedType(class_declaration -> get_type())) {
// TODO: Remove this !!!
//cout << "Type " << class_declaration -> get_type() -> get_qualified_name().getString() << " was imported" << endl;
//cout.flush();
            break;
        }
    }

// TODO: Remove this !!!
//cout << "Type " << result << " was found before we reached the global scope" << endl;
//cout.flush();

    return result;
}


string getParameters(SgJavaParameterizedType *parm_type) {
    string result = "";
    if (parm_type -> get_type_list()) {
        result += "<";
        SgTemplateParameterPtrList arg_list = parm_type -> get_type_list() -> get_args();

     // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
     // for (int i = 0; i < arg_list.size(); i++) {
        for (size_t i = 0; i < arg_list.size(); i++) {
            if (i > 0) {
                result += ", ";
            }
            SgTemplateParameter *templateParameter = arg_list[i];
            SgType *argument_type = templateParameter -> get_type();
            result += getTypeName(argument_type);
        }
        result += ">";
    }
    return result;
}


//
//
//
string getTypeName(SgType *type) {
    SgJavaParameterizedType *param_type = isSgJavaParameterizedType(type); 
    SgJavaQualifiedType *qualified_type = isSgJavaQualifiedType(type); 
    SgArrayType *array_type = isSgArrayType(type);
    SgClassType *class_type = isSgClassType(type);
    SgJavaWildcardType *wild_type = isSgJavaWildcardType(type);
    SgJavaUnionType *union_type = isSgJavaUnionType(type);
    string result;

    if (param_type) {
         result = getTypeName(param_type -> get_raw_type()) + getParameters(param_type);
    }
    else if (qualified_type) {
         result = getTypeName(qualified_type -> get_parent_type()) + "." + getUnqualifiedTypeName(qualified_type -> get_type());
    }
    else if (array_type) {
         result = getTypeName(array_type -> get_base_type()) + "[]";
    }
    else if (class_type) {
         result = getTypeName(class_type);
    }
    else if (wild_type) {
         result = getWildcardTypeName(wild_type);
    }
    else if (union_type) {
         result = getUnionTypeName(union_type);
    }
    else result = getPrimitiveTypeName(type);

    return result;
}


//
//
//
string getUnqualifiedTypeName(SgType *type) {
    SgJavaParameterizedType *param_type = isSgJavaParameterizedType(type); 
    SgJavaQualifiedType *qualified_type = isSgJavaQualifiedType(type); 
    SgArrayType *array_type = isSgArrayType(type);
    SgClassType *class_type = isSgClassType(type);
    SgJavaWildcardType *wild_type = isSgJavaWildcardType(type);
    SgJavaUnionType *union_type = isSgJavaUnionType(type);
    string result;

    if (param_type) {
        result = getUnqualifiedTypeName(param_type -> get_raw_type()) + getParameters(param_type);
    }
    else if (qualified_type) { // an explicitly-specified qualified type should not be unqualified.
         result = getTypeName(qualified_type);
    }
    else if (array_type) {
         result = getUnqualifiedTypeName(array_type -> get_base_type()) + "[]";
    }
    else if (class_type) {
         result = class_type -> get_name().getString();
    }
    else if (wild_type) {
         result = "?";
         SgType *bound_type = wild_type -> get_bound_type();
         if (wild_type -> get_has_extends()) {
             result += " extends ";
         }
         else if (wild_type -> get_has_super()) {
             result += " super ";
         }
         result += getUnqualifiedTypeName(bound_type);
    }
    else if (union_type) { // Never need to unqualify a Union type.
         SgTypePtrList type_list = union_type -> get_type_list();
         ROSE_ASSERT(type_list.size());
         result = getUnqualifiedTypeName(type_list[0]);

      // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
      // for (int i = 1; i < type_list.size(); i++) {
         for (size_t i = 1; i < type_list.size(); i++) {
             result += " | ";
             result += getUnqualifiedTypeName(type_list[i]);
         }
    }
    else result = getPrimitiveTypeName(type);  // already unqualified.

    return result;
}


//
//
//
string getFullyQualifiedTypeName(SgType *type) {
    SgJavaParameterizedType *param_type = isSgJavaParameterizedType(type); 
    SgJavaQualifiedType *qualified_type = isSgJavaQualifiedType(type); 
    SgArrayType *array_type = isSgArrayType(type);
    SgClassType *class_type = isSgClassType(type);
    SgJavaWildcardType *wild_type = isSgJavaWildcardType(type);
    SgJavaUnionType *union_type = isSgJavaUnionType(type);
    string result;

    if (param_type) {
        result = getFullyQualifiedTypeName(param_type -> get_raw_type());
        if (param_type -> get_type_list()) {
            result += "<";
            SgTemplateParameterPtrList arg_list = param_type -> get_type_list() -> get_args();
 
         // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
            for (size_t i = 0; i < arg_list.size(); i++) {
                if (i > 0) {
                    result += ", ";
                }
                SgTemplateParameter *templateParameter = arg_list[i];
                SgType *argument_type = templateParameter -> get_type();
                result += getFullyQualifiedTypeName(argument_type);
            }
            result += ">";
        }
    }
    else if (qualified_type) { // an explicitly-specified qualified type should not be unqualified.
        result = getFullyQualifiedTypeName(qualified_type -> get_parent_type()) + "." + getUnqualifiedTypeName(qualified_type -> get_type());
    }
    else if (array_type) {
         result = getFullyQualifiedTypeName(array_type -> get_base_type()) + "[]";
    }
    else if (class_type) {
         if (isSgJavaParameterType(class_type)) {
             result = class_type -> get_name().getString();
         }
         else {
             SgClassDeclaration *declaration = isSgClassDeclaration(class_type -> get_declaration());
             ROSE_ASSERT(declaration);
             SgScopeStatement *scope = declaration -> get_scope();
             result = (isSgFunctionDefinition(scope)  // a local class?
                           ? class_type -> get_name().getString()
                           : class_type -> get_qualified_name().getString());
         }
    }
    else if (wild_type) {
         result = "?";
         SgType *bound_type = wild_type -> get_bound_type();
         if (wild_type -> get_has_extends()) {
             result += " extends ";
         }
         else if (wild_type -> get_has_super()) {
             result += " super ";
         }
         result += getFullyQualifiedTypeName(bound_type);
    }
    else if (union_type) {
         SgTypePtrList type_list = union_type -> get_type_list();
         ROSE_ASSERT(type_list.size());
         result = getFullyQualifiedTypeName(type_list[0]);

      // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=sign-compare.
         for (size_t i = 1; i < type_list.size(); i++) {
             result += " | ";
             result += getFullyQualifiedTypeName(type_list[i]);
         }
    }
    else result = getPrimitiveTypeName(type);  // already unqualified.

    return result;
}


// 
// Global stack of scopes.
// 
ScopeStack astJavaScopeStack;

// 
// Global stack of expressions and statements
// 
ComponentStack astJavaComponentStack;

// TODO: Remove this !
/* 
string getCurrentJavaFilename() {
// TODO: remove this !!!
//    ROSE_ASSERT(::globalScope != NULL);
//    SgSourceFile *sourceFile = isSgSourceFile(::globalScope -> get_parent());
//    ROSE_ASSERT(sourceFile != NULL);
//
//    return sourceFile -> getFileName();

if (!::currentSourceFile){
if (::project == NULL) cout << "The project is NULL" << endl;
if (::globalScope == NULL) cout << "The global scope is NULL" << endl;
cout.flush();
string *p = NULL;
if (p -> size());
}
  return ::currentSourceFile -> getFileName();
}
*/


/*
 * Wrapper to create an Sg_File_Info from line/col info
 */
Sg_File_Info *createSgFileInfo(string filename, int line, int col) {
    Sg_File_Info *sg_fi = new Sg_File_Info(filename, line, col);

    ROSE_ASSERT(sg_fi -> isTransformation()    == false);
    ROSE_ASSERT(sg_fi -> isCompilerGenerated() == false);

    if (line == 0 && col == 0) {
        if (SgProject::get_verbose() > 2)
            printf ("Found source position info (line == 0 && col == 0) indicating compiler generated code \n");

        sg_fi -> setCompilerGenerated();

        // sg_fi -> display("Found source position info (line == 0 && col == 0) indicating compiler generated code");
    }

    return sg_fi;
}


//
//
//
void setJavaSourcePosition(SgLocatedNode*locatedNode, Token_t *token) {
    ROSE_ASSERT(locatedNode);
    ROSE_ASSERT(token);
    JavaSourceCodePosition *posInfo = token -> getSourcecodePosition();

    // This function sets the source position if java position information has been provided
    // (posInfo != NULL), otherwise it is marked as not available.
    // These nodes WILL be unparsed in the code generation phase.
    // if (posInfo -> getLineStart() == 0) {
    if (posInfo -> getLineEnd() == 0) {
        if (locatedNode -> get_startOfConstruct() == NULL){
            locatedNode -> set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
        }
        if (locatedNode -> get_endOfConstruct() == NULL){
            locatedNode -> set_endOfConstruct(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
        }
        SageInterface::setSourcePosition(locatedNode); // setJavaSourcePositionUnavailableInFrontend(locatedNode);
        return;
    }

    // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
    ROSE_ASSERT(locatedNode != NULL);

    // Make sure we never try to reset the source position of the global scope (set elsewhere in ROSE).
    ROSE_ASSERT(isSgGlobal(locatedNode) == NULL);

    // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
    if (locatedNode -> get_endOfConstruct() != NULL || locatedNode -> get_startOfConstruct() != NULL) {
        if (SgProject::get_verbose() > 1) {
            printf ("In setSourcePosition(SgLocatedNode *locatedNode): Warning about existing file info data at locatedNode = %p = %s \n", locatedNode, locatedNode -> class_name().c_str());
        }

        if (locatedNode -> get_startOfConstruct() != NULL) {
            delete locatedNode -> get_startOfConstruct();
            locatedNode -> set_startOfConstruct(NULL);
        }

        if (locatedNode -> get_endOfConstruct() != NULL) {
            delete locatedNode -> get_endOfConstruct();
            locatedNode -> set_endOfConstruct(NULL);
        }
    }

    // DQ (8/16/2011): Added support for setting the operator source code position 
    // (Note that for expressions get_file_info() returns get_operatorPosition()).
    SgExpression *expression = isSgExpression(locatedNode);
    if (expression != NULL) {
        if (expression -> get_operatorPosition() != NULL) {
            delete expression -> get_operatorPosition();
            expression -> set_operatorPosition(NULL);
        }
    }

    if (posInfo == NULL) {
        // Call a mechanism defined in the SageInterface support

        //
        // If this file was not already tagged as an erroneous file, do so now.
        //
        if (! ::currentSourceFile -> attributeExists("error")) {
            ::currentSourceFile -> setAttribute("error", new AstRegExAttribute("JavaSourceCodePosition Translation error"));
        }
        printf("ERROR: JavaSourceCodePosition *posInfo == NULL triggering use of SageInterface::setSourcePosition() (locatedNode = %p = %s) \n", locatedNode, locatedNode -> class_name().c_str());
        // keep going !!!ROSE_ASSERT(false);

        SageInterface::setSourcePosition(locatedNode);
    } 
    else {
        // java position info is available
        Sg_File_Info *start_fileInfo = createSgFileInfo(token -> getFileName(), posInfo -> getLineStart(), posInfo -> getColStart());
        Sg_File_Info *end_fileInfo   = createSgFileInfo(token -> getFileName(), posInfo -> getLineEnd(), posInfo -> getColEnd());

        ROSE_ASSERT(start_fileInfo -> isTransformation() == false);
        ROSE_ASSERT(end_fileInfo -> isTransformation() == false);

        // updating the sgnode
        locatedNode -> set_startOfConstruct(start_fileInfo);
        locatedNode -> set_endOfConstruct(end_fileInfo);

        ROSE_ASSERT(start_fileInfo -> isTransformation() == false);
        ROSE_ASSERT(end_fileInfo -> isTransformation() == false);

        ROSE_ASSERT(locatedNode -> get_startOfConstruct() -> isTransformation() == false);
        ROSE_ASSERT(locatedNode -> get_endOfConstruct() -> isTransformation() == false);

        // DQ (8/16/2011): Added support for setting the operator source code position 
        // (Note that for expressions get_file_info() returns get_operatorPosition()).
        // SgExpression *expression = isSgExpression(locatedNode);
        if (expression != NULL) {
            Sg_File_Info *operator_fileInfo = createSgFileInfo(token -> getFileName(), posInfo -> getLineStart(), posInfo -> getColStart());
            expression -> set_operatorPosition(operator_fileInfo);

            ROSE_ASSERT(locatedNode -> get_file_info() -> isTransformation() == false);
        }

        ROSE_ASSERT(locatedNode -> get_file_info() -> isTransformation() == false);
    }

    ROSE_ASSERT(locatedNode -> get_file_info() -> isTransformation() == false);
    ROSE_ASSERT(locatedNode -> get_startOfConstruct() -> isTransformation() == false);
    ROSE_ASSERT(locatedNode -> get_endOfConstruct() -> isTransformation() == false);
}

//
//
//
void setJavaSourcePosition(SgLocatedNode *locatedNode, JNIEnv *env, jobject jToken) {
    Token_t *token = convert_Java_token(env, jToken);
    setJavaSourcePosition(locatedNode, token);
    delete token;
}


/*
//
//
//
void setJavaSourcePositionUnavailableInFrontend(SgLocatedNode *locatedNode) {
    ROSE_ASSERT(locatedNode != NULL);
    ROSE_ASSERT(locatedNode -> get_startOfConstruct() != NULL);
    ROSE_ASSERT(locatedNode -> get_endOfConstruct()   != NULL);

    // This is redundant for non-expression IR nodes.
    ROSE_ASSERT(locatedNode -> get_file_info() != NULL);

    locatedNode -> get_startOfConstruct() -> setSourcePositionUnavailableInFrontend();
    locatedNode -> get_endOfConstruct() -> setSourcePositionUnavailableInFrontend();

    locatedNode -> get_startOfConstruct() -> setOutputInCodeGeneration();
    locatedNode -> get_endOfConstruct() -> setOutputInCodeGeneration();

    locatedNode -> get_startOfConstruct() -> unsetTransformation();
    locatedNode -> get_endOfConstruct() -> unsetTransformation();

    locatedNode -> get_startOfConstruct() -> unsetCompilerGenerated();
    locatedNode -> get_endOfConstruct() -> unsetCompilerGenerated();
*/
/* // TODO: This code causes problems for Annotations!!!
    // DQ (8/16/2011): Added support for setting the operator source code position 
    // (Note that for expressions get_file_info() returns get_operatorPosition()).
    SgExpression *expression = isSgExpression(locatedNode);
    if (expression != NULL) {
        ROSE_ASSERT(expression -> get_operatorPosition() != NULL);
        expression -> get_operatorPosition() -> setSourcePositionUnavailableInFrontend();
        expression -> get_operatorPosition() -> setOutputInCodeGeneration();

        expression -> get_operatorPosition() -> unsetTransformation();
        expression -> get_operatorPosition() -> unsetCompilerGenerated();
    }
*/
//}


//
// PC: This code was derived from SageBuilder::buildClassDeclaration_nfi( ... ).  04/17/2013
// Note that a package declaration does NOT belong to any one file. Thus, it is assigned no particular location
// when it is constructed.
//
SgJavaPackageDeclaration *buildPackageDeclaration(SgScopeStatement *scope, const SgName &package_name, JNIEnv *env, jobject loc) {
    ROSE_ASSERT(lookupClassSymbolInScope(scope, package_name) == NULL);

    SgJavaPackageDeclaration *nondefining_package_declaration = new SgJavaPackageDeclaration(package_name, SgClassDeclaration::e_class, NULL, NULL);
    nondefining_package_declaration -> set_firstNondefiningDeclaration(nondefining_package_declaration);
    ROSE_ASSERT(nondefining_package_declaration == nondefining_package_declaration -> get_firstNondefiningDeclaration());
    nondefining_package_declaration -> set_scope(scope);
    nondefining_package_declaration -> set_parent(scope);
    if (nondefining_package_declaration -> get_type () == NULL) { // TODO: Should this be true? Turn this conditional into an assertion?
        SgClassType *type = SgClassType::createType(nondefining_package_declaration);
        nondefining_package_declaration -> set_type(type);
    }
    ROSE_ASSERT(isSgClassDeclaration(nondefining_package_declaration));
    ROSE_ASSERT(nondefining_package_declaration -> get_type());
    ROSE_ASSERT(nondefining_package_declaration -> get_type() -> get_declaration());
    ROSE_ASSERT(nondefining_package_declaration -> get_type() -> get_declaration() == nondefining_package_declaration);
    nondefining_package_declaration -> setForward();

    SgClassSymbol *class_symbol = new SgClassSymbol(nondefining_package_declaration);
    scope -> insert_symbol(package_name, class_symbol);
    setJavaSourcePosition(nondefining_package_declaration, env, loc);

    SageBuilder::testTemplateArgumentParents(nondefining_package_declaration);

    // 
    // Now, build the defining definition.
    // 
    SgClassDefinition *package_definition = SageBuilder::buildClassDefinition(NULL, false);
    assert(package_definition != NULL);
    setJavaSourcePosition(package_definition, env, loc);

    SgJavaPackageDeclaration *package_declaration  = new SgJavaPackageDeclaration(package_name, SgClassDeclaration::e_class, NULL, package_definition);
    ROSE_ASSERT(package_declaration -> get_definition() == package_definition);
    nondefining_package_declaration -> set_definingDeclaration(package_declaration);
    ROSE_ASSERT(nondefining_package_declaration -> get_definingDeclaration() == package_declaration);
    package_definition -> set_declaration(package_declaration);
    package_declaration -> set_definingDeclaration(package_declaration);

    SageBuilder::testTemplateArgumentParents(nondefining_package_declaration);
    SageBuilder::testTemplateArgumentParents(package_declaration);

    package_declaration -> set_firstNondefiningDeclaration(nondefining_package_declaration);
    ROSE_ASSERT(package_declaration -> get_type() == NULL);
    ROSE_ASSERT(nondefining_package_declaration -> get_type() != NULL);

    package_declaration -> set_type(nondefining_package_declaration -> get_type());

    ROSE_ASSERT(package_declaration -> get_type() == nondefining_package_declaration -> get_type());

    SageInterface::fixStructDeclaration(package_declaration, scope);
    SageInterface::fixStructDeclaration(nondefining_package_declaration, scope);

    package_declaration -> set_scope(scope);
    package_declaration -> set_parent(scope);
    setJavaSourcePosition(package_declaration, env, loc);
    return package_declaration;
}


/*
SgClassDeclaration *buildDefiningClassDeclaration(SgClassDeclaration::class_types kind, SgName class_name, SgScopeStatement *scope) {
    ROSE_ASSERT(scope);
    SgClassSymbol *class_symbol = lookupClassSymbolInScope(scope, class_name);
//
// TODO: Remove this !!!
//
//
//if (class_symbol != NULL) {
//cout << "Class symbol "
//     << class_name.getString()
//     << " already exists!!!"
//     << endl;
//if (class_symbol -> get_declaration()){
//cout << "The qualified type name is "
//     << class_symbol -> get_declaration() -> get_qualified_name().getString()
//     << endl;
//}
//}
    ROSE_ASSERT(class_symbol == NULL);

    SgClassDeclaration* nonDefiningDecl              = NULL;
    bool buildTemplateInstantiation                  = false;
    SgTemplateArgumentPtrList* templateArgumentsList = NULL;

    SgClassDeclaration* class_declaration = SageBuilder::buildClassDeclaration_nfi(class_name, kind, scope, nonDefiningDecl, buildTemplateInstantiation, templateArgumentsList);
    ROSE_ASSERT(class_declaration);
    class_declaration -> set_parent(scope);
    class_declaration -> set_scope(scope);

    ROSE_ASSERT(class_declaration -> get_scope() == scope); // There use to be a bug in the builder...

    return class_declaration;
}
*/


SgClassDefinition *findOrInsertPackage(SgScopeStatement *scope, const SgName &package_name, JNIEnv *env, jobject loc) {
    SgClassSymbol *package_symbol = lookupClassSymbolInScope(scope, package_name);
    SgJavaPackageDeclaration *package_declaration;
    SgClassDefinition *package_definition;
    if (package_symbol == NULL) {
        package_declaration = buildPackageDeclaration(scope, package_name, env, loc);
        package_definition = package_declaration -> get_definition();
        setJavaSourcePosition(package_declaration, env, loc);
        setJavaSourcePosition(package_definition, env, loc);
// TODO: Remove this !!!
/*
cout << "*** Inserting package "
<< package_definition -> get_qualified_name().getString()
<< endl;
cout.flush();
*/
        ROSE_ASSERT(package_definition);
        SgClassDefinition *class_scope = isSgClassDefinition(scope);
        SgGlobal *global_scope = isSgGlobal(scope);
        if (class_scope)
             class_scope -> append_member(package_declaration);
        else if (global_scope)
             global_scope -> append_declaration(package_declaration);
        else ROSE_ASSERT(false);
    }
    else {
        package_declaration = isSgJavaPackageDeclaration(package_symbol -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(package_declaration);
        package_definition = package_declaration -> get_definition();
        ROSE_ASSERT(package_definition);
    }

    return package_definition;
}


SgClassDefinition *findOrInsertPackage(SgName &package_name, JNIEnv *env, jobject loc) {
    list<SgName> name_list = generateQualifierList(package_name);
    SgScopeStatement *scope = ::globalScope;
    SgClassDefinition *package_definition = NULL;
    for (list<SgName>::iterator name = name_list.begin(); name != name_list.end(); name++) {
        package_definition = findOrInsertPackage(scope, *name, env, loc);
        scope = package_definition;
    }
    ROSE_ASSERT(package_definition);
    return package_definition;
}


SgJavaPackageDeclaration *findPackageDeclaration(SgName &package_name) {
    list<SgName> name_list = generateQualifierList(package_name);
    SgScopeStatement *scope = ::globalScope;
    SgJavaPackageDeclaration *package_declaration = NULL;
    for (list<SgName>::iterator name = name_list.begin(); name != name_list.end(); name++) {
        SgClassSymbol *package_symbol = lookupClassSymbolInScope(scope, *name);
        if (package_symbol == NULL)
            return NULL;
        package_declaration = isSgJavaPackageDeclaration(package_symbol->get_declaration()->get_definingDeclaration());
        ROSE_ASSERT(package_declaration != NULL);
        ROSE_ASSERT(package_declaration->get_scope() == scope);
        scope = package_declaration->get_definition();
    }
    return package_declaration;
}


SgMemberFunctionDeclaration *buildDefiningMemberFunction(const SgName &inputName, SgClassDefinition *class_definition, int num_arguments, JNIEnv *env, jobject method_location, jobject args_location) {
    if (SgProject::get_verbose() > 0)
        printf ("Inside of buildDefiningMemberFunction(): name = %s in scope = %p = %s = %s \n", inputName.str(), class_definition, class_definition -> class_name().c_str(), class_definition -> get_declaration() -> get_name().str());

    // This is abstracted so that we can build member functions as require to define Java specific default functions (e.g. super()).

    ROSE_ASSERT(class_definition != NULL);
    ROSE_ASSERT(class_definition -> get_declaration() != NULL);

    SgFunctionParameterTypeList *typeList = SageBuilder::buildFunctionParameterTypeList();
    ROSE_ASSERT(typeList != NULL);

    SgFunctionParameterList *parameterlist =  SageBuilder::buildFunctionParameterList();
    ROSE_ASSERT(parameterlist != NULL);

    // Loop over the types in the astJavaComponentStack (the rest of the stack).
    list<Sg_File_Info *> startLocation,
                         endLocation;
    list<SgInitializedName *> names;
    for (int i = 0; i < num_arguments; i++) { // charles4 10/12/2011: Reverse the content of the stack.
        SgNode *node = astJavaComponentStack.pop();
        SgInitializedName *initializedName = isSgInitializedName(node);
        ROSE_ASSERT(initializedName);
        names.push_front(initializedName);
        startLocation.push_front(initializedName -> get_startOfConstruct());
        endLocation.push_front(initializedName -> get_endOfConstruct());
    }

    // charles4 10/12/2011: Now, iterate over the list in the proper order
    while (! names.empty()) {
        SgInitializedName *initializedName = names.front();
        ROSE_ASSERT(initializedName != NULL);
        names.pop_front();

        SgType *parameterType = initializedName -> get_type();
        ROSE_ASSERT(parameterType != NULL);

        typeList -> append_argument(parameterType);

        parameterlist -> append_arg(initializedName);
        initializedName -> set_parent(parameterlist);
    }
// TODO: Remove this !!!
/*
cout << "Adding function " 
<< name
<< " in type "
<< class_definition -> get_qualified_name()
<< " with parameter types: (";
 SgTypePtrList::iterator i = typeList -> get_arguments().begin();
 if (i != typeList -> get_arguments().end()) {
cout << getTypeName(*i);
 for (i++; i != typeList -> get_arguments().end(); i++) {
cout << ", " << getTypeName(*i);
}
}
cout << ")"
<< endl;
cout.flush();
*/
    // This is the return type for the member function (top of the stack).
    SgType *return_type = astJavaComponentStack.popType();
    ROSE_ASSERT(return_type != NULL);

    // Specify if this is const, volatile, or restrict (0 implies normal member function).
    unsigned int mfunc_specifier = 0;
    SgMemberFunctionType *member_function_type = SageBuilder::buildMemberFunctionType(return_type, typeList, class_definition, mfunc_specifier);
    ROSE_ASSERT(member_function_type != NULL);
// TODO: Remove this !!!

if (member_function_type -> get_return_type() != return_type){
cout << "Mismatch in the return type of "
     << inputName.getString()
     << " in class "
     << class_definition -> get_qualified_name().getString()
     << "; The types are "
     << getTypeName(member_function_type -> get_return_type())
     << " ("
     << member_function_type -> get_return_type() -> class_name()
     << ")  and  "
     << getTypeName(return_type)
     << " ("
     << return_type -> class_name()
     << ")"
<< endl;
cout.flush();
}

    ROSE_ASSERT(member_function_type -> get_return_type() == return_type);

    // parameterlist = SageBuilder::buildFunctionParameterList(typeList);
    ROSE_ASSERT(parameterlist != NULL);

// TODO: remove this !!!
//
// PC (12/19/13) This old code is DEFINITELY NOT NEEDED for translation.  However, it is a useful fot detecting
// member methods that have been accidentally entered more than once in a scope.
//
//
// PC: This needs to be reviewed.  Is it needed? 04-03-13
//
// DQ (3/24/2011): Currently we am introducing a mechanism to make sure that overloaded function will have 
// a unique name. It is temporary until we can handle correct mangled name support using the argument types.
SgName name = inputName;
SgFunctionSymbol *func_symbol = NULL;
bool func_symbol_found = true;
while (func_symbol_found == true) {
    // DQ (3/24/2011): This function should not already exist (else it should be an error).
    func_symbol = class_definition -> lookup_function_symbol(name, member_function_type);
    // ROSE_ASSERT(func_symbol == NULL);

    if (func_symbol != NULL) {
        func_symbol_found = true;

        // This is a temporary mean to force overloaded functions to have unique names.
        name += "_overloaded_";
        if (SgProject::get_verbose() > 0)
            printf ("Using a temporary mean to force overloaded functions to have unique names (name = %s) \n", name.str());
    }
    else {
        func_symbol_found = false;
    }
}
if (name.getString().compare(inputName.getString()) != 0) { // PC - 04-03-13 - added this check because I don't understand the reason for the code above.
cout << "In class "
     << class_definition -> get_qualified_name().getString()
     <<", the name "
     << name.getString()
     << " is the new name for "
     << inputName.getString()
     << endl;
cout.flush();
}
ROSE_ASSERT(name.getString().compare(inputName.getString()) == 0); // PC - 04-03-13 - added this check because I don't understand the reason for the code above.

    //SgMemberFunctionDeclaration*
    //buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList);
    //
    // TODO: This line is needed for EDG4 [DONE]
    SgMemberFunctionDeclaration *nondefining_function_declaration = SageBuilder::buildNondefiningMemberFunctionDeclaration(name, return_type, parameterlist, class_definition, NULL, 0, false, NULL);

    //
    //SgMemberFunctionDeclaration*
    //buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, unsigned int functionConstVolatileFlags, SgMemberFunctionDeclaration* first_nondefinng_declaration, SgTemplateArgumentPtrList* templateArgumentsList);
    //
    // TODO: This line is needed for EDG4 [DONE]
    SgMemberFunctionDeclaration *function_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration(name, return_type, parameterlist, class_definition, NULL, false, 0, nondefining_function_declaration, NULL);

    // TODO: Remove the EDG3 line below. [DONE]
    //SgMemberFunctionDeclaration *function_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration(name, member_function_type, parameterlist, class_definition, NULL);
    ROSE_ASSERT(function_declaration);

    vector<SgInitializedName *> args = function_declaration -> get_args();
    setJavaSourcePosition(parameterlist, env, args_location);
    for (vector<SgInitializedName *>::iterator name_it = args.begin(); name_it != args.end(); name_it++) {
        SgInitializedName *locatedNode = *name_it;
        ROSE_ASSERT(! startLocation.empty());
        ROSE_ASSERT(! endLocation.empty());
        locatedNode -> set_startOfConstruct(startLocation.front());
        locatedNode -> set_endOfConstruct(endLocation.front());
        startLocation.pop_front();
        endLocation.pop_front();
    }
    ROSE_ASSERT(startLocation.empty());
    ROSE_ASSERT(endLocation.empty());
    ROSE_ASSERT(function_declaration != NULL);
    ROSE_ASSERT(function_declaration -> get_definingDeclaration() != NULL);
    ROSE_ASSERT(function_declaration -> get_definition() != NULL);

    setJavaSourcePosition(function_declaration -> get_definition(), env, method_location);

    return function_declaration;
}


/**
 * Although iterating over the methods this way appears to violate the Java spec rule that instructs
 * the compiler to "...get the list of methods seen from the class and get the most specific one...",
 * it is in fact correct since ECJ has already chosen the correct function and what we are doing
 * here is to look for a "perfect" mach with the function that was chosen.
 */
/*
SgMemberFunctionDeclaration *findMemberFunctionDeclarationInClass(SgClassDefinition *class_definition, const SgName &function_name, list<SgType *>& formal_types) {
    SgMemberFunctionDeclaration *method_declaration = lookupMemberFunctionDeclarationInClassScope(class_definition, function_name, formal_types);
    if (method_declaration == NULL) {
        const SgBaseClassPtrList &inheritance = class_definition->get_inheritances();
        for (SgBaseClassPtrList::const_iterator it = inheritance.begin(); method_declaration == NULL && it != inheritance.end(); it++) { // Iterate over super class, if any, then the interfaces, if any.
            SgClassDeclaration *decl = (*it) -> get_base_class();
            method_declaration = findMemberFunctionDeclarationInClass(decl -> get_definition(), function_name, formal_types);
        }
    }
    return method_declaration;
}
*/

/**
 * Lookup a member function in current class only (doesn't look in super and interfaces classes)
 */
/*
SgMemberFunctionDeclaration *lookupMemberFunctionDeclarationInClassScope(SgClassDefinition *class_definition, const SgName &function_name, list<SgType *>& types) {
    int num_arguments = types.size();
    SgMemberFunctionDeclaration *method_declaration = NULL;
    for (SgFunctionSymbol *function_symbol = class_definition -> lookup_function_symbol(function_name); function_symbol != NULL;  function_symbol = class_definition -> next_function_symbol()) {
        method_declaration = isSgMemberFunctionDeclaration(function_symbol -> get_declaration());
        if (method_declaration -> get_name().getString().compare(function_name.getString()) == 0) {
ROSE_ASSERT(method_declaration -> get_parent() == class_definition);
// TODO: REMOVE THIS !
//cout << "Found a match for function " << function_name.getString() << " in class " << class_definition -> get_qualified_name().getString() << endl;
//cout.flush();
            vector<SgInitializedName *> args = method_declaration -> get_args();
            if (args.size() == num_arguments) {
                list<SgType *>::const_iterator j = types.begin();
                int k;
                for (k = 0; k < num_arguments; k++, j++) {
                    SgType *type = (*j);
                    if (! isCompatibleTypes(type, args[k] -> get_type())) {
                        // Not all types are compatible, continue to look
                        break;
                    }
                }

                //
                // If we have a mismatch, it's possible that the method signature was updated.
                // In such a case, we check the updated list of parameter types.
                //
                if (k < num_arguments) {
                    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) method_declaration -> getAttribute("updated-parameter-types");
                    if (attribute) {
                        ROSE_ASSERT(attribute -> size() == num_arguments);
                        for ( j = types.begin(), k = 0; k < num_arguments; j++, k++) {
                            SgType *type = isSgType(attribute -> getNode(k));
                            if (! isCompatibleTypes((*j), type)) {
                                break;
                            }
                        }
                    }
                }

                //
                // All the arguments match?  If so, we are done!
                //
                if (k == num_arguments) {
                    break;
                }
            }
        }
    }

    return method_declaration;
}
*/

/*
SgMemberFunctionDeclaration *lookupMemberFunctionDeclarationInClassScope(SgClassDefinition *class_definition, const SgName &function_name, int num_arguments) {
    ROSE_ASSERT(class_definition != NULL);

    // Loop over the types in the astJavaComponentStack (the rest of the stack).
    list<SgType *> types;
    for (int i = 0; i < num_arguments; i++) { // charles4 10/12/2011: Reverse the content of the stack.
        SgType *type = astJavaComponentStack.popType();
        types.push_front(type);
    }

    SgType *return_type = astJavaComponentStack.popType(); // Remove the return type ... We don't need it!
    ROSE_ASSERT(return_type != NULL);

    SgMemberFunctionDeclaration *method_declaration = NULL;
    method_declaration = lookupMemberFunctionDeclarationInClassScope(class_definition, function_name, types);
    ROSE_ASSERT(method_declaration != NULL);

    return method_declaration;
}
*/

/*
SgMemberFunctionSymbol *findFunctionSymbolInClass(SgClassDefinition *class_definition, const SgName &function_name, list<SgType *> &formal_types) {
    ROSE_ASSERT(class_definition != NULL);

    SgMemberFunctionDeclaration *method_declaration = findMemberFunctionDeclarationInClass(class_definition, function_name, formal_types);
    if (method_declaration == NULL) {
        method_declaration = lookupMemberFunctionDeclarationInClassScope(ObjectClassDefinition, function_name, formal_types);
    }

// TODO: Remove this !!!

if (! method_declaration){
cout << "Could not find function " << function_name.getString() << "(";
std::list<SgType*>::iterator i = formal_types.begin();
if (i != formal_types.end()) {
cout << getTypeName(*i);
for (i++; i != formal_types.end(); i++) {
cout << ", " << getTypeName(*i);
}
}
cout << ") in class " 
<< class_definition -> get_qualified_name()
<< " in file " 
<< ::currentSourceFile -> getFileName()
<< endl;
cout.flush();

cout << endl
<< "The scope "
<< class_definition -> get_qualified_name().getString()
<< " contains:"
<< endl;
for (SgFunctionSymbol *function_symbol = class_definition -> first_function_symbol(); function_symbol != NULL; function_symbol = class_definition -> next_function_symbol()) {
  SgMemberFunctionDeclaration *decl = isSgMemberFunctionDeclaration(function_symbol -> get_declaration());
  ROSE_ASSERT(decl);
  cout << "    " << function_symbol -> get_name().getString()
       << "(";
SgInitializedNamePtrList &args = decl -> get_args();
for (int i = 0; i < args.size(); i++)
   cout << getTypeName(args[i] -> get_type());
cout << ")" << endl;
}
}

    ROSE_ASSERT(method_declaration);

    SgSymbol *symbol =  method_declaration -> search_for_symbol_from_symbol_table();
    ROSE_ASSERT(symbol);
    SgMemberFunctionSymbol *function_symbol = isSgMemberFunctionSymbol(symbol);
    ROSE_ASSERT(function_symbol);

    return function_symbol;
}
*/


list<SgName> generateQualifierList (const SgName &classNameWithQualification) {
    list<SgName> returnList;
    SgName classNameWithoutQualification;

    classNameWithoutQualification = classNameWithQualification;

    // Names of implicitly defined classes have names that start with "java." and these have to be translated.
    string original_classNameString = classNameWithQualification.str();
    string classNameString = classNameWithQualification.str();

    // Also replace '.' with '_'
    replace(classNameString.begin(), classNameString.end(), '.', '_');

    // Also replace '$' with '_' (not clear on what '$' means yet (something related to inner and outer class nesting).
    replace(classNameString.begin(), classNameString.end(), '$', '_');

    SgName name = classNameString;

    // We should not have a '.' in the class name.  Or it will fail the current ROSE name mangling tests.
    ROSE_ASSERT(classNameString.find('.') == string::npos);

    // DQ (3/20/2011): Detect use of '$' in class names. Current best reference 
    // is: http://www.java-forums.org/new-java/27577-specific-syntax-java-util-regex-pattern-node.html
    ROSE_ASSERT(classNameString.find('$') == string::npos);

    // Parse the original_classNameString to a list of what will be classes.
    size_t lastPosition = 0;
    size_t position = original_classNameString.find('.', lastPosition);
    while (position != string::npos) {
        string parentClassName = original_classNameString.substr(lastPosition, position - lastPosition);
        if (SgProject::get_verbose() > 0)
            printf ("parentClassName = %s \n", parentClassName.c_str());

        returnList.push_back(parentClassName);

        lastPosition = position+1;
        position = original_classNameString.find('.', lastPosition);
        if (SgProject::get_verbose() > 0)
            printf ("lastPosition = %" PRIuPTR " position = %" PRIuPTR " \n", lastPosition, position);
    }

    string className = original_classNameString.substr(lastPosition, position - lastPosition);

    if (SgProject::get_verbose() > 0)
        printf ("className for implicit (leaf) class = %s \n", className.c_str());

    // Reset the name for the most inner nested implicit class.  This allows a class such as "java.lang.System" 
    // to be build as "System" inside of "class "lang" inside of class "java" (without resetting the name we 
    // would have "java.lang.System" inside of "class "lang" inside of class "java").
    name = className;

    if (SgProject::get_verbose() > 0)
        printf ("last name = %s \n", name.str());

    // Push the last name onto the list.
    returnList.push_back(name);

    if (SgProject::get_verbose() > 0)
        printf ("returnList.size() = %" PRIuPTR " \n", returnList.size());

    return returnList;
}


//
//
//
/*
SgClassSymbol *lookupParameterTypeByName(const SgName &name) {
    SgClassSymbol *class_symbol = NULL;
    for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); class_symbol == NULL && i != astJavaScopeStack.end(); i++) {
        if ((*i) == ::globalScope) // ignore the global scope... It may appear in the middle of the stack for inner classes...
            continue;

        SgScopeStatement *scope = (*i);
// TODO: Remove this !!!
//cout << "Looking for " << name.getString() << " in scope " << (isSgClassDefinition(scope) ? isSgClassDefinition(scope) -> get_qualified_name().getString() : scope -> class_name()) << endl;
//cout.flush();
        class_symbol = lookupClassSymbolInScope(scope, name);
        if (! class_symbol) {
            SgClassDefinition *class_definition = isSgClassDefinition(scope);
            if (class_definition) {
//cout << "Looking for " << name.getString() << " in " << class_definition -> get_qualified_name().getString() << endl;
//cout.flush();
                class_symbol = lookupUniqueSimpleNameTypeInClass(name, class_definition);
            }
            else {
                SgFunctionDefinition *method_definition = isSgFunctionDefinition(scope);
                if (method_definition) {
                    AstSgNodeAttribute *type_space_attribute = (AstSgNodeAttribute *) method_definition -> get_declaration() -> getAttribute("type_space");
                    if (type_space_attribute) { // Initializer blocks don't have a type_space attribute
                        SgScopeStatement *type_space = isSgScopeStatement(type_space_attribute -> getNode());
                        ROSE_ASSERT(type_space);
//cout << "Looking for " << name.getString() << " in " << method_definition -> get_declaration() -> get_name().getString() << endl;
//cout.flush();
                        class_symbol = lookupClassSymbolInScope(type_space, name);
                    }
                }
            }
        }
    }

    return class_symbol;
}
*/


//
//
//
//
SgClassSymbol *lookupUniqueSimpleNameTypeInClass(const SgName &name, SgClassDefinition *class_definition) {
    ROSE_ASSERT(class_definition);
    ROSE_ASSERT(class_definition -> get_declaration());

    SgClassSymbol *class_symbol = lookupClassSymbolInScope(class_definition, name);
// TODO: Remove this!
/*
if (class_symbol != NULL){
SgDeclarationStatement *declaration = class_symbol -> get_declaration() -> get_definingDeclaration();
SgClassDeclaration *class_declaration = isSgClassDeclaration(declaration);
ROSE_ASSERT(class_declaration);
SgScopeStatement *scope = isSgClassDefinition(class_declaration -> get_parent());
ROSE_ASSERT(scope);
cout << "Symbol " << name.getString() << " was found right away in scope " << class_definition -> get_qualified_name().getString() 
//<< " but it is in scope " << scope -> get_qualified_name().getString() 
<< endl;
cout.flush();
}
*/
    vector<SgBaseClass *> &inheritances = class_definition -> get_inheritances();
    for (int k = 0; class_symbol == NULL && k < (int) inheritances.size(); k++) {
        SgClassDeclaration *super_declaration = inheritances[k] -> get_base_class();
        class_definition = super_declaration -> get_definition(); // get the super class definition
        class_symbol = lookupUniqueSimpleNameTypeInClass(name, class_definition);
    }

// TODO: Remove this!
/*
if (class_symbol != NULL){
SgDeclarationStatement *declaration = class_symbol -> get_declaration() -> get_definingDeclaration();
SgClassDeclaration *class_declaration = isSgClassDeclaration(declaration);
ROSE_ASSERT(class_declaration);
SgScopeStatement *scope = isSgClassDefinition(class_declaration -> get_parent());
ROSE_ASSERT(scope);
cout << "Symbol " << class_symbol -> class_name() << " was found for " << name.getString() << " in scope " << scope -> get_qualified_name().getString() << endl;
cout.flush();
}
*/
// TODO: Remove this !!!
//    if (class_symbol == NULL) {
//        class_symbol = lookupClassSymbolInScope(::ObjectClassDefinition, name);
//    }

    return class_symbol;
}


//
// Compute the list of all local classes with the given name that is visible in this class_definition.
// The result is placed in class_list.
//
void lookupAllSimpleNameTypesInClass(list<SgClassSymbol *> &class_list, const SgName &name, SgClassDefinition *class_definition) {
    ROSE_ASSERT(class_definition);
    SgClassSymbol *class_symbol = lookupClassSymbolInScope(class_definition, name);
    if (class_symbol) {
        class_list.push_back(class_symbol);
// TODO: Remove this!
/*
if (class_symbol != NULL){
SgDeclarationStatement *declaration = class_symbol -> get_declaration() -> get_definingDeclaration();
SgClassDeclaration *class_declaration = isSgClassDeclaration(declaration);
ROSE_ASSERT(class_declaration);
SgScopeStatement *scope = isSgClassDefinition(class_declaration -> get_parent());
ROSE_ASSERT(scope);
cout << "Symbol " << name.getString() << " was found in scope " << class_definition -> get_qualified_name().getString() 
<< endl;
cout.flush();
}
*/
        return; // if the type is an inner type in this scope then it hides the name in any super classes
    }

    vector<SgBaseClass *> &inheritances = class_definition -> get_inheritances();
    for (int k = 0; k < (int) inheritances.size(); k++) {
        SgClassDeclaration *super_declaration = inheritances[k] -> get_base_class();
        class_definition = super_declaration -> get_definition(); // get the super class definition
        lookupAllSimpleNameTypesInClass(class_list, name, class_definition);
    }

    return;
}


SgVariableSymbol *lookupSimpleNameVariableInClass(const SgName &name, SgClassDefinition *class_definition) {
    ROSE_ASSERT(class_definition);
    ROSE_ASSERT(class_definition -> get_declaration());
    SgVariableSymbol *symbol = class_definition -> lookup_variable_symbol(name);

    vector<SgBaseClass *> &inheritances = class_definition -> get_inheritances();
    for (int k = 0; symbol == NULL && k < (int) inheritances.size(); k++) {
        SgClassDeclaration *super_declaration = inheritances[k] -> get_base_class();
        class_definition = super_declaration -> get_definition(); // get the super class definition
        symbol = lookupSimpleNameVariableInClass(name, class_definition);
    }

    if (symbol == NULL) {
        symbol = ::ObjectClassDefinition -> lookup_variable_symbol(name);
    }

    return symbol;
}


//
// Search the scope stack for a variable declaration for the name in question.
//
SgVariableSymbol *lookupVariableByName(const SgName &name) {
    ROSE_ASSERT(! astJavaScopeStack.empty());

    //
    // Iterate over the scope stack... At each point, look to see if the variable is there.
    // Note that in the case of a class, we recursively search the class as well as its
    // super class and interfaces.
    //
    SgSymbol *symbol = NULL;
    for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); (symbol == NULL || (! isSgVariableSymbol(symbol))) && i != astJavaScopeStack.end(); i++) {
        if ((*i) == ::globalScope) // ignore the global scope... It may appear in the middle of the stack for inner classes...
            continue;

        symbol = (isSgClassDefinition(*i)
                      ? lookupSimpleNameVariableInClass(name, (SgClassDefinition *) (*i))
                   // DQ (8/16/2013): The API for this function has changed slightly and I expect that the more specific 
                   // lookup_variable_symbol() should have been called in place of the more general lookup_symbol() function.
                   // : (*i) -> lookup_symbol(name));
                      : (*i) -> lookup_variable_symbol(name));
    }

    //
    // If we still have not found the name, check to see if it is a static field that can be imported on-demand.
    //
    if (symbol == NULL && ::currentSourceFile != NULL) { // We are processing a source file
        AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("imported_fields");
        if (attribute) {
            for (int i = 0; i < attribute -> size(); i++) {
                SgVariableSymbol *variable_symbol = isSgVariableSymbol(attribute -> getNode(i));
                ROSE_ASSERT(variable_symbol);
                if (variable_symbol -> get_name().getString().compare(name.getString()) == 0) {
                    return variable_symbol;
                }
            }
        }

        attribute = (AstSgNodeListAttribute *) ::currentSourceFile -> getAttribute("import_on_demand_types");
        if (attribute) {
            for (int i = 0; i < attribute -> size(); i++) {
                SgClassDefinition *on_demand_definition = isSgClassDefinition(attribute -> getNode(i));
                ROSE_ASSERT(on_demand_definition);
                SgVariableSymbol *variable_symbol = on_demand_definition -> lookup_variable_symbol(name);
                if (variable_symbol) {
                    return variable_symbol;
                }
            }
        }
    }

    return isSgVariableSymbol(symbol);
}


//
// Search the scope stack for a variable declaration for the name in question.
//
SgJavaLabelSymbol *lookupLabelByName(const SgName &name) {
    ROSE_ASSERT(! astJavaScopeStack.empty());

    SgSymbol *symbol = NULL;

    //
    // Iterate over the scope stack... At each point, look to see if the variable is there.
    // Note that in the case of a class, we recursively search the class as well as its
    // super class and interfaces.
    //
    for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); (symbol == NULL || (! isSgJavaLabelSymbol(symbol))) && i != astJavaScopeStack.end(); i++) {
        if ((*i) == ::globalScope) // ignore the global scope... It may appear in the middle of the stack for inner classes...
            continue;

// DQ (8/16/2013): The API for this function has changed slightly and I expect that the more specific 
// symbol = (*i) -> lookup_symbol(name);

        symbol = (*i) -> lookup_symbol(name,NULL,NULL);

// TODO: Remove this!
//        if ((*i) == ::globalScope)
//            break;
    }

    return isSgJavaLabelSymbol(symbol);
}



void lookupLocalTypeSymbols(list<SgClassSymbol *> &local_class_symbols, SgName &type_name) {
    ROSE_ASSERT(local_class_symbols.size() == 0); // I am creating a fresh list!
// TODO: Remove this !!!
/*
cout << "Here is the stack: " << endl;
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
    //
    // Iterate over the scope stack... At each point, look to see if the variable is there.
    // Note that in the case of a class, we recursively search the class as well as its
    // super class and interfaces.
    //
    for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); (*i) != ::globalScope; i++) {
        SgClassDefinition *class_definition = isSgClassDefinition(*i);

// TODO: Remove this!
/*
cout << "Looking for type "
<< type_name
<< " in "
<< (class_definition ? class_definition -> get_qualified_name().getString()
    : isSgFunctionDefinition(*i) ? (isSgFunctionDefinition(*i) -> get_declaration() -> get_name().getString() + "(...)")
                                                  : (*i) -> class_name())
<< endl;
*/
        if (class_definition) {
            if (isSgJavaPackageDeclaration(class_definition -> get_parent())) { // Have we reached a package
// TODO: Remove this !!!
/*
cout << "Stopping the search at package "
     << class_definition -> get_qualified_name().getString()
     << endl;
cout.flush();
*/
                break;
            }

            lookupAllSimpleNameTypesInClass(local_class_symbols, type_name, class_definition);

// TODO: Remove this!
/*
if (local_class_symbols.size() > 0){ // any classes?  print the first one
cout << local_class_symbols.size() << " symbol(s) named " << local_class_symbols.front() -> class_name() << " was found for " << type_name.getString() << " in class definition " << class_definition -> get_qualified_name().getString() << endl;
cout.flush();
}
*/
            SgClassDeclaration *class_declaration = isSgClassDeclaration(class_definition -> get_declaration());
            ROSE_ASSERT(class_declaration);
            if (class_declaration -> get_explicit_interface() || class_declaration -> get_explicit_enum() || class_declaration -> get_declarationModifier().get_storageModifier().isStatic()) {
// TODO: Remove this!
/*
cout << "I encountered a static region: "
     << class_declaration -> get_qualified_name().getString()
     << " while looking for "
     << type_name.getString()
     << " #found_types = "
     << local_class_symbols.size()
     << endl;
cout.flush();
cout << "Here is the stack: " << endl;
for (std::list<SgScopeStatement*>::iterator e = astJavaScopeStack.begin(); e != astJavaScopeStack.end(); e++) {
cout << "    "
<< (isSgClassDefinition(*e) ? isSgClassDefinition(*e) -> get_qualified_name().getString()
                            : isSgFunctionDefinition(*e) ? (isSgFunctionDefinition(*e) -> get_declaration() -> get_name().getString() + "(...)")
                                                         : (*e) -> class_name())
<< " ("
<< ((unsigned long) (*e))
<< ")"
<< endl;
cout.flush();
}
*/
                SgClassSymbol *class_symbol = NULL;
                if (class_declaration -> get_name().getString().compare(type_name.getString()) == 0) {
                    class_symbol = isSgClassSymbol(class_declaration -> search_for_symbol_from_symbol_table());
                    ROSE_ASSERT(class_symbol);
                    local_class_symbols.push_back(class_symbol);
                }
                break;
            }
        }
        else {
            SgClassSymbol *class_symbol = lookupClassSymbolInScope((*i), type_name);
            if (class_symbol) {
// TODO: Remove this!
/*
cout << "Symbol " << class_symbol -> class_name() << " was found for " << type_name.getString()
<< " in "
<< (isSgFunctionDefinition(*i) ? (isSgFunctionDefinition(*i) -> get_declaration() -> get_name().getString() + "(...)")
                : isSgClassDefinition(*i) ? isSgClassDefinition(*i) -> get_qualified_name().getString() : (*i) -> class_name())
 << endl;
cout.flush();
*/
                local_class_symbols.push_back(class_symbol);
            }

            SgFunctionDefinition *method_definition = isSgFunctionDefinition(*i);
            if (method_definition) {
                if (local_class_symbols.size() > 0) { // if we've reached a method header and at least one type was found, we're done!
// TODO: Remove this!
/*
cout << "Am I here !? ... exiting at "
  << method_definition -> get_declaration() -> get_name().getString() << " with "
  << local_class_symbols.size()
  << " instances of type "
  << type_name.getString()
  << " found ?"
  << endl;
cout.flush();
*/
                    break;
                }

                //
                // Check to see if the type is a parameter type.
                //
                AstSgNodeAttribute *type_space_attribute = (AstSgNodeAttribute *) method_definition -> get_declaration() -> getAttribute("type_space");
                if (type_space_attribute) { // Initializer blocks don't have a type_space attribute
                    SgScopeStatement *type_space = isSgScopeStatement(type_space_attribute -> getNode());
                    ROSE_ASSERT(type_space);
                    class_symbol = lookupClassSymbolInScope(type_space, type_name);
                    if (class_symbol) {
// TODO: Remove this!
/*
cout << "Looking for " << type_name.getString() << " in " << method_definition -> get_declaration() -> get_name().getString() << endl;
cout.flush();
*/
                        local_class_symbols.push_back(class_symbol);
                        break;
                    }
                }
            }

// TODO: Remove this!
/*
if (class_symbol != NULL){
SgDeclarationStatement *declaration = class_symbol -> get_declaration() -> get_definingDeclaration();
SgClassDeclaration *class_declaration = isSgClassDeclaration(declaration);
ROSE_ASSERT(class_declaration);
SgScopeStatement *scope = isSgScopeStatement(class_declaration -> get_parent());
ROSE_ASSERT(scope);
cout << "Symbol " << class_symbol -> class_name() << " was found for " << type_name.getString() << " in scope " << (isSgClassDefinition(scope) ? isSgClassDefinition(scope) -> get_qualified_name().getString() : scope -> class_name()) << endl;
cout.flush();
}
*/
        }
    }

    return;
}


SgType *lookupTypeByName(SgName &package_name, SgName &type_name, int num_dimensions) {
    SgType *type = NULL;
 
    ROSE_ASSERT(! astJavaScopeStack.empty());

    //
    // First check to see if the type is a primitive type.
    //
    if (package_name.getString().size() == 0) {
        if (type_name.getString().compare("boolean") == 0) {
            type = SgTypeBool::createType();
        }
        else if (type_name.getString().compare("byte") == 0) {
            type = SgTypeSignedChar::createType();
        }
        else if (type_name.getString().compare("char") == 0) {
            type = SgTypeWchar::createType();
        }
        else if (type_name.getString().compare("int") == 0) {
            type = SgTypeInt::createType();
        }
        else if (type_name.getString().compare("short") == 0) {
            type = SgTypeShort::createType();
        }
        else if (type_name.getString().compare("float") == 0) {
            type = SgTypeFloat::createType();
        }
        else if (type_name.getString().compare("long") == 0) {
            type = SgTypeLong::createType();
        }
        else if (type_name.getString().compare("double") == 0) {
            type = SgTypeDouble::createType();
        }
        else if (type_name.getString().compare("void") == 0) {
            type = SgTypeVoid::createType();
        }
    }


    //
    // If we are dealing with Reference type ...
    //
    if (type == NULL) { // not a primitive type
        list<SgName> qualifiedTypeName = generateQualifierList(type_name);
        ROSE_ASSERT(qualifiedTypeName.size());
        list<SgName>::iterator name = qualifiedTypeName.begin();

        //
        // If no package was specified, we first check to see if the type is a local type.
        //
        list<SgClassSymbol *> local_class_symbols;
        if (package_name.getString().size() == 0 && qualifiedTypeName.size() == 1) { // No package?  Check to see if this is a local type.
            SgName short_name = *name;
// TODO: Remove this!
/*
cout << "Checking for local type "
     << (*name).getString()
     << endl;
cout.flush();
*/
            lookupLocalTypeSymbols(local_class_symbols, short_name);
        }
        SgClassSymbol *class_symbol = (local_class_symbols.size() == 1 ? local_class_symbols.front() : NULL);

        //
        // ... Continue Search ...
        //
        if (class_symbol == NULL) {
// TODO: Remove this!!!
/*
cout << "Looking for package : \"" << package_name.getString() << "\""
     << endl;
cout.flush();
*/
            SgJavaPackageDeclaration *package_declaration = findPackageDeclaration(package_name);
// TODO: Remove this!!!
/*
if (! package_declaration){
  cout << "The package : \"" << package_name.getString() << "\" does not exist in the global scope."
     << endl;
cout.flush();
}
*/
            ROSE_ASSERT(package_declaration);
            SgClassDefinition *package_definition = package_declaration -> get_definition();
            ROSE_ASSERT(package_definition);
            class_symbol = lookupClassSymbolInScope(package_definition, *name);

            //
            // If the class_symbol still has not been found and no package was specified, look for the type
            // in java.lang.
            //
            if (class_symbol == NULL && package_name.getString().size() == 0) {
                class_symbol = lookupClassSymbolInScope(::javaLangPackageDefinition, type_name);
            }

// TODO: Remove this!!!
/*
if (! class_symbol){
cout << "No symbol found for " << package_name.str() << (package_name.getString().size() ? "." : "") << type_name.str() 
     << " in file "
     << (::currentSourceFile ? ::currentSourceFile  -> getFileName(): "???")
<< endl;
cout.flush();
cout << "Here is the stack: " << endl;
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
            ROSE_ASSERT(class_symbol);

            for (name++; name != qualifiedTypeName.end(); name++) {
                SgClassDeclaration *declaration = isSgClassDeclaration(class_symbol -> get_declaration() -> get_definingDeclaration());
                ROSE_ASSERT(declaration);
                SgClassDefinition *definition = declaration -> get_definition();
                class_symbol = lookupUniqueSimpleNameTypeInClass((*name), definition);

// TODO: Remove this!!!
/*
if (! class_symbol){
cout << "Type " << (*name).getString() << " not found in " << definition -> get_qualified_name().getString()
     << endl;
cout.flush();
cout << "Here is the stack: " << endl;
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
                ROSE_ASSERT(class_symbol);
            }
        }

        type = class_symbol -> get_type();

// TODO: This looks like DEAD CODE !!!
/*
        SgClassType *class_type = isSgClassType(type);
        ROSE_ASSERT(class_type);
        getFullyQualifiedTypeName(class_type);
*/
    }

    //
    // If we are dealing with an array, build the Array type...
    //
    if (num_dimensions > 0) {
        type = SageBuilder::getUniqueJavaArrayType(type, num_dimensions);
    }

    ROSE_ASSERT(type);

    return type;
}


//
//
//
SgClassDefinition *getCurrentTypeDefinition() {
    SgClassDefinition *class_definition = NULL;
    std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin();
    while (i != astJavaScopeStack.end() && isSgClassDefinition(*i) == NULL) {
        i++;
    }

    if (i != astJavaScopeStack.end()) {
        class_definition = isSgClassDefinition(*i);
    }
    else {
        printf ("Error in getCurrentTypeDefinition(): SgTypeDefinition not found \n");
        ROSE_ASSERT(false);
    }

    ROSE_ASSERT(class_definition != NULL);

    return class_definition;
}


//
//
//
/*
SgScopeStatement *get_scope_from_symbol(SgSymbol *symbol) {
    SgScopeStatement *currentScope = NULL;
    if (isSgClassSymbol(symbol)) {
        SgClassSymbol *class_symbol = (SgClassSymbol *) symbol;

        SgDeclarationStatement *declarationFromSymbol = class_symbol -> get_declaration();
        ROSE_ASSERT(declarationFromSymbol != NULL);

        SgClassDeclaration *class_declaration  = isSgClassDeclaration(declarationFromSymbol -> get_definingDeclaration());
        ROSE_ASSERT(class_declaration != NULL);

        currentScope = class_declaration -> get_definition();
    }
*/
// TODO: Remove this !
/*
    else if (isSgNamespaceSymbol(symbol)) {
        SgNamespaceSymbol *namespace_symbol = (SgNamespaceSymbol *) symbol;

        SgDeclarationStatement *declarationFromSymbol = namespace_symbol -> get_declaration();
        ROSE_ASSERT(declarationFromSymbol != NULL);

        SgNamespaceDeclarationStatement *namespace_declaration  = isSgNamespaceDeclarationStatement(declarationFromSymbol -> get_definingDeclaration());
        ROSE_ASSERT(namespace_declaration != NULL);

        currentScope = namespace_declaration -> get_definition();
    }
*/
/*
    ROSE_ASSERT(currentScope != NULL);

    return currentScope;
}
*/


#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// Java support for calling C/C++.
#include <jni.h>

// Support functions declaration of function defined in this file.
#include "java_support.h"
#include "jni_token.h"

SgGlobal *globalScope = NULL;
SgClassType *ObjectClassType = NULL;
SgClassType *StringClassType = NULL;
SgClassType *ClassClassType = NULL;
SgClassDefinition *ObjectClassDefinition = NULL;

int initializerCount = 0;

SgArrayType *getUniqueArrayType(SgType *base_type, int num_dimensions) {
    ROSE_ASSERT(num_dimensions > 0);
    ostringstream convert;     // stream used for the conversion
    convert << num_dimensions; // insert the textual representation of num_dimensions in the characters in the stream
    string dimensions = convert.str();

    AstArrayTypeAttribute *attribute = (AstArrayTypeAttribute *) base_type -> getAttribute(dimensions);
    if (attribute == NULL) {
        SgArrayType *array_type = SageBuilder::buildArrayType(base_type);
        array_type -> set_rank(num_dimensions);
        attribute = new AstArrayTypeAttribute(array_type);
        base_type -> setAttribute(dimensions, attribute);
    }

    return attribute -> getArrayType();
}


//
// Turn Java array into a pointer to make it look like C++ in order to please the consistency check in Cxx_Grammar.C
//
SgPointerType *getUniquePointerType(SgType *base_type, int num_dimensions) {
    ROSE_ASSERT(base_type);
    ROSE_ASSERT(num_dimensions > 0);
    ostringstream convert;     // stream used for the conversion
    convert << num_dimensions; // insert the textual representation of num_dimensions in the characters in the stream
    string dimensions = convert.str() + "!";

    AstPointerTypeAttribute *attribute = (AstPointerTypeAttribute *) base_type -> getAttribute(dimensions);
    if (attribute == NULL) { // This Array type does not yet exist!
        //
        // WARNING:  Do not use this function: SageBuilder::buildPointerType(base_type), to create a
        // SgPointerType!!!  For some reason, it may return a pointer type that is different from the
        // base_type that is passed as argument.  Instead, use the command "new SgPointerType(base_type)".
        //
        SgPointerType *pointer_type =  (num_dimensions == 1 // end the recursion
                                           ? new SgPointerType(base_type)
                                           : pointer_type =  SageBuilder::buildPointerType(getUniquePointerType(base_type, num_dimensions - 1)));

        attribute = new AstPointerTypeAttribute(pointer_type);
        base_type -> setAttribute(dimensions, attribute);
    }

    return attribute -> getPointerType();
}


SgJavaParameterizedType *getUniqueParameterizedType(SgClassType *raw_type, SgTemplateParameterPtrList &newArgs) {
    if (! raw_type -> attributeExists("parameterized types")) {
        raw_type -> setAttribute("parameterized types", new AstParameterizedTypeAttribute(raw_type) );
    }
    AstParameterizedTypeAttribute *attribute = (AstParameterizedTypeAttribute *) raw_type -> getAttribute("parameterized types");
    ROSE_ASSERT(attribute);

    return attribute -> findOrInsertParameterizedType(newArgs);
}


//
// Generate the unbound wildcard if it does not yet exist and return it.  Once the unbound Wildcard
// is generated, it is attached to the Object type so that it can be retrieved later. 
//
SgJavaWildcardType *getUniqueWildcardUnbound() {
    ROSE_ASSERT(::ObjectClassType);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) ::ObjectClassType -> getAttribute("unbound");
    if (! attribute) {
        SgJavaWildcardType *wildcard = new SgJavaWildcardType();
        wildcard -> set_is_unbound(true);
        wildcard -> set_has_extends(false);
        wildcard -> set_has_super(false);

        attribute = new AstSgNodeAttribute(wildcard);
        ::ObjectClassType -> setAttribute("unbound", attribute);
    }

    return isSgJavaWildcardType(attribute -> getNode());
}


//
// If it does not exist yet, generate wildcard type that extends this type.  Return the wildcard in question. 
//
SgJavaWildcardType *getUniqueWildcardExtends(SgType *type) {
    ROSE_ASSERT(type);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) type -> getAttribute("extends");
    if (! attribute) {
        SgJavaWildcardType *wildcard = new SgJavaWildcardType(type);
        wildcard -> set_is_unbound(false);
        wildcard -> set_has_extends(true);
        wildcard -> set_has_super(false);

        attribute = new AstSgNodeAttribute(wildcard);
        type -> setAttribute("extends", attribute);
    }

    return isSgJavaWildcardType(attribute -> getNode());
}


//
// If it does not exist yet, generate a super wildcard for this type.  Return the wildcard in question. 
//
SgJavaWildcardType *getUniqueWildcardSuper(SgType *type) {
    ROSE_ASSERT(type);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) type -> getAttribute("super");
    if (! attribute) {
        SgJavaWildcardType *wildcard = new SgJavaWildcardType(type);
        wildcard -> set_is_unbound(false);
        wildcard -> set_has_extends(false);
        wildcard -> set_has_super(true);

        attribute = new AstSgNodeAttribute(wildcard);
        type -> setAttribute("super", attribute);
    }

    return isSgJavaWildcardType(attribute -> getNode());
}


//
// Always map the Rose SgTypeString into java.lang.String before making comparison. This is required
// because Rose assigns the the type SgTypeString by default to a string constant (an SgStringVal).
//
bool isCompatibleTypes(SgType *source_type, SgType *target_type) {
    if (isSgJavaParameterizedType(source_type))
        source_type = isSgJavaParameterizedType(source_type) -> get_raw_type();
    if (isSgJavaParameterizedType(target_type))
        target_type = isSgJavaParameterizedType(target_type) -> get_raw_type();

    if (isSgTypeString(source_type))
        source_type = StringClassType;
    if (isSgTypeString(target_type))
        target_type = StringClassType;

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
    else {
cout << "***I don't recognize the type " << type -> class_name() << endl;
        ROSE_ASSERT(false);
    }

    return type_name;
}


//
//
//
string getWildcardTypeName(SgJavaWildcardType *wild_type) {
    string name = "?";

    if (! wild_type -> get_is_unbound()) {
        name += (wild_type -> get_has_extends() ? " extends " : " super ");
        name += getTypeName(wild_type -> get_bound_type());
    }

    return name;
}


//
//
//
string getArrayTypeName(SgPointerType *pointer_type) {
    string name;
    SgType *base_type = pointer_type -> get_base_type();
    SgPointerType *sub_array = isSgPointerType(base_type);
    if (sub_array) {
        name = getArrayTypeName(sub_array);
    }
    else {
        SgClassType *class_type = isSgClassType(base_type);
        SgJavaParameterizedType *parm_type = isSgJavaParameterizedType(base_type); 
        name = (parm_type ? getFullyQualifiedTypeName(parm_type)
                          : class_type ? getFullyQualifiedTypeName(class_type)
                                       : getPrimitiveTypeName(base_type));
    }
    return name  + "[]";
}

//
//
//
string getFullyQualifiedName(SgClassDefinition *definition) {
    if (definition -> attributeExists("namespace")) {
        AstRegExAttribute *attribute = (AstRegExAttribute *) definition -> getAttribute("namespace");
        return attribute -> expression;
    }

    string name = definition -> get_declaration() -> get_name();
    if (isSgClassDefinition(definition -> get_scope())) {
        string prefix = getFullyQualifiedName((SgClassDefinition *) definition -> get_scope());
        return (prefix.size() > 0 ? (prefix + ".") : "") + name;
    }

    ROSE_ASSERT(false /* && definition -> get_scope() -> class_name() */);

    return "";
}

//
//
//
string getFullyQualifiedTypeName(SgClassType *class_type) {
    AstRegExAttribute *attribute = (AstRegExAttribute *) class_type -> getAttribute("name");
    if (! attribute) {
        SgClassDeclaration *declaration = isSgClassDeclaration(class_type -> get_declaration());
        ROSE_ASSERT(declaration);
        SgClassDeclaration *defining_declaration = isSgClassDeclaration(declaration -> get_definingDeclaration());
        ROSE_ASSERT(defining_declaration);
        SgClassDefinition *definition = defining_declaration -> get_definition();
        ROSE_ASSERT(definition);
        attribute = new AstRegExAttribute(getFullyQualifiedName(definition));
        class_type -> setAttribute("name", attribute);
    }

    return attribute -> expression;
}

string getFullyQualifiedTypeName(SgJavaParameterizedType *parm_type) {
    string result = getTypeName(parm_type -> get_raw_type());

    result += "<";
    SgTemplateParameterPtrList arg_list = parm_type -> get_type_list() -> get_args();
    for (int i = 0; i < arg_list.size(); i++) {
        SgTemplateParameter *templateParameter = arg_list[i];
        SgType *argument_type = templateParameter -> get_type();
        result += getTypeName(argument_type);
        if (i + 1 < arg_list.size()) {
            result += ", ";
        }
    }
    result += ">";

    return result;
}

//
//
//
string getTypeName(SgType *type) {
    SgJavaParameterizedType *parm_type = isSgJavaParameterizedType(type); 
    SgClassType *class_type = isSgClassType(type);
    SgPointerType *pointer_type = isSgPointerType(type);
    SgJavaWildcardType *wild_type = isSgJavaWildcardType(type);
    string result;

    if (parm_type) {
         result = getFullyQualifiedTypeName(parm_type);
    }
    else if (class_type) {
        result = (class_type -> attributeExists("is_parameter_type") ? class_type -> get_name().getString() : getFullyQualifiedTypeName(class_type));
    }
    else if (pointer_type) {
         result = getArrayTypeName(pointer_type);
    }
    else if (wild_type) {
         result = getWildcardTypeName(wild_type);
    }
    else result = getPrimitiveTypeName(type);

    return result;
}

//
// Replace newline character by its escape character sequence.
//
// TODO: PC Question: Shouldn't the unparser be doing this as it is already processing
//                    other escape sequences such as \" and \'.
//
string normalize(string source) {
    string target = "";
    for (string::iterator it = source.begin(); it < source.end(); it++) {
        switch(*it) {
            case '\0':
                target += "\\0";
                break;
            case '\n':
                target += "\\n";
                break;
            case '\r':
                target += "\\r";
                break;
            case '\\':
                target += "\\\\";
                break;
            default:
                target += (*it);
                break;
        }
    }

    return target;
}

// 
// Global stack of scopes.
// 
extern ScopeStack astJavaScopeStack;

// 
// Global stack of expressions and statements
// 
ComponentStack astJavaComponentStack;

// 
// 
// 
string getCurrentJavaFilename() {
    ROSE_ASSERT(::globalScope != NULL);
    SgSourceFile *sourceFile = isSgSourceFile(::globalScope -> get_parent());
    ROSE_ASSERT(sourceFile != NULL);

    return sourceFile -> getFileName();
 }

/*
 * Wrapper to create an Sg_File_Info from line/col info
 */
Sg_File_Info *createSgFileInfo(int line, int col) {
    // Sg_File_Info *sg_fi = Sg_File_Info::generateDefaultFileInfo();
    Sg_File_Info *sg_fi = new Sg_File_Info(getCurrentJavaFilename(), line, col);
    // sg_fi -> set_line(line);
    // sg_fi -> set_col(col);

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
void setJavaSourcePosition(SgLocatedNode*locatedNode, JavaSourceCodePosition *posInfo) {
    // This function sets the source position if java position information has been provided
    // (posInfo != NULL), otherwise it is marked as not available.
    // These nodes WILL be unparsed in the code generation phase.
    if (posInfo -> getLineStart() == 0) {
        setJavaSourcePositionUnavailableInFrontend(locatedNode);
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

        printf ("ERROR: JavaSourceCodePosition *posInfo == NULL triggering use of SageInterface::setSourcePosition() (locatedNode = %p = %s) \n", locatedNode, locatedNode -> class_name().c_str());
        ROSE_ASSERT(false);

        SageInterface::setSourcePosition(locatedNode);
    } 
    else {
        // java position info is available
        Sg_File_Info *start_fileInfo = createSgFileInfo(posInfo -> getLineStart(), posInfo -> getColStart());
        Sg_File_Info *end_fileInfo   = createSgFileInfo(posInfo -> getLineEnd(), posInfo -> getColEnd());

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
            Sg_File_Info *operator_fileInfo = createSgFileInfo(posInfo -> getLineStart(), posInfo -> getColStart());
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
    setJavaSourcePosition(locatedNode, convert_Java_token(env, jToken) -> getSourcecodePosition());
}


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
}


//
// TODO: DO this right at some point !!!  In particular, this should produce a wstring ...
//
string convertJavaStringValToWString(JNIEnv *env, const jstring &java_string) {
    std::string value;

    const jchar *raw = env -> GetStringChars(java_string, NULL);
    if (raw != NULL) {
        jsize len = env -> GetStringLength(java_string);
        for (const jchar *temp = raw; len > 0; len--,temp++) {
            if (*temp > 127) {
                // TODO: Do the right thing!
                if (! ::globalScope -> attributeExists("contains_wide_characters")) {
                    ostringstream convert;     // stream used for the conversion
                    convert << ((int) *temp); // insert the textual representation of num_dimensions in the characters in the stream
                    ::globalScope -> setAttribute("contains_wide_characters", new AstRegExAttribute(convert.str()));
                }
                value += ' ';
            }
            else value += *temp;
        }
        env -> ReleaseStringChars(java_string, raw);
    }

// TODO: Remove this !!!
/*
    const char *str = env -> GetStringUTFChars(java_string, NULL);
    value =  str;
cout << "The converted string is: \"";
for (int i = 0; i < value.size(); i++)
cout << str[i];
cout << "\"" << endl;
*/
    return normalize(value);
}


string convertJavaStringToCxxString(JNIEnv *env, const jstring &java_string) {
     // Note that "env" can't be passed into this function as "const".
    const char *str = env -> GetStringUTFChars(java_string, NULL);
    ROSE_ASSERT(str != NULL);

    string returnString = str;

    // printf ("Inside of convertJavaStringToCxxString s = %s \n", str);

    // Note that str is not set to NULL.
    env -> ReleaseStringUTFChars(java_string, str);

    return normalize(returnString);
}


//
//
//
string convertJavaPackageNameToCxxString(JNIEnv *env, const jstring &java_string) {
    string package_name =  convertJavaStringToCxxString(env, java_string);
    replace(package_name.begin(), package_name.end(), '.', '_');
    return package_name;
}


void memberFunctionSetup(SgName &name, SgClassDefinition *class_definition, int num_arguments, SgFunctionParameterList *&parameterlist, SgMemberFunctionType *&memberFunctionType,
                         list<Sg_File_Info *> &startLocation, list<Sg_File_Info *> &endLocation) {
    // Refactored code.

    // This is abstracted so that we can build member functions as require to define Java specific default functions (e.g. super()).

    ROSE_ASSERT(class_definition != NULL);
    ROSE_ASSERT(class_definition -> get_declaration() != NULL);

    if (SgProject::get_verbose() > 0)
        printf ("Inside of memberFunctionSetup(): name = %s in scope = %p = %s = %s \n", name.str(), class_definition, class_definition -> class_name().c_str(), class_definition -> get_declaration() -> get_name().str());

    SgFunctionParameterTypeList *typeList = SageBuilder::buildFunctionParameterTypeList();
    ROSE_ASSERT(typeList != NULL);

    ROSE_ASSERT(parameterlist == NULL);
    parameterlist = SageBuilder::buildFunctionParameterList();
    ROSE_ASSERT(parameterlist != NULL);

    // Loop over the types in the astJavaComponentStack (the rest of the stack).
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
    memberFunctionType = SageBuilder::buildMemberFunctionType(return_type, typeList, class_definition, mfunc_specifier);
    ROSE_ASSERT(memberFunctionType != NULL);

    // parameterlist = SageBuilder::buildFunctionParameterList(typeList);
    ROSE_ASSERT(parameterlist != NULL);

    // SgFunctionType *func_type = SageBuilder::buildFunctionType(return_type, parameterlist);
    SgFunctionType *func_type = memberFunctionType;
    ROSE_ASSERT(func_type != NULL);

    // DQ (3/24/2011): Currently we am introducing a mechanism to make sure that overloaded function will have 
    // a unique name. It is temporary until we can handle correct mangled name support using the argument types.
    SgFunctionSymbol *func_symbol = NULL;
    bool func_symbol_found = true;
    while (func_symbol_found == true) {
        // DQ (3/24/2011): This function should not already exist (else it should be an error).
        func_symbol = class_definition -> lookup_function_symbol(name, func_type);
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
}


SgMemberFunctionDeclaration *buildDefiningMemberFunction(const SgName &inputName, SgClassDefinition *class_definition, int num_arguments, JNIEnv *env, jobject method_location, jobject args_location) {
    // This is abstracted so that we can build member functions as require to define Java specific default functions (e.g. super()).

    SgName name = inputName;

    SgFunctionParameterList *parameterlist = NULL;
    SgMemberFunctionType *memberFunctionType = NULL;
    list<Sg_File_Info *> startLocation,
                         endLocation;

    // printf("Build defining member function %s\n", inputName.str());
    // Refactored code.
    memberFunctionSetup(name, class_definition, num_arguments, parameterlist, memberFunctionType, startLocation, endLocation);

    ROSE_ASSERT(parameterlist != NULL);
    ROSE_ASSERT(memberFunctionType != NULL);

    SgMemberFunctionDeclaration *functionDeclaration = SageBuilder::buildDefiningMemberFunctionDeclaration(name, memberFunctionType, parameterlist, class_definition);
    vector<SgInitializedName *> args = functionDeclaration -> get_args();
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
    ROSE_ASSERT(functionDeclaration != NULL);
    ROSE_ASSERT(functionDeclaration -> get_definingDeclaration() != NULL);
    ROSE_ASSERT(functionDeclaration -> get_definition() != NULL);

    SgFunctionDefinition *functionDefinition = functionDeclaration -> get_definition();
    ROSE_ASSERT(functionDefinition != NULL);
    setJavaSourcePosition(functionDefinition, env, method_location);

    return functionDeclaration;
}


/**
 * Although iterating over the methods this way appears to violate the Java spec rule that instructs
 * the compiler to "...get the list of methods seen from the class and get the most specific one...",
 * it is in fact correct since ECJ has already chosen the correct function and what we are doing
 * here is to look for a "perfect" mach with the function that was chosen.
 */
SgMemberFunctionDeclaration *findMemberFunctionDeclarationInClass(SgClassDefinition *class_definition, const SgName &function_name, list<SgType *>& formal_types) {
    SgMemberFunctionDeclaration *function_declaration = lookupMemberFunctionDeclarationInClassScope(class_definition, function_name, formal_types);
    if (function_declaration == NULL) {
        const SgBaseClassPtrList &inheritance = class_definition->get_inheritances();
        for (SgBaseClassPtrList::const_iterator it = inheritance.begin(); function_declaration == NULL && it != inheritance.end(); it++) { // Iterate over super class, if any, then the interfaces, if any.
            SgClassDeclaration *decl = (*it) -> get_base_class();
// TODO: Remove this !
/*
cout << "Looking for method "
     << function_name.getString()
     << " in class "
     << decl -> get_definition() -> get_qualified_name()
     << endl;
cout.flush();
*/
            function_declaration = findMemberFunctionDeclarationInClass(decl -> get_definition(), function_name, formal_types);
        }
    }
    return function_declaration;
}

/**
 * Lookup a member function in current class only (doesn't look in super and interfaces classes)
 */
SgMemberFunctionDeclaration *lookupMemberFunctionDeclarationInClassScope(SgClassDefinition *class_definition, const SgName &function_name, list<SgType *>& types) {
    int num_arguments = types.size();
    SgMemberFunctionDeclaration *function_declaration = NULL;
    vector<SgDeclarationStatement *> declarations = class_definition -> get_members();
    for (int i = 0; i < declarations.size(); i++, function_declaration = NULL) {
        SgDeclarationStatement *declaration = declarations[i];
        function_declaration = isSgMemberFunctionDeclaration(declaration);
        if (function_declaration && function_declaration -> get_name().getString().compare(function_name) == 0) {
            vector<SgInitializedName *> args = function_declaration -> get_args();
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

                if (k == num_arguments) {// all the arguments match?
                    break;
                }
            }
        }
    }

    return function_declaration;
}

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

    SgMemberFunctionDeclaration *function_declaration = NULL;
    function_declaration = lookupMemberFunctionDeclarationInClassScope(class_definition, function_name, types);
// TODO: REMOVE THIS !
if (!function_declaration){
cout << "Could not find function " << function_name.getString() << "(";
std::list<SgType*>::iterator i = types.begin();
if (i != types.end()) {
cout << getTypeName(*i);
for (i++; i != types.end(); i++) {
cout << ", " << getTypeName(*i);
}
}
cout << ") in class " 
<< class_definition -> get_qualified_name()
<< endl;
cout.flush();
}
    ROSE_ASSERT(function_declaration != NULL);

    return function_declaration;
}

SgMemberFunctionSymbol *findFunctionSymbolInClass(SgClassDefinition *class_definition, const SgName &function_name, list<SgType *> &formal_types) {
    ROSE_ASSERT(class_definition != NULL);

    SgMemberFunctionDeclaration *function_declaration = findMemberFunctionDeclarationInClass(class_definition, function_name, formal_types);
    if (function_declaration == NULL) {
        function_declaration = lookupMemberFunctionDeclarationInClassScope(ObjectClassDefinition, function_name, formal_types);
    }

// TODO: Remove this !!!
if (!function_declaration){
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
<< endl;
cout.flush();
}

    ROSE_ASSERT(function_declaration);

    SgSymbol *symbol =  function_declaration -> get_symbol_from_symbol_table();
    ROSE_ASSERT(symbol);
    SgMemberFunctionSymbol *function_symbol = isSgMemberFunctionSymbol(symbol);
    ROSE_ASSERT(function_symbol);

    return function_symbol;
}


SgClassDeclaration *buildJavaClass(const SgName &className, SgScopeStatement *scope, JNIEnv *env, jobject jToken) {
    ROSE_ASSERT(scope != NULL);

    // Note that this will also build the non-defining declaration.
    SgClassDeclaration *declaration = SageBuilder::buildDefiningClassDeclaration (className, scope);
    ROSE_ASSERT(declaration != NULL);

    ROSE_ASSERT(declaration -> get_type() != NULL);
    ROSE_ASSERT(declaration -> get_scope() != NULL);

    ROSE_ASSERT(declaration -> get_firstNondefiningDeclaration() != NULL);

    // DQ (3/24/2011): I think we should be able to assect this since the scope was valid.
    ROSE_ASSERT(declaration -> get_symbol_from_symbol_table() == NULL);
    ROSE_ASSERT(declaration -> get_firstNondefiningDeclaration() -> get_symbol_from_symbol_table() != NULL);

    // Make sure that the new class has been added to the correct synbol table.
    ROSE_ASSERT (scope -> lookup_class_symbol(declaration -> get_name()) != NULL);

    setJavaSourcePosition(declaration, env, jToken);

    ROSE_ASSERT(astJavaScopeStack.empty() == false);
    SgClassDefinition *class_definition = SageBuilder::buildClassDefinition(declaration);
    ROSE_ASSERT(class_definition != NULL);

    setJavaSourcePosition(class_definition, env, jToken);

    // DQ (3/25/2011): Added testing.
    ROSE_ASSERT(class_definition -> get_declaration() == declaration);
    ROSE_ASSERT(class_definition -> get_declaration() != NULL);
    ROSE_ASSERT(class_definition -> get_declaration() != NULL && class_definition -> get_declaration() -> get_symbol_from_symbol_table() == NULL);
    ROSE_ASSERT(class_definition -> get_declaration() != NULL && class_definition -> get_declaration() -> get_firstNondefiningDeclaration() -> get_symbol_from_symbol_table() != NULL);

    size_t declarationListSize = class_definition -> generateStatementList().size();

    if (SgProject::get_verbose() > 0)
        printf ("declarationListSize = %zu \n", declarationListSize);

    ROSE_ASSERT(declarationListSize == 0);

    return declaration;
}


SgVariableDeclaration *buildSimpleVariableDeclaration(const SgName &name, SgType *type) {
    if (SgProject::get_verbose() > 0)
        printf ("Building a variable (%s) within scope = %p = %s \n", name.str(), astJavaScopeStack.top(), astJavaScopeStack.top() -> class_name().c_str());

    // We are not supporting an initialized at this point in the implementation of the Java support.
    SgVariableDeclaration *variableDeclaration = SageBuilder::buildVariableDeclaration(name, type, NULL, astJavaScopeStack.top());
    ROSE_ASSERT(variableDeclaration != NULL);

    // DQ (8/21/2011): Note that the default access permission is default, but this is the same enum value as public.
    // Most language support ignores this in the unparser, but we might want to set it better than this.

    // DQ (8/21/2011): Debugging declarations in local function should (should not be marked as public).
    // ROSE_ASSERT(variableDeclaration -> get_declarationModifier().get_accessModifier().isPublic() == false);

    // DQ (7/16/2011): This is a test to debug failing test in resetParentPointers.C:1733
    ROSE_ASSERT(SageInterface::is_Fortran_language() == false);
    SgInitializedName *initializedName = variableDeclaration -> get_decl_item (name);
    ROSE_ASSERT(initializedName != NULL);
    ROSE_ASSERT(initializedName -> get_scope() != NULL);

    return variableDeclaration;
}

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
            printf ("lastPosition = %zu position = %zu \n", lastPosition, position);
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
        printf ("returnList.size() = %zu \n", returnList.size());

    return returnList;
}


//
//
//
//
SgClassSymbol *lookupSimpleNameTypeInClass(const SgName &name, SgClassDefinition *class_definition) {
    ROSE_ASSERT(class_definition);
    ROSE_ASSERT(class_definition -> get_declaration());

    SgClassSymbol *symbol = class_definition -> lookup_class_symbol(name);
    vector<SgBaseClass *> &inheritances = class_definition -> get_inheritances();
    for (int k = 0; symbol == NULL && k < (int) inheritances.size(); k++) {
        SgClassDeclaration *super_declaration = inheritances[k] -> get_base_class();
        class_definition = super_declaration -> get_definition(); // get the super class definition
        symbol = lookupSimpleNameTypeInClass(name, class_definition);
    }

    if (symbol == NULL) {
        symbol = ::ObjectClassDefinition -> lookup_class_symbol(name);
    }

    return symbol;
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

    SgSymbol *symbol = NULL;

    //
    // Iterate over the scope stack... At each point, look to see if the variable is there.
    // Note that in the case of a class, we recursively search the class as well as its
    // super class and interfaces.
    //
    for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); (symbol == NULL || (! isSgVariableSymbol(symbol))) && i != astJavaScopeStack.end(); i++)  {
        symbol = (isSgClassDefinition(*i)
                      ? lookupSimpleNameVariableInClass(name, (SgClassDefinition *) (*i))
                      : (*i) -> lookup_symbol(name));
        if ((*i) == ::globalScope)
            break;
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
    for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); (symbol == NULL || (! isSgJavaLabelSymbol(symbol))) && i != astJavaScopeStack.end(); i++)  {
        if (isSgClassDefinition(*i))
            break;
        symbol = (*i) -> lookup_symbol(name);
        if ((*i) == ::globalScope)
            break;
    }

    return isSgJavaLabelSymbol(symbol);
}



SgType *lookupTypeByName(SgName &package_name, SgName &type_name, int num_dimensions) {
    SgType *type = NULL;
 
   list<SgName> qualifiedTypeName = generateQualifierList(type_name);

    ROSE_ASSERT(! astJavaScopeStack.empty());
    ROSE_ASSERT(qualifiedTypeName.size());

    SgClassSymbol *class_symbol = NULL;

    list<SgName>::iterator name = qualifiedTypeName.begin();

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
        else {
            //
            // Iterate over the scope stack... At each point, look to see if the variable is there.
            // Note that in the case of a class, we recursively search the class as well as its
            // super class and interfaces.
            //
            for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); class_symbol == NULL && i != astJavaScopeStack.end(); i++)  {
                class_symbol = (isSgClassDefinition(*i)
                                    ? lookupSimpleNameTypeInClass((*name), (SgClassDefinition *) (*i))
                                    : (*i) -> lookup_class_symbol(*name));
                if ((*i) == ::globalScope)
                    break;
            }

            //
            // If the class_symbol still has not been found, look for it in java.lang!
            // At this point, the type_symbol in qustion must be a class_symbol.
            //
            if (class_symbol == NULL) {
                package_name = "java_lang";
                SgClassSymbol *namespace_symbol = ::globalScope -> lookup_class_symbol(package_name);
                ROSE_ASSERT(namespace_symbol);
                SgClassDeclaration *declaration = (SgClassDeclaration *) namespace_symbol -> get_declaration() -> get_definingDeclaration();
                SgClassDefinition *package = declaration -> get_definition();
                ROSE_ASSERT(package);
                class_symbol = package -> lookup_class_symbol(*name);
            }
        }
    }
    else {
        SgClassSymbol *namespace_symbol = ::globalScope -> lookup_class_symbol(package_name);
// TODO: Remove this!!!
if (! namespace_symbol){
cout << "The namespace : " << package_name << " does not exist in the global scope."
     << endl;
cout.flush();
}
        ROSE_ASSERT(namespace_symbol);
        SgClassDeclaration *declaration = (SgClassDeclaration *) namespace_symbol -> get_declaration() -> get_definingDeclaration();
        SgClassDefinition *package = declaration -> get_definition();
        ROSE_ASSERT(package);
        class_symbol = package -> lookup_class_symbol(*name);
// TODO: Remove this!!!
if (! class_symbol){
cout << "The name is: " << *name << "; "
     << "The package symbol is: " << package -> get_qualified_name() << "; "
     << "No symbol found for " << package_name.str() << (package_name.getString().size() ? "." : "") << type_name.str() << endl;
cout.flush();
}
    }

    //
    // If we are dealing with an instantiable type, we only have a class_symbol at this point.
    //
    if (type == NULL) { // not a primitive type
// TODO: Remove this!!!
if (! class_symbol){
cout << "No symbol found for " << package_name.str() << (package_name.getString().size() ? "." : "") << type_name.str() << endl;
cout.flush();
}
//else{
//cout << "Found symbol " << isSgClassType(class_symbol -> get_type()) -> get_qualified_name().str() << endl;
//cout.flush();
//}
        ROSE_ASSERT(class_symbol);

        for (name++; name != qualifiedTypeName.end(); name++) {
            SgClassDeclaration *declaration = isSgClassDeclaration(class_symbol -> get_declaration() -> get_definingDeclaration());
            ROSE_ASSERT(declaration);
            SgClassDefinition *definition = declaration -> get_definition();
            class_symbol = lookupSimpleNameTypeInClass((*name), definition);
            ROSE_ASSERT(class_symbol);
        }

        type = class_symbol -> get_type();

        SgClassType *class_type = isSgClassType(type);
        ROSE_ASSERT(class_type);
        getFullyQualifiedTypeName(class_type);
    }

    //
    // If we are dealing with an array, build the Array type...
    //
    if (num_dimensions > 0) {
        type = getUniquePointerType(type, num_dimensions); // getUniqueArrayType(type, num_dimensions);
    }

    ROSE_ASSERT(type);

    return type;
}


SgClassSymbol *lookupSymbolFromQualifiedName(string className) {
    // Java qualified names are separate by "." and can refer to classes that
    // are implicit (not appearing in the source code).  ROSE determines all
    // referenced implicit classes (recursively) and includes them in the AST
    // to support a proper AST with full type resolution, etc.  This can make 
    // the AST for even a trivial Java program rather large.

    list<SgName> qualifiedClassName = generateQualifierList(className);

    SgClassSymbol *previousClassSymbol = NULL;
    SgScopeStatement *previousClassScope = astJavaScopeStack.top();
    ROSE_ASSERT(previousClassScope != NULL);

    // Traverse all of the classes to get to the class containing the functionName.
    for (list<SgName>::iterator i = qualifiedClassName.begin(); i != qualifiedClassName.end(); i++) {
        // Get the class from the current scope of the nearest outer most scope.

        ROSE_ASSERT(previousClassScope != NULL);

        if (SgProject::get_verbose() > 2)
            printf ("Lookup SgSymbol for name = %s in scope = %p = %s = %s \n", (*i).str(), previousClassScope, previousClassScope -> class_name().c_str(), SageInterface::get_name(previousClassScope).c_str());

        SgSymbol *tmpSymbol = SageInterface::lookupSymbolInParentScopes(*i, previousClassScope);
        // ROSE_ASSERT(tmpSymbol != NULL);
        if (tmpSymbol != NULL) {
            if (SgProject::get_verbose() > 2)
                printf ("Found a symbol tmpSymbol = %s = %s \n", tmpSymbol -> class_name().c_str(), tmpSymbol -> get_name().str());

            // This is either a proper class or an alias to a class where the class is implicit or included via an import statement.
            SgClassSymbol *classSymbol       = isSgClassSymbol(tmpSymbol);
            SgVariableSymbol *variableSymbol = isSgVariableSymbol(tmpSymbol);
            SgAliasSymbol *aliasSymbol       = isSgAliasSymbol(tmpSymbol);

            if (classSymbol == NULL && aliasSymbol != NULL) {
                // printf ("Trace through the alias to the proper symbol in another scope. \n");
                classSymbol = isSgClassSymbol(aliasSymbol -> get_alias());
            }
            else {
                // This could be a call to "System.out.println();" (see test2011_04.java) in which case
                // this is a variableSymbol and the type of the variable is the class which has the 
                // "println();" function.
                if (classSymbol == NULL && variableSymbol != NULL) {
                    // Find the class associated with the type of the variable (this could be any expression so this get's messy!)
                    SgType *tmpType = variableSymbol -> get_type();
                    ROSE_ASSERT(tmpType != NULL);

                    printf ("variable type = %p = %s \n", tmpType, tmpType -> class_name().c_str());

                    // This should be a SgClassType but currently all variables are build with SgTypeInt.
                    // So this is the next item to fix in the AST.
                    SgClassType *classType = isSgClassType(tmpType);

                    // ROSE_ASSERT(classType != NULL);
                    if (classType != NULL) {
                        ROSE_ASSERT(classType -> get_declaration() != NULL);
                        SgClassDeclaration *class_declaration = isSgClassDeclaration(classType -> get_declaration());
                        ROSE_ASSERT(class_declaration != NULL);

                        SgSymbol *tmpSymbol = class_declaration -> search_for_symbol_from_symbol_table();
                        ROSE_ASSERT(tmpSymbol != NULL);
                        classSymbol = isSgClassSymbol(tmpSymbol);
                        ROSE_ASSERT(classSymbol != NULL);
                    }
                    else {
                        // This case happens when we are debugging the Java support and we have not built all the 
                        // implicit classes and yet we discover a type used in a function argument list or retun 
                        // type that is missing. In this case return NULL and it will be handled by the calling function.
                        printf ("WARNING: lookupSymbolFromQualifiedName(name = %s) is returning NULL since the class type was not found (debug mode) \n", className.c_str());
                        return NULL;
                    }
                }
                else {
                    // DQ (7/17/2011): This is not from a variable, it can be associated with a function when we started inside of the class.  See test2011_21.java.
                    SgFunctionSymbol *functionSymbol = isSgFunctionSymbol(tmpSymbol);
                    if (functionSymbol != NULL) {
                        // printf ("This could/should the constructor for the class we want, we just want the class... \n");

                        // Get the class directly since it is likely a parent class of the current scope.
                        classSymbol = SageInterface::lookupClassSymbolInParentScopes(*i, previousClassScope);
                        ROSE_ASSERT(classSymbol != NULL);
                    }

                    ROSE_ASSERT(classSymbol != NULL);
                }

                ROSE_ASSERT(aliasSymbol == NULL);
            }

            ROSE_ASSERT(classSymbol != NULL);

            if (SgProject::get_verbose() > 2)
                printf ("classSymbol = %p for class name = %s \n", classSymbol, (*i).str());

            previousClassSymbol = classSymbol;
            SgClassDeclaration *class_declaration = isSgClassDeclaration(classSymbol -> get_declaration() -> get_definingDeclaration());
            ROSE_ASSERT(class_declaration != NULL);
            // previousClassScope = classSymbol -> get_declaration() -> get_scope();
            previousClassScope = class_declaration -> get_definition();
            ROSE_ASSERT(previousClassScope != NULL);
        }
        else {
            // This is OK when we are only processing a small part of the implicit class space (debugging mode) and have not built all the SgClassDeclarations. 
            // printf ("WARNING: SgClassSymbol NOT FOUND in lookupSymbolFromQualifiedName(): name = %s \n", className.c_str());
            previousClassSymbol = NULL;
        }
    }

    return previousClassSymbol;
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
SgFunctionDefinition *getCurrentMethodDefinition() {
    for (std::list<SgScopeStatement*>::iterator i = astJavaScopeStack.begin(); i != astJavaScopeStack.end(); i++) {
        SgFunctionDefinition *method_definition = isSgFunctionDefinition(*i);
        if (method_definition) {
            return method_definition;
        }
        i++;
    }

    return NULL;
}


//
//
//
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
    else if (isSgNamespaceSymbol(symbol)) {
        SgNamespaceSymbol *namespace_symbol = (SgNamespaceSymbol *) symbol;

        SgDeclarationStatement *declarationFromSymbol = namespace_symbol -> get_declaration();
        ROSE_ASSERT(declarationFromSymbol != NULL);

        SgNamespaceDeclarationStatement *namespace_declaration  = isSgNamespaceDeclarationStatement(declarationFromSymbol -> get_definingDeclaration());
        ROSE_ASSERT(namespace_declaration != NULL);

        currentScope = namespace_declaration -> get_definition();
    }

    ROSE_ASSERT(currentScope != NULL);

    return currentScope;
}

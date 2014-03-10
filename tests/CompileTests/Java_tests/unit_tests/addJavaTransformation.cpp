// Expressions can be built using both bottomup (recommended ) and topdown orders.
// Bottomup: build operands first, operation later
// Topdown: build operation first, set operands later on.

#include <iostream>
#include "rose.h"
#include "ecj.h"
using namespace Rose::Frontend::Java::Ecj;
using namespace SageBuilder;
using namespace SageInterface;
using namespace std;

/**
 * Example of how to manipulate a Java Ast in ROSE.
 */
int main(int argc, char **argv) {
    SgProject *project = frontend(argc, argv); // Run the input example and create an initial AST
    SgGlobal *global_scope = project -> get_globalScopeAcrossFiles(); // get the one-and-only global scope

    setSourcePositionClassificationMode(e_sourcePositionFrontendConstruction);

    //
    // Allocate a temporary directory to store newly created info.
    //
    string directory_name = getTempDirectory(project); // get a temporary directory.

    //
    // Various varable declarations we will need for this exercise...
    //
    SgClassDefinition *package_definition = NULL;
    SgJavaImportStatement *import_statement = NULL;
    SgFunctionParameterList *parameter_list = NULL;
    SgInitializedName *initialized_name = NULL;
    SgMemberFunctionDeclaration *method_declaration = NULL;
    SgMemberFunctionDeclaration *initializer_declaration = NULL;
    SgFunctionDefinition *method_definition = NULL;
    SgFunctionDefinition *initializer_definition = NULL;
    SgFunctionParameterTypeList *typeList = NULL;
    SgMemberFunctionType *member_function_type = NULL;
    SgMemberFunctionSymbol *constructor_symbol = NULL;
    SgBasicBlock *method_body = NULL;
    SgExprListExp *arguments = NULL;
    SgFunctionCallExp *function_call_exp = NULL;
    SgConstructorInitializer *constructor_initializer = NULL;
    SgNewExp *new_expression = NULL;
    SgExprStatement *expression_statement = NULL;
    SgReturnStmt *return_statement = NULL;
    SgVariableDeclaration *variable_declaration = NULL;
    SgIntVal *integer_value = NULL;
    SgStringVal *string_value = NULL;

    //
    // Create a new package "p".
    //
    SgClassDefinition *p_package_definition = findOrInsertJavaPackage(project, "p", true /* create associated directory if package p is inserted */);

    //
    // Create sourcefile A.java for class A.
    //
    SgSourceFile *A_sourcefile = buildJavaSourceFile(project, directory_name, p_package_definition, "A");

    //
    // Insert the package "java.io" in the global space.
    //
    package_definition = findOrInsertJavaPackage(project, "java.io");

    //
    // Rose always makes 3 Java classes available: java.lang.Object, java.lang.String, java.lang.Class.
    // If the user wants to use another class, he must first import it into the project.
    // See use of processImport(...) function below.
    //
    package_definition = findJavaPackage(global_scope, "java.lang"); // We know for sure that java.lang is in the project!
    ROSE_ASSERT(package_definition);
    SgClassSymbol *class_symbol = package_definition -> lookup_class_symbol("Object");
    SgClassType *object_type = isSgClassType(class_symbol -> get_type());
    class_symbol = package_definition -> lookup_class_symbol("String");
    SgClassType *string_type = isSgClassType(class_symbol -> get_type());
    class_symbol = package_definition -> lookup_class_symbol("Class");
    SgClassType *class_type = isSgClassType(class_symbol -> get_type());

    //
    // Add 3 import statements:
    //
    {
        SgJavaImportStatementList *import_list = A_sourcefile -> get_import_list();

        //
        // import java.lang.class;
        //
        import_statement = buildJavaImportStatement(class_type -> get_qualified_name(), // java.lang.Class is already available
                                                                 false); // contains_wildcard
        import_statement -> set_parent(package_definition); // The parent of a statement must be a scope.
        import_list -> get_java_import_list().push_back(import_statement);

        //
        // import java.awt.*;
        //
        package_definition = findOrInsertJavaPackage(project, "java.awt"); // we only need the package for this on-demand import.
        import_statement = buildJavaImportStatement(package_definition -> get_qualified_name().getString(),
                                                                 true); // contains_wildcard
        import_statement -> set_parent(package_definition); // The parent of a statement must be a scope.
        import_list -> get_java_import_list().push_back(import_statement);

        //
        // import java.io.File
        //
        import_statement = buildJavaImportStatement(preprocessImport(project, "java.io.File"), // java.io.File may not be available; import it.
                                                                 false); // contains_wildcard
        import_statement -> set_parent(package_definition); // The parent of a statement must be a scope.
        import_list -> get_java_import_list().push_back(import_statement);
    }

    //
    // Build a class A in package p and add it to the sourcefile A.java
    //
    SgClassDeclaration *A_class_declaration = buildJavaDefiningClassDeclaration(p_package_definition, "A");
    SgClassDefinition *A_class_definition = A_class_declaration -> get_definition();
    A_class_declaration -> set_explicit_annotation_interface(false);
    A_class_declaration -> set_explicit_interface(false);
    A_class_declaration -> set_explicit_enum(false);
    A_sourcefile -> get_class_list() -> get_java_class_list().push_back(A_class_declaration);

    //
    // Add a constructor "A(String)" to class A.
    //
    {
        //
        // Create a declaration for the parameter String x and add it to a parameterlist
        //
        initialized_name = buildInitializedName("x", string_type, NULL);
        parameter_list = buildFunctionParameterList(initialized_name);
        initialized_name -> set_parent(parameter_list);

        //
        // Now create the method declaration and make it a member of the class A.
        //
        method_declaration = buildDefiningMemberFunctionDeclaration("<init>", SgTypeVoid::createType(), parameter_list, A_class_definition);
        method_declaration -> get_specialFunctionModifier().setConstructor();
        A_class_definition -> append_member(method_declaration);
    }

    //
    // Add a super() call to the constructor A(String)
    //
    {
        SgClassDefinition *object_class_definition = isSgClassDeclaration(object_type -> get_declaration() -> get_definingDeclaration()) -> get_definition();

        //
        // Build a function type for the Object default constructor. Since no super type was specified for A then Object is its super type.
        //
        typeList = buildFunctionParameterTypeList();
        member_function_type = buildMemberFunctionType(SgTypeVoid::createType(),
                                                       typeList,
                                                       object_class_definition,
                                                       0); // mfunc_specifier
        constructor_symbol = isSgMemberFunctionSymbol(object_class_definition -> lookup_function_symbol("<init>", member_function_type));
        ROSE_ASSERT(constructor_symbol);

        //
        // Build a super function call to Object()
        //
        arguments = buildExprListExp();
        function_call_exp = buildFunctionCallExp(constructor_symbol, arguments);
        function_call_exp -> setAttribute("<init>", new AstRegExAttribute("super"));

        //
        // Wrap the super call in an expression statement.
        //
        expression_statement = buildExprStatement(function_call_exp);

        //
        // Add the expression statement to the body of the constructor of A(String);
        //
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();
        method_body -> append_statement(expression_statement); // Add the Explicit constructor call.
    }

    //
    // Add a method "void m(int i)" to class A.
    //
    initialized_name = buildInitializedName("i", SgTypeInt::createType(), NULL);
    parameter_list = buildFunctionParameterList(initialized_name);
    initialized_name -> set_parent(parameter_list);
    method_declaration = buildDefiningMemberFunctionDeclaration("m", SgTypeVoid::createType(), parameter_list, A_class_definition);
    A_class_definition -> append_member(method_declaration);

    //
    // Add a method "static int zero()" to class A.
    //
    {
        parameter_list = buildFunctionParameterList();
        method_declaration = buildDefiningMemberFunctionDeclaration("zero", SgTypeInt::createType(), parameter_list, A_class_definition);
        method_declaration -> get_declarationModifier().get_storageModifier().setStatic();
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();

        //
        // Add the statement:  "return 0;" to the method body.
        //
        integer_value = buildIntVal(0); //create an integer literal with value 0.
        return_statement = buildReturnStmt_nfi(integer_value); // create a return statement.
        method_body -> append_statement(return_statement); // Add the return statement to method zero.

        A_class_definition -> append_member(method_declaration); // Add method zero declaration to A.
    }

    //
    // Add a method "int one()" to class A.
    //
    {
        parameter_list = buildFunctionParameterList();
        method_declaration = buildDefiningMemberFunctionDeclaration("one", SgTypeInt::createType(), parameter_list, A_class_definition);
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();

        //
        // Add the statement:  "return 0;" to the method body.
        //
        integer_value = buildIntVal(1); //create an integer literal with value 0.
        return_statement = buildReturnStmt_nfi(integer_value); // create a return statement.
        method_body -> append_statement(return_statement); // Add the return statement to method one

        A_class_definition -> append_member(method_declaration); // Add method one declaration to A.
    }

    //
    // Create sourcefile I.java for interface I.
    //
    SgSourceFile *I_sourcefile = buildJavaSourceFile(project, directory_name, p_package_definition, "I");

    //
    // build an interface I and add it to the sourcefile I.java
    //
    SgClassDeclaration *I_class_declaration = buildJavaDefiningClassDeclaration(p_package_definition, "I");
    SgClassDefinition *I_class_definition = I_class_declaration -> get_definition();
    I_class_declaration -> set_explicit_annotation_interface(false);
    I_class_declaration -> set_explicit_interface(true);
    I_class_declaration -> set_explicit_enum(false);
    I_sourcefile -> get_class_list() -> get_java_class_list().push_back(I_class_declaration);

    //
    // Add an abstract method "int one();" to interface I.
    //
    parameter_list = buildFunctionParameterList();
    method_declaration = buildDefiningMemberFunctionDeclaration("one", SgTypeInt::createType(), parameter_list, I_class_definition);
    method_declaration -> get_declarationModifier().setJavaAbstract();
    method_declaration -> setForward(); // indicate that this function does not contain a body.
    I_class_definition -> append_member(method_declaration);

    //
    // Make class A implement interface I
    //
    SgBaseClass *base = new SgBaseClass(I_class_declaration);
    base -> set_parent(A_class_definition);
    A_class_definition -> append_inheritance(base);
    AstRegExAttribute *attribute = (AstRegExAttribute *) A_class_definition -> getAttribute("extension_type_names");
    attribute -> expression = " implements " + I_class_declaration -> get_name().getString();

    //
    // Add a "private static final int a;" member to class A
    //
    variable_declaration = buildVariableDeclaration("a", SgTypeInt::createType(), NULL, A_class_definition);
    variable_declaration -> set_parent(A_class_definition);
    variable_declaration -> get_declarationModifier().get_accessModifier().setPrivate();
    variable_declaration -> get_declarationModifier().get_storageModifier().setStatic();
    variable_declaration -> get_declarationModifier().setFinal();
    initialized_name = *(variable_declaration -> get_variables().begin());
    A_class_definition -> append_member(variable_declaration);

    //
    // Add a static initialized block to initialize the member class a to 5 in class A.
    //
    {
        parameter_list = buildFunctionParameterList();
        initializer_declaration = buildDefiningMemberFunctionDeclaration("0block", // use an illegal name that starts with a number
                                                                         SgTypeVoid::createType(),
                                                                         parameter_list,
                                                                         A_class_definition);
        initializer_declaration -> get_functionModifier().setJavaInitializer();
        initializer_declaration -> get_declarationModifier().get_storageModifier().setStatic();
        A_class_definition -> append_member(initializer_declaration);

        //
        // Create assignment statement "a = 5;" and add it to the body of the static initializer.
        //
        expression_statement = buildAssignStatement(buildVarRefExp("a", A_class_definition), buildIntVal(5));
        initializer_definition = initializer_declaration -> get_definition();
        initializer_definition -> append_statement(expression_statement);
    }

    //
    // Define an anonymous type in P: "public A cheat() = { return new A("") { public int one() { return 2; } }; }"
    //
    {
        //
        // Create the anonymous class A$1 with super class A.
        //
        SgClassDeclaration *A$1_class_declaration = buildJavaDefiningClassDeclaration(p_package_definition, "A$1");
        SgClassDefinition *A$1_class_definition = A$1_class_declaration -> get_definition();
        {
            A$1_class_declaration -> setAttribute("anonymous", new AstRegExAttribute(""));
            A$1_class_declaration -> set_explicit_annotation_interface(false);
            A$1_class_declaration -> set_explicit_interface(false);
            A$1_class_declaration -> set_explicit_enum(false);

            //
            // Create a method "int one() { return 1; }" in A$1
            //
            parameter_list = buildFunctionParameterList();
            method_declaration = buildDefiningMemberFunctionDeclaration("one", SgTypeInt::createType(), parameter_list, A$1_class_definition);
            method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
            method_body = method_definition -> get_body();

            //
            // Add the statement:  "return 2;" to the method body of one in A$1.
            //
            integer_value = buildIntVal(2); //create an integer literal with value 0.
            return_statement = buildReturnStmt_nfi(integer_value); // create a return statement.
            method_body -> append_statement(return_statement); // Add the return statement.

            A$1_class_definition -> append_member(method_declaration);
        }

        //    
        // Create a constructor initializer with the string argument "" passed to it.
        //    
        string_value = buildStringVal("");
        constructor_initializer = buildConstructorInitializer(NULL,
                                                              buildExprListExp(string_value),
                                                              A_class_declaration -> get_type(),
                                                              false,
                                                              false,
                                                              false,
                                                              false);

        //    
        // Create a new expression with the constructor initializer.
        //    
        new_expression = buildNewExp(A_class_declaration -> get_type(),
                                     NULL,  // exprListExp,
                                     constructor_initializer,
                                     NULL,  // expr
                                     0,     // val
                                     NULL); // funcDecl
        constructor_initializer -> set_parent(new_expression);
        new_expression -> setAttribute("body", new AstSgNodeAttribute(A$1_class_declaration));

        //    
        // Construct a method declaration "A cheat() { return new ... }"
        //    
        parameter_list = buildFunctionParameterList();
        method_declaration = buildDefiningMemberFunctionDeclaration("cheat", A_class_declaration -> get_type(), parameter_list, A_class_definition);
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();

        return_statement = buildReturnStmt_nfi(new_expression); // create a return statement.
        method_body -> append_statement(return_statement);      // Add the return statement to the method's body.

        A_class_definition -> append_member(method_declaration); // Add method one declaration to A.
    }

    setSourcePositionClassificationMode(e_sourcePositionTransformation);
    AstTests::runAllTests(project);

    int return_code = backend(project); // invoke backend compiler to generate object/binary files

    destroyTempDirectory(directory_name); // Get rid of the temporary directory and all its included temporary files.

    return return_code;
}

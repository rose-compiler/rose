// Expressions can be built using both bottomup (recommended ) and topdown orders.
// Bottomup: build operands first, operation later
// Topdown: build operation first, set operands later on.

#include <iostream>
#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;
using namespace Rose::Frontend::Java;

using namespace std;

/**
 * Example of how to manipulate a Java Ast in ROSE.
 */
int main(int argc, char **argv) {
    SgProject *project = frontend(argc, argv); // Run the input example and create an initial AST

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
    SgMemberFunctionType *member_function_type = NULL;
    SgMemberFunctionSymbol *constructor_symbol = NULL;
    SgFunctionParameterTypeList *type_list = NULL;
    SgBasicBlock *method_body = NULL;
    SgExprListExp *arguments = NULL;
    SgFunctionCallExp *function_call_exp = NULL;
    SgConstructorInitializer *constructor_initializer = NULL;
    SgInitializer *initializer = NULL;
    SgExpression *expression = NULL;
    SgNewExp *new_expression = NULL;
    SgNullExpression *null_expression = NULL;
    SgJavaTypeExpression *type_expression = NULL;
    SgExprStatement *expression_statement = NULL;
    SgReturnStmt *return_statement = NULL;
    SgVariableDeclaration *variable_declaration = NULL;
    SgIntVal *integer_value = NULL;
    SgStringVal *string_value = NULL;
    SgFunctionSymbol *method_symbol = NULL;
    SgVariableSymbol *variable_symbol = NULL;
    SgClassType *class_type = NULL;
    unsigned int mfunc_specifier = 0;

    //
    // Create a new package "p".
    //
    SgClassDefinition *p_package_definition = findOrInsertJavaPackage(project, "p", true /* create associated directory if package p is inserted */);

    //
    // Create sourcefile A.java for class A.
    //
    SgSourceFile *A_sourcefile = SageBuilder::buildJavaSourceFile(project, directory_name, p_package_definition, "A");

    //
    // Find or insert the package "java.io" in the global space.
    //
    package_definition = findOrInsertJavaPackage(project, "java.io");

    //
    // Add 3 import statements. 
    //
    // Recall that the classes Object, String and Class are available. Every other class must be imported.
    //
    {
        SgJavaImportStatementList *import_list = A_sourcefile -> get_import_list();

        //
        // import java.lang.class;
        //
        import_statement = SageBuilder::buildJavaImportStatement(Rose::Frontend::Java::ClassClassType -> get_qualified_name(), // java.lang.Class is already available
                                                                 false); // contains_wildcard
        import_statement -> set_parent(package_definition); // The parent of a statement must be a scope.
        import_list -> get_java_import_list().push_back(import_statement);

        //
        // import java.awt.*;
        //
        package_definition = findOrInsertJavaPackage(project, "java.awt"); // we only need the package for this on-demand import.
        import_statement = SageBuilder::buildJavaImportStatement(package_definition -> get_qualified_name().getString(),
                                                                 true); // contains_wildcard
        import_statement -> set_parent(package_definition); // The parent of a statement must be a scope.
        import_list -> get_java_import_list().push_back(import_statement);

        //
        // import java.io.File
        //
        import_statement = SageBuilder::buildJavaImportStatement(SageInterface::preprocessImport(project, "java.io.File"), // java.io.File may not be available; import it.
                                                                 false); // contains_wildcard
        import_statement -> set_parent(package_definition); // The parent of a statement must be a scope.
        import_list -> get_java_import_list().push_back(import_statement);
    }

    //
    // Build a class A in package p and add it to the sourcefile A.java
    //
    SgClassDeclaration *A_class_declaration = SageBuilder::buildJavaDefiningClassDeclaration(p_package_definition, "A");
    SgClassDefinition *A_class_definition = A_class_declaration -> get_definition();
    A_sourcefile -> get_class_list() -> get_java_class_list().push_back(A_class_declaration);

    //
    // Add a constructor "A(String x) { ...  }" to class A.
    //
    {
        //
        // Create a declaration for the parameter String x and add it to a parameterlist
        //
      initialized_name = SageBuilder::buildInitializedName("x", Rose::Frontend::Java::StringClassType, NULL);
      parameter_list = SageBuilder::buildFunctionParameterList(initialized_name);
        initialized_name -> set_parent(parameter_list);

        //
        // Now create the method declaration and make it a member of the class A.
        //
        method_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("<init>", SgTypeVoid::createType(), parameter_list, A_class_definition);
        method_declaration -> get_specialFunctionModifier().setConstructor();
        A_class_definition -> append_member(method_declaration);
    }

    //
    // Add a super call to the constructor:  public A(String x) { super(); }
    //
    {
        SgClassDefinition *object_class_definition = isSgClassDeclaration(Rose::Frontend::Java::ObjectClassType -> get_declaration() -> get_definingDeclaration()) -> get_definition();

        //
        // Build a function type for the Object default constructor. Since no super type was specified for A then Object is its super type.
        //
        type_list = SageBuilder::buildFunctionParameterTypeList();
        member_function_type = SageBuilder::buildMemberFunctionType(SgTypeVoid::createType(),
                                                                    type_list,
                                                                    object_class_definition,
                                                                    mfunc_specifier);
        constructor_symbol = isSgMemberFunctionSymbol(object_class_definition -> lookup_function_symbol("<init>", member_function_type));
        ROSE_ASSERT(constructor_symbol);
        delete type_list;

        //
        // Build a super function call to Object()
        //
        arguments = SageBuilder::buildExprListExp();
        function_call_exp = SageBuilder::buildFunctionCallExp(constructor_symbol, arguments);
        function_call_exp -> setAttribute("<init>", new AstRegExAttribute("super"));

        //
        // Wrap the super call in an expression statement.
        //
        expression_statement = SageBuilder::buildExprStatement(function_call_exp);

        //
        // Add the expression statement to the body of the constructor of A(String);
        //
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();
        method_body -> append_statement(expression_statement); // Add the Explicit constructor call.
    }

    //
    // Add a method "public void m(int i) { }" to class A.
    //
    initialized_name = SageBuilder::buildInitializedName("i", SgTypeInt::createType(), NULL);
    parameter_list = SageBuilder::buildFunctionParameterList(initialized_name);
    initialized_name -> set_parent(parameter_list);
    method_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("m", SgTypeVoid::createType(), parameter_list, A_class_definition);
    A_class_definition -> append_member(method_declaration);

    //
    // Add a method "public static int zero() { return 0; }" to class A.
    //
    {
        parameter_list = SageBuilder::buildFunctionParameterList();
        method_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("zero", SgTypeInt::createType(), parameter_list, A_class_definition);
        method_declaration -> get_declarationModifier().get_storageModifier().setStatic();
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();

        //
        // Add the statement:  "return 0;" to the method body.
        //
        integer_value = SageBuilder::buildIntVal(0); //create an integer literal with value 0.
        return_statement = SageBuilder::buildReturnStmt(integer_value); // create a return statement.
        method_body -> append_statement(return_statement); // Add the return statement to method zero.

        A_class_definition -> append_member(method_declaration); // Add method zero declaration to A.
    }

    //
    // Add a method "public int one() { return 1; }" to class A.
    //
    {
        parameter_list = SageBuilder::buildFunctionParameterList();
        method_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("one", SgTypeInt::createType(), parameter_list, A_class_definition);
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();

        //
        // Add the statement:  "return 1;" to the method body.
        //
        integer_value = SageBuilder::buildIntVal(1); //create an integer literal with value 0.
        return_statement = SageBuilder::buildReturnStmt(integer_value); // create a return statement.
        method_body -> append_statement(return_statement); // Add the return statement to method one

        A_class_definition -> append_member(method_declaration); // Add method one declaration to A.
    }

    //
    // Create sourcefile I.java for interface I.
    //
    SgSourceFile *I_sourcefile = SageBuilder::buildJavaSourceFile(project, directory_name, p_package_definition, "I");

    //
    // build an interface I and add it to the sourcefile I.java
    //
    SgClassDeclaration *I_class_declaration = SageBuilder::buildJavaDefiningClassDeclaration(p_package_definition, "I");
    I_class_declaration -> set_explicit_interface(true);
    SgClassDefinition *I_class_definition = I_class_declaration -> get_definition();
    I_sourcefile -> get_class_list() -> get_java_class_list().push_back(I_class_declaration);

    //
    // Add an abstract method "abstract public int one();" to interface I.
    //
    parameter_list = SageBuilder::buildFunctionParameterList();
    method_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("one", SgTypeInt::createType(), parameter_list, I_class_definition);
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
    variable_declaration = SageBuilder::buildVariableDeclaration("a", SgTypeInt::createType(), NULL, A_class_definition);
    variable_declaration -> set_parent(A_class_definition);
    variable_declaration -> get_declarationModifier().get_accessModifier().setPrivate();
    variable_declaration -> get_declarationModifier().get_storageModifier().setStatic();
    variable_declaration -> get_declarationModifier().setFinal();
    A_class_definition -> append_member(variable_declaration);

    //
    // Add a static initializer block to initialize the member class "int a" to 5 in class A.
    //
    {
        parameter_list = SageBuilder::buildFunctionParameterList();
        initializer_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("0block", // use an illegal name that starts with a number
                                                                                      SgTypeVoid::createType(),
                                                                                      parameter_list,
                                                                                      A_class_definition);
        initializer_declaration -> get_functionModifier().setJavaInitializer();
        initializer_declaration -> get_declarationModifier().get_storageModifier().setStatic();
        A_class_definition -> append_member(initializer_declaration);

        //
        // Create assignment statement "a = 5;" and add it to the body of the static initializer.
        //
        expression_statement = SageBuilder::buildAssignStatement(buildVarRefExp("a", A_class_definition), buildIntVal(5));
        initializer_definition = initializer_declaration -> get_definition();
        initializer_definition -> append_statement(expression_statement);
    }

    //
    // Define an anonymous type in P: "public A cheat() = { return new A("") { public int one() { return 2; } }; }"
    //
    {
        //
        // Create the class A$1 that will be used as an anonymous class. Note that it looks like any other class.
        //
        SgClassDeclaration *A$1_class_declaration = SageBuilder::buildJavaDefiningClassDeclaration(p_package_definition, "A$1");
        A$1_class_declaration -> set_explicit_anonymous(true);
        SgClassDefinition *A$1_class_definition = A$1_class_declaration -> get_definition();

        //
        // Create a method "int one() { return 2; }" in A$1
        //
        {
            parameter_list = SageBuilder::buildFunctionParameterList();
            method_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("one", SgTypeInt::createType(), parameter_list, A$1_class_definition);
            method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
            method_body = method_definition -> get_body();

            //
            // Add the statement:  "return 2;" to the method body of one in A$1.
            //
            integer_value = SageBuilder::buildIntVal(2); //create an integer literal with value 0.
            return_statement = SageBuilder::buildReturnStmt(integer_value); // create a return statement.
            method_body -> append_statement(return_statement); // Add the return statement.

            A$1_class_definition -> append_member(method_declaration);
        }

        //    
        // Create a constructor initializer with the string argument "" passed to the anonymous class A$1
        // and identify its super class as A.
        //    
        string_value = SageBuilder::buildStringVal("");
        constructor_initializer = SageBuilder::buildConstructorInitializer(NULL,
                                                                           buildExprListExp(string_value),
                                                                           A_class_declaration -> get_type(),
                                                                           false,
                                                                           false,
                                                                           false,
                                                                           false);

        //    
        // Create a new expression with the constructor initializer.
        //    
        new_expression = SageBuilder::buildNewExp(A_class_declaration -> get_type(),
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
        parameter_list = SageBuilder::buildFunctionParameterList();
        method_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("cheat", A_class_declaration -> get_type(), parameter_list, A_class_definition);
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();

        return_statement = SageBuilder::buildReturnStmt(new_expression); // create a return statement. 
        method_body -> append_statement(return_statement);  // Add the return statement to the method's body.

        A_class_definition -> append_member(method_declaration); // Add method one declaration to A.
    }

    //
    // Create sourcefile A.java for class A.
    //
    SgSourceFile *Test_sourcefile = SageBuilder::buildJavaSourceFile(project, directory_name, p_package_definition, "Test");

    //
    // Build a class Test in package p and add it to the sourcefile Test.java
    //
    //     package p;
    //     class Test {
    //         public static void main(String[] args) {
    //             String output = lines("Once ", "upon ", "a ", "time ...");
    //             A a = new A("");
    //             A b = a.cheat();
    //             System.out.println("The value of One is " + b.one());
    //         }
    //     }
    //
    {
        SgClassDeclaration *Test_class_declaration = SageBuilder::buildJavaDefiningClassDeclaration(p_package_definition, "Test");
        SgClassDefinition *Test_class_definition = Test_class_declaration -> get_definition();
        Test_sourcefile -> get_class_list() -> get_java_class_list().push_back(Test_class_declaration);

        //
        //     String lines(String... lines) { return null; }
        //
        // TODO: build the above method !!!
        //
        initialized_name = SageBuilder::buildJavaFormalParameter(Rose::Frontend::Java::StringClassType, "lines", true /* is_var_args */);
        parameter_list = SageBuilder::buildFunctionParameterList(initialized_name);
        initialized_name -> set_parent(parameter_list);

        method_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("lines", Rose::Frontend::Java::StringClassType, parameter_list, Test_class_definition);
        method_declaration -> get_declarationModifier().get_storageModifier().setStatic();
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();

        null_expression = SageBuilder::buildNullExpression();
        return_statement = SageBuilder::buildReturnStmt(null_expression); // create a return statement. 
        method_body -> append_statement(return_statement);

        Test_class_definition -> append_member(method_declaration); // add method lines(...)

        //
        // Create a method "public static void main(String[] args) { ... }" in Test
        //
        SgArrayType *string_array_type = getUniqueJavaArrayType(Rose::Frontend::Java::StringClassType, 1);
        initialized_name = SageBuilder::buildJavaFormalParameter(string_array_type, "args");
        parameter_list = SageBuilder::buildFunctionParameterList(initialized_name);
        initialized_name -> set_parent(parameter_list);

        method_declaration = SageBuilder::buildDefiningMemberFunctionDeclaration("main", SgTypeVoid::createType(), parameter_list, Test_class_definition);
        method_declaration -> get_declarationModifier().get_storageModifier().setStatic();
        method_definition = isSgFunctionDefinition(method_declaration -> get_definition());
        method_body = method_definition -> get_body();

        Test_class_definition -> append_member(method_declaration); // add method main(...

        //
        // Construct the statement:
        //
        //     String output = lines("Once ", "upon ", "a ", "time ...");
        //
        // and add it to the method body.
        //
        //
        // Under the covers, the function "String lines(String...);" is implemented as "String lines(String[] args);".
        // Thus, we have to use the String array type to lookup the function.
        //
        type_list = SageBuilder::buildFunctionParameterTypeList();
        type_list -> append_argument(string_array_type);
        member_function_type = SageBuilder::buildMemberFunctionType(Rose::Frontend::Java::StringClassType, type_list, Test_class_definition, mfunc_specifier);
        method_symbol = Test_class_definition -> lookup_function_symbol("lines", member_function_type);
        ROSE_ASSERT(method_symbol);
        delete type_list;
        arguments = SageBuilder::buildExprListExp();
        arguments -> append_expression(SageBuilder::buildStringVal("Once "));
        arguments -> append_expression(SageBuilder::buildStringVal("upon "));
        arguments -> append_expression(SageBuilder::buildStringVal("a "));
        arguments -> append_expression(SageBuilder::buildStringVal("time ... "));
        function_call_exp = SageBuilder::buildFunctionCallExp(method_symbol, arguments);
        initializer = SageBuilder::buildAssignInitializer(function_call_exp);
        variable_declaration = SageBuilder::buildVariableDeclaration("output", Rose::Frontend::Java::StringClassType, initializer, method_body);
        variable_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);
        variable_declaration -> set_parent(method_body);
        method_body -> append_statement(variable_declaration);

        //    
        // Create a constructor initializer with the string argument "" passed to it.
        //    
        string_value = SageBuilder::buildStringVal("");
        constructor_initializer = SageBuilder::buildConstructorInitializer(NULL,
                                                                           buildExprListExp(string_value),
                                                                           A_class_declaration -> get_type(),
                                                                           false,
                                                                           false,
                                                                           false,
                                                                           false);

        //    
        // Create a new expression with the constructor initializer.
        //    
        new_expression = SageBuilder::buildNewExp(A_class_declaration -> get_type(),
                                                  NULL,  // exprListExp,
                                                  constructor_initializer,
                                                  NULL,  // expr
                                                  0,     // val
                                                  NULL); // funcDecl
        constructor_initializer -> set_parent(new_expression);

        //
        // Construct the statement  A a = new A(""); and add it to the method body
        //
        initializer = SageBuilder::buildAssignInitializer(new_expression);
        variable_declaration = SageBuilder::buildVariableDeclaration("a", A_class_declaration -> get_type(), initializer, method_body);
        variable_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);
        variable_declaration -> set_parent(method_body);
        method_body -> append_statement(variable_declaration);

        //
        // Create AST for "a.cheat()".
        //
        type_list = SageBuilder::buildFunctionParameterTypeList();
        member_function_type = SageBuilder::buildMemberFunctionType(A_class_declaration -> get_type(), type_list, A_class_definition, mfunc_specifier);
        method_symbol = A_class_definition -> lookup_function_symbol("cheat", member_function_type);
        ROSE_ASSERT(method_symbol);
        delete type_list;
        arguments = SageBuilder::buildExprListExp();
        function_call_exp = SageBuilder::buildFunctionCallExp(method_symbol, arguments);
        expression = SageBuilder::buildDotExp(buildVarRefExp("a", Test_class_definition), function_call_exp);

        //
        // Construct the statement  "A b = a.cheat();" and add it to the method body
        //
        initializer = SageBuilder::buildAssignInitializer(expression);
        variable_declaration = SageBuilder::buildVariableDeclaration("b", A_class_declaration -> get_type(), initializer, method_body);
        variable_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);
        variable_declaration -> set_parent(method_body);
        method_body -> append_statement(variable_declaration);

        //
        // Create AST for "\"The value of One is \" + b.one()".
        //
        type_list = SageBuilder::buildFunctionParameterTypeList();
        member_function_type = SageBuilder::buildMemberFunctionType(SgTypeInt::createType(), type_list, A_class_definition, mfunc_specifier);
        method_symbol = A_class_definition -> lookup_function_symbol("one", member_function_type);
        ROSE_ASSERT(method_symbol);
        delete type_list;
        arguments = SageBuilder::buildExprListExp();
        function_call_exp = SageBuilder::buildFunctionCallExp(method_symbol, arguments);
        expression = SageBuilder::buildDotExp(buildVarRefExp("b", Test_class_definition), function_call_exp);
        expression = SageBuilder::buildAddOp(buildStringVal("The value of One is "), expression);

        //
        // Retrieve the "System" class declaration; lookup the variable "out" in it. (I know that out is a member of System!)
        //
        SgClassDeclaration *System_class_declaration = SageInterface::findOrImportJavaClass(project, Rose::Frontend::Java::javaLangPackageDefinition, "System");
        ROSE_ASSERT(System_class_declaration);
        SgClassDefinition *System_class_definition = System_class_declaration -> get_definition();
        ROSE_ASSERT(System_class_definition);
        variable_symbol = System_class_definition -> lookup_variable_symbol("out");
        ROSE_ASSERT(variable_symbol);

        //
        // Find the method "println(String)" in out's class: PrintStream. (I know that "println(String)" is in PrintStream!)
        //
        class_type = isSgClassType(variable_symbol -> get_type());
        SgClassDeclaration *class_declaration_for_out = SageInterface::findOrImportJavaClass(project, class_type);
        ROSE_ASSERT(class_declaration_for_out);
        SgClassDefinition *class_definition_for_out = class_declaration_for_out -> get_definition();
        ROSE_ASSERT(class_definition_for_out);
        type_list = SageBuilder::buildFunctionParameterTypeList();
        type_list -> append_argument(Rose::Frontend::Java::StringClassType);
        member_function_type = SageBuilder::buildMemberFunctionType(SgTypeVoid::createType(), type_list, class_definition_for_out, mfunc_specifier);
        method_symbol = class_definition_for_out -> lookup_function_symbol("println", member_function_type);
        ROSE_ASSERT(method_symbol);
        delete type_list;

        //
        // Build AST function call for "println(\"The value of One is \" + b.one());".
        //
        arguments = SageBuilder::buildExprListExp();
        arguments -> append_expression(expression);
        function_call_exp = SageBuilder::buildFunctionCallExp(method_symbol, arguments);

        //
        // Construct the statement: "out.println(\"The value of One is \" + b.one());".
        //
        expression = SageBuilder::buildDotExp(buildVarRefExp("out", System_class_definition), function_call_exp);

        //
        // Create a type expression for System, complete the statement:
        //
        //     "System.out.println(\"The value of One is \" + b.one());"
        //
        // and add it the the main method.
        //
        type_expression = SageBuilder::buildJavaTypeExpression(System_class_declaration -> get_type());
        expression = SageBuilder::buildDotExp(type_expression, expression);
        expression_statement = SageBuilder::buildExprStatement(expression);
        method_body -> append_statement(expression_statement);

        //
        // Run findMain(...) test.
        //
        method_declaration = SageInterface::findJavaMain(Test_class_definition);
        ROSE_ASSERT(method_declaration);
        method_declaration = SageInterface::findJavaMain(Test_class_declaration -> get_type());
        ROSE_ASSERT(method_declaration);
        method_declaration = SageInterface::findJavaMain(A_class_definition);
        ROSE_ASSERT(! method_declaration);
        method_declaration = SageInterface::findJavaMain(A_class_declaration -> get_type());
        ROSE_ASSERT(! method_declaration);
    }

    setSourcePositionClassificationMode(e_sourcePositionTransformation);
    AstTests::runAllTests(project);

    int return_code = backend(project); // invoke backend compiler to generate object/binary files

    destroyTempDirectory(directory_name); // Get rid of the temporary directory and all its included temporary files.

    return return_code;
}

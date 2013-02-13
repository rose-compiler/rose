import org.eclipse.jdt.internal.compiler.batch.*;

import java.io.*;
import java.text.*;
import java.util.*;

import org.eclipse.jdt.core.compiler.*;
import org.eclipse.jdt.core.compiler.batch.*;
import org.eclipse.jdt.internal.compiler.*;
import org.eclipse.jdt.internal.compiler.Compiler;
import org.eclipse.jdt.internal.compiler.env.*;
import org.eclipse.jdt.internal.compiler.impl.*;
import org.eclipse.jdt.internal.compiler.ast.*;
import org.eclipse.jdt.internal.compiler.ast.Annotation;
import org.eclipse.jdt.internal.compiler.lookup.*;
import org.eclipse.jdt.internal.compiler.parser.*;
import org.eclipse.jdt.internal.compiler.problem.*;
import org.eclipse.jdt.internal.compiler.util.*;

// DQ (10/30/2010): Added support for reflection to get methods in implicitly included objects.
// import java.lang.reflect.*;


// DQ (10/12/2010): Make more like the OFP implementation (using Callable<Boolean> abstract base class). 
// class JavaTraversal {
import java.util.concurrent.Callable;
class JavaParser  implements Callable<Boolean> {
    // DQ (11/17/2010): This is the main class for the connection of the ECJ front-end to ROSE.
    // The design is that we use ECJ mostly unmodified, and use the visitor
    // traversal (ASTVisitor) of the ECJ AST.  Specifically we derive a class 
    // (ecjASTVisitory) from the abstract class (ASTVisitor in ECJ) and define
    // all of it's members so that we can support traversing the ECJ defined 
    // AST and construct the ROSE AST.  Most of the IR nodes used to support this
    // or borrowed from the existing C and C++ support in ROSE.  We will however
    // add any required IR nodes as needed.

    // -------------------------------------------------------------------------------------------
    public static native void cactionPushPackage(String package_name, JavaToken jToken);
    public static native void cactionPopPackage();

    // -------------------------------------------------------------------------------------------
    public static native void cactionTest();
    public static native void cactionCompilationUnitList(int argc, String[] argv);
    public static native void cactionCompilationUnitListEnd();
    public static native void cactionSetupObject();
    public static native void cactionSetupStringAndClassTypes();
    
    public static native void cactionParenthesizedExpression(int paren_count);
                              
    // These are used in the ecjASTVisitor (which is derived from the ECJ ASTVisitor class).
    public static native void cactionCompilationUnitDeclaration(String package_name, String filename, JavaToken jToken);
    public static native void cactionCompilationUnitDeclarationEnd(int java_numberOfStatements, JavaToken jToken);
    public static native void cactionTypeDeclaration(String package_name, String type_name, boolean is_interface, boolean is_enum, boolean is_abstract, boolean is_final, boolean is_private, boolean is_public, boolean is_protected, boolean is_static, boolean is_strictfp, JavaToken jToken);
    public static native void cactionTypeDeclarationHeader(boolean java_has_super_class, int java_numberOfinterfaces, int java_numberofTypeParameters, JavaToken jToken);
    public static native void cactionTypeDeclarationEnd(JavaToken jToken);

    // Need to change the names of the function parameters (should not all be "filename").
    public static native void cactionConstructorDeclaration(String filename, JavaToken jToken);
    public static native void cactionConstructorDeclarationHeader(String filename, boolean java_is_native, boolean java_is_private, int numberOfTypeParameters, int numberOfArguments, int numberOfThrows, JavaToken jToken);
    public static native void cactionConstructorDeclarationEnd(int java_numberOfStatements, JavaToken jToken);
    public static native void cactionExplicitConstructorCall(JavaToken jToken);
    public static native void cactionExplicitConstructorCallEnd(boolean is_implicit_super, boolean is_super, boolean has_qualification , int num_parameters, int numberOfTypeArguments, int numberOfArguments, JavaToken jToken);
    public static native void cactionMethodDeclaration(String name, JavaToken jToken);
    public static native void cactionMethodDeclarationHeader(String name, boolean java_is_abstract, boolean java_is_native, boolean java_is_static, boolean java_is_final, boolean java_is_synchronized, boolean java_is_public, boolean java_is_protected, boolean java_is_private, boolean java_is_strictfp, int numberOfTypeParameters, int numArguments, int numThrows, JavaToken jToken);
    public static native void cactionMethodDeclarationEnd(int numberOfStatements, JavaToken jToken);
    public static native void cactionTypeParameterReference(String package_name, String type_name, String type_parameter_name, JavaToken jToken);
    public static native void cactionTypeReference(String package_name, String type_name, JavaToken jToken);
    public static native void cactionArgument(String argumentName, JavaToken jToken);
    public static native void cactionArgumentEnd(String argumentName, boolean is_final, JavaToken jToken);
    public static native void cactionCatchArgument(String argumentName, JavaToken jToken);
    public static native void cactionCatchArgumentEnd(String argumentName, boolean is_final, JavaToken jToken);
//    public static native void cactionArrayTypeReference(String package_name, String type_name, int numberOfDimensions, JavaToken jToken);
    public static native void cactionArrayTypeReference(int numberOfDimensions, JavaToken jToken);
    public static native void cactionArrayTypeReferenceEnd(String filename, int numberOfDimensions, JavaToken jToken);
    public static native void cactionMessageSend(String packageName, String typeName, String functionName, JavaToken jToken);
//    public static native void cactionMessageSendEnd(boolean java_is_static, String packageName, String typeName, int num_dimensions, String functionName, int num_parameters, int numTypeArguments, int numArguments, JavaToken jToken);
    public static native void cactionMessageSendEnd(boolean java_is_static, boolean has_receiver, String functionName, int num_parameters, int numTypeArguments, int numArguments, JavaToken jToken);

    public static native void cactionStringLiteral(String value, JavaToken jToken);

    public static native void cactionAllocationExpression(JavaToken jToken);
    public static native void cactionAllocationExpressionEnd(boolean has_type, int numArguments, JavaToken jToken);
    public static native void cactionANDANDExpression(JavaToken jToken);
    public static native void cactionANDANDExpressionEnd(JavaToken jToken);
    public static native void cactionAnnotationMethodDeclaration(JavaToken jToken);
    public static native void cactionArrayAllocationExpression(JavaToken jToken);
    public static native void cactionArrayAllocationExpressionEnd(String nameOfType, int numberOfDimensions, boolean hasInitializers, JavaToken jToken);
    public static native void cactionArrayInitializer(JavaToken jToken);
    public static native void cactionArrayInitializerEnd(int numInitializers, JavaToken jToken);
    public static native void cactionArrayReference(JavaToken jToken);
    public static native void cactionArrayReferenceEnd(JavaToken jToken);
         
    public static native void cactionAssertStatement(JavaToken jToken);
    public static native void cactionAssertStatementEnd(boolean hasExceptionArgument, JavaToken jToken);
     
    public static native void cactionAssignment(JavaToken jToken);
    public static native void cactionAssignmentEnd(JavaToken jToken);
    public static native void cactionBinaryExpression(JavaToken jToken);
    public static native void cactionBinaryExpressionEnd(int java_operator_kind, JavaToken jToken);
    public static native void cactionBlock(JavaToken jToken);
    public static native void cactionBlockEnd(int java_numberOfStatement, JavaToken jTokens);
    public static native void cactionBreakStatement(String labelName, JavaToken jToken);
    public static native void cactionCaseStatement(boolean hasCaseExpression, JavaToken jToken);
    public static native void cactionCaseStatementEnd(boolean hasCaseExpression, JavaToken jToken);

    public static native void cactionCastExpression(JavaToken jToken);
    public static native void cactionCastExpressionEnd(JavaToken jToken);
    public static native void cactionCharLiteral(char value, JavaToken jToken);
    public static native void cactionClassLiteralAccess(JavaToken jToken);
    public static native void cactionClassLiteralAccessEnd(JavaToken jToken);
    public static native void cactionClinit(JavaToken jToken);
    public static native void cactionConditionalExpression(JavaToken jToken);
    public static native void cactionConditionalExpressionEnd(JavaToken jToken);
    public static native void cactionContinueStatement(String labelName, JavaToken jToken);
    public static native void cactionCompoundAssignment(JavaToken jToken);
    public static native void cactionCompoundAssignmentEnd(int java_operator_kind, JavaToken jToken);
    public static native void cactionDoStatement(JavaToken jToken);
    public static native void cactionDoStatementEnd(JavaToken jToken);
    public static native void cactionDoubleLiteral(double value, String source, JavaToken jToken);

    public static native void cactionEmptyStatement(JavaToken jToken);
    public static native void cactionEmptyStatementEnd(JavaToken jToken);

    public static native void cactionEqualExpression(JavaToken jToken);
    public static native void cactionEqualExpressionEnd(int java_operator_kind, JavaToken jToken);
    public static native void cactionExtendedStringLiteral(String value, JavaToken jToken);
    public static native void cactionFalseLiteral(JavaToken jToken);

    public static native void cactionFieldDeclarationEnd(String variableName, boolean java_is_enum_field, boolean java_hasInitializer, boolean java_is_final, boolean java_is_private, boolean java_is_protected, boolean java_is_public, boolean java_is_volatile, boolean java_is_synthetic, boolean java_is_static, boolean java_is_transient, JavaToken jToken);

    public static native void cactionFieldReference(String field_name, JavaToken jToken);
    public static native void cactionFieldReferenceEnd(boolean explicit_type, String field_name, JavaToken jToken);

    public static native void cactionFloatLiteral(float value, String source, JavaToken jToken);
     
    public static native void cactionForeachStatement(JavaToken jToken);
    public static native void cactionForeachStatementEnd(JavaToken jToken);
     
    public static native void cactionForStatement(JavaToken jToken);
    public static native void cactionForStatementEnd(int num_initializations, boolean has_condition, int num_increments, JavaToken jToken);
    public static native void cactionIfStatement(JavaToken jToken);
    public static native void cactionIfStatementEnd(boolean has_false_body, JavaToken jToken);

    public static native void cactionImportReference(boolean java_is_static, String package_name, String type_name, String name_suffix, boolean java_ContainsWildcard, JavaToken jToken);

    public static native void cactionInitializer(boolean java_is_static, String name, JavaToken jToken);
    public static native void cactionInitializerEnd(JavaToken jToken);
    public static native void cactionInstanceOfExpression(JavaToken jToken);
    public static native void cactionInstanceOfExpressionEnd(JavaToken jToken);
    public static native void cactionIntLiteral(int value, String source, JavaToken jToken);
/*    
    public static native void cactionJavadoc(JavaToken jToken);
    public static native void cactionJavadocClassScope(JavaToken jToken);
    public static native void cactionJavadocAllocationExpression(JavaToken jToken);
    public static native void cactionJavadocAllocationExpressionClassScope(JavaToken jToken);
    public static native void cactionJavadocArgumentExpression(JavaToken jToken);
    public static native void cactionJavadocArgumentExpressionClassScope(JavaToken jToken);
    public static native void cactionJavadocArrayQualifiedTypeReference(JavaToken jToken);
    public static native void cactionJavadocArrayQualifiedTypeReferenceClassScope(JavaToken jToken);
    public static native void cactionJavadocArraySingleTypeReference(JavaToken jToken);
    public static native void cactionJavadocArraySingleTypeReferenceClassScope(JavaToken jToken);
    public static native void cactionJavadocFieldReference(JavaToken jToken);
    public static native void cactionJavadocFieldReferenceClassScope(JavaToken jToken);
    public static native void cactionJavadocImplicitTypeReference(JavaToken jToken);
    public static native void cactionJavadocImplicitTypeReferenceClassScope(JavaToken jToken);
    public static native void cactionJavadocMessageSend(JavaToken jToken);
    public static native void cactionJavadocMessageSendClassScope(JavaToken jToken);
    public static native void cactionJavadocQualifiedTypeReference(JavaToken jToken);
    public static native void cactionJavadocQualifiedTypeReferenceClassScope(JavaToken jToken);

    public static native void cactionJavadocReturnStatement(JavaToken jToken);
    public static native void cactionJavadocReturnStatementClassScope(JavaToken jToken);
    public static native void cactionJavadocSingleNameReference(JavaToken jToken);
    public static native void cactionJavadocSingleNameReferenceClassScope(JavaToken jToken);
    public static native void cactionJavadocSingleTypeReference(JavaToken jToken);
    public static native void cactionJavadocSingleTypeReferenceClassScope(JavaToken jToken);
*/     
    public static native void cactionLabeledStatement(String labelName, JavaToken jToken);
    public static native void cactionLabeledStatementEnd(JavaToken jToken);

    public static native void cactionLocalDeclaration(String variableName, boolean java_is_final, JavaToken jToken);
    public static native void cactionLocalDeclarationEnd(String variableName, boolean is_initialized, boolean java_is_final, JavaToken jToken);

    public static native void cactionLongLiteral(long value, String source, JavaToken jToken);
    public static native void cactionMarkerAnnotation(JavaToken jToken);
    public static native void cactionMemberValuePair(JavaToken jToken);
    public static native void cactionStringLiteralConcatenation(JavaToken jToken);
    public static native void cactionNormalAnnotation(JavaToken jToken);
    public static native void cactionNullLiteral(JavaToken jToken);
    public static native void cactionORORExpression(JavaToken jToken);
    public static native void cactionORORExpressionEnd(JavaToken jToken);
    public static native void cactionParameterizedTypeReference(JavaToken jToken);
    public static native void cactionParameterizedTypeReferenceEnd(String package_name, String type_name, int java_numberOfTypeArguments, int java_dimensions, JavaToken jToken);
    public static native void cactionPostfixExpression(JavaToken jToken);
    public static native void cactionPostfixExpressionEnd(int java_operator_kind, JavaToken jToken);
    public static native void cactionPrefixExpression(JavaToken jToken);
    public static native void cactionPrefixExpressionEnd(int java_operator_kind, JavaToken jToken);
    public static native void cactionQualifiedAllocationExpression(JavaToken jToken);
    public static native void cactionQualifiedAllocationExpressionEnd(boolean has_type, boolean contains_enclosing_instance, int numberArguments, boolean isAnonymous, JavaToken jToken);
    public static native void cactionQualifiedSuperReference(JavaToken jToken);
    public static native void cactionQualifiedSuperReferenceEnd(JavaToken jToken);
    public static native void cactionQualifiedThisReference(JavaToken jToken);
    public static native void cactionQualifiedThisReferenceEnd(JavaToken jToken);

    public static native void cactionReturnStatement(JavaToken jToken);
    public static native void cactionReturnStatementEnd(boolean hasExpression, JavaToken jToken);

    public static native void cactionSingleMemberAnnotation(JavaToken jToken);
    public static native void cactionSingleNameReference(String package_name, String type_name, String variableName, JavaToken jToken);
    public static native void cactionSuperReference(JavaToken jToken);

    public static native void cactionSwitchStatement(JavaToken jToken);
    public static native void cactionSwitchStatementEnd(int numCases, boolean hasDefault, JavaToken jToken);

    public static native void cactionSynchronizedStatement(JavaToken jToken);
    public static native void cactionSynchronizedStatementEnd(JavaToken jToken);

    public static native void cactionThisReference(JavaToken jToken);

    public static native void cactionThrowStatement(JavaToken jToken);
    public static native void cactionThrowStatementEnd(JavaToken jToken);

    public static native void cactionTrueLiteral(JavaToken jToken);

    public static native void cactionTryStatement(int numCatchBlocks, boolean hasFinallyBlock, JavaToken jToken);
    public static native void cactionTryStatementEnd(int numCatchBlocks, boolean hasFinallyBlock, JavaToken jToken);

    public static native void cactionCatchBlockEnd(JavaToken jToken);

    public static native void cactionTypeParameter(String name, JavaToken jToken);
    public static native void cactionTypeParameterEnd(String name, boolean has_extends, int num_bounds, JavaToken jToken);
    public static native void cactionUnaryExpression(JavaToken jToken);
    public static native void cactionUnaryExpressionEnd(int java_operator_kind, JavaToken jToken);
    public static native void cactionWhileStatement(JavaToken jToken);
    public static native void cactionWhileStatementEnd(JavaToken jToken);
    public static native void cactionWildcard(JavaToken jToken);
    public static native void cactionWildcardEnd(boolean is_unbound, boolean has_extends_bound, boolean has_super_bound, JavaToken jToken);

    public static native void cactionInsertClassStart(String className, JavaToken jToken);
    public static native void cactionInsertClassEnd(String className, JavaToken jToken);

    public static native void cactionBuildClassSupportStart(String className, String externalName, boolean is_interface, boolean is_enum, boolean is_anonymous, JavaToken jToken);
    public static native void cactionBuildTypeParameter(String name, JavaToken jToken);
    public static native void cactionBuildClassExtendsAndImplementsSupport(int num_type_parameters, boolean has_super_class, int num_interfaces, JavaToken jToken);
    public static native void cactionBuildClassSupportEnd(String className, JavaToken jToken);
    public static native void cactionBuildArgumentSupport(String name, boolean is_var_args, boolean is_final, JavaToken jToken);
    public static native void cactionBuildMethodSupport(String methodName, boolean is_constructor, boolean is_abstract, boolean is_native, int num_arguments, boolean isUserDefined, JavaToken args_loc, JavaToken method_loc);
    public static native void cactionBuildFieldSupport(String fieldName, JavaToken jToken);
    public static native void cactionBuildInitializerSupport(boolean is_static, String name, JavaToken jToken);

    // Added new support functions for Argument IR nodes.
    public static native void cactionArgumentName(String name);
    public static native void cactionArgumentModifiers(int modifiers);

    // Support for primative types.
    public static native void generateBooleanType();
    public static native void generateByteType();
    public static native void generateCharType();
    public static native void generateIntType();
    public static native void generateShortType();
    public static native void generateFloatType();
    public static native void generateLongType();
    public static native void generateDoubleType();
    public static native void generateNullType();

    public static native void cactionGenerateToken(JavaToken t);
    public static native void cactionSetSourcePosition(JavaSourcePositionInformation sp);

    // Save the compilationResult as we process the CompilationUnitDeclaration class.
    // public CompilationResult rose_compilationResult;

    // DQ (10/12/2010): Added boolean value to report error to C++ calling program (similar to OFP).
    // public static boolean hasErrorOccurred = false;

    // DQ: This is the name of the C++ *.so file which has the implementations of the JNI functions.
    // The library with the C++ implementation of these function must be loaded in order to call the 
    // JNI functions.
    static { System.loadLibrary("JavaTraversal"); }

    // DQ (10/12/2010): Implemented abstract baseclass "call()" member function (similar to OFP).
    // This provides the support to detect errors and communicate them back to ROSE (C++ code).
    public Boolean call() throws Exception {
        // boolean error = false;
        // boolean error = true;

        // if (JavaParserSupport.verboseLevel > 0)
        //    System.out.println("Parser exiting normally");

        // return new Boolean(error);
        // return Boolean.valueOf(error);
        return Boolean.TRUE;
    } // end call()
}

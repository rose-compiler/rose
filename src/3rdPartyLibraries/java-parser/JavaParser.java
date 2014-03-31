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
class JavaParser implements Callable<Boolean> {
    // DQ (11/17/2010): This is the main class for the connection of the ECJ front-end to ROSE.
    // The design is that we use ECJ mostly unmodified, and use the visitor
    // traversal (ASTVisitor) of the ECJ AST.  Specifically we derive a class 
    // (ecjASTVisitory) from the abstract class (ASTVisitor in ECJ) and define
    // all of it's members so that we can support traversing the ECJ defined 
    // AST and construct the ROSE AST.  Most of the IR nodes used to support this
    // or borrowed from the existing C and C++ support in ROSE.  We will however
    // add any required IR nodes as needed.

    // -------------------------------------------------------------------------------------------
    public static native void cactionInsertImportedPackageOnDemand(String package_name, JavaToken jToken);
    public static native void cactionInsertImportedTypeOnDemand(String package_name, String type_name, JavaToken jToken);
    public static native void cactionInsertImportedType(String package_name, String type_name, JavaToken jToken);
    public static native void cactionInsertImportedStaticField(String name, JavaToken jToken);
    public static native void cactionPushPackage(String package_name, JavaToken jToken);
    public static native void cactionUpdatePushPackage(String package_name, JavaToken jToken);
    public static native void cactionPopPackage();
    public static native void cactionPushTypeScope(String package_name, String type_name, JavaToken jToken);
    public static native void cactionPopTypeScope();
    public static native void cactionPushTypeParameterScope(String package_name, String typename, JavaToken jToken);
    public static native void cactionPopTypeParameterScope(JavaToken jToken);

    // -------------------------------------------------------------------------------------------
    public static native void cactionTest();
    public static native void cactionCompilationUnitList();
    public static native boolean cactionIsSpecifiedSourceFile(String filename);
    public static native void cactionEcjFatalCompilationErrors(String filename);
    public static native void cactionCompilationUnitListEnd();
    public static native void cactionSetupBasicTypes();
    public static native void cactionParenthesizedExpression(int paren_count);
    public static native void cactionPackageAnnotations(int num_annotations, JavaToken jToken);
    public static native void cactionCompilationUnitDeclaration(String full_file_name, String package_name, String filename, JavaToken jToken);
    public static native void cactionCompilationUnitDeclarationEnd(JavaToken jToken);
    public static native void cactionCompilationUnitDeclarationError(String error_message, JavaToken jToken);
    public static native void cactionTypeDeclaration(String package_name, String type_name, int num_annotations, boolean has_super_class, boolean is_annotation_interface, boolean is_interface, boolean is_enum, boolean is_abstract, boolean is_final, boolean is_private, boolean is_public, boolean is_protected, boolean is_static, boolean is_strictfp, JavaToken jToken);
    public static native void cactionTypeDeclarationHeader(boolean java_has_super_class, int java_numberOfinterfaces, int java_numberofTypeParameters, JavaToken jToken);
    public static native void cactionTypeDeclarationEnd(JavaToken jToken);

    public static native void cactionConstructorDeclaration(String filename, int constructor_index, JavaToken jToken);
    public static native void cactionConstructorDeclarationHeader(String filename, boolean java_is_public, boolean java_is_protected, boolean java_is_private, int numberOfTypeParameters, int numberOfArguments, int numberOfThrows, JavaToken jToken);
    public static native void cactionConstructorDeclarationEnd(int num_annotations, int num_statements, JavaToken jToken);
    public static native void cactionExplicitConstructorCall(JavaToken jToken);
    public static native void cactionExplicitConstructorCallEnd(boolean is_implicit_super, boolean is_super, boolean has_qualification, String package_name, String type_name, int constructor_index, int numberOfTypeArguments, int numberOfArguments, JavaToken jToken);
    public static native void cactionMethodDeclaration(String name, int method_index, JavaToken jToken);
    public static native void cactionMethodDeclarationHeader(String name, boolean java_is_abstract, boolean java_is_native, boolean java_is_static, boolean java_is_final, boolean java_is_synchronized, boolean java_is_public, boolean java_is_protected, boolean java_is_private, boolean java_is_strictfp, int numberOfTypeParameters, int numArguments, int numThrows, JavaToken jToken);
    public static native void cactionMethodDeclarationEnd(int num_annotations, int num_statements, JavaToken jToken);
    public static native void cactionTypeParameterReference(String package_name, String type_name, int method_index, String type_parameter_name, JavaToken jToken);
    public static native void cactionTypeReference(String package_name, String type_name, JavaToken jToken);
    public static native void cactionQualifiedTypeReference(String package_name, String type_name, JavaToken jToken);
    public static native void cactionArgument(String argumentName, JavaToken jToken);
    public static native void cactionArgumentEnd(int num_annotations, String argument_name, JavaToken jToken);
    public static native void cactionCatchArgument(String argumentName, JavaToken jToken);
    public static native void cactionCatchArgumentEnd(int num_annotations, String argument_name, int number_of_types, boolean is_final, JavaToken jToken);
    public static native void cactionArrayTypeReference(int numberOfDimensions, JavaToken jToken);
    public static native void cactionArrayTypeReferenceEnd(String filename, int numberOfDimensions, JavaToken jToken);
    public static native void cactionMessageSend(String packageName, String typeName, String functionName, JavaToken jToken);
    public static native void cactionMessageSendEnd(boolean java_is_static, boolean has_receiver, String package_name, String type_name, String functionName, int method_index, int numTypeArguments, int numArguments, JavaToken jToken);
    public static native void cactionStringLiteral(String value, JavaToken jToken);
    public static native void cactionAllocationExpression(JavaToken jToken);
    public static native void cactionAllocationExpressionEnd(boolean has_type, int numArguments, JavaToken jToken);
    public static native void cactionANDANDExpression(JavaToken jToken);
    public static native void cactionANDANDExpressionEnd(JavaToken jToken);
    public static native void cactionAnnotationMethodDeclaration(String name, int method_index, JavaToken jToken);
    public static native void cactionAnnotationMethodDeclarationEnd(String name, int method_index, int num_annotations, boolean has_default, JavaToken jToken);
    public static native void cactionArrayAllocationExpression(JavaToken jToken);
    public static native void cactionArrayAllocationExpressionEnd(int numberOfDimensions, boolean hasInitializers, JavaToken jToken);
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
    public static native void cactionFieldDeclarationEnd(String variableName, int num_declarations, boolean java_is_enum_field, boolean java_hasInitializer, boolean java_is_final, boolean java_is_private, boolean java_is_protected, boolean java_is_public, boolean java_is_volatile, boolean java_is_synthetic, boolean java_is_static, boolean java_is_transient, JavaToken jToken);
    public static native void cactionFieldReference(String field_name, JavaToken jToken);
    public static native void cactionFieldReferenceEnd(boolean explicit_type, String field_name, JavaToken jToken);
    public static native void cactionFloatLiteral(float value, String source, JavaToken jToken);
    public static native void cactionForeachStatement(JavaToken jToken);
    public static native void cactionForeachStatementEnd(JavaToken jToken);
    public static native void cactionForStatement(JavaToken jToken);
    public static native void cactionForStatementEnd(int num_initializations, boolean has_condition, int num_increments, JavaToken jToken);
    public static native void cactionIfStatement(boolean has_false_body, JavaToken jToken);
    public static native void cactionIfStatementEnd(boolean has_false_body, JavaToken jToken);
    public static native void cactionImportReference(boolean java_is_static, String import_name, boolean java_ContainsWildcard, JavaToken jToken);
    public static native void cactionInitializer(boolean java_is_static, String name, int initializer_index, JavaToken jToken);
    public static native void cactionInitializerEnd(JavaToken jToken);
    public static native void cactionInstanceOfExpression(JavaToken jToken);
    public static native void cactionInstanceOfExpressionEnd(JavaToken jToken);
    public static native void cactionIntLiteral(int value, String source, JavaToken jToken);
    public static native void cactionLabeledStatement(String labelName, JavaToken jToken);
    public static native void cactionLabeledStatementEnd(JavaToken jToken);
    public static native void cactionLocalDeclaration(int num_annotations, String variableName, boolean java_is_final, JavaToken jToken);
    public static native void cactionLocalDeclarationEnd(String variableName, boolean is_initialized, JavaToken jToken);
    public static native void cactionLongLiteral(long value, String source, JavaToken jToken);
    public static native void cactionMarkerAnnotationEnd(JavaToken jToken);
    public static native void cactionMemberValuePairEnd(String name, JavaToken jToken);
    public static native void cactionStringLiteralConcatenation(JavaToken jToken);
    public static native void cactionNormalAnnotationEnd(int number_of_member_pairs, JavaToken jToken);
    public static native void cactionNullLiteral(JavaToken jToken);
    public static native void cactionORORExpression(JavaToken jToken);
    public static native void cactionORORExpressionEnd(JavaToken jToken);
    public static native void cactionParameterizedTypeReference(JavaToken jToken);
    public static native void cactionParameterizedTypeReferenceEnd(String package_name, String type_name, boolean is_parameterized, int java_numberOfTypeArguments, JavaToken jToken);
    public static native void cactionParameterizedQualifiedTypeReferenceEnd(String type_name, boolean is_parameterized, int java_numberOfTypeArguments, JavaToken jToken);
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
    public static native void cactionSingleMemberAnnotationEnd(JavaToken jToken);
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
    public static native void cactionTryStatementEnd(int numResources, int numCatchBlocks, boolean hasFinallyBlock, JavaToken jToken);
    public static native void cactionCatchBlockEnd(JavaToken jToken);
    public static native void cactionUnaryExpression(JavaToken jToken);
    public static native void cactionUnaryExpressionEnd(int java_operator_kind, JavaToken jToken);
    public static native void cactionWhileStatement(JavaToken jToken);
    public static native void cactionWhileStatementEnd(JavaToken jToken);
    public static native void cactionWildcard(JavaToken jToken);
    public static native void cactionWildcardEnd(boolean is_unbound, boolean has_extends_bound, boolean has_super_bound, JavaToken jToken);
    public static native void cactionSetupSourceFilename(String full_file_name);
    public static native void cactionClearSourceFilename();
    public static native void cactionInsertClassStart(String className, boolean is_interface, boolean is_enum, boolean is_anonymous, JavaToken jToken);
    public static native void cactionInsertClassEnd(String className, JavaToken jToken);
    public static native void cactionBuildClassSupportStart(String className, String externalName, boolean user_defined_class, boolean is_interface, boolean is_enum, boolean is_anonymous, JavaToken jToken);
    public static native void cactionUpdateClassSupportStart(String class_name, JavaToken jToken);
    public static native void cactionInsertTypeParameter(String name, JavaToken jToken);
    public static native void cactionBuildTypeParameterSupport(String package_name, String type_name, int method_index, String parameter_name, int num_bounds, JavaToken jToken);
    public static native void cactionUpdatePushMethodParameterScope(String package_name, String typename, JavaToken jToken);
    public static native void cactionUpdateTypeParameterSupport(String name, int method_index, int num_bounds, JavaToken jToken);
    public static native void cactionUpdatePopMethodParameterScope(JavaToken jToken);
    public static native void cactionBuildClassExtendsAndImplementsSupport(int num_type_parameters, boolean has_super_class, int num_interfaces, JavaToken jToken);
    public static native void cactionBuildClassSupportEnd(String className, int num_class_members, JavaToken jToken);
    public static native void cactionUpdateClassSupportEnd(String className, boolean has_super_class, int num_interfaces, int num_class_members, JavaToken jToken);
    public static native void cactionBuildInnerTypeSupport(String package_name, String type_name, JavaToken jToken);
    public static native void cactionUpdateInnerTypeSupport(String package_name, String type_name, JavaToken jToken);
    public static native void cactionBuildArgumentSupport(String name, String argument_type_name, boolean is_var_args, boolean is_final, JavaToken jToken);
    public static native void cactionUpdateArgumentSupport(int j, String argument_name, String argument_type_name, boolean is_var_args, boolean is_final, JavaToken jToken);
    public static native void cactionBuildMethodSupportStart(String methodName, int method_index, JavaToken method_loc);
    public static native void cactionUpdateMethodSupportStart(String method_name, int method_index, int num_formal_parameters, JavaToken jToken);
    public static native void cactionBuildMethodSupportEnd(String methodName, int method_index, boolean is_constructor, boolean is_abstract, boolean is_native, int num_type_parameters, int num_arguments, boolean is_compiler_generated, JavaToken method_loc, JavaToken args_loc);
    public static native void cactionUpdateMethodSupportEnd(String method_name, int method_index, boolean is_compiler_generated, int num_parameters, JavaToken args_location, JavaToken jToken);
    public static native void cactionBuildFieldSupport(String fieldName, JavaToken jToken);
    public static native void cactionUpdateFieldSupport(String name, JavaToken jToken);
    public static native void cactionBuildInitializerSupport(boolean is_static, String name, int initializer_index, JavaToken jToken);

    // Added new support functions for Argument IR nodes.
    public static native void cactionArgumentName(String name);
    public static native void cactionArgumentModifiers(int modifiers);

    //**********************************************************
    //*                                                        *
    //*    Start of JavaDoc Nodes.                             *
    //*                                                        *
    //**********************************************************
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
    //**********************************************************
    //*                                                        *
    //*    End of JavaDoc Nodes.                               *
    //*                                                        *
    //**********************************************************

    //
    // DQ (10/12/2010): Implemented abstract baseclass "call()" member function (similar to OFP).
    // This provides the support to detect errors and communicate them back to ROSE (C++ code).
    //
    public Boolean call() throws Exception {
        return Boolean.TRUE;
    }
}

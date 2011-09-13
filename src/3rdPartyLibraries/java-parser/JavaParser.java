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
import org.eclipse.jdt.internal.compiler.lookup.*;
import org.eclipse.jdt.internal.compiler.parser.*;
import org.eclipse.jdt.internal.compiler.problem.*;
import org.eclipse.jdt.internal.compiler.util.*;

// DQ (10/30/2010): Added support for reflection to get methods in implicitly included objects.
// import java.lang.reflect.*;


// DQ (10/12/2010): Make more like the OFP implementation (using Callable<Boolean> abstract base class). 
// class JavaTraversal {
import java.util.concurrent.Callable;
class JavaParser  implements Callable<Boolean>
   {
  // DQ (11/17/2010): This is the main class for the connection of the ECJ front-end to ROSE.
  // The design is that we use ECJ mostly unmodified, and use the visitor
  // traversal (ASTVisitor) of the ECJ AST.  Specifically we derive a class 
  // (ecjASTVisitory) from the abstract class (ASTVisitor in ECJ) and define
  // all of it's members so that we can support traversing the ECJ defined 
  // AST and construct the ROSE AST.  Most of the IR nodes used to support this
  // or borrowed from the existing C and C++ support in ROSE.  We will however
  // add any required IR nodes as needed.

     int verboseLevel = 0;

  // Build a specific constructor to handle the verbose option
     JavaParser ( int input_verboseLevel )
        {
          verboseLevel = input_verboseLevel;
          if (verboseLevel > 0)
             {
               System.out.println("In JavaParser constructor: verboseLevel = " + verboseLevel);
             }
        }
         
  // -------------------------------------------------------------------------------------------
     public native void cactionCompilationUnitList(int argc, String[] argv);

  // These are used in the ecjASTVisitor (which is derived from the ECJ ASTVisitor class).
     public native void cactionCompilationUnitDeclaration(String filename, JavaToken jToken);
     public native void cactionCompilationUnitDeclarationEnd(int java_numberOfStatements, JavaToken jToken);
     public native void cactionTypeDeclaration(String filename, JavaToken jToken);
  // public native void cactionTypeDeclaration(String filename, JavaToken jToken);
     public native void cactionTypeDeclarationEnd(String filename,int java_numberOfStatements, JavaToken jToken);

  // Need to change the names of the function parameters (should not all be "filename").
     public native void cactionConstructorDeclaration(String filename, boolean java_is_native, JavaToken jToken);
     public native void cactionConstructorDeclarationEnd(int java_numberOfStatements, JavaToken jToken);
     public native void cactionExplicitConstructorCall(String filename, JavaToken jToken);
     public native void cactionExplicitConstructorCallEnd(String filename, JavaToken jToken);
     public native void cactionMethodDeclaration(String filename, boolean java_is_abstract, boolean java_is_native, boolean java_is_static, boolean java_is_final, boolean java_is_synchronized, boolean java_is_public, boolean java_is_protected, boolean java_is_private, boolean java_is_strictfp, JavaToken jToken);
     public native void cactionSingleTypeReference(String filename, JavaToken jToken);
     public native void cactionArgument(String argumentName, int modifiers, JavaToken jToken);
     public native void cactionArrayTypeReference(String filename, JavaToken jToken);
     public native void cactionArrayTypeReferenceEnd(String filename, JavaToken jToken);
     public native void cactionMessageSend(String functionName, String associatedClassName, JavaToken jToken);

     public native void cactionMessageSendEnd(JavaToken jToken);

     public native void cactionQualifiedNameReference(String filename, JavaToken jToken);
     public native void cactionStringLiteral(String filename, JavaToken jToken);

     public native void cactionAllocationExpression(JavaToken jToken);
     public native void cactionAllocationExpressionEnd(String nameOfType, JavaToken jToken);
     public native void cactionANDANDExpression(JavaToken jToken);
     public native void cactionANDANDExpressionEnd(JavaToken jToken);
     public native void cactionAnnotationMethodDeclaration(JavaToken jToken);
     public native void cactionArgumentClassScope(String variableName, JavaToken jToken);
     public native void cactionArrayAllocationExpression(JavaToken jToken);
     public native void cactionArrayInitializer(JavaToken jToken);
     public native void cactionArrayQualifiedTypeReference(JavaToken jToken);
     public native void cactionArrayQualifiedTypeReferenceClassScope(JavaToken jToken);
     public native void cactionArrayReference(JavaToken jToken);
     public native void cactionArrayTypeReferenceClassScope(String filename, JavaToken jToken);
     
     public native void cactionAssertStatement(JavaToken jToken);
     public native void cactionAssertStatementEnd(boolean hasExceptionArgument, JavaToken jToken);
     
     public native void cactionAssignment(JavaToken jToken);
     public native void cactionAssignmentEnd(JavaToken jToken);
     public native void cactionBinaryExpression(JavaToken jToken);
     public native void cactionBinaryExpressionEnd(int java_operator_kind, JavaToken jToken);
     public native void cactionBlock(JavaToken jToken);
     public native void cactionBlockEnd(int java_numberOfStatement, JavaToken jTokens);
     public native void cactionBreakStatement(String labelName, JavaToken jToken);

     public native void cactionCaseStatement(boolean hasCaseExpression, JavaToken jToken);
     public native void cactionCaseStatementEnd(boolean hasCaseExpression, JavaToken jToken);

     public native void cactionCastExpression(JavaToken jToken);
     public native void cactionCastExpressionEnd(JavaToken jToken);
     public native void cactionCharLiteral(JavaToken jToken);
     public native void cactionClassLiteralAccess(JavaToken jToken);
     public native void cactionClinit(JavaToken jToken);
     public native void cactionConditionalExpression(JavaToken jToken);
     public native void cactionConditionalExpressionEnd(JavaToken jToken);
     public native void cactionContinueStatement(String labelName, JavaToken jToken);
     public native void cactionCompoundAssignment(JavaToken jToken);
     public native void cactionCompoundAssignmentEnd(int java_operator_kind, JavaToken jToken);
     public native void cactionDoStatement(JavaToken jToken);
     public native void cactionDoStatementEnd(JavaToken jToken);
     public native void cactionDoubleLiteral(JavaToken jToken);

     public native void cactionEmptyStatement(JavaToken jToken);
     public native void cactionEmptyStatementEnd(JavaToken jToken);

     public native void cactionEqualExpression(JavaToken jToken);
     public native void cactionEqualExpressionEnd(int java_operator_kind, JavaToken jToken);
     public native void cactionExtendedStringLiteral(JavaToken jToken);
     public native void cactionFalseLiteral(JavaToken jToken);

  // public native void cactionFieldDeclaration(String variableName, boolean java_hasInitializer, boolean java_is_final, boolean java_is_private, boolean java_is_protected, boolean java_is_public, boolean java_is_volatile, boolean java_is_synthetic, boolean java_is_static, boolean java_is_transient, JavaToken jToken);
  // public native void cactionFieldDeclarationEnd(String variableName, boolean java_hasInitializer, JavaToken jToken);
     public native void cactionFieldDeclarationEnd(String variableName, boolean java_hasInitializer, boolean java_is_final, boolean java_is_private, boolean java_is_protected, boolean java_is_public, boolean java_is_volatile, boolean java_is_synthetic, boolean java_is_static, boolean java_is_transient, JavaToken jToken);

     public native void cactionFieldReference(JavaToken jToken);
     public native void cactionFieldReferenceClassScope(JavaToken jToken);
     public native void cactionFloatLiteral(JavaToken jToken);
     
     public native void cactionForeachStatement(JavaToken jToken);
     public native void cactionForeachStatementEnd(JavaToken jToken);
     
     public native void cactionForStatement(JavaToken jToken);
     public native void cactionForStatementEnd(JavaToken jToken);
     public native void cactionIfStatement(JavaToken jToken);
     public native void cactionIfStatementEnd(int java_numberOfStatements, JavaToken jToken);

  // DQ (4/16/2011): I can't seem to get Boolean values to pass through the JNI C++ interface (so I will use an integer since that works fine).
  // public native void cactionImportReference(String path);
  // public native void cactionImportReference(String path , Boolean inputContainsWildcard );
  // public native void cactionImportReference(String path , int java_ContainsWildcard );
     public native void cactionImportReference(String path , int java_ContainsWildcard, JavaToken jToken);

     public native void cactionInitializer(JavaToken jToken);
     public native void cactionInstanceOfExpression(JavaToken jToken);
     public native void cactionInstanceOfExpressionEnd(JavaToken jToken);
  // public native void cactionIntLiteral();
  // public native void cactionIntLiteral(int value);
     public native void cactionIntLiteral(int value, JavaToken jToken);
     public native void cactionJavadoc(JavaToken jToken);
     public native void cactionJavadocClassScope(JavaToken jToken);
     public native void cactionJavadocAllocationExpression(JavaToken jToken);
     public native void cactionJavadocAllocationExpressionClassScope(JavaToken jToken);
     public native void cactionJavadocArgumentExpression(JavaToken jToken);
     public native void cactionJavadocArgumentExpressionClassScope(JavaToken jToken);
     public native void cactionJavadocArrayQualifiedTypeReference(JavaToken jToken);
     public native void cactionJavadocArrayQualifiedTypeReferenceClassScope(JavaToken jToken);
     public native void cactionJavadocArraySingleTypeReference(JavaToken jToken);
     public native void cactionJavadocArraySingleTypeReferenceClassScope(JavaToken jToken);
     public native void cactionJavadocFieldReference(JavaToken jToken);
     public native void cactionJavadocFieldReferenceClassScope(JavaToken jToken);
     public native void cactionJavadocImplicitTypeReference(JavaToken jToken);
     public native void cactionJavadocImplicitTypeReferenceClassScope(JavaToken jToken);
     public native void cactionJavadocMessageSend(JavaToken jToken);
     public native void cactionJavadocMessageSendClassScope(JavaToken jToken);
     public native void cactionJavadocQualifiedTypeReference(JavaToken jToken);
     public native void cactionJavadocQualifiedTypeReferenceClassScope(JavaToken jToken);

     public native void cactionJavadocReturnStatement(JavaToken jToken);
     public native void cactionJavadocReturnStatementClassScope(JavaToken jToken);
     public native void cactionJavadocSingleNameReference(JavaToken jToken);
     public native void cactionJavadocSingleNameReferenceClassScope(JavaToken jToken);
     public native void cactionJavadocSingleTypeReference(JavaToken jToken);
     public native void cactionJavadocSingleTypeReferenceClassScope(JavaToken jToken);
     
     public native void cactionLabeledStatement(JavaToken jToken);
     public native void cactionLabeledStatementEnd(String labelName, JavaToken jToken);

     public native void cactionLocalDeclaration(String variableName, boolean java_is_final, JavaToken jToken);
     public native void cactionLocalDeclarationEnd(String variableName, boolean java_is_final, JavaToken jToken);
     public native void cactionLocalDeclarationInitialization(JavaToken jToken);

     public native void cactionLongLiteral(JavaToken jToken);
     public native void cactionMarkerAnnotation(JavaToken jToken);
     public native void cactionMemberValuePair(JavaToken jToken);
     public native void cactionStringLiteralConcatenation(JavaToken jToken);
     public native void cactionNormalAnnotation(JavaToken jToken);
     public native void cactionNullLiteral(JavaToken jToken);
     public native void cactionORORExpression(JavaToken jToken);
     public native void cactionORORExpressionEnd(JavaToken jToken);
     public native void cactionParameterizedQualifiedTypeReference(JavaToken jToken);
     public native void cactionParameterizedQualifiedTypeReferenceClassScope(JavaToken jToken);
     public native void cactionParameterizedSingleTypeReference(JavaToken jToken);
     public native void cactionParameterizedSingleTypeReferenceEnd(String name, int java_numberOfTypeArguments, JavaToken jToken);
     public native void cactionParameterizedSingleTypeReferenceClassScope(JavaToken jToken);
     public native void cactionPostfixExpression(JavaToken jToken);
     public native void cactionPostfixExpressionEnd(int java_operator_kind, JavaToken jToken);
     public native void cactionPrefixExpression(JavaToken jToken);
     public native void cactionPrefixExpressionEnd(int java_operator_kind, JavaToken jToken);
     public native void cactionQualifiedAllocationExpression(JavaToken jToken);
     public native void cactionQualifiedSuperReference(JavaToken jToken);
     public native void cactionQualifiedSuperReferenceClassScope(JavaToken jToken);
     public native void cactionQualifiedThisReference(JavaToken jToken);
     public native void cactionQualifiedThisReferenceClassScope(JavaToken jToken);
     public native void cactionQualifiedTypeReference(JavaToken jToken);
     public native void cactionQualifiedTypeReferenceClassScope(JavaToken jToken);

     public native void cactionReturnStatement(JavaToken jToken);
     public native void cactionReturnStatementEnd(boolean hasExpression, JavaToken jToken);

     public native void cactionSingleMemberAnnotation(JavaToken jToken);
     public native void cactionSingleNameReference(String variableName, JavaToken jToken);
     public native void cactionSingleNameReferenceClassScope(JavaToken jToken);
     public native void cactionSuperReference(JavaToken jToken);

     public native void cactionSwitchStatement(JavaToken jToken);
     public native void cactionSwitchStatementEnd(int numCases, boolean hasDefault, JavaToken jToken);

     public native void cactionSynchronizedStatement(JavaToken jToken);
     public native void cactionSynchronizedStatementEnd(JavaToken jToken);

     public native void cactionThisReference(JavaToken jToken);
     public native void cactionThisReferenceClassScope(JavaToken jToken);

     public native void cactionThrowStatement(JavaToken jToken);
     public native void cactionThrowStatementEnd(JavaToken jToken);

     public native void cactionTrueLiteral(JavaToken jToken);

     public native void cactionTryStatement(int numCatchBlocks, boolean hasFinallyBlock, JavaToken jToken);
     public native void cactionTryStatementEnd(int numCatchBlocks, boolean hasFinallyBlock, JavaToken jToken);

     public native void cactionTypeParameter(JavaToken jToken);
     public native void cactionTypeParameterClassScope(JavaToken jToken);
     public native void cactionUnaryExpression(JavaToken jToken);
     public native void cactionUnaryExpressionEnd(int java_operator_kind, JavaToken jToken);
     public native void cactionWhileStatement(JavaToken jToken);
     public native void cactionWhileStatementEnd(JavaToken jToken);
     public native void cactionWildcard(JavaToken jToken);
     public native void cactionWildcardClassScope(JavaToken jToken);

  // These are static because they are called from the JavaParserSupport class which 
  // does not have a reference of the JavaParser object..  Not clear if it should.
  // Also it might be that all of the JNI functions should be static.
     public static native void cactionBuildImplicitClassSupportStart(String className);
     public static native void cactionBuildImplicitClassSupportEnd(int java_numberOfStatements, String className);
     public static native void cactionBuildImplicitMethodSupport(String methodName);
     public static native void cactionBuildImplicitFieldSupport(String fieldName);

  // Added new support functions for Argument IR nodes.
     public native void cactionArgumentName(String name);
     public native void cactionArgumentModifiers(int modifiers);
     public native void cactionArgumentEnd();

  // Type support
     public static native void cactionGenerateType(String typeName);
     public static native void cactionGenerateClassType(String className);

  // Closing support to finish up statement handling.
     public static native void cactionStatementEnd(String typeName);

  // public native void cactionMethodDeclarationEnd(String methodName);
  // public native void cactionMethodDeclarationEnd();
     public native void cactionMethodDeclarationEnd(int java_numberOfStatements, JavaToken jToken);

  // Build an array type using the base type that will be found on the astJavaTypeStack.
     public static native void cactionGenerateArrayType();

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

  // -------------------------------------------------------------------------------------------

     public void startParsingAST(CompilationUnitDeclaration unit /*, int input_veboseLevel */)
        {
       // Set the verbose level for ROSE specific processing on the Java specific ECJ/ROSE translation.
       // verboseLevel = input_veboseLevel;

       // Debugging support...
          if (verboseLevel > 0)
               System.out.println("Start parsing");

       // Example of how to call the 
       // traverseAST(unit);

          try
             {
            // Make a copy of the compiation unit so that we can compute source code positions.
            // rose_compilationResult = unit.compilationResult;
               JavaParserSupport.initialize(unit.compilationResult,verboseLevel);

            // Example of how to call the traversal using a better design that isolates out the traversal of the ECJ AST from the parser.
            // "final" is required because the traverse function requires the visitor to be final.
            // final ecjASTVisitor visitor = new ecjASTVisitor(this);
               ecjASTVisitor visitor = new ecjASTVisitor(this, unit);

               unit.traverse(visitor,unit.scope);

            // Experiment with error on Java side...catch on C++ side...
            // System.out.println("Exiting in JavaParser::startParsingAST()");
            // System.exit(1);
             }
          catch (Throwable e)
             {
               System.out.println("Caught error in JavaParser (Parser failed)");
               System.err.println(e);

            // Make sure we exit as quickly as possible to simplify debugging.
               System.exit(1);

            // Make sure we exit on any error so it is caught quickly.
            // System.exit(1);

            // throw e;
               return;
             }

       // Debugging support...
          if (verboseLevel > 0)
               System.out.println("Done parsing");
        }

  // DQ (10/12/2010): Implemented abstract baseclass "call()" member function (similar to OFP).
  // This provides the support to detect errors and communicate them back to ROSE (C++ code).
     public Boolean call() throws Exception
        {
       // boolean error = false;
       // boolean error = true;

          if (verboseLevel > 0)
               System.out.println("Parser exiting normally");

       // return new Boolean(error);
       // return Boolean.valueOf(error);
          return Boolean.TRUE;
        } // end call()

  // DQ (10/30/2010): Added boolean value to report error to C++ calling program (similar to OFP).
  /*  public static boolean getError()
        {
          return JavaTraversal.hasErrorOccurred;
        }
  */
  // End of JavaParser class
   }

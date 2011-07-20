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
     public native void cactionCompilationUnitDeclaration(String filename);
     public native void cactionTypeDeclaration(String filename);
     public native void cactionTypeDeclarationEnd(String filename);

  // Need to change the names of the function parameters (should not all be "filename").
     public native void cactionConstructorDeclaration(String filename);
     public native void cactionConstructorDeclarationEnd();
     public native void cactionExplicitConstructorCall(String filename);
     public native void cactionExplicitConstructorCallEnd(String filename);
     public native void cactionMethodDeclaration(String filename);
     public native void cactionSingleTypeReference(String filename);
     public native void cactionArgument(String argumentName, int modifiers);
     public native void cactionArrayTypeReference(String filename);
     public native void cactionMessageSend(String functionName, String associatedClassName);

     public native void cactionMessageSendEnd();

     public native void cactionQualifiedNameReference(String filename);
     public native void cactionStringLiteral(String filename);

     public native void cactionAllocationExpression();
     public native void cactionANDANDExpression();
     public native void cactionANDANDExpressionEnd();
     public native void cactionAnnotationMethodDeclaration();
     public native void cactionArgumentClassScope(String variableName);
     public native void cactionArrayAllocationExpression();
     public native void cactionArrayInitializer();
     public native void cactionArrayQualifiedTypeReference();
     public native void cactionArrayQualifiedTypeReferenceClassScope();
     public native void cactionArrayReference();
     public native void cactionArrayTypeReferenceClassScope(String filename);
     public native void cactionAssertStatement();
     public native void cactionAssignment();
     public native void cactionAssignmentEnd();
     public native void cactionBinaryExpression();
     public native void cactionBinaryExpressionEnd(int java_operator_kind);
     public native void cactionBlock();
     public native void cactionBlockEnd();
     public native void cactionBreakStatement();
     public native void cactionCaseStatement();
     public native void cactionCastExpression();
     public native void cactionCastExpressionEnd();
     public native void cactionCharLiteral();
     public native void cactionClassLiteralAccess();
     public native void cactionClinit();
     public native void cactionConditionalExpression();
     public native void cactionConditionalExpressionEnd();
     public native void cactionContinueStatement();
     public native void cactionCompoundAssignment();
     public native void cactionCompoundAssignmentEnd(int java_operator_kind);
     public native void cactionDoStatement();
     public native void cactionDoubleLiteral();
     public native void cactionEmptyStatement();
     public native void cactionEqualExpression();
     public native void cactionEqualExpressionEnd(int java_operator_kind);
     public native void cactionExtendedStringLiteral();
     public native void cactionFalseLiteral();
     public native void cactionFieldDeclaration();
     public native void cactionFieldReference();
     public native void cactionFieldReferenceClassScope();
     public native void cactionFloatLiteral();
     public native void cactionForeachStatement();
     public native void cactionForStatement();
     public native void cactionIfStatement();
     public native void cactionIfStatementEnd();

  // DQ (4/16/2011): I can't seem to get Boolean values to pass through the JNI C++ interface (so I will use an integer since that works fine).
  // public native void cactionImportReference(String path);
  // public native void cactionImportReference(String path , Boolean inputContainsWildcard );
     public native void cactionImportReference(String path , int java_ContainsWildcard );

     public native void cactionInitializer();
     public native void cactionInstanceOfExpression();
     public native void cactionInstanceOfExpressionEnd();
  // public native void cactionIntLiteral();
     public native void cactionIntLiteral(int value);
     public native void cactionJavadoc();
     public native void cactionJavadocClassScope();
     public native void cactionJavadocAllocationExpression();
     public native void cactionJavadocAllocationExpressionClassScope();
     public native void cactionJavadocArgumentExpression();
     public native void cactionJavadocArgumentExpressionClassScope();
     public native void cactionJavadocArrayQualifiedTypeReference();
     public native void cactionJavadocArrayQualifiedTypeReferenceClassScope();
     public native void cactionJavadocArraySingleTypeReference();
     public native void cactionJavadocArraySingleTypeReferenceClassScope();
     public native void cactionJavadocFieldReference();
     public native void cactionJavadocFieldReferenceClassScope();
     public native void cactionJavadocImplicitTypeReference();
     public native void cactionJavadocImplicitTypeReferenceClassScope();
     public native void cactionJavadocMessageSend();
     public native void cactionJavadocMessageSendClassScope();
     public native void cactionJavadocQualifiedTypeReference();
     public native void cactionJavadocQualifiedTypeReferenceClassScope();
     public native void cactionJavadocReturnStatement();
     public native void cactionJavadocReturnStatementClassScope();
     public native void cactionJavadocSingleNameReference();
     public native void cactionJavadocSingleNameReferenceClassScope();
     public native void cactionJavadocSingleTypeReference();
     public native void cactionJavadocSingleTypeReferenceClassScope();
     public native void cactionLabeledStatement();
     public native void cactionLocalDeclaration(String variableName);
     public native void cactionLocalDeclarationInitialization();
     public native void cactionLongLiteral();
     public native void cactionMarkerAnnotation();
     public native void cactionMemberValuePair();
     public native void cactionStringLiteralConcatenation();
     public native void cactionNormalAnnotation();
     public native void cactionNullLiteral();
     public native void cactionORORExpression();
     public native void cactionORORExpressionEnd();
     public native void cactionParameterizedQualifiedTypeReference();
     public native void cactionParameterizedQualifiedTypeReferenceClassScope();
     public native void cactionParameterizedSingleTypeReference();
     public native void cactionParameterizedSingleTypeReferenceClassScope();
     public native void cactionPostfixExpression();
     public native void cactionPostfixExpressionEnd(int java_operator_kind);
     public native void cactionPrefixExpression();
     public native void cactionPrefixExpressionEnd(int java_operator_kind);
     public native void cactionQualifiedAllocationExpression();
     public native void cactionQualifiedSuperReference();
     public native void cactionQualifiedSuperReferenceClassScope();
     public native void cactionQualifiedThisReference();
     public native void cactionQualifiedThisReferenceClassScope();
     public native void cactionQualifiedTypeReference();
     public native void cactionQualifiedTypeReferenceClassScope();
     public native void cactionReturnStatement();
     public native void cactionSingleMemberAnnotation();
     public native void cactionSingleNameReference(String variableName);
     public native void cactionSingleNameReferenceClassScope();
     public native void cactionSuperReference();
     public native void cactionSwitchStatement();
     public native void cactionSynchronizedStatement();
     public native void cactionThisReference();
     public native void cactionThisReferenceClassScope();
     public native void cactionThrowStatement();
     public native void cactionTrueLiteral();
     public native void cactionTryStatement();
     public native void cactionTypeParameter();
     public native void cactionTypeParameterClassScope();
     public native void cactionUnaryExpression();
     public native void cactionUnaryExpressionEnd(int java_operator_kind);
     public native void cactionWhileStatement();
     public native void cactionWildcard();
     public native void cactionWildcardClassScope();

  // These are static because they are called from the JavaParserSupport class which 
  // does not have a reference of the JavaParser object..  Not clear if it should.
  // Also it might be that all of the JNI functions should be static.
     public static native void cactionBuildImplicitClassSupportStart(String className);
     public static native void cactionBuildImplicitClassSupportEnd(String className);
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
     public native void cactionMethodDeclarationEnd();

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
               final ecjASTVisitor visitor = new ecjASTVisitor(this);
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
          boolean error   = true;
       // boolean verbose = true;

          if (error != false)
             {
               System.out.println("Parser failed");
             } 
            else
             {
            // Use the class's verbose level option to control output.
            // if (verbose)
               if (verboseLevel > 0)
                    System.out.println("Parser exiting normally");
             }// end else(parser exited normally)

          return new Boolean(error);
        } // end call()

  // DQ (10/30/2010): Added boolean value to report error to C++ calling program (similar to OFP).
  /*  public static boolean getError()
        {
          return JavaTraversal.hasErrorOccurred;
        }
  */
  // End of JavaParser class
   }

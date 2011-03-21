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
import java.lang.reflect.*;


class ecjASTVisitor extends ASTVisitor
   {
  // This class contains the visitor functions required to support a traversal over the high-level ECJ AST and 
  // call the JNI functions that will execute as C++ functions and construct the ROSE AST.

     final JavaParser java_parser;

     ecjASTVisitor (JavaParser x)
        {
          java_parser = x;
        }

  // visitor = new ASTVisitor()
     public boolean visit(AllocationExpression node,BlockScope scope)
        {
          System.out.println("Inside of visit (AllocationExpression,BlockScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionAllocationExpression();

          System.out.println("Leaving visit (AllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(AND_AND_Expression  node, BlockScope scope)
        {
          System.out.println("Inside of visit (AND_AND_Expression,BlockScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionAND_AND_Expression();

          System.out.println("Leaving visit (AND_AND_Expression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(AnnotationMethodDeclaration node,ClassScope classScope)
        {
          System.out.println("Inside of visit (AnnotationMethodDeclaration,ClassScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionAnnotationMethodDeclaration();

          System.out.println("Leaving visit (AnnotationMethodDeclaration,ClassScope)");

          return true; // do nothing by default, keep traversing
        }

     public boolean visit(Argument  node, BlockScope scope)
        {
          System.out.println("Inside of visit (Argument,BlockScope)");

          java_parser.cactionArgument("Argument_block_abc");

          System.out.println("Leaving visit (Argument,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Argument  node, ClassScope scope)
        {
          System.out.println("Inside of visit (Argument,ClassScope)");

          java_parser.cactionArgumentClassScope("Argument_class_abc");

          System.out.println("Leaving visit (Argument,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayAllocationExpression node,BlockScope scope)
        {
          System.out.println("Inside of visit (ArrayAllocationExpression,BlockScope)");

          java_parser.cactionArrayAllocationExpression();

          System.out.println("Leaving visit (ArrayAllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
		  }

     public boolean visit(ArrayInitializer  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ArrayInitializer,BlockScope)");

          java_parser.cactionArrayInitializer();

          System.out.println("Leaving visit (ArrayInitializer,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayQualifiedTypeReference node, BlockScope scope)
        {
          System.out.println("Inside of visit (ArrayQualifiedTypeReference,BlockScope)");

          java_parser.cactionArrayQualifiedTypeReference();

          System.out.println("Leaving visit (ArrayQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayQualifiedTypeReference node, ClassScope scope)
        {
          System.out.println("Inside of visit (ArrayQualifiedTypeReference,ClassScope)");

          java_parser.cactionArrayQualifiedTypeReferenceClassScope();

          System.out.println("Leaving visit (ArrayQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }
     public boolean visit(ArrayReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ArrayReference,BlockScope)");

          java_parser.cactionArrayReference();

          System.out.println("Leaving visit (ArrayReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayTypeReference  node, BlockScope scope)
        {
          java_parser.cactionArrayTypeReference("ArrayTypeReference_block_abc");
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayTypeReference  node, ClassScope scope)
        {
          java_parser.cactionArrayTypeReferenceClassScope("ArrayTypeReference_class_abc");
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(AssertStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (AssertStatement,BlockScope)");

          java_parser.cactionAssertStatement();

          System.out.println("Leaving visit (AssertStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Assignment  node, BlockScope scope)
        {
          System.out.println("Inside of visit Assignment(,BlockScope)");

          java_parser.cactionAssignment();

          System.out.println("Leaving visit (Assignment,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(BinaryExpression  node, BlockScope scope)
        {
          System.out.println("Inside of visit (BinaryExpression,BlockScope)");

          java_parser.cactionBinaryExpression();

          System.out.println("Leaving visit (BinaryExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Block  node, BlockScope scope)
        {
          System.out.println("Inside of visit (Block,BlockScope)");

          java_parser.cactionBlock();

          System.out.println("Leaving visit (Block,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(BreakStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (BreakStatement,BlockScope)");

          java_parser.cactionBreakStatement();

          System.out.println("Leaving visit (BreakStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(CaseStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (CaseStatement,BlockScope)");

          java_parser.cactionCaseStatement();

          System.out.println("Leaving visit (CaseStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(CastExpression  node, BlockScope scope)
        {
          System.out.println("Inside of visit (CastExpression,BlockScope)");

          java_parser.cactionCastExpression();

          System.out.println("Leaving visit (CastExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(CharLiteral  node, BlockScope scope)
        {
          System.out.println("Inside of visit (CharLiteral,BlockScope)");

          java_parser.cactionCharLiteral();

          System.out.println("Leaving visit (CharLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ClassLiteralAccess  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ClassLiteralAccess,BlockScope)");

          java_parser.cactionClassLiteralAccess();

          System.out.println("Leaving visit (ClassLiteralAccess,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Clinit  node, ClassScope scope)
        {
          System.out.println("Inside of visit (Clinit,ClassScope)");

          java_parser.cactionClinit();

          System.out.println("Leaving visit (Clinit,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( CompilationUnitDeclaration node, CompilationUnitScope scope)
        {
          System.out.println("Inside of visit (CompilationUnitDeclaration,CompilationUnitScope)");

       // Call the Java side of the JNI function.
          String s = new String(node.getFileName());
          System.out.println("Test A");
          java_parser.cactionCompilationUnitDeclaration(s);
          System.out.println("Leaving visit (CompilationUnitDeclaration,CompilationUnitScope)");

          return true; // do nothing by default, keep traversing
        }

     public boolean visit(CompoundAssignment  node, BlockScope scope)
        {
          System.out.println("Inside of visit (CompoundAssignment,BlockScope)");

          java_parser.cactionCompoundAssignment();

          System.out.println("Leaving visit (CompoundAssignment,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ConditionalExpression node,BlockScope scope)
        {
          System.out.println("Inside of visit (ConditionalExpression,BlockScope)");

          java_parser.cactionConditionalExpression();

          System.out.println("Leaving visit (ConditionalExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ConstructorDeclaration node, ClassScope scope)
        {
          System.out.println("Inside of visit (ConstructorDeclaration,ClassScope)");

       // char [] name = node.selector;
          System.out.println("Inside of visit (ConstructorDeclaration,ClassScope) method name = " + node.selector);

          java_parser.cactionConstructorDeclaration("ConstructorDeclaration_abc");

          System.out.println("Leaving visit (ConstructorDeclaration,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ContinueStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ContinueStatement,BlockScope)");

          java_parser.cactionContinueStatement();

          System.out.println("Leaving visit (ContinueStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(DoStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (DoStatement,BlockScope)");

          java_parser.cactionDoStatement();

          System.out.println("Leaving visit (DoStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(DoubleLiteral  node, BlockScope scope)
        {
          System.out.println("Inside of visit (DoubleLiteral,BlockScope)");

          java_parser.cactionDoubleLiteral();

          System.out.println("Leaving visit (DoubleLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(EmptyStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (EmptyStatement,BlockScope)");

          java_parser.cactionEmptyStatement();

          System.out.println("Leaving visit (EmptyStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(EqualExpression  node, BlockScope scope)
        {
          System.out.println("Inside of visit (EqualExpression,BlockScope)");

          java_parser.cactionEqualExpression();

          System.out.println("Leaving visit (EqualExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ExplicitConstructorCall node, BlockScope scope)
        {
       // java_parser.cactionExplicitConstructorCall("abc");
          if (node.qualification != null)
             {
            // Name qualification not implemented.
            // this.qualification.printExpression(0, output).append('.');
               System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: Name qualification");
             }

          if (node.typeArguments != null)
             {
            // output.append('<');
               System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: typeArguments");
               int max = node.typeArguments.length - 1;
               for (int j = 0; j < max; j++)
                  {
                 // node.typeArguments[j].print(0, output);
                 // output.append(", ");//$NON-NLS-1$
                  }
            // node.typeArguments[max].print(0, output);
            // output.append('>');
             }

          if (node.accessMode == ExplicitConstructorCall.This)
             {
            // output.append("this("); //$NON-NLS-1$
               System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: this");
             }
            else
             {
            // output.append("super("); //$NON-NLS-1$
            // System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: super()");
               java_parser.cactionExplicitConstructorCall("super");
             }

          if (node.arguments != null)
             {
               System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: arguments");
               for (int i = 0; i < node.arguments.length; i++)
                  {
                 // if (i > 0)
                 //      output.append(", "); //$NON-NLS-1$
                 // node.arguments[i].printExpression(0, output);
                  }
             }

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ExtendedStringLiteral node, BlockScope scope)
        {
          System.out.println("Inside of visit (ExtendedStringLiteral,BlockScope)");

          java_parser.cactionExtendedStringLiteral();

          System.out.println("Leaving visit (ExtendedStringLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(FalseLiteral  node, BlockScope scope) {
          System.out.println("Inside of visit (FalseLiteral,BlockScope)");

          java_parser.cactionFalseLiteral();

          System.out.println("Leaving visit (FalseLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(FieldDeclaration node, MethodScope scope)
        {
          System.out.println("Inside of visit (FieldDeclaration,BlockScope)");

          java_parser.cactionFieldDeclaration();

          System.out.println("Leaving visit (FieldDeclaration,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

          public boolean visit(FieldReference node, BlockScope scope) 
             {
               System.out.println("Sorry, not implemented in support for FieldReference(BlockScope): xxx");
          System.out.println("Inside of visit (FieldReference,BlockScope)");

          java_parser.cactionFieldReference();

          System.out.println("Leaving visit (FieldReference,BlockScope)");

               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(FieldReference  node, ClassScope scope)
             {
               System.out.println("Sorry, not implemented in support for FieldReference(ClassScope): xxx");
          System.out.println("Inside of visit (FieldReference,ClassScope)");

          java_parser.cactionFieldReferenceClassScope();

          System.out.println("Leaving visit (FieldReference,ClassScope)");

               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(FloatLiteral  node, BlockScope scope) {
          System.out.println("Inside of visit (FloatLiteral,BlockScope)");

          java_parser.cactionFloatLiteral();

          System.out.println("Leaving visit (FloatLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ForeachStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ForeachStatement,BlockScope)");

          java_parser.cactionForeachStatement();

          System.out.println("Leaving visit (ForeachStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ForStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ForStatement,BlockScope)");

          java_parser.cactionForStatement();

          System.out.println("Leaving visit (ForStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(IfStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (IfStatement,BlockScope)");

          java_parser.cactionIfStatement();

          System.out.println("Leaving visit (IfStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ImportReference  node, CompilationUnitScope scope)
        {
          System.out.println("Inside of visit (ImportReference,CompilationUnitScope)");

          java_parser.cactionImportReference();

          System.out.println("Leaving visit (ImportReference,CompilationUnitScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Initializer  node, MethodScope scope)
        {
          System.out.println("Inside of visit (Initializer,MethodScope)");

          java_parser.cactionInitializer();

          System.out.println("Leaving visit (Initializer,MethodScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( InstanceOfExpression node, BlockScope scope)
        {
          System.out.println("Inside of visit (InstanceOfExpression,BlockScope)");

          java_parser.cactionInstanceOfExpression();

          System.out.println("Leaving visit (InstanceOfExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(IntLiteral  node, BlockScope scope)
        {
          System.out.println("Inside of visit (IntLiteral,BlockScope)");

          java_parser.cactionIntLiteral();

          System.out.println("Leaving visit (IntLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Javadoc  node, BlockScope scope)
        {
          System.out.println("Inside of visit (Javadoc,BlockScope)");

          java_parser.cactionJavadoc();

          System.out.println("Leaving visit (Javadoc,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Javadoc  node, ClassScope scope)
        {
          System.out.println("Inside of visit (Javadoc,ClassScope)");

          java_parser.cactionJavadocClassScope();

          System.out.println("Leaving visit (Javadoc,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocAllocationExpression  node, BlockScope scope)
        {
          System.out.println("Inside of visit (JavadocAllocationExpression,BlockScope)");

          java_parser.cactionJavadocAllocationExpression();

          System.out.println("Leaving visit (JavadocAllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocAllocationExpression  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocAllocationExpression,ClassScope)");

          java_parser.cactionJavadocAllocationExpressionClassScope();

          System.out.println("Leaving visit (JavadocAllocationExpression,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArgumentExpression  node, BlockScope scope)
        {
          System.out.println("Inside of visit JavadocArgumentExpression(,BlockScope)");

          java_parser.cactionJavadocArgumentExpression();

          System.out.println("Leaving visit (JavadocArgumentExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArgumentExpression  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocArgumentExpression,ClassScope)");

          java_parser.cactionJavadocArgumentExpressionClassScope();

          System.out.println("Leaving visit (JavadocArgumentExpression,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArrayQualifiedTypeReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (JavadocArrayQualifiedTypeReference,BlockScope)");

          java_parser.cactionJavadocArrayQualifiedTypeReference();

          System.out.println("Leaving visit (JavadocArrayQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArrayQualifiedTypeReference  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocArrayQualifiedTypeReference,ClassScope)");

          java_parser.cactionJavadocArrayQualifiedTypeReferenceClassScope();

          System.out.println("Leaving visit (JavadocArrayQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArraySingleTypeReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (JavadocArraySingleTypeReference,BlockScope)");

          java_parser.cactionJavadocArraySingleTypeReference();

          System.out.println("Leaving visit (JavadocArraySingleTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArraySingleTypeReference  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocArraySingleTypeReference,ClassScope)");

          java_parser.cactionJavadocArraySingleTypeReferenceClassScope();

          System.out.println("Leaving visit (JavadocArraySingleTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocFieldReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (JavadocFieldReference,BlockScope)");

          java_parser.cactionJavadocFieldReference();

          System.out.println("Leaving visit (JavadocFieldReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocFieldReference  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocFieldReference,ClassScope)");

          java_parser.cactionJavadocFieldReferenceClassScope();

          System.out.println("Leaving visit (JavadocFieldReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocImplicitTypeReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (JavadocImplicitTypeReference,BlockScope)");

          java_parser.cactionJavadocImplicitTypeReference();

          System.out.println("Leaving visit (JavadocImplicitTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocImplicitTypeReference  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocImplicitTypeReference,ClassScope)");

          java_parser.cactionJavadocImplicitTypeReferenceClassScope();

          System.out.println("Leaving visit (JavadocImplicitTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocMessageSend  node, BlockScope scope)
        {
          System.out.println("Inside of visit (JavadocMessageSend,BlockScope)");

          java_parser.cactionJavadocMessageSend();

          System.out.println("Leaving visit (JavadocMessageSend,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocMessageSend  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocMessageSend,ClassScope)");

          java_parser.cactionJavadocMessageSendClassScope();

          System.out.println("Leaving visit (JavadocMessageSend,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocQualifiedTypeReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (JavadocQualifiedTypeReference,BlockScope)");

          java_parser.cactionJavadocQualifiedTypeReference();

          System.out.println("Leaving visit (JavadocQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocQualifiedTypeReference  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocQualifiedTypeReference,ClassScope)");

          java_parser.cactionJavadocQualifiedTypeReferenceClassScope();

          System.out.println("Leaving visit (JavadocQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocReturnStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (JavadocReturnStatement,BlockScope)");

          java_parser.cactionJavadocReturnStatement();

          System.out.println("Leaving visit (JavadocReturnStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocReturnStatement  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocReturnStatement,ClassScope)");

          java_parser.cactionJavadocReturnStatementClassScope();

          System.out.println("Leaving visit (JavadocReturnStatement,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocSingleNameReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (JavadocSingleNameReference,BlockScope)");

          java_parser.cactionJavadocSingleNameReference();

          System.out.println("Leaving visit (JavadocSingleNameReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocSingleNameReference  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocSingleNameReference,ClassScope)");

          java_parser.cactionJavadocSingleNameReferenceClassScope();

          System.out.println("Leaving visit (JavadocSingleNameReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocSingleTypeReference  node, BlockScope scope) {
          System.out.println("Inside of visit (JavadocSingleTypeReference,BlockScope)");

          java_parser.cactionJavadocSingleTypeReference();

          System.out.println("Leaving visit (JavadocSingleTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocSingleTypeReference  node, ClassScope scope)
        {
          System.out.println("Inside of visit (JavadocSingleTypeReference,ClassScope)");

          java_parser.cactionJavadocSingleTypeReferenceClassScope();

          System.out.println("Leaving visit (JavadocSingleTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(LabeledStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (LabeledStatement,BlockScope)");

          java_parser.cactionLabeledStatement();

          System.out.println("Leaving visit (LabeledStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(LocalDeclaration  node, BlockScope scope)
        {
          System.out.println("Inside of visit (LocalDeclaration,BlockScope)");

          java_parser.cactionLocalDeclaration();

          System.out.println("Leaving visit (LocalDeclaration,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(LongLiteral  node, BlockScope scope)
        {
          System.out.println("Inside of visit (LongLiteral,BlockScope)");

          java_parser.cactionLongLiteral();

          System.out.println("Leaving visit (LongLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(MarkerAnnotation  node, BlockScope scope)
        {
          System.out.println("Inside of visit (MarkerAnnotation,BlockScope)");

          java_parser.cactionMarkerAnnotation();

          System.out.println("Leaving visit (MarkerAnnotation,BlockScope)");

          return true;
        }

     public boolean visit(MemberValuePair  node, BlockScope scope)
        {
          System.out.println("Inside of visit (MemberValuePair,BlockScope)");

          java_parser.cactionMemberValuePair();

          System.out.println("Leaving visit (MemberValuePair,BlockScope)");

          return true;
        }

          public boolean visit(MessageSend  node, BlockScope scope)
             {
            // java_parser.cactionMessageSend("abc");
               JavaParserSupport.sourcePosition(node);

            /* Debugging code.
               try
                  {
                  // System.out is a QualifiedNameReference
                     System.out.println("node.receiver: name of type = " + node.receiver.getClass().toString());
                  }
               catch (Throwable e)
                  {
                    System.err.println(e);
                  }
             */

               if (node.typeArguments != null)
                  {
                    System.out.println("Sorry, not implemented in support for MessageSend: typeArguments");
                    for (int i = 0, typeArgumentsLength = node.typeArguments.length; i < typeArgumentsLength; i++)
                       {
                      // node.typeArguments[i].traverse(visitor, blockScope);
                       }
                  }

               if (node.arguments != null)
                  {
                    System.out.println("Sorry, not implemented in support for MessageSend: arguments");
                    int argumentsLength = node.arguments.length;
                    for (int i = 0; i < argumentsLength; i++)
                       {
                      // node.arguments[i].traverse(visitor, blockScope);
                       }
                  }

               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(MethodDeclaration  node, ClassScope scope)
        {
          System.out.println("Inside of visit (MethodDeclaration,ClassScope)");

          char [] name = node.selector;
          System.out.println("Inside of visit (MethodDeclaration,ClassScope) method name = " + name);

          java_parser.cactionMethodDeclaration("MethodDeclaration_abc");
       // java_parser.cactionMethodDeclaration();

          System.out.println("Leaving visit (MethodDeclaration,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( StringLiteralConcatenation node, BlockScope scope)
        {
          System.out.println("Inside of visit (StringLiteralConcatenation,BlockScope)");

          java_parser.cactionStringLiteralConcatenation();

          System.out.println("Leaving visit (StringLiteralConcatenation,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(NormalAnnotation  node, BlockScope scope)
        {
          System.out.println("Inside of visit (NormalAnnotation,BlockScope)");

          java_parser.cactionNormalAnnotation();

          System.out.println("Leaving visit (NormalAnnotation,BlockScope)");

          return true;
        }

     public boolean visit(NullLiteral  node, BlockScope scope)
        {
          System.out.println("Inside of visit (NullLiteral,BlockScope)");

          java_parser.cactionNullLiteral();

          System.out.println("Leaving visit (NullLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(OR_OR_Expression  node, BlockScope scope)
        {
          System.out.println("Inside of visit (OR_OR_Expression,BlockScope)");

          java_parser.cactionOR_OR_Expression();

          System.out.println("Leaving visit (OR_OR_Expression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ParameterizedQualifiedTypeReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ParameterizedQualifiedTypeReference,BlockScope)");

          java_parser.cactionParameterizedQualifiedTypeReference();

          System.out.println("Leaving visit (ParameterizedQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ParameterizedQualifiedTypeReference  node, ClassScope scope)
        {
          System.out.println("Inside of visit (ParameterizedQualifiedTypeReference,ClassScope)");

          java_parser.cactionParameterizedQualifiedTypeReferenceClassScope();

          System.out.println("Leaving visit (ParameterizedQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ParameterizedSingleTypeReference  node, BlockScope scope) {
          System.out.println("Inside of visit (ParameterizedSingleTypeReference,BlockScope)");

          java_parser.cactionParameterizedSingleTypeReference();

          System.out.println("Leaving visit (ParameterizedSingleTypeReference,BlockScope)");

         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ParameterizedSingleTypeReference  node, ClassScope scope) {
          System.out.println("Inside of visit (ParameterizedSingleTypeReference,ClassScope)");

          java_parser.cactionParameterizedSingleTypeReferenceClassScope();

          System.out.println("Leaving visit (ParameterizedSingleTypeReference,ClassScope)");

         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(PostfixExpression  node, BlockScope scope) {
          System.out.println("Inside of visit (PostfixExpression,BlockScope)");

          java_parser.cactionPostfixExpression();

          System.out.println("Leaving visit (PostfixExpression,BlockScope)");

         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(PrefixExpression  node, BlockScope scope) {
          System.out.println("Inside of visit (PrefixExpression,BlockScope)");

          java_parser.cactionPrefixExpression();

          System.out.println("Leaving visit (PrefixExpression,BlockScope)");

         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(QualifiedAllocationExpression node, BlockScope scope)
        {
          System.out.println("Inside of visit (QualifiedAllocationExpression,BlockScope)");

          java_parser.cactionQualifiedAllocationExpression();

          System.out.println("Leaving visit (QualifiedAllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

          public boolean visit(QualifiedNameReference node, BlockScope scope)
             {
            // Note that if we want to traverse this object in more detail then the QualifiedNameReference::traverse() functions can/should be modified.
               java_parser.cactionQualifiedNameReference("QualifiedNameReference_block_abc");

            // This is a reference to a variable (non data member)...? Is this correct?
               System.out.println("Sorry, not implemented in support for QualifiedNameReference(BlockScope): variable");

            // set the generic cast after the fact, once the type expectation is fully known (no need for strict cast)
               FieldBinding field = null;
               int length = node.otherBindings == null ? 0 : node.otherBindings.length;
               if (length == 0)
                  {
                    if ((node.bits & Binding.FIELD) != 0 && node.binding != null && node.binding.isValidBinding())
                       {
                         System.out.println("case of length == 0 and (...)");
                         field = (FieldBinding) node.binding;
                       }
                  }
                 else
                  {
                    System.out.println("case of length != 0");
                    field  = node.otherBindings[length-1];
                  }

               if (field != null)
                  {
                    System.out.println("Sorry, not implemented in support for QualifiedNameReference(BlockScope): field = " + field.toString());

                    Class cls = field.getClass();
                    System.out.println("----- field in class = " + cls.toString());
                    System.out.println("----- field in class = " + cls.toString());
                  }
                 else
                  {
                 // I don't know what this is, I have not seen an example of this case.
                    System.out.println("Sorry, not implemented in support for QualifiedNameReference(BlockScope): non-field");
                  }

            // Output the qualified name using the tokens (awful approach)
               for (int i = 0; i < node.tokens.length; i++)
                  {
                 // Note that the qualified name (parent classes fo the data member) appear to only be 
                 // available via the tokens (sort of ugly, I think; but this is the best I can figure out so far).

                    String tokenName = new String(node.tokens[i]);
                    System.out.println("----- tokens for qualification = " + tokenName);

                 // For the set of classes up to (but not including) the last data member reference we need to build support for the classes.
                    if (i < node.tokens.length-1)
                       {
                      // Finding the class has to be done in "try" ... "catch" blocks.
                         Class cls = null;
                         try
                            {
                           // Fortunately we can get the class from the introspection...is there a better way?
                              cls = Class.forName("java.lang."+tokenName);

                           // DQ (11/3/2010): This will generate an error which will be caught and force JavaTraversal.hasErrorOccurred 
                           // to be set and then the C++ side will query the error status and detect the error.  Thanks Thomas!
                           // cls = Class.forName("java.lang");
                            }
                         catch (Throwable e)
                            {
                              System.err.println(e);

                           // Make sure we exit on any error so it is caught quickly.
                           // System.exit(1);
                              JavaTraversal.hasErrorOccurred = true;

                              return false;
                            }

                         if (cls != null)
                            {
                           // We need to build these inplicitly referenced classes and also all
                           // of their public interface (member functions and data members).
                              System.out.println("----- tokens represents a class = " + tokenName);

                           // Build support for class (if we have not seen it before then it was implicitly 
                           // included (imported?) which is why this handling is father complex.
                              System.out.println("----- add support for class = " + cls.toString());
                              String className = tokenName;
                              JavaParserSupport.buildImplicitClassSupport("java.lang." + className);

                           // Experiment with error on Java side...catch on C++ side...
                           // System.out.println("Exiting in ecjASTVisitor::visit(QualifiedNameReference,BlockScope)");
                           // System.exit(1);

                              System.out.println("DONE: ----- add support for class = " + cls.toString() + " className = " + className);
                            }
                       }
                      else
                       {
                      // This is the last token standing for the data member reference (the "field" defined above).
                       }
                  }

               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(QualifiedNameReference node, ClassScope scope)
             {
            // This is a reference to a data member.
               System.out.println("Sorry, not implemented in support for QualifiedNameReference(ClassScope): data member");

               java_parser.cactionQualifiedNameReference("QualifiedNameReference_class_abc");

/*
	if (runtimeTimeType == null || compileTimeType == null)
     return;
	// set the generic cast after the fact, once the type expectation is fully known (no need for strict cast)
	FieldBinding field = null;
	int length = this.otherBindings == null ? 0 : this.otherBindings.length;
	if (length == 0) {
     if ((this.bits & Binding.FIELD) != 0 && this.binding != null && this.binding.isValidBinding()) {
     	field = (FieldBinding) this.binding;
     }
	} else {
     field  = this.otherBindings[length-1];
	}
*/
               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(QualifiedSuperReference node, BlockScope scope)
        {
          System.out.println("Inside of visit (QualifiedSuperReference,BlockScope)");

          java_parser.cactionQualifiedSuperReference();

          System.out.println("Leaving visit (QualifiedSuperReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( QualifiedSuperReference node, ClassScope scope)
        {
          System.out.println("Inside of visit (QualifiedSuperReference,ClassScope)");

          java_parser.cactionQualifiedSuperReferenceClassScope();

          System.out.println("Leaving visit (QualifiedSuperReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(QualifiedThisReference node, BlockScope scope)
        {
          System.out.println("Inside of visit (QualifiedThisReference,BlockScope)");

          java_parser.cactionQualifiedThisReference();

          System.out.println("Leaving visit (QualifiedThisReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( QualifiedThisReference node, ClassScope scope)
        {
          System.out.println("Inside of visit (QualifiedThisReference,ClassScope)");

          java_parser.cactionQualifiedThisReferenceClassScope();

          System.out.println("Leaving visit (QualifiedThisReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(QualifiedTypeReference node, BlockScope scope)
        {
          System.out.println("Inside of visit (QualifiedTypeReference,BlockScope)");

          java_parser.cactionQualifiedTypeReference();

          System.out.println("Leaving visit (QualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(QualifiedTypeReference node, ClassScope scope)
        {
          System.out.println("Inside of visit (QualifiedTypeReference,ClassScope)");

          java_parser.cactionQualifiedTypeReferenceClassScope();

          System.out.println("Leaving visit (QualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ReturnStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ReturnStatement,BlockScope)");

          java_parser.cactionReturnStatement();

          System.out.println("Leaving visit (ReturnStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SingleMemberAnnotation  node, BlockScope scope)
        {
          System.out.println("Inside of visit (SingleMemberAnnotation,BlockScope)");

          java_parser.cactionSingleMemberAnnotation();

          System.out.println("Leaving visit (SingleMemberAnnotation,BlockScope)");

          return true;
        }

     public boolean visit(SingleNameReference node, BlockScope scope)
        {
          System.out.println("Inside of visit (SingleNameReference,BlockScope)");

          java_parser.cactionSingleNameReference();

          System.out.println("Leaving visit (SingleNameReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SingleNameReference node, ClassScope scope)
        {
          System.out.println("Inside of visit (SingleNameReference,ClassScope)");

          java_parser.cactionSingleNameReferenceClassScope();

          System.out.println("Leaving visit (SingleNameReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

          public boolean visit(SingleTypeReference node, BlockScope scope)
             {
            // java_parser.cactionSingleTypeReference("abc");

               if (node.resolvedType != null)
                  {
                    java_parser.cactionSingleTypeReference("SingleTypeReference_block_abc");
                 // char[][] char_string = node.getTypeName();
                 // System.out.println(char_string);
                 // String typename = new String(node.getTypeName().toString());
                 // String typename = node.getTypeName().toString();
                    String typename = node.toString();
                    System.out.println("Sorry, not implemented SingleTypeReference (node.resolvedType != NULL): typename = " + typename);
                  }
                 else
                  {
                    System.out.println("Sorry, not implemented SingleTypeReference: node.resolvedType == NULL");
                  }

               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(SingleTypeReference node, ClassScope scope)
             {
               java_parser.cactionSingleTypeReference("SingleTypeReference_class_abc");
               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(StringLiteral  node, BlockScope scope)
        {
          System.out.println("Inside of visit (StringLiteral,BlockScope)");

          java_parser.cactionStringLiteral("StringLiteral_abc");

          System.out.println("Leaving visit (StringLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SuperReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (SuperReference,BlockScope)");

          java_parser.cactionSuperReference();

          System.out.println("Leaving visit (SuperReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SwitchStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (SwitchStatement,BlockScope)");

          java_parser.cactionSwitchStatement();

          System.out.println("Leaving visit (SwitchStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SynchronizedStatement node, BlockScope scope)
        {
          System.out.println("Inside of visit (SynchronizedStatement,BlockScope)");

          java_parser.cactionSynchronizedStatement();

          System.out.println("Leaving visit (SynchronizedStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ThisReference  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ThisReference,BlockScope)");

          java_parser.cactionThisReference();

          System.out.println("Leaving visit (ThisReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ThisReference  node, ClassScope scope)
        {
          System.out.println("Inside of visit (ThisReference,ClassScope)");

          java_parser.cactionThisReferenceClassScope();

          System.out.println("Leaving visit (ThisReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ThrowStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (ThrowStatement,BlockScope)");

          java_parser.cactionThrowStatement();

          System.out.println("Leaving visit (ThrowStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TrueLiteral  node, BlockScope scope)
        {
          System.out.println("Inside of visit (TrueLiteral,BlockScope)");

          java_parser.cactionTrueLiteral();

          System.out.println("Leaving visit (TrueLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TryStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (TryStatement,BlockScope)");

          java_parser.cactionTryStatement();

          System.out.println("Leaving visit (TryStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeDeclaration node,BlockScope scope)
        {
          System.out.println("visit TypeDeclaration -- BlockScope");
          String typename = new String(node.name);
          java_parser.cactionTypeDeclaration(typename);

          System.out.println("Leaving visit (TypeDeclaration,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeDeclaration node,ClassScope scope)
        {
          System.out.println("visit TypeDeclaration -- ClassScope");
          String typename = new String(node.name);
          java_parser.cactionTypeDeclaration(typename);

          System.out.println("Leaving visit (TypeDeclaration,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeDeclaration node,CompilationUnitScope scope)
        {
       // System.out.println("visit TypeDeclaration -- CompilationUnitScope");
          System.out.println("Inside of visit (TypeDeclaration,CompilationUnitScope)");

          String typename = new String(node.name);
          java_parser.cactionTypeDeclaration(typename);

          System.out.println("Leaving visit (TypeDeclaration,CompilationUnitScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeParameter  node, BlockScope scope) {
          System.out.println("Inside of visit (TypeParameter,BlockScope)");

          java_parser.cactionTypeParameter();

          System.out.println("Leaving visit (TypeParameter,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeParameter  node, ClassScope scope) {
          System.out.println("Inside of visit (TypeParameter,ClassScope)");

          java_parser.cactionTypeParameterClassScope();

          System.out.println("Leaving visit (TypeParameter,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(UnaryExpression  node, BlockScope scope)
        {
          System.out.println("Inside of visit (UnaryExpression,BlockScope)");

          java_parser.cactionUnaryExpression();

          System.out.println("Leaving visit (UnaryExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(WhileStatement  node, BlockScope scope)
        {
          System.out.println("Inside of visit (WhileStatement,BlockScope)");

          java_parser.cactionWhileStatement();

          System.out.println("Leaving visit (,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Wildcard  node, BlockScope scope)
        {
          System.out.println("Inside of visit (Wildcard,BlockScope)");

          java_parser.cactionWildcard();

          System.out.println("Leaving visit (Wildcard,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Wildcard  node, ClassScope scope)
        {
          System.out.println("Inside of visit (Wildcard,ClassScope)");

          java_parser.cactionWildcardClassScope();

          System.out.println("Leaving visit (Wildcard,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }


  // DQ (3/18/2011): Not clear if I need this yet.
     public void endVisit(AllocationExpression node, BlockScope scope)
        {
        // do nothing by default
        }
     public void endVisit(AND_AND_Expression  node, BlockScope scope)
        {
       // do nothing by default
        }
     public void endVisit(AnnotationMethodDeclaration node, ClassScope classScope)
        {
       // do nothing by default
        }
     public void endVisit(Argument  node, BlockScope scope)
        {
       // do nothing by default
        }
     public void endVisit(Argument  node,ClassScope scope)
        {
       // do nothing by default
        }
     public void endVisit(ArrayAllocationExpression node, BlockScope scope)
        {
       // do nothing by default
        }
     public void endVisit(ArrayInitializer  node, BlockScope scope)
        {
       // do nothing by default
        }
     public void endVisit(ArrayQualifiedTypeReference node, BlockScope scope)
        {
       // do nothing by default
        }
     public void endVisit(ArrayQualifiedTypeReference node, ClassScope scope)
        {
       // do nothing by default
        }
     public void endVisit(ArrayReference  node, BlockScope scope)
        {
       // do nothing by default
        }
     public void endVisit(ArrayTypeReference  node, BlockScope scope)
        {
       // do nothing by default
        }
     public void endVisit(ArrayTypeReference  node, ClassScope scope)
        {
       // do nothing by default
        }
     public void endVisit(AssertStatement  node, BlockScope scope)
        {
       // do nothing by default
        }
     public void endVisit(Assignment  node, BlockScope scope)
        {
       // do nothing by default
        }
     public void endVisit(BinaryExpression  node, BlockScope scope) {
       // do nothing by default
     }
     public void endVisit(Block  node, BlockScope scope) {
       // do nothing by default
     }
     public void endVisit(BreakStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(CaseStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(CastExpression  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(CharLiteral  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(ClassLiteralAccess  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(Clinit  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(CompilationUnitDeclaration node, CompilationUnitScope scope)
        {
       // do nothing by default
        }
     public void endVisit(CompoundAssignment  node, BlockScope scope)
        {
       // do nothing  by default
        }
     public void endVisit(ConditionalExpression node, BlockScope scope)
        {
       // do nothing  by default
        }

     public void endVisit(ConstructorDeclaration node, ClassScope scope)
        {
          java_parser.cactionConstructorDeclarationEnd();
        }

     public void endVisit(ContinueStatement  node, BlockScope scope)
        {
       // do nothing  by default
        }
     public void endVisit(DoStatement  node, BlockScope scope)
        {
       // do nothing  by default
        }
     public void endVisit(DoubleLiteral  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(EmptyStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(EqualExpression  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               ExplicitConstructorCall node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               ExtendedStringLiteral node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(FalseLiteral  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(FieldDeclaration  node, MethodScope scope) {
        // do nothing  by default
     }
     public void endVisit(FieldReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(FieldReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(FloatLiteral  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(ForeachStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(ForStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(IfStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(ImportReference  node, CompilationUnitScope scope) {
        // do nothing  by default
     }
     public void endVisit(Initializer  node, MethodScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               InstanceOfExpression node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(IntLiteral  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(Javadoc  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(Javadoc  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocAllocationExpression  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocAllocationExpression  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocArgumentExpression  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocArgumentExpression  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocArrayQualifiedTypeReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocArrayQualifiedTypeReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocArraySingleTypeReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocArraySingleTypeReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocFieldReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocFieldReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocImplicitTypeReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocImplicitTypeReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocMessageSend  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocMessageSend  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocQualifiedTypeReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocQualifiedTypeReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocReturnStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocReturnStatement  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocSingleNameReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocSingleNameReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocSingleTypeReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(JavadocSingleTypeReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(LabeledStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(LocalDeclaration  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(LongLiteral  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(MarkerAnnotation  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(MemberValuePair  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(MessageSend  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(MethodDeclaration  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(StringLiteralConcatenation  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(NormalAnnotation  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(NullLiteral  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(OR_OR_Expression  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(ParameterizedQualifiedTypeReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(ParameterizedQualifiedTypeReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(ParameterizedSingleTypeReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(ParameterizedSingleTypeReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(PostfixExpression  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(PrefixExpression  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               QualifiedAllocationExpression node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               QualifiedNameReference node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               QualifiedNameReference node,
               ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               QualifiedSuperReference node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               QualifiedSuperReference node,
               ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               QualifiedThisReference node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               QualifiedThisReference node,
               ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               QualifiedTypeReference node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               QualifiedTypeReference node,
               ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(ReturnStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(SingleMemberAnnotation  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               SingleNameReference node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               SingleNameReference node,
               ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               SingleTypeReference node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               SingleTypeReference node,
               ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(StringLiteral  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(SuperReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(SwitchStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               SynchronizedStatement node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(ThisReference  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(ThisReference  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(ThrowStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(TrueLiteral  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(TryStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               TypeDeclaration node,
               BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               TypeDeclaration node,
               ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(
               TypeDeclaration node,
               CompilationUnitScope scope) {
        // do nothing  by default
     }
     public void endVisit(TypeParameter  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(TypeParameter  node, ClassScope scope) {
        // do nothing  by default
     }
     public void endVisit(UnaryExpression  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(WhileStatement  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(Wildcard  node, BlockScope scope) {
        // do nothing  by default
     }
     public void endVisit(Wildcard  node, ClassScope scope)
        {
       // do nothing  by default
        }




   }

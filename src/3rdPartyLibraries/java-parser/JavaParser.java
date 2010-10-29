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


// DQ (10/12/2010): Make more like the OFP implementation (using Callable<Boolean> abstract base class). 
// class JavaTraversal {
import java.util.concurrent.Callable;
class JavaParser  implements Callable<Boolean>
   {
     static Main main;
     static BufferedWriter out;

  // -------------------------------------------------------------------------------------------
     public native void cactionCompilationUnitList(int argc, String[] argv);
     private native void cactionCompilationUnitDeclaration(String filename);
     private native void cactionTypeDeclaration(String filename);

     private native void cactionConstructorDeclaration(String filename);
     private native void cactionExplicitConstructorCall(String filename);
     private native void cactionMethodDeclaration(String filename);
     private native void cactionSingleTypeReference(String filename);
     private native void cactionArgument(String filename);
     private native void cactionArrayTypeReference(String filename);
     private native void cactionMessageSend(String filename);
     private native void cactionQualifiedNameReference(String filename);
     private native void cactionStringLiteral(String filename);


  // DQ (10/12/2010): Added boolean value to report error to C++ calling program (similar to OFP).
     private static boolean hasErrorOccurred = false;

     static { System.loadLibrary("JavaTraversal"); }

    // -------------------------------------------------------------------------------------------
  /* tps: Stack that is keeping track of the traversal we perform to connect children with parents in the DOT graph */
     private Stack<ASTNode> stack = new Stack<ASTNode>();
     public void pushNode(ASTNode node)
        {
          stack.push(node);
        }

     public ASTNode popNode()
        {
          if (!stack.empty())
               stack.pop();
            else
             {
               System.err.println("!!!!!!!!!!!! ERROR trying to access empty stack");
               System.exit(1);
             }
          if (!stack.empty())
               return (ASTNode)stack.peek();
          return null;
        }

     public void traverseAST(CompilationUnitDeclaration unit)
        {
          final ASTVisitor visitor = new ASTVisitor()
             {
               public boolean visit(AllocationExpression node,BlockScope scope)
                  {
                    pushNode(node); return true; // do nothing by  node, keep traversing
                  }

               public boolean visit(AND_AND_Expression  node, BlockScope scope)
                  {
                    pushNode(node);
                    return true; // do nothing by  node, keep traversing
                  }

               public boolean visit(AnnotationMethodDeclaration node,ClassScope classScope)
                  {
                    pushNode(node);
                 // node.traverse(this,node.scope);
                    return true; // do nothing by default, keep traversing
                  }

               public boolean visit(Argument  node, BlockScope scope)
                  {
                    cactionArgument("abc");
                    pushNode(node);
                    return true; // do nothing by  node, keep traversing
                  }

               public boolean visit(Argument  node, ClassScope scope)
                  {
                    pushNode(node);
                    return true; // do nothing by  node, keep traversing
                  }

               public boolean visit(ArrayAllocationExpression node,BlockScope scope)
                  {
                    pushNode(node);
                    return true; // do nothing by  node, keep traversing
		            }
		public boolean visit(ArrayInitializer  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ArrayQualifiedTypeReference node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ArrayQualifiedTypeReference node,
				     ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ArrayReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}

          public boolean visit(ArrayTypeReference  node, BlockScope scope)
             {
               cactionArrayTypeReference("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(ArrayTypeReference  node, ClassScope scope)
             {
               cactionArrayTypeReference("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

		public boolean visit(AssertStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Assignment  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(BinaryExpression  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Block  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(BreakStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(CaseStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(CastExpression  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(CharLiteral  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ClassLiteralAccess  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Clinit  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     CompilationUnitDeclaration node,
				     CompilationUnitScope scope) {

          System.out.println("Inside of visit (CompilationUnitDeclaration,CompilationUnitScope)");

       // Call the Java side of the JNI function.
          String s = new String(node.getFileName());
          System.out.println("Test A");
          cactionCompilationUnitDeclaration(s);
          System.out.println("Leaving visit (CompilationUnitDeclaration,CompilationUnitScope)");

		    pushNode(node);
		    return true; // do nothing by default, keep traversing
		}
		public boolean visit(CompoundAssignment  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ConditionalExpression node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}

          public boolean visit(ConstructorDeclaration node, ClassScope scope)
             {
               cactionConstructorDeclaration("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

		public boolean visit(ContinueStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(DoStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(DoubleLiteral  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(EmptyStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(EqualExpression  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}

          public boolean visit(ExplicitConstructorCall node, BlockScope scope)
             {
               cactionExplicitConstructorCall("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

		public boolean visit(
				     ExtendedStringLiteral node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FalseLiteral  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FieldDeclaration  node, MethodScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FieldReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FieldReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FloatLiteral  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ForeachStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ForStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(IfStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ImportReference  node, CompilationUnitScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Initializer  node, MethodScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     InstanceOfExpression node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(IntLiteral  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Javadoc  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Javadoc  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocAllocationExpression  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocAllocationExpression  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArgumentExpression  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArgumentExpression  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArrayQualifiedTypeReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArrayQualifiedTypeReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArraySingleTypeReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArraySingleTypeReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocFieldReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocFieldReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocImplicitTypeReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocImplicitTypeReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}

          public boolean visit(JavadocMessageSend  node, BlockScope scope)
             {
               pushNode(node); return true; // do nothing by  node, keep traversing
             }

		public boolean visit(JavadocMessageSend  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocQualifiedTypeReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocQualifiedTypeReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocReturnStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocReturnStatement  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocSingleNameReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocSingleNameReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocSingleTypeReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocSingleTypeReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(LabeledStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(LocalDeclaration  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(LongLiteral  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public boolean visit(MarkerAnnotation  node, BlockScope scope) {
		    pushNode(node); return true;
		}
		/**
		 * @param pair
		 * @param scope
		 * @shashcountere 3.1
		 */
		public boolean visit(MemberValuePair  node, BlockScope scope) {
		    pushNode(node); return true;
		}

          public boolean visit(MessageSend  node, BlockScope scope)
             {
               cactionMessageSend("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(MethodDeclaration  node, ClassScope scope)
             {
               cactionMethodDeclaration("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

		public boolean visit(
				     StringLiteralConcatenation node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public boolean visit(NormalAnnotation  node, BlockScope scope) {
		    pushNode(node); return true;
		}
		public boolean visit(NullLiteral  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(OR_OR_Expression  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ParameterizedQualifiedTypeReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ParameterizedQualifiedTypeReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ParameterizedSingleTypeReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ParameterizedSingleTypeReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(PostfixExpression  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(PrefixExpression  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedAllocationExpression node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}

          public boolean visit(QualifiedNameReference node, BlockScope scope)
             {
               cactionQualifiedNameReference("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(QualifiedNameReference node, ClassScope scope)
             {
               cactionQualifiedNameReference("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

		public boolean visit(
				     QualifiedSuperReference node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedSuperReference node,
				     ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedThisReference node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedThisReference node,
				     ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedTypeReference node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedTypeReference node,
				     ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ReturnStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public boolean visit(SingleMemberAnnotation  node, BlockScope scope) {
		    pushNode(node); return true;
		}
		public boolean visit(
				     SingleNameReference node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     SingleNameReference node,
				     ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}

          public boolean visit(SingleTypeReference node, BlockScope scope)
             {
               cactionSingleTypeReference("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(SingleTypeReference node, ClassScope scope)
             {
               cactionSingleTypeReference("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(StringLiteral  node, BlockScope scope)
             {
               cactionStringLiteral("abc");
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

		public boolean visit(SuperReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(SwitchStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     SynchronizedStatement node,
				     BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ThisReference  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ThisReference  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ThrowStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(TrueLiteral  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(TryStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}

          public boolean visit(TypeDeclaration node,BlockScope scope)
             {
               System.out.println("visit TypeDeclaration -- BlockScope");
               String typename = new String(node.name);
               cactionTypeDeclaration(typename);
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(TypeDeclaration node,ClassScope scope)
             {
               System.out.println("visit TypeDeclaration -- ClassScope");
               String typename = new String(node.name);
               cactionTypeDeclaration(typename);
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(TypeDeclaration node,CompilationUnitScope scope)
             {
               System.out.println("visit TypeDeclaration -- CompilationUnitScope");
               String typename = new String(node.name);
               cactionTypeDeclaration(typename);
               pushNode(node);
               return true; // do nothing by  node, keep traversing
             }

		public boolean visit(TypeParameter  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(TypeParameter  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(UnaryExpression  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(WhileStatement  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Wildcard  node, BlockScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Wildcard  node, ClassScope scope) {
		    pushNode(node); return true; // do nothing by  node, keep traversing
		}



		public void endVisit(
				     AllocationExpression node,
				     BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(AND_AND_Expression  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(
				     AnnotationMethodDeclaration node,
				     ClassScope classScope) {
		    popNode();			// do nothing by default
		}
		public void endVisit(Argument  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(Argument  node,ClassScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(
				     ArrayAllocationExpression node,
				     BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(ArrayInitializer  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(
				     ArrayQualifiedTypeReference node,
				     BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(
				     ArrayQualifiedTypeReference node,
				     ClassScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(ArrayReference  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(ArrayTypeReference  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(ArrayTypeReference  node, ClassScope scope) {
		    popNode();		    // do nothing by default
		}
		public void endVisit(AssertStatement  node, BlockScope scope) {
		    popNode();// do nothing by default
		}
		public void endVisit(Assignment  node, BlockScope scope) {
		    popNode();// do nothing by default
		}
		public void endVisit(BinaryExpression  node, BlockScope scope) {
		    popNode();// do nothing by default
		}
		public void endVisit(Block  node, BlockScope scope) {
		    popNode();// do nothing by default
		}
		public void endVisit(BreakStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(CaseStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(CastExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(CharLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ClassLiteralAccess  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Clinit  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     CompilationUnitDeclaration node,
				     CompilationUnitScope scope) {
		    popNode();
		    // do nothing by default
		}
		public void endVisit(CompoundAssignment  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     ConditionalExpression node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     ConstructorDeclaration node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ContinueStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(DoStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(DoubleLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(EmptyStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(EqualExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     ExplicitConstructorCall node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     ExtendedStringLiteral node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FalseLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FieldDeclaration  node, MethodScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FieldReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FieldReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FloatLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ForeachStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ForStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(IfStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ImportReference  node, CompilationUnitScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Initializer  node, MethodScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     InstanceOfExpression node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(IntLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Javadoc  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Javadoc  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocAllocationExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocAllocationExpression  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArgumentExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArgumentExpression  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArrayQualifiedTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArrayQualifiedTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArraySingleTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArraySingleTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocFieldReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocFieldReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocImplicitTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocImplicitTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocMessageSend  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocMessageSend  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocQualifiedTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocQualifiedTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocReturnStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocReturnStatement  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocSingleNameReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocSingleNameReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocSingleTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocSingleTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(LabeledStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(LocalDeclaration  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(LongLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public void endVisit(MarkerAnnotation  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		/**
		 * @param pair
		 * @param scope
		 */
		public void endVisit(MemberValuePair  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(MessageSend  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(MethodDeclaration  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(StringLiteralConcatenation  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public void endVisit(NormalAnnotation  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(NullLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(OR_OR_Expression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ParameterizedQualifiedTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ParameterizedQualifiedTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ParameterizedSingleTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ParameterizedSingleTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(PostfixExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(PrefixExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedAllocationExpression node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedNameReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedNameReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedSuperReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedSuperReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedThisReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedThisReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedTypeReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedTypeReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ReturnStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public void endVisit(SingleMemberAnnotation  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SingleNameReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SingleNameReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SingleTypeReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SingleTypeReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(StringLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(SuperReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(SwitchStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SynchronizedStatement node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ThisReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ThisReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ThrowStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(TrueLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(TryStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     TypeDeclaration node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     TypeDeclaration node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     TypeDeclaration node,
				     CompilationUnitScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(TypeParameter  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(TypeParameter  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(UnaryExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(WhileStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Wildcard  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Wildcard  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}



             };
          unit.traverse(visitor,unit.scope);
        }

     public void startParsingAST(CompilationUnitDeclaration unit)
        {
          System.out.println("Start parsing");
          traverseAST(unit);
          System.out.println("Done parsing");
        }


 // DQ (10/12/2010): Implemented abstract baseclass "call()" member function (similar to OFP).
     public Boolean call() throws Exception
        {
       // boolean error = false;
          boolean error   = true;
          boolean verbose = true;

          if (error != false)
             {
               System.out.println("Parser failed");
             } 
            else
             {
               if (verbose)
                    System.out.println("Parser exiting normally");
             }// end else(parser exited normally)

          return new Boolean(error);
        } // end call()

 // DQ (10/12/2010): Added boolean value to report error to C++ calling program (similar to OFP).
     public static boolean getError()
        {
          return hasErrorOccurred;
        }
}

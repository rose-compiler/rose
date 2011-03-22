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
     final JavaParser java_parser;

     ecjASTVisitor (JavaParser x)
        {
          java_parser = x;
        }

  // visitor = new ASTVisitor()
     public boolean visit(AllocationExpression node,BlockScope scope)
        {
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(AND_AND_Expression  node, BlockScope scope)
        {
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(AnnotationMethodDeclaration node,ClassScope classScope)
        {
          return true; // do nothing by default, keep traversing
        }

     public boolean visit(Argument  node, BlockScope scope)
        {
          java_parser.cactionArgument("abc");
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Argument  node, ClassScope scope)
        {
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayAllocationExpression node,BlockScope scope)
        {
          return true; // do nothing by  node, keep traversing
		  }

     public boolean visit(ArrayInitializer  node, BlockScope scope)
        {
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayQualifiedTypeReference node, BlockScope scope)
        {
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayQualifiedTypeReference node, ClassScope scope)
        {
          return true; // do nothing by  node, keep traversing
        }
     public boolean visit(ArrayReference  node, BlockScope scope)
        {
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayTypeReference  node, BlockScope scope)
        {
          java_parser.cactionArrayTypeReference("abc");
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayTypeReference  node, ClassScope scope)
        {
          java_parser.cactionArrayTypeReference("abc");
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(AssertStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(Assignment  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(BinaryExpression  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(Block  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(BreakStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(CaseStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(CastExpression  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(CharLiteral  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ClassLiteralAccess  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(Clinit  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               CompilationUnitDeclaration node,
               CompilationUnitScope scope) {

          System.out.println("Inside of visit (CompilationUnitDeclaration,CompilationUnitScope)");

       // Call the Java side of the JNI function.
          String s = new String(node.getFileName());
          System.out.println("Test A");
          java_parser.cactionCompilationUnitDeclaration(s);
          System.out.println("Leaving visit (CompilationUnitDeclaration,CompilationUnitScope)");

         return true; // do nothing by default, keep traversing
     }
     public boolean visit(CompoundAssignment  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               ConditionalExpression node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }

          public boolean visit(ConstructorDeclaration node, ClassScope scope)
             {
               java_parser.cactionConstructorDeclaration("abc");
               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(ContinueStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(DoStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(DoubleLiteral  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(EmptyStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(EqualExpression  node, BlockScope scope) {
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

     public boolean visit(
               ExtendedStringLiteral node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(FalseLiteral  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(FieldDeclaration  node, MethodScope scope) {
         return true; // do nothing by  node, keep traversing
     }

          public boolean visit(FieldReference  node, BlockScope scope) 
             {
               System.out.println("Sorry, not implemented in support for FieldReference(BlockScope): xxx");
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(FieldReference  node, ClassScope scope)
             {
               System.out.println("Sorry, not implemented in support for FieldReference(ClassScope): xxx");
               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(FloatLiteral  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ForeachStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ForStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(IfStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ImportReference  node, CompilationUnitScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(Initializer  node, MethodScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               InstanceOfExpression node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(IntLiteral  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(Javadoc  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(Javadoc  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocAllocationExpression  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocAllocationExpression  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocArgumentExpression  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocArgumentExpression  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocArrayQualifiedTypeReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocArrayQualifiedTypeReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocArraySingleTypeReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocArraySingleTypeReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocFieldReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocFieldReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocImplicitTypeReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocImplicitTypeReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }

          public boolean visit(JavadocMessageSend  node, BlockScope scope)
             {
               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(JavadocMessageSend  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocQualifiedTypeReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocQualifiedTypeReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocReturnStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocReturnStatement  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocSingleNameReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocSingleNameReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocSingleTypeReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(JavadocSingleTypeReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(LabeledStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(LocalDeclaration  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(LongLiteral  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(MarkerAnnotation  node, BlockScope scope) {
         return true;
     }
     public boolean visit(MemberValuePair  node, BlockScope scope) {
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
               java_parser.cactionMethodDeclaration("abc");
               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(
               StringLiteralConcatenation node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(NormalAnnotation  node, BlockScope scope) {
         return true;
     }
     public boolean visit(NullLiteral  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(OR_OR_Expression  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ParameterizedQualifiedTypeReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ParameterizedQualifiedTypeReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ParameterizedSingleTypeReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ParameterizedSingleTypeReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(PostfixExpression  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(PrefixExpression  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               QualifiedAllocationExpression node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }

          public boolean visit(QualifiedNameReference node, BlockScope scope)
             {
            // Note that if we want to traverse this object in more detail then the QualifiedNameReference::traverse() functions can/should be modified.
               java_parser.cactionQualifiedNameReference("abc");

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

               java_parser.cactionQualifiedNameReference("abc");

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

     public boolean visit(
               QualifiedSuperReference node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               QualifiedSuperReference node,
               ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               QualifiedThisReference node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               QualifiedThisReference node,
               ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               QualifiedTypeReference node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               QualifiedTypeReference node,
               ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ReturnStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(SingleMemberAnnotation  node, BlockScope scope) {
         return true;
     }
     public boolean visit(
               SingleNameReference node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               SingleNameReference node,
               ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }

          public boolean visit(SingleTypeReference node, BlockScope scope)
             {
            // java_parser.cactionSingleTypeReference("abc");

               if (node.resolvedType != null)
                  {
                    java_parser.cactionSingleTypeReference("abc");
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
               java_parser.cactionSingleTypeReference("abc");
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(StringLiteral  node, BlockScope scope)
             {
               java_parser.cactionStringLiteral("abc");
               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(SuperReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(SwitchStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(
               SynchronizedStatement node,
               BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ThisReference  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ThisReference  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(ThrowStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(TrueLiteral  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(TryStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }

          public boolean visit(TypeDeclaration node,BlockScope scope)
             {
               System.out.println("visit TypeDeclaration -- BlockScope");
               String typename = new String(node.name);
               java_parser.cactionTypeDeclaration(typename);
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(TypeDeclaration node,ClassScope scope)
             {
               System.out.println("visit TypeDeclaration -- ClassScope");
               String typename = new String(node.name);
               java_parser.cactionTypeDeclaration(typename);
               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(TypeDeclaration node,CompilationUnitScope scope)
             {
               System.out.println("visit TypeDeclaration -- CompilationUnitScope");
               String typename = new String(node.name);
               java_parser.cactionTypeDeclaration(typename);
               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(TypeParameter  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(TypeParameter  node, ClassScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(UnaryExpression  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(WhileStatement  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(Wildcard  node, BlockScope scope) {
         return true; // do nothing by  node, keep traversing
     }
     public boolean visit(Wildcard  node, ClassScope scope) {
          return true; // do nothing by  node, keep traversing
     }



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

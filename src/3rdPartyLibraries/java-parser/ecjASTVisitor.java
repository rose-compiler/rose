import org.eclipse.jdt.internal.compiler.batch.*;

// Test if this works in Java.
// import java; // fails
// import java.io; // fails
// import java.lang; // fails
// import java.lang.*; // works
// import java.*; // works

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

// DQ (8/13/2011): Used to support modifier handling.
import org.eclipse.jdt.internal.compiler.classfmt.ClassFileConstants;


class ecjASTVisitor extends ASTVisitor
   {
  // This class contains the visitor functions required to support a traversal over the high-level ECJ AST and 
  // call the JNI functions that will execute as C++ functions and construct the ROSE AST.

     final JavaParser java_parser;

     private final JavaSourcePositionInformationFactory posFactory;

  // This is the older version oc the constructor, before Vincent's work
  // to support the source code position.
  /* ecjASTVisitor (JavaParser x)
        {
          java_parser = x;
        }
  */

  // *************************************************
  // Support for source code position (from Vincent).
  // *************************************************
     public ecjASTVisitor(JavaParser javaParser, CompilationUnitDeclaration unit) {
        this.java_parser = javaParser;
        this.posFactory = new JavaSourcePositionInformationFactory(unit);
     }
     
     public JavaToken createJavaToken(ASTNode node) {
    	 JavaSourcePositionInformation pos = getPosInfoFactory().createPosInfo(node);
    	 // For now we return dummy text
    	 return new JavaToken("Dummy JavaToken (see createJavaToken)", pos);
     }

     public JavaToken createJavaToken(AbstractMethodDeclaration node) {
       // System.out.println("Create JAVA TOKEN FOR METHOD BODY"); 
    	 JavaSourcePositionInformation pos = getPosInfoFactory().createPosInfo(node);
    	 // For now we return dummy text
    	 return new JavaToken("Dummy JavaToken (see createJavaToken)", pos);
     }

     protected JavaSourcePositionInformationFactory getPosInfoFactory() {
         return this.posFactory;
     }

  // *************************************************

  // visitor = new ASTVisitor()
     public boolean visit(AllocationExpression node,BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (AllocationExpression,BlockScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionAllocationExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (AllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(AND_AND_Expression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (AND_AND_Expression,BlockScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionANDANDExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (AND_AND_Expression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(AnnotationMethodDeclaration node,ClassScope classScope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (AnnotationMethodDeclaration,ClassScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionAnnotationMethodDeclaration(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (AnnotationMethodDeclaration,ClassScope)");

          return true; // do nothing by default, keep traversing
        }

     public boolean visit(Argument  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Argument,BlockScope)");

       // Printing out the char[] type directly does not work, convert it to a string first and output the string.
       // System.out.println(" name      = " + node.name);
          String nameString = new String(node.name);
          if (java_parser.verboseLevel > 0)
             {
               System.out.println(" name      = " + nameString);
               System.out.println(" type      = " + node.type);
               System.out.println(" modifiers = " + node.modifiers);
             }

       // *******************************************************************************************
       // Build the lists of type parameters and function parameters as part of building the function 
       // declarations directly. So not there is no action for this function.

       // Either that or we have to reconcile implicit processing using reflection and explicit 
       // processing driven by the ECJ specific AST traversal ???
       // *******************************************************************************************

       // Divide up the work to define an argument, the name and modifiers are simple but the type has to
       // be constructed via recursive calls that will cause it to be generated on the astJavaTypeStack.
       // java_parser.cactionArgument("Argument_block_abc");
       // java_parser.cactionArgumentName(nameString);
       // JavaParserSupport.generateType(node.type);
       // java_parser.cactionArgumentModifiers(node.modifiers);
       // java_parser.cactionArgumentEnd();

       // JavaParserSupport.generateType(node.type);

       // This rule assumes that the type will be made available on the stack (astJavaTypeStack).
       // In general we want to have rules that are specific to IR nodes and pass any constructed
       // IR nodes via the stack (rules called should have generated the constructed IR nodes on 
       // the stack within ROSE).
       // java_parser.cactionArgument(nameString,node.modifiers);

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Argument,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Argument  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Argument,ClassScope)");

          java_parser.cactionArgumentClassScope("Argument_class_abc", this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Argument,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayAllocationExpression node,BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ArrayAllocationExpression,BlockScope)");

          java_parser.cactionArrayAllocationExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ArrayAllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
		  }

     public boolean visit(ArrayInitializer  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ArrayInitializer,BlockScope)");

          java_parser.cactionArrayInitializer(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ArrayInitializer,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayQualifiedTypeReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ArrayQualifiedTypeReference,BlockScope)");

          java_parser.cactionArrayQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ArrayQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayQualifiedTypeReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ArrayQualifiedTypeReference,ClassScope)");

          java_parser.cactionArrayQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ArrayQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }
     public boolean visit(ArrayReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ArrayReference,BlockScope)");

          java_parser.cactionArrayReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ArrayReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayTypeReference  node, BlockScope scope)
        {
          java_parser.cactionArrayTypeReference("ArrayTypeReference_block_abc", this.createJavaToken(node));
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ArrayTypeReference  node, ClassScope scope)
        {
          java_parser.cactionArrayTypeReferenceClassScope("ArrayTypeReference_class_abc", this.createJavaToken(node));
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(AssertStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (AssertStatement,BlockScope)");

          java_parser.cactionAssertStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (AssertStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Assignment  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Assignment,BlockScope)");

          java_parser.cactionAssignment(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Assignment,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(BinaryExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (BinaryExpression,BlockScope)");

          java_parser.cactionBinaryExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (BinaryExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Block  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Block,BlockScope)");

          java_parser.cactionBlock(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Block,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(BreakStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (BreakStatement,BlockScope)");

          java_parser.cactionBreakStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (BreakStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(CaseStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (CaseStatement,BlockScope)");

          java_parser.cactionCaseStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (CaseStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(CastExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (CastExpression,BlockScope)");

          java_parser.cactionCastExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (CastExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(CharLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (CharLiteral,BlockScope)");

          java_parser.cactionCharLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (CharLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ClassLiteralAccess  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ClassLiteralAccess,BlockScope)");

          java_parser.cactionClassLiteralAccess(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ClassLiteralAccess,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Clinit  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Clinit,ClassScope)");

          java_parser.cactionClinit(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Clinit,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( CompilationUnitDeclaration node, CompilationUnitScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (CompilationUnitDeclaration,CompilationUnitScope)");

       // Except for out here for debugging, the filename string is not used (even on the C++ side of the JNI function.
          String s = new String(node.getFileName());
          if (java_parser.verboseLevel > 0)
             {
               System.out.println("Compiling file = " + s);
             }

       // Ouput some information about the CompilationUnitScope (we don't use the package name currently).
          String packageReference = "";
          for (int i = 0, tokenArrayLength = scope.currentPackageName.length; i < tokenArrayLength; i++)
             {
               String tokenString = new String(scope.currentPackageName[i]);
               System.out.println("     --- packageReference tokens = " + tokenString);

               if (i > 0)
                    packageReference += '.';

               packageReference += tokenString;
             }

          if (java_parser.verboseLevel > 0)
               System.out.println("Package name = " + packageReference);

       // Call the Java side of the JNI function.
       // This function only does a few tests on the C++ side to make sure that it is ready to construct the ROSE AST.
          java_parser.cactionCompilationUnitDeclaration(s, this.createJavaToken(node));

       // Build the default implicit classes that will be required to process Java functions.
       // The symbol for these classes need to be added to the global scope and the then
       // we need to add the required name qualification support for data member and member 
       // function lookup.
          if (java_parser.verboseLevel > 1)
               System.out.println("Calling buildImplicitClassSupport for java.lang.System");

       // We now read in classes on demand as they are references, if there are references.  All classes are read as 
       // required to resolved all types in the program and the implicitly read classes.  Interestingly, there
       // are nearly 3000 classes in the 135 packages in the J2SE 1.4.2 standard class library (over 3000 classes 
       // in 165 packages in J2SE 5.0). Commercial class libraries that you might use add many more packages. 
       // This triggers the building of a recursively identified set of classes required to define all types in the problem.
       // JavaParserSupport.buildImplicitClassSupport("java.lang.System");

          if (java_parser.verboseLevel > 1)
               System.out.println("DONE: Calling buildImplicitClassSupport for java.lang.System");

       // System.out.println("Exiting as a test in visit (CompilationUnitDeclaration,CompilationUnitScope)...");
       // System.exit(1);

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (CompilationUnitDeclaration,CompilationUnitScope)");

          return true; // do nothing by default, keep traversing
        }

     public boolean visit(CompoundAssignment  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (CompoundAssignment,BlockScope)");

          java_parser.cactionCompoundAssignment(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (CompoundAssignment,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ConditionalExpression node,BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ConditionalExpression,BlockScope)");

          java_parser.cactionConditionalExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ConditionalExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ConstructorDeclaration node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ConstructorDeclaration,ClassScope)");

       // char [] name = node.selector;
       // System.out.println("Inside of visit (ConstructorDeclaration,ClassScope) method name = " + node.selector);
          String name = new String(node.selector);
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ConstructorDeclaration,ClassScope) method name = " + name);

       // argument types
          if (node.typeParameters != null)
             {
               for (int i = 0, typeArgumentsLength = node.typeParameters.length; i < typeArgumentsLength; i++)
                  {
                    System.out.println("     --- constructor typeParameters = " + node.typeParameters[i]);
                  }
             }
            else
             {
            // For a function defined in the input program, the typeParameters array is empty, but the ECJ
            // specific AST traversal will visit the type parameters. Not clear why this is organized like this.
            // System.out.println("     --- method typeParameters (empty) = " + node.typeParameters);
             }

       // Looking here for arguments (want the arguments) since they are not always in the node.typeParameters
          if (node.arguments != null)
             {
               for (int i = 0, typeArgumentsLength = node.arguments.length; i < typeArgumentsLength; i++)
                  {
                 // System.out.println("     --- constructor arguments = " + node.arguments[i].type);
                 // System.out.println("     --- constructor arguments (type = " + node.arguments[i].type + ", name = " + node.arguments[i].name);
                    if (java_parser.verboseLevel > 0)
                         System.out.println("     --- constructor arguments (type = " + node.arguments[i].type + ", name = " + node.arguments[i].name.toString() + ")");

                    String nameString = new String(node.arguments[i].name);

                 // System.out.println("This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (visit (ConstructorDeclaration,ClassScope)): type = " + node.arguments[i].type + " nameString = " + nameString);

                    JavaParserSupport.generateType(node.arguments[i].type);

                 // This rule assumes that the type will be made available on the stack (astJavaTypeStack).
                 // In general we want to have rules that are specific to IR nodes and pass any constructed
                 // IR nodes via the stack (rules called should have generated the constructed IR nodes on 
                 // the stack within ROSE).
                    java_parser.cactionArgument(nameString,node.modifiers, this.createJavaToken(node));

                 // System.out.println("DONE: This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (visit (ConstructorDeclaration,ClassScope)): type = " + node.arguments[i].type + " nameString = " + nameString);
                  }
             }
            else
             {
            // For a function defined in the input program, the typeParameters array is empty, but the ECJ
            // specific AST traversal will visit the type parameters. Not clear why this is organized like this.
               if (java_parser.verboseLevel > 0)
                    System.out.println("     --- method arguments (empty) = " + node.arguments);
             }

          if (java_parser.verboseLevel > 2)
               System.out.println("Push void as a return type for now (will be ignored because this is a constructor)");

       // Push a type to serve as the return type which will be ignored for the case of a constructor
       // (this allows us to reuse the general member function support).
          JavaParser.cactionGenerateType("void");

          if (java_parser.verboseLevel > 2)
               System.out.println("DONE: Push void as a return type for now (will be ignored because this is a constructor)");

          boolean isNative = node.isNative();
          boolean isPrivate = (node.binding != null && !node.binding.isPrivate()) ? true : false;

          boolean isStatic = node.isNative();

          java_parser.cactionConstructorDeclaration(name,isNative, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ConstructorDeclaration,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ContinueStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ContinueStatement,BlockScope)");

          java_parser.cactionContinueStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ContinueStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(DoStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (DoStatement,BlockScope)");

          java_parser.cactionDoStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (DoStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(DoubleLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (DoubleLiteral,BlockScope)");

          java_parser.cactionDoubleLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (DoubleLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(EmptyStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (EmptyStatement,BlockScope)");

          java_parser.cactionEmptyStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (EmptyStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(EqualExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (EqualExpression,BlockScope)");

          java_parser.cactionEqualExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (EqualExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ExplicitConstructorCall node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ExplicitConstructorCall,BlockScope)");

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

          if (java_parser.verboseLevel > 0)
               System.out.println("In visit (ExplicitConstructorCall,BlockScope): node.accessMode = " + node.accessMode);

          if (node.accessMode == ExplicitConstructorCall.This)
             {
            // output.append("this("); //$NON-NLS-1$
               System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: this");
             }
            else
             {
               if (node.accessMode == ExplicitConstructorCall.ImplicitSuper)
                  {
                 // System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: implicit super()");
                 // java_parser.cactionExplicitConstructorCall("ImplicitSuper");

                 // We have to implement the support for an impleicit super() call, even if it is marked explicitly 
                 // as implicit and not generated in the output source code.  It should still be explicit in the AST
                 // as a way to simplify analysis.
                    String name = new String("super");
                 // System.out.println("super function name = " + name);

                    String associatedClassName = node.binding.toString();
                 // System.out.println("super function associatedClassName = " + associatedClassName);

                    java_parser.cactionMessageSend(name,associatedClassName, this.createJavaToken(node));
                  }
                 else
                  {
                    if (node.accessMode == ExplicitConstructorCall.Super)
                       {
                         System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: super()");
                      // java_parser.cactionExplicitConstructorCall("super");

                         String name = new String("super");

                         if (java_parser.verboseLevel > 0)
                            {
                              System.out.println("super function name = " + name);

                              System.out.println("super function node = " + node);
                              System.out.println("super function node.isSuperAccess() = " + node.isSuperAccess());
                              System.out.println("super function node.isTypeAccess()  = " + node.isTypeAccess());
                              System.out.println("super function node.binding = " + node.binding);
                              System.out.println("super function node.binding = xxx" + node.binding.toString() + "xxx");
                              if (node.binding.toString() == "public void <init>() ")
                                 {
                                   System.out.println("super() in class without base class ...");
                                 }
                                else
                                 {
                                   System.out.println("Proper super() call to class with base class ...");
                                 }
                            }

                      // System.out.println("super function node.binding.declaringClass = " + node.binding.declaringClass);
                      // System.out.println("super function node.binding.declaringClass.toString() = " + node.binding.declaringClass.toString());

                      // String associatedClassName = node.binding.toString();
                      // String associatedClassName = node.binding.declaringClass.toString();
                         String associatedClassName = node.binding.toString();

                         if (java_parser.verboseLevel > 0)
                            {
                              System.out.println("super function associatedClassName = " + associatedClassName);
                              System.out.println("super function cactionMessageSend() associatedClassName = " + associatedClassName);
                            }

                         java_parser.cactionMessageSend(name,associatedClassName, this.createJavaToken(node));

                      // System.out.println("DONE: super function cactionMessageSend() associatedClassName = " + associatedClassName);
                       }
                      else
                       {
                         System.out.println("This is neither This, ImplicitSuper, or Super ...");
                         System.exit(1);
                       }
                  }
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

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ExplicitConstructorCall,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ExtendedStringLiteral node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ExtendedStringLiteral,BlockScope)");

          java_parser.cactionExtendedStringLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ExtendedStringLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(FalseLiteral  node, BlockScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (FalseLiteral,BlockScope)");

          java_parser.cactionFalseLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (FalseLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(FieldDeclaration node, MethodScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (FieldDeclaration,BlockScope)");

       // I think that it is enough that this is set via the LocalDeclaration.
       // boolean isFinal = node.binding.isFinal();

          boolean isPrivate   = (node.binding != null && node.binding.isPrivate())   ? true : false;
          boolean isProtected = (node.binding != null && node.binding.isProtected()) ? true : false;
          boolean isPublic    = (node.binding != null && node.binding.isPublic())    ? true : false;

          boolean isVolatile  = (node.binding != null && node.binding.isVolatile())  ? true : false;
          boolean isSynthetic = (node.binding != null && node.binding.isSynthetic()) ? true : false;
          boolean isStatic    = (node.binding != null && node.binding.isStatic())    ? true : false;
          boolean isTransient = (node.binding != null && node.binding.isTransient()) ? true : false;

          boolean hasInitializer = (node.initialization != null) ? true : false;

          String name = new String(node.name);

       // String selectorName = new String(node.selector);
       // System.out.println("node.name = " + selectorName);
       // System.out.println("node.modfiers = " + node.modfiers);

          if (java_parser.verboseLevel > 0)
             {
               System.out.println("node.name                     = " + name);
               System.out.println("node.binding                  = " + node.binding);
               System.out.println("node.binding.type             = " + node.binding.type);
               System.out.println("node.binding.type.id          = " + node.binding.type.id);
               System.out.println("node.binding.type.debugName() = " + node.binding.type.debugName());
               System.out.println("node.type                     = " + node.type);

               System.out.println("isPrivate                     = " + isPrivate);
               System.out.println("isProtected                   = " + isProtected);
               System.out.println("isPublic                      = " + isPublic);

               System.out.println("hasInitializer                = " + hasInitializer);
             }

       // Construct the type (will be constructed on the astJavaTypeStack.

       // DQ (7/18/2011): Switch to using the different generateType() function (taking a TypeReference).
       // JavaParserSupport.generateType(node.binding.type);
          JavaParserSupport.generateType(node.type);

          boolean isFinal = node.binding.isFinal();

       // DQ (8/13/2011): This information is stored in the FieldReference...(not clear how to get it).
       // boolean isPrivate = (node.binding != null && !node.binding.isPrivate()) ? true : false;

       // Build the variable declaration using the type from the astJavaTypeStack.
       // Note that this may have to handle an array of names or be even more complex in the future.
       // java_parser.cactionLocalDeclaration(name,isFinal);

          java_parser.cactionFieldDeclaration(name,hasInitializer,isFinal,isPrivate,isProtected,isPublic,isVolatile,isSynthetic,isStatic,isTransient, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (FieldDeclaration,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

          public boolean visit(FieldReference node, BlockScope scope) 
             {
               System.out.println("Sorry, not implemented in support for FieldReference(BlockScope): xxx");
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (FieldReference,BlockScope)");

          java_parser.cactionFieldReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (FieldReference,BlockScope)");

               return true; // do nothing by  node, keep traversing
             }

          public boolean visit(FieldReference  node, ClassScope scope)
             {
               System.out.println("Sorry, not implemented in support for FieldReference(ClassScope): xxx");
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (FieldReference,ClassScope)");

          java_parser.cactionFieldReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (FieldReference,ClassScope)");

               return true; // do nothing by  node, keep traversing
             }

     public boolean visit(FloatLiteral  node, BlockScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (FloatLiteral,BlockScope)");

          java_parser.cactionFloatLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (FloatLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ForeachStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ForeachStatement,BlockScope)");

          java_parser.cactionForeachStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ForeachStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ForStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ForStatement,BlockScope)");

          java_parser.cactionForStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ForStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(IfStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (IfStatement,BlockScope)");

          java_parser.cactionIfStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (IfStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ImportReference  node, CompilationUnitScope scope)
        {
       // This is the support for the Java "import" statement (declaration).
       // We can basically strify this step, since this is frontend handling that
       // has no analysis requirements (that I know of).  We could translate
       // the list of class names to a list of declarations, this might be useful
       // at a later point. However, this would require post processing since
       // we have not yet read in the classes that would be a part of those
       // implicitly read as a consequence of the "import" statements.

          if (java_parser.verboseLevel > -1)
               System.out.println("Inside of visit (ImportReference,CompilationUnitScope)");

          String importReference = "";
          for (int i = 0, tokenArrayLength = node.tokens.length; i < tokenArrayLength; i++)
             {
               String tokenString = new String(node.tokens[i]);
               System.out.println("     --- ImportReference tokens = " + tokenString);

               if (i > 0)
                    importReference += '.';

               importReference += tokenString;
             }

          boolean withOnDemand = true;
          boolean containsWildcard = false;
          String importReferenceWithoutWildcard = importReference;
          if (withOnDemand && ((node.bits & node.OnDemand) != 0))
             {
            // output.append(".*");
               System.out.println("     --- ImportReference tokens = *");
               importReference += ".*";
               containsWildcard = true;
             }

          if (java_parser.verboseLevel > -1)
               System.out.println("importReference (string) = " + importReference);

       // DQ (8/22/2011): Read the referenced class or set of classes defined by the import statement.
          System.out.println("In visit (ImportReference,CompilationUnitScope): Calling buildImplicitClassSupport() to recursively build the class structure with member declarations: name = " + importReferenceWithoutWildcard);
          JavaParserSupport.buildImplicitClassSupport(importReferenceWithoutWildcard);
          System.out.println("DONE: In visit (ImportReference,CompilationUnitScope): Calling buildImplicitClassSupport() to recursively build the class structure with member declarations: name = " + importReferenceWithoutWildcard);

       // DQ (4/15/2011): I could not get the passing of a boolean to work, so I am just passing an integer.
          int containsWildcard_integer = containsWildcard ? 1 : 0;
       // java_parser.cactionImportReference(importReferenceWithoutWildcard,containsWildcard_integer);
          java_parser.cactionImportReference(importReferenceWithoutWildcard,containsWildcard_integer,this.createJavaToken(node));

       // Use the token to set the source code position for ROSE.
       // java_parser.cactionGenerateToken(token);

/*
       // I now do not think this is worth doing at this point.  Basically, we will treat the "import" statement
       // as a stringified construct (as much as I don't like that approach within a compiler).

       // We now read in classes on demand as they are references, if they are references.  All classes are read as 
       // required to resolved all types in the program and the implicitly read classes.  Interestingly, there
       // are nearly 3000 classes in the 135 packages in the J2SE 1.4.2 standard class library (over 3000 classes 
       // in 165 packages in J2SE 5.0). Commercial class libraries that you might use add many more packages. 
       // This implements the equivalent of the C++ "use" statement on "java.lang".
          System.out.println("Processing import statement: path = " + importReference + " importReferenceWithoutWildcard = " + importReferenceWithoutWildcard);

          JavaParserSupport.buildImplicitClassSupport("java.lang");

       // Note that reflection does not work on all levels (e.g. "java.io"), so we have to handle some cases explicitly.
          String s = new String(node.tokens[0]);
          int tokenArrayLength = node.tokens.length;
          System.out.println("tokenArrayLength = " + tokenArrayLength + " s = " + s);
          String javaString = new String("java");
          if (s == javaString)
             {
               System.out.println("In path segment: s = " + s);
               if (tokenArrayLength > 1)
                  {
                    s = new String(node.tokens[1]);

                 // This is the start of the default implicit package within the Java standard.
                    if (s == "lang")
                       {
                      // This is the start of the default implicit language package within the Java standard.
                         if (containsWildcard == true)
                            {
                              System.out.println("Processing java.lang.*");
                            }
                           else
                            {
                              System.out.println("Error: import java.lang is not defined in Java");
                              System.exit(1);
                            }
                       }
                      else
                       {
                         if (s == "io")
                            {
                           // This is the start of the default implicit package within the Java standard.
                              if (containsWildcard == true)
                                 {
                                   System.out.println("Processing java.io.*");
                                 }
                                else
                                 {
                                   System.out.println("Error: import java.io is not defined in Java");
                                   System.exit(1);
                                 }
                            }
                           else
                            {
                              System.out.println("Sorry, import java." + s + " is not implemented yet");
                              System.exit(1);
                            }
                       }
                  }
                 else
                  {
                    if (containsWildcard == true)
                       {
                      // System.out.println("Processing java.*");
                         System.out.println("Sorry, import java.* is not implemented yet");
                         System.exit(1);
                       }
                      else
                       {
                         System.out.println("Error: import java is not implemented yet");
                         System.exit(1);
                       }
                  }
             }
            else
             {
               System.out.println("Not a part of java standard package: import " + s + " is not implemented yet");
               System.exit(1);
             }

       // JavaParserSupport.buildImplicitClassSupport(importReference);
          JavaParserSupport.buildImplicitClassSupport(importReferenceWithoutWildcard);

          System.out.println("DONE: Processing import statement: path = " + importReference + " importReferenceWithoutWildcard = " + importReferenceWithoutWildcard);
*/
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ImportReference,CompilationUnitScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Initializer  node, MethodScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Initializer,MethodScope)");

          java_parser.cactionInitializer(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Initializer,MethodScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( InstanceOfExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (InstanceOfExpression,BlockScope)");

          java_parser.cactionInstanceOfExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (InstanceOfExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(IntLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
             System.out.println("Inside of visit (IntLiteral,BlockScope) value = " + node.toString());

       // java_parser.cactionIntLiteral();
       // java_parser.cactionIntLiteral(node.value);
          java_parser.cactionIntLiteral(node.constant.intValue(), this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (IntLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Javadoc  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Javadoc,BlockScope)");

          java_parser.cactionJavadoc(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Javadoc,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Javadoc  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Javadoc,ClassScope)");

          java_parser.cactionJavadocClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Javadoc,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocAllocationExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocAllocationExpression,BlockScope)");

          java_parser.cactionJavadocAllocationExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocAllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocAllocationExpression  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocAllocationExpression,ClassScope)");

          java_parser.cactionJavadocAllocationExpressionClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocAllocationExpression,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArgumentExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit JavadocArgumentExpression(,BlockScope)");

          java_parser.cactionJavadocArgumentExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocArgumentExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArgumentExpression  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocArgumentExpression,ClassScope)");

          java_parser.cactionJavadocArgumentExpressionClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocArgumentExpression,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArrayQualifiedTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocArrayQualifiedTypeReference,BlockScope)");

          java_parser.cactionJavadocArrayQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocArrayQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArrayQualifiedTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocArrayQualifiedTypeReference,ClassScope)");

          java_parser.cactionJavadocArrayQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocArrayQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArraySingleTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocArraySingleTypeReference,BlockScope)");

          java_parser.cactionJavadocArraySingleTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocArraySingleTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocArraySingleTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocArraySingleTypeReference,ClassScope)");

          java_parser.cactionJavadocArraySingleTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocArraySingleTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocFieldReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocFieldReference,BlockScope)");

          java_parser.cactionJavadocFieldReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocFieldReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocFieldReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocFieldReference,ClassScope)");

          java_parser.cactionJavadocFieldReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocFieldReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocImplicitTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocImplicitTypeReference,BlockScope)");

          java_parser.cactionJavadocImplicitTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocImplicitTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocImplicitTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocImplicitTypeReference,ClassScope)");

          java_parser.cactionJavadocImplicitTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocImplicitTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocMessageSend  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocMessageSend,BlockScope)");

          java_parser.cactionJavadocMessageSend(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocMessageSend,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocMessageSend  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocMessageSend,ClassScope)");

          java_parser.cactionJavadocMessageSendClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocMessageSend,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocQualifiedTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocQualifiedTypeReference,BlockScope)");

          java_parser.cactionJavadocQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocQualifiedTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocQualifiedTypeReference,ClassScope)");

          java_parser.cactionJavadocQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocReturnStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocReturnStatement,BlockScope)");

          java_parser.cactionJavadocReturnStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocReturnStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocReturnStatement  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocReturnStatement,ClassScope)");

          java_parser.cactionJavadocReturnStatementClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocReturnStatement,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocSingleNameReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocSingleNameReference,BlockScope)");

          java_parser.cactionJavadocSingleNameReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocSingleNameReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocSingleNameReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocSingleNameReference,ClassScope)");

          java_parser.cactionJavadocSingleNameReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocSingleNameReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocSingleTypeReference  node, BlockScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocSingleTypeReference,BlockScope)");

          java_parser.cactionJavadocSingleTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocSingleTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(JavadocSingleTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (JavadocSingleTypeReference,ClassScope)");

          java_parser.cactionJavadocSingleTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (JavadocSingleTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(LabeledStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (LabeledStatement,BlockScope)");

          java_parser.cactionLabeledStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (LabeledStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(LocalDeclaration  node, BlockScope scope)
        {
       // LocalDeclarations is derived from AbstractVariableDeclaration

          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (LocalDeclaration,BlockScope)");

          String name = new String(node.name);

       // String selectorName = new String(node.selector);
       // System.out.println("node.name = " + selectorName);
       // System.out.println("node.modfiers = " + node.modfiers);

          if (java_parser.verboseLevel > 0)
             {
               System.out.println("node.name                     = " + name);
               System.out.println("node.binding                  = " + node.binding);
               System.out.println("node.binding.type             = " + node.binding.type);
               System.out.println("node.binding.type.id          = " + node.binding.type.id);
               System.out.println("node.binding.type.debugName() = " + node.binding.type.debugName());
               System.out.println("node.type                     = " + node.type);
             }

       // Construct the type (will be constructed on the astJavaTypeStack.

       // DQ (7/18/2011): Switch to using the different generateType() function (taking a TypeReference).
       // JavaParserSupport.generateType(node.binding.type);
          JavaParserSupport.generateType(node.type);

          boolean isFinal = node.binding.isFinal();

       // DQ (8/13/2011): This information is stored in the FieldReference...(not clear how to get it).
       // boolean isPrivate = (node.binding != null && !node.binding.isPrivate()) ? true : false;

       // Build the variable declaration using the type from the astJavaTypeStack.
       // Note that this may have to handle an array of names or be even more complex in the future.
          java_parser.cactionLocalDeclaration(name,isFinal, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (LocalDeclaration,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(LongLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (LongLiteral,BlockScope)");

          java_parser.cactionLongLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (LongLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(MarkerAnnotation  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (MarkerAnnotation,BlockScope)");

          java_parser.cactionMarkerAnnotation(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (MarkerAnnotation,BlockScope)");

          return true;
        }

     public boolean visit(MemberValuePair  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (MemberValuePair,BlockScope)");

          java_parser.cactionMemberValuePair(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (MemberValuePair,BlockScope)");

          return true;
        }

     public boolean visit(MessageSend  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (MessageSend,BlockScope)");

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

          String name                        = new String(node.selector);
          String associatedClassVariableName = node.receiver.toString();

       // String associatedClassName = node.actualReceiverType.toString();
          String associatedClassName = node.actualReceiverType.debugName();

          if (java_parser.verboseLevel > 0)
             {
               System.out.println("MessageSend node = " + node);

               System.out.println("     --- function call name = " + name);

               System.out.println("     --- function call from class name (binding)            = " + node.binding);
               System.out.println("     --- function call from class name (receiver)           = " + node.receiver);
               System.out.println("     --- function call from class name (associatedClassVar) = " + associatedClassVariableName);
               System.out.println("     --- function call from class name (associatedClass)    = " + associatedClassName);

               if (node.typeArguments != null)
                  {
                    System.out.println("Sorry, not implemented in support for MessageSend: typeArguments");
                    for (int i = 0, typeArgumentsLength = node.typeArguments.length; i < typeArgumentsLength; i++)
                       {
                      // node.typeArguments[i].traverse(visitor, blockScope);
                         System.out.println("     --- method send (function call) typeArguments = " + node.typeArguments[i]);
                       }
                  }

               if (node.arguments != null)
                  {
                    System.out.println("Sorry, not implemented in support for MessageSend: arguments");
                    int argumentsLength = node.arguments.length;
                    for (int i = 0; i < argumentsLength; i++)
                       {
                      // node.arguments[i].traverse(visitor, blockScope);
                         System.out.println("     --- method arguments (function call arguments) arguments = " + node.arguments[i]);
                       }
                  }
             }

       // Build this as an implicit class (if it is already built it will be detected and not rebuilt).
       // System.out.println("Calling buildImplicitClassSupport for associatedClassName = " + associatedClassName);

       // DQ (8/18/2011): check if the name starts with "java", if so it is an implicit class.
       // JavaParserSupport.buildImplicitClassSupport(associatedClassName);
          if (associatedClassName.startsWith("java") == true)
             {
               JavaParserSupport.buildImplicitClassSupport(associatedClassName);
             }

       // This is an error for test2001_04.java when println is found since it is not located in "java.lang" but is in java.io"
       // Maybe we need to process ""java.lang.System" and "java.io.String" and a few other classes explicitly.
       // JavaParserSupport.buildImplicitClassSupport("java.lang." + associatedClassName);
       // JavaParserSupport.buildImplicitClassSupport("java.io." + associatedClassName);
       // JavaParserSupport.buildImplicitClassSupport("java.lang.System");
       // System.out.println("DONE: Calling buildImplicitClassSupport for associatedClassName = " + associatedClassName);

       // System.out.println("Exiting after test...");
       // System.exit(1);

       // java_parser.cactionExplicitConstructorCall("super");
       // java_parser.cactionMessageSend(name,associatedClassVariableName, this.createJavaToken(node));
          java_parser.cactionMessageSend(name,associatedClassName, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (MessageSend,BlockScope)");

       // System.out.println("Exiting as a test in visit (MessageSend,BlockScope)...");
       // System.exit(1);

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(MethodDeclaration  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (MethodDeclaration,ClassScope)");

       // char [] name = node.selector;
       // System.out.println("Inside of visit (MethodDeclaration,ClassScope) method name = " + name);
          String name = new String(node.selector);
          if (java_parser.verboseLevel > 0)
             {
               System.out.println("Inside of visit (MethodDeclaration,ClassScope) method name = " + name);

            // Return type
               System.out.println("     --- method returnType = " + node.returnType);

            // argument types
               if (node.typeParameters != null)
                  {
                    for (int i = 0, typeArgumentsLength = node.typeParameters.length; i < typeArgumentsLength; i++)
                       {
                         System.out.println("     --- method typeParameters = " + node.typeParameters[i]);

                      // System.out.println("This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (visit (MethodDeclaration,ClassScope)): type = " + node.typeParameters[i]);
                      // JavaParserSupport.generateType(node.typeParameters[i]);
                      // System.out.println("DONE: This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (visit (MethodDeclaration,ClassScope)): type = " + node.typeParameters[i]);
                       }
                  }
                 else
                  {
                 // For a function defined in the input program, the typeParameters array is empty, but the ECJ
                 // specific AST traversal will visit the type parameters. Not clear why this is organized like this.
                    System.out.println("     --- method typeParameters (empty) = " + node.typeParameters);
                  }

               if (node.statements != null)
                  {
                    for (int i = 0, statementListLength = node.statements.length; i < statementListLength; i++)
                       {
                         System.out.println("     --- method statements[" + i + "] = " + node.statements[i]);
                       }
                  }
             }

       // Looking here for arguments (want the arguments) since they are not always in the node.typeParameters
          if (node.arguments != null)
             {
               for (int i = 0, typeArgumentsLength = node.arguments.length; i < typeArgumentsLength; i++)
                  {
                 // System.out.println("     --- method arguments = " + node.arguments[i].type);
                    if (java_parser.verboseLevel > 0)
                         System.out.println("     --- method arguments (type = " + node.arguments[i].type + ", name = " + new String(node.arguments[i].name) + ")");

                    String nameString = new String(node.arguments[i].name);
                 // System.out.println("This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (visit (MethodDeclaration,ClassScope)): type = " + node.arguments[i].type + " nameString = " + nameString);
                    JavaParserSupport.generateType(node.arguments[i].type);

                 // This rule assumes that the type will be made available on the stack (astJavaTypeStack).
                 // In general we want to have rules that are specific to IR nodes and pass any constructed
                 // IR nodes via the stack (rules called should have generated the constructed IR nodes on 
                 // the stack within ROSE).
                    java_parser.cactionArgument(nameString,node.modifiers, this.createJavaToken(node));

                 // System.out.println("DONE: This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (visit (MethodDeclaration,ClassScope)): type = " + node.arguments[i].type + " nameString = " + nameString);
                  }
             }
            else
             {
            // For a function defined in the input program, the typeParameters array is empty, but the ECJ
            // specific AST traversal will visit the type parameters. Not clear why this is organized like this.

               if (java_parser.verboseLevel > 0)
                    System.out.println("     --- method arguments (empty) = " + node.arguments);
             }

          if (java_parser.verboseLevel > 2)
               System.out.println("Process the return type = " + node.returnType);

          if (node.returnType == null)
             {
               System.out.println("Error: no return type defined for the member function node.returnType = " + node.returnType);
               System.exit(1);
             }

       // Build the return type in ROSE and push it onto the stack (astJavaTypeStack).
          JavaParserSupport.generateType(node.returnType);

          if (java_parser.verboseLevel > 2)
               System.out.println("DONE: Process the return type = " + node.returnType);

       // System.out.println("Exiting to test error handling! \n");
       // System.exit(0);

       // Setup the function modifiers
          boolean isAbstract = node.isAbstract();
          boolean isNative   = node.isNative();
          boolean isStatic   = node.isStatic();

          boolean isFinal    = node.binding.isFinal();

          boolean isPrivate = (node.binding != null && node.binding.isPrivate()) ? true : false;

       // These is no simple function for theses cases.
          boolean isSynchronized = ((node.modifiers & ClassFileConstants.AccSynchronized) != 0) ? true : false;
          boolean isPublic       = ((node.modifiers & ClassFileConstants.AccPublic)       != 0) ? true : false;
          boolean isProtected    = ((node.modifiers & ClassFileConstants.AccProtected)    != 0) ? true : false;

          boolean isStrictfp     = node.binding.isStrictfp();

       // These are always false for member functions.
          boolean isVolatile     = false;
          boolean isTransient    = false;

          if (java_parser.verboseLevel > 2)
             {
               System.out.println("In visit (MethodDeclaration,ClassScope): isPrivate      = " + (isPrivate      ? "true" : "false"));
               System.out.println("In visit (MethodDeclaration,ClassScope): isSynchronized = " + (isSynchronized ? "true" : "false"));
               System.out.println("In visit (MethodDeclaration,ClassScope): isPublic       = " + (isPublic       ? "true" : "false"));
               System.out.println("In visit (MethodDeclaration,ClassScope): isProtected    = " + (isProtected    ? "true" : "false"));
               System.out.println("In visit (MethodDeclaration,ClassScope): isStrictfp     = " + (isStrictfp     ? "true" : "false"));
             }

/*
          if ((modifiers & ClassFileConstants.AccPublic) != 0)
               output.append("public "); //$NON-NLS-1$
          if ((modifiers & ClassFileConstants.AccPrivate) != 0)
               output.append("private "); //$NON-NLS-1$
          if ((modifiers & ClassFileConstants.AccProtected) != 0)
               output.append("protected "); //$NON-NLS-1$
          if ((modifiers & ClassFileConstants.AccStatic) != 0)
               output.append("static "); //$NON-NLS-1$
          if ((modifiers & ClassFileConstants.AccFinal) != 0)
               output.append("final "); //$NON-NLS-1$
          if ((modifiers & ClassFileConstants.AccSynchronized) != 0)
               output.append("synchronized "); //$NON-NLS-1$
          if ((modifiers & ClassFileConstants.AccVolatile) != 0)
               output.append("volatile "); //$NON-NLS-1$
          if ((modifiers & ClassFileConstants.AccTransient) != 0)
               output.append("transient "); //$NON-NLS-1$
          if ((modifiers & ClassFileConstants.AccNative) != 0)
               output.append("native "); //$NON-NLS-1$
          if ((modifiers & ClassFileConstants.AccAbstract) != 0)
               output.append("abstract "); //$NON-NLS-1$
*/

       // We can build this here but we can't put the symbol into the symbol tabel until 
       // we have gathered the function parameter types so that the correct function type 
       // can be computed.
          java_parser.cactionMethodDeclaration(name,isAbstract,isNative,isStatic,isFinal,isSynchronized,isPublic,isProtected,isPrivate,isStrictfp, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (MethodDeclaration,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( StringLiteralConcatenation node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (StringLiteralConcatenation,BlockScope)");

          java_parser.cactionStringLiteralConcatenation(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (StringLiteralConcatenation,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(NormalAnnotation  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (NormalAnnotation,BlockScope)");

          java_parser.cactionNormalAnnotation(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (NormalAnnotation,BlockScope)");

          return true;
        }

     public boolean visit(NullLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (NullLiteral,BlockScope)");

          java_parser.cactionNullLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (NullLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(OR_OR_Expression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (OR_OR_Expression,BlockScope)");

          java_parser.cactionORORExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (OR_OR_Expression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ParameterizedQualifiedTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ParameterizedQualifiedTypeReference,BlockScope)");

          java_parser.cactionParameterizedQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ParameterizedQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ParameterizedQualifiedTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ParameterizedQualifiedTypeReference,ClassScope)");

          java_parser.cactionParameterizedQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ParameterizedQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ParameterizedSingleTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ParameterizedSingleTypeReference,BlockScope)");

          java_parser.cactionParameterizedSingleTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ParameterizedSingleTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ParameterizedSingleTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ParameterizedSingleTypeReference,ClassScope)");

          java_parser.cactionParameterizedSingleTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ParameterizedSingleTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(PostfixExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (PostfixExpression,BlockScope)");

          java_parser.cactionPostfixExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (PostfixExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(PrefixExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (PrefixExpression,BlockScope)");

          java_parser.cactionPrefixExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (PrefixExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(QualifiedAllocationExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (QualifiedAllocationExpression,BlockScope)");

          java_parser.cactionQualifiedAllocationExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (QualifiedAllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(QualifiedNameReference node, BlockScope scope)
        {
       // Note that if we want to traverse this object in more detail then the QualifiedNameReference::traverse() functions can/should be modified.
          java_parser.cactionQualifiedNameReference("QualifiedNameReference_block_abc", this.createJavaToken(node));

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

          java_parser.cactionQualifiedNameReference("QualifiedNameReference_class_abc", this.createJavaToken(node));

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
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (QualifiedSuperReference,BlockScope)");

          java_parser.cactionQualifiedSuperReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (QualifiedSuperReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( QualifiedSuperReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (QualifiedSuperReference,ClassScope)");

          java_parser.cactionQualifiedSuperReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (QualifiedSuperReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(QualifiedThisReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (QualifiedThisReference,BlockScope)");

          java_parser.cactionQualifiedThisReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (QualifiedThisReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit( QualifiedThisReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (QualifiedThisReference,ClassScope)");

          java_parser.cactionQualifiedThisReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (QualifiedThisReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(QualifiedTypeReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (QualifiedTypeReference,BlockScope)");

          java_parser.cactionQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (QualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(QualifiedTypeReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (QualifiedTypeReference,ClassScope)");

          java_parser.cactionQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (QualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ReturnStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ReturnStatement,BlockScope)");

          java_parser.cactionReturnStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ReturnStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SingleMemberAnnotation  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (SingleMemberAnnotation,BlockScope)");

          java_parser.cactionSingleMemberAnnotation(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (SingleMemberAnnotation,BlockScope)");

          return true;
        }

     public boolean visit(SingleNameReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (SingleNameReference,BlockScope)");

          String varRefName = node.toString();
          if (java_parser.verboseLevel > 0)
             {
               System.out.println("Building a variable reference for name = " + varRefName);

               System.out.println("node.genericCast = " + node.genericCast);
             }

          java_parser.cactionSingleNameReference(varRefName, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (SingleNameReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SingleNameReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (SingleNameReference,ClassScope)");

          java_parser.cactionSingleNameReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (SingleNameReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SingleTypeReference node, BlockScope scope)
        {
       // java_parser.cactionSingleTypeReference("abc");

/*
          Moved to the endVisit() function.

          if (node.resolvedType != null)
             {
               java_parser.cactionSingleTypeReference("SingleTypeReference_block_abc", this.createJavaToken(node));
            // char[][] char_string = node.getTypeName();
            // System.out.println(char_string);
            // String typename = new String(node.getTypeName().toString());
            // String typename = node.getTypeName().toString();
               String typename = node.toString();
               System.out.println("Sorry, not implemented SingleTypeReference (node.resolvedType != NULL): typename = " + typename);

            // DQ (7/17/2011): I think we need a typeReferenceExpression specific to Java.
            // System.out.println("--- We need to build a reference to a type as an expression to be used in instanceof operator (for expressions)");

               JavaParserSupport.generateType(node);
             }
            else
             {
               System.out.println("Sorry, not implemented SingleTypeReference: node.resolvedType == NULL");
             }
*/
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SingleTypeReference node, ClassScope scope)
        {
          java_parser.cactionSingleTypeReference("SingleTypeReference_class_abc", this.createJavaToken(node));
          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(StringLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (StringLiteral,BlockScope)");

       // System.out.println("Inside of visit (StringLiteral,BlockScope): node = " + node);
          String literal = new String(node.source());
          System.out.println("Inside of visit (StringLiteral,BlockScope): node = " + literal);

          java_parser.cactionStringLiteral(literal, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (StringLiteral,BlockScope)");

       // System.out.println("Exiting at base of visit (StringLiteral,BlockScope)");
       // System.exit(1);

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SuperReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (SuperReference,BlockScope)");

          java_parser.cactionSuperReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (SuperReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SwitchStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (SwitchStatement,BlockScope)");

          java_parser.cactionSwitchStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (SwitchStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(SynchronizedStatement node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (SynchronizedStatement,BlockScope)");

          java_parser.cactionSynchronizedStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (SynchronizedStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ThisReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ThisReference,BlockScope)");

          java_parser.cactionThisReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ThisReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ThisReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ThisReference,ClassScope)");

          java_parser.cactionThisReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ThisReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(ThrowStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (ThrowStatement,BlockScope)");

          java_parser.cactionThrowStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (ThrowStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TrueLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (TrueLiteral,BlockScope)");

          java_parser.cactionTrueLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (TrueLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TryStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (TryStatement,BlockScope)");

          java_parser.cactionTryStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (TryStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeDeclaration node,BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("visit TypeDeclaration -- BlockScope");

          String typename = new String(node.name);
          java_parser.cactionTypeDeclaration(typename, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (TypeDeclaration,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeDeclaration node,ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("visit TypeDeclaration -- ClassScope");

          String typename = new String(node.name);
          java_parser.cactionTypeDeclaration(typename, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (TypeDeclaration,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeDeclaration node,CompilationUnitScope scope)
        {
       // System.out.println("visit TypeDeclaration -- CompilationUnitScope");
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (TypeDeclaration,CompilationUnitScope)");

          String typename = new String(node.name);
          java_parser.cactionTypeDeclaration(typename, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (TypeDeclaration,CompilationUnitScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeParameter  node, BlockScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (TypeParameter,BlockScope)");

          java_parser.cactionTypeParameter(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (TypeParameter,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(TypeParameter  node, ClassScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (TypeParameter,ClassScope)");

          java_parser.cactionTypeParameterClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (TypeParameter,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(UnaryExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (UnaryExpression,BlockScope)");

          java_parser.cactionUnaryExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (UnaryExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(WhileStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (WhileStatement,BlockScope)");

          java_parser.cactionWhileStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Wildcard  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Wildcard,BlockScope)");

          java_parser.cactionWildcard(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Wildcard,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean visit(Wildcard  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of visit (Wildcard,ClassScope)");

          java_parser.cactionWildcardClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving visit (Wildcard,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }


  // DQ (3/18/2011): Not clear if I need this yet.
  // DQ (3/25/2011): I think we do need them.
     public void endVisit(AllocationExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (AllocationExpression,BlockScope)");

       // String nameString = new String(node.type);
          String nameString = node.type.toString();
          if (java_parser.verboseLevel > 0)
             {
               System.out.println(" name      = " + nameString);
               System.out.println(" type      = " + node.type);
             }

          if (node.typeArguments != null)
             {
               for (int i = 0, typeArgumentsLength = node.typeArguments.length; i < typeArgumentsLength; i++)
                  {

                  }
             }

       // Generate the associated type and Push it onto the stack
          JavaParserSupport.generateType(node.type);

       // Call the Java side of the JNI function.
          java_parser.cactionAllocationExpressionEnd(nameString,this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (AllocationExpression,BlockScope)");

        }

     public void endVisit(AND_AND_Expression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (AND_AND_Expression,BlockScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionANDANDExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (AND_AND_Expression,BlockScope)");

        }

     public void endVisit(AnnotationMethodDeclaration node, ClassScope classScope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (AnnotationMethodDeclaration,ClassScope)");
        }

     public void endVisit(Argument  node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (Argument,BlockScope)");
        }

     public void endVisit(Argument  node,ClassScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (Argument,ClassScope)");
        }

     public void endVisit(ArrayAllocationExpression node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ArrayAllocationExpression,BlockScope)");
        }

     public void endVisit(ArrayInitializer  node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ArrayInitializer,BlockScope)");
        }

     public void endVisit(ArrayQualifiedTypeReference node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ArrayQualifiedTypeReference,BlockScope)");
        }

     public void endVisit(ArrayQualifiedTypeReference node, ClassScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ArrayQualifiedTypeReference,ClassScope)");
        }

     public void endVisit(ArrayReference  node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ArrayReference,BlockScope)");
        }

     public void endVisit(ArrayTypeReference  node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ArrayTypeReference,BlockScope)");
        }

     public void endVisit(ArrayTypeReference  node, ClassScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ArrayTypeReference,ClassScope)");
        }

     public void endVisit(AssertStatement  node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (AssertStatement,BlockScope)");
        }

     public void endVisit(Assignment  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (Assignment,BlockScope)");

          java_parser.cactionAssignmentEnd(this.createJavaToken(node));
        }


     public void endVisit(BinaryExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (BinaryExpression,BlockScope)");

          int operatorKind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT;

          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (BinaryExpression,BlockScope): operatorKind = " + operatorKind);

          java_parser.cactionBinaryExpressionEnd(operatorKind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (BinaryExpression,BlockScope)");

        }

     public void endVisit(Block  node, BlockScope scope) 
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (Block,BlockScope)");

          if (java_parser.verboseLevel > 1)
               System.out.println("node.explicitDeclarations = " + node.explicitDeclarations);

          int numberOfStatements = 0;
          if (node.statements != null)
               numberOfStatements = node.statements.length;

          if (java_parser.verboseLevel > 0)
               System.out.println("numberOfStatements = " + numberOfStatements);

       // DQ (9/30/2011): We need to pass the number of statments so that we can pop 
       // a pricise number of statements off of the stack (and not the whole stack).
          java_parser.cactionBlockEnd(numberOfStatements, this.createJavaToken(node));
        }

     public void endVisit(BreakStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (BreakStatement,BlockScope)");
        }

     public void endVisit(CaseStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (CaseStatement,BlockScope)");
        }

     public void endVisit(CastExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (CastExpression,BlockScope)");

          java_parser.cactionCastExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (CastExpression,BlockScope)");
        }

     public void endVisit(CharLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (CharLiteral,BlockScope)");
        }

     public void endVisit(ClassLiteralAccess  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ClassLiteralAccess,BlockScope)");
        }

     public void endVisit(Clinit  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (Clinit,BlockScope)");
        }

     public void endVisit(CompilationUnitDeclaration node, CompilationUnitScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (CompilationUnitDeclaration,CompilationUnitScope)");

          int numberOfStatements = 0;
          if (node.types != null)
             {
               numberOfStatements += node.types.length;
               if (java_parser.verboseLevel > 0)
                    System.out.println("node.types.length = " + node.types.length);
             }

          if (node.imports != null)
             {
               numberOfStatements += node.imports.length;
               if (java_parser.verboseLevel > 0)
                    System.out.println("node.imports.length = " + node.imports.length);
             }

          if (java_parser.verboseLevel > 0)
               System.out.println("numberOfStatements = " + numberOfStatements);

          java_parser.cactionCompilationUnitDeclarationEnd(numberOfStatements, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (CompilationUnitDeclaration,CompilationUnitScope)");
        }

     public void endVisit(CompoundAssignment  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (CompoundAssignment,BlockScope)");

          if (java_parser.verboseLevel > 0)
             System.out.println("Inside of endVisit (CompoundAssignment,BlockScope): operator_kind" + node.toString());

          int operator_kind = node.operator;

          java_parser.cactionCompoundAssignmentEnd(operator_kind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (CompoundAssignment,BlockScope)");
        }

     public void endVisit(ConditionalExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (ConditionalExpression,BlockScope)");

          java_parser.cactionConditionalExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ConditionalExpression,BlockScope)");
        }

     public void endVisit(ConstructorDeclaration node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (ConstructorDeclaration,ClassScope)");

       // DQ (7/31/2011): Added more precise handling of statements to be collected from the statement stack.
          int numberOfStatements = 0;
          if (node.statements != null)
             {
               numberOfStatements = node.statements.length;

               if (java_parser.verboseLevel > 0)
                    System.out.println("Inside of endVisit (ConstructorDeclaration,ClassScope): numberOfStatements = " + numberOfStatements);
             }

       // DQ (7/31/2011): I don't know if there is just one of these (super()) or if there could be many.
          if (node.constructorCall != null)
             {
               numberOfStatements++;
            // System.out.println("Inside of endVisit (ConstructorDeclaration,ClassScope): increment the numberOfStatements = " + numberOfStatements);
             }

          java_parser.cactionConstructorDeclarationEnd(numberOfStatements, this.createJavaToken(node));
        }

     public void endVisit(ContinueStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ContinueStatement,BlockScope)");
        }

     public void endVisit(DoStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (DoStatement,BlockScope)");

          java_parser.cactionDoStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (DoStatement,BlockScope)");
        }

     public void endVisit(DoubleLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (DoubleLiteral,BlockScope)");
        }

     public void endVisit(EmptyStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (EmptyStatement,BlockScope)");
        }

     public void endVisit(EqualExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (EqualExpression,BlockScope)");

          int operator_kind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT; // EQUAL_EQUAL or NOT_EQUAL

          java_parser.cactionEqualExpressionEnd(operator_kind, this.createJavaToken(node));
        }

     public void endVisit(ExplicitConstructorCall node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (ExplicitConstructorCall,BlockScope)");

          java_parser.cactionExplicitConstructorCallEnd("abc", this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ExplicitConstructorCall,BlockScope)");
        }

     public void endVisit(ExtendedStringLiteral node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ExtendedStringLiteral,BlockScope)");
        }

     public void endVisit(FalseLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (FalseLiteral,BlockScope)");
        }

     public void endVisit(FieldDeclaration  node, MethodScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (FieldDeclaration,BlockScope)");

       // We pass the name to support debugging...
          String name = new String(node.name);

          boolean hasInitializer = (node.initialization != null) ? true : false;

          java_parser.cactionFieldDeclarationEnd(name,hasInitializer, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (FieldDeclaration,BlockScope)");
        }

     public void endVisit(FieldReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (FieldReference,BlockScope)");
        }

     public void endVisit(FieldReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (FieldReference,ClassScope)");
        }

     public void endVisit(FloatLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (FloatLiteral,BlockScope)");
        }

     public void endVisit(ForeachStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ForeachStatement,BlockScope)");
        }

     public void endVisit(ForStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (ForStatement,BlockScope)");

          java_parser.cactionForStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ForStatement,BlockScope)");
        }

     public void endVisit(IfStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (IfStatement,BlockScope)");

          int numberOfStatements = 0;
          if (node.thenStatement != null)
             {
            // System.out.println("Inside of visit (IfStatement,BlockScope): thenStatement detected");
               numberOfStatements = 1;
             }
          
          if (node.elseStatement != null)
             {
            // System.out.println("Inside of visit (IfStatement,BlockScope): elseStatement detected");
               numberOfStatements = 2;
             }

          java_parser.cactionIfStatementEnd(numberOfStatements, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (IfStatement,BlockScope)");
        }

     public void endVisit(ImportReference  node, CompilationUnitScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ImportReference,CompilationUnitScope)");
        }

     public void endVisit(Initializer  node, MethodScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (Initializer,MethodScope)");
        }

     public void endVisit(InstanceOfExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (InstanceOfExpression,BlockScope)");

          java_parser.cactionInstanceOfExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (InstanceOfExpression,BlockScope)");
        }

     public void endVisit(IntLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (IntLiteral,BlockScope)");
        }

     public void endVisit(Javadoc  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (Javadoc,BlockScope)");
        }

     public void endVisit(Javadoc  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (Javadoc,ClassScope)");
        }

     public void endVisit(JavadocAllocationExpression  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocAllocationExpression,BlockScope)");
        }

     public void endVisit(JavadocAllocationExpression  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocAllocationExpression,ClassScope)");
        }

     public void endVisit(JavadocArgumentExpression  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocArgumentExpression,BlockScope)");
        }

     public void endVisit(JavadocArgumentExpression  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocArgumentExpression,ClassScope)");
        }

     public void endVisit(JavadocArrayQualifiedTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocArrayQualifiedTypeReference,BlockScope)");
        }

     public void endVisit(JavadocArrayQualifiedTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocArrayQualifiedTypeReference,ClassScope)");
        }

     public void endVisit(JavadocArraySingleTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocArraySingleTypeReference,BlockScope)");
        }

     public void endVisit(JavadocArraySingleTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocArraySingleTypeReference,ClassScope)");
        }

     public void endVisit(JavadocFieldReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocFieldReference,BlockScope)");
        }

     public void endVisit(JavadocFieldReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocFieldReference,ClassScope)");
        }

     public void endVisit(JavadocImplicitTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocImplicitTypeReference,BlockScope)");
        }

     public void endVisit(JavadocImplicitTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocImplicitTypeReference,ClassScope)");
        }

     public void endVisit(JavadocMessageSend  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocMessageSend,BlockScope)");
        }

     public void endVisit(JavadocMessageSend  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocMessageSend,ClassScope)");
        }

     public void endVisit(JavadocQualifiedTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocQualifiedTypeReference,BlockScope)");
        }

     public void endVisit(JavadocQualifiedTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocQualifiedTypeReference,ClassScope)");
        }

     public void endVisit(JavadocReturnStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocReturnStatement,BlockScope)");
        }

     public void endVisit(JavadocReturnStatement  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocReturnStatement,ClassScope)");
        }

     public void endVisit(JavadocSingleNameReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocSingleNameReference,BlockScope)");
        }

     public void endVisit(JavadocSingleNameReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocSingleNameReference,ClassScope)");
        }

     public void endVisit(JavadocSingleTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocSingleTypeReference,BlockScope)");
        }

     public void endVisit(JavadocSingleTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (JavadocSingleTypeReference,ClassScope)");
        }

     public void endVisit(LabeledStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (LabeledStatement,BlockScope)");
        }

     public void endVisit(LocalDeclaration  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (LocalDeclaration,BlockScope)");

          if (java_parser.verboseLevel > 0)
               System.out.println("If there is an expression on the stack it is the initializer...");

          if (node.initialization != null)
             {
               if (java_parser.verboseLevel > 0)
                    System.out.println("Process the initializer expression...");

               java_parser.cactionLocalDeclarationInitialization(this.createJavaToken(node));
             }
        }

     public void endVisit(LongLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (LongLiteral,BlockScope)");
        }

     public void endVisit(MarkerAnnotation  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (MarkerAnnotation,BlockScope)");
        }

     public void endVisit(MemberValuePair  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (MemberValuePair,BlockScope)");
        }

     public void endVisit(MessageSend  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > -1)
               System.out.println("Inside of endVisit (MessageSend,BlockScope)");

          java_parser.cactionMessageSendEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (MessageSend,BlockScope)");
        }

     public void endVisit(MethodDeclaration  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (MethodDeclaration,ClassScope)");

       // DQ (9/30/2011): We have to specify exactly how many statements we will collect off the stack.
          int numberOfStatements = 0;
          if (node.statements != null)
               numberOfStatements = node.statements.length;
       // System.out.println("numberOfStatements = " + numberOfStatements);

          java_parser.cactionMethodDeclarationEnd(numberOfStatements, this.createJavaToken(node));
        }

     public void endVisit(StringLiteralConcatenation  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (StringLiteralConcatenation,BlockScope)");
        }

     public void endVisit(NormalAnnotation  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (NormalAnnotation,BlockScope)");
        }

     public void endVisit(NullLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (NullLiteral,BlockScope)");
        }

     public void endVisit(OR_OR_Expression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (OR_OR_Expression,BlockScope)");

          java_parser.cactionORORExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (OR_OR_Expression,BlockScope)");
        }

     public void endVisit(ParameterizedQualifiedTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ParameterizedQualifiedTypeReference,BlockScope)");
        }

     public void endVisit(ParameterizedQualifiedTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ParameterizedQualifiedTypeReference,ClassScope)");
        }

     public void endVisit(ParameterizedSingleTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ParameterizedSingleTypeReference,BlockScope)");

          int numberOfTypeArguments = 0;
          if (node.typeArguments != null)
               numberOfTypeArguments = node.typeArguments.length;

          java_parser.cactionParameterizedSingleTypeReferenceEnd(numberOfTypeArguments,this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ParameterizedSingleTypeReference,BlockScope)");
        }

     public void endVisit(ParameterizedSingleTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ParameterizedSingleTypeReference,ClassScope)");
        }

     public void endVisit(PostfixExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (PostfixExpression,BlockScope)");

          int operator_kind = node.operator;

          java_parser.cactionPostfixExpressionEnd(operator_kind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (PostfixExpression,BlockScope)");
        }

     public void endVisit(PrefixExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (PrefixExpression,BlockScope)");

          int operator_kind = node.operator;

          java_parser.cactionPrefixExpressionEnd(operator_kind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (PrefixExpression,BlockScope)");
        }

     public void endVisit(QualifiedAllocationExpression node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (QualifiedAllocationExpression,BlockScope)");
        }

     public void endVisit(QualifiedNameReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (QualifiedNameReference,BlockScope)");
        }

     public void endVisit(QualifiedNameReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (QualifiedNameReference,ClassScope)");
        }

     public void endVisit(QualifiedSuperReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (QualifiedSuperReference,BlockScope)");
        }

     public void endVisit(QualifiedSuperReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (QualifiedSuperReference,ClassScope)");
        }

     public void endVisit(QualifiedThisReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (QualifiedThisReference,BlockScope)");
        }

     public void endVisit(QualifiedThisReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (QualifiedThisReference,ClassScope)");
        }

     public void endVisit(QualifiedTypeReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (QualifiedTypeReference,BlockScope)");
        }

     public void endVisit(QualifiedTypeReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (QualifiedTypeReference,ClassScope)");
        }

     public void endVisit(ReturnStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ReturnStatement,BlockScope)");
        }

     public void endVisit(SingleMemberAnnotation  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (SingleMemberAnnotation,BlockScope)");
        }

     public void endVisit(SingleNameReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (SingleNameReference,BlockScope)");
        }

     public void endVisit(SingleNameReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (SingleNameReference,ClassScope)");
        }

     public void endVisit(SingleTypeReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (SingleTypeReference,BlockScope)");

          if (node.resolvedType != null)
             {
               java_parser.cactionSingleTypeReference("SingleTypeReference_block_abc", this.createJavaToken(node));
            // char[][] char_string = node.getTypeName();
            // System.out.println(char_string);
            // String typename = new String(node.getTypeName().toString());
            // String typename = node.getTypeName().toString();
               String typename = node.toString();
            // System.out.println("Sorry, not implemented SingleTypeReference (node.resolvedType != NULL): typename = " + typename);

            // DQ (7/17/2011): I think we need a typeReferenceExpression specific to Java.
            // System.out.println("--- We need to build a reference to a type as an expression to be used in instanceof operator (for expressions)");

               JavaParserSupport.generateType(node);
             }
            else
             {
               System.out.println("Sorry, not implemented SingleTypeReference: node.resolvedType == NULL");
             }
        }

     public void endVisit(SingleTypeReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (SingleTypeReference,ClassScope)");
        }

     public void endVisit(StringLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (StringLiteral,BlockScope)");
        }

     public void endVisit(SuperReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (SuperReference,BlockScope)");
        }

     public void endVisit(SwitchStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (SwitchStatement,BlockScope)");
        }

     public void endVisit(SynchronizedStatement node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (SynchronizedStatement,BlockScope)");
        }

     public void endVisit(ThisReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ThisReference,BlockScope)");
        }

     public void endVisit(ThisReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ThisReference,ClassScope)");
        }

     public void endVisit(ThrowStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (ThrowStatement,BlockScope)");
        }

     public void endVisit(TrueLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (TrueLiteral,BlockScope)");
        }

     public void endVisit(TryStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (TryStatement,BlockScope)");
        }

     public void endVisit(TypeDeclaration node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("endVisit TypeDeclaration -- BlockScope");

          String typename = new String(node.name);
          java_parser.cactionTypeDeclarationEnd(typename,0, this.createJavaToken(node));

          System.out.println("Leaving endVisit (TypeDeclaration,BlockScope)");
        }

     public void endVisit(TypeDeclaration node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("endVisit TypeDeclaration -- ClassScope");

          String typename = new String(node.name);
          java_parser.cactionTypeDeclarationEnd(typename,0, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (TypeDeclaration,ClassScope)");
        }

     public void endVisit(TypeDeclaration node, CompilationUnitScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (TypeDeclaration,CompilationUnitScope)");

          String typename = new String(node.name);

          int numberOfMethods     = 0;
          int numberOfMemberTypes = 0;
          int numberOfFields      = 0;

          if (node.methods != null)
             {
            // System.out.println("Inside of endVisit (TypeDeclaration,CompilationUnitScope): node.methods.length     = " + node.methods.length);
               numberOfMethods = node.methods.length;
             }

          if (node.memberTypes != null)
             {
            // System.out.println("Inside of endVisit (TypeDeclaration,CompilationUnitScope): node.memberTypes.length = " + node.memberTypes.length);
               numberOfMemberTypes = node.memberTypes.length;
             }

          if (node.fields != null)
             {
            // System.out.println("Inside of endVisit (TypeDeclaration,CompilationUnitScope): node.fields.length      = " + node.fields.length);
               numberOfFields = node.fields.length;
             }

          int numberOfStatements = numberOfMethods + numberOfMemberTypes + numberOfFields;

          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (TypeDeclaration,CompilationUnitScope): numberOfStatements      = " + numberOfStatements);

          java_parser.cactionTypeDeclarationEnd(typename,numberOfStatements, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (TypeDeclaration,CompilationUnitScope)");
        }

     public void endVisit(TypeParameter  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (TypeParameter,BlockScope)");
        }

     public void endVisit(TypeParameter  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (TypeParameter,ClassScope)");
        }

     public void endVisit(UnaryExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (UnaryExpression,BlockScope)");

       // Not clear what the valueRequired filed means.
          int operator_kind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT;

          java_parser.cactionUnaryExpressionEnd(operator_kind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (UnaryExpression,BlockScope)");
        }

     public void endVisit(WhileStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (WhileStatement,BlockScope)");

          java_parser.cactionWhileStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving endVisit (,BlockScope)");
        }

     public void endVisit(Wildcard  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (Wildcard,BlockScope)");
        }

     public void endVisit(Wildcard  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of endVisit (Wildcard,ClassScope)");
        }

   }



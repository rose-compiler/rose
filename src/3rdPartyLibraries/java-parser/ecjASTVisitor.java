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


class ecjASTVisitor extends ExtendedASTVisitor
   {
  // This class contains the visitor functions required to support a traversal over the high-level ECJ AST and 
  // call the JNI functions that will execute as C++ functions and construct the ROSE AST.

     final JavaParser java_parser;

     private final JavaSourcePositionInformationFactory posFactory;
     
     //
     // Keep track of the set of Catch block arguments.
     // Keep track of the set of Blocks that are Catch blocks.
     //
     HashSet<Argument> catchArguments = new HashSet<Argument>();
     HashSet<Block> catchBlocks = new HashSet<Block>();
     
     //
     // Keep track of Argument(s) that were already processed in a different context.
     //
     HashSet<Argument> processedArguments = new HashSet<Argument>();

     //
     // Keep track of Argument(s) that were already processed in a different context.
     //
     HashSet<TypeDeclaration> TypesWithNoBody = new HashSet<TypeDeclaration>();
     HashMap<ASTNode, TypeDeclaration> TypeHeaderDelimiters = new HashMap<ASTNode, TypeDeclaration>();
     HashSet<AbstractMethodDeclaration> MethodsWithNoBody = new HashSet<AbstractMethodDeclaration>();
     HashMap<ASTNode, AbstractMethodDeclaration> MethodHeaderDelimiters = new HashMap<ASTNode, AbstractMethodDeclaration>();

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
     public boolean preVisit(ASTNode node) {
         if (TypeHeaderDelimiters.containsKey(node)) {
// System.out.println("At the end of a Type Header !");
             TypeDeclaration type = TypeHeaderDelimiters.get(node);
             java_parser.cactionTypeDeclarationHeader(type.superclass != null,
                                                      type.superInterfaces == null ? 0 : type.superInterfaces.length,
                                                      type.typeParameters == null  ? 0 : type.typeParameters.length,
                                                      this.createJavaToken(node));
         }
         else if (MethodHeaderDelimiters.containsKey(node)) {
// System.out.println("At the end of a Method or Constructor Header !");
             // Do Something !!!
         }

         return true;
     }

     public void postVisit(ASTNode node) {
         // Do Something !!!
         return;
     }
     // *************************************************
     public void enterTypeDeclaration(TypeDeclaration node) {
            if (node.memberTypes != null && node.memberTypes.length > 0) {
            	TypeHeaderDelimiters.put(node.memberTypes[0], node);
            }
            else if (node.fields != null && node.fields.length > 0) {
            	TypeHeaderDelimiters.put(node.fields[0], node); 
            }
            else if (node.methods != null && node.methods.length > 0) {
            	TypeHeaderDelimiters.put(node.methods[0], node); 
            }
            else TypesWithNoBody.add(node);
            
	        String typename = new String(node.name);
	        java_parser.cactionTypeDeclaration(typename,
	                                           node.kind(node.modifiers) == TypeDeclaration.INTERFACE_DECL,
	                                           (node.binding != null && node.binding.isAbstract()),
	                                           (node.binding != null && node.binding.isFinal()),
	                                           (node.binding != null && node.binding.isPrivate()),
	                                           (node.binding != null && node.binding.isPublic()),
	                                           (node.binding != null && node.binding.isProtected()),
	                                           (node.binding != null && node.binding.isStatic() && node.binding.isNestedType()),
	                                           (node.binding != null && node.binding.isStrictfp()),
	                                           this.createJavaToken(node));
     }

     public void exitTypeDeclaration(TypeDeclaration node) {
    	 String typename = new String(node.name);
         if (TypesWithNoBody.contains(node)) {
// System.out.println("Type " + typename + " does not have a body!"); 
             java_parser.cactionTypeDeclarationHeader(node.superclass != null,
                                                      node.superInterfaces == null ? 0 : node.superInterfaces.length,
                                                      node.typeParameters == null  ? 0 : node.typeParameters.length,
                                                      this.createJavaToken(node));
         }
//         else {
         java_parser.cactionTypeDeclarationEnd(this.createJavaToken(node));
//         }
   }


  // visitor = new ASTVisitor()
     public boolean enter(AllocationExpression node,BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (AllocationExpression,BlockScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionAllocationExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (AllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(AND_AND_Expression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (AND_AND_Expression,BlockScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionANDANDExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (AND_AND_Expression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(AnnotationMethodDeclaration node,ClassScope classScope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (AnnotationMethodDeclaration,ClassScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionAnnotationMethodDeclaration(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (AnnotationMethodDeclaration,ClassScope)");

          return true; // do nothing by default, keep traversing
        }

     public boolean enter(Argument node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Argument,BlockScope)");

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
       // java_parser.cactionArgument(nameString, node.modifiers, this.createJavaToken(node));
          if (processedArguments.contains(node)) {
              return false;
          }
          else {
              JavaParserSupport.generateType(node.type);
              if (! catchArguments.contains(node)) {
                  java_parser.cactionArgument(nameString, node.modifiers, this.createJavaToken(node));
              }
          }
        
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Argument,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(Argument node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Argument,ClassScope)");

          java_parser.cactionArgumentClassScope("Argument_class_abc", this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Argument,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ArrayAllocationExpression node,BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ArrayAllocationExpression,BlockScope)");

          JavaParserSupport.generateType(node.type);
          java_parser.cactionArrayAllocationExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ArrayAllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ArrayInitializer  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ArrayInitializer,BlockScope)");

          java_parser.cactionArrayInitializer(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ArrayInitializer,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ArrayQualifiedTypeReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ArrayQualifiedTypeReference,BlockScope)");

          java_parser.cactionArrayQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ArrayQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ArrayQualifiedTypeReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ArrayQualifiedTypeReference,ClassScope)");

          java_parser.cactionArrayQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ArrayQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }
     public boolean enter(ArrayReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ArrayReference,BlockScope)");

          java_parser.cactionArrayReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ArrayReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ArrayTypeReference  node, BlockScope scope)
        {
         if (java_parser.verboseLevel > 0)
             System.out.println("Inside of enter (ArrayTypeReference,BlockScope)");

          if (node.resolvedType != null) {
              JavaParserSupport.generateType(node);
          }
          else {
              System.out.println("Sorry, not implemented SingleTypeReference: node.resolvedType == NULL");
          }

          java_parser.cactionArrayTypeReference(new String(node.token) /* node.resolvedType.debugName()*/, node.dimensions(), this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving enter (ArrayTypeReference,BlockScope)");

         return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ArrayTypeReference  node, ClassScope scope)
        {
         if (java_parser.verboseLevel > 0)
             System.out.println("Inside of enter (ArrayTypeReference,ClassScope)");

         if (node.resolvedType != null) {
             JavaParserSupport.generateType(node);
         }
         else {
             System.out.println("Sorry, not implemented SingleTypeReference: node.resolvedType == NULL");
         }

         java_parser.cactionArrayTypeReference(new String(node.token) /* node.resolvedType.debugName()*/, node.dimensions(), this.createJavaToken(node));

         if (java_parser.verboseLevel > 0)
             System.out.println("Leaving enter (ArrayTypeReference, ClassScope)");

        return true; // do nothing by  node, keep traversing
        }

     public boolean enter(AssertStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (AssertStatement,BlockScope)");

          java_parser.cactionAssertStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (AssertStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(Assignment  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Assignment,BlockScope)");

          java_parser.cactionAssignment(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Assignment,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(BinaryExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (BinaryExpression,BlockScope)");

          java_parser.cactionBinaryExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (BinaryExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(Block  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Block,BlockScope)");

          java_parser.cactionBlock(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Block,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(BreakStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (BreakStatement,BlockScope)");

          java_parser.cactionBreakStatement((node.label == null ? "" : new String(node.label)),
                                            this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (BreakStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(CaseStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (CaseStatement,BlockScope)");

          java_parser.cactionCaseStatement(node.constantExpression != null, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (CaseStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(CastExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (CastExpression,BlockScope)");

          java_parser.cactionCastExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (CastExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(CharLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (CharLiteral,BlockScope)");

          java_parser.cactionCharLiteral(node.constant.charValue(), this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (CharLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ClassLiteralAccess  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ClassLiteralAccess,BlockScope)");

          java_parser.cactionClassLiteralAccess(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ClassLiteralAccess,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(Clinit  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Clinit,ClassScope)");

          java_parser.cactionClinit(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Clinit,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter( CompilationUnitDeclaration node, CompilationUnitScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (CompilationUnitDeclaration,CompilationUnitScope)");

       // Except for out here for debugging, the filename string is not used (even on the C++ side of the JNI function.
          String s = new String(node.getFileName());
          if (java_parser.verboseLevel > 0)
             {
               System.out.println("Compiling file = " + s);
             }

       // Ouput some information about the CompilationUnitScope (we don't use the package name currently).
       // DQ (9/11/2011): Static analysis tools suggest using StringBuffer instead of String.
       // String packageReference = "";
          StringBuffer packageReference = new StringBuffer();
          for (int i = 0, tokenArrayLength = scope.currentPackageName.length; i < tokenArrayLength; i++)
             {
               String tokenString = new String(scope.currentPackageName[i]);
               System.out.println("     --- packageReference tokens = " + tokenString);

               if (i > 0)
                  {
                 // packageReference += '.';
                     packageReference.append('.');
                  }

            // packageReference += tokenString;
               packageReference.append(tokenString);
             }

          if (java_parser.verboseLevel > 0)
             System.out.println("Package name = " + packageReference.toString());

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
               System.out.println("Leaving enter (CompilationUnitDeclaration,CompilationUnitScope)");

          return true; // do nothing by default, keep traversing
        }

     public boolean enter(CompoundAssignment  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (CompoundAssignment,BlockScope)");

          java_parser.cactionCompoundAssignment(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (CompoundAssignment,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ConditionalExpression node,BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ConditionalExpression,BlockScope)");

          java_parser.cactionConditionalExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ConditionalExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ConstructorDeclaration node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ConstructorDeclaration,ClassScope)");

       // char [] name = node.selector;
       // System.out.println("Inside of enter (ConstructorDeclaration,ClassScope) method name = " + node.selector);
          String name = new String(node.selector);
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ConstructorDeclaration,ClassScope) method name = " + name);

        //
        //
        //
        if (node.statements != null && node.statements.length > 0) {
             MethodHeaderDelimiters.put(node.statements[0], node);
        }
        else MethodsWithNoBody.add(node);
        
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
                    processedArguments.add(node.arguments[i]);

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
          JavaParser.cactionGenerateType("void", 0);

          if (java_parser.verboseLevel > 2)
               System.out.println("DONE: Push void as a return type for now (will be ignored because this is a constructor)");

          boolean isNative = node.isNative();
          boolean isPrivate = (node.binding != null && !node.binding.isPrivate()) ? true : false;

          boolean isStatic = node.isNative();

          java_parser.cactionConstructorDeclaration(name,
                                                    node.arguments == null ? 0 : node.arguments.length,
                                                    isNative, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ConstructorDeclaration,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ContinueStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ContinueStatement,BlockScope)");

          java_parser.cactionContinueStatement((node.label == null ? "" : new String(node.label)),
                                               this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ContinueStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(DoStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (DoStatement,BlockScope)");

          java_parser.cactionDoStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (DoStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(DoubleLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (DoubleLiteral,BlockScope)");

          java_parser.cactionDoubleLiteral(node.constant.doubleValue(), new String(node.source()), this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (DoubleLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(EmptyStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (EmptyStatement,BlockScope)");

          java_parser.cactionEmptyStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (EmptyStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(EqualExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (EqualExpression,BlockScope)");

          java_parser.cactionEqualExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (EqualExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ExplicitConstructorCall node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ExplicitConstructorCall,BlockScope)");

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
               String name = "super";
               if (node.accessMode == ExplicitConstructorCall.ImplicitSuper)
                  {
                 // System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: implicit super()");
                 // java_parser.cactionExplicitConstructorCall("ImplicitSuper");

                 // We have to implement the support for an impleicit super() call, even if it is marked explicitly 
                 // as implicit and not generated in the output source code.  It should still be explicit in the AST
                 // as a way to simplify analysis.
                 // String name = new String("super");
                 // System.out.println("super function name = " + name);

                    String associatedClassName = node.binding.toString();
                 // System.out.println("super function associatedClassName = " + associatedClassName);

                    java_parser.cactionMessageSend(name,associatedClassName, this.createJavaToken(node));
                  }
                 else
                  {
                    if (node.accessMode == ExplicitConstructorCall.Super)
                       {
                      // System.out.println("Sorry, not implemented in support for ExplicitConstructorCall: super()");
                      // java_parser.cactionExplicitConstructorCall("super");

                      // String name = new String("super");
                      // String name = "super";

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
               System.out.println("Leaving enter (ExplicitConstructorCall,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ExtendedStringLiteral node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ExtendedStringLiteral,BlockScope)");

          java_parser.cactionExtendedStringLiteral(node.constant.stringValue(), this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ExtendedStringLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(FalseLiteral  node, BlockScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (FalseLiteral,BlockScope)");

          java_parser.cactionFalseLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (FalseLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(FieldDeclaration node, MethodScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (FieldDeclaration,BlockScope)");
/*
       // DQ (9/5/2011): Old code from before this was processed bottom up.

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

       // String selectorName = new String5      // System.out.println("node.name = " + selectorName);
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
*/
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (FieldDeclaration,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

          public boolean enter(FieldReference node, BlockScope scope) 
             {
            // System.out.println("Sorry, not implemented in support for FieldReference(BlockScope): xxx");
               if (java_parser.verboseLevel > 0)
                    System.out.println("Inside of enter (FieldReference,BlockScope)");

               String fieldRefName = new String(node.token);
               
               java_parser.cactionFieldReference(fieldRefName, this.createJavaToken(node));

               if (java_parser.verboseLevel > 0)
                    System.out.println("Leaving enter (FieldReference,BlockScope)");

               return true; // do nothing by  node, keep traversing
             }

          public boolean enter(FieldReference  node, ClassScope scope)
             {
            // System.out.println("Sorry, not implemented in support for FieldReference(ClassScope): xxx");
               if (java_parser.verboseLevel > 0)
                    System.out.println("Inside of enter (FieldReference,ClassScope)");


               String fieldRefName = new String(node.token);

               java_parser.cactionFieldReferenceClassScope(fieldRefName, this.createJavaToken(node));

               if (java_parser.verboseLevel > 0)
                    System.out.println("Leaving enter (FieldReference,ClassScope)");

               return true; // do nothing by  node, keep traversing
             }

     public boolean enter(FloatLiteral  node, BlockScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (FloatLiteral,BlockScope)");

          java_parser.cactionFloatLiteral(node.constant.floatValue(), new String(node.source()), this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (FloatLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ForeachStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ForeachStatement,BlockScope)");

          java_parser.cactionForeachStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ForeachStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ForStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ForStatement,BlockScope)");

          java_parser.cactionForStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ForStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(IfStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (IfStatement,BlockScope)");

          java_parser.cactionIfStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (IfStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ImportReference  node, CompilationUnitScope scope)
        {
       // This is the support for the Java "import" statement (declaration).
       // We can basically strify this step, since this is frontend handling that
       // has no analysis requirements (that I know of).  We could translate
       // the list of class names to a list of declarations, this might be useful
       // at a later point. However, this would require post processing since
       // we have not yet read in the classes that would be a part of those
       // implicitly read as a consequence of the "import" statements.

          if (java_parser.verboseLevel > 1)
               System.out.println("Inside of enter (ImportReference,CompilationUnitScope)");

       // DQ (9/11/2011): Static analysis tools suggest using StringBuffer instead of String.
       // String importReference = "";
          StringBuffer importReference = new StringBuffer();
          for (int i = 0, tokenArrayLength = node.tokens.length; i < tokenArrayLength; i++)
             {
               String tokenString = new String(node.tokens[i]);
            // System.out.println("     --- ImportReference tokens = " + tokenString);

               if (i > 0)
                  {
                    importReference.append('.');
                  }

               importReference.append(tokenString);
             }

          boolean containsWildcard = ((node.bits & node.OnDemand) != 0);
          String importReferenceWithoutWildcard = importReference.toString();
          if (containsWildcard)
             {
               importReference.append(".*");
             }

          if (java_parser.verboseLevel > 1)
             System.out.println("importReference (string) = " + importReference.toString());

       // DQ (8/22/2011): Read the referenced class or set of classes defined by the import statement.
       // System.out.println("In visit (ImportReference,CompilationUnitScope): Calling buildImplicitClassSupport() to recursively build the class structure with member declarations: name = " + importReferenceWithoutWildcard);
          if (! containsWildcard) { // Build support for a real class
              JavaParserSupport.buildImplicitClassSupport(importReferenceWithoutWildcard);
          }
          else { // just prime the system for an "import ... .*;"
	      //              JavaParser.cactionLookupClassType(importReferenceWithoutWildcard);
              java_parser.cactionBuildImplicitClassSupportStart(importReferenceWithoutWildcard);
              java_parser.cactionBuildImplicitClassSupportEnd(0, importReferenceWithoutWildcard);
          }

       // System.out.println("DONE: In visit (ImportReference,CompilationUnitScope): Calling buildImplicitClassSupport() to recursively build the class structure with member declarations: name = " + importReferenceWithoutWildcard);

       // DQ (4/15/2011): I could not get the passing of a boolean to work, so I am just passing an integer.
          java_parser.cactionImportReference(importReferenceWithoutWildcard, containsWildcard, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ImportReference,CompilationUnitScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(Initializer  node, MethodScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Initializer,MethodScope)");

          java_parser.cactionInitializer(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Initializer,MethodScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter( InstanceOfExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (InstanceOfExpression,BlockScope)");

          java_parser.cactionInstanceOfExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (InstanceOfExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(IntLiteral node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
             System.out.println("Inside of enter (IntLiteral,BlockScope) value = " + node.toString());

          java_parser.cactionIntLiteral(node.constant.intValue(), new String(node.source()), this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (IntLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(Javadoc  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Javadoc,BlockScope)");

          java_parser.cactionJavadoc(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Javadoc,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(Javadoc  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Javadoc,ClassScope)");

          java_parser.cactionJavadocClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Javadoc,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocAllocationExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocAllocationExpression,BlockScope)");

          java_parser.cactionJavadocAllocationExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocAllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocAllocationExpression  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocAllocationExpression,ClassScope)");

          java_parser.cactionJavadocAllocationExpressionClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocAllocationExpression,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocArgumentExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter JavadocArgumentExpression(,BlockScope)");

          java_parser.cactionJavadocArgumentExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocArgumentExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocArgumentExpression  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocArgumentExpression,ClassScope)");

          java_parser.cactionJavadocArgumentExpressionClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocArgumentExpression,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocArrayQualifiedTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocArrayQualifiedTypeReference,BlockScope)");

          java_parser.cactionJavadocArrayQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocArrayQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocArrayQualifiedTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocArrayQualifiedTypeReference,ClassScope)");

          java_parser.cactionJavadocArrayQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocArrayQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocArraySingleTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocArraySingleTypeReference,BlockScope)");

          java_parser.cactionJavadocArraySingleTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocArraySingleTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocArraySingleTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocArraySingleTypeReference,ClassScope)");

          java_parser.cactionJavadocArraySingleTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocArraySingleTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocFieldReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocFieldReference,BlockScope)");

          java_parser.cactionJavadocFieldReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocFieldReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocFieldReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocFieldReference,ClassScope)");

          java_parser.cactionJavadocFieldReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocFieldReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocImplicitTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocImplicitTypeReference,BlockScope)");

          java_parser.cactionJavadocImplicitTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocImplicitTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocImplicitTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocImplicitTypeReference,ClassScope)");

          java_parser.cactionJavadocImplicitTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocImplicitTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocMessageSend  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocMessageSend,BlockScope)");

          java_parser.cactionJavadocMessageSend(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocMessageSend,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocMessageSend  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocMessageSend,ClassScope)");

          java_parser.cactionJavadocMessageSendClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocMessageSend,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocQualifiedTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocQualifiedTypeReference,BlockScope)");

          java_parser.cactionJavadocQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocQualifiedTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocQualifiedTypeReference,ClassScope)");

          java_parser.cactionJavadocQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocReturnStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocReturnStatement,BlockScope)");

          java_parser.cactionJavadocReturnStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocReturnStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocReturnStatement  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocReturnStatement,ClassScope)");

          java_parser.cactionJavadocReturnStatementClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocReturnStatement,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocSingleNameReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocSingleNameReference,BlockScope)");

          java_parser.cactionJavadocSingleNameReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocSingleNameReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocSingleNameReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocSingleNameReference,ClassScope)");

          java_parser.cactionJavadocSingleNameReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocSingleNameReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocSingleTypeReference  node, BlockScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocSingleTypeReference,BlockScope)");

          java_parser.cactionJavadocSingleTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocSingleTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(JavadocSingleTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (JavadocSingleTypeReference,ClassScope)");

          java_parser.cactionJavadocSingleTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (JavadocSingleTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(LabeledStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (LabeledStatement,BlockScope)");

          java_parser.cactionLabeledStatement(new String(node.label), this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (LabeledStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(LocalDeclaration  node, BlockScope scope)
        {
       // LocalDeclarations is derived from AbstractVariableDeclaration

          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (LocalDeclaration,BlockScope)");

/*
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

       // Construct the type (will be constructed on the astJavaTypeStack).

       // DQ (7/18/2011): Switch to using the different generateType() function (taking a TypeReference).
       // JavaParserSupport.generateType(node.binding.type);
          JavaParserSupport.generateType(node.type);

          boolean isFinal = node.binding.isFinal();

       // DQ (8/13/2011): This information is stored in the FieldReference...(not clear how to get it).
       // boolean isPrivate = (node.binding != null && !node.binding.isPrivate()) ? true : false;

       // Build the variable declaration using the type from the astJavaTypeStack.
       // Note that this may have to handle an array of names or be even more complex in the future.
          java_parser.cactionLocalDeclaration(name,isFinal, this.createJavaToken(node));
*/
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (LocalDeclaration,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(LongLiteral node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (LongLiteral,BlockScope)");

          java_parser.cactionLongLiteral(node.constant.longValue(), new String(node.source()), this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (LongLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(MarkerAnnotation  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (MarkerAnnotation,BlockScope)");

          java_parser.cactionMarkerAnnotation(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (MarkerAnnotation,BlockScope)");

          return true;
        }

     public boolean enter(MemberValuePair  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (MemberValuePair,BlockScope)");

          java_parser.cactionMemberValuePair(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (MemberValuePair,BlockScope)");

          return true;
        }

     public boolean enter(MessageSend  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (MessageSend,BlockScope)");

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
//System.out.println(node.receiver instanceof ThisReference ? "The receiver is a \"this\"" : "The receiver is NOT a \"this\" !!!");

          java_parser.cactionMessageSend(name,associatedClassName, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (MessageSend,BlockScope)");

       // System.out.println("Exiting as a test in visit (MessageSend,BlockScope)...");
       // System.exit(1);

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(MethodDeclaration  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (MethodDeclaration,ClassScope)");

          //
          //
          //
          if (node.statements != null && node.statements.length > 0) {
               MethodHeaderDelimiters.put(node.statements[0], node);
          }
          else MethodsWithNoBody.add(node);

          // char [] name = node.selector;
       // System.out.println("Inside of enter (MethodDeclaration,ClassScope) method name = " + name);
          String name = new String(node.selector);
          if (java_parser.verboseLevel > 0)
             {
               System.out.println("Inside of enter (MethodDeclaration,ClassScope) method name = " + name);

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
                    processedArguments.add(node.arguments[i]);

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
/*
          if (node.returnType == null)
             {
               System.out.println("Error: no return type defined for the member function node.returnType = " + node.returnType);
               System.exit(1);
             }
*/
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
          java_parser.cactionMethodDeclaration(name,
        		                               isAbstract,isNative,isStatic,isFinal,isSynchronized,isPublic,isProtected,isPrivate,isStrictfp, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (MethodDeclaration,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter( StringLiteralConcatenation node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (StringLiteralConcatenation,BlockScope)");

          java_parser.cactionStringLiteralConcatenation(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (StringLiteralConcatenation,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(NormalAnnotation  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (NormalAnnotation,BlockScope)");

          java_parser.cactionNormalAnnotation(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (NormalAnnotation,BlockScope)");

          return true;
        }

     public boolean enter(NullLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (NullLiteral,BlockScope)");

          java_parser.cactionNullLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (NullLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(OR_OR_Expression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (OR_OR_Expression,BlockScope)");

          java_parser.cactionORORExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (OR_OR_Expression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ParameterizedQualifiedTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ParameterizedQualifiedTypeReference,BlockScope)");

          java_parser.cactionParameterizedQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ParameterizedQualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ParameterizedQualifiedTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ParameterizedQualifiedTypeReference,ClassScope)");

          java_parser.cactionParameterizedQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ParameterizedQualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ParameterizedSingleTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ParameterizedSingleTypeReference,BlockScope)");

          java_parser.cactionParameterizedSingleTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ParameterizedSingleTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ParameterizedSingleTypeReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ParameterizedSingleTypeReference,ClassScope)");

          java_parser.cactionParameterizedSingleTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ParameterizedSingleTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(PostfixExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (PostfixExpression,BlockScope)");

          java_parser.cactionPostfixExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (PostfixExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(PrefixExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (PrefixExpression,BlockScope)");

          java_parser.cactionPrefixExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (PrefixExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(QualifiedAllocationExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (QualifiedAllocationExpression,BlockScope)");

          java_parser.cactionQualifiedAllocationExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (QualifiedAllocationExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(QualifiedNameReference node, BlockScope scope)
        {
       // Note that if we want to traverse this object in more detail than the QualifiedNameReference::traverse() functions can/should be modified.
          String components[] = new String[node.tokens.length];
          StringBuffer strbuf = new StringBuffer();
          int typePrefixLength = node.indexOfFirstFieldBinding - 1;
          for (int i = 0; i < typePrefixLength; i++) {
              components[i] = new String(node.tokens[i]);
              strbuf.append(components[i]);
              if (i + 1 < typePrefixLength)
                  strbuf.append(".");
          }

          String type_prefix = new String(strbuf);
          String package_name = new String(node.actualReceiverType.qualifiedPackageName());
          String type_name = new String(node.actualReceiverType.qualifiedSourceName());
          if (typePrefixLength > 0) { // is there an type prefix?
              String full_type_name = (package_name.length() > 0
            		                      ? new String(node.actualReceiverType.qualifiedPackageName()) + "."
            		                      : "")
            		                  + type_name;
              JavaParserSupport.buildImplicitClassSupport(full_type_name);
          }
          
          strbuf = new StringBuffer();
          for (int i = typePrefixLength; i < node.tokens.length; i++) {
               components[i] = new String(node.tokens[i]);
               strbuf.append(components[i]);
               if (i + 1 < node.tokens.length)
                   strbuf.append(".");
          }
          String name = new String(strbuf);

          java_parser.cactionQualifiedNameReference(package_name, type_prefix, type_name, name, this.createJavaToken(node));
/*
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
*/
          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(QualifiedNameReference node, ClassScope scope)
        {
       // This is a reference to a data member.
          System.out.println("Sorry, not implemented in support for QualifiedNameReference(ClassScope): data member");

          // Note that if we want to traverse this object in more detail then the QualifiedNameReference::traverse() functions can/should be modified.
          String components[] = new String[node.tokens.length];
          StringBuffer strbuf = new StringBuffer();
          int typePrefixLength = node.indexOfFirstFieldBinding - 1;

          for (int i = 0; i < typePrefixLength; i++) {
              components[i] = new String(node.tokens[i]);
              strbuf.append(components[i]);
              if (i + 1 < typePrefixLength)
                  strbuf.append(".");
          }

          String type_prefix = new String(strbuf);

          String package_name = new String(node.actualReceiverType.qualifiedPackageName());
          String type_name = new String(node.actualReceiverType.qualifiedSourceName());
          if (typePrefixLength > 0) { // is there an type prefix?
              String full_type_name = (package_name.length() > 0
            		                      ? new String(node.actualReceiverType.qualifiedPackageName()) + "."
            		                      : "")
            		                  + type_name;
              JavaParserSupport.buildImplicitClassSupport(full_type_name);
          }
          
          strbuf = new StringBuffer();
          for (int i = typePrefixLength; i < node.tokens.length; i++) {
               components[i] = new String(node.tokens[i]);
               strbuf.append(components[i]);
               if (i + 1 < node.tokens.length)
                   strbuf.append(".");
          }
          String name = new String(strbuf);

          java_parser.cactionQualifiedNameReference(package_name, type_prefix, type_name, name, this.createJavaToken(node));


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

     public boolean enter(QualifiedSuperReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (QualifiedSuperReference,BlockScope)");

          java_parser.cactionQualifiedSuperReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (QualifiedSuperReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter( QualifiedSuperReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (QualifiedSuperReference,ClassScope)");

          java_parser.cactionQualifiedSuperReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (QualifiedSuperReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(QualifiedThisReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (QualifiedThisReference,BlockScope)");

          java_parser.cactionQualifiedThisReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (QualifiedThisReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter( QualifiedThisReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (QualifiedThisReference,ClassScope)");

          java_parser.cactionQualifiedThisReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (QualifiedThisReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(QualifiedTypeReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (QualifiedTypeReference,BlockScope)");

          java_parser.cactionQualifiedTypeReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (QualifiedTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(QualifiedTypeReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (QualifiedTypeReference,ClassScope)");

          java_parser.cactionQualifiedTypeReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (QualifiedTypeReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ReturnStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ReturnStatement,BlockScope)");

          java_parser.cactionReturnStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ReturnStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(SingleMemberAnnotation  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (SingleMemberAnnotation,BlockScope)");

          java_parser.cactionSingleMemberAnnotation(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (SingleMemberAnnotation,BlockScope)");

          return true;
        }

     public boolean enter(SingleNameReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (SingleNameReference,BlockScope)");

          String varRefName = node.toString();
          if (java_parser.verboseLevel > 0)
             {
               System.out.println("Building a variable reference for name = " + varRefName);

               System.out.println("node.genericCast = " + node.genericCast);
             }

          java_parser.cactionSingleNameReference(varRefName, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (SingleNameReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(SingleNameReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (SingleNameReference,ClassScope)");

          java_parser.cactionSingleNameReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (SingleNameReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(SingleTypeReference node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
              System.out.println("Inside of enter (SingleTypeReference,BlockScope)");

          if (node.resolvedType != null) {
              JavaParserSupport.generateType(node);
          }
          else {
              System.out.println("Sorry, not implemented SingleTypeReference: node.resolvedType == NULL");
          }

          java_parser.cactionSingleTypeReference(node.resolvedType.debugName(), this.createJavaToken(node));
          
          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving enter (SingleTypeReference,BlockScope)");

       // java_parser.cactionSingleTypeReference("abc");

/*
          Moved to the exit() function.

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

     public boolean enter(SingleTypeReference node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
              System.out.println("Inside of enter (SingleTypeReference,BlockScope)");
         
          if (node.resolvedType != null) {
              JavaParserSupport.generateType(node);
          }
          else {
              System.out.println("Sorry, not implemented SingleTypeReference: node.resolvedType == NULL");
          }

          java_parser.cactionSingleTypeReference(node.resolvedType.debugName(), this.createJavaToken(node));
          
          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving enter (SingleTypeReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(StringLiteral  node, BlockScope scope)
        {
         if (java_parser.verboseLevel > 0)
             System.out.println("Inside of enter (StringLiteral,BlockScope)");

       // System.out.println("Inside of enter (StringLiteral,BlockScope): node = " + node);
          String literal = new String(node.source());
          System.out.println("Inside of enter (StringLiteral,BlockScope): node = " + literal);

          java_parser.cactionStringLiteral(literal, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving enter (StringLiteral,BlockScope)");

       // System.out.println("Exiting at base of visit (StringLiteral,BlockScope)");
       // System.exit(1);

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(SuperReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (SuperReference,BlockScope)");

          java_parser.cactionSuperReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (SuperReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(SwitchStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (SwitchStatement,BlockScope)");

          java_parser.cactionSwitchStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (SwitchStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(SynchronizedStatement node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (SynchronizedStatement,BlockScope)");

          java_parser.cactionSynchronizedStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (SynchronizedStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ThisReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ThisReference,BlockScope)");

          java_parser.cactionThisReference(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ThisReference,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ThisReference  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ThisReference,ClassScope)");

          java_parser.cactionThisReferenceClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ThisReference,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(ThrowStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (ThrowStatement,BlockScope)");

          java_parser.cactionThrowStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (ThrowStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(TrueLiteral  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (TrueLiteral,BlockScope)");

          java_parser.cactionTrueLiteral(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (TrueLiteral,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(TryStatement node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (TryStatement,BlockScope)");

          //
          // charles4 (09/26/2011): Keep track of the set of Arguments and the set of Blocks that 
          // make up the catch blocks. 
          //
          if (node.catchArguments != null) {
              for (int i = 0; i < node.catchBlocks.length; i++) {
                  catchArguments.add(node.catchArguments[i]);
                  catchBlocks.add(node.catchBlocks[i]);
              }
          }
          
          java_parser.cactionTryStatement(node.catchArguments == null ? 0 : node.catchBlocks.length, node.finallyBlock != null, this.createJavaToken(node));
          
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (TryStatement,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

    public boolean enter(TypeDeclaration node, BlockScope scope) {
        if (java_parser.verboseLevel > 0)
            System.out.println("visit TypeDeclaration -- BlockScope");

        enterTypeDeclaration(node);
        
        if (java_parser.verboseLevel > 0)
            System.out.println("Leaving enter(TypeDeclaration,BlockScope)");

        return true; // do nothing by  node, keep traversing
    }

    public boolean enter(TypeDeclaration node, ClassScope scope) {
        if (java_parser.verboseLevel > 0)
            System.out.println("visit TypeDeclaration -- ClassScope");

        enterTypeDeclaration(node);

        if (java_parser.verboseLevel > 0)
            System.out.println("Leaving enter(TypeDeclaration,ClassScope)");

        return true; // do nothing by  node, keep traversing
    }

    public boolean enter(TypeDeclaration node, CompilationUnitScope scope) {
       // System.out.println("visit TypeDeclaration -- CompilationUnitScope");
        if (java_parser.verboseLevel > 0)
            System.out.println("Inside of enter(TypeDeclaration, CompilationUnitScope)");

        enterTypeDeclaration(node);

        if (java_parser.verboseLevel > 0)
            System.out.println("Leaving enter(TypeDeclaration, CompilationUnitScope)");

        return true; // do nothing by  node, keep traversing
    }

     public boolean enter(TypeParameter  node, BlockScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (TypeParameter,BlockScope)");

          java_parser.cactionTypeParameter(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (TypeParameter,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(TypeParameter  node, ClassScope scope) {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (TypeParameter,ClassScope)");

          java_parser.cactionTypeParameterClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (TypeParameter,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(UnaryExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (UnaryExpression,BlockScope)");

          java_parser.cactionUnaryExpression(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (UnaryExpression,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(WhileStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (WhileStatement,BlockScope)");

          java_parser.cactionWhileStatement(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(Wildcard  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Wildcard,BlockScope)");

          java_parser.cactionWildcard(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Wildcard,BlockScope)");

          return true; // do nothing by  node, keep traversing
        }

     public boolean enter(Wildcard  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of enter (Wildcard,ClassScope)");

          java_parser.cactionWildcardClassScope(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving enter (Wildcard,ClassScope)");

          return true; // do nothing by  node, keep traversing
        }


  // DQ (3/18/2011): Not clear if I need this yet.
  // DQ (3/25/2011): I think we do need them.
     public void exit(AllocationExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (AllocationExpression,BlockScope)");

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
          java_parser.cactionAllocationExpressionEnd(nameString,
        		                                     node.arguments == null ? 0 : node.arguments.length,
        				                             this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (AllocationExpression,BlockScope)");

        }

     public void exit(AND_AND_Expression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (AND_AND_Expression,BlockScope)");

       // Call the Java side of the JNI function.
          java_parser.cactionANDANDExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving exit (AND_AND_Expression,BlockScope)");

        }

     public void exit(AnnotationMethodDeclaration node, ClassScope classScope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (AnnotationMethodDeclaration,ClassScope)");
        }

     public void exit(Argument node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (Argument,BlockScope)");

          if (! processedArguments.contains(node)) { // If argument was already processed, skip it!
              String nameString = new String(node.name);
          
              java_parser.cactionArgumentEnd(nameString, node.modifiers, this.createJavaToken(node));

              //
              // charles4 (09/26/2011): If this Argument declaration belongs to a Catch statement,
              // initiate processing of the Catch statement.
              //
              if (catchArguments.contains(node)) {
                  java_parser.cactionCatchBlock(this.createJavaToken(node)); // Set up Catch Block environment
              }
          }
          
          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving exit (Argument,BlockScope)");

        }

     public void exit(Argument node,ClassScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (Argument,ClassScope)");
        }

     public void exit(ArrayAllocationExpression node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ArrayAllocationExpression,BlockScope)");

          java_parser.cactionArrayAllocationExpressionEnd(node.type.toString(),
        		                                          node.dimensions == null ? 0 : node.dimensions.length,
        		                                          node.initializer != null,
        		                                          this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving exit (ArrayAllocationExpression,BlockScope)");
        }

     public void exit(ArrayInitializer  node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ArrayInitializer, BlockScope)");
          
          java_parser.cactionArrayInitializerEnd(node.expressions == null ? 0 : node.expressions.length,
                                                 this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving exit (ArrayInitializer, BlockScope)");
        }

     public void exit(ArrayQualifiedTypeReference node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ArrayQualifiedTypeReference,BlockScope)");
        }

     public void exit(ArrayQualifiedTypeReference node, ClassScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ArrayQualifiedTypeReference,ClassScope)");
        }

     public void exit(ArrayReference  node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ArrayReference,BlockScope)");

         java_parser.cactionArrayReferenceEnd(this.createJavaToken(node));

         if (java_parser.verboseLevel > 0)
              System.out.println("Leaving exit (ArrayReference,BlockScope)");
        }

     public void exit(ArrayTypeReference  node, BlockScope scope)
        {
         if (java_parser.verboseLevel > 0)
             System.out.println("Inside of exit (ArrayTypeReference,BlockScope)");
/*
          String type_name = new String(node.token);

          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (ArrayTypeReference,BlockScope): type name = " + type_name);

       // This will generate the base type
          JavaParserSupport.generateType(node);

       // This will use the base type from the stack and implement the array type.
       // TODO: Currently no dimenion information is passed, this should likely be fixed.
       //       Also, the name of the base type is passed, but not used except for debugging.
          java_parser.cactionArrayTypeReferenceEnd(type_name, this.createJavaToken(node));
*/
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ArrayTypeReference,BlockScope)");
        }

     public void exit(ArrayTypeReference  node, ClassScope scope)
        {
         if (java_parser.verboseLevel > 0)
             System.out.println("Inside of exit (ArrayTypeReference,ClassScope)");
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ArrayTypeReference,ClassScope)");
        }

     public void exit(AssertStatement  node, BlockScope scope)
        {
       // do nothing by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (AssertStatement,BlockScope)");

          java_parser.cactionAssertStatementEnd(node.exceptionArgument != null, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (AssertStatement,BlockScope)");
        }

     public void exit(Assignment  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (Assignment,BlockScope)");

          java_parser.cactionAssignmentEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving enter (Assignment,BlockScope)");
        }


     public void exit(BinaryExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (BinaryExpression,BlockScope)");

          int operatorKind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT;

          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (BinaryExpression,BlockScope): operatorKind = " + operatorKind);

          java_parser.cactionBinaryExpressionEnd(operatorKind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (BinaryExpression,BlockScope)");

        }

     public void exit(Block  node, BlockScope scope) 
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (Block,BlockScope)");

          if (java_parser.verboseLevel > 1)
               System.out.println("node.explicitDeclarations = " + node.explicitDeclarations);

          int numberOfStatements = 0;
          if (node.statements != null)
               numberOfStatements = node.statements.length;

          if (java_parser.verboseLevel > 0)
               System.out.println("numberOfStatements = " + numberOfStatements);

       // DQ (9/30/2011): We need to pass the number of statments so that we can pop 
       // a precise number of statements off of the stack (and not the whole stack).
          java_parser.cactionBlockEnd(numberOfStatements, this.createJavaToken(node));
          //
          // charles4 (09/26/2011): If this block belongs to a Catch statement,
          // close the catch statement.
          //
          if (catchBlocks.contains(node)) {
              java_parser.cactionCatchBlockEnd(this.createJavaToken(node));
          }

          if (java_parser.verboseLevel > 0)
              System.out.println("Leaving exit (Block,BlockScope)");
        }

     public void exit(BreakStatement  node, BlockScope scope)
        {
       // do nothing  by default
         if (java_parser.verboseLevel > 0)
             System.out.println("Leaving exit (BreakStatement,BlockScope)");
        }

     public void exit(CaseStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (CaseStatement,BlockScope)");

          java_parser.cactionCaseStatementEnd(node.constantExpression != null, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (CaseStatement,BlockScope)");
        }

     public void exit(CastExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (CastExpression,BlockScope)");

          java_parser.cactionCastExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (CastExpression,BlockScope)");
        }

     public void exit(CharLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (CharLiteral,BlockScope)");
        }

     public void exit(ClassLiteralAccess  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ClassLiteralAccess,BlockScope)");
        }

     public void exit(Clinit  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (Clinit,BlockScope)");
        }

     public void exit(CompilationUnitDeclaration node, CompilationUnitScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (CompilationUnitDeclaration,CompilationUnitScope)");

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
               System.out.println("Leaving exit (CompilationUnitDeclaration,CompilationUnitScope)");
        }

     public void exit(CompoundAssignment  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (CompoundAssignment,BlockScope)");

          if (java_parser.verboseLevel > 0)
             System.out.println("Inside of exit (CompoundAssignment,BlockScope): operator_kind" + node.toString());

          int operator_kind = node.operator;

          java_parser.cactionCompoundAssignmentEnd(operator_kind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (CompoundAssignment,BlockScope)");
        }

     public void exit(ConditionalExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (ConditionalExpression,BlockScope)");

          java_parser.cactionConditionalExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ConditionalExpression,BlockScope)");
        }

     public void exit(ConstructorDeclaration node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (ConstructorDeclaration,ClassScope)");

       // DQ (7/31/2011): Added more precise handling of statements to be collected from the statement stack.
          int numberOfStatements = 0;
          if (node.statements != null)
             {
               numberOfStatements = node.statements.length;

               if (java_parser.verboseLevel > 0)
                    System.out.println("Inside of exit (ConstructorDeclaration,ClassScope): numberOfStatements = " + numberOfStatements);
             }

       // DQ (7/31/2011): I don't know if there is just one of these (super()) or if there could be many.
          if (node.constructorCall != null)
             {
               numberOfStatements++;
            // System.out.println("Inside of exit (ConstructorDeclaration,ClassScope): increment the numberOfStatements = " + numberOfStatements);
             }
          
          if (MethodsWithNoBody.contains(node)) {
// System.out.println("Constructor < body");      	  
          }
//        else
          java_parser.cactionConstructorDeclarationEnd(node.arguments == null ? 0 : node.arguments.length,
                                                       numberOfStatements,
                                                       this.createJavaToken(node));
//        }
        }

     public void exit(ContinueStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ContinueStatement,BlockScope)");
        }

     public void exit(DoStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (DoStatement,BlockScope)");

          java_parser.cactionDoStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (DoStatement,BlockScope)");
        }

     public void exit(DoubleLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (DoubleLiteral,BlockScope)");
        }

     public void exit(EmptyStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (EmptyStatement,BlockScope)");

          java_parser.cactionEmptyStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (EmptyStatement,BlockScope)");
        }

     public void exit(EqualExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (EqualExpression,BlockScope)");

          int operator_kind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT; // EQUAL_EQUAL or NOT_EQUAL

          java_parser.cactionEqualExpressionEnd(operator_kind, this.createJavaToken(node));
        }

     public void exit(ExplicitConstructorCall node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (ExplicitConstructorCall,BlockScope)");

          java_parser.cactionExplicitConstructorCallEnd("abc", this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ExplicitConstructorCall,BlockScope)");
        }

     public void exit(ExtendedStringLiteral node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ExtendedStringLiteral,BlockScope)");
        }

     public void exit(FalseLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (FalseLiteral,BlockScope)");
        }

     public void exit(FieldDeclaration  node, MethodScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (FieldDeclaration,BlockScope)");
/*
       // DQ (9/5/2011): Old code from before this was processed bottom up.

       // We pass the name to support debugging...
          String name = new String(node.name);

          boolean hasInitializer = (node.initialization != null) ? true : false;

          java_parser.cactionFieldDeclarationEnd(name,hasInitializer, this.createJavaToken(node));
*/

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

       // DQ (9/5/2011): Now that we process this bottom up, we can expect the type be already be on the stack.
       // DQ (7/18/2011): Switch to using the different generateType() function (taking a TypeReference).
       // JavaParserSupport.generateType(node.binding.type);
       // JavaParserSupport.generateType(node.type);

          boolean isFinal = node.binding.isFinal();

       // DQ (8/13/2011): This information is stored in the FieldReference...(not clear how to get it).
       // boolean isPrivate = (node.binding != null && !node.binding.isPrivate()) ? true : false;

       // Build the variable declaration using the type from the astJavaTypeStack.
       // Note that this may have to handle an array of names or be even more complex in the future.
       // java_parser.cactionLocalDeclaration(name,isFinal);

          java_parser.cactionFieldDeclarationEnd(name,hasInitializer,isFinal,isPrivate,isProtected,isPublic,isVolatile,isSynthetic,isStatic,isTransient, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (FieldDeclaration,BlockScope)");
        }

     public void exit(FieldReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (FieldReference,BlockScope)");

          String fieldRefName = new String(node.token);
          
          java_parser.cactionFieldReferenceEnd(fieldRefName, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (FieldReference,BlockScope)");
        }

     public void exit(FieldReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (FieldReference,ClassScope)");

          String fieldRefName = new String(node.token);
          
          java_parser.cactionFieldReferenceClassScopeEnd(fieldRefName, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (FieldReference,BlockScope)");
        }

     public void exit(FloatLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (FloatLiteral,BlockScope)");
        }

     public void exit(ForeachStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ForeachStatement,BlockScope)");

          java_parser.cactionForeachStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ForeachStatement,BlockScope)");
        }

     public void exit(ForStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (ForStatement,BlockScope)");

          java_parser.cactionForStatementEnd(node.initializations == null ? 0 : node.initializations.length,
                                             node.condition != null,
                                             node.increments == null ? 0 : node.increments.length,
                                             this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ForStatement,BlockScope)");
        }

     public void exit(IfStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (IfStatement,BlockScope)");

          java_parser.cactionIfStatementEnd(node.elseStatement != null, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (IfStatement,BlockScope)");
        }

     public void exit(ImportReference  node, CompilationUnitScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ImportReference,CompilationUnitScope)");
        }

     public void exit(Initializer  node, MethodScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (Initializer,MethodScope)");
        }

     public void exit(InstanceOfExpression node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (InstanceOfExpression,BlockScope)");

          java_parser.cactionInstanceOfExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (InstanceOfExpression,BlockScope)");
        }

     public void exit(IntLiteral node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (IntLiteral,BlockScope)");
        }

     public void exit(Javadoc  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (Javadoc,BlockScope)");
        }

     public void exit(Javadoc  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (Javadoc,ClassScope)");
        }

     public void exit(JavadocAllocationExpression  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocAllocationExpression,BlockScope)");
        }

     public void exit(JavadocAllocationExpression  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocAllocationExpression,ClassScope)");
        }

     public void exit(JavadocArgumentExpression  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocArgumentExpression,BlockScope)");
        }

     public void exit(JavadocArgumentExpression  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocArgumentExpression,ClassScope)");
        }

     public void exit(JavadocArrayQualifiedTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocArrayQualifiedTypeReference,BlockScope)");
        }

     public void exit(JavadocArrayQualifiedTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocArrayQualifiedTypeReference,ClassScope)");
        }

     public void exit(JavadocArraySingleTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocArraySingleTypeReference,BlockScope)");
        }

     public void exit(JavadocArraySingleTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocArraySingleTypeReference,ClassScope)");
        }

     public void exit(JavadocFieldReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocFieldReference,BlockScope)");
        }

     public void exit(JavadocFieldReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocFieldReference,ClassScope)");
        }

     public void exit(JavadocImplicitTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocImplicitTypeReference,BlockScope)");
        }

     public void exit(JavadocImplicitTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocImplicitTypeReference,ClassScope)");
        }

     public void exit(JavadocMessageSend  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocMessageSend,BlockScope)");
        }

     public void exit(JavadocMessageSend  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocMessageSend,ClassScope)");
        }

     public void exit(JavadocQualifiedTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocQualifiedTypeReference,BlockScope)");
        }

     public void exit(JavadocQualifiedTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocQualifiedTypeReference,ClassScope)");
        }

     public void exit(JavadocReturnStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocReturnStatement,BlockScope)");
        }

     public void exit(JavadocReturnStatement  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocReturnStatement,ClassScope)");
        }

     public void exit(JavadocSingleNameReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocSingleNameReference,BlockScope)");
        }

     public void exit(JavadocSingleNameReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocSingleNameReference,ClassScope)");
        }

     public void exit(JavadocSingleTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocSingleTypeReference,BlockScope)");
        }

     public void exit(JavadocSingleTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (JavadocSingleTypeReference,ClassScope)");
        }

     public void exit(LabeledStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (LabeledStatement,BlockScope)");

          java_parser.cactionLabeledStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (LabeledStatement,BlockScope)");
        }

     public void exit(LocalDeclaration  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (LocalDeclaration,BlockScope)");

          if (java_parser.verboseLevel > 0)
               System.out.println("If there is an expression on the stack it is the initializer...");

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

       // Construct the type (it should already be present on the astJavaTypeStack).

       // DQ (7/18/2011): Switch to using the different generateType() function (taking a TypeReference).
       // JavaParserSupport.generateType(node.binding.type);
       // JavaParserSupport.generateType(node.type);

          boolean isFinal = node.binding.isFinal();

       // DQ (8/13/2011): This information is stored in the FieldReference...(not clear how to get it).
       // boolean isPrivate = (node.binding != null && !node.binding.isPrivate()) ? true : false;

       // Build the variable declaration using the type from the astJavaTypeStack.
       // Note that this may have to handle an array of names or be even more complex in the future.
          java_parser.cactionLocalDeclarationEnd(name, node.initialization != null, isFinal, this.createJavaToken(node));
// charles4 10/12/2011: Remove this!
//
//          if (node.initialization != null)
//             {
//               if (java_parser.verboseLevel > 0)
//                    System.out.println("Process the initializer expression...");
//
//               java_parser.cactionLocalDeclarationInitialization(this.createJavaToken(node));
//             }
        }

     public void exit(LongLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (LongLiteral,BlockScope)");
        }

     public void exit(MarkerAnnotation  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (MarkerAnnotation,BlockScope)");
        }

     public void exit(MemberValuePair  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (MemberValuePair,BlockScope)");
        }

     public void exit(MessageSend  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (MessageSend,BlockScope)");

          java_parser.cactionMessageSendEnd(node.typeArguments == null ? 0 : node.typeArguments.length,
                                            node.arguments == null ? 0 : node.arguments.length,
                                            this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (MessageSend,BlockScope)");
        }

     public void exit(MethodDeclaration  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (MethodDeclaration,ClassScope)");

        if (MethodsWithNoBody.contains(node)) {
// System.out.println("Method " + new String(node.selector) + " has no body");      	  
        }
//        else
          java_parser.cactionMethodDeclarationEnd(node.arguments == null  ? 0 : node.arguments.length,
                                                  node.thrownExceptions == null ? 0 : node.thrownExceptions.length,
                		                          node.statements == null ? 0 : node.statements.length,
        		                                  this.createJavaToken(node));
//        }
        }

     public void exit(StringLiteralConcatenation  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (StringLiteralConcatenation,BlockScope)");
        }

     public void exit(NormalAnnotation  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (NormalAnnotation,BlockScope)");
        }

     public void exit(NullLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (NullLiteral,BlockScope)");
        }

     public void exit(OR_OR_Expression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (OR_OR_Expression,BlockScope)");

          java_parser.cactionORORExpressionEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (OR_OR_Expression,BlockScope)");
        }

     public void exit(ParameterizedQualifiedTypeReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ParameterizedQualifiedTypeReference,BlockScope)");
        }

     public void exit(ParameterizedQualifiedTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ParameterizedQualifiedTypeReference,ClassScope)");
        }

     public void exit(ParameterizedSingleTypeReference  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("At top of exit (ParameterizedSingleTypeReference,BlockScope)");

          int numberOfTypeArguments = 0;
          if (node.typeArguments != null)
             {
               numberOfTypeArguments = node.typeArguments.length;
             }

          int numberOfDimensions = node.dimensions;

          if (java_parser.verboseLevel > 0)
               System.out.println("At top of exit (ParameterizedSingleTypeReference,BlockScope) numberOfDimensions = " + numberOfDimensions);

          String name = new String(node.token);

          if (java_parser.verboseLevel > 0)
               System.out.println("At top of exit (ParameterizedSingleTypeReference,BlockScope) name = " + name);

       // We need to find the qualified name for the associated type name (it should be unique).
       // This has to be handled on the Java side...

          String qualifiedTypeName = JavaParserSupport.hashmapOfQualifiedNamesOfClasses.get(name);

          if (java_parser.verboseLevel > 0)
               System.out.println("At top of exit (ParameterizedSingleTypeReference,BlockScope) qualifiedTypeName = " + qualifiedTypeName);

          java_parser.cactionParameterizedSingleTypeReferenceEnd(qualifiedTypeName,numberOfTypeArguments,this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ParameterizedSingleTypeReference,BlockScope)");
        }

     public void exit(ParameterizedSingleTypeReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ParameterizedSingleTypeReference,ClassScope)");
        }

     public void exit(PostfixExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (PostfixExpression,BlockScope)");

          int operator_kind = node.operator;

          java_parser.cactionPostfixExpressionEnd(operator_kind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (PostfixExpression,BlockScope)");
        }

     public void exit(PrefixExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (PrefixExpression,BlockScope)");

          int operator_kind = node.operator;

          java_parser.cactionPrefixExpressionEnd(operator_kind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (PrefixExpression,BlockScope)");
        }

     public void exit(QualifiedAllocationExpression node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (QualifiedAllocationExpression,BlockScope)");
        }

     public void exit(QualifiedNameReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (QualifiedNameReference,BlockScope)");
        }

     public void exit(QualifiedNameReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (QualifiedNameReference,ClassScope)");
        }

     public void exit(QualifiedSuperReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (QualifiedSuperReference,BlockScope)");
        }

     public void exit(QualifiedSuperReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (QualifiedSuperReference,ClassScope)");
        }

     public void exit(QualifiedThisReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (QualifiedThisReference,BlockScope)");
        }

     public void exit(QualifiedThisReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (QualifiedThisReference,ClassScope)");
        }

     public void exit(QualifiedTypeReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (QualifiedTypeReference,BlockScope)");
        }

     public void exit(QualifiedTypeReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (QualifiedTypeReference,ClassScope)");
        }

     public void exit(ReturnStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ReturnStatement,BlockScope)");

          java_parser.cactionReturnStatementEnd(node.expression != null, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ReturnStatement,BlockScope)");
        }

     public void exit(SingleMemberAnnotation  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SingleMemberAnnotation,BlockScope)");
        }

     public void exit(SingleNameReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SingleNameReference,BlockScope)");
        }

     public void exit(SingleNameReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SingleNameReference,ClassScope)");
        }

     public void exit(SingleTypeReference node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SingleTypeReference,BlockScope)");
/*
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
*/
        }

     public void exit(SingleTypeReference node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SingleTypeReference,ClassScope)");
        }

     public void exit(StringLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (StringLiteral,BlockScope)");
        }

     public void exit(SuperReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SuperReference,BlockScope)");
        }

     public void exit(SwitchStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SwitchStatement,BlockScope)");

          java_parser.cactionSwitchStatementEnd(node.caseCount, node.defaultCase != null, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SwitchStatement,BlockScope)");
        }

     public void exit(SynchronizedStatement node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SynchronizedStatement,BlockScope)");

          java_parser.cactionSynchronizedStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (SynchronizedStatement,BlockScope)");
        }

     public void exit(ThisReference  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ThisReference,BlockScope)");
        }

     public void exit(ThisReference  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ThisReference,ClassScope)");
        }

     public void exit(ThrowStatement node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ThrowStatement,BlockScope)");

          java_parser.cactionThrowStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (ThrowStatement,BlockScope)");
}

     public void exit(TrueLiteral  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (TrueLiteral,BlockScope)");
        }

     public void exit(TryStatement  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("exit TryStatement -- BlockScope");

          java_parser.cactionTryStatementEnd(node.catchArguments == null ? 0 : node.catchBlocks.length, node.finallyBlock != null, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (TryStatement,BlockScope)");
        }

     public void exit(TypeDeclaration node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("exit TypeDeclaration -- BlockScope");

          exitTypeDeclaration(node);

          System.out.println("Leaving exit (TypeDeclaration,BlockScope)");
        }

     public void exit(TypeDeclaration node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("exit TypeDeclaration -- ClassScope");

          exitTypeDeclaration(node);

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (TypeDeclaration,ClassScope)");
        }

     public void exit(TypeDeclaration node, CompilationUnitScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (TypeDeclaration,CompilationUnitScope)");

          exitTypeDeclaration(node);

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (TypeDeclaration,CompilationUnitScope)");
        }

     public void exit(TypeParameter  node, BlockScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (TypeParameter,BlockScope)");
        }

     public void exit(TypeParameter  node, ClassScope scope)
        {
       // do nothing  by default
          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (TypeParameter,ClassScope)");
        }

     public void exit(UnaryExpression  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (UnaryExpression,BlockScope)");

       // Not clear what the valueRequired filed means.
          int operator_kind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT;

          java_parser.cactionUnaryExpressionEnd(operator_kind, this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (UnaryExpression,BlockScope)");
        }

     public void exit(WhileStatement  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (WhileStatement,BlockScope)");

          java_parser.cactionWhileStatementEnd(this.createJavaToken(node));

          if (java_parser.verboseLevel > 0)
               System.out.println("Leaving exit (,BlockScope)");
        }

     public void exit(Wildcard  node, BlockScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (Wildcard,BlockScope)");
        }

     public void exit(Wildcard  node, ClassScope scope)
        {
          if (java_parser.verboseLevel > 0)
               System.out.println("Inside of exit (Wildcard,ClassScope)");
        }

   }



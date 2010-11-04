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
  // This is the main class for the connection of the ECJ front-end to ROSE.
  // The design is that we use ECJ mostly unmodified, and use the visitor
  // traversal (ASTVisitor) of the ECJ AST.  Specifically we derive a class 
  // (ecjASTVisitory) from the abstract class (ASTVisitor in ECJ) and define
  // all of it's members so that we can support traversing the ECJ defined 
  // AST and construct the ROSE AST.  Most of the IR nodes used to support this
  // or borrowed from the existing C and C++ support in ROSE.  We will however
  // add any required IR nodes as needed.

  // -------------------------------------------------------------------------------------------
     public native void cactionCompilationUnitList(int argc, String[] argv);

  // These are used in the ecjASTVisitor (which is derived from the ECJ ASTVisitor class).
     public native void cactionCompilationUnitDeclaration(String filename);
     public native void cactionTypeDeclaration(String filename);

  // Need to change the names of the function parameters (should not all be "filename").
     public native void cactionConstructorDeclaration(String filename);
     public native void cactionConstructorDeclarationEnd();
     public native void cactionExplicitConstructorCall(String filename);
     public native void cactionMethodDeclaration(String filename);
     public native void cactionSingleTypeReference(String filename);
     public native void cactionArgument(String filename);
     public native void cactionArrayTypeReference(String filename);
     public native void cactionMessageSend(String filename);
     public native void cactionQualifiedNameReference(String filename);
     public native void cactionStringLiteral(String filename);

  // These are static because they are called from the JavaParserSupport class which 
  // does not have a reference of the JavaParser object..  Not clear if it should.
  // Also it might be that all of the JNI functions should be static.
     public static native void cactionBuildImplicitClassSupportStart(String className);
     public static native void cactionBuildImplicitClassSupportEnd(String className);
     public static native void cactionBuildImplicitMethodSupport(String methodName);
     public static native void cactionBuildImplicitFieldSupport(String fieldName);

  // Save the compilationResult as we process the CompilationUnitDeclaration class.
  // public CompilationResult rose_compilationResult;

  // DQ (10/12/2010): Added boolean value to report error to C++ calling program (similar to OFP).
  // public static boolean hasErrorOccurred = false;

  // DQ: This is the name of the C++ *.so file which has the implementations of the JNI functions.
  // The library with the C++ implementation of these function must be loaded in order to call the functions.
     static { System.loadLibrary("JavaTraversal"); }

  // -------------------------------------------------------------------------------------------

     public void startParsingAST(CompilationUnitDeclaration unit)
        {
          System.out.println("Start parsing");

       // Example of how to call the 
       // traverseAST(unit);

       // Make a copy of the compiation unit so that we can compute source code positions.
       // rose_compilationResult = unit.compilationResult;
          JavaParserSupport.initialize(unit.compilationResult);

          try
             {
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

            // Make sure we exit on any error so it is caught quickly.
            // System.exit(1);
            // throw e;
               return;
             }

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
  /*  public static boolean getError()
        {
          return JavaTraversal.hasErrorOccurred;
        }
  */
  // End of JavaParser class
   }

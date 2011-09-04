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


// DQ (11/1/2010): This improved design separates out the parsing support, from the ECJ AST traversal, and the parser.
class JavaParserSupport
   {
  // DQ (8/20/2011): Added a simple way to control the number of data members, constructors, and member functions built.
  // 0: No significant limits applied to number of constructs in the AST.
  // 1: Limits the number to be built into the enerated AST
     static boolean VISUALIZE_AST = false;

     static int implicitClassCounterBound     = VISUALIZE_AST ? 1   : 1000;
     static int methodCounterBound            = VISUALIZE_AST ? 2   : 1000;
     static int constructorMethodCounterBound = VISUALIZE_AST ? 2   : 1000;
     static int dataMemberCounterBound        = VISUALIZE_AST ? 2   : 1000;
     static int interfaceCounterBound         = VISUALIZE_AST ? 2   : 1000;

  // This class is intended to contain functions to support the JNI specific in src/frontend/ECJ_ROSE_Connection/JavaParserActionROSE.C.
  // Note that the functions in JavaParserActionROSE.C are JNI functions that are called by the Java
  // code in src/3rdPartyLibraries/java-parser/ecjASTVisitor.java, this layout of code is similar to the handling of the Fortran support 
  // in ROSE (except that OFP calls functions representing parser actions while this Java support calls functions representing the
  // translation of the ECJ AST to build the ROSE AST (so the Java support has less to do with parsing than AST translation)).
  // The AST translation is similar to the translation from the EDG AST translation to build the ROSE AST (supporting C and C++).

  // This is used to compute source code positions.
     private static CompilationResult rose_compilationResult;

  // This is used to save a history of what implecit classes have been seen.
     private static Set<Class> setOfClasses;

  // DQ (8/24/2011): This is the support for information on the Java side that 
  // we would know on the C++ side, but require on the Java side to support the 
  // introspection (which requires fully qualified names).
  // We use a hashmap to store fully qualified names associated with class names.
  // This is used to translate class names used in type references into fully
  // qualified names where they are implicit classes and we require introspection 
  // to support reading them to translate their member data dn function into JNI
  // calls that will force the ROSE AST to be built.
  // private static HashMap<String,String> hashmapOfQualifiedNamesOfClasses;
     public static HashMap<String,String> hashmapOfQualifiedNamesOfClasses;

  // Counter for recursive function call...debugging support.
     private static int implicitClassCounter = 0;

     private static int verboseLevel = 0;

  // Initialization function, but be called before we can use member functions in this class.
     public static void initialize(CompilationResult x, int input_verboseLevel)
        {
       // This has to be set first (required to support source position translation).
          rose_compilationResult = x;
          setOfClasses           = new HashSet<Class>();
          implicitClassCounter   = 0;

       // DQ (8/24/2011): Added hashmap to support mapping of unqualified class names to qualified class names.
          hashmapOfQualifiedNamesOfClasses = new HashMap<String,String>();

       // Set the verbose level (passed in from ROSE's "-rose:verbose n")
          verboseLevel = input_verboseLevel;
        }

     public static void sourcePosition(ASTNode node)
        {
       // The source positon (line and comun numbers) can be computed within ECJ. 
       // This is an example of how to do it.

       // We need the CompilationResult which is stored in the CompilationUnit (as I recall).
          assert rose_compilationResult != null : "rose_compilationResult not initialized";

          int startingSourcePosition = node.sourceStart();
          int endingSourcePosition   = node.sourceEnd();

          if (verboseLevel > 2)
               System.out.println("In JavaParserSupport::sourcePosition(ASTNode): start = " + startingSourcePosition + " end = " + endingSourcePosition);

       // Example of how to compute the starting line number and column position of any AST node.
          int problemStartPosition = startingSourcePosition;
          int[] lineEnds;
          int lineNumber   = problemStartPosition >= 0 ? Util.getLineNumber(problemStartPosition, lineEnds = rose_compilationResult.getLineSeparatorPositions(), 0, lineEnds.length-1) : 0;
          int columnNumber = problemStartPosition >= 0 ? Util.searchColumnNumber(rose_compilationResult.getLineSeparatorPositions(), lineNumber, problemStartPosition) : 0;

          if (verboseLevel > 2)
               System.out.println("In JavaParserSupport::sourcePosition(ASTNode): lineNumber = " + lineNumber + " columnNumber = " + columnNumber);

       // Example of how to compute the ending line number and column position of any AST node.
          int problemEndPosition = endingSourcePosition;
          int lineNumber_end   = problemEndPosition >= 0 ? Util.getLineNumber(problemEndPosition, lineEnds = rose_compilationResult.getLineSeparatorPositions(), 0, lineEnds.length-1) : 0;
          int columnNumber_end = problemEndPosition >= 0 ? Util.searchColumnNumber(rose_compilationResult.getLineSeparatorPositions(), lineNumber, problemEndPosition) : 0;

          if (verboseLevel > 2)
               System.out.println("In JavaParserSupport::sourcePosition(ASTNode): lineNumber_end = " + lineNumber_end + " columnNumber_end = " + columnNumber_end);
        }


     public static void processType( Class typeClass)
        {
       // This function processes all the references to types found in data members, function 
       // return types, function argument types, etc.  With each type it is included into a set
       // (if it is not a primative type) and then an SgClassType is generated in the ROSE AST
       // so that all references to this type can be supported.  Note that for trivial input 
       // codes, most of the referenced classes are implicit classes; reflection is used to 
       // traversal all of these and recursively build all types.  This is part of what is required 
       // to support a consistant AST in ROSE.

       // More inforamtion on what is in the Class (and other reflection classes) can be found at:
       //      http://download.oracle.com/javase/6/docs/api/java/lang/Class.html

          String nestedClassName = typeClass.getName();

          if (verboseLevel > 1)
               System.out.println("In processType(): type = " + typeClass);

          if (verboseLevel > 1)
             {
            // This code is part of an interogation of the data in the field and needs to be hidden yet available to support debugging.
            // ******************************************************************************
            // System.out.println("type = " + typeClass);

               if (verboseLevel > 5)
                  {
                    System.out.println("fld.getType().isAnnotation()                 = " + typeClass.isAnnotation());
                 // System.out.println("fld.getType().isAnnotationPresent(Class<? extends Annotation> annotationClass) = " + fld.getType().isAnnotationPresent(fld.getType()));
                    System.out.println("fld.getType().isAnonymousClass()             = " + typeClass.isAnonymousClass());
                    System.out.println("fld.getType().isArray()                      = " + typeClass.isArray());
                 // Not clear what class to use as a test input for isAssignableFrom(Class<?> cls) function...
                    System.out.println("fld.getType().isAssignableFrom(Class<?> cls) = " + typeClass.isAssignableFrom(typeClass));
                    System.out.println("fld.getType().isEnum()                       = " + typeClass.isEnum());
                    System.out.println("fld.getType().isInstance(Object obj)         = " + typeClass.isInstance(typeClass));
                    System.out.println("fld.getType().isInterface()                  = " + typeClass.isInterface());
                    System.out.println("fld.getType().isLocalClass()                 = " + typeClass.isLocalClass());
                    System.out.println("fld.getType().isMemberClass()                = " + typeClass.isMemberClass());
                    System.out.println("fld.getType().isPrimitive()                  = " + typeClass.isPrimitive());
                    System.out.println("fld.getType().isSynthetic()                  = " + typeClass.isSynthetic());
                    System.out.println("-----");
                 // ******************************************************************************
                  }
             }

       // We don't have to support Java primative types as classes in the AST (I think).
          if (typeClass.isPrimitive() == false)
             {
            // Check if this is a type (class) that has already been handled.
               if (setOfClasses.contains(typeClass) == false)
                  {
                 // Investigate any new type.
                    if (typeClass.isArray() == true)
                       {
                      // DQ (4/3/2011): Added support for extracting the base type of an array and recursively processing the base type.

                         if (verboseLevel > 1)
                              System.out.println("Process the base type of the array of type ... base type = " + typeClass.getComponentType());

                         processType(typeClass.getComponentType());

                      // System.out.println("Exiting as a test...");
                      // System.exit(1);
                       }
                      else
                       {
                      // This is not an array type and not a primative type (so it should be a class, I think).
                         if (verboseLevel > 1)
                              System.out.println("Recursive call to buildImplicitClassSupport() to build type = " + nestedClassName);

                      // Add this to the set of classes that we have seen... so that we will not try to process it more than once...
                         setOfClasses.add(typeClass);

                      // String unqualifiedClassName = "X" + nestedClassName;

                         int startOfUnqualifiedClassName = nestedClassName.lastIndexOf(".");

                         if (verboseLevel > 1)
                              System.out.println("startOfUnqualifiedClassName = " + startOfUnqualifiedClassName);

                         String unqualifiedClassName = nestedClassName.substring(startOfUnqualifiedClassName+1);

                      // Add a map from the class name to its fully qualified name (used in type lookup).
                         if (verboseLevel > 1)
                              System.out.println("############# Set entry in hashmapOfQualifiedNamesOfClasses: unqualifiedClassName = " + unqualifiedClassName + " nestedClassName = " + nestedClassName);

                         hashmapOfQualifiedNamesOfClasses.put(unqualifiedClassName,nestedClassName);

                      // Control the level of recursion so that we can debug this...it seems that
                      // this is typically as high as 47 to process the implicitly included classes.
                         if (implicitClassCounter < implicitClassCounterBound)
                            {
                           // DQ (11/2/2010): comment out this recursive call for now.
                              buildImplicitClassSupport(nestedClassName);
                            }
                           else
                            {
                              if (verboseLevel > 5)
                                   System.out.println("WARNING: Exceeded recursion level " + implicitClassCounter + " nestedClassName = " + nestedClassName);
                            }
                       }
                  }
                 else
                  {
                    if (verboseLevel > 4)
                         System.out.println("This class has been seen previously: nestedClassName = " + nestedClassName);
                 // setOfClasses.add(typeClass);
                  }
             }
            else
             {
            // We might actually do have to include these since they are classes in Java... 
            // What member functions are there on primative types?
               if (verboseLevel > 4)
                    System.out.println("This class is a primitive type (sorry not implemented): type name = " + nestedClassName);
             }
        }



     public static void buildImplicitClassSupport( String className)
        {
       // DQ (12/15/2010): Implicit class support seems to be unavailable via Java reflection...(working on solution to this).

       // There is a lot of information that we need about any implicitly included class.
       // Information about the introspection support is at: http://download.oracle.com/javase/1.4.2/docs/api/java/lang/Class.html
       // Additional information required should include:
       //    1) Class hierarchy.
       //    2) Interfaces
       //    3) package information
       //    4) modifiers (for this class)
       //    5) ProtectionDomain
       //    6) Resources (URLs?)
       //    7) Signers
       //    8) Superclass (part of the class hiearchy)
       //    9) Array information (is the class an array of some base type)
       //   10) See member function of the "Class" class for introspection for more details...

       // List of pakages needed to be include for initial work:
       //    1) java.lang
       //    2) java.io
       //    3) java.util

       // We can't use reflection to get the classes in a package (amazing but true).
       // so for the default packages we have to build a list of the classes that we will include.
       // So we have a list of classes that we will include for each package
       // (http://en.wikipedia.org/wiki/Java_package):
       //    1) java.lang (http://download.oracle.com/javase/6/docs/api/java/lang/package-summary.html)
       //       a. System 
       //    2) java.io  (http://download.oracle.com/javase/6/docs/api/java/io/package-summary.html)
       //       a. InputStream
       //       b. OutputStream
       //    3) java.util

       // Better (best) yet would be that we load what we need as we see it within the compilation.
       // so ever reference class and in that class the classes used as types for every field and method.
       // This will be a large list, but it should terminate, and be a minimal set of types (classes)
       // required to represent the problem :-).

       // See: http://www.java2s.com/Tutorial/Java/0125__Reflection/Catalog0125__Reflection.htm
       // for example of how to handle reflection details.
       // See also: http://www.java2s.com/Tutorial/Java/CatalogJava.htm
/*
          try
             {
            // This is the top level of the list of all packages, build each package as a SgNamespace, then we can build classes (SgClassDeclaration) for each class in each package.
               Package[] packageList = Package.getPackages();
               for (int i = 0; i < packageList.length; i++)
                  {
                    Package pkg = packageList[i];
                    System.out.println("packageList name = " + pkg.getName());

                 // Class [] classList = Class.getDeclaredClasses();
                    Class test_cls = Class.forName(pkg.getName());
                    Field[] fields = getAllFields(test_cls);

                    for (int j = 0; j < fields.length; j++) 
                       {
                         Field fld = fields[j];
                         System.out.println("data member (field) name = " + fld.getName());
                       }
                  }

               System.out.println("Exiting after returning from getAllFields call...");
               System.exit(1);
             }

          catch (Throwable e)
             {
               System.out.println("Caught error in JavaParserSupport (getAllSuperclasses() failed)");
               System.err.println(e);

            // I think we could also rethrough using: "throw e;"

            // Make sure we exit on any error so it is caught quickly.
               System.exit(1);
             }
*/


          implicitClassCounter++;

          if (verboseLevel > 2)
               System.out.println("In buildImplicitClassSupport("+className+"): implicitClassCounter = " + implicitClassCounter);

       // Get the fields, constructors, and methods used in this class.
       // Note that try ... catch is required for using the reflection support in Java.
          try
             {
            // Class cls = Class.forName("java.lang.String");
            // Class cls = Class.forName("java.lang."+node.receiver.toString());

               if (verboseLevel > 1)
                    System.out.println("Generate the implicit Java class for className = " + className + " implicitClassCounter = " + implicitClassCounter);

            // Note that "java.lang" does not appear to be a class (so is that root of all implicitly included classes?).
            // Class cls = Class.forName("java.lang");
            // Class cls = Class.forName("java.io.InputStream");
               Class cls = Class.forName(className);

               if (verboseLevel > 2)
                    System.out.println("Generate the interface list for class " + className);

            // Generate the list if interfaces
            // Class interfaceList[] = cls.getGenericInterfaces();
               Class interfaceList[] = cls.getInterfaces();

               if (verboseLevel > 2)
                    System.out.println("Generate the method list for class " + className);

               Method methlist[] = cls.getDeclaredMethods();

               if (verboseLevel > 2)
                    System.out.println("Calling JavaParser.cactionBuildImplicitClassSupportStart() for className = " + className);

            // Replace any names like "java.lang.System" with "java_lang_System".
               JavaParser.cactionBuildImplicitClassSupportStart(className);
            // String modifiedClassName = className.replace('.','_');
            // JavaParser.cactionBuildImplicitClassSupportStart(modifiedClassName);

               if (verboseLevel > 2)
                    System.out.println("After call to cactionBuildImplicitClassSupportStart");

            // This will get all fields (including private fields), getFields() will not include private fields.
               Field fieldlist[] = cls.getDeclaredFields();

            // This is a way to limit the number of fields to be traversed and thus control the complexity of the implicitly defined class structure.
               int numberOfFields = fieldlist.length;

               int dataMemberCounter = 0;
               for (int i = 0; i < numberOfFields; i++)
                  {
                    Field fld = fieldlist[i];

                    if (verboseLevel > 0)
                       {
                      // This code is part of an interogation of the data in the field and needs to be hidden yet available to support debugging.
                      // ******************************************************************************
                         System.out.println("data member (field) name = " + fld.getName());

                         System.out.println("decl class  = " + fld.getDeclaringClass());
                         System.out.println("type = " + fld.getType());
                         System.out.println("genericType = " + fld.getGenericType());
                         int mod = fld.getModifiers();
                         System.out.println("modifiers   = " + Modifier.toString(mod));

                         System.out.println("fld.isEnumConstant() = " + fld.isEnumConstant());

                      // I think that "synthetic" means compler generated.
                         System.out.println("fld.isSynthetic()    = " + fld.isSynthetic());
                         System.out.println("-----");
                      // ******************************************************************************
                       }

                 // Error: This appears to have "class " prepended to the generated string...causing problems below. 
                 // String nestedClassName = fld.getType().toString();
                 // System.out.println("nestedClassName = " + nestedClassName);

                 // How do I do this in Java???
                 // if (map.find(nestedClassName) == map.end())

                 // Get the class associated with the field (all types in Java are a class, so this is only strange relative to C++).
                    Class typeClass = fld.getType();

                    Type genericType = fld.getGenericType();

                 // Note that if we use "nestedClassName = fld.getType().toString();" nestedClassName has the
                 // name "class " as a prefix and this causes an error, so use "typeClass.getName()" instead.
                    String nestedClassName = typeClass.getName();

                 // Replace any names like "java.lang.System" with "java_lang_System".
                 // nestedClassName = nestedClassName.replace('.','_');

                 // Need to test for: isPrimative(), isArray(), isInterface(), isAssignableFrom(), isInstance()
                 // More documentation at: http://download.oracle.com/javase/1.4.2/docs/api/java/lang/Class.html

                 // We can't output the full field in processType() if the type is an array (so this is just debug support).
                    if (verboseLevel > 2)
                       {
                         if (typeClass.isArray() == true)
                            {
                           // DQ (3/21/2011): If this is an array of some type then we have to query the base type and for now I will skip this.
                              System.out.println("Skipping case of array of type for now (sorry not implemented)... data field = " + fld);
                            }
                       }

                 // Refactored this work so it could be called elsewhere.
                    processType(typeClass);

                 // System.out.println("Exiting after returning from recursive call...");
                 // System.exit(1);

                    if (verboseLevel > 2)
                         System.out.println("Build the implicit type for the data member (field) of type = " + nestedClassName);

                 // Note that i == dataMemberCounter
                    if (dataMemberCounter < dataMemberCounterBound)
                       {
                      // System.out.println("#############################################################################################");
                      // System.out.println("This call to JavaParserSupport.generateType() appears to be a problem: nestedClassName = " + nestedClassName);
                         JavaParserSupport.generateType(typeClass);
                      // System.out.println("DONE: This call to JavaParserSupport.generateType() appears to be a problem: nestedClassName = " + nestedClassName);

                         if (verboseLevel > 2)
                              System.out.println("Build the data member (field) for name = " + fld.getName());

                      // System.out.println("Exiting after call to JavaParserSupport.generateType(typeClass) implicitClassCounter = " + implicitClassCounter);
                      // System.exit(1);

                      // This function assumes that a type has been placed onto the astJavaTypeStack.
                         JavaParser.cactionBuildImplicitFieldSupport(fld.getName());
                       }
                      else
                       {
                         if (verboseLevel > 2)
                              System.out.println("WARNING: Exceeded data member (field) handling iteration count " + i + " className = " + className);
                       }
                    

                    if (verboseLevel > 2)
                         System.out.println("DONE: Building the data member (field) for name = " + fld.getName());

                    if (implicitClassCounter > 5 && false)
                       {
                         System.out.println("Exiting as a test implicitClassCounter = " + implicitClassCounter);
                         System.exit(1);
                       }

                    dataMemberCounter++;
                  }

            // A traversal over the constructors will have to look at all types of constructor arguments 
            // and trigger a recursive call to buildImplicitClassSupport() for any new types.
               Constructor ctorlist[] = cls.getDeclaredConstructors();
               int constructorMethodCounter = 0;
               for (int i = 0; i < ctorlist.length; i++)
                  {
                    Constructor ct = ctorlist[i];
                 /* System.out.println("decl class = " + ct.getDeclaringClass());
                    Class pvec[] = ct.getParameterTypes();
                    for (int j = 0; j < pvec.length; j++)
                         System.out.println("param #" + j + " " + pvec[j]);
                    Class evec[] = ct.getExceptionTypes();
                    for (int j = 0; j < evec.length; j++)
                         System.out.println("exc #" + j + " " + evec[j]);
                    System.out.println("-----");
                  */

                    Class pvec[] = ct.getParameterTypes();

                 // Note that I am ignoring the constructor parameter types at the moment.
                    if (verboseLevel > 2)
                       {
                         System.out.println("constructor name = " + ct.getName());
                         for (int j = 0; j < pvec.length; j++)
                            {
                              System.out.println("   constructor parameter type = " + pvec[j]);
                            }
                       }

                 // System.out.println("constructor name = " + ct.getName());
                    for (int j = 0; j < pvec.length; j++)
                       {
                         if (verboseLevel > 2)
                              System.out.println("   constructor parameter type = " + pvec[j]);

                      // Process the paramter type (add a class if this is not already in the ROSE AST).
                         processType(pvec[j]);
                       }

                 // Simplify the generated AST by skipping the construction of all the member functions in each class.
                 // We might only want to build those member functions that are referenced in the input program (as an option).
                 // JavaParser.cactionBuildImplicitMethodSupport(ct.getName());

                    if (constructorMethodCounter < constructorMethodCounterBound)
                       {
                      // Note that we only want to build types for those function that we want to build.
                      // This mechanism is one way to simplify the generated AST for debugging (restricting 
                      // the number of functions built).

                      // System.out.println("constructor name = " + ct.getName());
                         for (int j = 0; j < pvec.length; j++)
                            {
                           // If we push all the types onto the stack then we have to build every constructor.
                              if (verboseLevel > 2)
                                   System.out.println("This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (constructor): type = " + pvec[j].getName());
                              JavaParserSupport.generateType(pvec[j]);
                              if (verboseLevel > 2)
                                   System.out.println("DONE: This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (constructor): type = " + pvec[j].getName());
                            }

                         if (verboseLevel > 2)
                              System.out.println("Push void as a return type for now (ignored because this is a constructor)");

                      // Push a type to serve as the return type which will be ignored for the case of a constructor
                      // (this allows us to reuse the general member function support).
                         JavaParser.cactionGenerateType("void");

                         if (verboseLevel > 2)
                              System.out.println("DONE: Push void as a return type for now (ignored because this is a constructor)");

                         JavaParser.cactionBuildImplicitMethodSupport(ct.getName());
                       }
                      else
                       {
                         if (verboseLevel > 2)
                              System.out.println("WARNING: Exceeded constructor method handling iteration count " + constructorMethodCounter + " className = " + className);
                       }

                    constructorMethodCounter++;
                  }

            // A traversal over the methods will have to look at all types of method return types and arguments 
            // and trigger a recursive call to buildImplicitClassSupport() for any new types.
            // System.out.println("(skipped method handling) Number of methods = " + methlist.length);
               int methodCounter = 0;
               for (int i = 0; i < methlist.length; i++)
                  {
                    Method m = methlist[i];

                 /* System.out.println("name = " + m.getName());
                    System.out.println("decl class = " + m.getDeclaringClass());
                    Class pvec[] = m.getParameterTypes();
                    for (int j = 0; j < pvec.length; j++)
                         System.out.println("param #" + j + " " + pvec[j]);
                    Class evec[] = m.getExceptionTypes();
                    for (int j = 0; j < evec.length; j++)
                         System.out.println("exc #" + j + " " + evec[j]);
                    System.out.println("return type = " + m.getReturnType());
                    System.out.println("-----");
                 */

                    Class pvec[] = m.getParameterTypes();

                 // Note that I am ignoring the constructor parameter types at the moment.
                    if (verboseLevel > 5)
                       {
                         System.out.println("method name = " + m.getName());
                         System.out.println("   method return type = " + m.getReturnType());
                         for (int j = 0; j < pvec.length; j++)
                            {
                              System.out.println("   method parameter type = " + pvec[j]);
                            }
                       }

                 // Process the return type (add a class if this is not already in the ROSE AST).
                    processType(m.getReturnType());

                 // System.out.println("method name = " + m.getName());
                    for (int j = 0; j < pvec.length; j++)
                       {
                         if (verboseLevel > 4)
                              System.out.println("   method return type = " + m.getReturnType());

                         if (verboseLevel > 4)
                              System.out.println("   method parameter type = " + pvec[j]);

                      // Process the paramter type (add a class if this is not already in the ROSE AST).
                         processType(pvec[j]);
                       }

                 // Simplify the generated AST by skipping the construction of all the member functions in each class.
                 // We might only want to build those member functions that are referenced in the input program (as an option).

                    if (methodCounter < methodCounterBound)
                       {
                      // System.out.println("method name = " + m.getName());
                         for (int j = 0; j < pvec.length; j++)
                            {
                           // If we push all the types onto the stack then we have to build every method.
                              if (verboseLevel > 2)
                                   System.out.println("This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (method): type = " + pvec[j].getName());
                              JavaParserSupport.generateType(pvec[j]);
                              if (verboseLevel > 2)
                                   System.out.println("DONE: This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (method): type = " + pvec[j].getName());
                            }

                      // DQ (4/10/11): Fix this to use the proper return type now (pushed onto stack last and interpreted at the return type).
                      // Push a type to serve as the return type which will be ignored for the case of a method
                      // (this allows us to reuse the general member function support).
                      // System.out.println("Testing with made up return type");
                      // JavaParser.cactionGenerateType("void");
                         JavaParserSupport.generateType(m.getReturnType());

                         JavaParser.cactionBuildImplicitMethodSupport(m.getName());
                       }
                      else
                       {
                         if (verboseLevel > 4)
                              System.out.println("WARNING: Exceeded method handling iteration count " + methodCounter + " className = " + className);
                       }

                    methodCounter++;
                  }

            // Process the interfaces.
               int interfaceCounter = 0;
               for (int i = 0; i < interfaceList.length; i++)
                  {
                    if (verboseLevel > 2)
                       {
                         System.out.println("interface name = " + interfaceList[i].getName());
                       }

                    if (interfaceCounter < interfaceCounterBound)
                       {
                      // Process the interface type (add a class if this is not already in the ROSE AST).
                         processType(interfaceList[i]);
                       }

                    interfaceCounter++;
                  }

            // Compute the total number of statements that we will have be poped from the stack to complete the class definition for ROSE.
               int numberOfStatements = methodCounter + constructorMethodCounter + dataMemberCounter + interfaceCounter;

               if (verboseLevel > 1)
                    System.out.println("Implicit class support: numberOfStatements = " + numberOfStatements + " for className = " + className);

            // This wraps up the details of processing all of the child classes (such as forming SgAliasSymbols for them in the global scope).
               JavaParser.cactionBuildImplicitClassSupportEnd(numberOfStatements,className);
             }

       // try ... catch is required for using the reflection support in Java.
          catch (Throwable e)
             {
               System.out.println("Caught error in JavaParserSupport (Parser failed)");
               System.err.println(e);

            // I think we could also rethrough using: "throw e;"

            // Make sure we exit on any error so it is caught quickly.
               System.exit(1);
             }
        }


     public static boolean isPrimativeType ( TypeBinding typeBinding ) 
        {
          switch (typeBinding.id) 
             {
               case TypeIds.T_void:
               case TypeIds.T_boolean:
               case TypeIds.T_byte:
               case TypeIds.T_char:
               case TypeIds.T_short:
               case TypeIds.T_double:
               case TypeIds.T_float:
               case TypeIds.T_int:
               case TypeIds.T_long:
               case TypeIds.T_JavaLangObject:
            //	case TypeIds.T_JavaLangString:
                    return true;

               default:
            		return false;
             }
   }


     public static void generateType(TypeReference node)
        {
       // This function traverses the type and calls JNI functions to 
       // at the end of the function define a type built in the ROSE 
       // AST and left of the top of the astJavaTypeStack.
       // This is designed as a recursive function.

          if (verboseLevel > 0)
               System.out.println("Inside of generateType(TypeReference)");

          assert(node != null);

          if (verboseLevel > 1)
             {
               System.out.println("Inside of generateType(TypeReference) TypeReference node                               = " + node);
               System.out.println("Inside of generateType(TypeReference) TypeReference node.implicitConversion            = " + node.implicitConversion);

            // DQ (9/3/2011): This causes too much output.
            // System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType                  = " + node.resolvedType);

               System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType.isArrayType()    = " + node.resolvedType.isArrayType());
               System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType.isGenericType()  = " + node.resolvedType.isGenericType());
               System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType.isClass()        = " + node.resolvedType.isClass());
               System.out.println("Inside of generateType(TypeReference) TypeReference isPrimativeType(node.resolvedType) = " + isPrimativeType(node.resolvedType));

               System.out.println("Inside of generateType(TypeReference) TypeReference node.getTypeName()                 = " + node.getTypeName());
               System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType.isClass()        = " + (node.resolvedType.isClass() ? "true" : "false"));
             }

          if (node.resolvedType.isArrayType() == true)
             {
            // TypeBinding baseType = ((ArrayBinding) node.resolvedType).leafComponentType;
               ArrayBinding arrayType = (ArrayBinding) node.resolvedType;
               if (verboseLevel > 1)
                    System.out.println("Inside of generateType(TypeReference) ArrayBinding dimensions = " + arrayType.dimensions);
               TypeBinding baseType = arrayType.leafComponentType;

            // This outputs the declartion for the whole class.
            // System.out.println("Inside of generateType(TypeReference) ArrayBinding baseType   = " + baseType);
               if (verboseLevel > 1)
                  {
                    System.out.println("Inside of generateType(TypeReference) ArrayBinding baseType (debugName) = " + baseType.debugName());
                    System.out.println("Inside of generateType(TypeReference) recursive call to generateType()");
                  }

               generateType(baseType);
             }
            else
             {
            // NOTE: It would be more elegant to not depend upon the debugName() function.
               String name = node.resolvedType.debugName();

               if (verboseLevel > 1)
                    System.out.println("Inside of generateType(TypeReference): NOT an array type so build SgIntType -- TypeReference node = " + name);

            // DQ (8/20/2011): Moved to be after buildImplicitClassSupport().
            // JavaParser.cactionGenerateType(name);

               if (verboseLevel > 1)
                    System.out.println("After building the class we have to build the data members and member functions (built type name " + name + " by default) in generateType(TypeReference)");

            // System.out.println("Calling processType() to recursively build the class structure with member declarations.");
            // This does not work...
            // processType(node.resolvedType);

            // DQ (8/20/2011): Need a better way to handle detecting if this is an implicit class...
            // Maybe we could detect if it is a supported type in the global type map.

            /* Not clear how to generate a typeClass from the TypeReference???
               if (setOfClasses.contains(typeClass) == false)
                  {
                 // Investigate any new type.
                  }
            */

            // DQ (8/22/2011): The reason why we need this is that the import statement allows for the names to be used unqualified.
            // Once we implement proper support for the import statement then we will be able to search the symbol tables for any type
            // names that we can't identify because they lack name qualification!!!

            // If this is a generic type then the "<name>" has to be separated so we can use only the base name of the class (the raw type name).
               String rawTypeName = name;

               int firstAngleBracket = rawTypeName.indexOf("<",0);
               int lastAngleBracket = rawTypeName.lastIndexOf(">",rawTypeName.length()-1);

            // System.out.println("In generateType(TypeReference): firstAngleBracket = " + firstAngleBracket + " lastAngleBracket = " + lastAngleBracket);
               if (firstAngleBracket > 0 && firstAngleBracket < lastAngleBracket)
                  {
                    rawTypeName = rawTypeName.substring(0,firstAngleBracket);

                    name = rawTypeName;
                  }

            // System.out.println("In generateType(TypeReference): rawTypeName = " + rawTypeName);

            // We don't really want to handl this as a special case, but from the Java side I don't know how to generate the qualified name.
               if (rawTypeName.startsWith("List") == true)
                    name = "java.util.List";

            // System.out.println("In generateType(TypeReference): After reset (for List) name = " + name);

            // buildImplicitClassSupport("java.util.List");

            // DQ (8/23/2011): Note that implicit classes will evaluate to "node.resolvedType.isClass() == false" while classes define in the file will be true.
               if (isPrimativeType(node.resolvedType) == false && node.resolvedType.isClass() == false)
                  {
                 // System.out.println("In generateType(TypeReference): Calling buildImplicitClassSupport() to recursively build the class structure with member declarations: name = " + name);
                 // buildImplicitClassSupport(name);

                 // DQ (9/4/2011): This does not work well enough since the set is interms of the internal types instead of type names.
                 // So use the rawTypeName and the hashmapOfQualifiedNamesOfClasses instead (since it is clearer test to debug).
                 // DQ (9/3/2011): Check if this is a type (class) that has already been handled.
                 // if (setOfClasses.contains(node.resolvedType) == false)

                     System.out.println("In generateType(TypeReference): Output set of entries in hashmapOfQualifiedNamesOfClasses for name = " + name);
                     Set<Map.Entry<String,String>> set = hashmapOfQualifiedNamesOfClasses.entrySet();
                     Iterator i = set.iterator();
                  // Display elements
                     while(i.hasNext())
                        {
                          Map.Entry<String,String> me = (Map.Entry<String,String>)i.next();
                          System.out.print("Hashmap hashmapOfQualifiedNamesOfClasses entry: " + me.getKey() + ": ");
                          System.out.println(me.getValue());
                        }


                  // DQ (/4/2011): This code is a problem, I think that the set types are inconsistant so this predicate is always false.
                     if (hashmapOfQualifiedNamesOfClasses.entrySet().contains(rawTypeName) == false)
                       {
                         System.out.println("In generateType(TypeReference): This class has not been seen previously: name = " + name);
                         buildImplicitClassSupport(name);
                         System.out.println("DONE: In generateType(TypeReference): This class has not been seen previously: name = " + name);
                       }
                      else
                       {
                         System.out.println("In generateType(TypeReference): This class already been handled: name = " + name);
                       }

                 // System.out.println("DONE: In generateType(TypeReference): Calling buildImplicitClassSupport() to recursively build the class structure with member declarations: name = " + name);
                  }

            // DQ (8/20/2011): Moved to be after buildImplicitClassSupport().
               JavaParser.cactionGenerateType(name);

            // System.out.println("Exiting as a test (built type name " + name + " by default) in generateType(TypeReference)");
            // System.exit(1);
            }

       // JavaParser.cactionGenerateType("int");

/*
       // switch(expressionType.kind())
          switch(node.kind())
             {
               case Binding.BASE_TYPE :
            //-----------cast to something which is NOT a base type--------------------------
                    if (expressionType == TypeBinding.NULL)
                       {
                         tagAsUnnecessaryCast(scope, castType);
                         return true; //null is compatible with every thing
                       }
                    return false;

               case Binding.ARRAY_TYPE :
                    if (castType == expressionType)
                       {
                         tagAsUnnecessaryCast(scope, castType);
                         return true; // identity conversion
                       }
                    switch (castType.kind())
                       {
                         case Binding.ARRAY_TYPE :
                         // ( ARRAY ) ARRAY
                              TypeBinding castElementType = ((ArrayBinding) castType).elementsType();
                              TypeBinding exprElementType = ((ArrayBinding) expressionType).elementsType();
                              if (exprElementType.isBaseType() || castElementType.isBaseType())
                                 {
                                   if (castElementType == exprElementType)
                                      {
                                        tagAsNeedCheckCast();
                                        return true;
                                      }
                                   return false;
                                 }
                            // recurse on array type elements
                               return checkCastTypesCompatibility(scope, castElementType, exprElementType, expression);

                         case Binding.TYPE_PARAMETER :
                           // ( TYPE_PARAMETER ) ARRAY
                              TypeBinding match = expressionType.findSuperTypeOriginatingFrom(castType);
                              if (match == null)
                                 {
                                   checkUnsafeCast(scope, castType, expressionType, null, true);
                                 }
                           // recurse on the type variable upper bound
                              return checkCastTypesCompatibility(scope, ((TypeVariableBinding)castType).upperBound(), expressionType, expression);

                         default:
                           // ( CLASS/INTERFACE ) ARRAY
                              switch (castType.id)
                                 {
                                   case T_JavaLangCloneable :
                                   case T_JavaIoSerializable :
                                        tagAsNeedCheckCast();
                                        return true;
                                   case T_JavaLangObject :
                                        tagAsUnnecessaryCast(scope, castType);
                                        return true;
                                   default :
                                        return false;
                                 }
                       }

               case Binding.TYPE_PARAMETER :
                    TypeBinding match = expressionType.findSuperTypeOriginatingFrom(castType);
                    if (match != null)
                       {
                         return checkUnsafeCast(scope, castType, expressionType, match, false);
                       }
                 // recursively on the type variable upper bound
                    return checkCastTypesCompatibility(scope, castType, ((TypeVariableBinding)expressionType).upperBound(), expression);

               case Binding.WILDCARD_TYPE :
               case Binding.INTERSECTION_TYPE :
                    match = expressionType.findSuperTypeOriginatingFrom(castType);
                    if (match != null)
                       {
                         return checkUnsafeCast(scope, castType, expressionType, match, false);
                       }
                // recursively on the type variable upper bound
                   return checkCastTypesCompatibility(scope, castType, ((WildcardBinding)expressionType).bound, expression);

               default:
                  {
                  }
             }
*/

       // System.out.println("Exiting as a test in generateType(TypeReference)");
       // System.exit(1);
        }

/*
     public static int typeCode(String className)
        {
          if (verboseLevel > -1)
               System.out.println("Inside of typeCode(String className) className = " + className);

          if (className == "boolean")
             {
               JavaParser.generateBooleanType();
             }
            else if (className == "byte")
             {
               JavaParser.generateByteType();
             }
            else if (className == "char")
             {
               JavaParser.generateCharType();
             }
            else if (className == "int")
             {
               JavaParser.generateIntType();
             }
            else if (className == "short")
             {
               JavaParser.generateShortType();
             }
            else if (className == "float")
             {
               JavaParser.generateFloatType();
             }
            else if (className == "long")
             {
               JavaParser.generateLongType();
             }
            else if (className == "double")
             {
               JavaParser.generateDoubleType();
             }
            else if (className == "null")
             {
               JavaParser.generateNullType();
             }
            else
             {
               System.out.println("className = " + className + "not handled...");
               System.exit(1);
             }
        }
*/

     public static void generateType(Class node)
        {
       // This function is used to build types that are classes (implicit classes 
       // that already exist (have been built) and thus just need be found and a 
       // reference put onto the astJavaTypeStack).

          if (verboseLevel > 0)
               System.out.println("Inside of generateType(Class) (sorry, not implemented) class = " + node);

          if (node.isPrimitive() == false)
             {
            // Investigate any new type.
               if (node.isArray() == true)
                  {
                 // DQ (3/21/2011): If this is an array of some type then we have to query the base type and for now I will skip this.
                 // System.out.println("Skipping case of array of type for now (sorry not implemented)... " + node.getComponentType());

                 // Build an integer type instead of an array of the proper type (temporary fix so that I can focus on proper class support).
                 // JavaParser.cactionGenerateType("int");
                    generateType(node.getComponentType());

                 // System.out.println("Calling JavaParser.cactionGenerateArrayType()");
                    JavaParser.cactionGenerateArrayType();

                 // System.out.println("Exiting as a test in generateType(Class) (case of array type class)");
                 // System.exit(1);
                  }
                 else
                  {
                 // Note that "toString()" inserts "class" into the generated name of the type (so use "getName()").
                    String className = node.getName();

                 // If this is a class type (e.g. in the C++ sense) then we want to build a proper SgClassType IR node.
                 // System.out.println("Build a proper class for this type = " + node);
                 // System.out.println("Build a proper class for this type = " + className);

                 // We know that this name should be interpreted as a proper class so we need to call a specific JNI function to cause it to be generated on the C++ side.
                 // JavaParser.cactionGenerateType(className);
                    JavaParser.cactionGenerateClassType(className);

                 // System.out.println("Exiting as a test in generateType(Class) (case of proper class)");
                 // System.exit(1);
                  }
             }
            else
             {
               if (verboseLevel > 0)
                    System.out.println("Build a primative type: int ");

               String className = node.getName();

            // For now just build the type to be SgTypeInt.
            // JavaParser.cactionGenerateType("int");
               JavaParser.cactionGenerateType(className);

            // buildType(className);
             }

          if (verboseLevel > 0)
               System.out.println("Leaving generateType(Class) (case of proper class)");

       // System.out.println("Exiting as a test at bottom of generateType(Class) (case of proper class)");
       // System.exit(1);
        }

     public static void generateType(TypeBinding node)
        {
       // TypeBindings are used in variable declarations (I think that these are the primative types).
       // No, they can be class types, and when they are this is a current problem.

       // This function traverses the type and calls JNI functions to 
       // at the end of the function define a type built in the ROSE 
       // AST and left of the top of the astJavaTypeStack.
       // This is designed as a recursive function.

          if (verboseLevel > 0)
               System.out.println("Inside of generateType(TypeBinding): node.id = " + node.id);

       // Scope scope = node.scope;
          int id = node.id;

          if (verboseLevel > 0)
               System.out.println("Inside of generateType(): switch on id = " + id);

          switch (id)
             {
               case TypeIds.T_boolean:
                  {
                 // return TypeBinding.BOOLEAN;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_boolean");
                    JavaParser.cactionGenerateType("boolean");
                    break;
                  }

               case TypeIds.T_byte:
                  {
                 // return TypeBinding.BYTE;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_byte");
                    JavaParser.cactionGenerateType("byte");
                    break;
                  }

               case TypeIds.T_char:
                  {
                 // return TypeBinding.CHAR;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_char");
                    JavaParser.cactionGenerateType("char");
                    break;
                  }

               case TypeIds.T_short:
                  {
                 // return TypeBinding.SHORT;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_short");
                    JavaParser.cactionGenerateType("short");
                    break;
                  }

               case TypeIds.T_double:
                  {
                 // return TypeBinding.DOUBLE;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_double");
                    JavaParser.cactionGenerateType("double");
                    break;
                  }

               case TypeIds.T_float:
                  {
                 // return TypeBinding.FLOAT;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_float");
                    JavaParser.cactionGenerateType("float");
                    break;
                  }

               case TypeIds.T_int:
                  {
                 // return TypeBinding.INT;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_int");

                 // Later we want to have the "id" be passed directly.
                    JavaParser.cactionGenerateType("int");
                    break;
                  }

               case TypeIds.T_long:
                  {
                 // return TypeBinding.LONG;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_long");
                    JavaParser.cactionGenerateType("long");
                    break;
                  }

               case TypeIds.T_JavaLangObject:
                  {
                 // return scope.getJavaLangObject();
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_JavaLangObject");

                    System.out.println("Case of JavaLangObject not implemented.");
                    System.exit(1);

                 // JavaParser.cactionGenerateType("Object");
                    break;
                  }

               case TypeIds.T_JavaLangString:
                  {
                 // return scope.getJavaLangString();
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_JavaLangString");

                    JavaParser.cactionGenerateType("java.lang.String");
                    break;
                  }

               default:
                  {
                    System.out.println("Error: unknown type in generateType(): id = " + id);
                 // System.abort(1);
                    System.exit(1);
                    break;
                  }
             }


        }


  /**
   * Return a list of all fields (whatever access status, and on whatever
   * superclass they were defined) that can be found on this class.
   * This is like a union of {@link Class#getDeclaredFields()} which
   * ignores and super-classes, and {@link Class#getFields()} which ignored
   * non-public fields
   * @param clazz The class to introspect
   * @return The complete list of fields
   */
  public static Field[] getAllFields(Class<?> clazz)
  {
      List<Class<?>> classes = getAllSuperclasses(clazz);
      classes.add(clazz);
      return getAllFields(classes);
  }
  /**
   * As {@link #getAllFields(Class)} but acts on a list of {@link Class}s and
   * uses only {@link Class#getDeclaredFields()}.
   * @param classes The list of classes to reflect on
   * @return The complete list of fields
   */
  private static Field[] getAllFields(List<Class<?>> classes)
  {
      Set<Field> fields = new HashSet<Field>();
      for (Class<?> clazz : classes)
      {
          fields.addAll(Arrays.asList(clazz.getDeclaredFields()));
      }

      return fields.toArray(new Field[fields.size()]);
  }
  /**
   * Return a List of super-classes for the given class.
   * @param clazz the class to look up
   * @return the List of super-classes in order going up from this one
   */
  public static List<Class<?>> getAllSuperclasses(Class<?> clazz)
  {
      List<Class<?>> classes = new ArrayList<Class<?>>();

      Class<?> superclass = clazz.getSuperclass();
      while (superclass != null)
      {
          classes.add(superclass);
          superclass = superclass.getSuperclass();
      }

      return classes;
  }



   }

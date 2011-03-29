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

  // Counter for recursive function call...debugging support.
     private static int implicitClassCounter = 0;

     private static int verboseLevel = 0;

  // Initialization function, but be called before we can use member functions in this class.
         public static void initialize(CompilationResult x, int input_verboseLevel)
        {
       // This has to be set first (required to support source position translation).
          rose_compilationResult = x;
          setOfClasses = new HashSet<Class>();
          implicitClassCounter = 0;

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
          try
             {
            // Class cls = Class.forName("java.lang.String");
            // Class cls = Class.forName("java.lang."+node.receiver.toString());

               if (verboseLevel > -1)
                    System.out.println("Generate the class for implicit className = " + className);

            // Note that "java.lang" does not appear to be a class (so is that root of all implicitly included classes?).
            // Class cls = Class.forName("java.lang");
            // Class cls = Class.forName("java.io.InputStream");
               Class cls = Class.forName(className);

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

               Field fieldlist[] = cls.getDeclaredFields();
               for (int i = 0; i < fieldlist.length; i++)
                  {
                    Field fld = fieldlist[i];

                    if (verboseLevel > 4)
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

                         System.out.println("fld.getType().isAnnotation()                 = " + fld.getType().isAnnotation());
                      // System.out.println("fld.getType().isAnnotationPresent(Class<? extends Annotation> annotationClass) = " + fld.getType().isAnnotationPresent(fld.getType()));
                         System.out.println("fld.getType().isAnonymousClass()             = " + fld.getType().isAnonymousClass());
                         System.out.println("fld.getType().isArray()                      = " + fld.getType().isArray());
                      // Not clear what class to use as a test input for isAssignableFrom(Class<?> cls) function...
                         System.out.println("fld.getType().isAssignableFrom(Class<?> cls) = " + fld.getType().isAssignableFrom(fld.getType()));
                         System.out.println("fld.getType().isEnum()                       = " + fld.getType().isEnum());
                         System.out.println("fld.getType().isInstance(Object obj)         = " + fld.getType().isInstance(fld.getType()));
                         System.out.println("fld.getType().isInterface()                  = " + fld.getType().isInterface());
                         System.out.println("fld.getType().isLocalClass()                 = " + fld.getType().isLocalClass());
                         System.out.println("fld.getType().isMemberClass()                = " + fld.getType().isMemberClass());
                         System.out.println("fld.getType().isPrimitive()                  = " + fld.getType().isPrimitive());
                         System.out.println("fld.getType().isSynthetic()                  = " + fld.getType().isSynthetic());
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

                 // We don't have to support Java primative types as classes in the AST (I think).
                    if (typeClass.isPrimitive() == false)
                       {
                      // Check if this is a type (class) that has already been handled.
                         if (setOfClasses.contains(typeClass) == false)
                            {
                           // Investigate any new type.
                              if (typeClass.isArray() == true)
                                 {
                                // DQ (3/21/2011): If this is an array of some type then we have to query the base type and for now I will skip this.
                                   System.out.println("Skipping case of array of type for now (sorry not implemented)... data field = " + fld);
                                 }
                                else
                                 {
                                // This is not an array type and not a primative type (so it should be a class, I think).
                                   if (verboseLevel > 1)
                                        System.out.println("Recursive call to buildImplicitClassSupport() to build type = " + nestedClassName);

                                // Add this to the set of classes that we have seen... so that we will not try to process it more than once...
                                   setOfClasses.add(typeClass);

                                // Control the level of recursion so that we can debug this...it seems that
                                // this is typically as high as 47 to process the implicitly included classes.
                                   int implicitClassCounterBound = 10;
                                   if (implicitClassCounter < implicitClassCounterBound)
                                      {
                                     // DQ (11/2/2010): comment out this recursive call for now.
                                        buildImplicitClassSupport(nestedClassName);
                                      }
                                     else
                                      {
                                        System.out.println("WARNING: Exceeded recursion level " + implicitClassCounter + " nestedClassName = " + nestedClassName);
                                      }
                                 }
                            }
                           else
                            {
                              if (verboseLevel > 2)
                                   System.out.println("This class has been seen previously: nestedClassName = " + nestedClassName);
                           // setOfClasses.add(typeClass);
                            }
                       }
                      else
                       {
                      // We might actually do have to include these since they are classes in Java... 
                      // What member functions are there on primative types?
                         if (verboseLevel > 2)
                              System.out.println("This class is a primitive type (sorry not implemented): type name = " + nestedClassName);
                       }

                 // System.out.println("Exiting after returning from recursive call...");
                 // System.exit(1);

                    if (verboseLevel > 2)
                         System.out.println("Build the implicit type for the data member (field) of type = " + nestedClassName);

                    JavaParserSupport.generateType(typeClass);

                    if (verboseLevel > 2)
                         System.out.println("Build the data member (field) for name = " + fld.getName());

                    JavaParser.cactionBuildImplicitFieldSupport(fld.getName());

                    if (verboseLevel > 2)
                         System.out.println("DONE: Building the data member (field) for name = " + fld.getName());
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
                 // Note that I am ignoring the constructor parameter types at the moment.
                    if (verboseLevel > 2)
                         System.out.println("constructor name = " + ct.getName());

                 // Simplify the generated AST by skipping the construction of all the member functions in each class.
                 // We might only want to build those member functions that are referenced in the input program (as an option).
                 // JavaParser.cactionBuildImplicitMethodSupport(ct.getName());
                    int constructorMethodCounterBound = 0;
                 // int constructorMethodCounterBound = 1000;
                    if (constructorMethodCounter < constructorMethodCounterBound)
                       {
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

                 // DQ (11/1/2010): Comment out while I handle data member types...
                 // Note that I am ignoring the function type at the moment.
                    if (verboseLevel > 2)
                         System.out.println("method name = " + m.getName());

                 // Simplify the generated AST by skipping the construction of all the member functions in each class.
                 // We might only want to build those member functions that are referenced in the input program (as an option).
                    int methodCounterBound = 2;
                 // int methodCounterBound = 1000;
                    if (methodCounter < methodCounterBound)
                       {
                         JavaParser.cactionBuildImplicitMethodSupport(m.getName());
                       }
                      else
                       {
                         if (verboseLevel > 2)
                              System.out.println("WARNING: Exceeded method handling iteration count " + methodCounter + " className = " + className);
                       }

                    methodCounter++;
                  }

              JavaParser.cactionBuildImplicitClassSupportEnd(className);
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



     public static void generateType(TypeReference node)
        {
       // This function traverses the type and calls JNI functions to 
       // at the end of the function define a type built in the ROSE 
       // AST and left of the top of the astJavaTypeStack.
       // This is designed as a recursive function.

          if (verboseLevel > 0)
               System.out.println("Inside of generateType(TypeReference)");

          JavaParser.cactionGenerateType("int");

        }

     public static void generateType(Class node)
        {
       // This function is used to build types that are classes (implicit classes 
       // that already exist (have been built) and thus just need be found and a 
       // reference put onto the astJavaTypeStack).

          if (verboseLevel > 0)
               System.out.println("Inside of generateType(Class) (sorry, not implemented)");

       // For now just build the type to be SgTypeInt.
          JavaParser.cactionGenerateType("int");
        }

     public static void generateType(TypeBinding node)
        {
       // TypeBindings are used in variable declarations (I think that these are the primative types .

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
                    break;
                  }

               case TypeIds.T_byte:
                  {
                 // return TypeBinding.BYTE;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_byte");
                    break;
                  }

               case TypeIds.T_char:
                  {
                 // return TypeBinding.CHAR;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_char");
                    break;
                  }

               case TypeIds.T_short:
                  {
                 // return TypeBinding.SHORT;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_short");
                    break;
                  }

               case TypeIds.T_double:
                  {
                 // return TypeBinding.DOUBLE;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_double");
                    break;
                  }

               case TypeIds.T_float:
                  {
                 // return TypeBinding.FLOAT;
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_float");
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
                    break;
                  }

               case TypeIds.T_JavaLangObject:
                  {
                 // return scope.getJavaLangObject();
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_JavaLangObject");
                    break;
                  }

               case TypeIds.T_JavaLangString:
                  {
                 // return scope.getJavaLangString();
                    if (verboseLevel > 2)
                         System.out.println("switch case of T_JavaLangString");
                    break;
                  }

               default:
                  {
                    System.out.println("Error: unknown type in generateType()");
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

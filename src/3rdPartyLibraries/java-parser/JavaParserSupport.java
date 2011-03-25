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
     private static int counter = 0;

  // Initialization function, but be called before we can use member functions in this class.
     public static void initialize(CompilationResult x)
        {
       // This has to be set first (required to support source position translation).
          rose_compilationResult = x;
          setOfClasses = new HashSet<Class>();
          counter = 0;
        }

     public static void sourcePosition(ASTNode node)
        {
       // The source positon (line and comun numbers) can be computed within ECJ. 
       // This is an example of how to do it.

       // We need the CompilationResult which is stored in the CompilationUnit (as I recall).
          assert rose_compilationResult != null : "rose_compilationResult not initialized";

          int startingSourcePosition = node.sourceStart();
          int endingSourcePosition   = node.sourceEnd();
          System.out.println("In JavaParserSupport::sourcePosition(ASTNode): start = " + startingSourcePosition + " end = " + endingSourcePosition);

       // Example of how to compute the starting line number and column position of any AST node.
          int problemStartPosition = startingSourcePosition;
          int[] lineEnds;
          int lineNumber   = problemStartPosition >= 0 ? Util.getLineNumber(problemStartPosition, lineEnds = rose_compilationResult.getLineSeparatorPositions(), 0, lineEnds.length-1) : 0;
          int columnNumber = problemStartPosition >= 0 ? Util.searchColumnNumber(rose_compilationResult.getLineSeparatorPositions(), lineNumber, problemStartPosition) : 0;
          System.out.println("In JavaParserSupport::sourcePosition(ASTNode): lineNumber = " + lineNumber + " columnNumber = " + columnNumber);

       // Example of how to compute the ending line number and column position of any AST node.
          int problemEndPosition = endingSourcePosition;
          int lineNumber_end   = problemEndPosition >= 0 ? Util.getLineNumber(problemEndPosition, lineEnds = rose_compilationResult.getLineSeparatorPositions(), 0, lineEnds.length-1) : 0;
          int columnNumber_end = problemEndPosition >= 0 ? Util.searchColumnNumber(rose_compilationResult.getLineSeparatorPositions(), lineNumber, problemEndPosition) : 0;
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


          counter++;

          System.out.println("In buildImplicitClassSupport("+className+"): counter = "+counter);

       // Get the fields, constructors, and methods used in this class.
          try
             {
            // Class cls = Class.forName("java.lang.String");
            // Class cls = Class.forName("java.lang."+node.receiver.toString());

               System.out.println("Generate the class for className = " + className);

            // Note that "java.lang" does not appear to be a class (so is that root of all implicitly included classes?).
            // Class cls = Class.forName("java.lang");
            // Class cls = Class.forName("java.io.InputStream");
               Class cls = Class.forName(className);

               System.out.println("Generate the method list for class " + className);
               Method methlist[] = cls.getDeclaredMethods();

               System.out.println("Calling JavaParser.cactionBuildImplicitClassSupportStart() for className = " + className);

            // Replace any names like "java.lang.System" with "java_lang_System".
               JavaParser.cactionBuildImplicitClassSupportStart(className);
            // String modifiedClassName = className.replace('.','_');
            // JavaParser.cactionBuildImplicitClassSupportStart(modifiedClassName);

               System.out.println("After call to cactionBuildImplicitClassSupportStart");

               Field fieldlist[] = cls.getDeclaredFields();
               for (int i = 0; i < fieldlist.length; i++)
                  {
                    Field fld = fieldlist[i];
                    System.out.println("data member (field) name = " + fld.getName());

                    System.out.println("decl class  = " + fld.getDeclaringClass());
                    System.out.println("type = " + fld.getType());
                    System.out.println("genericType = " + fld.getGenericType());
                    int mod = fld.getModifiers();
                    System.out.println("modifiers   = " + Modifier.toString(mod));

                    System.out.println("fld.isEnumConstant() = " + fld.isEnumConstant());
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
                    if (typeClass.isPrimitive() == false)
                       {
                         if (setOfClasses.contains(typeClass) == false)
                            {
                              if (typeClass.isArray() == true)
                                 {
                                // DQ (3/21/2011): If this is an array of some type then we have to query the base type and for now I will skip this.
                                   System.out.println("Skipping case of array of type... = " + nestedClassName);
                                 }
                                else
                                 {
                                // This is not an array type and not a primative type (so it should be a class, I think).
                                   System.out.println("Recursive call to buildImplicitClassSupport() to build type = " + nestedClassName);

                                // Add this to the set of classes that we have seen... so that we will not try to process it more than once...
                                   setOfClasses.add(typeClass);

                                // Control the level of recursion so that we can debug this...it seems that
                                // this is typically as high as 47 to process the implicitly included classes.
                                   if (counter < 100)
                                      {
                                     // DQ (11/2/2010): comment out this recursive call for now.
                                        buildImplicitClassSupport(nestedClassName);
                                      }
                                     else
                                      {
                                        System.out.println("WARNING: Exceeded recursion level " + counter + " nestedClassName = " + nestedClassName);
                                      }
                                 }
                            }
                           else
                            {
                              System.out.println("This class has been seen previously: nestedClassName = " + nestedClassName);
                           // setOfClasses.add(typeClass);
                            }
                       }
                      else
                       {
                      // We actually do have to include these since they are classes in Java...
                         System.out.println("This class is a primative type: nestedClass = " + nestedClassName);
                       }

                 // System.out.println("Exiting after returning from recursive call...");
                 // System.exit(1);

                    JavaParser.cactionBuildImplicitFieldSupport(fld.getName());
                  }

               Constructor ctorlist[] = cls.getDeclaredConstructors();
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
                    System.out.println("constructor name = " + ct.getName());

                 // Simplify the generated AST by skipping the construction of all the member functions in each class.
                 // We might only want to build those member functions that are referenced in the input program (as an option).
                    JavaParser.cactionBuildImplicitMethodSupport(ct.getName());
                  }

               System.out.println("(skipped method handling) Number of methods = " + methlist.length);
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
                    System.out.println("method name = " + m.getName());

                 // Simplify the generated AST by skipping the construction of all the member functions in each class.
                 // We might only want to build those member functions that are referenced in the input program (as an option).
                    if (methodCounter < 600)
                       {
                         JavaParser.cactionBuildImplicitMethodSupport(m.getName());
                       }
                      else
                       {
                         System.out.println("WARNING: Exceeded method handling iteration count " + methodCounter + " className = " + className);
                       }

                    methodCounter++;
                  }

              JavaParser.cactionBuildImplicitClassSupportEnd(className);
             }

          catch (Throwable e)
             {
               System.out.println("Caught error in JavaParserSupport (Parser failed)");
               System.err.println(e);

            // I think we could also rethrough using: "throw e;"

            // Make sure we exit on any error so it is caught quickly.
               System.exit(1);
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

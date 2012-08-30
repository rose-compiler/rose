import org.eclipse.jdt.internal.compiler.batch.*;
import org.eclipse.jdt.internal.compiler.classfmt.ClassFileConstants;

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

import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;


// DQ (11/1/2010): This improved design separates out the parsing support, from the ECJ AST traversal, and the parser.
class JavaParserSupport {
    // DQ (8/20/2011): Added a simple way to control the number of data members, constructors, and member functions built.
    // 0: No significant limits applied to number of constructs in the AST.
    // 1: Limits the number to be built into the enerated AST
    static boolean VISUALIZE_AST = false;

    // This class is intended to contain functions to support the JNI specific in src/frontend/ECJ_ROSE_Connection/JavaParserActionROSE.C.
    // Note that the functions in JavaParserActionROSE.C are JNI functions that are called by the Java
    // code in src/3rdPartyLibraries/java-parser/ecjASTVisitor.java, this layout of code is similar to the handling of the Fortran support 
    // in ROSE (except that OFP calls functions representing parser actions while this Java support calls functions representing the
    // translation of the ECJ AST to build the ROSE AST (so the Java support has less to do with parsing than AST translation)).
    // The AST translation is similar to the translation from the EDG AST translation to build the ROSE AST (supporting C and C++).

    // This is used to compute source code positions.
    private static CompilationResult rose_compilationResult;

    public static ArrayList<String> importOnDemandList = null;
    //
    // Create a loader for finding classes.
    //
    public static ClassLoader pathLoader = null;

    //
    // Create a symbolTable map to keep track of user-defined type declarations.
    //
    public static HashMap<Class, TypeDeclaration> userTypeTable = new HashMap<Class, TypeDeclaration>();

    //
    // Create a symbolTable map to keep track of packages and type that have already been encountered.
    //
    public static HashMap<String, HashMap<String, Class>> symbolTable = new HashMap<String, HashMap<String, Class>>();

    public static boolean typeExists(String package_name, String type_name) {
        return (symbolTable.containsKey(package_name) ? symbolTable.get(package_name).containsKey(type_name) : false); 
    }

    public static boolean typeExists(String type_name) {
        return typeExists("", type_name); 
    }

    public static void insertType(String package_name, String type_name) {
        assert (! (symbolTable.containsKey(package_name) && symbolTable.get(package_name).containsKey(type_name))); 
        if (! symbolTable.containsKey(package_name)) {
            symbolTable.put(package_name, new HashMap<String, Class>());
        }
        symbolTable.get(package_name).put(type_name, null); 
    }
    
    public static void insertType(String package_name, Class type) {
        String type_name = new String(type.getSimpleName());
        assert (! (symbolTable.containsKey(package_name) && symbolTable.get(package_name).containsKey(type_name))); 
        if (! symbolTable.containsKey(package_name)) {
            symbolTable.put(package_name, new HashMap<String, Class>());
        }
        symbolTable.get(package_name).put(type_name, type); 
    }

/*
    public static void insertType(TypeDeclaration type) {
        insertType("", type); 
    }
    

    public static void processType(String package_name, TypeDeclaration type) {
        String type_name = new String(type.name);
        if (! typeExists(package_name, type_name)) {
            insertType(package_name, type);
    
            cactionPushPackage(package_name);
            traverseType(type);
            cactionPopPackage();
        }
    }
*/

    static Class getClassForName(String typename) {
        Class cls = null;
        try {
            cls = Class.forName(typename, true, pathLoader);
        }
        catch (ClassNotFoundException ee) {
            if (verboseLevel > 0) {
                System.out.println("(2) Caught error in JavaParserSupport (Parser failed)");
                System.err.println(ee);
            }
        }
        catch(NoClassDefFoundError eee) {
            System.err.println("(1) Error in getClassForName --- Could not read type: " + eee.getMessage()); 
            System.exit(1);
        }
        
    	return cls;
    }
    
    /**
     * 
     */
    static void identifyUserDefinedTypes(Class cls, TypeDeclaration node) {
        userTypeTable.put(cls, node);   // keep track of user-defined TypeDeclarations

        if (node.memberTypes != null) {
            for (int i = 0; i < node.memberTypes.length; i++) { // for each inner type of this type ...
                String typename = new String(node.name);
                Class inner[] = cls.getDeclaredClasses();
                for (int k = 0; k < inner.length; k++) { // ... look for its matching counterpart.
                    if (inner[k].getSimpleName() == typename) {
                        identifyUserDefinedTypes(inner[k], node.memberTypes[i]);
//System.out.println("Matching user-defined inner class " + typename + " with " + inner[k].getCanonicalName());
                    }
                }
            }
        }
    }
    
    /**
     * 
     * 
     */
    static void identifyUserDefinedTypes(String prefix, TypeDeclaration node) {
        String typename = prefix + (prefix.length() > 0 ? "." : "") + new String(node.name);
// TODO: REMOVE THIS
/*
System.out.println("Preprocessing type " + typename);

if (node.superclass != null){
String name = new String(node.superclass.resolvedType.getPackage().readableName()),
sname = node.superclass.resolvedType.debugName().substring(name.length() + 1);

System.out.println("** The package is: " + name + ";  The class name is " + sname + "; The super class is " + name + "." + sname);
}
*/
        try {
            Class cls = getClassForName(typename);
            if (cls == null) throw new ClassNotFoundException(typename);
            String canonical_name = cls.getCanonicalName(),
                   class_name = cls.getName(),
                   simple_name = cls.getSimpleName(),
                   class_package = (simple_name.length() < canonical_name.length()
                                          ? canonical_name.substring(0, canonical_name.length() - simple_name.length() -1)
                                          : "");
// TODO: REMOVE THIS
/*            
System.out.println("(1) The canonical name is: " + canonical_name +
                   "; The prefix is: " + prefix +
                   "; The typename is: " + typename +
                   "; The class package is: " + class_package +
                   ";  The class name is " + class_name +
                   ";  The simple class name is " + simple_name);
*/
            assert(cls.getEnclosingClass() == null); // not an inner class
//            insertType(class_package, cls); // keep track of top-level classes that have been seen
            identifyUserDefinedTypes(cls, node);
        }
        catch (ClassNotFoundException e) {
            System.out.println("(1) Caught error in JavaParserSupport (Parser failed)");
            System.err.println(e);

            System.exit(1);
        }
    }

public static void processClasspath(String classpath) {
    importOnDemandList = new ArrayList<String>();
    importOnDemandList.add(""); // The null package must be first!
		
    ArrayList<File> files = new ArrayList<File>();
    while(classpath.length() > 0) {
    	int index = classpath.indexOf(':');
        if (index == -1) {
        	files.add(new File(classpath));
        	classpath = "";
        }
        else {
        	String filename = classpath.substring(0, index);
        	files.add(new File(filename));
        	classpath = classpath.substring(index + 1);
        }
    }

    try {
        // Convert File to a URL
        URL[] urls = new URL[files.size() + 1];
        for (int i = 0; i < files.size(); i++) {
            urls[i] = files.get(i).toURL();
        }
        urls[files.size()] = new File(".").toURL(); // always add curent directory

        // Create a new class loader with the directories
        pathLoader = new URLClassLoader(urls, ClassLoader.getSystemClassLoader());
    } catch (MalformedURLException e) {
        System.err.println("(3) Error in processClasspath: " + e.getMessage()); 
        System.exit(1);
    }
}
    
    
    /**
     * @param unit
     * 
     */
    public static void preprocess(CompilationUnitDeclaration unit) {
        String package_name = "";
// TODO: REMOVE THIS

        //
        // Make sure that Object is processed first!
        //
        if (! typeExists("java.lang", "Object")) {
            preprocessClass("java.lang.Object");
// TODO: REMOVE THIS
//System.out.println("done processing java.lang.Object");                     
/*            
            insertType("java.lang", Object.class);
System.out.println("Inserting type " + "java.lang.Object");
            JavaParser.cactionGenerateType("java.lang.Object", 0);
            JavaParser.cactionTypeReference("java.lang.Object", new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
            JavaParser.cactionProcessObject();
*/
            JavaParser.cactionSetupString();
        }

        //
        //
        //
        if (unit.currentPackage != null) {
            ImportReference importReference = unit.currentPackage;
            StringBuffer package_buffer = new StringBuffer();
            for (int i = 0, tokenArrayLength = importReference.tokens.length; i < tokenArrayLength; i++) {
                String tokenString = new String(importReference.tokens[i]);
                if (i > 0) {
                    package_buffer.append('.');
                }
                package_buffer.append(tokenString);
            }
            package_name = package_buffer.toString();
//System.out.println("Processed package " + package_name);                     
        }
        
        //
        //
        //
        if (unit.imports != null) {
            for (int k = 0; k < unit.imports.length; k++) {
                ImportReference node = unit.imports[k];

                // DQ (9/11/2011): Static analysis tools suggest using StringBuffer instead of String.
                // String importReference = "";
                StringBuffer package_buffer = new StringBuffer();
                for (int i = 0, tokenArrayLength = node.tokens.length; i < tokenArrayLength; i++) {
                    String tokenString = new String(node.tokens[i]);
                    if (i > 0) {
                        package_buffer.append('.');
                    }
                    package_buffer.append(tokenString);
                }

                boolean containsWildcard = ((node.bits & node.OnDemand) != 0);
// TODO: REMOVE THIS
//System.out.println("The import statement " + package_buffer + (containsWildcard ? " contains wildcard" : " does not contain wilcards"));
                String importReferenceWithoutWildcard = package_buffer.toString();
                if (containsWildcard) {
                    package_buffer.append(".*");
                }

                if (JavaParser.verboseLevel > 1)
                    System.out.println("importReference (string) = " + package_buffer.toString());

                // DQ (8/22/2011): Read the referenced class or set of classes defined by the import statement.
                if (! containsWildcard) { // Build support for a real class
                    preprocessClass(importReferenceWithoutWildcard);
//                    JavaParserSupport.buildImplicitClassSupport(importReferenceWithoutWildcard);
                }
                //
                // TODO: REMOVE THIS!!! shpuld not be needed,
                /*
                else { // just prime the system for an "import ... .*;"
                    //              JavaParser.cactionLookupClassType(importReferenceWithoutWildcard);
                    JavaParser.cactionBuildImplicitClassSupportStart(importReferenceWithoutWildcard);
                    JavaParser.cactionBuildImplicitClassSupportEnd(0, importReferenceWithoutWildcard);
                }
                */
            }
        }

        //
        //
        //
        if (unit.types == null) { // No units!?  Process the "null" package anyway.
            JavaParser.cactionPushPackage(package_name);
            JavaParser.cactionPopPackage();
        }
        else {
            for (int i = 0; i < unit.types.length; i++) {
                TypeDeclaration node = unit.types[i];
//System.out.println("identifying type " + new String(node.name));                                     
                identifyUserDefinedTypes(package_name, node);
            }
            
            for (Class cls : userTypeTable.keySet()) {
                if (cls.getEnclosingClass() == null) {
// TODO: REMOVE THIS
//System.out.println("Inserting type " + cls.getCanonicalName());
//
//                    insertClasses(cls);
//                    traverseClass(cls);
                    preprocessClass(cls);
                }
            }
/*
            for (int i = 0; i < unit.types.length; i++) {
                TypeDeclaration node = unit.types[i];
                String typename = package_name + (package_name.length() > 0 ? "." : "") + new String(node.name);
System.out.println("Preprocessing type " + typename);

String name = new String(node.superclass.resolvedType.getPackage().readableName()),
sname = node.superclass.resolvedType.debugName().substring(name.length() + 1);

System.out.println("** The package is: " + name + ";  The class name is " + sname + "; The super class is " + name + "." + sname);

// Replace this simple function call by the code below it because we don't know how to process packages for
// source code yet in ROSE... The problem is that ROSE is file-centric...
//
//                preprocessClass(typename);
//
                try {
                    Class cls = getClassForName(typename);
                    if (cls == null) throw new ClassNotFoundException(typename);                    
                    String canonical_name = cls.getCanonicalName(),
                           class_name = cls.getName(),
                           simple_name = cls.getSimpleName(),
                           class_package = (simple_name.length() < canonical_name.length()
                                                ? canonical_name.substring(0, canonical_name.length() - simple_name.length() -1)
                                                : "");
System.out.println("(1) The canonical name is: " + canonical_name +
                   "; The package is: " + package_name + ";  The class name is " + class_name + ";  The simple class name is " + simple_name);

                    assert(cls.getEnclosingClass() == null);
                    insertType(class_package, cls);
System.out.println("Inserting type " + canonical_name);
                    insertClasses(cls);
                    traverseClass(cls);
                }
                catch (ClassNotFoundException e) {
                    System.out.println("Caught error in JavaParserSupport (Parser failed)");
                    System.err.println(e);

                    System.exit(1);
                 }
             }
             */
        }    
    }
    
    // DQ (8/24/2011): This is the support for information on the Java side that 
    // we would know on the C++ side, but require on the Java side to support the 
    // introspection (which requires fully qualified names).
    // We use a hashmap to store fully qualified names associated with class names.
    // This is used to translate class names used in type references into fully
    // qualified names where they are implicit classes and we require introspection 
    // to support reading them to translate their member data dn function into JNI
    // calls that will force the ROSE AST to be built.
    // private static HashMap<String,String> hashmapOfQualifiedNamesOfClasses;
//    public static HashMap<String,String> hashmapOfQualifiedNamesOfClasses;

    private static int verboseLevel = 0;
/*
    // Initialization function, but be called before we can use member functions in this class.
    public static void initialize(CompilationResult x, int input_verboseLevel) {
        // This has to be set first (required to support source position translation).
        rose_compilationResult = x;

        // DQ (8/24/2011): Added hashmap to support mapping of unqualified class names to qualified class names.
        hashmapOfQualifiedNamesOfClasses = new HashMap<String,String>();

        // Set the verbose level (passed in from ROSE's "-rose:verbose n")
        verboseLevel = input_verboseLevel;
    }

    public static void sourcePosition(ASTNode node) {
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
        int lineNumber = problemStartPosition >= 0
                             ? Util.getLineNumber(problemStartPosition, lineEnds = rose_compilationResult.getLineSeparatorPositions(), 0, lineEnds.length-1)
                             : 0;
        int columnNumber = problemStartPosition >= 0
                               ? Util.searchColumnNumber(rose_compilationResult.getLineSeparatorPositions(), lineNumber, problemStartPosition)
                               : 0;

        if (verboseLevel > 2)
            System.out.println("In JavaParserSupport::sourcePosition(ASTNode): lineNumber = " + lineNumber + " columnNumber = " + columnNumber);

        // Example of how to compute the ending line number and column position of any AST node.
        int problemEndPosition = endingSourcePosition;
        int lineNumber_end = problemEndPosition >= 0
                                 ? Util.getLineNumber(problemEndPosition, lineEnds = rose_compilationResult.getLineSeparatorPositions(), 0, lineEnds.length-1)
                                 : 0;
        int columnNumber_end = problemEndPosition >= 0
                                   ? Util.searchColumnNumber(rose_compilationResult.getLineSeparatorPositions(), lineNumber, problemEndPosition)
                                   : 0;

        if (verboseLevel > 2)
            System.out.println("In JavaParserSupport::sourcePosition(ASTNode): lineNumber_end = " + lineNumber_end + " columnNumber_end = " + columnNumber_end);
    }
*/

    public static void processConstructorDeclarationHeader(ConstructorDeclaration constructor, JavaToken jToken) {
    	assert(! constructor.isDefaultConstructor());
    
        String name = new String(constructor.selector);
        boolean is_native = constructor.isNative();
        boolean is_private = (constructor.binding != null) && (! constructor.binding.isPrivate());
        JavaParser.cactionConstructorDeclarationHeader(name,
                                                       is_native,
                                                       is_private,
                                                       constructor.typeParameters == null ? 0 : constructor.typeParameters.length, 
                                                       constructor.arguments == null ? 0 : constructor.arguments.length,
                                                       constructor.thrownExceptions == null ? 0 : constructor.thrownExceptions.length,
                                                       jToken
                                                      );
         
         
    }
     
    public static void processMethodDeclarationHeader(MethodDeclaration method, JavaToken jToken) {
        String name = new String(method.selector);

        // Setup the function modifiers
        boolean is_abstract = method.isAbstract();
        boolean is_native   = method.isNative();
        boolean is_static   = method.isStatic();
        boolean is_final    = method.binding != null && method.binding.isFinal();
        boolean is_private  = method.binding != null && method.binding.isPrivate();

        // These is no simple function for theses cases.
        boolean is_synchronized = ((method.modifiers & ClassFileConstants.AccSynchronized) != 0);
        boolean is_public       = ((method.modifiers & ClassFileConstants.AccPublic)       != 0);
        boolean is_protected    = ((method.modifiers & ClassFileConstants.AccProtected)    != 0);

        boolean is_strictfp     = method.binding != null && method.binding.isStrictfp();

        // These are always false for member functions.
        boolean is_volatile     = false;
        boolean is_transient    = false;
        
        JavaParser.cactionMethodDeclarationHeader(name,
                                                  is_abstract,
                                                  is_native,
                                                  is_static,
                                                  is_final,
                                                  is_synchronized,
                                                  is_public,
                                                  is_protected,
                                                  is_private,
                                                  is_strictfp, 
                                                  method.typeParameters == null ? 0 : method.typeParameters.length,
                                                  method.arguments == null ? 0 : method.arguments.length,
                                                  method.thrownExceptions == null ? 0 : method.thrownExceptions.length,
                                                  jToken
                                                 );
    }
    
    /**
     * 
     * @param typeClass
     */
    /*
    private static void processType(Class typeClass) {
        // This function processes all the references to types found in data members, function 
        // return types, function argument types, etc.  With each type it is included into a set
        // (if it is not a primitive type) and then an SgClassType is generated in the ROSE AST
        // so that all references to this type can be supported.  Note that for trivial input 
        // codes, most of the referenced classes are implicit classes; reflection is used to 
        // traversal all of these and recursively build all types.  This is part of what is required 
        // to support a consistant AST in ROSE.

        // More inforamtion on what is in the Class (and other reflection classes) can be found at:
        //      http://download.oracle.com/javase/6/docs/api/java/lang/Class.html

        String nestedClassName = typeClass.getName();

        if (verboseLevel > 1)
            System.out.println("In processType(): type = " + typeClass);

        if (verboseLevel > 1) {
            // This code is part of an interogation of the data in the field and needs to be hidden yet available to support debugging.
            // ******************************************************************************
            // System.out.println("type = " + typeClass);

            if (verboseLevel > 5) {
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

        // We don't have to support Java primitive types as classes in the AST (I think).
        if (! typeClass.isPrimitive()) {
            // Check if this is a type (class) that has already been handled.
            if (setOfClasses.contains(typeClass) == false) {
                // Investigate any new type.
                if (typeClass.isArray() == true) {
                    // DQ (4/3/2011): Added support for extracting the base type of an array and recursively processing the base type.

                    if (verboseLevel > 1)
                        System.out.println("Process the base type of the array of type ... base type = " + typeClass.getComponentType());

                    processType(typeClass.getComponentType());

                    // System.out.println("Exiting as a test...");
                    // System.exit(1);
                }
                else {
                    // This is not an array type and not a primitive type (so it should be a class, I think).
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
                    if (implicitClassCounter < implicitClassCounterBound) {
                        // DQ (11/2/2010): comment out this recursive call for now.
                        buildImplicitClassSupport(nestedClassName);
                    }
                    else {
                        if (verboseLevel > 5)
                            System.out.println("WARNING: Exceeded recursion level " + implicitClassCounter + " nestedClassName = " + nestedClassName);
                    }
                }
            }
            else {
                if (verboseLevel > 4)
                    System.out.println("This class has been seen previously: nestedClassName = " + nestedClassName);
                // setOfClasses.add(typeClass);
            }
        }
        else {
            // We might actually do have to include these since they are classes in Java... 
            // What member functions are there on primitive types?
            if (verboseLevel > 4)
                System.out.println("This class is a primitive type (sorry not implemented): type name = " + nestedClassName);
        }
    }
*/

    /**
     * 
     */
    public static String qualifiedName(char [][]tokens) {
        StringBuffer name = new StringBuffer();
        for (int i = 0, tokenArrayLength = tokens.length; i < tokenArrayLength; i++) {
            String tokenString = new String(tokens[i]);

            if (i > 0) {
                name.append('.');
            }

            name.append(tokenString);
        }

        return name.toString();
    }
    
    /**
     * 
     * @param binding
     * @return
     */
    public static String getPackageName(TypeBinding binding) {
        return (binding.getPackage() != null ? new String(binding.getPackage().readableName()) : "");
    }
    
    /**
     * 
     * @param binding
     * @return
     */
    public static String getTypeName(TypeBinding binding) {
        String debug_name = binding.debugName(),
               type_name;
    
        if (binding.isRawType()) {
            assert(debug_name.endsWith("#RAW"));
//System.out.println("Found raw type " + debug_name);    
            type_name = debug_name.substring(0, debug_name.length() - 4);
//System.out.println("Converting it to " + type_name);    
        }
        else {
            String package_name = getPackageName(binding);
            type_name = debug_name.substring(package_name.length() == 0 ? 0 : package_name.length() + 1);
//System.out.println("Found a regular type " + type_name);                
        }

        return type_name;
    }
    

    /**
     * 
     * @param binding
     * @return
     */
    public static String getFullyQualifiedTypeName(TypeBinding binding) {
        String package_name = getPackageName(binding),
               type_name = getTypeName(binding);
        return (package_name.length() == 0 ? type_name : package_name + "." + type_name);
    }

    //
    // If we have an an inner class, get its outermost enclosing parent.
    //
    public static String getMainPackageName(Class base_class, int num) {
        while (base_class.getDeclaringClass() != null) {
            base_class = base_class.getDeclaringClass();
        }
assert(! base_class.isSynthetic());        
        String canonical_name = base_class.getCanonicalName(),
               class_name = base_class.getName(),
               simple_name = base_class.getSimpleName(),
               package_name = (simple_name.length() < canonical_name.length()
                                     ? canonical_name.substring(0, canonical_name.length() - simple_name.length() -1)
                                     : "");
// TODO: REMOVE THIS
//System.out.println("(2, " + num + ") The canonical name is: " + canonical_name + "; The package is: " + package_name + ";  The class name is " + class_name + ";  The simple class name is " + simple_name);
        return package_name;
    }

    public static Class preprocessClass(TypeBinding binding) {
        while (binding.enclosingType() != null) {
            binding = binding.enclosingType();
        }
        return preprocessClass(getFullyQualifiedTypeName(binding));
    }
    
    /**
     * 
     */
    public static Class preprocessClass(String typename) {
        Class cls = getClassForName(typename);
        if (cls != null) {
            preprocessClass(cls);
        }
        return cls;
    }
/*
    public static String cleanTypeName(String typename) {
        if (typename.endsWith("#RAW")) {
System.out.println("Found raw type " + typename);    
            typename = typename.substring(0, typename.length() - 4);
System.out.println("Converting it to " + typename);    
        }
        return typename;
    }
*/
    /**
     * 
     * @param cls
     */
    public static void preprocessClass(Class cls) {
        //
        // If the class in question is an array, get the ultimate base class.
        //
        Class base_class = cls;
        while (base_class.isArray()) {
            base_class = base_class.getComponentType();
        }

        //
        // If we have an an inner class, get its outermost enclosing parent.
        //
        while (base_class.getDeclaringClass() != null) {
            base_class = base_class.getDeclaringClass();
        }

        //
        // If the candidate is not a primitive and has not yet been processed, then process it.
        //
        if (! base_class.isPrimitive()) {
            String canonical_name = base_class.getCanonicalName(),
                   class_name = base_class.getName(),
                   simple_name = base_class.getSimpleName(),
                   package_name = getMainPackageName(base_class, 1); 

// TODO: REMOVE THIS
// System.out.println("(2) The canonical name is: " + canonical_name + "; The package is: " + package_name + ";  The class name is " + class_name + ";  The simple class name is " + simple_name);

           if (! typeExists(package_name, simple_name)) {
                insertType(package_name, base_class);
// TODO: REMOVE THIS
// System.out.println("Inserting type " + canonical_name);
                //
                // TODO:  For now, we make an exception with user-specified classes and do not insert them
                // into the package that they are declared in. From Rose's point of view, these classes will
                // be declared in their respective SgGlobal environment.
                //
                assert(symbolTable.get(package_name).get(simple_name) != null);

                JavaParser.cactionPushPackage(package_name);
                insertClasses(base_class);
                traverseClass(base_class);
                JavaParser.cactionPopPackage();
            }
// TODO: REMOVE THIS
//else {
//System.out.println("The type " + canonical_name  + " has already been processed");
//}
        }
    }


    /**
     * 
     * @param cls
     * 
     * Note that it is very important that a given class and all its inner classes be inserted
     * in the translator prior to traversing the classes to process the members of a given class.
     * This is necessary because Java allows forward references.  For example, a field or method
     * may refer to an inner class which has not yet been processed as its type.
     * 
     */
    private static void insertClasses(Class cls) {
        JavaParser.cactionInsertClassStart(cls.getSimpleName());
        Class innerClasslist[] = cls.getDeclaredClasses();
        for (int i = 0; i < innerClasslist.length; i++) {
            Class inner_class = innerClasslist[i];
            insertClasses(inner_class);
        }
        JavaParser.cactionInsertClassEnd(cls.getSimpleName());
    }


    /**
     * 
     * @param cls
     * @param className
     */
    private static void traverseClass(Class cls) {
// TODO: REMOVE THIS
//System.out.println("Starting with class " + cls.getCanonicalName());
        String class_name = cls.getSimpleName();

        //
        // Get the fields, constructors, and methods used in this class.
        // Note that try ... catch is required for using the reflection support in Java.
        //
        Class super_class = cls.getSuperclass();
        Class interfaceList[] = cls.getInterfaces();
        Method methlist[] = cls.getDeclaredMethods();
        Field fieldlist[] = cls.getDeclaredFields();
        Constructor ctorlist[] = cls.getDeclaredConstructors();
        Class innerClasslist[] = cls.getDeclaredClasses();

        JavaParser.cactionBuildClassSupportStart(class_name, cls.isInterface());
        if (cls.getCanonicalName().equals("java.lang.Object")) { // If we are processing Object, signal this to the translator.
            JavaParser.cactionSetupObject();
        }

        if (verboseLevel > 2)
            System.out.println("After call to cactionBuildClassSupportStart");

        // process the super class
        if (super_class != null) {
            if (verboseLevel > 2) {
                System.out.println("Super Class name = " + super_class.getName());
            }

            preprocessClass(super_class);
            generateAndPushType(super_class);
//            catch (NoClassDefFoundError eee) { 
        }

        // Process the interfaces.
        for (int i = 0; i < interfaceList.length; i++) {
            if (verboseLevel > 2) {
                System.out.println("interface name = " + interfaceList[i].getName());
            }
            preprocessClass(interfaceList[i]);
            generateAndPushType(interfaceList[i]);
        }

        JavaParser.cactionBuildClassExtendsAndImplementsSupport((super_class != null), interfaceList.length);

        //
        // Process the inner classes. Note that the inner classes must be processed first in case
        // one of these types are used for a field or a method. (e.g., See java.net.InetAddress)
        //
        for (int i = 0; i < innerClasslist.length; i++) {
            Class inner_class = innerClasslist[i];
// TODO: REMOVE THIS
//System.out.println("About to process inner class: " + inner_class.getCanonicalName() + " with class name " + inner_class.getName() + " and simple name " + inner_class.getSimpleName() + (inner_class.isSynthetic() ? " (Synthetic)" : ""));
            if (! inner_class.isSynthetic()) {
                traverseClass(inner_class);
            }
        }

        //
        for (int i = 0; i < fieldlist.length; i++) {
            Field fld = fieldlist[i];
            if (fld.isSynthetic()) // skip synthetic fields
                continue;

            if (verboseLevel > 0) {
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

            Class type = fld.getType();
            preprocessClass(type);

            JavaParserSupport.generateAndPushType(type);

            if (verboseLevel > 2)
                System.out.println("Build the data member (field) for name = " + fld.getName());
// TODO: REMOVE THIS
//if (fld.getName().indexOf('$') != -1) System.out.println("*Field " + fld.getName() + " in class " + cls.getCanonicalName());
            JavaParser.cactionBuildFieldSupport(fld.getName());

            if (verboseLevel > 2)
                System.out.println("DONE: Building the data member (field) for name = " + fld.getName());
        }

        
        //
        // Process the constructor parameter types.
        //
        for (int i = 0; i < ctorlist.length; i++) {
            Constructor ct = ctorlist[i];
            if (ct.isSynthetic()) // skip synthetic constructors
                continue;
            Class pvec[] = ct.getParameterTypes();
            for (int j = 0; j < pvec.length; j++) {
                preprocessClass(pvec[j]);
            }
        }

        //
        // Process the method parameter types.
        //
        for (int i = 0; i < methlist.length; i++) {
            Method m = methlist[i];
            if (m.isSynthetic()) // skip synthetic methods
                continue;
            Class pvec[] = m.getParameterTypes();

            // Process the return type (add a class if this is not already in the ROSE AST).
            preprocessClass(m.getReturnType());

            for (int j = 0; j < pvec.length; j++) {
                preprocessClass(pvec[j]);
            }
        }        
        
        
        TypeDeclaration dcl = userTypeTable.get(cls);
        if (dcl != null && dcl.methods != null) {
            for (int i = 0; i < dcl.methods.length; i++) {
                AbstractMethodDeclaration method = dcl.methods[i];
                if (method.isClinit() || method.isDefaultConstructor()) // An initializer block?
                    continue;

// TODO: REMOVE THIS
//System.out.println("Processing a user-defined method " + new String(method.selector));
                MethodBinding method_binding = method.binding;

                if (method.isConstructor()) {
// TODO: REMOVE THIS
//System.out.println(new String(method.selector) + " is a constructor");
                    JavaParser.cactionTypeReference("", "void", new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
                }
                else {
                    if (method_binding.returnType.canBeInstantiated()) {
                        generateAndPushType(method_binding.returnType);
                    }
                    else JavaParser.cactionTypeReference("", method_binding.returnType.debugName(), new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
                }

                Argument args[] = method.arguments;
                TypeVariableBinding type_bindings[] = method_binding.typeVariables;
                
                if (args != null) {
// TODO: REMOVE THIS
//System.out.println("This method has " + args.length + " arguments");
//                    assert(args.length == type_bindings.length);
                    for (int j = 0; j < args.length; j++) {
// TODO: REMOVE THIS
//System.out.println("Processing argument " + j + " of method " + new String(method.selector));
                        Argument arg = args[j];
                        generateAndPushType(arg.type.resolvedType);
                        JavaParser.cactionArgumentEnd(new String(args[j].name), false /* not a Catch argument */, new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
                    }
                }

                //
                // TODO: process Throws list ... not relevant for now because the translator does not handle them yet.
                //
            
                JavaParser.cactionBuildMethodSupport(new String(method.selector),
                                                     method.isConstructor(),
                                                     method.isAbstract(),
                                                     method.isNative(),
                                                     args == null ? 0 : args.length);
            }
        }
        else {
            // A traversal over the constructors will have to look at all types of constructor arguments 
            // and trigger a recursive call to buildImplicitClassSupport() for any new types.
            for (int i = 0; i < ctorlist.length; i++) {
                Constructor ct = ctorlist[i];
                if (ct.isSynthetic()) // skip synthetic constructors
                     continue;

                Class pvec[] = ct.getParameterTypes();

                JavaParser.cactionTypeReference("", "void", new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));

                for (int j = 0; j < pvec.length; j++) {
                    JavaParserSupport.generateAndPushType(pvec[j]);
                    JavaParser.cactionArgumentEnd(ct.getName() + j, false /* not a Catch argument */, new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
                }

                //
                // TODO: process Throws list ... not relevant for now because the translator does not handle them yet.
                //
            
                JavaParser.cactionBuildMethodSupport(class_name /* ct.getName() */,
                                                     true, /* a constructor */
                                                     Modifier.isAbstract(ct.getModifiers()),
                                                     Modifier.isNative(ct.getModifiers()),
                                                     pvec == null ? 0 : pvec.length);
            }
            
            for (int i = 0; i < methlist.length; i++) {
                Method m = methlist[i];

                Class pvec[] = m.getParameterTypes();

                JavaParserSupport.generateAndPushType(m.getReturnType());

                // System.out.println("method name = " + m.getName());
                for (int j = 0; j < pvec.length; j++) {
                    JavaParserSupport.generateAndPushType(pvec[j]);
                    JavaParser.cactionArgumentEnd(m.getName() + j, false /* not a Catch argument */, new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
                }

                //
                // TODO: process Throws list ... not relevant for now because the translator does not handle them yet.
                //
                

// TODO: REMOVE THIS
//System.out.println("*Method " + m.getName() + " in class " + cls.getCanonicalName() + " is " + (Modifier.isAbstract(m.getModifiers()) ? "" : "not ") + "abstract");
                JavaParser.cactionBuildMethodSupport(m.getName().replace('$', '_'),
                                                     false, /* NOT a constructor! */
                                                     Modifier.isAbstract(m.getModifiers()),
                                                     Modifier.isNative(m.getModifiers()),
                                                     pvec == null ? 0 : pvec.length);
            }
        }

        // This wraps up the details of processing all of the child classes (such as forming SgAliasSymbols for them in the global scope).
        JavaParser.cactionBuildClassSupportEnd(class_name);
// TODO: REMOVE THIS
//System.out.println("Done with class " + cls.getCanonicalName());
    }

  
    /*
    public static void buildImplicitClassSupport(String className) {
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

//        if (verboseLevel > 2)
            System.out.println("In buildImplicitClassSupport("+className+"): implicitClassCounter = " + implicitClassCounter);

        // Get the fields, constructors, and methods used in this class.
        // Note that try ... catch is required for using the reflection support in Java.
        try {
            // Class cls = Class.forName("java.lang.String");
            // Class cls = Class.forName("java.lang."+node.receiver.toString());

            if (verboseLevel > 1)
                System.out.println("Generate the implicit Java class for className = " + className + " implicitClassCounter = " + implicitClassCounter);

            // Note that "java.lang" does not appear to be a class (so is that root of all implicitly included classes?).
            // Class cls = Class.forName("java.lang");
            // Class cls = Class.forName("java.io.InputStream");
            Class cls = Class.forName(className);
            String canonical_name = cls.getCanonicalName(),
                   simple_name = cls.getSimpleName(),
                   package_name = (simple_name.length() < canonical_name.length()
                                         ? canonical_name.substring(0, canonical_name.length() - simple_name.length() -1)
                                         : "");
System.out.println("The canonical name is: " + cls.getCanonicalName() +
                   "; The package is: " + package_name + ";  The class name is " + simple_name);

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
            for (int i = 0; i < numberOfFields; i++) {
                Field fld = fieldlist[i];

                if (verboseLevel > 0) {
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

                // DQ (9/9/2011): Bug fix this is not referenced.
                // Type genericType = fld.getGenericType();

                // Note that if we use "nestedClassName = fld.getType().toString();" nestedClassName has the
                // name "class " as a prefix and this causes an error, so use "typeClass.getName()" instead.
                String nestedClassName = typeClass.getName();

                // Replace any names like "java.lang.System" with "java_lang_System".
                // nestedClassName = nestedClassName.replace('.','_');

                // Need to test for: isPrimitive(), isArray(), isInterface(), isAssignableFrom(), isInstance()
                // More documentation at: http://download.oracle.com/javase/1.4.2/docs/api/java/lang/Class.html

                // We can't output the full field in processType() if the type is an array (so this is just debug support).
                if (verboseLevel > 2) {
                    if (typeClass.isArray() == true) {
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
                if (dataMemberCounter < dataMemberCounterBound) {
                    // System.out.println("#############################################################################################");
                    // System.out.println("This call to JavaParserSupport.<() appears to be a problem: nestedClassName = " + nestedClassName);
                    JavaParserSupport.generateAndPushType(typeClass);
                    // System.out.println("DONE: This call to JavaParserSupport.generateType() appears to be a problem: nestedClassName = " + nestedClassName);

                    if (verboseLevel > 2)
                        System.out.println("Build the data member (field) for name = " + fld.getName());

                    // System.out.println("Exiting after call to JavaParserSupport.generateType(typeClass) implicitClassCounter = " + implicitClassCounter);
                    // System.exit(1);

                    // This function assumes that a type has been placed onto the astJavaTypeStack.
                    JavaParser.cactionBuildImplicitFieldSupport(fld.getName());
                }
                else {
                    if (verboseLevel > 2)
                        System.out.println("WARNING: Exceeded data member (field) handling iteration count " + i + " className = " + className);
                }

                if (verboseLevel > 2)
                    System.out.println("DONE: Building the data member (field) for name = " + fld.getName());

                if (implicitClassCounter > 5 && false) {
                    System.out.println("Exiting as a test implicitClassCounter = " + implicitClassCounter);
                    System.exit(1);
                }

                dataMemberCounter++;
            }

            // A traversal over the constructors will have to look at all types of constructor arguments 
            // and trigger a recursive call to buildImplicitClassSupport() for any new types.
            Constructor ctorlist[] = cls.getDeclaredConstructors();
            int constructorMethodCounter = 0;
            for (int i = 0; i < ctorlist.length; i++) {
                Constructor ct = ctorlist[i];
                Class pvec[] = ct.getParameterTypes();

                // Note that I am ignoring the constructor parameter types at the moment.
                if (verboseLevel > 2) {
                    System.out.println("constructor name = " + ct.getName());
                    for (int j = 0; j < pvec.length; j++) {
                         System.out.println("   constructor parameter type = " + pvec[j]);
                    }
                }

                // System.out.println("constructor name = " + ct.getName());
                for (int j = 0; j < pvec.length; j++) {
                    if (verboseLevel > 2)
                        System.out.println("   constructor parameter type = " + pvec[j]);

                    // Process the paramter type (add a class if this is not already in the ROSE AST).
                    processType(pvec[j]);
                }

                // Simplify the generated AST by skipping the construction of all the member functions in each class.
                // We might only want to build those member functions that are referenced in the input program (as an option).
                // JavaParser.cactionBuildImplicitMethodSupport(ct.getName());

                if (constructorMethodCounter < constructorMethodCounterBound) {
                    // Note that we only want to build types for those function that we want to build.
                    // This mechanism is one way to simplify the generated AST for debugging (restricting 
                    // the number of functions built).

                    if (verboseLevel > 2)
                        System.out.println("Push void as a return type for now (ignored because this is a constructor)");

                    // Push a type to serve as the return type which will be ignored for the case of a constructor
                    // (this allows us to reuse the general member function support).
                    JavaParser.cactionTypeReference("void", new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));

                    if (verboseLevel > 2)
                        System.out.println("DONE: Push void as a return type for now (ignored because this is a constructor)");

                    // System.out.println("constructor name = " + ct.getName());
                    for (int j = 0; j < pvec.length; j++) {
                        // If we push all the types onto the stack then we have to build every constructor.
                        if (verboseLevel > 2)
                            System.out.println("This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (constructor): type = " + pvec[j].getName());
                        JavaParserSupport.generateAndPushType(pvec[j]);
                        JavaParser.cactionArgumentEnd(ct.getName() + j, false, // not a Catch argument
                                                      new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
                        if (verboseLevel > 2)
                            System.out.println("DONE: This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (constructor): type = " + pvec[j].getName());
                    }

                    JavaParser.cactionBuildImplicitMethodSupport(ct.getName(), pvec == null ? 0 : pvec.length);
                }
                else {
                    if (verboseLevel > 2)
                        System.out.println("WARNING: Exceeded constructor method handling iteration count " + constructorMethodCounter + " className = " + className);
                }

                constructorMethodCounter++;
            }

            // A traversal over the methods will have to look at all types of method return types and arguments 
            // and trigger a recursive call to buildImplicitClassSupport() for any new types.
            // System.out.println("(skipped method handling) Number of methods = " + methlist.length);
            int methodCounter = 0;
            for (int i = 0; i < methlist.length; i++) {
                Method m = methlist[i];

                Class pvec[] = m.getParameterTypes();

                // Note that I am ignoring the constructor parameter types at the moment.
                if (verboseLevel > 5) {
                    System.out.println("method name = " + m.getName());
                    System.out.println("   method return type = " + m.getReturnType());
                    for (int j = 0; j < pvec.length; j++) {
                        System.out.println("   method parameter type = " + pvec[j]);
                    }
                }

                // Process the return type (add a class if this is not already in the ROSE AST).
                processType(m.getReturnType());

                // System.out.println("method name = " + m.getName());
                for (int j = 0; j < pvec.length; j++) {
                    if (verboseLevel > 4)
                        System.out.println("   method return type = " + m.getReturnType());

                    if (verboseLevel > 4)
                        System.out.println("   method parameter type = " + pvec[j]);

                    // Process the paramter type (add a class if this is not already in the ROSE AST).
                    processType(pvec[j]);
                }

                // Simplify the generated AST by skipping the construction of all the member functions in each class.
                // We might only want to build those member functions that are referenced in the input program (as an option).

                if (methodCounter < methodCounterBound) {
                    // DQ (4/10/11): Fix this to use the proper return type now (pushed onto stack last and interpreted at the return type).
                    // Push a type to serve as the return type which will be ignored for the case of a method
                    // (this allows us to reuse the general member function support).
                    // System.out.println("Testing with made up return type");
                    // JavaParser.cactionGenerateType("void");
                    JavaParserSupport.generateAndPushType(m.getReturnType());

                    // System.out.println("method name = " + m.getName());
                    for (int j = 0; j < pvec.length; j++) {
                        // If we push all the types onto the stack then we have to build every method.
                        if (verboseLevel > 2)
                            System.out.println("This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (method): type = " + pvec[j].getName());
                        JavaParserSupport.generateAndPushType(pvec[j]);
                        JavaParser.cactionArgumentEnd(m.getName(), false, // not a Catch argument
                                                      new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
                        if (verboseLevel > 2)
                            System.out.println("DONE: This call to JavaParserSupport.generateType() pushes a type onto the astJavaTypeStack (method): type = " + pvec[j].getName());
                    }

                    JavaParser.cactionBuildImplicitMethodSupport(m.getName(), pvec == null ? 0 : pvec.length);
                }
                else {
                    if (verboseLevel > 4)
                        System.out.println("WARNING: Exceeded method handling iteration count " + methodCounter + " className = " + className);
                }

                methodCounter++;
            }

            // Process the interfaces.
            int interfaceCounter = 0;
            for (int i = 0; i < interfaceList.length; i++) {
                if (verboseLevel > 2) {
                    System.out.println("interface name = " + interfaceList[i].getName());
                }

                if (interfaceCounter < interfaceCounterBound) {
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
        catch (Throwable e) {
            System.out.println("Caught error in JavaParserSupport (Parser failed)");
            System.err.println(e);

            // I think we could also rethrough using: "throw e;"

            // Make sure we exit on any error so it is caught quickly.
            System.exit(1);
        }
    }
*/

   public static boolean isPrimitiveType ( TypeBinding typeBinding ) {
        switch (typeBinding.id) {
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
            //    case TypeIds.T_JavaLangString:
                return true;

            default:
                return false;
        }
    }


   /*
    public static void generateType(TypeReference node) {
        // This function traverses the type and calls JNI functions to 
        // at the end of the function define a type built in the ROSE 
        // AST and left of the top of the astJavaTypeStack.
        // This is designed as a recursive function.
        if (verboseLevel > 0)
            System.out.println("Inside of generateType(TypeReference)");

        assert(node != null);

        if (verboseLevel > 1) {
            System.out.println("Inside of generateType(TypeReference) TypeReference node                               = " + node);
            System.out.println("Inside of generateType(TypeReference) TypeReference node.implicitConversion            = " + node.implicitConversion);

            // DQ (9/3/2011): This causes too much output.
            // System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType                  = " + node.resolvedType);

            System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType.isArrayType()    = " + node.resolvedType.isArrayType());
            System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType.isGenericType()  = " + node.resolvedType.isGenericType());
            System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType.isClass()        = " + node.resolvedType.isClass());
            System.out.println("Inside of generateType(TypeReference) TypeReference isPrimitiveType(node.resolvedType) = " + isPrimitiveType(node.resolvedType));

            System.out.println("Inside of generateType(TypeReference) TypeReference node.getTypeName()                 = " + node.getTypeName());
            System.out.println("Inside of generateType(TypeReference) TypeReference node.resolvedType.isClass()        = " + (node.resolvedType.isClass() ? "true" : "false"));
        }

        if (node.resolvedType.isArrayType() == true) {
            // TypeBinding baseType = ((ArrayBinding) node.resolvedType).leafComponentType;
            ArrayBinding arrayType = (ArrayBinding) node.resolvedType;
            if (verboseLevel > 1)
                System.out.println("Inside of generateType(TypeReference) ArrayBinding dimensions = " + arrayType.dimensions);
            TypeBinding baseType = arrayType.leafComponentType;
            if (baseType.canBeInstantiated()) {
                preprocessClass(baseType.debugName());
            }
// TODO: Remove this!
/*
else {
System.out.println("The array base type " + baseType.debugName() + " cannot be instantiated");
}
*/
/*
            // This outputs the declartion for the whole class.
            // System.out.println("Inside of generateType(TypeReference) ArrayBinding baseType   = " + baseType);
            if (verboseLevel > 1) {
                System.out.println("Inside of generateType(TypeReference) ArrayBinding baseType (debugName) = " + baseType.debugName());
                System.out.println("Inside of generateType(TypeReference) recursive call to generateType()");
            }
// charles4: 02/24/2012 12:57AM   -- Replace this call by the one below it.               
//               generateType(baseType);
            String package_name = new String(baseType.getPackage().readableName()),
                   simple_name = baseType.debugName().substring(package_name.length() == 0 ? 0 : package_name.length() + 1);

// TODO: REMOVE THIS
//System.out.println("**+ The package is: " + package_name + ";  The class name is " + simple_name);

            JavaParser.cactionGenerateType(package_name,
                                           baseType.debugName().substring(package_name.length() == 0 ? 0 : package_name.length() + 1),
                                           arrayType.dimensions());
        }
        else {
            // NOTE: It would be more elegant to not depend upon the debugName() function.
            String name = node.resolvedType.debugName();
            if (node.resolvedType.canBeInstantiated()) {
                preprocessClass(node.resolvedType.debugName());
/*
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

            // DQ (8/22/2011): The reason why we need this is that the import statement allows for the names to be used unqualified.
            // Once we implement proper support for the import statement then we will be able to search the symbol tables for any type
            // names that we can't identify because they lack name qualification!!!

            // If this is a generic type then the "<name>" has to be separated so we can use only the base name of the class (the raw type name).
            String rawTypeName = name;

            int firstAngleBracket = rawTypeName.indexOf("<",0);
            int lastAngleBracket = rawTypeName.lastIndexOf(">",rawTypeName.length()-1);

            // System.out.println("In generateType(TypeReference): firstAngleBracket = " + firstAngleBracket + " lastAngleBracket = " + lastAngleBracket);
            if (firstAngleBracket > 0 && firstAngleBracket < lastAngleBracket) {
                    rawTypeName = rawTypeName.substring(0,firstAngleBracket);

                    name = rawTypeName;
            }
*/
  /*
                // DQ (8/20/2011): Moved to be after buildImplicitClassSupport().
                String package_name = new String(node.resolvedType.getPackage().readableName()),
                       simple_name = node.resolvedType.debugName().substring(package_name.length() == 0 ? 0 : package_name.length() + 1);

// TODO: REMOVE THIS
//System.out.println("**- The package is: " + package_name + ";  The class name is " + simple_name);

                JavaParser.cactionGenerateType(package_name, simple_name, 0);

            // System.out.println("Exiting as a test (built type name " + name + " by default) in generateType(TypeReference)");
            // System.exit(1);
            }
// TODO: Remove this!
/*
else {
System.out.println("The type " + node.resolvedType.debugName() + " cannot be instantiated");
}
*/
   /*
        }
// TODO: REMOVE THIS
//System.out.println("returning !!!");
    }
    */


    public static void generateAndPushType(Class node) {
        // This function is used to build types that are classes (implicit classes 
        // that already exist (have been built) and thus just need be found and a 
        // reference put onto the astJavaTypeStack).
        if (verboseLevel > 0)
            System.out.println("Inside of generateAndPushType(Class) for class = " + node);

        if (! node.isPrimitive()) {
            // Investigate any new type.
            if (node.isArray() == true) {
                // DQ (3/21/2011): If this is an array of some type then we have to query the base type and for now I will skip this.
                // System.out.println("Skipping case of array of type for now (sorry not implemented)... " + node.getComponentType());

                // Build an integer type instead of an array of the proper type (temporary fix so that I can focus on proper class support).
                // JavaParser.cactionGenerateType("int");
                int num_dimensions = 0;
                Class n = node;
                while (n.isArray()) {
                     num_dimensions++;
                     n = n.getComponentType();
                }

                String canonical_name = n.getCanonicalName(),
                       class_name = n.getName(),
                       simple_name = n.getSimpleName(),
                       package_name = getMainPackageName(n, 2),
                       type_name = canonical_name.substring(package_name.length() == 0 ? 0 : package_name.length() + 1);
// TODO: REMOVE THIS
//System.out.println("(3) The canonical name is: " + node.getCanonicalName() +
//         "; The package is: " + package_name + ";  The class name is " + class_name + ";  The simple class name is " + simple_name + ";  The type name is " + type_name);

                JavaParser.cactionGenerateType(package_name, type_name, num_dimensions);
                JavaParser.cactionArrayTypeReference(package_name, type_name, num_dimensions, new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
            }
            else {
                // Note that "toString()" inserts "class" into the generated name of the type (so use "getName()").
                String className = node.getName();

                // If this is a class type (e.g. in the C++ sense) then we want to build a proper SgClassType IR node.
                // System.out.println("Build a proper class for this type = " + node);
                // System.out.println("Build a proper class for this type = " + className);

                // We know that this name should be interpreted as a proper class so we need to call a specific JNI function to cause it to be generated on the C++ side.
                // JavaParser.cactionGenerateType(className);
                String canonical_name = node.getCanonicalName(),
                       class_name = node.getName(),
                       simple_name = node.getSimpleName(),
                       package_name = getMainPackageName(node, 3),
                       type_name = canonical_name.substring(package_name.length() == 0 ? 0 : package_name.length() + 1);
// TODO: REMOVE THIS
//System.out.println("(4) The canonical name is: " + node.getCanonicalName() +
//         "; The package is: " + package_name  + ";  The class name is " + class_name + ";  The simple class name is " + simple_name + ";  The type name is " + type_name);

                JavaParser.cactionGenerateType(package_name, type_name, 0);
                JavaParser.cactionTypeReference(package_name, type_name, new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
                // System.out.println("Exiting as a test in generateType(Class) (case of proper class)");
                // System.exit(1);
            }
        }
        else {
            if (verboseLevel > 0)
                System.out.println("Build a primitive type: int ");

            String type_name = node.getName();

            JavaParser.cactionGenerateType("" /* primitive type has no package */ , type_name, 0);
            JavaParser.cactionTypeReference("", type_name, new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
        }

        if (verboseLevel > 0)
            System.out.println("Leaving generateType(Class) (case of proper class)");
    }


    /**
     * 
     * @param type_binding
     */
    static public void generateAndPushType(TypeBinding type_binding) {
        if (! type_binding.canBeInstantiated()) {
            JavaParser.cactionTypeReference("", type_binding.debugName(), new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
        }
        else if (type_binding instanceof ArrayBinding) {
            ArrayBinding arrayType = (ArrayBinding) type_binding;
            TypeBinding baseType = arrayType.leafComponentType;

            String package_name = (baseType.getPackage() != null ? new String(baseType.getPackage().readableName()) : ""),
                   type_name = baseType.debugName().substring(package_name.length() == 0 ? 0 : package_name.length() + 1);

// TODO: REMOVE THIS
//System.out.println("**- The package is: " + package_name + ";  The class name is " + type_name);
     
            JavaParser.cactionArrayTypeReference(package_name, type_name, arrayType.dimensions(), new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
        }
        else {
            String package_name = (type_binding.getPackage() != null ? new String(type_binding.getPackage().readableName()) : ""),
                   type_name = type_binding.debugName().substring(package_name.length() == 0 ? 0 : package_name.length() + 1);

// TODO: REMOVE THIS
//System.out.println("**- The package is: " + package_name + ";  The class name is " + type_name);

            JavaParser.cactionTypeReference(package_name, type_name, new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
        }
    }
}

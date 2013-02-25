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
import java.lang.Class;

// DQ (11/1/2010): This improved design separates out the parsing support, from the ECJ AST traversal, and the parser.
class JavaParserSupport {
    public int verboseLevel = 0;

    //
    // Keep track of the position factory for the unit being processed.
    //
    private JavaSourcePositionInformationFactory posFactory = null;

    public JavaToken createJavaToken(ASTNode node) {
        JavaSourcePositionInformation pos = this.posFactory.createPosInfo(node);
        // For now we return dummy text
        return new JavaToken("Dummy JavaToken (see createJavaToken)", pos);
    }

    public JavaToken createJavaToken(ASTNode lnode, ASTNode rnode) {
        JavaSourcePositionInformation pos = this.posFactory.createPosInfo(lnode.sourceStart(), rnode.sourceEnd());
        // For now we return dummy text
        return new JavaToken("Dummy JavaToken (see createJavaToken)", pos);
    }

   //
    // Create a loader for finding classes.
    //
    public ClassLoader pathLoader = null;
    
    //
    // The package of this compilation unit.
    //
    String unitPackageName = "";

    //
    // Map each Type to the list of local and anonymous types that are immediately
    // enclosed in it.
    //
    // Map each anonymous or local type into a triple <package, name, class>.
    //
    public HashMap<TypeDeclaration, ArrayList<TypeDeclaration>> LocalOrAnonymousTypesOf = new HashMap<TypeDeclaration, ArrayList<TypeDeclaration>>();
    class LocalOrAnonymousType {
        public String package_name,
                      typename,
                      simplename;
        public Class cls;
        private boolean isAnonymous;

        public boolean isAnonymous() { return isAnonymous; }
        
        public LocalOrAnonymousType(String package_name, String typename, String simplename, Class cls, boolean isAnonymous) {
            this.package_name = isAnonymous ? package_name : "";
            this.typename     = typename;
            this.simplename   = simplename;
            this.isAnonymous  = isAnonymous;
            
            this.cls = cls;
        }
    }
    public HashMap<TypeDeclaration, LocalOrAnonymousType> localOrAnonymousType = new HashMap<TypeDeclaration, LocalOrAnonymousType>();

    //
    // Create a map to keep track of user-defined type declarations - This 
    // is a map from a Class file to the TypdeDeclaration that produced it.
    //
    public HashMap<Class, TypeDeclaration> userTypeTable = null;
 
    //
    // Create a symbolTable map to keep track of packages and types that have
    // already been encountered. This is a map from a package name to a map
    // that maps a type name to its corresponding class.
    //
    public HashMap<String, HashMap<String, Class>> symbolTable = null;

    //
    // Create a map from user-defined types into an array list of the class members
    // of the type in question sorted by the order in which they were specified.
    //
    public HashMap<TypeDeclaration, ASTNode[]> orderedClassMembers;

    //
    // Map each initializer into a unique name.
    //
    public HashMap<Initializer, String> initializerName;
        
    public JavaParserSupport(String classpath, int input_verbose_level) {
        // Set the verbose level for ROSE specific processing on the Java specific ECJ/ROSE translation.
        this.verboseLevel = input_verbose_level;

        // Reinitialize the type and symbol table for this compilation unit.
        this.userTypeTable = new HashMap<Class, TypeDeclaration>();
        this.symbolTable = new HashMap<String, HashMap<String, Class>>();
        this.orderedClassMembers = new HashMap<TypeDeclaration, ASTNode[]>();
        this.initializerName = new HashMap<Initializer, String>();

        //
        // Now process the classpath 
        //
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

        //
        // Now create a new class loader with the classpath.
        //
        try {
            // Convert File to a URL
            URL[] urls = new URL[files.size()];
            for (int i = 0; i < files.size(); i++) {
                urls[i] = files.get(i).toURI().toURL();
            }

            // Create a new class loader with the directories
            this.pathLoader = new URLClassLoader(urls);
        } catch (MalformedURLException e) {
            System.err.println("(3) Error in processClasspath: " + e.getMessage()); 
            System.exit(1);
        }
    }

    public boolean typeExists(String package_name, String type_name) {
        return (symbolTable.containsKey(package_name) ? symbolTable.get(package_name).containsKey(type_name) : false); 
    }

    public boolean typeExists(String type_name) {
        return typeExists("", type_name); 
    }

    public void insertType(String package_name, String type_name) {
        assert (! (symbolTable.containsKey(package_name) && symbolTable.get(package_name).containsKey(type_name))); 
        if (! symbolTable.containsKey(package_name)) {
            symbolTable.put(package_name, new HashMap<String, Class>());
        }
        symbolTable.get(package_name).put(type_name, null); 
    }
    
    public void insertType(String package_name, Class type) {
        String type_name = new String(type.getSimpleName());
        assert (! (symbolTable.containsKey(package_name) && symbolTable.get(package_name).containsKey(type_name))); 
        if (! symbolTable.containsKey(package_name)) {
            symbolTable.put(package_name, new HashMap<String, Class>());
        }
        symbolTable.get(package_name).put(type_name, type); 
    }

// TODO: REMOVE THIS !!!
/*    
    public Class findClass(String package_name, String type_name) {
        HashMap<String, Class> table = symbolTable.get(package_name);
        int i = type_name.indexOf('.');
        String next_name = (i == -1 ? type_name : type_name.substring(0, i));
        Class cls = (table == null ? null : table.get(next_name));

        while (i > -1) {
            assert (cls != null);
            type_name = type_name.substring(i + 1);
            i = type_name.indexOf('.');
            next_name = (i == -1 ? type_name : type_name.substring(0, i));
            Class members[] = cls.getDeclaredClasses();
            assert(members != null);
            int k;
            for (k = 0; k < members.length; k++) {
                if (members[k].getSimpleName().equals(next_name)) {
                    cls = members[k];
                    break;
                }
            }
            assert(k < members.length);
        }

        return cls;
    }
*/
    
    public Class findPrimitiveClass(BaseTypeBinding binding) {
        if (binding == TypeBinding.INT)
            return java.lang.Integer.TYPE;
        else if (binding == TypeBinding.BYTE)
            return java.lang.Byte.TYPE;
        else if (binding == TypeBinding.SHORT)
            return java.lang.Short.TYPE;
        else if (binding == TypeBinding.CHAR)
            return java.lang.Character.TYPE;
        else if (binding == TypeBinding.LONG)
            return java.lang.Long.TYPE;
        else if (binding == TypeBinding.FLOAT)
            return java.lang.Float.TYPE;
        else if (binding == TypeBinding.DOUBLE)
            return java.lang.Double.TYPE;
        else if (binding == TypeBinding.BOOLEAN)
            return java.lang.Boolean.TYPE;
        else if (binding == TypeBinding.VOID)
            return java.lang.Void.TYPE;
        else if (binding == TypeBinding.NULL) {
            System.out.println();
            System.out.println("Don't Know what to do with ECJ's Null type!");
            System.exit(1);
        }
        System.out.println();
        System.out.println("Don't Know what to do with ECJ's " + binding.getClass().getCanonicalName());
        System.exit(1);
        return null;
    }
    

    // TODO: See error statements below! 
    public Class findClass(TypeBinding binding) {
        Class cls = null;
        if (binding instanceof BaseTypeBinding) {
            cls = findPrimitiveClass((BaseTypeBinding) binding);
        }
        else if (binding instanceof TypeVariableBinding) {
            TypeVariableBinding var_binding = (TypeVariableBinding) binding;
            cls = findClass(var_binding.erasure());
        }
        else if (binding instanceof ArrayBinding) {
            ArrayBinding array_binding = (ArrayBinding) binding;
            String array_name = new String(array_binding.signature());
            cls = getClassForName(array_name.replace('/', '.'));
        }
        else if (binding instanceof LocalTypeBinding) {
            LocalTypeBinding local_binding = (LocalTypeBinding) binding;
            String pool_name = new String(local_binding.constantPoolName());
            cls = getClassForName(pool_name.replace('/', '.'));
        }
        else if (binding.isMemberType()) {
            ReferenceBinding ref_binding = (ReferenceBinding) binding;
            cls = findClass(ref_binding.enclosingType());
            assert(cls != null);
            Class members[] = cls.getDeclaredClasses();
            assert(members != null);
            cls = null;
            for (int k = 0; k < members.length; k++) {
                if (members[k].getSimpleName().equals(new String(ref_binding.sourceName()))) {
                    cls = members[k];
                    break;
                }
            }
        }
        else if (binding instanceof ParameterizedTypeBinding) {
            ParameterizedTypeBinding parm_binding = (ParameterizedTypeBinding) binding;
            cls = findClass(parm_binding.erasure());
        }
        else if (binding instanceof WildcardBinding) {
            System.out.println();
            System.out.println("What(1) !???" + ((WildcardBinding) binding).debugName());
            System.exit(1);
        }

// TODO: Remove this!
//if ((! (binding instanceof BaseTypeBinding)) && (! (binding instanceof TypeVariableBinding))) System.out.println("Found a binding type " + binding.getClass().getCanonicalName() + " for " + binding.debugName() + " and its signature " + new String(binding.signature()) + (cls == getClassForName(new String(binding.signature()).replace('/', '.')) ? " matches " : " does not match"));

        return (cls == null ? getClassForName(getFullyQualifiedTypeName(binding)) : cls);
    }


// TODO: Remove this
/*
    private boolean typeMatch(Type in_type, TypeBinding argument) { 
        if (in_type instanceof TypeVariable){
            TypeVariable<?> type = (TypeVariable<?>) in_type;
            if (type.getName().equals(argument.debugName())) {
                return true;
            }
        }
        else if (in_type instanceof Class){
            Class type_arg = (Class) in_type;
            if (argument instanceof ArrayBinding) {
                if (! type_arg.isArray()) {
                    return false;
                }
                
                Class base_type = type_arg;
                int num_dimensions = 0;
                do { // get the leaf type of the array parameter and count how many dimensions it has.
                    num_dimensions++;
                    base_type = base_type.getComponentType();
                } while (base_type.isArray());
                ArrayBinding array_binding = (ArrayBinding) argument;
                return (num_dimensions ==  array_binding.dimensions() && typeMatch(base_type, array_binding.leafComponentType()));
            }
            else {
                if (type_arg == findClass(argument)) {
                    return true;
                }
            }
        }
        else if (in_type instanceof GenericArrayType){
            GenericArrayType generic_type = (GenericArrayType) in_type;
            if (! argument.isArrayType()) {
                return false;
            }
            ArrayBinding array_binding = (ArrayBinding) argument;
            return typeMatch(generic_type.getGenericComponentType(), array_binding.leafComponentType());
        }
        else if (in_type instanceof ParameterizedType){
            ParameterizedType param_type = (ParameterizedType) in_type;
            if (! (argument instanceof ParameterizedTypeBinding)) {
                return false;
            }
            ParameterizedTypeBinding param_type_binding = (ParameterizedTypeBinding) argument;
//System.out.println("(2) Don't know what to do with parameter type " + param_type.getRawType().getClass().getCanonicalName());
//System.out.println("(2) The argument is " + param_type_binding.leafComponentType().debugName() + " (" + param_type_binding.leafComponentType().getClass().getCanonicalName() + ")");
            return typeMatch(param_type.getRawType(), param_type_binding.leafComponentType());
        }
        else {
            System.out.println("(3) Don't know what to do with parameter type " + in_type.getClass().getCanonicalName() + " and argument " + argument.getClass().getCanonicalName());
            System.exit(1);
        }

        return false;
    }
*/


// TODO: REMOVE THIS !!!
/*
private String getTypeName(Type in_type) { 
    if (in_type instanceof TypeVariable){
        TypeVariable<?> type = (TypeVariable<?>) in_type;
        return type.getName();
    }
    else if (in_type instanceof Class){
        Class class_arg = (Class) in_type;
        return class_arg.getName();
    }
    else if (in_type instanceof GenericArrayType){
        GenericArrayType generic_type = (GenericArrayType) in_type;
        return getTypeName(generic_type.getGenericComponentType());
    }
    else if (in_type instanceof ParameterizedType){
        ParameterizedType param_type = (ParameterizedType) in_type;
        return getTypeName(param_type.getRawType());
    }

    return in_type.getClass().getCanonicalName() + "*";
}
*/
    

//TODO: REMOVE THIS !!!
/*
    Method getRawMethod(ParameterizedMethodBinding parameterized_method_binding) {
        TypeBinding type_binding = parameterized_method_binding.declaringClass;
        TypeBinding arguments[] = parameterized_method_binding.original().parameters;
// TODO: REMOVE THIS !!!        
//        String package_name = getPackageName(type_binding),
//               type_name = getTypeName(type_binding);
//        Class<?> cls = findClass(package_name, type_name);
        
        Class cls = findClass(type_binding);
//if (cls == null){
//System.out.println("(2) Could not find type " + type_binding.debugName() + " with binding type " + type_binding.getClass().getCanonicalName());
//System.exit(1);
//}
        assert(cls != null);
        String method_name = new String(parameterized_method_binding.selector);
        Method methods[] = cls.getDeclaredMethods();
        for (int i = 0; i < methods.length; i++) {
            Method method = methods[i];
            Type[] types = method.getGenericParameterTypes();
            assert(types != null);
            if (types.length == arguments.length && method_name.equals(method.getName())) {
                int j = 0;
                for (; j < types.length; j++) {
                    if (! typeMatch(types[j], arguments[j]))
                        break;
                }
                if (j == types.length) {
                    return method;
                }
            }
        }

        return null;
    }


    Constructor getRawConstructor(ParameterizedMethodBinding parameterized_constructor_binding) {
        TypeBinding type_binding = parameterized_constructor_binding.declaringClass;
        TypeBinding arguments[] = parameterized_constructor_binding.original().parameters;
// TODO: REMOVE THIS!        
//        String package_name = getPackageName(type_binding),
//               type_name = getTypeName(type_binding);
//        Class<?> cls = findClass(package_name, type_name);
        Class cls = findClass(type_binding);
        assert(cls != null);
        Constructor constructors[] = cls.getDeclaredConstructors();
        for (int i = 0; i < constructors.length; i++) {
            Constructor constructor = constructors[i];
            Type[] types = constructor.getGenericParameterTypes();
            assert(types != null);
            if (types.length == arguments.length) {
                int j = 0;
                for (; j < types.length; j++) {
                    if (! typeMatch(types[j], arguments[j]))
                        break;
                }
                if (j == types.length) {
                    return constructor;
                }
            }
        }
                
        return null;
    }
 */


    Method getRawMethod(MethodBinding method_binding) {
        String method_name = new String(method_binding.selector);
        TypeBinding type_binding = method_binding.declaringClass;
        TypeBinding arguments[] = method_binding.parameters;
        Class cls = findClass(type_binding);
        Class<?> parameterTypes[] = new Class<?>[arguments.length];
        for (int i = 0; i < arguments.length; i++) {
            parameterTypes[i] = findClass(arguments[i]);
        }

        Method method = null;
        try {
            method = cls.getDeclaredMethod(method_name, parameterTypes);
        }
        catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
        catch (NoClassDefFoundError e) {
            try {
                //
                // This second attempt is useful in retrieving methods that depend on restricted classes.
                //
                // See http://stackoverflow.com/questions/8869912/what-library-i-need-so-i-can-acces-this-com-sun-image-codec-jpeg-in-java
                //
                // See http://stackoverflow.com/questions/10391271/itext-bouncycastle-classnotfound-org-bouncycastle-asn1-derencodable-and-org-boun
                //
                method = cls.getMethod(method_name, parameterTypes);
            }
            catch (NoSuchMethodException ee) {
                ee.printStackTrace();
            }
            catch (NoClassDefFoundError eee) {
                eee.printStackTrace();
            }
        }

        if (method == null) {
            System.out.println("Could not find method " + new String(method_binding.readableName()) + " declared in " + method_binding.declaringClass.debugName());
            System.exit(1);
        }

        return method;
    }
    
// TODO: Remove this!
/*
Method getRawMethod(MethodBinding method_binding) {
try{
    TypeBinding type_binding = method_binding.declaringClass;
    TypeBinding arguments[] = method_binding.parameters;
    Class cls = findClass(type_binding);
// TODO: Remove this!        
//System.out.println("Looking into class " + cls.getCanonicalName());
    assert(cls != null);
    String method_name = new String(method_binding.selector);
// TODO: Remove this!        
//System.out.println("Looking for method " + method_name + " in class " + cls.getCanonicalName());
    Method methods[] = cls.getDeclaredMethods();
// TODO: Remove this!        
//System.out.println("Looking for method " + new String(method_binding.readableName()) + " declared in " + method_binding.declaringClass.debugName() + " containing " + methods.length + " methopds.");
    for (int i = 0; i < methods.length; i++) {
        Method method = methods[i];
        Type[] types = method.getGenericParameterTypes();
        assert(types != null);
        if (types.length == arguments.length && method_name.equals(method.getName())) {
// TODO: Remove this!
/*
System.out.println("    Looking at method " + new String(method_binding.readableName()) + "(");
for (int j = 0; j < types.length; j++) {
System.out.println(types[i].getClass().getCanonicalName());
if (j + 1 < types.length)
System.out.println(", ");
}
System.out.println(")");
*/
/*
            int j = 0;
            for (; j < types.length; j++) {
                if (! typeMatch(types[j], arguments[j]))
                    break;
            }
            if (j == types.length) {
                return method;
            }
        }
    }
}
catch(NoClassDefFoundError e){
    if (e.getLocalizedMessage().startsWith("com/sun/")) {
        //
        // See http://stackoverflow.com/questions/8869912/what-library-i-need-so-i-can-acces-this-com-sun-image-codec-jpeg-in-java
        //
        System.out.println();
        System.out.println("*** No support yet for classes that depend on the resctricted class " + e.getLocalizedMessage());
        //
        // There are also lots of isses with apache-james-***
        //
        // See http://stackoverflow.com/questions/10391271/itext-bouncycastle-classnotfound-org-bouncycastle-asn1-derencodable-and-org-boun
        //
    }
    else {
        System.out.println("Could not find method " + new String(method_binding.readableName()) + " declared in " + method_binding.declaringClass.debugName());
        e.printStackTrace();
    }
    System.exit(1);
}

    return null;
}
*/


    Constructor getRawConstructor(MethodBinding constructor_binding) {
        TypeBinding type_binding = constructor_binding.declaringClass;
        TypeBinding arguments[] = constructor_binding.parameters;
        Class cls = findClass(type_binding);

        Class<?> parameterTypes[] = new Class<?>[arguments.length];
        for (int i = 0; i < arguments.length; i++) {
            parameterTypes[i] = findClass(arguments[i]);
        }

        Constructor constructor = null;
        try {
            constructor = cls.getDeclaredConstructor(parameterTypes);
        }
        catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
        catch (NoClassDefFoundError e) {
            try {
                //
                // This second attempt is useful in retrieving methods that depend on restricted classes.
                //
                // See http://stackoverflow.com/questions/8869912/what-library-i-need-so-i-can-acces-this-com-sun-image-codec-jpeg-in-java
                //
                // See http://stackoverflow.com/questions/10391271/itext-bouncycastle-classnotfound-org-bouncycastle-asn1-derencodable-and-org-boun
                //
                constructor = cls.getConstructor(parameterTypes);
            }
            catch (NoSuchMethodException ee) {
                ee.printStackTrace();
            }
            catch (NoClassDefFoundError eee) {
                eee.printStackTrace();
            }
        }

        if (constructor == null) {
            System.out.println("Could not find constructor " + new String(constructor_binding.readableName()) + " declared in " + constructor_binding.declaringClass.debugName());
            System.exit(1);
        }

        return constructor;
    }
    
    
// TODO: Remove this!
/*
Constructor getRawConstructor(MethodBinding constructor_binding) {
    TypeBinding type_binding = constructor_binding.declaringClass;
    TypeBinding arguments[] = constructor_binding.parameters;
    Class cls = findClass(type_binding);
    assert(cls != null);
    Constructor constructors[] = cls.getDeclaredConstructors();
    for (int i = 0; i < constructors.length; i++) {
        Constructor constructor = constructors[i];
        Type[] types = constructor.getGenericParameterTypes();
        assert(types != null);
        if (types.length == arguments.length) {
            int j = 0;
            for (; j < types.length; j++) {
                if (! typeMatch(types[j], arguments[j]))
                    break;
            }
            if (j == types.length) {
                return constructor;
            }
        }
    }
            
    return null;
}
*/



    Class getClassForName(String typename) {
        Class cls = null;
        try {
            cls = Class.forName(typename, true, pathLoader);
        }
        catch (ClassNotFoundException e) { // ...
            if (verboseLevel > 0) {
                System.out.println("Class " + typename + " was not found");
                System.out.println("(1) Caught error in JavaParserSupport (Parser failed)");
                System.err.println(e);
                e.printStackTrace();
            }
        }
        catch (Throwable e) {
            if (verboseLevel > 0) {
                System.out.println("Class " + typename + " was not found");
                System.out.println("(2) Caught error in JavaParserSupport (Parser failed)");
                System.err.println(e);
                e.printStackTrace();   
            }

            //
            // Perhaps the class has not yet being loaded at all.  So, try once more ... with feelings.
            //
            try {
                cls = pathLoader.loadClass(typename);
            } catch (Throwable ee) {
                // Did not find the class ... It's ok! 
            }
        }

        return cls;
    }


    /**
     * Quicksort the content of this array in the range low..high.
     *
     * NOTE that the reason why I wrote this function instead of invoking Collection.sort(...)
     * on it is because Collection.sort() invokes Array.sort() which returns a sorted list of
     * clones of the array elements instead of the originals in sorted order.  Since we have
     * several maps that are defined on these nodes, we need to sort them directly and not
     * their clones!!!
     *
     * Low - low index of range to sort.
     * high - high index of range to sort.
     */
    private static void quicksort(ASTNode array[], int low, int high) {
        if (low >= high)
            return;

        ASTNode pivot = array[low];
        int i = low;
        for (int j = low + 1; j <= high; j++) {
            if (array[j].sourceStart < pivot.sourceStart) {
                array[i] = array[j];
                i++;
                array[j] = array[i];
            }
        }
        array[i] = pivot;

        quicksort(array, low, i - 1);
        quicksort(array, i + 1, high);
    }

    /**
     * 
     */
    void identifyUserDefinedTypes(Class cls, TypeDeclaration node) {
        userTypeTable.put(cls, node);   // keep track of user-defined TypeDeclarations

        //
        // First, sort the class members based on the order in which they were specified.
        //
        int num_member_types = (node.memberTypes != null ? node.memberTypes.length : 0),
            num_fields = (node.fields != null ? node.fields.length : 0),
            num_methods = (node.methods != null ? node.methods.length : 0);
        ASTNode node_list[] = new ASTNode[num_member_types + num_fields + num_methods];
        int index = 0;
        for (int i = 0; i < num_member_types; i++) {
            node_list[index++] = node.memberTypes[i];
        }
        for (int i = 0; i < num_fields; i++) {
            node_list[index++] = node.fields[i];
        }
        for (int i = 0; i < num_methods; i++) {
            AbstractMethodDeclaration method = (AbstractMethodDeclaration) node.methods[i];
            node_list[index++] = method;
        }

        quicksort(node_list, 0, node_list.length - 1);
        orderedClassMembers.put(node, node_list);

        //
        // If this type contains inner classes, process them. 
        //
        for (int i = 0; i < num_member_types; i++) { // for each inner type of this type ...
            String typename = new String(node.memberTypes[i].name);
            Class inner[] = cls.getDeclaredClasses();
            int k;
            for (k = 0; k < inner.length; k++) { // ... look for its matching counterpart.
                if (inner[k].getSimpleName().equals(typename)) {
                    identifyUserDefinedTypes(inner[k], node.memberTypes[i]);
                    break;
                }
            }
            assert(k < inner.length);
        }
    }
    
    /**
     * 
     * 
     */
    void identifyUserDefinedTypes(String package_name, TypeDeclaration node) {
        String typename = package_name + (package_name.length() > 0 ? "." : "") + new String(node.name);

        try {
            Class cls = getClassForName(typename);
            if (cls == null) throw new ClassNotFoundException(typename);
            String canonical_name = cls.getCanonicalName(),
                   class_name = cls.getName(),
                   simple_name = cls.getSimpleName(),
                   class_package = (simple_name.length() < canonical_name.length()
                                          ? canonical_name.substring(0, canonical_name.length() - simple_name.length() -1)
                                          : "");

            assert(cls.getEnclosingClass() == null); // not an inner class

            identifyUserDefinedTypes(cls, node);
        }
        catch (ClassNotFoundException e) {
            e.printStackTrace();
            System.out.println("(2) Caught error in JavaParserSupport (Parser failed) - " + typename);
            System.err.println(e);

            System.exit(1);
        }
    }

    /**
     * @param unit
     * 
     */
    public void preprocess(CompilationUnitDeclaration unit) {
        this.posFactory = new JavaSourcePositionInformationFactory(unit);

        //
        // Make sure that Object is processed first!
        //
        if (! typeExists("java.lang", "Object")) {
            preprocessClass("java.lang.Object");
            JavaParser.cactionSetupStringAndClassTypes();
        }

        //
        //
        //
        if (unit.currentPackage != null) {
            ImportReference importReference = unit.currentPackage;
            StringBuffer package_name_buffer = new StringBuffer();
            for (int i = 0, tokenArrayLength = importReference.tokens.length; i < tokenArrayLength; i++) {
                String tokenString = new String(importReference.tokens[i]);
                if (i > 0) {
                    package_name_buffer.append('.');
                }
                package_name_buffer.append(tokenString);
            }
            unitPackageName = package_name_buffer.toString();
        }

        //
        //
        //
        if (unit.types == null || unit.isPackageInfo()) { // No units!?  ... then at least process the package.
            JavaParser.cactionPushPackage(unitPackageName, createJavaToken(unit));
            JavaParser.cactionPopPackage();
        }
        else {
            LocalTypeBinding[] local_types = unit.localTypes;
            if (local_types != null) {
                for (int i = 0; i < local_types.length; i++) {
                    if (local_types[i] != null) {
/**/
                        SourceTypeBinding enclosing_type_binding = (SourceTypeBinding)  (local_types[i].enclosingMethod != null
                                                                                                       ? local_types[i].enclosingMethod.declaringClass
                                                                                                       : local_types[i].enclosingType);
                        TypeDeclaration enclosing_declaration = enclosing_type_binding.scope.referenceContext,
                        declaration = local_types[i].scope.referenceContext;
                        String // pool_name = new String(local_types[i].constantPoolName()),
                        source_name = new String(local_types[i].sourceName());

                        if (verboseLevel > 0) {
                            System.out.println();
                            System.out.println("    unique key           " + ": " + new String(local_types[i].computeUniqueKey()));
                            if (local_types[i].constantPoolName() != null)
                                System.out.println("    constant pool name   " + ": " + new String(local_types[i].constantPoolName()));
                            System.out.println("    genericTypeSignature " + ": " + new String(local_types[i].genericTypeSignature()));
                            System.out.println("    readable name        " + ": " + new String(local_types[i].readableName())); 
                            System.out.println("    short readable name  " + ": " + new String(local_types[i].readableName())); 
                            System.out.println("    signature            " + ": " + new String(local_types[i].signature())); 
                            System.out.println("    source name          " + ": " + new String(local_types[i].sourceName())); 
                            System.out.println("    debug name           " + ": " + local_types[i].debugName()); 
                            System.out.println("    declaration          " + ": " + new String(declaration.name));
                        }

                        if (local_types[i].constantPoolName() != null) {
                            String pool_name = new String(local_types[i].constantPoolName());
                            int index = pool_name.lastIndexOf('/');
                            String package_name = (index == -1 ? "" : pool_name.substring(0, index).replace('/', '.')),
                                   typename = pool_name.substring(index + 1),
                                   simplename = (local_types[i].isAnonymousType()
                                                      ? source_name.substring(4, source_name.indexOf('('))
                                                      : source_name);
                            Class cls = getClassForName(pool_name.replace('/', '.'));
                            if (verboseLevel > 0) {
                                System.out.println("    Local or Anonymous Type " + i + " nested in type " + 
                                                   new String(local_types[i].enclosingType.readableName()) + ": " +
                                                   (local_types[i].isAnonymousType() ? " (Anonymous)" : " (Local)"));
                                System.out.println("    prefix               " + ": " + package_name);
                                System.out.println("    typename             " + ": " + typename);
                                System.out.println("    Class Name           " + ": " + (cls == null ? "What!?" : cls.getCanonicalName()));
                            }
                        }
/**/
                        processLocalOrAnonymousType(local_types[i]);
                    }
// TODO: Remove this !!!
// else System.out.println("A blank Local type was encountered at index " + i);
                }
            }

            for (int i = 0; i < unit.types.length; i++) {
                TypeDeclaration node = unit.types[i];

                if (node.name != TypeConstants.PACKAGE_INFO_NAME) { // ignore package-info declarations
                    identifyUserDefinedTypes(unitPackageName, node);
                }
            }

            for (Class cls : userTypeTable.keySet()) {
                if (cls.getEnclosingClass() == null) { // a top-level class?
                    preprocessClass(cls);
                }
            }
        }    
    }

    /**
     * 
     * @param local_type
     */
    public void processLocalOrAnonymousType(LocalTypeBinding local_type) {
// System.out.println("Looking at binding for type " + local_type.debugName());
          SourceTypeBinding enclosing_type_binding = (SourceTypeBinding)
                                                     (local_type.enclosingMethod != null
                                                           ? local_type.enclosingMethod.declaringClass
                                                           : local_type.enclosingType);
          TypeDeclaration enclosing_declaration = enclosing_type_binding.scope.referenceContext,
                          declaration = local_type.scope.referenceContext;
// TODO: Remove this !
//
// THIS CASE OCCURS WHEN a local type is nested in an unreachable block:
//
//      boolean DEBUG = false;
//
//      if (DEBUG) { .... new Object() { ... }  ... }
//
if (local_type.constantPoolName() == null) {
System.out.println("Type " + local_type.debugName() + " has null constantPoolName");
System.out.println("The ReferenceBinding constantPoolName of " + local_type.debugName() + " is: " + new String(CharOperation.concatWith(local_type.compoundName, '/')));


System.out.println("Local or Anonymous Type nested in type " + 
            new String(local_type.enclosingType.readableName()) + ": " +
            (local_type.isAnonymousType() ? " (Anonymous)" : " (Local)")); 
System.out.println("    unique key           " + ": " + new String(local_type.computeUniqueKey()));
if (local_type.genericTypeSignature() != null)
System.out.println("    genericTypeSignature " + ": " + new String(local_type.genericTypeSignature()));
if (local_type.readableName() != null)
System.out.println("    readable name        " + ": " + new String(local_type.readableName()));
if (local_type.shortReadableName() != null)
System.out.println("    short readable name  " + ": " + new String(local_type.shortReadableName()));
if (local_type.signature() != null)
System.out.println("    signature            " + ": " + new String(local_type.signature()));
if (local_type.sourceName() != null)
System.out.println("    source name          " + ": " + new String(local_type.sourceName()));
System.out.println("    debug name           " + ": " + local_type.debugName()); 
System.out.println("    declaration          " + ": " + new String(declaration.name));
}

          String pool_name = new String(local_type.constantPoolName()),
                 source_name = new String(local_type.sourceName());
          int index = pool_name.lastIndexOf('/');
          String package_name = (index == -1 ? "" : pool_name.substring(0, index).replace('/', '.')),
                 typename = pool_name.substring(index + 1),
                 simplename = (local_type.isAnonymousType()
                                         ? source_name.substring(4, source_name.indexOf('('))
                                         : source_name);
          Class cls = getClassForName(pool_name.replace('/', '.'));

          localOrAnonymousType.put(declaration, new LocalOrAnonymousType(package_name,
                                                                         typename,
                                                                         simplename,
                                                                         cls,
                                                                         local_type.isAnonymousType()));
          identifyUserDefinedTypes(cls, declaration);
          assert(declaration.binding == local_type);
          if (LocalOrAnonymousTypesOf.get(enclosing_declaration) == null) {
              LocalOrAnonymousTypesOf.put(enclosing_declaration, new ArrayList<TypeDeclaration>());
          }
          LocalOrAnonymousTypesOf.get(enclosing_declaration).add(declaration);
// TODO: Remove this !!!
/*
System.out.println("Local or Anonymous Type " + i + " nested in type " + 
                   new String(local_type.enclosingType.readableName()) + ": " +
                   (local_type.isAnonymousType() ? " (Anonymous)" : " (Local)")); 
System.out.println("    unique key           " + ": " + new String(local_type.computeUniqueKey()));
System.out.println("    constant pool name   " + ": " + new String(local_type.constantPoolName()));
System.out.println("    genericTypeSignature " + ": " + new String(local_type.genericTypeSignature()));
System.out.println("    readable name        " + ": " + new String(local_type.readableName())); 
System.out.println("    short readable name  " + ": " + new String(local_type.readableName())); 
System.out.println("    signature            " + ": " + new String(local_type.signature())); 
System.out.println("    source name          " + ": " + new String(local_type.sourceName())); 
System.out.println("    debug name           " + ": " + local_type.debugName()); 
System.out.println("    declaration          " + ": " + new String(declaration.name)); 

System.out.println("    prefix               " + ": " + package_name);
System.out.println("    typename             " + ": " + typename);
System.out.println("    Class Name           " + ": " + (cls == null ? "What!?" : cls.getCanonicalName()));
*/
    }
    

    public void processConstructorDeclarationHeader(ConstructorDeclaration constructor, JavaToken jToken) {
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


    /**
     * 
     * @param method
     * @param jToken
     */
    public void processMethodDeclarationHeader(MethodDeclaration method, JavaToken jToken) {
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
     * @param node
     * @param jToken
     */
    void processQualifiedNameReference(QualifiedNameReference node) {
        JavaToken jToken = createJavaToken(node);

        int type_prefix_length = node.indexOfFirstFieldBinding - 1;
        TypeBinding type_binding = null;
        if (node.binding instanceof FieldBinding) {
            type_binding = ((FieldBinding) node.binding).declaringClass;
            assert(type_binding == node.actualReceiverType);
        }
        else if (node.binding instanceof VariableBinding) {
            assert(type_prefix_length == 0);
        }
        else {
            assert(node.binding instanceof TypeBinding);
            type_binding = (TypeBinding) node.binding;
            type_prefix_length++; // The field in question is a type.
        }

        StringBuffer strbuf = new StringBuffer();
        for (int i = 0; i < type_prefix_length; i++) {
            strbuf.append(node.tokens[i]);
            if (i + 1 < type_prefix_length)
                strbuf.append(".");
        }

        String type_prefix = new String(strbuf);
// TODO: Remove this !!!
//System.out.println("Number of field bindings is " + (node.otherBindings == null ? 0 : node.otherBindings.length));
        if (type_prefix.length() > 0) {
            Class cls = preprocessClass(type_binding);
            assert(cls != null);

            JavaParser.cactionTypeReference(getPackageName(type_binding), getTypeName(type_binding), jToken);
// TODO: Remove this !!!
//System.out.println("(1) Number of tokens traversed " + (node.tokens.length - type_prefix_length));
//System.out.println("(1) The actual receiver type is " + node.actualReceiverType.debugName());
            for (int i = type_prefix_length; i < node.tokens.length; i++) {
                String field = new String(node.tokens[i]);
                JavaParser.cactionFieldReferenceEnd(false /* explicit type not passed */, field, jToken);
            }
        }
        else {
            int field_index = type_prefix_length;
            String first_field = new String(node.tokens[field_index++]);
            assert(node.binding instanceof VariableBinding);

            JavaParser.cactionSingleNameReference("", "", first_field, jToken);

            strbuf = new StringBuffer();
// TODO: Remove this !!!
//System.out.println("(2) Number of tokens traversed " + (node.tokens.length - field_index));            
//System.out.println("(1) The actual receiver type is " + node.actualReceiverType.debugName());
            for (int i = field_index; i < node.tokens.length; i++) {
                String field = new String(node.tokens[i]);

                JavaParser.cactionFieldReferenceEnd(false /* explicit type not passed */, field, jToken);
            
                strbuf.append(field);
                if (i + 1 < node.tokens.length)
                    strbuf.append(".");
            }
            String name = new String(strbuf);
        }
    }


     /**
     * 
     * @param binding
     * @return
     */
    public String getPackageName(TypeBinding binding) {
        if (binding instanceof LocalTypeBinding) {
            TypeDeclaration node = ((LocalTypeBinding) binding).scope.referenceContext;
            assert(node != null);
            LocalOrAnonymousType special_type = localOrAnonymousType.get(node);
            return special_type.package_name;
        }

        return new String(binding.qualifiedPackageName());
    }
    
    /**
     * 
     * @param binding
     * @return
     */
    public String getTypeName(TypeBinding binding) {
        String type_name;

        if (binding instanceof ParameterizedTypeBinding) {
            ParameterizedTypeBinding parm_binding = (ParameterizedTypeBinding) binding;
            binding = parm_binding.erasure();
        }
        
        if (binding instanceof LocalTypeBinding) {
            TypeDeclaration node = ((LocalTypeBinding) binding).scope.referenceContext;
            assert(node != null);
            LocalOrAnonymousType special_type = localOrAnonymousType.get(node);
            type_name = special_type.isAnonymous() ? special_type.typename : special_type.simplename;
        }
        else {
            type_name = new String(binding.qualifiedSourceName());

             if (binding.isArrayType()) { // Remove the suffix:  DIMS X "[]"
                 type_name = type_name.substring(0, type_name.length() - (binding.dimensions() * 2));
             }
        }

        return type_name;
    }

    /**
     * 
     * @param binding
     * @return
     */
    public String getFullyQualifiedTypeName(TypeBinding binding) {
        String package_name = getPackageName(binding),
               type_name = getTypeName(binding),
               full_name = (package_name.length() == 0 ? type_name : package_name + "." + type_name);
        for (int i = 0; i < binding.dimensions(); i++) {
            full_name += "[]";
        }
        return full_name;
    }

    //
    // If we have an an inner class, get its outermost enclosing parent.
    //
    public String getMainPackageName(Class base_class) {
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
        return package_name;
    }

    public Class lookupClass(String typename) {
        Class cls = getClassForName(typename);
        return cls;    
    }


    public Class preprocessClass(TypeBinding binding) {
        if (binding.isArrayType()) {
            ArrayBinding array_binding = (ArrayBinding) binding;
            binding = array_binding.leafComponentType;
        }
    
        while (binding.enclosingType() != null) {
            binding = binding.enclosingType();
        }

        Class cls = null;
        if (! (binding instanceof TypeVariableBinding || binding.isBaseType())) {
            cls = preprocessClass(getFullyQualifiedTypeName(binding));
        }

        return cls;
    }

    private Class preprocessClass(String typename) {
        Class cls = lookupClass(typename);
        if (cls == null) {
            try {
                throw new ClassNotFoundException("*** Fatal Error: Could not load class " + typename);
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
                System.exit(1);
            }
        }
        preprocessClass(cls);

        return cls;
    }
    
    /**
     * 
     * @param cls
     */
    public void preprocessClass(Class cls) {
        if (cls.isAnonymousClass() || cls.isLocalClass()) {
            return; // Anonymous and local classes are processed when they are encountered during the visit.
        }
    
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
                   package_name = getMainPackageName(base_class); 

           if (! typeExists(package_name, simple_name)) {
                insertType(package_name, base_class);
                //
                // TODO:  For now, we make an exception with user-specified classes and do not insert them
                // into the package that they are declared in. From Rose's point of view, these classes will
                // be declared in their respective SgGlobal environment.
                //
                assert(symbolTable.get(package_name).get(simple_name) != null);

                TypeDeclaration node = userTypeTable.get(cls);
                assert(node == null); // TODO: simplify next statement if this is true!
                JavaToken location = (node != null
                                            ? createJavaToken(node)
                                            : new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
                JavaParser.cactionPushPackage(package_name, location);
                insertClasses(base_class);
                traverseClass(base_class);
                JavaParser.cactionPopPackage();
            }
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
    public void insertClasses(Class cls) {
        TypeDeclaration node = userTypeTable.get(cls);
        LocalOrAnonymousType special_type = (node != null ? localOrAnonymousType.get(node) : null);

        JavaToken location = (node != null
                                    ? createJavaToken(node)
                                    : new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));

        String class_name = (special_type != null 
                                           ? (special_type.isAnonymous() ? special_type.typename : special_type.simplename)
                                           : cls.getSimpleName());

        JavaParser.cactionInsertClassStart(class_name, location);
        Class innerClasslist[] = cls.getDeclaredClasses();
        for (int i = 0; i < innerClasslist.length; i++) {
            Class inner_class = innerClasslist[i];
            insertClasses(inner_class);
        }
        JavaParser.cactionInsertClassEnd(class_name, location);
    }

    /**
     * 
     * @param cls
     */
    public void traverseClass(Class cls) {
        assert(cls != null);
// TODO: Remove this !!!
/*
System.out.println("Traversing class " + cls.getCanonicalName());        
System.out.println();
System.out.println("Processing class " + cls.getCanonicalName() + " with type parameters:");        
TypeVariable<Class<?>>[] type_parameters = cls.getTypeParameters();
if (type_parameters != null){
    for (int i = 0; i < type_parameters.length; i++) {
        TypeVariable<Class<?>> type_parameter = type_parameters[i];
        System.out.println("    " + type_parameter.getName());
    }
}
else System.out.println("NO type parameters!!!");
*/
        TypeDeclaration node = userTypeTable.get(cls); // is this a user-defined type?

        LocalOrAnonymousType special_type = (node != null ? localOrAnonymousType.get(node) : null);

        JavaToken location = (node != null
                                    ? createJavaToken(node)
                                    : new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));

        String class_name = (special_type != null 
                                    ? (special_type.isAnonymous() ? special_type.typename : special_type.simplename)
                                    : cls.getSimpleName());

        JavaParser.cactionBuildClassSupportStart(class_name,
                                                 (special_type == null ? "" : special_type.simplename), 
                                                 cls.isInterface(),
                                                 cls.isEnum(),
                                                 special_type != null && special_type.isAnonymous(), // Anonymous class?
                                                 location);

        if (special_type == null && cls.getCanonicalName().equals("java.lang.Object")) { // If we are processing Object, signal this to the translator.
            JavaParser.cactionSetupObject();
        }

        if (verboseLevel > 2)
            System.out.println("After call to cactionBuildClassSupportStart");

        //
        // Get the fields, constructors, and methods used in this class.
        // Note that try ... catch is required for using the reflection support in Java.
        //
        Class super_class = cls.getSuperclass();
        Class interfacelist[] = cls.getInterfaces();

        Method method_list[] = null;
        try {
            method_list = cls.getDeclaredMethods();
        }
        catch (Throwable e) {
            //
            // This can happen if a method result or parameter type is unavailable.  We assume in such a case 
            // that the methods are not invoked and that only the type itself is of any importance. Of course,
            // if this assumption is wrong, the program will fail later.
            //
            if (cls != null) {
                if (verboseLevel > 2) {
                    System.out.println("Problem accessing methods for " + cls.getCanonicalName());
                }
                method_list = new Method[0];
            }
            else {
                e.printStackTrace();
                System.exit(1);
            }
        }

        Field field_list[] = null;
        try {
            field_list = cls.getDeclaredFields();
        }
        catch (Throwable e) {
            //
            // This can happen if a field type is unavailable.  We assume in such a case that
            // the field is not accessed and that only the type itself is of any importance. Of course,
            // if this assumption is wrong, the program will fail later.
            //
            if (cls != null) {
                if (verboseLevel > 2) {
                    System.out.println("Problem accessing fields for " + cls.getCanonicalName());
                }
                field_list = new Field[0];
            }
            else {
                e.printStackTrace();
                System.exit(1);
            }
        }

        Constructor ctor_list[] = null;
        try {
            ctor_list = cls.getDeclaredConstructors();
        }
        catch (Throwable e) {
            //
            // This can happen if a constructor parameter type is unavailable.  We assume in such a case that
            // the constructors are not invoked and that only the type itself is of any importance. Of course,
            // if this assumption is wrong, the program will fail later.
            //
            if (cls != null) {
                if (verboseLevel > 2) {
                    System.out.println("Problem accessing constructors for " + cls.getCanonicalName());
                }
                ctor_list = new Constructor[0];
            }
            else {
                e.printStackTrace();
                System.exit(1);
            }
        }

        Class inner_class_list[] = null;
        try {
            inner_class_list = cls.getDeclaredClasses();
        }
        catch (Throwable e) {
            if (verboseLevel > 2) {
                System.out.println("Problem inner classes for " + cls.getCanonicalName());
            }
            e.printStackTrace();
            System.exit(1);
        }

        TypeVariable<?> parameters[] = cls.getTypeParameters();
 // TODO: Remove this !!!
 /*
 if (parameters.length > 0) {
 System.out.print("Processing parameterized type " + cls.getSimpleName() + "<" + parameters[0].getName());
 for (int i = 1; i < parameters.length; i++) {
     System.out.print(", " + parameters[i].getName());
 }
 System.out.println(">");
 }
 */
         //
         // If this is a generic type, process its type parameters.  Note that it's important to process the type
         // parameters of this "master" type (if any) prior to processing its super class and interfaces as these may
         // themselves be parameterized types that use parameters originally associated with this "master" type.
         //
         for (int i = 0; i < parameters.length; i++) {
              TypeVariable<?> parameter = parameters[i];
              JavaParser.cactionBuildTypeParameter(parameter.getName(), location);
         }

        // process the super class
        if (super_class != null) {
            if (verboseLevel > 2) {
                System.out.println("Super Class name = " + super_class.getName());
            }

            preprocessClass(super_class);

            //
            // When we have access to a user-specified source, we use it in case this type is a parameterized type.
            // Note that we should not user node.superclass here (instead of node.binding.superclass) because in the
            // case of an Anonymous type, node.superclass is null!
            //
            if (node != null && node.binding.superclass != null) {
                generateAndPushType(node.binding.superclass, location);
            }
            else {
                generateAndPushType(super_class);
            }
        }

        // Process the interfaces.
        for (int i = 0; i < interfacelist.length; i++) {
            if (verboseLevel > 2) {
                System.out.println("interface name = " + interfacelist[i].getName());
            }

            preprocessClass(interfacelist[i]);

            //
            // When we have access to a user-specified source, we use it in case this type is a parameterized type.
            //
            if (node != null && node.superInterfaces != null) {
                generateAndPushType(node.superInterfaces[i].resolvedType, createJavaToken(node.superInterfaces[i]));
            }
            else {
                generateAndPushType(interfacelist[i]);
            }
        }

        JavaParser.cactionBuildClassExtendsAndImplementsSupport(parameters.length, (super_class != null), interfacelist.length, location);

        //
        // Process the inner classes. Note that the inner classes must be processed first in case
        // one of these types are used for a field or a method. (e.g., See java.net.InetAddress)
        //
        for (int i = 0; i < inner_class_list.length; i++) {
            Class inner_class = inner_class_list[i];
            if (! inner_class.isSynthetic()) {
                traverseClass(inner_class);
            }
        }

        //
        // Preprocess the types in field declarations
        //
        if (node != null) {
            //
            // For some reason, some times we do not get all the the fields from the Class generated for a
            // type. Thus, if we have access to to the ECJ AST for the type, we traverse its fields directly. 
            //
            if (node.fields != null) {
                for (int i = 0; i < node.fields.length; i++) {
                    FieldDeclaration field_declaration = node.fields[i];
                    if (field_declaration.type != null) { // I am not sure why this is sometimes false!
                        preprocessClass(field_declaration.type.resolvedType);
                    }
                }
            }
        }
        else {
            for (int i = 0; i < field_list.length; i++) {
                Field fld = field_list[i];
                if (fld.isSynthetic()) // skip synthetic fields
                    continue;
                Class type = fld.getType();
                preprocessClass(type);
            }
        }

        //
        // Process the constructor and methods parameter types.
        //
        if (node != null) {
            if (node.methods != null) {
                for (int i = 0; i < node.methods.length; i++) {
                    AbstractMethodDeclaration method_declaration = node.methods[i];
                    MethodBinding method_binding = method_declaration.binding;
                    if (method_binding != null) { // We need this guard because default constructors have no binding.
                        if (! method_binding.isConstructor()) {
                            preprocessClass(method_binding.returnType);
                        }
                        for (int k = 0; k < method_binding.parameters.length; k++) {
                            preprocessClass(method_binding.parameters[k]);
                        }
                    }
                }
            }
        }
        else {
            //
            // Process the constructor parameter types.
            //
            for (int i = 0; i < ctor_list.length; i++) {
                Constructor ct = ctor_list[i];
                if (ct.isSynthetic()) // skip synthetic constructors
                    continue;
                Class pvec[] = ct.getParameterTypes();
                for (int j = 0; j < pvec.length; j++) {
                    preprocessClass(pvec[j]);
                }
// TODO: Remove this !!!            
/*
System.out.println();
System.out.print("    Processing constructor " + ct.getName() + "(");
Type[] types = ct.getGenericParameterTypes();
if (types != null && types.length > 0) {
    assert(pvec.length == types.length);
    System.out.print("(" + pvec[0].getCanonicalName() + ") ");
    if (types[0] == null)
         System.out.print("*");
    else if (types[0] instanceof TypeVariable<?>)
         System.out.print(((TypeVariable<?>) types[0]).getName());
    else if (types[0] instanceof Class)
         System.out.print("+" + ((Class) types[0]).getName());
    else System.out.print("-" + types[0].getClass().getCanonicalName());

    for (int j = 1; j < types.length; j++) {
        System.out.print(", ");
//        System.out.print(types[j] == null ? "*" : (types[j] instanceof TypeVariable ? ((TypeVariable<?>) types[j]).getName() : types[j].getClass().getCanonicalName()));
        System.out.print(", (" + pvec[j].getCanonicalName() + ") ");
        if (types[j] == null)
            System.out.print("*");
        else if (types[j] instanceof TypeVariable){
            TypeVariable<?> type = (TypeVariable<?>) types[j];
            System.out.print("(" + type.getClass().getCanonicalName() + ") " + type.getName());
        }
        else if (types[j] instanceof Class){
            System.out.print("+" + ((Class) types[j]).getName());
        }
        else {
            System.out.print("-" + types[j].getClass().getCanonicalName());
        }
    }
}
else System.out.print("...");
System.out.println(") in class " + cls.getCanonicalName());
*/
            }

            //
            // Process the method parameter types.
            //
            for (int i = 0; i < method_list.length; i++) {
                Method m = method_list[i];
                if (m.isSynthetic()) {// skip synthetic methods
                    continue;
                }

//if (cls.getSimpleName().equals("Enum")) {
//System.out.println();
//System.out.println("Adding method " + m.getName() + " to class " + cls.getCanonicalName());
//}
                Class pvec[] = m.getParameterTypes();

                // Process the return type (add a class if this is not already in the ROSE AST).
                preprocessClass(m.getReturnType());
//if (cls.getSimpleName().equals("Enum"))
//System.out.println("The return type is " + m.getReturnType().getCanonicalName() + " and it has " + pvec.length + " parameters");
                for (int j = 0; j < pvec.length; j++) {
                    preprocessClass(pvec[j]);
//if (cls.getSimpleName().equals("Enum"))
//System.out.println("Parameter " + j + " has type " + pvec[j].getCanonicalName());
                }
            }
        }        

        //
        // If this class is associated with a user-defined type, process the original source in order to
        // obtain accurate location information.
        //
        if (node != null) {
            //
            // Now, traverse the class members in the order in which they were specified.
            //
            ASTNode node_list[] = orderedClassMembers.get(node);
            assert(node_list != null);
            for (int k = 0; k < node_list.length; k++) {
                ASTNode class_member = node_list[k];
                if (class_member instanceof TypeDeclaration) {
                    TypeDeclaration inner_class = (TypeDeclaration) class_member;
                    // Inner classes already processed above.
                    // Do Nothing!
                }
                else if (class_member instanceof FieldDeclaration) {
                    FieldDeclaration field = (FieldDeclaration) class_member;
                    JavaToken field_location = createJavaToken(field);

                    if (field instanceof Initializer) {
                        Initializer initializer = (Initializer) field;
                        String name = k + "block";
                        initializerName.put(initializer, name);
                        JavaParser.cactionBuildInitializerSupport(initializer.isStatic(), name, field_location);
                    }
                    else {
                        generateAndPushType(field.binding.type, field_location);

                        if (verboseLevel > 2)
                            System.out.println("Build the data member (field) for name = " + new String(field.name));

                        JavaParser.cactionBuildFieldSupport(new String(field.name), field_location);
                    }
                }
                else if (class_member instanceof AbstractMethodDeclaration) {
                    AbstractMethodDeclaration method = (AbstractMethodDeclaration) class_member;
                    Argument args[] = method.arguments;
                    if (method.isClinit() || (method.isConstructor() && ((ConstructorDeclaration) method).isDefaultConstructor() && special_type != null && special_type.isAnonymous())) // (args == null || args.length == 0)))
                        continue;

                    //
                    // TODO: Remove this when the feature is implemented
                    //
                    if (method.typeParameters() != null) {
                        System.out.println();
                        System.out.println("*** No support yet for method/constructor type parameters");
                        System.exit(1);
                    }

                    JavaToken method_location = createJavaToken(method);

                    MethodBinding method_binding = method.binding;

                    if (method.isConstructor()) {
                        JavaParser.cactionTypeReference("", "void", method_location);
                    }
                    else {
                        generateAndPushType(method_binding.returnType, method_location);
                    }

                    TypeVariableBinding type_bindings[] = method_binding.typeVariables;

                    JavaToken args_location = null;
                    if (args != null) {
                        args_location = createJavaToken(args[0], args[args.length - 1]); 

                        for (int j = 0; j < args.length; j++) {
                            Argument arg = args[j];
                            JavaToken arg_location = createJavaToken(arg);
                            generateAndPushType(arg.type.resolvedType, arg_location);
                            JavaParser.cactionBuildArgumentSupport(new String(args[j].name),
                                                                   arg.isVarArgs(),
                                                                   arg.binding.isFinal(),
                                                                   arg_location);
                        }
                    }

                    //
                    // TODO: process Throws list ... not relevant for now because the translator does not handle them yet.
                    //

                    String method_name = new String(method.selector);
                    JavaParser.cactionBuildMethodSupport(method_name,
                                                         method.isConstructor(),
                                                         method.isAbstract(),
                                                         method.isNative(),
                                                         args == null ? 0 : args.length,
                                                         true, /* user-defined-method */
                                                         args_location != null ? args_location : method_location,
                                                         method_location);
                }
                else assert(false);
            }
        }
        else {
            //
            // Preprocess the fields in this class.
            //
            for (int i = 0; i < field_list.length; i++) {
                Field fld = field_list[i];
                if (fld.isSynthetic()) // skip synthetic fields
                    continue;

                generateAndPushType(fld.getType());

                if (verboseLevel > 2)
                    System.out.println("Build the data member (field) for name = " + fld.getName());

                JavaParser.cactionBuildFieldSupport(fld.getName(), location);

                if (verboseLevel > 2)
                    System.out.println("DONE: Building the data member (field) for name = " + fld.getName());
            }

            //
            // Process the constructors in this class.
            //
            for (int i = 0; i < ctor_list.length; i++) {
                Constructor ct = ctor_list[i];
                if (ct.isSynthetic()) // skip synthetic constructors
                     continue;

                //
                // TODO: Do something !!!
                //
                // if (ct.getTypeParameters().length > 0) {
                //    System.out.println();
                //    System.out.println("*** No support yet for constructor type parameters");
                //    System.exit(1);
                // }

                Class pvec[] = ct.getParameterTypes();

                JavaParser.cactionTypeReference("", "void", location);

                for (int j = 0; j < pvec.length; j++) {
                    generateAndPushType(pvec[j]);
                    JavaParser.cactionBuildArgumentSupport(ct.getName() + j,
                                                           false, /* mark as not having var args */ 
                                                           true, /* mark as final */ 
                                                           location);
                }

                //
                // TODO: process Throws list ... not relevant for now because the translator does not handle them yet.
                //
            
                JavaParser.cactionBuildMethodSupport(class_name /* ct.getName() */,
                                                     true, /* a constructor */
                                                     Modifier.isAbstract(ct.getModifiers()),
                                                     Modifier.isNative(ct.getModifiers()),
                                                     pvec == null ? 0 : pvec.length,
                                                     false, /* class-defined-method */
                                                     location,
                                                     location);
            }
            
            for (int i = 0; i < method_list.length; i++) {
                Method m = method_list[i];
                if (m.isSynthetic())
                    continue;

                //
                // TODO: Do something !!!
                //
                // if (m.getTypeParameters().length > 0) {
                //    System.out.println();
                //    System.out.println("*** No support yet for method type parameters");
                //    System.exit(1);
                // }

                Class pvec[] = m.getParameterTypes();

                generateAndPushType(m.getReturnType());

                for (int j = 0; j < pvec.length; j++) {
                    generateAndPushType(pvec[j]);
                    JavaParser.cactionBuildArgumentSupport(m.getName() + j,
                                                           false, /* mark as not having var args */
                                                           true,  /* mark as final */ 
                                                           location);
                }

                //
                // TODO: process Throws list ... not relevant for now because the translator does not handle them yet.
                //
                
                JavaParser.cactionBuildMethodSupport(m.getName().replace('$', '_'),
                                                     false, /* NOT a constructor! */
                                                     Modifier.isAbstract(m.getModifiers()),
                                                     Modifier.isNative(m.getModifiers()),
                                                     pvec == null ? 0 : pvec.length,
                                                     false, /* class-defined-method */
                                                     location,
                                                     location);
            }
        }

        // This wraps up the details of processing all of the child classes (such as forming SgAliasSymbols for them in the global scope).
        JavaParser.cactionBuildClassSupportEnd(class_name, location);
    }


    public void generateAndPushType(Class cls) {
        //
        // This function is used to push a type that came from a class.
        //
        if (verboseLevel > 0)
            System.out.println("Inside of generateAndPushType(Class) for class = " + cls);

        TypeDeclaration node = userTypeTable.get(cls);
        JavaToken location = (node != null
                                    ? createJavaToken(node)
                                    : new JavaToken("Dummy JavaToken (see createJavaToken)", new JavaSourcePositionInformation(0)));
 
        int num_dimensions = 0;
        while (cls.isArray()) {
            num_dimensions++;
            cls = cls.getComponentType();
        }

        if (num_dimensions > 0) {
// TODO: Remove this !!!
//            String canonical_name = cls.getCanonicalName(),
//                   package_name = getMainPackageName(cls),
//                   type_name = canonical_name.substring(package_name.length() == 0 ? 0 : package_name.length() + 1);
            generateAndPushType(cls);
            JavaParser.cactionArrayTypeReference(num_dimensions, location);
        }
        else if (cls.isPrimitive()) {
            String type_name = cls.getName();
            JavaParser.cactionTypeReference("", type_name, location);
        }
        else {
            String className = cls.getName();

            String canonical_name = cls.getCanonicalName(),
                   package_name = getMainPackageName(cls),
                   type_name = canonical_name.substring(package_name.length() == 0 ? 0 : package_name.length() + 1);
            JavaParser.cactionTypeReference(package_name, type_name, location);
        }

        if (verboseLevel > 0)
            System.out.println("Leaving generateAndPushType(Class) (case of proper class)");
    }


    /**
     * 
     * @param type_binding
     */
    public void generateAndPushType(TypeBinding type_binding, JavaToken location) {
        if (type_binding.isParameterizedTypeWithActualArguments()) { // isBoundParameterizedType() || type_binding.isUnboundWildcard() || type_binding.isWildcard()) {
            ParameterizedTypeBinding param_type_binding = (ParameterizedTypeBinding) type_binding;
            String package_name = getPackageName(param_type_binding),
                   type_name = getTypeName(param_type_binding);
            if (param_type_binding.arguments != null) {
                for (int i = 0; i < param_type_binding.arguments.length; i++) {
                    assert(param_type_binding.arguments[i] != null);
                    if (! (param_type_binding.arguments[i] instanceof WildcardBinding)) {
                        preprocessClass(param_type_binding.arguments[i]);
                    }
                    generateAndPushType(param_type_binding.arguments[i], location);
                }

                JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                                type_name,
                                                                param_type_binding.arguments.length,
                                                                0,
                                                                location);
            }
            else {
                JavaParser.cactionTypeReference(package_name, type_name, location);
            }
        }
        else if (type_binding instanceof ArrayBinding) {
            ArrayBinding arrayType = (ArrayBinding) type_binding;
            TypeBinding base_type_binding = arrayType.leafComponentType;

            String package_name = getPackageName(base_type_binding),
                   type_name = getTypeName(base_type_binding);
            assert(! (base_type_binding instanceof ArrayBinding));

            if (base_type_binding.isParameterizedTypeWithActualArguments()) {
                ParameterizedTypeBinding param_type_binding = (ParameterizedTypeBinding) base_type_binding;
                if (param_type_binding.arguments != null) {
                    for (int i = 0; i < param_type_binding.arguments.length; i++) {
                        if (! (param_type_binding.arguments[i] instanceof WildcardBinding)) {
                            preprocessClass(param_type_binding.arguments[i]);
                        }
                        generateAndPushType(param_type_binding.arguments[i], location);
                    }

                    JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                                    type_name,
                                                                    param_type_binding.arguments.length,
                                                                    arrayType.dimensions(),
                                                                    location);
                }
                else {
                    generateAndPushType(base_type_binding, location);
                    JavaParser.cactionArrayTypeReference(arrayType.dimensions(), location);
                }
            }
            else {
                generateAndPushType(base_type_binding, location);
                JavaParser.cactionArrayTypeReference(arrayType.dimensions(), location);
            }
        }
        else if (type_binding instanceof TypeVariableBinding) {
            if (type_binding instanceof CaptureBinding) {
                CaptureBinding capture_binding = (CaptureBinding) type_binding;
                if (capture_binding.wildcard != null) {
                    generateAndPushType(capture_binding.wildcard, location);
                }
                else {
                    System.out.println();
                    System.out.println("*** No support yet for Type Variable binding " + new String(type_binding.shortReadableName()) + " with binding type " + type_binding.getClass().getCanonicalName());
                    System.exit(1);
                }
            }
            else {
                Binding binding = ((TypeVariableBinding) type_binding).declaringElement;
// TODO: Remove this !
// System.out.println("Looking at type variable " + type_binding.debugName() + " in containing type " + new String(binding.readableName()));
                if (binding instanceof TypeBinding) {
                    String type_parameter_name = getTypeName(type_binding);
                    TypeBinding enclosing_binding = (TypeBinding) binding;
                    String package_name = getPackageName(enclosing_binding),
                           type_name = getTypeName(enclosing_binding);
// TODO: Remove this !                
// System.out.println("package name = " + package_name + ";  type name = " + type_name + " ;  type parameter name = " + type_parameter_name + " ; declaring element's binding type = " + binding.getClass().getCanonicalName());
                    JavaParser.cactionTypeParameterReference(package_name, type_name, type_parameter_name, location);
                }
                else {
                    System.out.println();
                    System.out.println("*** No support yet for Type Variable " + new String(type_binding.shortReadableName()) + " with binding type " + type_binding.getClass().getCanonicalName() + " enclosed in " + (binding == null ? "?" : binding.getClass().getCanonicalName()));
                    System.exit(1);
                }
            }
        }
        else if (type_binding instanceof BaseTypeBinding ||
                 type_binding instanceof BinaryTypeBinding ||
                 type_binding instanceof MemberTypeBinding ||
                 type_binding instanceof SourceTypeBinding ||
                 type_binding instanceof ParameterizedTypeBinding ||
                 type_binding instanceof RawTypeBinding) {
            String package_name = getPackageName(type_binding),
                   type_name = getTypeName(type_binding);
            JavaParser.cactionTypeReference(package_name, type_name, location);
        }
        else if (type_binding instanceof WildcardBinding) {
            WildcardBinding wildcard_binding = (WildcardBinding) type_binding;
            
            JavaParser.cactionWildcard(location);
            
            if (! wildcard_binding.isUnboundWildcard()) { // there is a bound!
                preprocessClass(wildcard_binding.bound);
                generateAndPushType(wildcard_binding.bound, location);
            }

            JavaParser.cactionWildcardEnd(wildcard_binding.boundKind == Wildcard.UNBOUND, wildcard_binding.boundKind == Wildcard.EXTENDS,  wildcard_binding.boundKind == Wildcard.SUPER, location);
        }
        else {
            System.out.println();
            System.out.println("*** No support yet for " + type_binding.getClass().getCanonicalName() + ": " + type_binding.debugName());
            System.exit(1);
        }
    }

// -------------------------------------------------------------------------------------------
    
    public void translate(CompilationUnitDeclaration unit) {
        // Debugging support...
        if (verboseLevel > 0)
            System.out.println("Start parsing");

        try {
            this.posFactory = new JavaSourcePositionInformationFactory(unit);
            ecjASTVisitor ecjVisitor = new ecjASTVisitor(unit, this);
            unit.traverse(ecjVisitor, unit.scope);
        }
        catch (Throwable e) {
            e.printStackTrace();
            System.out.println("Caught error in JavaParser (Parser failed)");
            System.err.println(e);

            // Make sure we exit as quickly as possible to simplify debugging.
            System.exit(1);
        }

        // Debugging support...
        if (verboseLevel > 0)
            System.out.println("Done parsing");
    }
}

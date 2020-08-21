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

class JavaParserSupport {
    class DuplicateTypeException extends Exception {
        DuplicateTypeException(String s) {
            super(s);
        }
    }

    //
    //
    //
    void resetUnitInfo(UnitInfo unit_info) {
        if (unit_info != null) {
            JavaParser.cactionSetupSourceFilename(unit_info.fileName);
        }
        else {
            clearUnitInfo();
        }
    }
    void clearUnitInfo() {
        JavaParser.cactionClearSourceFilename(); 
    }

    //
    // Capture these basic bindings on the first call to translate().
    //
    TypeBinding objectBinding = null;

    //
    // Map each Type to the list of local and anonymous types that are immediately
    // enclosed in it.
    //
    // Map each anonymous or local type into a triple <package, name, class>.
    //
    public HashMap<TypeDeclaration, ArrayList<TypeDeclaration>> LocalOrAnonymousTypesOf = new HashMap<TypeDeclaration, ArrayList<TypeDeclaration>>();
    class LocalOrAnonymousType {
        public ReferenceBinding enclosingType;
        public String typename,
                      simplename,
                      method_name;

        private boolean isAnonymous;

        public boolean isAnonymous() { return isAnonymous; }
        public ReferenceBinding getEnclosingType() { return enclosingType; }
        
        public String qualifiedName() throws Exception {
            return  (isAnonymous ? (getTypeName(enclosingType) + "." + typename) : (typename + "._" + method_name + "_." + simplename));
        }
        
        public LocalOrAnonymousType(ReferenceBinding enclosing_type, String typename, String simplename, String method_name, boolean isAnonymous) {
            this.enclosingType = enclosing_type;
            this.typename     = typename;
            this.simplename   = simplename;
            this.method_name  = method_name;
            this.isAnonymous  = isAnonymous;
        }
    }
    public HashMap<TypeDeclaration, LocalOrAnonymousType> localOrAnonymousType = new HashMap<TypeDeclaration, LocalOrAnonymousType>();

    //
    // Create a table to keep track of classes that are preprocessed.
    //
    public HashMap<String, Object> classProcessed = new HashMap<String, Object>();
    public HashSet<String> setupClassDone = new HashSet<String>();

    //
    // Create a table to map each unit into its basic information that contains a given user-specified type declaration.
    //
    public HashMap<CompilationUnitDeclaration, UnitInfo> unitInfoTable = new HashMap<CompilationUnitDeclaration, UnitInfo>();
    public HashMap<String, UnitInfo> unitOf = new HashMap<String, UnitInfo>();
    
    //
    // Create a map from user-defined types into an array list of the class members
    // of the type in question sorted by the order in which they were specified.
    //
    public HashMap<TypeDeclaration, ASTNode[]> orderedClassMembers = new HashMap<TypeDeclaration, ASTNode[]>();

    //
    // Map each initializer into a unique name.
    //
    public HashMap<Initializer, String> initializerName = new HashMap<Initializer, String>();

    //
    // Map each block in a given class into a unique index.
    //
    public HashMap<Initializer, Integer> classInitializerTable = new HashMap<Initializer, Integer>(); 
        
    //
    // Map each method declarations in a given type declaration into a unique index.
    //
    public HashMap<String, Integer> typeDeclarationMethodTable = new HashMap<String, Integer>(); 

    //
    // Map each type that was preprocessed from a ReferenceBinding into the last method index that
    // was generated during the preprocessing.
    //
    public HashMap<String, Integer> lastMethodIndexUsed = new HashMap<String, Integer>();

    //
    // Map each method declarations in a given class into a unique index.
    //
    public HashMap<String, HashMap<String, Integer>> classMethodTable = new HashMap<String, HashMap<String, Integer>>(); 

    //
    // Map each Enum type declaration into the index of its values() and valueOf() methods.
    //
    public HashMap<TypeDeclaration, Integer> enumTypeDeclarationToValuesMethodIndexTable = new HashMap<TypeDeclaration, Integer>(); 
    public HashMap<TypeDeclaration, Integer> enumTypeDeclarationToValueOfMethodTable = new HashMap<TypeDeclaration, Integer>(); 

    public HashMap<ReferenceBinding, Integer> enumReferenceBindingToValuesMethodIndexTable = new HashMap<ReferenceBinding, Integer>(); 
    public HashMap<ReferenceBinding, Integer> enumReferenceBindingToValueOfMethodIndexTable = new HashMap<ReferenceBinding, Integer>();
    
    ecjASTVisitor ecjVisitor = null;
    
    /**
     * 
     * Using the first compilation unit encountered, setup the Java environment. 
     * 
     * @param unit
     */
    JavaParserSupport(CompilationUnitDeclaration unit) {
        //
        // First things first!
        //
        //    . Initialize the visitor
        //    . Initialize objectBinding
        //    . preprocess Object class
        //    . Set up String and Class type
        //    . insert Default package
        //
        this.ecjVisitor = new ecjASTVisitor();
    
        UnitInfo unit_info = new UnitInfo(unit,
                                          (unit.currentPackage == null ? "" : new String(CharOperation.concatWith(unit.currentPackage.tokens, '.'))),
                                          new String(unit.getFileName()),
                                          new JavaSourcePositionInformationFactory(unit)
                                         );
        this.objectBinding = unit.scope.getJavaLangObject();
        try {
            setupClass(this.objectBinding, unit_info);
            setupClass(unit_info.unit.scope.getJavaLangString(), unit_info);
            setupClass(unit_info.unit.scope.getJavaLangClass(), unit_info);
        }
        catch (Throwable e) { // If we can't even process the basic types, quit !!!
            e.printStackTrace();
            System.exit(1); // Make sure we exit as quickly as possible to simplify debugging.            
        }
        JavaParser.cactionSetupBasicTypes();
            
        String default_package_name = "";
        JavaParser.cactionPushPackage(default_package_name, unit_info.getDefaultLocation());
        JavaParser.cactionPopPackage();
    }


    /**
     * 
     * @param binding
     * @return
     */
    public String getCanonicalName(ReferenceBinding binding) throws Exception {
        String qualified_name;
        
        if (binding.isParameterizedType()) { // TODO: Review this !!!!
            binding = (ReferenceBinding) ((ParameterizedTypeBinding) binding).erasure();
        }
        
        if (binding.isAnonymousType() || binding.isLocalType()) {
            assert(binding instanceof SourceTypeBinding);
            TypeDeclaration type_declaration = ((SourceTypeBinding) binding).scope.referenceContext;
            LocalOrAnonymousType special_type = localOrAnonymousType.get(type_declaration);
            assert(special_type != null);

            qualified_name = special_type.qualifiedName();
        }
        else {
            String suffix = "";
            while (binding.enclosingType() != null) {
                suffix = "." + new String(binding.sourceName) + suffix; 
                binding = binding.enclosingType();
            }
            qualified_name = new String(CharOperation.concatWith(binding.compoundName, '.')) + suffix;
        }
        return qualified_name;
    }


    /**
     * 
     * @param method_binding
     * @return
     */
    public String getMethodName(MethodBinding method_binding) {
        return new String(method_binding.isConstructor() ? method_binding.declaringClass.sourceName : method_binding.selector);
    }
    

    /**
     * 
     * @param method_binding
     * @return
     */
    public String getMethodKey(MethodBinding method_binding) throws Exception {
        //
        // PC:  I bumped into either a bad bug or an Eclipse feature here. It turns out that the call to computeUniqueKey()
        // on the method binding in the AbstractMethodDeclaration yields a different result than the method binding from 
        // the BinaryTypeBinding for the same method.
        //
        // return new String(method_binding.computeUniqueKey());
        //
        // So, instead of the beautiful code above, we shall use the "patch" below! 
        //
        assert(method_binding != null);
        assert(method_binding.declaringClass != null);
        method_binding = method_binding.original();
        ReferenceBinding declaring_type_binding = (ReferenceBinding) method_binding.declaringClass.erasure();
        String result = getCanonicalName(declaring_type_binding) + "_" + getMethodName(method_binding) + "(";
        if (method_binding.parameters != null) {
            for (int i = 0, length = method_binding.parameters.length; i < length; i++) {
                if (i > 0)
                    result += ", ";
                result += method_binding.parameters[i].erasure().debugName();
            }
        }
        return result + ")";
    }


    /**
     * 
     * @param method_binding
     * @return
     */
    public int getMethodIndex(MethodBinding method_binding) throws Exception {
        assert(method_binding != null); 
        String key = getMethodKey(method_binding);
        Integer index = typeDeclarationMethodTable.get(key);
        return index;
    }

    /**
     * 
     * @param method_declaration
     * @return
     */
    public int getMethodIndex(AbstractMethodDeclaration method_declaration) throws Exception {
        return getMethodIndex(method_declaration.binding); 
    }

    /**
     * 
     * @param method_declaration
     * @param index
     */
    public void setMethodIndex(MethodBinding method_binding, int method_index) throws Exception {
        assert(method_binding != null);
        String key = getMethodKey(method_binding);
        Integer index = typeDeclarationMethodTable.get(key);
        if (index == null || index != method_index) {
            typeDeclarationMethodTable.put(key, method_index);
        }
    }

    /**
     * 
     * @param binding
     * @return
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
            throw new RuntimeException("Don't Know what to do with ECJ's Null type!"); // System.exit(1);
        }
        throw new RuntimeException("Don't Know what to do with ECJ's " + binding.getClass().getCanonicalName()); // System.exit(1);
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
    void identifyUserDefinedTypes(TypeDeclaration node, UnitInfo unit_info) {
        //
        // First, sort the class members based on the order in which they were specified.
        //
        ArrayList<ASTNode> array_list = new ArrayList<ASTNode>();
        for (int i = 0, max = (node.memberTypes != null ? node.memberTypes.length : 0); i < max; i++) {
            array_list.add(node.memberTypes[i]);
        }
        for (int i = 0, max = (node.fields != null ? node.fields.length : 0); i < max; i++) {
            array_list.add(node.fields[i]);
        }
        for (int i = 0, max = (node.methods != null ? node.methods.length : 0); i < max; i++) {
            AbstractMethodDeclaration method = (AbstractMethodDeclaration) node.methods[i];
            array_list.add(method);
        }

        ASTNode node_list[] = array_list.toArray(new ASTNode[array_list.size()]);

        quicksort(node_list, 0, node_list.length - 1);
        orderedClassMembers.put(node, node_list);

        //
        // If this type contains inner classes, process them. 
        //
        for (int i = 0, max = (node.memberTypes != null ? node.memberTypes.length : 0); i < max; i++) { // for each inner type of this type ...
             identifyUserDefinedTypes(node.memberTypes[i], unit_info);
        }
    }
    

    /**
     * @param unit
     * 
     */
    public void preprocess(UnitInfo unit_info, boolean temporary_import_processing) throws Exception {
        CompilationUnitDeclaration unit = unit_info.unit;
    
        //
        // If there is a package statement, process it.
        //
        JavaParser.cactionInsertImportedPackageOnDemand(unit_info.packageName, unit_info.createJavaToken(unit));

        for (int i = 0, max = unit.scope.imports.length; i < max; i++){
            ImportBinding import_binding = unit.scope.imports[i];
            ImportReference node = import_binding.reference;
            JavaToken location = (node != null ? unit_info.createJavaToken(node) : unit_info.getDefaultLocation());
            String import_name = new String(CharOperation.concatWith(import_binding.compoundName, '.'));
            Binding binding = unit.scope.getImport(import_binding.compoundName, import_binding.onDemand, import_binding.isStatic());
            if (binding instanceof PackageBinding) {
                JavaParser.cactionInsertImportedPackageOnDemand(import_name, location);
            }
            else if (binding instanceof ReferenceBinding) {
                ReferenceBinding type_binding = (ReferenceBinding) binding;
                if (temporary_import_processing) {
                    preprocessClass(type_binding, unit_info);
                }
                else {
                    setupClass(type_binding, unit_info);
                }
                if (import_binding.onDemand)
                     JavaParser.cactionInsertImportedTypeOnDemand(getPackageName(type_binding), getTypeName(type_binding), location);
                else JavaParser.cactionInsertImportedType(getPackageName(type_binding), getTypeName(type_binding), location);
            }
            else if (binding instanceof FieldBinding) {
                FieldBinding field_binding = (FieldBinding) binding;
                ReferenceBinding type_binding = field_binding.declaringClass;
                preprocessClass(type_binding, unit_info);
                generateAndPushType(type_binding, unit_info, location);
                JavaParser.cactionInsertImportedStaticField(new String(field_binding.name), location);
            }
        }

        //
        // Process the local and anonymous types associated with this compilation unit.
        //
        if (unit.types != null) { // There are type declarations!?
            //
            // Process the classes for the user defined types here. The classes for the local and anonymous types will be processed during the translation.
            //
            for (int i = 0, max = unit.types.length; i < max; i++) {
                TypeDeclaration node = unit.types[i];
                if (node.name != TypeConstants.PACKAGE_INFO_NAME) { // package-info are not in the class table
                    Object processed_object = classProcessed.get(getCanonicalName(node.binding));
                    if (processed_object == null) { // Type not yet processed. Process this java source.
                        JavaToken location = unit_info.createJavaToken(node);
                        JavaParser.cactionPushPackage(unit_info.packageName, location);
                        insertClasses(node);
                        traverseTypeDeclaration(node, unit_info);
                        JavaParser.cactionPopPackage();
                    }
                    else if (processed_object instanceof TypeDeclaration) { // a previous source was found?
                        throw new DuplicateTypeException("(1) Invalid attempt to redefine the type " + getCanonicalName(node.binding) + 
                                                         " is in file " + new String(node.compilationResult.fileName) +
                                                         ".  The prior definition is in file " + 
                                                         new String(((TypeDeclaration)  processed_object).getCompilationUnitDeclaration().getFileName()));
                    }
                    else { // this type was already processed by a class or a ReferenceBinding?
                        assert(processed_object instanceof ReferenceBinding);
                        ReferenceBinding reference_binding = (ReferenceBinding) processed_object;
                        if (! reference_binding.isEquivalentTo(node.binding)) {
                            throw new DuplicateTypeException("(2) Invalid attempt to redefine the type " + getCanonicalName(reference_binding) + 
                                                             " found in file " + new String(node.binding.getFileName()) +
                                                             ".  The prior definition is in file " +  
                                                             new String(reference_binding.getFileName()));
                        }

                        String package_name = new String(node.binding.getPackage().readableName());
                        JavaToken location = unitInfoTable.get(unit).createJavaToken(node);
                        
                        JavaParser.cactionUpdatePushPackage(package_name, location);
                        updateReferenceBinding(node, unit_info, reference_binding);
                        JavaParser.cactionPopPackage();
                    }
                }
            }

            LocalTypeBinding[] local_types = unit.localTypes;
            if (local_types != null) {
                for (int i = 0; i < local_types.length; i++) {
                    if (local_types[i] != null) {
                        processLocalOrAnonymousType(local_types[i], unit_info);
                    }
                }
            }
        }    
    }

    
    /**
     * 
     * @param local_type
     */
    public void processLocalOrAnonymousType(LocalTypeBinding local_type, UnitInfo unit_info) {
        SourceTypeBinding enclosing_type_binding = (SourceTypeBinding)
                                                   (local_type.enclosingMethod != null
                                                         ? local_type.enclosingMethod.declaringClass
                                                         : local_type.enclosingType);
        TypeDeclaration enclosing_declaration = enclosing_type_binding.scope.referenceContext,
                        declaration = local_type.scope.referenceContext;
        String pool_name = new String(local_type.constantPoolName()),
               source_name = new String(local_type.sourceName());
        int index = pool_name.lastIndexOf('/');
        String // package_name = (index == -1 ? "" : pool_name.substring(0, index).replace('/', '.')),
               typename = pool_name.substring(index + 1),
               simplename = (local_type.isAnonymousType()
                                  ? source_name.substring(4, source_name.indexOf('('))
                                  : source_name);

        localOrAnonymousType.put(declaration, new LocalOrAnonymousType(local_type.enclosingType, // package_name,
                                                                       typename,
                                                                       simplename,
                                                                       (local_type.enclosingMethod != null ? new String(local_type.enclosingMethod.selector) : ""),
                                                                       local_type.isAnonymousType()));
        identifyUserDefinedTypes(declaration, unit_info);
        assert(declaration.binding == local_type);
        if (LocalOrAnonymousTypesOf.get(enclosing_declaration) == null) {
            LocalOrAnonymousTypesOf.put(enclosing_declaration, new ArrayList<TypeDeclaration>());
        }
        LocalOrAnonymousTypesOf.get(enclosing_declaration).add(declaration);
    }
    

    public void processConstructorDeclarationHeader(ConstructorDeclaration constructor, JavaToken jToken) {
        assert(! constructor.isDefaultConstructor());
    
        String name = getMethodName(constructor.binding);
        JavaParser.cactionConstructorDeclarationHeader(name,
                                                       constructor.binding != null && constructor.binding.isPublic(),
                                                       constructor.binding != null && constructor.binding.isProtected(),
                                                       constructor.binding != null && constructor.binding.isPrivate(),
                                                       constructor.typeParameters == null ? 0 : constructor.typeParameters.length, 
                                                       constructor.arguments == null ? 0 : constructor.arguments.length,
                                                       constructor.thrownExceptions == null ? 0 : constructor.thrownExceptions.length,
                                                       jToken
                                                      );
         
         
    }


    /**
     * 
     * @param node
     * @param jToken
     */
    void processQualifiedNameReference(QualifiedNameReference node, Scope scope, UnitInfo unit_info) throws Exception {
        JavaToken jToken = unit_info.createJavaToken(node);

        Binding binding = scope.getPackage(node.tokens);
        PackageBinding package_binding = (binding == null || (! (binding instanceof PackageBinding)) ? null : (PackageBinding) binding);
        int first_type_index = (package_binding == null ? 0 : package_binding.compoundName.length),
            first_field_index = node.indexOfFirstFieldBinding - 1;

        //
        // If there are other bindings associated with this qualified name reference, preprocess them.
        //
        if (node.otherBindings != null) {
            assert(node.otherBindings.length == node.tokens.length - node.indexOfFirstFieldBinding);
            for (int i = 0; i < node.otherBindings.length; i++) {
                preprocessClass(node.otherBindings[i].type, unit_info);
            }
        }

        //
        // If the first name is a type, compute its type binding. 
        //
        ReferenceBinding type_binding = (first_field_index == 0
                                              ? null
                                              : package_binding == null
                                                       ? (ReferenceBinding) scope.getType(node.tokens[first_type_index]) 
                                                       : (ReferenceBinding) package_binding.getTypeOrPackage(node.tokens[first_type_index]));
        if (type_binding != null && type_binding.isValidBinding()) { // a reference type?
            preprocessClass(type_binding, unit_info);

            if (type_binding instanceof TypeVariableBinding) {
                Binding scope_binding = ((TypeVariableBinding) type_binding).declaringElement;
                String type_parameter_name = getTypeName(type_binding);
                if (scope_binding instanceof TypeBinding) {
                    TypeBinding enclosing_binding = (TypeBinding) scope_binding;
                    String package_name = getPackageName(enclosing_binding),
                           type_name = getTypeName(enclosing_binding);
                    JavaParser.cactionTypeParameterReference(package_name, type_name, (int) -1 /* no method index */, type_parameter_name, unit_info.getDefaultLocation());
                }
                else if (scope_binding instanceof MethodBinding) {
                    MethodBinding method_binding = (MethodBinding) scope_binding;
                    AbstractMethodDeclaration method_declaration = method_binding.sourceMethod();
                    int method_index = getMethodIndex(method_binding);
                    TypeBinding enclosing_type_binding = method_binding.declaringClass;
                    String package_name = getPackageName(enclosing_type_binding),
                           type_name = getTypeName(enclosing_type_binding);
                    JavaParser.cactionTypeParameterReference(package_name, type_name, method_index, type_parameter_name, unit_info.getDefaultLocation());
                }
            }
            else {
                String package_name = getPackageName(type_binding),
                       type_name = getTypeName(type_binding);
                JavaParser.cactionTypeReference(package_name, type_name, unit_info.getDefaultLocation());
            }

            int index;
            for (index = first_type_index + 1; index < node.tokens.length; index++) {
                type_binding =  scope.getMemberType(node.tokens[index], type_binding);
                assert(type_binding instanceof ReferenceBinding);
                if (!type_binding.isValidBinding()) { // not a type
                    assert(index == first_field_index);
                    break;
                }
                preprocessClass(type_binding, unit_info);
                JavaParser.cactionQualifiedTypeReference(getPackageName(type_binding), getTypeName(type_binding), unit_info.getDefaultLocation());
            }
            
            assert(node.otherGenericCasts == null); // TODO: we are NOT able to assert this!!! Need to copy code from below here...

            for (int i = index; i < node.tokens.length; i++) {
                String field_name = new String(node.tokens[i]);

                JavaParser.cactionFieldReferenceEnd(false /* explicit type not passed */, field_name, jToken);
            }
        }
        else {
            assert(node.binding instanceof VariableBinding);

            //
            // Preprocess the type of the fields in the qualified name
            //
            VariableBinding variable_binding = (VariableBinding) node.binding;
            preprocessClass(variable_binding.type, unit_info);
            String field_name = new String(node.tokens[first_field_index]);
            JavaParser.cactionSingleNameReference("", "", field_name, jToken);

            for (int i = first_field_index + 1, j = 0; i < node.tokens.length; i++, j++) {
                field_name = new String(node.tokens[i]);
                generateAndPushType(variable_binding.type, unit_info, jToken);
                JavaParser.cactionFieldReferenceEnd(true /* explicit type passed */, field_name, jToken);
                variable_binding = node.otherBindings[j];
                assert(variable_binding != null && new String(variable_binding.name).equals(field_name));
            }
        }
    }


    public void processQualifiedTypeReference(char tokens[][], int length, Scope scope, UnitInfo unit_info) throws Exception {
        Binding binding = scope.getPackage(tokens);
        PackageBinding package_binding = (binding == null || (! (binding instanceof PackageBinding)) ? null : (PackageBinding) binding);
        int first_type_index = (package_binding == null ? 0 : package_binding.compoundName.length);
        ReferenceBinding type_binding = (package_binding == null 
                                                ? (ReferenceBinding) scope.getType(tokens[first_type_index])
                                                : (ReferenceBinding) package_binding.getTypeOrPackage(tokens[first_type_index]));
        assert(type_binding instanceof ReferenceBinding);
        setupClass(type_binding, unit_info);
        JavaParser.cactionTypeReference(getPackageName(type_binding), getTypeName(type_binding), unit_info.getDefaultLocation());
        for (int i = first_type_index + 1; i < length; i++) {
            type_binding =  scope.getMemberType(tokens[i], type_binding);
            assert(type_binding instanceof ReferenceBinding);
            setupClass(type_binding, unit_info);
            JavaParser.cactionQualifiedTypeReference(getPackageName(type_binding), getTypeName(type_binding), unit_info.getDefaultLocation());
        }
    }


    public void processQualifiedParameterizedTypeReference(ParameterizedQualifiedTypeReference node, ASTVisitor visitor, Scope scope, UnitInfo unit_info) throws Exception {
        if (node.resolvedType.isClass() || node.resolvedType.isInterface()) { 
            if (JavaTraversal.verboseLevel > 0)
                System.out.println("(01) The parameterized qualified type referenced is bound to type " + node.resolvedType.debugName());
            setupClass(node.resolvedType, unit_info);
        }

        TypeBinding type_bindings[] = new TypeBinding[node.typeArguments.length];
        TypeBinding type_binding = node.resolvedType;
        int k = node.typeArguments.length;
        do {
             type_bindings[--k] = type_binding;
             type_binding = type_binding.enclosingType();
        } while(type_binding != null);
        
        
        for (int i = k; i < node.typeArguments.length; i++) {
            type_binding = type_bindings[i];
            TypeReference arguments[] = node.typeArguments[i];
            if (arguments != null) {
                for (int j = 0; j < arguments.length; j++) {
                    if (scope instanceof BlockScope)
                         arguments[j].traverse(visitor, (BlockScope) scope);
                    else arguments[j].traverse(visitor, (ClassScope) scope);
                }
            }

            if (i == k) { // first type encountered?
                String package_name = getPackageName(type_binding),
                       type_name = getTypeName(type_binding);
                
                if (arguments == null) { // the first type has no argument?
                    JavaParser.cactionTypeReference(package_name, type_name, unit_info.createJavaToken(node));
                }
                else {
                    JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                                    type_name,
                                                                    arguments != null, // must be true!
                                                                    arguments.length,
                                                                    unit_info.createJavaToken(node));
                }
            }
            else {
                JavaParser.cactionParameterizedQualifiedTypeReferenceEnd(getSimpleTypeName(type_binding), // new String(type_binding.sourceName()),
                                                                         arguments != null,
                                                                         (arguments == null ? 0 : arguments.length),
                                                                         unit_info.createJavaToken(node));
            }
        }
        
        //
        // If this type is an array, process it as such.
        //
        if (node.dimensions() > 0) { // an array?
            JavaParser.cactionArrayTypeReference(node.dimensions(), unit_info.createJavaToken(node));
        }
    }


     /**
     * 
     * @param binding
     * @return
     */
    public String getPackageName(TypeBinding binding) throws Exception {
        if (binding instanceof LocalTypeBinding) {
            TypeDeclaration node = ((LocalTypeBinding) binding).scope.referenceContext;
            assert(node != null);
            LocalOrAnonymousType special_type = localOrAnonymousType.get(node);
            return (special_type.isAnonymous() ? new String(special_type.getEnclosingType().qualifiedPackageName()) : "");
        }

        //
        // If this is a problematic reference binding, use its closest match
        //
        if (binding instanceof ProblemReferenceBinding) {
            throw new Exception("Unresolved type " + ((ProblemReferenceBinding) binding).debugName());
        }

        return new String(binding.qualifiedPackageName());
    }
    
    /**
     * 
     * @param binding
     * @return
     */
    public String getTypeName(TypeBinding binding) throws Exception {
        String type_name;

        //
        // If this is a problematic reference binding, use its closest match
        //
        if (binding instanceof ProblemReferenceBinding) {
            throw new Exception("Unresolved type " + ((ProblemReferenceBinding) binding).debugName());
        }

        if (binding.isArrayType()) { // an array type?
            binding = binding.leafComponentType();
        }
        
        if (binding instanceof ParameterizedTypeBinding) { // a parameterized type?
            ParameterizedTypeBinding parm_binding = (ParameterizedTypeBinding) binding;
            binding = parm_binding.genericType(); // parm_binding.erasure();
        }

        if (binding instanceof LocalTypeBinding) {
            TypeDeclaration node = ((LocalTypeBinding) binding).scope.referenceContext;
            assert(node != null);
            LocalOrAnonymousType special_type = localOrAnonymousType.get(node);
            if (special_type.isAnonymous()) {
                type_name = getTypeName(special_type.getEnclosingType()) + "." + special_type.typename;
            }
            else {
                type_name = special_type.simplename;
            }
        }
        else {
            type_name = new String(binding.qualifiedSourceName());
        }

        return type_name;
    }

    /**
     * 
     * @param binding
     * @return
     */
    public String getSimpleTypeName(TypeBinding binding) throws Exception {
        String type_name;

        //
        // If this is a problematic reference binding, use its closest match
        //
        if (binding instanceof ProblemReferenceBinding) {
            throw new Exception("Unresolved type " + ((ProblemReferenceBinding) binding).debugName());
        }

        if (binding.isArrayType()) { // an array type?
            binding = binding.leafComponentType();
        }
        
        if (binding instanceof ParameterizedTypeBinding) { // a parameterized type?
            ParameterizedTypeBinding parm_binding = (ParameterizedTypeBinding) binding;
            binding = parm_binding.genericType(); // parm_binding.erasure();
        }

        if (binding instanceof LocalTypeBinding) {
            TypeDeclaration node = ((LocalTypeBinding) binding).scope.referenceContext;
            assert(node != null);
            LocalOrAnonymousType special_type = localOrAnonymousType.get(node);
            type_name = (special_type.isAnonymous() ? special_type.typename : special_type.simplename);
        }
        else {
            type_name = new String(binding.sourceName());
        }

        return type_name;
    }

    /**
     * 
     * @param binding
     * @return
     */
    public String getFullyQualifiedTypeName(TypeBinding binding) throws Exception {
        String package_name = getPackageName(binding),
               type_name = getTypeName(binding),
               full_name = (package_name.length() == 0 ? type_name : package_name + "." + type_name);
        for (int i = 0; i < binding.dimensions(); i++) {
            full_name += "[]";
        }
        return full_name;
    }

 
    public void preprocessClass(TypeBinding binding, UnitInfo unit_info) throws Exception {
        if (binding.isBaseType()) { // A primitive type?
            ; // ignore it!
        }
        else if (binding.isArrayType()) { // an array type?
            ArrayBinding array_binding = (ArrayBinding) binding;
            preprocessClass(array_binding.leafComponentType, unit_info);
        }
        else if (binding instanceof TypeVariableBinding) { // A type parameter?
            TypeVariableBinding type_variable_binding = (TypeVariableBinding) binding;

            if (type_variable_binding.superclass != null) {
                preprocessClass(type_variable_binding.superclass, unit_info);
            }
            if (type_variable_binding.superInterfaces != null) {
                for (int k = 0, length = type_variable_binding.superInterfaces.length; k < length; k++) {
                    preprocessClass(type_variable_binding.superInterfaces[k], unit_info);
                }
            }
        }
        else if (binding.enclosingType() != null) { // an inner class?
            preprocessClass(binding.enclosingType(), unit_info);
        }
        else { // Some top-level type that has a ReferenceBinding.
            ReferenceBinding reference_binding = (ReferenceBinding) binding;
            if (reference_binding instanceof ParameterizedTypeBinding) {
                reference_binding = (ReferenceBinding) ((ParameterizedTypeBinding) reference_binding).erasure();
            }
            Object processed_object = classProcessed.get(getCanonicalName(reference_binding));
            if (processed_object == null) { // Type not yet processsed?
                assert(reference_binding instanceof BinaryTypeBinding || reference_binding instanceof SourceTypeBinding);
                String package_name = new String(reference_binding.getPackage().readableName());
                JavaParser.cactionPushPackage(package_name, unit_info.getDefaultLocation());
                insertClasses(reference_binding, unit_info);
                traverseReferenceBinding(reference_binding, unit_info);
                JavaParser.cactionPopPackage();
            }
        }

        return;
    }


    /**
     * 
     * @param binding
     */
    public void insertClasses(ReferenceBinding binding, UnitInfo unit_info) {
        String class_name = new String(binding.sourceName);
        JavaToken location = unit_info.getDefaultLocation();
        JavaParser.cactionInsertClassStart(class_name, 
                                           binding.isInterface(),
                                           binding.isEnum(),
                                           binding.isAnonymousType(),
                                           location);


        ReferenceBinding inner_class_list[] = (binding instanceof BinaryTypeBinding
                                                       ? ((BinaryTypeBinding) binding).memberTypes()
                                                       : binding instanceof SourceTypeBinding
                                                                 ? ((SourceTypeBinding) binding).memberTypes()
                                                                 : ((ParameterizedTypeBinding) binding).memberTypes());

        for (int i = 0; i < inner_class_list.length; i++) {
            insertClasses(inner_class_list[i], unit_info);
        }
        
        JavaParser.cactionInsertClassEnd(class_name, location);
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
    public void insertClasses(TypeDeclaration node) {
        CompilationUnitDeclaration unit = node.getCompilationUnitDeclaration();
        JavaToken location = unitInfoTable.get(unit).createJavaToken(node);

        LocalOrAnonymousType special_type = localOrAnonymousType.get(node);
        String class_name = (special_type != null 
                                           ? (special_type.isAnonymous() ? special_type.typename : special_type.simplename)
                                           : new String(node.name));

        JavaParser.cactionInsertClassStart(class_name,
                                           node.binding.isInterface(),
                                           node.binding.isEnum(),
                                           node.binding.isAnonymousType(),
                                           location);
        TypeDeclaration member_types[] = node.memberTypes;
        if (member_types != null) {
            for (int i = 0; i < member_types.length; i++) {
                insertClasses(member_types[i]);
            }
        }
        JavaParser.cactionInsertClassEnd(class_name, location);
    }


    /**
     * 
     * @param file_name
     * @param package_name
     * @param type_name
     * @return
     */
    boolean hasConflicts(String file_name, String package_name, String type_name) {
        UnitInfo unit_info = unitOf.get(file_name);
        if (unit_info != null) {
            String qualified_name = (package_name.length() == 0 ? "" : (package_name + ".")) + type_name;

            //
            // To start with, look to see if there is a class with the given name in the current package.
            // Next, check to see if there is a conflict with an imported class.
            //
            Binding found = unit_info.unit.scope.fPackage.getTypeOrPackage(type_name.toCharArray());

            //
            // Now look at each import, in turn, to see if there is a conflict.
            //
            for (int i = 0, max = unit_info.unit.scope.imports.length; i < max; i++) {
                ImportBinding import_binding = unit_info.unit.scope.imports[i];
                Binding binding = unit_info.unit.scope.getImport(import_binding.compoundName, import_binding.onDemand, import_binding.isStatic());
                if (binding instanceof ReferenceBinding) {
                    ReferenceBinding reference_binding = (ReferenceBinding) binding;
                    if (reference_binding.debugName().equals(qualified_name)) { // A precise single type import exists for this class.
                        return false;
                    }
                    else if (type_name.equals(new String(reference_binding.shortReadableName()))) {
                        return true;
                    }
                }
                else if (binding instanceof PackageBinding) { // import on demand?
                    PackageBinding package_binding = (PackageBinding) binding;
                    while (package_binding != null) {
                        binding = package_binding.getTypeOrPackage(type_name.toCharArray()); // a hit?
                        package_binding = (binding instanceof PackageBinding ? (PackageBinding) binding : null);
                    } 
                    ReferenceBinding reference_binding = (ReferenceBinding) binding;
                    if (reference_binding != null) {  // a hit?
                        if (found != null) { // a second hit?
                            return true;
                        }
                        found = reference_binding;
                    }
                }
            }
        }

        return false;
    }


    /**
     * 
     * @param node
     * @param method_index
     * 
     * T[] values();
     *
     * First, push the return type for the values function, an one-dimensional array of
     * the enum class in question; next call the method support start, assigning node_list.length
     * as the method index; finally, complete the declaration.
     *
     */
    void addEnumValuesMethodDeclaration(TypeDeclaration node, UnitInfo unit_info, int method_index) throws Exception {
        JavaToken location = unit_info.getDefaultLocation();
        generateAndPushType(node.binding, unit_info, location);
        JavaParser.cactionArrayTypeReference(1, location);

        method_index++; // calculate token index for method "values"
        MethodBinding method_binding = node.binding.getExactMethod("values".toCharArray(), new TypeBinding[0], null);
        setMethodIndex(method_binding, method_index);
        enumTypeDeclarationToValuesMethodIndexTable.put(node, method_index);
        JavaParser.cactionBuildMethodSupportStart("values",
                                                  method_index, 
                                                  location);
        JavaParser.cactionBuildMethodSupportEnd("values",
                                                method_index,
                                                false, // method.isConstructor(),
                                                true,  // method.isAbstract(),
                                                false, // method.isNative(),
                                                0,     // method.typeParameters()
                                                0,     // method.arguments
                                                true,  // compiler-generated
                                                location,
                                                location);
    }

    
    /**
     * 
     * @param node
     * @param method_index
     *
     * T valueOf(String x);
     *
     * First, push the return type for the valueOf function, the enum class in question;
     * next call the method support start, assigning node_list.length + 1 as the method
     * index; next, push the type of the parameter: String; finally, complete the declaration.
     *
     */
    void addEnumValueOfMethodDeclaration(TypeDeclaration node, UnitInfo unit_info, int method_index) throws Exception {
        JavaToken location = unit_info.getDefaultLocation();
        generateAndPushType(node.binding, unit_info, location);
        method_index++; // calculate token index for method "valueOf" - one more than values()'s index.
        TypeBinding string_binding = unit_info.unit.scope.getJavaLangString(); // node.binding.getExactMethod("toString".toCharArray(), new TypeBinding[0], null).returnType;
        MethodBinding method_binding = node.binding.getExactMethod("valueOf".toCharArray(), new TypeBinding[] { string_binding }, null);
        setMethodIndex(method_binding, method_index);
        enumTypeDeclarationToValueOfMethodTable.put(node, method_index);
        JavaParser.cactionBuildMethodSupportStart("valueOf",
                                                  method_index, 
                                                  location);

        generateAndPushType(string_binding, unit_info, location);
        JavaParser.cactionBuildArgumentSupport("arg",
                new String(string_binding.shortReadableName()),
                                               false, // mark as not a var args
                                               false, // mark as not final - does not matter anyway. 
                                               location);

        JavaParser.cactionBuildMethodSupportEnd("valueOf",
                                                method_index, 
                                                false, // method.isConstructor(),
                                                true,  // method.isAbstract(),
                                                false, // method.isNative(),
                                                0,     // method.typeParameters()
                                                1,     // method.arguments
                                                true,  // compiler-generated
                                                location,
                                                location);
    }
    
    
    /**
     * 
     * @param type_binding
     * @param method_index
     * @return
     */
    boolean addEnumValuesMethodDeclaration(ReferenceBinding type_binding, UnitInfo unit_info, int method_index) throws Exception {
        //
        // T[] values();
        // 
        // Check to see if the values() method is contained in type_binding and has been processed already. 
        // 
        MethodBinding method_binding = type_binding.getExactMethod("values".toCharArray(), new TypeBinding[0], null);
        if (method_binding != null && typeDeclarationMethodTable.containsKey(getMethodKey(method_binding))) { // If the values() method was already processed
            int assigned_index = typeDeclarationMethodTable.get(getMethodKey(method_binding)); 
            assert(enumReferenceBindingToValuesMethodIndexTable.get(type_binding) == assigned_index);

            return false;
        }

        //
        // First, push the return type for the values function, an one-dimensional array of
        // the enum class in question; next call the method support start, assigning node_list.length
        // as the method index; finally, complete the declaration.
        //
        JavaToken location = unit_info.getDefaultLocation();
        generateAndPushType(type_binding, unit_info, location);
        JavaParser.cactionArrayTypeReference(1, location);
        method_index++; // calculate token index for method "values"
        setMethodIndex(method_binding, method_index);
        enumReferenceBindingToValuesMethodIndexTable.put(type_binding, method_index);
        JavaParser.cactionBuildMethodSupportStart("values",
                                                  method_index, 
                                                  location);
        JavaParser.cactionBuildMethodSupportEnd("values",
                                                method_index,
                                                false, // method.isConstructor(),
                                                true,  // method.isAbstract(),
                                                false, // method.isNative(),
                                                0,     // method.typeParameters()
                                                0,     // method.arguments
                                                true,  // compiler-generated
                                                location,
                                                location);
        return true;
    }

    
    /**
     * 
     * @param type_binding
     * @param method_index
     * @return
     */
    boolean addEnumValueOfMethodDeclaration(ReferenceBinding type_binding, UnitInfo unit_info, int method_index) throws Exception {
        //
        // T valueOf(String x);
        //
        // 
        // Check to see if the values() method is contained in type_binding and has been processed already. 
        //
        TypeBinding string_binding = unit_info.unit.scope.getJavaLangString(); // type_binding.getExactMethod("toString".toCharArray(), new TypeBinding[0], null).returnType;
        MethodBinding method_binding = type_binding.getExactMethod("valueOf".toCharArray(), new TypeBinding[] { string_binding }, null);
        if (method_binding != null && typeDeclarationMethodTable.containsKey(getMethodKey(method_binding))) { // If the valueOf() method was already processed
            int assigned_index = typeDeclarationMethodTable.get(getMethodKey(method_binding)); 
            assert(enumReferenceBindingToValueOfMethodIndexTable.get(type_binding) == assigned_index);
            
            return false;
        }
        
        // 
        // First, push the return type for the valueOf function, the enum class in question;
        // next call the method support start, assigning node_list.length + 1 as the method
        // index; next, push the type of the parameter: String; finally, complete the declaration.
        //
        JavaToken location = unit_info.getDefaultLocation();
        generateAndPushType(type_binding, unit_info, location);
        method_index++; // calculate token index for method "valueOf" - one more than values()'s index.
        setMethodIndex(method_binding, method_index);
        enumReferenceBindingToValueOfMethodIndexTable.put(type_binding, method_index);
        JavaParser.cactionBuildMethodSupportStart("valueOf",
                                                  method_index, 
                                                  location);

        generateAndPushType(string_binding, unit_info, location);
        JavaParser.cactionBuildArgumentSupport("arg",
                                               new String(string_binding.shortReadableName()),
                                               false, // mark as not a var args
                                               false, // mark as not final - does not matter anyway. 
                                               location);

        JavaParser.cactionBuildMethodSupportEnd("valueOf",
                                                method_index, 
                                                false, // method.isConstructor(),
                                                true,  // method.isAbstract(),
                                                false, // method.isNative(),
                                                0,     // method.typeParameters()
                                                1,     // method.arguments
                                                true,  // compiler-generated
                                                location,
                                                location);

        return true;
    }


    /**
     * 
     * @param node
     * 
     * Just insert this class and its subclasses so that they can be used for declarations.
     */
    public void setupClasses(TypeBinding binding, UnitInfo unit_info) {
        binding = binding.original();
        assert(binding instanceof BinaryTypeBinding || binding instanceof SourceTypeBinding);
        
        JavaToken location = unit_info.getDefaultLocation();
        String class_name = new String(binding.sourceName());
        JavaParser.cactionInsertClassStart(class_name,
                                           binding.isInterface(),
                                           binding.isEnum(),
                                           binding.isAnonymousType(),
                                           location);
        ReferenceBinding member_types[] = (binding instanceof BinaryTypeBinding ? ((BinaryTypeBinding) binding).memberTypes()
                                                                                : ((SourceTypeBinding) binding).memberTypes()); 
        if (member_types != null) {
            for (int i = 0; i < member_types.length; i++) {
                setupClasses(member_types[i], unit_info);
            }
        }
        
        JavaParser.cactionInsertClassEnd(class_name, location);
    }

    /**
     * 
     * @param binding
     */
    void setupClass(TypeBinding binding, UnitInfo unit_info) throws Exception {
        if (binding instanceof ArrayBinding) {
            binding = ((ArrayBinding) binding).leafComponentType;
        }
    
        if ((binding instanceof TypeVariableBinding) || (! (binding instanceof ReferenceBinding))) {
            return;
        }

        for (TypeBinding enclosing_type_binding = binding.original().enclosingType(); enclosing_type_binding != null; enclosing_type_binding = binding.enclosingType()) {
            binding = enclosing_type_binding;
        }
        ReferenceBinding reference_binding = (ReferenceBinding) binding;
        String name = getCanonicalName(reference_binding);
        if ((! setupClassDone.contains(name)) && (! classProcessed.containsKey(name))) { // already processed this type?
            setupClassDone.add(name);
            String package_name = new String(reference_binding.getPackage().readableName());
            JavaParser.cactionPushPackage(package_name, unit_info.getDefaultLocation());
            setupClasses(reference_binding, unit_info);
            JavaParser.cactionPopPackage();
        }
    }
    

    /**
     * 
     * @param unit_info
     * @param method
     * @param method_binding
     * @param method_index
     */
    private void buildMethod(UnitInfo unit_info, AbstractMethodDeclaration method, int method_index) throws Exception {
        JavaToken default_location = unit_info.getDefaultLocation();
        JavaToken method_location = (unit_info == null ? default_location : unit_info.createJavaToken(method));

        String method_name = getMethodName(method.binding);

        setMethodIndex(method.binding, method_index);

        JavaParser.cactionBuildMethodSupportStart(method_name,
                                                  method_index, 
                                                  method_location);

        if (method.typeParameters() != null) {
            TypeParameter type_parameters[] = method.typeParameters();

            ReferenceBinding type_binding = method.binding.declaringClass;
            String package_name = getPackageName(type_binding),
                   type_name = getTypeName(type_binding);
            
            for (int i = 0; i < type_parameters.length; i++) {
                TypeParameter type_parameter = type_parameters[i];
                JavaToken parameter_location = (unit_info == null ? default_location : unit_info.createJavaToken(type_parameter));

                JavaParser.cactionInsertTypeParameter(type_parameter.binding.debugName(), parameter_location);
            }

            for (int i = 0; i < type_parameters.length; i++) {
                TypeParameter type_parameter = type_parameters[i];
                JavaToken parameter_location = (unit_info == null ? default_location : unit_info.createJavaToken(type_parameter));
                TypeVariableBinding type_variable = type_parameter.binding;
                
                if (type_variable.superclass == type_variable.firstBound) {
                    setupClass(type_variable.superclass, unit_info);
                    generateAndPushType(type_variable.superclass, unit_info, parameter_location);
                }
                if (type_variable.superInterfaces != null) {
                    for (int k = 0, length = type_variable.superInterfaces.length; k < length; k++) {
                        setupClass(type_variable.superInterfaces[k], unit_info);
                        generateAndPushType(type_variable.superInterfaces[k], unit_info, parameter_location);
                    }
                }
                
                JavaParser.cactionBuildTypeParameterSupport(package_name,
                                                            type_name,
                                                            method_index,
                                                            type_parameter.binding.debugName(),
                                                            type_variable.boundsCount(),
                                                            parameter_location);
            }
        }
    
        if (method.isConstructor()) {
            JavaParser.cactionTypeReference("", "void", method_location);
        }
        else {
            generateAndPushType(method.binding.returnType, unit_info, method_location);
        }

        TypeVariableBinding type_bindings[] = method.binding.typeVariables;

        Argument args[] = method.arguments;
        JavaToken args_location = null;
        if (args != null) {
            args_location = (unit_info == null ? default_location : unit_info.createJavaToken(args[0], args[args.length - 1]));
            for (int j = 0; j < args.length; j++) {
                Argument arg = args[j];
                JavaToken arg_location = (unit_info == null ? default_location : unit_info.createJavaToken(arg));
                generateAndPushType(arg.type.resolvedType, unit_info, arg_location);
                String argument_name = new String(arg.name);
                JavaParser.cactionBuildArgumentSupport(argument_name,
                                                       arg.type.print(0, new StringBuffer()).toString(),
                                                       arg.isVarArgs(),
                                                       arg.binding.isFinal(),
                                                       arg_location);
            }
        }

        //
        // TODO: process Throws list ... not relevant for now because the translator does not handle them yet.
        //
        JavaParser.cactionBuildMethodSupportEnd(method_name,
                                                method_index, 
                                                method.isConstructor(),
                                                method.isAbstract(),
                                                method.isNative(),
                                                method.typeParameters() == null ? 0 : method.typeParameters().length,
                                                args == null ? 0 : args.length,
                                                method.isDefaultConstructor(), // compiler-generated
                                                args_location != null ? args_location : method_location,
                                                method_location);
        return;
    }


    /**
     *     
     * @param binding
     */
    public void traverseReferenceBinding(ReferenceBinding binding, UnitInfo unit_info) throws Exception {
        assert(binding != null);
        String qualified_name = getCanonicalName(binding);
//      System.out.println("Traversing Reference Binding for " + qualified_name);

        classProcessed.put(qualified_name, binding);

        JavaToken location = unit_info.getDefaultLocation(); 
        
        String class_name = new String(binding.sourceName);

        JavaParser.cactionPushTypeParameterScope(getPackageName(binding), getTypeName(binding), location);

        //
        // If this is a generic type, process its type parameters.  Note that it's important to process the type
        // parameters of this "master" type (if any) prior to processing its super class and interfaces as these may
        // themselves be parameterized types that use parameters originally associated with this "master" type.
        //
        TypeVariableBinding type_variables[] = binding.typeVariables();
        if (type_variables != null) {
            String package_name = getPackageName(binding),
                   type_name = getTypeName(binding);

            for (int i = 0; i < type_variables.length; i++) {
                TypeVariableBinding type_variable = type_variables[i];
                String type_parameter_name = new String(type_variable.sourceName);
                JavaParser.cactionInsertTypeParameter(type_parameter_name, location);
            }
            
            for (int i = 0; i < type_variables.length; i++) {
                TypeVariableBinding type_variable = type_variables[i];
                String type_parameter_name = new String(type_variable.sourceName);

                if (type_variable.superclass == type_variable.firstBound) {
                    setupClass(type_variable.superclass, unit_info);
                    generateAndPushType(type_variable.superclass, unit_info, location);
                }
                if (type_variable.superInterfaces != null) {
                    for (int k = 0, length = type_variable.superInterfaces.length; k < length; k++) {
                        setupClass(type_variable.superInterfaces[k], unit_info);
                        generateAndPushType(type_variable.superInterfaces[k], unit_info, location);
                    }
                }

                if (JavaTraversal.verboseLevel > 2)
                    System.out.println("Type Parameter Support added for " + type_parameter_name);         

                JavaParser.cactionBuildTypeParameterSupport(package_name,
                                                            type_name,
                                                            -1, // method index
                                                            type_parameter_name,
                                                            type_variable.boundsCount(),
                                                            location);
            }
        }

        JavaParser.cactionBuildClassSupportStart(class_name,
                                                 "", // the external name for the type
                                                 false, // a user-defined class? Assume we may not processed the source.
                                                 binding.isInterface(),
                                                 binding.isEnum(),
                                                 binding.isAnonymousType(), // Anonymous class?
                                                 location);

        if (JavaTraversal.verboseLevel > 2)
            System.out.println("After call to cactionBuildClassSupportStart");

        // process the super class
        ReferenceBinding super_class = binding.superclass();
        if ((! binding.isInterface()) && super_class != null) {
            if (JavaTraversal.verboseLevel > 2) {
                System.out.println("Super Class name = " + new String(super_class.sourceName));
            }

            preprocessClass(super_class, unit_info);

            //
            // When we have access to a user-specified source, we use it in case this type is a parameterized type.
            // Note that we should not use node.superclass here (instead of node.binding.superclass) because in the
            // case of an Anonymous type, node.superclass is null!
            //
            generateAndPushType(super_class, unit_info, location);
        }

        // Process the interfaces.
        ReferenceBinding interfaces[] = null;
        try {
            interfaces = binding.superInterfaces(); // (binding.isAnnotationType() ? null : binding.superInterfaces()); // Don't process super interfaces for Annotation types.
        }
        catch(AbortCompilation e) {
            if (JavaTraversal.verboseLevel > 2) {
                System.out.println("There is an issue with the super interfaces of type (" + binding.getClass().getCanonicalName() + ") " + qualified_name +
                                   (binding.isEnum() ? "; -> Enum" : "") +
                                   (binding.isInterface() ? "; -> Interface" : "") +
                                   (binding.isAnnotationType() ? "; -> Annotation" : "") +
                                   (binding.isNestedType() ? "; -> Nested" : "")
                                  );
                e.printStackTrace();
            }
        }
        if (interfaces != null) {
            for (int i = 0; i < interfaces.length; i++) {
                if (JavaTraversal.verboseLevel > 2) {
                    System.out.println("interface name = " + interfaces[i].debugName());
                }

                preprocessClass(interfaces[i], unit_info);

                //
                // When we have access to a user-specified source, we use it in case this type is a parameterized type.
                //
                generateAndPushType(interfaces[i], unit_info, location);
            }
        }

        JavaParser.cactionBuildClassExtendsAndImplementsSupport((type_variables == null ? 0 : type_variables.length),
                                                                ((! binding.isInterface()) && super_class != null),
                                                                (interfaces == null ? 0 : interfaces.length), location);

        //
        // Initial setup of this class based on information in the bindings.
        //
        int num_class_members = 0;
        int method_index = -1;

        //
        // Process the inner classes. Note that the inner classes must be processed first in case
        // one of these types are used for a field or a method. (e.g., See java.net.InetAddress)
        //
        ReferenceBinding inner_class_list[] = binding.memberTypes();
        if (inner_class_list != null) {
            for (int i = 0; i < inner_class_list.length; i++) {
                ReferenceBinding inner_class = inner_class_list[i];
                traverseReferenceBinding(inner_class, unit_info);

                String package_name = getPackageName(inner_class),
                       typename = getTypeName(inner_class);
                JavaParser.cactionBuildInnerTypeSupport(package_name, typename, location);                    
                num_class_members++;
            }
        }

        //
        // Preprocess the types in field declarations
        //
        FieldBinding field_bindings[] = null;
        try {
            field_bindings = binding.availableFields(); // binding.fields();
        }
        catch(Throwable e) {
            System.out.println("*** Could not load fields for (" + binding.getClass().getCanonicalName() + ") " + qualified_name);
            System.out.println("*** The exception thrown is: " + e.getClass().getCanonicalName());
            ; // do nothing
        }
        if (field_bindings != null) {
            for (int i = 0; i < field_bindings.length; i++) {
                FieldBinding field_binding = field_bindings[i];
                if (field_binding.type != null) { // May be null if enum field
                    setupClass(field_binding.type, unit_info);

                    generateAndPushType(field_binding.type, unit_info, location);

                    if (JavaTraversal.verboseLevel > 2)
                        System.out.println("(ReferenceBinding) Build the data member (field) for name = " + new String(field_binding.name));

                    JavaParser.cactionBuildFieldSupport(new String(field_binding.name), location);
                    num_class_members++;
                }
            }
        }

        //
        // Process the constructor and methods parameter types.
        //
        MethodBinding method_bindings[] = null;
        try {
            method_bindings = binding.availableMethods(); // binding.methods();
        }
        catch(Throwable e) {
            System.out.println("*** Could not load methods for (" + binding.getClass().getCanonicalName() + ") " + qualified_name);
            System.out.println("*** The exception thrown is: " + e.getClass().getCanonicalName());
            ; // do nothing
        }
        if (method_bindings != null) {
            for (int i = 0; i < method_bindings.length; i++) {
                MethodBinding method_binding = method_bindings[i];
                if (! method_binding.isConstructor()) {
                    setupClass(method_binding.returnType, unit_info);
                }
                for (int k = 0; k < method_binding.parameters.length; k++) {
                    setupClass(method_binding.parameters[k], unit_info);
                }
                for (int k = 0; k < method_binding.thrownExceptions.length; k++) {
                    setupClass(method_binding.thrownExceptions[k], unit_info);
                }

                String method_name = getMethodName(method_binding);

                if (JavaTraversal.verboseLevel > 2)
                    System.out.println("(ReferenceBinding)  Build the data member (method) for name = " + method_name);

                method_index++; // calculate token index for this method or constructor.
                setMethodIndex(method_binding, method_index);

                assert(typeDeclarationMethodTable.containsKey(getMethodKey(method_binding)));
                JavaParser.cactionBuildMethodSupportStart(method_name,
                                                          method_index, 
                                                          location);

                TypeVariableBinding method_type_variables[] = method_binding.typeVariables;
                if (method_type_variables != null) {
                    ReferenceBinding type_binding = method_binding.declaringClass;
                    String package_name = getPackageName(type_binding),
                           type_name = getTypeName(type_binding);

                    for (int k = 0; k < method_type_variables.length; k++) {
                        TypeVariableBinding method_type_variable = method_type_variables[k];
                        String type_parameter_name = new String(method_type_variable.sourceName);
                        JavaParser.cactionInsertTypeParameter(type_parameter_name, location);
                    }
                        
                    for (int k = 0; k < method_type_variables.length; k++) {
                        TypeVariableBinding method_type_variable = method_type_variables[k];                        

                        if (method_type_variable.superclass == method_type_variable.firstBound) {
                            setupClass(method_type_variable.superclass, unit_info);
                            generateAndPushType(method_type_variable.superclass, unit_info, location);
                        }
                        if (method_type_variable.superInterfaces != null) {
                            for (int j = 0, length = method_type_variable.superInterfaces.length; j < length; j++) {
                                setupClass(method_type_variable.superInterfaces[j], unit_info);
                                generateAndPushType(method_type_variable.superInterfaces[j], unit_info, location);
                            }
                        }

                        String type_parameter_name = new String(method_type_variable.sourceName);
                        JavaParser.cactionBuildTypeParameterSupport(package_name,
                                                                    type_name,
                                                                    method_index,
                                                                    type_parameter_name,
                                                                    method_type_variable.boundsCount(),
                                                                    location);
                    }
                }
            
                if (method_binding.isConstructor()) {
                    JavaParser.cactionTypeReference("", "void", location);
                }
                else {
                    generateAndPushType(method_binding.returnType, unit_info, location);
                }

                TypeVariableBinding type_bindings[] = method_binding.typeVariables;

                TypeBinding parameters[] = method_binding.parameters;
                if (parameters != null) {
                    for (int j = 0; j < parameters.length; j++) {
                        TypeBinding parameter = parameters[j];
                        generateAndPushType(parameter, unit_info, location);
                        String argument_name = "" + j + method_name; // create an "illegal" name for this parameter. 
                        JavaParser.cactionBuildArgumentSupport(argument_name,
                                                               new String(parameter.readableName()),
                                                               (j + 1 == parameters.length ? method_binding.isVarargs() : false),
                                                               false, // mark as not final - does not matter anyway. 
                                                               location);
                    }
                }

                //
                // TODO: process Throws list ... not relevant for now because the translator does not handle them yet.
                //
                JavaParser.cactionBuildMethodSupportEnd(method_name,
                                                        method_index, 
                                                        method_binding.isConstructor(),
                                                        method_binding.isAbstract(),
                                                        method_binding.isNative(),
                                                        method_type_variables == null ? 0 : method_type_variables.length,
                                                        parameters == null ? 0 : parameters.length,
                                                        false, // compiler-generated
                                                        location,
                                                        location);

                //
                // If we are updating an Enum, we need to keep track of the special methods: values() and valueOf()
                //
                if (binding.isEnum()) {
                    if (method_name.equals("values") && (parameters == null || parameters.length == 0)) {
                        enumReferenceBindingToValuesMethodIndexTable.put(binding, method_index);
                    }
                    if (method_name.equals("valueOf") && (parameters != null && parameters.length == 1 && parameters[0].debugName().equals("java.lang.String"))) {
                        enumReferenceBindingToValueOfMethodIndexTable.put(binding, method_index);
                    }
                }

                num_class_members++;
            }
        }
        
        //
        // If this is an Enum, generate method headers for values() and valueOf(String x).
        //
        if (binding.isEnum()) {
            if (addEnumValuesMethodDeclaration(binding, unit_info, method_index)) {
                method_index++;
                num_class_members++;
            }
            if (addEnumValueOfMethodDeclaration(binding, unit_info, method_index)) {
                method_index++;
                num_class_members++;
            }
        }

        lastMethodIndexUsed.put(qualified_name, method_index);

        JavaParser.cactionBuildClassSupportEnd(class_name, num_class_members, location);
        JavaParser.cactionPopTypeParameterScope(location);
//      System.out.println("Done Traversing Reference Binding for " + qualified_name);
    }

    
    /**
     * 
     * @param node
     */
    public void traverseTypeDeclaration(TypeDeclaration node, UnitInfo unit_info) throws Exception {
        assert (unit_info != null);

        LocalOrAnonymousType special_type = localOrAnonymousType.get(node);

        String class_name = (special_type != null 
                                           ? (special_type.isAnonymous() ? special_type.typename : special_type.simplename)
                                           : new String(node.name));

        String qualified_name = (special_type != null ? special_type.qualifiedName() : getCanonicalName(node.binding));
//      System.out.println("Traversing Type Declaration for " + qualified_name);

        classProcessed.put(qualified_name, node);

        JavaToken location = unit_info.createJavaToken(node);
        
        ASTNode node_list[] = orderedClassMembers.get(node);
        assert(node_list != null);

        JavaParser.cactionPushTypeParameterScope(getPackageName(node.binding), getTypeName(node.binding),
                                                 node.typeParameters == null ? location : unit_info.createJavaToken(node.typeParameters[0]));

        //
        // If this is a generic type, process its type parameters.  Note that it's important to process the type
        // parameters of this "master" type (if any) prior to processing its super class and interfaces as these may
        // themselves be parameterized types that use parameters originally associated with this "master" type.
        //
        TypeParameter parameters[] = node.typeParameters;
        if (parameters != null) {
            TypeBinding enclosing_binding = (TypeBinding) node.binding;
            String package_name = getPackageName(enclosing_binding),
                   type_name = getTypeName(enclosing_binding);

            for (int i = 0; i < parameters.length; i++) {
                TypeParameter parameter = parameters[i];
                JavaToken parameter_location = unit_info.createJavaToken(parameter);
                String parameter_name = new String(parameter.name);
                JavaParser.cactionInsertTypeParameter(parameter_name, parameter_location);
            }

            for (int i = 0; i < parameters.length; i++) {
                TypeParameter parameter = parameters[i];
                JavaToken parameter_location = unit_info.createJavaToken(parameter);
                TypeVariableBinding type_variable = parameter.binding;
            
                if (type_variable.superclass == type_variable.firstBound) {
                    setupClass(type_variable.superclass, unit_info);
                    generateAndPushType(type_variable.superclass, unit_info, parameter_location);
                }
                if (type_variable.superInterfaces != null) {
                    for (int k = 0, length = type_variable.superInterfaces.length; k < length; k++) {
                        setupClass(type_variable.superInterfaces[k], unit_info);
                        generateAndPushType(type_variable.superInterfaces[k], unit_info, parameter_location);
                    }
                }

                String parameter_name = new String(parameter.name);
                JavaParser.cactionBuildTypeParameterSupport(package_name,
                                                            type_name,
                                                            -1, // method index
                                                            parameter_name,
                                                            type_variable.boundsCount(),
                                                            parameter_location);
            }
        }

        JavaParser.cactionBuildClassSupportStart(class_name,
                                                 (special_type == null ? "" : special_type.simplename),
                                                 node != null, // a user-defined class?
                                                 node.binding.isInterface(),
                                                 node.binding.isEnum(),
                                                 node.binding.isAnonymousType(), // Anonymous class?
                                                 location);

        if (JavaTraversal.verboseLevel > 2)
            System.out.println("After call to cactionBuildClassSupportStart");

        // process the super class
        if ((! node.binding.isInterface()) && node.binding.superclass != null) {
            if (JavaTraversal.verboseLevel > 2) {
                System.out.println("Super Class name = " + new String(node.binding.superclass.sourceName));
            }

            preprocessClass(node.binding.superclass, unit_info);

            //
            // When we have access to a user-specified source, we use it in case this type is a parameterized type.
            // Note that we should not use node.superclass here (instead of node.binding.superclass) because in the
            // case of an Anonymous type, node.superclass is null!
            //
            generateAndPushType(node.binding.superclass, unit_info, (node.superclass == null ? unit_info.getDefaultLocation() : unit_info.createJavaToken(node.superclass)));
        }

        // Process the interfaces.
        ReferenceBinding interfaces[] = node.binding.superInterfaces(); // (node.binding.isAnnotationType() ? null : node.binding.superInterfaces); // Don't process super interfaces for Annotation types.
        if (interfaces != null) {
            for (int i = 0; i < interfaces.length; i++) {
                if (JavaTraversal.verboseLevel > 2) {
                    System.out.println("interface name = " + interfaces[i].debugName());
                }

                preprocessClass(interfaces[i], unit_info);

                //
                // When we have access to a user-specified source, we use it in case this type is a parameterized type.
                //
                generateAndPushType(interfaces[i], unit_info, (node.superInterfaces == null ? unit_info.getDefaultLocation() : unit_info.createJavaToken(node.superInterfaces[i])));
            }
        }

        JavaParser.cactionBuildClassExtendsAndImplementsSupport((parameters == null ? 0 : parameters.length), 
                                                                (! node.binding.isInterface()) && node.binding.superclass != null,
                                                                (interfaces == null ? 0 : interfaces.length),
                                                                location);

        //
        // Process the inner classes. Note that the inner classes must be processed first in case
        // one of these types are used for a field or a method. (e.g., See java.net.InetAddress)
        //
        if (node.memberTypes != null) {
            TypeDeclaration inner_class_list[] = node.memberTypes;
            for (int i = 0; i < inner_class_list.length; i++) {
                TypeDeclaration inner_class = inner_class_list[i];
                traverseTypeDeclaration(inner_class, unit_info);
            }
        }

        //
        // Preprocess the types in field declarations
        //
        if (node.fields != null) {
            for (int i = 0; i < node.fields.length; i++) {
                FieldDeclaration field_declaration = node.fields[i];
                if (field_declaration.type != null) { // false for enum fields
                    setupClass(field_declaration.type.resolvedType, unit_info);
                }
            }
        }

        //
        // Process the constructor and methods parameter types.
        //
        if (node.methods != null) {
            for (int i = 0; i < node.methods.length; i++) {
                AbstractMethodDeclaration method_declaration = node.methods[i];
                MethodBinding method_binding = method_declaration.binding;
                if (method_binding != null) { // We need this guard because default constructors have no binding.
                    if (! method_binding.isConstructor()) {
                        setupClass(method_binding.returnType, unit_info);
                    }
                    for (int k = 0; k < method_binding.parameters.length; k++) {
                        setupClass(method_binding.parameters[k], unit_info);
                    }
                    for (int k = 0; k < method_binding.thrownExceptions.length; k++) {
                        setupClass(method_binding.thrownExceptions[k], unit_info);

                    }
                }
            }
        }

        //
        // If this class is associated with a user-defined type, process the original source in order to
        // obtain accurate location information.
        //
        int num_class_members = 0;
        int method_index = -1;

        //
        // Now, traverse the class members in the order in which they were specified.
        //
        for (int k = 0; k < node_list.length; k++) {
            ASTNode class_member = node_list[k];
            if (class_member instanceof TypeDeclaration) {
                TypeDeclaration inner_class = (TypeDeclaration) class_member;
                JavaToken  inner_class_location = unit_info.createJavaToken(inner_class);
                // Inner classes already processed above.
                // However, we need to now process it as a class member declaration.
                String package_name = getPackageName(inner_class.binding),
                       typename = getTypeName(inner_class.binding);
                JavaParser.cactionBuildInnerTypeSupport(package_name, typename, inner_class_location);                    
                num_class_members++;
            }
            else if (class_member instanceof FieldDeclaration) {
                FieldDeclaration field = (FieldDeclaration) class_member;
                JavaToken field_location = unit_info.createJavaToken(field);

                if (field instanceof Initializer) {
                    Initializer initializer = (Initializer) field;
                    String name = k + "block";
                    method_index++; // calculate method index for this initializer
                    classInitializerTable.put((Initializer) class_member, method_index);
                    initializerName.put(initializer, name);
                    JavaParser.cactionBuildInitializerSupport(initializer.isStatic(), name, method_index, field_location);
                }
                else {
                    generateAndPushType(field.binding.type, unit_info, field_location);

                    if (JavaTraversal.verboseLevel > 2)
                        System.out.println("(TypeDeclaration) Build the data member (field) for name = " + new String(field.name));

                    JavaParser.cactionBuildFieldSupport(new String(field.name), field_location);
                }
                num_class_members++;
            }
            else if (class_member instanceof AbstractMethodDeclaration) {
                AbstractMethodDeclaration method = (AbstractMethodDeclaration) class_member;
                if (// method instanceof AnnotationMethodDeclaration || // TODO: We need to implement this properly at some point! 
                    method.isClinit() ||
                    ((node.kind(node.modifiers) == TypeDeclaration.ENUM_DECL) && (method.isDefaultConstructor())) ||
                    (method.isDefaultConstructor() /* && ((ConstructorDeclaration) method).isDefaultConstructor() */ && special_type != null && special_type.isAnonymous()))
                        continue;

                MethodBinding method_binding = method.binding;
                
                method_index++; // calculate token index for this method or constructor.
                buildMethod(unit_info, method, method_index);

                num_class_members++;
            }
            else assert(false);
        }

        //
        // If this is an Enum, generate method headers for values() and valueOf(String x).
        //
        if (node.kind(node.modifiers) == TypeDeclaration.ENUM_DECL) {
            method_index++;
            addEnumValuesMethodDeclaration(node, unit_info, method_index);
            num_class_members++;
            method_index++;
            addEnumValueOfMethodDeclaration(node, unit_info, method_index);
            num_class_members++;
        }

        JavaParser.cactionBuildClassSupportEnd(class_name, num_class_members, location);
        JavaParser.cactionPopTypeParameterScope(location); // The location is irrrelevant here!
//      System.out.println("Done Traversing Type Declaration for " + qualified_name);
    }


    /**
     * 
     * @param node
     * @param unit_info
     * @param binding
     */
    public void updateReferenceBinding(TypeDeclaration node, UnitInfo unit_info, ReferenceBinding binding) throws Exception {
        String class_name = new String(node.name),
               qualified_name = getCanonicalName(node.binding);
        assert(qualified_name.equals(getCanonicalName(binding)));
//      System.out.println("Updating Reference Binding for " + qualified_name);

        assert(! (classProcessed.get(qualified_name) instanceof TypeDeclaration));
        classProcessed.put(qualified_name, node);

        assert(localOrAnonymousType.get(node) == null);

        JavaToken location = unit_info.createJavaToken(node);

        JavaParser.cactionPushTypeParameterScope(getPackageName(binding), getTypeName(binding), (node.typeParameters == null ? location : unit_info.createJavaToken(node.typeParameters[0])));

        //
        // If this is a generic type, process its type parameters.  Note that it's important to process the type
        // parameters of this "master" type (if any) prior to processing its super class and interfaces as these may
        // themselves be parameterized types that use parameters originally associated with this "master" type.
        //
        if (node.typeParameters != null) {
            for (int i = 0; i < node.typeParameters.length; i++) {
                TypeParameter parameter = node.typeParameters[i];
                if (JavaTraversal.verboseLevel > 2) {
                    System.out.println("Updating parameter type " + new String(parameter.name));
                }
                JavaToken parameter_location = unit_info.createJavaToken(parameter);

                TypeVariableBinding type_variable = parameter.binding;
                if (type_variable.superclass == type_variable.firstBound) {
                    setupClass(type_variable.superclass, unit_info);
                    generateAndPushType(type_variable.superclass, unit_info, parameter_location);
                }
                if (type_variable.superInterfaces != null) {
                    for (int k = 0, length = type_variable.superInterfaces.length; k < length; k++) {
                        setupClass(type_variable.superInterfaces[k], unit_info);
                        generateAndPushType(type_variable.superInterfaces[k], unit_info, parameter_location);
                    }
                }

                JavaParser.cactionUpdateTypeParameterSupport(new String(parameter.name),
                                                             -1 /* method_index*/, 
                                                             type_variable.boundsCount(),
                                                             parameter_location);
            }
        }

        JavaParser.cactionUpdateClassSupportStart(class_name, location);

        //
        // Process the inner classes. Note that the inner classes must be processed first in case
        // one of these types are used for a field or a method. (e.g., See java.net.InetAddress)
        //
        if (node.memberTypes != null) {
            for (int i = 0; i < node.memberTypes.length; i++) {
                TypeDeclaration member = node.memberTypes[i];
                if (JavaTraversal.verboseLevel > 2) {
                    System.out.println("Updating inner type " + member.binding.debugName());
                }
                updateReferenceBinding(member, unit_info, (ReferenceBinding) classProcessed.get(getCanonicalName(member.binding)));
            }
        }

        // process the super class
        if ((! node.binding.isInterface()) && node.binding.superclass != null) {
            if (JavaTraversal.verboseLevel > 2) {
                System.out.println("Updating Super Class name = " + new String(node.binding.superclass.sourceName));
            }

            //
            // When we have access to a user-specified source, we use it in case this type is a parameterized type.
            // Note that we should not use node.superclass here (instead of node.binding.superclass) because in the
            // case of an Anonymous type, node.superclass is null!
            //
            generateAndPushType(node.binding.superclass, unit_info, (node.superclass == null ? unit_info.getDefaultLocation() : unit_info.createJavaToken(node.superclass)));
        }

        // Process the interfaces.
        ReferenceBinding interfaces[] = node.binding.superInterfaces(); // (binding.isAnnotationType() ? null : node.binding.superInterfaces); // Don't process super interfaces for Annotation types.
        if (interfaces != null) {
            for (int i = 0; i < interfaces.length; i++) {
                if (JavaTraversal.verboseLevel > 2) {
                    System.out.println("interface name = " + interfaces[i].debugName());
                }

                //
                // When we have access to a user-specified source, we use it in case this type is a parameterized type.
                //
                generateAndPushType(interfaces[i], unit_info, (node.superInterfaces == null ? unit_info.getDefaultLocation() : unit_info.createJavaToken(node.superInterfaces[i])));
            }
        }

        //
        // This class is associated with a user-defined type, process the original source in order to
        // obtain accurate location information.
        //
        HashMap<String, Integer> table = classMethodTable.get(getCanonicalName(binding));
        int method_index = lastMethodIndexUsed.get(qualified_name);
        int num_class_members = 0;
        ASTNode node_list[] = orderedClassMembers.get(node);
        for (int k = 0; k < node_list.length; k++) {
            ASTNode class_member = node_list[k];

            if (class_member instanceof TypeDeclaration) {
                TypeDeclaration inner_class = (TypeDeclaration) class_member;
                JavaToken  inner_class_location = unit_info.createJavaToken(inner_class);
                String package_name = getPackageName(inner_class.binding),
                       typename = getTypeName(inner_class.binding);
                JavaParser.cactionUpdateInnerTypeSupport(package_name, typename, inner_class_location);                    
            }
            else if (class_member instanceof FieldDeclaration) {
                FieldDeclaration field = (FieldDeclaration) class_member;
                JavaToken field_location = unit_info.createJavaToken(field);

                if (field instanceof Initializer) {
                    Initializer initializer = (Initializer) field;
                    String name = k + "block";
                    initializerName.put(initializer, name);
                    method_index++; // calculate method index for this initializer
                    classInitializerTable.put(initializer, method_index);
                    JavaParser.cactionBuildInitializerSupport(initializer.isStatic(), name, method_index, field_location);
                }
                else {
                    generateAndPushType(field.binding.type, unit_info, field_location);
                    JavaParser.cactionUpdateFieldSupport(new String(field.name), field_location);
                }
            }
            else if (class_member instanceof AbstractMethodDeclaration) {
                AbstractMethodDeclaration method = (AbstractMethodDeclaration) class_member;
                if (// method instanceof AnnotationMethodDeclaration || // TODO: We need to implement this properly at some point! 
                    method.isClinit() ||
                    ((node.kind(node.modifiers) == TypeDeclaration.ENUM_DECL) && (method.isDefaultConstructor())))
                    continue;

                //
                // PC:  I bumped into either a bad bug or a feature in Eclipse here. It turns out that the call to computeUniqueKey()
                // on the method binding in the AbstractMethodDeclaration yields a different result from the method binding from 
                // the BinaryTypeBinding.  Hence, since the typeDeclarationMethodTable was based on the key from the BynaryTypeBinding,
                // we retrieve it for the purpose of checking if a method was previously processed.
                //
                MethodBinding binary_binding = method.binding; 

                //
                //
                //
                if (! typeDeclarationMethodTable.containsKey(getMethodKey(binary_binding))) { // Certain incompletely processed methods are ignored in BinaryTypeBinding in the preprocessing phase.
                    method_index++; // allocate a new method index for this constructor
                    buildMethod(unit_info, method, method_index);
                }
                else {
                    JavaToken method_location = unit_info.createJavaToken(method);
                    int assigned_index = getMethodIndex(binary_binding);
                    assert(assigned_index >= 0);
                    setMethodIndex(method.binding, assigned_index); // TODO: If I run into trouble, I should check this!!!
              
                    if (method.isConstructor()) {
                        JavaParser.cactionTypeReference("", "void", method_location);
                    }
                    else {
                        generateAndPushType(method.binding.returnType, unit_info, method_location);
                    }

                    //
                    // Note that we use the raw types from the class here so as to get the proper type in order to 
                    // retrieve the right function.
                    //
                    Argument args[] = method.arguments;
                    int num_formal_parameters = (args == null ? 0 : args.length);
                    JavaToken args_location = (args == null ? null : unit_info.createJavaToken(args[0], args[args.length - 1])); 

                    String method_name = getMethodName(method.binding);
                    JavaParser.cactionUpdateMethodSupportStart(method_name,
                                                               assigned_index,
                                                               num_formal_parameters,
                                                               method_location);

                    int num_type_parameters = 0;
                    if (method.typeParameters() != null) {
                        TypeParameter type_parameters[] = method.typeParameters();
                        num_type_parameters = type_parameters.length;
                        for (int i = 0; i < num_type_parameters; i++) {
                            TypeParameter type_parameter = type_parameters[i];                   
                            JavaToken parameter_location = unit_info.createJavaToken(type_parameter);

                            TypeVariableBinding type_variable = type_parameter.binding;
                            if (type_variable.superclass == type_variable.firstBound) {
                                setupClass(type_variable.superclass, unit_info);
                                generateAndPushType(type_variable.superclass, unit_info, parameter_location);
                            }
                            if (type_variable.superInterfaces != null) {
                                for (int j = 0, length = type_variable.superInterfaces.length; j < length; j++) {
                                    setupClass(type_variable.superInterfaces[j], unit_info);
                                    generateAndPushType(type_variable.superInterfaces[j], unit_info, parameter_location);
                                }
                            }

                            JavaParser.cactionUpdateTypeParameterSupport(new String(type_parameter.name),
                                                                         assigned_index,
                                                                         type_variable.boundsCount(),
                                                                         parameter_location);
                        }
                    }
                        
                    if (args != null) {
                        for (int j = 0; j < num_formal_parameters; j++) {
                            Argument arg = args[j];
                            JavaToken arg_location = unit_info.createJavaToken(arg);
                            generateAndPushType(arg.type.resolvedType, unit_info, arg_location);
                            String argument_name = new String(arg.name);
                            JavaParser.cactionUpdateArgumentSupport(j,
                                                                    argument_name,
                                                                    arg.type.print(0, new StringBuffer()).toString(),
                                                                    arg.isVarArgs(),
                                                                    arg.binding.isFinal(),
                                                                    arg_location);
                        }
                    }

                    //
                    // TODO: process Throws list ... not relevant for now because the translator does not handle them yet. ???
                    //
                    JavaParser.cactionUpdateMethodSupportEnd(method_name,
                                                             assigned_index,
                                                             method.isDefaultConstructor(), // compiler-generated
                                                             num_formal_parameters,
                                                             args_location != null ? args_location : method_location,
                                                             method_location);
                }
            }
            else assert(false);

            num_class_members++;
        }

        //
        //
        //
        if (binding.isEnum()) {
            Integer index = enumReferenceBindingToValuesMethodIndexTable.get(binding);
            assert(index != null);
            enumTypeDeclarationToValuesMethodIndexTable.put(node, index);

            index = enumReferenceBindingToValueOfMethodIndexTable.get(binding);
            assert(index != null);
            enumTypeDeclarationToValueOfMethodTable.put(node, index);
        }

        JavaParser.cactionUpdateClassSupportEnd(class_name, ((! node.binding.isInterface()) && node.binding.superclass != null), (interfaces == null ? 0 : interfaces.length), num_class_members, location);
        JavaParser.cactionPopTypeParameterScope(location); // the location does not matter
//      System.out.println("Done updating Reference Binding for " + qualified_name);
    }


    /**
     * 
     * @param type_binding
     */
    public void generateAndPushType(TypeBinding type_binding, UnitInfo unit_info, JavaToken location) throws Exception {
        assert(type_binding != null);
        if (type_binding instanceof ParameterizedTypeBinding) {
            ParameterizedTypeBinding parameterized_type_binding = (ParameterizedTypeBinding) type_binding;
            
            //
            //
            //
            Stack<TypeBinding> binding_stack = new Stack<TypeBinding>();
            for (TypeBinding t = parameterized_type_binding; t != null; t = t.enclosingType()){
                binding_stack.push(t);
            }

            //
            //
            //
            for (int i = 0, max = binding_stack.size(); i < max; i++) {
                TypeBinding binding = binding_stack.pop();
                TypeBinding arg_bindings[] = null;
                if (binding instanceof ParameterizedTypeBinding) {
                    parameterized_type_binding = (ParameterizedTypeBinding) binding;
                    if (parameterized_type_binding.isParameterizedTypeWithActualArguments()) {
                        arg_bindings = parameterized_type_binding.arguments;
                        assert(arg_bindings != null);
                        for (int k = 0; k < arg_bindings.length; k++) {
                            assert(arg_bindings[k] != null);
                            if (! (arg_bindings[k] instanceof WildcardBinding)) {
                                preprocessClass(arg_bindings[k], unit_info);
                            }
                            generateAndPushType(arg_bindings[k], unit_info, location);
                        }
                    }
                }

                if (i == 0) { // first type encountered?
                    String package_name = getPackageName(binding),
                           type_name = getTypeName(binding);
                    if (arg_bindings == null) { // the first type has no argument?
                        JavaParser.cactionTypeReference(package_name, type_name, location);
                    }
                    else {
                        JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                                        type_name,
                                                                        arg_bindings != null, // must be true
                                                                        arg_bindings.length,
                                                                        location);
                    }
                }
                else {
                    JavaParser.cactionParameterizedQualifiedTypeReferenceEnd(getSimpleTypeName(binding), //new String(binding.sourceName()),
                                                                             arg_bindings != null,
                                                                             (arg_bindings == null ? 0 : arg_bindings.length),
                                                                             location);
                }
            }
            assert(binding_stack.empty());
        }
        else if (type_binding instanceof ArrayBinding) {
            ArrayBinding arrayType = (ArrayBinding) type_binding;
            TypeBinding base_type_binding = arrayType.leafComponentType;
            assert(! (base_type_binding instanceof ArrayBinding));

            generateAndPushType(base_type_binding, unit_info, location);

            JavaParser.cactionArrayTypeReference(arrayType.dimensions(), location);
        }
        else if (type_binding instanceof TypeVariableBinding) {
            if (type_binding instanceof CaptureBinding) {
                CaptureBinding capture_binding = (CaptureBinding) type_binding;
                if (capture_binding.wildcard != null) {
                    generateAndPushType(capture_binding.wildcard, unit_info, location);
                }
                else {
                    throw new RuntimeException("*** No support yet for Type Variable binding " + new String(type_binding.shortReadableName()) + " with binding type " + type_binding.getClass().getCanonicalName()); // System.exit(1);
                }
            }
            else {
                Binding scope_binding = ((TypeVariableBinding) type_binding).declaringElement;
                String type_parameter_name = getTypeName(type_binding);
                if (scope_binding instanceof TypeBinding) {
                    TypeBinding enclosing_binding = (TypeBinding) scope_binding;
                    String package_name = getPackageName(enclosing_binding),
                           type_name = getTypeName(enclosing_binding);
                    JavaParser.cactionTypeParameterReference(package_name, type_name, (int) -1 /* no method index */, type_parameter_name, location);
                }
                else if (scope_binding instanceof MethodBinding) {
                    MethodBinding method_binding = (MethodBinding) scope_binding;
                    AbstractMethodDeclaration method_declaration = method_binding.sourceMethod();
                    int method_index = getMethodIndex(method_binding);
                    TypeBinding enclosing_type_binding = method_binding.declaringClass;
                    String package_name = getPackageName(enclosing_type_binding),
                           type_name = getTypeName(enclosing_type_binding);
                    JavaParser.cactionTypeParameterReference(package_name, type_name, method_index, type_parameter_name, location);
                }
                else {
                    throw new RuntimeException("*** No support yet for Type Variable " + new String(type_binding.shortReadableName()) + " with binding type " + type_binding.getClass().getCanonicalName() + " enclosed in " + (scope_binding == null ? "?" : scope_binding.getClass().getCanonicalName())); // System.exit(1);
                }
            }
        }
        else if (type_binding instanceof MemberTypeBinding) {
            MemberTypeBinding member_type_binding = (MemberTypeBinding) type_binding;
            generateAndPushType(member_type_binding.enclosingType(), unit_info, location);
            JavaParser.cactionQualifiedTypeReference(getPackageName(member_type_binding), getTypeName(member_type_binding), unit_info.getDefaultLocation());
        }
        else if (type_binding instanceof WildcardBinding) {
            WildcardBinding wildcard_binding = (WildcardBinding) type_binding;

            JavaParser.cactionWildcard(location);

            if (! wildcard_binding.isUnboundWildcard()) { // there is a bound!
                preprocessClass(wildcard_binding.bound, unit_info);
                generateAndPushType(wildcard_binding.bound, unit_info, location);
            }

            JavaParser.cactionWildcardEnd(wildcard_binding.boundKind == Wildcard.UNBOUND, wildcard_binding.boundKind == Wildcard.EXTENDS,  wildcard_binding.boundKind == Wildcard.SUPER, location);
        }
        else { // if (type_binding instanceof BaseTypeBinding || type_binding instanceof ReferenceBinding) {
            JavaParser.cactionTypeReference(getPackageName(type_binding), getTypeName(type_binding), location);
        }
    }

// -------------------------------------------------------------------------------------------
    
    public void translate(ArrayList<CompilationUnitDeclaration> units, boolean temporary_import_processing) {
        if (units.size() == 0) { // nothing to do?
            return;
        }
        
        // Debugging support...
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Start translating");

        //
        //
        //
        UnitInfo unitInfos[] = new UnitInfo[units.size()];

        //
        // create a map from each unit into its basic info. 
        //
        for (int i = 0; i < units.size(); i++) {
            CompilationUnitDeclaration unit = units.get(i);
            unitInfos[i] = new UnitInfo(unit,
                                        (unit.currentPackage == null ? "" : new String(CharOperation.concatWith(unit.currentPackage.tokens, '.'))),
                                        new String(unit.getFileName()),
                                        new JavaSourcePositionInformationFactory(unit));
            unitInfoTable.put(unit, unitInfos[i]);
            unitOf.put(unitInfos[i].fileName, unitInfos[i]); // map the source file name into its unit info
        }

        //
        // Preprocess all the user-defined types in this list of compilation units.
        //
        for (int i = 0; i < units.size(); i++) {
            //
            // Sort class members and build table of user-defined types.
            //
            UnitInfo unit_info = unitInfos[i];
            CompilationUnitDeclaration unit = unit_info.unit;
            if (unit.types != null) {
                for (TypeDeclaration node : unit.types) {
                    if (node.name != TypeConstants.PACKAGE_INFO_NAME) { // ignore package-info declarations
                        identifyUserDefinedTypes(node, unit_info);
                    }
                }
            }
        }            

        //
        // Preprocess the types
        //
        for (int i = 0; i < units.size(); i++) {
            UnitInfo unit_info = unitInfos[i];
            try {
                if (unit_info.unit.compilationResult.hasMandatoryErrors()) {
                    throw new Exception("Erroneous compilation unit");
                }

                JavaParser.cactionSetupSourceFilename(unit_info.fileName);
                preprocess(unit_info, temporary_import_processing);
            }
            catch (DuplicateTypeException e) {
                e.printStackTrace();
                JavaParser.cactionCompilationUnitDeclarationError(e.getMessage(), unit_info.createJavaToken(unit_info.unit));
                System.exit(1); // Make sure we exit as quickly as possible to simplify debugging.
            }
            catch (Exception e) {
                e.printStackTrace();
                JavaParser.cactionCompilationUnitDeclarationError(e.getMessage(), unit_info.createJavaToken(unit_info.unit));
            }
            catch (Throwable e) {
                e.printStackTrace();
                System.exit(1); // Make sure we exit as quickly as possible to simplify debugging.
            }
        }
            
        //
        // Visit the types.
        //
        for (int i = 0; i < units.size(); i++) {
            UnitInfo unit_info = unitInfos[i];
            try {
                if (unit_info.unit.compilationResult.hasMandatoryErrors()) { 
                    continue;
                }

                JavaParser.cactionSetupSourceFilename(unit_info.fileName);
                this.ecjVisitor.startVisit(this, unit_info);
            }
            catch (Exception e) {
                if (e.getMessage().length() > 0) {
                    e.printStackTrace();
                }
                JavaParser.cactionCompilationUnitDeclarationError(e.getMessage(), unit_info.createJavaToken(unit_info.unit));
                if (e instanceof DuplicateTypeException) {
                    System.exit(1); // Make sure we exit as quickly as possible to simplify debugging.
                }
            }
            catch (Throwable e) {
                e.printStackTrace();
                // Make sure we exit as quickly as possible to simplify debugging.
                System.exit(1);
            }
        }

        JavaParser.cactionClearSourceFilename(); // Release last source file processed.

        // Debugging support...
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Done translating");
    }
}

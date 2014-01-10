import java.util.*;
import org.eclipse.jdt.internal.compiler.ast.*;
import org.eclipse.jdt.internal.compiler.lookup.*;
import org.eclipse.jdt.internal.compiler.ast.Annotation;
import org.eclipse.jdt.core.compiler.CharOperation;
import org.eclipse.jdt.internal.compiler.classfmt.ClassFileConstants;

class ecjASTVisitor extends ExtendedASTVisitor {
    //
    // Keep track of the set of Catch block arguments.
    // Keep track of the set of Blocks that are Catch blocks.
    //
    HashSet<Argument> catchArguments = new HashSet<Argument>();
    HashSet<Block> catchBlocks = new HashSet<Block>();

    //
    // Keep track of Argument(s) that were already processed in a different context.
    //
    HashSet<TypeDeclaration> TypesWithNoBody = new HashSet<TypeDeclaration>();
    HashMap<ASTNode, TypeDeclaration> TypeHeaderDelimiters = new HashMap<ASTNode, TypeDeclaration>();
    HashSet<AbstractMethodDeclaration> MethodsWithNoBody = new HashSet<AbstractMethodDeclaration>();
    HashMap<ASTNode, AbstractMethodDeclaration> MethodHeaderDelimiters = new HashMap<ASTNode, AbstractMethodDeclaration>();

    JavaParserSupport javaParserSupport;
    UnitInfo unitInfo;

    // *************************************************
    // Support for source code position (from Vincent).
    // *************************************************
    public ecjASTVisitor(UnitInfo unit_info, JavaParserSupport java_parser_support) {
        this.javaParserSupport = java_parser_support;
        this.unitInfo = unit_info;
    }

    // *************************************************
    public String nodeId(ASTNode node) {
        if (node instanceof TypeDeclaration) {
            TypeDeclaration type_declaration = (TypeDeclaration) node;
            JavaParserSupport.LocalOrAnonymousType special_type = javaParserSupport.localOrAnonymousType.get(node);
            String typename = (special_type == null
                                             ? new String(type_declaration.name)
                                             : special_type.isAnonymous()
                                                      ? special_type.typename
                                                      : special_type.simplename);
            return " TypeDeclaration - " + (type_declaration.kind(type_declaration.modifiers) == TypeDeclaration.INTERFACE_DECL
                                                ? "interface"
                                                : type_declaration.kind(type_declaration.modifiers) == TypeDeclaration.ANNOTATION_TYPE_DECL
                                                      ? "@interface"
                                                      : type_declaration.kind(type_declaration.modifiers) == TypeDeclaration.ENUM_DECL
                                                            ? "enum" 
                                                            : "class") +
                   ": " + typename + "(***) -- binding: " + ((TypeDeclaration) node).binding.getClass().getCanonicalName();
        }
        else if (node instanceof MessageSend)
            return " MessageSend: " + javaParserSupport.getMethodName(((MessageSend) node).binding) + "( ... )";
        else if (node instanceof MethodDeclaration) // && (! (node instanceof AnnotationMethodDeclaration)))
            return " MethodDeclaration: " + javaParserSupport.getMethodName(((MethodDeclaration) node).binding) + "(***)";
        else if (node instanceof SingleNameReference)
            return " SingleNameReference: " + new String(((SingleNameReference) node).token);
        return node.getClass().getName();
    }
    
    public boolean preVisit(ASTNode node) {
        if (javaParserSupport.verboseLevel > 1)
            System.out.println("Pre-visiting " + nodeId(node));

        if (node instanceof Javadoc) { // Ignore all Javadoc nodes!!!
            return false;
        }

        if (// node instanceof AnnotationMethodDeclaration || // TODO: We need to implement this properly at some point! 
            node instanceof Annotation) { // Ignore all Annotation nodes!!!
            return false;
        }

        if (TypeHeaderDelimiters.containsKey(node)) {
            TypeDeclaration type = TypeHeaderDelimiters.get(node);
            if (javaParserSupport.verboseLevel > 1)
                System.out.println("    Side-visiting Type Declaration Header for " + type.getClass().getName());
            JavaParser.cactionTypeDeclarationHeader((! type.binding.isInterface()) && type.binding.superclass != null, // type.binding.superclass != null || type.kind(type.modifiers) == TypeDeclaration.ENUM_DECL,
                                                    type.superInterfaces == null ? 0 : type.superInterfaces.length,
                                                    type.typeParameters == null  ? 0 : type.typeParameters.length,
                                                    this.unitInfo.createJavaToken(type));
        }
        else if (MethodHeaderDelimiters.containsKey(node)) {
            AbstractMethodDeclaration method = MethodHeaderDelimiters.get(node);
            if (method instanceof ConstructorDeclaration) {
                if (javaParserSupport.verboseLevel > 1)
                    System.out.println("    Side-visiting Constructor Declaration Header for " + method.getClass().getName());
                ConstructorDeclaration constructor = (ConstructorDeclaration) method;
                javaParserSupport.processConstructorDeclarationHeader(constructor, this.unitInfo.createJavaToken(method));
            }
            else {
                if (javaParserSupport.verboseLevel > 1)
                    System.out.println("    Side-visiting Method Declaration Header for " + method.getClass().getName());
                javaParserSupport.processMethodDeclarationHeader((MethodDeclaration) method, this.unitInfo.createJavaToken(method)); 
            }
        }

        //
        // Ignore default constructors.
        //
        if (node instanceof ConstructorDeclaration) { // a constructor declaration?
            ConstructorDeclaration constructor = (ConstructorDeclaration) node;
            if (constructor.isDefaultConstructor()) { // a default constructor?
                return false; // ignore it !
            }
        }

        return true;
    }

    public void postVisit(ASTNode node) {
       if (javaParserSupport.verboseLevel > 1)
            System.out.println("Post-visiting " + nodeId(node));

        //
        // If you need to do something at the end of a visit, do it here.
        //
        if (node instanceof Expression) {
            int paren_count = (node.bits & ASTNode.ParenthesizedMASK) >> ASTNode.ParenthesizedSHIFT;
            if (paren_count > 0) {
                JavaParser.cactionParenthesizedExpression(paren_count);
            }
        }

        return;
    }

    // *************************************************

    private void enterSingleNameReference(SingleNameReference node) {
        String varRefName = node.toString();
        if (javaParserSupport.verboseLevel > 0) {
            System.out.println("Building a variable reference for name = " + varRefName);
            System.out.println("node.genericCast = " + node.genericCast);
        }

        if (node.binding instanceof TypeVariableBinding) { // is this name a type variable?
            TypeVariableBinding type_variable_binding = (TypeVariableBinding) node.binding;
            if (javaParserSupport.verboseLevel > 0) {
                System.out.println("The Single name referenced " + varRefName + " is bound to type " + type_variable_binding.debugName());
            }

            String type_parameter_name = javaParserSupport.getTypeName(type_variable_binding);
            Binding binding = type_variable_binding.declaringElement;
            if (binding instanceof TypeBinding) {
                TypeBinding enclosing_binding = (TypeBinding) binding;
                String package_name = javaParserSupport.getPackageName(enclosing_binding),
                       type_name = javaParserSupport.getTypeName(enclosing_binding);
//System.out.println("(1) TypeParameterReference: package name = " + package_name + ";  type name = " + type_name);
                JavaParser.cactionTypeParameterReference(package_name, type_name, -1 /* no method index */, type_parameter_name, this.unitInfo.createJavaToken(node));
            }
            else if (binding instanceof MethodBinding) {
                MethodBinding method_binding = (MethodBinding) binding;
                AbstractMethodDeclaration method_declaration = method_binding.sourceMethod();
                if (method_declaration != null) {
                    ReferenceBinding type_binding = method_binding.declaringClass;
                    String package_name = javaParserSupport.getPackageName(type_binding),
                           type_name = javaParserSupport.getTypeName(type_binding);
                    int method_index = javaParserSupport.getMethodIndex(method_declaration);
//System.out.println("(2) TypeParameterReference: package name = " + package_name + ";  type name = " + type_name);                    
                    JavaParser.cactionTypeParameterReference(package_name, type_name, method_index, type_parameter_name, this.unitInfo.createJavaToken(node));
                }
                else {
                    throw new RuntimeException("*** No support yet for Type Variable enclosed in " + binding.getClass().getCanonicalName() + " without source being available"); // System.exit(1);
                }
            }
            else {
                throw new RuntimeException("*** No support yet for Type Variable enclosed in " + binding.getClass().getCanonicalName()); // System.exit(1);
            }
        }
        else if (node.binding instanceof TypeBinding) { // is this name a type?
            TypeBinding type_binding = (TypeBinding) node.binding;
            assert(type_binding.isClass() || type_binding.isInterface() || type_binding.isEnum());
            if (javaParserSupport.verboseLevel > 0) {
                System.out.println("The Single name referenced " + varRefName + " is bound to type " + type_binding.debugName());
            }

            javaParserSupport.setupClass(type_binding, this.unitInfo);

//System.out.println("TypeReference 1");
            JavaParser.cactionTypeReference(javaParserSupport.getPackageName(type_binding),
                                            javaParserSupport.getTypeName(type_binding),
                                            this.unitInfo.createJavaToken(node));
        }
        else { // the name is a variable
            String package_name = "",
                   type_name = "";
/*
System.out.println("SingleNameReference 1 on " + varRefName + (node.syntheticAccessors != null ? " - not null " : " - null"));
if (node.syntheticAccessors != null) {
System.out.println("The variable " + varRefName + " has " + node.syntheticAccessors.length + " synthetic accessors (Read, Write):");
if (node.syntheticAccessors[SingleNameReference.READ] != null)
System.out.println("    " + "accessor[" + SingleNameReference.READ + "] =>  " + new String(node.syntheticAccessors[SingleNameReference.READ].readableName()) + " in class " + node.syntheticAccessors[SingleNameReference.READ].declaringClass.debugName());
if (node.syntheticAccessors[SingleNameReference.WRITE] != null)
System.out.println("    " + "accessor[" + SingleNameReference.WRITE + "] =>  " + new String(node.syntheticAccessors[SingleNameReference.WRITE].readableName()) + " in class " + node.syntheticAccessors[SingleNameReference.WRITE].declaringClass.debugName());
}
*/
            if (node.localVariableBinding() == null) { // not a local variable
                TypeBinding type_binding = node.actualReceiverType;
                assert(type_binding.isClass() || type_binding.isInterface() || type_binding.isEnum());
                if (javaParserSupport.verboseLevel > 0) {
                    System.out.println("The  Single name referenced " + varRefName + " is bound to type " + type_binding.debugName());
                }
                javaParserSupport.preprocessClass(type_binding, this.unitInfo);
                package_name = javaParserSupport.getPackageName(type_binding);
                type_name = javaParserSupport.getTypeName(type_binding);
            }

            JavaParser.cactionSingleNameReference(package_name, type_name, varRefName, this.unitInfo.createJavaToken(node));
        }
    }

    private void enterTypeDeclaration(TypeDeclaration node) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enterTypeDeclaration(node)");

        //
        // Get the list of type members in sorted order.
        //
        ASTNode node_list[] = javaParserSupport.orderedClassMembers.get(node);
        assert(node_list != null);
        if (node_list.length > 0) {
            ASTNode member = node_list[0];
            TypeHeaderDelimiters.put(member, node); 
        }
        else TypesWithNoBody.add(node);

        JavaParserSupport.LocalOrAnonymousType special_type = javaParserSupport.localOrAnonymousType.get(node);
        String package_name = (special_type == null
                                      ? javaParserSupport.getPackageName(node.binding)
                                      : special_type.package_name
                              ),
               enclosing_classname = (special_type == null ? "" : new String(node.binding.enclosingType().sourceName)),
               typename = (special_type == null
                                  ? javaParserSupport.getTypeName(node.binding)
                                  : special_type.isAnonymous()
                                           ? special_type.typename
                                           : special_type.simplename
                          );

        JavaParser.cactionTypeDeclaration(package_name,
                                          typename,
                                          (! node.binding.isInterface()) && node.binding.superclass != null, // see traverseTypeDeclaration and traverseReferenceBinding for detail!
                                          node.kind(node.modifiers) == TypeDeclaration.ANNOTATION_TYPE_DECL,
                                          node.kind(node.modifiers) == TypeDeclaration.INTERFACE_DECL,
                                          node.kind(node.modifiers) == TypeDeclaration.ENUM_DECL,
                                          (node.binding != null && node.binding.isAbstract()),
                                          (node.binding != null && node.binding.isFinal()),
                                          (node.binding != null && node.binding.isPrivate()),
                                          (node.binding != null && node.binding.isPublic()),
                                          (node.binding != null && node.binding.isProtected()),
                                          (node.binding != null && node.binding.isStatic() && node.binding.isNestedType()),
                                          (node.binding != null && node.binding.isStrictfp()),
                                          this.unitInfo.createJavaToken(node));

        if (node.javadoc != null) { // Javadoc(s) are ignored for now. See preVisit(...)
            node.javadoc.traverse(this, node.scope);
        }

        if (node.annotations != null) { // Annotations are ignored for now. See preVisit(...)
            int annotationsLength = node.annotations.length;
            for (int i = 0; i < annotationsLength; i++) {
                node.annotations[i].traverse(this, node.staticInitializerScope);
            }
        }

        if ((! node.binding.isInterface()) && node.binding.superclass != null) { // see traverseTypeDeclaration and traverseReferenceBinding for detail as to why the superclass is processed in a special way!
            JavaParser.cactionTypeReference(javaParserSupport.getPackageName(node.binding.superclass), javaParserSupport.getTypeName(node.binding.superclass), this.unitInfo.getDefaultLocation());
        }

        if (node.superInterfaces != null) {
            int length = node.superInterfaces.length;
            for (int i = 0; i < length; i++) {
                node.superInterfaces[i].traverse(this, node.scope);
            }
        }

        if (node.typeParameters != null) {
            int length = node.typeParameters.length;
            for (int i = 0; i < length; i++) {
                node.typeParameters[i].traverse(this, node.scope);
            }
        }

        //
        // Now, traverse the class members in the order in which they were specified.
        //
        for (ASTNode class_member : node_list) {
            if (class_member instanceof TypeDeclaration) {
                ((TypeDeclaration) class_member).traverse(this, node.scope);
            }
            else if (class_member instanceof FieldDeclaration) {
                FieldDeclaration field = (FieldDeclaration) class_member;
                if (field.isStatic()) {
                    field.traverse(this, node.staticInitializerScope);
                }
                else {
                    field.traverse(this, node.initializerScope);
                }
            }
            else if (class_member instanceof AbstractMethodDeclaration) {
                ((AbstractMethodDeclaration) class_member).traverse(this, node.scope);
            }
            else assert(false); // can't happen
        }

        //
        // If this is an Enum type, process the method header declarations for values() and valueOf(String x).
        //
        if (node.kind(node.modifiers) == TypeDeclaration.ENUM_DECL) {
            //
            // values()
            //
            // See the function traverseClass for more information about this method.
            //
if (javaParserSupport.enumTypeDeclarationToValuesMethodIndexTable.get(node) == null) {
System.out.println("The enum Values() method is not defined for " + node.binding.debugName());
}
if (javaParserSupport.enumTypeDeclarationToValueOfMethodTable.get(node) == null) {
System.out.println("The enum ValueOf() method is not defined for " + node.binding.debugName());
}
            int values_index = javaParserSupport.enumTypeDeclarationToValuesMethodIndexTable.get(node),
                valueOf_index = javaParserSupport.enumTypeDeclarationToValueOfMethodTable.get(node);
            JavaParser.cactionMethodDeclaration("values", values_index, this.unitInfo.getDefaultLocation());
            JavaParser.cactionMethodDeclarationHeader("values",
                                                      true,  // is_abstract
                                                      false, // is_native,
                                                      true,  // is_static,
                                                      false, // is_final,
                                                      false, // is_synchronized,
                                                      true,  // is_public,
                                                      false, // is_protected,
                                                      false, // is_private,
                                                      false, // is_strictfp, 
                                                      0,     // No type parameters
                                                      0,     // No formal parameters
                                                      0,     // No thrown exceptions
                                                      this.unitInfo.getDefaultLocation()
                                                     );
            JavaParser.cactionMethodDeclarationEnd(0, this.unitInfo.getDefaultLocation());

            //
            // valueOf()
            //
            // See the function traverseClass for more information about this method.
            //
            JavaParser.cactionMethodDeclaration("valueOf", valueOf_index, this.unitInfo.getDefaultLocation());
// TODO: Remove this because formal parameters have already been processed.
/*            
            TypeBinding string_binding = node.binding.getExactMethod("toString".toCharArray(), new TypeBinding[0], null).returnType;
            javaParserSupport.generateAndPushType(string_binding, this.unitInfo, this.unitInfo.getDefaultLocation(), true); // is_formal_parameter_type_mapping
*/            
            JavaParser.cactionMethodDeclarationHeader("valueOf",
                                                      true,  // is_abstract
                                                      false, // is_native,
                                                      true,  // is_static,
                                                      false, // is_final,
                                                      false, // is_synchronized,
                                                      true,  // is_public,
                                                      false, // is_protected,
                                                      false, // is_private,
                                                      false, // is_strictfp, 
                                                      0,     // method.typeParameters
                                                      1,     // method.arguments
                                                      0,     // method.thrownExceptions
                                                      this.unitInfo.getDefaultLocation()
                                                     );
            JavaParser.cactionMethodDeclarationEnd(0, this.unitInfo.getDefaultLocation());
        }
    }

    private void exitTypeDeclaration(TypeDeclaration node) {
        if (TypesWithNoBody.contains(node)) {
            if (javaParserSupport.verboseLevel > 1)
                System.out.println("    Side-visiting Type Declaration Header for " + node.getClass().getName());
            JavaParser.cactionTypeDeclarationHeader((! node.binding.isInterface()) && node.binding.superclass != null, // node.binding.superclass != null || node.kind(node.modifiers) == TypeDeclaration.ENUM_DECL,
                                                    node.superInterfaces == null ? 0 : node.superInterfaces.length,
                                                    node.typeParameters == null  ? 0 : node.typeParameters.length,
                                                    this.unitInfo.createJavaToken(node));
        }
        JavaParser.cactionTypeDeclarationEnd(this.unitInfo.createJavaToken(node));
    }

    public boolean enter(AllocationExpression node,BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (AllocationExpression,BlockScope)");

        if (node.type != null) {
            javaParserSupport.preprocessClass(node.type.resolvedType, this.unitInfo);
        }

        // Call the Java side of the JNI function.
        JavaParser.cactionAllocationExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (AllocationExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(AllocationExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (AllocationExpression,BlockScope)");

        if (javaParserSupport.verboseLevel > 0 && node.type != null) 
            System.out.println("The Allocation type is bound to type " + node.type.resolvedType.debugName());

        JavaParser.cactionAllocationExpressionEnd(node.type != null, node.arguments == null ? 0 : node.arguments.length, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (AllocationExpression,BlockScope)");
    }


    public boolean enter(AND_AND_Expression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (AND_AND_Expression,BlockScope)");

        // Call the Java side of the JNI function.
        JavaParser.cactionANDANDExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (AND_AND_Expression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(AND_AND_Expression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (AND_AND_Expression,BlockScope)");

        // Call the Java side of the JNI function.
        JavaParser.cactionANDANDExpressionEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (AND_AND_Expression,BlockScope)");
    }


    public boolean enter(AnnotationMethodDeclaration node,ClassScope classScope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (AnnotationMethodDeclaration,ClassScope)");

        String name = javaParserSupport.getMethodName(node.binding);
        int method_index = javaParserSupport.getMethodIndex(node);
        JavaParser.cactionAnnotationMethodDeclaration(name, method_index, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (AnnotationMethodDeclaration,ClassScope)");
/*
        if (node.annotations != null) {
        	int annotationsLength = node.annotations.length;
        	for (int i = 0; i < annotationsLength; i++)
        		node.annotations[i].traverse(this, node.scope);
        }

        if (node.returnType != null) {
        	node.returnType.traverse(this, node.scope);
        }
*/        
        if (node.defaultValue != null && (! (node.defaultValue instanceof Annotation))) {
        	node.defaultValue.traverse(this, node.scope);
        }

        return false; // true; // do nothing by default, keep traversing
    }

    public void exit(AnnotationMethodDeclaration node, ClassScope classScope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (AnnotationMethodDeclaration,ClassScope)");

        String name = javaParserSupport.getMethodName(node.binding);
        int method_index = javaParserSupport.getMethodIndex(node);
System.out.println("The default expression of method " + name + " is of type " + (node.defaultValue == null ? "???" : node.defaultValue.getClass().getCanonicalName()));        
        JavaParser.cactionAnnotationMethodDeclarationEnd(name, method_index, node.defaultValue != null && (! (node.defaultValue instanceof Annotation)), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (AnnotationMethodDeclaration,ClassScope)");
    }


    public boolean enter(Argument node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Argument,BlockScope)");

        String name = new String(node.name);

        if (javaParserSupport.verboseLevel > 0) {
            System.out.println(" name      = " + name);
            System.out.println(" type      = " + node.type);
            System.out.println(" modifiers = " + node.modifiers);
        }

        if (catchArguments.contains(node)) {
            JavaParser.cactionCatchArgument(name, this.unitInfo.createJavaToken(node));
        }
        else {
            JavaParser.cactionArgument(name, this.unitInfo.createJavaToken(node));
        }
        
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Argument,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Argument node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Argument,BlockScope)");

        String name = new String(node.name);

        boolean is_final = node.binding.isFinal();

        if (catchArguments.contains(node)) {
            int number_of_types = (node.type instanceof UnionTypeReference ? ((UnionTypeReference ) node.type).typeReferences.length : 1);
            JavaParser.cactionCatchArgumentEnd(name, number_of_types, is_final, this.unitInfo.createJavaToken(node));
        }
        else {
            JavaParser.cactionArgumentEnd(name, is_final, this.unitInfo.createJavaToken(node));
        }

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Argument,BlockScope)");
    }


    public boolean enter(Argument node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Argument,ClassScope)");

        String name = new String(node.name);

        if (javaParserSupport.verboseLevel > 0) {
            System.out.println(" name      = " + name);
            System.out.println(" type      = " + node.type);
            System.out.println(" modifiers = " + node.modifiers);
        }

        if (catchArguments.contains(node)) {
            JavaParser.cactionCatchArgument(name, this.unitInfo.createJavaToken(node));
        }
        else {
            JavaParser.cactionArgument(name, this.unitInfo.createJavaToken(node));
        }
        
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Argument,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Argument node,ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Argument,ClassScope)");

        String name = new String(node.name);

        boolean is_final = node.binding.isFinal(); 

        if (catchArguments.contains(node)) {
            int number_of_types = (node.type instanceof UnionTypeReference ? ((UnionTypeReference ) node.type).typeReferences.length : 1);
            JavaParser.cactionCatchArgumentEnd(name, number_of_types, is_final, this.unitInfo.createJavaToken(node));
        }
        else {
            JavaParser.cactionArgumentEnd(name, is_final, this.unitInfo.createJavaToken(node));
        }

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Argument,ClassScope)");
    }


    public boolean enter(ArrayAllocationExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ArrayAllocationExpression,BlockScope)");

        //
        // Traverse the children of this node here because we need to generate a 0 integer
        // expression for each missing dimension.
        //
        int dimensions_length = node.dimensions.length;
        node.type.traverse(this, scope);
        for (int i = 0; i < dimensions_length; i++) {
            if (node.dimensions[i] != null)
                 node.dimensions[i].traverse(this, scope);
            else JavaParser.cactionIntLiteral(0, "0", this.unitInfo.createJavaToken(node));
        }
        if (node.initializer != null) {
            node.initializer.traverse(this, scope);
        }

        javaParserSupport.preprocessClass(node.type.resolvedType, this.unitInfo);

        JavaParser.cactionArrayAllocationExpression(this.unitInfo.createJavaToken(node));
        
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ArrayAllocationExpression,BlockScope)");

        return false; // We've already traversed the children of this node. Indicate this by returning false!
    }

    public void exit(ArrayAllocationExpression node, BlockScope scope) {
        // do nothing by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ArrayAllocationExpression,BlockScope)");
        
        JavaParser.cactionArrayAllocationExpressionEnd(node.dimensions == null ? 0 : node.dimensions.length,
                                                       node.initializer != null,
                                                       this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ArrayAllocationExpression,BlockScope)");
    }


    public boolean enter(ArrayInitializer node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ArrayInitializer,BlockScope)");

        JavaParser.cactionArrayInitializer(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ArrayInitializer,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ArrayInitializer node, BlockScope scope) {
        // do nothing by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ArrayInitializer, BlockScope)");
          
        JavaParser.cactionArrayInitializerEnd(node.expressions == null ? 0 : node.expressions.length,
                                              this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ArrayInitializer, BlockScope)");
    }


    public boolean enter(ArrayQualifiedTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ArrayQualifiedTypeReference,BlockScope)");

        ArrayBinding array_type = (ArrayBinding) node.resolvedType;
        TypeBinding base_type = array_type.leafComponentType;

        if (base_type.isClass() || base_type.isInterface() || base_type.isEnum()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("Array base type referenced is bound to " + base_type.debugName());
            javaParserSupport.setupClass(base_type, this.unitInfo);
        }

        javaParserSupport.generateAndPushType(base_type, this.unitInfo, this.unitInfo.createJavaToken(node), false /* is_formal_parameter_type_mapping */);
        JavaParser.cactionArrayTypeReference(node.dimensions(), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ArrayQualifiedTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ArrayQualifiedTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ArrayQualifiedTypeReference,BlockScope)");

        // do nothing by default
    }


    public boolean enter(ArrayQualifiedTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ArrayQualifiedTypeReference,ClassScope)");

        ArrayBinding array_type = (ArrayBinding) node.resolvedType;
        TypeBinding base_type = array_type.leafComponentType;

        if (base_type.isClass() || base_type.isInterface() || base_type.isEnum()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("Array base type referenced is bound to " + base_type.debugName());
            javaParserSupport.setupClass(base_type, this.unitInfo);
        }
        
        javaParserSupport.generateAndPushType(base_type, this.unitInfo, this.unitInfo.createJavaToken(node), false /* is_formal_parameter_type_mapping */);
        JavaParser.cactionArrayTypeReference(node.dimensions(),
                                             this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ArrayQualifiedTypeReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ArrayQualifiedTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ArrayQualifiedTypeReference,ClassScope)");

        // do nothing by default
    }


    public boolean enter(ArrayReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ArrayReference,BlockScope)");

        JavaParser.cactionArrayReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ArrayReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ArrayReference node, BlockScope scope) {
        // do nothing by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ArrayReference,BlockScope)");

        JavaParser.cactionArrayReferenceEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ArrayReference,BlockScope)");
    }


    public boolean enter(ArrayTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ArrayTypeReference,BlockScope)");

        ArrayBinding array_type = (ArrayBinding) node.resolvedType;
        TypeBinding base_type = array_type.leafComponentType;

        if (base_type.isClass() || base_type.isInterface() || base_type.isEnum()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("Array base type referenced is bound to " + base_type.debugName());
            javaParserSupport.setupClass(base_type, this.unitInfo);
        }
        
        javaParserSupport.generateAndPushType(base_type, this.unitInfo, this.unitInfo.createJavaToken(node), false /* is_formal_parameter_type_mapping */);
        JavaParser.cactionArrayTypeReference(node.dimensions(),
                                             this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ArrayTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ArrayTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (ArrayTypeReference,BlockScope)");

         // do nothing by default

         if (javaParserSupport.verboseLevel > 0)
               System.out.println("Leaving exit (ArrayTypeReference,BlockScope)");
    }


    public boolean enter(ArrayTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ArrayTypeReference,ClassScope)");

        ArrayBinding array_type = (ArrayBinding) node.resolvedType;
        TypeBinding base_type = array_type.leafComponentType;

        if (base_type.isClass() || base_type.isInterface() || base_type.isEnum()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("Array base type referenced is bound to " + base_type.debugName());
            javaParserSupport.setupClass(base_type, this.unitInfo);
        }

        javaParserSupport.generateAndPushType(base_type, this.unitInfo, this.unitInfo.createJavaToken(node), false /* is_formal_parameter_type_mapping */);
        JavaParser.cactionArrayTypeReference(node.dimensions(),
                                             this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ArrayTypeReference, ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ArrayTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (ArrayTypeReference,ClassScope)");
        // do nothing by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ArrayTypeReference,ClassScope)");
    }


    public boolean enter(AssertStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (AssertStatement,BlockScope)");

        JavaParser.cactionAssertStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (AssertStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(AssertStatement node, BlockScope scope) {
        // do nothing by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (AssertStatement,BlockScope)");

        JavaParser.cactionAssertStatementEnd(node.exceptionArgument != null, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (AssertStatement,BlockScope)");
    }


    public boolean enter(Assignment node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Assignment,BlockScope)");

        JavaParser.cactionAssignment(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Assignment,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Assignment node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (Assignment,BlockScope)");

        JavaParser.cactionAssignmentEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Assignment,BlockScope)");
    }


    public boolean enter(BinaryExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (BinaryExpression,BlockScope)");

        JavaParser.cactionBinaryExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (BinaryExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(BinaryExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (BinaryExpression,BlockScope)");

        int operatorKind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT;

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (BinaryExpression,BlockScope): operatorKind = " + operatorKind);

        JavaParser.cactionBinaryExpressionEnd(operatorKind, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (BinaryExpression,BlockScope)");
    }


    public boolean enter(Block node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Block,BlockScope)");

        JavaParser.cactionBlock(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Block,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Block node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (Block,BlockScope)");

        if (javaParserSupport.verboseLevel > 1)
            System.out.println("node.explicitDeclarations = " + node.explicitDeclarations);

        int number_of_statements = 0;
        if (node.statements != null)
            number_of_statements = node.statements.length;

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("numberOfStatements = " + number_of_statements);

        JavaParser.cactionBlockEnd(number_of_statements, this.unitInfo.createJavaToken(node));

        //
        // charles4 (09/26/2011): If this block belongs to a Catch statement,
        // close the catch statement.
        //
        if (catchBlocks.contains(node)) {
            JavaParser.cactionCatchBlockEnd(this.unitInfo.createJavaToken(node));
        }

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Block,BlockScope)");
    }


    public boolean enter(BreakStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (BreakStatement,BlockScope)");

        JavaParser.cactionBreakStatement((node.label == null ? "" : new String(node.label)),
                                         this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (BreakStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(BreakStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (BreakStatement,BlockScope)");
    }


    public boolean enter(CaseStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (CaseStatement,BlockScope)");

        JavaParser.cactionCaseStatement(node.constantExpression != null, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (CaseStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(CaseStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (CaseStatement,BlockScope)");

        JavaParser.cactionCaseStatementEnd(node.constantExpression != null, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (CaseStatement,BlockScope)");
    }


    public boolean enter(CastExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (CastExpression,BlockScope)");

        JavaParser.cactionCastExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (CastExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(CastExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (CastExpression,BlockScope)");

        JavaParser.cactionCastExpressionEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (CastExpression,BlockScope)");
    }


    public boolean enter(CharLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (CharLiteral,BlockScope)");

        JavaParser.cactionCharLiteral(node.constant.charValue(), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (CharLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(CharLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (CharLiteral,BlockScope)");
    }


    public boolean enter(ClassLiteralAccess node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ClassLiteralAccess,BlockScope)");

        JavaParser.cactionClassLiteralAccess(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ClassLiteralAccess,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ClassLiteralAccess node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit(ClassLiteralAccess,BlockScope)");

        JavaParser.cactionClassLiteralAccessEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ClassLiteralAccess,BlockScope)");
    }


    public boolean enter(Clinit node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Clinit,ClassScope)");

        JavaParser.cactionClinit(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Clinit,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Clinit node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Clinit,ClassScope)");
    }

    public boolean enter(CompilationUnitDeclaration node, CompilationUnitScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (CompilationUnitDeclaration,CompilationUnitScope)");

        // Except for out here for debugging, the filename string is not used (even on the C++ side of the JNI function.
        String filename = new String(node.getFileName());
        if (javaParserSupport.verboseLevel > 0) {
            System.out.println("Compiling file = " + filename);
        }

        // Ouput some information about the CompilationUnitScope (we don't use the package name currently).
        // DQ (9/11/2011): Static analysis tools suggest using StringBuffer instead of String.
        // String packageReference = "";
        StringBuffer packageReference = new StringBuffer();
        for (int i = 0, tokenArrayLength = scope.currentPackageName.length; i < tokenArrayLength; i++) {
            String tokenString = new String(scope.currentPackageName[i]);

            if (i > 0) {
                packageReference.append('.');
            }

            packageReference.append(tokenString);
        }
        String package_name = new String(packageReference);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Package name = " + packageReference.toString());

        // Call the Java side of the JNI function.
        // This function only does a few tests on the C++ side to make sure that it is ready to construct the ROSE AST.
        JavaParser.cactionCompilationUnitDeclaration(new String(node.getFileName()), package_name, filename, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (CompilationUnitDeclaration,CompilationUnitScope)");

        return true; // do nothing by default, keep traversing
    }

    public void exit(CompilationUnitDeclaration node, CompilationUnitScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (CompilationUnitDeclaration,CompilationUnitScope)");

        if (node.types != null) {
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("node.types.length = " + node.types.length);
        }

        if (node.imports != null) {
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("node.imports.length = " + node.imports.length);
        }

        JavaParser.cactionCompilationUnitDeclarationEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (CompilationUnitDeclaration,CompilationUnitScope)");
    }


    public boolean enter(CompoundAssignment node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (CompoundAssignment,BlockScope)");

        JavaParser.cactionCompoundAssignment(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (CompoundAssignment,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(CompoundAssignment node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (CompoundAssignment,BlockScope)");

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (CompoundAssignment,BlockScope): operator_kind" + node.toString());

        int operator_kind = node.operator;

        JavaParser.cactionCompoundAssignmentEnd(operator_kind, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (CompoundAssignment,BlockScope)");
    }


    public boolean enter(ConditionalExpression node,BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ConditionalExpression,BlockScope)");

        JavaParser.cactionConditionalExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ConditionalExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ConditionalExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (ConditionalExpression,BlockScope)");

        JavaParser.cactionConditionalExpressionEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ConditionalExpression,BlockScope)");
    }


    public boolean enter(ConstructorDeclaration node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ConstructorDeclaration,ClassScope)");

        assert(! node.isDefaultConstructor());
        
        // char [] name = node.selector;
        // System.out.println("Inside of enter (ConstructorDeclaration,ClassScope) method name = " + node.selector);
        String name = javaParserSupport.getMethodName(node.binding);
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ConstructorDeclaration,ClassScope) method name = " + name);

        //
        //
        //
        if (node.constructorCall != null) {
            MethodHeaderDelimiters.put(node.constructorCall, node);
        }
        else if (node.statements != null && node.statements.length > 0) {
            MethodHeaderDelimiters.put(node.statements[0], node);
        }
        
        //
        // TODO: Do something !!!
        //
/*        
        if (node.typeParameters != null) {
            System.out.println();
            System.out.println("*** No support yet for constructor type parameters");
            throw new RuntimeException(); // System.exit(1);
        }
*/

        int constructor_index = javaParserSupport.getMethodIndex(node);
        JavaParser.cactionConstructorDeclaration(name, constructor_index, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ConstructorDeclaration,ClassScope)");

        //
        // We perform our own traversal for MethodDeclaration so that we can skip certain subcomponents
        // (commented out below).
        //
        //
        /*
       if (node.javadoc != null) {
            node.javadoc.traverse(this, node.scope);
        }
        if (node.annotations != null) {
            int annotationsLength = node.annotations.length;
            for (int i = 0; i < annotationsLength; i++)
                node.annotations[i].traverse(this, node.scope);
        }
        */

        if (node.typeParameters != null) {
            int typeParametersLength = node.typeParameters.length;
            for (int i = 0; i < typeParametersLength; i++) {
                node.typeParameters[i].traverse(this, node.scope);
            }
        }

        /*
        if (node.arguments != null) {
            int argumentLength = node.arguments.length;
            for (int i = 0; i < argumentLength; i++)
                node.arguments[i].traverse(this, node.scope);
        }
        */

        if (node.thrownExceptions != null) {
            int thrownExceptionsLength = node.thrownExceptions.length;
            for (int i = 0; i < thrownExceptionsLength; i++)
                node.thrownExceptions[i].traverse(this, node.scope);
        }
        if (node.constructorCall != null)
            node.constructorCall.traverse(this, node.scope);
        if (node.statements != null) {
            int statementsLength = node.statements.length;
            for (int i = 0; i < statementsLength; i++)
                node.statements[i].traverse(this, node.scope);
        }
        return false; // do nothing by node, keep traversing
    }

    public void exit(ConstructorDeclaration node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (ConstructorDeclaration,ClassScope)");

        if (! node.isDefaultConstructor()) {        
            // DQ (7/31/2011): Added more precise handling of statements to be collected from the statement stack.
            int numberOfStatements = 0;
            if (node.statements != null) {
                numberOfStatements = node.statements.length;
                if (javaParserSupport.verboseLevel > 0)
                    System.out.println("Inside of exit (ConstructorDeclaration,ClassScope): numberOfStatements = " + numberOfStatements);
            }

            if (node.constructorCall != null && (! node.constructorCall.isImplicitSuper())) { // is there an Explicit constructor call?
                numberOfStatements++;
                // System.out.println("Inside of exit (ConstructorDeclaration,ClassScope): increment the numberOfStatements = " + numberOfStatements);
            }
          
            if (node.constructorCall == null && (node.statements == null || node.statements.length == 0)) {
                if (javaParserSupport.verboseLevel > 1)
                    System.out.println("    Side-visiting Constructor Declaration Header for " + node.getClass().getName());
                javaParserSupport.processConstructorDeclarationHeader(node, this.unitInfo.createJavaToken(node));
            }

            JavaParser.cactionConstructorDeclarationEnd(numberOfStatements, this.unitInfo.createJavaToken(node));
        }
    }


    public boolean enter(ContinueStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ContinueStatement,BlockScope)");

        JavaParser.cactionContinueStatement((node.label == null ? "" : new String(node.label)), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ContinueStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ContinueStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ContinueStatement,BlockScope)");
    }


    public boolean enter(DoStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (DoStatement,BlockScope)");

        JavaParser.cactionDoStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (DoStatement,BlockScope)");

        return true; // do nothing by  node, keep traversing
    }

    public void exit(DoStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (DoStatement,BlockScope)");

        JavaParser.cactionDoStatementEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (DoStatement,BlockScope)");
    }


    public boolean enter(DoubleLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (DoubleLiteral,BlockScope)");

        JavaParser.cactionDoubleLiteral(node.constant.doubleValue(), new String(node.source()), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (DoubleLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(DoubleLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (DoubleLiteral,BlockScope)");
    }


    public boolean enter(EmptyStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (EmptyStatement,BlockScope)");

        JavaParser.cactionEmptyStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (EmptyStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(EmptyStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (EmptyStatement,BlockScope)");

        JavaParser.cactionEmptyStatementEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (EmptyStatement,BlockScope)");
    }


    public boolean enter(EqualExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (EqualExpression,BlockScope)");

        JavaParser.cactionEqualExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (EqualExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(EqualExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (EqualExpression,BlockScope)");

        int operator_kind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT; // EQUAL_EQUAL or NOT_EQUAL

        JavaParser.cactionEqualExpressionEnd(operator_kind, this.unitInfo.createJavaToken(node));
    }


    public boolean enter(ExplicitConstructorCall node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ExplicitConstructorCall,BlockScope)");

        if (node.isImplicitSuper())
            return false;
        //
        // TODO: Do something !!!
        //
/*        
        if (node.genericTypeArguments != null) {
            System.out.println();
            System.out.println("*** No support yet for constructor type arguments");
            throw new RuntimeException(); // System.exit(1);
        }
*/

        JavaParser.cactionExplicitConstructorCall(this.unitInfo.createJavaToken(node));
          
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ExplicitConstructorCall,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ExplicitConstructorCall node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (ExplicitConstructorCall,BlockScope)");

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("In visit (ExplicitConstructorCall,BlockScope): node.accessMode = " + node.accessMode);

        if (node.isImplicitSuper())
            return;

        pushRawMethodParameterTypes(node.binding, this.unitInfo.createJavaToken(node));

        JavaParser.cactionExplicitConstructorCallEnd(node.isImplicitSuper(),
                                                     node.isSuperAccess(),
                                                     node.qualification != null,
                                                     node.binding.parameters.length,
                                                     node.typeArguments == null ? 0 : node.typeArguments.length,
                                                     node.arguments == null ? 0 : node.arguments.length,
                                                     this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ExplicitConstructorCall,BlockScope)");
    }


    public boolean enter(ExtendedStringLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ExtendedStringLiteral,BlockScope)");

        JavaParser.cactionExtendedStringLiteral(node.constant.stringValue(), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ExtendedStringLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ExtendedStringLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ExtendedStringLiteral,BlockScope)");
    }


    public boolean enter(FalseLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (FalseLiteral,BlockScope)");

        JavaParser.cactionFalseLiteral(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (FalseLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(FalseLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (FalseLiteral,BlockScope)");
    }


    public boolean enter(FieldDeclaration node, MethodScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (FieldDeclaration,BlockScope)");

        // DO NOTHING !!!
          
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (FieldDeclaration,BlockScope)");

        return true;
    }

    public void exit(FieldDeclaration node, MethodScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (FieldDeclaration,BlockScope)");

        boolean isPrivate   = (node.binding != null && node.binding.isPrivate());
        boolean isProtected = (node.binding != null && node.binding.isProtected());
        boolean isPublic    = (node.binding != null && node.binding.isPublic());

        boolean isVolatile  = (node.binding != null && node.binding.isVolatile());
        boolean isSynthetic = (node.binding != null && node.binding.isSynthetic());
        boolean isStatic    = (node.binding != null && node.binding.isStatic());
        boolean isTransient = (node.binding != null && node.binding.isTransient());

        boolean isFinal = node.binding.isFinal();

        boolean hasInitializer = node.initialization != null;

// TODO: Remove this
//
// TODO: Process this properly !!!
//
/*
if (node.getKind() == AbstractVariableDeclaration.ENUM_CONSTANT) {
if (hasInitializer) {
System.out.println("** Found an enumeration field " + new String(node.name) + ", with initialization !!!");        
//            throw new RuntimeException("*** No support yet for Enum Values with initialization"); // System.exit(1);
}
else System.out.println("** Found an enumeration field " + new String(node.name) + ", without initialization !!!");        
}
else System.out.println("** Found a  regular field " + new String(node.name));        
*/
        String name = new String(node.name);

        JavaParser.cactionFieldDeclarationEnd(name, 
                                              node.getKind() == AbstractVariableDeclaration.ENUM_CONSTANT,
                                              hasInitializer,
                                              isFinal,
                                              isPrivate,
                                              isProtected,
                                              isPublic,
                                              isVolatile,
                                              isSynthetic,
                                              isStatic,
                                              isTransient,
                                              this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (FieldDeclaration,BlockScope)");
    }


    public boolean enter(FieldReference node, BlockScope scope) {
        // System.out.println("Sorry, not implemented in support for FieldReference(BlockScope): xxx");
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (FieldReference,BlockScope)");

        JavaParser.cactionFieldReference(new String(node.token), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (FieldReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(FieldReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (FieldReference,BlockScope)");

// TODO: Remove this !!!
//StringBuffer buffer = new StringBuffer();
//System.out.println("(1) I am looking at field reference " + new String(node.printExpression(0, buffer)) + " with receiver type " + node.actualReceiverType.debugName());
        javaParserSupport.generateAndPushType(node.actualReceiverType, this.unitInfo, this.unitInfo.createJavaToken(node), false /* is_formal_parameter_type_mapping */); // push the receiver type
        JavaParser.cactionFieldReferenceEnd(true /* explicit type passed */, new String(node.token), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (FieldReference,BlockScope)");
    }


    public boolean enter(FieldReference node, ClassScope scope) {
        // System.out.println("Sorry, not implemented in support for FieldReference(ClassScope): xxx");
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (FieldReference,ClassScope)");

        JavaParser.cactionFieldReference(new String(node.token), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (FieldReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(FieldReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (FieldReference,ClassScope)");

// TODO: Remove this !!!
//StringBuffer buffer = new StringBuffer();
//System.out.println("(2) I am looking at field reference " + new String(node.printExpression(0, buffer)) + " with receiver type " + node.actualReceiverType.debugName());
        javaParserSupport.generateAndPushType(node.actualReceiverType, this.unitInfo, this.unitInfo.createJavaToken(node), false /* is_formal_parameter_type_mapping */); // push the receiver type
        JavaParser.cactionFieldReferenceEnd(true /* explicit type passed */, new String(node.token), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (FieldReference,BlockScope)");
    }


    public boolean enter(FloatLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (FloatLiteral,BlockScope)");

        JavaParser.cactionFloatLiteral(node.constant.floatValue(), new String(node.source()), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (FloatLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(FloatLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (FloatLiteral,BlockScope)");
    }


    public boolean enter(ForeachStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
               System.out.println("Inside of enter (ForeachStatement,BlockScope)");

        JavaParser.cactionForeachStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ForeachStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ForeachStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ForeachStatement,BlockScope)");

        JavaParser.cactionForeachStatementEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ForeachStatement,BlockScope)");
    }


    public boolean enter(ForStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ForStatement,BlockScope)");

        JavaParser.cactionForStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ForStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ForStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (ForStatement,BlockScope)");

        JavaParser.cactionForStatementEnd(node.initializations == null ? 0 : node.initializations.length,
                                          node.condition != null,
                                          node.increments == null ? 0 : node.increments.length,
                                          this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ForStatement,BlockScope)");
    }


    public boolean enter(IfStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (IfStatement,BlockScope)");

        JavaParser.cactionIfStatement(node.elseStatement != null, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (IfStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(IfStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (IfStatement,BlockScope)");

        JavaParser.cactionIfStatementEnd(node.elseStatement != null, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (IfStatement,BlockScope)");
    }

    public boolean enter(ImportReference node, CompilationUnitScope scope) {
        if (javaParserSupport.verboseLevel > 1)
            System.out.println("Inside of enter (ImportReference,CompilationUnitScope)");

// TODO: Remove this - Already taken care of during preprocessing of unit        
/*
        //
        // An Import statement may refer to a package name (optionally followed by ".*"), a type name, which
        // in the case of a static import may optionally followed by ".*" or a name suffix (the suffix
        // represents a field or a method name contained in the type in question). 
        //
        Class cls = null;
        String name_suffix = "";

        //
        // Construct the import reference name.  While we are at it, look for a contained class, if any, in that name.
        //
        StringBuffer import_reference = new StringBuffer();
        for (int i = 0, tokenArrayLength = node.tokens.length; i < tokenArrayLength; i++) {
            String token_string = new String(node.tokens[i]);

            if (i > 0) { // Not the first simple name?  Add a "." seperator to the QualifiedName 
                import_reference.append('.');
            }
            import_reference.append(token_string);

            //
            // Now, look to see if the name we've constructed so far is the name of a class.
            // Once we've found the longest prefix of the import name that is a class, we append
            // the remaining suffix to the name_suffix variable.
            //
            Class c = javaParserSupport.getClassForName(import_reference.toString());
            if (c != null) { // a class?
                cls = c; // save the first class we find.
            }
            else if (cls != null){ // already found a class, so this name is a suffix.
                name_suffix += token_string;
                // TODO: look for the inner class here? 
            }
        }

        boolean containsWildcard = ((node.bits & node.OnDemand) != 0);
        String importReferenceWithoutWildcard = import_reference.toString();

        if (javaParserSupport.verboseLevel > 1)
            System.out.println("importReference (string) = " + import_reference.toString());

        String package_name,
               type_name;

        if (cls == null) { // We may just have a package ... Process it.
            package_name = importReferenceWithoutWildcard;
            type_name = "";

            JavaParser.cactionPushPackage(package_name, this.unitInfo.createJavaToken(node));
            JavaParser.cactionIdentifyImportedPackage(package_name, this.unitInfo.createJavaToken(node));
            JavaParser.cactionPopPackage();
        }
        else {
            // 
            // Make sure this class is available
            //
            javaParserSupport.preprocessClass(cls);

            String canonical_name = cls.getCanonicalName();
            package_name = javaParserSupport.getMainPackageName(cls);
            type_name = canonical_name.substring(package_name.length() == 0 ? 0 : package_name.length() + 1);
        }

        if (! (package_name.equals(javaParserSupport.unitPackageName) && type_name.length() == 0)) { // Do not import the current package! 
            JavaParser.cactionImportReference(node.isStatic(), package_name, type_name, name_suffix, containsWildcard, this.unitInfo.createJavaToken(node));
        }
*/
/*        
        String package_name = (javaParserSupport.importPackageName.containsKey(node) ? javaParserSupport.importPackageName.get(node) : ""),
               type_name = (javaParserSupport.importTypeName.containsKey(node) ? javaParserSupport.importTypeName.get(node) : ""),
               name_suffix = (javaParserSupport.importNameSuffix.containsKey(node) ? javaParserSupport.importNameSuffix.get(node) : "");

        boolean contains_wildcard = ((node.bits & node.OnDemand) != 0);
        
        if ((package_name.length() > 0 || type_name.length() > 0) && (! (package_name.equals(javaParserSupport.unitPackageName) && type_name.length() == 0))) { // Do not import the current package! 
            // 
            // Make sure this class is available
            //
            Class cls = javaParserSupport.importClass.get(node);
            if (cls != null) {
                javaParserSupport.preprocessClass(cls);
            }

            JavaParser.cactionImportReference(node.isStatic(), package_name, type_name, name_suffix, contains_wildcard, this.unitInfo.createJavaToken(node));
        }
*/
        if (node != this.unitInfo.unit.currentPackage) { // Do no import the current package.
            boolean contains_wildcard = ((node.bits & node.OnDemand) != 0);
/*
            // 
            // Make sure this class is available
            //
            String filename = new String(scope.referenceContext.getFileName());
            Class cls = javaParserSupport.importClass.get(filename).get(node);
            if (cls != null) {
                Class base_class = cls;
                while (base_class.getDeclaringClass() != null) {
                    name_suffix = (name_suffix.length() == 0 ? base_class.getSimpleName() : (base_class.getSimpleName() + "." + name_suffix));
                    base_class = base_class.getDeclaringClass();
                }

                String canonical_name = base_class.getCanonicalName();
                type_name = base_class.getSimpleName();
                package_name = (type_name.length() < canonical_name.length()
                                    ? canonical_name.substring(0, canonical_name.length() - type_name.length() - 1)
                                    : "");
            }
            else {
                package_name = new String(CharOperation.concatWith(node.tokens, '.'));
            }
*/
      
            String import_name = new String(CharOperation.concatWith(node.getImportName(), '.'));
            JavaParser.cactionImportReference(node.isStatic(), import_name, contains_wildcard, this.unitInfo.createJavaToken(node));
        }

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ImportReference,CompilationUnitScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ImportReference node, CompilationUnitScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ImportReference,CompilationUnitScope)");
    }


    public boolean enter(Initializer node, MethodScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Initializer,MethodScope)");
        int initializer_index = javaParserSupport.classInitializerTable.get(node);
        JavaParser.cactionInitializer(node.isStatic(), javaParserSupport.initializerName.get(node), initializer_index, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Initializer,MethodScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Initializer node, MethodScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Initializer,MethodScope)");

        JavaParser.cactionInitializerEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Initializer, MethodScope)");
    }


    public boolean enter( InstanceOfExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (InstanceOfExpression,BlockScope)");

        JavaParser.cactionInstanceOfExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (InstanceOfExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(InstanceOfExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (InstanceOfExpression,BlockScope)");

        JavaParser.cactionInstanceOfExpressionEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (InstanceOfExpression,BlockScope)");
    }


    public boolean enter(IntLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (IntLiteral,BlockScope) value = " + node.toString());

        //
        // Do not use node.source() for the string representation of this integer because it yields an incorrect result for -2147483648
        //
        JavaParser.cactionIntLiteral(node.constant.intValue(), "" + node.constant.intValue(), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (IntLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(IntLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (IntLiteral,BlockScope)");
    }


    //**********************************************************
    //*                                                        *
    //*    Start of JavaDoc Nodes.                             *
    //*                                                        *
    //**********************************************************
/*
    public boolean enter(Javadoc node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Javadoc,BlockScope)");

        JavaParser.cactionJavadoc(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Javadoc,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Javadoc node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Javadoc,BlockScope)");
    }


    public boolean enter(Javadoc node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Javadoc,ClassScope)");

        JavaParser.cactionJavadocClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Javadoc,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Javadoc node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Javadoc,ClassScope)");
    }


    public boolean enter(JavadocAllocationExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocAllocationExpression,BlockScope)");

        JavaParser.cactionJavadocAllocationExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocAllocationExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocAllocationExpression node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocAllocationExpression,BlockScope)");
    }


    public boolean enter(JavadocAllocationExpression node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocAllocationExpression,ClassScope)");

        JavaParser.cactionJavadocAllocationExpressionClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocAllocationExpression,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocAllocationExpression node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocAllocationExpression,ClassScope)");
    }


    public boolean enter(JavadocArgumentExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter JavadocArgumentExpression(JavadocArgumentExpression,BlockScope)");

        JavaParser.cactionJavadocArgumentExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocArgumentExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocArgumentExpression node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocArgumentExpression,BlockScope)");
    }


    public boolean enter(JavadocArgumentExpression node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocArgumentExpression,ClassScope)");

        JavaParser.cactionJavadocArgumentExpressionClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocArgumentExpression,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocArgumentExpression node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocArgumentExpression,ClassScope)");
    }


    public boolean enter(JavadocArrayQualifiedTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocArrayQualifiedTypeReference,BlockScope)");

        JavaParser.cactionJavadocArrayQualifiedTypeReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocArrayQualifiedTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocArrayQualifiedTypeReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocArrayQualifiedTypeReference,BlockScope)");
    }


    public boolean enter(JavadocArrayQualifiedTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocArrayQualifiedTypeReference,ClassScope)");

        JavaParser.cactionJavadocArrayQualifiedTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocArrayQualifiedTypeReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocArrayQualifiedTypeReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocArrayQualifiedTypeReference,ClassScope)");
    }


    public boolean enter(JavadocArraySingleTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocArraySingleTypeReference,BlockScope)");

        JavaParser.cactionJavadocArraySingleTypeReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocArraySingleTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocArraySingleTypeReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocArraySingleTypeReference,BlockScope)");
    }


    public boolean enter(JavadocArraySingleTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocArraySingleTypeReference,ClassScope)");

        JavaParser.cactionJavadocArraySingleTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocArraySingleTypeReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocArraySingleTypeReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocArraySingleTypeReference,ClassScope)");
    }


    public boolean enter(JavadocFieldReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocFieldReference,BlockScope)");

        JavaParser.cactionJavadocFieldReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocFieldReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocFieldReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocFieldReference,BlockScope)");
    }


    public boolean enter(JavadocFieldReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocFieldReference,ClassScope)");

        JavaParser.cactionJavadocFieldReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocFieldReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocFieldReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocFieldReference,ClassScope)");
    }


    public boolean enter(JavadocImplicitTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocImplicitTypeReference,BlockScope)");

        JavaParser.cactionJavadocImplicitTypeReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocImplicitTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocImplicitTypeReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocImplicitTypeReference,BlockScope)");
    }


    public boolean enter(JavadocImplicitTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocImplicitTypeReference,ClassScope)");

        JavaParser.cactionJavadocImplicitTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocImplicitTypeReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocImplicitTypeReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocImplicitTypeReference,ClassScope)");
    }


    public boolean enter(JavadocMessageSend node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocMessageSend,BlockScope)");

        JavaParser.cactionJavadocMessageSend(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocMessageSend,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocMessageSend node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocMessageSend,BlockScope)");
    }


    public boolean enter(JavadocMessageSend node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocMessageSend,ClassScope)");

        JavaParser.cactionJavadocMessageSendClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocMessageSend,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocMessageSend node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocMessageSend,ClassScope)");
    }


    public boolean enter(JavadocQualifiedTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocQualifiedTypeReference,BlockScope)");

        JavaParser.cactionJavadocQualifiedTypeReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocQualifiedTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocQualifiedTypeReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocQualifiedTypeReference,BlockScope)");
    }


    public boolean enter(JavadocQualifiedTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocQualifiedTypeReference,ClassScope)");

        JavaParser.cactionJavadocQualifiedTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocQualifiedTypeReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocQualifiedTypeReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocQualifiedTypeReference,ClassScope)");
    }


    public boolean enter(JavadocReturnStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocReturnStatement,BlockScope)");

        JavaParser.cactionJavadocReturnStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocReturnStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocReturnStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocReturnStatement,BlockScope)");
    }


    public boolean enter(JavadocReturnStatement node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocReturnStatement,ClassScope)");

        JavaParser.cactionJavadocReturnStatementClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocReturnStatement,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocReturnStatement node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocReturnStatement,ClassScope)");
    }


    public boolean enter(JavadocSingleNameReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocSingleNameReference,BlockScope)");

        JavaParser.cactionJavadocSingleNameReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocSingleNameReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocSingleNameReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocSingleNameReference,BlockScope)");
    }


    public boolean enter(JavadocSingleNameReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocSingleNameReference,ClassScope)");

        JavaParser.cactionJavadocSingleNameReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocSingleNameReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocSingleNameReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocSingleNameReference,ClassScope)");
    }


    public boolean enter(JavadocSingleTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocSingleTypeReference,BlockScope)");

        JavaParser.cactionJavadocSingleTypeReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocSingleTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocSingleTypeReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocSingleTypeReference,BlockScope)");
    }


    public boolean enter(JavadocSingleTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (JavadocSingleTypeReference,ClassScope)");

        JavaParser.cactionJavadocSingleTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (JavadocSingleTypeReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(JavadocSingleTypeReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (JavadocSingleTypeReference,ClassScope)");
    }
*/

    //**********************************************************
    //*                                                        *
    //*    End of JavaDoc Nodes.                               *
    //*                                                        *
    //**********************************************************

    public boolean enter(LabeledStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (LabeledStatement,BlockScope)");

        JavaParser.cactionLabeledStatement(new String(node.label), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (LabeledStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(LabeledStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (LabeledStatement,BlockScope)");

        JavaParser.cactionLabeledStatementEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (LabeledStatement,BlockScope)");
    }


    public boolean enter(LocalDeclaration node, BlockScope scope) {
        // LocalDeclarations is derived from AbstractVariableDeclaration

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (LocalDeclaration,BlockScope)");

        // Do Nothing 

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (LocalDeclaration,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(LocalDeclaration node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (LocalDeclaration,BlockScope)");

        String name = new String(node.name);

        boolean is_final = node.binding.isFinal();

        // Build the variable declaration using the type from the astJavaTypeStack.
        // Note that this may have to handle an array of names or be even more complex in the future.
        JavaParser.cactionLocalDeclarationEnd(name, node.initialization != null, is_final, this.unitInfo.createJavaToken(node));
    }


    public boolean enter(LongLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (LongLiteral,BlockScope)");

        JavaParser.cactionLongLiteral(node.constant.longValue(), new String(node.source()), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (LongLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(LongLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (LongLiteral,BlockScope)");
    }


    public boolean enter(MarkerAnnotation node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (MarkerAnnotation,BlockScope)");

        JavaParser.cactionMarkerAnnotation(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (MarkerAnnotation,BlockScope)");

        return true;
    }

    public void exit(MarkerAnnotation node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (MarkerAnnotation,BlockScope)");
    }


    public boolean enter(MemberValuePair node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (MemberValuePair,BlockScope)");

        JavaParser.cactionMemberValuePair(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (MemberValuePair,BlockScope)");

        return true;
    }

    public void exit(MemberValuePair node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (MemberValuePair,BlockScope)");
    }


    public boolean enter(MessageSend node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (MessageSend,BlockScope)");

        String method_name = javaParserSupport.getMethodName(node.binding);

// TODO: Remove this!?        
/*
        if (node.actualReceiverType.isEnum()) {
            if (method_name.equals("values")) {
                throw new RuntimeException("*** No support yet for values() method in an enum"); // System.exit(1);
            }
            else if (method_name.equals("valueOf")) {
                throw new RuntimeException("*** No support yet for valueOf() method in an enum"); // System.exit(1);
            }
        }
*/
        if (javaParserSupport.verboseLevel > 0) {
            System.out.println("MessageSend node = " + node);

            System.out.println("     --- function call name = " + method_name);

            System.out.println("     --- function call from class name (binding)            = " + node.binding);
            System.out.println("     --- function call from class name (receiver)           = " + node.receiver);
            System.out.println("     --- function call from class name (associatedClassVar) = " + node.receiver.toString());
            System.out.println("     --- function call from class name (associatedClass)    = " + node.actualReceiverType.debugName());
        }

// TODO: Remove this!?
        //
        // TODO: Do something !!!
        //
/*        
        if (node.genericTypeArguments != null) {
            System.out.println();
            System.out.println("*** No support yet for method type arguments");
            throw new RuntimeException(); // System.exit(1);
        }
*/

        // 
        // Make sure this class is available
        //
        javaParserSupport.preprocessClass(node.actualReceiverType, this.unitInfo);

// TODO: Remove this !!!
/*        
if (node.syntheticAccessor != null) {
System.out.println("The method " + method_name + " has a synthetic accessor: " + new String(node.syntheticAccessor.readableName()) + " in class " + node.syntheticAccessor.declaringClass.debugName());
}
*/
        JavaParser.cactionMessageSend(javaParserSupport.getPackageName(node.actualReceiverType),
                                      javaParserSupport.getTypeName(node.actualReceiverType),
                                      method_name,
                                      this.unitInfo.createJavaToken(node));
        
        //
        // We perform our own traversal for MessageSend so that we can control whether or not the
        // receiver is processed.
        //
        if (! node.receiver.isImplicitThis()) { // traverse the receiver only if it's not an implicit this.
// TODO: Remove this !!!
/*
StringBuffer buffer = new StringBuffer();
System.out.println("I am looking at method reference " + new String(node.receiver.printExpression(0, buffer)) + " with receiver type " + node.actualReceiverType.debugName() + " (" + node.receiver.resolvedType.debugName() + ")");
*/
            node.receiver.traverse(this, scope);
        }
        if (node.typeArguments != null) { // traverse the type arguments
            for (int i = 0, typeArgumentsLength = node.typeArguments.length; i < typeArgumentsLength; i++) {
                node.typeArguments[i].traverse(this, scope);
            }
        }
        if (node.arguments != null) { // traverse the method arguments
            for (int i = 0, argumentsLength = node.arguments.length; i < argumentsLength; i++) {
                node.arguments[i].traverse(this, scope);
            }
        }

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (MessageSend,BlockScope)");

        return false;
    }


    public void exit(MessageSend node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (MessageSend,BlockScope): " + nodeId(node));

        pushRawMethodParameterTypes(node.binding, this.unitInfo.createJavaToken(node));

// TODO: Remove this !
/*        
// javaParserSupport.generateAndPushType(node.binding.returnType, this.unitInfo.createJavaToken(node)); // push the return type
System.out.print("node.actualReceiverType = " + node.actualReceiverType.debugName() + " (" + node.actualReceiverType.getClass().getCanonicalName() + ")");
if(node.actualReceiverType.isAnonymousType())
	System.out.print("; anonymous");
if (node.actualReceiverType.isBoundParameterizedType())
	System.out.print("; bound parameterized");
if (node.actualReceiverType.isCapture())
	System.out.print("; Capture");
if (node.actualReceiverType.isLocalType())
	System.out.print("; local");
System.out.println();
System.out.println("node.actualReceiverType.qualifiedSourceName = " + new String(node.actualReceiverType.qualifiedSourceName()));
System.out.println("node.actualReceiverType.shortReadableName = " + new String(node.actualReceiverType.shortReadableName()));
System.out.println("node.actualReceiverType.readableName = " + new String(node.actualReceiverType.readableName()));
*/
        javaParserSupport.generateAndPushType(node.actualReceiverType.erasure(), this.unitInfo, this.unitInfo.createJavaToken(node), false /* is_formal_parameter_type_mapping */); // push the receiver type

        JavaParser.cactionMessageSendEnd(node.binding.isStatic(),
                                         (! node.receiver.isImplicitThis()),
                                         javaParserSupport.getMethodName(node.binding),
                                         node.binding.parameters.length,
                                         node.typeArguments == null ? 0 : node.typeArguments.length,
                                         node.arguments == null ? 0 : node.arguments.length,
                                         this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (MessageSend,BlockScope)");
    }


    /**
     * 
     * @param method_binding
     * @param location
     */
    private void pushRawMethodParameterTypes(MethodBinding method_binding, JavaToken location) {
//        method_binding = method_binding.original();
// TODO: Remove this !
//System.out.println("Looking for method " + new String(method_binding.selector) + " with binding type " + method_binding.getClass().getCanonicalName() +
//                        " in type " + method_binding.declaringClass.debugName() + " with binding type " + method_binding.declaringClass.getClass().getCanonicalName());

//        method_binding = method_binding.original();

        String qualified_name = javaParserSupport.getCanonicalName(method_binding.declaringClass);

/*        
        if (javaParserSupport.classProcessed.get(qualified_name) instanceof TypeDeclaration) {
//TODO: Remove this !
//System.out.println("Processing original parameters for Method " + new String(method_binding.selector) + " declared in class " + method_binding.declaringClass.debugName() + " in file " +
//               new String(((SourceTypeBinding) type_binding).scope.compilationUnitScope().referenceContext.getFileName()));

            TypeBinding parameter_types[] = method_binding.parameters;
            for (int i = 0; i < parameter_types.length; i++) {
                TypeBinding parameter_type = parameter_types[i];
                javaParserSupport.generateAndPushType(parameter_type, location);
            }
        }
        else { // A method imported from a class ... even if it is a user-defined method.
            method_binding = method_binding.original();
//TODO: Remove this !
//if (type_binding instanceof SourceTypeBinding)
System.out.println("Processing raw parameters for method: " + javaParserSupport.getMethodName(method_binding) + " in type " + type_binding.debugName() + " (" + qualified_name + ")");

            processRawParameters(method_binding);
        }
*/        
        if (! (javaParserSupport.classProcessed.get(qualified_name) instanceof TypeDeclaration)) {
            method_binding = method_binding.original();
        }

        TypeBinding parameter_types[] = method_binding.parameters;
        for (int i = 0; i < parameter_types.length; i++) {
            TypeBinding parameter_type_binding = parameter_types[i];
/*            
            if (parameter_type_binding instanceof TypeVariableBinding) {
                if (parameter_type_binding instanceof CaptureBinding) {
                    CaptureBinding capture_binding = (CaptureBinding) parameter_type_binding;
                    if (capture_binding.wildcard != null) {
                        javaParserSupport.generateAndPushType(capture_binding.wildcard, location);
                    }
                    else {
                        throw new RuntimeException("*** No support yet for Type Variable binding " + new String(parameter_type_binding.shortReadableName()) + " with binding type " + parameter_type_binding.getClass().getCanonicalName()); // System.exit(1);
                    }
                }
                else {
                    Binding scope_binding = ((TypeVariableBinding) parameter_type_binding).declaringElement;
    // TODO: Remove this !
    System.out.println("Looking at type variable " + parameter_type_binding.debugName() + " in container " + new String(scope_binding.readableName())+ " of type " + scope_binding.getClass().getCanonicalName());
                    String type_parameter_name = javaParserSupport.getTypeName(parameter_type_binding);
                    if (scope_binding instanceof TypeBinding) {
                        TypeBinding enclosing_binding = (TypeBinding) scope_binding;
                        String package_name = javaParserSupport.getPackageName(enclosing_binding),
                               type_name = javaParserSupport.getTypeName(enclosing_binding);
                        JavaParser.cactionArgumentTypeParameterReference(package_name, type_name, (int) -1, type_parameter_name, location);
                    }
                    else if (scope_binding instanceof MethodBinding) {
                        MethodBinding containing_method_binding = (MethodBinding) scope_binding;
                        AbstractMethodDeclaration method_declaration = containing_method_binding.sourceMethod();
                        int method_index = javaParserSupport.getMethodIndex(containing_method_binding);
                        TypeBinding enclosing_type_binding = containing_method_binding.declaringClass;
                        String package_name = javaParserSupport.getPackageName(enclosing_type_binding),
                               type_name = javaParserSupport.getTypeName(enclosing_type_binding);
                        JavaParser.cactionArgumentTypeParameterReference(package_name, type_name, method_index, type_parameter_name, location);
                    }
                    else {
                        throw new RuntimeException("*** No support yet for Type Variable " + new String(parameter_type_binding.shortReadableName()) + " with binding type " + parameter_type_binding.getClass().getCanonicalName() + " enclosed in " + (scope_binding == null ? "?" : scope_binding.getClass().getCanonicalName())); // System.exit(1);
                    }
                }
            }
            else 
*/
                javaParserSupport.generateAndPushType(parameter_type_binding, this.unitInfo, location, true /* is_formal_parameter_type_mapping */);
        }
    }

    //
    // Push the types of the parameters of a method that was matched for a call so that the
    // translator can retrieve the exact method in question.
    //
// OLDCODE:
/*    
    private void pushRawMethodParameterTypes(MethodBinding method_binding, JavaToken location) {
*/
// TODO: Remove this !
/*    
//System.out.println("Looking for method " + new String(method_binding.selector) + " with binding type " + method_binding.getClass().getCanonicalName() +
//                   " in type " + method_binding.declaringClass.debugName() + " with binding type " + method_binding.declaringClass.getClass().getCanonicalName());
        TypeBinding type_binding = method_binding.declaringClass;
        if (type_binding instanceof ParameterizedTypeBinding) {
// TODO: Remove this !
//System.out.println("Method " + new String(method_binding.selector) + " is a parameterized method"); 
            ParameterizedTypeBinding parm_type_binding = (ParameterizedTypeBinding) type_binding;
            if (parm_type_binding.original() instanceof BinaryTypeBinding) {
                method_binding = ((ParameterizedMethodBinding) method_binding).original();
                type_binding = parm_type_binding.original();
// TODO: Remove this !
//System.out.println("Looking for Method " + new String(method_binding.selector) + " in raw type " + type_binding.debugName()); 
            }
        }
        
        //
        // If the method being invoked belongs to a class file (binary), retrieve the raw method signature from the class.
        //
        if (type_binding instanceof BinaryTypeBinding) {
// TODO: Remove this !
//System.out.println("Processing Method " + new String(method_binding.selector) + " in raw type " + type_binding.debugName()); 
            processRawParameters(method_binding);
        }
        else {
            Class cls = javaParserSupport.findClass(type_binding);
            assert(cls != null);

            if (method_binding instanceof SyntheticMethodBinding) { // What is this !???
                SyntheticMethodBinding synthetic_binding = (SyntheticMethodBinding) method_binding;

                System.out.println();
                System.out.println("*** No support yet for calls to method with synthetic binding");
                throw new RuntimeException(); // System.exit(1);
            }
            
            if (javaParserSupport.userTypeTable.containsKey(cls)) { // A user-defined method that I am currently processing?
// TODO: Remove this !
System.out.println("Processing original parameters for Method " + new String(method_binding.selector) + " declared in class " + method_binding.declaringClass.debugName());
if (method_binding.declaringClass instanceof ParameterizedTypeBinding) {
    ParameterizedTypeBinding parm_type_binding = (ParameterizedTypeBinding) type_binding;
System.out.println("The original method is " + new String(method_binding.original().selector) + " declared in class " + method_binding.original().declaringClass.debugName());    
}
                TypeBinding parameter_types[] = method_binding.parameters;
                for (int i = 0; i < parameter_types.length; i++) {
                    TypeBinding parameter_type = parameter_types[i];
                    javaParserSupport.generateAndPushType(parameter_type, location);
                }
            }
            else { // A method imported from a class ... even if it is a user-defined method.
// TODO: Remove this !
//System.out.println("Processing raw parameters for imported user-defined Method: " + new String(method_binding.selector));             
                processRawParameters(method_binding);
            }
        }
*/

//OLDCODE:
/*    
        method_binding = method_binding.original();
*/
    
// TODO: Remove this !
//System.out.println("Looking for method " + new String(method_binding.selector) + " with binding type " + method_binding.getClass().getCanonicalName() +
//                   " in type " + method_binding.declaringClass.debugName() + " with binding type " + method_binding.declaringClass.getClass().getCanonicalName());

//OLDCODE:
/*    
        TypeBinding type_binding = method_binding.declaringClass;
*/            
        //
        // If the method being invoked belongs to a class file (binary), retrieve the raw method signature from the class.
        //

//OLDCODE:
/*    
        if (type_binding instanceof BinaryTypeBinding) {
*/

// TODO: Remove this !
//System.out.println("Processing Method " + new String(method_binding.selector) + " in raw type " + type_binding.debugName() +
//                   " with binding type " + type_binding.getClass().getCanonicalName());

//OLDCODE:
/*    
            processRawParameters(method_binding);
         }
*/
        
// TODO: Remove this !
/*
        else if (method_binding instanceof SyntheticMethodBinding) {
            SyntheticMethodBinding synthetic_binding = (SyntheticMethodBinding) method_binding;

            System.out.println();
            System.out.println("*** No support yet for calls to method (" + new String(method_binding.selector) + " in type " + type_binding.debugName() +  ") with synthetic binding and \"purpose\" = " + synthetic_binding.purpose);
            throw new RuntimeException(); // System.exit(1);
        }
*/

//OLDCODE:
/*    
        else {
*/

// TODO: Remove this !
/*
            Class cls = javaParserSupport.findClass(type_binding);
            assert(cls != null);
System.out.println("qualifiedPackageName = " + new String(type_binding.qualifiedPackageName()));
System.out.println("qualifiedSourceName = " + new String(type_binding.qualifiedSourceName()));
System.out.println("signature = " + new String(type_binding.signature()));
System.out.println("sourceName = " + new String(type_binding.sourceName()));
System.out.println("readableName = " + new String(type_binding.readableName()));
System.out.println("debugName = " + type_binding.debugName());
System.out.println("The binding type is " + type_binding.getClass().getCanonicalName());

            if (javaParserSupport.userTypeTable.containsKey(cls)) { // A user-defined method that I am currently processing?
// TODO: Remove this !
System.out.println("Processing original parameters for Method " + new String(method_binding.selector) + " declared in class " + method_binding.declaringClass.debugName());
                TypeBinding parameter_types[] = method_binding.parameters;
                for (int i = 0; i < parameter_types.length; i++) {
                    TypeBinding parameter_type = parameter_types[i];
                    javaParserSupport.generateAndPushType(parameter_type, location);
                }
            }
*/

//OLDCODE:
/*
            SourceTypeBinding source_type_binding = (type_binding instanceof SourceTypeBinding ? (SourceTypeBinding) type_binding : null);
            if (source_type_binding != null && JavaTraversal.processedFiles.contains(new String(source_type_binding.scope.compilationUnitScope().referenceContext.getFileName()))) {
*/

// TODO: Remove this !
//System.out.println("Processing original parameters for Method " + new String(method_binding.selector) + " declared in class " + method_binding.declaringClass.debugName() + " in file " +
//                   new String(source_type_binding.scope.compilationUnitScope().referenceContext.getFileName()));
//if (new String(method_binding.selector).equals("analyzeMethod"))
//System.out.println("analyzeMethod() in " + source_type_binding.debugName() + " has a SourceTypeBinding");

// OLDCODE:
/*    
                TypeBinding parameter_types[] = method_binding.parameters;
                for (int i = 0; i < parameter_types.length; i++) {
                    TypeBinding parameter_type = parameter_types[i];
                    javaParserSupport.generateAndPushType(parameter_type, location);
                }
            }
            else { // A method imported from a class ... even if it is a user-defined method.
*/

// TODO: Remove this !
//System.out.println("Processing raw parameters for imported user-defined Method: " + new String(method_binding.selector) + " in type " + type_binding.debugName());
//if (new String(method_binding.selector).equals("analyzeMethod"))
//System.out.println("analyzeMethod() in " + source_type_binding.debugName() + " has a source binding but it was loaded from a class file and should use a BinaryTypeBinding");

//OLDCODE:
/*    

                processRawParameters(method_binding);
            }
        }
    }
*/

    public void processRawParameters(MethodBinding method_binding) {
/*
        Class parameter_types[] = null;

// TODO: Remove this!        
//System.out.println("Preprocessing class " + method_binding.declaringClass.debugName());
//javaParserSupport.preprocessClass(method_binding.declaringClass);
//System.out.println("Done preprocessing class " + method_binding.declaringClass.debugName());

        ReferenceBinding type_binding = method_binding.declaringClass;
        Class cls = javaParserSupport.findClass(type_binding);

        if (method_binding.isConstructor()) {
            Constructor constructor = javaParserSupport.getRawConstructor(cls, method_binding);
            assert(constructor != null);
            parameter_types = constructor.getParameterTypes();
        }
        else {
            Method method = javaParserSupport.getRawMethod(cls, method_binding);
            assert(method != null);
            parameter_types = method.getParameterTypes();
        }

        assert(parameter_types != null);
// TODO: Remove this!        
//System.out.println("Method " + new String(method_binding.selector) + " has " + parameter_types.length + " parameters");
        for (int i = 0; i < parameter_types.length; i++) {
            assert(parameter_types[i] != null);
// TODO: Remove this!
//System.out.println("Processing parameter type " + parameter_types[i].getName());            
            javaParserSupport.generateAndPushType(parameter_types[i]);
        }
*/
        TypeBinding parameter_types[] = method_binding.parameters;
        if (parameter_types != null) {
            for (int i = 0; i < parameter_types.length; i++) {
                javaParserSupport.generateAndPushType(parameter_types[i], this.unitInfo, this.unitInfo.getDefaultLocation(), true /* is_formal_parameter_type_mapping */);
            }
        }
    }


    public boolean enter(MethodDeclaration node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (MethodDeclaration,ClassScope)");

        //
        //
        //
        if (node.statements != null && node.statements.length > 0) {
            MethodHeaderDelimiters.put(node.statements[0], node);
        }

        String name = javaParserSupport.getMethodName(node.binding);

        if (javaParserSupport.verboseLevel > 2)
            System.out.println("Process the return type = " + node.returnType);

        if (javaParserSupport.verboseLevel > 2)
            System.out.println("DONE: Process the return type = " + node.returnType);

// TODO: Remove this !
/*        
        //
        // TODO: Do something !!!
        //
        if (node.typeParameters != null) {
            System.out.println();
            System.out.println("*** No support yet for method type parameters");
            throw new RuntimeException(); // System.exit(1);
        }
*/
            
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

        if (javaParserSupport.verboseLevel > 2) {
            System.out.println("In visit (MethodDeclaration,ClassScope): isPrivate      = " + (isPrivate      ? "true" : "false"));
            System.out.println("In visit (MethodDeclaration,ClassScope): isSynchronized = " + (isSynchronized ? "true" : "false"));
            System.out.println("In visit (MethodDeclaration,ClassScope): isPublic       = " + (isPublic       ? "true" : "false"));
            System.out.println("In visit (MethodDeclaration,ClassScope): isProtected    = " + (isProtected    ? "true" : "false"));
            System.out.println("In visit (MethodDeclaration,ClassScope): isStrictfp     = " + (isStrictfp     ? "true" : "false"));
        }

        // We can build this here but we can't put the symbol into the symbol tabel until 
        // we have gathered the function parameter types so that the correct function type 
        // can be computed.
        int method_index = javaParserSupport.getMethodIndex(node);
        JavaParser.cactionMethodDeclaration(name, method_index, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (MethodDeclaration,ClassScope)");

        //
        // We perform our own traversal for MethodDeclaration so that we can skip certain subcomponents
        // (commented out below.
        //
        //
        /*
        if (node.javadoc != null) {
            node.javadoc.traverse(this, node.scope);
        }
        if (node.annotations != null) {
            int annotationsLength = node.annotations.length;
            for (int i = 0; i < annotationsLength; i++)
                node.annotations[i].traverse(this, node.scope);
        }
        */
        if (node.typeParameters != null) {
            for (int i = 0; i < node.typeParameters.length; i++) {
                node.typeParameters[i].traverse(this, node.scope);
            }
        }
/*
        if (node.returnType != null) {
            node.returnType.traverse(this, node.scope);
        }

        if (node.arguments != null) {
            int argumentLength = node.arguments.length;
            for (int i = 0; i < argumentLength; i++) {
                node.arguments[i].traverse(this, node.scope);
System.out.println("Looking at argument " + i + " of method " + name + ": " + node.arguments[i].type.resolvedType.debugName());
            }
        }
*/
        if (node.thrownExceptions != null) {
            int thrownExceptionsLength = node.thrownExceptions.length;
            for (int i = 0; i < thrownExceptionsLength; i++)
                node.thrownExceptions[i].traverse(this, node.scope);
        }
        if (node.statements != null) {
            int statementsLength = node.statements.length;
            for (int i = 0; i < statementsLength; i++)
                node.statements[i].traverse(this, node.scope);
        }

        return false; // do nothing by node, keep traversing
    }

    public void exit(MethodDeclaration node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (MethodDeclaration,ClassScope)");

        if (node.statements == null || node.statements.length == 0) {
            if (javaParserSupport.verboseLevel > 1)
                System.out.println("    Side-visiting Method Declaration Header for " + node.getClass().getName());
            javaParserSupport.processMethodDeclarationHeader(node, this.unitInfo.createJavaToken(node));
        }

        JavaParser.cactionMethodDeclarationEnd(node.statements == null ? 0 : node.statements.length,
                                               this.unitInfo.createJavaToken(node));
    }

    
    public boolean enter( StringLiteralConcatenation node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (StringLiteralConcatenation,BlockScope)");

        JavaParser.cactionStringLiteralConcatenation(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (StringLiteralConcatenation,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(StringLiteralConcatenation node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (StringLiteralConcatenation,BlockScope)");
    }


    public boolean enter(NormalAnnotation node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (NormalAnnotation,BlockScope)");

        JavaParser.cactionNormalAnnotation(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (NormalAnnotation,BlockScope)");

        return true;
    }

    public void exit(NormalAnnotation node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (NormalAnnotation,BlockScope)");
    }


    public boolean enter(NullLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (NullLiteral,BlockScope)");

        JavaParser.cactionNullLiteral(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (NullLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(NullLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (NullLiteral,BlockScope)");

    }


    public boolean enter(OR_OR_Expression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (OR_OR_Expression,BlockScope)");

        JavaParser.cactionORORExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (OR_OR_Expression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(OR_OR_Expression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (OR_OR_Expression,BlockScope)");

        JavaParser.cactionORORExpressionEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (OR_OR_Expression,BlockScope)");
    }


    public boolean enter(ParameterizedQualifiedTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ParameterizedQualifiedTypeReference,BlockScope)");

        javaParserSupport.processQualifiedParameterizedTypeReference(node, this, scope, this.unitInfo);

/*        
//        JavaParser.cactionParameterizedTypeReference(this.unitInfo.createJavaToken(node));
        if (node.resolvedType.isClass() || node.resolvedType.isInterface()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("(01) The parameterized qualified type referenced is bound to type " + node.resolvedType.debugName());
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
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
System.out.println("Processing parameterized type " + ((ReferenceBinding) type_binding).debugName());
            int num_type_arguments = (node.typeArguments[i] == null ? 0 : node.typeArguments[i].length);
            for (int j = 0; j < num_type_arguments; j++) {
                node.typeArguments[i][j].traverse(this, scope);
            }
            String package_name = javaParserSupport.getPackageName(type_binding);
            String  type_name = javaParserSupport.getTypeName(type_binding);

            if (i == k) {
                if (num_type_arguments == 0) {
                    JavaParser.cactionTypeReference(package_name, type_name, this.unitInfo.createJavaToken(node));
                }
                else {
                    JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                                    type_name,
                                                                    num_type_arguments,
                                                                    (i + 1 < node.typeArguments.length ? 0 : node.dimensions()),
                                                                    this.unitInfo.createJavaToken(node));
                }
            }
            else {
                JavaParser.cactionParameterizedQualifiedTypeReferenceEnd(type_name,
                                                                         num_type_arguments,
                                                                         (i + 1 < node.typeArguments.length ? 0 : node.dimensions()),
                                                                         this.unitInfo.createJavaToken(node));
            }
        }
*/
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ParameterizedQualifiedTypeReference,BlockScope)");

        return false; // short-circuit the traversal
//        return true; // do nothing by node, keep traversing
    }

    public void exit(ParameterizedQualifiedTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Entering exit (ParameterizedQualifiedTypeReference, BlockScope)");
// TODO: remove this
/*
        StringBuffer strbuf = new StringBuffer();
        for (int i = 0; i < node.tokens.length; i++) {
            strbuf.append(node.tokens[i]);
            if (i + 1 < node.tokens.length)
                strbuf.append(".");
        }
        String qualifiedTypeName = new String(strbuf);
*/
/*
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("At top of exit (ParameterizedQualifiedTypeReference,BlockScope) name = " + node.resolvedType.debugName());

        if (node.resolvedType.isClass() || node.resolvedType.isInterface()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("(01) The parameterized qualified type referenced is bound to type " + node.resolvedType.debugName());
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
        }

        String package_name = javaParserSupport.getPackageName(node.resolvedType);
        String  type_name = javaParserSupport.getTypeName(node.resolvedType);

        //
        // TODO: Do this right !!!  This is a temporary patch !!!
        //
        // PC:  I have no idea why the number of type arguments is not precise here!!!
        // What is the purpose of these null elements in this array !!!???
        //
        // NOTE: Do not change this SLOPPY code!!!   It was copied AS IS from the "traverse(...)" method
        // in ParameterizedQualifiedTypeReference.
        //
        int num_type_arguments = 0;
        for (int i = 0, max = node.typeArguments.length; i < max; i++) {
            if (node.typeArguments[i] != null) {
                for (int j = 0, max2 = node.typeArguments[i].length; j < max2; j++) {
                    num_type_arguments++;
                }
            }
        }

        JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                        type_name,
                                                        num_type_arguments,
                                                        node.dimensions(),
                                                        this.unitInfo.createJavaToken(node));
*/
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ParameterizedQualifiedTypeReference, BlockScope)");
    }


    public boolean enter(ParameterizedQualifiedTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ParameterizedQualifiedTypeReference,ClassScope)");

        javaParserSupport.processQualifiedParameterizedTypeReference(node, this, scope, this.unitInfo);
/*        
//        JavaParser.cactionParameterizedTypeReference(this.unitInfo.createJavaToken(node));
        if (node.resolvedType.isClass() || node.resolvedType.isInterface()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("(01) The parameterized qualified type referenced is bound to type " + node.resolvedType.debugName());
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
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
System.out.println("Processing parameterized type " + ((ReferenceBinding) type_binding).debugName());
            int num_type_arguments = (node.typeArguments[i] == null ? 0 : node.typeArguments[i].length);
            for (int j = 0; j < num_type_arguments; j++) {
                node.typeArguments[i][j].traverse(this, scope);
            }
            String package_name = javaParserSupport.getPackageName(type_binding);
            String  type_name = javaParserSupport.getTypeName(type_binding);

            if (i == k) {
                if (num_type_arguments == 0) {
                    JavaParser.cactionTypeReference(package_name, type_name, this.unitInfo.createJavaToken(node));
                }
                else {
                    JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                                    type_name,
                                                                    num_type_arguments,
                                                                    (i + 1 < node.typeArguments.length ? 0 : node.dimensions()),
                                                                    this.unitInfo.createJavaToken(node));
                }
            }
            else {
                JavaParser.cactionParameterizedQualifiedTypeReferenceEnd(type_name,
                                                                         num_type_arguments,
                                                                         (i + 1 < node.typeArguments.length ? 0 : node.dimensions()),
                                                                         this.unitInfo.createJavaToken(node));
            }
        }
*/
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ParameterizedQualifiedTypeReference,ClassScope)");

        return false; // short-circuit the traversal
//      return true; // do nothing by node, keep traversing
    }

    public void exit(ParameterizedQualifiedTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Entering exit (ParameterizedQualifiedTypeReference,ClassScope)");

// TODO: Remove this
/*        
        if (node.resolvedType.isClass() || node.resolvedType.isInterface()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("(01) The parameterized qualified type referenced is bound to type " + node.resolvedType.debugName());
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
        }
        
        //
        // TODO: Do this right !!!  This is a temporary patch !!!
        //
        // PC:  I have no idea why the number of type arguments is not precise here!!!
        // What is the purpose of these null elements in this array !!!???
        //
        // NOTE: Do not change this SLOPPY code!!!   It was copied AS IS from the "traverse(...)" method
        // in ParameterizedQualifiedTypeReference.
        //
        int num_type_arguments = 0;
        for (int i = 0, max = node.typeArguments.length; i < max; i++) {
            if (node.typeArguments[i] != null) {
                for (int j = 0, max2 = node.typeArguments[i].length; j < max2; j++) {
                    num_type_arguments++;
                }
            }
        }
*/
        
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ParameterizedQualifiedTypeReference,BlockScope)");
    }


    public boolean enter(ParameterizedSingleTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ParameterizedSingleTypeReference,BlockScope)");

        JavaParser.cactionParameterizedTypeReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ParameterizedSingleTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ParameterizedSingleTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("At top of exit (ParameterizedSingleTypeReference,BlockScope)");
// TODO: Remove this
/*
        String name = new String(node.token);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("At top of exit (ParameterizedSingleTypeReference,BlockScope) name = " + name);

        // We need to find the qualified name for the associated type name (it should be unique).
        // This has to be handled on the Java side...

        String qualifiedTypeName = node.resolvedType.debugName();

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("At top of exit (ParameterizedSingleTypeReference,BlockScope) qualifiedTypeName = " + qualifiedTypeName);

        JavaParser.cactionParameterizedSingleTypeReferenceEnd(qualifiedTypeName,
                                                              node.typeArguments == null ? 0 : node.typeArguments.length,
                                                              this.unitInfo.createJavaToken(node));

        // TODO: if this type is an array, take care of the dimensions !!!
*/
        
        if (node.resolvedType.isClass() || node.resolvedType.isInterface()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("(01) The parameterized single type referenced is bound to type " + node.resolvedType.debugName());
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
        }

        String package_name = javaParserSupport.getPackageName(node.resolvedType);
        String  type_name = javaParserSupport.getTypeName(node.resolvedType);

        JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                        type_name,
                                                        node.typeArguments.length,
                                                        node.dimensions(),
                                                        this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ParameterizedSingleTypeReference,BlockScope)");
    }


    public boolean enter(ParameterizedSingleTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ParameterizedSingleTypeReference,ClassScope)");

        JavaParser.cactionParameterizedTypeReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ParameterizedSingleTypeReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ParameterizedSingleTypeReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ParameterizedSingleTypeReference,ClassScope)");

        if (node.resolvedType.isClass() || node.resolvedType.isInterface()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("(01) The parameterized single type referenced is bound to type " + node.resolvedType.debugName());
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
        }

        String package_name = javaParserSupport.getPackageName(node.resolvedType);
        String  type_name = javaParserSupport.getTypeName(node.resolvedType);
        
        JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                        type_name,
                                                        node.typeArguments.length,
                                                        node.dimensions(),
                                                        this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ParameterizedSingleTypeReference,ClassScope)");
    }


    public boolean enter(PostfixExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (PostfixExpression,BlockScope)");

        JavaParser.cactionPostfixExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (PostfixExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(PostfixExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (PostfixExpression,BlockScope)");

        int operator_kind = node.operator;

        JavaParser.cactionPostfixExpressionEnd(operator_kind, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (PostfixExpression,BlockScope)");
    }


    public boolean enter(PrefixExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (PrefixExpression,BlockScope)");

        JavaParser.cactionPrefixExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (PrefixExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(PrefixExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (PrefixExpression,BlockScope)");

        int operator_kind = node.operator;

        JavaParser.cactionPrefixExpressionEnd(operator_kind, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (PrefixExpression,BlockScope)");
    }


    public boolean enter(QualifiedAllocationExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (QualifiedAllocationExpression,BlockScope)");

        if (node.type != null) {
            javaParserSupport.preprocessClass(node.type.resolvedType, this.unitInfo);
        }
            
        JavaParser.cactionQualifiedAllocationExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (QualifiedAllocationExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(QualifiedAllocationExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedAllocationExpression,BlockScope)");

        if (javaParserSupport.verboseLevel > 0 && node.type != null)
            System.out.println("The Allocation type is bound to type " + node.type.resolvedType.debugName());

        JavaParser.cactionQualifiedAllocationExpressionEnd(node.type != null, 
                                                           node.enclosingInstance != null,
                                                           node.arguments == null ? 0 : node.arguments.length,
                                                           node.anonymousType != null,
                                                           this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedAllocationExpression,BlockScope)");
    }


    public boolean enter(QualifiedNameReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (QualifiedNameReference, BlockScope)");

        javaParserSupport.processQualifiedNameReference(node, this.unitInfo);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (QualifiedNameReference, BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(QualifiedNameReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedNameReference,BlockScope)");
    }


    public boolean enter(QualifiedNameReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (QualifiedNameReference, ClassScope)");

        javaParserSupport.processQualifiedNameReference(node, this.unitInfo);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (QualifiedNameReference, ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(QualifiedNameReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedNameReference,ClassScope)");
    }


    public boolean enter(QualifiedSuperReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (QualifiedSuperReference,BlockScope)");

        JavaParser.cactionQualifiedSuperReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (QualifiedSuperReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(QualifiedSuperReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit(QualifiedSuperReference,BlockScope)");

        JavaParser.cactionQualifiedSuperReferenceEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedSuperReference,BlockScope)");
    }


    public boolean enter(QualifiedSuperReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (QualifiedSuperReference,ClassScope)");

        JavaParser.cactionQualifiedSuperReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (QualifiedSuperReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(QualifiedSuperReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit(QualifiedSuperReference,ClassScope)");

        JavaParser.cactionQualifiedSuperReferenceEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedSuperReference,ClassScope)");
    }


    public boolean enter(QualifiedThisReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (QualifiedThisReference,BlockScope)");

        JavaParser.cactionQualifiedThisReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (QualifiedThisReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(QualifiedThisReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit(QualifiedThisReference,BlockScope)");

        JavaParser.cactionQualifiedThisReferenceEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedThisReference,BlockScope)");
    }


    public boolean enter(QualifiedThisReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (QualifiedThisReference,ClassScope)");

        JavaParser.cactionQualifiedThisReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (QualifiedThisReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(QualifiedThisReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit(QualifiedThisReference,ClassScope)");

        JavaParser.cactionQualifiedThisReferenceEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedThisReference,ClassScope)");
    }


    public boolean enter(QualifiedTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (QualifiedTypeReference,BlockScope)");

        if (node.resolvedType.isClass() || node.resolvedType.isInterface() || node.resolvedType.isEnum()) {
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("The qualified type referenced is bound to type " + node.resolvedType.debugName());        
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
        }
        else if (! (node.resolvedType instanceof BaseTypeBinding)) {
            throw new RuntimeException("*** No support yet for type: " + node.resolvedType.getClass().getCanonicalName()); // System.exit(1);
        }

        String package_name = javaParserSupport.getPackageName(node.resolvedType),
               type_name = javaParserSupport.getTypeName(node.resolvedType);
// TODO: Remove this !
/*
System.out.println("***Processing type " + node.resolvedType.debugName() +
                   "; package = " + package_name +
                   "; type = " + type_name);
*/
//System.out.println("TypeReference 2");
        JavaParser.cactionTypeReference(package_name, type_name, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (QualifiedTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(QualifiedTypeReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedTypeReference,BlockScope)");
    }

 
    public boolean enter(QualifiedTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (QualifiedTypeReference,ClassScope)");

        if (node.resolvedType.isClass() || node.resolvedType.isInterface() || node.resolvedType.isEnum()) {
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("The qualified type referenced is bound to type " + node.resolvedType.debugName());        
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
        }
        else if (! (node.resolvedType instanceof BaseTypeBinding)) {
            throw new RuntimeException("*** No support yet for type: " + node.resolvedType.getClass().getCanonicalName()); // System.exit(1);
        }

        String package_name = javaParserSupport.getPackageName(node.resolvedType),
               type_name = javaParserSupport.getTypeName(node.resolvedType);
// TODO: Remove this !
/*
System.out.println("***Processing type " + node.resolvedType.debugName() +
                   "; package = " + package_name +
                   "; type = " + type_name);
*/
//System.out.println("TypeReference 3");
        JavaParser.cactionTypeReference(package_name, type_name, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (QualifiedTypeReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(QualifiedTypeReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (QualifiedTypeReference,ClassScope)");
    }


    public boolean enter(ReturnStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ReturnStatement,BlockScope)");

        JavaParser.cactionReturnStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ReturnStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ReturnStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ReturnStatement,BlockScope)");

        JavaParser.cactionReturnStatementEnd(node.expression != null, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ReturnStatement,BlockScope)");
    }


    public boolean enter(SingleMemberAnnotation node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (SingleMemberAnnotation,BlockScope)");

        JavaParser.cactionSingleMemberAnnotation(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (SingleMemberAnnotation,BlockScope)");

        return true;
    }

    public void exit(SingleMemberAnnotation node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SingleMemberAnnotation,BlockScope)");
    }


    public boolean enter(SingleNameReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (SingleNameReference,BlockScope)");

        enterSingleNameReference(node);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (SingleNameReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(SingleNameReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SingleNameReference,BlockScope)");
    }


    public boolean enter(SingleNameReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (SingleNameReference,ClassScope)");

        enterSingleNameReference(node);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (SingleNameReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(SingleNameReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SingleNameReference,ClassScope)");
    }


    public boolean enter(SingleTypeReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (SingleTypeReference,BlockScope)");

        if (node.resolvedType.isClass() || node.resolvedType.isInterface() || node.resolvedType.isEnum()) { 
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("(1) The single type referenced is bound to type " + node.resolvedType.debugName());
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
        }
        else if (! (node.resolvedType instanceof BaseTypeBinding)) {
            throw new RuntimeException("*** No support yet for type: " + node.resolvedType.getClass().getCanonicalName()); // System.exit(1);
        }

        if (node.resolvedType instanceof TypeVariableBinding) { // is this name a type variable?
            TypeVariableBinding type_variable_binding = (TypeVariableBinding) node.resolvedType;
            if (javaParserSupport.verboseLevel > 0) {
                System.out.println("The Single name referenced " + node.toString() + " is bound to type " + type_variable_binding.debugName());
            }

            String type_parameter_name = javaParserSupport.getTypeName(type_variable_binding);
            Binding binding = type_variable_binding.declaringElement;
            if (binding instanceof TypeBinding) {
                TypeBinding enclosing_binding = (TypeBinding) binding;
                String package_name = javaParserSupport.getPackageName(enclosing_binding),
                       type_name = javaParserSupport.getTypeName(enclosing_binding);
//System.out.println("(3) TypeParameterReference: package name = " + package_name + ";  type name = " + type_name);                
                JavaParser.cactionTypeParameterReference(package_name, type_name, -1 /* no method index */, type_parameter_name, this.unitInfo.createJavaToken(node));
            }
            else if (binding instanceof MethodBinding) {
                MethodBinding method_binding = (MethodBinding) binding;
                AbstractMethodDeclaration method_declaration = method_binding.sourceMethod();
                if (method_declaration != null) {
                    ReferenceBinding type_binding = method_binding.declaringClass;
                    String package_name = javaParserSupport.getPackageName(type_binding),
                           type_name = javaParserSupport.getTypeName(type_binding);
                    int method_index = javaParserSupport.getMethodIndex(method_declaration);
//System.out.println("(4) TypeParameterReference: package name = " + package_name + ";  type name = " + type_name);                    
                    JavaParser.cactionTypeParameterReference(package_name, type_name, method_index, type_parameter_name, this.unitInfo.createJavaToken(node));
                }
                else {
                    throw new RuntimeException("*** No support yet for Type Variable enclosed in " + binding.getClass().getCanonicalName() + " without source being available"); // System.exit(1);
                }
            }
            else {
                throw new RuntimeException("*** No support yet for Type Variable enclosed in " + binding.getClass().getCanonicalName()); // System.exit(1);
            }
        }
        else {
            String package_name = javaParserSupport.getPackageName(node.resolvedType),
                   type_name = javaParserSupport.getTypeName(node.resolvedType);
//System.out.println("TypeReference 4");
            JavaParser.cactionTypeReference(package_name, type_name, this.unitInfo.createJavaToken(node));
        }

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (SingleTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(SingleTypeReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SingleTypeReference,BlockScope)");
    }


    public boolean enter(SingleTypeReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (SingleTypeReference,BlockScope)");

        if (node.resolvedType.isClass() || node.resolvedType.isInterface() || node.resolvedType.isEnum()) {
            if (javaParserSupport.verboseLevel > 0)
                System.out.println("(2) The single type referenced is bound to type " + node.resolvedType.debugName());        
            javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
        }
        else if (! (node.resolvedType instanceof BaseTypeBinding)) {
            throw new RuntimeException("*** No support yet for type: " + node.resolvedType.getClass().getCanonicalName()); // System.exit(1);
        }

        if (node.resolvedType instanceof TypeVariableBinding) { // is this name a type variable?
            TypeVariableBinding type_variable_binding = (TypeVariableBinding) node.resolvedType;
            if (javaParserSupport.verboseLevel > 0) {
                System.out.println("The Single name referenced " + node.toString() + " is bound to type " + type_variable_binding.debugName());
            }

            String type_parameter_name = javaParserSupport.getTypeName(type_variable_binding);
            Binding binding = type_variable_binding.declaringElement;
            if (binding instanceof TypeBinding) {
                TypeBinding enclosing_binding = (TypeBinding) binding;
                String package_name = javaParserSupport.getPackageName(enclosing_binding),
                       type_name = javaParserSupport.getTypeName(enclosing_binding);
//System.out.println("(5) TypeParameterReference: package name = " + package_name + ";  type name = " + type_name);                
                JavaParser.cactionTypeParameterReference(package_name, type_name, -1 /* no method index */, type_parameter_name, this.unitInfo.createJavaToken(node));
            }
            else if (binding instanceof MethodBinding) {
                MethodBinding method_binding = (MethodBinding) binding;
                AbstractMethodDeclaration method_declaration = method_binding.sourceMethod();
                if (method_declaration != null) {
                    ReferenceBinding enclosing_type_binding = method_binding.declaringClass;
                    String package_name = javaParserSupport.getPackageName(enclosing_type_binding),
                           type_name = javaParserSupport.getTypeName(enclosing_type_binding);
                    int method_index = javaParserSupport.getMethodIndex(method_declaration);
//System.out.println("(6) TypeParameterReference: package name = " + package_name + ";  type name = " + type_name);                    
                    JavaParser.cactionTypeParameterReference(package_name, type_name, method_index, type_parameter_name, this.unitInfo.createJavaToken(node));
                }
                else {
                    throw new RuntimeException("*** No support yet for Type Variable enclosed in " + binding.getClass().getCanonicalName() + " without source being available"); // System.exit(1);
                }
            }
            else {
                throw new RuntimeException("*** No support yet for Type Variable enclosed in " + binding.getClass().getCanonicalName()); // System.exit(1);
            }
        }
        else { 
            String package_name = javaParserSupport.getPackageName(node.resolvedType),
                   type_name = javaParserSupport.getTypeName(node.resolvedType);
//System.out.println("TypeReference 5");
            JavaParser.cactionTypeReference(package_name, type_name, this.unitInfo.createJavaToken(node));
        }
          
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (SingleTypeReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(SingleTypeReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SingleTypeReference,ClassScope)");
    }


    public boolean enter(StringLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (StringLiteral,BlockScope)");

        // System.out.println("Inside of enter (StringLiteral,BlockScope): node = " + node);
        String literal = new String(node.source());

        JavaParser.cactionStringLiteral(literal, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (StringLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(StringLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (StringLiteral,BlockScope)");
    }


    public boolean enter(SuperReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (SuperReference,BlockScope)");

        JavaParser.cactionSuperReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (SuperReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(SuperReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SuperReference,BlockScope)");
    }


    public boolean enter(SwitchStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (SwitchStatement,BlockScope)");

        JavaParser.cactionSwitchStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (SwitchStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(SwitchStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SwitchStatement,BlockScope)");

        JavaParser.cactionSwitchStatementEnd(node.caseCount, node.defaultCase != null, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SwitchStatement,BlockScope)");
    }


    public boolean enter(SynchronizedStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (SynchronizedStatement,BlockScope)");

        JavaParser.cactionSynchronizedStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (SynchronizedStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(SynchronizedStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SynchronizedStatement,BlockScope)");

        JavaParser.cactionSynchronizedStatementEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (SynchronizedStatement,BlockScope)");
    }


    public boolean enter(ThisReference node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ThisReference,BlockScope)");

        JavaParser.cactionThisReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ThisReference,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ThisReference node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ThisReference,BlockScope)");
    }


    public boolean enter(ThisReference node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ThisReference,ClassScope)");

        JavaParser.cactionThisReference(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ThisReference,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ThisReference node, ClassScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ThisReference,ClassScope)");
    }


    public boolean enter(ThrowStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (ThrowStatement,BlockScope)");

        JavaParser.cactionThrowStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (ThrowStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(ThrowStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ThrowStatement,BlockScope)");

        JavaParser.cactionThrowStatementEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (ThrowStatement,BlockScope)");
    }


    public boolean enter(TrueLiteral node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (TrueLiteral,BlockScope)");

        JavaParser.cactionTrueLiteral(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (TrueLiteral,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(TrueLiteral node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (TrueLiteral,BlockScope)");
    }


    public boolean enter(TryStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
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
          
        JavaParser.cactionTryStatement(node.catchArguments == null ? 0 : node.catchBlocks.length, node.finallyBlock != null, this.unitInfo.createJavaToken(node));
          
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (TryStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(TryStatement node, BlockScope scope) {
        // do nothing  by default
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("exit TryStatement -- BlockScope");

        JavaParser.cactionTryStatementEnd(node.resources.length, node.catchArguments == null ? 0 : node.catchBlocks.length, node.finallyBlock != null, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (TryStatement,BlockScope)");
    }

    //
    // Local and Anonymous types are processed here.
    //
    public boolean enter(TypeDeclaration node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("visit TypeDeclaration -- BlockScope");

        assert(node.binding instanceof LocalTypeBinding); 

        //
        // First, preprocess the symbol table information of the type in question.
        //
        JavaParserSupport.LocalOrAnonymousType special_type = javaParserSupport.localOrAnonymousType.get(node);

        if (node.binding.isAnonymousType()) {
            JavaToken location = this.unitInfo.createJavaToken(node);
//            String enclosing_classname = new String(node.binding.enclosingType().sourceName);

            JavaParser.cactionPushPackage(special_type.package_name, location);
//            JavaParser.cactionInsertClassStart(enclosing_classname, location);
            javaParserSupport.insertClasses(node);
//System.out.println("About to process anonymous type " + special_type.qualifiedName());            
            javaParserSupport.traverseTypeDeclaration(node, this.unitInfo);
//            JavaParser.cactionInsertClassEnd(enclosing_classname, location);
            JavaParser.cactionPopPackage();
        }
        else {
            javaParserSupport.insertClasses(node);
//System.out.println("About to process local type " + special_type.qualifiedName());            
            javaParserSupport.traverseTypeDeclaration(node, this.unitInfo);
        }

        enterTypeDeclaration(node);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter(TypeDeclaration,BlockScope)");

        return false; // The traversal is done in enterTypeDeclaration(node) in the user-specified order of the class members
    }

    public void exit(TypeDeclaration node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("exit TypeDeclaration -- BlockScope");

        exitTypeDeclaration(node);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (TypeDeclaration,BlockScope)");
    }


    public boolean enter(TypeDeclaration node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("visit TypeDeclaration -- ClassScope");

        enterTypeDeclaration(node);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter(TypeDeclaration,ClassScope)");

        return false; // The traversal is done in enterTypeDeclaration(node) in the user-specified order of the class members
    }

    public void exit(TypeDeclaration node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("exit TypeDeclaration -- ClassScope");

        exitTypeDeclaration(node);

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (TypeDeclaration,ClassScope)");
    }


    public boolean enter(TypeDeclaration node, CompilationUnitScope scope) {
        // System.out.println("visit TypeDeclaration -- CompilationUnitScope");
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter(TypeDeclaration, CompilationUnitScope)");

        if (node.name != TypeConstants.PACKAGE_INFO_NAME) { // ignore package-info declarations
            enterTypeDeclaration(node);
        }

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter(TypeDeclaration, CompilationUnitScope)");

        return false; // The traversal is done in enterTypeDeclaration(node) in the user-specified order of the class members
    }

    public void exit(TypeDeclaration node, CompilationUnitScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (TypeDeclaration,CompilationUnitScope)");

        if (node.name != TypeConstants.PACKAGE_INFO_NAME) { // ignore package-info declarations
            exitTypeDeclaration(node);
        }

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (TypeDeclaration,CompilationUnitScope)");
    }


    public boolean enter(TypeParameter node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (TypeParameter,BlockScope)");

        JavaParser.cactionTypeParameter(node.binding.debugName(), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (TypeParameter,BlockScope)");

        return true;
    }

    public void exit(TypeParameter node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (TypeParameter,BlockScope)");

        JavaParser.cactionTypeParameterEnd(node.binding.debugName(), (node.binding.firstBound != null), node.binding.boundsCount(), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (TypeParameter,BlockScope)");
    }


    public boolean enter(TypeParameter node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (TypeParameter,ClassScope)");

        JavaParser.cactionTypeParameter(node.binding.debugName(), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (TypeParameter,ClassScope)");

        return true;
    }

    public void exit(TypeParameter node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside exit (TypeParameter,ClassScope)");

        JavaParser.cactionTypeParameterEnd(node.binding.debugName(), (node.binding.firstBound != null), node.binding.boundsCount(), this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (TypeParameter,ClassScope)");
    }

    public boolean enter(UnaryExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (UnaryExpression,BlockScope)");

        JavaParser.cactionUnaryExpression(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (UnaryExpression,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(UnaryExpression node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (UnaryExpression,BlockScope)");

        // Not clear what the valueRequired filed means.
        int operator_kind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT;

        JavaParser.cactionUnaryExpressionEnd(operator_kind, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (UnaryExpression,BlockScope)");
    }


    public boolean enter(WhileStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (WhileStatement,BlockScope)");

        JavaParser.cactionWhileStatement(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (WhileStatement,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(WhileStatement node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (WhileStatement,BlockScope)");

        JavaParser.cactionWhileStatementEnd(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (WhileStatement,BlockScope)");
    }


    public boolean enter(Wildcard node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Wildcard,BlockScope)");

        JavaParser.cactionWildcard(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Wildcard,BlockScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Wildcard node, BlockScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (Wildcard,BlockScope)");

        JavaParser.cactionWildcardEnd(node.kind == Wildcard.UNBOUND, node.kind == Wildcard.EXTENDS, node.kind == Wildcard.SUPER, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Wildcard ,BlockScope)");
    }


    public boolean enter(Wildcard node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of enter (Wildcard,ClassScope)");

        JavaParser.cactionWildcard(this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving enter (Wildcard,ClassScope)");

        return true; // do nothing by node, keep traversing
    }

    public void exit(Wildcard node, ClassScope scope) {
        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Inside of exit (Wildcard,ClassScope)");

        JavaParser.cactionWildcardEnd(node.kind == Wildcard.UNBOUND, node.kind == Wildcard.EXTENDS, node.kind == Wildcard.SUPER, this.unitInfo.createJavaToken(node));

        if (javaParserSupport.verboseLevel > 0)
            System.out.println("Leaving exit (Wildcard,ClassScope)");
    }
}

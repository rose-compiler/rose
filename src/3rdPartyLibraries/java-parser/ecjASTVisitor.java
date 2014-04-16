import java.util.*;
import org.eclipse.jdt.internal.compiler.ast.*;
import org.eclipse.jdt.internal.compiler.lookup.*;
import org.eclipse.jdt.internal.compiler.ast.Annotation;
import org.eclipse.jdt.core.compiler.CharOperation;
import org.eclipse.jdt.internal.compiler.classfmt.ClassFileConstants;

class ecjASTVisitor extends ExtendedASTVisitor {
    JavaParserSupport javaParserSupport = null;
    UnitInfo unitInfo = null;

    //
    // Keep track of the set of Catch blocks and Catch arguments.
    //
    HashSet<Block> catchBlocks = null;
    HashSet<Argument> catchArguments = null;

    //
    // Keep track of Argument(s) that were already processed in a different context.
    //
    HashMap<ASTNode, AbstractMethodDeclaration> MethodHeaderDelimiters = null;

    /**
     * Construct a visitor and associate it with the given support class.
     */
    public ecjASTVisitor() {
    }

    /**
     * Visit this compilation unit. 
     * 
     * @param unit_info
     */
    public void startVisit(JavaParserSupport java_parser_support, UnitInfo unit_info) {
        this.javaParserSupport = java_parser_support;
        this.unitInfo = unit_info;

        CompilationUnitDeclaration unit = unit_info.unit;

        catchBlocks = new HashSet<Block>();
        catchArguments = new HashSet<Argument>();
        MethodHeaderDelimiters = new HashMap<ASTNode, AbstractMethodDeclaration>();
        
        unit.traverse(this, unit.scope);

        //
        // Turn the global maps into garbage.
        //
        this.javaParserSupport = null;
        this.unitInfo = null;
        this.catchBlocks = null;
        this.catchArguments = null;
        this.MethodHeaderDelimiters = null;
    }

    /**
     * 
     * @param node
     * @return
     */
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
        else if (node instanceof NameReference)
            return node.getClass().getName() + ": " +  ((NameReference) node).print(0, new StringBuffer()).toString();
        else if (node instanceof TypeReference)
            return node.getClass().getName() + ": " + ((TypeReference) node).print(0, new StringBuffer()).toString();
        return node.getClass().getName();
    }
    
    public boolean preVisit(ASTNode node) {
        if (JavaTraversal.verboseLevel > 1)
            System.out.println("Pre-visiting " + nodeId(node));

        if (node instanceof Javadoc) { // Ignore all Javadoc nodes!!!
            return false;
        }

        if (MethodHeaderDelimiters.containsKey(node)) {
            AbstractMethodDeclaration abstract_method = MethodHeaderDelimiters.get(node);
            if (abstract_method instanceof ConstructorDeclaration) {
                if (JavaTraversal.verboseLevel > 1)
                    System.out.println("    Side-visiting Constructor Declaration Header for " + abstract_method.getClass().getName());
                ConstructorDeclaration constructor = (ConstructorDeclaration) abstract_method;
                javaParserSupport.processConstructorDeclarationHeader(constructor, this.unitInfo.createJavaToken(abstract_method));
            }
            else {
                if (JavaTraversal.verboseLevel > 1)
                    System.out.println("    Side-visiting Method Declaration Header for " + abstract_method.getClass().getName());
                MethodDeclaration method = (MethodDeclaration) abstract_method;
                JavaParser.cactionMethodDeclarationHeader(javaParserSupport.getMethodName(method.binding),
                                                          method.isAbstract(),
                                                          method.isNative(),
                                                          method.isStatic(),
                                                          method.binding.isFinal(),
                                                          method.binding.isSynchronized(),
                                                          method.binding.isPublic(),
                                                          method.binding.isProtected(),
                                                          method.binding.isPrivate(),
                                                          method.binding.isStrictfp(), 
                                                          method.typeParameters == null ? 0 : method.typeParameters.length,
                                                          method.arguments == null ? 0 : method.arguments.length,
                                                          method.thrownExceptions == null ? 0 : method.thrownExceptions.length,
                                                          this.unitInfo.createJavaToken(method)
                                                         );
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
       if (JavaTraversal.verboseLevel > 1)
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

    private void enterSingleNameReference(SingleNameReference node) throws Exception {
        String varRefName = node.toString();
        if (JavaTraversal.verboseLevel > 0) {
            System.out.println("Building a variable reference for name = " + varRefName);
            System.out.println("node.genericCast = " + node.genericCast);
        }

        if (node.binding instanceof TypeVariableBinding) { // is this name a type variable?
            TypeVariableBinding type_variable_binding = (TypeVariableBinding) node.binding;
            if (JavaTraversal.verboseLevel > 0) {
                System.out.println("The Single name referenced " + varRefName + " is bound to type " + type_variable_binding.debugName());
            }

            String type_parameter_name = javaParserSupport.getTypeName(type_variable_binding);
            Binding binding = type_variable_binding.declaringElement;
            if (binding instanceof TypeBinding) {
                TypeBinding enclosing_binding = (TypeBinding) binding;
                String package_name = javaParserSupport.getPackageName(enclosing_binding),
                       type_name = javaParserSupport.getTypeName(enclosing_binding);
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
            if (JavaTraversal.verboseLevel > 0) {
                System.out.println("The Single name referenced " + varRefName + " is bound to type " + type_binding.debugName());
            }

            javaParserSupport.setupClass(type_binding, this.unitInfo);

            JavaParser.cactionTypeReference(javaParserSupport.getPackageName(type_binding),
                                            javaParserSupport.getTypeName(type_binding),
                                            this.unitInfo.createJavaToken(node));
        }
        else { // the name is a variable
            String package_name = "",
                   type_name = "";
            if (node.localVariableBinding() == null) { // not a local variable
                TypeBinding type_binding = node.actualReceiverType;
                assert(type_binding.isClass() || type_binding.isInterface() || type_binding.isEnum());
                if (JavaTraversal.verboseLevel > 0) {
                    System.out.println("The  Single name referenced " + varRefName + " is bound to type " + type_binding.debugName());
                }
                javaParserSupport.preprocessClass(type_binding, this.unitInfo);
                package_name = javaParserSupport.getPackageName(type_binding);
                type_name = javaParserSupport.getTypeName(type_binding);
            }

            JavaParser.cactionSingleNameReference(package_name, type_name, varRefName, this.unitInfo.createJavaToken(node));
        }
    }

    private void enterTypeDeclaration(TypeDeclaration node) throws Exception {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enterTypeDeclaration(node)");

        JavaToken location = this.unitInfo.createJavaToken(node);

        //
        // Get the list of type members in sorted order.
        //
        ASTNode node_list[] = javaParserSupport.orderedClassMembers.get(node);
        assert(node_list != null);

        JavaParserSupport.LocalOrAnonymousType special_type = javaParserSupport.localOrAnonymousType.get(node);
        String package_name = javaParserSupport.getPackageName(node.binding),
               enclosing_classname = (special_type == null || (! special_type.isAnonymous()) ? "" : new String(node.binding.enclosingType().sourceName)),
               typename = (special_type == null || special_type.isAnonymous()
                                                 ? javaParserSupport.getTypeName(node.binding)
                                                 : special_type.simplename
                          );

        if (node.javadoc != null) { // Javadoc(s) are ignored for now. See preVisit(...)
            node.javadoc.traverse(this, node.scope);
        }

        if (node.annotations != null) { // Annotations are ignored for now. See preVisit(...)
            int annotationsLength = node.annotations.length;
            for (int i = 0; i < annotationsLength; i++) {
                node.annotations[i].traverse(this, node.staticInitializerScope);
            }
        }

        JavaParser.cactionTypeDeclaration(package_name,
                                          typename,
                                          (node.annotations == null ? 0 : node.annotations.length),
                                          (! node.binding.isInterface()) && node.binding.superclass != null, // see traverseTypeDeclaration and traverseReferenceBinding for detail!
                                          (node.kind(node.modifiers) == TypeDeclaration.ANNOTATION_TYPE_DECL),
                                          (node.kind(node.modifiers) == TypeDeclaration.INTERFACE_DECL),
                                          (node.kind(node.modifiers) == TypeDeclaration.ENUM_DECL),
                                          (node.binding != null && node.binding.isAbstract()),
                                          (node.binding != null && node.binding.isFinal()),
                                          (node.binding != null && node.binding.isPrivate()),
                                          (node.binding != null && node.binding.isPublic()),
                                          (node.binding != null && node.binding.isProtected()),
                                          (node.binding != null && node.binding.isStatic() && node.binding.isNestedType()),
                                          (node.binding != null && node.binding.isStrictfp()),
                                          this.unitInfo.createJavaToken(node));

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
            JavaParser.cactionMethodDeclarationEnd(0, 0, this.unitInfo.getDefaultLocation());

            //
            // valueOf()
            //
            // See the function traverseClass for more information about this method.
            //
            JavaParser.cactionMethodDeclaration("valueOf", valueOf_index, this.unitInfo.getDefaultLocation());
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
            JavaParser.cactionMethodDeclarationEnd(0, 0, this.unitInfo.getDefaultLocation());
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enterTypeDeclaration(node)");
    }

    private void exitTypeDeclaration(TypeDeclaration node) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exitTypeDeclaration(node)");
      
        JavaParser.cactionTypeDeclarationEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exitTypeDeclaration(node)");
    }

    public boolean enter(AllocationExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(AllocationExpression, BlockScope)");

        if (node.type != null) {
            try {
                javaParserSupport.preprocessClass(node.type.resolvedType, this.unitInfo);
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
        }

        JavaParser.cactionAllocationExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(AllocationExpression, BlockScope)");

        return true;
    }

    public void exit(AllocationExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(AllocationExpression, BlockScope)");

        if (JavaTraversal.verboseLevel > 0 && node.type != null) 
            System.out.println("The Allocation type is bound to type " + node.type.resolvedType.debugName());

        JavaParser.cactionAllocationExpressionEnd(node.type != null, node.arguments == null ? 0 : node.arguments.length, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(AllocationExpression, BlockScope)");
    }


    public boolean enter(AND_AND_Expression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(AND_AND_Expression, BlockScope)");

        JavaParser.cactionANDANDExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(AND_AND_Expression, BlockScope)");

        return true;
    }

    public void exit(AND_AND_Expression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(AND_AND_Expression, BlockScope)");

        JavaParser.cactionANDANDExpressionEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(AND_AND_Expression, BlockScope)");
    }


    public boolean enter(AnnotationMethodDeclaration node, ClassScope classScope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(AnnotationMethodDeclaration, ClassScope)");

        String name = javaParserSupport.getMethodName(node.binding);
        try {
            int method_index = javaParserSupport.getMethodIndex(node);
            JavaParser.cactionAnnotationMethodDeclaration(name, method_index, this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(AnnotationMethodDeclaration, ClassScope)");

        if (node.annotations != null) {
            int annotationsLength = node.annotations.length;
            for (int i = 0; i < annotationsLength; i++)
                node.annotations[i].traverse(this, node.scope);
        }
        /*
        if (node.returnType != null) { // return type was already processed.
            node.returnType.traverse(this, node.scope);
        }
        */        
        if (node.defaultValue != null) {
            node.defaultValue.traverse(this, node.scope);
        }

        return false;
    }

    public void exit(AnnotationMethodDeclaration node, ClassScope classScope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(AnnotationMethodDeclaration, ClassScope)");

        String name = javaParserSupport.getMethodName(node.binding);
        
        try {
            int method_index = javaParserSupport.getMethodIndex(node);
            JavaParser.cactionAnnotationMethodDeclarationEnd(name,
                                                             method_index,
                                                             (node.annotations == null ? 0 : node.annotations.length),
                                                             node.defaultValue != null,
                                                             this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(AnnotationMethodDeclaration, ClassScope)");
    }


    public boolean enter(Argument node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Argument, BlockScope)");

        String name = new String(node.name);

        if (JavaTraversal.verboseLevel > 0) {
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
        
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Argument, BlockScope)");

        return true;
    }

    public void exit(Argument node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Argument, BlockScope)");

        String name = new String(node.name);

        boolean is_final = node.binding.isFinal();

        if (catchArguments.contains(node)) {
            int number_of_types = (node.type instanceof UnionTypeReference ? ((UnionTypeReference ) node.type).typeReferences.length : 1);
            JavaParser.cactionCatchArgumentEnd(node.annotations == null ? 0 : node.annotations.length, name, number_of_types, is_final, this.unitInfo.createJavaToken(node));
        }
        else {
            JavaParser.cactionArgumentEnd(node.annotations == null ? 0 : node.annotations.length, name, this.unitInfo.createJavaToken(node));
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Argument, BlockScope)");
    }


    public boolean enter(Argument node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Argument, ClassScope)");

        String name = new String(node.name);

        if (JavaTraversal.verboseLevel > 0) {
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
        
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Argument, ClassScope)");

        return true;
    }

    public void exit(Argument node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Argument, ClassScope)");

        String name = new String(node.name);

        boolean is_final = node.binding.isFinal(); 

        if (catchArguments.contains(node)) {
            int number_of_types = (node.type instanceof UnionTypeReference ? ((UnionTypeReference ) node.type).typeReferences.length : 1);
            JavaParser.cactionCatchArgumentEnd(node.annotations == null ? 0 : node.annotations.length, name, number_of_types, is_final, this.unitInfo.createJavaToken(node));
        }
        else {
            JavaParser.cactionArgumentEnd(node.annotations == null ? 0 : node.annotations.length, name, this.unitInfo.createJavaToken(node));
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Argument, ClassScope)");
    }


    public boolean enter(ArrayAllocationExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ArrayAllocationExpression, BlockScope)");

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

        try {
            javaParserSupport.preprocessClass(node.type.resolvedType, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        JavaParser.cactionArrayAllocationExpression(this.unitInfo.createJavaToken(node));
        
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ArrayAllocationExpression, BlockScope)");

        return false; // We've already traversed the children of this node. Indicate this by returning false!
    }

    public void exit(ArrayAllocationExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ArrayAllocationExpression, BlockScope)");
        
        JavaParser.cactionArrayAllocationExpressionEnd(node.dimensions == null ? 0 : node.dimensions.length,
                                                       node.initializer != null,
                                                       this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ArrayAllocationExpression, BlockScope)");
    }


    public boolean enter(ArrayInitializer node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ArrayInitializer, BlockScope)");

        JavaParser.cactionArrayInitializer(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ArrayInitializer, BlockScope)");

        return true;
    }

    public void exit(ArrayInitializer node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ArrayInitializer, BlockScope)");
          
        JavaParser.cactionArrayInitializerEnd(node.expressions == null ? 0 : node.expressions.length,
                                              this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ArrayInitializer, BlockScope)");
    }


    public boolean enter(ArrayQualifiedTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ArrayQualifiedTypeReference, BlockScope)");

        ArrayBinding array_type = (ArrayBinding) node.resolvedType;

        try {
            javaParserSupport.processQualifiedTypeReference(node.tokens, node.tokens.length, scope, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        JavaParser.cactionArrayTypeReference(node.dimensions(), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ArrayQualifiedTypeReference, BlockScope)");

        return true;
    }

    public void exit(ArrayQualifiedTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ArrayQualifiedTypeReference, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ArrayQualifiedTypeReference, BlockScope)");
    }


    public boolean enter(ArrayQualifiedTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ArrayQualifiedTypeReference, ClassScope)");

        ArrayBinding array_type = (ArrayBinding) node.resolvedType;

        try {
            javaParserSupport.processQualifiedTypeReference(node.tokens, node.tokens.length, scope, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        JavaParser.cactionArrayTypeReference(node.dimensions(), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ArrayQualifiedTypeReference, ClassScope)");

        return true;
    }

    public void exit(ArrayQualifiedTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ArrayQualifiedTypeReference, ClassScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ArrayQualifiedTypeReference, ClassScope)");
    }


    public boolean enter(ArrayReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ArrayReference, BlockScope)");

        JavaParser.cactionArrayReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ArrayReference, BlockScope)");

        return true;
    }

    public void exit(ArrayReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ArrayReference, BlockScope)");

        JavaParser.cactionArrayReferenceEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ArrayReference, BlockScope)");
    }


    public boolean enter(ArrayTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ArrayTypeReference, BlockScope)");

        ArrayBinding array_type = (ArrayBinding) node.resolvedType;
        TypeBinding base_type = array_type.leafComponentType;

        try {
            if (base_type.isClass() || base_type.isInterface() || base_type.isEnum()) { 
                if (JavaTraversal.verboseLevel > 0)
                    System.out.println("Array base type referenced is bound to " + base_type.debugName());
                javaParserSupport.setupClass(base_type, this.unitInfo);
            }
            javaParserSupport.generateAndPushType(base_type, this.unitInfo, this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        JavaParser.cactionArrayTypeReference(node.dimensions(), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ArrayTypeReference, BlockScope)");

        return true;
    }

    public void exit(ArrayTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ArrayTypeReference, BlockScope)");

         // do nothing by default

         if (JavaTraversal.verboseLevel > 0)
               System.out.println("Leaving exit(ArrayTypeReference, BlockScope)");
    }


    public boolean enter(ArrayTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ArrayTypeReference, ClassScope)");

        ArrayBinding array_type = (ArrayBinding) node.resolvedType;
        TypeBinding base_type = array_type.leafComponentType;

        try {
            if (base_type.isClass() || base_type.isInterface() || base_type.isEnum()) { 
                if (JavaTraversal.verboseLevel > 0)
                    System.out.println("Array base type referenced is bound to " + base_type.debugName());
                javaParserSupport.setupClass(base_type, this.unitInfo);
            }
            javaParserSupport.generateAndPushType(base_type, this.unitInfo, this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        JavaParser.cactionArrayTypeReference(node.dimensions(), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ArrayTypeReference, ClassScope)");

        return true;
    }

    public void exit(ArrayTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ArrayTypeReference, ClassScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ArrayTypeReference, ClassScope)");
    }


    public boolean enter(AssertStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(AssertStatement, BlockScope)");

        JavaParser.cactionAssertStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(AssertStatement, BlockScope)");

        return true;
    }

    public void exit(AssertStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(AssertStatement, BlockScope)");

        JavaParser.cactionAssertStatementEnd(node.exceptionArgument != null, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(AssertStatement, BlockScope)");
    }


    public boolean enter(Assignment node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Assignment, BlockScope)");

        JavaParser.cactionAssignment(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Assignment, BlockScope)");

        return true;
    }

    public void exit(Assignment node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(Assignment, BlockScope)");

        JavaParser.cactionAssignmentEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Assignment, BlockScope)");
    }


    public boolean enter(BinaryExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(BinaryExpression, BlockScope)");

        JavaParser.cactionBinaryExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(BinaryExpression, BlockScope)");

        return true;
    }

    public void exit(BinaryExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(BinaryExpression, BlockScope)");

        int operatorKind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT;

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(BinaryExpression, BlockScope): operatorKind = " + operatorKind);

        JavaParser.cactionBinaryExpressionEnd(operatorKind, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(BinaryExpression, BlockScope)");
    }


    public boolean enter(Block node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Block, BlockScope)");

        JavaParser.cactionBlock(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Block, BlockScope)");

        return true;
    }

    public void exit(Block node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(Block, BlockScope)");

        if (JavaTraversal.verboseLevel > 1)
            System.out.println("node.explicitDeclarations = " + node.explicitDeclarations);

        int number_of_statements = 0;
        if (node.statements != null)
            number_of_statements = node.statements.length;

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("numberOfStatements = " + number_of_statements);

        JavaParser.cactionBlockEnd(number_of_statements, this.unitInfo.createJavaToken(node));

        //
        // charles4 (09/26/2011): If this block belongs to a Catch statement,
        // close the catch statement.
        //
        if (catchBlocks.contains(node)) {
            JavaParser.cactionCatchBlockEnd(this.unitInfo.createJavaToken(node));
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Block, BlockScope)");
    }


    public boolean enter(BreakStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(BreakStatement, BlockScope)");

        JavaParser.cactionBreakStatement((node.label == null ? "" : new String(node.label)),
                                         this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(BreakStatement, BlockScope)");

        return true;
    }

    public void exit(BreakStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(BreakStatement, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(BreakStatement, BlockScope)");
    }


    public boolean enter(CaseStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(CaseStatement, BlockScope)");

        JavaParser.cactionCaseStatement(node.constantExpression != null, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(CaseStatement, BlockScope)");

        return true;
    }

    public void exit(CaseStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(CaseStatement, BlockScope)");

        JavaParser.cactionCaseStatementEnd(node.constantExpression != null, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(CaseStatement, BlockScope)");
    }


    public boolean enter(CastExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(CastExpression, BlockScope)");

        JavaParser.cactionCastExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(CastExpression, BlockScope)");

        return true;
    }

    public void exit(CastExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(CastExpression, BlockScope)");

        JavaParser.cactionCastExpressionEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(CastExpression, BlockScope)");
    }


    public boolean enter(CharLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(CharLiteral, BlockScope)");

        JavaParser.cactionCharLiteral(node.constant.charValue(), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(CharLiteral, BlockScope)");

        return true;
    }

    public void exit(CharLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(CharLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(CharLiteral, BlockScope)");
    }


    public boolean enter(ClassLiteralAccess node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ClassLiteralAccess, BlockScope)");

        JavaParser.cactionClassLiteralAccess(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ClassLiteralAccess, BlockScope)");

        return true;
    }

    public void exit(ClassLiteralAccess node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Inside of exit(ClassLiteralAccess, BlockScope)");

        JavaParser.cactionClassLiteralAccessEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ClassLiteralAccess, BlockScope)");
    }


    public boolean enter(Clinit node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Clinit, ClassScope)");

        JavaParser.cactionClinit(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Clinit, ClassScope)");

        return true;
    }

    public void exit(Clinit node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(Clinit, ClassScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Clinit, ClassScope)");
    }

    public boolean enter(CompilationUnitDeclaration node, CompilationUnitScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(CompilationUnitDeclaration, CompilationUnitScope)");

        String filename = new String(node.getFileName());
        if (JavaTraversal.verboseLevel > 0) {
            System.out.println("Compiling file = " + filename);
        }

        String package_name = (node.currentPackage == null ? "" : node.currentPackage.print(0, new StringBuffer(), false /* Not on-demand package */).toString());

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Package name = " + package_name);

        JavaParser.cactionCompilationUnitDeclaration(new String(node.getFileName()), package_name, filename, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(CompilationUnitDeclaration, CompilationUnitScope)");

        return true;
    }

    public void exit(CompilationUnitDeclaration node, CompilationUnitScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(CompilationUnitDeclaration, CompilationUnitScope)");

        if (node.types != null) {
            if (JavaTraversal.verboseLevel > 0)
                System.out.println("node.types.length = " + node.types.length);
        }

        if (node.imports != null) {
            if (JavaTraversal.verboseLevel > 0)
                System.out.println("node.imports.length = " + node.imports.length);
        }

        JavaParser.cactionCompilationUnitDeclarationEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(CompilationUnitDeclaration, CompilationUnitScope)");
    }


    public boolean enter(CompoundAssignment node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(CompoundAssignment, BlockScope)");

        JavaParser.cactionCompoundAssignment(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(CompoundAssignment, BlockScope)");

        return true;
    }

    public void exit(CompoundAssignment node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(CompoundAssignment, BlockScope)");

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(CompoundAssignment, BlockScope): operator_kind" + node.toString());

        int operator_kind = node.operator;

        JavaParser.cactionCompoundAssignmentEnd(operator_kind, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(CompoundAssignment, BlockScope)");
    }


    public boolean enter(ConditionalExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ConditionalExpression, BlockScope)");

        JavaParser.cactionConditionalExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ConditionalExpression, BlockScope)");

        return true;
    }

    public void exit(ConditionalExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ConditionalExpression, BlockScope)");

        JavaParser.cactionConditionalExpressionEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ConditionalExpression, BlockScope)");
    }


    public boolean enter(ConstructorDeclaration node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ConstructorDeclaration, ClassScope)");

        assert(! node.isDefaultConstructor());
        
        String name = javaParserSupport.getMethodName(node.binding);
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ConstructorDeclaration, ClassScope) method name = " + name);

        //
        //
        //
        if (node.constructorCall != null) {
            MethodHeaderDelimiters.put(node.constructorCall, node);
        }
        else if (node.statements != null && node.statements.length > 0) {
            MethodHeaderDelimiters.put(node.statements[0], node);
        }
        
        try {
            int constructor_index = javaParserSupport.getMethodIndex(node);
            JavaParser.cactionConstructorDeclaration(name, constructor_index, this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }           

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ConstructorDeclaration, ClassScope)");

        //
        // We perform our own traversal for MethodDeclaration so that we can skip certain subcomponents
        // (commented out below).
        //
        //
        /*
        if (node.javadoc != null) {
            node.javadoc.traverse(this, node.scope);
        }
        */
        if (node.annotations != null) {
            int annotationsLength = node.annotations.length;
            for (int i = 0; i < annotationsLength; i++)
                node.annotations[i].traverse(this, node.scope);
        }

        /* No need for this!!!  The type parameters are fully processed during the initial traversal.
        if (node.typeParameters != null) {
            int typeParametersLength = node.typeParameters.length;
            for (int i = 0; i < typeParametersLength; i++) {
                node.typeParameters[i].traverse(this, node.scope);
            }
        }
        */

        if (node.arguments != null) {
            int argumentLength = node.arguments.length;
            for (int i = 0; i < argumentLength; i++) {
                Argument argument = node.arguments[i];
                if (argument.annotations != null) { // does this parameter have annotations?
                    for (int k = 0, max = argument.annotations.length; k < max; k++) {
                        argument.annotations[k].traverse(this, node.scope);
                    }
                    JavaParser.cactionArgumentEnd(argument.annotations.length,
                                                  new String(argument.name),
                                                  this.unitInfo.createJavaToken(argument));
                }
            }
        }

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
        return false;
    }

    public void exit(ConstructorDeclaration node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ConstructorDeclaration, ClassScope)");

        if (! node.isDefaultConstructor()) {        
            // DQ (7/31/2011): Added more precise handling of statements to be collected from the statement stack.
            int num_statements = 0;
            if (node.statements != null) {
                num_statements = node.statements.length;
                if (JavaTraversal.verboseLevel > 0)
                    System.out.println("Entering exit(ConstructorDeclaration, ClassScope): numberOfStatements = " + num_statements);
            }

            if (node.constructorCall != null && (! node.constructorCall.isImplicitSuper())) { // is there an Explicit constructor call?
                num_statements++;
                // System.out.println("Entering exit(ConstructorDeclaration, ClassScope): increment the numberOfStatements = " + numberOfStatements);
            }
          
            if (node.constructorCall == null && (node.statements == null || node.statements.length == 0)) {
                if (JavaTraversal.verboseLevel > 1)
                    System.out.println("    Side-visiting Constructor Declaration Header for " + node.getClass().getName());
                javaParserSupport.processConstructorDeclarationHeader(node, this.unitInfo.createJavaToken(node));
            }

            JavaParser.cactionConstructorDeclarationEnd(node.annotations == null ? 0 : node.annotations.length,
                                                        num_statements,
                                                        this.unitInfo.createJavaToken(node));
        }
    }


    public boolean enter(ContinueStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ContinueStatement, BlockScope)");

        JavaParser.cactionContinueStatement((node.label == null ? "" : new String(node.label)), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ContinueStatement, BlockScope)");

        return true;
    }

    public void exit(ContinueStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ContinueStatement, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ContinueStatement, BlockScope)");
    }


    public boolean enter(DoStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(DoStatement, BlockScope)");

        JavaParser.cactionDoStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(DoStatement, BlockScope)");

        return true;
    }

    public void exit(DoStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(DoStatement, BlockScope)");

        JavaParser.cactionDoStatementEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(DoStatement, BlockScope)");
    }


    public boolean enter(DoubleLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(DoubleLiteral, BlockScope)");

        double value = node.constant.doubleValue();
        String source = (value < 0 ? "-" : "") + new String(node.source());
        JavaParser.cactionDoubleLiteral(value, source, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(DoubleLiteral, BlockScope)");

        return true;
    }

    public void exit(DoubleLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(DoubleLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(DoubleLiteral, BlockScope)");
    }


    public boolean enter(EmptyStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(EmptyStatement, BlockScope)");

        JavaParser.cactionEmptyStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(EmptyStatement, BlockScope)");

        return true;
    }

    public void exit(EmptyStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(EmptyStatement, BlockScope)");

        JavaParser.cactionEmptyStatementEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(EmptyStatement, BlockScope)");
    }


    public boolean enter(EqualExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(EqualExpression, BlockScope)");

        JavaParser.cactionEqualExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(EqualExpression, BlockScope)");

        return true;
    }

    public void exit(EqualExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(EqualExpression, BlockScope)");

        int operator_kind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT; // EQUAL_EQUAL or NOT_EQUAL

        JavaParser.cactionEqualExpressionEnd(operator_kind, this.unitInfo.createJavaToken(node));
    }


    public boolean enter(ExplicitConstructorCall node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ExplicitConstructorCall, BlockScope)");

        if (node.isImplicitSuper())
            return false;

        JavaParser.cactionExplicitConstructorCall(this.unitInfo.createJavaToken(node));
          
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ExplicitConstructorCall, BlockScope)");

        return true;
    }

    public void exit(ExplicitConstructorCall node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ExplicitConstructorCall, BlockScope)");

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("In visit (ExplicitConstructorCall, BlockScope): node.accessMode = " + node.accessMode);

        if (node.isImplicitSuper())
            return;

        try {
            int constructor_index = javaParserSupport.getMethodIndex(node.binding);
            JavaParser.cactionExplicitConstructorCallEnd(node.isImplicitSuper(),
                                                         node.isSuperAccess(),
                                                         node.qualification != null,
                                                         javaParserSupport.getPackageName(node.binding.declaringClass),
                                                         javaParserSupport.getTypeName(node.binding.declaringClass),
                                                         constructor_index,
                                                         node.typeArguments == null ? 0 : node.typeArguments.length,
                                                         node.arguments == null ? 0 : node.arguments.length,
                                                         this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ExplicitConstructorCall, BlockScope)");
    }


    public boolean enter(ExtendedStringLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ExtendedStringLiteral, BlockScope)");

        JavaParser.cactionExtendedStringLiteral(node.constant.stringValue(), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ExtendedStringLiteral, BlockScope)");

        return true;
    }

    public void exit(ExtendedStringLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ExtendedStringLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ExtendedStringLiteral, BlockScope)");
    }


    public boolean enter(FalseLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(FalseLiteral, BlockScope)");

        JavaParser.cactionFalseLiteral(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(FalseLiteral, BlockScope)");

        return true;
    }

    public void exit(FalseLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(FalseLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(FalseLiteral, BlockScope)");
    }


    public boolean enter(FieldDeclaration node, MethodScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(FieldDeclaration, MethodScope)");

        // Do nothing 

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(FieldDeclaration, MethodScope)");

        return true;
    }

    public void exit(FieldDeclaration node, MethodScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(FieldDeclaration, MethodScope)");

        JavaParser.cactionFieldDeclarationEnd(new String(node.name),
                                              node.annotations == null ? 0 : node.annotations.length,
                                              node.getKind() == AbstractVariableDeclaration.ENUM_CONSTANT,
                                              node.initialization != null,
                                              node.binding != null && node.binding.isFinal(),
                                              node.binding != null && node.binding.isPrivate(),
                                              node.binding != null && node.binding.isProtected(),
                                              node.binding != null && node.binding.isPublic(),
                                              node.binding != null && node.binding.isVolatile(),
                                              node.binding != null && node.binding.isSynthetic(),
                                              node.binding != null && node.binding.isStatic(),
                                              node.binding != null && node.binding.isTransient(),
                                              this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(FieldDeclaration, MethodScope)");
    }


    public boolean enter(FieldReference node, BlockScope scope) {
        // System.out.println("Sorry, not implemented in support for FieldReference(BlockScope): xxx");
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(FieldReference, BlockScope)");

        try {
            javaParserSupport.preprocessClass(node.actualReceiverType, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        JavaParser.cactionFieldReference(new String(node.token), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(FieldReference, BlockScope)");

        return true;
    }

    public void exit(FieldReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(FieldReference, BlockScope)");

        try {
            javaParserSupport.generateAndPushType(node.actualReceiverType, this.unitInfo, this.unitInfo.createJavaToken(node)); // push the receiver type
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        String field_name = new String(node.token);

        JavaParser.cactionFieldReferenceEnd(true /* explicit type passed */, field_name, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(FieldReference, BlockScope)");
    }


    public boolean enter(FieldReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(FieldReference, ClassScope)");

        try {
            javaParserSupport.preprocessClass(node.actualReceiverType, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        JavaParser.cactionFieldReference(new String(node.token), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(FieldReference, ClassScope)");

        return true;
    }

    public void exit(FieldReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(FieldReference, ClassScope)");

        try {
            javaParserSupport.generateAndPushType(node.actualReceiverType, this.unitInfo, this.unitInfo.createJavaToken(node)); // push the receiver type
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        String field_name = new String(node.token);

        JavaParser.cactionFieldReferenceEnd(true /* explicit type passed */, field_name, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(FieldReference, BlockScope)");
    }


    public boolean enter(FloatLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(FloatLiteral, BlockScope)");

        float value = node.constant.floatValue();
        String source = (value < 0 ? "-" : "") + new String(node.source());
        JavaParser.cactionFloatLiteral(value, source, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(FloatLiteral, BlockScope)");

        return true;
    }

    public void exit(FloatLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(FloatLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(FloatLiteral, BlockScope)");
    }


    public boolean enter(ForeachStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
               System.out.println("Entering enter(ForeachStatement, BlockScope)");

        JavaParser.cactionForeachStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ForeachStatement, BlockScope)");

        return true;
    }

    public void exit(ForeachStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ForeachStatement, BlockScope)");

        JavaParser.cactionForeachStatementEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ForeachStatement, BlockScope)");
    }


    public boolean enter(ForStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ForStatement, BlockScope)");

        JavaParser.cactionForStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ForStatement, BlockScope)");

        return true;
    }

    public void exit(ForStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ForStatement, BlockScope)");

        JavaParser.cactionForStatementEnd(node.initializations == null ? 0 : node.initializations.length,
                                          node.condition != null,
                                          node.increments == null ? 0 : node.increments.length,
                                          this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ForStatement, BlockScope)");
    }


    public boolean enter(IfStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(IfStatement, BlockScope)");

        JavaParser.cactionIfStatement(node.elseStatement != null, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(IfStatement, BlockScope)");

        return true;
    }

    public void exit(IfStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(IfStatement, BlockScope)");

        JavaParser.cactionIfStatementEnd(node.elseStatement != null, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(IfStatement, BlockScope)");
    }

    public boolean enter(ImportReference node, CompilationUnitScope scope) {
        if (JavaTraversal.verboseLevel > 1)
            System.out.println("Entering enter(ImportReference, CompilationUnitScope)");

        //
        // Do no import the current package.
        //
        if (node == this.unitInfo.unit.currentPackage) {
            if (node.annotations != null && node.annotations.length > 0) {
                JavaParser.cactionPackageAnnotations(node.annotations.length, this.unitInfo.createJavaToken(node));
            }
        }
        else { // Do no import the current package.
            boolean contains_wildcard = ((node.bits & node.OnDemand) != 0);
            String import_name = new String(CharOperation.concatWith(node.getImportName(), '.'));
            JavaParser.cactionImportReference(node.isStatic(), import_name, contains_wildcard, this.unitInfo.createJavaToken(node));
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ImportReference, CompilationUnitScope)");

        return true;
    }

    public void exit(ImportReference node, CompilationUnitScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ImportReference, CompilationUnitScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ImportReference, CompilationUnitScope)");
    }


    public boolean enter(Initializer node, MethodScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Initializer, MethodScope)");

        int initializer_index = javaParserSupport.classInitializerTable.get(node);
        JavaParser.cactionInitializer(node.isStatic(), javaParserSupport.initializerName.get(node), initializer_index, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Initializer, MethodScope)");

        return true;
    }

    public void exit(Initializer node, MethodScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Initializer, MethodScope)");

        JavaParser.cactionInitializerEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Initializer, MethodScope)");
    }


    public boolean enter( InstanceOfExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(InstanceOfExpression, BlockScope)");

        JavaParser.cactionInstanceOfExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(InstanceOfExpression, BlockScope)");

        return true;
    }

    public void exit(InstanceOfExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(InstanceOfExpression, BlockScope)");

        JavaParser.cactionInstanceOfExpressionEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(InstanceOfExpression, BlockScope)");
    }


    public boolean enter(IntLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(IntLiteral, BlockScope) value = " + node.toString());

        int value = node.constant.intValue();
        char source_array[] = node.source();
        String source = (value < 0 &&
                         (! (source_array.length > 1 && // at least 2 elements
                             source_array[0] == '0'  && // starts with 0 
                             (source_array[1] == 'x' || source_array[1] == 'X' || source_array[1] == 'b' || source_array[1] == 'B'))) // not a binary or Heaxadecimal
                         ? "-"
                         : "") + new String(source_array);
        JavaParser.cactionIntLiteral(value, source, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(IntLiteral, BlockScope)");

        return true;
    }

    public void exit(IntLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(IntLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(IntLiteral, BlockScope)");
    }


    public boolean enter(LabeledStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(LabeledStatement, BlockScope)");

        JavaParser.cactionLabeledStatement(new String(node.label), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(LabeledStatement, BlockScope)");

        return true;
    }

    public void exit(LabeledStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(LabeledStatement, BlockScope)");

        JavaParser.cactionLabeledStatementEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(LabeledStatement, BlockScope)");
    }


    public boolean enter(LocalDeclaration node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(LocalDeclaration, BlockScope)");

        //
        // We have to visit the LocalDeclaration node here in case the variable we are declaring 
        // is used in the initialization.  We bumped into this corner case in the Apache Lucene
        // application...
        //
        //        int x = x;
        //
        if (node.annotations != null) {
            for (int i = 0, annotationsLength = node.annotations.length; i < annotationsLength; i++) {
                node.annotations[i].traverse(this, scope);
            }
        }
        node.type.traverse(this, scope);

        JavaParser.cactionLocalDeclaration(node.annotations == null ? 0 : node.annotations.length,
                                           new String(node.name),
                                           node.binding != null && node.binding.isFinal(),
                                           this.unitInfo.createJavaToken(node));

        //
        // Now we can process the initialization even if it uses the variable we just declared.
        //
        if (node.initialization != null) {
            node.initialization.traverse(this, scope);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(LocalDeclaration, BlockScope)");

        return false;
    }

    public void exit(LocalDeclaration node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(LocalDeclaration, BlockScope)");

        JavaParser.cactionLocalDeclarationEnd(new String(node.name),
                                              node.initialization != null,
                                              this.unitInfo.createJavaToken(node));
    }


    public boolean enter(LongLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(LongLiteral, BlockScope)");

        long value = node.constant.longValue();
        char source_array[] = node.source();
        String source = (value < 0 &&
                         (! (source_array.length > 1 && // at least 2 elements
                             source_array[0] == '0'  && // starts with 0 
                             (source_array[1] == 'x' || source_array[1] == 'X' || source_array[1] == 'b' || source_array[1] == 'B'))) // not a binary or Heaxadecimal
                        ? "-"
                        : "") + new String(source_array);
        JavaParser.cactionLongLiteral(value, source, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(LongLiteral, BlockScope)");

        return true;
    }

    public void exit(LongLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(LongLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(LongLiteral, BlockScope)");
    }


    public boolean enter(MarkerAnnotation node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(MarkerAnnotation, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(MarkerAnnotation, BlockScope)");

        return true;
    }

    public void exit(MarkerAnnotation node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(MarkerAnnotation, BlockScope)");

        if (node.type != null) {
            JavaParser.cactionMarkerAnnotationEnd(this.unitInfo.createJavaToken(node));
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(MarkerAnnotation, BlockScope)");
    }


    public boolean enter(MarkerAnnotation node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(MarkerAnnotation, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(MarkerAnnotation, BlockScope)");

        return true;
    }

    public void exit(MarkerAnnotation node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(MarkerAnnotation, BlockScope)");

        if (node.type != null) {
            JavaParser.cactionMarkerAnnotationEnd(this.unitInfo.createJavaToken(node));
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(MarkerAnnotation, BlockScope)");
    }


    public boolean enter(MemberValuePair node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(MemberValuePair, BlockScope)");

        assert(node.value != null); // The member value pair must have a value

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(MemberValuePair, BlockScope)");

        return true;
    }

    public void exit(MemberValuePair node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(MemberValuePair, BlockScope)");

        JavaParser.cactionMemberValuePairEnd(new String(node.name), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(MemberValuePair, BlockScope)");
    }


    public boolean enter(MessageSend node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(MessageSend, BlockScope)");

        String method_name = javaParserSupport.getMethodName(node.binding);

        // 
        // Make sure these two classes are available
        //
        try {
            javaParserSupport.preprocessClass(node.actualReceiverType, this.unitInfo);
            JavaParser.cactionMessageSend(javaParserSupport.getPackageName(node.actualReceiverType),
                                          javaParserSupport.getTypeName(node.actualReceiverType),
                                          method_name,
                                          this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        
        //
        // We perform our own traversal for MessageSend so that we can control whether or not the
        // receiver is processed.
        //
        if (! node.receiver.isImplicitThis()) { // traverse the receiver only if it's not an implicit this.
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

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(MessageSend, BlockScope)");

        return false;
    }


    public void exit(MessageSend node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(MessageSend, BlockScope): " + nodeId(node));

        try {
            int method_index = javaParserSupport.getMethodIndex(node.binding);
            javaParserSupport.generateAndPushType(node.actualReceiverType.erasure(), this.unitInfo, this.unitInfo.createJavaToken(node)); // push the receiver type
            JavaParser.cactionMessageSendEnd(node.binding.isStatic(),
                                             (! node.receiver.isImplicitThis()),
                                             javaParserSupport.getPackageName(node.binding.declaringClass),
                                             javaParserSupport.getTypeName(node.binding.declaringClass),
                                             javaParserSupport.getMethodName(node.binding),
                                             method_index,
                                             node.typeArguments == null ? 0 : node.typeArguments.length,
                                             node.arguments == null ? 0 : node.arguments.length,
                                             this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(MessageSend, BlockScope)");
    }


    public boolean enter(MethodDeclaration node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(MethodDeclaration, ClassScope)");

        //
        // Keep track of methods with no bodies. 
        //
        if (node.statements != null && node.statements.length > 0) {
            MethodHeaderDelimiters.put(node.statements[0], node);
        }

        String name = javaParserSupport.getMethodName(node.binding);

        try {
            int method_index = javaParserSupport.getMethodIndex(node);
            JavaParser.cactionMethodDeclaration(name, method_index, this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(MethodDeclaration, ClassScope)");

        //
        // We perform our own traversal for MethodDeclaration so that we can skip certain subcomponents
        // (commented out below.
        //
        //
        /*
        if (node.javadoc != null) {
            node.javadoc.traverse(this, node.scope);
        }
        */
        if (node.annotations != null) {
            int annotationsLength = node.annotations.length;
            for (int i = 0; i < annotationsLength; i++)
                node.annotations[i].traverse(this, node.scope);
        }

        /* No need for this!!!  The type parameters are fully processed during the initial traversal.
        if (node.typeParameters != null) {
            for (int i = 0; i < node.typeParameters.length; i++) {
                node.typeParameters[i].traverse(this, node.scope);
            }
        }
        */

        /*
        if (node.returnType != null) {
            node.returnType.traverse(this, node.scope);
        }
        */
        if (node.arguments != null) {
            int argumentLength = node.arguments.length;
            for (int i = 0; i < argumentLength; i++) {
                Argument argument = node.arguments[i];
                if (argument.annotations != null) { // does this parameter have annotations?
                    for (int k = 0, max = argument.annotations.length; k < max; k++) {
                        argument.annotations[k].traverse(this, node.scope);
                    }

                    JavaParser.cactionArgumentEnd(argument.annotations.length,
                                                  new String(argument.name),
                                                  this.unitInfo.createJavaToken(argument));
                }
            }
        }

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

        return false;
    }

    public void exit(MethodDeclaration node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(MethodDeclaration, ClassScope)");

        if (node.statements == null || node.statements.length == 0) {
            if (JavaTraversal.verboseLevel > 1)
                System.out.println("    Side-visiting Method Declaration Header for " + node.getClass().getName());
            JavaParser.cactionMethodDeclarationHeader(javaParserSupport.getMethodName(node.binding),
                                                      node.isAbstract(),
                                                      node.isNative(),
                                                      node.isStatic(),
                                                      node.binding.isFinal(),
                                                      node.binding.isSynchronized(),
                                                      node.binding.isPublic(),
                                                      node.binding.isProtected(),
                                                      node.binding.isPrivate(),
                                                      node.binding.isStrictfp(), 
                                                      node.typeParameters == null ? 0 : node.typeParameters.length,
                                                      node.arguments == null ? 0 : node.arguments.length,
                                                      node.thrownExceptions == null ? 0 : node.thrownExceptions.length,
                                                      this.unitInfo.createJavaToken(node)
                                                     );
        }

        JavaParser.cactionMethodDeclarationEnd(node.annotations == null ? 0 : node.annotations.length,
                                               node.statements == null ? 0 : node.statements.length,
                                               this.unitInfo.createJavaToken(node));
    }

    
    public boolean enter( StringLiteralConcatenation node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(StringLiteralConcatenation, BlockScope)");

        JavaParser.cactionStringLiteralConcatenation(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(StringLiteralConcatenation, BlockScope)");

        return true;
    }

    public void exit(StringLiteralConcatenation node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(StringLiteralConcatenation, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(StringLiteralConcatenation, BlockScope)");
    }


    public boolean enter(NormalAnnotation node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(NormalAnnotation, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(NormalAnnotation, BlockScope)");

        return true;
    }

    public void exit(NormalAnnotation node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(NormalAnnotation, BlockScope)");

        assert(node.type != null);
        JavaParser.cactionNormalAnnotationEnd((node.memberValuePairs != null ? node.memberValuePairs.length : 0), this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(NormalAnnotation, BlockScope)");
    }


    public boolean enter(NullLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(NullLiteral, BlockScope)");

        JavaParser.cactionNullLiteral(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(NullLiteral, BlockScope)");

        return true;
    }

    public void exit(NullLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(NullLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(NullLiteral, BlockScope)");
    }


    public boolean enter(OR_OR_Expression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(OR_OR_Expression, BlockScope)");

        JavaParser.cactionORORExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(OR_OR_Expression, BlockScope)");

        return true;
    }

    public void exit(OR_OR_Expression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(OR_OR_Expression, BlockScope)");

        JavaParser.cactionORORExpressionEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(OR_OR_Expression, BlockScope)");
    }


    public boolean enter(ParameterizedQualifiedTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ParameterizedQualifiedTypeReference, BlockScope)");

        try {
            javaParserSupport.processQualifiedParameterizedTypeReference(node, this, scope, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ParameterizedQualifiedTypeReference, BlockScope)");

        return false; // short-circuit the traversal here... See processQualifiedParameterizedTypeReference(...)
    }

    public void exit(ParameterizedQualifiedTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ParameterizedQualifiedTypeReference, BlockScope)");

        // Do nothing

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ParameterizedQualifiedTypeReference, BlockScope)");
    }


    public boolean enter(ParameterizedQualifiedTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ParameterizedQualifiedTypeReference, ClassScope)");

        try {
            javaParserSupport.processQualifiedParameterizedTypeReference(node, this, scope, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ParameterizedQualifiedTypeReference, ClassScope)");

        return false; // short-circuit the traversal... see processQualifiedParameterizedTypeReference(...)
    }

    public void exit(ParameterizedQualifiedTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ParameterizedQualifiedTypeReference, ClassScope)");

        // Do nothing
        
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ParameterizedQualifiedTypeReference, BlockScope)");
    }


    public boolean enter(ParameterizedSingleTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ParameterizedSingleTypeReference, BlockScope)");

        JavaParser.cactionParameterizedTypeReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ParameterizedSingleTypeReference, BlockScope)");

        return true;
    }

    public void exit(ParameterizedSingleTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("At top of exit(ParameterizedSingleTypeReference, BlockScope)");
        
        try {
            if (node.resolvedType.isClass() || node.resolvedType.isInterface()) { 
                if (JavaTraversal.verboseLevel > 0)
                    System.out.println("(01) The parameterized single type referenced is bound to type " + node.resolvedType.debugName());
                javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
            }

            String package_name = javaParserSupport.getPackageName(node.resolvedType);
            String type_name = javaParserSupport.getTypeName(node.resolvedType);

            JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                            type_name,
                                                            node.typeArguments != null, // We can assert that this is true, I think.
                                                            node.typeArguments.length,
                                                            this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (node.dimensions() > 0) {
            JavaParser.cactionArrayTypeReference(node.dimensions(), this.unitInfo.createJavaToken(node));
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ParameterizedSingleTypeReference, BlockScope)");
    }


    public boolean enter(ParameterizedSingleTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ParameterizedSingleTypeReference, ClassScope)");

        JavaParser.cactionParameterizedTypeReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ParameterizedSingleTypeReference, ClassScope)");

        return true;
    }

    public void exit(ParameterizedSingleTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ParameterizedSingleTypeReference, ClassScope)");

        try {
            if (node.resolvedType.isClass() || node.resolvedType.isInterface()) { 
                if (JavaTraversal.verboseLevel > 0)
                    System.out.println("(01) The parameterized single type referenced is bound to type " + node.resolvedType.debugName());
                javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
            }
            String package_name = javaParserSupport.getPackageName(node.resolvedType);
            String type_name = javaParserSupport.getTypeName(node.resolvedType);
            JavaParser.cactionParameterizedTypeReferenceEnd(package_name,
                                                            type_name,
                                                            node.typeArguments != null, // We can assert that this is true, I think.
                                                            node.typeArguments.length,
                                                            this.unitInfo.createJavaToken(node));
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (node.dimensions() > 0) {
            JavaParser.cactionArrayTypeReference(node.dimensions(), this.unitInfo.createJavaToken(node));
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ParameterizedSingleTypeReference, ClassScope)");
    }


    public boolean enter(PostfixExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(PostfixExpression, BlockScope)");

        JavaParser.cactionPostfixExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(PostfixExpression, BlockScope)");

        return true;
    }

    public void exit(PostfixExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(PostfixExpression, BlockScope)");

        JavaParser.cactionPostfixExpressionEnd(node.operator, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(PostfixExpression, BlockScope)");
    }


    public boolean enter(PrefixExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(PrefixExpression, BlockScope)");

        JavaParser.cactionPrefixExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(PrefixExpression, BlockScope)");

        return true;
    }

    public void exit(PrefixExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(PrefixExpression, BlockScope)");

        JavaParser.cactionPrefixExpressionEnd(node.operator, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(PrefixExpression, BlockScope)");
    }


    public boolean enter(QualifiedAllocationExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(QualifiedAllocationExpression, BlockScope)");

        if (node.type != null) {
            try {
                javaParserSupport.preprocessClass(node.type.resolvedType, this.unitInfo);
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
            
        JavaParser.cactionQualifiedAllocationExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(QualifiedAllocationExpression, BlockScope)");

        return true;
    }

    public void exit(QualifiedAllocationExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedAllocationExpression, BlockScope)");

        if (JavaTraversal.verboseLevel > 0 && node.type != null)
            System.out.println("The Allocation type is bound to type " + node.type.resolvedType.debugName());

        JavaParser.cactionQualifiedAllocationExpressionEnd(node.type != null, 
                                                           node.enclosingInstance != null,
                                                           node.arguments == null ? 0 : node.arguments.length,
                                                           node.anonymousType != null,
                                                           this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedAllocationExpression, BlockScope)");
    }


    public boolean enter(QualifiedNameReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(QualifiedNameReference, BlockScope)");

        try {
            javaParserSupport.processQualifiedNameReference(node, scope, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(QualifiedNameReference, BlockScope)");

        return true;
    }

    public void exit(QualifiedNameReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(QualifiedNameReference, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedNameReference, BlockScope)");
    }


    public boolean enter(QualifiedNameReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(QualifiedNameReference, ClassScope)");

        try {
            javaParserSupport.processQualifiedNameReference(node, scope, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(QualifiedNameReference, ClassScope)");

        return true;
    }

    public void exit(QualifiedNameReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(QualifiedNameReference, ClassScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedNameReference, ClassScope)");
    }


    public boolean enter(QualifiedSuperReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(QualifiedSuperReference, BlockScope)");

        JavaParser.cactionQualifiedSuperReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(QualifiedSuperReference, BlockScope)");

        return true;
    }

    public void exit(QualifiedSuperReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Inside of exit(QualifiedSuperReference, BlockScope)");

        JavaParser.cactionQualifiedSuperReferenceEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedSuperReference, BlockScope)");
    }


    public boolean enter(QualifiedSuperReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(QualifiedSuperReference, ClassScope)");

        JavaParser.cactionQualifiedSuperReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(QualifiedSuperReference, ClassScope)");

        return true;
    }

    public void exit(QualifiedSuperReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Inside of exit(QualifiedSuperReference, ClassScope)");

        JavaParser.cactionQualifiedSuperReferenceEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedSuperReference, ClassScope)");
    }


    public boolean enter(QualifiedThisReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(QualifiedThisReference, BlockScope)");

        JavaParser.cactionQualifiedThisReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(QualifiedThisReference, BlockScope)");

        return true;
    }

    public void exit(QualifiedThisReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Inside of exit(QualifiedThisReference, BlockScope)");

        JavaParser.cactionQualifiedThisReferenceEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedThisReference, BlockScope)");
    }


    public boolean enter(QualifiedThisReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(QualifiedThisReference, ClassScope)");

        JavaParser.cactionQualifiedThisReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(QualifiedThisReference, ClassScope)");

        return true;
    }

    public void exit(QualifiedThisReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Inside of exit(QualifiedThisReference, ClassScope)");

        JavaParser.cactionQualifiedThisReferenceEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedThisReference, ClassScope)");
    }


    public boolean enter(QualifiedTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(QualifiedTypeReference, BlockScope)");

        try {
            javaParserSupport.processQualifiedTypeReference(node.tokens, node.tokens.length, scope, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }        

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(QualifiedTypeReference, BlockScope)");

        return true;
    }

    public void exit(QualifiedTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(QualifiedTypeReference, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedTypeReference, BlockScope)");
    }

 
    public boolean enter(QualifiedTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(QualifiedTypeReference, ClassScope)");

        try {
            javaParserSupport.processQualifiedTypeReference(node.tokens, node.tokens.length, scope, this.unitInfo);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(QualifiedTypeReference, ClassScope)");

        return true;
    }

    public void exit(QualifiedTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(QualifiedTypeReference, ClassScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(QualifiedTypeReference, ClassScope)");
    }


    public boolean enter(ReturnStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ReturnStatement, BlockScope)");

        JavaParser.cactionReturnStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ReturnStatement, BlockScope)");

        return true;
    }

    public void exit(ReturnStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ReturnStatement, BlockScope)");

        JavaParser.cactionReturnStatementEnd(node.expression != null, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ReturnStatement, BlockScope)");
    }


    public boolean enter(SingleMemberAnnotation node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(SingleMemberAnnotation, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(SingleMemberAnnotation, BlockScope)");

        return true;
    }

    public void exit(SingleMemberAnnotation node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(SingleMemberAnnotation, BlockScope)");

        if (node.type != null && node.memberValue != null) {
            JavaParser.cactionSingleMemberAnnotationEnd(this.unitInfo.createJavaToken(node));
        }
        else assert(node.type == null && node.memberValue == null);

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SingleMemberAnnotation, BlockScope)");
    }


    public boolean enter(SingleNameReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(SingleNameReference, BlockScope)");

        try {
            enterSingleNameReference(node);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(SingleNameReference, BlockScope)");

        return true;
    }

    public void exit(SingleNameReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(SingleNameReference, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SingleNameReference, BlockScope)");
    }


    public boolean enter(SingleNameReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(SingleNameReference, ClassScope)");

        try {
            enterSingleNameReference(node);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(SingleNameReference, ClassScope)");

        return true;
    }

    public void exit(SingleNameReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(SingleNameReference, ClassScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SingleNameReference, ClassScope)");
    }


    public boolean enter(SingleTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(SingleTypeReference, BlockScope)");

        try {
            if (node.resolvedType.isClass() || node.resolvedType.isInterface() || node.resolvedType.isEnum()) { 
                if (JavaTraversal.verboseLevel > 0)
                    System.out.println("(1) The single type referenced is bound to type " + node.resolvedType.debugName());
                javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
            }
            else if (! (node.resolvedType instanceof BaseTypeBinding)) {
                throw new RuntimeException("*** No support yet for type: " + node.resolvedType.getClass().getCanonicalName()); // System.exit(1);
            }

            if (node.resolvedType instanceof TypeVariableBinding) { // is this name a type variable?
                TypeVariableBinding type_variable_binding = (TypeVariableBinding) node.resolvedType;
                if (JavaTraversal.verboseLevel > 0) {
                    System.out.println("The Single name referenced " + node.toString() + " is bound to type " + type_variable_binding.debugName());
                }

                String type_parameter_name = javaParserSupport.getTypeName(type_variable_binding);
                Binding binding = type_variable_binding.declaringElement;
                if (binding instanceof TypeBinding) {
                    TypeBinding enclosing_binding = (TypeBinding) binding;
                    String package_name = javaParserSupport.getPackageName(enclosing_binding),
                           type_name = javaParserSupport.getTypeName(enclosing_binding);
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
                JavaParser.cactionTypeReference(package_name, type_name, this.unitInfo.createJavaToken(node));
            }
        }
        catch (RuntimeException e) {
            throw e;
        }
        catch (Throwable e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(SingleTypeReference, BlockScope)");

        return true;
    }

    public void exit(SingleTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(SingleTypeReference, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SingleTypeReference, BlockScope)");
    }


    public boolean enter(SingleTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(SingleTypeReference, BlockScope)");

        try {
            if (node.resolvedType.isClass() || node.resolvedType.isInterface() || node.resolvedType.isEnum()) {
                if (JavaTraversal.verboseLevel > 0)
                    System.out.println("(2) The single type referenced is bound to type " + node.resolvedType.debugName());        
                javaParserSupport.setupClass(node.resolvedType, this.unitInfo);
            }
            else if (! (node.resolvedType instanceof BaseTypeBinding)) {
                throw new RuntimeException("*** No support yet for type: " + node.resolvedType.getClass().getCanonicalName()); // System.exit(1);
            }

            if (node.resolvedType instanceof TypeVariableBinding) { // is this name a type variable?
                TypeVariableBinding type_variable_binding = (TypeVariableBinding) node.resolvedType;
                if (JavaTraversal.verboseLevel > 0) {
                    System.out.println("The Single name referenced " + node.toString() + " is bound to type " + type_variable_binding.debugName());
                }

                String type_parameter_name = javaParserSupport.getTypeName(type_variable_binding);
                Binding binding = type_variable_binding.declaringElement;
                if (binding instanceof TypeBinding) {
                    TypeBinding enclosing_binding = (TypeBinding) binding;
                    String package_name = javaParserSupport.getPackageName(enclosing_binding),
                           type_name = javaParserSupport.getTypeName(enclosing_binding);
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
                JavaParser.cactionTypeReference(package_name, type_name, this.unitInfo.createJavaToken(node));
            }
        }
        catch (RuntimeException e) {
            throw e;
        }
        catch (Throwable e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(SingleTypeReference, BlockScope)");

        return true;
    }

    public void exit(SingleTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(SingleTypeReference, ClassScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SingleTypeReference, ClassScope)");
    }


    public boolean enter(StringLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(StringLiteral, BlockScope)");

        String source = new String(node.source());
        JavaParser.cactionStringLiteral(source, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(StringLiteral, BlockScope)");

        return true;
    }

    public void exit(StringLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(StringLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(StringLiteral, BlockScope)");
    }


    public boolean enter(SuperReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(SuperReference, BlockScope)");

        JavaParser.cactionSuperReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(SuperReference, BlockScope)");

        return true;
    }

    public void exit(SuperReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SuperReference, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(SuperReference, BlockScope)");
    }


    public boolean enter(SwitchStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(SwitchStatement, BlockScope)");

        JavaParser.cactionSwitchStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(SwitchStatement, BlockScope)");

        return true;
    }

    public void exit(SwitchStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SwitchStatement, BlockScope)");

        JavaParser.cactionSwitchStatementEnd(node.caseCount, node.defaultCase != null, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SwitchStatement, BlockScope)");
    }


    public boolean enter(SynchronizedStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(SynchronizedStatement, BlockScope)");

        JavaParser.cactionSynchronizedStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(SynchronizedStatement, BlockScope)");

        return true;
    }

    public void exit(SynchronizedStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SynchronizedStatement, BlockScope)");

        JavaParser.cactionSynchronizedStatementEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(SynchronizedStatement, BlockScope)");
    }


    public boolean enter(ThisReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ThisReference, BlockScope)");

        JavaParser.cactionThisReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ThisReference, BlockScope)");

        return true;
    }

    public void exit(ThisReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ThisReference, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ThisReference, BlockScope)");
    }


    public boolean enter(ThisReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ThisReference, ClassScope)");

        JavaParser.cactionThisReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ThisReference, ClassScope)");

        return true;
    }

    public void exit(ThisReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(ThisReference, ClassScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ThisReference, ClassScope)");
    }


    public boolean enter(ThrowStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(ThrowStatement, BlockScope)");

        JavaParser.cactionThrowStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(ThrowStatement, BlockScope)");

        return true;
    }

    public void exit(ThrowStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ThrowStatement, BlockScope)");

        JavaParser.cactionThrowStatementEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(ThrowStatement, BlockScope)");
    }


    public boolean enter(TrueLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(TrueLiteral, BlockScope)");

        JavaParser.cactionTrueLiteral(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(TrueLiteral, BlockScope)");

        return true;
    }

    public void exit(TrueLiteral node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(TrueLiteral, BlockScope)");

        // do nothing by default

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(TrueLiteral, BlockScope)");
    }


    public boolean enter(TryStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(TryStatement, BlockScope)");

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
          
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(TryStatement, BlockScope)");

        return true;
    }

    public void exit(TryStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("exit TryStatement -- BlockScope");

        JavaParser.cactionTryStatementEnd(node.resources.length, node.catchArguments == null ? 0 : node.catchBlocks.length, node.finallyBlock != null, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(TryStatement, BlockScope)");
    }

    //
    // Local and Anonymous types are processed here.
    //
    public boolean enter(TypeDeclaration node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("visit TypeDeclaration -- BlockScope");

        assert(node.binding instanceof LocalTypeBinding); 

        //
        // First, preprocess the symbol table information of the type in question.
        //
        JavaParserSupport.LocalOrAnonymousType special_type = javaParserSupport.localOrAnonymousType.get(node);

        try {
            if (node.binding.isAnonymousType()) {
                JavaToken location = this.unitInfo.createJavaToken(node);
                ReferenceBinding enclosing_type = special_type.getEnclosingType();
                JavaParser.cactionPushTypeScope(javaParserSupport.getPackageName(enclosing_type), javaParserSupport.getTypeName(enclosing_type), location);
                javaParserSupport.insertClasses(node);
                javaParserSupport.traverseTypeDeclaration(node, this.unitInfo);
                JavaParser.cactionPopTypeScope();
            }
            else {
                javaParserSupport.insertClasses(node);
                javaParserSupport.traverseTypeDeclaration(node, this.unitInfo);
            }
        
            enterTypeDeclaration(node);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(TypeDeclaration, BlockScope)");

        return false; // The traversal is done in enterTypeDeclaration(node) in the user-specified order of the class members
    }

    public void exit(TypeDeclaration node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("exit TypeDeclaration -- BlockScope");

        exitTypeDeclaration(node);

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(TypeDeclaration, BlockScope)");
    }


    public boolean enter(TypeDeclaration node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("visit TypeDeclaration -- ClassScope");

        try {
            enterTypeDeclaration(node);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(TypeDeclaration, ClassScope)");

        return false; // The traversal is done in enterTypeDeclaration(node) in the user-specified order of the class members
    }

    public void exit(TypeDeclaration node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("exit TypeDeclaration -- ClassScope");

        exitTypeDeclaration(node);

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(TypeDeclaration, ClassScope)");
    }


    public boolean enter(TypeDeclaration node, CompilationUnitScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Inside of enter(TypeDeclaration, CompilationUnitScope)");

        if (node.name != TypeConstants.PACKAGE_INFO_NAME) { // ignore package-info declarations
            try {
                enterTypeDeclaration(node);
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
        } 

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(TypeDeclaration, CompilationUnitScope)");

        return false; // The traversal is done in enterTypeDeclaration(node) in the user-specified order of the class members
    }

    public void exit(TypeDeclaration node, CompilationUnitScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(TypeDeclaration, CompilationUnitScope)");

        if (node.name != TypeConstants.PACKAGE_INFO_NAME) { // ignore package-info declarations
            exitTypeDeclaration(node);
        }

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(TypeDeclaration, CompilationUnitScope)");
    }


    public boolean enter(TypeParameter node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(TypeParameter, BlockScope)");

        assert(false); // Not suppose to get here!

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(TypeParameter, BlockScope)");

        return true;
    }

    public void exit(TypeParameter node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(TypeParameter, BlockScope)");

        assert(false); // Not suppose to get here!

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(TypeParameter, BlockScope)");
    }


    public boolean enter(TypeParameter node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(TypeParameter, ClassScope)");

        assert(false); // Not suppose to get here!

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(TypeParameter, ClassScope)");

        return true;
    }

    public void exit(TypeParameter node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Inside exit(TypeParameter, ClassScope)");

        assert(false); // Not suppose to get here!

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(TypeParameter, ClassScope)");
    }

    public boolean enter(UnaryExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(UnaryExpression, BlockScope)");

        JavaParser.cactionUnaryExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(UnaryExpression, BlockScope)");

        return true;
    }

    public void exit(UnaryExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(UnaryExpression, BlockScope)");

        // Not clear what the valueRequired filed means.
        int operator_kind = (node.bits & ASTNode.OperatorMASK) >> ASTNode.OperatorSHIFT;

        JavaParser.cactionUnaryExpressionEnd(operator_kind, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(UnaryExpression, BlockScope)");
    }


    public boolean enter(WhileStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(WhileStatement, BlockScope)");

        JavaParser.cactionWhileStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(WhileStatement, BlockScope)");

        return true;
    }

    public void exit(WhileStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(WhileStatement, BlockScope)");

        JavaParser.cactionWhileStatementEnd(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(WhileStatement, BlockScope)");
    }


    public boolean enter(Wildcard node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Wildcard, BlockScope)");

        JavaParser.cactionWildcard(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Wildcard, BlockScope)");

        return true;
    }

    public void exit(Wildcard node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(Wildcard, BlockScope)");

        JavaParser.cactionWildcardEnd(node.kind == Wildcard.UNBOUND, node.kind == Wildcard.EXTENDS, node.kind == Wildcard.SUPER, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Wildcard , BlockScope)");
    }


    public boolean enter(Wildcard node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Wildcard, ClassScope)");

        JavaParser.cactionWildcard(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Wildcard, ClassScope)");

        return true;
    }

    public void exit(Wildcard node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering exit(Wildcard, ClassScope)");

        JavaParser.cactionWildcardEnd(node.kind == Wildcard.UNBOUND, node.kind == Wildcard.EXTENDS, node.kind == Wildcard.SUPER, this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Wildcard, ClassScope)");
    }
    
    //**********************************************************
    //*                                                        *
    //*    Start of JavaDoc Nodes.                             *
    //*                                                        *
    //**********************************************************
    /*
    public boolean enter(Javadoc node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Javadoc, BlockScope)");

        JavaParser.cactionJavadoc(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Javadoc, BlockScope)");

        return true;
    }

    public void exit(Javadoc node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Javadoc, BlockScope)");
    }


    public boolean enter(Javadoc node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(Javadoc, ClassScope)");

        JavaParser.cactionJavadocClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(Javadoc, ClassScope)");

        return true;
    }

    public void exit(Javadoc node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(Javadoc, ClassScope)");
    }


    public boolean enter(JavadocAllocationExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocAllocationExpression, BlockScope)");

        JavaParser.cactionJavadocAllocationExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocAllocationExpression, BlockScope)");

        return true;
    }

    public void exit(JavadocAllocationExpression node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocAllocationExpression, BlockScope)");
    }


    public boolean enter(JavadocAllocationExpression node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocAllocationExpression, ClassScope)");

        JavaParser.cactionJavadocAllocationExpressionClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocAllocationExpression, ClassScope)");

        return true;
    }

    public void exit(JavadocAllocationExpression node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocAllocationExpression, ClassScope)");
    }


    public boolean enter(JavadocArgumentExpression node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enterJavadocArgumentExpression(JavadocArgumentExpression, BlockScope)");

        JavaParser.cactionJavadocArgumentExpression(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocArgumentExpression, BlockScope)");

        return true;
    }

    public void exit(JavadocArgumentExpression node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocArgumentExpression, BlockScope)");
    }


    public boolean enter(JavadocArgumentExpression node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocArgumentExpression, ClassScope)");

        JavaParser.cactionJavadocArgumentExpressionClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocArgumentExpression, ClassScope)");

        return true;
    }

    public void exit(JavadocArgumentExpression node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocArgumentExpression, ClassScope)");
    }


    public boolean enter(JavadocArrayQualifiedTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocArrayQualifiedTypeReference, BlockScope)");

        JavaParser.cactionJavadocArrayQualifiedTypeReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocArrayQualifiedTypeReference, BlockScope)");

        return true;
    }

    public void exit(JavadocArrayQualifiedTypeReference node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocArrayQualifiedTypeReference, BlockScope)");
    }


    public boolean enter(JavadocArrayQualifiedTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocArrayQualifiedTypeReference, ClassScope)");

        JavaParser.cactionJavadocArrayQualifiedTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocArrayQualifiedTypeReference, ClassScope)");

        return true;
    }

    public void exit(JavadocArrayQualifiedTypeReference node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocArrayQualifiedTypeReference, ClassScope)");
    }


    public boolean enter(JavadocArraySingleTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocArraySingleTypeReference, BlockScope)");

        JavaParser.cactionJavadocArraySingleTypeReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocArraySingleTypeReference, BlockScope)");

        return true;
    }

    public void exit(JavadocArraySingleTypeReference node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocArraySingleTypeReference, BlockScope)");
    }


    public boolean enter(JavadocArraySingleTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocArraySingleTypeReference, ClassScope)");

        JavaParser.cactionJavadocArraySingleTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocArraySingleTypeReference, ClassScope)");

        return true;
    }

    public void exit(JavadocArraySingleTypeReference node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocArraySingleTypeReference, ClassScope)");
    }


    public boolean enter(JavadocFieldReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocFieldReference, BlockScope)");

        JavaParser.cactionJavadocFieldReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocFieldReference, BlockScope)");

        return true;
    }

    public void exit(JavadocFieldReference node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocFieldReference, BlockScope)");
    }


    public boolean enter(JavadocFieldReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocFieldReference, ClassScope)");

        JavaParser.cactionJavadocFieldReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocFieldReference, ClassScope)");

        return true;
    }

    public void exit(JavadocFieldReference node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocFieldReference, ClassScope)");
    }


    public boolean enter(JavadocImplicitTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocImplicitTypeReference, BlockScope)");

        JavaParser.cactionJavadocImplicitTypeReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocImplicitTypeReference, BlockScope)");

        return true;
    }

    public void exit(JavadocImplicitTypeReference node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocImplicitTypeReference, BlockScope)");
    }


    public boolean enter(JavadocImplicitTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocImplicitTypeReference, ClassScope)");

        JavaParser.cactionJavadocImplicitTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocImplicitTypeReference, ClassScope)");

        return true;
    }

    public void exit(JavadocImplicitTypeReference node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocImplicitTypeReference, ClassScope)");
    }


    public boolean enter(JavadocMessageSend node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocMessageSend, BlockScope)");

        JavaParser.cactionJavadocMessageSend(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocMessageSend, BlockScope)");

        return true;
    }

    public void exit(JavadocMessageSend node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocMessageSend, BlockScope)");
    }


    public boolean enter(JavadocMessageSend node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocMessageSend, ClassScope)");

        JavaParser.cactionJavadocMessageSendClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocMessageSend, ClassScope)");

        return true;
    }

    public void exit(JavadocMessageSend node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocMessageSend, ClassScope)");
    }


    public boolean enter(JavadocQualifiedTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocQualifiedTypeReference, BlockScope)");

        JavaParser.cactionJavadocQualifiedTypeReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocQualifiedTypeReference, BlockScope)");

        return true;
    }

    public void exit(JavadocQualifiedTypeReference node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocQualifiedTypeReference, BlockScope)");
    }


    public boolean enter(JavadocQualifiedTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocQualifiedTypeReference, ClassScope)");

        JavaParser.cactionJavadocQualifiedTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocQualifiedTypeReference, ClassScope)");

        return true;
    }

    public void exit(JavadocQualifiedTypeReference node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocQualifiedTypeReference, ClassScope)");
    }


    public boolean enter(JavadocReturnStatement node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocReturnStatement, BlockScope)");

        JavaParser.cactionJavadocReturnStatement(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocReturnStatement, BlockScope)");

        return true;
    }

    public void exit(JavadocReturnStatement node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocReturnStatement, BlockScope)");
    }


    public boolean enter(JavadocReturnStatement node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocReturnStatement, ClassScope)");

        JavaParser.cactionJavadocReturnStatementClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocReturnStatement, ClassScope)");

        return true;
    }

    public void exit(JavadocReturnStatement node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocReturnStatement, ClassScope)");
    }


    public boolean enter(JavadocSingleNameReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocSingleNameReference, BlockScope)");

        JavaParser.cactionJavadocSingleNameReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocSingleNameReference, BlockScope)");

        return true;
    }

    public void exit(JavadocSingleNameReference node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocSingleNameReference, BlockScope)");
    }


    public boolean enter(JavadocSingleNameReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocSingleNameReference, ClassScope)");

        JavaParser.cactionJavadocSingleNameReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocSingleNameReference, ClassScope)");

        return true;
    }

    public void exit(JavadocSingleNameReference node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocSingleNameReference, ClassScope)");
    }


    public boolean enter(JavadocSingleTypeReference node, BlockScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocSingleTypeReference, BlockScope)");

        JavaParser.cactionJavadocSingleTypeReference(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocSingleTypeReference, BlockScope)");

        return true;
    }

    public void exit(JavadocSingleTypeReference node, BlockScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocSingleTypeReference, BlockScope)");
    }


    public boolean enter(JavadocSingleTypeReference node, ClassScope scope) {
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Entering enter(JavadocSingleTypeReference, ClassScope)");

        JavaParser.cactionJavadocSingleTypeReferenceClassScope(this.unitInfo.createJavaToken(node));

        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving enter(JavadocSingleTypeReference, ClassScope)");

        return true;
    }

    public void exit(JavadocSingleTypeReference node, ClassScope scope) {
        // do nothing by default
        if (JavaTraversal.verboseLevel > 0)
            System.out.println("Leaving exit(JavadocSingleTypeReference, ClassScope)");
    }
    */

    //**********************************************************
    //*                                                        *
    //*    End of JavaDoc Nodes.                               *
    //*                                                        *
    //**********************************************************
}

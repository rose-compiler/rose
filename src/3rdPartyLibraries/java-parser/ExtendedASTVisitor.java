import org.eclipse.jdt.core.compiler.IProblem;
import org.eclipse.jdt.internal.compiler.ASTVisitor;
import org.eclipse.jdt.internal.compiler.ast.*;
import org.eclipse.jdt.internal.compiler.lookup.BlockScope;
import org.eclipse.jdt.internal.compiler.lookup.ClassScope;
import org.eclipse.jdt.internal.compiler.lookup.CompilationUnitScope;
import org.eclipse.jdt.internal.compiler.lookup.MethodScope;

/**
 * A extended visitor for iterating through an ECJ parse tree. Unlike the base ASTVisitor,
 * this extended visitor first preVisits each node (generically); then visits the node
 * (specifically); then endVisits the node (specifically); and, finally, postVisits the
 * node (generically).
 */
public abstract class ExtendedASTVisitor extends ASTVisitor {
    /**
     * This preVisit() function gives the user the opportunity to either proceed or to
     * short-circuit a traversal prior to visiting a node in question.
     *
     * This function is particularly usefull for detecting the end of a construct that
     * is not clearly delineated in the ECJ AST structure.  For example, there is no node
     * for a ClassBody or a MethodBody.  Thus, the end of the class header can be detected
     * when pre-visiting the first class-member-declaration of a given class.  Similarly
     * the end of a method header can be detected when pre-visiting the first statement or
     * declaration in the method body.
     *
     *     class C extends ... { int i; ... }
     *                           ^
     *                           |
     */
    public boolean preVisit(ASTNode node) {
        // do nothing by default
        return true;
    }

    /**
     * This postVisit() function gives the user a final opportunity to perform a task for
     * the node in question after all its components have been processed.
     *
     * For example, if a class declaration or method declaration does not contain a body,
     * we detect that we are at the end of the header declaration when post visiting the
     * node in question:
     *
     *     class C {}
     *
     *         or
     *
     *     void f();
     *
     *         or
     *
     *     void f(int x) throws ... {}
     *
     */
    public void postVisit(ASTNode node) {
        // do nothing by default
    }
    
    /**
     * The function visit() first preVisits() the node in question.  If the preVisit() is
     * successful then it enters() the node to process its components.
     */
    public boolean visit(AllocationExpression allocationExpression, BlockScope scope) {
        return preVisit(allocationExpression) && enter(allocationExpression, scope);
    }
    public boolean visit(AND_AND_Expression and_and_Expression, BlockScope scope) {
        return preVisit(and_and_Expression) && enter(and_and_Expression, scope);
    }
    public boolean visit(AnnotationMethodDeclaration annotationTypeDeclaration, ClassScope classScope) {
        return preVisit(annotationTypeDeclaration) && enter(annotationTypeDeclaration, classScope);
    }
    public boolean visit(Argument argument, BlockScope scope) {
        return preVisit(argument) && enter(argument, scope);
    }
    public boolean visit(Argument argument, ClassScope scope) {
        return preVisit(argument) && enter(argument, scope);
    }
    public boolean visit(ArrayAllocationExpression arrayAllocationExpression, BlockScope scope) {
        return preVisit(arrayAllocationExpression) && enter(arrayAllocationExpression, scope);
    }
    public boolean visit(ArrayInitializer arrayInitializer, BlockScope scope) {
        return preVisit(arrayInitializer) && enter(arrayInitializer, scope);
    }
    public boolean visit(ArrayQualifiedTypeReference arrayQualifiedTypeReference, BlockScope scope) {
        return preVisit(arrayQualifiedTypeReference) && enter(arrayQualifiedTypeReference, scope);
    }
    public boolean visit(ArrayQualifiedTypeReference arrayQualifiedTypeReference, ClassScope scope) {
        return preVisit(arrayQualifiedTypeReference) && enter(arrayQualifiedTypeReference, scope);
    }
    public boolean visit(ArrayReference arrayReference, BlockScope scope) {
        return preVisit(arrayReference) && enter(arrayReference, scope);
    }
    public boolean visit(ArrayTypeReference arrayTypeReference, BlockScope scope) {
        return preVisit(arrayTypeReference) && enter(arrayTypeReference, scope);
    }
    public boolean visit(ArrayTypeReference arrayTypeReference, ClassScope scope) {
        return preVisit(arrayTypeReference) && enter(arrayTypeReference, scope);
    }
    public boolean visit(AssertStatement assertStatement, BlockScope scope) {
        return preVisit(assertStatement) && enter(assertStatement,  scope);
    }
    public boolean visit(Assignment assignment, BlockScope scope) {
        return preVisit(assignment) && enter(assignment, scope);
    }
    public boolean visit(BinaryExpression binaryExpression, BlockScope scope) {
        return preVisit(binaryExpression) && enter(binaryExpression, scope);
    }
    public boolean visit(Block block, BlockScope scope) {
        return preVisit(block) && enter(block, scope);
    }
    public boolean visit(BreakStatement breakStatement, BlockScope scope) {
        return preVisit(breakStatement) && enter(breakStatement, scope);
    }
    public boolean visit(CaseStatement caseStatement, BlockScope scope) {
        return preVisit(caseStatement) && enter(caseStatement, scope);
    }
    public boolean visit(CastExpression castExpression, BlockScope scope) {
        return preVisit(castExpression) && enter(castExpression, scope);
    }
    public boolean visit(CharLiteral charLiteral, BlockScope scope) {
        return preVisit(charLiteral) && enter(charLiteral, scope);
    }
    public boolean visit(ClassLiteralAccess classLiteral, BlockScope scope) {
        return preVisit(classLiteral) && enter(classLiteral, scope);
    }
    public boolean visit(Clinit clinit, ClassScope scope) {
        return preVisit(clinit) && enter(clinit, scope);
    }
    public boolean visit(CompilationUnitDeclaration compilationUnitDeclaration, CompilationUnitScope scope) {
        return preVisit(compilationUnitDeclaration) && enter(compilationUnitDeclaration, scope);
    }
    public boolean visit(CompoundAssignment compoundAssignment, BlockScope scope) {
        return preVisit(compoundAssignment) && enter(compoundAssignment, scope);
    }
    public boolean visit(ConditionalExpression conditionalExpression, BlockScope scope) {
        return preVisit(conditionalExpression) && enter(conditionalExpression, scope);
    }
    public boolean visit(ConstructorDeclaration constructorDeclaration, ClassScope scope) {
        return preVisit(constructorDeclaration) && enter(constructorDeclaration, scope);
    }
    public boolean visit(ContinueStatement continueStatement, BlockScope scope) {
        return preVisit(continueStatement) && enter(continueStatement, scope);
    }
    public boolean visit(DoStatement doStatement, BlockScope scope) {
        return preVisit(doStatement) && enter(doStatement, scope);
    }
    public boolean visit(DoubleLiteral doubleLiteral, BlockScope scope) {
        return preVisit(doubleLiteral) && enter(doubleLiteral, scope);
    }
    public boolean visit(EmptyStatement emptyStatement, BlockScope scope) {
        return preVisit(emptyStatement) && enter(emptyStatement, scope);
    }
    public boolean visit(EqualExpression equalExpression, BlockScope scope) {
        return preVisit(equalExpression) && enter(equalExpression, scope);
    }
    public boolean visit(ExplicitConstructorCall explicitConstructor, BlockScope scope) {
        return preVisit(explicitConstructor) && enter(explicitConstructor, scope);
    }
    public boolean visit(ExtendedStringLiteral extendedStringLiteral, BlockScope scope) {
        return preVisit(extendedStringLiteral) && enter(extendedStringLiteral, scope);
    }
    public boolean visit(FalseLiteral falseLiteral, BlockScope scope) {
        return preVisit(falseLiteral) && enter(falseLiteral, scope);
    }
    public boolean visit(FieldDeclaration fieldDeclaration, MethodScope scope) {
        return preVisit(fieldDeclaration) && enter(fieldDeclaration, scope);
    }
    public boolean visit(FieldReference fieldReference, BlockScope scope) {
        return preVisit(fieldReference) && enter(fieldReference, scope);
    }
    public boolean visit(FieldReference fieldReference, ClassScope scope) {
        return preVisit(fieldReference) && enter(fieldReference, scope);
    }
    public boolean visit(FloatLiteral floatLiteral, BlockScope scope) {
        return preVisit(floatLiteral) && enter(floatLiteral, scope);
    }
    public boolean visit(ForeachStatement forStatement, BlockScope scope) {
        return preVisit(forStatement) && enter(forStatement, scope);
    }
    public boolean visit(ForStatement forStatement, BlockScope scope) {
        return preVisit(forStatement) && enter(forStatement, scope);
    }
    public boolean visit(IfStatement ifStatement, BlockScope scope) {
        return preVisit(ifStatement) && enter(ifStatement, scope);
    }
    public boolean visit(ImportReference importRef, CompilationUnitScope scope) {
        return preVisit(importRef) && enter(importRef, scope);
    }
    public boolean visit(Initializer initializer, MethodScope scope) {
        return preVisit(initializer) && enter(initializer, scope);
    }
    public boolean visit(InstanceOfExpression instanceOfExpression, BlockScope scope) {
        return preVisit(instanceOfExpression) && enter(instanceOfExpression, scope);
    }
    public boolean visit(IntLiteral intLiteral, BlockScope scope) {
        return preVisit(intLiteral) && enter(intLiteral, scope);
    }
    public boolean visit(Javadoc javadoc, BlockScope scope) {
        return preVisit(javadoc) && enter(javadoc, scope);
    }
    public boolean visit(Javadoc javadoc, ClassScope scope) {
        return preVisit(javadoc) && enter(javadoc, scope);
    }
    public boolean visit(JavadocAllocationExpression expression, BlockScope scope) {
        return preVisit(expression) && enter(expression, scope);
    }
    public boolean visit(JavadocAllocationExpression expression, ClassScope scope) {
        return preVisit(expression) && enter(expression, scope);
    }
    public boolean visit(JavadocArgumentExpression expression, BlockScope scope) {
        return preVisit(expression) && enter(expression, scope);
    }
    public boolean visit(JavadocArgumentExpression expression, ClassScope scope) {
        return preVisit(expression) && enter(expression, scope);
    }
    public boolean visit(JavadocArrayQualifiedTypeReference typeRef, BlockScope scope) {
        return preVisit(typeRef) && enter(typeRef, scope);
    }
    public boolean visit(JavadocArrayQualifiedTypeReference typeRef, ClassScope scope) {
        return preVisit(typeRef) && enter(typeRef, scope);
    }
    public boolean visit(JavadocArraySingleTypeReference typeRef, BlockScope scope) {
        return preVisit(typeRef) && enter(typeRef, scope);
    }
    public boolean visit(JavadocArraySingleTypeReference typeRef, ClassScope scope) {
        return preVisit(typeRef) && enter(typeRef, scope);
    }
    public boolean visit(JavadocFieldReference fieldRef, BlockScope scope) {
        return preVisit(fieldRef) && enter(fieldRef, scope);
    }
    public boolean visit(JavadocFieldReference fieldRef, ClassScope scope) {
        return preVisit(fieldRef) && enter(fieldRef, scope);
    }
    public boolean visit(JavadocImplicitTypeReference implicitTypeReference, BlockScope scope) {
        return preVisit(implicitTypeReference) && enter(implicitTypeReference, scope);
    }
    public boolean visit(JavadocImplicitTypeReference implicitTypeReference, ClassScope scope) {
        return preVisit(implicitTypeReference) && enter(implicitTypeReference, scope);
    }
    public boolean visit(JavadocMessageSend messageSend, BlockScope scope) {
        return preVisit(messageSend) && enter(messageSend, scope);
    }
    public boolean visit(JavadocMessageSend messageSend, ClassScope scope) {
        return preVisit(messageSend) && enter(messageSend, scope);
    }
    public boolean visit(JavadocQualifiedTypeReference typeRef, BlockScope scope) {
        return preVisit(typeRef) && enter(typeRef, scope);
    }
    public boolean visit(JavadocQualifiedTypeReference typeRef, ClassScope scope) {
        return preVisit(typeRef) && enter(typeRef, scope);
    }
    public boolean visit(JavadocReturnStatement statement, BlockScope scope) {
        return preVisit(statement) && enter(statement, scope);
    }
    public boolean visit(JavadocReturnStatement statement, ClassScope scope) {
        return preVisit(statement) && enter(statement, scope);
    }
    public boolean visit(JavadocSingleNameReference argument, BlockScope scope) {
        return preVisit(argument) && enter(argument, scope);
    }
    public boolean visit(JavadocSingleNameReference argument, ClassScope scope) {
        return preVisit(argument) && enter(argument, scope);
    }
    public boolean visit(JavadocSingleTypeReference typeRef, BlockScope scope) {
        return preVisit(typeRef) && enter(typeRef, scope);
    }
    public boolean visit(JavadocSingleTypeReference typeRef, ClassScope scope) {
        return preVisit(typeRef) && enter(typeRef, scope);
    }
    public boolean visit(LabeledStatement labeledStatement, BlockScope scope) {
        return preVisit(labeledStatement) && enter(labeledStatement, scope);
    }
    public boolean visit(LocalDeclaration localDeclaration, BlockScope scope) {
        return preVisit(localDeclaration) && enter(localDeclaration, scope);
    }
    public boolean visit(LongLiteral longLiteral, BlockScope scope) {
        return preVisit(longLiteral) && enter(longLiteral, scope);
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public boolean visit(MarkerAnnotation annotation, BlockScope scope) {
        return preVisit(annotation) && enter(annotation, scope);
    }
    /**
     * @param pair
     * @param scope
     * @since 3.1
     */
    public boolean visit(MemberValuePair pair, BlockScope scope) {
        return preVisit(pair) && enter(pair, scope);
    }
    public boolean visit(MessageSend messageSend, BlockScope scope) {
        return preVisit(messageSend) && enter(messageSend, scope);
    }
    public boolean visit(MethodDeclaration methodDeclaration, ClassScope scope) {
        return preVisit(methodDeclaration) && enter(methodDeclaration, scope);
    }
    public boolean visit(StringLiteralConcatenation literal, BlockScope scope) {
        return preVisit(literal) && enter(literal, scope);
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public boolean visit(NormalAnnotation annotation, BlockScope scope) {
        return preVisit(annotation) && enter(annotation, scope);
    }
    public boolean visit(NullLiteral nullLiteral, BlockScope scope) {
        return preVisit(nullLiteral) && enter(nullLiteral, scope);
    }
    public boolean visit(OR_OR_Expression or_or_Expression, BlockScope scope) {
        return preVisit(or_or_Expression) && enter(or_or_Expression, scope);
    }
    public boolean visit(ParameterizedQualifiedTypeReference parameterizedQualifiedTypeReference, BlockScope scope) {
        return preVisit(parameterizedQualifiedTypeReference) && enter(parameterizedQualifiedTypeReference, scope);
    }
    public boolean visit(ParameterizedQualifiedTypeReference parameterizedQualifiedTypeReference, ClassScope scope) {
        return preVisit(parameterizedQualifiedTypeReference) && enter(parameterizedQualifiedTypeReference, scope);
    }
    public boolean visit(ParameterizedSingleTypeReference parameterizedSingleTypeReference, BlockScope scope) {
        return preVisit(parameterizedSingleTypeReference) && enter(parameterizedSingleTypeReference, scope);
    }
    public boolean visit(ParameterizedSingleTypeReference parameterizedSingleTypeReference, ClassScope scope) {
        return preVisit(parameterizedSingleTypeReference) && enter(parameterizedSingleTypeReference, scope);
    }
    public boolean visit(PostfixExpression postfixExpression, BlockScope scope) {
        return preVisit(postfixExpression) && enter(postfixExpression, scope);
    }
    public boolean visit(PrefixExpression prefixExpression, BlockScope scope) {
        return preVisit(prefixExpression) && enter(prefixExpression, scope);
    }
    public boolean visit(QualifiedAllocationExpression qualifiedAllocationExpression, BlockScope scope) {
        return preVisit(qualifiedAllocationExpression) && enter(qualifiedAllocationExpression, scope);
    }
    public boolean visit(QualifiedNameReference qualifiedNameReference, BlockScope scope) {
        return preVisit(qualifiedNameReference) && enter(qualifiedNameReference, scope);
    }
    public boolean visit(QualifiedNameReference qualifiedNameReference, ClassScope scope) {
        return preVisit(qualifiedNameReference) && enter(qualifiedNameReference, scope);
    }
    public boolean visit(QualifiedSuperReference qualifiedSuperReference, BlockScope scope) {
        return preVisit(qualifiedSuperReference) && enter(qualifiedSuperReference, scope);
    }
    public boolean visit(QualifiedSuperReference qualifiedSuperReference, ClassScope scope) {
        return preVisit(qualifiedSuperReference) && enter(qualifiedSuperReference, scope);
    }
    public boolean visit(QualifiedThisReference qualifiedThisReference, BlockScope scope) {
        return preVisit(qualifiedThisReference) && enter(qualifiedThisReference, scope);
    }
    public boolean visit(QualifiedThisReference qualifiedThisReference, ClassScope scope) {
        return preVisit(qualifiedThisReference) && enter(qualifiedThisReference, scope);
    }
    public boolean visit(QualifiedTypeReference qualifiedTypeReference, BlockScope scope) {
        return preVisit(qualifiedTypeReference) && enter(qualifiedTypeReference, scope);
    }
    public boolean visit(QualifiedTypeReference qualifiedTypeReference, ClassScope scope) {
        return preVisit(qualifiedTypeReference) && enter(qualifiedTypeReference, scope);
    }
    public boolean visit(ReturnStatement returnStatement, BlockScope scope) {
        return preVisit(returnStatement) && enter(returnStatement, scope);
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public boolean visit(SingleMemberAnnotation annotation, BlockScope scope) {
        return preVisit(annotation) && enter(annotation, scope);
    }
    public boolean visit(SingleNameReference singleNameReference, BlockScope scope) {
        return preVisit(singleNameReference) && enter(singleNameReference, scope);
    }
    public boolean visit(SingleNameReference singleNameReference, ClassScope scope) {
        return preVisit(singleNameReference) && enter(singleNameReference, scope);
    }
    public boolean visit(SingleTypeReference singleTypeReference, BlockScope scope) {
        return preVisit(singleTypeReference) && enter(singleTypeReference, scope);
    }
    public boolean visit(SingleTypeReference singleTypeReference, ClassScope scope) {
        return preVisit(singleTypeReference) && enter(singleTypeReference, scope);
    }
    public boolean visit(StringLiteral stringLiteral, BlockScope scope) {
        return preVisit(stringLiteral) && enter(stringLiteral, scope);
    }
    public boolean visit(SuperReference superReference, BlockScope scope) {
        return preVisit(superReference) && enter(superReference, scope);
    }
    public boolean visit(SwitchStatement switchStatement, BlockScope scope) {
        return preVisit(switchStatement) && enter(switchStatement, scope);
    }
    public boolean visit(SynchronizedStatement synchronizedStatement, BlockScope scope) {
        return preVisit(synchronizedStatement) && enter(synchronizedStatement, scope);
    }
    public boolean visit(ThisReference thisReference, BlockScope scope) {
        return preVisit(thisReference) && enter(thisReference, scope);
    }
    public boolean visit(ThisReference thisReference, ClassScope scope) {
        return preVisit(thisReference) && enter(thisReference, scope);
    }
    public boolean visit(ThrowStatement throwStatement, BlockScope scope) {
        return preVisit(throwStatement) && enter(throwStatement, scope);
    }
    public boolean visit(TrueLiteral trueLiteral, BlockScope scope) {
        return preVisit(trueLiteral) && enter(trueLiteral, scope);
    }
    public boolean visit(TryStatement tryStatement, BlockScope scope) {
        return preVisit(tryStatement) && enter(tryStatement, scope);
    }
    public boolean visit(TypeDeclaration localTypeDeclaration, BlockScope scope) {
        return preVisit(localTypeDeclaration) && enter(localTypeDeclaration, scope);
    }
    public boolean visit(TypeDeclaration memberTypeDeclaration, ClassScope scope) {
        return preVisit(memberTypeDeclaration) && enter(memberTypeDeclaration, scope);
    }
    public boolean visit(TypeDeclaration typeDeclaration, CompilationUnitScope scope) {
        return preVisit(typeDeclaration) && enter(typeDeclaration, scope);
    }
    public boolean visit(TypeParameter typeParameter, BlockScope scope) {
        return preVisit(typeParameter) && enter(typeParameter, scope);
    }
    public boolean visit(TypeParameter typeParameter, ClassScope scope) {
        return preVisit(typeParameter) && enter(typeParameter, scope);
    }
    public boolean visit(UnaryExpression unaryExpression, BlockScope scope) {
        return preVisit(unaryExpression) && enter(unaryExpression, scope);
    }
    public boolean visit(WhileStatement whileStatement, BlockScope scope) {
        return preVisit(whileStatement) && enter(whileStatement, scope);
    }
    public boolean visit(Wildcard wildcard, BlockScope scope) {
        return preVisit(wildcard) && enter(wildcard, scope);
    }
    public boolean visit(Wildcard wildcard, ClassScope scope) {
        return preVisit(wildcard) && enter(wildcard, scope);
    }

    public boolean enter(AllocationExpression allocationExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(AND_AND_Expression and_and_Expression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(AnnotationMethodDeclaration annotationTypeDeclaration, ClassScope classScope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Argument argument, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Argument argument, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ArrayAllocationExpression arrayAllocationExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ArrayInitializer arrayInitializer, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ArrayQualifiedTypeReference arrayQualifiedTypeReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ArrayQualifiedTypeReference arrayQualifiedTypeReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ArrayReference arrayReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ArrayTypeReference arrayTypeReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ArrayTypeReference arrayTypeReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(AssertStatement assertStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Assignment assignment, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(BinaryExpression binaryExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Block block, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(BreakStatement breakStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(CaseStatement caseStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(CastExpression castExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(CharLiteral charLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ClassLiteralAccess classLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Clinit clinit, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(CompilationUnitDeclaration compilationUnitDeclaration, CompilationUnitScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(CompoundAssignment compoundAssignment, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ConditionalExpression conditionalExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ConstructorDeclaration constructorDeclaration, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ContinueStatement continueStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(DoStatement doStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(DoubleLiteral doubleLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(EmptyStatement emptyStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(EqualExpression equalExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ExplicitConstructorCall explicitConstructor, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ExtendedStringLiteral extendedStringLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(FalseLiteral falseLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(FieldDeclaration fieldDeclaration, MethodScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(FieldReference fieldReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(FieldReference fieldReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(FloatLiteral floatLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ForeachStatement forStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ForStatement forStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(IfStatement ifStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ImportReference importRef, CompilationUnitScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Initializer initializer, MethodScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(InstanceOfExpression instanceOfExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(IntLiteral intLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Javadoc javadoc, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Javadoc javadoc, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocAllocationExpression expression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocAllocationExpression expression, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocArgumentExpression expression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocArgumentExpression expression, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocArrayQualifiedTypeReference typeRef, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocArrayQualifiedTypeReference typeRef, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocArraySingleTypeReference typeRef, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocArraySingleTypeReference typeRef, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocFieldReference fieldRef, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocFieldReference fieldRef, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocImplicitTypeReference implicitTypeReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocImplicitTypeReference implicitTypeReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocMessageSend messageSend, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocMessageSend messageSend, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocQualifiedTypeReference typeRef, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocQualifiedTypeReference typeRef, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocReturnStatement statement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocReturnStatement statement, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocSingleNameReference argument, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocSingleNameReference argument, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocSingleTypeReference typeRef, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(JavadocSingleTypeReference typeRef, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(LabeledStatement labeledStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(LocalDeclaration localDeclaration, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(LongLiteral longLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public boolean enter(MarkerAnnotation annotation, BlockScope scope) {
        return true;
    }
    /**
     * @param pair
     * @param scope
     * @since 3.1
     */
    public boolean enter(MemberValuePair pair, BlockScope scope) {
        return true;
    }
    public boolean enter(MessageSend messageSend, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(MethodDeclaration methodDeclaration, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(StringLiteralConcatenation literal, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public boolean enter(NormalAnnotation annotation, BlockScope scope) {
        return true;
    }
    public boolean enter(NullLiteral nullLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(OR_OR_Expression or_or_Expression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ParameterizedQualifiedTypeReference parameterizedQualifiedTypeReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ParameterizedQualifiedTypeReference parameterizedQualifiedTypeReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ParameterizedSingleTypeReference parameterizedSingleTypeReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ParameterizedSingleTypeReference parameterizedSingleTypeReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(PostfixExpression postfixExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(PrefixExpression prefixExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(QualifiedAllocationExpression qualifiedAllocationExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(QualifiedNameReference qualifiedNameReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(QualifiedNameReference qualifiedNameReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(QualifiedSuperReference qualifiedSuperReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(QualifiedSuperReference qualifiedSuperReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(QualifiedThisReference qualifiedThisReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(QualifiedThisReference qualifiedThisReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(QualifiedTypeReference qualifiedTypeReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(QualifiedTypeReference qualifiedTypeReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ReturnStatement returnStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public boolean enter(SingleMemberAnnotation annotation, BlockScope scope) {
        return true;
    }
    public boolean enter(SingleNameReference singleNameReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(SingleNameReference singleNameReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(SingleTypeReference singleTypeReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(SingleTypeReference singleTypeReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(StringLiteral stringLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(SuperReference superReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(SwitchStatement switchStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(SynchronizedStatement synchronizedStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ThisReference thisReference, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ThisReference thisReference, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(ThrowStatement throwStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(TrueLiteral trueLiteral, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(TryStatement tryStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(TypeDeclaration localTypeDeclaration, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(TypeDeclaration memberTypeDeclaration, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(TypeDeclaration typeDeclaration, CompilationUnitScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(TypeParameter typeParameter, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(TypeParameter typeParameter, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(UnaryExpression unaryExpression, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(WhileStatement whileStatement, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Wildcard wildcard, BlockScope scope) {
        return true; // do nothing by default, keep traversing
    }
    public boolean enter(Wildcard wildcard, ClassScope scope) {
        return true; // do nothing by default, keep traversing
    }

    public void exit(AllocationExpression allocationExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(AND_AND_Expression and_and_Expression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(AnnotationMethodDeclaration annotationTypeDeclaration, ClassScope classScope) {
            // do nothing by default
    }
    public void exit(Argument argument, BlockScope scope) {
        // do nothing by default
    }
    public void exit(Argument argument, ClassScope scope) {
        // do nothing by default
    }
    public void exit(ArrayAllocationExpression arrayAllocationExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ArrayInitializer arrayInitializer, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ArrayQualifiedTypeReference arrayQualifiedTypeReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ArrayQualifiedTypeReference arrayQualifiedTypeReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(ArrayReference arrayReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ArrayTypeReference arrayTypeReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ArrayTypeReference arrayTypeReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(AssertStatement assertStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(Assignment assignment, BlockScope scope) {
        // do nothing by default
    }
    public void exit(BinaryExpression binaryExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(Block block, BlockScope scope) {
        // do nothing by default
    }
    public void exit(BreakStatement breakStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(CaseStatement caseStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(CastExpression castExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(CharLiteral charLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ClassLiteralAccess classLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(Clinit clinit, ClassScope scope) {
        // do nothing by default
    }
    public void exit(CompilationUnitDeclaration compilationUnitDeclaration, CompilationUnitScope scope) {
        // do nothing by default
    }
    public void exit(CompoundAssignment compoundAssignment, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ConditionalExpression conditionalExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ConstructorDeclaration constructorDeclaration, ClassScope scope) {
        // do nothing by default
    }
    public void exit(ContinueStatement continueStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(DoStatement doStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(DoubleLiteral doubleLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(EmptyStatement emptyStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(EqualExpression equalExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ExplicitConstructorCall explicitConstructor, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ExtendedStringLiteral extendedStringLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(FalseLiteral falseLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(FieldDeclaration fieldDeclaration, MethodScope scope) {
        // do nothing by default
    }
    public void exit(FieldReference fieldReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(FieldReference fieldReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(FloatLiteral floatLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ForeachStatement forStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ForStatement forStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(IfStatement ifStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ImportReference importRef, CompilationUnitScope scope) {
        // do nothing by default
    }
    public void exit(Initializer initializer, MethodScope scope) {
        // do nothing by default
    }
    public void exit(InstanceOfExpression instanceOfExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(IntLiteral intLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(Javadoc javadoc, BlockScope scope) {
        // do nothing by default
    }
    public void exit(Javadoc javadoc, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocAllocationExpression expression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocAllocationExpression expression, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocArgumentExpression expression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocArgumentExpression expression, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocArrayQualifiedTypeReference typeRef, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocArrayQualifiedTypeReference typeRef, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocArraySingleTypeReference typeRef, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocArraySingleTypeReference typeRef, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocFieldReference fieldRef, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocFieldReference fieldRef, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocImplicitTypeReference implicitTypeReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocImplicitTypeReference implicitTypeReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocMessageSend messageSend, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocMessageSend messageSend, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocQualifiedTypeReference typeRef, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocQualifiedTypeReference typeRef, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocReturnStatement statement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocReturnStatement statement, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocSingleNameReference argument, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocSingleNameReference argument, ClassScope scope) {
        // do nothing by default
    }
    public void exit(JavadocSingleTypeReference typeRef, BlockScope scope) {
        // do nothing by default
    }
    public void exit(JavadocSingleTypeReference typeRef, ClassScope scope) {
        // do nothing by default
    }
    public void exit(LabeledStatement labeledStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(LocalDeclaration localDeclaration, BlockScope scope) {
        // do nothing by default
    }
    public void exit(LongLiteral longLiteral, BlockScope scope) {
        // do nothing by default
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public void exit(MarkerAnnotation annotation, BlockScope scope) {
        // do nothing by default
    }
    /**
     * @param pair
     * @param scope
     */
    public void exit(MemberValuePair pair, BlockScope scope) {
        // do nothing by default
    }
    public void exit(MessageSend messageSend, BlockScope scope) {
        // do nothing by default
    }
    public void exit(MethodDeclaration methodDeclaration, ClassScope scope) {
        // do nothing by default
    }
    public void exit(StringLiteralConcatenation literal, BlockScope scope) {
        // do nothing by default
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public void exit(NormalAnnotation annotation, BlockScope scope) {
        // do nothing by default
    }
    public void exit(NullLiteral nullLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(OR_OR_Expression or_or_Expression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ParameterizedQualifiedTypeReference parameterizedQualifiedTypeReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ParameterizedQualifiedTypeReference parameterizedQualifiedTypeReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(ParameterizedSingleTypeReference parameterizedSingleTypeReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ParameterizedSingleTypeReference parameterizedSingleTypeReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(PostfixExpression postfixExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(PrefixExpression prefixExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(QualifiedAllocationExpression qualifiedAllocationExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(QualifiedNameReference qualifiedNameReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(QualifiedNameReference qualifiedNameReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(QualifiedSuperReference qualifiedSuperReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(QualifiedSuperReference qualifiedSuperReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(QualifiedThisReference qualifiedThisReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(QualifiedThisReference qualifiedThisReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(QualifiedTypeReference qualifiedTypeReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(QualifiedTypeReference qualifiedTypeReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(ReturnStatement returnStatement, BlockScope scope) {
        // do nothing by default
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public void exit(SingleMemberAnnotation annotation, BlockScope scope) {
        // do nothing by default
    }
    public void exit(SingleNameReference singleNameReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(SingleNameReference singleNameReference, ClassScope scope) {
            // do nothing by default
    }
    public void exit(SingleTypeReference singleTypeReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(SingleTypeReference singleTypeReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(StringLiteral stringLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(SuperReference superReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(SwitchStatement switchStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(SynchronizedStatement synchronizedStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ThisReference thisReference, BlockScope scope) {
        // do nothing by default
    }
    public void exit(ThisReference thisReference, ClassScope scope) {
        // do nothing by default
    }
    public void exit(ThrowStatement throwStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(TrueLiteral trueLiteral, BlockScope scope) {
        // do nothing by default
    }
    public void exit(TryStatement tryStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(TypeDeclaration localTypeDeclaration, BlockScope scope) {
        // do nothing by default
    }
    public void exit(TypeDeclaration memberTypeDeclaration, ClassScope scope) {
        // do nothing by default
    }
    public void exit(TypeDeclaration typeDeclaration, CompilationUnitScope scope) {
        // do nothing by default
    }
    public void exit(TypeParameter typeParameter, BlockScope scope) {
        // do nothing by default
    }
    public void exit(TypeParameter typeParameter, ClassScope scope) {
        // do nothing by default
    }
    public void exit(UnaryExpression unaryExpression, BlockScope scope) {
        // do nothing by default
    }
    public void exit(WhileStatement whileStatement, BlockScope scope) {
        // do nothing by default
    }
    public void exit(Wildcard wildcard, BlockScope scope) {
        // do nothing by default
    }
    public void exit(Wildcard wildcard, ClassScope scope) {
        // do nothing by default
    }

    /**
     * The function endVisit is called after all the components of a given node have
     * been processed.  If first invokes the function exit to wrap-up then it invokes
     * the function postVisit in case any final clean-up is required for the node in question.
     */
    public void endVisit(AllocationExpression allocationExpression, BlockScope scope) {
        // do nothing by default
        exit(allocationExpression, scope);
        postVisit(allocationExpression);
    }
    public void endVisit(AND_AND_Expression and_and_Expression, BlockScope scope) {
        // do nothing by default
        exit(and_and_Expression, scope);
        postVisit(and_and_Expression);
    }
    public void endVisit(AnnotationMethodDeclaration annotationTypeDeclaration, ClassScope classScope) {
            // do nothing by default
        exit(annotationTypeDeclaration, classScope);
        postVisit(annotationTypeDeclaration);
    }
    public void endVisit(Argument argument, BlockScope scope) {
        // do nothing by default
        exit(argument, scope);
        postVisit(argument);
    }
    public void endVisit(Argument argument, ClassScope scope) {
        // do nothing by default
        exit(argument, scope);
        postVisit(argument);
    }
    public void endVisit(ArrayAllocationExpression arrayAllocationExpression, BlockScope scope) {
        // do nothing by default
        exit(arrayAllocationExpression, scope);
        postVisit(arrayAllocationExpression);
    }
    public void endVisit(ArrayInitializer arrayInitializer, BlockScope scope) {
        // do nothing by default
        exit(arrayInitializer, scope);
        postVisit(arrayInitializer);
    }
    public void endVisit(ArrayQualifiedTypeReference arrayQualifiedTypeReference, BlockScope scope) {
        // do nothing by default
        exit(arrayQualifiedTypeReference, scope);
        postVisit(arrayQualifiedTypeReference);
    }
    public void endVisit(ArrayQualifiedTypeReference arrayQualifiedTypeReference, ClassScope scope) {
        // do nothing by default
        exit(arrayQualifiedTypeReference, scope);
        postVisit(arrayQualifiedTypeReference);
    }
    public void endVisit(ArrayReference arrayReference, BlockScope scope) {
        // do nothing by default
        exit(arrayReference, scope);
        postVisit(arrayReference);
    }
    public void endVisit(ArrayTypeReference arrayTypeReference, BlockScope scope) {
        // do nothing by default
        exit(arrayTypeReference, scope);
        postVisit(arrayTypeReference);
    }
    public void endVisit(ArrayTypeReference arrayTypeReference, ClassScope scope) {
        // do nothing by default
        exit(arrayTypeReference, scope);
        postVisit(arrayTypeReference);
    }
    public void endVisit(AssertStatement assertStatement, BlockScope scope) {
        // do nothing by default
        exit(assertStatement, scope);
        postVisit(assertStatement);
    }
    public void endVisit(Assignment assignment, BlockScope scope) {
        // do nothing by default
        exit(assignment, scope);
        postVisit(assignment);
    }
    public void endVisit(BinaryExpression binaryExpression, BlockScope scope) {
        // do nothing by default
        exit(binaryExpression, scope);
        postVisit(binaryExpression);
    }
    public void endVisit(Block block, BlockScope scope) {
        // do nothing by default
        exit(block, scope);
        postVisit(block);
    }
    public void endVisit(BreakStatement breakStatement, BlockScope scope) {
        // do nothing by default
        exit(breakStatement, scope);
        postVisit(breakStatement);
    }
    public void endVisit(CaseStatement caseStatement, BlockScope scope) {
        // do nothing by default
        exit(caseStatement, scope);
        postVisit(caseStatement);
    }
    public void endVisit(CastExpression castExpression, BlockScope scope) {
        // do nothing by default
        exit(castExpression, scope);
        postVisit(castExpression);
    }
    public void endVisit(CharLiteral charLiteral, BlockScope scope) {
        // do nothing by default
        exit(charLiteral, scope);
        postVisit(charLiteral);
    }
    public void endVisit(ClassLiteralAccess classLiteral, BlockScope scope) {
        // do nothing by default
        exit(classLiteral, scope);
        postVisit(classLiteral);
    }
    public void endVisit(Clinit clinit, ClassScope scope) {
        // do nothing by default
        exit(clinit, scope);
        postVisit(clinit);
    }
    public void endVisit(CompilationUnitDeclaration compilationUnitDeclaration, CompilationUnitScope scope) {
        // do nothing by default
        exit(compilationUnitDeclaration, scope);
        postVisit(compilationUnitDeclaration);
    }
    public void endVisit(CompoundAssignment compoundAssignment, BlockScope scope) {
        // do nothing by default
        exit(compoundAssignment, scope);
        postVisit(compoundAssignment);
    }
    public void endVisit(ConditionalExpression conditionalExpression, BlockScope scope) {
        // do nothing by default
        exit(conditionalExpression, scope);
        postVisit(conditionalExpression);
    }
    public void endVisit(ConstructorDeclaration constructorDeclaration, ClassScope scope) {
        // do nothing by default
        exit(constructorDeclaration, scope);
        postVisit(constructorDeclaration);
    }
    public void endVisit(ContinueStatement continueStatement, BlockScope scope) {
        // do nothing by default
        exit(continueStatement, scope);
        postVisit(continueStatement);
    }
    public void endVisit(DoStatement doStatement, BlockScope scope) {
        // do nothing by default
        exit(doStatement, scope);
        postVisit(doStatement);
    }
    public void endVisit(DoubleLiteral doubleLiteral, BlockScope scope) {
        // do nothing by default
        exit(doubleLiteral, scope);
        postVisit(doubleLiteral);
    }
    public void endVisit(EmptyStatement emptyStatement, BlockScope scope) {
        // do nothing by default
        exit(emptyStatement, scope);
        postVisit(emptyStatement);
    }
    public void endVisit(EqualExpression equalExpression, BlockScope scope) {
        // do nothing by default
        exit(equalExpression, scope);
        postVisit(equalExpression);
    }
    public void endVisit(ExplicitConstructorCall explicitConstructor, BlockScope scope) {
        // do nothing by default
        exit(explicitConstructor, scope);
        postVisit(explicitConstructor);
    }
    public void endVisit(ExtendedStringLiteral extendedStringLiteral, BlockScope scope) {
        // do nothing by default
        exit(extendedStringLiteral, scope);
        postVisit(extendedStringLiteral);
    }
    public void endVisit(FalseLiteral falseLiteral, BlockScope scope) {
        // do nothing by default
        exit(falseLiteral, scope);
        postVisit(falseLiteral);
    }
    public void endVisit(FieldDeclaration fieldDeclaration, MethodScope scope) {
        // do nothing by default
        exit(fieldDeclaration, scope);
        postVisit(fieldDeclaration);
    }
    public void endVisit(FieldReference fieldReference, BlockScope scope) {
        // do nothing by default
        exit(fieldReference, scope);
        postVisit(fieldReference);
    }
    public void endVisit(FieldReference fieldReference, ClassScope scope) {
        // do nothing by default
        exit(fieldReference, scope);
        postVisit(fieldReference);
    }
    public void endVisit(FloatLiteral floatLiteral, BlockScope scope) {
        // do nothing by default
        exit(floatLiteral, scope);
        postVisit(floatLiteral);
    }
    public void endVisit(ForeachStatement forStatement, BlockScope scope) {
        // do nothing by default
        exit(forStatement, scope);
        postVisit(forStatement);
    }
    public void endVisit(ForStatement forStatement, BlockScope scope) {
        // do nothing by default
        exit(forStatement, scope);
        postVisit(forStatement);
    }
    public void endVisit(IfStatement ifStatement, BlockScope scope) {
        // do nothing by default
        exit(ifStatement, scope);
        postVisit(ifStatement);
    }
    public void endVisit(ImportReference importRef, CompilationUnitScope scope) {
        // do nothing by default
        exit(importRef, scope);
        postVisit(importRef);
    }
    public void endVisit(Initializer initializer, MethodScope scope) {
        // do nothing by default
        exit(initializer, scope);
        postVisit(initializer);
    }
    public void endVisit(InstanceOfExpression instanceOfExpression, BlockScope scope) {
        // do nothing by default
        exit(instanceOfExpression, scope);
        postVisit(instanceOfExpression);
    }
    public void endVisit(IntLiteral intLiteral, BlockScope scope) {
        // do nothing by default
        exit(intLiteral, scope);
        postVisit(intLiteral);
    }
    public void endVisit(Javadoc javadoc, BlockScope scope) {
        // do nothing by default
        exit(javadoc, scope);
        postVisit(javadoc);
    }
    public void endVisit(Javadoc javadoc, ClassScope scope) {
        // do nothing by default
        exit(javadoc, scope);
        postVisit(javadoc);
    }
    public void endVisit(JavadocAllocationExpression expression, BlockScope scope) {
        // do nothing by default
        exit(expression, scope);
        postVisit(expression);
    }
    public void endVisit(JavadocAllocationExpression expression, ClassScope scope) {
        // do nothing by default
        exit(expression, scope);
        postVisit(expression);
    }
    public void endVisit(JavadocArgumentExpression expression, BlockScope scope) {
        // do nothing by default
        exit(expression, scope);
        postVisit(expression);
    }
    public void endVisit(JavadocArgumentExpression expression, ClassScope scope) {
        // do nothing by default
        exit(expression, scope);
        postVisit(expression);
    }
    public void endVisit(JavadocArrayQualifiedTypeReference typeRef, BlockScope scope) {
        // do nothing by default
        exit(typeRef, scope);
        postVisit(typeRef);
    }
    public void endVisit(JavadocArrayQualifiedTypeReference typeRef, ClassScope scope) {
        // do nothing by default
        exit(typeRef, scope);
        postVisit(typeRef);
    }
    public void endVisit(JavadocArraySingleTypeReference typeRef, BlockScope scope) {
        // do nothing by default
        exit(typeRef, scope);
        postVisit(typeRef);
    }
    public void endVisit(JavadocArraySingleTypeReference typeRef, ClassScope scope) {
        // do nothing by default
        exit(typeRef, scope);
        postVisit(typeRef);
    }
    public void endVisit(JavadocFieldReference fieldRef, BlockScope scope) {
        // do nothing by default
        exit(fieldRef, scope);
        postVisit(fieldRef);
    }
    public void endVisit(JavadocFieldReference fieldRef, ClassScope scope) {
        // do nothing by default
        exit(fieldRef, scope);
        postVisit(fieldRef);
    }
    public void endVisit(JavadocImplicitTypeReference implicitTypeReference, BlockScope scope) {
        // do nothing by default
        exit(implicitTypeReference, scope);
        postVisit(implicitTypeReference);
    }
    public void endVisit(JavadocImplicitTypeReference implicitTypeReference, ClassScope scope) {
        // do nothing by default
        exit(implicitTypeReference, scope);
        postVisit(implicitTypeReference);
    }
    public void endVisit(JavadocMessageSend messageSend, BlockScope scope) {
        // do nothing by default
        exit(messageSend, scope);
        postVisit(messageSend);
    }
    public void endVisit(JavadocMessageSend messageSend, ClassScope scope) {
        // do nothing by default
        exit(messageSend, scope);
        postVisit(messageSend);
    }
    public void endVisit(JavadocQualifiedTypeReference typeRef, BlockScope scope) {
        // do nothing by default
        exit(typeRef, scope);
        postVisit(typeRef);
    }
    public void endVisit(JavadocQualifiedTypeReference typeRef, ClassScope scope) {
        // do nothing by default
        exit(typeRef, scope);
        postVisit(typeRef);
    }
    public void endVisit(JavadocReturnStatement statement, BlockScope scope) {
        // do nothing by default
        exit(statement, scope);
        postVisit(statement);
    }
    public void endVisit(JavadocReturnStatement statement, ClassScope scope) {
        // do nothing by default
        exit(statement, scope);
        postVisit(statement);
    }
    public void endVisit(JavadocSingleNameReference argument, BlockScope scope) {
        // do nothing by default
        exit(argument, scope);
        postVisit(argument);
    }
    public void endVisit(JavadocSingleNameReference argument, ClassScope scope) {
        // do nothing by default
        exit(argument, scope);
        postVisit(argument);
    }
    public void endVisit(JavadocSingleTypeReference typeRef, BlockScope scope) {
        // do nothing by default
        exit(typeRef, scope);
        postVisit(typeRef);
    }
    public void endVisit(JavadocSingleTypeReference typeRef, ClassScope scope) {
        // do nothing by default
        exit(typeRef, scope);
        postVisit(typeRef);
    }
    public void endVisit(LabeledStatement labeledStatement, BlockScope scope) {
        // do nothing by default
        exit(labeledStatement, scope);
        postVisit(labeledStatement);
    }
    public void endVisit(LocalDeclaration localDeclaration, BlockScope scope) {
        // do nothing by default
        exit(localDeclaration, scope);
        postVisit(localDeclaration);
    }
    public void endVisit(LongLiteral longLiteral, BlockScope scope) {
        // do nothing by default
        exit(longLiteral, scope);
        postVisit(longLiteral);
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public void endVisit(MarkerAnnotation annotation, BlockScope scope) {
        // do nothing by default
        exit(annotation, scope);
        postVisit(annotation);
    }
    /**
     * @param pair
     * @param scope
     */
    public void endVisit(MemberValuePair pair, BlockScope scope) {
        // do nothing by default
        exit(pair, scope);
        postVisit(pair);
    }
    public void endVisit(MessageSend messageSend, BlockScope scope) {
        // do nothing by default
        exit(messageSend, scope);
        postVisit(messageSend);
    }
    public void endVisit(MethodDeclaration methodDeclaration, ClassScope scope) {
        // do nothing by default
        exit(methodDeclaration, scope);
        postVisit(methodDeclaration);
    }
    public void endVisit(StringLiteralConcatenation literal, BlockScope scope) {
        // do nothing by default
        exit(literal, scope);
        postVisit(literal);
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public void endVisit(NormalAnnotation annotation, BlockScope scope) {
        // do nothing by default
        exit(annotation, scope);
        postVisit(annotation);
    }
    public void endVisit(NullLiteral nullLiteral, BlockScope scope) {
        // do nothing by default
        exit(nullLiteral, scope);
        postVisit(nullLiteral);
    }
    public void endVisit(OR_OR_Expression or_or_Expression, BlockScope scope) {
        // do nothing by default
        exit(or_or_Expression, scope);
        postVisit(or_or_Expression);
    }
    public void endVisit(ParameterizedQualifiedTypeReference parameterizedQualifiedTypeReference, BlockScope scope) {
        // do nothing by default
        exit(parameterizedQualifiedTypeReference, scope);
        postVisit(parameterizedQualifiedTypeReference);
    }
    public void endVisit(ParameterizedQualifiedTypeReference parameterizedQualifiedTypeReference, ClassScope scope) {
        // do nothing by default
        exit(parameterizedQualifiedTypeReference, scope);
        postVisit(parameterizedQualifiedTypeReference);
    }
    public void endVisit(ParameterizedSingleTypeReference parameterizedSingleTypeReference, BlockScope scope) {
        // do nothing by default
        exit(parameterizedSingleTypeReference, scope);
        postVisit(parameterizedSingleTypeReference);
    }
    public void endVisit(ParameterizedSingleTypeReference parameterizedSingleTypeReference, ClassScope scope) {
        // do nothing by default
        exit(parameterizedSingleTypeReference, scope);
        postVisit(parameterizedSingleTypeReference);
    }
    public void endVisit(PostfixExpression postfixExpression, BlockScope scope) {
        // do nothing by default
        exit(postfixExpression, scope);
        postVisit(postfixExpression);
    }
    public void endVisit(PrefixExpression prefixExpression, BlockScope scope) {
        // do nothing by default
        exit(prefixExpression, scope);
        postVisit(prefixExpression);
    }
    public void endVisit(QualifiedAllocationExpression qualifiedAllocationExpression, BlockScope scope) {
        // do nothing by default
        exit(qualifiedAllocationExpression, scope);
        postVisit(qualifiedAllocationExpression);
    }
    public void endVisit(QualifiedNameReference qualifiedNameReference, BlockScope scope) {
        // do nothing by default
        exit(qualifiedNameReference, scope);
        postVisit(qualifiedNameReference);
    }
    public void endVisit(QualifiedNameReference qualifiedNameReference, ClassScope scope) {
        // do nothing by default
        exit(qualifiedNameReference, scope);
        postVisit(qualifiedNameReference);
    }
    public void endVisit(QualifiedSuperReference qualifiedSuperReference, BlockScope scope) {
        // do nothing by default
        exit(qualifiedSuperReference, scope);
        postVisit(qualifiedSuperReference);
    }
    public void endVisit(QualifiedSuperReference qualifiedSuperReference, ClassScope scope) {
        // do nothing by default
        exit(qualifiedSuperReference, scope);
        postVisit(qualifiedSuperReference);
    }
    public void endVisit(QualifiedThisReference qualifiedThisReference, BlockScope scope) {
        // do nothing by default
        exit(qualifiedThisReference, scope);
        postVisit(qualifiedThisReference);
    }
    public void endVisit(QualifiedThisReference qualifiedThisReference, ClassScope scope) {
        // do nothing by default
        exit(qualifiedThisReference, scope);
        postVisit(qualifiedThisReference);
    }
    public void endVisit(QualifiedTypeReference qualifiedTypeReference, BlockScope scope) {
        // do nothing by default
        exit(qualifiedTypeReference, scope);
        postVisit(qualifiedTypeReference);
    }
    public void endVisit(QualifiedTypeReference qualifiedTypeReference, ClassScope scope) {
        // do nothing by default
        exit(qualifiedTypeReference, scope);
        postVisit(qualifiedTypeReference);
    }
    public void endVisit(ReturnStatement returnStatement, BlockScope scope) {
        // do nothing by default
        exit(returnStatement, scope);
        postVisit(returnStatement);
    }
    /**
     * @param annotation
     * @param scope
     * @since 3.1
     */
    public void endVisit(SingleMemberAnnotation annotation, BlockScope scope) {
        // do nothing by default
        exit(annotation, scope);
        postVisit(annotation);
    }
    public void endVisit(SingleNameReference singleNameReference, BlockScope scope) {
        // do nothing by default
        exit(singleNameReference, scope);
        postVisit(singleNameReference);
    }
    public void endVisit(SingleNameReference singleNameReference, ClassScope scope) {
            // do nothing by default
        exit(singleNameReference, scope);
        postVisit(singleNameReference);
    }
    public void endVisit(SingleTypeReference singleTypeReference, BlockScope scope) {
        // do nothing by default
        exit(singleTypeReference, scope);
        postVisit(singleTypeReference);
    }
    public void endVisit(SingleTypeReference singleTypeReference, ClassScope scope) {
        // do nothing by default
        exit(singleTypeReference, scope);
        postVisit(singleTypeReference);
    }
    public void endVisit(StringLiteral stringLiteral, BlockScope scope) {
        // do nothing by default
        exit(stringLiteral, scope);
        postVisit(stringLiteral);
    }
    public void endVisit(SuperReference superReference, BlockScope scope) {
        // do nothing by default
        exit(superReference, scope);
        postVisit(superReference);
    }
    public void endVisit(SwitchStatement switchStatement, BlockScope scope) {
        // do nothing by default
        exit(switchStatement, scope);
        postVisit(switchStatement);
    }
    public void endVisit(SynchronizedStatement synchronizedStatement, BlockScope scope) {
        // do nothing by default
        exit(synchronizedStatement, scope);
        postVisit(synchronizedStatement);
    }
    public void endVisit(ThisReference thisReference, BlockScope scope) {
        // do nothing by default
        exit(thisReference, scope);
        postVisit(thisReference);
    }
    public void endVisit(ThisReference thisReference, ClassScope scope) {
        // do nothing by default
        exit(thisReference, scope);
        postVisit(thisReference);
    }
    public void endVisit(ThrowStatement throwStatement, BlockScope scope) {
        // do nothing by default
        exit(throwStatement, scope);
        postVisit(throwStatement);
    }
    public void endVisit(TrueLiteral trueLiteral, BlockScope scope) {
        // do nothing by default
        exit(trueLiteral, scope);
        postVisit(trueLiteral);
    }
    public void endVisit(TryStatement tryStatement, BlockScope scope) {
        // do nothing by default
        exit(tryStatement, scope);
        postVisit(tryStatement);
    }
    public void endVisit(TypeDeclaration localTypeDeclaration, BlockScope scope) {
        // do nothing by default
        exit(localTypeDeclaration, scope);
        postVisit(localTypeDeclaration);
    }
    public void endVisit(TypeDeclaration memberTypeDeclaration, ClassScope scope) {
        // do nothing by default
        exit(memberTypeDeclaration, scope);
        postVisit(memberTypeDeclaration);
    }
    public void endVisit(TypeDeclaration typeDeclaration, CompilationUnitScope scope) {
        // do nothing by default
        exit(typeDeclaration, scope);
        postVisit(typeDeclaration);
    }
    public void endVisit(TypeParameter typeParameter, BlockScope scope) {
        // do nothing by default
        exit(typeParameter, scope);
        postVisit(typeParameter);
    }
    public void endVisit(TypeParameter typeParameter, ClassScope scope) {
        // do nothing by default
        exit(typeParameter, scope);
        postVisit(typeParameter);
    }
    public void endVisit(UnaryExpression unaryExpression, BlockScope scope) {
        // do nothing by default
        exit(unaryExpression, scope);
        postVisit(unaryExpression);
    }
    public void endVisit(WhileStatement whileStatement, BlockScope scope) {
        // do nothing by default
        exit(whileStatement, scope);
        postVisit(whileStatement);
    }
    public void endVisit(Wildcard wildcard, BlockScope scope) {
        // do nothing by default
        exit(wildcard, scope);
        postVisit(wildcard);
    }
    public void endVisit(Wildcard wildcard, ClassScope scope) {
        // do nothing by default
        exit(wildcard, scope);
        postVisit(wildcard);
    }
}


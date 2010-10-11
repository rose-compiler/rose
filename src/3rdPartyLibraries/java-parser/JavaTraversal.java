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


class JavaTraversal  {
    static Main main;
    static BufferedWriter out;

    // -------------------------------------------------------------------------------------------
    /* tps: Invoke C Code , the int nr represents a unique nr for a node which is used for DOT representation*/
    private native void invokeINIT();
    private native void invokeEND ();
    private native void invokeNODE(String className, int nr);
    private native void invokeEDGE(String className1, int nr,String className2, int nr2);
    static {
	System.loadLibrary("JavaTraversal");
    }

    // -------------------------------------------------------------------------------------------
    /* tps: Creating a hashmap to keep track of nodes that we have already seen. Needed for DOT graph */
    static HashMap<Integer,Integer> hashm = new HashMap<Integer,Integer>();
    static int hashcounter=0;

    // -------------------------------------------------------------------------------------------
    /* tps: A class is represented as x.y.z.f.g --> this function returns the last string after the last dot */
    public String getLast(String s) {
	int index = s.lastIndexOf(".");
	if (index==-1) return s;
	return s.substring(index+1,s.length());
    } 

    // -------------------------------------------------------------------------------------------
    /* tps: For any Node we print its String representation. This function returns up to 30 characters of the String for DOT */
    public String getLine(ASTNode current) {
	int length=current.toString().length();
	int pos=current.toString().indexOf("\n");
	if (pos!=-1) length=pos-1;
	if (length>30) length=30;
	String substr = (current.toString()).substring(0,length); 
	return substr.replace("\"","'");
    } 

    // -------------------------------------------------------------------------------------------
    /* tps: Add nodes into a HashMap. We need this to connect edges between Nodes for DOT */
    public int getHashValue(ASTNode current) {
	int valc=0;
	Integer hashc = (Integer)hashm.get(current.hashCode()); 
	if (hashc!=null) {
	    valc = (hashc).intValue();
	} else {
	    valc=hashcounter;
	    hashm.put(current.hashCode(),hashcounter++);
	}
	return valc;
    }

    // -------------------------------------------------------------------------------------------
    /* tps : print to DOT   */
    public void printToDOT(ASTNode current) {
	ASTNode before=null;
	if (!stack.empty())
	    before = (ASTNode)stack.peek();
	try {
	    if (before==null) {
		//System.out.println("---------------------------------------------");
		//System.out.println("******** Root Node: "+getLast(current.getClass().toString())+"   Adding node to hashmap: "+hashcounter);
		int valc = getHashValue(current);
		out.write(" \""+valc+getLast(current.getClass().toString())+"\" [label=\""+getLast(current.getClass().toString())+"\\n"+getLine(current) +"\"];\n");
		invokeNODE(getLast(current.getClass().toString()), valc);
	    } else {
		//System.out.println("-----------------------");
		//System.out.println("New Node: "+getLast(current.getClass().toString()));
		//System.out.println("New Edge: "+getLast(before.getClass().toString())+" -- "+getLast(current.getClass().toString()));
		Integer valbint = ((Integer)hashm.get(before.hashCode()));
		if (valbint==null) { 
		    System.err.println("Error : hashvalue valbint == null for node :"+before.getClass()); 
		    System.exit(1); 
		}
		int valb = valbint.intValue();
		int valc = getHashValue(current);
		out.write(" \""+valc+""+getLast(current.getClass().toString())+"\" [label=\""+getLast(current.getClass().toString())+"\\n"+getLine(current) +"\"];\n");
		out.write("    \""+valb+""+getLast(before.getClass().toString())+"\" -> \""+valc+""+getLast(current.getClass().toString())+"\";\n");
		invokeEDGE(getLast(before.getClass().toString()),valb,getLast(current.getClass().toString()),valc);
		invokeNODE(getLast(current.getClass().toString()),valc);
	    }

	} catch (Exception e) { System.err.println("Error: " + e.getMessage()); }
    }

    // -------------------------------------------------------------------------------------------
    /* tps: Stack that is keeping track of the traversal we perform to connect children with parents in the DOT graph */
    private Stack<ASTNode> stack = new Stack<ASTNode>();
    public void pushNode(ASTNode node) {
	stack.push(node);
    }
    public ASTNode popNode() {
	if (!stack.empty())
	    stack.pop();
	else {
	    System.err.println("!!!!!!!!!!!! ERROR trying to access empty stack");
	    System.exit(1);
	}
	if (!stack.empty())
	    return (ASTNode)stack.peek();
	return null;
    }


    /* tps (10/08/10): AST traversal contains all current nodes of ECJ ---------------------------------------------- */
    public void traverseAST(CompilationUnitDeclaration unit) {

	final ASTVisitor visitor = new ASTVisitor() {
		public void endVisit(
				     AllocationExpression node,
				     BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(AND_AND_Expression  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(
				     AnnotationMethodDeclaration node,
				     ClassScope classScope) {
		    popNode();			// do nothing by default
		}
		public void endVisit(Argument  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(Argument  node,ClassScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(
				     ArrayAllocationExpression node,
				     BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(ArrayInitializer  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(
				     ArrayQualifiedTypeReference node,
				     BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(
				     ArrayQualifiedTypeReference node,
				     ClassScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(ArrayReference  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(ArrayTypeReference  node, BlockScope scope) {
		    popNode();		// do nothing by default
		}
		public void endVisit(ArrayTypeReference  node, ClassScope scope) {
		    popNode();		    // do nothing by default
		}
		public void endVisit(AssertStatement  node, BlockScope scope) {
		    popNode();// do nothing by default
		}
		public void endVisit(Assignment  node, BlockScope scope) {
		    popNode();// do nothing by default
		}
		public void endVisit(BinaryExpression  node, BlockScope scope) {
		    popNode();// do nothing by default
		}
		public void endVisit(Block  node, BlockScope scope) {
		    popNode();// do nothing by default
		}
		public void endVisit(BreakStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(CaseStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(CastExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(CharLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ClassLiteralAccess  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Clinit  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     CompilationUnitDeclaration node,
				     CompilationUnitScope scope) {
		    popNode();
		    // do nothing by default
		}
		public void endVisit(CompoundAssignment  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     ConditionalExpression node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     ConstructorDeclaration node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ContinueStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(DoStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(DoubleLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(EmptyStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(EqualExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     ExplicitConstructorCall node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     ExtendedStringLiteral node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FalseLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FieldDeclaration  node, MethodScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FieldReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FieldReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(FloatLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ForeachStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ForStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(IfStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ImportReference  node, CompilationUnitScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Initializer  node, MethodScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     InstanceOfExpression node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(IntLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Javadoc  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Javadoc  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocAllocationExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocAllocationExpression  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArgumentExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArgumentExpression  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArrayQualifiedTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArrayQualifiedTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArraySingleTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocArraySingleTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocFieldReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocFieldReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocImplicitTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocImplicitTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocMessageSend  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocMessageSend  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocQualifiedTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocQualifiedTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocReturnStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocReturnStatement  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocSingleNameReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocSingleNameReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocSingleTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(JavadocSingleTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(LabeledStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(LocalDeclaration  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(LongLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public void endVisit(MarkerAnnotation  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		/**
		 * @param pair
		 * @param scope
		 */
		public void endVisit(MemberValuePair  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(MessageSend  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(MethodDeclaration  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(StringLiteralConcatenation  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public void endVisit(NormalAnnotation  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(NullLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(OR_OR_Expression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ParameterizedQualifiedTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ParameterizedQualifiedTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ParameterizedSingleTypeReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ParameterizedSingleTypeReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(PostfixExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(PrefixExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedAllocationExpression node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedNameReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedNameReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedSuperReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedSuperReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedThisReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedThisReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedTypeReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     QualifiedTypeReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ReturnStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public void endVisit(SingleMemberAnnotation  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SingleNameReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SingleNameReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SingleTypeReference node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SingleTypeReference node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(StringLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(SuperReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(SwitchStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     SynchronizedStatement node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ThisReference  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ThisReference  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(ThrowStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(TrueLiteral  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(TryStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     TypeDeclaration node,
				     BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     TypeDeclaration node,
				     ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(
				     TypeDeclaration node,
				     CompilationUnitScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(TypeParameter  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(TypeParameter  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(UnaryExpression  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(WhileStatement  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Wildcard  node, BlockScope scope) {
		    popNode(); // do nothing  by default
		}
		public void endVisit(Wildcard  node, ClassScope scope) {
		    popNode(); // do nothing  by default
		}





		public boolean visit(
				     AllocationExpression node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(AND_AND_Expression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     AnnotationMethodDeclaration node,
				     ClassScope classScope) {
		    printToDOT(node); pushNode(node);
		    //		    node.traverse(this,node.scope);
		    return true; // do nothing by default, keep traversing
		}
		public boolean visit(Argument  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Argument  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ArrayAllocationExpression node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ArrayInitializer  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ArrayQualifiedTypeReference node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ArrayQualifiedTypeReference node,
				     ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ArrayReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ArrayTypeReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ArrayTypeReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(AssertStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Assignment  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(BinaryExpression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Block  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(BreakStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(CaseStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(CastExpression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(CharLiteral  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ClassLiteralAccess  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Clinit  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     CompilationUnitDeclaration node,
				     CompilationUnitScope scope) {
		    printToDOT(node); pushNode(node);
		    return true; // do nothing by default, keep traversing
		}
		public boolean visit(CompoundAssignment  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ConditionalExpression node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ConstructorDeclaration node,
				     ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ContinueStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(DoStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(DoubleLiteral  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(EmptyStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(EqualExpression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ExplicitConstructorCall node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     ExtendedStringLiteral node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FalseLiteral  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FieldDeclaration  node, MethodScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FieldReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FieldReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(FloatLiteral  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ForeachStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ForStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(IfStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ImportReference  node, CompilationUnitScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Initializer  node, MethodScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     InstanceOfExpression node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(IntLiteral  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Javadoc  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Javadoc  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocAllocationExpression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocAllocationExpression  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArgumentExpression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArgumentExpression  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArrayQualifiedTypeReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArrayQualifiedTypeReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArraySingleTypeReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocArraySingleTypeReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocFieldReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocFieldReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocImplicitTypeReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocImplicitTypeReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocMessageSend  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocMessageSend  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocQualifiedTypeReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocQualifiedTypeReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocReturnStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocReturnStatement  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocSingleNameReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocSingleNameReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocSingleTypeReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(JavadocSingleTypeReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(LabeledStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(LocalDeclaration  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(LongLiteral  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public boolean visit(MarkerAnnotation  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true;
		}
		/**
		 * @param pair
		 * @param scope
		 * @shashcountere 3.1
		 */
		public boolean visit(MemberValuePair  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true;
		}
		public boolean visit(MessageSend  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(MethodDeclaration  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     StringLiteralConcatenation node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public boolean visit(NormalAnnotation  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true;
		}
		public boolean visit(NullLiteral  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(OR_OR_Expression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ParameterizedQualifiedTypeReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ParameterizedQualifiedTypeReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ParameterizedSingleTypeReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ParameterizedSingleTypeReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(PostfixExpression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(PrefixExpression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedAllocationExpression node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedNameReference node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedNameReference node,
				     ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedSuperReference node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedSuperReference node,
				     ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedThisReference node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedThisReference node,
				     ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedTypeReference node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     QualifiedTypeReference node,
				     ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ReturnStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		/**
		 * @param annotation
		 * @param scope
		 * @shashcountere 3.1
		 */
		public boolean visit(SingleMemberAnnotation  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true;
		}
		public boolean visit(
				     SingleNameReference node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     SingleNameReference node,
				     ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     SingleTypeReference node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     SingleTypeReference node,
				     ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(StringLiteral  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(SuperReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(SwitchStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     SynchronizedStatement node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ThisReference  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ThisReference  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(ThrowStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(TrueLiteral  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(TryStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     TypeDeclaration node,
				     BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     TypeDeclaration node,
				     ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(
				     TypeDeclaration node,
				     CompilationUnitScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(TypeParameter  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(TypeParameter  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(UnaryExpression  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(WhileStatement  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Wildcard  node, BlockScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}
		public boolean visit(Wildcard  node, ClassScope scope) {
		    printToDOT(node); pushNode(node); return true; // do nothing by  node, keep traversing
		}

	    };
	unit.traverse(visitor,unit.scope);
    }



    public static void main(String args[])    {
	/* tps : set up and configure ---------------------------------------------- */
	System.out.println("Compiling ...");
	main = new Main(new PrintWriter(System.out), new PrintWriter(System.err), true/*systemExit*/,  null/*options*/, null/*progress*/);
	main.configure(args);
	FileSystem environment = main.getLibraryAccess();
	main.compilerOptions = new CompilerOptions(main.options);
	main.compilerOptions.performMethodsFullRecovery = false;
	main.compilerOptions.performStatementsRecovery = false;
	main.batchCompiler =  new Compiler(  environment,
					    main.getHandlingPolicy(),
					    main.compilerOptions,
					    main.getBatchRequestor(),
					    main.getProblemFactory(),
					    null,
					    main.progress);

	/* tps : handle compilation units--------------------------------------------- */
	ICompilationUnit[] sourceUnits = main.getCompilationUnits();
	int maxUnits = sourceUnits.length;
	main.batchCompiler.totalUnits = 0;
	main.batchCompiler.unitsToProcess = new CompilationUnitDeclaration[maxUnits];

	/* tps : Debug: print Compilation units --------------------------- */
	/*tps*/System.out.println("We got "+maxUnits+" compilation units");
	CompilationUnit[] units = main.getCompilationUnits();
	System.out.println("Nr of Compilation Units : "+units.length);
	for (CompilationUnit myunit : units) {
	    System.out.println("File : "+myunit);
	}


	/* tps : start parsing ------------------------------------------------------- */
	main.batchCompiler.internalBeginToCompile(sourceUnits, maxUnits);
	//main.batchCompiler.compile(sourceUnits);


	CompilationUnitDeclaration unit = null;
	JavaTraversal jt = new JavaTraversal();
	jt.invokeINIT();
	try { // writing to the DOT file
	    FileWriter fstream = new FileWriter("ast.dot");
	    out = new BufferedWriter(fstream);
	    out.write("Digraph G {\n");
	} catch (Exception e) { System.err.println("Error: " + e.getMessage()); }



	/* tps : compile the files and produce class files --------------------------- */
	ProcessTaskManager processingTask = null;
	for (int i = 0; i < main.batchCompiler.totalUnits; i++) {
	    unit = main.batchCompiler.unitsToProcess[i];
	    try {
		main.batchCompiler.process(unit, i);
		jt.traverseAST(unit); /*tps this is a better place for the traversal */
	    } finally {
		// cleanup compilation unit result
		unit.cleanUp();
	    }
	    main.batchCompiler.unitsToProcess[i] = null; // release reference to processed unit declaration
	    main.batchCompiler.stats.lineCount += unit.compilationResult.lineSeparatorPositions.length;
	    main.batchCompiler.requestor.acceptResult(unit.compilationResult.tagAsAccepted());
	}

	jt.invokeEND();
	try { // closing the DOT file
	    out.write("}\n");
	    out.close();
	} catch (Exception e) { System.err.println("Error: " + e.getMessage()); }


	System.out.println("Done compiling");
    }

}
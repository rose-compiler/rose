import org.eclipse.jdt.internal.compiler.ast.ASTNode;
import org.eclipse.jdt.internal.compiler.ast.AbstractMethodDeclaration;
import org.eclipse.jdt.internal.compiler.ast.CompilationUnitDeclaration;
import org.eclipse.jdt.internal.compiler.util.Util;

public class JavaSourcePositionInformationFactory {
    private int [] sepPos = null;
    private int lwBound = 0;
    private int upBound = 0;

    public JavaSourcePositionInformationFactory(CompilationUnitDeclaration cu) {
        this.sepPos = cu.compilationResult().getLineSeparatorPositions();
        this.lwBound = 0;
        this.upBound = sepPos.length-1;
    }
    
    public JavaSourcePositionInformation createPosInfo(AbstractMethodDeclaration node) {
        return createPosInfo(node.bodyStart, node.bodyEnd);
    }

    public JavaSourcePositionInformation createCompilerGeneratedPosInfo() {
        return createPosInfo(0, 0, 0, 0);
    }
    
    public JavaSourcePositionInformation createPosInfo(int startLine, int endLine, 
            int startColumn, int endColumn) {
        return new JavaSourcePositionInformation(startLine, endLine, startColumn, endColumn);
    }
    
    /**
     * Returns a JavaSourcePositionInformation object built from data found in ASTNode
     */
    public JavaSourcePositionInformation createPosInfo(ASTNode node) {
        // NOTE: This code could be improved by passing the line as an extra 
        // arg to columns related function to avoid computing line twice.  
        return createPosInfo(node.sourceStart(), node.sourceEnd());
    }

    public JavaSourcePositionInformation createPosInfo(int startPos, int endPos) {
        if (startPos == 0) {
            // ECJ starts numbering at '1', so this is probably compiler generated
            return createCompilerGeneratedPosInfo();
        } else{
            int startLine = getLine(startPos);
            int endLine = getLine(endPos);
            int startColumn = getColumn(startPos, startLine);
            int endColumn = getColumn(endPos, endLine);
            
            assert (startLine > -1);
            assert (endLine > -1);
            assert (startColumn > -1);
            assert (endColumn > -1);
            return createPosInfo(startLine, endLine, startColumn, endColumn);
        }
    }
    
    public int getStartLine(ASTNode node) {
        return getLine(node.sourceStart());
    }
    
    public int getEndLine(ASTNode node) {
        return getLine(node.sourceEnd());       
    }

    public int getStartColumn(ASTNode node) {
        return getColumn(node.sourceStart());
    }
    
    public int getEndColumn(ASTNode node) {
        return getColumn(node.sourceEnd());     
    }

    public int getLine(int pos) {
        return Util.getLineNumber(pos, this.sepPos, this.lwBound, this.upBound);
    }

    public int getColumn(int pos) {
        return getColumn(pos, getLine(pos));
    }

    public int getColumn(int pos, int line) {
        return Util.searchColumnNumber(this.sepPos, line, pos);
    }

}

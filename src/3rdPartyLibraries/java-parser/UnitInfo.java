import org.eclipse.jdt.internal.compiler.ast.ASTNode;
import org.eclipse.jdt.internal.compiler.ast.CompilationUnitDeclaration;

//
// The compilation unit we are processing.
//
public class UnitInfo {
    public CompilationUnitDeclaration unit;
    public String packageName;
    public String fileName;
    public JavaSourcePositionInformationFactory posFactory;
    private JavaToken defaultLocation;

    public JavaToken createJavaToken(ASTNode node) {
        //
        // DO NOT CHANGE !!!
        //
        // The reason for all this complicated code below is to bypass a bug in the following ECJ
        // function in the class JavaSourcePositionInformationFactory:
        //
        //         public JavaSourcePositionInformation createPosInfo(int startPos, int endPos) {
        //            if (startPos == 0) {
        //                // ECJ starts numbering at '1', so this is probably compiler generated
        //                return createCompilerGeneratedPosInfo();
        //            } else{
        //                int startLine = getLine(startPos);
        //                int endLine = getLine(endPos);
        //                int startColumn = getColumn(startPos, startLine);
        //                int endColumn = getColumn(endPos, endLine);
        //        
        //                assert (startLine > -1);
        //                assert (endLine > -1);
        //                assert (startColumn > -1);
        //                assert (endColumn > -1);
        //                return createPosInfo(startLine, endLine, startColumn, endColumn);
        //            }
        //         }
        //
        // Despite the promise made in the comment in the above function, the starting position of a
        // CompilationUnitDeclaration node appears to be 0.
        //
        JavaSourcePositionInformation pos = (node.sourceStart() == 0 && node.sourceEnd() == 0
                                                                      ? posFactory.createCompilerGeneratedPosInfo()
                                                                      : posFactory.createPosInfo(node.sourceStart() > 0 ? node.sourceStart() : 1, node.sourceEnd()));
        //System.out.println("The start position is " + node.sourceStart + "; the end position is " + node.sourceEnd + "; the returned starting line position is " + pos.getLineStart());
        JavaToken token = new JavaToken(fileName, pos);
        return token;
    }

    public JavaToken createJavaToken(ASTNode lnode, ASTNode rnode) {
        // 
        // DO NOT CHANGE !!!    See comments above
        //
        JavaSourcePositionInformation pos = (lnode.sourceStart() == 0 && rnode.sourceEnd() == 0
                                                                       ? posFactory.createCompilerGeneratedPosInfo()
                                                                       : posFactory.createPosInfo(lnode.sourceStart() > 0 ? lnode.sourceStart() : 1, rnode.sourceEnd()));
        // System.out.println("The start position is " + lnode.sourceStart + "; the end position is " + rnode.sourceEnd + "; the returned starting line position is " + pos.getLineStart());
        JavaToken token = new JavaToken(fileName, pos);
        return token;
    }
    
    JavaToken getDefaultLocation() {
        if (defaultLocation == null) {
            this.defaultLocation = new JavaToken(new String(unit.getFileName()), new JavaSourcePositionInformation(0));
        }
        return this.defaultLocation;
    }

    UnitInfo(CompilationUnitDeclaration unit, String package_name, String file_name, JavaSourcePositionInformationFactory pos_factory) {
        this.unit = unit;
        this.packageName = package_name;
        this.fileName = file_name;
        this.posFactory = pos_factory;
    }
}

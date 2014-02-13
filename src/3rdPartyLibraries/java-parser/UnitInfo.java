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
        JavaSourcePositionInformation pos = posFactory.createPosInfo(node);
        JavaToken token = new JavaToken(fileName, pos);
        return token;
    }

    public JavaToken createJavaToken(ASTNode lnode, ASTNode rnode) {
        JavaSourcePositionInformation pos = posFactory.createPosInfo(lnode.sourceStart(), rnode.sourceEnd());
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

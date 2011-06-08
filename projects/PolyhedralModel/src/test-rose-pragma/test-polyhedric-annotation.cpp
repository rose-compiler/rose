
#include "rose-pragma/PolyhedricPragma.hpp"

int main ( int argc, char* argv[] ) {
	SgProject * project = frontend ( argc , argv ) ;
	
	Rose_STL_Container<SgNode*> pragma_decls = NodeQuery::querySubTree(project, V_SgPragmaDeclaration);
	Rose_STL_Container<SgNode*>::iterator it;
	for (it = pragma_decls.begin(); it != pragma_decls.end(); it++) {
		SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(*it);
		
		try {
			PolyhedricAnnotation::parse(pragma_decl);
		}
		catch (Exception::ExceptionBase & e) {
			e.print(std::cerr);
			continue;
		}
		
		std::cout << PolyhedricAnnotation::getPolyhedralProgram<SgPragmaDeclaration, SgExprStatement, RoseVariable>(pragma_decl);
	}
	
	return 0;
}


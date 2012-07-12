
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
		
		std::vector<PolyhedricDependency::PragmaDependency *> * dependencies =
			PolyhedricDependency::ComputeDependencies<SgPragmaDeclaration, SgExprStatement, RoseVariable>(
				PolyhedricAnnotation::getPolyhedralProgram<SgPragmaDeclaration, SgExprStatement, RoseVariable>(pragma_decl)
			);
		
		std::vector<PolyhedricDependency::PragmaDependency *>::iterator it;
		for (it = dependencies->begin(); it != dependencies->end(); it++) {
			(*it)->print(std::cout);
			delete *it;
		}
		
		delete dependencies;
	}
	
	return 0;
}


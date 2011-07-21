
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
		
		PolyhedricAnnotation::PragmaPolyhedralProgram & polyhedral_program =
				PolyhedricAnnotation::getPolyhedralProgram<SgPragmaDeclaration, SgExprStatement, RoseVariable>(pragma_decl);
		
		Scheduling::PragmaSchedule schedule(polyhedral_program, 0);
		CodeGeneration::generateAndReplace(schedule);
	}
	
	project->unparse();
	
	return 0;
}



#include "RoseToFadaAST.hpp"

int main ( int argc, char* argv[] ) {
	SgProject * project = frontend ( argc , argv ) ;
	
	SgFilePtrList & file_list = project->get_fileList();
	SgFilePtrList::iterator it_file_list;
   	for (it_file_list = file_list.begin(); it_file_list != file_list.end(); it_file_list++) {
   	
		SgSourceFile * source_file = isSgSourceFile(*it_file_list);
		ROSE_ASSERT(source_file);
		
		SgDeclarationStatementPtrList & decl_list = source_file->get_globalScope()->get_declarations();
		SgDeclarationStatementPtrList::iterator it_decl_list;
		for (it_decl_list = decl_list.begin(); it_decl_list != decl_list.end(); it_decl_list++) {
		
			SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(*it_decl_list);
			if (!func_decl || func_decl->get_name().getString()[0] == '_') continue;
			
			fada::Program * pgm = new fada::Program();
			pgm->SetSyntaxTree(RoseToFada::parseSgFunctionDeclarationToFadaAST(func_decl));
			
			pgm->ComputeSourcesForAllReadVariables();
			
		}
	}
	
	return 0;
}

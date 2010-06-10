#include <rose.h>

#include <iostream>

int main(int argc, char ** argv) {

	ROSE_ASSERT(argc == 2);

	SgProject* project = frontend(argc, argv);
	ROSE_ASSERT(project != NULL);
	SgSourceFile * sfile = isSgSourceFile((*project)[0]);
	ROSE_ASSERT(sfile);
	
	SgDeclarationStatementPtrList & decl_list = sfile->get_globalScope()->get_declarations();
	SgDeclarationStatementPtrList::iterator it_decl_list;
	for (it_decl_list = decl_list.begin(); it_decl_list != decl_list.end(); it_decl_list++) {
		
		SgFunctionDeclaration * kernel = isSgFunctionDeclaration(*it_decl_list);
		if (kernel && kernel->get_name().getString() == "kernel") {
		
			kernel->get_functionModifier().setOpenclKernel();
			
			kernel->get_functionModifier().setOpenclVecTypeHint();
			kernel->get_functionModifier().set_opencl_vec_type(SageBuilder::buildFloatType());
			
			kernel->get_functionModifier().setOpenclWorkGroupSizeHint();
			kernel->get_functionModifier().setOpenclWorkGroupSizeReq();
			SgFunctionModifier::opencl_work_group_size_t vect = {1, 1, 1};
			kernel->get_functionModifier().set_opencl_work_group_size(vect);
		
		}
		
		SgVariableDeclaration * var = isSgVariableDeclaration(*it_decl_list);
		if (var && var->get_variables()[0]->get_name().getString() == "var") {
		
			var->get_declarationModifier().get_storageModifier().setOpenclGlobal();
			
		}
	}
	
	project->unparse();

	return 0;
}

int main( int argc, char * argv[] ) 
{
	// Build the AST used by ROSE
	project = frontend(argc,argv);
	
	cfgUtils::initCFGUtils(project);

	Rose_STL_Container<SgFunctionDefinition*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for(Rose_STL_Container<SgFunctionDefinition*>::iterator it = functions.begin(); it!=functions.end(); it++)
	{
		SgFunctionDefinition* func = *it;
		SgBasicBlock* funcBody = func->get_body();
		
		// look for all the pragmas in this function's body
		Rose_STL_Container<SgPragmaDeclaration*> pragmas = NodeQuery::querySubTree(funcBody, V_SgPragmaDeclaration);
		for(Rose_STL_Container<SgPragmaDeclaration*>::iterator it2 = pragmas.begin(); it2!=pragmas.end(); it2++)
		{
			
		}	
	}

	printf("========== D O N E ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}

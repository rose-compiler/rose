// clones a function definition, but also the corresponding declaration
SgFunctionDefinition* cloneFunctionDefinition(SgFunctionDefinition* originalFunDef, string prefix, string suffix) {
    SgFunctionDeclaration* originalFunDecl=originalFunDef->get_declaration();
    SgFunctionDeclaration* clonedFunDecl=isSgFunctionDeclaration(SageInterface::deepCopyNode(originalFunDecl));
    clonedFunDecl->set_name(SgName(prefix+clonedFunDecl->get_name().getString()+suffix));
    SgScopeStatement* originalFunctionParentScope=originalFunDecl->get_scope();
    SageInterface::appendStatement(clonedFunDecl, originalFunctionParentScope);
    SgFunctionDefinition* clonedFunDef=clonedFunDecl->get_definition();
    return clonedFunDef;
}


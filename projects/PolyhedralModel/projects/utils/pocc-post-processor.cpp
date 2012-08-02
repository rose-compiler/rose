
#include "rose-utils.hpp"

#include "rose.h"

int main(int argc, char ** argv) {
    char * args[3] = {argv[0], (char *)"-DPARAMETRIC", argv[1]};

    SgProject * project = frontend ( 3 , args );

    SgGlobal * global_scope = isSgSourceFile((*project)[0])->get_globalScope();

  // Scan all functions
    std::vector<SgNode*> func_decls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
    std::vector<SgNode*>::iterator it_;
    for (it_ = func_decls.begin(); it_ != func_decls.end(); it_++) {
        SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(*it_);

        // Look for the kernel function
        std::string name = func_decl->get_name().getString();
        if (name.compare(0, 7, "kernel_") != 0) continue;
        SgFunctionDefinition * func_def = func_decl->get_definition();
        if (func_def == NULL) continue;
        SgBasicBlock * func_body = func_def->get_body();
        if (func_body == NULL) continue;

        // Preprocessing to clean PoCC codes
        func_def->set_body(cleanPoCC(func_body));
    }

    project->unparse();

    return 0;
}


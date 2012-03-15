
#include "PolyhedralKernel.hpp"

int main ( int argc, char* argv[] ) {
    SgProject * project = frontend ( argc , argv ) ;

    std::vector<SgNode*> func_decls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
    std::vector<SgNode*>::iterator it;
    for (it = func_decls.begin(); it != func_decls.end(); it++) {
        SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(*it);

        try {
            if (!PolyhedricAnnotation::parse(func_decl, "kernel_"))
                continue;
        }
        catch (Exception::ExceptionBase & e) {
            e.print(std::cerr);
            continue;
        }

        std::cout << PolyhedricAnnotation::getPolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable>(func_decl);

        std::vector<PolyhedricDependency::FunctionDependency *> * dependencies =
//      PolyhedricDependency::ComputeDependencies<SgFunctionDeclaration, SgExprStatement, RoseVariable>(
        PolyhedricDependency::ComputeRaW<SgFunctionDeclaration, SgExprStatement, RoseVariable>(
                   PolyhedricAnnotation::getPolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable>(func_decl)
        );

        std::vector<PolyhedricDependency::FunctionDependency *>::iterator it;
        for (it = dependencies->begin(); it != dependencies->end(); it++) {
            (*it)->print(std::cout);
            delete *it;
        }

        delete dependencies;
    }

    return 0;
}


#include <rose.h>
#include <rosetollvm/RoseToLLVMModule.hpp>

int RoseToLLVMModule::visit(SgProject *project) {

    /**
     * Add required code generation attributes to Ast.
     */
    CodeAttributesVisitor attribute_visitor(*options, *control);
    attribute_visitor.traverseInputFiles(project);
    attribute_visitor.processRemainingComponents();

    /**
     * Revisit Ast to generate code.
     */
    CodeGeneratorVisitor generator_visitor(*options, *control);
    generator_visitor.traverseInputFiles(project); // generate code for function bodies
    generator_visitor.processRemainingFunctions();

    /**
     * Generate output if necessary.
     */
    control -> generateOutput();

    delete control; // release attributes, modules, ...
    control = new Control(*options);

    return 0;
}

int RoseToLLVMModule::queryVisit(SgProject *project, SgFunctionDeclaration *function_decl) {
    options -> setQuery();

    assert(function_decl);

    SgScopeStatement *scope = function_decl -> get_scope();
    SgGlobal *global = isSgGlobal(scope);
    assert(global);
    SgFile *file = isSgFile(global -> get_parent());
    assert(file);

    control -> SetAttribute(file, Control::LLVM_TRANSLATE);
    control -> SetAttribute(function_decl, Control::LLVM_TRANSLATE);
    visit(project);

    options -> resetQuery();
}

void RoseToLLVMModule::handleModuleOptions(Rose_STL_Container<string> &args) {
    options = new Option(args);
}

#include <rose.h>
#include <rosetollvm/RoseToLLVM.hpp>

const std::string RoseToLLVM::kTranslateExternals = "TRANSLATE_EXTERNALS";
/**
 * 
 */
SgFunctionDeclaration *RoseToLLVM::findEnclosingFunction(SgNode *node) {
    return (node == NULL || isSgProject(node)
                 ? (SgFunctionDeclaration *) NULL
                 : isSgFunctionDeclaration(node)
                       ? (SgFunctionDeclaration *) node
                       : findEnclosingFunction(node -> get_parent()));
}

/**
 * 
 */
SgFile *RoseToLLVM::findEnclosingFile(SgNode *node) {
    return (node == NULL || isSgProject(node)
                  ? (SgFile *) NULL
                  : isSgFile(node)
                        ? (SgFile *) node
                        : findEnclosingFile(node -> get_parent()));
}

/**
 * 
 */
SgProject *RoseToLLVM::findEnclosingProject(SgNode *node) {
    return (node == NULL || isSgProject(node)
                  ? (SgProject *) node
                  : findEnclosingProject(node -> get_parent()));
}

/**
 * 
 */
void RoseToLLVM::astVisit(SgProject *project) {

    bool translateExternal = project -> attributeExists(kTranslateExternals);

    /**
     * First, construct an attributes visitor to be used for attributing snippets of code.
     */
    CodeAttributesVisitor ad_hoc_attribute_visitor(*options, *control);
    control -> setAdHocAttributesVisitor(&ad_hoc_attribute_visitor);

    /**
     * Visit Ast to add required code generation attributes.
     */
    CodeAttributesVisitor attribute_visitor(*options, *control);
    if (translateExternal) {
        attribute_visitor.traverse(project);
    }
    else {
        attribute_visitor.traverseInputFiles(project);
    }
    attribute_visitor.processRemainingComponents();

    /**
     * Next, construct an generator visitor to be used for generating code for snippets.
     */
    CodeGeneratorVisitor ad_hoc_generator_visitor(*options, *control);
    control -> setAdHocGeneratorVisitor(&ad_hoc_generator_visitor);

    /**
     * Revisit Ast to generate code after attribute visit.
     */
    CodeGeneratorVisitor generator_visitor(*options, *control);
    generator_visitor.processDimensionExpressions(); // generate code for dimension expressions
    if (translateExternal) { // generate code for function bodies
        generator_visitor.traverse(project);
    }
    else {
        generator_visitor.traverseInputFiles(project);
    }
    generator_visitor.processRemainingFunctions();
}

/**
 * 
 */
llvm::Module *RoseToLLVM::translate(SgForStatement *for_stmt) {
    control -> SetAttribute(for_stmt, Control::LLVM_COST_ANALYSIS);
    SgFunctionDeclaration *function_decl = findEnclosingFunction(for_stmt);
    return translate(function_decl, false);
}

/**
 *
 */
llvm::Module *RoseToLLVM::translate(SgFunctionDeclaration *function_decl, bool translate_whole_function) {
    options -> setQuery();
    if (translate_whole_function) {
        options -> setTranslating();
    }

    ROSE2LLVM_ASSERT(function_decl);
    SgFile *file = findEnclosingFile(function_decl);
    ROSE2LLVM_ASSERT(file);
    SgProject *project = findEnclosingProject(file);
    ROSE2LLVM_ASSERT(project);

    /**
     * Mark the only file and function (contained in the file in question) to be translated.
     */
    control -> SetAttribute(file, Control::LLVM_TRANSLATE);
    control -> SetAttribute(function_decl, Control::LLVM_TRANSLATE);

    std::vector<llvm::Module *> res = translate(project);
    ROSE2LLVM_ASSERT(res.size() == 1);

    options -> resetQuery();
    if (translate_whole_function) {
      options -> resetTranslating();
    }

    if (options -> isDebugOutput()) {
      // No modules are created in this case, and debug output has
      // already been produced, so exit.
      exit(EXIT_SUCCESS);
    }
    return res[0];
}

/**
 *
 */
std::vector<llvm::Module *> RoseToLLVM::translate(SgProject *project) {
    astVisit(project);

    /**
     * Generate output if necessary.
     */
    std::vector<llvm::Module *> res = control -> generateOutput();

    delete control; // release attributes, modules, ...
    control = new Control(*options);

    return res;
}

/**
 *
 */
void RoseToLLVM::addOptionsToDescription(boost::program_options::options_description& desc) {
    Option::addOptionsToDescription(desc);
}

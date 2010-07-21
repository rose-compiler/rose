#include <rose.h>
#include <AliasAnalysisModule.h>
#include <AnnotateAST.h>
#include <rosetollvm/Control.h>
#include <llvm/Module.h>

int AliasAnalysisModule::visit(SgProject *project)
{
    /**
     * Add attributes to generate code
     */
    CodeAttributesVisitor attributeVisitor(*options, *control);
    attributeVisitor.traverseInputFiles(project);
    attributeVisitor.processRemainingComponents();

    /**
     * Generate the Code
     */
    CodeGeneratorVisitor generatorVisitor(*options, *control);
    generatorVisitor.traverseInputFiles(project);
    generatorVisitor.processRemainingFunctions();

    control->generateModules();

    return 0;
}

int AliasAnalysisModule::annotateAST(SgProject *project)
{
    AnnotateAST annotate(*options, *control);
    annotate.traverseInputFiles(project);

    return 0;
}

llvm::Module* AliasAnalysisModule::getModule(int index)
{
    assert(control != NULL);
    return control->getModuleRef(index);
}

void AliasAnalysisModule::handleModuleOptions(Rose_STL_Container<string> &args) 
{
    options = new Option(args);
}

int AliasAnalysisModule::getLLVMModuleSize()
{
    assert(control != NULL);
    return control->getLLVMModuleSize();
}

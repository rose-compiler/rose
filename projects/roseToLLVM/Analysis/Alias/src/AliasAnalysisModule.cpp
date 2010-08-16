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

int AliasAnalysisModule::annotateAST(SgNode *astRoot)
{
    /*
     * query all function nodes
     */

    Rose_STL_Container<SgNode*> _functionlist = NodeQuery::querySubTree(astRoot, V_SgFunctionDeclaration);
    Rose_STL_Container<SgNode*> _translatedfunctions = NodeQuery::queryNodeList(_functionlist, &querySolverTranslatedFunctions);

    Rose_STL_Container<SgNode*>::iterator I;

    /*
     * Function traversal to associate ROSE AST with LLVM Attributes
     */
   
    AnnotateAST *_annotateAST = new AnnotateAST(*options, *control) ;
    for(I = _translatedfunctions.begin(); I != _translatedfunctions.end(); ++I) {
        SgFunctionDeclaration *_fn_decl = isSgFunctionDeclaration(*I);

        ROSE_ASSERT(_fn_decl != NULL);

        std::string _modulename = AliasSetHandler::getInstance()->getModuleName(_fn_decl->get_name());

        ROSE_ASSERT(!_modulename.empty());

        _annotateAST->setFunctionName(_fn_decl->get_name().str());

        _annotateAST->findContainer(_modulename);

        _annotateAST->traverse(_fn_decl);
    }

    delete _annotateAST;
    return 0;
}

int AliasAnalysisModule::queryAST(SgNode *astRoot)
{
    /*
     * query all function nodes
     */

    Rose_STL_Container<SgNode*> _functionlist = NodeQuery::querySubTree(astRoot, V_SgFunctionDeclaration);
    Rose_STL_Container<SgNode*> _translatedfunctions = NodeQuery::queryNodeList(_functionlist, &querySolverTranslatedFunctions);

    Rose_STL_Container<SgNode*>::iterator I;

    /*
     * Function traversal to associate ROSE AST with LLVM Attributes
     */
   
    QueryAST *_queryAST = new QueryAST(*options, *control) ;
    for(I = _translatedfunctions.begin(); I != _translatedfunctions.end(); ++I) {
        SgFunctionDeclaration *_fn_decl = isSgFunctionDeclaration(*I);

        ROSE_ASSERT(_fn_decl != NULL);

        std::string _modulename = AliasSetHandler::getInstance()->getModuleName(_fn_decl->get_name());

        ROSE_ASSERT(!_modulename.empty());

        _queryAST->setFunctionName(_fn_decl->get_name().str());

        _queryAST->findContainer(_modulename);

        _queryAST->traverse(_fn_decl);
    }

    delete _queryAST;
    return 0;
}


int AliasAnalysisModule::associateAST(SgNode *astRoot)
{
    /*
     * query all function nodes
     */

    Rose_STL_Container<SgNode*> _functionlist = NodeQuery::querySubTree(astRoot, V_SgFunctionDeclaration);
    Rose_STL_Container<SgNode*> _translatedfunctions = NodeQuery::queryNodeList(_functionlist, &querySolverTranslatedFunctions);

    Rose_STL_Container<SgNode*>::iterator I;

    /*
     * Function traversal to associate ROSE AST with LLVM Attributes
     */
   
    AssociateRoseAST *_associateAST = new AssociateRoseAST(*options, *control) ;
    for(I = _translatedfunctions.begin(); I != _translatedfunctions.end(); ++I) {
        SgFunctionDeclaration *_fn_decl = isSgFunctionDeclaration(*I);

        ROSE_ASSERT(_fn_decl != NULL);

        std::string _modulename = AliasSetHandler::getInstance()->getModuleName(_fn_decl->get_name());

        ROSE_ASSERT(!_modulename.empty());

        _associateAST->setFunctionName(_fn_decl->get_name().str());

        _associateAST->findContainer(_modulename);

        _associateAST->traverse(_fn_decl);
    }

    delete _associateAST;
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

NodeQuerySynthesizedAttributeType querySolverTranslatedFunctions(SgNode *node)
{
    NodeQuerySynthesizedAttributeType _nodelist;

    SgFunctionDeclaration *_fndecl = isSgFunctionDeclaration(node);

    if(_fndecl->attributeExists(Control::LLVM_LOCAL_DECLARATIONS)) {
        _nodelist.push_back(node);
    }

    return _nodelist;
}


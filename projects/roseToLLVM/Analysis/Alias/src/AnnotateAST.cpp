#include <AnnotateAST.h>
#include <string>
#include <rosetollvm/Control.h>


const char *AnnotateAST::ALIAS_SET = "ROSE_ALIAS_SET";

void AnnotateAST::findContainer(std::string _modulename)
{
    AliasSetContainerList *list = AliasSetHandler::getInstance()->getAliasSetContainerList(_modulename);
    _container = list->getContainer(_functionname);

    ROSE_ASSERT(_container != NULL);
}

void AnnotateAST::setFunctionName(std::string _function)
{
    _functionname = _function;
}

void AnnotateAST::preOrderVisit(SgNode *node)
{

}

void AnnotateAST::postOrderVisit(SgNode *node)
{
    int size = _container->getSize();

    std::set<SgNode*> diff;
    std::set<SgNode*> singleton;

    for(int i = 0; i < size; ++i) {
        if(_container->isSgNodePresent(i, node)) {
            singleton.insert(node);
            const std::set<SgNode*> &ref = _container->getSgNodeSet(i);
            std::set_difference(ref.begin(), ref.end(), singleton.begin(), singleton.end(), std::inserter(diff, diff.end())); 
            AliasType type = _container->getAliasType(i);
            AliasDataSet::RoseAliasSet _rosealiasset = std::make_pair(type, diff);
            AliasSetAstAttribute *attribute = new AliasSetAstAttribute(_rosealiasset);
            control.SetAttribute(node, AnnotateAST::ALIAS_SET, attribute); 
           }
    }

//    std::cout << "AST Annotated\n";
}

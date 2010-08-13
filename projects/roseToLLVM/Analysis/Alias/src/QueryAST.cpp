#include <QueryAST.h>
#include <string>
#include <rosetollvm/Control.h>


const char *QueryAST::SKIP_NODE = "SKIP_AST";

void QueryAST::findContainer(std::string _modulename)
{
    AliasSetContainerList *list = AliasSetHandler::getInstance()->getAliasSetContainerList(_modulename);
    _container = list->getContainer(_functionname);

    ROSE_ASSERT(_container != NULL);
}

void QueryAST::setFunctionName(std::string _function)
{
    _functionname = _function;
}

void QueryAST::preOrderVisit(SgNode *node)
{

}

void QueryAST::postOrderVisit(SgNode *node)
{
    if(node->attributeExists(AnnotateAST::ALIAS_SET)) {
        std::cout << node->class_name() << std::endl;

        if(node->attributeExists(QueryAST::SKIP_NODE)) {
            // do nothing
        }
        else {
            // unparse the set

        }
                
    }
}


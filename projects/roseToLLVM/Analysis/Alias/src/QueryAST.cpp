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

        if(node->attributeExists(QueryAST::SKIP_NODE)) {
            // do nothing
        }
        else {
            AliasSetAstAttribute *attribute = (AliasSetAstAttribute*) node->getAttribute(AnnotateAST::ALIAS_SET);
            AliasDataSet::RoseAliasSet ref = attribute->getAliasSet();
            AliasType _type = ref.first;
            AliasDataSet::AliasNodes &set = ref.second;

            std::set<SgNode*>::iterator it;

           QueryASTData::_Locations  _locations;
            _locations.insert(node->unparseToString());

            for(it=set.begin(); it != set.end(); ++it){
                SgNode *_element = *it;
                ROSE_ASSERT(_element != NULL);
                _locations.insert(_element->unparseToString());
                SkipNodeAttribute *attribute = new SkipNodeAttribute(true);
                control.SetAttribute(_element, QueryAST::SKIP_NODE, attribute);
            }

            QueryASTData::_Set _newset = std::make_pair(_type, _locations);
            _aliasSetList.push_back(_newset);
        }
    }
}

void QueryAST::atTraversalEnd()
{
    QueryASTData::_List::iterator it;
    QueryASTData::_Locations::const_iterator sit;

    for(it = _aliasSetList.begin(); it != _aliasSetList.end(); ++it) {
        QueryASTData::_Set myset = *it;
        AliasType _type = myset.first;
        QueryASTData::_Locations &_tempset = myset.second;
        std::cout << "{";
        for(sit = _tempset.begin(); sit != _tempset.end(); ++sit) {
            std::cout << *sit << ",";
        }
        std::cout << "}" << std::endl;
    }
}


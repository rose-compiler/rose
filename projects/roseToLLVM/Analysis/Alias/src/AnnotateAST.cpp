#include <AnnotateAST.h>
#include <string>
#include <rosetollvm/Control.h>

void AnnotateAST::preOrderVisit(SgNode *node)
{

}

void AnnotateAST::postOrderVisit(SgNode *node)
{
    if(isSgFunctionDeclaration(node) != NULL) {
        SgFunctionDeclaration *function= dynamic_cast<SgFunctionDeclaration*> (node);
        if(function->attributeExists(Control::LLVM_REFERENCE_NAME))
            std::cout << ((StringAstAttribute*)function->getAttribute(Control::LLVM_REFERENCE_NAME))->getValue() << std::endl;

        SgFunctionDefinition *function_def = function->get_definition();
        Rose_STL_Container<SgNode*> pointerList = NodeQuery::querySubTree(function_def, V_SgPointerDerefExp);

        assert(pointerList.size() > 0);        

        for(Rose_STL_Container<SgNode*>::iterator iter = pointerList.begin(); iter != pointerList.end(); ++iter) {
            SgPointerDerefExp *ptrExp = dynamic_cast<SgPointerDerefExp*> (*iter);
            assert(ptrExp != NULL);
            std::cout << ((StringAstAttribute*)ptrExp->getAttribute(Control::LLVM_REFERENCE_NAME))->getValue() << std::endl; 
        }
    }

}

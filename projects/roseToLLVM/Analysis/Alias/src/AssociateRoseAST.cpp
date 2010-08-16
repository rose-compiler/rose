#include <AssociateRoseAST.h>

void AssociateRoseAST::findContainer(std::string _modulename)
{
    AliasSetContainerList *list = AliasSetHandler::getInstance()->getAliasSetContainerList(_modulename);
    _container = list->getContainer(_functionname);

    ROSE_ASSERT(_container != NULL);
    _container->initSets();
}

void AssociateRoseAST::setFunctionName(std::string _function)
{
    _functionname = _function;
}

void AssociateRoseAST::postOrderVisit(SgNode *node)
{
    // pointer deref
    // array deref
    // varref
    //

    SgPointerDerefExp *ptrDeRef;
    SgPntrArrRefExp *pntrArrRef;
    SgInitializedName *varRef;
    std::string value;

    if(isSgPointerDerefExp(node)) {
        ptrDeRef = isSgPointerDerefExp(node);
        ROSE_ASSERT(ptrDeRef != NULL);
        if(ptrDeRef->attributeExists(Control::LLVM_REFERENCE_NAME)) {
            value = ((StringAstAttribute*)ptrDeRef->getAttribute(Control::LLVM_REFERENCE_NAME))->getValue();
        }
    }

    else if(isSgPntrArrRefExp(node)) {
        pntrArrRef = isSgPntrArrRefExp(node);
        ROSE_ASSERT(pntrArrRef != NULL);
        if(pntrArrRef->attributeExists(Control::LLVM_REFERENCE_NAME)) {
            value = ((StringAstAttribute*)pntrArrRef->getAttribute(Control::LLVM_REFERENCE_NAME))->getValue();
        }
    }
    else if(isSgInitializedName(node)) {
        varRef = isSgInitializedName(node);
        ROSE_ASSERT(varRef != NULL);
        if(varRef->attributeExists(Control::LLVM_NAME)) {
            value = ((StringAstAttribute*)varRef->getAttribute(Control::LLVM_NAME))->getValue();
        }
    }
    else {
        //
    }

    int size = _container->getSize();

    for(int i = 0; i < size; ++i) {
        if(_container->isPresent(i, value)) {
            _container->addSgNode(i, node);
            break;                                          // Alias Sets are disjoint
        }
    }
}

void AssociateRoseAST::preOrderVisit(SgNode *node)
{
    // post order visit
}

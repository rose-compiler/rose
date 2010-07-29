#include <AliasSetHandler.h>

AliasSetHandler* AliasSetHandler::_handler = NULL;

void AliasSetHandler::addnew(std::string modulename)
{
    AliasSetContainerList *list = new AliasSetContainerList(modulename);
    _mlist[modulename] = list;
}

AliasSetHandler::~AliasSetHandler()
{
    std::map<std::string, AliasSetContainerList*>::iterator I;
    for(I = _mlist.begin(); I != _mlist.end(); ++I) {
        delete I->second;
    }

    delete _handler;
}

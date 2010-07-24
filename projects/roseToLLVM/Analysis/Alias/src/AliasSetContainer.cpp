#include <AliasSetContainer.h>
#include <map>
#include <string>

AliasSetContainer::AliasSetContainer(std::string _fn_name)
{
    _functionname = _fn_name;
}

void AliasSetContainer::addaliasinglocations(std::string _locations)
{
    _aliaslocations = _locations;
}

AliasSetContainerList::AliasSetContainerList(std::string _module)
{
    _modulename = _module;
}

void AliasSetContainerList::addNew(std::string _functionname ) 
{
    AliasSetContainer *_ascontainer = new AliasSetContainer(_functionname);
    _list[_functionname] = _ascontainer;
//  _list.insert(std::make_pair(_functionname, _ascontainer));            
}
        
AliasSetContainer* AliasSetContainerList::getContainer(std::string _functionname )
{
    AliasSetContainer *ptr = _list.find(_functionname)->second;
    return ptr;                  
}

AliasSetContainerList:: ~AliasSetContainerList()
{
    // Cleanup
    std::map<std::string, AliasSetContainer*>::iterator I;
    for(I = _list.begin(); I != _list.end(); ++I) {
        delete I->second;
    }
       
    _list.erase(_list.begin(), _list.end());
}     


#include <AliasSetContainer.h>
#include <map>
#include <string>
#include <iostream>


/*
 * Defintion for AliasSetContainer
 */

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

void AliasSetContainer::print()
{
    std::cout << _aliaslocations << std::endl;
}

void AliasSetContainer::parseAliasSet()
{
    assert( !_aliaslocations.empty() );
    
    std::vector<std::string> _allSetsinThisFunc ;

    boost::split (_allSetsinThisFunc, _aliaslocations, boost::is_any_of("\n"));

    std::vector<std::string>::iterator I;

    for( I = _allSetsinThisFunc.begin(); I != _allSetsinThisFunc.end(); ++I) {
        std::cout << I->data() << std::endl;
                          
    }
}

/*
 * Definition for AliasSetContainerList
 */

void AliasSetContainerList::addNewFunction(std::string _functionname ) 
{
    AliasSetContainer *_ascontainer = new AliasSetContainer(_functionname);
    _list[_functionname] = _ascontainer;
//  _list.insert(std::make_pair(_functionname, _ascontainer));            
}
        
AliasSetContainer* AliasSetContainerList::getContainer(std::string _functionname )
{
    return _list.find(_functionname)->second;
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

void AliasSetContainerList::parseAliasSet()
{
    std::map<std::string, AliasSetContainer*>::iterator I;
    for(I = _list.begin(); I != _list.end(); ++I) {
        std::cout << I->first << std::endl;
        I->second->parseAliasSet();
        std::cout << std::endl;
    }
}

void AliasSetContainerList::print()
{
    std::map<std::string, AliasSetContainer*>::iterator I;
    for(I = _list.begin(); I != _list.end(); ++I) {
        I->second->print();
    }
}


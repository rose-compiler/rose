#ifndef _ALIASSETCONTAINER_H
#define _ALIASSETCONTAINER_H

#include <set>
#include <vector>
#include <map>
#include <llvm/Function.h>
#include <llvm/Module.h>

class AliasSetContainer
{
    std::string _functionname;       
    std::string _aliaslocations;
    
    public:
    AliasSetContainer(std::string _fn_name );
    
    void addaliasinglocations(std::string _alias);        


};

class AliasSetContainerList
{
    std::string _modulename;
    std::map<std::string, AliasSetContainer*>  _list;

    public:
        AliasSetContainerList(std::string _module_name);

        void addNew(std::string _functionname ) ;

        AliasSetContainer* getContainer(std::string _functionname );

        ~AliasSetContainerList();
};

#endif

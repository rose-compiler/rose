#ifndef _ALIASSETHANDLER_H
#define _ALIASSETHANDLER_H

#include <AliasSetContainer.h>

/*
 * - Singleton class that manages AliasSetContainer
 * - LLVM Pass can write results using this handler
 * - Result can be later read through this handler to annotate the AST
 */

class AliasSetHandler
{
    private:
        AliasSetHandler();
        AliasSetHandler(AliasSetHandler const &);
        AliasSetHandler& operator = (AliasSetHandler const &);

        static AliasSetHandler* _handler;

        // Create a AliasSetContainerList for each module
        std::map<std::string, AliasSetContainerList*> _mlist;
        

    public:        
        static AliasSetHandler* getInstance()            
        {
            if(!_handler)
                _handler = new AliasSetHandler();
            return _handler;
        }
           
        ~AliasSetHandler() 
        {
            delete _handler;
        }

        void addnew(std::string modulename);
};


#endif

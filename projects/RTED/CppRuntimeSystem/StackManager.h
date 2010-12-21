
#ifndef STACKMANAGER_H
#define STACKMANAGER_H

#include "Util.h"

#include <map>
#include <vector>
#include <iostream>

#include "ptrops.h"

class VariablesType;


class StackManager
{
    public:
        StackManager();
        ~StackManager() {}


        void addVariable(VariablesType * var);

        /// Each variable is associated with a scope, use this function to create a new scope
        /// @param name  string description of scope, may be function name or "for-loop" ...
        void beginScope(const std::string& name);

        /// Closes a scope and deletes all variables which where created via registerVariable()
        /// from the stack, tests for
        void endScope ();


        // Access to variables/scopes
        int                 getScopeCount()     const;
        const std::string & getScopeName(int i) const;

        typedef std::vector<VariablesType*>::const_iterator VariableIter;
        VariableIter variablesBegin(int scopeId) const;
        VariableIter variablesEnd(int scopeId)   const;


        /// Returns variable at given memory location, or NULL if no var found
        VariablesType * getVariable(MemoryAddress);

        /// Returns Variable by mangledName (which is unique)
        /// much slower thant the version with address!, does linear search in stack
        VariablesType * getVariable(const std::string & mangledName);

        /// Finds all variables with given name, and puts them in a vector
        /// (only mangled name is unique! )
        void getVariableByName(const std::string & name, std::vector<VariablesType*> & result);


        void print(std::ostream & os) const;

        void clearStatus();

    protected:

        struct ScopeInfo
        {
            ScopeInfo( const std::string & _name, int index)
                : name(_name),stackIndex(index)
            {}

            std::string name;        /// description of scope, either function-name or something like "for-loop"
            int         stackIndex;  /// index in stack-array where this scope starts
        };
        std::vector<ScopeInfo> scope;

        std::vector<VariablesType *> stack;

        typedef std::map<MemoryAddress, VariablesType*> AddrToVarMap;
        AddrToVarMap addrToVarMap;
};




#endif


#ifndef STACKMANAGER_H
#define STACKMANAGER_H

#include <map>
#include <vector>
#include <iosfwd>

#include "Util.h"
#include "rted_typedefs.h"

class VariablesType;
class RsType;

struct ScopeInfo
{
    ScopeInfo(const char* _name, int index)
    : name(_name), stackIndex(index)
    {}

    const char* name;        /// description of scope, either function-name or something like "for-loop"
    size_t      stackIndex;  /// index in stack-array where this scope starts
};


struct StackManager
{
        typedef Address                                      Location;
        typedef std::vector<VariablesType>                   VariableStack;
        typedef std::vector<ScopeInfo>                       ScopeStack;

        //~ enum { immediate = false, delay = true };

        StackManager();

        void addVariable(Location address, const char* name, const char* mangledName, const RsType& t, long blocksize);

        /// \brief Each variable is associated with a scope, use this function to create a new scope
        /// \param name  string description of scope, may be function name or "for-loop" ...
        void beginScope(const char* name);

        /// \brief Closes a scope and deletes all variables which where created
        ///        via registerVariable() from the stack, testing for memory
        ///        leaks (@ref registerPointerChange).
        /// \param scopecount number of scopes to close (e.g., return from inner block)
        void endScope(size_t scopecount);

        // Access to variables/scopes
        int                getScopeCount()     const;
        std::string        getScopeName(size_t i) const;

        VariableStack::const_iterator variablesBegin(size_t scopeId) const;
        VariableStack::const_iterator variablesEnd(size_t scopeId)   const;

        /// Returns variable at given memory location, or NULL if no var found
        const VariablesType* getVariable(Location) const;

        /// Returns Variable by mangledName (which is unique)
        /// much slower thant the version with address!, does linear search in stack
        const VariablesType* getVariableByMangledName(const std::string& mangledName) const;

        void print(std::ostream& os) const;

        void clearStatus();

    protected:
        ScopeStack                   scope;
        VariableStack                stack;
};

#endif

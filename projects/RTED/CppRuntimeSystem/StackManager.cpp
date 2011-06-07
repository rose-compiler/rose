#include "StackManager.h"

#include "VariablesType.h"
#include "CppRuntimeSystem.h"

using namespace std;

StackManager::StackManager()
{
    beginScope("Globals");
}

void StackManager::addVariable(VariablesType* var, long blocksize)
{
    assert(scope.size() > 0);
    // if the variable is (or contains) pointer register it to pointer manager

    const RsType* vartype = var->getType();
    const Address varaddr = var->getAddress();

    rtedRTS(this)->getPointerManager()->createPointer(varaddr, vartype, blocksize);

    addrToVarMap.insert(AddrToVarMap::value_type(varaddr, var));
    stack.push_back(var);
}



const VariablesType* StackManager::getVariable(Location addr) const
{
    AddrToVarMap::const_iterator it = addrToVarMap.find(addr);

    if (it == addrToVarMap.end()) return NULL;

    return it->second;
}

VariablesType * StackManager::getVariableByMangledName(const string& mangledName)
{
    for (size_t i=0; i <  stack.size(); i++)
    {
        if (stack[i]->getMangledName() == mangledName)
            return stack[i];
    }
    return NULL;
}

void StackManager::getVariableByName(const std::string& name, vector<VariablesType*> & result)
{
    for (size_t i=0; i < stack.size(); ++i)
    {
        if(stack[i]->getName() == name)
            result.push_back(stack[i]);
    }
}



void StackManager::beginScope(const std::string & name)
{
    scope.push_back(ScopeInfo(name,stack.size()));
}

void StackManager::endScope()
{
    const size_t noScopes = scope.size();

    assert( noScopes > 0 );

    ScopeInfo lastScope = scope.back();

    scope.pop_back();

    for (int i=stack.size()-1; i >= lastScope.stackIndex; --i)
    {
        VariablesType* var = stack.back();
        const Address  varaddr = var->getAddress();

        stack.pop_back();
        addrToVarMap.erase(varaddr);
        rtedRTS(this)->getMemManager()->freeStackMemory(varaddr);

        delete var;
    }

    assert((int)stack.size() == lastScope.stackIndex);
}

int StackManager::getScopeCount()  const
{
    return scope.size();
}

const std::string & StackManager::getScopeName(int i) const
{
    assert(i >=0 );
    assert(i < (int)scope.size());
    return scope[i].name;
}

StackManager::VariableStack::const_iterator
StackManager::variablesBegin(int i) const
{
    assert(i >=0 );
    assert(i < (int)scope.size());

    return stack.begin() + scope[i].stackIndex;
}

StackManager::VariableStack::const_iterator
StackManager::variablesEnd(int i) const
{
    assert(i >=0 );
    assert(i < (int)scope.size());

    if(i+1 == (int)scope.size())
        return stack.end();
    else
        return stack.begin() + scope[i+1].stackIndex;
}


void StackManager::clearStatus()
{
    while(scope.size() > 0)
        endScope();

    assert(stack.size() ==0);
    assert(addrToVarMap.size() == 0);

    beginScope("Globals");
}


void StackManager::print(ostream & os) const
{
    os << endl;
    os << "------------------------------- Stack Status --------------------------------------" << endl << endl;

    for (unsigned int sc=0; sc < scope.size(); sc++)
    {
        os << scope[sc].name << ":" << endl;

        for (VariableStack::const_iterator i = variablesBegin(sc); i != variablesEnd(sc); ++i)
             os << "\t" << **i << endl;

    }

    os << endl;
}

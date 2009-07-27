#include "StackManager.h"

#include "VariablesType.h"
#include "CppRuntimeSystem.h"

using namespace std;

StackManager::StackManager()
{
    beginScope("Globals");
}

void StackManager::addVariable(VariablesType * var)
{
    assert(scope.size() > 0);
    // if the variable is (or contains) pointer register it to pointer manager
    RuntimeSystem::instance()->getPointerManager()->createPointer(var->getAddress(),var->getType());

    addrToVarMap.insert(AddrToVarMap::value_type(var->getAddress(),var));
    stack.push_back(var);
}



VariablesType * StackManager::getVariable(addr_type addr)
{
    AddrToVarMap::iterator it = addrToVarMap.find(addr);
    if(it == addrToVarMap.end())
        return NULL;
    else
        return it->second;
}

VariablesType * StackManager::getVariable(const string & mangledName)
{
    for(int i=0; i <  stack.size(); i++)
    {
        if(stack[i]->getMangledName() == mangledName)
            return stack[i];
    }
    return NULL;
}

void StackManager::getVariableByName(const std::string & name, vector<VariablesType*> & result)
{
    for(int i=0; i <  stack.size(); i++)
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
    assert( scope.size() > 0);

    ScopeInfo lastScope = scope.back();
    scope.pop_back();

    for(int i=stack.size()-1; i >= lastScope.stackIndex ; i--)
    {
        addrToVarMap.erase(stack.back()->getAddress());
        delete stack.back();

        stack.pop_back();
    }

    assert(stack.size() == lastScope.stackIndex);
}

int StackManager::getScopeCount()  const
{
    return scope.size();
}

const std::string & StackManager::getScopeName(int i) const
{
    assert(i >=0 );
    assert(i < scope.size());
    return scope[i].name;
}

StackManager::VariableIter StackManager::variablesBegin(int i) const
{
    assert(i >=0 );
    assert(i < scope.size());

    return stack.begin() + scope[i].stackIndex;
}

StackManager::VariableIter StackManager::variablesEnd(int i) const
{
    assert(i >=0 );
    assert(i < scope.size());

    if(i+1 == scope.size())
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

    for(int sc=0; sc < scope.size(); sc++)
    {
        os << scope[sc].name << ":" << endl;

        for (VariableIter i  = variablesBegin(sc); i != variablesEnd(sc); ++i)
             os << "\t" << **i << endl;

    }

    os << endl;
}







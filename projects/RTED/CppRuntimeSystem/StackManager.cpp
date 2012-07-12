#include "StackManager.h"

#include "VariablesType.h"
#include "CppRuntimeSystem.h"

StackManager::StackManager()
{
    beginScope("Globals");
}

void StackManager::addVariable(Location address, const char* name, const char* mangledName, const RsType& t, long /*blocksize*/)
{
    assert(scope.size() > 0);

    // if the variable is (or contains) pointer register it to pointer manager
    // rtedRTS(this).getPointerManager().createPointer(address, &t, blocksize);

    // addrToVarMap.insert(AddrToVarMap::value_type(varaddr, var));
    stack.push_back(VariablesType(address, name, mangledName, &t));
}

namespace
{
  struct AtAddress
  {
    const Address where;

    explicit
    AtAddress(Address loc)
    : where(loc)
    {}

    bool operator()(const VariablesType& var) const
    {
      return var.getAddress() == where;
    }
  };
}

static inline
AtAddress atAddress(Address addr)
{
  return AtAddress(addr);
}

const VariablesType*
StackManager::getVariable(Location addr) const
{
    // \todo use lower_bound
    VariableStack::const_iterator zz = stack.end();
    VariableStack::const_iterator pos = std::find_if(stack.begin(), zz, atAddress(addr));

    if (pos == zz) return NULL;
    return &*pos;
}

const VariablesType*
StackManager::getVariableByMangledName(const std::string& mangledName) const
{
    for (size_t i=0; i <  stack.size(); ++i)
    {
        if (stack[i].getMangledName() == mangledName)
            return &stack[i];
    }
    return NULL;
}

void StackManager::beginScope(const char* name)
{
    scope.push_back( ScopeInfo(name, stack.size()) );
}

void StackManager::endScope(size_t scopecount)
{
  const ScopeStack::iterator limit   = scope.end();
  const ScopeStack::iterator new_top = limit - scopecount;
  const size_t               noScopes = scope.size();
  assert( noScopes >= scopecount );

  const size_t               new_stack_size = new_top->stackIndex;
  size_t                     curr_stack_size = stack.size();
  assert(curr_stack_size >= new_stack_size);

  RuntimeSystem&             rs = rtedRTS(this);
  MemoryManager&             memmgr = rs.getMemManager();

  while (curr_stack_size > new_stack_size)
  {
      const Address varaddr = stack.back().getAddress();

      memmgr.freeStackMemory(varaddr);
      stack.pop_back();
      --curr_stack_size;
  }

  scope.erase(new_top, limit);
}

int StackManager::getScopeCount()  const
{
    return scope.size();
}

std::string StackManager::getScopeName(size_t i) const
{
    assert(i < scope.size());
    return scope[i].name;
}

StackManager::VariableStack::const_iterator
StackManager::variablesBegin(size_t i) const
{
    assert(i < scope.size());

    return stack.begin() + scope[i].stackIndex;
}

StackManager::VariableStack::const_iterator
StackManager::variablesEnd(size_t i) const
{
    assert(i < scope.size());

    if (i+1 == scope.size()) return stack.end();

    return stack.begin() + scope[i+1].stackIndex;
}


void StackManager::clearStatus()
{
    endScope(scope.size());

    assert(stack.size() == 0);

    beginScope("Globals");
}


void StackManager::print(std::ostream& os) const
{
    os << std::endl;
    os << "------------------------------- Stack Status --------------------------------------" << std::endl << std::endl;

    for (unsigned int sc=0; sc < scope.size(); sc++)
    {
        os << scope[sc].name << ":" << std::endl;

        for (VariableStack::const_iterator i = variablesBegin(sc); i != variablesEnd(sc); ++i)
             os << "\t" << *i << std::endl;

    }

    os << std::endl;
}

#include "PointerManager.h"
#include "CppRuntimeSystem.h"

#include <cassert>

using namespace std;

// -------------------------- Pointer Info -------------------------------


PointerInfo::PointerInfo(addr_type s)
    : source(s), target(0), baseType(NULL)
{
}

PointerInfo::PointerInfo(addr_type s, addr_type t, RsType * type)
    : source(s), target(0), baseType(type)
{
    setTargetAddress(t);
}


PointerInfo::~PointerInfo()
{
    // If still valid, invalidate the pointer
    if(target != 0)
        setTargetAddress(0);
}

VariablesType * PointerInfo::getVariable() const
{
    return RuntimeSystem::instance()->getStackManager()->getVariable(source);
}

bool PointerInfo::operator< (const PointerInfo & other ) const
{
    return source < other.source;
}

void PointerInfo::setTargetAddress(addr_type newAddr, bool doChecks)
{
    RuntimeSystem * rs = RuntimeSystem::instance();
    MemoryManager * mm = rs->getMemManager();


    addr_type oldTarget = target;
    target = newAddr;

    if(oldTarget == 0)//inital assignment -> no checks possible
        doChecks=false;


    // Check if newAddr points to valid mem-region
    MemoryType * newMem = mm->findContainingMem(target,   baseType->getByteSize());
    MemoryType * oldMem = mm->findContainingMem(oldTarget,baseType->getByteSize());

    if(!newMem && newAddr != 0) //new address is invalid
    {
        stringstream ss;
        ss << "Tried to assign non allocated address 0x" << hex << newAddr;
        VariablesType * var = getVariable();
        if(var)
            ss << " to pointer " << var->getName();
        else
            ss << " to pointer at address 0x" << hex << source;

        ss << endl;

        target=0;
        rs->violationHandler(RuntimeViolation::INVALID_PTR_ASSIGN,ss.str());
        return;
    }

    // If a pointer points to that memory region, assume that his mem-region if of right type
    // even if actual error would only happen on deref
    if(newMem)
        newMem->accessMemWithType(newAddr-newMem->getAddress(),baseType);


    // if old target was valid
    if(oldTarget != 0)
    {
        assert(oldMem);  //has to be valid, because checked when set

        // old and new address is valid
        if( newMem && doChecks )
            mm->checkIfSameChunk(oldTarget,target,baseType);
    }
}


void PointerInfo::print(ostream & os) const
{
    os << "0x" << hex << getSourceAddress() << "\t"
       << "0x" << hex << getTargetAddress() << "\t"
       << getBaseType()->getName();

    VariablesType * var = getVariable();
    if(var)
        os << "\t" << *var;

}

ostream& operator<< (ostream &os, const PointerInfo & m)
{
    m.print(os);
    return os;
}


// -------------------------- PointerManager -------------------------------


void PointerManager::createPointer(addr_type sourceAddress, RsType * type)
{
    PointerInfo * pi = new PointerInfo(sourceAddress,0,type);
    pointerInfoSet.insert(pi);

    typedef TargetToPointerMap::value_type ValType;
    targetToPointerMap.insert(ValType(0,pi));
}


void PointerManager::deletePointer(addr_type src)
{
    PointerInfo dummy (src);
    PointerSetIter i = pointerInfoSet.find(&dummy);
    assert(i != pointerInfoSet.end());
    PointerInfo * pi = *i;

    // Delete from map
    bool res = removeFromRevMap(pi);
    assert(res);

    // Delete from set
    pointerInfoSet.erase(i);
    delete pi;
}


void PointerManager::deletePointerInRegion(addr_type from, addr_type to)
{
    if(to <= from)
        return;

    PointerInfo objFrom(from);
    PointerInfo objTo(to-1);

    PointerSetIter it  = pointerInfoSet.lower_bound(&objFrom);
    PointerSetIter end = pointerInfoSet.upper_bound(&objTo  );

    // First store all pointer which have to be deleted
    // erase(it,end) doesn't work because the pointer need to be removed from map
    vector<addr_type> toDelete;
    for(; it != end; ++it)
        toDelete.push_back((*it)->getSourceAddress());

    for(int i=0; i<toDelete.size(); i++ )
    {
        //cout << "Deleting pointer at " << toDelete[i] << endl;
        deletePointer(toDelete[i]);
    }
}


void PointerManager::registerPointerChange( addr_type src, addr_type target, bool checks)
{
    PointerInfo dummy(src);
    PointerSetIter i = pointerInfoSet.find(&dummy);
    assert(pointerInfoSet.find(&dummy) != pointerInfoSet.end()); //forgot to call createPointer?
    PointerInfo * pi = *i;

    addr_type oldTarget = pi->getTargetAddress();

    // Remove the pointer info from TargetToPointerMap
    bool res = removeFromRevMap(pi);
    assert(res); // assert that an entry has been deleted

    try
    {
        // set correct target
        pi->setTargetAddress(target);
    }
    catch(RuntimeViolation & vio)
    {
        // if target could not been set, then set pointer to null
        pi->setTargetAddressForce(0);
        targetToPointerMap.insert(TargetToPointerMap::value_type(0,pi));
        throw vio;
    }

    // ...and insert it again with changed target
    targetToPointerMap.insert(TargetToPointerMap::value_type(target,pi));



    if(checks)
    {
        MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
        mm->checkIfSameChunk(oldTarget,target,pi->getBaseType());
    }

    bool memLeakChecks=true;
    if(memLeakChecks)
    {
        MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
        MemoryType * oldMem = mm->findContainingMem(oldTarget,pi->getBaseType()->getByteSize());

        if(oldMem && !oldMem->isOnStack()) // no memory leaks on stack
        {
            // find other pointer still pointing to this region
            TargetToPointerMapIter begin = targetRegionIterBegin(oldMem->getAddress());
            TargetToPointerMapIter end   = targetRegionIterBegin(oldMem->getAddress() + oldMem->getSize());

            // if nothing found create violation
            if(begin == end)
            {
                RuntimeSystem * rs = RuntimeSystem::instance();
                stringstream ss;
                ss << "No pointer to allocated memory region: " << endl;
                ss << *oldMem << endl << "because this pointer has changed: " <<endl;
                ss << *pi << endl;
                rs->violationHandler(RuntimeViolation::MEM_WITHOUT_POINTER, ss.str());
            }

        }
    }
}



void PointerManager::checkPointerDereference( addr_type src, addr_type deref_addr)
{
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();

    PointerInfo dummy (src);
    PointerSetIter i = pointerInfoSet.find(&dummy);
    assert(i != pointerInfoSet.end());
    PointerInfo * pi = *i;

    mm->checkIfSameChunk(pi->getTargetAddress(),deref_addr,pi->getBaseType());
}

void PointerManager::invalidatePointerToRegion(addr_type from, addr_type to, bool remove)
{
    if(to <= from)
        return;

    typedef TargetToPointerMap::iterator Iter;
    Iter start= targetToPointerMap.lower_bound(from);
    Iter end  = targetToPointerMap.upper_bound(to-1);

    //cout << "Invalidating region " << from << " " << to << endl;
    //print(cout);
    for(Iter i = start; i != end; ++i)
    {
        //cout << "Matching Pointers " << i->first << endl << *(i->second) << endl << endl;
        PointerInfo * pi = i->second;
        // assert that no pointer overlaps given region
        assert(pi->getTargetAddress() <= to - pi->getBaseType()->getByteSize());

        if(remove)
        {
            PointerSet::iterator it = pointerInfoSet.find(pi);
            assert(it != pointerInfoSet.end());
            pointerInfoSet.erase(it);
        }
        else
        {
            pi->setTargetAddress(0);
            targetToPointerMap.insert(TargetToPointerMap::value_type(0,pi));
        }
    }

    targetToPointerMap.erase(start,end);
}


bool PointerManager::removeFromRevMap(PointerInfo * pi)
{
    typedef TargetToPointerMap::iterator MapIter;
    pair<MapIter,MapIter> range = targetToPointerMap.equal_range(pi->getTargetAddress());
    bool found=false;
    for(MapIter i = range.first; i !=  range.second; ++i)
    {
        if( i->second == pi)
        {
            targetToPointerMap.erase(i);
            return true;
        }
    }

    return false;
}


PointerManager::PointerSetIter PointerManager::sourceRegionIter(addr_type sourceAddr)
{
    PointerInfo dummy(sourceAddr);
    return pointerInfoSet.lower_bound(&dummy);
}

PointerManager::TargetToPointerMapIter PointerManager::targetRegionIterBegin(addr_type targetAddr)
{
    return targetToPointerMap.lower_bound(targetAddr);
}

PointerManager::TargetToPointerMapIter PointerManager::targetRegionIterEnd (addr_type targetAddr)
{
    return targetToPointerMap.upper_bound(targetAddr);
}


void PointerManager::clearStatus()
{
    for(PointerSet::iterator it = pointerInfoSet.begin();
            it != pointerInfoSet.end(); ++it)
    {
        (*it)->setTargetAddressForce(0); // skip pointer checks on delete
        delete *it;
    }
    pointerInfoSet.clear();

    targetToPointerMap.clear();
}


void PointerManager::print(ostream & os) const
{
    os << "---------------- Registered Pointer ----------------------" << endl;

    for(PointerSet::const_iterator it = pointerInfoSet.begin();
         it != pointerInfoSet.end(); ++it)
    {
        PointerInfo * pi = *it;
        //consistency check
        assert(targetToPointerMap.find(pi->getTargetAddress()) != targetToPointerMap.end() );

        os << *pi << endl;
    }

    os << "----------------------------------------------------------" << endl;
}







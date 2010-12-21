// vim:sta et:
#include "PointerManager.h"
#include "CppRuntimeSystem.h"

#include <cassert>

// -------------------------- Pointer Info -------------------------------


PointerInfo::PointerInfo(MemoryAddress s)
: source(s), target(nullAddr()), baseType(NULL)
{}

PointerInfo::PointerInfo(MemoryAddress s, MemoryAddress t, RsType * type)
: source(s), target(nullAddr()), baseType(type)
{
    setTargetAddress(t);
}


PointerInfo::~PointerInfo()
{
    // If still valid, invalidate the pointer
    if(!isNullAddr(target))
       setTargetAddress(nullAddr());
}

VariablesType * PointerInfo::getVariable() const
{
    return RuntimeSystem::instance()->getStackManager()->getVariable(source);
}

bool PointerInfo::operator< (const PointerInfo & other ) const
{
    return source < other.source;
}


void PointerInfo::setTargetAddress(MemoryAddress newAddr, bool doChecks)
{
    RuntimeSystem * rs = RuntimeSystem::instance();
    MemoryManager * mm = rs->getMemManager();


    MemoryAddress oldTarget = target;
    target = newAddr;

    if (isNullAddr(oldTarget))//inital assignment -> no checks possible
        doChecks=false;


    // Check if newAddr points to valid mem-region
    MemoryType * newMem = mm->findContainingMem(target,    baseType->getByteSize());
    MemoryType * oldMem = mm->findContainingMem(oldTarget, baseType->getByteSize());

    if (!newMem && !isNullAddr(newAddr)) //new address is invalid
    {
        std::stringstream ss;
        ss << "Tried to assign non allocated address 0x" << newAddr;
        VariablesType * var = getVariable();
        if(var)
            ss << " to pointer " << var->getName();
        else
            ss << " to pointer at address 0x" << source;

        ss << std::endl;

        target = nullAddr();
        rs->violationHandler(RuntimeViolation::INVALID_PTR_ASSIGN,ss.str());
        return;
    }

    // Don't update the target with type information just because a pointer
    // points there -- but do check to make sure that the pointer isn't
    // definitely illegal (e.g. an int pointer pointing to a known double).
    if(newMem)
        // FIXME 2: This should really only check, and not merge
        newMem->checkAndMergeMemType(newAddr - newMem->getAddress(),baseType);


    // if old target was valid
    if (!isNullAddr(oldTarget))
    {
        assert(oldMem);  //has to be valid, because checked when set

        // old and new address is valid
        if( newMem && doChecks )
            mm->checkIfSameChunk(oldTarget,target,baseType);
    }
}


void PointerInfo::print(std::ostream& os) const
{
    os << "0x" << getSourceAddress() << "\t"
       << "0x" << getTargetAddress() << "\t"
       << getBaseType()->getName();

    VariablesType * var = getVariable();
    if(var)
        os << "\t" << *var;

}

std::ostream& operator<< (std::ostream& os, const PointerInfo& m)
{
    m.print(os);
    return os;
}


// -------------------------- PointerManager -------------------------------


void PointerManager::createDereferentiableMem(MemoryAddress sourceAddress, RsType * type)
{
    PointerInfo * pi = new PointerInfo(sourceAddress, nullAddr(), type);
    std::pair<PointerSet::iterator, bool>  res = pointerInfoSet.insert(pi);
    if(!res.second)//pointer was already registered
    {
        delete pi;
        return;
    }

    targetToPointerMap.insert(TargetToPointerMap::value_type(nullAddr(), pi));
}


void PointerManager::createPointer(MemoryAddress baseAddr, RsType * type)
{
    // If type is a pointer register it
    RsPointerType * pt = dynamic_cast<RsPointerType*>(type);
    if(pt)
        createDereferentiableMem(baseAddr,pt->getBaseType());

    // arrays are 'pseudo-pointers' whose target and source addresses are the
    // same, i.e.
    //  type x[ element_count ];
    //  &x == x         // this is true
    RsArrayType * at = dynamic_cast<RsArrayType*>(type);
    if (at) {
        createDereferentiableMem(baseAddr,at->getBaseType());
        registerPointerChange( baseAddr, baseAddr, nocheck, check );
    }

    // If compound type, break down to basic types
    for (int i = 0; i < type->getSubtypeCount(); ++i)
        createPointer(baseAddr + type->getSubtypeOffset(i), type->getSubtype(i) );
}


void PointerManager::deletePointer( MemoryAddress src, bool checks )
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

    if( checks )
        checkForMemoryLeaks( pi );

    delete pi;
}


void PointerManager::deletePointerInRegion( MemoryAddress from, MemoryAddress to, bool checks)
{
    // \pp why not assert(from < to)?
    if(to <= from)
        return;

    PointerInfo objFrom(from);
    PointerInfo objTo(to-1);

    PointerSetIter lb = pointerInfoSet.lower_bound(&objFrom);
    PointerSetIter ub = pointerInfoSet.upper_bound(&objTo  );

    // First store all pointer which have to be deleted
    // erase(lb, ub) doesn't work because the pointer need to be removed from map
    std::vector< PointerInfo* > toDelete(lb, ub);

    for(unsigned int i=0; i<toDelete.size(); i++ )
    {
        PointerInfo*  curr = toDelete[i];
        MemoryAddress loc = curr->getTargetAddress();

        // check a pointer if it is required and the pointer points outside
        //   the region
        const bool    check_this_pointer = (  checks
                                           && (loc < from || to < loc)
                                           );

        deletePointer(curr->getSourceAddress(), check_this_pointer);
    }
}


void PointerManager::registerPointerChange( MemoryAddress src, MemoryAddress target, RsType * bt, bool checkPointerMove, bool checkMemLeaks)
{
    PointerInfo dummy(src);
    PointerSetIter i = pointerInfoSet.find(&dummy);

    if (i != pointerInfoSet.end())
    {
        // if pointer exists already, make sure that baseTypes match
        assert( (*i) -> baseType = bt );
    }
    else
    {
        // create the pointer
        createDereferentiableMem(src,bt);
    }

    registerPointerChange(src,target,checkPointerMove, checkMemLeaks);
}

void PointerManager::registerPointerChange( MemoryAddress src, MemoryAddress target, bool checkPointerMove, bool checkMemLeaks)
{
    PointerInfo dummy(src);
    PointerSetIter i = pointerInfoSet.find(&dummy);

    // forgot to call createPointer?
    // call the overloaded registerPointerChange() instead which does not require createPointer()
    assert(i != pointerInfoSet.end());

    PointerInfo * pi = *i;

    MemoryAddress oldTarget = pi->getTargetAddress();


    bool legal_move = true;
    if( checkPointerMove )
    {
        MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
        legal_move = mm -> checkIfSameChunk(
                oldTarget,
                target,
                pi -> getBaseType() -> getByteSize(),
                RuntimeViolation::NONE );

        // This isn't a legal
        RuntimeSystem* rs = RuntimeSystem::instance();
        if( !legal_move ) {
            if( rs -> violationTypePolicy[ RuntimeViolation::INVALID_PTR_ASSIGN ]
                    == ViolationPolicy::InvalidatePointer) {
                // Don't complain now, but invalidate the pointer so that subsequent
                // reads or writes without first registering a pointer change will
                // be treated as invalid
                pi -> setTargetAddressForce( nullAddr() );
                targetToPointerMap.insert(TargetToPointerMap::value_type(nullAddr(), pi));
                if( !( rs -> testingMode ))
                  *point_to<int>(pi->getSourceAddress()) = 0;
            } else {
                // repeat check but throw invalid ptr assign
                mm -> checkIfSameChunk(
                    oldTarget,
                    target,
                    pi -> getBaseType() -> getByteSize(),
                    RuntimeViolation::INVALID_PTR_ASSIGN );
                // shouldn't get here
                assert( false );
            }
        }
    }


    // Remove the pointer info from TargetToPointerMap
    bool res = removeFromRevMap(pi);
    assert(res); // assert that an entry has been deleted

    if( legal_move ) {
        try {
            // set correct target
            pi->setTargetAddress(target);
        } catch(RuntimeViolation & vio) {
            // if target could not been set, then set pointer to null
            pi->setTargetAddressForce(nullAddr());
            targetToPointerMap.insert(TargetToPointerMap::value_type(nullAddr(), pi));
            throw vio;
        }
        // ...and insert it again with changed target
        targetToPointerMap.insert(TargetToPointerMap::value_type(target,pi));
    }


    if( checkMemLeaks )
        checkForMemoryLeaks( oldTarget, pi -> getBaseType() -> getByteSize() );

}


bool PointerManager::checkForMemoryLeaks( PointerInfo* pi )
{
    return checkForMemoryLeaks(
        pi -> getTargetAddress(),
        pi -> getBaseType() -> getByteSize(),
        pi
    );
}

bool PointerManager::checkForMemoryLeaks( MemoryAddress address, size_t type_size, PointerInfo* pointer_to_blame )
{
    bool found_leaks = false;
    MemoryManager * mm = RuntimeSystem::instance() -> getMemManager();
    MemoryType * mem = mm->findContainingMem( address, type_size );

    if( mem && !mem->isOnStack() ) // no memory leaks on stack
    {
        // find other pointer still pointing to this region
        TargetToPointerMapIter begin = targetRegionIterBegin( mem -> getAddress() );
        TargetToPointerMapIter end   = targetRegionIterBegin( mem -> getAddress() + mem->getSize() );

        // if nothing found create violation
        if(begin == end)
        {
            found_leaks = true;
            RuntimeSystem * rs = RuntimeSystem::instance();
            std::stringstream ss;
            ss << "No pointer to allocated memory region: " << std::endl;
            if( pointer_to_blame ) {
                ss << *mem << std::endl << "because this pointer has changed: " <<std::endl;
                ss << *pointer_to_blame << std::endl;
            }
            rs->violationHandler(RuntimeViolation::MEM_WITHOUT_POINTER, ss.str());
        }

    }
    return found_leaks;
}


void PointerManager::checkIfPointerNULL( void* pointer)
{
	RuntimeSystem * rs = RuntimeSystem::instance();
	if (pointer==NULL) {
		std::stringstream ss;
		ss << "Accessing (This) NULL Pointer: " << std::endl;
		rs->violationHandler(RuntimeViolation::INVALID_WRITE, ss.str());
	}
}

void PointerManager::checkPointerDereference( MemoryAddress src, MemoryAddress deref_addr)
{
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();

    PointerInfo dummy (src);
    PointerSetIter i = pointerInfoSet.find(&dummy);
    assert(i != pointerInfoSet.end());
    PointerInfo * pi = *i;

    mm->checkIfSameChunk(pi->getTargetAddress(),deref_addr,pi->getBaseType());
}

void PointerManager::invalidatePointerToRegion(MemoryAddress from, MemoryAddress to, bool remove)
{
    if(to <= from)
        return;

    typedef TargetToPointerMap::iterator Iter;
    Iter start= targetToPointerMap.lower_bound(from);
    Iter end  = targetToPointerMap.upper_bound(to-1);

    //cout << "Invalidating region " << from << " " << to << std::endl;
    //print(cout);
    for(Iter i = start; i != end; ++i)
    {
        //cout << "Matching Pointers " << i->first << std::endl << *(i->second) << std::endl << std::endl;
        PointerInfo * pi = i->second;
        // assert that no pointer overlaps given region
        assert(pi->getTargetAddress() <= to - pi->getBaseType()->getByteSize());

        if (remove)
        {
            PointerSet::iterator it = pointerInfoSet.find(pi);
            assert(it != pointerInfoSet.end());
            pointerInfoSet.erase(it);
        }
        else
        {
            pi->setTargetAddress(nullAddr());
            targetToPointerMap.insert(TargetToPointerMap::value_type(nullAddr(), pi));
        }
    }

    targetToPointerMap.erase(start,end);
}


bool PointerManager::removeFromRevMap(PointerInfo * pi)
{
    typedef TargetToPointerMap::iterator MapIter;
    std::pair<MapIter,MapIter> range = targetToPointerMap.equal_range(pi->getTargetAddress());
    //    bool found=false;
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


PointerManager::PointerSetIter PointerManager::sourceRegionIter(MemoryAddress sourceAddr)
{
    PointerInfo dummy(sourceAddr);
    return pointerInfoSet.lower_bound(&dummy);
}

PointerManager::TargetToPointerMapIter PointerManager::targetRegionIterBegin(MemoryAddress targetAddr)
{
    return targetToPointerMap.lower_bound(targetAddr);
}

PointerManager::TargetToPointerMapIter PointerManager::targetRegionIterEnd (MemoryAddress targetAddr)
{
    return targetToPointerMap.upper_bound(targetAddr);
}


void PointerManager::clearStatus()
{
    for(PointerSet::iterator it = pointerInfoSet.begin();
            it != pointerInfoSet.end(); ++it)
    {
        (*it)->setTargetAddressForce(nullAddr()); // skip pointer checks on delete
        delete *it;
    }
    pointerInfoSet.clear();

    targetToPointerMap.clear();
}


void PointerManager::print(std::ostream& os) const
{
    os << "---------------- Registered Pointer ----------------------" << std::endl;

    for(PointerSet::const_iterator it = pointerInfoSet.begin();
         it != pointerInfoSet.end(); ++it)
    {
        PointerInfo * pi = *it;
        //consistency check
        assert(targetToPointerMap.find(pi->getTargetAddress()) != targetToPointerMap.end() );

        os << *pi << std::endl;
    }

    os << "----------------------------------------------------------" << std::endl;
}

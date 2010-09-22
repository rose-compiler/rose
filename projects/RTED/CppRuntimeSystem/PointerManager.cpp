// vim:sta et:
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

    // Don't update the target with type information just because a pointer
    // points there -- but do check to make sure that the pointer isn't
    // definitely illegal (e.g. an int pointer pointing to a known double).
    if(newMem)
        // FIXME 2: This should really only check, and not merge
        newMem->checkAndMergeMemType(newAddr-newMem->getAddress(),baseType);


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


void PointerManager::createDereferentiableMem(addr_type sourceAddress, RsType * type)
{
    PointerInfo * pi = new PointerInfo(sourceAddress,0,type);
    pair<PointerSet::iterator, bool>  res = pointerInfoSet.insert(pi);
    if(!res.second)//pointer was already registered
    {
        delete pi;
        return;
    }


    typedef TargetToPointerMap::value_type ValType;
    targetToPointerMap.insert(ValType(0,pi));
}


void PointerManager::createPointer(addr_type baseAddr, RsType * type)
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
    if(at) {
        createDereferentiableMem(baseAddr,at->getBaseType());
        registerPointerChange( baseAddr, baseAddr );
    }

    // If compound type, break down to basic types
    for(int i = 0; i < type->getSubtypeCount(); i++)
        createPointer(baseAddr + type->getSubtypeOffset(i), type->getSubtype(i) );
}


void PointerManager::deletePointer( addr_type src, bool checks )
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


void PointerManager::deletePointerInRegion( addr_type from, addr_type to, bool checks)
{
    if(to <= from)
        return;

    PointerInfo objFrom(from);
    PointerInfo objTo(to-1);

    PointerSetIter it  = pointerInfoSet.lower_bound(&objFrom);
    PointerSetIter end = pointerInfoSet.upper_bound(&objTo  );

    // First store all pointer which have to be deleted
    // erase(it,end) doesn't work because the pointer need to be removed from map
    vector< PointerInfo* > toDelete;
    for(; it != end; ++it)
        toDelete.push_back( *it );

    for(int i=0; i<toDelete.size(); i++ )
    {
        bool check_this_pointer = 
            checks && (
                toDelete[ i ] -> getTargetAddress() < from
                || toDelete[ i ] -> getTargetAddress() > to
            );
        //cout << "Deleting pointer at " << toDelete[i] << endl;
        deletePointer( toDelete[i] -> getSourceAddress(), check_this_pointer );
    }
}


void PointerManager::registerPointerChange( addr_type src, addr_type target, RsType * bt, bool checkPointerMove, bool checkMemLeaks)
{
    PointerInfo dummy(src);
    PointerSetIter i = pointerInfoSet.find(&dummy);

    if(pointerInfoSet.find(&dummy) != pointerInfoSet.end())
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

void PointerManager::registerPointerChange( addr_type src, addr_type target, bool checkPointerMove, bool checkMemLeaks)
{
    PointerInfo dummy(src);
    PointerSetIter i = pointerInfoSet.find(&dummy);

    // forgot to call createPointer?
    // call the overloaded registerPointerChange() instead which does not require createPointer()
    assert(pointerInfoSet.find(&dummy) != pointerInfoSet.end());

    PointerInfo * pi = *i;

    addr_type oldTarget = pi->getTargetAddress();


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
                pi -> setTargetAddressForce( 0 );
                targetToPointerMap.insert(TargetToPointerMap::value_type(0,pi));
                if( !( rs -> testingMode ))
                    *((int*) (pi -> getSourceAddress())) = NULL;
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
            pi->setTargetAddressForce(0);
            targetToPointerMap.insert(TargetToPointerMap::value_type(0,pi));
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

bool PointerManager::checkForMemoryLeaks( addr_type address, size_t type_size, PointerInfo* pointer_to_blame )
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
            stringstream ss;
            ss << "No pointer to allocated memory region: " << endl;
            if( pointer_to_blame ) {
                ss << *mem << endl << "because this pointer has changed: " <<endl;
                ss << *pointer_to_blame << endl;
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
		stringstream ss;
		ss << "Accessing (This) NULL Pointer: " << endl;
		rs->violationHandler(RuntimeViolation::INVALID_WRITE, ss.str());
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







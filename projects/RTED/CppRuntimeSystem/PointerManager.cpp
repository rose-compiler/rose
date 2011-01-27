// vim:sta et:
#include "PointerManager.h"
#include "CppRuntimeSystem.h"

#include <cassert>

// -------------------------- Pointer Info -------------------------------

static
void insert(PointerManager::TargetToPointerMap& m, const PointerManager::TargetToPointerMap::value_type& val)
{
  if (val.first != val.second->getTargetAddress())
  {
    std::cout << HexToString(val.first) << " <---> "
              << HexToString(val.second->getTargetAddress())
              << std::endl;
    assert(val.first == val.second->getTargetAddress());
  }

  m.insert(val);
}


PointerInfo::PointerInfo(Location s)
: source(s), target(NULL), baseType(NULL)
{}

PointerInfo::PointerInfo(Location s, Location t, RsType * type)
: source(s), target(NULL), baseType(type)
{
    setTargetAddress(t);
}


PointerInfo::~PointerInfo()
{
  // \pp ???

  // If still valid, invalidate the pointer
  if(target)
     setTargetAddress(NULL);
}

VariablesType * PointerInfo::getVariable() const
{
    return RuntimeSystem::instance()->getStackManager()->getVariable(source);
}

bool PointerInfo::operator< (const PointerInfo & other ) const
{
    return source < other.source;
}


void PointerInfo::setTargetAddress(Location newAddr, bool doCheck)
{
    RuntimeSystem* rs = RuntimeSystem::instance();
    MemoryManager* mm = rs->getMemManager();
    const size_t   objsz = baseType->getByteSize();
    MemoryType*    newMem = mm->findContainingMem(newAddr, objsz);

    std::cerr << "size(mm) = " << mm->getAllocationSet().size()
              << " @ " << HexToString(newAddr)
              << " == ";

    if (newMem) std::cerr << HexToString(newMem->getAddress());
    else std::cerr << "<null>";
    std::cerr << std::endl;

    if (!newMem && newAddr) //new address is invalid
    {
      std::cerr << "{NULL" << std::endl;
      std::stringstream ss;
      ss << "Tried to assign non allocated address 0x" << newAddr;
      VariablesType * var = getVariable();
      if (var)
          ss << " to pointer " << var->getName();
      else
          ss << " to pointer at address 0x" << HexToString(source);

      ss << std::endl;

      target = NULL;
      std::cerr << "NULL}" << std::endl;
      rs->violationHandler(RuntimeViolation::INVALID_PTR_ASSIGN, ss.str());
      return;
    }

    // Don't update the target with type information just because a pointer
    // points there -- but do check to make sure that the pointer isn't
    // definitely illegal (e.g. an int pointer pointing to a known double).
    if (newMem)
    {
      std::cerr << "{bbb" << std::endl;
      // FIXME 2: This should really only check, and not merge
      assert(newMem->getAddress() <= newAddr);
      newMem->checkAndMergeMemType(newAddr - newMem->getAddress(), baseType);
      std::cerr << "bbb}" << std::endl;
    }

    // if old target is valid
    if (target)
    {
      std::cerr << "{aaa" << std::endl;
      // has to hold, because checked when set
      assert(mm->findContainingMem(target, objsz) != NULL);

      // old and new address is valid
      if (newMem && doCheck)
      {
        mm->checkIfSameChunk(target, newAddr, baseType);
      }
      std::cerr << "aaa}" << std::endl;
    }

    std::cerr << "target-set" << HexToString(newAddr) << std::endl;
    target = newAddr;
}


void PointerInfo::print(std::ostream& os) const
{
    os << "0x" << HexToString(getSourceAddress()) << "\t"
       << "0x" << HexToString(getTargetAddress()) << "\t"
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


void PointerManager::createDereferentiableMem(Location sourceAddress, RsType * type)
{
    PointerInfo * pi = new PointerInfo(sourceAddress, NULL, type);
    std::pair<PointerSet::iterator, bool>  res = pointerInfoSet.insert(pi);
    if(!res.second)//pointer was already registered
    {
        delete pi;
        return;
    }

    insert(targetToPointerMap, TargetToPointerMap::value_type(NULL, pi));
}


void PointerManager::createPointer(Location baseAddr, RsType * type)
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


void PointerManager::deletePointer( Location src, bool checks )
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


void PointerManager::deletePointerInRegion( Location from, Location to, bool checks)
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
        Location loc = curr->getTargetAddress();

        // check a pointer if it is required and the pointer points outside
        //   the region
        const bool    check_this_pointer = (  checks
                                           && (loc < from || to < loc)
                                           );

        deletePointer(curr->getSourceAddress(), check_this_pointer);
    }
}


void PointerManager::registerPointerChange( Location src, Location target, RsType * bt, bool checkPointerMove, bool checkMemLeaks)
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

void PointerManager::registerPointerChange( Location src, Location target, bool checkPointerMove, bool checkMemLeaks)
{
    PointerInfo dummy(src);
    PointerSetIter i = pointerInfoSet.find(&dummy);

    // forgot to call createPointer?
    // call the overloaded registerPointerChange() instead which does not require createPointer()
    assert(i != pointerInfoSet.end());

    PointerInfo * pi = *i;

    Location oldTarget = pi->getTargetAddress();


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
                pi -> setTargetAddressForce( NULL );
                insert(targetToPointerMap, TargetToPointerMap::value_type( NULL, pi ));
                if( !( rs -> testingMode ))
                  *const_cast<int*>(reinterpret_cast<const int*>(pi->getSourceAddress())) = 0;
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
            pi->setTargetAddressForce(NULL);
            insert(targetToPointerMap, TargetToPointerMap::value_type(NULL, pi));
            throw vio;
        }
        // ...and insert it again with changed target
        std::cerr << "here$$$" << std::endl;
        insert(targetToPointerMap, TargetToPointerMap::value_type(target, pi));
        std::cerr << "there$$$" << std::endl;
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

bool PointerManager::checkForMemoryLeaks( Location address, size_t type_size, PointerInfo* pointer_to_blame )
{
    bool found_leaks = false;
    MemoryManager * mm = RuntimeSystem::instance() -> getMemManager();
    MemoryType * mem = mm->findContainingMem( address, type_size );

    if(mem && (mem->howCreated() != akStack)) // no memory leaks on stack
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

void PointerManager::checkPointerDereference( Location src, Location deref_addr)
{
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();

    PointerInfo dummy (src);
    PointerSetIter i = pointerInfoSet.find(&dummy);
    assert(i != pointerInfoSet.end());
    PointerInfo * pi = *i;

    mm->checkIfSameChunk(pi->getTargetAddress(),deref_addr,pi->getBaseType());
}

void PointerManager::invalidatePointerToRegion(Location from, Location to, bool remove)
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
            pi->setTargetAddress(NULL);
            insert(targetToPointerMap, TargetToPointerMap::value_type(NULL, pi));
        }
    }

    targetToPointerMap.erase(start,end);
}


bool PointerManager::removeFromRevMap(PointerInfo * pi)
{
    assert(!targetToPointerMap.empty());

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


PointerManager::PointerSetIter PointerManager::sourceRegionIter(Location sourceAddr)
{
    PointerInfo dummy(sourceAddr);
    return pointerInfoSet.lower_bound(&dummy);
}

PointerManager::TargetToPointerMapIter PointerManager::targetRegionIterBegin(Location targetAddr)
{
    return targetToPointerMap.lower_bound(targetAddr);
}

PointerManager::TargetToPointerMapIter PointerManager::targetRegionIterEnd (Location targetAddr)
{
    return targetToPointerMap.upper_bound(targetAddr);
}


void PointerManager::clearStatus()
{
    for(PointerSet::iterator it = pointerInfoSet.begin();
            it != pointerInfoSet.end(); ++it)
    {
        (*it)->setTargetAddressForce(NULL); // skip pointer checks on delete
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

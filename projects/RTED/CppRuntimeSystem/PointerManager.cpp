// vim:sta et:
#include <iomanip>
#include <cassert>
#include <iostream>

#include "PointerManager.h"
#include "CppRuntimeSystem.h"
#include "MemoryManager.h"


// -------------------------- Pointer Info -------------------------------

static
void zeroIntValueAt(Address addr)
{
  rted_setIntVal(addr, 0);
}

static
void insert(PointerManager::TargetToPointerMap& m, const PointerManager::TargetToPointerMap::value_type& val)
{
  Address loc = val.first;

  // note, the following assert does not hold, as it can store pointers
  //   where the targetAddress is set to 0, if the pointer points to nowhere
  //   (e.g., one past the last element)
  //   was: assert(val.first == val.second->getTargetAddress())
  if (loc != val.second->getTargetAddress())
  {
    RuntimeSystem::instance()->printMessage("#WARNING: corrected pointer target");

    loc = val.second->getTargetAddress();
  }

  m.insert( PointerManager::TargetToPointerMap::value_type(loc, val.second) );
}


PointerInfo::PointerInfo(Location s)
: source(s), target(nullAddr()), baseType(NULL)
{}

PointerInfo::PointerInfo(Location s, Location t, RsType * type)
: source(s), target(nullAddr()), baseType(type)
{
    setTargetAddress(t);
}


PointerInfo::~PointerInfo()
{
  // \pp ???

  // If still valid, invalidate the pointer
  if (!isNullAddr(target))
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


void PointerInfo::setTargetAddress(Location newAddr, bool doCheck)
{
    RuntimeSystem* rs = RuntimeSystem::instance();
    MemoryManager* mm = rs->getMemManager();
    const size_t   objsz = baseType->getByteSize();
    MemoryType*    newMem = mm->findContainingMem(newAddr, objsz);

    if (!newMem && !isNullAddr(newAddr)) //new address is invalid
    {
      std::stringstream ss;
      ss << "Tried to assign non allocated address 0x" << newAddr;
      VariablesType * var = getVariable();
      if (var)
          ss << " to pointer " << var->getName();
      else
          ss << " to pointer at address 0x" << source;

      ss << std::endl;

      target = nullAddr();
      rs->violationHandler(RuntimeViolation::INVALID_PTR_ASSIGN, ss.str());
      return;
    }

    // Don't update the target with type information just because a pointer
    // points there -- but do check to make sure that the pointer isn't
    // definitely illegal (e.g. an int pointer pointing to a known double).
    if (newMem)
    {
      // FIXME 2: This should really only check, and not merge
      assert(newMem->beginAddress() <= newAddr);
      newMem->checkAndMergeMemType(newAddr, baseType);
    }

    // if old target is valid
    if (!isNullAddr(target))
    {
      // has to hold, because checked when set
      assert(mm->findContainingMem(target, objsz) != NULL);

      // old and new address is valid
      if (newMem && doCheck)
      {
        mm->checkIfSameChunk(target, newAddr, baseType);
      }
    }

    target = newAddr;
}


void PointerInfo::print(std::ostream& os) const
{
    os << std::setw(6) << std::setfill('0') << getSourceAddress() << "\t"
       << std::setw(6) << std::setfill('0') << getTargetAddress() << "\t"
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
    PointerInfo * pi = new PointerInfo(sourceAddress, nullAddr(), type);
    std::pair<PointerSet::iterator, bool>  res = pointerInfoSet.insert(pi);
    if(!res.second)//pointer was already registered
    {
        delete pi;
        return;
    }

    insert(targetToPointerMap, TargetToPointerMap::value_type(nullAddr(), pi)) /* insert */;;
}


void PointerManager::createPointer(Location baseAddr, RsType * type, bool distributed)
{
    bool basetype_distributed = false;

    if (RsArrayType * at = dynamic_cast<RsArrayType*>(type))
    {
        // arrays are 'pseudo-pointers' whose target and source addresses are the
        // same, i.e.
        //  type x[ element_count ];
        //  &x == x         // this is true
        createDereferentiableMem(baseAddr, at->getBaseType());
        registerPointerChange( baseAddr, baseAddr, nocheck, check );

        basetype_distributed = distributed && ( dynamic_cast<RsArrayType*>(at->getBaseType()) != NULL );
    }
    else if (RsPointerType * pt = dynamic_cast<RsPointerType*>(type))
    {
        // If type is a pointer register it
        createDereferentiableMem(baseAddr, pt->getBaseType());
        basetype_distributed = distributed && ( dynamic_cast<RsArrayType*>(pt->getBaseType()) != NULL );
    }
    else
    {
      // plain types, and structures are never distributed
      assert(!distributed);
    }


    // If compound type, break down to basic types
    for (int i = 0; i < type->getSubtypeCount(); ++i)
    {
        Location l = add(baseAddr, type->getSubtypeOffset(i), distributed);

        createPointer(l, type->getSubtype(i), basetype_distributed );
    }

    // \pp \todo \distmem
}


void PointerManager::deletePointer( Location src, bool checks )
{
    PointerInfo    dummy(src);
    PointerSetIter i = pointerInfoSet.find(&dummy);

    assert(i != pointerInfoSet.end());
    PointerInfo*   pi = *i;

    // Delete from map
    bool res = removeFromRevMap(pi);
    assert(res);

    // Delete from set
    pointerInfoSet.erase(i);

    if( checks )
        checkForMemoryLeaks( pi );

    delete pi;
}


void PointerManager::deletePointerInRegion( MemoryType& mt )
{
    Location       from(mt.beginAddress());
    Location       to(mt.lastValidAddress());
    PointerInfo    objFrom(from);
    PointerInfo    objTo(to);
    PointerSetIter lb = pointerInfoSet.lower_bound(&objFrom);
    PointerSetIter ub = pointerInfoSet.upper_bound(&objTo  );

    // First store all pointer which have to be deleted
    // erase(lb, ub) doesn't work because the pointer need to be removed from map
    std::vector< PointerInfo* > toDelete(lb, ub);

    for ( unsigned int i=0; i<toDelete.size(); ++i )
    {
        PointerInfo*  curr = toDelete[i];
        Location      loc = curr->getTargetAddress();

        // check a pointer if the pointer points outside the region
        const bool    check_this_pointer = !mt.containsAddress(loc);

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

    registerPointerChange(src,target, checkPointerMove, checkMemLeaks);
}

void PointerManager::registerPointerChange( Location src, Location target, bool checkPointerMove, bool checkMemLeaks)
{
    PointerInfo    dummy(src);
    PointerSetIter i = pointerInfoSet.find(&dummy);

    // forgot to call createPointer?
    // call the overloaded registerPointerChange() instead which does not require createPointer()
    assert(i != pointerInfoSet.end());

    PointerInfo& pi = **i;
    Location     oldTarget = pi.getTargetAddress();
    bool         legal_move = true;

    if( checkPointerMove )
    {
        MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
        const size_t    bytesize = pi.getBaseType() -> getByteSize();

        legal_move = mm -> checkIfSameChunk(oldTarget, target, bytesize, RuntimeViolation::NONE );

        // This isn't a legal
        if( !legal_move )
        {
            RuntimeSystem*               rs = RuntimeSystem::instance();
            const RuntimeViolation::Type vioid = RuntimeViolation::INVALID_PTR_ASSIGN;

            if (rs->violationTypePolicy[vioid] != ViolationPolicy::InvalidatePointer)
            {
                // repeat check but throw invalid ptr assign
                mm -> checkIfSameChunk(oldTarget, target, bytesize, vioid );

                // shouldn't get here
                assert( false );
            }

            // Don't complain now, but invalidate the pointer so that subsequent
            // reads or writes without first registering a pointer change will
            // be treated as invalid
            pi.setTargetAddressForce( nullAddr() );
            insert(targetToPointerMap, TargetToPointerMap::value_type( nullAddr(), &pi )) /* insert */;

            // \pp this is a bug, as RTED changes data of a running program
            if ( !(rs->testingMode) ) zeroIntValueAt(pi.getSourceAddress());
        }
    }

    // Remove the pointer info from TargetToPointerMap
    bool res = removeFromRevMap(&pi);
    assert(res); // assert that an entry has been deleted

    if( legal_move ) {
        try {
            // set correct target
            pi.setTargetAddress(target);
        } catch(RuntimeViolation & vio) {
            // if target could not been set, then set pointer to null
            pi.setTargetAddressForce( nullAddr() );
            insert(targetToPointerMap, TargetToPointerMap::value_type(nullAddr(), &pi)) /* insert */;;
            throw vio;
        }
        // ...and insert it again with changed target
        insert(targetToPointerMap, TargetToPointerMap::value_type(target, &pi)) /* insert */;;
    }

    if( checkMemLeaks )
        checkForMemoryLeaks( oldTarget, pi.getBaseType()->getByteSize() );

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
    typedef TargetToPointerMap::const_iterator ConstIterator;

    bool found_leaks = false;
    MemoryManager * mm = RuntimeSystem::instance() -> getMemManager();
    MemoryType * mem = mm->findContainingMem( address, type_size );

    if(mem && (mem->howCreated() != akStack)) // no memory leaks on stack
    {
        // find other pointer still pointing to this region
        ConstIterator begin = targetRegionIterBegin( mem -> beginAddress() );
        ConstIterator end   = targetRegionIterBegin( mem -> endAddress() );

        // if nothing found create violation
        if (begin == end)
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

#if OBSOLETE_CODE
void PointerManager::checkPointerDereference( Location src, Location deref_addr)
{
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();

    PointerInfo dummy (src);
    PointerSetIter i = pointerInfoSet.find(&dummy);
    assert(i != pointerInfoSet.end());
    PointerInfo * pi = *i;

    mm->checkIfSameChunk(pi->getTargetAddress(),deref_addr,pi->getBaseType());
}
#endif /* OBSOLETE_CODE */

static inline
bool equalThreadId(const PointerManager::Location lhs, const PointerManager::Location rhs)
{
#ifdef WITH_UPC
  return lhs.thread_id == rhs.thread_id;
#else /* WITH_UPC */
  return true;
#endif /* WITH_UPC */
}

void PointerManager::invalidatePointerToRegion(MemoryType& mt)
{
    typedef TargetToPointerMap::iterator Iter;

    const Address lb = mt.beginAddress();
    const Address ub = mt.lastValidAddress();
    const Iter    aa = targetToPointerMap.lower_bound(lb);
    const Iter    zz = targetToPointerMap.upper_bound(ub);
    Location      limit = mt.endAddress();
    Location      nullLoc = nullAddr();
    const bool    distr = mt.isDistributed();

    for(Iter i = aa; i != zz; ++i)
    {
        // \pp !distr and threads!=thread seems to be an odd condition
        if ( !distr && !equalThreadId(aa->first, limit) ) continue;

        PointerInfo * pi = i->second;

        // assert that the object/value pointed does not cross the
        //   memory region boundary.
        assert(add(pi->getTargetAddress(), pi->getBaseType()->getByteSize(), distr) <= limit);

        pi->setTargetAddress( nullLoc );
        targetToPointerMap.erase(i);

        insert(targetToPointerMap, TargetToPointerMap::value_type(nullLoc, pi)) /* insert */;;
    }
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

PointerManager::TargetToPointerMap::const_iterator
PointerManager::targetRegionIterBegin(Location targetAddr) const
{
    return targetToPointerMap.lower_bound(targetAddr);
}

PointerManager::TargetToPointerMap::const_iterator
PointerManager::targetRegionIterEnd(Location targetAddr) const
{
    return targetToPointerMap.upper_bound(targetAddr);
}


void PointerManager::clearStatus()
{
    for(PointerSet::iterator it = pointerInfoSet.begin();
            it != pointerInfoSet.end(); ++it)
    {
        (*it)->setTargetAddressForce( nullAddr() ); // skip pointer checks on delete
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

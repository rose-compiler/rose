// vim:sta et:
#include <iomanip>
#include <cassert>
#include <iostream>
#include <algorithm>

#include "PointerManager.h"
#include "CppRuntimeSystem.h"
#include "MemoryManager.h"


// auxiliary classes and functions

static inline
bool equalThreadId(const Address& lhs, const Address& rhs)
{
#ifdef WITH_UPC
  return lhs.thread_id == rhs.thread_id;
#else /* WITH_UPC */
  return true || (&lhs == &rhs);
#endif /* WITH_UPC */
}


template <class _ForwardIterator, class _Predicate>
struct SequenceFilter
{
  typedef _ForwardIterator              iterator;
  typedef typename iterator::value_type value_type;
  typedef typename iterator::reference  reference;

  iterator       aa;
  const iterator zz;
  _Predicate     pred;

  SequenceFilter(iterator first, iterator last, _Predicate p)
  : aa(first), zz(last), pred(p)
  {
    if ((aa != zz) && !pred(*aa)) next();
  }

  bool empty() const { return aa == zz; }

  void next()
  {
    ++aa;
    aa = std::find_if(aa, zz, pred);
  }

  iterator  iter() { return aa; }
  reference elem() { return *aa; }
};

struct MemareaMembershipChecker
{
  const bool    distributed;
  Address       baseaddr;

  explicit
  MemareaMembershipChecker(const MemoryType& memtype)
  : distributed(memtype.isDistributed()), baseaddr(memtype.beginAddress())
  {}

  bool operator()(const PointerManager::TargetToPointerMap::value_type& entry) const
  {
    return distributed || equalThreadId(entry.first, baseaddr);
  }
};



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
    RuntimeSystem::instance().printMessage("#WARNING: corrected pointer target");

    loc = val.second->getTargetAddress();
  }

  m.insert( PointerManager::TargetToPointerMap::value_type(loc, val.second) );
}

struct LeakReporter
{
  const PointerManager&    pm;
  const PointerInfo* const pi;

  LeakReporter(const PointerManager& pointerManager, const PointerInfo* pinfo)
  : pm(pointerManager), pi(pinfo)
  {}

  void operator()(const MemoryType* mem) const
  {
    typedef PointerManager::TargetToPointerMap::const_iterator      ConstIterator;
    typedef SequenceFilter<ConstIterator, MemareaMembershipChecker> PointerFilter;

    if (!mem) return;

    ConstIterator aa = pm.targetRegionIterBegin( mem->beginAddress() );
    ConstIterator zz = pm.targetRegionIterBegin( mem->endAddress() );
    PointerFilter range(aa, zz, MemareaMembershipChecker(*mem));
    const bool    found_leaks = range.empty();

    if (found_leaks)
    {
        std::stringstream msg;

        msg << "Memory leak at: " << mem << std::endl;

        if (pi)
          msg <<  "  this pointer was modified: " << *pi << std::endl;
        else
          msg <<  "  because a pointer went out of scope." << std::endl;

        RuntimeSystem::instance().violationHandler(RuntimeViolation::MEM_WITHOUT_POINTER, msg.str());
    }
  }
};


static
const MemoryType*
findMem(Address address, size_t type_size)
{
    const MemoryManager& mm = rtedRTS(non_modifying()).getMemManager();
    const MemoryType*    mem = mm.findContainingMem( address, type_size );

    if (!mem || (mem->howCreated() & akNamedMemory)) return NULL;

    return mem;
}

PointerInfo::~PointerInfo()
{
  // \pp ???

  // If still valid, invalidate the pointer
  if (!isNullAddr(target))
     setTargetAddress(nullAddr());
}

const VariablesType*
PointerInfo::getVariable() const
{
    return rtedRTS(this).getStackManager().getVariable(source);
}

bool PointerInfo::operator< (const PointerInfo & other ) const
{
    return source < other.source;
}


void PointerInfo::setTargetAddress(Location newAddr, bool doCheck)
{
    MemoryManager& mm = rtedRTS(this).getMemManager();
    const size_t   objsz = baseType->getByteSize();
    MemoryType*    newMem = mm.findContainingMem(newAddr, objsz);

    if (!newMem && !isNullAddr(newAddr)) //new address is invalid
    {
      std::stringstream    ss;

      ss << "Tried to assign non allocated address 0x" << newAddr;
      if (const VariablesType* var = getVariable())
          ss << " to pointer " << var->getName();
      else
          ss << " to pointer at address 0x" << source;

      ss << std::endl;

      target = nullAddr();
      RuntimeSystem::instance().violationHandler(RuntimeViolation::INVALID_PTR_ASSIGN, ss.str());
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
      assert(mm.findContainingMem(target, objsz) != NULL);

      // old and new address is valid
      if (newMem && doCheck)
      {
        mm.checkIfSameChunk(target, newAddr, baseType);
      }
    }

    target = newAddr;
}


void PointerInfo::print(std::ostream& os) const
{
    os << std::setw(6) << std::setfill('0') << getSourceAddress() << "\t"
       << std::setw(6) << std::setfill('0') << getTargetAddress() << "\t"
       << getBaseType()->getName()
       << (getVariable() ? "\t" : "");
}

std::ostream& operator<< (std::ostream& os, const PointerInfo& m)
{
    m.print(os);
    return os;
}


// -------------------------- PointerManager -------------------------------


PointerManager::PointerSet::iterator
PointerManager::createDereferentiableMem(Location sourceAddress, const RsType& type)
{
    PointerInfo*                          pi = new PointerInfo(sourceAddress, nullAddr(), type);
    std::pair<PointerSet::iterator, bool> res = pointerInfoSet.insert(pi);

    // was the pointer already registered?
    if (!res.second)
    {
        // \pp not sure how this can happen ...
        if (diagnostics::warning())
        {
          std::stringstream msg;

          msg << "WARNING: pointer is already registered\n"
              << "         location: " << sourceAddress
              << "         type: " << type.getName() << std::endl;

          RuntimeSystem::instance().printMessage(msg.str());
        }

        delete pi;
        return res.first;
    }

    insert(targetToPointerMap, TargetToPointerMap::value_type(nullAddr(), pi)) /* insert */;
    return res.first;
}

void PointerManager::createTransientPointer(Location points_to, const RsType& t)
{
    // \todo get all pointers in t
    ez::unused(t);

    if (points_to == nullAddr()) return;

    transientPointers.push_back(TransientPointer(PointerInfo(nullAddr()), points_to));

    TransientPointer& transientPtr = transientPointers.back();
    Location          loc = rted_Addr(&transientPtr.second);

    transientPtr.first = PointerInfo(loc, points_to, t);
    insert(targetToPointerMap, TargetToPointerMap::value_type(points_to, &transientPtr.first));
}

struct MemRefDecrementer
{
  typedef PointerManager::TargetToPointerMap TargetToPointerMap;
  typedef PointerManager::TransientPointer   TransientPointer;

  TargetToPointerMap& mapTargetPtr;

  explicit
  MemRefDecrementer(TargetToPointerMap& mapTargPtr)
  : mapTargetPtr(mapTargPtr)
  {}

  void operator()(const TransientPointer& tp)
  {
    // if the memory does not exist anymore, then there is nothing to do
    if (tp.first.getTargetAddress() == nullAddr()) return;

    TargetToPointerMap::iterator zz = mapTargetPtr.end();
    TargetToPointerMap::iterator cand = mapTargetPtr.lower_bound(tp.second);

    while ((cand != zz) && (cand->second != &tp.first)) ++cand;

    assert(cand != zz);
    mapTargetPtr.erase(cand);
  }
};


void PointerManager::clearTransientPtr()
{
    assert(transientPointers.size() <= 1);
    std::for_each(transientPointers.begin(), transientPointers.end(), MemRefDecrementer(targetToPointerMap));

    transientPointers.clear();
}

bool PointerManager::checkTransientPtr()
{
    size_t szTransientPointerList = transientPointers.size();

    assert(szTransientPointerList <= 1);
    return (  (szTransientPointerList == 0)
           || (transientPointers.back().first.getTargetAddress() != nullAddr())
           );
}

void PointerManager::createPointer(Location baseAddr, const RsType* type, long blocksize)
{
    if (const RsPointerType* pt = dynamic_cast<const RsPointerType*>(type))
    {
        assert(pt->getSubtypeCount() == 0);

        // If type is a pointer register it
        createDereferentiableMem(baseAddr, *pt->getBaseType());
        return;
    }

    if (dynamic_cast<const RsArrayType*>(type) == NULL)
    {
        // plain types, and structures are never distributed
        blocksize = 0;
    }

    // If compound type, break down to basic types
    const int szSubtypes = type->getSubtypeCount();
    for (int i = 0; i < szSubtypes; ++i)
    {
        Location l = add(baseAddr, type->getSubtypeOffset(i), blocksize);

        createPointer(l, type->getSubtype(i), blocksize);
    }
}

void PointerManager::deletePointer(Location src, bool checkleak)
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

    if (checkleak)
    {
      const MemoryType* mem = findMem( pi->getTargetAddress(), pi->getBaseType()->getByteSize() );

      LeakReporter(*this, pi)(mem);
    }

    delete pi;
}


void PointerManager::deletePointerInRegion( const MemoryType& mt )
{
    Location       from(mt.beginAddress());
    Location       to(mt.lastValidAddress());
    PointerInfo    objFrom(from);
    PointerInfo    objTo(to);

    // \klocworkbug Iterator 'toDelete' retrieved for container 'this->pointerInfoSet'
    //              at line 284 is dereferenced when it can be equal to value returned
    //              by call to [r]end().
    // \pp          not sure what this refers to?
    PointerSetIter lb = pointerInfoSet.lower_bound(&objFrom);
    PointerSetIter ub = pointerInfoSet.upper_bound(&objTo  );

    // \todo use UPC blocking factor to determine whether an address is
    //       actually part of this allocation (if mt.allocKind & upcShared)

    // First store all pointers that have to be deleted
    // erase(lb, ub) doesn't work because the pointers need to be removed from map
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

void PointerManager::checkForMemoryLeaks( Location target, size_t len, const PointerInfo* culprit ) const
{
  const MemoryType* mem = findMem(target, len);

  if (mem && ((mem->howCreated() & akNamedMemory) == 0))
  {
    LeakReporter(*this, culprit)(mem);
  }
}


void PointerManager::registerPointerChange(Location src, Location target, const RsType& target_type, bool checkPointerMove)
{
    PointerInfo    dummy(src);
    PointerSetIter it = pointerInfoSet.find(&dummy);

    if (it != pointerInfoSet.end())
    {
        // if pointer exists already, make sure that baseTypes match
        // \note this was assert( (*it)->baseType = bt )
        //       after changing = to ==, the assumption does not hold
        //       assert( (*it)->baseType == bt )
        const RsType* base_type = (*it)->getBaseType();

        if (diagnostics::warning() && (base_type != &target_type))
        {
          std::stringstream msg;

          msg << "WARNING: type mismatch.\n"
              << "  original type = " << base_type->getName()
              << "  new type =      " << target_type.getName()
              << std::endl;

          RuntimeSystem::instance().printMessage(msg.str());

          (*it)->setBaseType(target_type); // \todo remove
        }
    }
    else
    {
        // \pp I do not understand, why we create pointer if it is
        //     previously unknown. Should not that be handled by createVariable
        if (diagnostics::warning())
        {
          std::stringstream msg;

          msg << "WARNING: create previously unregistered pointer\n"
              << "         location: " << src
              << "         type: " << target_type.getName() << std::endl;

          RuntimeSystem::instance().printMessage(msg.str());
        }

        // create the pointer
        it = createDereferentiableMem(src, target_type);
    }

    assert(it != pointerInfoSet.end());

    PointerInfo& pi = **it;
    Location     oldTarget = pi.getTargetAddress();
    bool         legal_move = true;

    if ( checkPointerMove )
    {
        const MemoryManager& mm = rtedRTS(this).getMemManager();
        const size_t         bytesize = pi.getBaseType() -> getByteSize();

        legal_move = mm.checkIfSameChunk(oldTarget, target, bytesize, RuntimeViolation::NONE );

        // This isn't a legal
        if( !legal_move )
        {
            RuntimeSystem&               rs = RuntimeSystem::instance();
            const RuntimeViolation::Type vioid = RuntimeViolation::INVALID_PTR_ASSIGN;

            if (rs.vioPolicy(vioid) != ViolationPolicy::InvalidatePointer)
            {
                // repeat check but throw invalid ptr assign
                mm.checkIfSameChunk( oldTarget, target, bytesize, vioid );

                // shouldn't get here
                assert( false );
            }

            // Don't complain now, but invalidate the pointer so that subsequent
            // reads or writes without first registering a pointer change will
            // be treated as invalid
            pi.setTargetAddressForce( nullAddr() );
            insert(targetToPointerMap, TargetToPointerMap::value_type( nullAddr(), &pi )) /* insert */;

            // \pp this is a bug, as RTED changes data of a running program
            if ( !(rs.testing()) ) zeroIntValueAt(pi.getSourceAddress());
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
            insert(targetToPointerMap, TargetToPointerMap::value_type(nullAddr(), &pi)) /* insert */;
            throw vio;
        }
        // ...and insert it again with changed target
        insert(targetToPointerMap, TargetToPointerMap::value_type(target, &pi)) /* insert */;
    }

    checkForMemoryLeaks( oldTarget, pi );
}



void PointerManager::checkIfPointerNULL( const void* pointer) const
{
  if (pointer==NULL)
  {
    std::stringstream ss;
    ss << "Accessing (This) NULL Pointer: " << std::endl;

    RuntimeSystem::instance().violationHandler(RuntimeViolation::INVALID_WRITE, ss.str());
  }
}

void PointerManager::invalidatePointerToRegion(const MemoryType& mt)
{
    typedef TargetToPointerMap::iterator                   Iter;
    typedef SequenceFilter<Iter, MemareaMembershipChecker> PointerFilter;

    const Address  lb = mt.beginAddress();
    const Address  ub = mt.lastValidAddress();
    const Iter     aa = targetRegionIterBegin(lb);
    const Iter     zz = targetRegionIterEnd(ub);
    PointerFilter  range(aa, zz, MemareaMembershipChecker(mt));
    const Location nullLoc = nullAddr();
    const long     blocksz = mt.blockSize();

    while (!range.empty())
    {
      PointerInfo* pi = range.elem().second;

      assert(add(pi->getTargetAddress(), pi->getBaseType()->getByteSize()-1, blocksz) <= ub);

      // \todo check if setTargetAddress does not already erase the element
      //       from the map
      pi->setTargetAddress( nullLoc );

      TargetToPointerMap::iterator toErase = range.iter();
      range.next(); //Once we erase the iterator it's invalid and we can't call next()
      targetToPointerMap.erase( toErase );
      insert(targetToPointerMap, TargetToPointerMap::value_type(nullLoc, pi)) /* insert */;
    }
}


bool PointerManager::removeFromRevMap(PointerInfo* pi)
{
    typedef TargetToPointerMap::iterator MapIter;

    assert(!targetToPointerMap.empty());

    std::pair<MapIter,MapIter> range = targetToPointerMap.equal_range(pi->getTargetAddress());

    // \note no need to filter, all entries point to a specific target
    for (MapIter i = range.first; i !=  range.second; ++i)
    {
        if( i->second == pi)
        {
            targetToPointerMap.erase(i);
            return true;
        }
    }

    return false;
}


PointerManager::PointerSetIter
PointerManager::sourceRegionIter(Location sourceAddr) const
{
    PointerInfo dummy(sourceAddr);
    return pointerInfoSet.lower_bound(&dummy);
}

void PointerManager::clearStatus()
{
    for (PointerSet::iterator it = pointerInfoSet.begin(); it != pointerInfoSet.end(); ++it)
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

void PointerManager::printPointersToThisChunk(std::ostream& os, const MemoryType& mt) const
{
    typedef TargetToPointerMap::const_iterator                      ConstIterator;
    typedef SequenceFilter<ConstIterator, MemareaMembershipChecker> PointerFilter;

    ConstIterator aa = targetRegionIterBegin(mt.beginAddress());
    ConstIterator zz = targetRegionIterEnd(mt.lastValidAddress());
    PointerFilter range(aa, zz, MemareaMembershipChecker(mt));

    // \todo \upcptr filter out pointers that do not belong here
    if (!range.empty())
    {
        os << "\tPointer to this chunk: ";
        while (!range.empty())
        {
            if (const VariablesType* vt = range.elem().second->getVariable())
            {
                os << "\t" << vt->getName() << " ";
            }
            else
            {
                os << "\t" << "Addr: " << range.elem().second->getSourceAddress() << " ";
            }

            range.next();
        }

        os <<std::endl;
    }
}

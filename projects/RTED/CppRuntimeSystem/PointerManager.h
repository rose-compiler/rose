#ifndef POINTERMANAGER_H_
#define POINTERMANAGER_H_

#include <map>
#include <set>
#include <vector>
#include <list>
#include <iosfwd>

#include "Util.h"
#include "TypeSystem.h"

#include "ptrops.h"
#include "ptrops_operators.h"
#include "rted_typedefs.h"

class VariablesType;
class MemoryType;

/**
 * Stores infos for a dereferentiable memory region (mostly but not always a pointer)
 * Example char ** b; for this case we have two PointerInfos, one for c, and one for *c.
 */
struct PointerInfo
{
        typedef Address Location;

        Location      getSourceAddress() const { return source; }
        Location      getTargetAddress() const { return target; }
        const RsType* getBaseType()      const { return baseType; }

        // \pp \todo remove
        void setBaseType(const RsType& newtype){ baseType = &newtype; }

        /// Checks if there is a variable registered for sourceAddress
        /// if not @c NULL is returned
        const VariablesType* getVariable() const;

        /// Less operator uses sourceAddress (should be unique)
        bool operator< (const PointerInfo& other) const;

        void print(std::ostream& os) const;

        PointerInfo(Location loc, Location points_to, const RsType& t)
        : source(loc), target(nullAddr()), baseType(&t)
        {
          setTargetAddress(points_to);
        }

        /// Used only for creating DummyObject to search set
        explicit
        PointerInfo(Location s)
        : source(s), target(nullAddr()), baseType(NULL)
        {}

        ~PointerInfo();

        void setTargetAddress(Location t, bool checks=false);
        void setTargetAddressForce(Location t) { target = t; }
    private:
        Location      source;   ///< where pointer is stored in memory
        Location      target;   ///< where pointer points to
        const RsType* baseType; ///< the base type of the pointer i.e. type of target
};

std::ostream& operator<< (std::ostream& os, const PointerInfo& m);


/**
 * Keeps track of all dereferentiable memory regions.  It is expected that
 * PointerManager is used indirectly via the API for RuntimeSystem.
 */
//~
//~ /// \brief   represents a pointer that does not have a named memory location
//~ ///          (i.e., pointers returned from functions).
//~ /// \details The memory returned from a function cannot be reclaimed
//~ ///          until the next statement level (does not necessarily have to
//~ ///          be the call site) is executed.
//~ ///          Consider an example int* foo() { int* p = new int; return p; }
//~ ///          bar(foo()); /* assuming int bar(int* p) { return *p; } */
//~ ///          The body of bar may be instrumented with a transient value clean-up
//~ ///          statement. This does not do any harm (except for introducing a little
//~ ///          source location inaccuracy), because the memory pointed
//~ ///          to by the return value is referenced by bar's p.
//~ struct TransientPointer : PointerInfo
//~ {
    //~ TransientPointer();
//~
    //~ TransientPointer(Location where, Location points_to, RsType& t)
    //~ : PointerInfo(where, points_to, t)
    //~ {}
//~
    //~ TransientPointer(const TransientPointer& orig)
    //~ : PointerInfo(rted_Addr(&ptr), orig.getTargetAddress(), *orig.getBaseType()), ptr(reinterpret_cast<const char*>(this))
    //~ {}
//~
    //~ TransientPointer& operator=(const TransientPointer& rhs)
    //~ {
      //~ target = rhs.target;
      //~ baseType = rhs.baseType;
//~
      //~ return *this;
    //~ }
//~
  //~ private:
    //~ const char* ptr;  ///< so we have an address for the pointer
                      //~ ///  since we do not need to use it, this could be
                      //~ ///  static in case that memory becomes a issue
//~ };

struct PointerManager
{
        typedef Address                                  Location;
        typedef std::set<PointerInfo*, PointerCompare>   PointerSet;
        typedef PointerSet::const_iterator               PointerSetIter;
        typedef std::multimap<Location, PointerInfo*>    TargetToPointerMap;

        // \todo check if PointerInfo is sufficient ...
        typedef std::pair<PointerInfo, Address>          TransientPointer;

        // TransientPointerList must not relocate data
        typedef std::list<TransientPointer>              TransientPointerList;

        static const bool check = true;
        static const bool nocheck = !check;

        /// Registers a memory region from sourceAddress to sourceAddress+sizeof(void*)
        /// which stores another address
        /// second parameter specifies the type of the target
        PointerSet::iterator
        createDereferentiableMem(Location sourceAddress, const RsType& targetType);

        /// Call this function if a class was instantiated
        /// it iterates over the subtypes and if they are pointer or arrays
        /// they get registered as dereferentiable mem-regions
        /// @param classBaseAddr  the address where the class was instantiated
        /// @param type class type
        void createPointer(Location classBaseAddr, const RsType* type, long blocksz);

        /// Delete a registered pointer
        void deletePointer(Location sourceAddress, bool checkleaks);

        /// Deletes all pointer within the region defined by mt
        /// @param delaychecks, true iff the leakchecks are carried out delayed
        void deletePointerInRegion( const MemoryType& mt );

        /// Registers that targetAddress is stored at sourceAddress, and the type of targetAddress
        /// sourceAddress has to be registered first with createPointer
        /// if the pointer is not found, create a new one
        void registerPointerChange(Location sourceAddress, Location targetAddress, const RsType& baseType, bool checks);

#if OBSOLETE_CODE
        /// Registers that targetAddress is stored at sourceAddress, and the type of targetAddress
        /// sourceAddress has to be registered first with createPointer
        // void registerPointerChange( Location sourceAddress, Location targetAddress, bool checkPointerMove, bool checkMemLeaks);
#endif /* OBSOLETE_CODE */

#if UNUSED_CODE
        /// Behaves like registerPointerChange(sourceAddress,deref_address,true), but after the
        /// function the same targetAddress is registered for the pointer,
        /// Example: *(p++)  call registerPointerChange()
        ///          *(p+1)  call checkPointerDereference()
        void checkPointerDereference( Location sourceAddress, Location derefed_address );
#endif /* UNUSED_CODE */

        void checkIfPointerNULL( const void* pointer) const;

        /// Invalidates all "Pointer" i.e. dereferentiable memory regions
        /// point in memory chunk defined by mt
        void invalidatePointerToRegion( const MemoryType& mt );

        /// Use this to get pointerInfos which have sourceAddr in a specific region
        /// to iterate over region a1 until a2 (where a2 is exclusive!) use
        /// for(PointerSetIter i = sourceRegionIter(a1); i!= sourceRegionIter(a2); ++i)
        ///     PointerInfo * info = *i;
        PointerSetIter sourceRegionIter(Location sourceAddr) const;

        /// Use this to get pointerInfos which have targetAddr in a specific region
        /// to iterate over region a1 until a2 (where a2 is exclusive!) use
        /// for(TargetToPointerMapIter i = targetRegionIterBegin(a1); i!= targetRegionIterEnd(a2); ++i)
        ///     PointerInfo * info = i->second; (i->first is the targetAddr)
        TargetToPointerMap::const_iterator targetRegionIterBegin(Location targetAddr) const
        {
          return targetToPointerMap.lower_bound(targetAddr);
        }

        /// \overload
        TargetToPointerMap::iterator targetRegionIterBegin(Location targetAddr)
        {
          return targetToPointerMap.lower_bound(targetAddr);
        }

        TargetToPointerMap::const_iterator targetRegionIterEnd(Location targetAddr) const
        {
            return targetToPointerMap.upper_bound(targetAddr);
        }

        TargetToPointerMap::iterator targetRegionIterEnd(Location targetAddr)
        {
            return targetToPointerMap.upper_bound(targetAddr);
        }

        TargetToPointerMap::const_iterator targetBegin() const
        {
          return targetToPointerMap.begin();
        }

        TargetToPointerMap::const_iterator targetEnd() const
        {
          return targetToPointerMap.end();
        }

        size_t targetSize() const
        {
          return targetToPointerMap.size();
        }

        const PointerSet& getPointerSet() const { return pointerInfoSet; }

        /// Print status to a stream
        void print(std::ostream& os) const;

        /// Print pointers pointing into @mt on the stream @os
        void printPointersToThisChunk(std::ostream& os, const MemoryType& mt) const;

        /// clears status for debugging purposes
        void clearStatus();

#if OBSOLETE_CODE
        /// checks whether a pointer to heap memory exists
        void leakCheckDelayed();

        /// stores scope info for delayed checks
        void delayedScopeExit(const SourceInfo& si);
#endif /* OBSOLETE_CODE */

        /// \brief stores the pointer info for the transient pointer
        void createTransientPointer(Location points_to, const RsType& t);

        /// \brief   removes all transient pointers from the list
        ///          and checks for remaining memory leaks
        /// \details this function can only be called AFTER the transient pointer
        ///          was assigned to another pointer (or we can assume that the
        ///          memory reference vanished).
        void clearTransientPtr();

        /// \brief returns the number of transient pointers
        size_t transientPtrSize() { return transientPointers.size(); }

        /// \brief checks 0 or 1 transient pointers for deallocated memory
        bool checkTransientPtr();

        /// \brief   Checks to see if some pointer is still pointing to target
        /// \param   target
        /// \param   len size of memory chunk
        /// \param   culprit if not NULL, @culprit will be reported as the last pointer
        ///          to that memory chunk.
        /// \details Note that this will give false positives for
        ///          memory chunks, pointers to which are "computable", e.g. if the user
        ///          stores some fixed offset to its address.  Avoiding such false
        ///          positives will require data flow analysis on our part.
        void checkForMemoryLeaks( Location target, size_t len, const PointerInfo* culprit = 0 ) const;
    protected:
        /// Removes the a PointerInfo from targetToPointerMap
        /// @return false if not found in map
        bool removeFromRevMap(PointerInfo * p);

#if OBSOLETE_CODE
        /// Checks to see if pointer_being_removed was the last pointer pointing
        /// to some memory chunk.
        void checkForMemoryLeaks( const PointerInfo& pi ) const
        {
            checkForMemoryLeaks( pi.getTargetAddress(), pi );
        }
#endif /* OBSOLETE_CODE */

        /// Checks to see if any pointer exists that points to address, and
        /// whose base type is of size type_size.  If not, a violation is
        /// raised. @pi is reported as the source of the memory
        /// leak.
        void checkForMemoryLeaks(Location address, const PointerInfo& pi) const
        {
          checkForMemoryLeaks(address, pi.getBaseType()->getByteSize(), &pi);
        }

        PointerSet                pointerInfoSet;      ///< contains all pointers (aka dereferential memory)

        TargetToPointerMap        targetToPointerMap;  ///< Map to get all pointer which point to a specific target address
                                                       ///  maps targetAddress -> Set of PointerInfos

        TransientPointerList      transientPointers;
};

#endif

#ifndef POINTERMANAGER_H_
#define POINTERMANAGER_H_

#include "Util.h"
#include <map>
#include <set>

#include "rted_typedefs.h"

class RsType;
class VariablesType;

class PointerManager;

/**
 * Stores infos for a dereferentiable memory region (mostly but not always a pointer)
 * Example char ** b; for this case we have two PointerInfos, one for c, and one for *c.
 */
class PointerInfo
{
    public:
        typedef const char* Location;

        friend class PointerManager; // because of protected constructors

        Location getSourceAddress() const { return source; }
        Location getTargetAddress() const { return target; }
        RsType * getBaseType()      const { return baseType; }


        /// Checks if there is a variable registered for sourceAddress
        /// if not @c NULL is returned
        VariablesType * getVariable() const;

        /// Less operator uses sourceAddress (should be unique)
        bool operator< (const PointerInfo & other) const;

        void print(std::ostream & os) const;

    protected:
        PointerInfo(Location source, Location target, RsType * type);

        // Use only for creating DummyObject to search set
        explicit
        PointerInfo(Location source);

        ~PointerInfo();


        void setTargetAddress(Location t, bool checks=false);
        void setTargetAddressForce(Location t) { target = t; }


        Location source;   ///< where pointer is stored in memory
        Location target;   ///< where pointer points to
        RsType *      baseType; ///< the base type of the pointer i.e. type of target
};

std::ostream& operator<< (std::ostream& os, const PointerInfo& m);


/**
 * Keeps track of all dereferentiable memory regions.  It is expected that
 * PointerManager is used indirectly via the API for RuntimeSystem.
 */
class PointerManager
{
    public:
        static const bool check = true;
        static const bool nocheck = !check;

        typedef PointerInfo::Location Location;

        /// Registers a memory region  from sourceAddress to sourceAddress+sizeof(void*)
        /// which stores another address
        /// second parameter specifies the type of the target
        void createDereferentiableMem(Location sourceAddress, RsType * targetType);

        /// Call this function if a class was instantiated
        /// it iterates over the subtypes and if they are pointer or arrays
        /// they get registered as dereferentiable mem-regions
        /// @param classBaseAddr  the address where the class was instantiated
        /// @param type class type
        void createPointer(Location classBaseAddr, RsType * type);

        /// Delete a registered pointer
        void deletePointer(Location sourceAddress, bool checks);

        /// Deletes all pointer with startaddress>=from and startaddress < to
        /// @param checks   if true, also test for memory leaks
        void deletePointerInRegion( Location from, Location to, bool checks);

        /// Registers that targetAddress is stored at sourceAddress, and the type of targetAddress
        /// sourceAddress has to be registered first with createPointer
        void registerPointerChange( Location sourceAddress, Location targetAddress, bool checkPointerMove, bool checkMemLeaks);

        /// This function behaves like the previous implementation
        /// except when no pointer was found at this address it creates a new one with the given baseType
        void registerPointerChange( Location sourceAddress, Location targetAddress, RsType * baseType, bool checks, bool checkMemLeaks);

        /// Behaves like registerPointerChange(sourceAddress,deref_address,true), but after the
        /// function the same targetAddress is registered for the pointer,
        /// Example: *(p++)  call registerPointerChange()
        ///          *(p+1)  call checkPointerDereference()
        void checkPointerDereference( Location sourceAddress, Location derefed_address );
        void checkIfPointerNULL( void* pointer);

        /// Invalidates all "Pointer" i.e. dereferentiable memory regions
        /// point in given memory chunk
        /// @param remove if true the pointer is only set to null
        ///               if false they are deleted
        void invalidatePointerToRegion(Location from, Location to, bool remove=false);


        typedef std::set<PointerInfo*, PointerCompare> PointerSet;
        typedef PointerSet::const_iterator PointerSetIter;

        /// Use this to get pointerInfos which have sourceAddr in a specific region
        /// to iterate over region a1 until a2 (where a2 is exclusive!) use
        /// for(PointerSetIter i = sourceRegionIter(a1); i!= sourceRegionIter(a2); ++i)
        ///     PointerInfo * info = *i;
        PointerSetIter sourceRegionIter(Location sourceAddr);


        typedef std::multimap<Location,PointerInfo* > TargetToPointerMap;
        typedef TargetToPointerMap::const_iterator TargetToPointerMapIter;

        /// Use this to get pointerInfos which have targetAddr in a specific region
        /// to iterate over region a1 until a2 (where a2 is exclusive!) use
        /// for(TargetToPointerMapIter i = targetRegionIterBegin(a1); i!= targetRegionIterEnd(a2); ++i)
        ///     PointerInfo * info = i->second; (i->first is the targetAddr)
        TargetToPointerMapIter targetRegionIterBegin(Location targetAddr);
        TargetToPointerMapIter targetRegionIterEnd (Location targetAddr);


        const PointerSet & getPointerSet() const { return pointerInfoSet; }

        /// Print status to a stream
        void print(std::ostream & os) const;


        void clearStatus();

    protected:
        /// Removes the a PointerInfo from targetToPointerMap
        /// @return false if not found in map
        bool removeFromRevMap(PointerInfo * p);

        /// Checks to see if pointer_being_removed was the last pointer pointing
        /// to some memory chunk.  Note that this will give false positives for
        /// memory chunks, pointers to which are "computable", e.g. if the user
        /// stores some fixed offset to its address.  Avoiding such false
        /// positives will require data flow analysis on our part.
        bool checkForMemoryLeaks( PointerInfo* pointer_begin_removed );
        /// Checks to see if any pointer exists that points to address, and
        /// whose base type is of size type_size.  If not, a violation is
        /// raised.  If a violation is raised and pointer_to_blame is specified,
        /// the destruction of its value is reported as the source of the memory
        /// leak.
        bool checkForMemoryLeaks(
            Location       address,
            size_t          type_size,
            PointerInfo*    pointer_to_blame = NULL);


        PointerSet pointerInfoSet;

        /// Map to get all pointer which point to a specific target address
        /// maps targetAddress -> Set of PointerInfos
        TargetToPointerMap targetToPointerMap;
};


#endif

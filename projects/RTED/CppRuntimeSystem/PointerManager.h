#ifndef POINTERMANAGER_H_
#define POINTERMANAGER_H_

#include "Util.h"
#include <map>
#include <set>

class RsType;
class VariablesType;

class PointerManager;

/**
 * Stores infos for a dereferentiable memory region (mostly but not always a pointer)
 * Example char ** b; for this case we have two PointerInfos, one for c, and one for *c
 */
class PointerInfo
{
    public:
        friend class PointerManager; // because of protected constructors

        addr_type getSourceAddress() const { return source; }
        addr_type getTargetAddress() const { return target; }
        RsType *  getBaseType()      const { return baseType; }


        /// Checks if there is a variable registered for sourceAddress
        /// if not NULL is returned
        VariablesType * getVariable() const;

        /// Less operator uses sourceAddress (should be unique)
        bool operator< (const PointerInfo & other) const;

        void print(std::ostream & os) const;

    protected:
        PointerInfo(addr_type source, addr_type target,RsType * type);

        // Use only for creating DummyObject to search set
        PointerInfo(addr_type source);

        ~PointerInfo();


        void setTargetAddress(addr_type t, bool checks=false);
        void setTargetAddressForce(addr_type t) { target = t; }


        addr_type source;   ///< where pointer is stored in memory
        addr_type target;   ///< where pointer points to
        RsType *  baseType; ///< the base type of the pointer i.e. type of target
};

std::ostream& operator<< (std::ostream &os, const PointerInfo & m);


/**
 * Keeps track of all dereferentiable memory regions
 */
class PointerManager
{
    public:
        /// Registers a memory region  from sourceAddress to sourceAddress+sizeof(void*)
        /// which stores another address
        /// second parameter specifies the type of the target
        void createDereferentiableMem(addr_type sourceAddress, RsType * targetType);

        /// Call this function if a class was instantiated
        /// it iterates over the subtypes and if they are pointer or arrays
        /// they get registered as dereferentiable mem-regions
        /// @param classBaseAddr  the address where the class was instantiated
        /// @param type class type
        void createPointer(addr_type classBaseAddr, RsType * type);

        /// Delete a registered pointer
        void deletePointer(addr_type sourceAddress, bool checks = false );

        /// Deletes all pointer with startaddress>=from and startaddress < to
        /// @param checks   if true, also test for memory leaks
        void deletePointerInRegion( addr_type from, addr_type to, bool checks = false );

        /// Registers that targetAddress is stored at sourceAddress, and the type of targetAddress
        /// sourceAddress has to be registered first with createPointer
        void registerPointerChange( addr_type sourceAddress, addr_type targetAddress, bool checkPointerMove=false, bool checkMemLeaks=true);

        /// This function behaves like the previous implementation
        /// except when no pointer was found at this address it creates a new one with the given baseType
        void registerPointerChange( addr_type sourceAddress, addr_type targetAddress, RsType * baseType, bool checks=false, bool checkMemLeaks=true);

        /// Behaves like registerPointerChange(sourceAddress,deref_address,true), but after the
        /// function the same targetAddress is registered for the pointer,
        /// Example: *(p++)  call registerPointerChange()
        ///          *(p+1)  call checkPointerDereference()
        void checkPointerDereference( addr_type sourceAddress, addr_type derefed_address );


        /// Invalidates all "Pointer" i.e. dereferentiable memory regions
        /// point in given memory chunk
        /// @param remove if true the pointer is only set to null
        ///               if false they are deleted
        void invalidatePointerToRegion(addr_type from, addr_type to, bool remove=false);


        typedef std::set<PointerInfo*,PointerCmpFunc<PointerInfo> > PointerSet;
        typedef PointerSet::const_iterator PointerSetIter;

        /// Use this to get pointerInfos which have sourceAddr in a specific region
        /// to iterate over region a1 until a2 (where a2 is exclusive!) use
        /// for(PointerSetIter i = sourceRegionIter(a1); i!= sourceRegionIter(a2); ++i)
        ///     PointerInfo * info = *i;
        PointerSetIter sourceRegionIter(addr_type sourceAddr);


        typedef std::multimap<addr_type,PointerInfo* > TargetToPointerMap;
        typedef TargetToPointerMap::const_iterator TargetToPointerMapIter;

        /// Use this to get pointerInfos which have targetAddr in a specific region
        /// to iterate over region a1 until a2 (where a2 is exclusive!) use
        /// for(TargetToPointerMapIter i = targetRegionIterBegin(a1); i!= targetRegionIterEnd(a2); ++i)
        ///     PointerInfo * info = i->second; (i->first is the targetAddr)
        TargetToPointerMapIter targetRegionIterBegin(addr_type targetAddr);
        TargetToPointerMapIter targetRegionIterEnd (addr_type targetAddr);


        const PointerSet & getPointerSet() const { return pointerInfoSet; }

        /// Print status to a stream
        void print(std::ostream & os) const;


        void clearStatus();

    protected:
        /// Removes the a PointerInfo from targetToPointerMap
        /// @return false if not found in map
        bool removeFromRevMap(PointerInfo * p);

        // TODO 1 djh: comment
        bool checkForMemoryLeaks( PointerInfo* pointer_begin_removed );
        bool checkForMemoryLeaks(
            addr_type       address,
            size_t          type_size,
            PointerInfo*    pointer_to_blame = NULL);


        PointerSet pointerInfoSet;

        /// Map to get all pointer which point to a specific target address
        /// maps targetAddress -> Set of PointerInfos
        TargetToPointerMap targetToPointerMap;
};


#endif

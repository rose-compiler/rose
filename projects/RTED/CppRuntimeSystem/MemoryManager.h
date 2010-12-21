// vim:et sta sw=4 ts=4
#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H


#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cassert>
#include <iomanip>

#include "CStdLibManager.h"
#include "Util.h"

#include "ptrops.h"
#include "ptrops_operators.h"


class RuntimeSystem;
class VariablesType;
class RsType;


/**
 * Keeps track of information about memory, such as address, size, and known
 * type information.  It is generally unnecessary for users to handle MemoryType
 * objects directly.
 *
 * Most uses of the MemoryManager can be handled indirectly via RuntimeSystem.
 */
class MemoryType
{
    public:
        typedef std::map<size_t, RsType*> TypeInfoMap;
        typedef TypeInfoMap::iterator     TiIter;


        MemoryType(MemoryAddress addr, size_t size, bool onStack, bool fromMalloc, const SourcePosition & pos);
        MemoryType(MemoryAddress addr, size_t size, bool onStack, bool fromMalloc, const std::string & file, int line1, int line2);

        // constructor which initialized only the address, used for comparison purposes
        // MemoryType(MemoryAddress addr, bool onStack = false);

        ~MemoryType();

        /// Checks if an address lies in this memory chunk
        bool containsAddress(MemoryAddress addr);
        /// Checks if a memory area is part of this allocation
        bool containsMemArea(MemoryAddress addr, size_t size);
        /// Checks if this MemoryType overlaps another area
        bool overlapsMemArea(MemoryAddress queryAddr, size_t querySize);


        /// Less operator uses startAdress
        bool operator< (const MemoryType & other) const;


        MemoryAddress          getAddress() const { return startAddress; }
        size_t                 getSize()    const { return size; }
        const SourcePosition & getPos()     const { return allocPos; }
        bool                   isOnStack()  const { return onStack; }
        bool                   wasFromMalloc() const { return fromMalloc; }

        void                   resize( size_t size );

        /// Tests if a part of memory is initialized
        bool  isInitialized(size_t offsetFrom, size_t offsetTo) const;

        /// Initialized a part of memory
        void  initialize   (size_t offsetFrom, size_t offsetTo) ;

        /// Returns "Initialized" "Not initialized" or "Partially initialized"
        /// only for display purposes
        std::string getInitString() const;


        /// Prints info about this allocation
        void print(std::ostream & os) const;
        /// Prints information to stderr
        void print() const;

        template<typename T>
        T * readMemory(size_t offset)
        {
            assert(offset<0 && offset+sizeof(T) >= size);
            assert(isInitialized(offset,offset+sizeof(T)));

            char * charAddress = static_cast<char*>(startAddress);
            charAddress += offset;
            return static_cast<T*>(charAddress);
        }

        /// This functions checks and registers typed access to memory
        /// the first time memory is accessed with a specific type,
        /// this type is associated with this mem-region,
        /// and if its later on accessed with a different type, a violation is reported
        /// however it is possible to access the mem-region later with "containing" types
        /// f.e. first access with int-pointer, then with struct pointer, which as as first member an int
        void registerMemType(size_t offset, RsType * type);

        /**
         * As @see registerMemType, except that memory is not checked.  The type
         * at @c offset is forced to be @c type.
         */
        void forceRegisterMemType(size_t offset, RsType* type );

        /**
         * As @see registerMemType, except that memory is only checked and
         * merged, i.e. besides merging, no new types will be registered.
         *
         * @return  @c true @b iff a merge occurred.
         */
        bool checkAndMergeMemType(size_t offset, RsType * type);


        /// Returns the RsType, or the CONTAINING ARRAY type which is associated with that offset
        /// to distinguish between nested types (class with has members of other classes)
        /// an additional size parameter is needed
        /// if no TypeInfo is found, null is returned
        RsType* getTypeAt(size_t offset, size_t size);

        const TypeInfoMap & getTypeInfoMap() const { return typeInfo; }


    private:
        typedef std::pair<TiIter,TiIter> TiIterPair;


        void insertType(MemoryAddress offset, RsType * type);

        const MemoryAddress startAddress; ///< address where memory chunk starts
        size_t              size;         ///< Size of allocation
        std::vector<bool>   initialized;  ///< stores for every byte if it was initialized
        bool                onStack;      ///< Whether the memory lives on the stack or not (i.e. on the heap)
        bool                fromMalloc;   ///< Whether the memory was allocated via a call to malloc (vice new)
        SourcePosition      allocPos;     ///< Position in source file where malloc/new was called


        /// Determines all typeinfos which intersect the defined offset-range [from,to)
        /// "to" is exclusive i.e. typeInfos with startOffset==to are not included
        TiIterPair getOverlappingTypeInfos(size_t from, size_t to);

        /// A entry in this map means, that on offset <key> is stored the type <value>
        TypeInfoMap typeInfo;

        /// \brief   Computes an @c RsCompoundType for offset, size pairs for
        ///          which a type could not be found. The compound type will
        ///          have as much information as possible filled in. A compound
        ///          type is not necessary a class -- it could still be an
        ///          array (e.g. int at offset 0, int at offset 20).
        ///
        /// \return  a pointer to the newly constructed @c RsCompoundType. This
        ///          object will be on the heap and callers are responsible for
        ///          deleting it.
        RsType* computeCompoundTypeAt(size_t offset, size_t size);

        /// if the entry at (iter-1) overlaps with from, use (iter-1)
        ///   as the start of the range.
        TiIter adjustPosOnOverlap(TiIter iter, size_t from);

        friend class MemoryManager;
};
std::ostream& operator<< (std::ostream &os, const MemoryType & m);



/**
 * \class MemoryManager
 * \brief MemoryManager tracks allocated memory and known type information.
 *
 * With the exception of bounds checking via @ref checkIfSameChunk, MemoryManager is
 * expected to be used indirectly, via calls to functions of RuntimeSystem.
 */
class MemoryManager
{
    public:
        MemoryManager();

        /// Destructor checks if there are still allocations which are not freed
        ~MemoryManager();

        /// Adds new allocation, the MemoryType structure is freed by this class
        void allocateMemory(const MemoryType& alloc);

        /// Frees allocated memory, throws error when no allocation is managed at this addr
        void freeMemory(MemoryAddress addr, bool onStack=false, bool fromMalloc = false);


        /// Prints information about all currently allocated memory areas
        void print(std::ostream & os) const;

        /// Check if memory region is allocated and initialized
        /// @param size     size=sizeof(DereferencedType)
        void checkRead  (MemoryAddress addr, size_t size, RsType * t=NULL);

        /// Checks if memory at position can be safely written, i.e. is allocated
        /// if true it marks that memory region as initialized
        /// that means this function should be called on every write!
        void checkWrite (MemoryAddress addr, size_t size, RsType * t=NULL);


        /**
         * @return @b true if the memory region containing
         * @c addr @c.. @c addr+size is initialized. */
        bool  isInitialized(MemoryAddress addr, size_t size);


        /// This check is intended to detect array out of bounds
        /// even if the plain memory access is legal (
        /// Example: struct{ int arr[10]; int i}; ) -> detect s.arr[11] as error
        /// The check is done if a pointer changes via arithmetic or on array access
        /// @param a1 address of the pointer before (or base of array); in example &arr[0]
        /// @param a2 the new pointer target address, or derefed addr; in example &arr[11]
        /// @return true if both addresses are from the same chunk, false
        ///         otherwise.
        /// There a two kinds of violation: change of allocation chunk
        ///                                 change of "typed-chunk" (see example)
        bool checkIfSameChunk(MemoryAddress a1, MemoryAddress a2, RsType * t);
        bool checkIfSameChunk(
                MemoryAddress a1,
                MemoryAddress a2,
                size_t size,
                RuntimeViolation::Type violation = RuntimeViolation::POINTER_CHANGED_MEMAREA
        );

        /// Reports a violation for all non freed memory locations
        /// call this function at end of program
        void checkForNonFreedMem() const;

        /// Deletes all collected data
        /// normally only needed for debug purposes
        void clearStatus() { mem.clear(); }

        /// Returns the MemoryType which stores the allocation information which is
        /// registered for this addr, or NULL if nothing is registered
        MemoryType * getMemoryType(MemoryAddress addr);

        /// Returns mem-area which contains a given area, or NULL if nothing found
        MemoryType * findContainingMem(MemoryAddress addr, size_t size = 1) ;

        /// Returns mem-area which overlaps with given area, or NULL if nothing found
        bool existOverlappingMem(MemoryAddress addr, size_t size) ;


        typedef std::map<MemoryAddress, MemoryType> MemoryTypeSet;
        const  MemoryTypeSet & getAllocationSet() const { return mem; }


        template<typename T>
        T* readMemory(MemoryAddress address)
        {
            checkRead(address, sizeof(T));
            return point_to<T>(address);
        }


    private:

        /**
         * Checks if memory region is allocated
         * @param addr  startAddress
         * @param size  size of chunk
         * @param t     if t != NULL, a accessMemWithType is called
         *              (which registers type and if there is already a type registered checks for consistency)
         * @param mt    return value -> the allocated memory chunk
         * @param vio   the violation which is thrown if the chunk is not allocated
         *              (should be INVALID_READ or INVALID_WRITE)
         */
        void checkAccess(MemoryAddress addr, size_t size, RsType * t, MemoryType * & mt,RuntimeViolation::Type vio);

        /// Queries the map for a potential matching memory area
        /// finds the memory region with next lower or equal address
        MemoryType * findPossibleMemMatch(MemoryAddress addr);

        void failNotSameChunk( RsType*, RsType*, size_t, size_t, MemoryType*, MemoryType*, RuntimeViolation::Type violation);


        MemoryTypeSet mem;


        friend class CStdLibManager;
};

std::ostream& operator<< (std::ostream &os, const MemoryManager & m);








#endif

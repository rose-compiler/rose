// vim:et sta sw=4 ts=4
#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H


#include <string>
#include <iosfwd>
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
class RsCompoundType;

/**
 * Keeps track of information about memory, such as address, size, and known
 * type information.  It is generally unnecessary for users to handle MemoryType
 * objects directly.
 *
 * Most uses of the MemoryManager can be handled indirectly via RuntimeSystem.
 */
struct MemoryType
{
        typedef rted_Address                    Location;
        typedef rted_AllocKind                  AllocKind;

        typedef const char*                     LocalPtr;
        typedef std::vector<bool>               InitData;
        typedef std::map<size_t, const RsType*> TypeData;
        typedef TypeData::iterator              TiIter;

        enum InitStatus { none, some, all };

        MemoryType(Location addr, size_t size, AllocKind kind, long blsz, const SourcePosition& pos);

        /// Checks if an address lies in this memory chunk
        bool containsAddress(Location addr) const;
        /// Checks if a memory area is part of this allocation
        bool containsMemArea(Location addr, size_t size) const;

        /// Less operator uses startAdress
        bool operator< (const MemoryType& other) const;

        Location               beginAddress() const { return startAddress; }
        size_t                 getSize()      const { return initdata.size(); }
        const SourcePosition & getPos()       const { return allocPos; }
        AllocKind              howCreated()   const { return origin; }

        /// Returns one past the last writeable address
        Location               endAddress() const;

        /// Returns the last writeable address
        Location               lastValidAddress() const;

        void                   resize( size_t size );

        /// Tests if a part of memory is initialized
        bool  isInitialized(Location addr, size_t len) const;

        /// Initialized a part of memory
        /// returns true, iff the location was not initialized before
        bool initialize   (Location addr, size_t len) ;

        /// Returns "Initialized" "Not initialized" or "Partially initialized"
        /// only for display purposes
        std::string getInitString() const;


        /// Prints info about this allocation
        void print(std::ostream & os) const;
        /// Prints information to stderr
        void print() const;

        template<typename T>
        std::string readMemory(Location addr) const
        {
            if (isInitialized(addr, sizeof(T))) return "not initialized";

            // \pp \todo for upc shared memory reads
            if (!rted_isLocal(addr)) return "not accessible";

            std::stringstream mem;

            mem << *(reinterpret_cast<const T*>(addr.local));
            return mem.str();
        }

        /// This functions checks and registers typed access to memory
        /// the first time memory is accessed with a specific type,
        /// this type is associated with this mem-region,
        /// and if its later on accessed with a different type, a violation is reported
        /// however it is possible to access the mem-region later with "containing" types
        /// f.e. first access with int-pointer, then with struct pointer, which as as first member an int
        /// \return true, iff the internal state changed
        bool registerMemType(Location loc, size_t ofs, const RsType* type);

        /**
         * As @see registerMemType, except that memory is not checked.  The type
         * at offset 0 is forced to be @c type.
         */
        void forceRegisterMemType(const RsType* type);

        /**
         * As @see registerMemType, except that memory is only checked and
         * merged, i.e. besides merging, no new types will be registered.
         *
         * @return  @c (true, X) @b iff a merge occurred.
         *             (X, true) iff there was a state modification
         */
        std::pair<bool, bool>
        checkAndMergeMemType(size_t ofs, const RsType* type);

        //! \overload
        std::pair<bool, bool>
        checkAndMergeMemType(Location addr, const RsType* type);

        /// Returns the RsType, or the CONTAINING ARRAY type which is associated with that offset
        /// to distinguish between nested types (class with has members of other classes)
        /// an additional size parameter is needed
        /// if no TypeInfo is found, null is returned
        const RsType* getTypeAt(size_t addr, size_t size) const;

        bool isDistributed() const { return blocksize != 0; }

        long blockSize() const { return blocksize; }

        /// \brief C++ ctors assume that an object is constructed on the heap.
        ///        Stack allocated objects straighten this out when their
        ///        initialization is processesed.
        void fixAllocationKind(AllocKind kind)
        {
          assert( (origin & akCxxHeap) == akCxxHeap );
          origin = kind;
        }

    private:
        typedef std::pair<TiIter,TiIter> TiIterPair;

        void insertType(Location offset, const RsType* type);

        Location        startAddress; ///< address where memory chunk starts
        AllocKind       origin;       ///< Where the memory is located and how the location was created
        SourcePosition  allocPos;     ///< Position in source file where malloc/new was called
        TypeData        typedata;
        InitData        initdata;
        long            blocksize;

        // \brief Determines all typeinfos which intersect the defined offset-range [from,to)
        //        "to" is exclusive i.e. typeInfos with startOffset==to are not included
        // \note made static function in cpp file
        // TiIterPair getOverlappingTypeInfos(TypeData& tmap, size_t startofs, size_t len);


        /// \brief   Computes an @c RsCompoundType for offset, size pairs for
        ///          which a type could not be found. The compound type will
        ///          have as much information as possible filled in. A compound
        ///          type is not necessary a class -- it could still be an
        ///          array (e.g. int at offset 0, int at offset 20).
        ///
        /// \return  a pointer to the newly constructed @c RsCompoundType. This
        ///          object will be on the heap and callers are responsible for
        ///          deleting it.
        const RsCompoundType* computeCompoundTypeAt(size_t ofs, size_t size) const;

        friend class MemoryManager;
};

std::ostream& operator<< (std::ostream& os, const MemoryType& m);


/**
 * \class MemoryManager
 * \brief MemoryManager tracks allocated memory and known type information.
 *
 * With the exception of bounds checking via @ref checkIfSameChunk, MemoryManager is
 * expected to be used indirectly, via calls to functions of RuntimeSystem.
 */
struct MemoryManager
{
        typedef Address Location;

        MemoryManager()
        : mem()
        {}

        /// Destructor checks if there are still allocations which are not freed
        ~MemoryManager();

        /// \brief  Create a new allocation based on the parameters
        /// \return a pointer to the actual stored object (NULL in case something went wrong)
        MemoryType* allocateMemory(Location addr, size_t size, MemoryType::AllocKind kind, long blocksize, const SourcePosition& pos);

        /// tracks dynamic memory deallocations
        void freeHeapMemory(Location addr, MemoryType::AllocKind freekind);

        /// tracks deallocations related to scope exits
        void freeStackMemory(Location addr);

        /// Prints information about all currently allocated memory areas
        void print(std::ostream & os) const;

        /// Check if memory region is allocated and initialized
        /// @param size     size=sizeof(DereferencedType)
        void checkRead (Location addr, size_t size) const;

        /// \brief  Checks if memory at position can be safely written, i.e. is allocated
        ///         if true it marks that memory region as initialized
        ///         that means this function should be called on every write!
        /// \return the allocation method and a flag indicating whether there was a status change
        ///         (i.e. from uninitialized to initialized (or storing a different type)
        std::pair<MemoryType*, bool> checkWrite (Location addr, size_t size, const RsType* t = NULL);

        /// \brief returns the allocation region around addr
        const MemoryType* checkLocation(Location addr, size_t size, RuntimeViolation::Type vioType) const;

        /**
         * @return @b true if the memory region containing
         * @c addr @c.. @c addr+size is initialized. */
        bool  isInitialized(Location addr, size_t size) const;

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
        bool checkIfSameChunk(Location a1, Location a2, const RsType* t) const;
        bool checkIfSameChunk( Location a1,
                               Location a2,
                               size_t size,
                               RuntimeViolation::Type violation = RuntimeViolation::POINTER_CHANGED_MEMAREA
                             ) const;

        /// Reports a violation for all non freed memory locations
        /// call this function at end of program
        void checkForNonFreedMem() const;

        /// Deletes all collected data
        /// normally only needed for debug purposes
        void clearStatus() { mem.clear(); }

        /// Returns the MemoryType which stores the allocation information which is
        /// registered for this addr, or NULL if nothing is registered
        MemoryType*       getMemoryType(Location addr);
        const MemoryType* getMemoryType(Location addr) const;

        /// Returns mem-area which contains a given area, or NULL if nothing found
        MemoryType*       findContainingMem(Location addr, size_t size) ;
        const MemoryType* findContainingMem(Location addr, size_t size) const;

        /// Returns mem-area which overlaps with given area, or NULL if nothing found
        bool existOverlappingMem(Location addr, size_t size, long blocksize) const;

        /// Queries the map for a potential matching memory area
        /// finds the memory region with next lower or equal address
        MemoryType*       findPossibleMemMatch(Location addr);
        const MemoryType* findPossibleMemMatch(Location addr) const;

        typedef std::map<Location, MemoryType> MemoryTypeSet;
        const MemoryTypeSet& getAllocationSet() const { return mem; }

        template<typename T>
        std::string readMemory(Location addr) const
        {
            const MemoryType* mt = findContainingMem(addr, sizeof(T));

            if (!mt) return "unknown location";

            return mt->readMemory<T>(addr);
        }

    private:
        void failNotSameChunk( const RsType& type1,
                               const RsType& type2,
                               Location addr1,
                               Location addr2,
                               const MemoryType& mem,
                               RuntimeViolation::Type violation
                             ) const;

        /// Frees allocated memory, throws error when no allocation is managed at this addr
        void freeMemory(MemoryType* m, MemoryType::AllocKind);

        MemoryTypeSet mem;

        friend class CStdLibManager;
};

std::ostream& operator<< (std::ostream &os, const MemoryManager & m);

#endif

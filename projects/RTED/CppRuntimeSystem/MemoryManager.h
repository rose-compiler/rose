#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H


#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <cassert>

#include "CStdLibManager.h"
#include "Util.h"



class RuntimeSystem;
class VariablesType;

/**
 * This class represents a memory allocation, made by malloc/new or just on the stack
 */
class MemoryType
{
    public:
        MemoryType(addr_type addr, size_t size, const SourcePosition & pos);
        MemoryType(addr_type addr, size_t size,
                   const std::string & file, int line1, int line2);

        // constructor which initialized only the address, used for comparison purposes
        MemoryType(addr_type addr);

        ~MemoryType() {}
        /// Checks if an address lies in this memory chunk
        bool containsAddress(addr_type addr);
        /// Checks if a memory area is part of this allocation
        bool containsMemArea(addr_type addr, size_t size);
        /// Checks if this MemoryType overlaps another area
        bool overlapsMemArea(addr_type queryAddr, size_t querySize);


        /// Less operator uses startAdress
        bool operator< (const MemoryType & other) const;


        addr_type              getAddress() const { return startAddress; }
        size_t                 getSize()    const { return size; }
        const SourcePosition & getPos()     const { return allocPos; }

        /// Tests if a part of memory is initialized
        bool  isInitialized(int offsetFrom, int offsetTo) const;

        /// Initialized a part of memory
        void  initialize   (int offsetFrom, int offsetTo) ;


        /// Notifies the Chunk that a pointer points to it
        /// when the mem-chunk is freed the registered pointer is invalidated
        void registerPointer  (VariablesType * var);

        /// Notifies the Chunk that a certain pointer does not pointer there anymore
        /// if doChecks is true, and the last pointer to the chunk is removed
        /// a violation is created
        void deregisterPointer(VariablesType * var, bool doChecks);


        /// Prints info about this allocation
        void print(std::ostream & os) const;


        template<typename T>
        T * readMemory(int offset)
        {
            assert(offset<0 && offset+sizeof(T) >= size);
            assert(isInitialized(offset,offset+sizeof(T)));

            char * charAddress = static_cast<char*>(startAddress);
            charAddress += offset;
            return static_cast<T*>(charAddress);
        }


    private:
        addr_type         startAddress; ///< address where memory chunk starts
        size_t            size;         ///< Size of allocation
        SourcePosition    allocPos;     ///< Position in source file where malloc/new was called
        std::vector<bool> initialized;  ///< stores for every byte if it was initialized

        /// Set of pointers which currently point into this memory chunk
        std::set<VariablesType*> pointerSet;
};
std::ostream& operator<< (std::ostream &os, const MemoryType & m);




class MemoryManager
{
    public:
        MemoryManager();

        /// Destructor checks if there are still allocations which are not freed
        ~MemoryManager();

        /// Adds new allocation, the MemoryType structure is freed by this class
        void allocateMemory(MemoryType * alloc);

        /// Frees allocated memory, throws error when no allocation is managed at this addr
        void freeMemory(addr_type addr);


        /// Prints information about all currently allocated memory areas
        void print(std::ostream & os) const;

        /// Check if memory region is allocated and initialized
        /// @param size     size=sizeof(DereferencedType)
        void checkRead  (addr_type addr, size_t size);

        /// Checks if memory at position can be safely written, i.e. is allocated
        /// if true it marks that memory region as initialized
        /// that means this function should be called on every write!
        void checkWrite (addr_type addr, size_t size);



        void checkForNonFreedMem() const;

        /// Deletes all collected data
        /// normally only needed for debug purposes
        void clearStatus() { mem.clear(); }

        /// Returns the MemoryType which stores the allocation information which is
        /// registered for this addr, or NULL if nothing is registered
        MemoryType * getMemoryType(addr_type addr);

        /// Returns mem-area which contains a given area, or NULL if nothing found
        MemoryType * findContainingMem(addr_type addr, size_t size = 1);

        /// Returns mem-area which overlaps with given area, or NULL if nothing found
        MemoryType * findOverlappingMem(addr_type addr, size_t size);

    private:
        /// Queries the map for a potential matching memory area
        /// finds the memory region with next lower or equal address
        MemoryType * findPossibleMemMatch(addr_type addr);


        typedef std::set<MemoryType*,PointerCmpFunc<MemoryType> > MemoryTypeSet;
        MemoryTypeSet mem;

	friend class CStdLibManager;
};

std::ostream& operator<< (std::ostream &os, const MemoryManager & m);








#endif

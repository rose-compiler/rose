#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H


#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cassert>


#include "CStdLibManager.h"
#include "Util.h"


class RuntimeSystem;
class VariablesType;
class RsType;
/**
 * This class represents a memory allocation, made by malloc/new or just on the stack
 */
class MemoryType
{
    public:

        typedef std::map<addr_type, RsType*> TypeInfoMap;
        typedef TypeInfoMap::iterator TiIter;




        MemoryType(addr_type addr, size_t size, const SourcePosition & pos, bool onStack);
        MemoryType(addr_type addr, size_t size, bool onStack,
                   const std::string & file, int line1, int line2);

        // constructor which initialized only the address, used for comparison purposes
        MemoryType(addr_type addr, bool onStack = false);

        ~MemoryType();
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
        bool                   isOnStack()  const { return onStack; }

        /// Tests if a part of memory is initialized
        bool  isInitialized(int offsetFrom, int offsetTo) const;

        /// Initialized a part of memory
        void  initialize   (int offsetFrom, int offsetTo) ;

        /// Returns "Initialized" "Not initialized" or "Partially initialized"
        /// only for display purposes
        std::string getInitString() const;


        /// Notifies the Chunk that a pointer points to it
        /// when the mem-chunk is freed the registered pointer is invalidated
        void registerPointer  (VariablesType * var);

        /// Notifies the Chunk that a certain pointer does not pointer there anymore
        /// if doChecks is true, and the last pointer to the chunk is removed
        /// a violation is created
        void deregisterPointer(VariablesType * var);


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

        /// This functions checks and registers typed access to memory
        /// the first time memory is accessed with a specific type,
        /// this type is associated with this mem-region,
        /// and if its later on accessed with a different type, a violation is reported
        /// however it is possible to access the mem-region later with "containing" types
        /// f.e. first access with int-pointer, then with struct pointer, which as as first member an int
        void accessMemWithType(addr_type offset, RsType * type);


        /// Returns the type-name , or the CONTAINING ARRAY name which is associated with that offset
        /// to distinguish between nested types (class with has members of other classes)
        /// an additional size parameter is needed
        /// if not TypeInfo is found, NULL is returned
        /// example: returns "typeName.member.submember", where subMember has the specified size
        ///          or subMember is an array, and one element has specified size
        std::string getTypeAt(addr_type offset, size_t size);


        const TypeInfoMap & getTypeInfoMap() const { return typeInfo; }

        /// Returns a set with pointer-variables which point into this allocation
        typedef std::set<VariablesType*> VariableSet;
        const VariableSet & getPointerSet() const  { return pointerSet; }


        /// If set to true a violation is reported if the last pointer
        /// which pointed to a mem region changes (i.e. mem region not reachable any more)
        /// is not necessary an error, f.e. p=malloc(3); p+=1000; p-=1000; free(p)
        /// this code is valid but would report a violation, because after the "+="
        /// no pointer points to the chunk any more
        /// per default these errors are reported
        static void setViolationOnMemWithoutPointer(bool val) {checkMemWithoutPointer = val; }
        static bool violationOnMemWithoutPointer()            { return checkMemWithoutPointer; }


    private:
        typedef std::pair<TiIter,TiIter> TiIterPair;


        void insertType(addr_type offset,RsType * type);

        addr_type         startAddress; ///< address where memory chunk starts
        size_t            size;         ///< Size of allocation
        SourcePosition    allocPos;     ///< Position in source file where malloc/new was called
        std::vector<bool> initialized;  ///< stores for every byte if it was initialized
		bool			  onStack;		///< Whether the memory lives on the stack or not (i.e. on the heap)


        /// Determines all typeinfos which intersect the defined offset-range [from,to)
        /// "to" is exclusive - typeInfos with startOffset==to , are not included
        TiIterPair getOverlappingTypeInfos(addr_type from, addr_type to);

        /// A entry in this map means, that on offset <key> is stored the type <value>
        TypeInfoMap typeInfo;

        /// Set of pointers which currently point into this memory chunk
        VariableSet pointerSet;

        static bool checkMemWithoutPointer;


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
        void freeMemory(addr_type addr, bool onStack=false);


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


        typedef std::set<MemoryType*,PointerCmpFunc<MemoryType> > MemoryTypeSet;
        const  MemoryTypeSet & getAllocationSet() const { return mem; }

    private:
        /// Queries the map for a potential matching memory area
        /// finds the memory region with next lower or equal address
        MemoryType * findPossibleMemMatch(addr_type addr);


        MemoryTypeSet mem;


        friend class CStdLibManager;
};

std::ostream& operator<< (std::ostream &os, const MemoryManager & m);








#endif


#ifndef VARIABLESTYPE_H
#define VARIABLESTYPE_H


#include <string>
#include <iostream>
#include "Util.h"

class MemoryType;

class VariablesType
{
    public:
        VariablesType(const std::string & name,
                      const std::string & mangledName,
                      const std::string & typeStr,
                      addr_type address,
                      size_t size);

        ~VariablesType();

        const std::string & getName()        const  { return name;        }
        const std::string & getMangledName() const  { return mangledName; }
        const std::string & getType()        const  { return type;        }

        addr_type           getAddress()     const  { return address; }

        size_t              getSize()        const  { return size;    }


        /// Marks this variable as pointer and stores the address it points to
        /// @param newAddr  the new target-address of this pointer
        /// @param doChecks if true a violation is created when the pointer changes
        ///                 the memory-chunk it points to, normally not wanted if pointer
        ///                 is assigned, only when pointer arithmetic is done
        ///                 heuristic: after operation with pointer it should still point to same mem-chunk
        void setPointerTarget(addr_type newAddr, bool doChecks);

        /// Is called by memory-chunk when it gets freed
        void invalidatePointer() { setPointerTarget(0,false); }

        /// Returns the Memory Chunk this pointer points to
        /// or NULL if this variable is not a pointer
        /// do not use this function to check if a pointer deref is valid
        /// use checkMemRead() instead
        MemoryType *        getTargetAllocation() const;


        /// returns the allocation information for this var
        MemoryType *        getAllocation()  const;


        void print(std::ostream & os) const;

    private:
        /// stack variable name
        std::string name;

        /// mangled name
        std::string mangledName;

        /// Size of variable in memory
        size_t size;

        /// string with class name of rose-type
        std::string type;

        /// address of this variable in memory
        addr_type address;

        /// Is 0 when var is not a pointer
        /// or the target address if this var was registered as pointer
        addr_type pointerTarget;
};


std::ostream& operator<< (std::ostream &os, const VariablesType & m);


#endif

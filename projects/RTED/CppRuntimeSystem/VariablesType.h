
#ifndef VARIABLESTYPE_H
#define VARIABLESTYPE_H

#include <string>
#include <iosfwd>
#include <cassert>

#include "RsType.h"
#include "Util.h"

#include "rted_typedefs.h"

class MemoryType;
class PointerInfo;

/// \brief   represents Variables
/// \details variables are names that have a stack address associated
struct VariablesType
{
        typedef Address Location;

        VariablesType(Address addr, const char* id, const char* mangledId, const RsType* t)
        : address(addr), name(id), mangledName(mangledId), type(t)
        {}

        std::string   getName()        const  { return name;                }
        std::string   getMangledName() const  { return mangledName;         }
        const RsType* getType()        const  { return type;                }
        Location      getAddress()     const  { return address;             }
        size_t        getSize()        const  { return type->getByteSize(); }

        /// returns the allocation information for this var
        const MemoryType*  getAllocation()  const;

        void print(std::ostream& os) const;

        /// If this variable is registered as a pointer the PointerInfo is return, else NULL
        const PointerInfo* getPointerInfo() const;

    private:
        /// address of this variable in memory
        Location address;

        /// stack variable name
        const char* name;

        /// mangled name
        const char* mangledName;

        /// type rep
        const RsType* type;
};

std::ostream& operator<< (std::ostream& os, const VariablesType& m);

#endif

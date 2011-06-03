
#ifndef VARIABLESTYPE_H
#define VARIABLESTYPE_H

#include <string>
#include <iosfwd>
#include <cassert>

#include "Util.h"

#include "rted_typedefs.h"


class MemoryType;
class RsType;
class PointerInfo;


/// \brief   represents Variables
/// \details variables are names that have a stack address associated
class VariablesType
{
  typedef Address Location;

    public:
        VariablesType( Address address,
                       const std::string & name,
                       const std::string & mangledName,
                       const RsType* type
                     );

        const std::string & getName()        const  { return name;        }
        const std::string & getMangledName() const  { return mangledName; }
        const RsType*       getType()        const  { return type;        }
        Location            getAddress()     const  { return address; }
        size_t              getSize()        const;

        /// returns the allocation information for this var
        const MemoryType*   getAllocation()  const;

        void print(std::ostream & os) const;

        /// If this variable is registered as a pointer the PointerInfo is return, else NULL
        const PointerInfo* getPointerInfo() const;

    private:
        /// address of this variable in memory
        Location address;

        /// stack variable name
        std::string name;

        /// mangled name
        std::string mangledName;

        /// string with class name of rose-type
        const RsType* type;
};


std::ostream& operator<< (std::ostream &os, const VariablesType & m);




#endif

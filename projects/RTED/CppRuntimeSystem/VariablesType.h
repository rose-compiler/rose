
#ifndef VARIABLESTYPE_H
#define VARIABLESTYPE_H


#include <string>
#include <iostream>
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
  typedef const char* Location;

    public:
        VariablesType( const std::string & name,
                       const std::string & mangledName,
                       RsType * type,
                       Address address
                     );

        ~VariablesType();

        const std::string & getName()        const  { return name;        }
        const std::string & getMangledName() const  { return mangledName; }
        RsType *            getType()        const  { return type;        }
        Location            getAddress()     const  { return address; }
        size_t              getSize()        const;

        /// returns the allocation information for this var
        MemoryType *        getAllocation()  const;

        void print(std::ostream & os) const;

        /// If this variable is registered as a pointer the PointerInfo is return, else NULL
        PointerInfo * getPointerInfo() const ;

    private:
        /// stack variable name
        std::string name;

        /// mangled name
        std::string mangledName;

        /// string with class name of rose-type
        RsType * type;

        /// address of this variable in memory
        Location address;
};


std::ostream& operator<< (std::ostream &os, const VariablesType & m);




#endif

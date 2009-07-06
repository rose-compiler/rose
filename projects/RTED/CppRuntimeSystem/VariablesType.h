
#ifndef VARIABLESTYPE_H
#define VARIABLESTYPE_H


#include <string>
#include <iostream>
#include "Util.h"

class MemoryType;

class VariablesType
{
    public:
        VariablesType(const std::string & _name,
                      const std::string & _mangledName,
                      const std::string & _typeStr,
                      addr_type _address);

        ~VariablesType();

        const std::string & getName()        const  { return name;}
        const std::string & getMangledName() const  { return mangledName; }
        addr_type           getAddress()     const  { return address; }

        /// returns the allocation information for this var
        MemoryType *        getAllocation()  const;

        void print(std::ostream & os) const;

    private:
        std::string name; ///< stack variable name
        std::string mangledName; ///< mangled name

        //FIXME do not store string here but type-enum
        std::string type; ///< string with class name of rose-type

        addr_type address; ///< address of this variable in memory
};


std::ostream& operator<< (std::ostream &os, const VariablesType & m);


#endif

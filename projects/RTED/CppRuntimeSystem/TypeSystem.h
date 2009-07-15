
#ifndef TYPESYSTEM_H
#define TYPESYSTEM_H

#include <set>
#include <iostream>

#include "Util.h"
#include "RsType.h"

class TypeSystem
{
    public:
        /// Registers all base-types
        TypeSystem();

        /// Frees all RsType* in the types map
        ~TypeSystem();


        /// Add a type to the typesystem
        /// @param type  pointer to type-info, the object is freed by the TypeSystem
        /// @return false if type with that name already exists
        bool registerType(RsType * type);



        /// Returns type-information for a registered type
        /// or NULL if type is unknown
        RsType * getTypeInfo(const std::string & name);


        RsType * getArrayType(const std::string & baseTypeName, size_t size);

        /// Removes all registered datatypes, and adds the base datatypes
        void clear();

        void print(std::ostream & os) const;

    protected:
        typedef std::set<RsType*,PointerCmpFunc<RsType> > TypeSet;
        TypeSet types;
};

std::ostream& operator<< (std::ostream &os, const TypeSystem & m);



#endif

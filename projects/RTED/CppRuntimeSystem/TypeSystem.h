
#ifndef TYPESYSTEM_H
#define TYPESYSTEM_H

#include <set>
#include "Util.h"
#include "RsTypes.h"

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

    protected:

        typedef std::set<RsType*,PointerCmpFunc<RsType> > TypeSet;
        TypeSet types;
};

#endif

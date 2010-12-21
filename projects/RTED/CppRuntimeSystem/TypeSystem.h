
#ifndef TYPESYSTEM_H
#define TYPESYSTEM_H

#include <set>
#include <map>
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


        RsArrayType * getArrayType(RsType * baseType, size_t size);
        RsArrayType * getArrayType(const std::string & baseTypeName, size_t size);


        RsPointerType * getPointerType(RsType * baseType, size_t levelOfIndirection=1);
        RsPointerType * getPointerType(const std::string & baseTypeName, size_t levelOfIndirection=1);


        /// Removes all registered datatypes, and adds the base datatypes
        void clearStatus();

        void print(std::ostream & os) const;

        typedef std::set<RsType*, PointerCompare > TypeSet;
        typedef TypeSet::const_iterator const_iterator;

        const_iterator begin() const { return types.begin(); }
        const_iterator end()   const { return types.end();   }
    protected:
        TypeSet types;

        ///arrTypeMap[baseType][arraySize] returns the ArrayType
        std::map<RsType *,std::map<size_t, RsArrayType*> >   arrTypeMap;

        ///ptrTypeMap[baseType][indirectionLevel] return PointerType
        std::map<RsType *,std::map<size_t, RsPointerType*> > ptrTypeMap;

};

std::ostream& operator<< (std::ostream &os, const TypeSystem & m);



#endif


#ifndef TYPESYSTEM_H
#define TYPESYSTEM_H

#include <set>
#include <map>
#include <iosfwd>

#include "Util.h"
#include "RsType.h"

struct TypeSystem
{
        typedef std::set< const RsType*, PointerCompare > NamedTypeContainer;
        typedef std::vector< RsBasicType >                BasicTypeContainer;

        /// Registers all base-types
        TypeSystem();

        /// Returns type-information for a registered type
        /// or NULL if type is unknown
        const RsType* getTypeInfo(const std::string & name);

        /// creates an array with a given base type
        const RsArrayType* getArrayType(const RsType* baseType, size_t elemsize);
        const RsArrayType* getArrayType(const std::string& baseTypeName, size_t size);

        const RsPointerType* getPointerType(const RsType* baseType, AddressDesc levelOfIndirection);
        const RsPointerType* getPointerType(const std::string& baseTypeName, AddressDesc levelOfIndirection);

        const RsPointerType* getPointerType(const RsType* baseType);
        const RsPointerType* getPointerType(const std::string& baseTypeName);

        RsClassType& getClassType(const std::string& name, size_t sz = 0, bool uniontype = false);

        /// Removes all registered datatypes, and adds the base datatypes
        void clearStatus();

        void print(std::ostream& os) const;

        NamedTypeContainer::const_iterator begin() const { return types.begin(); }
        NamedTypeContainer::const_iterator end()   const { return types.end();   }
    protected:
        typedef std::map<size_t, RsPointerType>           TypeDerivatives;
        typedef std::map<const RsType*, TypeDerivatives>  TypeContainer;

        typedef std::map<size_t, RsArrayType>             ArrayDimensions;
        typedef std::map<const RsType*, ArrayDimensions>  ArrayDimContainer;

        typedef std::map< std::string, RsClassType >      ClassContainer;

        /// Add a type to the typesystem
        /// @param type  pointer to type-info, the object is freed by the TypeSystem
        void registerType(const RsType& t);

        /// adds all base types
        void registerBaseTypes();

        NamedTypeContainer       types;
        TypeContainer            ptrTypeMap;
        ArrayDimContainer        arrTypeMap;
        ClassContainer           classMap;
        const BasicTypeContainer basictypes;
};

std::ostream& operator<< (std::ostream& os, const TypeSystem& m);

#endif

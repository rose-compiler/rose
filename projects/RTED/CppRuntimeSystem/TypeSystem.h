
#ifndef TYPESYSTEM_H
#define TYPESYSTEM_H

#include <map>
#include <boost/unordered_map.hpp>
#include <iosfwd>

#include "Util.h"
#include "RsType.h"

/*

#include <boost/unordered_set.hpp>
#include <boost/functional/hash.hpp>

struct TypeHasher : public std::unary_function<const RsType*, std::size_t>
{
  size_t operator()(const RsType* t) const
  {
    using ::boost::hash_value;

    return hash_value(t->getName());
  }
};

struct TypeComp : public std::binary_function<const RsType*, const RsType*, bool>
{
  size_t operator()(const RsType* lhs, const RsType* rhs) const
  {
    return lhs->getName() == rhs->getName();
  }
};

*/

struct TypeSystem
{
        // typedef boost::unordered_set< const RsType*, TypeHasher, TypeComp > NamedTypeContainer;
        // typedef std::set<const RsType*, PointerCompare>                     NamedTypeContainer;

        // typedef std::map<std::string, const RsType*>                        NamedTypeContainer;

        typedef boost::unordered_map<std::string, const RsType*>            NamedTypeContainer;

        typedef std::vector< RsBasicType >                                  BasicTypeContainer;

        /// Registers all base-types
        TypeSystem();

        /// Returns type-information for a registered type
        /// or NULL if type is unknown
        const RsType* getTypeInfo(const std::string& name) const;

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

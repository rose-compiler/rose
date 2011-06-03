#include <cassert>
#include <iostream>
#include <algorithm>

#include "TypeSystem.h"
#include "CppRuntimeSystem.h"

static
TypeSystem::BasicTypeContainer makeBasicTypes()
{
  TypeSystem::BasicTypeContainer container;
  const size_t                   szBasicTypes = RsBasicType::SgUnknownType - RsBasicType::SgTypeBool;

  container.reserve(szBasicTypes);
  for (RsBasicType::SgType i = RsBasicType::SgTypeBool; i < RsBasicType::SgUnknownType; ++i)
  {
    container.push_back(RsBasicType::create(i));
  }

  return container;
}

TypeSystem::TypeSystem()
: types(), ptrTypeMap(), arrTypeMap(), basictypes(makeBasicTypes())
{
    // register all base types
    registerBaseTypes();
}

struct TypeRegistrar
{
  typedef void (TypeSystem::* Memfun)(const RsType&);

  TypeSystem& ts;
  Memfun      fn;

  TypeRegistrar(TypeSystem& typesys, Memfun f)
  : ts(typesys), fn(f)
  {}

  void operator()(const RsBasicType& t)
  {
    (ts.*fn)(t);
  }
};


void TypeSystem::registerBaseTypes()
{
  std::for_each( basictypes.begin(),
                 basictypes.end(),
                 TypeRegistrar(*this, &TypeSystem::registerType)
                 // std::bind1st(std::mem_fun(), this)
               );
}

void TypeSystem::clearStatus()
{
    types.clear();
    classMap.clear();
    registerBaseTypes();
}


void TypeSystem::registerType(const RsType& t)
{
    std::pair<NamedTypeContainer::iterator, bool> res = types.insert(&t);

    if (!res.second)
    {
        std::cerr << "Error: tried to register type " << t.getName() << " twice!" << std::endl;
        assert( false );
    }
}

RsClassType& TypeSystem::getClassType(const std::string& name, size_t sz /*= 0*/, bool uniontype /*= false*/)
{
  RsClassType& entry = classMap[name];

  if (entry.invalid())
  {
    entry = RsClassType(name, sz, uniontype);
    registerType(entry);
  }

  return entry;
}

const RsType* TypeSystem::getTypeInfo(const std::string& name)
{
    // TODO remove SgPointerType and SgArrayType from BasicTypes
    if (name == "SgPointerType" || name == "SgArrayType")
    {
      if ( diagnostics::warning() )
      {
        std::stringstream msg;

        msg << "getTypeInfo for " << name << "was called:" << std::endl;
        msg << "*** SEVERE WARNING: Pointer and Arrays have now be registered differently!" << std::endl;

        RuntimeSystem::instance()->printMessage(msg.str());
      }
    }

    InvalidType                  comparisonObject(name);
    NamedTypeContainer::iterator i = types.find(&comparisonObject);

    if (i == types.end()) return NULL;
    return *i;
}


const RsArrayType* TypeSystem::getArrayType(const std::string& name, size_t size)
{
    const RsType* bt = getTypeInfo(name);

    assert( bt );
    return getArrayType(bt, size);
}


const RsArrayType* TypeSystem::getArrayType(const RsType* bt, size_t size)
{
    ArrayDimensions& m = arrTypeMap[bt];
    RsArrayType&     t = m[size];

    // new entry?
    if (t == RsArrayType())
    {
      t = RsArrayType(bt, size);
    }

    return &t;
}


const RsPointerType* TypeSystem::getPointerType(const RsType* bt, AddressDesc desc)
{
    assert( rted_isPtr(desc) >= 1 );
    assert( bt != NULL );

    TypeDerivatives& m = ptrTypeMap[bt];
    RsPointerType&   t = m[desc.levels];

    // new entry?
    if (t == RsPointerType())
    {
      AddressDesc desc_prime = rted_deref_desc(desc);

      // multiple pointers are represented as pointer which have a pointer basetype
      // build these base-types recursively
      if (rted_isPtr(desc_prime))
          bt = getPointerType(bt, desc_prime);

      t = RsPointerType(bt);
    }

    return &t;
}

const RsPointerType* TypeSystem::getPointerType(const std::string& name, AddressDesc desc)
{
    const RsType* bt = getTypeInfo(name);
    return getPointerType(bt, desc);
}

const RsPointerType* TypeSystem::getPointerType(const RsType* bt)
{
    return getPointerType(bt, rted_ptr());
}

const RsPointerType* TypeSystem::getPointerType(const std::string & name)
{
    return getPointerType(getTypeInfo(name));
}


void TypeSystem::print(std::ostream & os) const
{
    os << "--------------  All Registered Types -----------------------" << std::endl;
    for (NamedTypeContainer::iterator it = types.begin(); it != types.end(); ++it)
        os << *it;

    os << "------------------------------------------------------------" << std::endl;

}


std::ostream& operator<< (std::ostream &os, const TypeSystem & m)
{
    m.print(os);
    return os;
}

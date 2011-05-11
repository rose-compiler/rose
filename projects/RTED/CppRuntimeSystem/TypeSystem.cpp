#include <cassert>
#include <iostream>

#include "TypeSystem.h"
#include "CppRuntimeSystem.h"

TypeSystem::TypeSystem()
{
    //call clear, to register all base types
    clearStatus();
}

TypeSystem::~TypeSystem()
{
    for(TypeSet::iterator i = types.begin(); i != types.end(); ++i)
        delete *i;
}

void TypeSystem::clearStatus()
{
    types.clear();

    for (RsBasicType::SgType i = RsBasicType::SgTypeBool; i < RsBasicType::SgUnknownType; ++i)
    {
        const bool success = registerType( new RsBasicType(RsBasicType::create(i)) );
        assert(success);
    }
}


bool TypeSystem::registerType(RsType * t)
{
    std::pair<TypeSet::iterator, bool> res;
    res =types.insert(t);

    if(!res.second) {
        std::cerr << "Error: tried to register type " << t->getName() << " twice!" << std::endl;
        assert( false );
    }

    return res.second;
}

RsType * TypeSystem::getTypeInfo(const std::string & name)
{

    //TODO remove SgPointerType and SgArrayType from BasicTypes
    if(name == "SgPointerType" || name == "SgArrayType" )
    {
      if ( diagnostics::warning() )
      {
        std::stringstream msg;

        msg << "getTypeInfo for " << name << "was called:" << std::endl;
        msg << "*** SEVERE WARNING: Pointer and Arrays have now be registered differently!" << std::endl;

        RuntimeSystem::instance()->printMessage(msg.str());
      }
    }

    InvalidType comparisonObject(name);

    TypeSet::iterator i = types.find(&comparisonObject);
    if(i == types.end())
        return NULL;
    else
        return *i;
}


RsArrayType * TypeSystem::getArrayType(const std::string& name, size_t size)
{
    RsType * bt = getTypeInfo(name);

    assert( bt );
    return getArrayType(bt,size);
}


RsArrayType* TypeSystem::getArrayType(RsType* bt, size_t size)
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


RsPointerType* TypeSystem::getPointerType(RsType* bt, AddressDesc desc)
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

RsPointerType * TypeSystem::getPointerType(const std::string & name, AddressDesc desc)
{
    RsType * bt = getTypeInfo(name);
    return getPointerType(bt, desc);
}

RsPointerType * TypeSystem::getPointerType(RsType * bt)
{
    return getPointerType(bt, rted_ptr());
}

RsPointerType * TypeSystem::getPointerType(const std::string & name)
{
    return getPointerType(getTypeInfo(name));
}


void TypeSystem::print(std::ostream & os) const
{
    os << "--------------  All Registered Types -----------------------" << std::endl;
    for(TypeSet::iterator it = types.begin(); it != types.end(); ++it)
        os << *it;

    os << "------------------------------------------------------------" << std::endl;

}


std::ostream& operator<< (std::ostream &os, const TypeSystem & m)
{
    m.print(os);
    return os;
}

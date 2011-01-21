#include "TypeSystem.h"

#include <cassert>

using namespace std;

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
    pair<TypeSet::iterator, bool> res;
    res =types.insert(t);

    if(!res.second) {
        cerr << "Error: tried to register type " << t->getName() << " twice!" << endl;
        assert( false );
    }

    return res.second;
}

RsType * TypeSystem::getTypeInfo(const string & name)
{

    //TODO remove SgPointerType and SgArrayType from BasicTypes
    if(name == "SgPointerType" || name == "SgArrayType" )
    {
      cerr << endl << "getTypeInfo for " << name << "was called:" << endl;
      cerr << "SEVERE WARNING: Pointer and Arrays have now be registered differently!" << endl;
    }

    InvalidType comparisonObject(name);

    TypeSet::iterator i = types.find(&comparisonObject);
    if(i == types.end())
        return NULL;
    else
        return *i;
}


RsArrayType * TypeSystem::getArrayType(const string& name, size_t size)
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

RsPointerType * TypeSystem::getPointerType(const string & name, AddressDesc desc)
{
    RsType * bt = getTypeInfo(name);
    return getPointerType(bt, desc);
}

RsPointerType * TypeSystem::getPointerType(RsType * bt)
{
    return getPointerType(bt, rted_ptr());
}

RsPointerType * TypeSystem::getPointerType(const string & name)
{
    return getPointerType(getTypeInfo(name));
}





/*
RsType * TypeSystem::getArrayType(RsType * bt, size_t size)
{
    string arrName (RsArrayType::getArrayTypeName(bt,size));
    InvalidType comparisonObject(arrName);

    TypeSet::iterator i = types.find(&comparisonObject);
    if(i == types.end())
    {
        //Not yet needed -> create it
        RsArrayType * at = new RsArrayType(bt,size);
        registerType(at);
        return at;
    }
    else
        return *i;
}*/




void TypeSystem::print(ostream & os) const
{
    os << "--------------  All Registered Types -----------------------" << endl;
    for(TypeSet::iterator it = types.begin(); it != types.end(); ++it)
        os << *it;

    os << "------------------------------------------------------------" << endl;

}


std::ostream& operator<< (std::ostream &os, const TypeSystem & m)
{
    m.print(os);
    return os;
}

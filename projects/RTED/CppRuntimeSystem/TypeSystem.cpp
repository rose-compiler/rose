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

    // Register all Base-Types
    int count = RsBasicType::getBaseTypeCount();
    for(int i=0; i<count; i++)
    {
        RsBasicType::SgType t = RsBasicType::getBaseType(i);
        bool success = registerType(new RsBasicType(t));
        assert(success);
    }
}


bool TypeSystem::registerType(RsType * t)
{
    pair<TypeSet::iterator, bool> res;
    res =types.insert(t);

    if(!res.second)
        cerr << "Warning: tried to register type " << t->getName() << " twice!" << endl;

    return res.second;
}

RsType * TypeSystem::getTypeInfo(const string & name)
{

    //TODO remove SgPointerType and SgArrayType from BasicTypes
    if(name == "SgPointerType" || name == "SgArrayType" )
    {
        cerr << "getTypeInfo for " << name << "was called:" << endl;
        cerr << "SEVERE WARNING: Pointer and Arrays have now be registered differently!" << endl;
    }

    InvalidType comparisonObject(name);

    TypeSet::iterator i = types.find(&comparisonObject);
    if(i == types.end())
        return NULL;
    else
        return *i;
}


RsArrayType * TypeSystem::getArrayType(const string & name, size_t size)
{
    RsType * bt = getTypeInfo(name);

    assert( bt );
    return getArrayType(bt,size);
}


RsArrayType * TypeSystem::getArrayType(RsType * bt, size_t size)
{
    map<size_t, RsArrayType*> & m = arrTypeMap[bt];

    map<size_t,RsArrayType*>::iterator it = m.find(size);
    if( it == m.end() )
    {
        RsArrayType * arrType = new RsArrayType(bt,size);
        m.insert(make_pair<size_t,RsArrayType*>(size,arrType));
        return arrType;
    }
    else
        return it->second;
}

RsPointerType * TypeSystem::getPointerType(const string & name, size_t indirection)
{
    RsType * bt = getTypeInfo(name);
    return getPointerType(bt,indirection);
}


RsPointerType * TypeSystem::getPointerType(RsType * bt, size_t indirection)
{
    assert(indirection >= 1);

    map<size_t, RsPointerType * > & m = ptrTypeMap[bt];

    map<size_t,RsPointerType*>::iterator it = m.find(indirection);
    if( it == m.end() )
    {
        RsType * base = NULL;
        // multiple pointers are represented as pointer which have a pointer basetype
        // build these base-types recursively
        if(indirection > 1)
            base = getPointerType(bt,indirection-1);
        else
            base = bt;

        RsPointerType * ptrType = new RsPointerType(base);
        m.insert(make_pair<size_t,RsPointerType*>(indirection,ptrType));
        return ptrType;
    }
    else
        return it->second;

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




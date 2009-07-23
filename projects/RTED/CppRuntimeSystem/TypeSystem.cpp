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

RsType * TypeSystem::getTypeInfo(const string & name_)
{
    //TODO apparently SgCharType and SgTypeChar exist?!
    string name = name_;
    if(name == "SgCharType")
        name = "SgTypeChar";


    InvalidType comparisonObject(name);

    TypeSet::iterator i = types.find(&comparisonObject);
    if(i == types.end())
        return NULL;
    else
        return *i;
}


RsType * TypeSystem::getArrayType(const string & name, size_t size)
{
    RsType * bt = getTypeInfo(name);
    getArrayType(bt,size);
}


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
}



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




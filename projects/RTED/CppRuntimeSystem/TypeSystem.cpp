#include "TypeSystem.h"

#include <cassert>


TypeSystem::TypeSystem()
{
    // Register all Base-Types
    int count = RsBasicType::getBaseTypeCount();
    for(int i=0; i<count; i++)
    {
        RsBasicType::SgType t = RsBasicType::getBaseType(i);
        bool success = registerType(new RsBasicType(t));
        assert(success);
    }
}

TypeSystem::~TypeSystem()
{
    for(TypeSet::iterator i = types.begin(); i != types.end(); ++i)
        delete *i;
}


bool TypeSystem::registerType(RsType * t)
{
    //TODO
    return true;
}


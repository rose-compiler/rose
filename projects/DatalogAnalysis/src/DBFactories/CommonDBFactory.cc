#include "common.hh"
#include "CommonDBFactory.hh"
#include <assert.h>

Domain *
CommonDBFactory::createDomain( const string &name )
{

    Domain *f= findDomain( name );
    if (f) {
        throw string("Domain ")+name+" already created";
    }

    Domain *dom= newDomain(name);
    domains[name]= dom;
    return dom;
}

Relation *
CommonDBFactory::createRelation( const string &name,
                                 int arity,
                                 Domain *sig[],
                                 string *names
                               )
{
    for (int i= 0; i < arity; i++) {
        assert( sig[i]->in( this ) );
    }

    Relation *r= findRelation( name );
    if (r) {
        throw string("Relation ")+name+" already created";
    }

    Relation *rel= newRelation( arity, sig, names );
    relations[name]= rel;
    return rel;
}

Domain *
CommonDBFactory::findDomain( const string &name )
{
    StringMap<Domain*>::Type::iterator i= domains.find( name );
    if (i == domains.end()) {
        return NULL;
    }

    return i->second;
}

Relation *
CommonDBFactory::findRelation( const string &name )

{
    StringMap<Relation*>::Type::iterator i= relations.find( name );
    if (i == relations.end()) {
        return NULL;
    }

    return i->second;
}

CommonDBFactory::~CommonDBFactory()
{
    for (   StringMap<Relation*>::Type::iterator i= relations.begin();
            i != relations.end();
            i++
        )
    {
        delete i->second;
    }

    for (   StringMap<Domain*>::Type::iterator i= domains.begin();
            i != domains.end();
            i++
        )
    {
        delete i->second;
    }
}


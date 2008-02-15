#include "TupleDomain.hh"

Element *
TupleDomain::createElement( const string &name )
{
/*    Element *e= findElement( name );
    if (e) {
        return e;
    }
    */

    Element *e= new TupleElement(this,name,velements.size());
    elements.insert( StringMultiMap<Element*>::Type::value_type(name,e) );
    velements.push_back(e);
    return e;
}

Element *
TupleDomain::findElement( const string &name )
{
    StringMap<Element*>::Type::iterator i= elements.find(name);
    if (i == elements.end())
        return NULL;
    return i->second;
}

TupleDomain::~TupleDomain()
{
    for ( unsigned int i= 0; i < velements.size(); i++)
    {
        delete velements[i];
    }
}

void
TupleDomain::output( ostream &o )
{
    for ( unsigned int i= 0; i < velements.size(); i++)
    {
        const TupleElement *e= static_cast<const TupleElement*>(velements[i]);
        o << e->name << endl;
    }
}


int
TupleDomain::size()
{
    return velements.size();
}

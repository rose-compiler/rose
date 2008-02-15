#ifndef TUPLEDOMAIN_HH
#define TUPLEDOMAIN_HH

#include "common.hh"
#include "StringMap.hh"
#include "Domain.hh"
#include <vector>

class TupleElement : public Element {
    public:
        bool in( Domain *d ) {
            return owner == d;
        }

        TupleElement( Domain *owner, const string &name, int label )
            : Element( owner ), name(name), label(label)
        {}
        string name;
        int label;
};


class TupleDomain : public Domain {
    public:
        TupleDomain( DBFactory *owner, string name ) : Domain(owner,name)
        {}

        virtual Element *createElement( const string &name );
        virtual Element *findElement( const string &name );
        virtual void output( ostream & );

        virtual ~TupleDomain();

        bool in( DBFactory *dbf ) {
            return owner == dbf;
        }

        int size();

    private:
        StringMultiMap<Element*>::Type elements;
        vector<Element*> velements;
};

#endif

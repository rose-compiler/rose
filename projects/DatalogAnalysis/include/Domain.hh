#ifndef DOMAIN_HH
#define DOMAIN_HH

#include "common.hh"

class Domain;

/* A single unique Element that must exist within a domain.
 * This is an interface
 */
class Element {
    public:
        bool in( Domain *d ) {
            return owner == d;
        }

    protected:
        Element( Domain *owner ) {
            this->owner = owner;
        }
        Domain *owner;

    INTERFACE( Element );
};

class DBFactory;

/* Domain
 * A set of elements.
 * Constructed by factories.
 */
class Domain {
    public:
        virtual Element *createElement( const string &name ) = 0;
        virtual Element *findElement( const string &name ) = 0;

        virtual void output( ostream & ) =0;

        bool in( DBFactory *db ) {
            return owner == db;
        }

        virtual int size(void) =0;

        string getName() {return name;}

    protected:
        Domain( DBFactory *owner, string name ) : owner(owner), name(name) {}

        DBFactory *owner;
        string name;

    INTERFACE( Domain );
};

#endif

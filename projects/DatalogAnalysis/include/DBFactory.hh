#ifndef DBFACTORY_HH
#define DBFACTORY_HH

#include "common.hh"
#include "Domain.hh"
#include "Relation.hh"


/*
 * The interface for databases, independent of the backend used.
 * This should allow us to use bddbddb, crocopat, etc.
 * This is an Abstract Factory.
 */
class DBFactory
{
    public:
        /* Create a domain for elements.
         * Prevent name collisions within the same db
         */
        virtual Domain *createDomain( const string &name ) =0;

        /* Create a relation with
         * name: the relation's name
         * arity: the number of elemetns in each tuple
         * sig: the domain for each signature
         * docs: the short name for each elemnent in the tuples (docs)
         */
        virtual Relation *createRelation( const string &name,
                                          int arity,
                                          Domain **sig,
                                          string *docs
                                        ) =0;

        virtual Domain *findDomain( const string &name ) =0;
        virtual Relation *findRelation( const string &name ) =0;

        /*output the entire database to directory dir*/
        virtual void outputDB( const string &dir ) =0;

    INTERFACE( DBFactory );
};

#endif

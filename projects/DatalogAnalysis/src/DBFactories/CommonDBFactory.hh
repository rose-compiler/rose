#ifndef COMMONDBFACTORY_HH
#define COMMONDBFACTORY_HH

#include "common.hh"
#include "DBFactory.hh"
#include "StringMap.hh"

/* A DBFactory mostly complete, although it used the factory methods
 * newDomain and newRelation to do allocation, so that subclasses
 * may choose the representation for Domains and Relations
 */
class CommonDBFactory : public DBFactory {
    public:
        virtual Domain *createDomain( const string & name );
        virtual Relation *createRelation( const string & name,
                                          int arity,
                                          Domain *sig[],
                                          string *names
                                        );
        virtual Domain *findDomain( const string &name );
        virtual Relation *findRelation( const string &name );
        


    protected:
        virtual Domain *newDomain( string name ) =0;
        virtual Relation *newRelation( int arity, Domain *sig[], string *names ) =0;

        StringMap<Domain*>::Type domains;
        StringMap<Relation*>::Type relations;

        CommonDBFactory() {}
        virtual ~CommonDBFactory();

    NOCOPYING( CommonDBFactory );
};

#endif

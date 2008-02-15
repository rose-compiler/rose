#ifndef TUPLEFACTORY_HH
#define TUPLEFACTORY_HH

#include "common.hh"
#include "CommonDBFactory.hh"

/* The DBFactory for bddbddb's .tuple format
 */
class TupleFactory : public CommonDBFactory
{
    public:
        TupleFactory(){};

        virtual Domain *newDomain( string name );
        virtual Relation *newRelation( int, Domain **sig, string *names );
        virtual void outputDB( const string &dir );

        NOCOPYING(TupleFactory);
};

#endif

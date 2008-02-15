#include "TupleFactory.hh"
#include "TupleDomain.hh"
#include "TupleRelation.hh"

#include <fstream>

Domain *
TupleFactory::newDomain( string name )
{
    return new TupleDomain(this,name);
}

Relation *
TupleFactory::newRelation( int arity, Domain *sig[], string *names )
{
    Relation *r;

    /* If the arity is less than 11, use the specialized Relations,
     * else use the general.  */
    switch (arity) {
        case 1:
            r= new KTupleRelation<1>( sig, names );
            break;
        case 2:
            r= new KTupleRelation<2>( sig, names );
            break;
        case 3:
            r= new KTupleRelation<3>( sig, names );
            break;
        case 4:
            r= new KTupleRelation<4>( sig, names );
            break;
        case 5:
            r= new KTupleRelation<5>( sig, names );
            break;
        case 6:
            r= new KTupleRelation<6>( sig, names );
            break;
        case 7:
            r= new KTupleRelation<7>( sig, names );
            break;
        case 8:
            r= new KTupleRelation<8>( sig, names );
            break;
        case 9:
            r= new KTupleRelation<9>( sig, names );
            break;
        case 10:
            r= new KTupleRelation<10>( sig, names );
            break;
        default:
            r= new TupleRelation( arity, sig, names );

    }

    return r;
}

void
TupleFactory::outputDB( const string &dir )
{
    string ename= dir + "/everything";
    ofstream e(ename.c_str());

    for (   StringMap<Domain*>::Type::iterator i= domains.begin();
            i != domains.end();
            i++
        )
    {
        string filename= dir + '/' + i->first + ".map";
        e << i->first << ' ' << i->second->size()
          << ' ' << i->first << ".map" << endl;

        ofstream of( filename.c_str() );
        i->second->output(of);
        of.close();
    }

    e << endl;

    //ofstream relations( dir + '/' + "relations" );

    for (   StringMap<Relation*>::Type::iterator i= relations.begin();
            i != relations.end();
            i++
        )
    {
        string filename= dir + '/' + i->first + ".tuples";
        ofstream of( filename.c_str() );
        i->second->output(of);
        of.close();

        e << i->first << "( ";
        for (int j= 0; j < i->second->getArity(); j++) {
            e << "n" << j << ":" << i->second->signature[j]->getName();
            if ( j+1 < i->second->getArity())
                e << ", ";
        }
        e << " ) inputtuples" << endl;

        string docname= dir + '/' + i->first + ".doc";
        ofstream dof( docname.c_str() );
        for ( int j= 0; j < i->second->getArity(); j++) {
            dof << i->second->names[j] << endl;
        }
        dof.close();
    }

    e << endl;
}

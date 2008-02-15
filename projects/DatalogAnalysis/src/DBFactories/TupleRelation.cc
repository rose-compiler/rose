#include <assert.h>
#include "TupleRelation.hh"
#include <ostream>

TupleRelation::~TupleRelation()
{
    for (   set< Tuple, TupleCompare >::iterator i= tuples.begin();
            i != tuples.end();
            i++
        )
    {
        delete[] i->e;
    }
}

void
TupleRelation::insert( Element **t )
{
    Tuple tuple;
    tuple.arity= arity;
    tuple.e= new Element*[arity];

    for (int i= 0; i < arity; i++) {
        if (t[i]==NULL) {
            throw "bad"; 
            tuple.e[i]= signature[i]->createElement("null");
            continue;
        }
        assert( t[i]->in(signature[i]) );
        tuple.e[i]= t[i];
    }

    tuples.insert( tuple );
}

void
TupleRelation::output( ostream &o )
{
    for (   set< Tuple, TupleCompare >::iterator i= tuples.begin();
            i != tuples.end();
            i++
        )
    {
        for (int j=0; j< arity; j++) {
            o << static_cast<const TupleElement *>(i->e[j])->label <<' ';
        }
        o << endl;
    }

}

template<int k>
KTupleRelation<k>::~KTupleRelation()
{}

template<int k>
void
KTupleRelation<k>::insert( Element **t )
{
    KTuple<k> tuple;

    for (int i= 0; i < k; i++) {
        if (t[i]==NULL) {
             throw "bad"; 
             tuple.e[i]= signature[i]->createElement("null");
             continue;
        }
        assert( t[i]->in(signature[i]) );
        tuple.e[i]= t[i];
    }

    tuples.insert( tuple );
}

template<int k>
void
KTupleRelation<k>::output( ostream &o )
{
    for (   typename set< KTuple<k>, KTupleCompare<k> >::iterator i= tuples.begin();
            i != tuples.end();
            i++
        )
    {
        for (int j=0; j< arity; j++) {
            o << static_cast<const TupleElement *>(i->e[j])->label <<' ';
        }
        o << endl;
    }

}


template class KTupleRelation<1>;
template class KTupleRelation<2>;
template class KTupleRelation<3>;
template class KTupleRelation<4>;
template class KTupleRelation<5>;
template class KTupleRelation<6>;
template class KTupleRelation<7>;
template class KTupleRelation<8>;
template class KTupleRelation<9>;
template class KTupleRelation<10>;


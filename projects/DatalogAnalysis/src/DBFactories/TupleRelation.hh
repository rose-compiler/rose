#ifndef TUPLERELATION_HH
#define TUPLERELATION_HH

#include "common.hh"
#include "Relation.hh"
#include "TupleDomain.hh"
#include <set>
#include <assert.h>

struct Tuple {
    int arity;
    Element **e;
};

/* Arbitrary length tuples for bddbddb relations */
struct TupleCompare {
    bool operator()( const Tuple &l, const Tuple &r )
    {
        assert( l.arity == r.arity );
        for (int i= 0; i < l.arity; i++) {
            int ll= (static_cast<TupleElement*>(l.e[i]))->label;
            int rr= (static_cast<TupleElement*>(r.e[i]))->label;
            if (ll < rr) return true;
            if (rr > ll) return false;
        }
        return false;
    }
};

class TupleRelation : public Relation {
public:
    TupleRelation( int arity, Domain **sig, string *names )
        : Relation(arity,sig,names)
    {}
    virtual ~TupleRelation();
    virtual void insert( Element **tuple );
    virtual void output( ostream & );
private:
    set< Tuple, TupleCompare > tuples;
};


/*
 * Fixed length tuples, to prevent excessive allocation.
 */
template<int k>
struct KTuple {
    Element *e[k];
};

template<int k>
struct KTupleCompare {
    bool operator()( const KTuple<k> &l, const KTuple<k> &r )
    {
        for (int i= 0; i < k; i++) {
            int ll= (static_cast<const TupleElement*>(l.e[i]))->label;
            int rr= (static_cast<const TupleElement*>(r.e[i]))->label;
            if (ll < rr) return true;
            if (rr > ll) return false;
        }
        return false;
    }
};

template<int k>
class KTupleRelation : public Relation {
public:
    KTupleRelation( Domain **sig, string *names )
        : Relation(k,sig,names)
    {}
    virtual ~KTupleRelation();
    virtual void insert( Element **tuple );
    virtual void output( ostream & );
private:
    set< KTuple<k>, KTupleCompare<k> > tuples;
};

#endif

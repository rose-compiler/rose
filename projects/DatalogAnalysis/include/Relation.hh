#ifndef RELATION_HH
#define RELATION_HH

#include "common.hh"

class Element;
class Domain;

class Relation {
public:
    /* Constructor
     * airty: the number of elements in each tuple
     * sig: the domain of each element in the tuples
     * names: a short name for each element (for documentation)
     */
    Relation( int arity, Domain **sig, string *names ) : arity(arity)
    {
        signature= new Domain*[arity];
        this->names= new string[arity];
        for (int i= 0; i < arity; i++) {
            signature[i]= sig[i];
            this->names[i]= names[i];
        }
    }

    /* Insert a tuple into the relation.
     * The tuple must have the same arity and elements
     * within the correct domain (matched with sig)
     */
    virtual void insert( Element **tuple ) =0;

    /*
     * Output the relation to an ostream object
     */
    virtual void output( ostream & ) =0;

    int getArity() const { return arity; }
    
    /* These should be changed to access functions. */
    string *names;
    Domain **signature;

protected:
    int arity;

    INTERFACE( Relation );
};

#endif

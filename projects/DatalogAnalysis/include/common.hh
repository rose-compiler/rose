#ifndef COMMON_HH
#define COMMON_HH

#include <string>
#include <iostream>

using namespace std;


/* the Global Database */
class DBFactory;
extern DBFactory *db;


class Element;
class SgNode;

Element *getElement( SgNode*);

class Domain;

extern Domain *nodes;

/* perhaps this will be used in the move back to separate domains */
#if 0
class Domain;
extern Domain *locations;
extern Domain *types;
extern Domain *symbols;
#endif



/* Use this macro in class definitions where the class should
 * not be copyed
 */
#define NOCOPYING( CLAZZ )  \
    private: \
        CLAZZ( const CLAZZ & ); \
        const CLAZZ &operator=( const CLAZZ & );

/* Class has no default constructors
 */
#define NODEFAULT( CLAZZ ) \
    protected: \
        CLAZZ() ; \
        CLAZZ( const CLAZZ & ); \
        const CLAZZ &operator=( const CLAZZ & );

/* Interfaces are abstract without any need for definition constructos
 */
#define INTERFACE( CLAZZ ) \
    protected: \
        CLAZZ(); \
    public: \
        virtual ~CLAZZ();

#endif

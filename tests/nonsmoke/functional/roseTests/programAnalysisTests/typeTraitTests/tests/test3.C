/*
 __has_nothrow_constructor (type)
 If __has_trivial_constructor (type) is true then the trait is true, else if type is a cv class or union type (or array thereof) with a default constructor that is known not to throw an exception then the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(__has_nothrow_constructor(int));
    if(__has_nothrow_constructor(const int));
    if(!__has_nothrow_constructor(float &));
    if(__has_nothrow_constructor(float[]));
    if(__has_nothrow_constructor(void*));
    
    struct A{
        int i;
    };

    if(__has_nothrow_constructor(A));
    if(__has_nothrow_constructor(const A));
    if(!__has_nothrow_constructor(A&));
    if(__has_nothrow_constructor(A[]));
    if(__has_nothrow_constructor(A*));
    

    class B{
        public:
        B(){}
    };
    // explicit constructor w/o no throw
    if(!__has_nothrow_constructor(B));

    
    class C: B{
    };
    // explicit constructor in the base class w/o no throw
    if(!__has_nothrow_constructor(C));

    
    class D{
    public:
        D()__attribute__((nothrow)){}
    };
    // explicit constructor w/ no throw
    if(__has_nothrow_constructor(D));
    
    
    class E: D{
    };
    // explicit constructor in the base class w nothrow and implicit constructor in the derived type
    if(__has_nothrow_constructor(E));

    class F: B{
        F() __attribute__((nothrow)){}
    };
    // explicit constructor in the derived class w/ nothrow but the base type has explicit constructor w/o nothrow
    if(!__has_nothrow_constructor(F));

    // pointers are always trivial.
    if(__has_nothrow_constructor(F*));
    
}
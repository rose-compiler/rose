/*
 __has_nothrow_copy (type)
 If __has_trivial_copy (type) is true then the trait is true, else if type is a cv class or union type with copy constructors that are known not to throw an exception then the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(__has_nothrow_copy(int));
    if(__has_nothrow_copy(const int));
    if(__has_nothrow_copy(float &));
    if(__has_nothrow_copy(float[]));
    if(__has_nothrow_copy(double *));
    
    struct A{
        int i;
    };

    
    if(__has_nothrow_copy(A));
    if(__has_nothrow_copy(const A));
    if(__has_nothrow_copy(A&));
    if(__has_nothrow_copy(A[]));
    if(__has_nothrow_copy(A*));
    

    class B{
        public:
        B(const B&){}
        B(){}
    };
    // overridden copy
    if(!__has_nothrow_copy(B));

    
    class C: B{
    };
    // overridden copy in the base type
    if(!__has_nothrow_copy(C));

    
    class D{
    public:
        D(const D&)__attribute__((nothrow)){}
    };
    // nothow copy
    if(__has_nothrow_copy(D));
    
    
    class E: D{
    };
    // nothow copy in the inheritance
    if(__has_nothrow_copy(E));

    class F: B{
        F(const F&) __attribute__((nothrow)){}
    };
    // Derived type has nothrow but the base type does not
    if(!__has_nothrow_copy(F));
    
    // pointers are always copy-able
    if(__has_nothrow_copy(F*));
    
}
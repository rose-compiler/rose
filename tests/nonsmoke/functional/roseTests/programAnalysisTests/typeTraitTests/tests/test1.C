// test for __has_nothrow_assign
/*
 __has_nothrow_assign (type)
 If type is const qualified or is a reference type then the trait is false. 
 Otherwise if __has_trivial_assign (type) is true then the trait is true, else if type is a cv class or union type with copy assignment operators that are known not to throw an exception then the trait is true, else it is false.
 Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(__has_nothrow_assign(int));
    if(!__has_nothrow_assign(const int));
    if(!__has_nothrow_assign(float &));
    if(__has_nothrow_assign(float[]));
    if(__has_nothrow_assign(double *));
    
    struct A{
        int i;
    };

    if(__has_nothrow_assign(A));
    
    // const an ref types are not nothrow_assign
    if(!__has_nothrow_assign(const A));
    if(!__has_nothrow_assign(A&));
    if(__has_nothrow_assign(A[]));
    if(__has_nothrow_assign(A*));
    

    class B{
        public:
        B & operator=(const B&){}
    };
    // overloaded assign op
    if(!__has_nothrow_assign(B));

    
    class C: B{
    };
    // overloaded assign op in the inheritance
    if(!__has_nothrow_assign(C));

    
    class D{
    public:
        D & operator=(const D&)__attribute__((nothrow)){}
    };
    // overloaded assign op with nothrow
    if(__has_nothrow_assign(D));
    
    
    class E: D{
    };
    // overloaded assign op with nothrow in the hierarchy
    if(__has_nothrow_assign(E));

    
    
    class F: B{
        F & operator=(const F&) __attribute__((nothrow)){}
    };
    // overloaded assign op with nothrow in derived type but not in the base type
    if(!__has_nothrow_assign(F));
    // Pointers are always assignable
    if(__has_nothrow_assign(F*));

    
    
    
}
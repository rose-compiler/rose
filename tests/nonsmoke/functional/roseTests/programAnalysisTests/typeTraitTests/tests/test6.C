/*
 __has_trivial_constructor (type)
 If __is_pod (type) is true then the trait is true, else if type is a cv class or union type (or array thereof) with a trivial default constructor ([class.ctor]) then the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(__has_trivial_constructor(int));
    if(__has_trivial_constructor(const int));
    if(!__has_trivial_constructor(float &));
    if(__has_trivial_constructor(float[]));
    if(__has_trivial_constructor(void*));
    
    struct A{
        int i;
        struct AA {
            int j;
        };
        AA a;
    };

    if(__has_trivial_constructor(A));
    if(__has_trivial_constructor(const A));
    if(!__has_trivial_constructor(A&));
    if(__has_trivial_constructor(A[]));
    if(__has_trivial_constructor(A*));
    

    class B{
        public:
        B(){}
    };
    // explicity default constructor
    if(!__has_trivial_constructor(B));

    
    class C: B{
    };
    // explicity default constructor in the base type
    if(!__has_trivial_constructor(C));

    
    class D{
    public:
        D()__attribute__((nothrow)){}
    };
    // explicity default constructor (nothrow makes no difference)
    if(!__has_trivial_constructor(D));
    
    
    class E: D{
    };
    // explicity default constructor in the base type (nothrow makes no difference)
    if(!__has_trivial_constructor(E));

    class F: B{
        F() __attribute__((nothrow)){}
    };
    if(!__has_trivial_constructor(F));
    
    // pointers are always trivial
    if(__has_trivial_constructor(F*));
    
}
/*
 __has_trivial_destructor (type)
 If __is_pod (type) is true or type is a reference type then the trait is true, else if type is a cv class or union type (or array thereof) with a trivial destructor ([class.dtor]) then the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(__has_trivial_destructor(int));
    if(__has_trivial_destructor(const int));
    if(__has_trivial_destructor(float &));
    if(__has_trivial_destructor(float[]));
    if(__has_trivial_destructor(void *));
    
    struct A{
        int i;
        struct AA {
            int j;
        };
        AA a;
    };

    if(__has_trivial_destructor(A));
    if(__has_trivial_destructor(const A));
    if(__has_trivial_destructor(A&));
    if(__has_trivial_destructor(A[]));
    if(__has_trivial_destructor(A*));
    

    class B{
        public:
        ~B(){}
    };
    // destructor in the type
    if(!__has_trivial_destructor(B));

    
    class C: B{
    };
    // destructor in the base eventhough none in the derived
    if(!__has_trivial_destructor(C));

    
    class D{
    public:
        ~D(){}
    };
    // destructor in the type
    if(!__has_trivial_destructor(D));
    
    
    class E: D{
    };
    // destructor in the base type even though none in the derived type
    if(!__has_trivial_destructor(E));

    class F: B{
        ~F(){}
    };
    // destructor in both base and derived types
    if(!__has_trivial_destructor(F));
    
    // pointers are trivial
    if(__has_trivial_destructor(F*));
    
}
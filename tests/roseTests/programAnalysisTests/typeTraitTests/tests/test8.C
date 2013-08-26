/*
 __has_virtual_destructor (type)
 If type is a class type with a virtual destructor ([class.dtor]) then the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.

 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(!__has_virtual_destructor(int));
    if(!__has_virtual_destructor(const int));
    if(!__has_virtual_destructor(float &));
    if(!__has_virtual_destructor(float[]));
    if(!__has_virtual_destructor(void *));
    
    struct A{
        int i;
        struct AA {
            int j;
        };
        AA a;
    };

    if(!__has_virtual_destructor(A));
    if(!__has_virtual_destructor(const A));
    if(!__has_virtual_destructor(A&));
    if(!__has_virtual_destructor(A[]));
    if(!__has_virtual_destructor(A*));
    

    class B{
        public:
        ~B(){}
    };
    // not a virtual destructor
    if(!__has_virtual_destructor(B));

    
    class C: B{
    };
    // No virtual destructor in the hierarchy
    if(!__has_virtual_destructor(C));

    
    class D{
    public:
        virtual ~D(){}
    };
    // has virtual destructor
    if(__has_virtual_destructor(D));
    
    
    class E: D{
    };
    // has virtual destructor in the hierarchy
    if(__has_virtual_destructor(E));

    class F: E{
        ~F(){}
    };
    // has virtual destructor in the hierarchy
    if(__has_virtual_destructor(F));
    
    // pointer types don't have destructors
    if(!__has_virtual_destructor(F*));
    
}
/*
 __has_trivial_copy (type)
 If __is_pod (type) is true or type is a reference type then the trait is true, else if type is a cv class or union type with a trivial copy constructor ([class.copy]) then the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
 */


template<class T>
class M{};



void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(__has_trivial_copy(int));
    if(__has_trivial_copy(const int));
    if(__has_trivial_copy(float &));
    if(__has_trivial_copy(float[]));
    if(__has_trivial_copy(void*));
    
    struct A{
        int i;
        struct AA {
            int A;
        };
        AA a;
    };

    if(__has_trivial_copy(A));
    if(__has_trivial_copy(const A));
    if(__has_trivial_copy(A&));
    if(__has_trivial_copy(A[]));
    if(__has_trivial_copy(A*));
    

    class B{
        public:
        B(const B&){}
        B(){}
    };
    // not trivial copy since the type has a copy constructor
    if(!__has_trivial_copy(B));

    
    class C: B{
    };
    // not trivial copy since the base type has a copy constructor
    if(!__has_trivial_copy(C));

    
    class D{
    public:
        D(const D&)__attribute__((nothrow)){}
    };
    // not trivial copy since the type has a copy constructor (nothrow has no role to play)
    if(!__has_trivial_copy(D));
    
    
    class E: D{
    };
    // not trivial copy since the base type has a copy constructor (nothrow has no role to play)
    if(!__has_trivial_copy(E));

    class F: B{
        F(const F&) __attribute__((nothrow)){}
    };
    // not trivial copy since the derived type has a copy constructor (nothrow has no role to play)
    if(!__has_trivial_copy(F));

    // pointers are always trivial
    if(__has_trivial_copy(F*));
    
    // template instantiation on int is trivial copy
    if(__has_trivial_copy(M<int>));
    
    
}
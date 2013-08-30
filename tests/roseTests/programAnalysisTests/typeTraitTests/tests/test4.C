/*
 __has_trivial_assign (type)
 If type is const qualified or is a reference type then the trait is false. Otherwise if __is_pod (type) is true then the trait is true, else if type is a cv class or union type with a trivial copy assignment ([class.copy]) then the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(__has_trivial_assign(int));
    if(!__has_trivial_assign(const int));
    if(!__has_trivial_assign(float &));
    if(__has_trivial_assign(float[]));
    if(__has_trivial_assign(void*));
    
    struct A{
        int i;
        struct AA {
            int i;
        };
        
        AA a;
    };
    
    if(__has_trivial_assign(A));
    if(!__has_trivial_assign(const A));
    if(!__has_trivial_assign(A&));
    if(__has_trivial_assign(A[]));
    if(__has_trivial_assign(A*));
    
    
    class B{
    public:
        B & operator=(const B&){}
    };
    // not trivial since there is overridden assign op
    if(!__has_trivial_assign(B));
    
    
    class C: B{
    };
    // not trivial since there is overridden assign op in the hierarchy
    if(!__has_trivial_assign(C));
    
    
    class D{
    public:
        D & operator=(const D&)__attribute__((nothrow)){}
    };
    // not trivial since there is overridden assign op (nothrow has no role to play here)
    if(!__has_trivial_assign(D));
    
    
    class E: D{
    };
    // not trivial since there is overridden assign op in the hierarchy
    if(!__has_trivial_assign(E));
    
    class F: B{
        F & operator=(const F&) __attribute__((nothrow)){}
    };
    if(!__has_trivial_assign(F));
    
    // pointers are trivial
    if(__has_trivial_assign(F*));
    
}
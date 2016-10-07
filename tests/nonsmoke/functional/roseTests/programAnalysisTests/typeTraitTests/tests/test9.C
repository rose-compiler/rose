/*
 __is_abstract (type)
 If type is a class type with a virtual destructor ([class.dtor]) then the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.

 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(!__is_abstract(int));
    if(!__is_abstract(const int));
    if(!__is_abstract(float &));
    if(!__is_abstract(float[]));
    if(!__is_abstract(void *));
    
    struct A{
        int i;
        struct AA {
            int j;
        };
        AA a;
    };

    if(!__is_abstract(A));
    if(!__is_abstract(const A));
    if(!__is_abstract(A&));
    if(!__is_abstract(A[]));
    if(!__is_abstract(A*));
    

    class B{
        public:
        virtual void B_1(){}
    };
    // has virtual function but not pure virtual
    if(!__is_abstract(B));

    
    class C: B{
        virtual void C_1() = 0;
    };
    // has pure virtual function
    if(__is_abstract(C));

    
    class D{
    public:
        virtual void D_1() =0 ;
    };
    // has pure virtual function
    if(__is_abstract(D));
    
    
    class E: D{
    };
    // has pure virtual function in the hierarchy and not implemented by any.
    if(__is_abstract(E));

    // Pointer types are trivial types
    if(!__is_abstract(E*));

    union U {
        int i;
        int j;
    };

    // Unions can't be abstract
    if(!__is_abstract(U));
    
}
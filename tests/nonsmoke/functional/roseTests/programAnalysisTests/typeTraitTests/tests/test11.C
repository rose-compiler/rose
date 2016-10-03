/*
 __is_class (type)
 If type is a cv class type, and not a union type ([basic.compound]) the trait is true, else it is false.

 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(!__is_class(int));
    if(!__is_class(int* ));
    if(!__is_class(int []));
    
    struct A{
        int i;
        struct AA {
            int j;
        };
        AA a;
    };

    typedef A TA;
    
    if(__is_class(A));
    if(!__is_class(A&));
    if(!__is_class(A*));
    if(__is_class(const A));

    
    if(__is_class(TA));
    if(!__is_class(TA&));
    if(!__is_class(TA*));
    if(__is_class(const TA));


    class B{
        int i;
        struct BB {
            int j;
        };
        BB b;
    };
    
    typedef B TB;
    
    if(__is_class(B));
    if(!__is_class(B&));
    if(!__is_class(B*));
    if(__is_class(const B));

    
    if(__is_class(TB));
    if(!__is_class(TB&));
    if(!__is_class(TB*));
    if(__is_class(const TB));


    union C{
        int i;
        struct CC {
            int j;
        };
        CC c;
    };
    
    if(!__is_class(C));
    if(!__is_class(C&));
    if(!__is_class(C*));
    if(!__is_class(const C));
    
}

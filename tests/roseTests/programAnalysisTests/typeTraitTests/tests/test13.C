/*
 __is_enum (type)
 If type is a cv enumeration type ([basic.compound]) the trait is true, else it is false.
 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(!__is_enum(int));
    if(!__is_enum(int *));
    if(!__is_enum(int []));
    if(!__is_enum(int &));
    struct A{
        int i;
    };
    if(!__is_enum(A));
    if(!__is_enum(A*));
    if(!__is_enum(A[]));
    if(!__is_enum(A&));
    
    enum E {e};

    typedef E ET;
    
    if(__is_enum(E));
    if(__is_enum(const E));

    if(__is_enum(ET));
    if(__is_enum(const ET));
    
    if(!__is_enum(E&));
    if(!__is_enum(const E&));
    if(!__is_enum(E*));
    if(!__is_enum(E[]));
    
}
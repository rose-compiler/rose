/*
 __is_polymorphic (type)
 If type is a polymorphic class ([class.virtual]) then the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
 
 http://www.cplusplus.com/reference/type_traits/is_polymorphic/
 Trait class that identifies whether T is a polymorphic class.
 A polymorphic class is a class that declares or inherits a virtual function.
 */

class A{
};

class B:A{
};

class C: virtual A{
};


class A2{
    virtual void Foo(){}
};

class B2:A2{
};

class C2: virtual B2{
};



void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(!__is_polymorphic(int));

    if(!__is_polymorphic(A));
    if(!__is_polymorphic(B));
    if(!__is_polymorphic(C));

    if(!__is_polymorphic(A[]));
    if(!__is_polymorphic(B[]));
    if(!__is_polymorphic(C[]));

    
    if(__is_polymorphic(A2));
    if(__is_polymorphic(B2));
    if(__is_polymorphic(C2));

    
    if(__is_polymorphic(A2[]));
    if(__is_polymorphic(B2[]));
    if(__is_polymorphic(C2[]));
}
/*
 __is_base_of (base_type, derived_type)
 If base_type is a base class of derived_type ([class.derived]) then the trait is true, otherwise it is false. Top-level cv qualifications of base_type and derived_type are ignored. For the purposes of this trait, a class type is considered is own base. Requires: if __is_class (base_type) and __is_class (derived_type) are true and base_type and derived_type are not the same type (disregarding cv-qualifiers), derived_type shall be a complete type. Diagnostic is produced if this requirement is not met.
 
 http://www.cplusplus.com/reference/type_traits/is_base_of/
 
 Trait class that identifies whether Base is a base class of (or the same class as) Derived, without regard to their const and/or volatile qualification. Only classes that are not unions are considered.
 

 */

void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(!__is_base_of(int, int));
    if(!__is_base_of(int, float));
    if(!__is_base_of(int* , float));
    if(!__is_base_of(int, float *));
    
    struct A{
        int i;
        struct AA {
            int j;
        };
        AA a;
    };

    // A is A's base and igmore const modifier.
    if(__is_base_of(A, A));
    if(__is_base_of(const A, A));
    // pointer, reference, and arry types don't carry this inheritance
    if(!__is_base_of(A&, A));
    if(!__is_base_of(A[], A));
    if(!__is_base_of(A[], A[]));
    if(!__is_base_of(A*, A*));

    class B : A{
    };
    
    // A is B's base
    if(__is_base_of(A, B));
    // B is not A's base
    if(!__is_base_of(B, A));


    class C: B{
    };
    if(__is_base_of(A, C));
    if(__is_base_of(A, const C));
    if(__is_base_of(const A, C));
    if(__is_base_of(A, volatile C));
    if(__is_base_of(volatile A, C));

    if(__is_base_of(B, C));
    if(__is_base_of(B, const C));
    if(__is_base_of(const B, C));
    if(__is_base_of(B, volatile C));
    if(__is_base_of(volatile B, C));

    
    if(!__is_base_of(C, A));
    if(!__is_base_of(C, B));
    if(__is_base_of(C, C));

    
}
/*
 __is_empty (type)
 If type is a cv class type, and not a union type ([basic.compound]) the trait is true, else it is false.

 */


struct A{
    int i:4;
};

struct A2{
};

struct A2_static{
    // static data
    static int i;
    
    // non static function
    void Foo(){}
    
};


class B{
    // static data
    static int i;
    
    // non static function
    void Foo(){}
};

class B_static{
    // non static function
    void Foo(){}
};



union C{
};



class VirtualMember{
    virtual void Foo(){}
};

class VirtualMemberInBase: VirtualMember{
};


class Base{
};

class VirtualInheritance: virtual Base{
};

class VirtualInheritance2: VirtualInheritance{
};

class NonEmptyBase: A{
};


class NonEmptyBase2: VirtualMember{
};



void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(!__is_empty(int));
    if(!__is_empty(int* ));
    if(!__is_empty(int []));
    


    if(!__is_empty(A));
    if(!__is_empty(A&));
    if(!__is_empty(A*));
    if(!__is_empty(const A));

    if(__is_empty(A2));
    if(!__is_empty(A2&));
    if(!__is_empty(A2*));
    if(__is_empty(const A2));
    if(!__is_empty(const A2&));
    
    if(__is_empty(A2_static));
    if(!__is_empty(A2_static&));
    if(!__is_empty(A2_static*));
    if(__is_empty(const A2_static));
    if(!__is_empty(const A2_static&));
    
    if(__is_empty(B));
    if(!__is_empty(B&));
    if(!__is_empty(B*));
    if(__is_empty(const B));
    if(!__is_empty(const B&));

    if(__is_empty(B_static));
    if(!__is_empty(B_static&));
    if(!__is_empty(B_static*));
    if(__is_empty(const B_static));
    if(!__is_empty(const B_static&));
    
    if(!__is_empty(C));
    if(!__is_empty(C&));
    if(!__is_empty(C*));
    if(!__is_empty(const C));
    if(!__is_empty(const C&));
    
    if(!__is_empty(VirtualMember));
    if(!__is_empty(const VirtualMember));
    if(!__is_empty(const VirtualMember&));
    
    if(!__is_empty(VirtualMemberInBase));
    if(!__is_empty(VirtualInheritance));
    if(!__is_empty(VirtualInheritance2));
    if(!__is_empty(NonEmptyBase));
    if(!__is_empty(NonEmptyBase2));
    
}

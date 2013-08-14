/*
 __is_standard_layout (type)
 If type is a standard-layout type ([basic.types]) the trait is true, else it is false. Requires: type shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
 
 
 A standard-layout type is a type with a simple linear data structure and access control that can easily be used to communicate with code written in other programming languages, such as C, either cv-qualified or not. This is true for scalar types, standard-layout classes and arrays of any such types.
 
 A standard-layout class is a class (defined with class, struct or union) that:
 has no virtual functions and no virtual base classes.
 has the same access control (private, protected, public) for all its non-static data members.
 either has no non-static data members in the most derived class and at most one base class with non-static data members, or has no base classes with non-static data members.
 its base class (if any) is itself also a standard-layout class. And,
 has no base classes of the same type as its first non-static data member.
 */

struct SimpleStruct{
    int i;
};


struct NestedStruct{
    int i;
    struct N{
        int j;
    };
    N n;
};


class NoVirtualFunctions{
    int i;
    void Foo(){}
};


class VirtualFunctions{
    int i;
    virtual void Foo(){}
};

class Base{};

class NoVirtualBaseClass: Base{
    int i;
    void Foo(){}
};

class VirtualBaseClass: virtual Base{
    int i;
    void Foo(){}
};



class SameAccessControl{
    int i;
    void Foo(){}
    int j;
private:
    int k;
    void Boo(){}
};




class DifferentAccessControl{
    int i;
    void Foo(){}
    int j;
public:
    int k;
    void Boo(){}
    
};



class BaseWithNoNonStatic{
    static int m;
    static int n;
};
int BaseWithNoNonStatic::m;
int BaseWithNoNonStatic::n;


class MostDerivedHasNonStaticDataAndNoneOfBaseClassesHaveNonStaticData: BaseWithNoNonStatic{
    int i;
    int j;
    static int k;
};
int MostDerivedHasNonStaticDataAndNoneOfBaseClassesHaveNonStaticData::k;


class BaseWithNonStatic{
    int m;
    int n;
};

class MostDerivedHasNonStaticDataAndBaseClassesAlsoHasNonStaticData: BaseWithNonStatic{
    int i;
    int j;
    static int k;
};
int MostDerivedHasNonStaticDataAndBaseClassesAlsoHasNonStaticData::k;



class BaseWithNonStatic2{
    int m;
    int n;
};

class OnlyOneBaseClassHasNonStaticData: BaseWithNonStatic, BaseWithNoNonStatic{
    static int k;
};
int OnlyOneBaseClassHasNonStaticData::k;


class MultipleBaseClassesWithNonStaticData: BaseWithNonStatic, BaseWithNonStatic2{
    static int k;
};

int MultipleBaseClassesWithNonStaticData::k;


class Base2{

};

class Base3 : public Base2 {
    
};

class FirstNonStaticSameAsOneOfBaseTypes: public Base3{
public:
    static int i;
    static int j;
    Base2 k;
};
int FirstNonStaticSameAsOneOfBaseTypes::i;
int FirstNonStaticSameAsOneOfBaseTypes::j;






void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(__is_standard_layout(int));
    if(__is_standard_layout(int&));
    if(__is_standard_layout(int*));
    if(__is_standard_layout(int[]));

    
    if(__is_standard_layout(SimpleStruct));
    if(__is_standard_layout(SimpleStruct&));
    if(__is_standard_layout(SimpleStruct*));
    if(__is_standard_layout(SimpleStruct[]));

    if(__is_standard_layout(NestedStruct));
    if(__is_standard_layout(NestedStruct&));
    if(__is_standard_layout(NestedStruct*));
    if(__is_standard_layout(NestedStruct[]));
    
    
    if(__is_standard_layout(NoVirtualFunctions));
    if(!__is_standard_layout(VirtualFunctions));

    if(__is_standard_layout(NoVirtualBaseClass));
    if(!__is_standard_layout(VirtualBaseClass));

    if(__is_standard_layout(SameAccessControl));
    if(!__is_standard_layout(DifferentAccessControl));
    
    if(__is_standard_layout(MostDerivedHasNonStaticDataAndNoneOfBaseClassesHaveNonStaticData));
    if(!__is_standard_layout(MostDerivedHasNonStaticDataAndBaseClassesAlsoHasNonStaticData));

    if(__is_standard_layout(OnlyOneBaseClassHasNonStaticData));
    if(!__is_standard_layout(MultipleBaseClassesWithNonStaticData));

    if(!__is_standard_layout(FirstNonStaticSameAsOneOfBaseTypes));

    
    
    
}
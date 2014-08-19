// C++11 also adds the ability to prevent inheriting from classes or simply preventing overriding methods 
// in derived classes. This is done with the special identifier final. For example:

struct Base1 final { };

// struct Derived1 : Base1 { }; // ill-formed because the class Base1 has been marked final

struct Base2 {
    virtual void f() final;
};
 
struct Derived2 : Base2 {
//  void f(); // ill-formed because the virtual function Base2::f has been marked final
};


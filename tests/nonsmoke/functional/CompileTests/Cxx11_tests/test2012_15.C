// Explicit overrides and final
// In C++03, it is possible to accidentally create a new virtual function, when one intended to override 
// a base class function. For example:

struct Base {
    virtual void some_func(float);
};
 
struct Derived : Base {
    virtual void some_func(int);
};

// The Derived::some_func is intended to replace the base class version. But because it has a different 
// interface, it creates a second virtual function. This is a common problem, particularly when a user goes 
// to modify the base class.

// C++11 provides syntax to solve this problem.

struct Base2 {
    virtual void some_func(float);
};
 
struct Derived2 : Base2 {
  // virtual void some_func(int) override; // ill-formed because it doesn't override a base class method
};

// The override special identifier means that the compiler will check the base class(es) to see if there 
// is a virtual function with this exact signature. And if there is not, the compiler will error out.

// C++11 also adds the ability to prevent inheriting from classes or simply preventing overriding methods 
// in derived classes. This is done with the special identifier final. For example:

struct Base1 final { };
 
// struct Derived1 : Base1 { }; // ill-formed because the class Base1 has been marked final
 
struct Base3 {
    virtual void f() final;
};
 
struct Derived3 : Base3 {
  // void f(); // ill-formed because the virtual function Base3::f has been marked final
};

// In this example, the virtual void f() final; statement declares a new virtual function, but it also 
// prevents derived classes from overriding it. It also has the effect of preventing derived classes 
// from using that particular function name and parameter combination.

// Note that neither override nor final are language keywords. They are technically identifiers; they gain 
// special meaning only when used in those specific contexts. In any other location, they can be valid 
// identifiers.

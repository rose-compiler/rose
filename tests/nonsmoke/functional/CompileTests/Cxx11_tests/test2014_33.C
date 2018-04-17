struct Base2 {
    virtual void f() final;
};
 
struct Derived2 : Base2 {
//  void f(); // ill-formed because the virtual function Base2::f has been marked final
};


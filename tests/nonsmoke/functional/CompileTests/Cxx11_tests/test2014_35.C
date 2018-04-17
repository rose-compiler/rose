struct Base2 {
   virtual void f() {}
};


struct Derived2 : Base2 {

// Note that both final and override are allowed.
   void f() final override {} 
};


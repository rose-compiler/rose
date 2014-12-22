struct Base {
    virtual void some_func(float);
};

// BUG: Note that the base class is missing in the unparsed code.
struct Derived : Base {
 // virtual void some_func(int) override; // ill-formed because it doesn't override a base class method
 // virtual void some_func(float) override; // properly formed.
    virtual void some_func(float) override {} // properly formed.
};

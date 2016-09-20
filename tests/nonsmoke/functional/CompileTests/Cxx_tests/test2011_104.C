template <class Derived> 
struct Base
{
    void interface()
    {
        // ...
        static_cast<Derived*>(this)->implementation();
        // ...
    }
 
    static void static_func()
    {
        // ...
        Derived::static_sub_func();
        // ...
    }
};

struct Derived : Base<Derived>
{
    void implementation();
    static void static_sub_func();
};

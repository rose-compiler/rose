template<typename T>
struct A
{
    int get() const;
};

// This should preceed the extern template declaration
// but it is not present in the AST from EDG (I think).
template<> int A<long>::get() const;

// extern template class A<int>;
// extern template class A<long>;

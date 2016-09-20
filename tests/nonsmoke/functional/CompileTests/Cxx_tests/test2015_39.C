template<typename T>
struct A
{
    int get() const;
};

// This should preceed the extern template declaration
// but it is not present in the AST from EDG (I think).
template<> int A<long>::get() const;

extern template class A<int>;
extern template class A<long>;

template<typename T>
int A<T>::get() const
{
    return 0;
}

template<>
int A<long>::get() const
{
    return 1;
}

// template class A<int>;
// template class A<long>;

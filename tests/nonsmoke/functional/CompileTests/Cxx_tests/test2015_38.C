template<typename T>
struct A
{
    int get() const;
};

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

template class A<int>;
template class A<long>;

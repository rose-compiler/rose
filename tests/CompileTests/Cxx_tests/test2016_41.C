
#if 1
template <typename T>
inline void Release(T **ptr)
{
}
#endif
void Release2(int * __restrict__ *ptr2)
{
}

template <typename T>
void Release(T * __restrict__ *ptr)
{
}




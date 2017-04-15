// Liao, This is a test to check if ROSE can differentiate 
// T * __restrict__ * ptr   vs. T *  * ptr
#if 1
template < typename T >
void Release ( T * __restrict__ * ptr )
{
}
#endif

void Release2(int * __restrict__ *ptr2)
{
}

template < typename T >
void Release ( T *  * ptr )
{
}

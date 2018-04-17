// Example of recursive template.

// Remove the I/O to make this a smaller AST.
// #include <cstdio>

template <int N>
struct Fibonacci
{
    static const int VAL =
        Fibonacci<N - 1>::VAL +
        Fibonacci<N - 2>::VAL;
};

template <>
struct Fibonacci<1> { static const int VAL = 1; };

template <>
struct Fibonacci<0> { static const int VAL = 1; };

template <int N>
struct FibonacciArray
{
    FibonacciArray<N - 1> rest_of_the_data;

    int this_data;

    FibonacciArray(): this_data(Fibonacci<N>::VAL) {}

    int operator [] (int i) const
    {
        return *( (int *)this + i);
    }
};

template<>
struct FibonacciArray<0>
{
    int this_data;

    FibonacciArray(): this_data(Fibonacci<0>::VAL) {}
};

int main()
{
    const int size = 10;

    const FibonacciArray<size> fib_arr;

#if 0
    for (int i = 0; i < size; ++i)
        printf("%d\n", fib_arr[i]), fflush(stdout);
#endif

    return 0;
}

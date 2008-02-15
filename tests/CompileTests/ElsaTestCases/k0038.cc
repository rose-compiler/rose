// templatized struct as template parameter to function

// from gcc-3.3 <locale> header

// error: `S1' used as a variable, but it's actually a type
// error: `T' used as a variable, but it's actually a type

// ERR-MATCH: used as a variable, but it's actually a type

template<typename T>
T foo(int) {}

template<typename T>
struct S1
{
};

template<typename T>
void bar(T dummy) {
    foo<S1<T> >(42);
}

int main()
{
    int x;
    bar(x);
}

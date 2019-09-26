
template<typename T0>
T0 declval();

template<typename T1, typename = decltype(declval<T1>().~T1())>
void test();

void foo();
auto bar() -> decltype( foo() );


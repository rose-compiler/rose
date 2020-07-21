template<typename T>
struct S {
    constexpr operator bool() const { return true; }
};
 
template<typename T>
    requires (S<T>{})
void f(T); // #1
 
void f(int); // #2
 
void g() {
    f(0); // error: S<int>{} does not have type bool when checking #1,
          // even though #2 is a better match
}


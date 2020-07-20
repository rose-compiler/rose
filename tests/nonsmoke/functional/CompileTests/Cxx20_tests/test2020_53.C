template<typename T>
constexpr bool get_value() { return T::value; }
 
template<typename T>
    requires (sizeof(T) > 1 && get_value<T>())
void f(T); // #1
 
void f(int); // #2
 
void g() {
    f('A'); // OK, calls #2. When checking the constraints of #1,
            // 'sizeof(char) > 1' is not satisfied, so get_value<T>() is not checked
}


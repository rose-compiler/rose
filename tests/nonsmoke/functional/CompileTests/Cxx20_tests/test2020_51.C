template <class T, class U>
concept Derived = std::is_base_of<U, T>::value;
 
template<Derived<Base> T>
void f(T);  // T is constrained by Derived<T, Base>


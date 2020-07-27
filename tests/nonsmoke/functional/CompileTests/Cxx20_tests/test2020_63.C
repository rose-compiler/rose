// DQ (7/21/2020): Concept support is not available in EDG 6.0.

template<typename T> using Ref = T&;
template<typename T> concept C =
requires {
    typename T::inner; // required nested member name
    typename S<T>;     // required class template specialization
    typename Ref<T>;   // required alias template substitution
};
 
template <class T, class U> using CommonType = std::common_type_t<T, U>;
template <class T, class U> concept Common =
requires (T t, U u) {
    typename CommonType<T, U>; // CommonType<T, U> is valid and names a type
    { CommonType<T, U>{std::forward<T>(t)} }; 
    { CommonType<T, U>{std::forward<U>(u)} }; 
};



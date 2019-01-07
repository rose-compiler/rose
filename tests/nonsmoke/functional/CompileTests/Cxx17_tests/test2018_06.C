// Declaring non-type template parameters with auto

template <auto v>    class Y { };
template <auto* p>   class Y<p> { };  // #3
template <auto** pp> class Y<pp> { }; // #4

template <auto* p0>   void g(Y<p0>);  // C
template <auto** pp0> void g(Y<pp0>); // D

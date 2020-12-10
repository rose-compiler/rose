// DQ (7/21/2020): Concept support is not available in EDG 6.0.

template<typename T> concept C2 =
requires(T x) {
    {*x} -> std::convertible_to<typename T::inner>; // the expression *x must be valid
                                                    // AND the type T::inner must be valid
                                                    // AND the result of *x must be convertible to T::inner
    {x + 1} -> std::same_as<int>; // the expression x + 1 must be valid 
                               // AND std::same_as<decltype((x + 1)), int> must be satisfied
                               // i.e., (x + 1) must be a prvalue of type int
    {x * 1} -> std::convertible_to<T>; // the expression x * 1 must be valid
                                       // AND its result must be convertible to T
};



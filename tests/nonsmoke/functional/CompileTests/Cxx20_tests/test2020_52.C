// DQ (7/21/2020): Concept support is not available in EDG 6.0.

template <class T>
concept Integral = std::is_integral<T>::value;
template <class T>
concept SignedIntegral = Integral<T> && std::is_signed<T>::value;
template <class T>
concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;



// DQ (7/21/2020): Concept support is not available in EDG 6.0.

template<typename T> concept A = T::value || true;
template<typename U> 
concept B = A<U*>; // OK: normalized to the disjunction of 
                   // - T::value (with mapping T -> U*) and
                   // - true (with an empty mapping).
                   // No invalid type in mapping even though
                   // T::value is ill-formed for all pointer types
 
template<typename V> 
concept C = B<V&>; // Normalizes to the disjunction of
                   // - T::value (with mapping T-> V&*) and
                   // - true (with an empty mapping).
                   // Invalid type V&* formed in mapping => ill-formed NDR



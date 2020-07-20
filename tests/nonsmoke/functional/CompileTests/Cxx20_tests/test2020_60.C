template<typename T>
concept Addable = requires (T x) { x + x; }; // requires-expression
 
template<typename T> requires Addable<T> // requires-clause, not requires-expression
T add(T a, T b) { return a + b; }
 
template<typename T>
    requires requires (T x) { x + x; } // ad-hoc constraint, note keyword used twice
T add(T a, T b) { return a + b; }



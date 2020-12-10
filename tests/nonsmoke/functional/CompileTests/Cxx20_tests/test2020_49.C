// DQ (7/21/2020): Concept support is not available in EDG 6.0.

template<typename T>
concept V = V<T*>; // error: recursive concept
 
template<class T> concept C1 = true;
template<C1 T>
concept Error1 = true; // Error: C1 T attempts to constrain a concept definition
template<class T> requires C1<T>
concept Error2 = true; // Error: the requires-clause attempts to constrain a concept



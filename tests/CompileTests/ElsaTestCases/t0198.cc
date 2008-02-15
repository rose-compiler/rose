// t0198.cc
// some erroroneous code, related to t0197.cc

template <class T> class A;
            
// parameter type mismatch
//ERROR(1): template <int T> class A {};

// too many parameters
//ERROR(2): template <class S, class T> class A;

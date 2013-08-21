// Example from: http://stackoverflow.com/questions/1511532/variable-length-template-arguments-list
// Simple hack to represent variable length templates arguments.
// This is supported better in C++11 with varadic template parameters.

class nothing1 {};
class nothing2 {};
class nothing3 {};

template <class T1 = nothing1, class T2 = nothing2, class T3 = nothing3> class X : public T1, public T2, public T3 {};

// Example from: http://stackoverflow.com/questions/1511532/variable-length-template-arguments-list
// Simple hack to represent variable length templates arguments.
// This is supported better in C++11 with varadic template parameters.

class nothing1 {};
class nothing2 {};
class nothing3 {};

// template <class T1 = nothing1, class T2 = nothing2, class T3 = nothing3> class X : public T1, public T2, public T3 {};

// Or you can get more sophisticated and use recursion. First you forward-declare the template:

class nothing {};

template <class T1 = nothing, class T2 = nothing, class T3 = nothing> class X;

// Then you specialise for the case where all the parameters are default:

template <> class X<nothing, nothing, nothing> {};

// Then you properly define the general template (which previously you've only forward-declared):

template <class T1, class T2, class T3> class X : public T1, public X<T2, T3> {};


// Notice how in the base class, you inherit X but you miss the first parameter. So they all slide along one place. Eventually they will all be defaults, and the specialization will kick in, which doesn't inherit anything, thus terminating the recursion.


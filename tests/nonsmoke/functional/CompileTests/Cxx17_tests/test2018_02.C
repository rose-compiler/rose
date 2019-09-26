// Example of C++17: Implicitly synthesized Deduction Guides

// template<class ... Ts> struct X { X(Ts...) };
template<class Ts> struct X { X(Ts); };

X x1{1}; // OK X<int>


// These are related, but I'm not yet clear on the correct syntax.
// fails in EDG 5.0: X x11; // OK X<>
// fails in EDG 5.0: template<class T> X xv{(T*)0}; // OK decltype(xv<int>) == X<int*>
// extern X x2; // NOT OK, needs to be a definition
// fails in EDG 5.0: X arr[10];   // OK X<>
// fails in EDG 5.0: X x1{1}, x2{2}; // OK, deduced to the same type X<int>
// X *pointer = 0; // NOT OK
// fails in EDG 5.0: X &&reference = X<int>{1};
// X function(); // NOT OK

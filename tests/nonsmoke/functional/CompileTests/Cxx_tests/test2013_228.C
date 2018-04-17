template<int I, int J> class X { };

// Invalid partial specialization
// template<int I> class X <I * 4, I + 3> { };

// Valid partial specialization
template <int I> class X <I, I> { }; 

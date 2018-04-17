// g0002.cc
// strange extern-template thing (gnu specific?)

//template int x;

// I assume you need a template decl ...
template <class T>
class allocator {};

extern template class allocator<char>;

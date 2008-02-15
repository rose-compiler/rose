// t0523.cc
// explicit inst request doesn't specify throw clause

template <class T>
void f(T) throw() {}

// 'throw' isn't part of the type, so should not be used
// in the signature comparison
template void f(int);

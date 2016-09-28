// t0243.cc
// throw(bad_alloc)
// from my own new.h!

class bad_alloc {};

void* operator new(int size) throw(bad_alloc);             

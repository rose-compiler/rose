

int *ptr;

// test use of reinterpret_cast for conversion of pointer to int (very dangerous code)
// int a = reinterpret_cast<int>(ptr);

void *f;
int *b = static_cast<int*>(f);

#if 0
class X {};
class Y : public X {};
Y* g;
X* c = dynamic_cast<X*>(g);
#endif


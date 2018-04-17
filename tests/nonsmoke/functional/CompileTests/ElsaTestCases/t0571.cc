// t0571.cc
// destroy a union

template <class T>
void destroy(T *p)
{
  p->~T();
}

union U {
  int x;
  float y;
};

void foo()
{
  U *u = 0;
  destroy(u);
}

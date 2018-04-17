void g ();
void g (int *);

template <class T> T m () {}

void r() {
  g(m<int*>());
}

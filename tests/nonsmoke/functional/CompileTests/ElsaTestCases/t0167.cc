// t0167.cc
// variation on d0046.cc: specialization refers to its own
// class using template arguments

template <class T>
struct B {};

template <class T2>
struct B<T2*> {
  void g(B<T2*> &);
};

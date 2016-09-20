// minimized from in/big/nsCLiveconnectFactory.i

template <class T>
struct D : T {};

struct Q {};

template <class T>
struct C {
  C (Q const &h);
  operator D<T> *();
};

struct N {};

Q d(N *a);
int s(N *lhs, N *rhs) {
  C<N> (d (rhs));
  return 0;
}

class S {};
template <class T> class C {};
template <> struct C <S> {};
struct G {
  C<S> m;
};

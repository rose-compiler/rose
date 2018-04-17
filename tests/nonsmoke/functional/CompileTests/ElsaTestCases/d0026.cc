template <class T> struct F {};
template <class T> struct I {
  F<T> m;
};
struct C {
  typedef short char_type;
  typedef F<char_type> f;       // commenting this line out fixes it!
  typedef I<char_type> i;
};

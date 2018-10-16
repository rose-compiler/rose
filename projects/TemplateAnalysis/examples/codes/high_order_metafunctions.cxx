
template <int N>
struct integer {
  constexpr static int value = N;
  typedef integer type ;
};

template <class Arg1 , class Arg2 >
struct multiply : integer < Arg1::value * Arg2::value > {};

template <class F, class X, unsigned N>
struct nest : nest< F, typename F::template apply< X >::type, N-1 > {};

template <class F, class X>
struct nest <F,X ,0 > : X {};

struct squared_f {
  template <class Arg >
  struct apply : multiply <Arg ,Arg > {};
};

template <typename Vin, int rep>
struct nest_suare {
  typedef typename nest< squared_f, Vin, rep >::type type;
};

#ifdef RUNTEST
int main (int , char **) {
  typedef integer<5> five;
  nest_suare< five, 3 >::type::value; // ((5^2) ^2) ^2
}
#endif


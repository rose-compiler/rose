// Here is another one minimized from nsCLiveconnectFactory.i that is
// a counter example to some ideas I've tried.

struct A {};

template<class T> struct E {};

template<class T> struct F {
  void g(E<A> &a) {};
};

E<A> q;

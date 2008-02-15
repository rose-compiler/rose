// from nsCLiveconnectFactory.i
template <class T>
struct E {
  // if you remove this line and (1) below, error goes away; if you
  // move it to the bottom of the class, error does not go away.
  E (T *a);

  // if you make this definition into a declaration one of the errors goes away
  E (const E<T> &s) {}
};

// replacing int with a struct maintains the error
E<int> q = new int;                 // (1)

//  CoderInfo-ootm.ii:10:22: error: variable name `B<T>::S' used as if it were a type
//  CoderInfo-ootm.ii:10:27: error: the name `B<T>::find' is overloaded, but the type `<error> ()(T */*anon*/, int /*anon*/)' doesn't match any of the 2 declared overloaded instances

template<class T> struct B {
  typedef int S;
  S find(T*, int);
  S find(T*);
};

// this is invalid code, because 'typename' is required;
// gcc rejects, icc accepts
//ERROR(1): template<class T> B<T>::S B<T>::find(T*, int) {}

// this is valid
template<class T> typename B<T>::S B<T>::find(T*, int) {}

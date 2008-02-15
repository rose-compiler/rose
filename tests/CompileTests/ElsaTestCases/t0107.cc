// t0107.cc
// really evil ambiguity..

template <class T>
class Foo {
  Foo& get();
};

template <class T>
Foo<T> & Foo<T>::get()
{}


// bad interpretation, ruled out by unparenthesized greater-than
//Foo<   T > &Foo < T   >::get()

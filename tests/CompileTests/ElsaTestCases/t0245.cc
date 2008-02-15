// t0245.cc
// invoking a static method of a template class w/o prior instantiation
// from nsAtomTable.i, copy_string


template <class T>
struct A {
  static void method();
};

void g()
{
  typedef A<unsigned short *> AU;

  // explicit instantiation first would solve it
  //AU x;

  AU::method();

  // this should be an error, but should not cause an assertion failure
  //ERROR(1): A<U>::method();
}



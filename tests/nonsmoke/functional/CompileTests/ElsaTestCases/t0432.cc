// t0432.cc
// erroneous code that is causing assertion failure

void foo()
{
  class C {
    //ERROR(1): template <class T> friend void bar(T t);

    //ERROR(2): template <class T> void bar2(T t);
  };

  //ERROR(3): template <class T> void bar2(T t);
}

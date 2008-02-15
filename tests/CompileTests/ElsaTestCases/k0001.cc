// invoking a pointer to member function of a template class

// originally found in package apt

// error: left side of .* must be a class or reference to a class

// ERR-MATCH: left side of [.][*] must be a class

template <class T> void foo() {
    T t;
    int (T::*bar)() = &T::barfunc;
    t.*bar;     // useless reference to the name
    (t.*bar)();

    T *p = &t;
    (p->*bar)();

    //ERROR(1): int (T::*bar2)() = &T::barfuncqqq;   // does not exist
}

struct A {
  int barfunc();
};

void f()
{
  foo<A>();
}

// explicit instantiation of template loses original template parameter names

// error: there is no type called `K1' (inst from a.ii:13:13)

// originally found in package kdelibs

// ERR-MATCH: there is no type called


// ---- defn then decl ----

template <class K1> struct S1 {
};

// defn
template<class K1> struct S2 {
    typedef S1< K1 > S1;
};

// decl
template <class K2> struct S2;

int foo() {
    S2<int> m;
}




// ---- decl then defn ----
// decl
template <class T1>
struct A;

// defn
template <class T2>
struct A {
  T2 *p;
};

A<int> a;




// ---- defn then decl for func ----
template <class T1>
int bar(T1 t)
{
  T1 *p = 0;
  return 0;
}

template <class T2>
int bar(T2 t);

int b()
{
  int x;
  return bar(x);
}

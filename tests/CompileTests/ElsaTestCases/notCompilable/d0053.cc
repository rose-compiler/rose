// d0053.cc
// test function template argument inference

// The way that this works as a test is that template arugment
// inference is the only way to get an argument for the template.
// Therefore if it doesn't work, you will get an error that there was
// an attempt to instantiate a template and no template argument was
// provided for a template parameter.  An example is provided by the
// call "g1b(a, a)"; it results in "error: No argument for parameter
// `S'"

// ---

//  cppstd: 14.8.2.4 para 9:

//  A template type argument T, a template template argument TT or a
//  template non-type argument i can be deduced if P and A have one of
//  the following forms.

//      T
//      cv-list T
//      T*
//      T&
//      T[integer-constant]
//      template-name<T> (where template-name refers to a class template)
//      type(*)(T)
//      T(*)()
//      T(*)(T)
//      T type:*
//      type T::*
//      T T::*
//      T (type:*)()
//      type (type::*)(T)
//      T (T::*)()
//      T (T::*)(T)
//      type[i]
//      template-name<i> (where template-name refers to a class template)
//      TT<T>
//      TT<i>
//      TT<>

//  where (T) represents argument lists where at least one arugment
//  type contains a T, and () represents argument lists where no
//  parameter contains a T.  Similarly, <T> represents template
//  arugment lists wher at least one argument contians a T, <i>
//  represents template argument lists where at least one argument
//  contians an i and <> represents template argument lists where no
//  argument contains a T or an i.

// ---

//      T
template<class T> void g1(T x) {}

template<class T, class S> void g1b(T x, T y) {}

template<class T, class S> void g1c(T x, S y) {}

//      cv-list T
template<class T> void g2(T const volatile x) {}

//      T*
template<class T> void g3(T *x) {}

//      T&
template<class T> void g4(T &x) {}

//      T[integer-constant]
// NOTE: this is NOT the "type[i]" case; see below
template<class T> void g5(T x[4]) {}

//      template-name<T> (where template-name refers to a class template)
template<class T> struct A1 {};
//ERROR(8): template<class T> struct A1b {}; // distractor
template<class T> void g6(A1<T> x) {}

//      template-name<i> (where template-name refers to a class template)
template<int J> struct A2 {};
template<int I> void g7(A2<I> x) {}

//      type(*)(T)
template<class T> void g8( int(*f)(T x) ) {}

//      T(*)()
template<class T> void g9( T(*f)() ) {}

// testing the combination?
//      T(*)(T)
template<class T> void g10( T(*f)(T x) ) {}

//      T type::*
struct A3 {};
template<class T> void g11( T A3::*x ) {}

//      type T::*
template<class T> void g12( int T::*x ) {}

// testing the combination?
//      T T::*
template<class T> void g13( T T::*x ) {}

//      T (type::*)()
template<class T> void g14( T (A3::*x)() ) {}

//      type (type::*)(T)
template<class T> void g15( A3 (A3::*x)(T y) ) {}

//      T (T::*)()
template<class T> void g16( T (T::*x)() ) {}

// testing the combination?!
//      T (T::*)(T)
template<class T> void g17( T (T::*x)(T y) ) {}

//      type[i]
// 14 April 2004: Scott decided that this feature is more work than it
// is worth so we omit it for now.
// cppstd 14.8.2.4 para 13: Note: except for reference and pointer
// types, a major array bound is not part of a function parameter type
// and cannot be deduced from an argument.
template<int i> void f1(int a[10][i]);
template<int i> void f2(int a[i][20]);
template<int i> void f3(int (&a)[i][20]);
void g()
{
  int v[10][20];
  // FIX: unimplemented
//    f1(v);                        // OK: i deduced to be 20

  // FIX: unimplemented
//    f1<20>(v);                    // OK

  // error: cannot deduce template-argument i
  //ERROR(1): f2(v);
  // FIX: unimplemented
//    f2<10>(v);                    // OK

  // FIX: unimplemented
//    f3(v);                        // OK: i deduced to be 10
}

// FIX: unimplemented; implement when do template template arguments
//      TT<T>
//      TT<i>
//      TT<>

// --- test conversions; in contrast to the above, this section is not
// meant to be exhaustive

// convert an array to a pointer
template<class T> void h1(T *x) {}

// convert a function to a pointer to a function
template<class T> void h2(T (*f)()) {}
int f_h2();

// --- test failure

template<class T, class S> S j1(T x, T y) {}

// --- main body that causes the above to be instantiated

int main() {
  //      T
  struct A {};
  A a;
  g1(a);

  // NOTE: there is nothing to fix here; Just testing that if you fail
  // to deduce an argument, it is caught.
//  in/d0054.cc:147:3: error: No argument for parameter `S'
  //ERROR(10): g1b(a, a);

  // also, just testing that if you inconsistently over-determine the
  // system that it is also caught
  {
    int x;
    short y;
    //  in/d0053.cc:177:5: error: during function template instantiation:  argument 2 `short int &' is incompatable with parameter, `T'
    //ERROR(11): j1(x, y);
  }

  // Check that we iterate over the list and will do more than just
  // the first one.
  struct B {};
  B b;
  g1c(a, b);

  //      cv-list T
  A const volatile a2;
  g2(a2);

  //      T*
  A *a3;
  g3(a3);

  //      T&
  A &a4;
  g4(a4);

  //      T[integer-constant]
  // NOTE: this is NOT the "type[i]" case; see below
  A a5[4];
  g5(a5);

  //      template-name<T> (where template-name refers to a class template)
  A1<A> a6;
  g6(a6);
  // a different but isomorphic atomic type construction should not match
  //ERROR(8):    A1b<A> a6b;
  //ERROR(8):    g6(a6b);

  //      template-name<i> (where template-name refers to a class template)
  A2<17> a7;
  g7(a7);

  //      type(*)(T)
  int (*f8)(int x);
  g8(f8);

  //      T(*)()
  int (*f9)();
  g9(f9);

  // testing the combination?
  //      T(*)(T)
  int (*f10)(int x);
  g10(f10);
  //ERROR(2): int (*f10b)(short x);
  //ERROR(2): g10b(f10b);

  //      T type::*
  int A3::*a11;
  g11(a11);

  //      type T::*
  int A3::*a12;
  g12(a12);

  // testing the combination?
  //      T T::*
  A3 A3::*a13;
  g13(a13);
  //ERROR(3): int A3::*a13;
  //ERROR(3): g13b(a13b);

  //      T (type::*)()
  int (A3::*a14)();
  g14(a14);

  //      type (type::*)(T)
  A3 (A3::*a15)(int y);
  g15(a15);

  //      T (T::*)()
  A3 (A3::*a16)();
  g16(a16);
  //ERROR(4): int (A3::*a16b)();
  //ERROR(4): g16(a16b);

  // testing the combination?!
  //      T (T::*)(T)
  A3 (A3::*a17)(A3 y2);
  g17(a17);
//ERROR(5): int (A3::*a17)(A3 y2);
//ERROR(5): g17b(a17b);
//ERROR(6): A3 (A3::*a17)(int y2);
//ERROR(6): g17c(a17c);
//ERROR(7): struct A4 {};
//ERROR(7): A3 (A4::*a17)(int y2);
//ERROR(7): g17d(a17d);

  // --- test conversions

  // convert an array to a pointer
  int x[4];
  h1(x);

  // convert a function to a pointer to a function; both should work:
  h2(f_h2);                     // without & operator
  h2(&f_h2);                    // with & operator
}

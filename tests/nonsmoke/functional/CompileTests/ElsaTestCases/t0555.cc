// t0555.cc
// explicit specialization of data members using typedefs

namespace N {
  template <class T>
  struct A {
    static int a1;
    static int a2;
    static int a3;
    static int a4;
    static int a5;
    static int a6;

    template <class S>
    struct B {
      static int b1;
      static int b2;
      static int b3;
      static int b4;
      static int b5;
      static int b6;
      static int b7;
      static int b8;
      static int b9;
      static int b10;
    };

    typedef B<float> B_float;
  };

  typedef A<int> A_int;
  typedef A<int>::B<float> A_int_B_float;
}

typedef N::A<int> N_A_int;
typedef N::A<int>::B<float> N_A_int_B_float;


// normal, no typedef
template <>
int N::A<int>::a1 = 1;

// use typedef for A
template <>
int N::A_int::a2 = 2;

// typedef for N and A
template <>
int N_A_int::a3 = 3;


// no typedef
template <>
template <>
int N::A<int>::B<float>::b1 = 1;

// typedef for A
//ERROR(1): template <>     // would be superfluous
template <>
template <>
int N::A_int::B<float>::b2 = 2;

// typedef for B
template <>
template <>
int N::A<int>::B_float::b3 = 3;

// typedef for N and A
template <>                 //ERRORIFMISSING(2): required
template <>
int N_A_int::B<float>::b4 = 4;

// typedef for A and B
template <>
template <>
int N::A_int_B_float::b5 = 5;

// typedef for N and A and B
template <>
template <>
int N_A_int_B_float::b6 = 6;



// similar to above, but with N implicit
using namespace N;

// no typedef
template <>
int A<int>::a4 = 4;

// typedef for A
template <>
int A_int::a5 = 5;

// no typedef
template <>
template <>
int A<int>::B<float>::b7 = 7;

// typedef for A
template <>
template <>
int A_int::B<float>::b8 = 8;

// typedef for B
template <>
template <>
int A<int>::B_float::b9 = 9;

// typedef for A and B
template <>
template <>
int A_int_B_float::b10 = 10;


// t0464.cc
// ambiguous expression list

int f(int,int,int);
int f(int,int,int,int);

namespace N1 {
  template <int n, int m>
  struct A {
    static int B;
  };

  void foo()
  {
    //   +---------arg------+
    //   +----member----+
    //   +--qual---+
    f(1, A < 1 , 2 > :: B < 4, 3);
  }
}


int B;

namespace N2 {
  int A;

  void foo()
  {
    //           +----------+
    //           +------+
    //   +---+       +--+
    f(1, A < 1 , 2 > :: B < 4, 3);
  }
}


// EOF

// t0527.cc
// use an enumerator to hide a type
      
// well-known: use a variable
struct A {};
int A;                       

// news to me: use an enumerator
struct B {
  int f();
};
enum { B };

// but then use it for scope access!
int B::f() { return 1; }

// conflict with explicit typedef isn't ok, though
typedef int C;
//ERROR(1): enum { C };

// nor is it ok to conflict with a variable
int d;
//ERROR(2): enum { d };


// this is ok too
namespace N {
  struct D {
    int f();
  };

  enum { D };
}

using namespace N;

int D::f() { return 1; }



// EOF

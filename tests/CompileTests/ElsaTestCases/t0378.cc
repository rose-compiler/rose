// t0378.cc
// qualified lookup of names after '~'

struct A {
  ~A();
};

typedef A B;
  
namespace N {
  struct C {
    ~C();
  };
};



void foo(A *a, N::C *c)
{
  a->~A();         // explicit dtor call w/o qualification
  a->~B();         // use the typedef

  a->A::~A();      // qualified
  
  // this this legal?  both icc and gcc accept it, but I do not know
  // why; as far as I can tell, ~B should be looked up in the scope of
  // A, which won't find anything
  //a->A::~B();

  c->N::C::~C();
}


// EOF

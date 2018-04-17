// t0451.cc
// continuing to improve coverage, this time with base-class related stuff

// icc fails to reject: 7

//ERROR(1): struct C1 : nonexist {};

typedef int INT;
//ERROR(2): struct C2 : INT {};

//ERROR(3): struct C3 : int {};

struct C4 {
  //ERROR(4): C4() : nonexist(4) {};
  //ERROR(5): C4() : INT(4) {};
  //ERROR(6): C4() : int(4) {};
};

// wrong ctor/dtor names
struct C5 {
  //ERROR(7): C5a();      // maybe icc thinks implicit int?
  //ERROR(8): ~C5b();
};


struct A {
  A(int);
};

struct B {
  B();         // required but I do not notice...
  B(int);
};

struct C : A, virtual B {
  C(int)
    : A(1)
    , B(2)
  {}
};

struct D : A, C {
  D(int)
    : A(1)
    , B(1)
    , C(1)
  {}
};

struct E : A, D {
  E(int)
    : A(1)
    //ERROR(9): , C(1)
    , D(1)
  {}
};

struct F : A, B, D {
  F(int) 
    : A(1)
    //ERROR(10): , B(1)
    , D(1)
  {}
};



// incomplete base class
struct G;
//ERROR(11): struct C6 : G {};



// EOF

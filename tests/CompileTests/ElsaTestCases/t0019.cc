// cc.in19
// base classes

class B {
public:
  int x;
};

class C {
public:
  int z;
  //ERROR(1): int x;     // ambiguous
  
  C() : z(9) {}
};

class D : public B, public C {
public:
  int y;

  int f1() { return y; }    // D's member
  int f2() { return x; }    // B's member
  int f3() { return z; }    // C's member
  
  D() : C() { 18; }
};


class E : virtual public B {};

class F : virtual public B, public E {
public:
  int f4() { return x; }
};

class G : public B, public E {
public:
  // this one won't work because the virtual is missing along one path
  //ERROR(2): int f4() { return x; }    // ambiguous
};

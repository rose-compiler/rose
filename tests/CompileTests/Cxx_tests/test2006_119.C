// virtual method virtual_foo and virtual destructor
class A1 {
public:
  // no user-defined constructor
  A1() {}
  virtual ~A1();
  virtual int virtual_foo();
};

class B1 : public A1 {
public:
  // no user-defined constructor
  ~B1();
  int virtual_foo();
};

// non-virtual method non_virutal_foo_XX and non-virtual destructor
class A2 {
public:
  A2();
  ~A2();
  int nonvirtual_foo();
};

class B2 : public A2 {
public:
  B2();
  ~B2();
  int nonvirtual_foo();
};

// virtual method virtual_foo and virtual destructor
// (AND declared virtual in the derived class)
class A3 {
public:
  // no user-defined constructor
  A3() {}
  virtual ~A3();
  virtual int virtual_foo();
};

class B3 : public A3 {
public:
  // no user-defined constructor
  virtual ~B3();
  virtual int virtual_foo();
};


A1::~A1() {}
B1::~B1() {}

A2::A2() {}
A2::~A2() {}
B2::B2() {}
B2::~B2() {}

A3::~A3() {}
B3::~B3() {}

int A1::virtual_foo() { return 11; }
int B1::virtual_foo() { return 12; }
int A2::nonvirtual_foo() { return 21; }
int B2::nonvirtual_foo() { return 22; }
int A3::virtual_foo() { return 31; }
int B3::virtual_foo() { return 32; }

int main() {
  A1* a1=new A1();
  B1* b1=new B1();
  a1->virtual_foo();
  b1->virtual_foo();
  delete a1;
  delete b1;

  A2* a2=new A2();
  B2* b2=new B2();
  a2->nonvirtual_foo();
  b2->nonvirtual_foo();
  delete a2;
  delete b2;

  A3* a3=new A3();
  B3* b3=new B3();
  a3->virtual_foo();
  b3->virtual_foo();
  delete a3;
  delete b3;

  return 0;
}



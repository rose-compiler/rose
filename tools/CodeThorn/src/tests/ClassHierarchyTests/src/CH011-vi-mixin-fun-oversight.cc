
struct Mixin
{
  virtual void f() = 0;
};

// OVERSIGHT: inheritance misses virtual
struct MixinImpl : Mixin
{
  void f() override {}

  void test(MixinImpl) {}
};

struct A : virtual Mixin
{
  virtual void run() {}
};

struct B : A
{
  virtual void go() {}
  virtual void run() override = 0;
};

struct C : virtual MixinImpl, B
{  
  void run() override {}

  // class still considered abstract
  // void test(C) {}
};

struct D : virtual Mixin, C
{
  // void test(D) {}
};

struct E : D
{
  void f() override {}

  void test(E) {}
};



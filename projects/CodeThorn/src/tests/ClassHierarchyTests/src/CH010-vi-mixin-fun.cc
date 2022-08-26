
struct Mixin
{
  virtual void f() = 0;
};

struct MixinImpl : virtual Mixin
{
  void f() override {}
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
};

struct D : virtual Mixin, C
{
};



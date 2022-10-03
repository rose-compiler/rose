
struct Mixin
{
  virtual ~Mixin() = 0;
};

struct MixinImpl : Mixin
{
  ~MixinImpl() = default;

  void test(MixinImpl) {}
};

struct A : virtual Mixin
{
  virtual ~A() = 0;
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

  void test(C) {}
};

struct D : virtual Mixin, C
{
  void test(D) {}
};



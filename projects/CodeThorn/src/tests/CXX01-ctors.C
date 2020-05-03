#define assert(X)

namespace std
{
  typedef long unsigned int size_t;
}

void* operator new(std::size_t, void* __p) noexcept
{
  return __p;
}

namespace defs
{
  enum ID : int
  {
    a, b, c, d, last
  };
}

struct A
{
  A()
  : id(defs::a)
  {}

  A(int)
  : id(defs::a)
  {}

  virtual ~A() {}

  defs::ID id;
};

struct X
{
  X() {}
};

struct B : A, virtual X
{
  B()
  {
    id = defs::b;
  }
};


struct C : A, virtual X
{
  typedef A base;

  C()
  : base()
  {
    id = defs::c;
  }
};

struct D : B, C
{
  D()
  : B()
  {
    B::id = C::id = defs::d;
  }
};


A* createObject(int i)
{
  A* res = nullptr;

  switch (i % defs::last)
  {
    case defs::a :
    {
      res = new A;
      break;
    }

    case defs::b :
    {
      void* place = new char[sizeof(B)];

      res = new (place) B;
      break;
    }

    case defs::c :
    {
      res = ::new C();
      break;
    }

    case defs::d :
    {
      res = static_cast<C*>(new D);
      break;
    }

    default: ;
  }

  assert(res);
  return res;
}

int main(int argc, char** argv)
{
  //~ A  a_default;
  A  a(2);
  A* mayLeak = &a;

  for (int i = argc; i >= 0; --i)
  {
    mayLeak = createObject(i);
  }
}



#if WITH_CCTOR

A& cloneObject(A& a)
{
  static int ctr = 0;

  A* res = nullptr;

  switch (a.id)
  {
    case class_id::a :
    {
      res = new A(a);
    }

    case class_id::b :
    {
      res = malloc(sizeof(B));
      new (res) B(dynamic_cast<B&>(a));
    }

    case class_id::c :
    {
      res = ::new C(dynamic_cast<C&&>(c));
    }

    case class_id::d :
    {
      res = new D(dynamic_cast<D&&>(d));
    }

    default:
      assert(false);
  }

  return res;
}


#endif /* WITH_CCTOR */

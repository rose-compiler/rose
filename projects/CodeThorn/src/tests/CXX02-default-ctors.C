#define assert(X)

namespace std
{
  typedef long unsigned int size_t;
}

void* operator new(std::size_t, void* __p) noexcept
{
  return __p;
}

struct A
{
  A(int a_init = 8)
  : a(a_init)
  {}
  
  virtual ~A() {}

  int a;
};

struct X
{
  X() {}
  
  int x;
};

struct B : A, virtual X
{
  int b;
};


struct C : A, virtual X
{
  float c;
};

struct D : B, C
{
  double d;
};


A* createObject(int i)
{
  A* res = nullptr;

  switch (i)
  {
    case 0:
    {
      res = new A;
      break;
    }

    case 1:
    {
      void* place = new char[sizeof(B)];

      res = new (place) B;
      break;
    }

    case 2:
    {
      res = ::new C();
      break;
    }

    case 3:
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

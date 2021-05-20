

struct PrimaryBase
{
  virtual ~PrimaryBase() {}
  int a;
};

struct UnrelatedBase
{
  virtual ~UnrelatedBase() {}

  int i;
};

struct B : PrimaryBase {};

struct C : PrimaryBase { int x; };

struct D : B, C {};

struct X : virtual PrimaryBase { int x; };
struct Y : virtual PrimaryBase { int y; };

struct Z : virtual X, virtual Y, UnrelatedBase
{
  int z;
};

PrimaryBase* cast_primaryBase(UnrelatedBase* obj)
{
  return dynamic_cast<PrimaryBase*>(obj);
}


D* cast_D(PrimaryBase* obj)
{
  return dynamic_cast<D*>(obj);
}


int main()
{
  UnrelatedBase* iobj = new UnrelatedBase;
  PrimaryBase*   aobj = cast_primaryBase(iobj);
  D*             dobj = cast_D(aobj); 
}





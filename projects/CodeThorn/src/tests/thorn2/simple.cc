

struct PrimaryBase
{
  virtual ~PrimaryBase() {}
  virtual int id() = 0;
  int a;
};

struct UnrelatedBase
{
  virtual ~UnrelatedBase() {}

  int i;
};

struct B : PrimaryBase 
{
  int id() { return 'B'; }

  virtual B* clone() const { return new B(*this); }
};

struct C : PrimaryBase { int x; };

struct D : B, C 
{
  int id() { return 'D'; }
  D* clone() const override { return new D(*this); }
};

struct X : virtual PrimaryBase { int x; };

struct Y : virtual PrimaryBase 
{ 
  int y; 
  int id() { return 'Y'; }
};

struct Z : virtual X, virtual Y, UnrelatedBase
{
  int z;
  int id() { return 'Z'; }
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





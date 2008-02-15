// t0310.cc
// 5.16 para 5

// gcc (3.4.0) erroneously rejects many of the things this file tests,
// but Intel C++ (EDG) accepts it

struct Int {
  operator int();
};

struct Int2 {
  operator int();
};

struct Long {
  operator long();
};

struct Long2 {
  operator long();
};

struct IntLong {
  operator int();
  operator long();
};

struct IntLong2 {
  operator int();
  operator long();
};


struct PInt {
  operator int*();
};

struct PInt2 {
  operator int*();
};

struct PLong {
  operator long*();
};

struct PLong2 {
  operator long*();
};

struct PIntLong {
  operator int*();
  operator long*();
};

struct PIntLong2 {
  operator int*();
  operator long*();
};

       
//       A        .
//      / \       .
//     B   C      .
//      \ /       .
//       D        .
struct A {};
struct B : public A {};
struct C : public A {};
struct D : public B, public C {};

struct PA {
  operator A* ();
};
struct PB {
  operator B* ();
};
struct PC {
  operator C* ();
};
struct PD {
  operator D* ();
};


struct AcceptInt {
  void func(int i);
private:
  void func(long l);
};

struct AcceptLong {
  void func(long i);
private:
  void func(int l);
};

struct AcceptPInt {
  void func(int *i);
private:
  void func(long *l);
};

struct AcceptPLong {
  void func(long *i);
private:
  void func(int *l);
};

struct AcceptPA {
  void func(A*);    
  private:  void func(B*);  void func(C*);  void func(D*);
};
struct AcceptPB {
  void func(B*);    
  private:  void func(A*);  void func(C*);  void func(D*);
};
struct AcceptPC {
  void func(C*);    
  private:  void func(A*);  void func(B*);  void func(D*);
};
struct AcceptPD {
  void func(D*);    
  private:  void func(A*);  void func(B*);  void func(C*);
};


// this structure contains an overloaded 'op' set that is a (finite) subset
// of the signatures specified for '?:'; by filling it with all the relevant
// members, I can compare real overload resolution to what happens for '?:'
struct LikeQuestion {
  // arithmetic types
  int op(bool,int,int);
  long op(bool,int,long);
  long op(bool,long,int);
  long op(bool,long,long);

  // pointer types
  int* op(bool,int*,int*);
  long* op(bool,long*,long*);
  A* op(bool,A*,A*);
  B* op(bool,B*,B*);
  C* op(bool,C*,C*);
  D* op(bool,D*,D*);
};


void f(int x)
{
  Int i;
  Int2 i2;
  Long l;
  Long2 l2;
  IntLong il;
  IntLong2 il2;

  PInt pi;
  PInt2 pi2;
  PLong pl;
  PLong2 pl2;
  PIntLong pil;
  PIntLong2 pil2;

  PA pa;
  PB pb;
  PC pc;
  PD pd;

  AcceptInt accInt;
  AcceptLong accLong;
  AcceptPInt accPInt;
  AcceptPLong accPLong;
  AcceptPA accPA;
  AcceptPB accPB;
  AcceptPC accPC;
  AcceptPD accPD;

  LikeQuestion lq;

  // result is int
  __checkType(x? i : i2, (int)0);

  // result is long
  __checkType(x? i : l, (long)0);

  // result is long
  __checkType(x? l2 : l, (long)0);

  // ambiguous
  //ERROR(4): (x? i : il);

  // result is int*
  __checkType(x? pi : pi2, (int*)0);

  // result is int*
  __checkType(x? pi : pil, (int*)0);

  // result is long*
  __checkType(x? pl : pil, (long*)0);

  // ambiguous
  //ERROR(8): (x? pil : pil2);

  // result is A*
  __checkType(x? pa : pc, (A*)0);

  // result is B*
  __checkType(x? pb : pd, (B*)0);

  // result is A*
  __checkType(x? pb : pc, (A*)0);            // icc gets this wrong too
}




// EOF

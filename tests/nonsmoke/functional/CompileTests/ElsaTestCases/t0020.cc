// cc.in20
// operators..


class B {
public:
  operator char ();
  operator int ();

  int f(char);
};


class Bar {};

class Foo : public B {
public:
  Foo operator- (Foo &f);

  // no reason to do this
  //static Foo operator/ (Foo &f1, Foo &f2);

  // instead do this
  friend Foo operator/ (Foo &f1, Foo &f2);

  operator int ();
  operator short ();
  //operator Bar ();

  int f(short);
  int f(int);
  //ERROR(1): int f(int);    // duplicate definition
};

Foo Foo::operator- (Foo &f)
{
  return f;
}

// static would look like
//Foo Foo::operator/ (Foo &f1, Foo &f2) {}

// this is the friend
Foo operator/ (Foo &f1, Foo &f2) {}


Foo operator+ (Foo &f1, Foo &f2);

Bar operator+ (Bar &f1, Bar &f2);
  
// these can never be nonmembers
//operator int (Foo &f);


int main()
{
  Foo x, y;
  x + y;    // 2
  x - y;
  
  int q;
  q = x;    // could use "operator int (Foo &)"
  
  x.f(9);
}

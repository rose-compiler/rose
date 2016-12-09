// t0334.cc
// initialization by conversion
// from qt/src/kernel/qfont_x11.cpp

class QChar {
};

class QCharRef {
public:
  operator QChar ();
};

void f(QChar const &c);
void f2(QChar &c);

void foo()
{
  QCharRef r;

  f(r);            // init an ordinary parameter
  QChar c1(r);     // init with explicit copy ctor
  QChar c2 = r;    // init with implicit copy ctor
  
  // illegal b/c a temporary gets bound to a non-const reference
  //ERROR(1): f2(r);
}

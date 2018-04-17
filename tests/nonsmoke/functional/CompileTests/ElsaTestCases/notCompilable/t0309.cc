// t0309.cc
// 5.16 para 3, 4

struct Base {
  Base();
  void baseFunc();
  void baseFuncC() const;
};
struct Derived : public Base {
  Derived();
};

struct ConvToInt {
  operator int ();
};

struct ConvToIntRef {
  operator int& ();
};

struct ConvFromInt {
  ConvFromInt(int);
};

struct ConvBoth {
  ConvBoth(int);
  operator int ();
};

struct ConvToCharOrShort {
  operator char ();
  operator short ();
};

void f(int x, int y)
{
  Base b;
  Base const bc;
  Derived d;
  Derived const dc;

  int i = 3, j = 7;
  long k = 8;
  ConvToInt toInt;
  ConvToIntRef toIntRef;
  ConvFromInt fromInt(1);
  ConvBoth both(1);
  ConvToCharOrShort charShort;

  switch (x) {
    case 1:
      // conv second to lvalue of third
      (y? toIntRef : i) = 3;
      break;

    case 2:
      // conv third to lvalue of second
      (y? i : toIntRef) = 3;
      break;

    case 3:
      // derived-to-base as lvalue conv
      (y? b : d).baseFunc();
      break;

    case 4:
      // derived-to-base as rvalue conv
      //ERROR(1): (y? bc : d).baseFunc();
      (y? bc : d).baseFuncC();
      break;

    case 41:
      // actual rvalue, rather than ref-to-const?
      (y? Base() : Derived()).baseFunc();
      break;

    case 5:
      // illegal due to base having less cv qualification
      //ERROR(2): (y? b : dc);
      break;

    case 6:
      // second to rvalue of third
      x = (y? toInt : i);
      __checkType(y? toInt : i, (int)0);
      //(elsa doesn't detect this..) ERROR(3): (y? toInt : i) = 3;
      break;
      
    case 7:
      // can't tell which direction to convert
      //ERROR(4): (y? both : i);
      break;
      
    case 8:
      // can't convert in either direction
      //ERROR(5): (y? b : i);
      break;

    case 9:
      // simple lval situation
      (y? i : j) = 9;
      break;

    case 10:
      // simple rval situation
      (y? 1 : 2);
      __checkType(y? 1 : 2, (int)0);
      //(elsa doesn't detect) ERROR(6): (y? 1 : 2) = 10;
      break;

    case 11:
      // rval due to different types
      (y? i : k);
      __checkType(y? i : k, (long)0);
      //(elsa doesn't detect) ERROR(7): (y? i : k) = 11;
      break;
      
    case 12:
      // ambiguous th->el or el->th conversion
      (y? charShort : 'a'/*char*/);
      //ERROR(6): (y? charShort : 1/*int*/);
      //ERROR(7): (y? 1/*int*/ : charShort);
      break;
  }
}


// EOF

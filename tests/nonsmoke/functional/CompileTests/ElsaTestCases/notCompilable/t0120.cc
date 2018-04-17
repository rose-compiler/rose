// t0120.cc
// trying to test overload.cc; be careful with line numbers!

int f1();                                 // line 4
int f1(int);                              // line 5

int x1a = __testOverload(f1(), 4);
int x1b = __testOverload(f1(2), 5);
//ERROR(1): int x1c = f1(1,2);            // "no viable candidate"
int x1d = __testOverload(f1(2.5), 5);

int f1(short);                            // line 12

//ERROR(2): int x1e = f1(2.5);            // "ambiguous"


enum E1 { e1 };
enum E2 { e2 };
enum E3 { e3 };
enum E4 { e4 };
enum E5 { e5 };
enum E6 { e6 };

int f2(E1);                               // line 24
int f2(E2);                               // line 25
int f2(E3);                               // line 26
int f2(E4);                               // line 27
int f2(E5);                               // line 28
int f2(E6);                               // line 29

// this is intended to test the tournament algorithm
int x2a = __testOverload(f2(e1), 24);
int x2b = __testOverload(f2(e2), 25);
int x2c = __testOverload(f2(e3), 26);
int x2d = __testOverload(f2(e4), 27);
int x2e = __testOverload(f2(e5), 28);
int x2f = __testOverload(f2(e6), 29);


int f3(int);                              // line 40
int f3(...);                              // line 41

// test ellipsis vs others
int x3a = __testOverload(f3(), 41);
int x3b = __testOverload(f3(2), 40);
int x3c = __testOverload(f3(2.3), 40);
int x3d = __testOverload(f3(2, 3), 41);


int f4(int x = 5);                        // line 50
int f4(float);                            // line 51

int x4a = __testOverload(f4(), 50);
int x4b = __testOverload(f4(3), 50);
int x4c = __testOverload(f4(3.5f), 51);
//ERROR(3): int x4d = f4(2,3);            // "no viable"


class Src { public: Src(int); };
struct Dest {
  Dest(Src const &);                      // line 61
  Dest(E1);                               // line 62 
};

int f5(int);                              // line 65
int f5(Dest);                             // line 66
int f5(E1);                               // line 67

int x5a = __testOverload(f5(2), 65);          // no ctor is viable
int x5b = __testOverload(f5(Dest(e1)), 66);   // SC_IDENTITY
int x5c = __testOverload(f5(Src(2)), 66);     // convert via ctor
int x5d = __testOverload(f5(e1), 67);         // ctor is viable, but not best










// eof

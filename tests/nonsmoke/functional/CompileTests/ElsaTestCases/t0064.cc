// cc.in64
// problem with (void) parameter lists

class Foo {
public:
  int f(void);
};

int Foo::f()
{}


// no!
//ERROR(1): int g(void x);

// no!
//ERROR(2): int h(void, int);

// no!
//ERROR(3): int i(void = 4);


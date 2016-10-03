// cc.in69
// odd behavior of forward decls inside other constructs

// -----------------------
class Foo {
public:
  class Bar *b;    // functions as fwd decl (?)
  int f();
};

class Bar {
public:
  int x;
};

int Foo::f()
{
  return b->x;
}


// -----------------------
void g(class Baz *b);     // this should also introduce the name

Baz *z;


// but this won't work because it's captured by the paramter scope
void h(struct Frozz { int y; } *q);

//ERROR(1): Frozz *qq;


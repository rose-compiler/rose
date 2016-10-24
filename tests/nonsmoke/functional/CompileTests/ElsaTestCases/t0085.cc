// cc.in85
// demonstrate inability to simply separate qual_tcheck into
// two whole-AST passes to deal with scoping

class Foo {
public:
  int f()
  {
    x y;            // y's qvar needs x's qtype
    return y;
  }

  typedef int x;    // this is what sets x's qtype
};

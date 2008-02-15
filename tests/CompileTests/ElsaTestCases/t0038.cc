// cc.in38
// problem with order of processing inner class inline members

int f(int x);

class StringDict {
  class Iter {
  public:
    Iter(StringDict &dict) { dict.getIter(); }

    // ambiguous, so I need to check the bodies..
    int foo() { return f(3); }
  };
  Iter getIter();
};
     

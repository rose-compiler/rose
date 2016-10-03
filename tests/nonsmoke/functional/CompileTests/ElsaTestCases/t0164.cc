// t0164.cc
// derived from t0078.cc, but with a ctor for hash

//    struct hash2 {
//      int x;
//      //hash2(hash2 &other) /*: x(other.x)*/ {}
//      hash2(hash2 &other);
//    };

struct Foo {
  struct hash {
    int x;
    hash(hash &other) : x(other.x) {}
    //hash(hash &other);
  } hash;
//    Foo(Foo &other) : hash(other.hash) {}
};

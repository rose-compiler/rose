// declaring a constructor with class name

// originally found in package groff
// template version originally found in package aime

// error: undeclared identifier `S::S'

// error: the name `S::S' is overloaded, but the type `()(int x)' doesn't
// match any of the 2 declar

// ERR-MATCH: undeclared identifier .*::

struct S1 {
    S1::S1(int x) {}
};


struct S2 {
    S2::S2(int x);
};
S2::S2(int x) {}


template<class T> struct S3 {
    S3<T>::S3() {}
};

S3<int> s3;

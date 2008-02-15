// t0544.cc
// initialize an array of elements using a ctor

class A {
public:
  A(int);
};

A a[] = {
  1, 2
};


struct C {};


// initialize an "aggregate"
struct B 
//ERROR(2): :C                  // base class
{
//ERROR(1): private:            // private data
  int x, y;

//ERROR(3): virtual int f();    // virtual method

//ERROR(4): B(C*);              // incompatible user-def'd constructor
};

B b[] = {
  1,2,3,4,5,6
};



// ------- fragment from cc_type.cc -------
enum SimpleTypeId {
  ST_CHAR, ST_UNSIGNED_CHAR,
};

class SimpleType {
public:  
  SimpleType (SimpleTypeId t);
};

SimpleType fixed[] = {
  SimpleType (ST_CHAR),
};

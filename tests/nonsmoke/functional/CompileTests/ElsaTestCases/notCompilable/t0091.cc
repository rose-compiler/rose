// t0091.cc
// problem with explicit dtor call

class morkNode {
  ~morkNode();
  void foo();
};

void morkNode::foo()
{
  this->~morkNode();
  this->morkNode::~morkNode();
  this->morkNode::foo();
}



class forkNode {
  void foo();
};

void forkNode::foo()
{
  this->~forkNode();
  this->forkNode::~forkNode();
}


typedef unsigned uint;

void f()
{
  uint x;
  x.~uint();    // legal
  
  //ERROR(1): x.~uint(1);   // not legal
}


typedef unsigned uintArr[3];

void g()
{
  uintArr x;
  x.~uintArr();
}

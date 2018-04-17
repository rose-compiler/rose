/*
Jochen and I found the following bug while compiling ROSE. Compiling the
following code with ROSE:

gives the error:

rose_test.C:14: error: `none' was not declared in this scope
rose_test.C: In member function `void Y::func2(X)':
rose_test.C:25: error: `none' undeclared (first use this function)
rose_test.C:25: error: (Each undeclared identifier is reported only once for
   each function it appears in.)

and the output from ROSE is:

class X
{
  public: enum Component {none=-1,p,u,v,w}comp_in_staggered_grid;
  void setComponent(enum X::Component comp);
}

;

class Y
{


  public: inline void func1(const class X x,enum X::Component comp=none)
{
  }

  void func2(const class X x);
}

;

void Y::func2(const class X x)
{
  (this) -> func1(x,none);
}

As you can see the 'X::' disappears from 'X::none'. Normally I see that
EDG replaces the enum value with an integer.

Thanks
Andreas
*/


class X{
  public:
    enum Component { none=-1, p, u, v, w } comp_in_staggered_grid;
    void setComponent (X::Component comp);
};

class Y{
  public:
   void func1(const X x, X::Component  comp = X:: none){

  };
  void func2(const X x);
};

void Y::func2(const X x){

  func1(x, X::none);

};




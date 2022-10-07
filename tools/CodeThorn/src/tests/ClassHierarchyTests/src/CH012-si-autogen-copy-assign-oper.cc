
struct B;

struct A 
{
  virtual A& operator=(const B&) = 0;
};

struct B : A
{
  // compiler generates overrider -> B is concrete.
  // virtual A& operator=(const B&) = 0;
  
  // B is concrete and can be passed by copy
  void test(B) {}
};

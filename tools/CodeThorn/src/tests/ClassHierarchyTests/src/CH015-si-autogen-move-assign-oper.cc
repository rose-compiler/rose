
struct B;

struct A 
{
  virtual A& operator=(B&&) = 0;
};

struct B : A
{
  // compiler generates overrider -> B is concrete.
  // virtual A& operator=(const B&) = 0;
  //

  //B(const B&) {}
  //B& operator=(B&&) && { return *this; }
  //B& operator=(B&&)    { return *this; }
  // B& operator=(const B&) { return *this; }

  // B is concrete and can be passed by copy
  void test(B) {}
};

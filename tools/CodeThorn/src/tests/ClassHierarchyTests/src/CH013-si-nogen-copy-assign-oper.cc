
struct B;

struct A 
{
  virtual A& operator=(const B&) = 0;
};

// B remains abstract because the user defined
// copy constructor prevents the compiler from
// generating an overrider.
struct B : A
{
  B& operator=(B& b) { return *this = b; }
};

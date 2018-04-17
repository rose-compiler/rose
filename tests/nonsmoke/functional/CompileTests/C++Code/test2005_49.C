/* Example structImplicitConstruct.cpp from Andreas */

/*
   This problem occurs when you have an implicit call to the
   default constructor in a declared and defined class-like
   structure while constructing that struct as a variable 
   reference in the scope of another class-like structure.
 */

struct First 
{
//Constructor must be defined if problem is to occur
  public:
  First(){};
};

struct Second 
{
 public: 
//No problem if call to constructor of the class-like
//structure is called explicitly
  First first();
//Assertion if constructor of the struct is called
//implicitly
  First second;
}

;

int main()
{
  
  First noProblemOutside;
  First noProblemOutsideAlso();
  Second noProblem();
  Second givesAssertion;
  return 0;
}


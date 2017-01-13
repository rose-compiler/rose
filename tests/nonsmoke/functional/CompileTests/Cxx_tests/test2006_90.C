#if 0
// Example code from Jeremiah
class A {
  int z;
  public:
  A(): z(3) {}
  A(int w, int x): z(w + x) {}
  int zz() const {return z;}
};

int main(int, char**) {
  bool b;
  int x = (b ? A(5, 3) : A()).zz() + 55;
  // int x = b ? A(3).zz() : 1;
  A(3, 4).zz();
  A zzz(3, 7);
  return 0;
} 
#endif

#if 0
// previous version of test code
class A 
{
   public:
      int foo() const ;
      
};

bool g() 
{
   return true;
}


void foo()
  {
 // double x = double();
 // int y = int();
     bool b;
     
     int z = b ? A().foo() : 5;
  }
#endif

void foobar()
  {
 // This generates a constructor initializer using a primative type!
 // This is what should justify the use of primative types within the 
 // SgConstructorInitializer.
    double* x = new double();

 // This is a C style cast using C style notation
    int y1 = (int)1ULL;

 // This is a C style cast using constructor notation (semantically equivalent to the 
 // C style notation, though this is preferred by some in C++). We unparse the C style
 // notation within ROSE.
    int y2 = int(1ULL);

 // This is an assignment initializer using the "default value" for type int (zero is the default value for int so we get zero internally).
    int z = int();

#if 0
    int* y_ptr = &y1;

 // Call the integer destructor!
 // y_ptr->~int();
    typedef int xxx;

 // unparses as: "y_ptr -> ~int;" but should unparse as "typedef int xxx; y_ptr->~xxx();"
    y_ptr->~xxx();
#endif
  }

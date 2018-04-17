// section 3.4.3 example #1

class A {
public: 
  static int n; 
}; 
int main() 
{ 
  // the example includes this line
  //   int A;
  // but my current implementation can't handle it..

  A::n = 42;          // OK    
  //ERROR1: A b;                // ill-formed: A does not name a type
}


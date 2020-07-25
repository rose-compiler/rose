
// C++ code to demonstrate the 
// use of defaulted functions 
// #include <iostream> 
using namespace std; 
  
class A { 
public: 
  
    // A user-defined  
    // parameterized constructor 
    A(int x)  
    { 
   // cout << "This is a parameterized constructor"; 
    } 
      
    // Using the default specifier to instruct 
    // the compiler to create the default  
    // implementation of the constructor. 
    A() = default;  
}; 
  
int main() 
{ 
    // executes using defaulted constructor 
    A a;  
      
    // uses parametrized constructor 
    A x(1);  
    return 0; 
} 


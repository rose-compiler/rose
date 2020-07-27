// using namespace std; 
  
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

 // friend constexpr bool operator==(partial_ordering, partial_ordering) noexcept = default;
 // friend constexpr bool operator==(A,A) noexcept = default;

 // friend constexpr bool operator==(const A & x, const A & y) noexcept = default; // works
    friend constexpr bool operator==(const A & x, const A & y) noexcept = default; // works
}; 

int main() 
{ 
    // executes using defaulted constructor 
    A a;  
      
    // uses parametrized constructor 
    A x(1);  
    return 0; 
} 


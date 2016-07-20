
// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <iostream>
#include <string>
#include <map>

// Parent class 
class S{
   int* a;
   double v;
  public:
   // function that build the suitable element for S
   S* Constructor() {
    std::cout << "S" <<std::endl;
    return new S();
   }
   // virtual function to be called for testing the element
   virtual void Me () {
    std::cout << ".S." <<std::endl;
  }
};


//Derived class 1 
class A : public S {
   bool c; 
  public: 
   // function that build the suitable element for A
   A* Constructor() { 
    std::cout << "A" <<std::endl;
    return new A();
   }
   // virtual function to be called for testing the element
   virtual void Me () {
    std::cout << ".A." <<std::endl;
  }
   
};


//Derived class 2 
class B : public S{
   double f;
  public: 
   // function that build the suitable element for B
   B* Constructor() {
    std::cout << "B" <<std::endl;
    return new B();
   }
   // virtual function to be called for testing the element
   virtual void Me () {
    std::cout << ".B." <<std::endl;
  }
};

////////////////////////////////////////////////
typedef S* (S::*func) ( void ) ;
typedef std::map<char*, func > Register;

// the global mapping of names to function pointers
Register myMap;

// function for setting an entry
void reg ( char* name , func constr){
   // check if the entry is already there
   if ( myMap.find(name) == myMap.end() ){
     myMap[name] = constr;
   }
}
////////////////////////////////////////////////
int main(){
  
   // register the function
   reg ( "S", (func)(&S::Constructor) );
   reg ( "A", (func)(&A::Constructor) );
   reg ( "B", (func)(&B::Constructor) );
   
   std::cout << myMap.size() << std::endl;


// testing ... 
   // search name 
   char* tester =  "B" ;  
  
   S* a = NULL;
   if ( myMap.find(tester) != myMap.end() ){
       a = (a->*myMap[tester]) () ;
   }
   else {
       a = (a->*myMap["S"]) () ;
   }
// testing the result ...
   a->Me();

   return 0;
}

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif


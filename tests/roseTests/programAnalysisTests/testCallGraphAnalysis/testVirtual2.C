
// Example code from Matt.
class A
 {
public:
   virtual void foo();
 };


class B : public A
 {
public:
   virtual void foo(){};
 };

int foo2(){
   A* a_ptr;

 // The call graph will only point to A::foo() (and not also B::foo())
   a_ptr->foo();

};

int main()
 {
   A* a_ptr;

 // The call graph will only point to A::foo() (and not also B::foo())
   a_ptr->foo();

   return 0;
 }

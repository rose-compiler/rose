// version without typedef

class A
   {
     public:
       A(){}
          friend class B;

     private:
          A(unsigned int var);
   };

class B
   {
     public:
          static A foobar();
   };

extern "C" unsigned int ab_var;

A B::foobar()
   {
     class A a; 
#pragma rose_outline     
     a=A(ab_var);
//     return A(ab_var);
     return a; 
   }

class A
   {
     public:
          virtual void foo_1() = 0;
          void foo_2();
   };

class B: public A
   {
     public:
          void foo_1();
   };

class C
   {
     public:
          void foo_2();

     private:
          B ab_data[8];
   };

void C::foo_2()
   {
#pragma rose_outline     
     ab_data[42].foo_2();
   }

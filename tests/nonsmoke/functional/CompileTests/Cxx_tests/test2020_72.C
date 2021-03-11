
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
     ab_data[42].foo_2();
   }

#if 0
// BUG: in generated rose_test_98_lib.cpp
extern "C" void OUT_1_test_98_0(void **__out_argv)
{
  class C *this__ptr__ =  *((class ::C **)__out_argv[0]);

  // The cast should be to B and can't be to the abstract base class A.
  // ((class ::A )this__ptr__ -> ab_data[42]) .  foo_2 ();
     ((class ::B )this__ptr__ -> ab_data[42]) .  foo_2 ();

   *((class ::C **)__out_argv[0]) = this__ptr__;
}
#endif

extern "C" void OUT_1_test_175_0(void **__out_argv);
 
// This is paper version test_100.cpp
// error:  'AB_xxx' was not declared in this scope
template < typename T1, typename T2>
class Map
   {
     public: 
          void insert(int,int);
   }; 

class A
   {
     public: 
          enum B
             {
               AB_B1,
               AB_B2
             };
};

class C
   { 
 
friend void OUT_1_test_175_0(void **__out_argv);
     public:
          C();

     private:
          enum BAD
             {
               AB_B1,
               AB_B2
             };
     static Map<A::B, unsigned int> abMap;
   };

C::C()
   {
  // abMap.insert(A::AB_B1,AB_B1);
     abMap.insert(A::AB_B2,AB_B2);
     abMap.insert(A::AB_B2,AB_B2);
   } 
 

extern "C" void OUT_1_test_175_0(void **__out_argv)
 
{
 
 
  class ::Map< enum ::A::B  , unsigned int  > *abMap = (class ::Map< enum ::A::B  , unsigned int  > *)__out_argv[0];
 
// abMap.insert(A::AB_B1,AB_B1);
  ( *abMap) . insert(((int )::A::AB_B2),((int )::C::AB_B2));
 
  ( *abMap) . insert(((int )A::AB_B2),((int )C::AB_B2));
}
 

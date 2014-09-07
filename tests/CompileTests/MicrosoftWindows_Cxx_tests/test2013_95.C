// template <enum ENUM_TYPE>
template <typename ENUM_TYPE>
class A
   {
     public:
         void foo (ENUM_TYPE x);
   };

class B
   {
     public:
         typedef int x_enum;
      // void x_enum();
   };

class X
   {
     public:
#if 1
          struct x_enum 
             {
               int x;
             };
#endif
          struct Y
             {
            // This is output as: "class A<B::x_enum> a;" and should be "class A<enum B::x_enum> a;".
               A<B::x_enum> a;
             };
   };



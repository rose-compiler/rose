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
          struct x_enum 
             {
               int x;
             };
#if 1
         void x_enum();
#endif
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
               A<class B::x_enum> a;
             };
   };




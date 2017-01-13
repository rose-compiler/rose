template <int VALUE>
class A
   {
     public:
         void foo (int x = VALUE);
   };

class X
   {
     public:
          enum x_enum 
             {
               START, 
               END 
             };

          struct Y
             {
               A<START> a;
             };
            
   };




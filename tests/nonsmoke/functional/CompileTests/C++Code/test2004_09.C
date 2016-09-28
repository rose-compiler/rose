template<class T>
class ABC
   {
     public:
          T xyz;
   };

template<class T>
class DEF
   {
     public:
          T xyz;
   };

int foo()
   {
     ABC<int> object1;
     DEF<int> object2;
     object1.xyz = 7;
     object2.xyz = 8;

     return 0;
   }


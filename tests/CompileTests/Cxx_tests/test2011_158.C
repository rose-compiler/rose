template<class T >
class ABC
   {
     public:
          T xyz;
   };

int foo()
   {
  // unparsed as: "class ::ABC object1;"
     ABC<int> object1;

     object1.xyz = 7;

     return 0;
   }

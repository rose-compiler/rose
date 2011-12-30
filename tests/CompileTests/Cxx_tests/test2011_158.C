template<class T >
class ABC
   {
     public:
          T xyz;
   };

int foo()
   {
#if 1
  // unparsed as: "class ::ABC object1;"
     ABC<int> object1;
#endif
#if 1
     object1.xyz = 7;
#endif
     return 0;
   }

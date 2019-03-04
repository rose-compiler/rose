namespace N
   {
     class ABC { public: int x; };
   }

class XYZ
   {
     public:
       // The class definition can be in the typedef (C++11 alias typedef).
          using DEF = class N::ABC;

          DEF abc;
       // abc.x = 42;

   };

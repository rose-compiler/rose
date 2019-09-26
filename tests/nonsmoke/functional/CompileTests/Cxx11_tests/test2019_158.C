namespace N
   {
     template <typename T> class ABC { public: T x; };
   }

class XYZ
   {
     public:
       // The class definition can be in the typedef (C++11 alias typedef).
          using DEF = class N::ABC<int>;

          DEF abc;
       // abc.x = 42;

   };

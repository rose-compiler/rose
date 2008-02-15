template <typename T>
class X
   {
  // Class defined with no constructor (default constructor is compiler generated but not built in ROSE)
     public:
          T t;
   };

template <typename T>
class Y
   {
  // Class defined with constructors (two of them)
     public:
          T t;

          Y() {};
          Y(int i) {};
   };

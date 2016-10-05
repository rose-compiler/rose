template <typename T>
class A 
   {
     public:
          A() {};
          A(int x) {};
   };

class B : public A<int>
   {
  // Data member to use in preinitialization list (without a parameter)
     int integer;
     A<int> object;

     public:
            B() : A<int>(7), integer(42), object() {};
   };


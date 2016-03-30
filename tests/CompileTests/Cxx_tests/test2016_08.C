class A
   {
     public:
          A() {}
          A(int x):_x(x),_y(x) {}
          int _x;
          int _y;
   };

template<typename T>
class C 
   {
     public:
          C() {}
          C(int x):_x(x),_y(x) {}
          int _x;
          int _y;
   };


int main() 
   {
     A a;
     C<int> c;
   } 

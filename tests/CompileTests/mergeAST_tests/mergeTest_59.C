// Different types of SgVariableSymbol generating constructs
const int size = 2;
int abc;
class 
   {
     public:
          int a,b;
          int z;
   };

void foo()
   {
     for (int x; x < 10; x++);
   }

template<typename T>
struct X
   {
     int x,y;
     void foobar (int x);
     void foo (int x) { int y; }
     static const int z;
     int array[size];
   };

const int X<int>::z   = 42;
const int X<float>::z = 7;

X<int> a;
X<float> b;

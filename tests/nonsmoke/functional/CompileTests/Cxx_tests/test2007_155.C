// This is from test2004_76.C (which I broke while working on the AST copy mechanism)

template<class U>
class A
   {
     public:
         void f() {}   
   };

void foo()
   {
     A<int> a;

     a.f();
   }


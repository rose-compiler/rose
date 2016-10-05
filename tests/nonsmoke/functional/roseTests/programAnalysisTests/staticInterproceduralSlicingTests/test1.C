class A { public: int b; };

A a;
int x = 0, y = 0;

void foo(int a)
   {
     x = 0;
   }

int main()
   {
     x = 42;
     y = 7;
     foo(y);
     x = 5;

     a.b = 12;
     x = a.b;

#pragma SliceTarget
     return x;
   }


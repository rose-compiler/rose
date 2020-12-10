
namespace X
   {
     int i = 43;
     namespace Y
        {
          int i = 42;
          struct X
             {
               static void f();
             };
          struct Y
             {
               static void f();
             };
          void X::f()
             {
               namespace A = X;
               A::i;
             }
          void Y::f()
             {
               namespace A = ::X::Y;
               A::i;
             }
          void f()
             {
               namespace A = X;
               A::i;
             }
        }
   }

int main(int argc, char *argv[])
   {
     X::Y::X::f();
     X::Y::Y::f();
     X::Y::f();

     return 0;
   }


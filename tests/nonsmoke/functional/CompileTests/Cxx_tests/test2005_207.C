
class X {};

int x = sizeof(float);

namespace Y
   {
  // hide global class X
     class X {};

     X local_x;
     ::X global_x;

     void foo ( X x ) {}
     void foo ( ::X x ) {}
   }

// This will be output as "::X x0;" where the "::" is not required
X x0;

void foo ()
   {
     class X {};
     X hidden_x;
     if (true)
      {
        class X {};
        X local_x;
      }
   }

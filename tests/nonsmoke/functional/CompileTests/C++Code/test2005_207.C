
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

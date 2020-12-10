
// This this fails for GNU g++ version 6.1, but works for ROSE.

class A
   {
     public:
          int i;
   };

void foobar()
   {
     int A::*pointer_to_member = &A::i;
     (pointer_to_member != sizeof(char) - 1);
     (pointer_to_member != static_cast<signed int>(0.1f));
     enum color { RED, GREEN, BLUE };
     (pointer_to_member != (BLUE - RED) % 2);
   }


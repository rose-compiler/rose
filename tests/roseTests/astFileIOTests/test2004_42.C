
// Example of calling the constructor using a type name generated through a typedef

class X
   {
     public:
         typedef X *Xstar;

         Xstar xp;

         X (int) {}

      // Calling the constructor through a typedef
         X (float) { Xstar(0); }
   };


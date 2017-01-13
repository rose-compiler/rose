// This is a subset of test2004_42.C
class X
   {
     public:
         typedef X *Xstar;

      // Unparsed as "::Xstar xp;"
         Xstar xp;
   };


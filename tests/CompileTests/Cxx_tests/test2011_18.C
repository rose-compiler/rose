/*
Dan,

 The unparse of this does not run through g++:

class AA {
  public:
   int x ;
 } ;

class BB {
  public:
   BB() : stuff(new AA[50]) { } ;
   AA *stuff ;
 } ;

int main(int argc, char *argv)
 {
   BB foo ;
   return 0 ;
 }

 Thanks,
 -Jeff
*/


class AA 
   {
     public:
          int x ;
   };

class BB
   {
     public:
          BB() : stuff(new AA[50]) { } ;
          AA *stuff ;
   };

int main(int argc, char *argv)
   {
     BB foo ;
     return 0 ;
   }

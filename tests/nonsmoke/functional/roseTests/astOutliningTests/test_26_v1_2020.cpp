// test outlining a code block with zero variables referenced.
//
class A {

  public:
   A ()
   { 
#pragma rose_outline
     { 
       noop();
     }
   }
  
   void noop()
    {}

};

namespace N
{
  class A
  {
  public:
        int foo() { return 4; }
        
 // To avoid ORD violation we have to represent "biz()" the same in the 
 // separate file with the namespace "N" is generated as a dependent declaration.
    int biz () const
    {
#pragma rose_outline
      biz();

      return 0;
    }
  };
}

int
main ()
   {
     N::A x;
  // x.biz();
     return 0;
   }

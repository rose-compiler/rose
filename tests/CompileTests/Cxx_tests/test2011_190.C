template <typename T>
class X
   {
     public:
          T abc;

     int foo() const
        {
          return 0;
        }

     int foobar() const
        {
       // This returns a SgTemplateSymbol and we can't make a SgMemberFunctionRefExp out of that.
          return foo();
        }
   };






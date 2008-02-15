
// Test code to test use of "throw;"  (special case of use of throw without parameters)
class X { public: X(){}; };
class Y {};

void foobar() throw(X,Y);

void foo()
   {
     try
        {
       // int x = 42;
        }

     catch(X)
        {
       // int x = 43;
          throw;
        }

     catch(...)
        {
       // int x = 44;
          throw;
        }
   }


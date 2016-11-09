
bool something();
int foo();

struct BAR
   {
     int baz(int);
   };

BAR bar;

void foobar()
   {

  // C++14
     [=] 
        {                                     // ok, deduces "-> some_type"
          while( something() ) 
             {
               if( true ) 
                  {
                    return foo() * 42;            // with arbitrary control flow
                  }
             }

          return bar.baz(84);                   // & multiple returns
        };                                      //   (types must be the same)

   }


char b_();
auto c_( char c() ) -> decltype(c);

void foobar()
   {
     char c = c_(b_)();
   }



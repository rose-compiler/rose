
// auto a_( const char a[] ) -> decltype(a) { return a+1; }
char b_() { return 'b'; }
auto c_( char c() ) -> decltype(c) { return b_; }
// auto d_( const char d ) -> decltype(d)* { return "d"; }


void foobar()
   {
  // const char* a = a_("xabc");
  // steq(a, "abc");
     char c = c_(b_)();
  // ieq(c, 'b');
  // const char* d = d_('x');
  // steq(d, "d");
   }



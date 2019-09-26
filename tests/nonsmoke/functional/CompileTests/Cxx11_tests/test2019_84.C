auto a_func( const char a[] ) -> decltype(a) { return a+1; }
char b_func() { return 'b'; }
auto c_func( char c() ) -> decltype(c) { return b_func; }
auto d_func( const char d ) -> decltype(d)* { return "d"; }

void foobar()
   {
     const char* a = a_func("xabc");
     char c = c_func(b_func)();
     const char* d = d_func('x');
   }

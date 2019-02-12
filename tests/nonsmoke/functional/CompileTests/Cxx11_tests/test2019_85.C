char b_func() { return 'b'; }
auto c_func( char c() ) -> decltype(c) { return b_func; }

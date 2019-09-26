int n_ = 0;
void f_() { ++n_; }
auto g_() -> decltype( f_() )
	{ return f_(); }

void foobar()
   {
     int a = 1;
     int b = 2;
     (void)(a = b);
     f_(), (void)(a = b+2);
     a==4 ? f_() : (void)(a = b+5);
     f_();
     g_();
     (const void)(volatile void)(const volatile void)f_();
   }

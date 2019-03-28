// This test code does NOT compile wth GNU 6.1
#if 0
template<class...Ty> // _VARIADIC_TEMPLATE
int f_(Ty...a) 
   {
     auto d = [&, a...]() mutable ATTR -> int 
        {  
       // empty attr no effect
          return sizeof...(a); 
        };
     return d();
   }
#endif

template<class...Ty> 
int f_(Ty...a) 
   {
     auto d = [&, a...]() mutable [[]] -> int 
        {
          return sizeof...(a); 
	};
     return d();
   }

void foobar()
   {
     int i;
     i = f_();
     i = f_(1);
     i = f_(2, 2.0);
   }


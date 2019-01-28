namespace A
   {
     inline namespace B 
        {
          template <class T> int c(T const& t) { d(t); return 0; }

          template <class T> int e(int = c( []{} )) { }
	}

     template<class T> int d(T const&) { return 1; }
   }

void foo() 
   { 
     A::B::e<long>();
   }


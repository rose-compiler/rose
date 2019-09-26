namespace A_ {
	inline namespace B_ {
		template <class T>
		int c(T const& t) { d_(t); return 0; }
		template <class T>
		int e(int = c( []{} )) { }
	}
	template<class T>
	int d_(T const&) { return 1; }
}

int f_() { return A_::B_::e<long>(); }

void foobar()
   {
     int a = f_();
   }	


struct st_;
template<class T>
int operator+(st_, T);
struct st_
	{
	int i;
	friend int operator+<>(st_, long);
	st_(int j) : i(j) { }
	};

template<class T>
int operator+(st_ s, T j){ return 42; }
typedef int (*PFL_)(st_, long);
typedef int (*PFD_)(st_, double);

void foobar()
   {
     st_ s (0);
     PFL_ pfl = operator+<long>;
     pfl(s, 5);
     PFD_ pfd = operator+<double>;
     pfd(s, 6);
   }



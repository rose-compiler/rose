
struct st_;

template<class T> int operator+(st_, T);

struct st_
	{
          friend int operator+<>(st_, long);
	};

template<class T> int operator+(st_ s, T j){ return 42; }

typedef int (*PFL_)(st_, long);

void foobar()
   {
     PFL_ pfl = operator+<long>;
   }

// The BUG is that the following is output in the generated code.
// If it were required, then it would have to use "template <>" syntax as well.
// int operator+< long  > (struct st_ s,long j) { return 42; }


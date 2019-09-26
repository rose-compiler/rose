
template <class T>
struct A_
	{
	int i;
	int f() { return i + 999; }
	};

template<> struct A_<int>
	{
	double i;
	int f();
	};

// Original code: int A_<int>::f()
// Unparse as:    template<> int A_< int > ::f()
int A_<int>::f()
	{
	return 154;
	}

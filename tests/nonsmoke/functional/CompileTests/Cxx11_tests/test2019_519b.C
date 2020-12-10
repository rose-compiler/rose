
namespace N_
	{
	const class X
		{
	public:
		int i;
		X(int ii) : i(ii) { }
		} x(10);
	const class Y
		{
	public:
		const X x;
		const int j;
		Y(int jj) : x(jj + 1), j(jj + 2) { }
		} y(110);
	void f_();
	}

int main(int argc, char *argv[])
   {
     N_::f_();

     return 42;
   }


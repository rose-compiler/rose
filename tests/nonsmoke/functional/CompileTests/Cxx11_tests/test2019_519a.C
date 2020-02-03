

namespace N_
	{
	const class X
		{
	public:
		int i;
		X(int ii) : i(ii) { }
		} x(51);
	const class Y
		{
	public:
		const X x;
		const int j;
		Y(int jj) : x(jj + 1), j(jj + 2) { }
		} y(11);
	void f_()
		{
		N_::x.i;
		N_::y.x.i;
		N_::y.j;
		}
	}




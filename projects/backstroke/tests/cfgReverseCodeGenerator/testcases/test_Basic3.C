struct Line
{
	Line()
	{
		t = 0;
	}
	
	int t;
};

struct State
{
	int x;
	int y;
	int count;
};

class WirelessLink {
public:
int Handle(int in, State* state)
{
	in = state->x;

	int a;
        a = 12 * in + in;
	in = state->y;
	int b;
        b= in / 4;

	int i;
	i = 3;
	i *= b;

	//The following test fails
	//Line l1;
	//Line l = l1;
	//l.t = 3;


	//Swap a and b
	int t = a;
	a = b;
	b = t;

	//Swap state->x and state->y
	t = state->x;
	state->x = state->y;
	state->y = t;

	return a;
}
};



int main()
{
	State s;
        WirelessLink w;
	w.Handle(10, &s);

	return 0;
}

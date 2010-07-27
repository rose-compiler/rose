/*struct Line
{
	Line()
	{
		t = 0;
	}
	
	int t;
};*/

struct State
{
	int x;
	int y;
	int count;
};


int reverseMe(int in, State* state)
{
	in = state->x;

	int a = 12 * in + in;
	in = 17;
	int b = in / 4;

	int i;
	i = 3;
	i = b;

	//Line l;
	//l.t = 3; //How is this undone?

	//Swap a and b
	int t = a;
	a = b;
	b = t;
	return a;
}




int main()
{
	State s;
	reverseMe(10, &s);

	return 0;
}

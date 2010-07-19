struct State
{
	int x;
	int y;
	int count;
};


int reverseMe(int in, State* state)
{
	int a = 12 * in;
	in++;
	int b = in / 4;

	int i;
	i = 3;
	i = b;


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

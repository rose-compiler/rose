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

class InterfaceReal {
public:
int Handle(int in, State* state)
{
  int a;
  a=1;
  a=2;
  state->x=a;
  state->x=in;
  return a;
}
};



int main()
{
	State s;
	InterfaceReal ir;
	ir.Handle(10, &s);

	return 0;
}

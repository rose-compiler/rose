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


int event(int in, State* state)
{
  int a;
  a=1;
  a=2;

  int b;
  b=1;
  b=2;

  while(b<10) {
	b=b+1;
  }
  if(a==in) {
	b=b+1;
  } else {
	b=b-1;
	a=a+1;
  }
	return a;
}





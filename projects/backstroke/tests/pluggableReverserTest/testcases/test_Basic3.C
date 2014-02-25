// temporary to allow unknown unparsed types
#define UNKNOWN int

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

  int b;
  b=2;

  while(b<10) {
    if(a==in) {
      b=b+1;
    } else {
      b=b-1;
      a=a+1;
    }
    b=b+1;
  }
  return a;
}





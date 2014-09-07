
class State{
public:
int x; int y;};

int event(int in, State* state)
{
  int a1=0;
  int t1=a1; // ==> WARNING: The reaching definition for "t1" : is empty
  int xx;
  xx=t1; // RD(t1) is OK.
  // -------
  int a2;
  int t2;
  t2=a2; // RD(a2) is OK.
  // -------
  int t3=t2; // ==> WARNING: The reaching definition for "t3" : is empty
  int t4=a1+1; // ==> WARNING: The reaching definition for "t3" : is empty
  return a1;
}

int main(){return 0;}





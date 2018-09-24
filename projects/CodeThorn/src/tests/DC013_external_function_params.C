void myfun1(int a,int b,int c, int* inout);
void myfun2(int a,int b,int c);

// a potentially modified value passed by pointer to an external function
int f() {
  int inout=0;
  int a=0,b=0,c=0;
  int y=0;
  myfun1(a,b,c, &inout);
  if(inout) {
    y+=1; 
  } else {
    y+=2;
  }
  inout=0;
  myfun2(a,b,c);
  if(inout==0) {
    y+=1; 
  } else {
    y+=2;
  }
  return y;
  y=-1; // dead code (just to have not an empty set of dead code locations)
}

int main() {
  f();
}

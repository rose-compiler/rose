int x;
int y;

int main() {

  int a[]={1,2,3};
  int b[3]={4,5,6};
  int c[5]={7,8,9};
  int* p;
  int y=0;
  p=a;
  x=p[3]; // out of bounds access
  p[3]=x; // out of bounds access
  return 0;
}

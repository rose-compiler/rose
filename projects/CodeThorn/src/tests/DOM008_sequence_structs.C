struct S {
  short x;
  int y;
  long z;
};
int main() {
  S a;
  a.x=5;
  a.y=6;
  a.z=a.x;
  S b;
  b.y=a.x;
  int* p;
  p=&a.y;
  *p=*p+1;
  S* sp;
  sp=&a;
  sp->y=sp->y+1;
  p=&(sp->y);
  *p=*p+1;
  *&(sp->y)=*p+1;
}


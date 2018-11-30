int main() {
  int a[2]={1,2};
  // arithmetic with array elements
  a[0]=a[0]+1;
  int* p;
  p=a;
  // dereference of pointers to arrays
  *p=*p+1;
  // pointer arithmetic (with pointers to named objects)
  p=p+1;
  *p=*p+1;
  int* q;
  // pointer artihmetic with pointers to arrays
  q=a+1;
  // 2-level pointers
  int** r;
  r=&p;
  // 2-level pointer dereferencing
  **r=**r+1;
  // compution on array elements referred by array index expressions
  int i=0;
  a[i+1]=a[i+1]+1;
  int* s;
  s=&a[1];
  int* b[2];
  int e1;
  b[1]=&e1;
  *b[1]=1;
}


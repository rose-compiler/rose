struct S {
  int x;
  long a[3];
};

// variants of initializing the same data structure
int main() {
  S s1={1,{2,3,4}};
  s1.x=s1.x+1;
  S s2={1,2,3,4};
  s2.x=s2.a[1]+1;
  
  return 0;
}

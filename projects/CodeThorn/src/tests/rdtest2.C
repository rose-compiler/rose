int main() {
  int i;
  int j;
  int n=10;
  int z;
  int s=0;
  for(i=1;i<n;i++) {
    s=s+1;
    for(j=1;j<n;j++) {
      s=s+j;
    }
  }
  z=s;
  return z>42;
}

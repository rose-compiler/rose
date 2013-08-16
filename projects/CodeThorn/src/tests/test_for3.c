// we intentionally avoid using ++,-- in this test
int main() {
  int sum=0;
  for(int i=1;i<6;i++) {
    for(int j=1,k=j;j<3;j++) {
      int s; // int s=j*k computes to top
      s=j*k;
      sum=sum+s;
    }
    i++;
  }
  return 0;
}

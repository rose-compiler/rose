#ifdef PRINT_RESULT
#include <cstdio>
#endif

class UserType {
public:
  UserType() {}
  ~UserType() {}
  int x;
};

int main() {
  UserType* a=new UserType[10];

  const int n=10;
  for(int i=0;i<n;i++) {
    a[i].x=i+1;
  }

  int checksum=0;
  for(int i=0;i<n;i++) {
    checksum+=a[i].x;
  }
  delete[] a;
  const int expectedchecksum=n*(n+1)/2;
#ifdef PRINT_RESULT
printf("n=%d check-sum=%d expected-check-sum=%d\n",n,checksum,expectedchecksum);
#endif
 return (checksum==expectedchecksum);
}


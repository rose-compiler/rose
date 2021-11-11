// variants of array initialization with string literals
#include <cstdio>
int main() {
  const char a1[]="abc";
  //printf("sizeof(a1):%d\n",sizeof(a1));
  printf("a1[0]:%d\n",a1[0]);
  printf("a1[1]:%d\n",a1[1]);
  printf("a1[2]:%d\n",a1[2]);
  printf("a1[3]:%d\n",a1[3]);
  char a2[3]="";
  printf("a2[0]:%d\n",a2[0]);
  printf("a2[1]:%d\n",a2[1]);
  printf("a2[2]:%d\n",a2[2]);
  char a3[3]="a";
  printf("a3[0]:%d\n",a3[0]);
  printf("a3[1]:%d\n",a3[1]);
  printf("a3[2]:%d\n",a3[2]);
  char a4[3]="ab";
  printf("a4[0]:%d\n",a4[0]);
  printf("a4[1]:%d\n",a4[1]);
  printf("a4[2]:%d\n",a4[2]);
  char a5[3]={'a','b','c'};
  printf("a5[0]:%d\n",a5[0]);
  printf("a5[1]:%d\n",a5[1]);
  printf("a5[2]:%d\n",a5[2]);
}

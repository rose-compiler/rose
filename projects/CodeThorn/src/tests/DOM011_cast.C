#include "DOM.h"
int main() {
  int res=1;
  int* dataArray[3];
  int* data=0;
  dataArray[2]=data;
  {
    int* data=dataArray[2];
    if(data!=((void*)0))
      res=2;
    else
      res=3;
  }
  printf("res:%d\n",res);
  return 0;
}

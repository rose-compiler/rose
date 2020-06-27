#include "DOM.h"
#include <string.h>

int main() {
  char* data;
  char dataBuffer[10+1];
  //char dataBufferExp[10+1];
  //printf("%d\n",dataBuffer);
  data=dataBuffer;
  //printf("%d\n",data);
  data[0]= '\0';
  const char* stringPtr = "0123456789";
  char source[11] = "0123456789";
  //char source[11] = {'0','1',0};
  int i;
  int sourceLen=10;
  for (i = 0; i < sourceLen + 1; i++)
    {
      data[i] = source[i];
    }
  int len1,len2;
  len1=strlen(source);
  len2=strlen(stringPtr);
  printf("len1:%d\n",len1);
  printf("len2:%d\n",len2);
  return 0;
}

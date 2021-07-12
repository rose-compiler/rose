#include "DOM.h"
//#include <string.h>

size_t strlen1(const char *str) {
  const char* s;
  for (s = str; *s; ++s);
  return (s - str);
}

size_t strlen2(const char *str) {
  const char* s;
  size_t len=0;
  for (s = str; *s!=0; ++s)
    len++;
  return len;
}

size_t strlen3(const char *str) {
  const char* s;
  size_t len=0;
  s = str;
  while(*s!=0) {
    ++s;
    len++;
  }
  return len;
}

int main() {
  char* data;
  char dataBuffer[10+1];
  //char dataBufferExp[10+1];
  //printf("%d\n",dataBuffer);
  data=dataBuffer;
  //printf("%d\n",data);
  data[0]= '\0';
  const char* stringPtr = "0123456789";
  //stringPtr[10]=0;
  char source[11] = "0123456789";
  //char source[11] = {'0','1',0};
  int i;
  int sourceLen=10;
  for (i = 0; i < sourceLen + 1; i++)
    {
      data[i] = source[i];
    }
  int len1,len2,len3,len4,len5,len6;
  len1=strlen1(source);
  len2=strlen2(source);
  len3=strlen3(source);
  //len4=strlen3(stringPtr);
  //len5=strlen1(stringPtr);
  //len6=strlen3(stringPtr);
  printf("len1:%d\n",len1);
  printf("len2:%d\n",len2);
  printf("len3:%d\n",len3);
  //printf("len4:%d\n",len4);
  //printf("len5:%d\n",len5);
  //printf("len6:%d\n",len6);
  return 0;
}

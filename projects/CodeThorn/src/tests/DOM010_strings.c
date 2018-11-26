#include "string.h"

int main() {
  char* data;
  char dataBuffer[10+1];
  //char dataBufferExp[10+1];
  data=dataBuffer;
  data[0]= '\0';
  char* stringPtr = "0123456789";
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
  return 0;
}

#include <cstdio>

int main() {
  int buffer[10] = { 0 }; // bo: s01 1113, s04 972, 
  printf("buffer[0]:%d\n",buffer[0]); // provided 0
  printf("buffer[1]:%d\n",buffer[1]); // default init
  int data;
  char inputBuffer[3 * sizeof(data) + 2] = ""; // bo: s01 92
  printf("inputBuffer[0]:%d\n",inputBuffer[0]); // terminating 0
  printf("inputBuffer[1]:%d\n",inputBuffer[1]);
  char dest[50] = ""; // bo: s06 1001, s07 91, s08 273, s09 91
  printf("dest[0]:%d\n",dest[0]); // terminating 0
  printf("dest[1]:%d\n",dest[1]); // default init

  char* cdata;
  cdata="Good"; // np: 51, uninitvar: 70 (not allowed in ISO C++)
  printf("cdata[0]:%d\n",cdata[0]);
  printf("cdata[1]:%d\n",cdata[1]);
  printf("cdata[2]:%d\n",cdata[2]);
  printf("cdata[3]:%d\n",cdata[3]);
  printf("cdata[4]:%d\n",cdata[4]); // terminating 0
  
} 

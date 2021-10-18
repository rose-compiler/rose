#include "DOM.h"

char* strcpy1(char dest[], const char source[])
{
  char *temp = dest;
  int i = 0;
  //printf("strcpy1: dest:%s source:%s\n",dest,source);
  while (1) {
    //printf("strcpy2: &dest[%d]:%s &source[%d]:%s\n",i,&dest[i],i,&source[i]);
    dest[i] = source[i];
    //printf("strcpy3: dest[%d]:%s source[%d]:%d\n",i,dest[i],i,source[i]);
    if (dest[i] == '\0') {
      break;
    }
    i++;
  }
  return temp;
}

void print(const char *str) {
  const char* s;
  int i=0;
  for (s = str; *s; ++s) {
    //printf("i: %d s: %s %d\n",i,s,*s);
    printf("i: %d %d\n",i,*s);
    i++;
  }
  
}
void print2(const char *str, int maxlen) {
  const char* s;
  for (int i=0;i<maxlen;i++) {
    //printf("i: %d s: %s %d\n",i,s,*s);
    printf("i: %d %d\n",i,*s);
  }
  
}

size_t strlen1(const char *str) {
  const char* s;
  int i=0;
  for (s = str; *s; ++s) {
    i++;
  }
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
  data=dataBuffer;
  data[0]= '\0';
  const char* stringPtr = "0123456789";
  char source[11] = "0123456789";
  printf("source[0]:%d\n",source[0]);
  printf("source[1]:%d\n",source[1]);
  printf("source[2]:%d\n",source[2]);
  printf("source[3]:%d\n",source[3]);
  printf("source[4]:%d\n",source[4]);
  printf("source[5]:%d\n",source[5]);
  printf("source[6]:%d\n",source[6]);
  printf("source[7]:%d\n",source[7]);
  printf("source[8]:%d\n",source[8]);
  printf("source[9]:%d\n",source[9]);
  printf("source[10]:%d\n",source[10]);
  int i;
  int sourceLen=10;
  for (i = 0; i < sourceLen + 1; i++)
    {
      //printf("i:%d source:%s source[i]:%d\n",i,source,source[i]);
      data[i] = source[i];
    }
  print(source);
  print(data);
  return 0;
  int len1,len2,len3,len4,len5,len6;
  len1=strlen1(source);
  len2=strlen2(source);
  len3=strlen3(source);
  //printf("stringPtr:%s\n",stringPtr);
  len4=strlen3(stringPtr);
  len5=strlen1(stringPtr);
  len6=strlen3(stringPtr);
  printf("len1:%d\n",len1);
  printf("len2:%d\n",len2);
  printf("len3:%d\n",len3);
  printf("len4:%d\n",len4);
  printf("len5:%d\n",len5);
  printf("len6:%d\n",len6);

  // string copy test
  char target1[10+1];
  char* targetptr=target1;
  //printf("%s\n",target1);
  //printf("%s\n",targetptr);
  //strcpy1(targetptr,data);
  print2(target1,11);
  return 0;
}

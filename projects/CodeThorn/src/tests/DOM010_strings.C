#include "DOM.h"


char* strcpy1(char dest[], const char source[])
{
  char *temp = dest;
  int i = 0;
  //printf("strcpy1: dest:%s source:%s\n",dest,source);
  while (1) {
    //printf("strcpy2: &dest[%d]:%p &source[%d]:%p\n",i,&dest[i],i,&source[i]);
    dest[i] = source[i];
    //printf("strcpy3: dest[%d]:%d source[%d]:%d\n",i,dest[i],i,source[i]);
    if (dest[i] == '\0') {
      break;
    }
    i++;
    if(i>12)
      break;
  }
  return temp;
}

void print(const char *str) {
  const char* s;
  int i=0;
  for (s = str; *s; ++s) {
    //printf("print(1): i: %d s: %s %d\n",i,s,*s);
    printf("print(2): i: %d %d\n",i,*s);
    i++;
    if(i>12)
      break;
  }
  
}
void print2(const char *str, int maxlen) {
  for (int i=0;i<maxlen;i++) {
    //printf("i: %d s: %p %d\n",i,str,*str);
    //s++;
    printf("print2 i: %d %d\n",i,*str++); // all in one expr
  }
  
}

size_t strlen1(const char *str) {
  const char* s;
  int i=0;
  for (s = str; *s; ++s) {
    //printf("strlen1: i: %d s: %p *s:%d",i,s,*s);
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
  //printf("strlen3: str:%p, s:%p\n",str,s);
  while(*s!=0) {
    //printf("strlen3: at len: %d, val: %d\n",len,*s);
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
  source[10]=0; // terminate string
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
  for (i = 0; i < sourceLen + 1; i++) {
    data[i] = source[i];
    //printf("code loop i:%d source:%p source[i]:%d data[i]:%d\n",i,source,source[i],data[i]);
    printf("code loop i:%d source[i]:%d data[i]:%d\n",i,source[i],data[i]);
  }
  print(source);
  print(data);
  //printf("data:%p\n",data);

  int len1,len2,len3,len4,len5,len6;
  len1=strlen1(source);
  printf("len1:%d\n",len1);
  len2=strlen2(source);
  printf("len2:%d\n",len2);
  len3=strlen3(source);
  printf("len3:%d\n",len3);

  //printf("stringPtr:%s\n",stringPtr);
  len4=strlen3(stringPtr);
  printf("len4:%d\n",len4);
  len5=strlen1(stringPtr);
  printf("len5:%d\n",len5);
  len6=strlen3(stringPtr);
  printf("len6:%d\n",len6);

  // string copy test
  char array2[10+1];
  char* array2ptr=array2;
  //printf("%p\n",array2);
  //printf("%p\n",array2ptr);
  strcpy1(array2ptr,data);
  print(array2ptr); // ensure interpreter mode cannot read past end without terminating
  print2(array2ptr,11); // ensure interpreter mode cannot read past end without terminating

  return 0;
}

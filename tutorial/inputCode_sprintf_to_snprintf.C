
#include<stdio.h>
#include<string>

// For simple tests we can just define the functions I need
// typedef unsigned long size_t;
// int snprintf(char *restrict s, size_t n, const char *restrict format, ...);
// int sprintf(char *restrict s, const char *restrict format, ...);
// size_t strlen(const char *s);
// void *malloc(size_t size);

const int n = 100;
char global_buffer[500];

void foo()
   {
  // Trivial use of "sprintf" with locally defined buffer.
     char buffer[42];
     sprintf(buffer,"ASCI \n");

  // Use of "sprintf" with a buffer where the length is an expression (evaluatable at compile-time).
     char another_buffer[n/2];
     sprintf(another_buffer,"ASCI \n");

  // Use of "sprintf" with a buffer defined elsewhere.
     sprintf(global_buffer,"ASCI \n");

  // Notice, that the explicit use of "snprintf" could be incorrect, we could check this!.
     snprintf(global_buffer,n,"ASCI \n");

  // More interesting case where buffer is dynamically allocated.
  // Should generate: "snprintf(sNewName,sizeof(char)*(strlen(sName)+10),"%s = %d",sName,7);"
     char bufferName[10];
     char* newBufferName = (char*)malloc(sizeof(char)*(strlen(bufferName)+10));
     sprintf(newBufferName,"%s = %d",bufferName,7);
   }

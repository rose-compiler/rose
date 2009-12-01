/******** 'coverage_func.c' ********/
/*The functions in this file are:

void coverageTraceFunc1(int FileMethod);

Important Note :
To use the functions you must include 'coverage_funcsC.h' in your sources !
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/time.h>


#define TRUE 1
#define FALSE 0

void coveragePrintFunc();


FILE *trace_file_out;

int nextSnapShot = 1;

char FileMethodStr[200];

int traceStarted = 0;


/* Here is the exact call statement for you to use : 
       coverageTraceFunc1(cov)
   -Don't forget to include 'coverage_funcsC.h' in your source file !  */

void coverageTraceFunc1(const char* FileMethod) {

  struct timeval val;
  struct timezone zone;

  char filename[200];

  char tempStr[200];

  char attrValueStr[200];

  int occurCount = 0 , i=0;

  int caseError=0 , found=0;
  if ( traceStarted == 0 ) {
    gettimeofday(&val,&zone);
    sprintf(filename,"%s_trace_%d","C:\\focus\\New Folder (3)\\coverage",val.tv_sec);
    trace_file_out = fopen(filename,"w");
    strcpy(FileMethodStr,"ERROR ");
    nextSnapShot = 1;
  }
  if ( trace_file_out == NULL ) {
    fprintf(stderr,"\nERROR : UNABLE TO OPEN FILE\n");
    exit(1);
  }


  traceStarted = 1;


  strcpy(attrValueStr,"");

  strcpy(tempStr,"");

  if ( nextSnapShot != 1 ) {
    coveragePrintFunc();
    return;
  }

  nextSnapShot++;

/* Attribute : FileMethod */
  if ( 1 ) {
    sprintf(tempStr,"%d",FileMethod);
    if ( found == 0 ) {
      caseError = 1;
    }
    found = 0;
    strcat(tempStr," ");
    strcat(attrValueStr,tempStr);
    occurCount++;
  }

  if ( occurCount > 1 || caseError == 1 ) {
    strcpy(tempStr,"ERROR_");
    strcat(tempStr,attrValueStr);
  } else {
    if ( occurCount == 1 ) {
      strcpy(tempStr,attrValueStr);
    }
  }
  if ( occurCount == 0 ) {
    sprintf(tempStr,"ERROR_%d ",FileMethod);
  }
  sprintf(FileMethodStr,"%s",tempStr);

  strcpy(attrValueStr,"");

  strcpy(tempStr,"");
  occurCount = 0;


  coveragePrintFunc();


  return;
}


void coveragePrintFunc() {
  fprintf(trace_file_out,"%s",FileMethodStr);
  fprintf(trace_file_out,"\n");
  fflush(trace_file_out);
  strcpy(FileMethodStr,"ERROR ");
  nextSnapShot = 1;
}



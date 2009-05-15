#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H
#include <stdio.h>
//#include <cstdio>


/* -----------------------------------------------------------
 * tps : 6th April 2009: RTED
 * Contains variable names for variables that are passed via functions
 * -----------------------------------------------------------*/
struct RuntimeVariablesType {
  char* name;
  char* mangled_name;
};


/* -----------------------------------------------------------
 * tps : 6th April 2009: RTED
 * Store information about arrays and their sizes
 * -----------------------------------------------------------*/
#if 0
struct arrays1DType {
  char* name;
  int size1;
} ;

struct arrays2DType {
  char* name;
  int size1;
  int size2;
} ;
#endif

struct arraysType {
  char* name;
  int dim;
  int size1;
  int size2;  
};

/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * RuntimeSystem called by each transformed source file
 * -----------------------------------------------------------*/
struct RuntimeSystem  {
  int arrayDebug;
  int funccallDebug;

  int maxRuntimeVariablesEndIndex;
  int runtimeVariablesEndIndex;
  struct RuntimeVariablesType* runtimeVariablesOnStack;

  // a map of all arrays that were created
#if 0
  struct arrays1DType* arrays1D;
  int arrays1DEndIndex;
  struct arrays2DType* arrays2D;
  int arrays2DEndIndex;

  int maxArrays1DEndIndex;
  int maxArrays2DEndIndex;
#endif

  struct arraysType* arrays;
  int arraysEndIndex;
  int maxArraysEndIndex;

  int violation;
  FILE *myfile;
};

void RuntimeSystem_Const_RuntimeSystem();

//extern struct RuntimeSystem* rt_pi;
struct RuntimeSystem* rtsi();



void RuntimeSystem_handleSpecialFunctionCalls(char* funcname,char** args, int argsSize, char* filename, char* line, char* stmtStr);

void RuntimeSystem_increaseSizeRuntimeVariablesOnStack();                                               
char* RuntimeSystem_findVariablesOnStack(char* name);
//struct RuntimeVariablesType* RuntimeSystem_getVariablesOnStack(char* mangledname);
void RuntimeSystem_callExit(char* filename, char* line, char* reason, char* stmtStr);
char* RuntimeSystem_findLastUnderscore(char* s);
#if 0
void RuntimeSystem_increaseSizeArray1();                                               
int RuntimeSystem_findArrayName1D(char* n);
void RuntimeSystem_increaseSizeArray2();                                               
int RuntimeSystem_findArrayName2D(char* n);
#endif
int RuntimeSystem_findArrayName(char* n);
void RuntimeSystem_increaseSizeArray();                                               

char* RuntimeSystem_resBool(int val);
void RuntimeSystem_roseRtedClose();
// create array and store its size
void RuntimeSystem_roseCreateArray(char* name, int dimension, int stack, 
				   long int sizeA, long int sizeB, char* filename, char* line);
// check if array is out of bounds
void RuntimeSystem_roseArrayAccess(char* name, int posA, int posB, char* filename, char* line, char* stmtStr);
// this is a function call with all its parameters
void RuntimeSystem_roseFunctionCall(int count, ...);
// convert an integer to string
char* RuntimeSystem_roseConvertIntToString(int t);


//static struct RuntimeSystem* runtimeSystem = RuntimeSystem_Instance();
#endif


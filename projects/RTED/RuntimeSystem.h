#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H
#include <stdio.h>
//#include <cstdio>


/* -----------------------------------------------------------
 * tps : 6th April 2009: RTED
 * Contains variable names for variables that are passed via functions
 * -----------------------------------------------------------*/
struct RuntimeVariablesType {
  char* name; // stack variable name
  char* mangled_name; // mangled name
  char* type;
  char* initialized;
};


/* -----------------------------------------------------------
 * tps : 6th April 2009: RTED
 * Store information about arrays and their sizes
 * -----------------------------------------------------------*/
struct arraysType {
  char* name; // this represents the mangled name
  int dim; // the indicates the dimension
  int size1; // size of dimension 1
  int size2; // size of dimension 2
};

/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * RuntimeSystem called by each transformed source file
 * -----------------------------------------------------------*/
struct RuntimeSystem  {
  int arrayDebug; // show debug information for arrays ?
  int funccallDebug; // show debug information for function calls?

  // variables that are pushed and poped on/from stack
  // used to determine the real variable passed to a function
  int maxRuntimeVariablesOnStackEndIndex;
  int runtimeVariablesOnStackEndIndex;
  struct RuntimeVariablesType* runtimeVariablesOnStack; 

  // variables used
  int maxRuntimeVariablesEndIndex;
  int runtimeVariablesEndIndex;
  struct RuntimeVariablesType* runtimeVariables; 

  // a map of all arrays that were created
  int arraysEndIndex;
  int maxArraysEndIndex;
  struct arraysType* arrays;

  // did a violation occur?
  int violation;
  // output file for results
  FILE *myfile;
};


// Runtime System
struct RuntimeSystem* rtsi();
// Constructor - Destructor
void RuntimeSystem_Const_RuntimeSystem();
void RuntimeSystem_roseRtedClose();

// helper functions
char* RuntimeSystem_findLastUnderscore(char* s);
char* RuntimeSystem_resBool(int val);
char* RuntimeSystem_roseConvertIntToString(int t);
int RuntimeSystem_isInterestingFunctionCall(char* name);
int RuntimeSystem_getParamtersForFuncCall(char* name);

// array functions
int RuntimeSystem_findArrayName(char* mangled_name);
void RuntimeSystem_increaseSizeArray();                                               
void RuntimeSystem_roseCreateArray(char* name, int dimension, int stack, 
				   long int sizeA, long int sizeB, char* filename, char* line);

void RuntimeSystem_roseArrayAccess(char* name, int posA, int posB, char* filename, char* line, char* stmtStr);

// function calls 
char* RuntimeSystem_findVariablesOnStack(char* name);
void RuntimeSystem_increaseSizeRuntimeVariablesOnStack();                                               
void RuntimeSystem_roseCallStack(char* name, char* mangl_name, char* beforeStr,char* filename, char* line);

void RuntimeSystem_handleSpecialFunctionCalls(char* funcname,char** args, int argsSize, char* filename, char* line, char* stmtStr);
void RuntimeSystem_roseFunctionCall(int count, ...);
int  RuntimeSystem_isSizeOfVariableKnown(char* name);
int  RuntimeSystem_isModifyingOp(char* name);

// function used to indicate error
void RuntimeSystem_callExit(char* filename, char* line, char* reason, char* stmtStr);

// functions dealing with variables
void RuntimeSystem_roseCreateVariable(char* name, char* mangled_name, char* type, char* init);
void RuntimeSystem_increaseSizeRuntimeVariables();
char* RuntimeSystem_findVariables(char* name);
void RuntimeSystem_roseInitVariable(char* mangled_name);


#endif


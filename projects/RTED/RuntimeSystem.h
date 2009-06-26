#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H
#include <stdio.h>
//#include <cstdio>


/* -----------------------------------------------------------
 * tps : 6th April 2009: RTED
 * Contains variable names for variables that are passed via functions
 * -----------------------------------------------------------*/
struct RuntimeVariablesType {
  const char* name; // stack variable name
  const char* mangled_name; // mangled name
  const char* type;
  int initialized; // 0 = false
  const char* fileOpen; // r = read, w = write
  struct MemoryType* address;
  long int value;
  struct ArraysType* arrays; // exactly one array
};

/* -----------------------------------------------------------
 * tps : 10th June 2009: RTED
 * Store information about memory allocations
 * -----------------------------------------------------------*/
struct MemoryType {
  long int address; // address of memory
  int lastVariablePos;
  int maxNrOfVariables; // lets increase by the factor 2
  int size; // size of memory allocated in bytes
  struct MemoryVariableType* variables; // variables pointing to this location
};

/* -----------------------------------------------------------
 * tps : 10th June 2009: RTED
 * This is a container for all variables at one memory location
 * -----------------------------------------------------------*/
struct MemoryVariableType {
  struct RuntimeVariablesType* variable; // variables pointing to this location
};

/* -----------------------------------------------------------
 * tps : 6th April 2009: RTED
 * Store information about arrays and their sizes
 * -----------------------------------------------------------*/
struct ArraysType {
  const char* name; // this represents the mangled name
  int dim; // the indicates the dimension
  int size1; // size of dimension 1
  int size2; // size of dimension 2
  int ismalloc; // is it on the stack or heap?
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

  // memory used
  int maxMemoryEndIndex;
  int runtimeMemoryEndIndex;
  struct MemoryType* runtimeMemory; 

  // a map of all arrays that were created
  //int arraysEndIndex;
  //int maxArraysEndIndex;
  //struct arraysType* arrays;

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
const char* RuntimeSystem_resBool(int val);
const char* RuntimeSystem_roseConvertIntToString(int t);
int RuntimeSystem_isInterestingFunctionCall(const char* name);
int RuntimeSystem_getParamtersForFuncCall(const char* name);
int getSizeOfSgType(const char* type);

// memory handling
void RuntimeSystem_increaseSizeMemory();
struct MemoryVariableType* RuntimeSystem_findMemory(long int address);
struct MemoryType* RuntimeSystem_AllocateMemory(long int address, int sizeArray, struct RuntimeVariablesType* var);
void RuntimeSystem_increaseSizeMemoryVariables(  int pos);
void RuntimeSystem_RemoveVariableFromMemory(long int address, struct RuntimeVariablesType* runtimevar);
int checkMemoryLeakIssues(int pos, int address, const char* filename, const char* line, const char* stmtStr);


// array functions
int RuntimeSystem_findArrayName(const char* mangled_name);
//void RuntimeSystem_increaseSizeArray();                                               
void RuntimeSystem_roseCreateArray(const char* name, const char* mangl_name, int dimension,// int stack, 
				   long int sizeA, long int sizeB, int ismalloc, const char* filename, const char* line);

void RuntimeSystem_roseArrayAccess(const char* name, int posA, int posB, const char* filename, const char* line, const char* stmtStr);

// function calls 
const char* RuntimeSystem_findVariablesOnStack(const char* name);
void RuntimeSystem_increaseSizeRuntimeVariablesOnStack();                                               
void RuntimeSystem_roseCallStack(const char* name, const char* mangl_name, const char* beforeStr,const char* filename, const char* line);

void RuntimeSystem_handleSpecialFunctionCalls(const char* funcname,const char** args, int argsSize, const char* filename, const char* line, const char* stmtStr, const char* leftHandSideVar);
void RuntimeSystem_handleIOFunctionCall(const char* funcname,const char** args, int argsSize, const char* filename, const char* line, const char* stmtStr, const char* leftHandSideVar);
void RuntimeSystem_roseFunctionCall(int count, ...);
int  RuntimeSystem_isSizeOfVariableKnown(const char* name);
int  RuntimeSystem_isModifyingOp(const char* name);
int RuntimeSystem_isFileIOFunctionCall(const char* name);

// function used to indicate error
void RuntimeSystem_callExit(const char* filename, const char* line, const char* reason, const char* stmtStr);

// functions dealing with variables
void RuntimeSystem_roseCreateVariable(const char* name, const char* mangled_name, const char* type, int init,
				      const char* fOpen, const char* filename, const char* line, const char* lineTransformed);
void RuntimeSystem_increaseSizeRuntimeVariables();
struct RuntimeVariablesType* RuntimeSystem_findVariables(const char* name);
int RuntimeSystem_findVariablesPos(const char* mangled_name, int* isarray);
void RuntimeSystem_roseInitVariable(const char* name,
				    const char* mangled_name,
				    const char* typeOfVar2,
				    const char* baseType2,
				    unsigned long long address,
				    unsigned long long value,
				    int ismalloc,
				    const char* filename, const char* line, 
				    const char* stmtStr);
void RuntimeSystem_roseAccessVariable( const char* name,
				       const char* mangled_name,
				       const char* filename, const char* line, 
				       const char* lineTransformed,
				       const char* stmtStr);

// USE GUI for debugging
void Rted_debugDialog(const char* filename, int line);


#endif


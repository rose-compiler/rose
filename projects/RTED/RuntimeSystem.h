#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H
#include <stdio.h>
//#include <cstdio>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif


// helper functions
const char* RuntimeSystem_roseConvertIntToString(int t);


void RuntimeSystem_roseRtedClose(
  const char* filename,
  const char* line,
  const char* lineTransformed
);

void RuntimeSystem_roseCreateArray(const char* name, const char* mangl_name,
				   const char* type, const char* basetype, size_t indirection_level, unsigned long int
				   address, long int size, 
				   int ismalloc, long int mallocSize, const char* class_name, const char* filename,
				   const char* line, const char* lineTransformed, int dimensions, ...);

void RuntimeSystem_roseArrayAccess(const char* name, int posA, int posB, const char* filename, 
				   unsigned long int base_address, // &( array[ 0 ])
				   unsigned long int address, long int size, 
				   int read_write_mask,		// 1 = read, 2 = write
				   const char* line, const char* lineTransformed, const char* stmtStr);


// function calls 
void RuntimeSystem_roseCallStack(const char* name, const char* mangl_name, const char* beforeStr,const char* filename, const char* line);

void RuntimeSystem_handleSpecialFunctionCalls(const char* funcname,const char** args, int argsSize, const char* filename, const char* line, 
					      const char* lineTransformed, const char* stmtStr, const char* leftHandSideVar);
void RuntimeSystem_roseIOFunctionCall(const char* funcname,
					const char* filename, const char* line,
					const char* lineTransformed, const char* stmtStr, const char* leftHandSideVar, FILE* file,
					const char* arg1, const char* arg2);
void RuntimeSystem_roseFreeMemory(
  void* ptr,
  const char* filename,
  const char* line,
  const char* lineTransformed
);
void RuntimeSystem_roseReallocateMemory(
  void* ptr,
  unsigned long int size,
  const char* filename,
  const char* line,
  const char* lineTransformed
);


void RuntimeSystem_roseFunctionCall(int count, ...);
int RuntimeSystem_isFileIOFunctionCall(const char* name);

// handle scopes (so we can detect when locals go out of scope, free up the
// memory and possibly complain if the local was the last var pointing to some
// memory)
void RuntimeSystem_roseEnterScope( const char* scope_name);
void RuntimeSystem_roseExitScope( const char* filename, const char* line, const char* stmtStr);


// function used to indicate error
void RuntimeSystem_callExit(const char* filename, const char* line, const char* reason, const char* stmtStr);

// functions dealing with variables
void RuntimeSystem_roseCreateVariable(const char* name, const char*
				      mangled_name, const char* type, const char* basetype, size_t indirection_level,
				      unsigned long int address, unsigned int
				      size, int init, const char* fOpen,
				      const char* className, const char* filename, const char* line,
				      const char*
				      lineTransformed);
void RuntimeSystem_roseInitVariable(const char* name,
				    const char* mangled_name,
				    const char* typeOfVar2,
				    const char* baseType2,
					size_t indirection_level,
                    const char* class_name,
				    unsigned long long address,
				    unsigned int size,
				    int ismalloc,
                    int pointer_changed,
				    const char* filename, 
				    const char* line, const char* lineTransformed, 
				    const char* stmtStr);
void RuntimeSystem_roseAccessVariable( const char* name,
				       const char* mangled_name,
				       unsigned long long address,
				       unsigned int size,
				       const char* filename, const char* line, 
				       const char* lineTransformed,
				       const char* stmtStr);

void RuntimeSystem_checkMemoryAccess( unsigned long int address, long int size, int read_write_mask );

// handle structs and classes
void RuntimeSystem_roseRegisterTypeCall(int count, ...);


extern int RuntimeSystem_original_main(int argc, char**argv, char**envp);


// USE GUI for debugging
void Rted_debugDialog(const char* filename, int line, int lineTransformed);

#ifdef __cplusplus
} // extern "C"
#endif

#endif


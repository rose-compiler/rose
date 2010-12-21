#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H
#include <stdio.h>
//#include <cstdio>
#include <stddef.h>

#include "ptrops.h"

// typedef unsigned long addr_type;

#ifdef __cplusplus
extern "C" {
#endif

/***************************** HELPER FUNCTIONS *************************************/
const char* rted_ConvertIntToString(int t);

// USE GUI for debugging
void Rted_debugDialog(const char* filename, int line, int lineTransformed);

void rted_Checkpoint( const char* filename, const char* line, const char* lineTransformed );
/***************************** HELPER FUNCTIONS *************************************/



/***************************** ARRAY FUNCTIONS *************************************/

void
rted_CreateHeapArr( const char* name,
									  const char* mangl_name,
									  const char* type,
										const size_t* dimDescr,
										size_t totalsize,
									  const char* basetype,
									  MemoryAddress address,
									  const char* class_name,
									  const char* filename,
									  size_t line,
									  size_t lineTransformed
									);




#if OBSOLETE_CODE

void rted_CreateHeap(
        const char* name,
        const char* mangl_name,
        const char* type,
        const char* basetype,
        size_t indirection_level,       // how many dereferences to get to a non-pointer type
                                        // e.g. int*** has indirection level 3
        MemoryAddress address,
        long int size,
        long int mallocSize,
        int fromMalloc,                 // 1 if from call to malloc
                                        // 0 otherwise, if e.g. from new
        const char* class_name, const char* filename, const char* line,
        const char* lineTransformed, int dimensions, const size_t* dimDesc);

#endif /* OBSOLETE_CODE */


void rted_AccessHeap(const char* filename,
		MemoryAddress base_address, // &( array[ 0 ])
		MemoryAddress address, long int size, int read_write_mask, // 1 = read, 2 = write
		const char* line, const char* lineTransformed);
/***************************** ARRAY FUNCTIONS *************************************/



/***************************** FUNCTION CALLS *************************************/

void rted_AssertFunctionSignature(
		const char* filename, const char* line, const char* lineTransformed,
		const char* name, int type_count, ... );

void rted_ConfirmFunctionSignature(
		const char* name, int type_count, ... );

void RuntimeSystem_handleSpecialFunctionCalls(const char* funcname,
		const char** args, int argsSize, const char* filename,
		const char* line, const char* lineTransformed, const char* stmtStr,
		const char* leftHandSideVar);

void rted_IOFunctionCall(const char* funcname,
		const char* filename, const char* line, const char* lineTransformed,
		const char* stmtStr, const char* leftHandSideVar, void* file,
		const char* arg1, const char* arg2);

void rted_FunctionCall(int count, ...);
/***************************** FUNCTION CALLS *************************************/



/***************************** MEMORY FUNCTIONS *************************************/
void rted_FreeMemory(
        void* ptr,              // the address that is about to be freed
        int fromMalloc,         // whether the free expects to be paired with
                                // memory allocated via 'malloc'.  In short,
                                // whether this is a call to free (1) or delete
                                // (0)
        const char* filename,
        const char* line, const char* lineTransformed);

void rted_ReallocateMemory(void* ptr, unsigned long int size,
		const char* filename, const char* line, const char* lineTransformed);

void RuntimeSystem_checkMemoryAccess(MemoryAddress address, long int size,
		int read_write_mask);
/***************************** MEMORY FUNCTIONS *************************************/



/***************************** SCOPE *************************************/
// handle scopes (so we can detect when locals go out of scope, free up the
// memory and possibly complain if the local was the last var pointing to some
// memory)
void rted_EnterScope(const char* scope_name);
void rted_ExitScope(const char* filename, const char* line, const char* lineTransformed,
		const char* stmtStr);
/***************************** SCOPE *************************************/


  void rted_RtedClose(char* from);

// function used to indicate error
void RuntimeSystem_callExit(const char* filename, const char* line,
		const char* reason, const char* stmtStr);

extern int RuntimeSystem_original_main(int argc, char**argv, char**envp);
/***************************** INIT AND EXIT *************************************/



/***************************** VARIABLES *************************************/

int rted_CreateVariable(const char* name,
		const char* mangled_name, const char* type, const char* basetype,
		size_t indirection_level, MemoryAddress address, unsigned int size,
		int init, const char* className, const char* filename,
		const char* line, const char* lineTransformed);


#if NOT_YET_IMPLEMENTED
// for upc
int rted_CreateSharedVariable( const char* name,
																						const char* mangled_name,
																						const char* type,
																						const char* basetype,
																						size_t indirection_level,
																						shared void* address,
																						unsigned int size,
																						int init,
																						const char* className,
																						const char* filename,
																						const char* line,
																						const char* lineTransformed
																					);
#endif /* NOT_YET_IMPLEMENTED */

/**
 * Register the creation of a C++ object.  This function should only be called
 * at the start of an object's constructor.  It is safe to call this function
 * multiple times for the same address: e.g. if called in a base class
 * constructor and a sub class constructor.
 */
int rted_CreateObject(
        const char* type,
        const char* basetype,
        size_t indirection_level,
        MemoryAddress address,
        unsigned int size,
        const char* filename,
        const char* line,
        const char* lineTransformed );

int rted_InitVariable(const char* typeOfVar2,
		const char* baseType2, size_t indirection_level,
		const char* class_name, MemoryAddress address, unsigned int size,
		int ismalloc, int pointer_changed, const char* filename,
		const char* line, const char* lineTransformed);

/**
 * This function is called when pointers are incremented.  For example, it will
 * be called for the following:
 @code
       int* p;
       // ...
       ++p;
 @endcode
 * but not for simple assignment, as in the following:
 @code
       int* p;
       // ...
       p = ...
 @endcode
 * It verifies that the pointer stays within “memory bounds”.  In particular, if
 * the pointer points to an array, rted_MovePointer checks that it
 * isn't incremented beyond the bounds of the array, even if doing so results in
 * a pointer to allocated memory.
 */
void rted_MovePointer(
                MemoryAddress address,
                const char* type,
                const char* base_type,
                size_t indirection_level,
                const char* class_name,
                const char* filename,
                const char* line,
                const char* lineTransformed);

void rted_AccessVariable(
        MemoryAddress address,
        unsigned int size,
        MemoryAddress write_address,
        unsigned int write_size,
        int read_write_mask, //1 = read, 2 = write
        const char* filename, const char* line,
        const char* lineTransformed);

void rted_CheckIfThisNULL(
		void* thisExp,
		const char* filename, const char* line,
		const char* lineTransformed);
/***************************** VARIABLES *************************************/



/***************************** TYPES *************************************/
// handle structs and classes
void rted_RegisterTypeCall(int count, ...);
/***************************** TYPES *************************************/







#ifdef __cplusplus
} // extern "C"
#endif

#endif

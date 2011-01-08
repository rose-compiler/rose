#ifndef RTEDRUNTIME_H
#define RTEDRUNTIME_H

#include <stddef.h>

#include "CppRuntimeSystem/rted_iface_structs.h"
#include "CppRuntimeSystem/ptrops.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus

typedef struct TypeDesc TypeDesc;
typedef struct SourceInfo SourceInfo;

#endif

/***************************** HELPER FUNCTIONS *************************************/
const char* rted_ConvertIntToString(int t);

// USE GUI for debugging
void Rted_debugDialog(const char* filename, size_t line, size_t lineTransformed);

void rted_Checkpoint(const char* filename, size_t line, size_t lineTransformed);
/***************************** HELPER FUNCTIONS *************************************/



/***************************** ARRAY FUNCTIONS *************************************/


void rted_CreateHeapArr( TypeDesc      td,
												 Address       address,
												 size_t        elemsize,
												 size_t        totalsize,
												 const size_t* dimDescr,
												 const char*   name,
												 const char*   mangl_name,
												 const char*   class_name,
												 SourceInfo    si
											 );

void rted_CreateHeapPtr( TypeDesc    td,
												 Address     address,
												 size_t      size,
												 size_t      mallocSize,
												 int         fromMalloc,
												 const char* class_name,
												 SourceInfo  si
											 );


void rted_AccessHeap( Address base_address, // &( array[ 0 ])
										  Address address,
											size_t size,
											int read_write_mask,  // 1 = read, 2 = write
											SourceInfo si
										);

/***************************** ARRAY FUNCTIONS *************************************/



/***************************** FUNCTION CALLS *************************************/

void rted_AssertFunctionSignature( const char* name,
																	 size_t type_count,
																	 TypeDesc* typedescs,
                                   SourceInfo si
																 );

void rted_ConfirmFunctionSignature(const char* name, size_t type_count, TypeDesc* types);

void rted_IOFunctionCall( const char* fname,
													const char* /* stmtStr */,
													const char* /* leftVar */,
													void* file,
													const char* arg1,
													const char* arg2,
												  SourceInfo si
												);

void rted_FunctionCall( const char* name,
												const char* /*stmtStr*/,
												const char* /*leftVar*/,
												SourceInfo si,
												size_t argc,
												const char** args
											);

/***************************** FUNCTION CALLS *************************************/



/***************************** MEMORY FUNCTIONS *************************************/
void rted_FreeMemory( void* ptr,       ///< the address that is about to be freed
                      int fromMalloc,  ///< whether the free expects to be paired with
                                       ///  memory allocated via 'malloc'.  In short,
                                       ///  whether this is a call to free (1) or delete
                                       ///  (0)
                      SourceInfo si
							      );


void rted_ReallocateMemory( void* ptr, size_t size, SourceInfo si );
/***************************** MEMORY FUNCTIONS *************************************/



/***************************** SCOPE *************************************/
// handle scopes (so we can detect when locals go out of scope, free up the
// memory and possibly complain if the local was the last var pointing to some
// memory)
void rted_EnterScope(const char* scope_name);
void rted_ExitScope(const char*, SourceInfo si);

/***************************** SCOPE *************************************/


void rted_RtedClose(char* from);

// function used to indicate error
// \pp is this function used / defined?
// void RuntimeSystem_callExit(const char* filename, const char* line,
//		const char* reason, const char* stmtStr);

extern int RuntimeSystem_original_main(int argc, char**argv, char**envp);
/***************************** INIT AND EXIT *************************************/



/***************************** VARIABLES *************************************/

int rted_CreateVariable( TypeDesc td,
												 Address address,
												 size_t size,
												 const char* name,
												 const char* mangled_name,
												 int init,
												 const char* class_name,
												 SourceInfo si
						           );

/**
 * Register the creation of a C++ object.  This function should only be called
 * at the start of an object's constructor.  It is safe to call this function
 * multiple times for the same address: e.g. if called in a base class
 * constructor and a sub class constructor.
 */
void rted_CreateObject( TypeDesc td, Address address, SourceInfo si );

void rted_InitVariable( TypeDesc td,
			                  Address address,
			                  size_t size,
												const char* class_name,
			                  int ismalloc,
			                  int pointer_changed,
			                  SourceInfo si
											);

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
void rted_MovePointer( TypeDesc    td,
										   Address     address,
										   const char* class_name,
										   SourceInfo  si
										 );

void rted_AccessVariable( Address address,
													size_t size,
													Address write_address,
													size_t write_size,
													int read_write_mask,
													SourceInfo si
				                );

void rted_CheckIfThisNULL( void* thisExp, SourceInfo si );
/***************************** VARIABLES *************************************/



/***************************** TYPES *************************************/
// handle structs and classes
void rted_RegisterTypeCall( const char* nameC,
                            const char* /* typeC */,
														const char* isUnionType,
														size_t sizeC,
														SourceInfo si,
														size_t argc
														...
													);

/***************************** TYPES *************************************/



#ifdef __cplusplus
} // extern "C"
#endif

#endif

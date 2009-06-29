#ifndef RTED_QT_H
#define RTED_QT_H


#include <stdarg.h>

#include "RuntimeSystem.h"
 #ifdef __cplusplus
extern "C"
#endif
void showDebugDialog(struct RuntimeVariablesType * stack, int stackSize,
		     struct RuntimeVariablesType * heap, int heapSize,
		     struct MemoryType * mem, int memSize, char* filename, int row);




#include "RuntimeSystem.h"
 #ifdef __cplusplus
extern "C"
#endif
void printGui(const char * msg);


#include "RuntimeSystem.h"
 #ifdef __cplusplus
extern "C"
#endif
void printfGui(const char * format, ...);


#include "RuntimeSystem.h"
 #ifdef __cplusplus
extern "C"
#endif
void vprintfGui(const char * format, va_list ap);

#endif

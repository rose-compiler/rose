#ifndef RTED_QT_H
#define RTED_QT_H

//class RuntimeVariablesType;
//class MemoryType;
#include "RuntimeSystem.h"
 #ifdef __cplusplus
extern "C" 
#endif
void showDebugDialog(struct RuntimeVariablesType * stack, int stackSize, 
		     struct RuntimeVariablesType * heap, int heapSize, 
		     struct MemoryType * mem, int memSize, char* filename, int row);

#endif

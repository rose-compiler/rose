#ifndef RTED_QT_H
#define RTED_QT_H

class RuntimeVariablesType;
class MemoryType;

void showDebugDialog(RuntimeVariablesType * stack, int stackSize,
                     RuntimeVariablesType * heap, int heapSize,
                     MemoryType * mem, int memSize);


#endif

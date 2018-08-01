#ifndef AI_TOOL_RUNTIME_H
#define AI_TOOL_RUNTIME_H

#include <stdlib.h> 
#include <assert.h>
#include <stdio.h>
#include <string.h>

extern int * _loop_counters;

//initialize the runtime, accepting filename for storing results
void ai_runtime_init(char* outputfile_name, int loop_count);

// write results into a file, deallocate resources
void ai_runtime_terminate();

#endif //AI_TOOL_RUNTIME_H

// Code Instrumentor library functions: __ci_ as prefix
#ifndef INSTRUMENTOR_LIB_H
#define INSTRUMENTOR_LIB_H

/*
 *  Assumption:  the instrumentor tool has two execution modes: 
 *   1. count the total number of loops in an application, saving results like: 0 /path/to/firstloop:line:col
 *   2. using the results, instrument the loop with  __lc_counters[loop_index]++; and run the code
 *
 *   The execution of the code will write counters into a file like: 0 /path/to/firstloop:line:col 123 
 *   the 0th loop at location /path/to/firstloop:line:col, it has 123 iterations during the execution. 
 */
#ifdef __cplusplus
extern "C" {
#endif

void __ci_writeResultsToFile (char* input_file_name, char* output_file_name, int* loop_iter_counters, int loop_count);

#ifdef __cplusplus
}
#endif

#endif  // INSTRUMENTOR_LIB_H

#include <limits.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdint.h>
#include <stdio.h>



#define ROSE_TRACE_DIR_ENV "ROSE_TRACE_DIR"
#define ROSE_TRACE_FILE_PATH_ENV "ROSE_TRACE_FULL_FILE_PATH"
static FILE * __roseTraceFilePtr;
static char __roseTraceFile[PATH_MAX];

// __ROSE_TraceStartup() reads the ROSE_TRACE_FULL_FILE_PATH environment variable and use it if it is set. (DONT USE FOR MULTITHREADED CODES)
// Otherwise, it checks ROSE_TRACE_DIR environment variable and creates a rose trace file under it.
// If ROSE_TRACE_DIR is also not set, it uses the current directory to log traces, in which case the trace files have hostname-pid-threadid.rosetrace file names.

static void  __attribute__((constructor)) __ROSE_TraceStartup(){
    // Get the environment variable ROSE_TRACE_DIR
    
    char * roseTraceFilePath = getenv(ROSE_TRACE_FILE_PATH_ENV);
    
    if(!roseTraceFilePath) {
        
        char * roseTraceDir = getenv(ROSE_TRACE_DIR_ENV);
        if(!roseTraceDir){
            roseTraceDir = getcwd(roseTraceDir, PATH_MAX);
            fprintf(stderr, "\n ROSE_TRACE_DIR is not set, using current directory %s for logging traces", roseTraceDir);
        }
        
        char hostName[PATH_MAX];
        if(gethostname(hostName,PATH_MAX)) {
            fprintf(stderr, "\n Failed to  get gethostname() for trace file... exiting");
            exit(-1);
        }
        
        pid_t pid = getpid();
        snprintf(__roseTraceFile, PATH_MAX, "%s/%s-%lu-0.rosetrace", roseTraceDir, hostName, pid);
    } else {
        snprintf(__roseTraceFile, PATH_MAX, "%s", roseTraceFilePath);
    }
    
    if((__roseTraceFilePtr=fopen(__roseTraceFile, "wb")) == NULL){
        fprintf(stderr, "\n Failed to open the trace file %s ... exiting", __roseTraceFile);
        exit(-1);
    }
    fprintf(stdout, "\n  Trace file name = %s", __roseTraceFile);
}

// __ROSE_TraceEnd() closes the trace file.
static void  __attribute__((destructor)) __ROSE_TraceEnd(){
    if (__roseTraceFilePtr) {
        if(fclose(__roseTraceFilePtr)) {
            fprintf(stderr, "\n Trace file %s was not closed!", __roseTraceFile);
        } else {
            fprintf(stdout, "\n All traces written to file %s ", __roseTraceFile);
        }
    }
}

// Each instrumented SgNode calls this function.
// ROSE_Tracing_Instrumentor appedds the trace id to the trace file.
// TODO: THIS IS NOT THREAD SAFE. WE SHOULD MAKE IT THREAD SAFE.
int ROSE_Tracing_Instrumentor(uint64_t id){
    if(fwrite(&id, sizeof(uint64_t), 1, __roseTraceFilePtr) != 1){
        fprintf(stderr, "\n Failed to write to trace ... exiting", __roseTraceFile);
        fclose(__roseTraceFilePtr);
        __roseTraceFilePtr = NULL;
        exit(-1);
    }
}
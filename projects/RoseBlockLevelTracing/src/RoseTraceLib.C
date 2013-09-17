#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdint.h>
#include <stdio.h>



#define ROSE_TRACE_DIR_ENV "ROSE_TRACE_DIR"
static FILE * __roseTraceFilePtr;
static char __roseTraceFile[PATH_MAX];

static void  __attribute__((constructor)) TraceStartup(){
    // Get the environment variable ROSE_TRACE_DIR
    
    char * roseTraceDir = getenv(ROSE_TRACE_DIR_ENV);
    if(!roseTraceDir){
        roseTraceDir = getcwd(roseTraceDir, PATH_MAX); //"./";//getenv("TMPDIR");
        fprintf(stderr, "\n ROSE_TRACE_DIR is not set, using current directory %s for logging traces", roseTraceDir);
    }
    
    char hostName[PATH_MAX];
    if(gethostname(hostName,PATH_MAX)) {
        fprintf(stderr, "\n Failed to  get gethostname() for trace file... exiting");
        exit(-1);
    }
    
    pid_t pid = getpid();
    snprintf(__roseTraceFile, PATH_MAX, "%s/%s-%lu-0.rosetrace", roseTraceDir, hostName, pid);
    if((__roseTraceFilePtr=fopen(__roseTraceFile, "wb")) == NULL){
        fprintf(stderr, "\n Failed to open the trace file %s ... exiting", __roseTraceFile);
        exit(-1);        
    }
    fprintf(stdout, "\n  Trace file name = %s", __roseTraceFile);
}

static void  __attribute__((destructor)) TraceEnd(){
    if (__roseTraceFilePtr) {
        if(fclose(__roseTraceFilePtr)) {
            fprintf(stderr, "\n Trace file %s was not closed!", __roseTraceFile);
        } else {
            fprintf(stdout, "\n All traces written to file %s ", __roseTraceFile);
        }
    }
}

int ROSE_Tracing_Instrumentor(uint64_t id){
    if(fwrite(&id, sizeof(uint64_t), 1, __roseTraceFilePtr) != 1){
        fprintf(stderr, "\n Failed to write to trace ... exiting", __roseTraceFile);
        fclose(__roseTraceFilePtr);
        __roseTraceFilePtr = NULL;
        exit(-1);
    }
}
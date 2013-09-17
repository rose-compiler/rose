#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;
static FILE * __roseTraceFilePtr;
static char * __roseTraceFile;


static FILE * __roseProjectDBFilePtr;
static char * __roseProjectDBFile;
static std::vector<string> idToFile;

int main(int argc, char ** argv){
    
    if(argc < 3){
        fprintf(stderr, "\n Usage %s <RoseProjectDbFile> <TraceFile>", argv[0]);
        exit(-1);
    }
    
    __roseProjectDBFile = argv[1];
    __roseTraceFile = argv[2];
    
    if(!(__roseTraceFilePtr = fopen(__roseTraceFile, "rb"))){
        fprintf(stderr, "\n Failed to read TraceFile %s",__roseTraceFilePtr);
        exit(-1);
    }

    if(!(__roseProjectDBFilePtr = fopen(__roseProjectDBFile, "r"))){
        fprintf(stderr, "\n Failed to read roseDBFile %s",__roseProjectDBFile);
        exit(-1);
    }
    
    char fileName[PATH_MAX];
    // Read file DB
    uint32_t fileId = 0;
    while(fgets(fileName, PATH_MAX, __roseProjectDBFilePtr)){
        // kill \n
        fileName[strlen(fileName)-1] = '\0';
        idToFile.push_back(string(fileName));
    }
    fclose(__roseProjectDBFilePtr);
    
    // Read and print all trace records
    uint64_t traceId;
    std::cout<<"\n TraceId : File : Line : Column";
    cout<<"\n *******************************";
    while(fread(&traceId, sizeof(uint64_t), 1, __roseTraceFilePtr)){
        uint32_t fileId = traceId >> 32;
        uint32_t line = (traceId & 0xffffffff) >> 12;
        uint32_t col = (traceId & 0xfff);
        
        if (fileId > idToFile.size()) {
            cout<<"\n"<< traceId << " MISSING FILE!!!! id" << fileId;
        } else {
            cout<<"\n"<< traceId << ":" << idToFile[fileId] << ":" << line << ":" << col;
        }
    }
    cout<<"\n *******************************";
    fclose(__roseTraceFilePtr);

    return 0;
    
}
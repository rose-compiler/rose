#include "rose.h"
#include "compilationFileDatabase.h"
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
#include <iostream>
#include <sstream>
#include <functional>
#include <numeric>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "common.h"

using namespace std;
using namespace boost::algorithm;
using namespace SageBuilder;
using namespace SageInterface;

void Usage(int argc, char * argv[] ){
    fprintf(stderr, "\n Usage %s <TraceFile 1> .. <TraceFile N> -o <CommonPrefixTraceFile>", argv[0]);
    exit(-1);
}

int main( int argc, char * argv[] ) {
    
    if(argc < 4)
        Usage(argc, argv);
    
    vector<FILE *> roseTraceFilePtrs;
    vector<char *> roseTraceFiles;
    char * commonPrefixTraceFile;
    
    for(int i = 1 ; i < argc - 1; i++){
        if (string(argv[i]) == "-o") {
            commonPrefixTraceFile = argv[i+1];
            break;
        } else {
            roseTraceFiles.push_back(argv[i]);
            FILE * fp;
            if(!(fp = fopen(argv[i], "rb"))){
                fprintf(stderr, "\n Failed to read TraceFile %s",argv[i]);
                exit(-1);
            }
            roseTraceFilePtrs.push_back(fp);
        }
    }
    
    // Read all trace records from all traces
    uint64_t traceId;
    boost::unordered_map<uint64_t, SgNode*>::iterator it;
    uint64_t numTraces = roseTraceFilePtrs.size();
    
    vector<uint64_t> traceRecordVector[numTraces];
    
    for(uint64_t curTraceNo = 0 ; curTraceNo < numTraces ; curTraceNo++) {
        while(fread(&traceId, sizeof(uint64_t), 1, roseTraceFilePtrs[curTraceNo])){
            traceRecordVector[curTraceNo].push_back(traceId);
        }
        fclose(roseTraceFilePtrs[curTraceNo]);
    }
    
    
    // Now, compare traces till they start to diverge and populate them in a vector and write back.
    
    vector<uint64_t> commonPrefix;
    uint64_t minTraceSize = -1;
    uint64_t maxTraceSize = 0;

    for( uint64_t i = 0 ; i < numTraces; i++){
        if( traceRecordVector[i].size() < minTraceSize) {
            minTraceSize = traceRecordVector[i].size();
        }
        if( traceRecordVector[i].size() > maxTraceSize) {
            maxTraceSize = traceRecordVector[i].size();
        }
    }

    uint64_t commonPrefixLength = 0;
    for( ; commonPrefixLength < minTraceSize; commonPrefixLength++){
        uint64_t curTraceId = traceRecordVector[0][commonPrefixLength];
        for(uint64_t traceNo = 1 ; traceNo < numTraces ; traceNo++){
            if(traceRecordVector[traceNo][commonPrefixLength] != curTraceId){
                // Paths diverged
                cout<<"\n Paths diverge, common prefix length = " << commonPrefixLength;
                goto DONE;
            }
        }
        commonPrefix.push_back(curTraceId);
    }
    
    if(maxTraceSize == minTraceSize)
        cout<<"\n Paths match perfectly, length = " << commonPrefixLength;

    
DONE:
    // Write out the common prefix nodes
    FILE * outputFp = fopen(commonPrefixTraceFile, "wb");
    if(!outputFp){
        fprintf(stderr, "\n Failed to open output traceFile %s",commonPrefixTraceFile);
        exit(-1);
    }

    for( uint64_t i = 0; i < commonPrefixLength ; i++){
        if(fwrite(&commonPrefix[i], sizeof(uint64_t), 1, outputFp) != 1) {
            cout << "\n Failed to write to  file :" <<commonPrefixTraceFile;
            exit(-1);
        }
    }
    fclose(outputFp);
    cout << "\n Common prefix is written to file :" <<commonPrefixTraceFile;
    cout << "\n";
    return 0;
    
}

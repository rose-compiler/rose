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

static std::vector<string> idToFile;
static boost::unordered_map<uint64_t, SgNode*> idToNodeMap;

// A simple preorder traversal to assign unique ids to each node.
// The same order is used in in NodeToIdMapper for instrumentation.
class IdToNodeMapper: public AstSimpleProcessing {
    string m_fileUnderCompilation;
    uint32_t m_fileIdUnderCompilation;
    uint32_t m_counter;
    string m_roseDBFile;
    
    
public:
    IdToNodeMapper(SgFile * fileRoot, const string & dbFile){
        m_fileUnderCompilation = fileRoot->get_file_info()->get_physical_filename();
        m_roseDBFile = dbFile;
        m_counter = 0;
        m_fileIdUnderCompilation  = rose::GetProjectWideUniqueIdForPhysicalFile(m_roseDBFile, m_fileUnderCompilation);
        traverse(fileRoot, preorder);
    }
    
    
private:
    void virtual visit(SgNode * node) {
        TraceId id(m_fileIdUnderCompilation, m_counter++);
        idToNodeMap[id.GetSerializableId()] = node;
        cout<<"\n"<< std::hex << id.GetSerializableId() << " : mapped to " << std::hex << node;
    }
};


void Usage(int argc, char * argv[] ){
    fprintf(stderr, "\n Usage %s <Num TraceFiles> <TraceFile 1> .. <TraceFile N> rest of the arguments to rose .. must pass -rose:projectSpecificDatabaseFile", argv[0]);
    exit(-1);
}

int main( int argc, char * argv[] ) {
    
    if(argc < 4){
        Usage(argc, argv);
    }
    
    int numTraces = 0;
    
    try
    {
        numTraces = boost::lexical_cast<int>(argv[1]); 
    }
    catch(...) {
        Usage(argc, argv);
    }
    
    
    FILE * roseTraceFilePtrs[numTraces];
    char * roseTraceFiles[numTraces];
    FILE * roseProjectDBFilePtr;
    const char * roseProjectDBFile;

    
    for(int i = 0 ; i < numTraces; i++) {
        roseTraceFiles[i] = argv[i+2];
        if(!(roseTraceFilePtrs[i] = fopen(roseTraceFiles[i], "rb"))){
            fprintf(stderr, "\n Failed to read TraceFile %s",roseTraceFiles[i]);
            exit(-1);
        }
    }
    
    // patch argv
    for(int i = numTraces + 2 ; i < argc; i++){
        argv[i - 1 - numTraces] = argv[i];
    }
    int patchedArgc = argc - (numTraces + 1);
    
    
    // Generate the ROSE AST.
    
    SgProject* project = frontend(patchedArgc ,argv);
    // AST consistency tests (optional for users, but this enforces more of our tests)
    AstTests::runAllTests(project);
    
    
    // Open the DB file... DOn't need lock for reading.
    roseProjectDBFile = project->get_projectSpecificDatabaseFile().c_str();
    if(!(roseProjectDBFilePtr = fopen(roseProjectDBFile, "r"))){
        fprintf(stderr, "\n Failed to read roseDBFile %s", roseProjectDBFile);
        exit(-1);
    }
    
    char fileName[PATH_MAX];
    // Read DB file and assign file Ids
    while(fgets(fileName, PATH_MAX, roseProjectDBFilePtr)){
        // kill \n
        fileName[strlen(fileName)-1] = '\0';
        idToFile.push_back(string(fileName));
    }
    fclose(roseProjectDBFilePtr);
    
    // Build node to trace id map.
    for(int i = 0 ; i < project->numberOfFiles(); i++) {
        SgFile & file = project->get_file(i);
        IdToNodeMapper mapper(&file, project->get_projectSpecificDatabaseFile());
    }
    
    // Read all trace records from all traces
    uint64_t traceId;
    boost::unordered_map<uint64_t, SgNode*>::iterator it;
    
    vector<SgNode *> traceRecordVector[numTraces];
    
    for(int curTraceNo = 0 ; curTraceNo < numTraces ; curTraceNo++) {
        while(fread(&traceId, sizeof(uint64_t), 1, roseTraceFilePtrs[curTraceNo])){
            uint32_t fileId = traceId >> 32;
            uint32_t nodeId = (traceId & 0xffffffff);
            
            if (fileId > idToFile.size()) {
                cout<<"\n"<< std::hex << traceId << " MISSING FILE!!!! id" << std::hex << fileId;
                traceRecordVector[curTraceNo].push_back(NULL);
            } else {
                it = idToNodeMap.find(traceId);
                if(it == idToNodeMap.end()){
                    cout<<"\n"<< std::hex << traceId << " can't map back!!!" << std::hex << fileId << std::hex << nodeId;
                    traceRecordVector[curTraceNo].push_back(NULL);
                } else {
                    cout<<"\n"<< std::hex << traceId << ":"<< idToFile[fileId] << ":" << std::hex << (*it).second << ":" << ((*it).second)->class_name();
                    traceRecordVector[curTraceNo].push_back((*it).second);
                }
            }
        }
        fclose(roseTraceFilePtrs[curTraceNo]);
    }
    
    
    // Now, compare traces till they start to diverge and populate them in a vector so that one can iterate over them.
    
    vector<SgNode *> commonPrefix;
    uint64_t minTraceSize = -1;
    uint64_t maxTraceSize = 0;

    for( int i = 0 ; i < numTraces; i++){
        if( traceRecordVector[i].size() < minTraceSize) {
            minTraceSize = traceRecordVector[i].size();
        }
        if( traceRecordVector[i].size() > maxTraceSize) {
            maxTraceSize = traceRecordVector[i].size();
        }
    }

    uint64_t commonPrefixLength = 0;
    for( ; commonPrefixLength < minTraceSize; commonPrefixLength++){
        SgNode * curSgNode = traceRecordVector[0][commonPrefixLength];
        for(int traceNo = 1 ; traceNo < numTraces ; traceNo++){
            if(traceRecordVector[traceNo][commonPrefixLength] != curSgNode){
                // Paths diverged
                cout<<"\n Paths diverge, common prefix length = " << commonPrefixLength;
                goto DONE;
            }
        }
        commonPrefix.push_back(curSgNode);
    }
    
    if(maxTraceSize == minTraceSize)
        cout<<"\n Paths match perfectly, length = " << commonPrefixLength;

    
DONE:
    // Print the common prefix nodes
    cout<<"\n Common prefix is:";
    for( uint64_t i = 0; i < commonPrefixLength ; i++){
        SgNode * node = commonPrefix[i];
        cout<<"\n"<< std::hex << node << ":" << node->class_name();
    }
    return 0;
    
}

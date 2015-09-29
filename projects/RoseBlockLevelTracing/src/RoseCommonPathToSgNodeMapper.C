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
    fprintf(stderr, "\n Usage %s <TraceFile> <ProjectSpecificDatabaseFile> rest of the arguments to rose without any file to compile. It MUST match how you originally compiled the project.", argv[0]);
    exit(-1);
}

int main( int argc, char * argv[] ) {
    
    if(argc < 3){
        Usage(argc, argv);
    }
    
    char * roseTraceFile = argv[1];
    FILE * roseTraceFilePtr;
    
    if(!(roseTraceFilePtr = fopen(roseTraceFile, "rb"))){
        fprintf(stderr, "\n Failed to read TraceFile %s",roseTraceFile);
        exit(-1);
    }

    FILE * roseProjectDBFilePtr;
    const char * roseProjectDBFile = argv[2];
    // Open the DB file... Don't need lock for reading.
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
    
    
    
    // Read all trace records from the trace
    uint64_t traceId;
    boost::unordered_map<uint64_t, SgNode*>::iterator it;
    std::set<uint64_t> fileSet;
    
    vector<uint64_t> traceRecordVector;
    
    
    while(fread(&traceId, sizeof(uint64_t), 1, roseTraceFilePtr)){
        uint32_t fileId = traceId >> 32;
        if (fileId > idToFile.size()) {
            cout<<"\n"<< std::hex << traceId << " MISSING FILE!!!! id" << std::hex << fileId;
            traceRecordVector.push_back(NULL);
        } else {
            // If we have not seen this file, add it to teh set of files we want to open
            if(fileSet.find(fileId) == fileSet.end()){
                fileSet.insert(fileId);
            }
            traceRecordVector.push_back(traceId);
        }
    }
    fclose(roseTraceFilePtr);
    
    // Now build AST for each file in the set.
    
    
    for(std::set<uint64_t>::iterator it = fileSet.begin(), e = fileSet.end(); it != e; it++) {
        // prepare args.
        
        uint32_t fileId = *it;
        int numArgs = argc - 2 + 1; // -2 becoz we remove the trace and DB file. +1 because we add the source file.
        char * arguments[numArgs];
        for(int i = 3 ; i < numArgs - 1; i++){
            arguments[i] = argv[i];
        }
        arguments[0] = argv[0];
        arguments[numArgs-1] = (char *) idToFile[fileId].c_str();
        
        // Generate the ROSE AST.
        SgProject* project = frontend(numArgs, arguments);
        // AST consistency tests (optional for users, but this enforces more of our tests)
        AstTests::runAllTests(project);
        
        // Build node to trace id map.
        for(int i = 0 ; i < project->numberOfFiles(); i++) {
            SgFile & file = project->get_file(i);
            IdToNodeMapper mapper(&file, string(roseProjectDBFile));
        }
    }
    
    // Now, print the mapping from trace id to its SgNode
    
    for( vector<uint64_t>::iterator it = traceRecordVector.begin(), e = traceRecordVector.end(); it != e; it++){
        uint64_t traceId = *it;
        uint32_t fileId = traceId >> 32;
        SgNode * curSgNode = idToNodeMap[traceId];
        if(idToNodeMap.find(traceId) != idToNodeMap.end())
            cout <<"\n" << std::hex << traceId << idToFile[fileId] << ":" << std::hex << curSgNode << ":" << curSgNode->class_name();
        else
            cout <<"\n" << std::hex << traceId << "HAS NO MAPPING!!!";
    }
    
    return 0;
    
}

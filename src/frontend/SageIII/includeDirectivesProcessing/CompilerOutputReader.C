#include <rose.h>
#include <rose_getline.h>

#include "CompilerOutputReader.h"

using namespace std;

CompilerOutputReader::CompilerOutputReader(FILE* compilerOutput) {
    this -> compilerOutput = compilerOutput;
    line = NULL;
    hasCurrentLine = true; //initially assume that there is something to read
    readNextLine();
}

void CompilerOutputReader::readNextLine() {
    ROSE_ASSERT(hasCurrentLine); //should not try to read the next string if there is no current string
    size_t line_alloc = 0;
    ssize_t nread = rose_getline(&line, &line_alloc, compilerOutput);
    if (nread == -1) {
        hasCurrentLine = false;
    }
}

bool CompilerOutputReader::hasLine() {
    return hasCurrentLine;
}

string CompilerOutputReader::getLine() {
    ROSE_ASSERT(hasCurrentLine); //should not try to get a non existing string
    string str(line);
    str.erase(str.find_last_of("\n")); //get rid of the end line delimeters
    return str;
}

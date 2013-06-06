#include "rose.h"
#include <string>
#include <fstream>
#include <iostream>
#include <fstream>
#include <stack>
#include <deque>

#ifndef CREATE_CLONE_DETECTION_VECTORS_BINARY
#define CREATE_CLONE_DETECTION_VECTORS_BINARY

#include <stdio.h>
#include <iostream>
#include "rose.h"

#include "sqlite3x.h"

bool createVectorsForAllInstructions(SgNode* top, const std::string& filename, const std::string& functionName, int functionId, size_t windowSize, size_t stride, sqlite3x::sqlite3_connection& con); // Ignores function boundaries
void createVectorsRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride, sqlite3x::sqlite3_connection& con);
void createVectorsNotRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride, sqlite3x::sqlite3_connection& con);
void createDatabases(sqlite3x::sqlite3_connection& con);

#endif // CREATE_CLONE_DETECTION_VECTORS_BINARY

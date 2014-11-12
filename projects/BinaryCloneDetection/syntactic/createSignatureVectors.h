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

#include "sqlite3x.h"

bool createVectorsForAllInstructions(SgNode* top, const std::string& filename, const std::string& functionName, int functionId, size_t windowSize, size_t stride, sqlite3x::sqlite3_connection& con); // Ignores function boundaries

void createVectorsRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride, sqlite3x::sqlite3_connection& con);
void createVectorsNotRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride, sqlite3x::sqlite3_connection& con);

void createSourceVectorsRespectingFunctionBoundaries(SgNode* top, size_t minTokens, size_t stride, size_t windowSize,
                                          std::vector<int> variantNumVec, std::vector<int> variantToWriteToFile, const SqlDatabase::TransactionPtr &tx);

void dropDatabases(const SqlDatabase::TransactionPtr&);
void createDatabases(const SqlDatabase::TransactionPtr&);

#endif // CREATE_CLONE_DETECTION_VECTORS_BINARY

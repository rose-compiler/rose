#ifndef CREATE_CLONE_DETECTION_VECTORS_BINARY
#define CREATE_CLONE_DETECTION_VECTORS_BINARY

#include "rose.h"
#include "SqlDatabase.h"

#include <deque>
#include <fstream>
#include <fstream>
#include <iostream>
#include <iostream>
#include <stack>
#include <stdio.h>
#include <string>

bool createVectorsForAllInstructions(SgNode* top, const std::string& filename, const std::string& functionName, int functionId,
                                     size_t windowSize, size_t stride, const SqlDatabase::TransactionPtr&);
void createVectorsRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride,
                                               const SqlDatabase::TransactionPtr&);
void createVectorsNotRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride,
                                                  const SqlDatabase::TransactionPtr&);

void dropDatabases(const SqlDatabase::TransactionPtr &tx);
void createDatabases(const SqlDatabase::TransactionPtr &tx);

#endif

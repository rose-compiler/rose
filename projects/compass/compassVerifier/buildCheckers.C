// This is an automatically generated file
#include <iostream>
#include <rose.h>
#include <vector>
#include "compass.h"
#include "checkers.h"

void
buildCheckers( std::vector<Compass::TraversalBase*> &retVal, Compass::Parameters &params, Compass::OutputObject &output )
{


    try {
        CompassAnalyses::FileReadOnlyAccess::Traversal *traversal;
        traversal = new CompassAnalyses::FileReadOnlyAccess::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker FileReadOnlyAccess: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ForbiddenFunctions::Traversal *traversal;
        traversal = new CompassAnalyses::ForbiddenFunctions::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ForbiddenFunctions: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoAsmStmtsOps::Traversal *traversal;
        traversal = new CompassAnalyses::NoAsmStmtsOps::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoAsmStmtsOps: " << e.what() << std::endl;
    }

  return;
} //buildCheckers()


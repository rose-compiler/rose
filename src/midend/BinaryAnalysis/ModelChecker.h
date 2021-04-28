#ifndef ROSE_BinaryAnalysis_ModelChecker_H
#define ROSE_BinaryAnalysis_ModelChecker_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

// Include this header if you want everying related to model checking. A better (faster to compile) approach is to
// include Types.h (forward declarations for most things), and only those other headers you actually need.

#include <BinaryAnalysis/ModelChecker/BasicBlockUnit.h>
#include <BinaryAnalysis/ModelChecker/Engine.h>
#include <BinaryAnalysis/ModelChecker/ErrorTag.h>
#include <BinaryAnalysis/ModelChecker/ExecutionUnit.h>
#include <BinaryAnalysis/ModelChecker/ExternalFunctionUnit.h>
#include <BinaryAnalysis/ModelChecker/FailureUnit.h>
#include <BinaryAnalysis/ModelChecker/InstructionUnit.h>
#include <BinaryAnalysis/ModelChecker/NullDerefTag.h>
#include <BinaryAnalysis/ModelChecker/P2Model.h>
#include <BinaryAnalysis/ModelChecker/Path.h>
#include <BinaryAnalysis/ModelChecker/PathNode.h>
#include <BinaryAnalysis/ModelChecker/PathPredicate.h>
#include <BinaryAnalysis/ModelChecker/PathPrioritizer.h>
#include <BinaryAnalysis/ModelChecker/PathQueue.h>
#include <BinaryAnalysis/ModelChecker/Periodic.h>
#include <BinaryAnalysis/ModelChecker/SemanticCallbacks.h>
#include <BinaryAnalysis/ModelChecker/Settings.h>
#include <BinaryAnalysis/ModelChecker/SourceLister.h>
#include <BinaryAnalysis/ModelChecker/Tag.h>

#endif
#endif

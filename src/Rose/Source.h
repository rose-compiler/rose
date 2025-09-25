#ifndef ROSE_Source_H
#define ROSE_Source_H

namespace Rose {

//! Capabilities that are specific to the manipulation of source-code. Manipulation includes analysis, transformation, synthesis, and any composition of those three.
namespace Source {}

}

#include <Rose/Source/AST.h>
#include <Rose/Source/CodeGen.h>
#include <Rose/Source/Analysis.h>
#include <Rose/Source/Sarif.h>

#endif /* ROSE_Source_H */

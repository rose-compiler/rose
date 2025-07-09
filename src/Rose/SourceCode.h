#ifndef ROSE_SourceCode_H
#define ROSE_SourceCode_H

namespace Rose {

//! Capabilities that are specific to the manipulation of source-code. Manipulation includes analysis, transformation, synthesis, and any composition of those three.
namespace SourceCode {}

}

#include <Rose/SourceCode/AST.h>
#include <Rose/SourceCode/CodeGen.h>
#include <Rose/SourceCode/Analysis.h>
#include <Rose/SourceCode/Sarif.h>

#endif /* ROSE_SourceCode_H */

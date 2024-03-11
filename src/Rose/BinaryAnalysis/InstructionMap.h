#ifndef ROSE_BinaryAnalysis_InstructionMap_H
#define ROSE_BinaryAnalysis_InstructionMap_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Address.h>
#include <Map.h>                                        // rose

class SgAsmInstruction;

namespace Rose {
namespace BinaryAnalysis {

/** Mapping from instruction addresses to instructions. */
using InstructionMap = Map<Address, SgAsmInstruction*>;

} // namespace
} // namespace

#endif
#endif

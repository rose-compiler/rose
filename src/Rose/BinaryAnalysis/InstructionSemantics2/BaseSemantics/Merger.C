#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Merger.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

Merger::Merger()
    : memoryAddressesMayAlias_(true), memoryMergeDebugging_(false) {}

Merger::~Merger() {}

Merger::Ptr
Merger::instance() {
    return Ptr(new Merger);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
